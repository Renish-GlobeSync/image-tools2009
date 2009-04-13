
typedef struct
{
  gint  sharpen_percent;
} sharpen_config;

void operation_sharpen (void * in_buf, void * out_buf, long width, long height, sharpen_config * config);
