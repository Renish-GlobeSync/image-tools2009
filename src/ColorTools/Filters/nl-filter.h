
typedef struct
{
  gdouble  alpha;
  gdouble  radius;
  gint     filter;
} nl_filter_config;

typedef enum
{
  filter_alpha_trim,
  filter_opt_est,
  filter_edge_enhance
} FilterType;


void operation_nl_filter (void * in_buf, void * out_buf, long width, long height, nl_filter_config * config);
