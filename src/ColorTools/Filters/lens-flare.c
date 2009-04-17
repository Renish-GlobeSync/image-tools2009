
#include "config.h"

#include <string.h>

#include <libgimp/gimp.h>

#include "lens-flare.h"

/* --- Typedefs --- */

typedef struct REFLECT
{
  GimpRGB ccol;
  gfloat  size;
  gint    xp;
  gint    yp;
  gint    type;
} Reflect;



static void        FlareFX                        (GimpDrawable     *drawable,
                                                   GimpPreview      *preview);
static void mcolor  (guchar  *s,
                     gfloat   h);
static void mglow   (guchar  *s,
                     gfloat   h);
static void minner  (guchar  *s,
                     gfloat   h);
static void mouter  (guchar  *s,
                     gfloat   h);
static void mhalo   (guchar  *s,
                     gfloat   h);
static void initref (gint     sx,
                     gint     sy,
                     gint     width,
                     gint     height,
                     gint     matt);
static void fixpix  (guchar  *data,
                     float    procent,
                     GimpRGB *colpro);
static void mrt1    (guchar  *s,
                     Reflect *ref,
                     gint     col,
                     gint     row);
static void mrt2    (guchar  *s,
                     Reflect *ref,
                     gint     col,
                     gint     row);
static void mrt3    (guchar  *s,
                     Reflect *ref,
                     gint     col,
                     gint     row);
static void mrt4    (guchar  *s,
                     Reflect *ref,
                     gint     col,
                     gint     row);


/* --- Variables --- */

static lens_flare_config fvals =
{
  128, 128   /* posx, posy */
};

static gfloat     scolor, sglow, sinner, souter; /* size     */
static gfloat     shalo;
static gint       xs, ys;
static gint       numref;
static GimpRGB    color, glow, inner, outer, halo;
static Reflect    ref1[19];
static gboolean   show_cursor = TRUE;


