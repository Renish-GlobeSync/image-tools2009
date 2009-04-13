
#include "config.h"

#include <stdlib.h>
#include <string.h>

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

#include "libgimp/stdplugins-intl.h"

#include "value-propagate.h"

#define VP_RGB          (1 << 0)
#define VP_GRAY         (1 << 1)
#define VP_WITH_ALPHA   (1 << 2)
#define VP_WO_ALPHA     (1 << 3)
#define num_direction   4
#define Right2Left      0
#define Bottom2Top      1
#define Left2Right      2
#define Top2Bottom      3

static void         run                        (const gchar       *name,
                                                gint               nparams,
                                                const GimpParam   *param,
                                                gint              *nreturn_vals,
                                                GimpParam        **return_vals);

static GimpPDBStatusType  value_propagate      (GimpDrawable  *drawable);
static void         value_propagate_body       (GimpDrawable  *drawable,
                                                GimpPreview   *preview);
static gboolean     vpropagate_dialog          (GimpDrawable  *drawable);
static void         prepare_row                (GimpPixelRgn  *pixel_rgn,
                                                guchar        *data,
                                                gint           x,
                                                gint           y,
                                                gint           w);

static void    vpropagate_toggle_button_update (GtkWidget     *widget,
                                                gpointer       data);
static GtkWidget *  gtk_table_add_toggle       (GtkWidget     *table,
                                                const gchar   *name,
                                                gint           x1,
                                                gint           x2,
                                                gint           y,
                                                GCallback      update,
                                                gint          *value);

static int          value_difference_check  (guchar *, guchar *, int);
static void         set_value               (GimpImageBaseType,
                                             int, guchar *, guchar *, guchar *,
                                             void *);
static void         initialize_white        (GimpImageBaseType,
                                             int, guchar *, guchar *,
                                             void **);
static void         propagate_white         (GimpImageBaseType,
                                             int, guchar *, guchar *, guchar *,
                                             void *);
static void         initialize_black        (GimpImageBaseType,
                                             int, guchar *, guchar *,
                                             void **);
static void         propagate_black         (GimpImageBaseType,
                                             int, guchar *, guchar *, guchar *,
                                             void *);
static void         initialize_middle       (GimpImageBaseType,
                                             int, guchar *, guchar *,
                                             void **);
static void         propagate_middle        (GimpImageBaseType,
                                             int, guchar *, guchar *, guchar *,
                                             void *);
static void         set_middle_to_peak      (GimpImageBaseType,
                                             int, guchar *, guchar *, guchar *,
                                             void *);
static void         set_foreground_to_peak  (GimpImageBaseType,
                                             int, guchar *, guchar *, guchar *,
                                             void *);
static void         initialize_foreground   (GimpImageBaseType,
                                             int, guchar *, guchar *,
                                             void **);
static void         initialize_background   (GimpImageBaseType,
                                             int, guchar *, guchar *,
                                             void **);
static void         propagate_a_color       (GimpImageBaseType,
                                             int, guchar *, guchar *, guchar *,
                                             void *);
static void         propagate_opaque        (GimpImageBaseType,
                                             int, guchar *, guchar *, guchar *,
                                             void *);
static void         propagate_transparent   (GimpImageBaseType,
                                             int, guchar *, guchar *, guchar *,
                                             void *);

#define SCALE_WIDTH        100
#define PROPAGATING_VALUE  (1 << 0)
#define PROPAGATING_ALPHA  (1 << 1)

/* parameters */
typedef struct
{
  gint     propagate_mode;
  gint     propagating_channel;
  gdouble  propagating_rate;
  gint     direction_mask;
  gint     lower_limit;
  gint     upper_limit;
} VPValueType;

static VPValueType vpvals =
{
  0,        /* propagate_mode                        */
  3,        /* PROPAGATING_VALUE + PROPAGATING_ALPHA */
  1.0,      /* [0.0:1.0]                             */
  15,       /* propagate to all 4 directions         */
  0,        /* lower_limit                           */
  255       /* upper_limit                           */
};

