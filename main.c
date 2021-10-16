#include <gtk/gtk.h>
#include "litos.h"

extern void activate (GtkApplication* app, gpointer user_data);

int
main (int    argc,
      char **argv)
{
	GtkApplication *app;
	int status;

	struct lit litos;

	app = gtk_application_new ("org.litos.gtk", G_APPLICATION_FLAGS_NONE);

	g_signal_connect (app, "activate", G_CALLBACK (activate), &litos);

	status = g_application_run (G_APPLICATION (app), argc, argv);

	g_object_unref (app);

	return status;
}
