
#include "config.h"

#include <string.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "libgimp/stdplugins-intl.h"

#include "cartoon.h"

/*
 * Function prototypes.
 */

static void      cartoon        (GimpDrawable *drawable,
                                 GimpPreview  *preview);

static gdouble   compute_ramp   (guchar       *dest1,
                                 guchar       *dest2,
                                 gint          length,
                                 gdouble       pct_black);

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
                                    gint     bytes,
                                    gint     width);


/***** Local vars *****/

static cartoon_config cvals =
{
  7.0,  /* mask_radius */
  1.0,  /* threshold */
  0.2   /* pct_black */
};

/*
 * Cartoon algorithm
 * -----------------
 * Mask radius = radius of pixel neighborhood for intensity comparison
 * Threshold   = relative intensity difference which will result in darkening
 * Ramp        = amount of relative intensity difference before total black
 * Blur radius = mask radius / 3.0
 *
 * Algorithm:
 * For each pixel, calculate pixel intensity value to be: avg (blur radius)
 * relative diff = pixel intensity / avg (mask radius)
 * If relative diff < Threshold
 *   intensity mult = (Ramp - MIN (Ramp, (Threshold - relative diff))) / Ramp
 *   pixel intensity *= intensity mult
 */
static void
cartoon (GimpDrawable *drawable,
         GimpPreview  *preview)
{
  GimpPixelRgn  src_rgn, dest_rgn;
  GimpPixelRgn *pr;
  gint          width, height;
  gint          bytes;
  gboolean      has_alpha;
  guchar       *dest1;
  guchar       *dest2;
  guchar       *src;
  guchar       *src1, *sp_p1, *sp_m1;
  guchar       *src2, *sp_p2, *sp_m2;
  gdouble       n_p1[5], n_m1[5];
  gdouble       n_p2[5], n_m2[5];
  gdouble       d_p1[5], d_m1[5];
  gdouble       d_p2[5], d_m2[5];
  gdouble       bd_p1[5], bd_m1[5];
  gdouble       bd_p2[5], bd_m2[5];
  gdouble      *val_p1, *val_m1, *vp1, *vm1;
  gdouble      *val_p2, *val_m2, *vp2, *vm2;
  gint          x1, y1, x2, y2;
  gint          i, j;
  gint          row, col, b;
  gint          terms;
  gint          progress, max_progress;
  gint          initial_p1[4];
  gint          initial_p2[4];
  gint          initial_m1[4];
  gint          initial_m2[4];
  gdouble       radius;
  gdouble       std_dev1;
  gdouble       std_dev2;
  gdouble       ramp;
  guchar       *preview_buffer = NULL;

  if (preview)
    {
      //gimp_preview_get_position (preview, &x1, &y1);
      //gimp_preview_get_size (preview, &width, &height);
    }
  else
    {
      gimp_drawable_mask_bounds (drawable->drawable_id, &x1, &y1, &x2, &y2);

      width     = (x2 - x1);
      height    = (y2 - y1);
    }

  bytes     = drawable->bpp;
  has_alpha = gimp_drawable_has_alpha (drawable->drawable_id);

  val_p1 = g_new (gdouble, MAX (width, height) * bytes);
  val_p2 = g_new (gdouble, MAX (width, height) * bytes);
  val_m1 = g_new (gdouble, MAX (width, height) * bytes);
  val_m2 = g_new (gdouble, MAX (width, height) * bytes);

  src   = g_new (guchar, MAX (width, height) * bytes);
  dest1 = g_new0 (guchar, width * height);
  dest2 = g_new0 (guchar, width * height);

  gimp_pixel_rgn_init (&src_rgn, drawable,
                       0, 0, drawable->width, drawable->height, FALSE, FALSE);

  progress = 0;
  max_progress = width * height * 2;

  /*  Calculate the standard deviations  */
  radius   = 1.0; /* blur radius */
  radius   = fabs (radius) + 1.0;
  std_dev1 = sqrt (-(radius * radius) / (2 * log (1.0 / 255.0)));

  radius   = cvals.mask_radius;
  radius   = fabs (radius) + 1.0;
  std_dev2 = sqrt (-(radius * radius) / (2 * log (1.0 / 255.0)));

  /*  derive the constants for calculating the gaussian from the std dev  */
  find_constants (n_p1, n_m1, d_p1, d_m1, bd_p1, bd_m1, std_dev1);
  find_constants (n_p2, n_m2, d_p2, d_m2, bd_p2, bd_m2, std_dev2);

  /*  First the vertical pass  */
  for (col = 0; col < width; col++)
    {
      memset (val_p1, 0, height * bytes * sizeof (gdouble));
      memset (val_p2, 0, height * bytes * sizeof (gdouble));
      memset (val_m1, 0, height * bytes * sizeof (gdouble));
      memset (val_m2, 0, height * bytes * sizeof (gdouble));

      gimp_pixel_rgn_get_col (&src_rgn, src, col + x1, y1, height);

      src1  = src;
      sp_p1 = src1;
      sp_m1 = src1 + (height - 1) * bytes;
      vp1   = val_p1;
      vp2   = val_p2;
      vm1   = val_m1 + (height - 1) * bytes;
      vm2   = val_m2 + (height - 1) * bytes;

      /*  Set up the first vals  */
      for (i = 0; i < bytes; i++)
        {
          initial_p1[i] = sp_p1[i];
          initial_m1[i] = sp_m1[i];
        }

      for (row = 0; row < height; row++)
        {
          gdouble *vpptr1, *vmptr1;
          gdouble *vpptr2, *vmptr2;

          terms = (row < 4) ? row : 4;

          for (b = 0; b < bytes; b++)
            {
              vpptr1 = vp1 + b; vmptr1 = vm1 + b;
              vpptr2 = vp2 + b; vmptr2 = vm2 + b;

              for (i = 0; i <= terms; i++)
                {
                  *vpptr1 += n_p1[i] * sp_p1[(-i * bytes) + b] -
                    d_p1[i] * vp1[(-i * bytes) + b];
                  *vmptr1 += n_m1[i] * sp_m1[(i * bytes) + b] -
                    d_m1[i] * vm1[(i * bytes) + b];

                  *vpptr2 += n_p2[i] * sp_p1[(-i * bytes) + b] -
                    d_p2[i] * vp2[(-i * bytes) + b];
                  *vmptr2 += n_m2[i] * sp_m1[(i * bytes) + b] -
                    d_m2[i] * vm2[(i * bytes) + b];
                }

              for (j = i; j <= 4; j++)
                {
                  *vpptr1 += (n_p1[j] - bd_p1[j]) * initial_p1[b];
                  *vmptr1 += (n_m1[j] - bd_m1[j]) * initial_m1[b];

                  *vpptr2 += (n_p2[j] - bd_p2[j]) * initial_p1[b];
                  *vmptr2 += (n_m2[j] - bd_m2[j]) * initial_m1[b];
                }
            }

          sp_p1 += bytes;
          sp_m1 -= bytes;
          vp1   += bytes;
          vp2   += bytes;
          vm1   -= bytes;
          vm2   -= bytes;
        }

      transfer_pixels (val_p1, val_m1, dest1 + col, width, bytes, height);
      transfer_pixels (val_p2, val_m2, dest2 + col, width, bytes, height);

      if (!preview)
        {
          progress += height;
          if ((col % 5) == 0)
            gimp_progress_update ((gdouble) progress / (gdouble) max_progress);
        }
    }

  for (row = 0; row < height; row++)
    {
      memset (val_p1, 0, width * sizeof (gdouble));
      memset (val_p2, 0, width * sizeof (gdouble));
      memset (val_m1, 0, width * sizeof (gdouble));
      memset (val_m2, 0, width * sizeof (gdouble));

      src1 = dest1 + row * width;
      src2 = dest2 + row * width;

      sp_p1 = src1;
      sp_p2 = src2;
      sp_m1 = src1 + width - 1;
      sp_m2 = src2 + width - 1;
      vp1   = val_p1;
      vp2   = val_p2;
      vm1   = val_m1 + width - 1;
      vm2   = val_m2 + width - 1;

      /*  Set up the first vals  */
      initial_p1[0] = sp_p1[0];
      initial_p2[0] = sp_p2[0];
      initial_m1[0] = sp_m1[0];
      initial_m2[0] = sp_m2[0];

      for (col = 0; col < width; col++)
        {
          gdouble *vpptr1, *vmptr1;
          gdouble *vpptr2, *vmptr2;

          terms = (col < 4) ? col : 4;

          vpptr1 = vp1; vmptr1 = vm1;
          vpptr2 = vp2; vmptr2 = vm2;

          for (i = 0; i <= terms; i++)
            {
              *vpptr1 += n_p1[i] * sp_p1[-i] - d_p1[i] * vp1[-i];
              *vmptr1 += n_m1[i] * sp_m1[i] - d_m1[i] * vm1[i];

              *vpptr2 += n_p2[i] * sp_p2[-i] - d_p2[i] * vp2[-i];
              *vmptr2 += n_m2[i] * sp_m2[i] - d_m2[i] * vm2[i];
            }

          for (j = i; j <= 4; j++)
            {
              *vpptr1 += (n_p1[j] - bd_p1[j]) * initial_p1[0];
              *vmptr1 += (n_m1[j] - bd_m1[j]) * initial_m1[0];

              *vpptr2 += (n_p2[j] - bd_p2[j]) * initial_p2[0];
              *vmptr2 += (n_m2[j] - bd_m2[j]) * initial_m2[0];
            }

          sp_p1 ++;
          sp_p2 ++;
          sp_m1 --;
          sp_m2 --;
          vp1 ++;
          vp2 ++;
          vm1 --;
          vm2 --;
        }

      transfer_pixels (val_p1, val_m1, dest1 + row * width, 1, 1, width);
      transfer_pixels (val_p2, val_m2, dest2 + row * width, 1, 1, width);

      if (!preview)
        {
          progress += width;
          if ((row % 5) == 0)
            gimp_progress_update ((gdouble) progress / (gdouble) max_progress);
        }
    }

  /* Compute the ramp value which sets 'pct_black' % of the darkened pixels black */
  ramp = compute_ramp (dest1, dest2, width * height, cvals.pct_black);

  /* Initialize the pixel regions. */
  gimp_pixel_rgn_init (&src_rgn, drawable, x1, y1, width, height, FALSE, FALSE);

  if (preview)
    {
      pr = gimp_pixel_rgns_register (1, &src_rgn);
      preview_buffer = g_new (guchar, width * height * bytes);
    }
  else
    {
      gimp_pixel_rgn_init (&dest_rgn, drawable,
                           x1, y1, width, height, TRUE, TRUE);
      pr = gimp_pixel_rgns_register (2, &src_rgn, &dest_rgn);
    }

  while (pr)
    {
      guchar  *src_ptr  = src_rgn.data;
      guchar  *dest_ptr;
      guchar  *blur_ptr = dest1 + (src_rgn.y - y1) * width + (src_rgn.x - x1);
      guchar  *avg_ptr  = dest2 + (src_rgn.y - y1) * width + (src_rgn.x - x1);
      gdouble  diff;
      gdouble  mult     = 0.0;
      gdouble  lightness;

      if (preview)
        dest_ptr =
          preview_buffer +
          ((src_rgn.y - y1) * width + (src_rgn.x - x1)) * bytes;
      else
        dest_ptr = dest_rgn.data;

      for (row = 0; row < src_rgn.h; row++)
        {
          for (col = 0; col < src_rgn.w; col++)
            {
              if (avg_ptr[col] != 0)
                {
                  diff = (gdouble) blur_ptr[col] / (gdouble) avg_ptr[col];
                  if (diff < cvals.threshold)
                    {
                      if (ramp == 0.0)
                        mult = 0.0;
                      else
                        mult = (ramp - MIN (ramp, (cvals.threshold - diff))) / ramp;
                    }
                  else
                    mult = 1.0;
                }

              lightness = CLAMP (blur_ptr[col] * mult, 0, 255);

              if (bytes < 3)
                {
                  dest_ptr[col * bytes] = (guchar) lightness;
                  if (has_alpha)
                    dest_ptr[col * bytes + 1] = src_ptr[col * src_rgn.bpp + 1];
                }
              else
                {
                  /*  Convert to HLS, set lightness and convert back  */
                  gint r, g, b;

                  r = src_ptr[col * src_rgn.bpp + 0];
                  g = src_ptr[col * src_rgn.bpp + 1];
                  b = src_ptr[col * src_rgn.bpp + 2];

                  gimp_rgb_to_hsl_int (&r, &g, &b);
                  b = lightness;
                  gimp_hsl_to_rgb_int (&r, &g, &b);

                  dest_ptr[col * bytes + 0] = r;
                  dest_ptr[col * bytes + 1] = g;
                  dest_ptr[col * bytes + 2] = b;

                  if (has_alpha)
                    dest_ptr[col * bytes + 3] = src_ptr[col * src_rgn.bpp + 3];
                }
            }

          src_ptr  += src_rgn.rowstride;
          if (preview)
            dest_ptr += width * bytes;
          else
            dest_ptr += dest_rgn.rowstride;
          blur_ptr += width;
          avg_ptr  += width;
        }

      if (!preview)
        {
          progress += src_rgn.w * src_rgn.h;
          gimp_progress_update ((gdouble) progress / (gdouble) max_progress);
        }

      pr = gimp_pixel_rgns_process (pr);
    }

  if (preview)
    {
      //gimp_preview_draw_buffer (preview, preview_buffer, width * bytes);
      g_free (preview_buffer);
    }
  else
    {
      /*  merge the shadow, update the drawable  */
      gimp_drawable_flush (drawable);
      gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
      gimp_drawable_update (drawable->drawable_id, x1, y1, width, height);
    }

  /*  free up buffers  */
  g_free (val_p1);
  g_free (val_p2);
  g_free (val_m1);
  g_free (val_m2);
  g_free (src);
  g_free (dest1);
  g_free (dest2);
}

