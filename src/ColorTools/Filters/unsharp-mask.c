/*
 * Copyright (C) 1999 Winston Chang
 *                    <winstonc@cs.wisc.edu>
 *                    <winston@stdout.org>
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

#include <stdlib.h>

#include <libgimp/gimp.h>

#include "unsharp_mask.h"

#define SCALE_WIDTH   120
#define ENTRY_WIDTH     5

/* Uncomment this line to get a rough estimate of how long the plug-in
 * takes to run.
 */

/*  #define TIMER  */


static void      blur_line           (const gdouble  *ctable,
                                      const gdouble  *cmatrix,
                                      const gint      cmatrix_length,
                                      const guchar   *src,
                                      guchar         *dest,
                                      const gint      len,
                                      const gint      bytes);
static gint      gen_convolve_matrix (gdouble         std_dev,
                                      gdouble       **cmatrix);
static gdouble * gen_lookup_table    (const gdouble  *cmatrix,
                                      gint            cmatrix_length);
static void      unsharp_region      (GimpPixelRgn   *srcPTR,
                                      GimpPixelRgn   *dstPTR,
                                      gint            bytes,
                                      gdouble         radius,
                                      gdouble         amount,
                                      gint            x1,
                                      gint            x2,
                                      gint            y1,
                                      gint            y2,
                                      gboolean        show_progress);

static void      unsharp_mask        (GimpDrawable   *drawable,
                                      gdouble         radius,
                                      gdouble         amount);


/* create a few globals, set default values */
static unsharp_mask_config unsharp_params =
{
  5.0, /* default radius    */
  0.5, /* default amount    */
  0    /* default threshold */
};


/* This function is written as if it is blurring a column at a time,
 * even though it can operate on rows, too.  There is no difference
 * in the processing of the lines, at least to the blur_line function.
 */
static void
blur_line (const gdouble *ctable,
           const gdouble *cmatrix,
           const gint     cmatrix_length,
           const guchar  *src,
           guchar        *dest,
           const gint     len,
           const gint     bytes)
{
  const gdouble *cmatrix_p;
  const gdouble *ctable_p;
  const guchar  *src_p;
  const guchar  *src_p1;
  const gint     cmatrix_middle = cmatrix_length / 2;
  gint           row;
  gint           i, j;

  /* This first block is the same as the optimized version --
   * it is only used for very small pictures, so speed isn't a
   * big concern.
   */
  if (cmatrix_length > len)
    {
      for (row = 0; row < len; row++)
        {
          /* find the scale factor */
          gdouble scale = 0;

          for (j = 0; j < len; j++)
            {
              /* if the index is in bounds, add it to the scale counter */
              if (j + cmatrix_middle - row >= 0 &&
                  j + cmatrix_middle - row < cmatrix_length)
                scale += cmatrix[j];
            }

          src_p = src;

          for (i = 0; i < bytes; i++)
            {
              gdouble sum = 0;

              src_p1 = src_p++;

              for (j = 0; j < len; j++)
                {
                  if (j + cmatrix_middle - row >= 0 &&
                      j + cmatrix_middle - row < cmatrix_length)
                    sum += *src_p1 * cmatrix[j];

                  src_p1 += bytes;
                }

              *dest++ = (guchar) ROUND (sum / scale);
            }
        }
    }
  else
    {
      /* for the edge condition, we only use available info and scale to one */
      for (row = 0; row < cmatrix_middle; row++)
        {
          /* find scale factor */
          gdouble scale = 0;

          for (j = cmatrix_middle - row; j < cmatrix_length; j++)
            scale += cmatrix[j];

          src_p = src;

          for (i = 0; i < bytes; i++)
            {
              gdouble sum = 0;

              src_p1 = src_p++;

              for (j = cmatrix_middle - row; j < cmatrix_length; j++)
                {
                  sum += *src_p1 * cmatrix[j];
                  src_p1 += bytes;
                }

              *dest++ = (guchar) ROUND (sum / scale);
            }
        }

      /* go through each pixel in each col */
      for (; row < len - cmatrix_middle; row++)
        {
          src_p = src + (row - cmatrix_middle) * bytes;

          for (i = 0; i < bytes; i++)
            {
              gdouble sum = 0;

              cmatrix_p = cmatrix;
              src_p1 = src_p;
              ctable_p = ctable;

              for (j = 0; j < cmatrix_length; j++)
                {
                  sum += cmatrix[j] * *src_p1;
                  src_p1 += bytes;
                  ctable_p += 256;
                }

              src_p++;
              *dest++ = (guchar) ROUND (sum);
            }
        }

      /* for the edge condition, we only use available info and scale to one */
      for (; row < len; row++)
        {
          /* find scale factor */
          gdouble scale = 0;

          for (j = 0; j < len - row + cmatrix_middle; j++)
            scale += cmatrix[j];

          src_p = src + (row - cmatrix_middle) * bytes;

          for (i = 0; i < bytes; i++)
            {
              gdouble sum = 0;

              src_p1 = src_p++;

              for (j = 0; j < len - row + cmatrix_middle; j++)
                {
                  sum += *src_p1 * cmatrix[j];
                  src_p1 += bytes;
                }

              *dest++ = (guchar) ROUND (sum / scale);
            }
        }
    }
}

