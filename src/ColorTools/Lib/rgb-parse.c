
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "colortypes.h"

#include "colorspace.h"
#include "rgb.h"

#if WIN32
#define strcasecmp stricmp
#endif

static char    * rgb_parse_strip         (const char *str,
                                                int         len);

static bool   rgb_parse_name_internal (RGB     *rgb,
                                                const char *name);
static bool   rgb_parse_hex_internal  (RGB     *rgb,
                                                const char *hex);
static bool   rgb_parse_css_internal  (RGB     *rgb,
                                                const char *css);
static bool   rgba_parse_css_internal (RGB     *rgb,
                                                const char *css);


typedef struct
{
  const char  *name;
  const unsigned char  red;
  const unsigned char  green;
  const unsigned char  blue;
} ColorEntry;

static const ColorEntry named_colors[] =
{
  { "aliceblue",             240, 248, 255 },
  { "antiquewhite",          250, 235, 215 },
  { "aqua",                    0, 255, 255 },
  { "aquamarine",            127, 255, 212 },
  { "azure",                 240, 255, 255 },
  { "beige",                 245, 245, 220 },
  { "bisque",                255, 228, 196 },
  { "black",                   0,   0,   0 },
  { "blanchedalmond",        255, 235, 205 },
  { "blue",                    0,   0, 255 },
  { "blueviolet",            138,  43, 226 },
  { "brown",                 165,  42,  42 },
  { "burlywood",             222, 184, 135 },
  { "cadetblue",              95, 158, 160 },
  { "chartreuse",            127, 255,   0 },
  { "chocolate",             210, 105,  30 },
  { "coral",                 255, 127,  80 },
  { "cornflowerblue",        100, 149, 237 },
  { "cornsilk",              255, 248, 220 },
  { "crimson",               220,  20,  60 },
  { "cyan",                    0, 255, 255 },
  { "darkblue",                0,   0, 139 },
  { "darkcyan",                0, 139, 139 },
  { "darkgoldenrod",         184, 132,  11 },
  { "darkgray",              169, 169, 169 },
  { "darkgreen",               0, 100,   0 },
  { "darkgrey",              169, 169, 169 },
  { "darkkhaki",             189, 183, 107 },
  { "darkmagenta",           139,   0, 139 },
  { "darkolivegreen",         85, 107,  47 },
  { "darkorange",            255, 140,   0 },
  { "darkorchid",            153,  50, 204 },
  { "darkred",               139,   0,   0 },
  { "darksalmon",            233, 150, 122 },
  { "darkseagreen",          143, 188, 143 },
  { "darkslateblue",          72,  61, 139 },
  { "darkslategray",          47,  79,  79 },
  { "darkslategrey",          47,  79,  79 },
  { "darkturquoise",           0, 206, 209 },
  { "darkviolet",            148,   0, 211 },
  { "deeppink",              255,  20, 147 },
  { "deepskyblue",             0, 191, 255 },
  { "dimgray",               105, 105, 105 },
  { "dimgrey",               105, 105, 105 },
  { "dodgerblue",             30, 144, 255 },
  { "firebrick",             178,  34,  34 },
  { "floralwhite" ,          255, 255, 240 },
  { "forestgreen",            34, 139,  34 },
  { "fuchsia",               255,   0, 255 },
  { "gainsboro",             220, 220, 220 },
  { "ghostwhite",            248, 248, 255 },
  { "gold",                  255, 215,   0 },
  { "goldenrod",             218, 165,  32 },
  { "gray",                  128, 128, 128 },
  { "green",                   0, 128,   0 },
  { "greenyellow",           173, 255,  47 },
  { "grey",                  128, 128, 128 },
  { "honeydew",              240, 255, 240 },
  { "hotpink",               255, 105, 180 },
  { "indianred",             205,  92,  92 },
  { "indigo",                 75,   0, 130 },
  { "ivory",                 255, 255, 240 },
  { "khaki",                 240, 230, 140 },
  { "lavender",              230, 230, 250 },
  { "lavenderblush",         255, 240, 245 },
  { "lawngreen",             124, 252,   0 },
  { "lemonchiffon",          255, 250, 205 },
  { "lightblue",             173, 216, 230 },
  { "lightcoral",            240, 128, 128 },
  { "lightcyan",             224, 255, 255 },
  { "lightgoldenrodyellow",  250, 250, 210 },
  { "lightgray",             211, 211, 211 },
  { "lightgreen",            144, 238, 144 },
  { "lightgrey",             211, 211, 211 },
  { "lightpink",             255, 182, 193 },
  { "lightsalmon",           255, 160, 122 },
  { "lightseagreen",          32, 178, 170 },
  { "lightskyblue",          135, 206, 250 },
  { "lightslategray",        119, 136, 153 },
  { "lightslategrey",        119, 136, 153 },
  { "lightsteelblue",        176, 196, 222 },
  { "lightyellow",           255, 255, 224 },
  { "lime",                    0, 255,   0 },
  { "limegreen",              50, 205,  50 },
  { "linen",                 250, 240, 230 },
  { "magenta",               255,   0, 255 },
  { "maroon",                128,   0,   0 },
  { "mediumaquamarine",      102, 205, 170 },
  { "mediumblue",              0,   0, 205 },
  { "mediumorchid",          186,  85, 211 },
  { "mediumpurple",          147, 112, 219 },
  { "mediumseagreen",         60, 179, 113 },
  { "mediumslateblue",       123, 104, 238 },
  { "mediumspringgreen",       0, 250, 154 },
  { "mediumturquoise",        72, 209, 204 },
  { "mediumvioletred",       199,  21, 133 },
  { "midnightblue",           25,  25, 112 },
  { "mintcream",             245, 255, 250 },
  { "mistyrose",             255, 228, 225 },
  { "moccasin",              255, 228, 181 },
  { "navajowhite",           255, 222, 173 },
  { "navy",                    0,   0, 128 },
  { "oldlace",               253, 245, 230 },
  { "olive",                 128, 128,   0 },
  { "olivedrab",             107, 142,  35 },
  { "orange",                255, 165,   0 },
  { "orangered",             255,  69,   0 },
  { "orchid",                218, 112, 214 },
  { "palegoldenrod",         238, 232, 170 },
  { "palegreen",             152, 251, 152 },
  { "paleturquoise",         175, 238, 238 },
  { "palevioletred",         219, 112, 147 },
  { "papayawhip",            255, 239, 213 },
  { "peachpuff",             255, 218, 185 },
  { "peru",                  205, 133,  63 },
  { "pink",                  255, 192, 203 },
  { "plum",                  221, 160, 203 },
  { "powderblue",            176, 224, 230 },
  { "purple",                128,   0, 128 },
  { "red",                   255,   0,   0 },
  { "rosybrown",             188, 143, 143 },
  { "royalblue",              65, 105, 225 },
  { "saddlebrown",           139,  69,  19 },
  { "salmon",                250, 128, 114 },
  { "sandybrown",            244, 164,  96 },
  { "seagreen",               46, 139,  87 },
  { "seashell",              255, 245, 238 },
  { "sienna",                160,  82,  45 },
  { "silver",                192, 192, 192 },
  { "skyblue",               135, 206, 235 },
  { "slateblue",             106,  90, 205 },
  { "slategray",             119, 128, 144 },
  { "slategrey",             119, 128, 144 },
  { "snow",                  255, 255, 250 },
  { "springgreen",             0, 255, 127 },
  { "steelblue",              70, 130, 180 },
  { "tan",                   210, 180, 140 },
  { "teal",                    0, 128, 128 },
  { "thistle",               216, 191, 216 },
  { "tomato",                255,  99,  71 },
  { "turquoise",              64, 224, 208 },
  { "violet",                238, 130, 238 },
  { "wheat",                 245, 222, 179 },
  { "white",                 255, 255, 255 },
  { "whitesmoke",            245, 245, 245 },
  { "yellow",                255, 255,   0 },
  { "yellowgreen",           154, 205,  50 }
};


