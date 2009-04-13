
#include "config.h"

#include <string.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "libgimp/stdplugins-intl.h"

#include "red-eye-removal.h"

/* Declare local functions.
*/

static void     remove_redeye         (GimpDrawable     *drawable);
static void     remove_redeye_preview (GimpDrawable     *drawable,
                                       GimpPreview      *preview);
static void     redeye_inner_loop     (const guchar     *src,
                                       guchar           *dest,
                                       gint              width,
                                       gint              height,
                                       gint              bpp,
                                       gboolean          has_alpha,
                                       int               rowstride);


#define RED_FACTOR    0.5133333
#define GREEN_FACTOR  1
#define BLUE_FACTOR   0.1933333

#define SCALE_WIDTH   100


static gint threshold = 50;


/*
* Red Eye Removal Alorithm, based on using a threshold to detect
* red pixels. Having a user-made selection around the eyes will
* prevent incorrect pixels from being selected.
*/
static void
remove_redeye (GimpDrawable *drawable)
{
    GimpPixelRgn  src_rgn;
    GimpPixelRgn  dest_rgn;
    gint          progress, max_progress;
    gboolean      has_alpha;
    gint          x, y;
    gint          width, height;
    gint          i;
    gpointer      pr;

    if (! gimp_drawable_mask_intersect (drawable->drawable_id,
        &x, &y, &width, &height))
        return;

    gimp_progress_init (_("Removing red eye"));

    has_alpha = gimp_drawable_has_alpha (drawable->drawable_id);

    progress = 0;
    max_progress = width * height;

    gimp_pixel_rgn_init (&src_rgn, drawable,
        x, y, width, height, FALSE, FALSE);
    gimp_pixel_rgn_init (&dest_rgn, drawable,
        x, y, width, height, TRUE, TRUE);

    for (pr = gimp_pixel_rgns_register (2, &src_rgn, &dest_rgn), i = 0;
        pr != NULL;
        pr = gimp_pixel_rgns_process (pr), i++)
    {
        redeye_inner_loop (src_rgn.data, dest_rgn.data, src_rgn.w, src_rgn.h,
            src_rgn.bpp, has_alpha, src_rgn.rowstride);

        progress += src_rgn.w * src_rgn.h;

        if (i % 16 == 0)
            gimp_progress_update ((gdouble) progress / (gdouble) max_progress);
    }

    gimp_drawable_flush (drawable);
    gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
    gimp_drawable_update (drawable->drawable_id, x, y, width, height);
}

static void
redeye_inner_loop (const guchar *src,
                   guchar       *dest,
                   gint          width,
                   gint          height,
                   gint          bpp,
                   gboolean      has_alpha,
                   gint          rowstride)
{
    const gint red   = 0;
    const gint green = 1;
    const gint blue  = 2;
    const gint alpha = 3;
    gint       x, y;

    for (y = 0; y < height; y++)
    {
        const guchar *s = src;
        guchar       *d = dest;

        for (x = 0; x < width; x++)
        {
            gint adjusted_red       = s[red] * RED_FACTOR;
            gint adjusted_green     = s[green] * GREEN_FACTOR;
            gint adjusted_blue      = s[blue] * BLUE_FACTOR;
            gint adjusted_threshold = (threshold - 50) * 2;

            if (adjusted_red >= adjusted_green - adjusted_threshold &&
                adjusted_red >= adjusted_blue - adjusted_threshold)
            {
                d[red] = CLAMP (((gdouble) (adjusted_green + adjusted_blue)
                    / (2.0  * RED_FACTOR)), 0, 255);
            }
            else
            {
                d[red] = s[red];
            }

            d[green] = s[green];
            d[blue] = s[blue];

            if (has_alpha)
                d[alpha] = s[alpha];

            s += bpp;
            d += bpp;
        }

        src += rowstride;
        dest += rowstride;
    }
}

void operation_red_eye_removal(void * in_buf, void * out_buf, long width, long height, red_eye_removal_config * config)
{
    GimpDrawable * drawable = gimp_drawable_get (0, in_buf, out_buf, width, height);

    threshold = config->threshold;

    remove_redeye(drawable);

    gimp_drawable_detach (drawable);
}
