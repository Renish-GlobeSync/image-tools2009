
#include "config.h"

#include <libgimp/gimp.h>

#include "whirl-pinch.h"

/***** Magic numbers *****/

#define SCALE_WIDTH  200


/***** Prototypes *****/


static void      whirl_pinch                (GimpDrawable *drawable);
static int       calc_undistorted_coords    (double        wx,
                                             double        wy,
                                             double        whirl,
                                             double        pinch,
                                             double       *x,
                                             double       *y);



/***** Variables *****/

static whirl_pinch_config wpvals =
{
  90.0, /* whirl   */
  0.0,  /* pinch   */
  1.0   /* radius  */
};

static gint img_bpp, img_has_alpha;
static gint sel_x1, sel_y1, sel_x2, sel_y2;
static gint sel_width, sel_height;

static double cen_x, cen_y;
static double scale_x, scale_y;
static double radius, radius2;

/***** Functions *****/
static void
whirl_pinch (GimpDrawable *drawable)
{
  GimpPixelRgn      dest_rgn;
  gint              progress, max_progress;
  guchar           *top_row, *bot_row;
  guchar           *top_p, *bot_p;
  gint              row, col;
  guchar          **pixel;
  gdouble           whirl;
  gdouble           cx, cy;
  gint              ix, iy;
  gint              i;
  GimpPixelFetcher *pft, *pfb;
  GimpRGB           background;

  /* Initialize rows */
  top_row = g_new (guchar, img_bpp * sel_width);
  bot_row = g_new (guchar, img_bpp * sel_width);
  pixel = g_new (guchar *, 4);
  for (i = 0; i < 4; i++)
    pixel[i] = g_new (guchar, 4);

  /* Initialize pixel region */
  gimp_pixel_rgn_init (&dest_rgn, drawable,
                       sel_x1, sel_y1, sel_width, sel_height, TRUE, TRUE);

  pft = gimp_pixel_fetcher_new (drawable, FALSE);
  pfb = gimp_pixel_fetcher_new (drawable, FALSE);

  gimp_context_get_background (&background);
  gimp_pixel_fetcher_set_bg_color (pft, &background);
  gimp_pixel_fetcher_set_bg_color (pfb, &background);

  if (gimp_drawable_has_alpha (drawable->drawable_id))
    {
      gimp_pixel_fetcher_set_edge_mode (pft, GIMP_PIXEL_FETCHER_EDGE_BLACK);
      gimp_pixel_fetcher_set_edge_mode (pfb, GIMP_PIXEL_FETCHER_EDGE_BLACK);
    }
  else
    {
      gimp_pixel_fetcher_set_edge_mode (pft, GIMP_PIXEL_FETCHER_EDGE_BACKGROUND);
      gimp_pixel_fetcher_set_edge_mode (pfb, GIMP_PIXEL_FETCHER_EDGE_BACKGROUND);
    }

  progress     = 0;
  max_progress = sel_width * sel_height;

  gimp_progress_init (_("Whirling and pinching"));

  whirl   = wpvals.whirl * G_PI / 180;
  radius2 = radius * radius * wpvals.radius;

  for (row = sel_y1; row <= ((sel_y1 + sel_y2) / 2); row++)
    {
      top_p = top_row;
      bot_p = bot_row + img_bpp * (sel_width - 1);

      for (col = sel_x1; col < sel_x2; col++)
        {
          if (calc_undistorted_coords (col, row, whirl, wpvals.pinch, &cx, &cy))
            {
              /* We are inside the distortion area */

              /* Top */

              if (cx >= 0.0)
                ix = (int) cx;
              else
                ix = -((int) -cx + 1);

              if (cy >= 0.0)
                iy = (int) cy;
              else
                iy = -((int) -cy + 1);

              gimp_pixel_fetcher_get_pixel (pft, ix,     iy,     pixel[0]);
              gimp_pixel_fetcher_get_pixel (pft, ix + 1, iy,     pixel[1]);
              gimp_pixel_fetcher_get_pixel (pft, ix,     iy + 1, pixel[2]);
              gimp_pixel_fetcher_get_pixel (pft, ix + 1, iy + 1, pixel[3]);

              gimp_bilinear_pixels_8 (top_p, cx, cy, img_bpp, img_has_alpha,
                                      pixel);
              top_p += img_bpp;
              /* Bottom */

              cx = cen_x + (cen_x - cx);
              cy = cen_y + (cen_y - cy);

              if (cx >= 0.0)
                ix = (int) cx;
              else
                ix = -((int) -cx + 1);

              if (cy >= 0.0)
                iy = (int) cy;
              else
                iy = -((int) -cy + 1);

              gimp_pixel_fetcher_get_pixel (pfb, ix,     iy,     pixel[0]);
              gimp_pixel_fetcher_get_pixel (pfb, ix + 1, iy,     pixel[1]);
              gimp_pixel_fetcher_get_pixel (pfb, ix,     iy + 1, pixel[2]);
              gimp_pixel_fetcher_get_pixel (pfb, ix + 1, iy + 1, pixel[3]);

              gimp_bilinear_pixels_8 (bot_p, cx, cy, img_bpp, img_has_alpha,
                                      pixel);
              bot_p -= img_bpp;
            }
          else
            {
              /*  We are outside the distortion area;
               *  just copy the source pixels
               */

              /* Top */

              gimp_pixel_fetcher_get_pixel (pft, col, row, pixel[0]);

              for (i = 0; i < img_bpp; i++)
                *top_p++ = pixel[0][i];

              /* Bottom */

              gimp_pixel_fetcher_get_pixel (pfb,
                                       (sel_x2 - 1) - (col - sel_x1),
                                       (sel_y2 - 1) - (row - sel_y1),
                                       pixel[0]);

              for (i = 0; i < img_bpp; i++)
                *bot_p++ = pixel[0][i];

              bot_p -= 2 * img_bpp; /* We move backwards! */
            }
        }

      /* Paint rows to image */

      gimp_pixel_rgn_set_row (&dest_rgn, top_row, sel_x1, row, sel_width);
      gimp_pixel_rgn_set_row (&dest_rgn, bot_row,
                              sel_x1, (sel_y2 - 1) - (row - sel_y1), sel_width);

      /* Update progress */

      progress += sel_width * 2;
      gimp_progress_update ((double) progress / max_progress);
    }

  gimp_pixel_fetcher_destroy (pft);
  gimp_pixel_fetcher_destroy (pfb);

  for (i = 0; i < 4; i++)
    g_free (pixel[i]);
  g_free (pixel);
  g_free (top_row);
  g_free (bot_row);

  gimp_drawable_flush (drawable);
  gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
  gimp_drawable_update (drawable->drawable_id,
                        sel_x1, sel_y1, sel_width, sel_height);
}

