
#include "config.h"

#include <libgimp/gimp.h>

#include "pagecurl.h"

#include "pagecurl-icons.h"


#define NGRADSAMPLES    256


/***** Prototypes *****/

static void       set_default_params   (void);

static void       init_calculation     (gint32            drawable_id);
static gint32     do_curl_effect       (gint32            drawable_id);
static void       clear_curled_region  (gint32            drawable_id);
static gint32     page_curl            (gint32            drawable_id);
static guchar   * get_gradient_samples (gint32            drawable_id,
                                        gboolean          reverse);


/***** Variables *****/

static pagecurl_config curl;

/* Image parameters */

static gint32        image_id;
static GimpDrawable *curl_layer;

static const guint8 *curl_pixbufs[] =
{
  curl0,
  curl1,
  curl2,
  curl3,
  curl4,
  curl5,
  curl6,
  curl7
};

static gint   sel_x1, sel_y1, sel_x2, sel_y2;
static gint   true_sel_width, true_sel_height;
static gint   sel_width, sel_height;
static gint   drawable_position;

/* Center and radius of circle */

static GimpVector2 center;
static double      radius;

/* Useful points to keep around */

static GimpVector2 left_tangent;
static GimpVector2 right_tangent;

/* Slopes --- these are *not* in the usual geometric sense! */

static gdouble diagl_slope;
static gdouble diagr_slope;
static gdouble diagb_slope;
static gdouble diagm_slope;

/* User-configured parameters */

static guchar fore_color[3];
static guchar back_color[3];


/***** Functions *****/

/*****************************************************
 * Functions to locate the current point in the curl.
 *  The following functions assume an curl in the
 *  lower right corner.
 *  diagb crosses the two tangential points from the
 *  circle with diagl and diagr.
 *
 *   +------------------------------------------------+
 *   |                                          __-- /|
 *   |                                      __--   _/ |
 *   |                                  __--      /   |
 *   |                              __--        _/    |
 *   |                          __--           /      |
 *   |                      __--____         _/       |
 *   |                  __----~~    \      _/         |
 *   |              __--             |   _/           |
 *   |    diagl __--               _/| _/diagr        |
 *   |      __--            diagm_/  |/               |
 *   |  __--                    /___/                 |
 *   +-------------------------+----------------------+
 */

static inline gboolean
left_of_diagl (gdouble x,
	       gdouble y)
{
  return (x < (sel_width + (y - sel_height) * diagl_slope));
}

static inline gboolean
right_of_diagr (gdouble x,
		gdouble y)
{
  return (x > (sel_width + (y - sel_height) * diagr_slope));
}

static inline gboolean
below_diagb (gdouble x,
	     gdouble y)
{
  return (y < (right_tangent.y + (x - right_tangent.x) * diagb_slope));
}

static inline gboolean
right_of_diagm (gdouble x,
		gdouble y)
{
  return (x > (sel_width + (y - sel_height) * diagm_slope));
}

static inline gboolean
inside_circle (gdouble x,
	       gdouble y)
{
  x -= center.x;
  y -= center.y;

  return x * x + y * y <= radius * radius;
}

static void
set_default_params (void)
{
  curl.colors      = CURL_COLORS_FG_BG;
  curl.opacity     = 1.0;
  curl.shade       = TRUE;
  curl.edge        = CURL_EDGE_LOWER_RIGHT;
  curl.orientation = CURL_ORIENTATION_VERTICAL;
}

