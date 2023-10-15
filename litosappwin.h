#ifndef __LITOSAPPWIN_H
#define __LITOSAPPWIN_H

#include <gtk/gtk.h>
#include "litosapp.h"

#define LITOS_APP_WINDOW_TYPE (litos_app_window_get_type ())
G_DECLARE_FINAL_TYPE (LitosAppWindow, litos_app_window, LITOS, APP_WINDOW, GtkApplicationWindow)

LitosAppWindow       *litos_app_window_new          (LitosApp *app);

#endif /* __LITOSAPPWIN_H */