/* dialog variables */
static gint       propagate_alpha;
static gint       propagate_value;
static gint       direction_mask_vec[4];
static gint       channel_mask[] = { 1, 1, 1 };
static gint       peak_max = 1;
static gint       peak_min = 1;
static gint       peak_includes_equals = 1;
static guchar     fore[3];
static GtkWidget *preview;

typedef struct
{
  gint    applicable_image_type;
  gchar  *name;
  void  (*initializer) (GimpImageBaseType, gint, guchar *, guchar *, gpointer *);
  void  (*updater) (GimpImageBaseType, gint, guchar *, guchar *, guchar *, gpointer);
  void  (*finalizer) (GimpImageBaseType, gint, guchar *, guchar *, guchar *, gpointer);
} ModeParam;

#define num_mode 8
static ModeParam modes[num_mode] =
{
  { VP_RGB | VP_GRAY | VP_WITH_ALPHA | VP_WO_ALPHA,
    N_("More _white (larger value)"),
    initialize_white,      propagate_white,       set_value },
  { VP_RGB | VP_GRAY | VP_WITH_ALPHA | VP_WO_ALPHA,
    N_("More blac_k (smaller value)"),
    initialize_black,      propagate_black,       set_value },
  { VP_RGB | VP_GRAY | VP_WITH_ALPHA | VP_WO_ALPHA,
    N_("_Middle value to peaks"),
    initialize_middle,     propagate_middle,      set_middle_to_peak },
  { VP_RGB | VP_GRAY | VP_WITH_ALPHA | VP_WO_ALPHA,
    N_("_Foreground to peaks"),
    initialize_middle,     propagate_middle,      set_foreground_to_peak },
  { VP_RGB | VP_WITH_ALPHA | VP_WO_ALPHA,
    N_("O_nly foreground"),
    initialize_foreground, propagate_a_color,     set_value },
  { VP_RGB | VP_WITH_ALPHA | VP_WO_ALPHA,
    N_("Only b_ackground"),
    initialize_background, propagate_a_color,     set_value },
  { VP_RGB | VP_GRAY | VP_WITH_ALPHA,
    N_("Mor_e opaque"),
    NULL,                  propagate_opaque,      set_value },
  { VP_RGB | VP_GRAY | VP_WITH_ALPHA,
    N_("More t_ransparent"),
    NULL,                  propagate_transparent, set_value },
};

/* registered function entry */
static GimpPDBStatusType
value_propagate (GimpDrawable *drawable)
{
  /* check the validness of parameters */
  if (!(vpvals.propagating_channel & (PROPAGATING_VALUE | PROPAGATING_ALPHA)))
    {
      /* gimp_message ("No channel selected."); */
      return GIMP_PDB_EXECUTION_ERROR;
    }
  if (vpvals.direction_mask == 0)
    {
      /* gimp_message ("No direction selected."); */
      return GIMP_PDB_EXECUTION_ERROR;
    }
  if ((vpvals.lower_limit < 0) || (255 < vpvals.lower_limit) ||
       (vpvals.upper_limit < 0) || (255 < vpvals.lower_limit) ||
       (vpvals.upper_limit < vpvals.lower_limit))
    {
      /* gimp_message ("Limit values are not valid."); */
      return GIMP_PDB_EXECUTION_ERROR;
    }
  value_propagate_body (drawable, NULL);
  return GIMP_PDB_SUCCESS;
}

