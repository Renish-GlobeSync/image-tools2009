
#include "config.h"

#include <stdlib.h>
#include <string.h>

#include <libgimp/gimp.h>

#include "convolution_matrix.h"

static void      convolve_image        (GimpDrawable  *drawable,
                                        GimpPreview   *preview);

static void      check_config          (GimpDrawable  *drawable);

static gfloat    convolve_pixel        (guchar       **src_row,
                                        gint           x_offset,
                                        gint           channel,
                                        GimpDrawable  *drawable);

typedef convolution_matrix_config config_struct;
static config_struct config;

/*  A generic wrapper to gimp_pixel_rgn_get_row which handles unlimited
 *  wrapping or gives the transparent regions outside the image
 *  fills additional bytes before and after image row to provide border modes.
 */

static void
my_get_row (GimpPixelRgn *PR,
            guchar       *dest,
            gint          x,
            gint          y,
            gint          w)
{
  gint width, height, bpp;
  gint i;

  width  = PR->drawable->width;
  height = PR->drawable->height;
  bpp  = PR->drawable->bpp;

  /* Y-wrappings */
  switch (config.bmode)
    {
    case WRAP:
      /* Wrapped, so we get the proper row from the other side */
      while (y < 0) /* This is the _sure_ way to wrap. :) */
        y += height;
      while (y >= height)
        y -= height;
      break;

    case CLEAR:
      /* Beyond borders, so set full transparent. */
      if (y < 0 || y >= height)
        {
          memset (dest, 0, w * bpp);
          return; /* Done, so back. */
        }

    case EXTEND:
      y = CLAMP (y , 0 , height - 1);
      break;
    }


  /* X-wrappings */
  switch (config.bmode)
    {
    case CLEAR:
      if (x < 0)
        {
          i = MIN (w, -x);
          memset (dest, 0, i * bpp);
          dest += i * bpp;
          w -= i;
          x += i;
        }
      if (w)
        {
          i = MIN (w, width);
          gimp_pixel_rgn_get_row (PR, dest, x, y, i);
          dest += i * bpp;
          w -= i;
          x += i;
        }
      if (w)
        memset (dest, 0, w * bpp);
      break;

    case WRAP:
      while (x < 0)
        x += width;
      i = MIN (w, width - x);
      gimp_pixel_rgn_get_row (PR, dest, x, y, i);
      w -= i;
      dest += i * bpp;
      x = 0;
      while (w)
        {
          i = MIN (w, width);
          gimp_pixel_rgn_get_row (PR, dest, x, y, i);
          w -= i;
          dest += i * bpp;
        }
      break;

    case EXTEND:
      if (x < 0)
        {
          gimp_pixel_rgn_get_pixel (PR, dest, 0, y);
          x++;
          w--;
          dest += bpp;

          while (x < 0 && w)
            {
              for (i = 0; i < bpp; i++)
                {
                  *dest = *(dest - bpp);
                  dest++;
                }
              x++;
              w--;
            }
        }
      if (w && width - x > 0)
        {
          i = MIN (w, width - x);
          gimp_pixel_rgn_get_row (PR, dest, x, y, i);
          w -= i;
          dest += i * bpp;
        }
      while (w)
        {
          for (i = 0; i < bpp; i++)
            {
              *dest= *(dest - bpp);
              dest++;
            }
          x++;
          w--;
        }
      break;

    }
}

