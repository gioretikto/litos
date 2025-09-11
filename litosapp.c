/*	Copyright (C) 2023-2024 Giovanni Resta <giovannirestadev@gmail.com>

	This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

#include "litosapp.h"
#include "litosfile.h"
#include "litosappwin.h"

void litos_accels_setAccels (GApplication *app);
void litos_app_window_open_favorites(LitosAppWindow *win);

struct _LitosApp
{
	GtkApplication parent;

	GSettings *settings;
	GtkCssProvider *css_provider;
	GtkSourceStyleScheme *style_scheme;
};

G_DEFINE_TYPE(LitosApp, litos_app, GTK_TYPE_APPLICATION);

GtkSourceStyleScheme *litos_app_get_style_scheme(LitosApp *app)
{
    return app->style_scheme;
}

static void litos_app_init (LitosApp *app)
{
	app->settings = g_settings_new("org.gtk.litos");
	app->css_provider = gtk_css_provider_new ();
}

GtkCssProvider * litos_app_get_css_provider(LitosApp *app)
{
	return app->css_provider;
}


static void
litos_app_startup (GApplication *app)
{
    litos_accels_setAccels(app);

    G_APPLICATION_CLASS (litos_app_parent_class)->startup (app);

    gtk_source_init();

    g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);

    // Ottieni il riferimento all'app
    LitosApp *litos = LITOS_APP(app);

    // Carica il CSS da risorsa
    GtkCssProvider *provider = litos_app_get_css_provider(litos);
    gtk_css_provider_load_from_resource(provider, "/org/gtk/litos/style.css");

    gtk_style_context_add_provider_for_display(
        gdk_display_get_default(),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    // Carica lo schema di stile "oblivion"
    GtkSourceStyleSchemeManager *scheme_manager = gtk_source_style_scheme_manager_get_default();
    litos->style_scheme = gtk_source_style_scheme_manager_get_scheme(scheme_manager, "oblivion");

    if (!litos->style_scheme)
        g_warning("Schema di stile 'oblivion' non trovato");
}


static void
litos_app_activate (GApplication *app)
{
	LitosAppWindow *win = litos_app_window_new (LITOS_APP (app));

	GtkWindow *window = GTK_WINDOW (win);

	gtk_window_set_title (window, "Litos");
	gtk_window_maximize (window);
	gtk_window_present (window);

	litos_app_window_open_favorites(win);
}

gboolean litos_app_check_duplicate(char *filename, LitosAppWindow *win)
{
	GFile *current_gfile;
	int i, n_pages;

	n_pages = litos_app_window_get_array_len(win);

	if (n_pages == 0)
		return FALSE;

	for (i = 0; i < n_pages; i++)
	{
		current_gfile = litos_file_get_gfile(litos_app_window_get_file(win,&i));

		if (current_gfile != NULL)
		{
			char *current_file_name = g_file_get_path(current_gfile);

			if (strcmp(current_file_name, filename) == 0)
			{
				g_free(current_file_name);
				gtk_notebook_set_current_page(litos_app_window_get_nb(win), i);
				return TRUE;
			}

			g_free(current_file_name);
		}
	}

	return FALSE;
}

static void
litos_app_open (GApplication  *app,
                GFile        **files,
                int            n_files,
                const char    *hint G_GNUC_UNUSED)
{
    GList *windows;
    LitosAppWindow *win;
    int i;

    windows = gtk_application_get_windows (GTK_APPLICATION (app));

    if (windows)
        win = LITOS_APP_WINDOW (windows->data);
    else
        win = litos_app_window_new (LITOS_APP (app));

    // Open file passed as arguments
    for (i = 0; i < n_files; i++) {
        if (files[i] != NULL) {
            char *filename = g_file_get_path(files[i]);

            if (!litos_app_check_duplicate(filename, win)) {
                litos_app_window_open(win, files[i]);
            }

            g_free(filename);
        }
    }

    // Apri i file preferiti salvati in GSettings
    litos_app_window_open_favorites(win);

    gtk_window_set_title(GTK_WINDOW(win), "Litos");
    gtk_window_maximize(GTK_WINDOW(win));
    gtk_window_present(GTK_WINDOW(win));
}


static void
litos_app_class_init (LitosAppClass *class)
{
	G_APPLICATION_CLASS (class)->activate = litos_app_activate;
	G_APPLICATION_CLASS (class)->startup = litos_app_startup;
	G_APPLICATION_CLASS (class)->open = litos_app_open;
}

LitosApp *
litos_app_new (void)
{
	return g_object_new (LITOS_APP_TYPE,
			"application-id", "org.gtk.litos",
			"flags", G_APPLICATION_HANDLES_OPEN,
			NULL);
}

GSettings *litos_app_get_settings(LitosApp *app)
{
	return app->settings;
}