static void
value_propagate_body (GimpDrawable *drawable,
                      GimpPreview  *preview)
{
  GimpImageBaseType  dtype;
  ModeParam          operation;
  GimpPixelRgn       srcRgn, destRgn;
  guchar            *here, *best, *dest;
  guchar            *dest_row, *prev_row, *cur_row, *next_row;
  guchar            *pr, *cr, *nr, *swap;
  gint               width, height, bytes, index;
  gint               begx, begy, endx, endy, x, y, dx;
  gint               left_index, right_index, up_index, down_index;
  gpointer           tmp;
  GimpRGB            foreground;

  /* calculate neighbors' indexes */
  left_index  = (vpvals.direction_mask & (1 << Left2Right)) ? -1 : 0;
  right_index = (vpvals.direction_mask & (1 << Right2Left)) ?  1 : 0;
  up_index    = (vpvals.direction_mask & (1 << Top2Bottom)) ? -1 : 0;
  down_index  = (vpvals.direction_mask & (1 << Bottom2Top)) ?  1 : 0;
  operation   = modes[vpvals.propagate_mode];
  tmp         = NULL;

  dtype = gimp_drawable_type (drawable->drawable_id);
  bytes = drawable->bpp;

  /* Here I use the algorithm of blur.c . */
  if (preview)
    {
       //gimp_preview_get_position (preview, &begx, &begy);
       //gimp_preview_get_size (preview, &width, &height);
       //endx = begx + width;
       //endy = begy + height;
    }
  else
    {
      gimp_drawable_mask_bounds (drawable->drawable_id,
                                 &begx, &begy, &endx, &endy);
      width  = endx - begx;
      height = endy - begy;
    }

  gimp_tile_cache_ntiles (2 * ((width) / gimp_tile_width () + 1));

  prev_row = g_new (guchar, (width + 2) * bytes);
  cur_row  = g_new (guchar, (width + 2) * bytes);
  next_row = g_new (guchar, (width + 2) * bytes);
  dest_row = g_new (guchar, width * bytes);

  gimp_pixel_rgn_init (&srcRgn, drawable,
                       begx, begy, width, height,
                       FALSE, FALSE);
  gimp_pixel_rgn_init (&destRgn, drawable,
                       begx, begy, width, height,
                       (preview == NULL), TRUE);

  pr = prev_row + bytes;
  cr = cur_row + bytes;
  nr = next_row + bytes;

  prepare_row (&srcRgn, pr, begx, (0 < begy) ? begy : begy - 1, endx-begx);
  prepare_row (&srcRgn, cr, begx, begy, endx-begx);

  best = g_new (guchar, bytes);

  if (!preview)
    gimp_progress_init (_("Value Propagate"));

  gimp_context_get_foreground (&foreground);
  gimp_rgb_get_uchar (&foreground, fore+0, fore+1, fore+2);

  /* start real job */
  for (y = begy ; y < endy ; y++)
    {
      prepare_row (&srcRgn, nr, begx, ((y+1) < endy) ? y+1 : endy, endx-begx);

      for (index = 0; index < (endx - begx) * bytes; index++)
        dest_row[index] = cr[index];

      for (x = 0 ; x < endx - begx; x++)
        {
          dest = dest_row + (x * bytes);
          here = cr + (x * bytes);

          /* *** copy source value to best value holder *** */
          memcpy (best, here, bytes);

          if (operation.initializer)
            (* operation.initializer)(dtype, bytes, best, here, &tmp);

          /* *** gather neighbors' values: loop-unfolded version *** */
          if (up_index == -1)
            for (dx = left_index ; dx <= right_index ; dx++)
              (* operation.updater)(dtype, bytes, here, pr+((x+dx)*bytes), best, tmp);
          for (dx = left_index ; dx <= right_index ; dx++)
            if (dx != 0)
              (* operation.updater)(dtype, bytes, here, cr+((x+dx)*bytes), best, tmp);
          if (down_index == 1)
            for (dx = left_index ; dx <= right_index ; dx++)
              (* operation.updater)(dtype, bytes, here, nr+((x+dx)*bytes), best, tmp);
          /* *** store it to dest_row*** */
          (* operation.finalizer)(dtype, bytes, best, here, dest, tmp);
        }

      /* now store destline to destRgn */
      gimp_pixel_rgn_set_row (&destRgn, dest_row, begx, y, endx - begx);

      /* shift the row pointers  */
      swap = pr;
      pr = cr;
      cr = nr;
      nr = swap;


      if (((y % 16) == 0) && !preview)
        gimp_progress_update ((gdouble) y / (gdouble) (endy - begy));
    }

  if (preview)
    {
      //gimp_drawable_preview_draw_region (GIMP_DRAWABLE_PREVIEW (preview),
      //                                   &destRgn);
    }
  else
    {
      /*  update the region  */
      //gimp_progress_update (1.0);
      gimp_drawable_flush (drawable);
      gimp_drawable_merge_shadow (drawable->drawable_id, TRUE);
      gimp_drawable_update (drawable->drawable_id, begx, begy, endx-begx, endy-begy);
    }
}