static void
unsharp_mask (GimpDrawable *drawable,
              gdouble       radius,
              gdouble       amount)
{
  GimpPixelRgn srcPR, destPR;
  gint         x1, y1, x2, y2;

  /* initialize pixel regions */
  gimp_pixel_rgn_init (&srcPR, drawable,
                       0, 0, drawable->width, drawable->height, FALSE, FALSE);
  gimp_pixel_rgn_init (&destPR, drawable,
                       0, 0, drawable->width, drawable->height, TRUE, TRUE);

  /* Get the input */
  gimp_drawable_mask_bounds (drawable->drawable_id, &x1, &y1, &x2, &y2);

  unsharp_region (&srcPR, &destPR, drawable->bpp,
                  radius, amount,
                  x1, x2, y1, y2,
                  TRUE);

  gimp_drawable_flush (drawable);
  gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
  gimp_drawable_update (drawable->drawable_id, x1, y1, x2 - x1, y2 - y1);
}

/* Perform an unsharp mask on the region, given a source region, dest.
 * region, width and height of the regions, and corner coordinates of
 * a subregion to act upon.  Everything outside the subregion is unaffected.
 */
static void
unsharp_region (GimpPixelRgn *srcPR,
                GimpPixelRgn *destPR,
                gint          bytes,
                gdouble       radius,
                gdouble       amount,
                gint          x1,
                gint          x2,
                gint          y1,
                gint          y2,
                gboolean      show_progress)
{
  guchar  *src;
  guchar  *dest;
  gint     width   = x2 - x1;
  gint     height  = y2 - y1;
  gdouble *cmatrix = NULL;
  gint     cmatrix_length;
  gdouble *ctable;
  gint     row, col;
  gint     threshold = unsharp_params.threshold;

  if (show_progress)
    gimp_progress_init (_("Blurring"));

  /* generate convolution matrix
     and make sure it's smaller than each dimension */
  cmatrix_length = gen_convolve_matrix (radius, &cmatrix);

  /* generate lookup table */
  ctable = gen_lookup_table (cmatrix, cmatrix_length);

  /* allocate buffers */
  src  = g_new (guchar, MAX (width, height) * bytes);
  dest = g_new (guchar, MAX (width, height) * bytes);

  /* blur the rows */
  for (row = 0; row < height; row++)
    {
      gimp_pixel_rgn_get_row (srcPR, src, x1, y1 + row, width);
      blur_line (ctable, cmatrix, cmatrix_length, src, dest, width, bytes);
      gimp_pixel_rgn_set_row (destPR, dest, x1, y1 + row, width);

      if (show_progress && row % 8 == 0)
        gimp_progress_update ((gdouble) row / (3 * height));
    }

  /* blur the cols */
  for (col = 0; col < width; col++)
    {
      gimp_pixel_rgn_get_col (destPR, src, x1 + col, y1, height);
      blur_line (ctable, cmatrix, cmatrix_length, src, dest, height, bytes);
      gimp_pixel_rgn_set_col (destPR, dest, x1 + col, y1, height);

      if (show_progress && col % 8 == 0)
        gimp_progress_update ((gdouble) col / (3 * width) + 0.33);
    }

  if (show_progress)
    gimp_progress_set_text (_("Merging"));

  /* merge the source and destination (which currently contains
     the blurred version) images */
  for (row = 0; row < height; row++)
    {
      const guchar *s = src;
      guchar       *d = dest;
      gint          u, v;

      /* get source row */
      gimp_pixel_rgn_get_row (srcPR, src, x1, y1 + row, width);

      /* get dest row */
      gimp_pixel_rgn_get_row (destPR, dest, x1, y1 + row, width);

      /* combine the two */
      for (u = 0; u < width; u++)
        {
          for (v = 0; v < bytes; v++)
            {
              gint value;
              gint diff = *s - *d;

              /* do tresholding */
              if (abs (2 * diff) < threshold)
                diff = 0;

              value = *s++ + amount * diff;
              *d++ = CLAMP (value, 0, 255);
            }
        }

      if (show_progress && row % 8 == 0)
        gimp_progress_update ((gdouble) row / (3 * height) + 0.67);

      gimp_pixel_rgn_set_row (destPR, dest, x1, y1 + row, width);
    }

  if (show_progress)
    gimp_progress_update (1.0);

  g_free (dest);
  g_free (src);
  g_free (ctable);
  g_free (cmatrix);
}