static gdouble
compute_ramp (guchar  *dest1,
              guchar  *dest2,
              gint     length,
              gdouble  pct_black)
{
  gint    hist[100];
  gdouble diff;
  gint    count;
  gint    i;
  gint    sum;

  memset (hist, 0, sizeof (int) * 100);
  count = 0;

  for (i = 0; i < length; i++)
    {
      if (*dest2 != 0)
        {
          diff = (gdouble) *dest1 / (gdouble) *dest2;
          if (diff < 1.0)
            {
              hist[(int) (diff * 100)] += 1;
              count += 1;
            }
        }

      dest1++;
      dest2++;
    }

  if (pct_black == 0.0 || count == 0)
    return 1.0;

  sum = 0;
  for (i = 0; i < 100; i++)
    {
      sum += hist[i];
      if (((gdouble) sum / (gdouble) count) > pct_black)
        return (1.0 - (gdouble) i / 100.0);
    }

  return 0.0;
}


/*
 *  Gaussian blur helper functions
 */

static void
transfer_pixels (gdouble *src1,
                 gdouble *src2,
                 guchar  *dest,
                 gint     jump,
                 gint     bytes,
                 gint     width)
{
  gint    i, b;
  gdouble sum[4];

  for(i = 0; i < width; i++)
    {
      for (b = 0; b < bytes; b++)
        {
          sum[b] = src1[b] + src2[b];
          if (sum[b] > 255) sum[b] = 255;
          else if(sum[b] < 0) sum[b] = 0;
        }

      /*  Convert to lightness if RGB  */
      if (bytes > 2)
        *dest = (guchar) gimp_rgb_to_l_int (sum[0], sum[1], sum[2]);
      else
        *dest = (guchar) sum[0];

      src1 += bytes;
      src2 += bytes;
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

  div = sqrt (2 * G_PI) * std_dev;

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

void operation_cartoon (void * in_buf, void * out_buf, long width, long height, cartoon_config * config)
{
    GimpDrawable * drawable = gimp_drawable_get (0, in_buf, out_buf, width, height);

    cvals = *config;
 
    cartoon(drawable, NULL);

    gimp_drawable_detach (drawable);
}