static void
prepare_row (GimpPixelRgn *pixel_rgn,
             guchar       *data,
             gint          x,
             gint          y,
             gint          w)
{
  gint b;

  if (y <= 0)
    gimp_pixel_rgn_get_row (pixel_rgn, data, x, (y + 1), w);
  else if (y >= pixel_rgn->h)
    gimp_pixel_rgn_get_row (pixel_rgn, data, x, (y - 1), w);
  else
    gimp_pixel_rgn_get_row (pixel_rgn, data, x, y, w);

  /*  Fill in edge pixels  */
  for (b = 0; b < pixel_rgn->bpp; b++)
    {
      data[-(gint)pixel_rgn->bpp + b] = data[b];
      data[w * pixel_rgn->bpp + b] = data[(w - 1) * pixel_rgn->bpp + b];
    }
}

static void
set_value (GimpImageBaseType  dtype,
           gint               bytes,
           guchar            *best,
           guchar            *here,
           guchar            *dest,
           void              *tmp)
{
  gint  value_chs = 0;
  gint  alpha = 0;
  gint  ch;

  switch (dtype)
    {
    case GIMP_RGB_IMAGE:
      value_chs = 3;
      break;
    case GIMP_RGBA_IMAGE:
      value_chs = 3;
      alpha = 3;
      break;
    case GIMP_GRAY_IMAGE:
      value_chs = 1;
      break;
    case GIMP_GRAYA_IMAGE:
      value_chs = 1;
      alpha = 1;
      break;
    default:
      break;
    }
  for (ch = 0; ch < value_chs; ch++)
    {
      if (vpvals.propagating_channel & PROPAGATING_VALUE) /* value channel */
        *dest++ = (guchar)(vpvals.propagating_rate * best[ch]
                       + (1.0 - vpvals.propagating_rate) * here[ch]);
      else
        *dest++ = here[ch];
    }
  if (alpha)
    {
      if (vpvals.propagating_channel & PROPAGATING_ALPHA) /* alpha channel */
        *dest++ = (guchar)(vpvals.propagating_rate * best[alpha]
                       + (1.0 - vpvals.propagating_rate) * here[alpha]);
      else
        *dest++ = here[alpha];
    }
}

static inline int
value_difference_check (guchar *pos1,
                        guchar *pos2,
                        gint   ch)
{
  gint  index;
  int   diff;

  for (index = 0 ; index < ch; index++)
    if (channel_mask[index] != 0)
      {
        diff = abs(pos1[index] - pos2[index]);
        if (! ((vpvals.lower_limit <= diff) && (diff <= vpvals.upper_limit)))
          return 0;
      }
  return 1;
}

/* mothods for each mode */
static void
initialize_white (GimpImageBaseType   dtype,
                  gint                bytes,
                  guchar             *best,
                  guchar             *here,
                  void              **tmp)
{

  switch (dtype)
    {
    case GIMP_RGB_IMAGE:
    case GIMP_RGBA_IMAGE:
      if (*tmp == NULL)
    *tmp = (void *) g_new (gfloat, 1);
      **(float **)tmp = channel_mask[0] * here[0] * here[0]
                     + channel_mask[1] * here[1] * here[1]
                     + channel_mask[2] * here[2] * here[2];
      break;
    case GIMP_GRAYA_IMAGE:
    case GIMP_GRAY_IMAGE:
      break;
    default:
      break;
    }
}