static void
init_calculation (gint32 drawable_id)
{
  gdouble      k;
  gdouble      alpha, beta;
  gdouble      angle;
  GimpVector2  v1, v2;
  gint32      *image_layers;
  gint32       nlayers;
  GimpRGB      color;

  //gimp_layer_add_alpha (drawable_id);

  /* Image parameters */

  /* Determine Position of original Layer in the Layer stack. */

  //image_layers = gimp_image_get_layers (image_id, &nlayers);
  //drawable_position = 0;
  //while (drawable_position < nlayers &&
	 //image_layers[drawable_position] != drawable_id)
  //  drawable_position++;

  /* Get the bounds of the active selection */
  gimp_drawable_mask_bounds (drawable_id,
			     &sel_x1, &sel_y1, &sel_x2, &sel_y2);

  true_sel_width = sel_x2 - sel_x1;
  true_sel_height = sel_y2 - sel_y1;

  switch (curl.orientation)
    {
    case CURL_ORIENTATION_VERTICAL:
      sel_width  = true_sel_width;
      sel_height = true_sel_height;
      break;

    case CURL_ORIENTATION_HORIZONTAL:
      sel_width  = true_sel_height;
      sel_height = true_sel_width;
      break;
    }

  /* Circle parameters */

  alpha = atan ((double) sel_height / sel_width);
  beta = alpha / 2.0;
  k = sel_width / ((G_PI + alpha) * sin (beta) + cos (beta));
  gimp_vector2_set (&center, k * cos (beta), k * sin (beta));
  radius = center.y;

  /* left_tangent  */

  gimp_vector2_set (&left_tangent, radius * -sin (alpha), radius * cos (alpha));
  gimp_vector2_add (&left_tangent, &left_tangent, &center);

  /* right_tangent */

  gimp_vector2_sub (&v1, &left_tangent, &center);
  gimp_vector2_set (&v2, sel_width - center.x, sel_height - center.y);
  angle = -2.0 * acos (gimp_vector2_inner_product (&v1, &v2) /
		       (gimp_vector2_length (&v1) *
                        gimp_vector2_length (&v2)));
  gimp_vector2_set (&right_tangent,
		    v1.x * cos (angle) + v1.y * -sin (angle),
		    v1.x * sin (angle) + v1.y * cos (angle));
  gimp_vector2_add (&right_tangent, &right_tangent, &center);

  /* Slopes */

  diagl_slope = (double) sel_width / sel_height;
  diagr_slope = (sel_width - right_tangent.x) / (sel_height - right_tangent.y);
  diagb_slope = ((right_tangent.y - left_tangent.y) /
                 (right_tangent.x - left_tangent.x));
  diagm_slope = (sel_width - center.x) / sel_height;

  /* Colors */

  gimp_context_get_foreground (&color);
  gimp_rgb_get_uchar (&color, &fore_color[0], &fore_color[1], &fore_color[2]);

  gimp_context_get_background (&color);
  gimp_rgb_get_uchar (&color, &back_color[0], &back_color[1], &back_color[2]);
}