/* generates a 1-D convolution matrix to be used for each pass of
 * a two-pass gaussian blur.  Returns the length of the matrix.
 */
static gint
gen_convolve_matrix (gdouble   radius,
                     gdouble **cmatrix_p)
{
  gdouble *cmatrix;
  gdouble  std_dev;
  gdouble  sum;
  gint     matrix_length;
  gint     i, j;

  /* we want to generate a matrix that goes out a certain radius
   * from the center, so we have to go out ceil(rad-0.5) pixels,
   * inlcuding the center pixel.  Of course, that's only in one direction,
   * so we have to go the same amount in the other direction, but not count
   * the center pixel again.  So we double the previous result and subtract
   * one.
   * The radius parameter that is passed to this function is used as
   * the standard deviation, and the radius of effect is the
   * standard deviation * 2.  It's a little confusing.
   */
  radius = fabs (radius) + 1.0;

  std_dev = radius;
  radius = std_dev * 2;

  /* go out 'radius' in each direction */
  matrix_length = 2 * ceil (radius - 0.5) + 1;
  if (matrix_length <= 0)
    matrix_length = 1;

  *cmatrix_p = g_new (gdouble, matrix_length);
  cmatrix = *cmatrix_p;

  /*  Now we fill the matrix by doing a numeric integration approximation
   * from -2*std_dev to 2*std_dev, sampling 50 points per pixel.
   * We do the bottom half, mirror it to the top half, then compute the
   * center point.  Otherwise asymmetric quantization errors will occur.
   *  The formula to integrate is e^-(x^2/2s^2).
   */

  /* first we do the top (right) half of matrix */
  for (i = matrix_length / 2 + 1; i < matrix_length; i++)
    {
      gdouble base_x = i - (matrix_length / 2) - 0.5;

      sum = 0;
      for (j = 1; j <= 50; j++)
        {
          gdouble r = base_x + 0.02 * j;

          if (r <= radius)
            sum += exp (- SQR (r) / (2 * SQR (std_dev)));
        }

      cmatrix[i] = sum / 50;
    }

  /* mirror the thing to the bottom half */
  for (i = 0; i <= matrix_length / 2; i++)
    cmatrix[i] = cmatrix[matrix_length - 1 - i];

  /* find center val -- calculate an odd number of quanta to make it symmetric,
   * even if the center point is weighted slightly higher than others. */
  sum = 0;
  for (j = 0; j <= 50; j++)
    sum += exp (- SQR (- 0.5 + 0.02 * j) / (2 * SQR (std_dev)));

  cmatrix[matrix_length / 2] = sum / 51;

  /* normalize the distribution by scaling the total sum to one */
  sum = 0;
  for (i = 0; i < matrix_length; i++)
    sum += cmatrix[i];

  for (i = 0; i < matrix_length; i++)
    cmatrix[i] = cmatrix[i] / sum;

  return matrix_length;
}

/* ----------------------- gen_lookup_table ----------------------- */
/* generates a lookup table for every possible product of 0-255 and
   each value in the convolution matrix.  The returned array is
   indexed first by matrix position, then by input multiplicand (?)
   value.
*/
static gdouble *
gen_lookup_table (const gdouble *cmatrix,
                  gint           cmatrix_length)
{
  gdouble       *lookup_table   = g_new (gdouble, cmatrix_length * 256);
  gdouble       *lookup_table_p = lookup_table;
  const gdouble *cmatrix_p      = cmatrix;
  gint           i, j;

  for (i = 0; i < cmatrix_length; i++)
    {
      for (j = 0; j < 256; j++)
        *(lookup_table_p++) = *cmatrix_p * (gdouble) j;

      cmatrix_p++;
    }

  return lookup_table;
}

void operation_unsharp_mask (void * in_buf, void * out_buf, long width, long height, unsharp_mask_config * config)
{
    GimpDrawable * drawable = gimp_drawable_get (0, in_buf, out_buf, width, height);

    unsharp_params = *config;
 
    unsharp_mask (drawable, unsharp_params.radius, unsharp_params.amount);

    gimp_drawable_detach (drawable);
}
