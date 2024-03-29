/* LIBGIMP - The GIMP Library
 * Copyright (C) 1995-1997 Peter Mattis and Spencer Kimball
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <glib-object.h>

#include "libgimpmath/gimpmath.h"

#include "libgimpcolor/gimpcolortypes.h"

#include "libgimpcolor/gimpbilinear.h"


gdouble
gimp_bilinear (gdouble  x,
               gdouble  y,
               gdouble *values)
{
  gdouble m0, m1;

  g_return_val_if_fail (values != NULL, 0.0);

  x = fmod (x, 1.0);
  y = fmod (y, 1.0);

  if (x < 0.0)
    x += 1.0;
  if (y < 0.0)
    y += 1.0;

  m0 = (1.0 - x) * values[0] + x * values[1];
  m1 = (1.0 - x) * values[2] + x * values[3];

  return (1.0 - y) * m0 + y * m1;
}

guchar
gimp_bilinear_8 (gdouble x,
                 gdouble y,
                 guchar *values)
{
  gdouble m0, m1;

  g_return_val_if_fail (values != NULL, 0);

  x = fmod (x, 1.0);
  y = fmod (y, 1.0);

  if (x < 0.0)
    x += 1.0;
  if (y < 0.0)
    y += 1.0;

  m0 = (1.0 - x) * values[0] + x * values[1];
  m1 = (1.0 - x) * values[2] + x * values[3];

  return (guchar) ((1.0 - y) * m0 + y * m1);
}

guint16
gimp_bilinear_16 (gdouble  x,
                  gdouble  y,
                  guint16 *values)
{
  gdouble m0, m1;

  g_return_val_if_fail (values != NULL, 0);

  x = fmod (x, 1.0);
  y = fmod (y, 1.0);

  if (x < 0.0)
    x += 1.0;
  if (y < 0.0)
    y += 1.0;

  m0 = (1.0 - x) * values[0] + x * values[1];
  m1 = (1.0 - x) * values[2] + x * values[3];

  return (guint16) ((1.0 - y) * m0 + y * m1);
}

guint32
gimp_bilinear_32 (gdouble  x,
                  gdouble  y,
                  guint32 *values)
{
  gdouble m0, m1;

  g_return_val_if_fail (values != NULL, 0);

  x = fmod (x, 1.0);
  y = fmod (y, 1.0);

  if (x < 0.0)
    x += 1.0;
  if (y < 0.0)
    y += 1.0;

  m0 = (1.0 - x) * values[0] + x * values[1];
  m1 = (1.0 - x) * values[2] + x * values[3];

  return (guint32) ((1.0 - y) * m0 + y * m1);
}

GimpRGB
gimp_bilinear_rgb (gdouble  x,
                   gdouble  y,
                   GimpRGB *values)
{
  gdouble m0, m1;
  gdouble ix, iy;
  GimpRGB v = { 0, };

  g_return_val_if_fail (values != NULL, v);

  x = fmod(x, 1.0);
  y = fmod(y, 1.0);

  if (x < 0)
    x += 1.0;
  if (y < 0)
    y += 1.0;

  ix = 1.0 - x;
  iy = 1.0 - y;

  /* Red */

  m0 = ix * values[0].r + x * values[1].r;
  m1 = ix * values[2].r + x * values[3].r;

  v.r = iy * m0 + y * m1;

  /* Green */

  m0 = ix * values[0].g + x * values[1].g;
  m1 = ix * values[2].g + x * values[3].g;

  v.g = iy * m0 + y * m1;

  /* Blue */

  m0 = ix * values[0].b + x * values[1].b;
  m1 = ix * values[2].b + x * values[3].b;

  v.b = iy * m0 + y * m1;

  return v;
}

