
typedef struct
{
  gdouble  radius;
  gdouble  amount;
  gint     threshold;
} unsharp_mask_config;

void operation_unsharp_mask (void * in_buf, void * out_buf, long width, long height, unsharp_mask_config * config);

