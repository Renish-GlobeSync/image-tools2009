
typedef enum
{
  CURL_COLORS_FG_BG,
  CURL_COLORS_GRADIENT,
  CURL_COLORS_GRADIENT_REVERSE,
  CURL_COLORS_LAST = CURL_COLORS_GRADIENT_REVERSE
} CurlColors;

typedef enum
{
  CURL_ORIENTATION_VERTICAL,
  CURL_ORIENTATION_HORIZONTAL,
  CURL_ORIENTATION_LAST = CURL_ORIENTATION_HORIZONTAL
} CurlOrientation;

typedef enum
{
  CURL_EDGE_LOWER_RIGHT = 1,
  CURL_EDGE_LOWER_LEFT  = 2,
  CURL_EDGE_UPPER_LEFT  = 3,
  CURL_EDGE_UPPER_RIGHT = 4,
  CURL_EDGE_FIRST       = CURL_EDGE_LOWER_RIGHT,
  CURL_EDGE_LAST        = CURL_EDGE_UPPER_RIGHT
} CurlEdge;


#define CURL_EDGE_LEFT(e)  ((e) == CURL_EDGE_LOWER_LEFT  || \
                            (e) == CURL_EDGE_UPPER_LEFT)
#define CURL_EDGE_RIGHT(e) ((e) == CURL_EDGE_LOWER_RIGHT || \
                            (e) == CURL_EDGE_UPPER_RIGHT)
#define CURL_EDGE_LOWER(e) ((e) == CURL_EDGE_LOWER_RIGHT || \
                            (e) == CURL_EDGE_LOWER_LEFT)
#define CURL_EDGE_UPPER(e) ((e) == CURL_EDGE_UPPER_LEFT  || \
                            (e) == CURL_EDGE_UPPER_RIGHT)


/***** Types *****/

typedef struct
{
  CurlColors       colors;
  gdouble          opacity;
  gboolean         shade;
  CurlEdge         edge;
  CurlOrientation  orientation;
} pagecurl_config;

void operation_pagecurl (void * in_buf, void * out_buf, long width, long height, pagecurl_config * config);
