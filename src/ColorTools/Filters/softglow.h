typedef struct
{
  gdouble glow_radius;
  gdouble brightness;
  gdouble sharpness;
} softglow_config;

void operation_softglow (void * in_buf, void * out_buf, long width, long height, softglow_config * config);
