/*	Copyright (C) 2023-2024 Giovanni Resta <giovannirestadev@gmail.com>

	This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtk/gtk.h>

#include "litosapp.h"
#include "litosappprefs.h"
#include "litosfile.h"

gboolean litos_file_load (GFile *file, GtkTextBuffer *buffer, GError **error);
void litos_file_highlight_buffer(LitosFile *file);
GtkTextBuffer *litos_file_get_buffer(LitosFile *file);
GFile *litos_file_get_gfile(LitosFile* file);
void litos_file_reset_gfile(LitosFile *file);
gchar *litos_file_get_name(LitosFile *file);

gboolean litos_app_window_remove_child(LitosAppWindow *win);
void litos_app_window_save(LitosAppWindow *win, LitosFile *file);
void litos_app_window_save_as(LitosAppWindow *app);
LitosFile * litos_app_window_open(LitosAppWindow *win, GFile *gf);
LitosFile * litos_app_window_current_file(LitosAppWindow *win);
LitosFile * litos_app_window_new_file(LitosAppWindow *win);
guint litos_app_window_get_array_len(LitosAppWindow *win);
gboolean litos_app_window_quit (GtkWindow *window, gpointer user_data);
void ctrl_f(LitosAppWindow *win);
void Esc(LitosAppWindow *win);

void litos_app_error_dialog(GtkWindow *window, GError *error, char *filename);

gboolean litos_app_check_duplicate(char *filename, LitosAppWindow *win);
void set_search_entry(LitosAppWindow *win);

static void
file_dialog_open_cb(GObject *source_object, GAsyncResult *res, gpointer user_data)
{
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source_object);
    GFile *gfile = gtk_file_dialog_open_finish(dialog, res, NULL);
    LitosAppWindow *win = LITOS_APP_WINDOW(user_data);

    if (gfile != NULL)
    {
        char *gfile_name = g_file_get_path(gfile);

        if (gfile_name && !litos_app_check_duplicate(gfile_name, win))
        {
            litos_app_window_open(win, gfile);
        }

        g_free(gfile_name);
        g_object_unref(gfile);
    }

    g_object_unref(dialog);
}

static void
open_cb(GtkWidget *widget, gpointer user_data)
{
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_open(dialog, GTK_WINDOW(user_data), NULL, file_dialog_open_cb, user_data);
}


static void
esc_activated(GSimpleAction *action, GVariant *parameter, gpointer app)
{
	GtkWindow *window = gtk_application_get_active_window (GTK_APPLICATION (app));
	LitosAppWindow *win = LITOS_APP_WINDOW(window);
	Esc(win);
}

static void
open_activated(GSimpleAction *action, GVariant *parameter, gpointer app)
{
    GtkWindow *window = gtk_application_get_active_window(GTK_APPLICATION(app));
    LitosAppWindow *win = LITOS_APP_WINDOW(window);

    GtkFileDialog *dialog = gtk_file_dialog_new();

    // Imposta la cartella iniziale se disponibile
    if (litos_app_window_get_array_len(win) != 0)
    {
        LitosFile *file = litos_app_window_current_file(win);
        GFile *current_gfile = litos_file_get_gfile(file);

        if (current_gfile != NULL)
        {
            GFile *parent = g_file_get_parent(current_gfile);
            gtk_file_dialog_set_initial_folder(dialog, parent);
            g_object_unref(parent);
        }
    }

    gtk_file_dialog_open(dialog, window, NULL, file_dialog_open_cb, win);
}


static void
open_tmpl_cb(GObject *source, GAsyncResult *res, gpointer user_data)
{
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
    GFile *gfile = gtk_file_dialog_open_finish(dialog, res, NULL);
    LitosAppWindow *win = LITOS_APP_WINDOW(user_data);

    if (gfile != NULL)
    {
        LitosFile *file = litos_app_window_open(win, gfile);
        litos_file_reset_gfile(file);  // evita sovrascrittura del template
        g_object_unref(gfile);
    }

    g_object_unref(dialog);
}

static void
open_tmpl(GSimpleAction *action, GVariant *parameter, gpointer app)
{
    GtkWindow *win = gtk_application_get_active_window(GTK_APPLICATION(app));
    if (!win) return;

    GtkFileDialog *dialog = gtk_file_dialog_new();

    // Imposta la cartella iniziale su "Templates"
    const gchar *tmpl_path = g_get_user_special_dir(G_USER_DIRECTORY_TEMPLATES);
    if (!tmpl_path) {
        g_warning("Impossibile ottenere la cartella Templates");
        return;
    }

    GFile *tmpl_folder = g_file_new_for_path(tmpl_path);
    gtk_file_dialog_set_initial_folder(dialog, tmpl_folder);
    g_object_unref(tmpl_folder);

    gtk_file_dialog_open(dialog, win, NULL, open_tmpl_cb, win);
}


static void
save(GSimpleAction *action, GVariant *parameter, gpointer app)
{
	GtkWindow *window = gtk_application_get_active_window (GTK_APPLICATION (app));
	LitosAppWindow *win = LITOS_APP_WINDOW(window);

	LitosFile *file = litos_app_window_current_file(win);
	litos_app_window_save(win, file);
}

static void
save_as_dialog (GSimpleAction *action, GVariant *parameter, gpointer app)
{
	GtkWindow *win = gtk_application_get_active_window (GTK_APPLICATION (app));
	litos_app_window_save_as(LITOS_APP_WINDOW(win));
}

static void
preferences_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app)
{
	LitosAppPrefs *prefs;
	GtkWindow *win;

	win = gtk_application_get_active_window (GTK_APPLICATION (app));
	prefs = litos_app_prefs_new (LITOS_APP_WINDOW (win));
	gtk_window_present (GTK_WINDOW (prefs));
}

static void
close_activated (GSimpleAction *action, GVariant *parameter, gpointer app)
{
	GtkWindow *window = gtk_application_get_active_window (GTK_APPLICATION (app));

	LitosAppWindow *win = LITOS_APP_WINDOW(window);

	litos_app_window_remove_child(win);
}

static void
quit_activated (GSimpleAction *action, GVariant *parameter, gpointer app)
{
	GtkWindow *window = gtk_application_get_active_window (GTK_APPLICATION (app));
	LitosAppWindow *win = LITOS_APP_WINDOW(window);

	litos_app_window_quit(window, win);
}

static void
new_file (GSimpleAction *action,
                GVariant *parameter,
                gpointer app)
{
	GtkWindow *window = gtk_application_get_active_window (GTK_APPLICATION (app));
	LitosAppWindow *win = LITOS_APP_WINDOW(window);
	LitosFile * file = litos_app_window_new_file(win);
}

/* Used to inser characters like ⟶⟼⇒ and the like */
static void
insertChar (GSimpleAction *action, GVariant *parameter, gpointer app)
{
    GtkWindow *window = gtk_application_get_active_window (GTK_APPLICATION (app));
    if (!window) return;

    LitosAppWindow *win = LITOS_APP_WINDOW(window);
    if (!win) return;

    LitosFile *file = litos_app_window_current_file(win);
    if (!file) return;

    GtkTextBuffer *buffer = litos_file_get_buffer(file);
    if (!buffer) return;

    gchar *insertChar = NULL;
    g_variant_get(parameter, "s", &insertChar);
    if (!insertChar) return;

    gtk_text_buffer_insert_at_cursor(buffer, insertChar, -1); // -1 = lunghezza automatica

    g_free(insertChar);
}


