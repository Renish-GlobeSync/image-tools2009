
#include "config.h"

#include <libgimp/gimp.h>

#include "deinterlace.h"


/* Declare local functions.
 */

static void      deinterlace        (GimpDrawable *drawable,
                                     GimpPreview  *preview);

static deinterlace_config devals =
{
  EVEN_FIELDS   /* evenness */
};

static void
deinterlace (GimpDrawable *drawable,
             GimpPreview  *preview)
{
  GimpPixelRgn  srcPR, destPR;
  gboolean      has_alpha;
  guchar       *dest;
  guchar       *dest_buffer = NULL;
  guchar       *upper;
  guchar       *lower;
  gint          row, col;
  gint          x, y;
  gint          width, height;
  gint          bytes;

  bytes = drawable->bpp;

  if (preview)
    {
      //gimp_preview_get_position (preview, &x, &y);
      //gimp_preview_get_size (preview, &width, &height);

      //dest_buffer = g_new (guchar, width * height * bytes);
      //dest = dest_buffer;
    }
  else
    {
      gint x2, y2;

      gimp_drawable_mask_bounds (drawable->drawable_id, &x, &y, &x2, &y2);

      width  = x2 - x;
      height = y2 - y;

      dest = g_new (guchar, width * bytes);

      gimp_pixel_rgn_init (&destPR, drawable, x, y, width, height, TRUE, TRUE);
    }

  gimp_pixel_rgn_init (&srcPR, drawable,
                       x, MAX (y - 1, 0),
                       width, MIN (height + 1, drawable->height),
                       FALSE, FALSE);

  has_alpha = gimp_drawable_has_alpha (drawable->drawable_id);

  /*  allocate row buffers  */
  upper = g_new (guchar, width * bytes);
  lower = g_new (guchar, width * bytes);

  /*  loop through the rows, performing our magic  */
  for (row = y; row < y + height; row++)
    {
      gimp_pixel_rgn_get_row (&srcPR, dest, x, row, width);

      if (row % 2 != devals.evenness)
        {
          if (row > 0)
            gimp_pixel_rgn_get_row (&srcPR, upper, x, row - 1, width);
          else
            gimp_pixel_rgn_get_row (&srcPR, upper, x, devals.evenness, width);

          if (row + 1 < drawable->height)
            gimp_pixel_rgn_get_row (&srcPR, lower, x, row + 1, width);
          else
            gimp_pixel_rgn_get_row (&srcPR, lower, x, row - 1 + devals.evenness,
                                    width);

          if (has_alpha)
            {
              const guchar *upix = upper;
              const guchar *lpix = lower;
              guchar       *dpix = dest;

              for (col = 0; col < width; col++)
                {
                  guint ualpha = upix[bytes - 1];
                  guint lalpha = lpix[bytes - 1];
                  guint alpha  = ualpha + lalpha;

                  if ((dpix[bytes - 1] = (alpha >> 1)))
                    {
                      gint b;

                      for (b = 0; b < bytes - 1; b++)
                        dpix[b] = (upix[b] * ualpha + lpix[b] * lalpha) / alpha;
                    }

                  upix += bytes;
                  lpix += bytes;
                  dpix += bytes;
                }
            }
          else
            {
              for (col = 0; col < width * bytes; col++)
                dest[col] = ((guint) upper[col] + (guint) lower[col]) / 2;
            }
        }

      if (preview)
        {
          //dest += width * bytes;
        }
      else
        {
          gimp_pixel_rgn_set_row (&destPR, dest, x, row, width);

          if ((row % 20) == 0)
              gimp_progress_update ((double) row / (double) (height));
        }
    }

  if (preview)
    {
      //gimp_preview_draw_buffer (preview, dest_buffer, width * bytes);
      //dest = dest_buffer;
    }
  else
    {
      /*  update the deinterlaced region  */
      gimp_drawable_flush (drawable);
      gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
      gimp_drawable_update (drawable->drawable_id, x, y, width, height);
    }

  g_free (lower);
  g_free (upper);
  g_free (dest);
}

void operation_deinterlace (void * in_buf, void * out_buf, long width, long height, deinterlace_config * config)
{
    GimpDrawable * drawable = gimp_drawable_get (0, in_buf, out_buf, width, height);

    devals = *config;
 
    deinterlace(drawable, NULL);

    gimp_drawable_detach (drawable);
}