/* --- Filter functions --- */
static void
FlareFX (GimpDrawable *drawable,
         GimpPreview  *preview)
{
  GimpPixelRgn  srcPR, destPR;
  gint          width, height;
  gint          bytes;
  guchar       *cur_row, *s;
  guchar       *src  = NULL;
  guchar       *dest = NULL;
  gint          row, col, i;
  gint          x1, y1, x2, y2;
  gint          matt;
  gfloat        hyp;
  gdouble       zoom = 0.0;

  bytes  = drawable->bpp;
  if (preview)
    {
      //src = gimp_zoom_preview_get_source (GIMP_ZOOM_PREVIEW (preview),
      //                                    &width, &height, &bytes);

      //zoom = gimp_zoom_preview_get_factor (GIMP_ZOOM_PREVIEW (preview));

      //gimp_preview_transform (preview,
      //                        fvals.posx, fvals.posy, &xs, &ys);

      //x1 = 0;
      //y1 = 0;
      //x2 = width;
      //y2 = height;
      //dest = g_new (guchar, bytes * width * height);
    }
  else
    {
      gimp_drawable_mask_bounds (drawable->drawable_id, &x1, &y1, &x2, &y2);
      width  = drawable->width;
      height = drawable->height;

      xs = fvals.posx; /* set x,y of flare center */
      ys = fvals.posy;
      /*  initialize the pixel regions  */
      gimp_pixel_rgn_init (&srcPR, drawable, 0, 0, width, height, FALSE, FALSE);
      gimp_pixel_rgn_init (&destPR, drawable, 0, 0, width, height, TRUE, TRUE);
    }

  //if (preview)
  //  matt = width * zoom;
  //else
    matt = width;

  cur_row = g_new (guchar, (x2 - x1) * bytes);

  scolor = (gfloat) matt * 0.0375;
  sglow  = (gfloat) matt * 0.078125;
  sinner = (gfloat) matt * 0.1796875;
  souter = (gfloat) matt * 0.3359375;
  shalo  = (gfloat) matt * 0.084375;

  color.r = 239.0/255.0; color.g = 239.0/255.0; color.b = 239.0/255.0;
  glow.r  = 245.0/255.0; glow.g  = 245.0/255.0; glow.b  = 245.0/255.0;
  inner.r = 255.0/255.0; inner.g = 38.0/255.0;  inner.b = 43.0/255.0;
  outer.r = 69.0/255.0;  outer.g = 59.0/255.0;  outer.b = 64.0/255.0;
  halo.r  = 80.0/255.0;  halo.g  = 15.0/255.0;  halo.b  = 4.0/255.0;

  initref (xs, ys, width, height, matt);

  /*  Loop through the rows */
  for (row = y1; row < y2; row++) /* y-coord */
    {
      if (preview)
        memcpy (cur_row, src + row * width * bytes, width * bytes);
      else
        gimp_pixel_rgn_get_row (&srcPR, cur_row, x1, row, x2-x1);

      s = cur_row;
      for (col = x1; col < x2; col++) /* x-coord */
        {
          hyp = hypot (col-xs, row-ys);

          mcolor (s, hyp); /* make color */
          mglow (s, hyp);  /* make glow  */
          minner (s, hyp); /* make inner */
          mouter (s, hyp); /* make outer */
          mhalo (s, hyp);  /* make halo  */

          for (i = 0; i < numref; i++)
            {
              switch (ref1[i].type)
                {
                case 1:
                  mrt1 (s, ref1 + i, col, row);
                  break;
                case 2:
                  mrt2 (s, ref1 + i, col, row);
                  break;
                case 3:
                  mrt3 (s, ref1 + i, col, row);
                  break;
                case 4:
                  mrt4 (s, ref1 + i, col, row);
                  break;
                }
            }
          s += bytes;
        }
      if (preview)
        {
          memcpy (dest + row * width * bytes, cur_row, width * bytes);
        }
      else
        {
          /*  store the dest  */
          gimp_pixel_rgn_set_row (&destPR, cur_row, x1, row, (x2 - x1));
        }

      if ((row % 5) == 0 && !preview)
        gimp_progress_update ((double) row / (double) (y2 - y1));
    }

  if (preview)
    {
      //gimp_preview_draw_buffer (preview, dest, width * bytes);
      g_free (src);
      g_free (dest);
    }
  else
    {
      /*  update the textured region  */
      gimp_drawable_flush (drawable);
      gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
      gimp_drawable_update (drawable->drawable_id, x1, y1, (x2 - x1), (y2 - y1));
    }

  g_free (cur_row);
}

static void
mcolor (guchar *s,
        gfloat  h)
{
  gfloat procent;

  procent  = scolor - h;
  procent /= scolor;

  if (procent > 0.0)
    {
      procent *= procent;
      fixpix (s, procent, &color);
    }
}

static void
mglow (guchar *s,
       gfloat  h)
{
  gfloat procent;

  procent  = sglow - h;
  procent /= sglow;

  if (procent > 0.0)
    {
      procent *= procent;
      fixpix (s, procent, &glow);
    }
}

static void
minner (guchar *s,
        gfloat  h)
{
  gfloat procent;

  procent  = sinner - h;
  procent /= sinner;

  if (procent > 0.0)
    {
      procent *= procent;
      fixpix (s, procent, &inner);
    }
}

static void
mouter (guchar *s,
        gfloat  h)
{
  gfloat procent;

  procent  = souter - h;
  procent /= souter;

  if (procent > 0.0)
    fixpix (s, procent, &outer);
}

static void
mhalo (guchar *s,
       gfloat  h)
{
  gfloat procent;

  procent  = h - shalo;
  procent /= (shalo * 0.07);
  procent  = fabs (procent);

  if (procent < 1.0)
    fixpix (s, 1.0 - procent, &halo);
}

static void
fixpix (guchar   *data,
        float     procent,
        GimpRGB  *colpro)
{
  data[0] += (255 - data[0]) * procent * colpro->r;
  data[1] += (255 - data[1]) * procent * colpro->g;
  data[2] += (255 - data[2]) * procent * colpro->b;
}

