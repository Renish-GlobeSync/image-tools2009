
#include "config.h"

#include <string.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "libgimp/stdplugins-intl.h"

#include "softglow.h"

#define TILE_CACHE_SIZE 48
#define SIGMOIDAL_BASE   2
#define SIGMOIDAL_RANGE 20
#define INT_MULT(a,b,t)  ((t) = (a) * (b) + 0x80, ((((t) >> 8) + (t)) >> 8))


/*
 * Function prototypes.
 */


static void      softglow          (GimpDrawable     *drawable,
                                    GimpPreview      *preview);

/*
 * Gaussian blur helper functions
 */
static void      find_constants    (gdouble  n_p[],
                                    gdouble  n_m[],
                                    gdouble  d_p[],
                                    gdouble  d_m[],
                                    gdouble  bd_p[],
                                    gdouble  bd_m[],
                                    gdouble  std_dev);
static void      transfer_pixels   (gdouble *src1,
                                    gdouble *src2,
                                    guchar  *dest,
                                    gint     jump,
                                    gint     width);

/***** Local vars *****/


static softglow_config svals =
{
  10.0, /* glow_radius */
  0.75, /* brightness */
  0.85,  /* sharpness */
};


/***** Functions *****/
static void
softglow (GimpDrawable *drawable,
          GimpPreview  *preview)
{
  GimpPixelRgn  src_rgn, dest_rgn;
  GimpPixelRgn *pr;
  gint          width, height;
  gint          bytes;
  gboolean      has_alpha;
  guchar       *dest;
  guchar       *src, *sp_p, *sp_m;
  gdouble       n_p[5], n_m[5];
  gdouble       d_p[5], d_m[5];
  gdouble       bd_p[5], bd_m[5];
  gdouble      *val_p, *val_m, *vp, *vm;
  gint          x1, y1, x2, y2;
  gint          i, j;
  gint          row, col, b;
  gint          terms;
  gint          progress, max_progress;
  gint          initial_p[4];
  gint          initial_m[4];
  gint          tmp;
  gdouble       radius;
  gdouble       std_dev;
  gdouble       val;

  if (preview)
    {
      //gimp_preview_get_position (preview, &x1, &y1);
      //gimp_preview_get_size (preview, &width, &height);
      x2 = x1 + width;
      y2 = y1 + height;
    }
  else
    {
      gimp_drawable_mask_bounds (drawable->drawable_id, &x1, &y1, &x2, &y2);
      width     = (x2 - x1);
      height    = (y2 - y1);
    }

  bytes     = drawable->bpp;
  has_alpha = gimp_drawable_has_alpha (drawable->drawable_id);

  val_p = g_new (gdouble, MAX (width, height));
  val_m = g_new (gdouble, MAX (width, height));

  dest = g_new0 (guchar, width * height);

  progress = 0;
  max_progress = width * height * 3;

  /* Initialize the pixel regions. */
  gimp_pixel_rgn_init (&src_rgn, drawable, x1, y1, width, height, FALSE, FALSE);

  for (pr = gimp_pixel_rgns_register (1, &src_rgn);
       pr != NULL;
       pr = gimp_pixel_rgns_process (pr))
    {
      guchar *src_ptr  = src_rgn.data;
      guchar *dest_ptr = dest + (src_rgn.y - y1) * width + (src_rgn.x - x1);

      for (row = 0; row < src_rgn.h; row++)
        {
          for (col = 0; col < src_rgn.w; col++)
            {
              /* desaturate */
              if (bytes > 2)
                dest_ptr[col] = (guchar) gimp_rgb_to_l_int (src_ptr[col * bytes + 0],
                                                            src_ptr[col * bytes + 1],
                                                            src_ptr[col * bytes + 2]);
              else
                dest_ptr[col] = (guchar) src_ptr[col * bytes];

              /* compute sigmoidal transfer */
              val = dest_ptr[col] / 255.0;
              val = 255.0 / (1 + exp (-(SIGMOIDAL_BASE + (svals.sharpness * SIGMOIDAL_RANGE)) * (val - 0.5)));
              val = val * svals.brightness;
              dest_ptr[col] = (guchar) CLAMP (val, 0, 255);
            }

          src_ptr  += src_rgn.rowstride;
          dest_ptr += width;
        }

      if (!preview)
        {
          progress += src_rgn.w * src_rgn.h;
          gimp_progress_update ((gdouble) progress / (gdouble) max_progress);
        }
    }

  /*  Calculate the standard deviations  */
  radius  = fabs (svals.glow_radius) + 1.0;
  std_dev = sqrt (-(radius * radius) / (2 * log (1.0 / 255.0)));

  /*  derive the constants for calculating the gaussian from the std dev  */
  find_constants (n_p, n_m, d_p, d_m, bd_p, bd_m, std_dev);

  /*  First the vertical pass  */
  for (col = 0; col < width; col++)
    {
      memset (val_p, 0, height * sizeof (gdouble));
      memset (val_m, 0, height * sizeof (gdouble));

      src  = dest + col;
      sp_p = src;
      sp_m = src + width * (height - 1);
      vp   = val_p;
      vm   = val_m + (height - 1);

      /*  Set up the first vals  */
      initial_p[0] = sp_p[0];
      initial_m[0] = sp_m[0];

      for (row = 0; row < height; row++)
        {
          gdouble *vpptr, *vmptr;

          terms = (row < 4) ? row : 4;

          vpptr = vp; vmptr = vm;
          for (i = 0; i <= terms; i++)
            {
              *vpptr += n_p[i] * sp_p[-i * width] - d_p[i] * vp[-i];
              *vmptr += n_m[i] * sp_m[i * width] - d_m[i] * vm[i];
            }
          for (j = i; j <= 4; j++)
            {
              *vpptr += (n_p[j] - bd_p[j]) * initial_p[0];
              *vmptr += (n_m[j] - bd_m[j]) * initial_m[0];
            }

          sp_p += width;
          sp_m -= width;
          vp ++;
          vm --;
        }

      transfer_pixels (val_p, val_m, dest + col, width, height);

      if (!preview)
        {
          progress += height;
          if ((col % 5) == 0)
            gimp_progress_update ((gdouble) progress / (gdouble) max_progress);
        }
    }

  for (row = 0; row < height; row++)
    {
      memset (val_p, 0, width * sizeof (gdouble));
      memset (val_m, 0, width * sizeof (gdouble));

      src = dest + row * width;

      sp_p = src;
      sp_m = src + width - 1;
      vp = val_p;
      vm = val_m + width - 1;

      /*  Set up the first vals  */
      initial_p[0] = sp_p[0];
      initial_m[0] = sp_m[0];

      for (col = 0; col < width; col++)
        {
          gdouble *vpptr, *vmptr;

          terms = (col < 4) ? col : 4;

          vpptr = vp; vmptr = vm;

          for (i = 0; i <= terms; i++)
            {
              *vpptr += n_p[i] * sp_p[-i] - d_p[i] * vp[-i];
              *vmptr += n_m[i] * sp_m[i] - d_m[i] * vm[i];
            }

          for (j = i; j <= 4; j++)
            {
              *vpptr += (n_p[j] - bd_p[j]) * initial_p[0];
              *vmptr += (n_m[j] - bd_m[j]) * initial_m[0];
            }

          sp_p ++;
          sp_m --;
          vp ++;
          vm --;
        }

      transfer_pixels (val_p, val_m, dest + row * width, 1, width);

      if (!preview)
        {
          progress += width;
          if ((row % 5) == 0)
            gimp_progress_update ((gdouble) progress / (gdouble) max_progress);
        }
    }

  /* Initialize the pixel regions. */
  gimp_pixel_rgn_init (&src_rgn, drawable, x1, y1, width, height, FALSE, FALSE);
  gimp_pixel_rgn_init (&dest_rgn, drawable,
                       x1, y1, width, height, (preview == NULL), TRUE);

  for (pr = gimp_pixel_rgns_register (2, &src_rgn, &dest_rgn);
       pr != NULL;
       pr = gimp_pixel_rgns_process (pr))
    {
      guchar *src_ptr  = src_rgn.data;
      guchar *dest_ptr = dest_rgn.data;
      guchar *blur_ptr = dest + (src_rgn.y - y1) * width + (src_rgn.x - x1);

      for (row = 0; row < src_rgn.h; row++)
        {
          for (col = 0; col < src_rgn.w; col++)
            {
              /* screen op */
              for (b = 0; b < (has_alpha ? (bytes - 1) : bytes); b++)
                dest_ptr[col * bytes + b] =
                  255 - INT_MULT((255 - src_ptr[col * bytes + b]),
                                 (255 - blur_ptr[col]), tmp);
              if (has_alpha)
                dest_ptr[col * bytes + b] = src_ptr[col * bytes + b];
            }

          src_ptr  += src_rgn.rowstride;
          dest_ptr += dest_rgn.rowstride;
          blur_ptr += width;
        }

      if (preview)
        {
          //gimp_drawable_preview_draw_region (GIMP_DRAWABLE_PREVIEW (preview),
          //                                   &dest_rgn);
        }
      else
        {
          progress += src_rgn.w * src_rgn.h;
          gimp_progress_update ((gdouble) progress / (gdouble) max_progress);
        }
    }

  if (! preview)
    {
      /*  merge the shadow, update the drawable  */
      gimp_drawable_flush (drawable);
      gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
      gimp_drawable_update (drawable->drawable_id,
                            x1, y1, (x2 - x1), (y2 - y1));
    }

  /*  free up buffers  */
  g_free (val_p);
  g_free (val_m);
  g_free (dest);
}

