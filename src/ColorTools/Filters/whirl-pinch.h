
typedef struct
{
  gdouble  whirl;
  gdouble  pinch;
  gdouble  radius;
} whirl_pinch_config;

void operation_whirl_pinch (void * in_buf, void * out_buf, long width, long height, whirl_pinch_config * config);
