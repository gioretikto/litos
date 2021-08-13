#include <gtk/gtk.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <gtksourceview/gtksource.h>

GtkWidget *window;
GtkWidget *headbar;
GtkNotebook *notebook;
GtkSourceBuffer *buffer;
GtkWidget* new_sourceview();
extern _Bool changed[10];
