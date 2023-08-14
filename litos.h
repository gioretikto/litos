#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <gtksourceview/gtksource.h>

#define MAX_TAB 15

struct lit{
	GtkWidget *window;
	GtkNotebook *notebook;
	GtkSourceSearchContext *search_context; 	/* to highlight words searched */
	GPtrArray *search_context2;			/* to highlight mispelled words through aspell */
	GtkApplication *app;
	_Bool fileSaved[MAX_TAB];
	_Bool isTemplate;
	char *filename[MAX_TAB];
	int page;	/* holds current page */
};