/**
 * rgb_parse_name:
 * @rgb:  a #RGB struct used to return the parsed color
 * @name: a color name (in UTF-8 encoding)
 * @len:  the length of @name, in bytes. or -1 if @name is nul-terminated
 *
 * Attempts to parse a color name. This function accepts <ulink
 * url="http://www.w3.org/TR/SVG/types.html">SVG 1.0 color keywords</ulink>.
 *
 * This function does not touch the alpha component of @rgb.
 *
 * Return value: %TRUE if @name was parsed successfully and @rgb has
 *               been set, %FALSE otherwise
 *
 * Since:  2.2
 **/
bool
rgb_parse_name (RGB     *rgb,
                     const char *name,
                     int         len)
{
  char    *tmp;
  bool  result;
    
  tmp = rgb_parse_strip (name, len);

  result = rgb_parse_name_internal (rgb, tmp);

  free (tmp);

  return result;
}

/**
 * rgb_parse_hex:
 * @rgb: a #RGB struct used to return the parsed color
 * @hex: a string describing a color in hexadecimal notation
 * @len: the length of @hex, in bytes. or -1 if @hex is nul-terminated
 *
 * Attempts to parse a string describing an RGB color in hexadecimal
 * notation (optionally prefixed with a '#').
 *
 * This function does not touch the alpha component of @rgb.
 *
 * Return value: %TRUE if @hex was parsed successfully and @rgb has
 *               been set, %FALSE otherwise
 *
 * Since:  2.2
 **/