GimpRGB
gimp_bilinear_rgba (gdouble  x,
                    gdouble  y,
                    GimpRGB *values)
{
  gdouble m0, m1;
  gdouble ix, iy;
  gdouble a0, a1, a2, a3, alpha;
  GimpRGB v = { 0, };

  g_return_val_if_fail (values != NULL, v);

  x = fmod (x, 1.0);
  y = fmod (y, 1.0);

  if (x < 0)
    x += 1.0;
  if (y < 0)
    y += 1.0;

  ix = 1.0 - x;
  iy = 1.0 - y;

  a0 = values[0].a;
  a1 = values[1].a;
  a2 = values[2].a;
  a3 = values[3].a;

  /* Alpha */

  m0 = ix * a0 + x * a1;
  m1 = ix * a2 + x * a3;

  alpha = v.a = iy * m0 + y * m1;

  if (alpha > 0)
    {
      /* Red */

      m0 = ix * a0 * values[0].r + x * a1 * values[1].r;
      m1 = ix * a2 * values[2].r + x * a3 * values[3].r;

      v.r = (iy * m0 + y * m1)/alpha;

      /* Green */

      m0 = ix * a0 * values[0].g + x * a1 * values[1].g;
      m1 = ix * a2 * values[2].g + x * a3 * values[3].g;

      v.g = (iy * m0 + y * m1)/alpha;

      /* Blue */

      m0 = ix * a0 * values[0].b + x * a1 * values[1].b;
      m1 = ix * a2 * values[2].b + x * a3 * values[3].b;

      v.b = (iy * m0 + y * m1)/alpha;
    }

  return v;
}

/**
 * gimp_bilinear_pixels_8:
 * @dest: Pixel, where interpolation result is to be stored.
 * @x: x-coordinate (0.0 to 1.0).
 * @y: y-coordinate (0.0 to 1.0).
 * @bpp: Bytes per pixel.  @dest and each @values item is an array of
 *    @bpp bytes.
 * @has_alpha: %TRUE if the last channel is an alpha channel.
 * @values: Array of four pointers to pixels.
 *
 * Computes bilinear interpolation of four pixels.
 *
 * When @has_alpha is %FALSE, it's identical to gimp_bilinear_8() on
 * each channel separately.  When @has_alpha is %TRUE, it handles
 * alpha channel correctly.
 *
 * The pixels in @values correspond to corner x, y coordinates in the
 * following order: [0,0], [1,0], [0,1], [1,1].
 **/
void
gimp_bilinear_pixels_8 (guchar    *dest,
                        gdouble    x,
                        gdouble    y,
                        guint      bpp,
                        gboolean   has_alpha,
                        guchar   **values)
{
  guint i;

  g_return_if_fail (dest != NULL);
  g_return_if_fail (values != NULL);

  x = fmod (x, 1.0);
  y = fmod (y, 1.0);

  if (x < 0.0)
    x += 1.0;
  if (y < 0.0)
    y += 1.0;

  if (has_alpha)
    {
      guint   ai     = bpp - 1;
      gdouble alpha0 = values[0][ai];
      gdouble alpha1 = values[1][ai];
      gdouble alpha2 = values[2][ai];
      gdouble alpha3 = values[3][ai];
      gdouble alpha  = ((1.0 - y) * ((1.0 - x) * alpha0 + x * alpha1)
                        + y * ((1.0 - x) * alpha2 + x * alpha3));

      dest[ai] = (guchar) alpha;
      if (dest[ai])
        {
          for (i = 0; i < ai; i++)
            {
              gdouble m0 = ((1.0 - x) * values[0][i] * alpha0
                            + x * values[1][i] * alpha1);
              gdouble m1 = ((1.0 - x) * values[2][i] * alpha2
                            + x * values[3][i] * alpha3);

              dest[i] = (guchar) (((1.0 - y) * m0 + y * m1) / alpha);
            }
        }
    }
  else
    {
      for (i = 0; i < bpp; i++)
        {
          gdouble m0 = (1.0 - x) * values[0][i] + x * values[1][i];
          gdouble m1 = (1.0 - x) * values[2][i] + x * values[3][i];

          dest[i] = (guchar) ((1.0 - y) * m0 + y * m1);
        }
    }
}
