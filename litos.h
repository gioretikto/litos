#include <gtk/gtk.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <gtksourceview/gtksource.h>

struct lit{
	GtkWidget *window;
	GtkNotebook *notebook;
	GtkWidget *headbar;
	GtkSourceBuffer *buffer;
	_Bool changed[10];
	GtkApplication *app;	
};

GtkWidget* MyNewSourceview(struct lit *litos);