bool
rgb_parse_hex (RGB     *rgb,
                    const char *hex,
                    int         len)
{
  char    *tmp;
  bool  result;

  tmp = rgb_parse_strip (hex, len);

  result = rgb_parse_hex_internal (rgb, tmp);

  free (tmp);

  return result;
}

/**
 * rgb_parse_css:
 * @rgb: a #RGB struct used to return the parsed color
 * @css: a string describing a color in CSS notation
 * @len: the length of @hex, in bytes. or -1 if @hex is nul-terminated
 *
 * Attempts to parse a string describing an RGB color in CSS
 * notation. This can be either a numerical representation
 * (<code>rgb(255,0,0)</code> or <code>rgb(100%,0%,0%)</code>)
 * or a hexadecimal notation as parsed by rgb_parse_hex()
 * (<code>##ff0000</code>) or a color name as parsed by
 * rgb_parse_name() (<code>red</code>).
 *
 * This function does not touch the alpha component of @rgb.
 *
 * Return value: %TRUE if @css was parsed successfully and @rgb has been
 *               set, %FALSE otherwise
 *
 * Since:  2.2
 **/
bool
rgb_parse_css (RGB     *rgb,
                    const char *css,
                    int         len)
{
  char    *tmp;
  bool  result;

  tmp = rgb_parse_strip (css, len);

  result = rgb_parse_css_internal (rgb, tmp);

  free (tmp);

  return result;
}

/**
 * rgba_parse_css:
 * @rgba: a #RGB struct used to return the parsed color
 * @css: a string describing a color in CSS notation
 * @len: the length of @hex, in bytes. or -1 if @hex is nul-terminated
 *
 * Similar to rgb_parse_css() but handles RGB colors with alpha
 * channel in the numerical CSS notation (<code>rgba(255,0,0,255)</code>
 * or <code>rgba(100%,0%,0%,1000%)</code>).
 *
 * It doesn't handle the hexadecimal notation or color names because
 * they leave the alpha channel unspecified.
 *
 * Return value: %TRUE if @css was parsed successfully and @rgb has been
 *               set, %FALSE otherwise
 *
 * Since:  2.2
 **/
bool
rgba_parse_css (RGB     *rgba,
                     const char *css,
                     int         len)
{
  char    *tmp;
  bool  result;

  if (len < 0)
    len = strlen (css);

  tmp = rgb_parse_strip (css, len);

  if (strcmp (tmp, "transparent") == 0)
    {
      rgba_set (rgba, 0.0, 0.0, 0.0, 0.0);
      result = TRUE;
    }
  else
    {
      result = rgba_parse_css_internal (rgba, tmp);
    }

  free (tmp);

  return result;
}


/**
 * rgb_list_names:
 * @names:  return location for an array of color names
 * @colors: return location for an array of RGB structs
 *
 * Allows to retrieve the list of <ulink
 * url="http://www.w3.org/TR/SVG/types.html">SVG 1.0 color
 * keywords</ulink> that is used by rgb_parse_name().
 *
 * The returned strings are const and must not be freed. Only the two
 * arrays are allocated dynamically. You must call g_free() on the
 * @names and @colors arrays when they are not any longer needed.
 *
 * Return value: the number of named colors
 *               (i.e. the length of the returned arrays)
 *
 * Since:  2.2
 **/
int
rgb_list_names (const char ***names,
                     RGB      **colors)
{
  int i;
    
    int n = sizeof(named_colors) / sizeof(named_colors[0]);

  *names  = malloc(sizeof (const char *) * n);
  *colors = malloc(sizeof(RGB) * n);

  for (i = 0; i <  n; i++)
    {
      (*names)[i] = named_colors[i].name;

      rgba_set_uchar ((*colors) + i,
                           named_colors[i].red,
                           named_colors[i].green,
                           named_colors[i].blue,
                           0xFF);
    }

  return n;
}


static char *
rgb_parse_strip (const char *str,
                      int         len)
{
  char *result;

  while (len > 0 && isspace (*str))
    {
      str++;
      len--;
    }

  if (len < 0)
    {
      while (isspace (*str))
        str++;

      len = strlen (str);
    }

  while (len > 0 && isspace (str[len - 1]))
    len--;

  result = malloc (len + 1);

  memcpy (result, str, len);
  result[len] = '\0';

  return result;
}