static void
propagate_white (GimpImageBaseType  dtype,
                 gint               bytes,
                 guchar            *orig,
                 guchar            *here,
                 guchar            *best,
                 void              *tmp)
{
  float v_here;

  switch (dtype)
    {
    case GIMP_RGB_IMAGE:
    case GIMP_RGBA_IMAGE:
      v_here = channel_mask[0] * here[0] * here[0]
                     + channel_mask[1] * here[1] * here[1]
                     + channel_mask[2] * here[2] * here[2];
     if (*(float *)tmp < v_here && value_difference_check(orig, here, 3))
        {
          *(float *)tmp = v_here;
          memcpy(best, here, 3 * sizeof(guchar)); /* alpha channel holds old value */
        }
      break;
    case GIMP_GRAYA_IMAGE:
    case GIMP_GRAY_IMAGE:
      if (*best < *here && value_difference_check(orig, here, 1))
        *best = *here;
      break;
    default:
      break;
    }
}

static void
initialize_black (GimpImageBaseType   dtype,
                  gint                channels,
                  guchar             *best,
                  guchar             *here,
                  void              **tmp)
{
  switch (dtype)
    {
    case GIMP_RGB_IMAGE:
    case GIMP_RGBA_IMAGE:
      if (*tmp == NULL)
    *tmp = (void *) g_new (gfloat, 1);
      **(float **)tmp = (channel_mask[0] * here[0] * here[0]
                     + channel_mask[1] * here[1] * here[1]
                     + channel_mask[2] * here[2] * here[2]);
      break;
    case GIMP_GRAYA_IMAGE:
    case GIMP_GRAY_IMAGE:
      break;
    default:
      break;
    }
}

static void
propagate_black (GimpImageBaseType  image_type,
                 gint               channels,
                 guchar            *orig,
                 guchar            *here,
                 guchar            *best,
                 void              *tmp)
{
  float v_here;

  switch (image_type)
    {
    case GIMP_RGB_IMAGE:
    case GIMP_RGBA_IMAGE:
      v_here = (channel_mask[0] * here[0] * here[0]
                     + channel_mask[1] * here[1] * here[1]
                     + channel_mask[2] * here[2] * here[2]);
      if (v_here < *(float *)tmp && value_difference_check(orig, here, 3))
        {
          *(float *)tmp = v_here;
          memcpy (best, here, 3 * sizeof(guchar)); /* alpha channel holds old value */
        }
      break;
    case GIMP_GRAYA_IMAGE:
    case GIMP_GRAY_IMAGE:
      if (*here < *best && value_difference_check(orig, here, 1))
        *best = *here;
      break;
    default:
      break;
    }
}

typedef struct
{
  gshort min_modified;
  gshort max_modified;
  glong  original_value;
  glong  minv;
  guchar min[3];
  glong  maxv;
  guchar max[3];
} MiddlePacket;

static void
initialize_middle (GimpImageBaseType   image_type,
                   gint                channels,
                   guchar             *best,
                   guchar             *here,
                   void              **tmp)
{
  int index;
  MiddlePacket *data;

  if (*tmp == NULL)
    *tmp = (void *) g_new (MiddlePacket, 1);
  data = (MiddlePacket *)*tmp;
  for (index = 0; index < channels; index++)
    data->min[index] = data->max[index] = here[index];
  switch (image_type)
    {
    case GIMP_RGB_IMAGE:
    case GIMP_RGBA_IMAGE:
      data->original_value = (channel_mask[0] * here[0] * here[0]
                                   + channel_mask[1] * here[1] * here[1]
                                   + channel_mask[2] * here[2] * here[2]);
      break;
    case GIMP_GRAYA_IMAGE:
    case GIMP_GRAY_IMAGE:
      data->original_value = *here;
      break;
    default:
      break;
    }
  data->minv = data->maxv = data->original_value;
  data->min_modified = data->max_modified = 0;
}

