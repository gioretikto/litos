#include <gtk/gtk.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <gtksourceview/gtksource.h>

#define MAX_TAB 10
#define SAVE 2
#define CLOSE 1

struct lit{
	GtkWidget *window;
	GtkNotebook *notebook;
	GtkWidget *headbar;
	GtkSourceBuffer *buffer;
	_Bool fileSaved[MAX_TAB];
	GtkApplication *app;
	char *filename[MAX_TAB];
};

GtkWidget* MyNewSourceview(struct lit *litos);
