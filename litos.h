#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <gtksourceview/gtksource.h>

#define VERSION "2.8"
#define MAX_TAB 20

struct lit{
	GtkWidget *window;
	GtkNotebook *notebook;
	GtkWidget *headbar;
	GtkSourceBuffer *buffer;
	GtkSourceSearchContext *search_context;
	GtkApplication *app;
	_Bool fileSaved[MAX_TAB];
	char *filename[MAX_TAB];
	int page;
};
