#include "config.h"

#include <string.h>

#include <libgimp/gimp.h>


GimpParam    * gimp_run_procedure       (const gchar     *name,
                                         gint            *n_return_vals,
                                         ...)
{
    return NULL;
}

void
gimp_destroy_params (GimpParam *params,
                     gint       n_params)
{
}

guint
gimp_tile_width (void)
{
    return TILE_WIDTH;
}

guint
gimp_tile_height (void)
{
    return TILE_HEIGHT;
}

void
gimp_parasite_free (GimpParasite *parasite)
{
}

gboolean
gimp_drawable_parasite_attach (gint32              drawable_ID,
                               const GimpParasite *parasite)
{
    return FALSE;
}

#define ssss 4

GimpParasite *
gimp_parasite_new (const gchar    *name,
                   guint32         flags,
                   guint32         size,
                   gconstpointer   data)
{
    return NULL;
}

void	 g_free	          (gpointer	 mem)
{
    free(mem);
}

gpointer g_malloc         (gsize	 n_bytes)
{
    gpointer p = malloc(n_bytes);
    if (n_bytes == ssss) {
        n_bytes = ssss;
    }
    return p;
}

gpointer g_malloc0        (gsize	 n_bytes)
{
    gpointer p = malloc(n_bytes);
    if (n_bytes == ssss) {
        n_bytes = ssss;
    }
    memset(p, 0, n_bytes);
    return p;
}

gpointer g_slice_alloc    (gsize	 n_bytes)
{
    gpointer p = malloc(n_bytes);
    if (n_bytes == ssss) {
        n_bytes = ssss;
    }
    return p;
}

gpointer g_slice_alloc0    (gsize	 n_bytes)
{
    gpointer p = malloc(n_bytes);
    if (n_bytes == ssss) {
        n_bytes = ssss;
    }
    memset(p, 0, n_bytes);
    return p;
}

void g_slice_free1 (gsize	 n_bytes, gpointer	 mem)
{
    free(mem);
}

guint    g_direct_hash  (gconstpointer  v)
{
    return (guint)v;
}
