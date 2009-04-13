/*
 * This is a plug-in for GIMP.
 *
 * Copyright (C) 1995 Spencer Kimball and Peter Mattis
 * Copyright (C) 1996 Torsten Martinsen
 * Copyright (C) 2007 Daniel Richard G.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <string.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>
#include <libgimpmath/gimpmath.h>

#include "libgimp/stdplugins-intl.h"

#include "oilify.h"

#define SCALE_WIDTH    125
#define HISTSIZE       256


static void      oilify         (GimpDrawable     *drawable,
                                 GimpPreview      *preview);

static oilify_config ovals =
{
  8.0,        /* mask size          */
  FALSE,      /* use mask-size map? */
  -1,         /* mask-size map      */
  8.0,        /* exponent           */
  FALSE,      /* use exponent map?  */
  -1,         /* exponent map       */
  MODE_INTEN  /* mode               */
};


/*
 * Helper function to read a sample from a mask-size/exponent map
 */
static inline gfloat
get_map_value (const guchar *src,
               gint          bpp)
{
  gfloat value;

  if (bpp >= 3)
    value = GIMP_RGB_LUMINANCE (src[0], src[1], src[2]);
  else
    value = *src;

  if (value < 1.0)
    value = 1.0;

  /*  value should be in [0,1]  */
  value /= 255.0;

  return value;
}

/*
 * This is a special-case form of the powf() function, limited to integer
 * exponents. It calculates e.g. x^13 as (x^8)*(x^4)*(x^1).
 *
 * x can be anything, y must be in [0,255]
 */
static inline gfloat
fast_powf (gfloat x, gint y)
{
  gfloat value;
  gfloat x_pow[8];
  guint  y_uint = (guint) y;
  guint  bitmask;
  gint   i;

  if (y_uint & 0x01)
    value = x;
  else
    value = 1.0;

  x_pow[0] = x;

  for (bitmask = 0x02, i = 1;
       bitmask <= y_uint;
       bitmask <<= 1, i++)
    {
      /*  x_pow[i] == x_pow[i-1]^2 == x^(2^i)  */

      x_pow[i] = SQR (x_pow[i - 1]);

      if (y_uint & bitmask)
        value *= x_pow[i];
    }

  return value;
}

/*
 * For each i in [0, HISTSIZE), hist[i] is the number of occurrences of the
 * value i. Return a value in [0, HISTSIZE) weighted heavily toward the
 * most frequent values in the histogram.
 *
 * Assuming that hist_max is the maximum number of occurrences for any
 * one value in the histogram, the weight given to each value i is
 *
 *         weight = (hist[i] / hist_max)^exponent
 *
 * (i.e. the normalized histogram frequency raised to some power)
 */
static inline guchar
weighted_average_value (gint hist[HISTSIZE], gfloat exponent)
{
  gint   i;
  gint   hist_max = 1;
  gint   exponent_int = 0;
  gfloat sum = 0.0;
  gfloat div = 1.0e-6;
  gint   value;

  for (i = 0; i < HISTSIZE; i++)
    hist_max = MAX (hist_max, hist[i]);

  if ((exponent - floor (exponent)) < 0.001 && exponent <= 255.0)
    exponent_int = (gint) exponent;

  for (i = 0; i < HISTSIZE; i++)
    {
      gfloat ratio = (gfloat) hist[i] / (gfloat) hist_max;
      gfloat weight;

      if (exponent_int)
        weight = fast_powf (ratio, exponent_int);
      else
        weight = pow (ratio, exponent);

      sum += weight * (gfloat) i;
      div += weight;
    }

  value = (gint) (sum / div);

  return (guchar) CLAMP0255 (value);
}

/*
 * For each i in [0, HISTSIZE), hist[i] is the number of occurrences of
 * pixels with intensity i. hist_rgb[][i] is the average color of those
 * pixels with intensity i, but with each channel multiplied by hist[i].
 * Write to dest a pixel whose color is a weighted average of all the
 * colors in hist_rgb[][], biased heavily toward those with the most
 * frequently-occurring intensities (as noted in hist[]).
 *
 * The weight formula is the same as in weighted_average_value().
 */
