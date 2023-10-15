#ifndef __LITOSAPP_H
#define __LITOSAPP_H

#include <gtk/gtk.h>


#define LITOS_APP_TYPE (litos_app_get_type ())
G_DECLARE_FINAL_TYPE (LitosApp, litos_app, LITOS, APP, GtkApplication)


LitosApp     *litos_app_new         (void);


#endif /* __LITOSAPP_H */
