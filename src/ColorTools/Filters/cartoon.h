typedef struct
{
  gdouble  mask_radius;
  gdouble  threshold;
  gdouble  pct_black;
} cartoon_config;

void operation_cartoon (void * in_buf, void * out_buf, long width, long height, cartoon_config * config);
