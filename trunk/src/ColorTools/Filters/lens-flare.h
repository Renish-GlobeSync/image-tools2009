
typedef struct
{
  gint     posx;
  gint     posy;
} lens_flare_config;

void operation_lens_flare (void * in_buf, void * out_buf, long width, long height, lens_flare_config * config);