/* Called when Ctrl+B, Ctrl+i, etc is toggled */
static void
insertHtmlTags (GSimpleAction *action, GVariant *parameter, gpointer app)
{
    GtkWindow *window = gtk_application_get_active_window (GTK_APPLICATION (app));
    if (!window) return;

    LitosAppWindow *win = LITOS_APP_WINDOW(window);
    if (!win) return;

    LitosFile *file = litos_app_window_current_file(win);
    if (!file) return;

    GtkTextBuffer *buffer = litos_file_get_buffer(file);
    if (!buffer) return;

    gchar *tag = NULL;
    g_variant_get(parameter, "s", &tag);
    if (!tag) return;

    GtkTextIter start_sel, end_sel;

    if (gtk_text_buffer_get_selection_bounds(buffer, &start_sel, &end_sel))
    {
        gchar *selected_text = gtk_text_buffer_get_text(buffer, &start_sel, &end_sel, FALSE);
        if (selected_text)
        {
            gchar *wrapped_text = g_strdup_printf(tag, selected_text);
            gtk_text_buffer_delete(buffer, &start_sel, &end_sel);
            gtk_text_buffer_insert(buffer, &start_sel, wrapped_text, -1);
            g_free(wrapped_text);
            g_free(selected_text);
        }
    }
    else
    {
        // Inserisce solo il tag di chiusura, es: </b>
        gchar *closing_tag = g_strdup_printf("</%c>", tag[1]);
        gtk_text_buffer_insert_at_cursor(buffer, closing_tag, -1);
        g_free(closing_tag);
    }

    g_free(tag);
}

