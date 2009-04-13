
#include "config.h"

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "libgimp/stdplugins-intl.h"

#include "lens-apply.h"

static void      drawlens    (GimpDrawable *drawable,
                              GimpPreview  *preview);

static lens_apply_config lvals =
{
  /* Lens refraction value */
  1.7,
  /* Surroundings options */
  TRUE, FALSE, FALSE
};

/*
  Ellipsoid formula: x^2/a^2 + y^2/b^2 + z^2/c^2 = 1
 */
static void
find_projected_pos (gfloat  a2,
                    gfloat  b2,
                    gfloat  c2,
                    gfloat  x,
                    gfloat  y,
                    gfloat *projx,
                    gfloat *projy)
{
  gfloat z;
  gfloat nxangle, nyangle, theta1, theta2;
  gfloat ri1 = 1.0;
  gfloat ri2 = lvals.refraction;

  z = sqrt ((1 - x * x / a2 - y * y / b2) * c2);

  nxangle = acos (x / sqrt(x * x + z * z));
  theta1 = G_PI / 2 - nxangle;
  theta2 = asin (sin (theta1) * ri1 / ri2);
  theta2 = G_PI / 2 - nxangle - theta2;
  *projx = x - tan (theta2) * z;

  nyangle = acos (y / sqrt (y * y + z * z));
  theta1 = G_PI / 2 - nyangle;
  theta2 = asin (sin (theta1) * ri1 / ri2);
  theta2 = G_PI / 2 - nyangle - theta2;
  *projy = y - tan (theta2) * z;
}

static void
drawlens (GimpDrawable *drawable,
          GimpPreview  *preview)
{
  GimpImageType  drawtype = gimp_drawable_type (drawable->drawable_id);
  GimpPixelRgn   srcPR, destPR;
  gint           width, height;
  gint           bytes;
  gint           row;
  gint           x1, y1, x2, y2;
  guchar        *src, *dest;
  gint           i, col;
  gfloat         regionwidth, regionheight, dx, dy, xsqr, ysqr;
  gfloat         a, b, c, asqr, bsqr, csqr, x, y;
  glong          pixelpos, pos;
  GimpRGB        background;
  guchar         bgr_red, bgr_blue, bgr_green;
  guchar         alphaval;

  //gimp_context_get_background (&background);
  gimp_rgb_get_uchar (&background,
                      &bgr_red, &bgr_green, &bgr_blue);

  bytes = drawable->bpp;

  if (preview)
    {
      //gimp_preview_get_position (preview, &x1, &y1);
      //gimp_preview_get_size (preview, &width, &height);
      //x2 = x1 + width;
      //y2 = y1 + height;
      //src = gimp_drawable_get_thumbnail_data (drawable->drawable_id,
      //                                        &width, &height, &bytes);
      //regionwidth  = width;
      //regionheight = height;
    }
  else
    {
      gimp_drawable_mask_bounds (drawable->drawable_id, &x1, &y1, &x2, &y2);
      regionwidth = x2 - x1;
      regionheight = y2 - y1;
      width = drawable->width;
      height = drawable->height;
      gimp_pixel_rgn_init (&srcPR, drawable,
                           0, 0, width, height, FALSE, FALSE);
      gimp_pixel_rgn_init (&destPR, drawable,
                           0, 0, width, height, TRUE, TRUE);

      src  = g_new (guchar, regionwidth * regionheight * bytes);
      gimp_pixel_rgn_get_rect (&srcPR, src,
                               x1, y1, regionwidth, regionheight);
    }

  dest = g_new (guchar, regionwidth * regionheight * bytes);

  a = regionwidth / 2;
  b = regionheight / 2;

  c = MIN (a, b);

  asqr = a * a;
  bsqr = b * b;
  csqr = c * c;

  for (col = 0; col < regionwidth; col++)
    {
      dx = (gfloat) col - a + 0.5;
      xsqr = dx * dx;
      for (row = 0; row < regionheight; row++)
        {
          pixelpos = (col + row * regionwidth) * bytes;
          dy = -((gfloat) row - b) - 0.5;
          ysqr = dy * dy;
          if (ysqr < (bsqr - (bsqr * xsqr) / asqr))
            {
              find_projected_pos (asqr, bsqr, csqr, dx, dy, &x, &y);
              y = -y;
              pos = ((gint) (y + b) * regionwidth + (gint) (x + a)) * bytes;

              for (i = 0; i < bytes; i++)
                {
                  dest[pixelpos + i] = src[pos + i];
                }
            }
          else
            {
              if (lvals.keep_surr)
                {
                  for (i = 0; i < bytes; i++)
                    {
                      dest[pixelpos + i] = src[pixelpos + i];
                    }
                }
              else
                {
                  if (lvals.set_transparent)
                    alphaval = 0;
                  else
                    alphaval = 255;

                  switch (drawtype)
                    {
                    case GIMP_INDEXEDA_IMAGE:
                      dest[pixelpos + 1] = alphaval;
                    case GIMP_INDEXED_IMAGE:
                      dest[pixelpos + 0] = 0;
                      break;

                    case GIMP_RGBA_IMAGE:
                      dest[pixelpos + 3] = alphaval;
                    case GIMP_RGB_IMAGE:
                      dest[pixelpos + 0] = bgr_red;
                      dest[pixelpos + 1] = bgr_green;
                      dest[pixelpos + 2] = bgr_blue;
                      break;

                    case GIMP_GRAYA_IMAGE:
                      dest[pixelpos + 1] = alphaval;
                    case GIMP_GRAY_IMAGE:
                      dest[pixelpos+0] = bgr_red;
                      break;
                    }
                }
            }
        }

      if (!preview)
        {
          if (((gint) (regionwidth-col) % 5) == 0)
            gimp_progress_update ((gdouble) col / (gdouble) regionwidth);
        }
    }

  if (preview)
    {
      //gimp_preview_draw_buffer (preview, dest, bytes * regionwidth);
    }
  else
    {
      gimp_pixel_rgn_set_rect (&destPR, dest, x1, y1,
                               regionwidth, regionheight);

      gimp_drawable_flush (drawable);
      gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
      gimp_drawable_update (drawable->drawable_id, x1, y1, x2 - x1, y2 - y1);
    }

  g_free (src);
  g_free (dest);
}

void operation_lens_apply (void * in_buf, void * out_buf, long width, long height, lens_apply_config * config)
{
    GimpDrawable * drawable = gimp_drawable_get (0, in_buf, out_buf, width, height);

    lvals = *config;
 
    drawlens (drawable, NULL);

    gimp_drawable_detach (drawable);
}
