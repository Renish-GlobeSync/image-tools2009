
typedef struct
{
  gdouble  refraction;
  gboolean keep_surr;
  gboolean use_bkgr;
  gboolean set_transparent;
} lens_apply_config;

void operation_lens_apply (void * in_buf, void * out_buf, long width, long height, lens_apply_config * config);