static gint32
do_curl_effect (gint32 drawable_id)
{
  gint          x = 0;
  gint          y = 0;
  gboolean      color_image;
  gint          x1, y1, k;
  guint         alpha_pos, progress, max_progress;
  gdouble       intensity, alpha, beta;
  GimpVector2   v, dl, dr;
  gdouble       dl_mag, dr_mag, angle, factor;
  guchar       *pp, *dest, *sp, * src, fore_grayval, back_grayval;
  guchar       *gradsamp;
  GimpPixelRgn  dest_rgn, src_rgn;
  gpointer      pr;
  gint32        curl_layer_id;
  guchar       *grad_samples  = NULL;

  color_image = gimp_drawable_is_rgb (drawable_id);

  curl_layer = gimp_drawable_get_1(drawable_id);
    //gimp_drawable_get (gimp_layer_new (image_id,
				//       _("Curl Layer"),
				//       true_sel_width,
				//       true_sel_height,
				//       color_image ?
    //                                   GIMP_RGBA_IMAGE : GIMP_GRAYA_IMAGE,
				//       100, GIMP_NORMAL_MODE));

  curl_layer_id = curl_layer->drawable_id;

  //gimp_image_add_layer (image_id, curl_layer->drawable_id, drawable_position);
  //gimp_drawable_fill (curl_layer->drawable_id, GIMP_TRANSPARENT_FILL);

  //gimp_drawable_offsets (drawable_id, &x1, &y1);
  //gimp_layer_set_offsets (curl_layer->drawable_id, sel_x1 + x1, sel_y1 + y1);
  //gimp_tile_cache_ntiles (2 * (curl_layer->width / gimp_tile_width () + 1));

  gimp_pixel_rgn_init (&src_rgn, curl_layer,
		       sel_x1, sel_y1, true_sel_width, true_sel_height,
		       FALSE, FALSE);
  gimp_pixel_rgn_init (&dest_rgn, curl_layer,
		       0, 0, true_sel_width, true_sel_height, TRUE, TRUE);

  /* Init shade_under */
  gimp_vector2_set (&dl, -sel_width, -sel_height);
  dl_mag = gimp_vector2_length (&dl);
  gimp_vector2_set (&dr,
		    -(sel_width - right_tangent.x),
		    -(sel_height - right_tangent.y));
  dr_mag = gimp_vector2_length (&dr);
  alpha = acos (gimp_vector2_inner_product (&dl, &dr) / (dl_mag * dr_mag));
  beta = alpha / 2;

  /* Init shade_curl */

  fore_grayval = GIMP_RGB_LUMINANCE (fore_color[0],
                                     fore_color[1],
                                     fore_color[2]) + 0.5;
  back_grayval = GIMP_RGB_LUMINANCE (back_color[0],
                                     back_color[1],
                                     back_color[2]) + 0.5;

  /* Gradient Samples */
  switch (curl.colors)
    {
    case CURL_COLORS_FG_BG:
      break;
    case CURL_COLORS_GRADIENT:
      grad_samples = get_gradient_samples (curl_layer->drawable_id, FALSE);
      break;
    case CURL_COLORS_GRADIENT_REVERSE:
      grad_samples = get_gradient_samples (curl_layer->drawable_id, TRUE);
      break;
    }

  max_progress = 2 * sel_width * sel_height;
  progress = 0;

  alpha_pos = dest_rgn.bpp - 1;

  /* Main loop */
  for (pr = gimp_pixel_rgns_register (2, &dest_rgn, &src_rgn);
       pr != NULL;
       pr = gimp_pixel_rgns_process (pr))
    {
        src = src_rgn.data;
      dest = dest_rgn.data;

      for (y1 = dest_rgn.y; y1 < dest_rgn.y + dest_rgn.h; y1++)
	{
	  pp = dest;
      sp = src;
	  for (x1 = dest_rgn.x; x1 < dest_rgn.x + dest_rgn.w; x1++)
	    {
	      /* Map coordinates to get the curl correct... */
              switch (curl.orientation)
                {
                case CURL_ORIENTATION_VERTICAL:
		  x = CURL_EDGE_RIGHT (curl.edge) ? x1 : sel_width  - 1 - x1;
		  y = CURL_EDGE_UPPER (curl.edge) ? y1 : sel_height - 1 - y1;
                  break;

                case CURL_ORIENTATION_HORIZONTAL:
                  x = CURL_EDGE_LOWER (curl.edge) ? y1 : sel_width  - 1 - y1;
		  y = CURL_EDGE_LEFT  (curl.edge) ? x1 : sel_height - 1 - x1;
                  break;
		}

	      if (left_of_diagl (x, y))
		{ /* uncurled region */
		  for (k = 0; k <= alpha_pos; k++)
		    pp[k] = sp[k];
		}
	      else if (right_of_diagr (x, y) ||
		       (right_of_diagm (x, y) &&
			below_diagb (x, y) &&
			!inside_circle (x, y)))
		{
		  /* curled region */
		  for (k = 0; k <= alpha_pos; k++)
		    pp[k] = 0;
		}
	      else
		{
		  v.x = -(sel_width - x);
		  v.y = -(sel_height - y);
		  angle = acos (gimp_vector2_inner_product (&v, &dl) /
				(gimp_vector2_length (&v) * dl_mag));

		  if (inside_circle (x, y) || below_diagb (x, y))
		    {
		      /* Below the curl. */
		      factor = angle / alpha;
		      pp[alpha_pos] = (curl.shade ?
                                       (guchar) ((float) 255 * (float) factor) :
                                       0);
		      for (k = 0; k < alpha_pos; k++)
    			pp[k] = (int)sp[k] * (255 - pp[alpha_pos]) / 255;
		    }
		  else
		    {
		      /* On the curl */
                      switch (curl.colors)
                        {
                        case CURL_COLORS_FG_BG:
			  intensity = pow (sin (G_PI * angle / alpha), 1.5);
			  pp[alpha_pos] = (guchar) ((double) 255.99 *
                                                    (1.0 - intensity *
                                                     (1.0 - curl.opacity)));
			  if (color_image)
			    {
			      pp[0] = ((gdouble)sp[0] * (255.0 - pp[alpha_pos]) + (intensity * back_color[0] +
                                       (1.0 - intensity) * fore_color[0]) * pp[alpha_pos]) / 255.0;
			      pp[1] = ((gdouble)sp[1] * (255.0 - pp[alpha_pos]) + (intensity * back_color[1] +
                                       (1.0 - intensity) * fore_color[1]) * pp[alpha_pos]) / 255.0;
			      pp[2] = ((gdouble)sp[2] * (255.0 - pp[alpha_pos]) + (intensity * back_color[2] +
                                       (1.0 - intensity) * fore_color[2]) * pp[alpha_pos]) / 255.0;
			    }
			  else
			    pp[0] = ((gdouble)sp[0] * (255.0 - pp[alpha_pos]) + (intensity * back_grayval +
                                     (1 - intensity) * fore_grayval) * pp[alpha_pos]) / 255.0;

                          break;

                        case CURL_COLORS_GRADIENT:
                        case CURL_COLORS_GRADIENT_REVERSE:
			  /* Calculate position in Gradient */
                          intensity =
                            (angle/alpha) + sin (G_PI*2 * angle/alpha) * 0.075;

			  /* Check boundaries */
			  intensity = CLAMP (intensity, 0.0, 1.0);
			  gradsamp  = (grad_samples +
                                       ((guint) (intensity * NGRADSAMPLES)) *
                                       dest_rgn.bpp);

			  if (color_image)
			    {
			      pp[0] = gradsamp[0];
			      pp[1] = gradsamp[1];
			      pp[2] = gradsamp[2];
			    }
			  else
			    pp[0] = gradsamp[0];

			  pp[alpha_pos] =
                            (guchar) ((double) gradsamp[alpha_pos] *
                                      (1.0 - intensity * (1.0 - curl.opacity)));
                          break;
                        }
		    }
		}
          sp += src_rgn.bpp;
	      pp += dest_rgn.bpp;
	    }
        src += src_rgn.rowstride;
	  dest += dest_rgn.rowstride;
	}
      progress += dest_rgn.w * dest_rgn.h;
      gimp_progress_update ((double) progress / (double) max_progress);
    }

  gimp_drawable_flush (curl_layer);
  gimp_drawable_merge_shadow (curl_layer->drawable_id, FALSE);
  gimp_drawable_update (curl_layer->drawable_id,
			0, 0, curl_layer->width, curl_layer->height);
  //gimp_drawable_detach (curl_layer);

  g_free (grad_samples);

  return curl_layer_id;
}