static inline void
weighted_average_color (gint    hist[HISTSIZE],
                        gint    hist_rgb[4][HISTSIZE],
                        gfloat  exponent,
                        guchar *dest,
                        gint    bpp)
{
  gint   i, b;
  gint   hist_max = 1;
  gint   exponent_int = 0;
  gfloat div = 1.0e-6;
  gfloat color[4] = { 0.0, 0.0, 0.0, 0.0 };

  for (i = 0; i < HISTSIZE; i++)
    hist_max = MAX (hist_max, hist[i]);

  if ((exponent - floor (exponent)) < 0.001 && exponent <= 255.0)
    exponent_int = (gint) exponent;

  for (i = 0; i < HISTSIZE; i++)
    {
      gfloat ratio = (gfloat) hist[i] / (gfloat) hist_max;
      gfloat weight;

      if (exponent_int)
        weight = fast_powf (ratio, exponent_int);
      else
        weight = pow (ratio, exponent);

      if (hist[i] > 0)
        for (b = 0; b < bpp; b++)
          color[b] += weight * (gfloat) hist_rgb[b][i] / (gfloat) hist[i];

      div += weight;
    }

  for (b = 0; b < bpp; b++)
    {
      gint c = (gint) (color[b] / div);

      dest[b] = (guchar) CLAMP0255 (c);
    }
}

/*
 * For all x and y as requested, replace the pixel at (x,y)
 * with a weighted average of the most frequently occurring
 * values in a circle of mask_size diameter centered at (x,y).
 */