static void
propagate_middle (GimpImageBaseType  image_type,
                  gint               channels,
                  guchar            *orig,
                  guchar            *here,
                  guchar            *best,
                  void              *tmp)
{
  float v_here;
  MiddlePacket *data;

  data = (MiddlePacket *)tmp;

  switch (image_type)
    {
    case GIMP_RGB_IMAGE:
    case GIMP_RGBA_IMAGE:
      v_here = (channel_mask[0] * here[0] * here[0]
                     + channel_mask[1] * here[1] * here[1]
                     + channel_mask[2] * here[2] * here[2]);
      if ((v_here <= data->minv) && value_difference_check(orig, here, 3))
        {
          data->minv = v_here;
          memcpy (data->min, here, 3*sizeof(guchar));
          data->min_modified = 1;
        }
      if (data->maxv <= v_here && value_difference_check(orig, here, 3))
        {
          data->maxv = v_here;
          memcpy (data->max, here, 3*sizeof(guchar));
          data->max_modified = 1;
        }
      break;
    case GIMP_GRAYA_IMAGE:
    case GIMP_GRAY_IMAGE:
      if ((*here <= data->min[0]) && value_difference_check(orig, here, 1))
        {
          data->min[0] = *here;
          data->min_modified = 1;
        }
      if ((data->max[0] <= *here) && value_difference_check(orig, here, 1))
        {
          data->max[0] = *here;
          data->max_modified = 1;
        }
      break;
    default:
      break;
    }
}

static void
set_middle_to_peak (GimpImageBaseType  image_type,
                    gint               channels,
                    guchar            *here,
                    guchar            *best,
                    guchar            *dest,
                    void              *tmp)
{
  gint  value_chs = 0;
  gint  alpha = 0;
  gint  ch;
  MiddlePacket  *data;

  data = (MiddlePacket *)tmp;
  if (! ((peak_min & (data->minv == data->original_value))
         || (peak_max & (data->maxv == data->original_value))))
    return;
  if ((! peak_includes_equals)
      && ((peak_min & (! data->min_modified))
          || (peak_max & (! data->max_modified))))
      return;

  switch (image_type)
    {
    case GIMP_RGB_IMAGE:
      value_chs = 3;
      break;
    case GIMP_RGBA_IMAGE:
      value_chs = 3;
      alpha = 3;
      break;
    case GIMP_GRAY_IMAGE:
      value_chs = 1;
      break;
    case GIMP_GRAYA_IMAGE:
      value_chs = 1;
      alpha = 1;
      break;
    default:
      break;
    }
  for (ch = 0; ch < value_chs; ch++)
    {
      if (vpvals.propagating_channel & PROPAGATING_VALUE) /* value channel */
        *dest++ = (guchar)(vpvals.propagating_rate * 0.5 * (data->min[ch] + data->max[ch])
                       + (1.0 - vpvals.propagating_rate) * here[ch]);
      else
        *dest++ = here[ch];
    }
  if (alpha)
    {
      if (vpvals.propagating_channel & PROPAGATING_ALPHA) /* alpha channel */
        *dest++ = (guchar)(vpvals.propagating_rate * best[alpha]
                       + (1.0 - vpvals.propagating_rate) * here[alpha]);
      else
        *dest++ = here[alpha];
    }
}

static void
set_foreground_to_peak (GimpImageBaseType  image_type,
                        gint               channels,
                        guchar            *here,
                        guchar            *best,
                        guchar            *dest,
                        void              *tmp)
{
  gint  value_chs = 0;
  gint  alpha = 0;
  gint  ch;
  MiddlePacket  *data;

  data = (MiddlePacket *)tmp;
  if (! ((peak_min & (data->minv == data->original_value))
         || (peak_max & (data->maxv == data->original_value))))
    return;
  if (peak_includes_equals
      && ((peak_min & (! data->min_modified))
          || (peak_max & (! data->max_modified))))
      return;

  switch (image_type)
    {
    case GIMP_RGB_IMAGE:
      value_chs = 3;
      break;
    case GIMP_RGBA_IMAGE:
      value_chs = 3;
      alpha = 3;
      break;
    case GIMP_GRAY_IMAGE:
      value_chs = 1;
      break;
    case GIMP_GRAYA_IMAGE:
      value_chs = 1;
      alpha = 1;
      break;
    default:
      break;
    }
  for (ch = 0; ch < value_chs; ch++)
    {
      if (vpvals.propagating_channel & PROPAGATING_VALUE) /* value channel */
        *dest++ = (guchar)(vpvals.propagating_rate*fore[ch]
                       + (1.0 - vpvals.propagating_rate)*here[ch]);
      else
        *dest++ = here[ch];
    }
}

