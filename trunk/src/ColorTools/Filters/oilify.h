typedef struct
{
  gdouble  mask_size;
  gboolean use_mask_size_map;
  gint     mask_size_map;
  gdouble  exponent;
  gboolean use_exponent_map;
  gint     exponent_map;
  gint     mode;
} oilify_config;

#define MODE_RGB         0
#define MODE_INTEN       1

void operation_oilify (void * in_buf, void * out_buf, long width, long height, oilify_config * config);