static void
find_selection (GSimpleAction *action, GVariant *parameter, gpointer app)
{
	GtkWindow *window = gtk_application_get_active_window (GTK_APPLICATION (app));

	LitosAppWindow *win = LITOS_APP_WINDOW(window);

	if (litos_app_window_get_array_len(win) != 0)
		ctrl_f(win);
}

void setAccels (GApplication *app)
{
	long unsigned int i;

	/* map actions to callbacks */
	const GActionEntry app_entries[] = {
		{"preferences", preferences_activated, NULL, NULL, NULL },
		{"insert_html", insertHtmlTags, "s", NULL, NULL, {0,0,0}},
		{"insert_char", insertChar, "s", NULL, NULL, {0,0,0}},
		{"open", open_activated, NULL, NULL, NULL},
		{"esc", esc_activated, NULL, NULL, NULL},
		{"open_tmpl", open_tmpl, NULL, NULL, NULL},
		{"new", new_file, NULL, NULL, NULL},
		{"save", save, NULL, NULL, NULL, {0,0,0}},
		{"save_as", save_as_dialog, NULL, NULL, NULL, {0,0,0}},
		{"close", close_activated, NULL, NULL, NULL},
		{"find", find_selection, NULL, NULL, NULL, {0,0,0}},
		{"quit", quit_activated, NULL, NULL, NULL }
	};

	/* define keyboard accelerators*/
	struct {
	  const gchar *action;
	  const gchar *accels[2];
	} action_accels[] = {
		{ "app.open", { "<Control>o", NULL} },
		{ "app.new", { "<Control>n", NULL} },
		{ "app.save", { "<Control>s", NULL} },
		{ "app.save_as", { "<Shift><Control>s", NULL} },
		{ "app.close", { "<Control>w", NULL} },
		{ "app.quit", { "<Control>q", NULL} },
		{ "app.esc", { "Escape", NULL} },
		{ "app.find", { "<Control>f", NULL} },
		{ "app.insert_html(\"<b>%s</b>\")", { "<Control>b", NULL} },
		{ "app.insert_html(\"<i>%s</i>\")", { "<Control>i", NULL} },
		{ "app.insert_html(\"<h2>%s</h2>\")", { "<Control>2", NULL} },
		{ "app.insert_html(\"<h3>%s</h3>\")", { "<Control>3", NULL} },
		{ "app.insert_html('<a href=\"this.html\">%s</a>')", { "<Control>h", NULL} },
		{ "app.insert_html(\"<p>%s</p>\")", { "<Control>p", NULL} },
		{ "app.insert_html(\"<li><p>%s</p></li>\")", { "<Control>l", NULL} },
		{ "app.insert_html(\"<sup>%s</sup>\")", { "<Control>e", NULL} },
		{ "app.insert_html(\"<sub>%s</sub>\")", { "<Control>u", NULL} },
		{ "app.insert_html('<div class=\"eq\">\n<p>%s</p>\n</div>\')", { "<Control>g", NULL} },
		{ "app.insert_char('<i>f</i>(<i>x</i>)')", { "<Control>KP_0", NULL} },
		{ "app.insert_char(\"\\\\mathbf{}\")", { "<Control><Shift>m", NULL} },
		{ "app.insert_char(\"\\\\frac{}{}\")", { "<Control><Shift>f", NULL} },
		{ "app.insert_char(\"ℝ\")", { "<Control><Shift>r", NULL} },
		{ "app.insert_char(\"\\\\text{}\")", { "<Control><Shift>t", NULL} },
		{ "app.insert_char(\"−\")", { "<Control>m", NULL} },
		{ "app.insert_char(\"⋅\")", { "<Control>d", NULL} },
		{ "app.insert_char(\"⟶⟼⇒\")", { "<Control>y", NULL} },
		{ "app.insert_char(\"⇌⇔⟵⇐\")", { "<Control><Shift>y", NULL} },
		{ "app.insert_char(\"<br>\")", { "<Control>r", NULL} },
		{ "app.insert_char(\"&emsp;■□\")", { "<Control>t", NULL} },
	};

	g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);

	for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
		gtk_application_set_accels_for_action(GTK_APPLICATION(app), action_accels[i].action, action_accels[i].accels);
}
