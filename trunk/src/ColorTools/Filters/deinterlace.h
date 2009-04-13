enum
{
  ODD_FIELDS,
  EVEN_FIELDS
};

typedef struct
{
  gint     evenness;
} deinterlace_config;

void operation_deinterlace (void * in_buf, void * out_buf, long width, long height, deinterlace_config * config);
