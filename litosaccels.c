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
open_cb (GtkWidget *dialog, gint response, gpointer window)
{
	if (response == GTK_RESPONSE_ACCEPT)
	{
		GFile *gfile = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));

		LitosAppWindow *win = LITOS_APP_WINDOW(window);

		if (gfile != NULL)
		{
			char *gfile_name = g_file_get_path(gfile);

			if (!litos_app_check_duplicate(gfile_name,win))
			{
				LitosFile * file = litos_app_window_open(win, gfile);
			}

			g_free(gfile_name);
		}
	}

	gtk_window_destroy (GTK_WINDOW (dialog));
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
	GtkWidget *dialog;

	GtkWindow *window = gtk_application_get_active_window (GTK_APPLICATION (app));
	LitosAppWindow *win = LITOS_APP_WINDOW(window);

	dialog = gtk_file_chooser_dialog_new ("Open File",
		NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN,
		"Cancel",
		GTK_RESPONSE_CANCEL,
		"Open",
		GTK_RESPONSE_ACCEPT,
		NULL);

	if (litos_app_window_get_array_len(win) != 0)
	{
		GError *error = NULL;
		LitosFile *file = litos_app_window_current_file(win);

		if (litos_file_get_gfile(file) != NULL)
		{
			if (!gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), g_file_get_parent(litos_file_get_gfile(file)), &error))
				litos_app_error_dialog(GTK_WINDOW(win), error, "Current Folder");
		}
	}

	gtk_window_set_transient_for(GTK_WINDOW(dialog), window);

	gtk_widget_show(dialog);

	g_signal_connect (dialog, "response", G_CALLBACK (open_cb), window);
}

static void
open_tmpl_cb (GtkWidget *dialog, gint response, gpointer window)
{
	if (response == GTK_RESPONSE_ACCEPT)
	{
		GFile *gfile = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));

		LitosAppWindow *win = LITOS_APP_WINDOW(window);

		if (gfile != NULL)
		{
			LitosFile * file = litos_app_window_open(win, gfile);
			litos_file_reset_gfile(file);
		}
	}

	gtk_window_destroy (GTK_WINDOW (dialog));
}

static void
open_tmpl (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app)
{
	GtkWidget *dialog;

	GError *error = NULL;

	GFile *gfile = g_file_new_for_path(g_get_user_special_dir (G_USER_DIRECTORY_TEMPLATES));

	GtkWindow *win = gtk_application_get_active_window (GTK_APPLICATION (app));

	dialog = gtk_file_chooser_dialog_new ("Open File",
		NULL,
		GTK_FILE_CHOOSER_ACTION_OPEN,
		"Cancel",
		GTK_RESPONSE_CANCEL,
		"Open",
		GTK_RESPONSE_ACCEPT,
		NULL);

	if (!gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), gfile, &error))
		litos_app_error_dialog(GTK_WINDOW(win), error, "Templates");

	gtk_window_set_transient_for(GTK_WINDOW(dialog), win);

	gtk_widget_show(dialog);

	g_signal_connect (dialog, "response", G_CALLBACK (open_tmpl_cb), win);
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

/* Called when Ctrl+B, Ctrl+i, etc is toggled */
static void
insertChar (GSimpleAction *action, GVariant *parameter, gpointer app)
{
	GtkWindow *window = gtk_application_get_active_window (GTK_APPLICATION (app));

	LitosAppWindow *win = LITOS_APP_WINDOW(window);

	LitosFile *file = litos_app_window_current_file(win);

	GtkTextBuffer *buffer = litos_file_get_buffer(file);

	gchar *insertChar;

	g_variant_get (parameter, "s", &insertChar);

	gtk_text_buffer_insert_at_cursor (buffer, insertChar, (gint)strlen(insertChar));

	g_free(insertChar);
}

/* Called when Ctrl+B, Ctrl+i, etc is toggled */
static void
insertHtmlTags (GSimpleAction *action, GVariant *parameter, gpointer app)
{
	GtkWindow *window = gtk_application_get_active_window (GTK_APPLICATION (app));

	LitosAppWindow *win = LITOS_APP_WINDOW(window);

	LitosFile *file = litos_app_window_current_file(win);

	GtkTextBuffer *buffer = litos_file_get_buffer(file);

	char *string = NULL;

	char replaceString[650] = { 0 };

	gchar *tag;

	g_variant_get (parameter, "s", &tag);

	GtkTextIter start_sel, end_sel;

	if (gtk_text_buffer_get_selection_bounds(buffer, &start_sel, &end_sel))
	{
		string = gtk_text_buffer_get_text (buffer,
							&start_sel,
							&end_sel,
							FALSE);

		snprintf(replaceString, sizeof(replaceString), tag, string);
		gtk_text_buffer_delete (buffer, &start_sel, &end_sel);
		gtk_text_buffer_insert (buffer, &start_sel, replaceString, (gint)strlen(replaceString));
	}

	else
	{
		snprintf(replaceString, sizeof(replaceString), "</%c>", tag[1]);	/*e.g. enters </l> if Ctrl-l is pressed*/
		gtk_text_buffer_insert_at_cursor (buffer, replaceString,(gint)strlen(replaceString));
	}

	g_free(tag);
}

static void
find_selection (GSimpleAction *action, GVariant *parameter, gpointer app)
{
	GtkWindow *window = gtk_application_get_active_window (GTK_APPLICATION (app));

	LitosAppWindow *win = LITOS_APP_WINDOW(window);	

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
		{ "app.insert_char('<div class=\"eq\">\n<p>this</p>\n</div>\')", { "<Control>g", NULL} },
		{ "app.insert_char(\"−\")", { "<Control>m", NULL} },
		{ "app.insert_char(\"⋅\")", { "<Control>d", NULL} },
		{ "app.insert_char(\"⟶⟼⇒\")", { "<Control>y", NULL} },
		{ "app.insert_char(\"⇌⟵⇐\")", { "<Control><Shift>y", NULL} },
		{ "app.insert_char(\"<br>\")", { "<Control>r", NULL} },
		{ "app.insert_char(\"&emsp;■□\")", { "<Control>t", NULL} },
	};

	g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);

	for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
		gtk_application_set_accels_for_action(GTK_APPLICATION(app), action_accels[i].action, action_accels[i].accels);
}