static void
initref (gint sx,
         gint sy,
         gint width,
         gint height,
         gint matt)
{
  gint xh, yh, dx, dy;

  xh = width / 2; yh = height / 2;
  dx = xh - sx;   dy = yh - sy;
  numref = 19;
  ref1[0].type=1; ref1[0].size=(gfloat)matt*0.027;
  ref1[0].xp=0.6699*dx+xh; ref1[0].yp=0.6699*dy+yh;
  ref1[0].ccol.r=0.0; ref1[0].ccol.g=14.0/255.0; ref1[0].ccol.b=113.0/255.0;
  ref1[1].type=1; ref1[1].size=(gfloat)matt*0.01;
  ref1[1].xp=0.2692*dx+xh; ref1[1].yp=0.2692*dy+yh;
  ref1[1].ccol.r=90.0/255.0; ref1[1].ccol.g=181.0/255.0; ref1[1].ccol.b=142.0/255.0;
  ref1[2].type=1; ref1[2].size=(gfloat)matt*0.005;
  ref1[2].xp=-0.0112*dx+xh; ref1[2].yp=-0.0112*dy+yh;
  ref1[2].ccol.r=56.0/255.0; ref1[2].ccol.g=140.0/255.0; ref1[2].ccol.b=106.0/255.0;
  ref1[3].type=2; ref1[3].size=(gfloat)matt*0.031;
  ref1[3].xp=0.6490*dx+xh; ref1[3].yp=0.6490*dy+yh;
  ref1[3].ccol.r=9.0/255.0; ref1[3].ccol.g=29.0/255.0; ref1[3].ccol.b=19.0/255.0;
  ref1[4].type=2; ref1[4].size=(gfloat)matt*0.015;
  ref1[4].xp=0.4696*dx+xh; ref1[4].yp=0.4696*dy+yh;
  ref1[4].ccol.r=24.0/255.0; ref1[4].ccol.g=14.0/255.0; ref1[4].ccol.b=0.0;
  ref1[5].type=2; ref1[5].size=(gfloat)matt*0.037;
  ref1[5].xp=0.4087*dx+xh; ref1[5].yp=0.4087*dy+yh;
  ref1[5].ccol.r=24.0/255.0; ref1[5].ccol.g=14.0/255.0; ref1[5].ccol.b=0.0;
  ref1[6].type=2; ref1[6].size=(gfloat)matt*0.022;
  ref1[6].xp=-0.2003*dx+xh; ref1[6].yp=-0.2003*dy+yh;
  ref1[6].ccol.r=42.0/255.0; ref1[6].ccol.g=19.0/255.0; ref1[6].ccol.b=0.0;
  ref1[7].type=2; ref1[7].size=(gfloat)matt*0.025;
  ref1[7].xp=-0.4103*dx+xh; ref1[7].yp=-0.4103*dy+yh;
  ref1[7].ccol.b=17.0/255.0; ref1[7].ccol.g=9.0/255.0; ref1[7].ccol.r=0.0;
  ref1[8].type=2; ref1[8].size=(gfloat)matt*0.058;
  ref1[8].xp=-0.4503*dx+xh; ref1[8].yp=-0.4503*dy+yh;
  ref1[8].ccol.b=10.0/255.0; ref1[8].ccol.g=4.0/255.0; ref1[8].ccol.r=0.0;
  ref1[9].type=2; ref1[9].size=(gfloat)matt*0.017;
  ref1[9].xp=-0.5112*dx+xh; ref1[9].yp=-0.5112*dy+yh;
  ref1[9].ccol.r=5.0/255.0; ref1[9].ccol.g=5.0/255.0; ref1[9].ccol.b=14.0/255.0;
  ref1[10].type=2; ref1[10].size=(gfloat)matt*0.2;
  ref1[10].xp=-1.496*dx+xh; ref1[10].yp=-1.496*dy+yh;
  ref1[10].ccol.r=9.0/255.0; ref1[10].ccol.g=4.0/255.0; ref1[10].ccol.b=0.0;
  ref1[11].type=2; ref1[11].size=(gfloat)matt*0.5;
  ref1[11].xp=-1.496*dx+xh; ref1[11].yp=-1.496*dy+yh;
  ref1[11].ccol.r=9.0/255.0; ref1[11].ccol.g=4.0/255.0; ref1[11].ccol.b=0.0;
  ref1[12].type=3; ref1[12].size=(gfloat)matt*0.075;
  ref1[12].xp=0.4487*dx+xh; ref1[12].yp=0.4487*dy+yh;
  ref1[12].ccol.r=34.0/255.0; ref1[12].ccol.g=19.0/255.0; ref1[12].ccol.b=0.0;
  ref1[13].type=3; ref1[13].size=(gfloat)matt*0.1;
  ref1[13].xp=dx+xh; ref1[13].yp=dy+yh;
  ref1[13].ccol.r=14.0/255.0; ref1[13].ccol.g=26.0/255.0; ref1[13].ccol.b=0.0;
  ref1[14].type=3; ref1[14].size=(gfloat)matt*0.039;
  ref1[14].xp=-1.301*dx+xh; ref1[14].yp=-1.301*dy+yh;
  ref1[14].ccol.r=10.0/255.0; ref1[14].ccol.g=25.0/255.0; ref1[14].ccol.b=13.0/255.0;
  ref1[15].type=4; ref1[15].size=(gfloat)matt*0.19;
  ref1[15].xp=1.309*dx+xh; ref1[15].yp=1.309*dy+yh;
  ref1[15].ccol.r=9.0/255.0; ref1[15].ccol.g=0.0; ref1[15].ccol.b=17.0/255.0;
  ref1[16].type=4; ref1[16].size=(gfloat)matt*0.195;
  ref1[16].xp=1.309*dx+xh; ref1[16].yp=1.309*dy+yh;
  ref1[16].ccol.r=9.0/255.0; ref1[16].ccol.g=16.0/255.0; ref1[16].ccol.b=5.0/255.0;
  ref1[17].type=4; ref1[17].size=(gfloat)matt*0.20;
  ref1[17].xp=1.309*dx+xh; ref1[17].yp=1.309*dy+yh;
  ref1[17].ccol.r=17.0/255.0; ref1[17].ccol.g=4.0/255.0; ref1[17].ccol.b=0.0;
  ref1[18].type=4; ref1[18].size=(gfloat)matt*0.038;
  ref1[18].xp=-1.301*dx+xh; ref1[18].yp=-1.301*dy+yh;
  ref1[18].ccol.r=17.0/255.0; ref1[18].ccol.g=4.0/255.0; ref1[18].ccol.b=0.0;
}

