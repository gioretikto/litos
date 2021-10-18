#include <gtk/gtk.h>
#include "litos.h"

extern void activate (GtkApplication* app, gpointer user_data);

int
main (int    argc,
      char **argv)
{

	int status;

	GtkApplication* app;

	struct lit litos;

	int i;

	for (i = 0; i < MAX_TAB; i++)
	{
		litos.changed[i] = FALSE;
		litos.filename[i] = NULL;
	}

	app = gtk_application_new ("org.litos.gtk", G_APPLICATION_FLAGS_NONE);

	g_signal_connect (app, "activate", G_CALLBACK (activate), &litos);

	status = g_application_run (G_APPLICATION (app), argc, argv);

	g_object_unref (app);

	return status;
}
