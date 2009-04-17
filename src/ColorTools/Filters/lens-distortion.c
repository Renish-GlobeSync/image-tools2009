
#include "config.h"

#include <string.h>

#include <libgimp/gimp.h>

#include "lens-distortion.h"

#define RESPONSE_RESET   1

#define LENS_MAX_PIXEL_DEPTH        4


typedef struct
{
  gdouble  normallise_radius_sq;
  gdouble  centre_x;
  gdouble  centre_y;
  gdouble  mult_sq;
  gdouble  mult_qd;
  gdouble  rescale;
  gdouble  brighten;
} LensCalcValues;


static void     lens_distort    (GimpDrawable *drawable);

static lens_distortion_config vals = { 0.0, 0.0, 0.0, 0.0 };
static LensCalcValues     calc_vals;

static gint               drawable_width, drawable_height;
static guchar             background_color[4];

static void
lens_get_source_coords (gdouble  i,
                        gdouble  j,
                        gdouble *x,
                        gdouble *y,
                        gdouble *mag)
{
  gdouble radius_sq;

  gdouble off_x;
  gdouble off_y;

  gdouble radius_mult;

  off_x = i - calc_vals.centre_x;
  off_y = j - calc_vals.centre_y;
  radius_sq = (off_x * off_x) + (off_y * off_y);

  radius_sq *= calc_vals.normallise_radius_sq;

  radius_mult = radius_sq * calc_vals.mult_sq + radius_sq * radius_sq *
    calc_vals.mult_qd;
  *mag = radius_mult;
  radius_mult = calc_vals.rescale * (1.0 + radius_mult);

  *x = calc_vals.centre_x + radius_mult * off_x;
  *y = calc_vals.centre_y + radius_mult * off_y;
}

static void
lens_setup_calc (gint width, gint height)
{
  calc_vals.normallise_radius_sq =
    4.0 / (width * width + height * height);

  calc_vals.centre_x = width * (100.0 + vals.centre_x) / 200.0;
  calc_vals.centre_y = height * (100.0 + vals.centre_y) / 200.0;
  calc_vals.mult_sq = vals.square_a / 200.0;
  calc_vals.mult_qd = vals.quad_a / 200.0;
  calc_vals.rescale = pow(2.0, - vals.scale_a / 100.0);
  calc_vals.brighten = - vals.brighten / 10.0;
}

/*
 * Catmull-Rom cubic interpolation
 *
 * equally spaced points p0, p1, p2, p3
 * interpolate 0 <= u < 1 between p1 and p2
 *
 * (1 u u^2 u^3) (  0.0  1.0  0.0  0.0 ) (p0)
 *               ( -0.5  0.0  0.5  0.0 ) (p1)
 *               (  1.0 -2.5  2.0 -0.5 ) (p2)
 *               ( -0.5  1.5 -1.5  0.5 ) (p3)
 *
 */

static void
lens_cubic_interpolate (const guchar *src,
                        gint          row_stride,
                        gint          src_depth,
                        guchar       *dst,
                        gint          dst_depth,
                        gdouble       dx,
                        gdouble       dy,
                        gdouble       brighten)
{
  gfloat um1, u, up1, up2;
  gfloat vm1, v, vp1, vp2;
  gint   c;
  gfloat verts[4 * LENS_MAX_PIXEL_DEPTH];

  um1 = ((-0.5 * dx + 1.0) * dx - 0.5) * dx;
  u = (1.5 * dx - 2.5) * dx * dx + 1.0;
  up1 = ((-1.5 * dx + 2.0) * dx + 0.5) * dx;
  up2 = (0.5 * dx - 0.5) * dx * dx;

  vm1 = ((-0.5 * dy + 1.0) * dy - 0.5) * dy;
  v = (1.5 * dy - 2.5) * dy * dy + 1.0;
  vp1 = ((-1.5 * dy + 2.0) * dy + 0.5) * dy;
  vp2 = (0.5 * dy - 0.5) * dy * dy;

  /* Note: if dst_depth < src_depth, we calculate unneeded pixels here */
  /* later - select or create index array */
  for (c = 0; c < 4 * src_depth; ++c)
    {
      verts[c] = vm1 * src[c] + v * src[c+row_stride] +
        vp1 * src[c+row_stride*2] + vp2 * src[c+row_stride*3];
    }

  for (c = 0; c < dst_depth; ++c)
    {
      gfloat result;

      result = um1 * verts[c] + u * verts[c+src_depth] +
        up1 * verts[c+src_depth*2] + up2 * verts[c+src_depth*3];

      result *= brighten;

      dst[c] = CLAMP (result, 0, 255);
    }
}