static void
mrt1 (guchar  *s,
      Reflect *ref,
      gint     col,
      gint     row)
{
  gfloat procent;

  procent  = ref->size - hypot (ref->xp - col, ref->yp - row);
  procent /= ref->size;

  if (procent > 0.0)
    {
      procent *= procent;
      fixpix (s, procent, &ref->ccol);
    }
}

static void
mrt2 (guchar *s,
      Reflect *ref,
      gint    col,
      gint    row)
{
  gfloat procent;

  procent  = ref->size - hypot (ref->xp - col, ref->yp - row);
  procent /= (ref->size * 0.15);

  if (procent > 0.0)
    {
      if (procent > 1.0)
        procent = 1.0;

      fixpix (s, procent, &ref->ccol);
    }
}

static void
mrt3 (guchar *s,
      Reflect *ref,
      gint    col,
      gint    row)
{
  gfloat procent;

  procent  = ref->size - hypot (ref->xp - col, ref->yp - row);
  procent /= (ref->size * 0.12);

  if (procent > 0.0)
    {
      if (procent > 1.0)
        procent = 1.0 - (procent * 0.12);

      fixpix (s, procent, &ref->ccol);
    }
}

static void
mrt4 (guchar *s,
      Reflect *ref,
      gint    col,
      gint    row)
{
  gfloat procent;

  procent  = hypot (ref->xp - col, ref->yp - row) - ref->size;
  procent /= (ref->size*0.04);
  procent  = fabs (procent);

  if (procent < 1.0)
    fixpix (s, 1.0 - procent, &ref->ccol);
}

void operation_lens_flare (void * in_buf, void * out_buf, long width, long height, lens_flare_config * config)
{
    GimpDrawable * drawable = gimp_drawable_get (0, in_buf, out_buf, width, height);

    fvals = *config;
 
    FlareFX(drawable, NULL);

    gimp_drawable_detach (drawable);
}