static gfloat
convolve_pixel (guchar       **src_row,
                gint           x_offset,
                gint           channel,
                GimpDrawable  *drawable)
{
  static gfloat matrixsum = 0; /* FIXME: this certainly breaks the preview */
  static gint bpp         = 0;

  gfloat sum              = 0;
  gfloat alphasum         = 0;
  gfloat temp;
  gint   x, y;
  gint   alpha_channel;

  if (!bpp)
    {
      bpp = drawable->bpp;

      for (y = 0; y < MATRIX_SIZE; y++)
        for (x = 0; x < MATRIX_SIZE; x++)
          {
            temp = config.matrix[x][y];
            matrixsum += ABS (config.matrix[x][y]);
          }
    }

  alpha_channel = bpp - 1;

  for (y = 0; y < MATRIX_SIZE; y++)
    for (x = 0; x < MATRIX_SIZE; x++)
      {
        temp = config.matrix[x][y];

        if (channel != alpha_channel && config.alpha_weighting == 1)
          {
            temp *= src_row[y][x_offset + x * bpp + alpha_channel - channel];
            alphasum += ABS (temp);
          }

        temp *= src_row[y][x_offset + x * bpp];
        sum += temp;
      }

  sum /= config.divisor;

  if (channel != alpha_channel && config.alpha_weighting == 1)
    {
      if (alphasum != 0)
        sum = sum * matrixsum / alphasum;
      else
        sum = 0;
    }

  sum += config.offset;

  return sum;
}

static void
convolve_image (GimpDrawable *drawable,
                GimpPreview  *preview)
{
  GimpPixelRgn  srcPR, destPR;
  gint          width, height, row, col;
  gint          src_w, src_row_w, src_h, i;
  gint          src_x1, src_y1, src_x2, src_y2;
  gint          x1, x2, y1, y2;
  guchar       *dest_row[DEST_ROWS];
  guchar       *src_row[MATRIX_SIZE];
  guchar       *tmp_row;
  gint          x_offset;
  gboolean      chanmask[CHANNELS - 1];
  gint          bpp;
  gint          alpha_channel;

  /* Get the input area. This is the bounding box of the selection in
   *  the image (or the entire image if there is no selection). Only
   *  operating on the input area is simply an optimization. It doesn't
   *  need to be done for correct operation. (It simply makes it go
   *  faster, since fewer pixels need to be operated on).
   */
  if (preview)
    {
      //gimp_preview_get_position (preview, &src_x1, &src_y1);
      //gimp_preview_get_size (preview, &src_w, &src_h);
      //src_x2 = src_x1 + src_w;
      //src_y2 = src_y1 + src_h;
    }
  else
    {
      gimp_drawable_mask_bounds (drawable->drawable_id,
                                 &src_x1, &src_y1, &src_x2, &src_y2);
      src_w = src_x2 - src_x1;
      src_h = src_y2 - src_y1;
    }

  /* Get the size of the input image. (This will/must be the same
   *  as the size of the output image.
   */
  width  = drawable->width;
  height = drawable->height;
  bpp  = drawable->bpp;
  alpha_channel = bpp - 1;

  if (gimp_drawable_is_rgb (drawable->drawable_id))
    {
      for (i = 0; i < CHANNELS - 1; i++)
        chanmask[i] = config.channels[i + 1];
    }
  else /* Grayscale */
    {
      chanmask[0] = config.channels[0];
    }

  if (gimp_drawable_has_alpha (drawable->drawable_id))
    chanmask[alpha_channel] = config.channels[4];

  src_row_w = src_w + HALF_WINDOW + HALF_WINDOW;

  for (i = 0; i < MATRIX_SIZE; i++)
    src_row[i] = g_new (guchar, src_row_w * bpp);

  for (i = 0; i < DEST_ROWS; i++)
    dest_row[i]= g_new (guchar, src_w * bpp);

  /*  initialize the pixel regions  */
  x1 = MAX (src_x1 - HALF_WINDOW, 0);
  y1 = MAX (src_y1 - HALF_WINDOW, 0);
  x2 = MIN (src_x2 + HALF_WINDOW, width);
  y2 = MIN (src_y2 + HALF_WINDOW, height);
  gimp_pixel_rgn_init (&srcPR, drawable,
                       x1, y1, x2 - x1, y2 - y1, FALSE, FALSE);
  gimp_pixel_rgn_init (&destPR, drawable,
                       src_x1, src_y1, src_w, src_h,
                       preview == NULL, TRUE);

  /* initialize source arrays */
  for (i = 0; i < MATRIX_SIZE; i++)
    my_get_row (&srcPR, src_row[i], src_x1 - HALF_WINDOW,
                src_y1 - HALF_WINDOW + i , src_row_w);

  for (row = src_y1; row < src_y2; row++)
    {
      gint channel;

      x_offset = 0;

      for (col = src_x1; col < src_x2; col++)
        for (channel = 0; channel < bpp; channel++)
          {
            guchar d;

            if (chanmask[channel])
              {
                gint result;

                result = ROUND (convolve_pixel (src_row,
                                                x_offset, channel, drawable));
                d = CLAMP (result, 0, 255);
              }
            else
              {
                /* copy unmodified pixel */
                d = src_row[HALF_WINDOW][x_offset + HALF_WINDOW * bpp];
              }

            dest_row[HALF_WINDOW][x_offset] = d;
            x_offset++;
          }

      if (row >= src_y1 + HALF_WINDOW)
        gimp_pixel_rgn_set_row (&destPR,
                                dest_row[0], src_x1, row - HALF_WINDOW, src_w);

      if (row < src_y2 - 1)
        {
          tmp_row = dest_row[0];

          for (i = 0; i < DEST_ROWS - 1; i++)
            dest_row[i] = dest_row[i + 1];

          dest_row[DEST_ROWS - 1] = tmp_row;

          tmp_row = src_row[0];

          for (i = 0; i < MATRIX_SIZE - 1; i++)
            src_row[i] = src_row[i + 1];

          src_row[MATRIX_SIZE-1] = tmp_row;

          my_get_row (&srcPR, src_row[MATRIX_SIZE - 1],
                      src_x1 - HALF_WINDOW, row + HALF_WINDOW + 1, src_row_w);
        }

      if ((row % 10 == 0) && !preview)
        gimp_progress_update ((double) (row - src_y1) / src_h);
    }

  /* put the remaining rows in the buffer in place */
  for (i = 1; i <  DEST_ROWS; i++)
    gimp_pixel_rgn_set_row (&destPR, dest_row[i],
                            src_x1, src_y2 + i - 1 - HALF_WINDOW, src_w);


  /*  update the region  */
  if (preview)
    {
      //gimp_drawable_preview_draw_region (GIMP_DRAWABLE_PREVIEW (preview),
      //                                   &destPR);
    }
  else
    {
      gimp_drawable_flush (drawable);
      gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
      gimp_drawable_update (drawable->drawable_id,
                            src_x1, src_y1, src_x2 - src_x1, src_y2 - src_y1);
    }

  for (i = 0; i < MATRIX_SIZE; i++)
    g_free (src_row[i]);

  for (i = 0; i < DEST_ROWS; i++)
    g_free (dest_row[i]);
}