static void
lens_distort_func (gint              ix,
                   gint              iy,
                   guchar           *dest,
                   gint              bpp,
                   GimpPixelFetcher *pft)
{
  gdouble  src_x, src_y, mag;
  gdouble  brighten;
  guchar   pixel_buffer[16 * LENS_MAX_PIXEL_DEPTH];
  guchar  *pixel;
  gdouble  dx, dy;
  gint     x_int, y_int;
  gint     x, y;

  lens_get_source_coords (ix, iy, &src_x, &src_y, &mag);

  brighten = 1.0 + mag * calc_vals.brighten;
  x_int = floor (src_x);
  dx = src_x - x_int;

  y_int = floor (src_y);
  dy = src_y - y_int;

  pixel = pixel_buffer;
  for (y = y_int - 1; y <= y_int + 2; y++)
    {
      for (x = x_int -1; x <= x_int + 2; x++)
        {
          if (x >= 0  && y >= 0 &&
              x < drawable_width &&  y < drawable_height)
            {
              gimp_pixel_fetcher_get_pixel (pft, x, y, pixel);
            }
          else
            {
              gint i;

              for (i = 0; i < bpp; i++)
                pixel[i] = background_color[i];
            }

          pixel += bpp;
        }
    }

  lens_cubic_interpolate (pixel_buffer, bpp * 4, bpp,
                          dest, bpp, dx, dy, brighten);
}

static void
lens_distort (GimpDrawable *drawable)
{
  GimpRgnIterator  *iter;
  GimpPixelFetcher *pft;
  GimpRGB           background;

  lens_setup_calc (drawable->width, drawable->height);

  pft = gimp_pixel_fetcher_new (drawable, FALSE);

  gimp_context_get_background (&background);
  gimp_rgb_set_alpha (&background, 0.0);
  gimp_pixel_fetcher_set_bg_color (pft, &background);
  gimp_pixel_fetcher_set_edge_mode (pft, GIMP_PIXEL_FETCHER_EDGE_BACKGROUND);

  gimp_progress_init (_("Lens distortion"));

  iter = gimp_rgn_iterator_new (drawable, 0);
  gimp_rgn_iterator_dest (iter, (GimpRgnFuncDest) lens_distort_func, pft);
  gimp_rgn_iterator_free (iter);

  gimp_pixel_fetcher_destroy (pft);
}

void operation_lens_distortion (void * in_buf, void * out_buf, long width, long height, lens_distortion_config * config)
{
    GimpRGB            background;
    GimpDrawable * drawable = gimp_drawable_get (0, in_buf, out_buf, width, height);

    vals = *config;
 
    drawable_width = drawable->width;
    drawable_height = drawable->height;

    /* Get background color */
    gimp_context_get_background (&background);
    gimp_rgb_set_alpha (&background, 0.0);
    gimp_drawable_get_color_uchar (drawable->drawable_id, &background,
        background_color);

    /* Set the tile cache size */
    gimp_tile_cache_ntiles (2 * MAX (drawable->ntile_rows, drawable->ntile_cols));

    lens_distort (drawable);

    gimp_drawable_detach (drawable);
}