static void
initialize_foreground (GimpImageBaseType   image_type,
                       gint                channels,
                       guchar             *here,
                       guchar             *best,
                       void              **tmp)
{
  GimpRGB  foreground;
  guchar  *ch;

  if (*tmp == NULL)
    {
      *tmp = (void *) g_new (guchar, 3);
      ch = (guchar *)*tmp;
      //gimp_context_get_foreground (&foreground);
      gimp_rgb_get_uchar (&foreground, &ch[0], &ch[1], &ch[2]);
    }
}

static void
initialize_background (GimpImageBaseType   image_type,
                       gint                channels,
                       guchar             *here,
                       guchar             *best,
                       void              **tmp)
{
  GimpRGB  background;
  guchar  *ch;

  if (*tmp == NULL)
    {
      *tmp = (void *) g_new (guchar, 3);
      ch = (guchar *)*tmp;
      //gimp_context_get_background (&background);
      gimp_rgb_get_uchar (&background, &ch[0], &ch[1], &ch[2]);
    }
}

static void
propagate_a_color (GimpImageBaseType  image_type,
                   gint               channels,
                   guchar            *orig,
                   guchar            *here,
                   guchar            *best,
                   void              *tmp)
{
  guchar *fg = (guchar *)tmp;

  switch (image_type)
    {
    case GIMP_RGB_IMAGE:
    case GIMP_RGBA_IMAGE:
      if (here[0] == fg[0] && here[1] == fg[1] && here[2] == fg[2] &&
          value_difference_check(orig, here, 3))
        {
          memcpy (best, here, 3 * sizeof(guchar)); /* alpha channel holds old value */
        }
      break;
    case GIMP_GRAYA_IMAGE:
    case GIMP_GRAY_IMAGE:
      break;
    default:
      break;
    }
}

static void
propagate_opaque (GimpImageBaseType  image_type,
                  gint               channels,
                  guchar            *orig,
                  guchar            *here,
                  guchar            *best,
                  void              *tmp)
{
  switch (image_type)
    {
    case GIMP_RGBA_IMAGE:
      if (best[3] < here[3] && value_difference_check(orig, here, 3))
        memcpy(best, here, channels * sizeof(guchar));
      break;
    case GIMP_GRAYA_IMAGE:
      if (best[1] < here[1] && value_difference_check(orig, here, 1))
        memcpy(best, here, channels * sizeof(guchar));
      break;
    default:
      break;
    }
}

static void
propagate_transparent (GimpImageBaseType  image_type,
                       gint               channels,
                       guchar            *orig,
                       guchar            *here,
                       guchar            *best,
                       void              *tmp)
{
  switch (image_type)
    {
    case GIMP_RGBA_IMAGE:
      if (here[3] < best[3] && value_difference_check(orig, here, 3))
        memcpy(best, here, channels * sizeof(guchar));
      break;
    case GIMP_GRAYA_IMAGE:
      if (here[1] < best[1] && value_difference_check(orig, here, 1))
        memcpy(best, here, channels * sizeof(guchar));
      break;
    default:
      break;
    }
}

void operation_dilate (void * in_buf, void * out_buf, long width, long height, void * v)
{
    GimpDrawable * drawable = gimp_drawable_get (0, in_buf, out_buf, width, height);
    vpvals.propagating_channel = PROPAGATING_VALUE;
    vpvals.propagating_rate    = 1.0;
    vpvals.direction_mask      = 15;
    vpvals.lower_limit         = 0;
    vpvals.upper_limit         = 255;
    vpvals.propagate_mode      = 0;

    value_propagate (drawable);

    gimp_drawable_detach (drawable);
}

void operation_erode (void * in_buf, void * out_buf, long width, long height, void * v)
{
    GimpDrawable * drawable = gimp_drawable_get (0, in_buf, out_buf, width, height);
    vpvals.propagating_channel = PROPAGATING_VALUE;
    vpvals.propagating_rate    = 1.0;
    vpvals.direction_mask      = 15;
    vpvals.lower_limit         = 0;
    vpvals.upper_limit         = 255;
    vpvals.propagate_mode      = 1;

    value_propagate (drawable);

    gimp_drawable_detach (drawable);
}
