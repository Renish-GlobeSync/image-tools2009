
typedef struct
{
  gdouble  centre_x;
  gdouble  centre_y;
  gdouble  square_a;
  gdouble  quad_a;
  gdouble  scale_a;
  gdouble  brighten;
} lens_distortion_config;

void operation_lens_distortion (void * in_buf, void * out_buf, long width, long height, lens_distortion_config * config);