static gint
calc_undistorted_coords (gdouble  wx,
                         gdouble  wy,
                         gdouble  whirl,
                         gdouble  pinch,
                         gdouble *x,
                         gdouble *y)
{
  gdouble dx, dy;
  gdouble d, factor;
  gdouble dist;
  gdouble ang, sina, cosa;
  gint    inside;

  /* Distances to center, scaled */

  dx = (wx - cen_x) * scale_x;
  dy = (wy - cen_y) * scale_y;

  /* Distance^2 to center of *circle* (scaled ellipse) */

  d = dx * dx + dy * dy;

  /*  If we are inside circle, then distort.
   *  Else, just return the same position
   */

  inside = (d < radius2);

  if (inside)
    {
      dist = sqrt(d / wpvals.radius) / radius;

      /* Pinch */

      factor = pow (sin (G_PI_2 * dist), -pinch);

      dx *= factor;
      dy *= factor;

      /* Whirl */

      factor = 1.0 - dist;

      ang = whirl * factor * factor;

      sina = sin (ang);
      cosa = cos (ang);

      *x = (cosa * dx - sina * dy) / scale_x + cen_x;
      *y = (sina * dx + cosa * dy) / scale_y + cen_y;
    }
  else
    {
      *x = wx;
      *y = wy;
    }

  return inside;
}

void operation_whirl_pinch (void * in_buf, void * out_buf, long width, long height, whirl_pinch_config * config)
{
    double             xhsiz, yhsiz;
    GimpDrawable * drawable = gimp_drawable_get (0, in_buf, out_buf, width, height);

    wpvals = *config;

    img_bpp       = gimp_drawable_bpp (drawable->drawable_id);
    img_has_alpha = gimp_drawable_has_alpha (drawable->drawable_id);

    if (! gimp_drawable_mask_intersect (drawable->drawable_id,
        &sel_x1, &sel_y1,
        &sel_width, &sel_height))
    {
        return;
    }

    /* Set the tile cache size */
    gimp_tile_cache_ntiles (2 * drawable->ntile_cols);

    /* Calculate scaling parameters */

    sel_x2 = sel_x1 + sel_width;
    sel_y2 = sel_y1 + sel_height;

    cen_x = (double) (sel_x1 + sel_x2 - 1) / 2.0;
    cen_y = (double) (sel_y1 + sel_y2 - 1) / 2.0;

    xhsiz = (double) (sel_width - 1) / 2.0;
    yhsiz = (double) (sel_height - 1) / 2.0;

    if (xhsiz < yhsiz)
    {
        scale_x = yhsiz / xhsiz;
        scale_y = 1.0;
    }
    else if (xhsiz > yhsiz)
    {
        scale_x = 1.0;
        scale_y = xhsiz / yhsiz;
    }
    else
    {
        scale_x = 1.0;
        scale_y = 1.0;
    }

    radius = MAX(xhsiz, yhsiz);
    whirl_pinch (drawable);

    gimp_drawable_detach (drawable);
}