static int
rgb_color_entry_compare (const void * a,
                              const void * b)
{
  const char      *name  = a;
  const ColorEntry *entry = b;

  return strcasecmp (name, entry->name);
}

static bool
rgb_parse_name_internal (RGB     *rgb,
                              const char *name)
{
  ColorEntry *entry = bsearch (name, named_colors,
                               sizeof(named_colors) / sizeof(named_colors[0]), sizeof (ColorEntry),
                               rgb_color_entry_compare);

  if (entry)
    {
      rgb_set_uchar (rgb, entry->red, entry->green, entry->blue);

      return TRUE;
    }

  return FALSE;
}


static bool
rgb_parse_hex_component (const char *hex,
                              int         len,
                              double     *value)
{
  int  i;
  unsigned int c = 0;

  for (i = 0; i < len; i++, hex++)
    {
      if (!*hex || !isxdigit (*hex))
        return FALSE;

        c = (c << 4) | (*hex <='9' ? *hex - '0' : (*hex <= 'F' ?  *hex - 'A' : *hex - 'a'));
    }

  switch (len)
    {
    case 1: *value = (double) c /    15.0;  break;
    case 2: *value = (double) c /   255.0;  break;
    case 3: *value = (double) c /  4095.0;  break;
    case 4: *value = (double) c / 65535.0;  break;
    default:
      return (FALSE);
    }

  return TRUE;
}

static bool
rgb_parse_hex_internal (RGB     *rgb,
                             const char *hex)
{
  int     i;
  size_t    len;
  double  val[3];

  if (hex[0] == '#')
    hex++;

  len = strlen (hex);
  if (len % 3 || len < 3 || len > 12)
    return FALSE;

  len /= 3;

  for (i = 0; i < 3; i++, hex += len)
    {
      if (! rgb_parse_hex_component (hex, len, val + i))
        return FALSE;
    }

  rgb_set (rgb, val[0], val[1], val[2]);

  return TRUE;
}


static bool
rgb_parse_css_numeric (RGB     *rgb,
                            const char *css)
{
  double   values[4];
  bool  alpha;
  bool  hsl;
  int      i;

  if (css[0] == 'r' && css[1] == 'g' && css[2] == 'b')
    hsl = FALSE;
  else if (css[0] == 'h' && css[1] == 's' && css[2] == 'l')
    hsl = TRUE;
  else
    return FALSE;

  if (css[3] == 'a' && css[4] == '(')
    alpha = TRUE;
else if (css[3] == '(')
    alpha = FALSE;
  else
    return FALSE;

  css += (alpha ? 5 : 4);

  for (i = 0; i < (alpha ? 4 : 3); i++)
    {
      const char *end = css;

      while (*end && *end != ',' && *end != '%' && *end != ')')
        end++;

      if (i == 3 || *end == '%')
        {
          values[i] = strtod (css, (char **) &end);

          if (errno == ERANGE)
            return FALSE;

          if (*end == '%')
            {
              end++;
              values[i] /= 100.0;
            }
        }
      else
        {
          long  value = strtol (css, (char **) &end, 10);

          if (errno == ERANGE)
            return FALSE;

          if (hsl)
            values[i] = value / (i == 0 ? 360.0 : 100.0);
          else
            values[i] = value / 255.0;
        }

      while (*end == ',' || isspace (*end))
        end++;

      css = end;
    }

  if (*css != ')')
    return FALSE;

  if (alpha)
    rgba_set (rgb, values[0], values[1], values[2], values[3]);
  else
    rgb_set (rgb, values[0], values[1], values[2]);

  rgb_clamp (rgb);

  if (hsl)
    {
      HSL  tmp = (*((HSL *) rgb));

      hsl_to_rgb (&tmp, rgb);
    }

  return TRUE;
}

static bool
rgb_parse_css_internal (RGB     *rgb,
                             const char *css)
{
  if (css[0] == '#')
    {
      return rgb_parse_hex_internal (rgb, css);
    }
  else if (strncmp (css, "rgb(", 4) == 0 ||
           strncmp (css, "hsl(", 4) == 0)
    {
      return rgb_parse_css_numeric (rgb, css);
    }
  else
    {
      return rgb_parse_name_internal (rgb, css);
    }
}

static bool
rgba_parse_css_internal (RGB     *rgba,
                              const char *css)
{
  if (strncmp (css, "rgba(", 5) != 0 &&
      strncmp (css, "hsla(", 5) != 0)
    return FALSE;

  return rgb_parse_css_numeric (rgba, css);
}
