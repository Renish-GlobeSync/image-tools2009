#ifndef __COLOR_TYPES_H__
#define __COLOR_TYPES_H__

typedef struct _RGB  RGB;
typedef struct _HSV  HSV;
typedef struct _HSL  HSL;
typedef struct _CMYK CMYK;

struct _RGB
{
  double r, g, b, a;
};

struct _HSV
{
  double h, s, v, a;
};

struct _HSL
{
  double h, s, l, a;
};

struct _CMYK
{
  double c, m, y, k, a;
};

#define inline

typedef int bool;
#define FALSE 0
#define TRUE 1

#endif  /* __COLOR_TYPES_H__ */