/************************************************/

static void
clear_curled_region (gint32 drawable_id)
{
  GimpPixelRgn  src_rgn, dest_rgn;
  gpointer      pr;
  gint          x = 0;
  gint          y = 0;
  guint         x1, y1, i;
  guchar       *dest, *src, *pp, *sp;
  guint         alpha_pos, progress, max_progress;
  GimpDrawable *drawable;

  max_progress = 2 * sel_width * sel_height;
  progress = max_progress / 2;

  drawable = gimp_drawable_get_1 (drawable_id);

  gimp_tile_cache_ntiles (2 * (drawable->width / gimp_tile_width () + 1));
  gimp_pixel_rgn_init (&src_rgn, drawable,
		       sel_x1, sel_y1, true_sel_width, true_sel_height,
		       FALSE, FALSE);
  gimp_pixel_rgn_init (&dest_rgn, drawable,
		       sel_x1, sel_y1, true_sel_width, true_sel_height,
		       TRUE, TRUE);
  alpha_pos = dest_rgn.bpp - 1;

  for (pr = gimp_pixel_rgns_register (2, &dest_rgn, &src_rgn);
       pr != NULL;
       pr = gimp_pixel_rgns_process (pr))
    {
      dest = dest_rgn.data;
      src = src_rgn.data;

      for (y1 = dest_rgn.y; y1 < dest_rgn.y + dest_rgn.h; y1++)
	{
	  sp = src;
	  pp = dest;

	  for (x1 = dest_rgn.x; x1 < dest_rgn.x + dest_rgn.w; x1++)
	    {
	      /* Map coordinates to get the curl correct... */
              switch (curl.orientation)
                {
                case CURL_ORIENTATION_VERTICAL:
		  x = (CURL_EDGE_RIGHT (curl.edge) ?
                       x1 - sel_x1 : sel_width  - 1 - (x1 - sel_x1));
		  y = (CURL_EDGE_UPPER (curl.edge) ?
                       y1 - sel_y1 : sel_height - 1 - (y1 - sel_y1));
                  break;

                case CURL_ORIENTATION_HORIZONTAL:
		  x = (CURL_EDGE_LOWER (curl.edge) ?
                       y1 - sel_y1 : sel_width - 1 - (y1 - sel_y1));
                  y = (CURL_EDGE_LEFT (curl.edge)  ?
                       x1 - sel_x1 : sel_height - 1 - (x1 - sel_x1));
                  break;
                }

	      for (i = 0; i < alpha_pos; i++)
		pp[i] = sp[i];

	      if (right_of_diagr (x, y) ||
		  (right_of_diagm (x, y) &&
		   below_diagb (x, y) &&
		   !inside_circle (x, y)))
		{
		  /* Right of the curl */
		  pp[alpha_pos] = 0;
		}
	      else
		{
		  pp[alpha_pos] = sp[alpha_pos];
		}

	      pp += dest_rgn.bpp;
	      sp += src_rgn.bpp;
	    }

	  src += src_rgn.rowstride;
	  dest += dest_rgn.rowstride;
	}

      progress += dest_rgn.w * dest_rgn.h;
      gimp_progress_update ((double) progress / (double) max_progress);
    }

  gimp_drawable_flush (drawable);
  gimp_drawable_merge_shadow (drawable_id, TRUE);
  gimp_drawable_update (drawable_id,
			sel_x1, sel_y1, true_sel_width, true_sel_height);
  //gimp_drawable_detach (drawable);
}

