#ifndef __LITOSPPPREFS_H
#define __LITOSAPPPREFS_H

#include <gtk/gtk.h>
#include "litosappwin.h"


#define LITOS_APP_PREFS_TYPE (litos_app_prefs_get_type ())
G_DECLARE_FINAL_TYPE (LitosAppPrefs, litos_app_prefs, LITOS, APP_PREFS, GtkDialog)


LitosAppPrefs        *litos_app_prefs_new          (LitosAppWindow *win);


#endif /* __LITOSAPPPREFS_H */