static void
oilify (GimpDrawable *drawable,
        GimpPreview  *preview)
{
  gboolean      use_inten;
  gboolean      use_msmap = FALSE;
  gboolean      use_emap = FALSE;
  GimpDrawable *mask_size_map_drawable = NULL;
  GimpDrawable *exponent_map_drawable = NULL;
  GimpPixelRgn  mask_size_map_rgn;
  GimpPixelRgn  exponent_map_rgn;
  gint          msmap_bpp = 0;
  gint          emap_bpp = 0;
  GimpPixelRgn  dest_rgn;
  GimpPixelRgn *regions[3];
  gint          n_regions;
  gint          bpp;
  gint         *sqr_lut;
  gint          x1, y1, x2, y2;
  gint          width, height;
  gint          Hist[HISTSIZE];
  gint          Hist_rgb[4][HISTSIZE];
  gpointer      pr;
  gint          progress, max_progress;
  guchar       *src_buf;
  guchar       *src_inten_buf = NULL;
  gint          i;

  use_inten = (ovals.mode == MODE_INTEN);

  /*  Get the selection bounds  */
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
      width  = x2 - x1;
      height = y2 - y1;
    }

  progress = 0;
  max_progress = width * height;

  bpp = drawable->bpp;

  /*
   * Look-up-table implementation of the square function, for use in the
   * VERY TIGHT inner loops
   */
  {
    gint lut_size = (gint) ovals.mask_size / 2 + 1;

    sqr_lut = g_new (gint, lut_size);

    for (i = 0; i < lut_size; i++)
      sqr_lut[i] = SQR (i);
  }

  /*  Get the map drawables, if applicable  */

  if (ovals.use_mask_size_map && ovals.mask_size_map >= 0)
    {
      use_msmap = TRUE;

      mask_size_map_drawable = gimp_drawable_get_1 (ovals.mask_size_map);
      gimp_pixel_rgn_init (&mask_size_map_rgn, mask_size_map_drawable,
                           x1, y1, width, height, FALSE, FALSE);

      msmap_bpp = mask_size_map_drawable->bpp;
    }

  if (ovals.use_exponent_map && ovals.exponent_map >= 0)
    {
      use_emap = TRUE;

      exponent_map_drawable = gimp_drawable_get_1 (ovals.exponent_map);
      gimp_pixel_rgn_init (&exponent_map_rgn, exponent_map_drawable,
                           x1, y1, width, height, FALSE, FALSE);

      emap_bpp = exponent_map_drawable->bpp;
    }

  gimp_pixel_rgn_init (&dest_rgn, drawable,
                       x1, y1, width, height, (preview == NULL), TRUE);

  {
    GimpPixelRgn src_rgn;

    gimp_pixel_rgn_init (&src_rgn, drawable,
                         x1, y1, width, height, FALSE, FALSE);
    src_buf = g_new (guchar, width * height * bpp);
    gimp_pixel_rgn_get_rect (&src_rgn, src_buf, x1, y1, width, height);
  }

  /*
   * If we're working in intensity mode, then generate a separate intensity
   * map of the source image. This way, we can avoid calculating the
   * intensity of any given source pixel more than once.
   */
  if (use_inten)
    {
      guchar *src;
      guchar *dest;

      src_inten_buf = g_new (guchar, width * height);

      for (i = 0,
           src = src_buf,
           dest = src_inten_buf
           ;
           i < (width * height)
           ;
           i++,
           src += bpp,
           dest++)
        {
          *dest = (guchar) GIMP_RGB_LUMINANCE (src[0], src[1], src[2]);
        }
    }

  n_regions = 0;
  regions[n_regions++] = &dest_rgn;
  if (use_msmap)
    regions[n_regions++] = &mask_size_map_rgn;
  if (use_emap)
    regions[n_regions++] = &exponent_map_rgn;

  for (pr = gimp_pixel_rgns_register2 (n_regions, regions);
       pr != NULL;
       pr = gimp_pixel_rgns_process (pr))
    {
      gint    y;
      guchar *dest_row;
      guchar *src_msmap_row = NULL;
      guchar *src_emap_row = NULL;

      for (y = dest_rgn.y,
           dest_row = dest_rgn.data,
           src_msmap_row = mask_size_map_rgn.data,  /* valid iff use_msmap */
           src_emap_row = exponent_map_rgn.data     /* valid iff use_emap */
           ;
           y < (gint) (dest_rgn.y + dest_rgn.h)
           ;
           y++,
           dest_row += dest_rgn.rowstride,
           src_msmap_row += mask_size_map_rgn.rowstride,  /* valid iff use_msmap */
           src_emap_row += exponent_map_rgn.rowstride)    /* valid iff use_emap */
        {
          gint    x;
          guchar *dest;
          guchar *src_msmap = NULL;
          guchar *src_emap = NULL;

          for (x = dest_rgn.x,
               dest = dest_row,
               src_msmap = src_msmap_row,  /* valid iff use_msmap */
               src_emap = src_emap_row     /* valid iff use_emap */
               ;
               x < (gint) (dest_rgn.x + dest_rgn.w)
               ;
               x++,
               dest += bpp,
               src_msmap += msmap_bpp,  /* valid iff use_msmap */
               src_emap += emap_bpp)    /* valid iff use_emap */
            {
              gint    radius, radius_squared;
              gfloat  exponent;
              gint    mask_x1, mask_y1;
              gint    mask_x2, mask_y2;
              gint    mask_y;
              gint    src_offset;
              guchar *src_row;
              guchar *src_inten_row = NULL;

              if (use_msmap)
                {
                  gfloat factor = get_map_value (src_msmap, msmap_bpp);

                  radius = ROUND (factor * (0.5 * ovals.mask_size));
                }
              else
                {
                  radius = (gint) ovals.mask_size / 2;
                }

              radius_squared = SQR (radius);

              exponent = ovals.exponent;
              if (use_emap)
                exponent *= get_map_value (src_emap, emap_bpp);

              if (use_inten)
                memset (Hist, 0, sizeof (Hist));

              memset (Hist_rgb, 0, sizeof (Hist_rgb));

              mask_x1 = CLAMP ((x - radius), x1, x2);
              mask_y1 = CLAMP ((y - radius), y1, y2);
              mask_x2 = CLAMP ((x + radius + 1), x1, x2);
              mask_y2 = CLAMP ((y + radius + 1), y1, y2);

              src_offset = (mask_y1 - y1) * width + (mask_x1 - x1);

              for (mask_y = mask_y1,
                   src_row = src_buf + src_offset * bpp,
                   src_inten_row = src_inten_buf + src_offset  /* valid iff use_inten */
                   ;
                   mask_y < mask_y2
                   ;
                   mask_y++,
                   src_row += width * bpp,
                   src_inten_row += width)  /* valid iff use_inten */
                {
                  guchar *src;
                  guchar *src_inten = NULL;
                  gint    dy_squared = sqr_lut[ABS (mask_y - y)];
                  gint    mask_x;

                  for (mask_x = mask_x1,
                       src = src_row,
                       src_inten = src_inten_row  /* valid iff use_inten */
                       ;
                       mask_x < mask_x2
                       ;
                       mask_x++,
                       src += bpp,
                       src_inten++)  /* valid iff use_inten */
                    {
                      gint dx_squared = sqr_lut[ABS (mask_x - x)];
                      gint b;

                      /*  Stay inside a circular mask area  */
                      if ((dx_squared + dy_squared) > radius_squared)
                        continue;

                      if (use_inten)
                        {
                          gint inten = *src_inten;
                          ++Hist[inten];
                          for (b = 0; b < bpp; b++)
                            Hist_rgb[b][inten] += src[b];
                        }
                      else
                        {
                          for (b = 0; b < bpp; b++)
                            ++Hist_rgb[b][src[b]];
                        }

                    } /* for mask_x */
                } /* for mask_y */

              if (use_inten)
                {
                  weighted_average_color (Hist, Hist_rgb, exponent, dest, bpp);
                }
              else
                {
                  gint b;

                  for (b = 0; b < bpp; b++)
                    dest[b] = weighted_average_value (Hist_rgb[b], exponent);
                }

            } /* for x */
        } /* for y */

      if (preview)
        {
          //gimp_drawable_preview_draw_region (GIMP_DRAWABLE_PREVIEW (preview),
          //                                   &dest_rgn);
        }
      else
        {
          progress += dest_rgn.w * dest_rgn.h;
          gimp_progress_update ((gdouble) progress / (gdouble) max_progress);
        }
    } /* for pr */

  /*  Detach from the map drawables  */
  if (mask_size_map_drawable)
    gimp_drawable_detach (mask_size_map_drawable);

  if (exponent_map_drawable)
    gimp_drawable_detach (exponent_map_drawable);

  if (src_inten_buf)
    g_free (src_inten_buf);

  g_free (src_buf);
  g_free (sqr_lut);

  if (!preview)
    {
      /*  Update the oil-painted region  */
      gimp_drawable_flush (drawable);
      gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
      gimp_drawable_update (drawable->drawable_id, x1, y1, width, height);
    }
}

/*
 * Return TRUE iff the specified drawable can be used as a mask-size /
 * exponent map with the source image. The map and the image must have the
 * same dimensions.
 */
static gboolean
oilify_map_constrain (gint32   image_id G_GNUC_UNUSED,
                      gint32   drawable_id,
                      gpointer data)
{
  GimpDrawable *drawable = data;

  return (gimp_drawable_width (drawable_id)  == (gint) drawable->width &&
          gimp_drawable_height (drawable_id) == (gint) drawable->height);
}

void operation_oilify (void * in_buf, void * out_buf, long width, long height, oilify_config * config)
{
    GimpDrawable * drawable = gimp_drawable_get (0, in_buf, out_buf, width, height);

    ovals = *config;
 
    oilify(drawable, NULL);

    gimp_drawable_detach (drawable);
}