static gint32
page_curl (gint32 drawable_id)
{
  gint curl_layer_id;

  //gimp_image_undo_group_start (image_id);

  gimp_progress_init (_("Page Curl"));

  init_calculation (drawable_id);

  curl_layer_id = do_curl_effect (drawable_id);

    //clear_curled_region (drawable_id);

  //gimp_image_undo_group_end (image_id);

  return curl_layer_id;
}

/*
  Returns NGRADSAMPLES samples of active gradient.
  Each sample has (gimp_drawable_bpp (drawable_id)) bytes.
  "ripped" from gradmap.c.
 */
static guchar *
get_gradient_samples (gint32    drawable_id,
                      gboolean  reverse)
{
  gchar   *gradient_name;
  gint     n_f_samples;
  gdouble *f_samples, *f_samp;    /* float samples */
  guchar  *b_samples, *b_samp;    /* byte samples */
  gint     bpp, color, has_alpha, alpha;
  gint     i, j;

  //gradient_name = gimp_context_get_gradient ();

  //gimp_gradient_get_uniform_samples (gradient_name, NGRADSAMPLES, reverse,
  //                                   &n_f_samples, &f_samples);

  //g_free (gradient_name);

  bpp       = gimp_drawable_bpp (drawable_id);
  color     = gimp_drawable_is_rgb (drawable_id);
  has_alpha = gimp_drawable_has_alpha (drawable_id);
  alpha     = (has_alpha ? bpp - 1 : bpp);

  b_samples = g_new (guchar, NGRADSAMPLES * bpp);

  for (i = 0; i < NGRADSAMPLES; i++)
    {
      b_samp = &b_samples[i * bpp];
      f_samp = &f_samples[i * 4];

      if (color)
        for (j = 0; j < 3; j++)
          b_samp[j] = f_samp[j] * 255;
      else
        b_samp[0] = GIMP_RGB_LUMINANCE (f_samp[0], f_samp[1], f_samp[2]) * 255;

      if (has_alpha)
        b_samp[alpha] = f_samp[3] * 255;
    }

  g_free (f_samples);

  return b_samples;
}

void operation_pagecurl (void * in_buf, void * out_buf, long width, long height, pagecurl_config * config)
{
    GimpDrawable * drawable = gimp_drawable_get (0, in_buf, out_buf, width, height);

    curl = *config;
 
    page_curl(0);

    gimp_drawable_detach (drawable);
}