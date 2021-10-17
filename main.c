#include <gtk/gtk.h>
#include "litos.h"

extern void activate (GtkApplication* app, gpointer user_data);

int
main (int    argc,
      char **argv)
{
	int status;

	struct lit litos;

	int i;

	for (i = 0; i < 10; i++)
		litos.changed[i] = FALSE;

	litos.app = gtk_application_new ("org.litos.gtk", G_APPLICATION_FLAGS_NONE);

	g_signal_connect (litos.app, "activate", G_CALLBACK (activate), &litos);

	status = g_application_run (G_APPLICATION (litos.app), argc, argv);

	g_object_unref (litos.app);

	return status;
}