static void
check_matrix (void)
{
  gint      x, y;
  gboolean  valid = FALSE;
  gfloat    sum   = 0.0;

  for (y = 0; y < MATRIX_SIZE; y++)
    for (x = 0; x < MATRIX_SIZE; x++)
      {
        sum += config.matrix[x][y];
        if (config.matrix[x][y] != 0.0)
          valid = TRUE;
      }

  if (config.autoset)
    {
      if (sum > 0)
        {
          config.offset = 0;
          config.divisor = sum;
        }
      else if (sum < 0)
        {
          config.offset = 255;
          config.divisor = -sum;
        }
      else
        {
          config.offset = 128;
          /* The sum is 0, so this is probably some sort of
           * embossing filter. Should divisor be autoset to 1
           * or left undefined, ie. for the user to define? */
          config.divisor = 1;
        }
      redraw_off_and_div ();
    }
}


/* Checks that the configuration is valid for the image type */
static void
check_config (GimpDrawable *drawable)
{
  config.alpha_weighting = 0;

  if (!gimp_drawable_has_alpha (drawable->drawable_id))
    {
      config.alpha_weighting = -1;
      config.bmode           = EXTEND;
    }
}

void operation_convolution_matrix(
                        void * in_buf, 
                        void * out_buf, 
                        long width, 
                        long height, 
                        convolution_matrix_config * pconfig)
{
    GimpDrawable * drawable = gimp_drawable_get(0, in_buf, out_buf, width, height);
    config = *pconfig;
    convolve_image(drawable, NULL);
    gimp_drawable_detach (drawable);
}
