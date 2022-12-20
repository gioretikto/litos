#include <gtk/gtk.h>
#include "litos.h"

extern void activate_cb (GtkApplication* app, gpointer user_data);
void open_file (GFile *file, gpointer userData, gboolean template);

GtkSourceBuffer *highlightSearchBuffer;

static void
app_open_cb (GApplication	*app,
		GFile	**files,
		gint	n_files,
		gchar*	hint,
		gpointer	userData)
{
	(void) hint;

	int i;

	struct lit *litos = (struct lit*)userData;

	activate_cb (GTK_APPLICATION (app), litos);

	for (i = 0; i < n_files; i++)
		open_file (files[i], litos, FALSE);
}

int main (int    argc,
      char **argv)
{
	int status;

	struct lit litos;

	litos.window = NULL;

	GtkApplication* app;
	highlightSearchBuffer = NULL;

	int i;

	for (i = 0; i < MAX_TAB; i++)
	{
		litos.fileSaved[i] = TRUE;
		litos.filename[i] = NULL;
		litos.isTemplate = FALSE;
	}

	litos.page = 0;

	litos.search_context = NULL;

	litos.search_context2 = g_ptr_array_new_with_free_func(g_object_unref);

	app = gtk_application_new ("org.litos.gtk", G_APPLICATION_HANDLES_OPEN);

	g_signal_connect (app, "activate", G_CALLBACK (activate_cb), &litos);

	g_signal_connect (app, "open", G_CALLBACK (app_open_cb), &litos);

	status = g_application_run (G_APPLICATION (app), argc, argv);

	g_object_unref (app);

	g_ptr_array_unref(litos.search_context2);

	return status;
}
