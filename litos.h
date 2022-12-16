#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <gtksourceview/gtksource.h>

#define VERSION "3.0"
#define MAX_TAB 20

struct lit{
	GtkWidget *window;
	GtkNotebook *notebook;
	GtkWidget *headbar;
	GtkSourceBuffer *buffer;				/* to highlight words searched */
	GtkSourceSearchContext *search_context; /* to highlight mispelled words through aspell */
	GPtrArray *search_context2;
	GtkApplication *app;
	_Bool fileSaved[MAX_TAB];
	_Bool isTemplate;
	char *filename[MAX_TAB];
	int page;
};
