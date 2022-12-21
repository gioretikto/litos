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
	GtkSourceBuffer *source_buffer;
	GtkSourceSearchContext *search_context; /* to highlight words searched */
	GPtrArray *search_context2;				/* to highlight mispelled words through aspell */
	GtkApplication *app;
	_Bool fileSaved[MAX_TAB];
	_Bool isTemplate;
	char *filename[MAX_TAB];
	int page;
};
