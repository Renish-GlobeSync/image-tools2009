// convolution_matrix.h

#ifndef BIG_MATRIX
#define MATRIX_SIZE   (5)
#else
#define MATRIX_SIZE   (11)
#endif

#define HALF_WINDOW   (MATRIX_SIZE/2)
#define MATRIX_CELLS  (MATRIX_SIZE*MATRIX_SIZE)
#define DEST_ROWS     (MATRIX_SIZE/2 + 1)
#define CHANNELS      (5)
#define BORDER_MODES  (3)

typedef enum
{
  EXTEND,
  WRAP,
  CLEAR
} BorderMode;

typedef struct
{
  gfloat     matrix[MATRIX_SIZE][MATRIX_SIZE];
  gfloat     divisor;
  gfloat     offset;
  gint       alpha_weighting;
  BorderMode bmode;
  gboolean   channels[CHANNELS];
  gboolean   autoset;
} convolution_matrix_config;

#ifndef BIG_MATRIX
static const convolution_matrix_config convolution_matrix_default_config =
{
  {
    { 0.0, 0.0, 0.0, 0.0, 0.0 },
    { 0.0, 0.0, 0.0, 0.0, 0.0 },
    { 0.0, 0.0, 1.0, 0.0, 0.0 },
    { 0.0, 0.0, 0.0, 0.0, 0.0 },
    { 0.0, 0.0, 0.0, 0.0, 0.0 }
  },                 /* matrix */
  1,                 /* divisor */
  0,                 /* offset */
  1,                 /* Alpha-handling algorithm */
  CLEAR,             /* border-mode */
  { TRUE, TRUE, TRUE, TRUE, TRUE }, /* Channels mask */
  FALSE,              /* autoset */
};

#else

static const convolution_matrix_config convolution_matrix_default_config =
{
  {
    { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0  },
    { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0  },
    { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0  },
    { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0  },
    { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0  },
    { 0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0  },
    { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0  },
    { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0  },
    { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0  },
    { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0  },
    { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0  }
  },                 /* matrix */
  1,                 /* divisor */
  0,                 /* offset */
  1,                 /* Alpha-handling algorithm */
  CLEAR,             /* border-mode */
  { TRUE, TRUE, TRUE, TRUE, TRUE }, /* Channels mask */
  FALSE,              /* autoset */
};

#endif

void operation_convolution_matrix(void * in_buf, void * out_buf, long width, long height, convolution_matrix_config * config);