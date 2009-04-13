typedef struct
{
  gint     xcenter;
  gint     ycenter;
  GimpRGB  color;
  gint     radius;
  gint     nspoke;
  gint     randomhue;
} nova_config;

void operation_nova (void * in_buf, void * out_buf, long width, long height, nova_config * config);
