
#include "config.h"

#include <string.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "libgimp/stdplugins-intl.h"

#include "nova.h"


#define SCALE_WIDTH    125





static void        nova                          (GimpDrawable     *drawable,
                                                  GimpPreview      *preview);

static nova_config pvals =
{
  128, 128,                 /* xcenter, ycenter */
  { 0.35, 0.39, 1.0, 1.0 }, /* color */
  20,                       /* radius */
  100,                      /* nspoke */
  0                         /* random hue */
};


/*
  ################################################################
  ##                                                            ##
  ##                   Main Calculation                         ##
  ##                                                            ##
  ################################################################
*/

static gdouble
gauss (GRand *gr)
{
  gdouble sum = 0.0;
  gint    i;

  for (i = 0; i < 6; i++)
    sum += g_rand_double (gr);

  return sum / 6.0;
}

static void
nova (GimpDrawable *drawable,
      GimpPreview  *preview)
{
   GimpPixelRgn  src_rgn;
   GimpPixelRgn  dest_rgn;
   gpointer      pr;
   guchar       *src_row, *dest_row, *save_src;
   guchar       *src, *dest;
   gint          x1, y1, x2, y2, x, y;
   gint          row, col;
   gint          alpha, bpp;
   gint          progress, max_progress;
   gboolean      has_alpha;
   gint          xc, yc; /* center of nova */
   gdouble       u, v, l, w, w1, c, t;
   gdouble      *spoke;
   gdouble       nova_alpha, src_alpha, new_alpha = 0.0;
   gdouble       compl_ratio, ratio;
   GimpRGB       color;
   GimpRGB      *spokecolor;
   GimpHSV       hsv;
   gdouble       spokecol;
   gint          i;
   GRand        *gr;
   guchar       *cache = NULL;
   gint          width, height;
   gdouble       zoom = 0.0;

   gr = g_rand_new ();

   /* initialize */
   has_alpha = gimp_drawable_has_alpha (drawable->drawable_id);

   spoke = g_new (gdouble, pvals.nspoke);
   spokecolor = g_new (GimpRGB, pvals.nspoke);

   gimp_rgb_set_alpha (&pvals.color, 1.0);
   gimp_rgb_to_hsv (&pvals.color, &hsv);

   for (i = 0; i < pvals.nspoke; i++)
     {
       spoke[i] = gauss (gr);

       hsv.h += ((gdouble) pvals.randomhue / 360.0) *
                g_rand_double_range (gr, -0.5, 0.5);

       if (hsv.h < 0)
         hsv.h += 1.0;
       else if (hsv.h >= 1.0)
         hsv.h -= 1.0;

       gimp_hsv_to_rgb (&hsv, spokecolor + i);
     }

   if (preview)
     {
       //cache = gimp_zoom_preview_get_source (GIMP_ZOOM_PREVIEW (preview),
       //                                      &width, &height, &bpp);

       //zoom = gimp_zoom_preview_get_factor (GIMP_ZOOM_PREVIEW (preview));

       //gimp_preview_transform (preview,
       //                        pvals.xcenter, pvals.ycenter, &xc, &yc);

       //x1 = 0;
       //y1 = 0;
       //x2 = width;
       //y2 = height;
     }
   else
     {
       gimp_drawable_mask_bounds (drawable->drawable_id, &x1, &y1, &x2, &y2);
       bpp = gimp_drawable_bpp (drawable->drawable_id);
       xc = pvals.xcenter;
       yc = pvals.ycenter;

       gimp_pixel_rgn_init (&src_rgn, drawable,
                            x1, y1, x2-x1, y2-y1, FALSE, FALSE);
       gimp_pixel_rgn_init (&dest_rgn, drawable,
                            x1, y1, x2-x1, y2-y1, TRUE, TRUE);
     }

   alpha = (has_alpha) ? bpp - 1 : bpp;

   /* Initialize progress */
   progress     = 0;
   max_progress = (x2 - x1) * (y2 - y1);

   if (preview)
     {
     //  save_src = src_row  = g_new (guchar, y2 * width * bpp);
     //  memcpy (src_row, cache, y2 * width * bpp);

     //  dest_row = g_new (guchar, y2 * width * bpp);
     //  dest = dest_row;
     //  src  = src_row;

     //  for (row = 0, y = 0; row < y2; row++, y++)
     //    {

     //      for (col = 0, x = 0; col < x2; col++, x++)
     //        {
     //          u = ((gdouble) (x - xc) /
     //               ((gdouble) pvals.radius * width /
     //                drawable->width * zoom));
     //          v = ((gdouble) (y - yc) /
     //               ((gdouble) pvals.radius * height /
     //                drawable->height * zoom));

     //          l = sqrt (SQR (u) + SQR (v));

     //          /* This algorithm is still under construction. */
     //          t = (atan2 (u, v) / (2 * G_PI) + .51) * pvals.nspoke;
     //          i = (gint) floor (t);
     //          t -= i;
     //          i %= pvals.nspoke;
     //          w1 = spoke[i] * (1 - t) + spoke[(i + 1) % pvals.nspoke] * t;
     //          w1 = w1 * w1;

     //          w = 1.0 / (l + 0.001) * 0.9;

     //          nova_alpha = CLAMP (w, 0.0, 1.0);

     //          if (has_alpha)
     //          {
     //            src_alpha = (gdouble) src[alpha] / 255.0;
     //            new_alpha = src_alpha + (1.0 - src_alpha) * nova_alpha;
     //            if (new_alpha != 0.0)
     //              ratio = nova_alpha / new_alpha;
     //            else
     //              ratio = 0.0;
     //          }
     //          else
     //            ratio = nova_alpha;

     //          compl_ratio = 1.0 - ratio;

     //          /* red or gray */
     //          spokecol = (gdouble)spokecolor[i                   ].r * (1.0-t) +
     //                     (gdouble)spokecolor[(i+1) % pvals.nspoke].r * t;

     //          if (w>1.0)
     //            color.r = CLAMP (spokecol * w, 0.0, 1.0);
     //          else
     //            color.r = src[0]/255.0 * compl_ratio + spokecol * ratio;
     //          c = CLAMP (w1 * w, 0.0, 1.0);
     //          color.r += c;
     //          dest[0] = CLAMP (color.r*255.0, 0, 255);

     //          if (bpp>2)
     //          {
     //            /* green */
     //            spokecol = (gdouble)spokecolor[i                   ].g * (1.0-t) +
     //                       (gdouble)spokecolor[(i+1) % pvals.nspoke].g * t;

     //            if (w>1.0)
     //              color.g = CLAMP (spokecol * w, 0.0, 1.0);
     //            else
     //              color.g = src[1]/255.0 * compl_ratio + spokecol * ratio;
     //            c = CLAMP (w1 * w, 0.0, 1.0);
     //            color.g += c;
     //            dest[1] = CLAMP (color.g*255.0, 0, 255);

     //            /* blue */
     //            spokecol = (gdouble)spokecolor[i                   ].b * (1.0-t) +
     //                       (gdouble)spokecolor[(i+1) % pvals.nspoke].b * t;

     //            if (w>1.0)
     //              color.b = CLAMP (spokecol * w, 0.0, 1.0);
     //            else
     //              color.b = src[2]/255.0 * compl_ratio + spokecol * ratio;
     //            c = CLAMP (w1 * w, 0.0, 1.0);
     //            color.b += c;
     //            dest[2] = CLAMP (color.b*255.0, 0, 255);
     //          }

     //          /* alpha */
     //          if (has_alpha)
     //            dest[alpha] = new_alpha * 255.0;

     //          src  += bpp;
     //          dest += bpp;
     //        }
     //    }

     //  gimp_preview_draw_buffer (preview, dest_row, bpp * width);

     //  g_free (cache);
     //  g_free (save_src);
     //  g_free (dest_row);
     }
   else
     { /* normal mode */
       for (pr = gimp_pixel_rgns_register (2, &src_rgn, &dest_rgn);
            pr != NULL ;
            pr = gimp_pixel_rgns_process (pr))
         {
           src_row = src_rgn.data;
           dest_row = dest_rgn.data;

           for (row = 0, y = src_rgn.y; row < src_rgn.h; row++, y++)
             {
               src = src_row;
               dest = dest_row;

               for (col = 0, x = src_rgn.x; col < src_rgn.w; col++, x++)
                 {
                   u = (gdouble) (x-xc) / pvals.radius;
                   v = (gdouble) (y-yc) / pvals.radius;
                   l = sqrt(u*u + v*v);

                   /* This algorithm is still under construction. */
                   t = (atan2 (u, v) / (2 * G_PI) + .51) * pvals.nspoke;
                   i = (gint) floor (t);
                   t -= i;
                   i %= pvals.nspoke;
                   w1 = spoke[i] * (1 - t) + spoke[(i + 1) % pvals.nspoke] * t;
                   w1 = w1 * w1;

                   w = 1/(l+0.001)*0.9;

                   nova_alpha = CLAMP (w, 0.0, 1.0);

                   if (has_alpha)
                     {
                       src_alpha = (gdouble) src[alpha] / 255.0;
                       new_alpha = src_alpha + (1.0 - src_alpha) * nova_alpha;

                       if (new_alpha != 0.0)
                         ratio = nova_alpha / new_alpha;
                       else
                         ratio = 0.0;
                     }
                   else
                     ratio = nova_alpha;

                   compl_ratio = 1.0 - ratio;

                   switch (bpp)
                     {
                     case 1:
                     case 2:
                       /* gray */
                       spokecol = (gdouble)spokecolor[i                   ].r * (1.0-t) +
                                  (gdouble)spokecolor[(i+1) % pvals.nspoke].r * t;
                       if (w>1.0)
                         color.r = CLAMP (spokecol * w, 0.0, 1.0);
                       else
                         color.r = src[0]/255.0 * compl_ratio + spokecol * ratio;
                       c = CLAMP (w1 * w, 0.0, 1.0);
                       color.r += c;
                       dest[0] = CLAMP (color.r*255.0, 0, 255);
                       break;

                     case 3:
                     case 4:
                       /* red */
                       spokecol = (gdouble)spokecolor[i                   ].r * (1.0-t) +
                                  (gdouble)spokecolor[(i+1) % pvals.nspoke].r * t;
                       if (w>1.0)
                         color.r = CLAMP (spokecol * w, 0.0, 1.0);
                       else
                         color.r = src[0]/255.0 * compl_ratio + spokecol * ratio;
                       c = CLAMP (w1 * w, 0.0, 1.0);
                       color.r += c;
                       dest[0] = CLAMP (color.r*255.0, 0, 255);
                       /* green */
                       spokecol = (gdouble)spokecolor[i                   ].g * (1.0-t) +
                                  (gdouble)spokecolor[(i+1) % pvals.nspoke].g * t;
                       if (w>1.0)
                         color.g = CLAMP (spokecol * w, 0.0, 1.0);
                       else
                         color.g = src[1]/255.0 * compl_ratio + spokecol * ratio;
                       c = CLAMP (w1 * w, 0.0, 1.0);
                       color.g += c;
                       dest[1] = CLAMP (color.g*255.0, 0, 255);
                       /* blue */
                       spokecol = (gdouble)spokecolor[i                   ].b * (1.0-t) +
                                  (gdouble)spokecolor[(i+1) % pvals.nspoke].b * t;
                       if (w>1.0)
                         color.b = CLAMP (spokecol * w, 0.0, 1.0);
                       else
                         color.b = src[2]/255.0 * compl_ratio + spokecol * ratio;
                       c = CLAMP (w1 * w, 0.0, 1.0);
                       color.b += c;
                       dest[2] = CLAMP (color.b*255.0, 0, 255);
                       break;
                     }

                   if (has_alpha)
                     dest[alpha] = new_alpha * 255.0;

                   src += src_rgn.bpp;
                   dest += dest_rgn.bpp;
                 }

               src_row += src_rgn.rowstride;
               dest_row += dest_rgn.rowstride;
             }

           /* Update progress */
           progress += src_rgn.w * src_rgn.h;
           gimp_progress_update ((gdouble) progress / (gdouble) max_progress);
         }

       gimp_drawable_flush (drawable);
       gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
       gimp_drawable_update (drawable->drawable_id,
                             x1, y1, (x2 - x1), (y2 - y1));
     }

   g_free (spoke);
   g_free (spokecolor);
   g_rand_free (gr);
}

void operation_nova (void * in_buf, void * out_buf, long width, long height, nova_config * config)
{
    GimpDrawable * drawable = gimp_drawable_get (0, in_buf, out_buf, width, height);

    pvals = *config;
 
    nova(drawable, NULL);

    gimp_drawable_detach (drawable);
}

