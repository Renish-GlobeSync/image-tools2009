#include "config.h"

#include <libgimp/gimp.h>


gboolean      gimp_progress_init            (const gchar              *message)
{
    return TRUE;
}

gboolean      gimp_progress_update          (gdouble                   percentage)
{
    return TRUE;
}

gboolean                 gimp_progress_set_text          (const gchar *message)
{
    return TRUE;
}
