#include <gtk/gtk.h>
#include "litosapp.h"

int
main (int argc, char *argv[])
{
	return g_application_run (G_APPLICATION (litos_app_new ()), argc, argv);
}