/*
 *  Gaussian blur helper functions
 */

static void
transfer_pixels (gdouble *src1,
                 gdouble *src2,
                 guchar  *dest,
                 gint     jump,
                 gint     width)
{
  gint    i;
  gdouble sum;

  for (i = 0; i < width; i++)
    {
      sum = src1[i] + src2[i];

      sum = CLAMP0255 (sum);

      *dest = (guchar) sum;
      dest += jump;
    }
}

static void
find_constants (gdouble n_p[],
                gdouble n_m[],
                gdouble d_p[],
                gdouble d_m[],
                gdouble bd_p[],
                gdouble bd_m[],
                gdouble std_dev)
{
  gint    i;
  gdouble constants [8];
  gdouble div;

  /*  The constants used in the implemenation of a casual sequence
   *  using a 4th order approximation of the gaussian operator
   */

  div = sqrt(2 * G_PI) * std_dev;

  constants [0] = -1.783  / std_dev;
  constants [1] = -1.723  / std_dev;
  constants [2] =  0.6318 / std_dev;
  constants [3] =  1.997  / std_dev;
  constants [4] =  1.6803 / div;
  constants [5] =  3.735  / div;
  constants [6] = -0.6803 / div;
  constants [7] = -0.2598 / div;

  n_p [0] = constants[4] + constants[6];
  n_p [1] = exp (constants[1]) *
    (constants[7] * sin (constants[3]) -
     (constants[6] + 2 * constants[4]) * cos (constants[3])) +
       exp (constants[0]) *
         (constants[5] * sin (constants[2]) -
          (2 * constants[6] + constants[4]) * cos (constants[2]));
  n_p [2] = 2 * exp (constants[0] + constants[1]) *
    ((constants[4] + constants[6]) * cos (constants[3]) * cos (constants[2]) -
     constants[5] * cos (constants[3]) * sin (constants[2]) -
     constants[7] * cos (constants[2]) * sin (constants[3])) +
       constants[6] * exp (2 * constants[0]) +
         constants[4] * exp (2 * constants[1]);
  n_p [3] = exp (constants[1] + 2 * constants[0]) *
    (constants[7] * sin (constants[3]) - constants[6] * cos (constants[3])) +
      exp (constants[0] + 2 * constants[1]) *
        (constants[5] * sin (constants[2]) - constants[4] * cos (constants[2]));
  n_p [4] = 0.0;

  d_p [0] = 0.0;
  d_p [1] = -2 * exp (constants[1]) * cos (constants[3]) -
    2 * exp (constants[0]) * cos (constants[2]);
  d_p [2] = 4 * cos (constants[3]) * cos (constants[2]) * exp (constants[0] + constants[1]) +
    exp (2 * constants[1]) + exp (2 * constants[0]);
  d_p [3] = -2 * cos (constants[2]) * exp (constants[0] + 2 * constants[1]) -
    2 * cos (constants[3]) * exp (constants[1] + 2 * constants[0]);
  d_p [4] = exp (2 * constants[0] + 2 * constants[1]);

#ifndef ORIGINAL_READABLE_CODE
  memcpy(d_m, d_p, 5 * sizeof(gdouble));
#else
  for (i = 0; i <= 4; i++)
    d_m [i] = d_p [i];
#endif

  n_m[0] = 0.0;
  for (i = 1; i <= 4; i++)
    n_m [i] = n_p[i] - d_p[i] * n_p[0];

  {
    gdouble sum_n_p, sum_n_m, sum_d;
    gdouble a, b;

    sum_n_p = 0.0;
    sum_n_m = 0.0;
    sum_d   = 0.0;

    for (i = 0; i <= 4; i++)
      {
        sum_n_p += n_p[i];
        sum_n_m += n_m[i];
        sum_d += d_p[i];
      }

#ifndef ORIGINAL_READABLE_CODE
    sum_d++;
    a = sum_n_p / sum_d;
    b = sum_n_m / sum_d;
#else
    a = sum_n_p / (1 + sum_d);
    b = sum_n_m / (1 + sum_d);
#endif

    for (i = 0; i <= 4; i++)
      {
        bd_p[i] = d_p[i] * a;
        bd_m[i] = d_m[i] * b;
      }
  }
}

void operation_softglow (void * in_buf, void * out_buf, long width, long height, softglow_config * config)
{
    GimpDrawable * drawable = gimp_drawable_get (0, in_buf, out_buf, width, height);

    svals = *config;
 
    softglow(drawable, NULL);

    gimp_drawable_detach (drawable);
}
