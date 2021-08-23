#include <gtk/gtk.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <gtksourceview/gtksource.h>

extern GtkWidget *window;
extern GtkWidget *headbar;
extern GtkNotebook *notebook;
extern GtkSourceBuffer *buffer;
GtkWidget* new_sourceview();
extern _Bool changed[10];
