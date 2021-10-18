#include <string.h>
#include "litos.h"

#define CURRENT_PAGE -2

void open_file(struct lit *litos);
void menu_save (GtkWidget *widget, gpointer userData);
void save_as_dialog(struct lit *litos);
void save_as_file(GtkFileChooser *chooser, struct lit *litos);
void save_file(struct lit *litos);

void close_tab (GtkButton *button, gpointer userData);
void menu_findreplaceall(void);
void menu_newtab (GtkWidget *widget, gpointer userData);
void find_button_clicked ();

unsigned int saveornot_before_close(gint page, struct lit *litos);
gboolean get_notebook_no_pages(struct lit *litos);
gboolean check_equals_unsaved(struct lit *litos);
const gchar* get_current_tab_label_text();
void highlight_buffer(struct lit *litos);
GtkSourceView* tab_get_sourceview(int page, struct lit *litos);
GtkTextBuffer* get_current_buffer(struct lit *litos);


void open_dialog (GtkWidget *widget, gpointer userData);

void action_find_replace(GSimpleAction *action, GVariant *parameter, gpointer user_data) {(void)user_data; (void)action; (void)parameter; find_button_clicked();}
void action_save_dialog(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; menu_save(NULL, userData);}
void action_new_tab(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; menu_newtab (NULL, userData);}
void action_close_tab(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; close_tab(NULL, userData);}
void action_open_dialog(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; open_dialog(NULL, userData);}
void action_save_as_dialog(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; save_as_dialog(userData);}

void action_quit_activated(GSimpleAction *action, GVariant *parameter, void* userData)
{
	(void)action;
	(void)parameter;

	gint i;

	unsigned int res = CLOSE;
	
	struct lit *litos = (struct lit*)userData;

	GtkTextBuffer *current_buffer = get_current_buffer(litos);

	for (i = 0; i < gtk_notebook_get_n_pages(litos->notebook); i++)
	{
 		if (litos->changed[i] == TRUE && (gtk_text_buffer_get_char_count(current_buffer)) != 0)
 		{
			res = saveornot_before_close(i, litos);

		    if (res == CLOSE)
		    	i--;
		}
 	}

	if (res == CLOSE || res == SAVE)
		g_application_quit (G_APPLICATION (litos->app));
}

const GActionEntry app_entries[] = {
    {"new", action_new_tab, NULL, NULL, NULL, {0,0,0}},
	{"open", action_open_dialog, NULL, NULL, NULL, {0,0,0}},
	{"save", action_save_dialog, NULL, NULL, NULL, {0,0,0}},
	{"save_as", action_save_as_dialog, NULL, NULL, NULL, {0,0,0}},
	{"find_replace", action_find_replace, NULL, NULL, NULL, {0,0,0}},
	{"close_tab", action_close_tab, NULL, NULL, NULL, {0,0,0}},
    {"quit", action_quit_activated, NULL, NULL, NULL, {0,0,0}}
};

struct {
  const gchar *action;
  const gchar *accels[2];
} action_accels[] = {
  { "app.new", { "<Control>n", NULL} },
  { "app.open", { "<Control>o", NULL} },
  { "app.close_tab", { "<Control>w", NULL} },
  { "app.quit", { "<Control>q", NULL} },
  { "app.save", { "<Control>s", NULL} },
  { "app.save_as", { "<Shift><Control>s", NULL} },
  { "app.find_replace", { "<Control>r", NULL} },
  { "win.close", { "<Control>w", NULL} },
  { "win.cut", { "<Control>x", NULL} },
  { "win.copy", { "<Control>c", NULL} },
  { "win.paste", { "<Control>v", NULL} },
  { "win.selectall", { "<Control>a", NULL} }
};

void my_grab_focus(struct lit *litos)
{
	gtk_widget_grab_focus(GTK_WIDGET(tab_get_sourceview(CURRENT_PAGE, litos)));
}

void set_acels (GtkApplication *app)
{
	long unsigned int i;
	
	g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), app);

	for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
		gtk_application_set_accels_for_action(GTK_APPLICATION(app), action_accels[i].action, action_accels[i].accels);
}

GtkTextBuffer* get_current_buffer(struct lit *litos)
{
	GtkSourceView *source_view;
	GtkTextBuffer *current_buffer;

	source_view = tab_get_sourceview(CURRENT_PAGE, litos);

  	current_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(source_view));
  	
  	return current_buffer;
}


void close_tab (GtkButton *button, gpointer userData)
{
	(void)button;

	struct lit *litos = (struct lit*)userData;

	GtkTextBuffer *current_buffer = get_current_buffer(litos);
	
    if (get_notebook_no_pages(litos) || (gtk_text_buffer_get_char_count(current_buffer)) == 0)
		return;

	gint page = gtk_notebook_get_current_page(litos->notebook);

	if (litos->changed[page] == TRUE)
		saveornot_before_close(page, litos);

	else
	    gtk_notebook_remove_page(litos->notebook, page);	    
}


void menu_save (GtkWidget *widget, gpointer userData)
{
	(void)widget;

	struct lit *litos = (struct lit*)userData;

    if (get_notebook_no_pages(litos))
		return;

    else if (check_equals_unsaved(litos))
		save_as_dialog(litos);

    else
		save_file(litos);
		
	litos->changed[gtk_notebook_get_current_page(litos->notebook)] = FALSE;

	my_grab_focus(litos);
}

void open_file(struct lit *litos)
{
    gboolean read_file_status;
   	GError *error;
   	char* contents;

    read_file_status = g_file_get_contents(litos->filename, &contents, NULL, &error);

    if (read_file_status == FALSE)
    {
		g_error("error opening file: %s\n",error && error->message ? error->message : "No Detail");
			return;
    }

	GtkTextBuffer *current_buffer = get_current_buffer(litos);

    if ((gtk_text_buffer_get_char_count(current_buffer)) == 0)
    {
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(current_buffer), contents, -1);

		gtk_notebook_set_tab_label_text(
			litos->notebook,
			gtk_notebook_get_nth_page(
		        litos->notebook,
		        gtk_notebook_get_current_page(litos->notebook)
			),
			litos->filename
		);

		gtk_notebook_set_menu_label_text(
			litos->notebook,
			gtk_notebook_get_nth_page(
			litos->notebook,
			gtk_notebook_get_current_page(litos->notebook)
			),
			litos->filename
		);
		
		highlight_buffer(litos);
	}

	else
	{
    	menu_newtab(NULL, litos);
		gtk_text_buffer_set_text(GTK_TEXT_BUFFER(litos->buffer), contents, -1);
	}

	litos->changed[gtk_notebook_get_current_page(litos->notebook)] = FALSE;
}

const gchar* get_current_tab_label_text(struct lit *litos)
{
    return gtk_notebook_get_menu_label_text(
		litos->notebook,
		gtk_notebook_get_nth_page(
	        litos->notebook,
	        gtk_notebook_get_current_page(litos->notebook)
		)
    );
}

GList* get_tabbox_children(GtkNotebook *tabnotebook, const gint page)
{
    return gtk_container_get_children(GTK_CONTAINER(gtk_notebook_get_nth_page(
		tabnotebook,
		page
    )));
}

gboolean check_equals_unsaved(struct lit *litos)
{
    return g_strcmp0(
		get_current_tab_label_text(litos),
		"unsaved"
    ) == 0;
}

GtkSourceView* tab_get_sourceview(int page, struct lit *litos)
{
    if (page == CURRENT_PAGE)
		page = gtk_notebook_get_current_page(litos->notebook);

    GList *children = get_tabbox_children(litos->notebook,page);

    return GTK_SOURCE_VIEW(gtk_bin_get_child(GTK_BIN(g_list_nth_data(children, 0))));
}

gboolean get_notebook_no_pages(struct lit *litos)
{
    return gtk_notebook_get_n_pages(litos->notebook) <= 0;
}

GtkWidget* MyNewSourceview(struct lit *litos)
{
	GtkWidget *source_view;

	litos->buffer = gtk_source_buffer_new (NULL);

	source_view = gtk_source_view_new_with_buffer (litos->buffer);

	gtk_source_buffer_set_style_scheme (litos->buffer,
    gtk_source_style_scheme_manager_get_scheme (
    gtk_source_style_scheme_manager_get_default (), "solarized-dark"));

	gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW((source_view)), TRUE);

	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(source_view), GTK_WRAP_WORD_CHAR);

    return source_view;
}

void save_file(struct lit *litos)
{
	GtkTextIter start, end;

	gchar *content;

	GtkTextView *text_view = GTK_TEXT_VIEW(tab_get_sourceview(CURRENT_PAGE, litos));

	GtkTextBuffer *current_buffer = gtk_text_view_get_buffer (text_view);

	gtk_text_buffer_get_bounds (current_buffer, &start, &end);

	content = gtk_text_buffer_get_text (current_buffer, &start, &end, FALSE);

	if (!g_file_set_contents (litos->filename, content, -1, NULL))
		g_warning ("The file '%s' could not be written!", litos->filename);

	g_free (content);
}

void save_as_file(GtkFileChooser *chooser, struct lit *litos)
{
	GFile *loc = gtk_file_chooser_get_file(chooser);

	GtkTextIter begin, end;

	GtkTextView *text_view = GTK_TEXT_VIEW(tab_get_sourceview(CURRENT_PAGE, litos));

	GtkTextBuffer *current_buffer = gtk_text_view_get_buffer (text_view);

	gtk_text_buffer_get_start_iter(current_buffer, &begin);
	gtk_text_buffer_get_end_iter(current_buffer, &end);

	char* contents = gtk_text_buffer_get_text(current_buffer, &begin, &end, 0);

	g_file_replace_contents(loc, contents, strlen(contents), NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, NULL);

	char *filename;

	filename = gtk_file_chooser_get_filename (chooser);

    gtk_notebook_set_tab_label_text(
    	litos->notebook,
    	gtk_notebook_get_nth_page(
            litos->notebook,
            gtk_notebook_get_current_page(litos->notebook)
		),
		filename
	);

	gtk_notebook_set_menu_label_text(
		litos->notebook,
		gtk_notebook_get_nth_page(
		litos->notebook,
		gtk_notebook_get_current_page(litos->notebook)
		),
		filename
	);

   	g_free (filename);

	g_object_unref(loc);
}

void highlight_buffer(struct lit *litos) /* Apply different font styles depending on file extension .html .c, etc */
{	
	if (strcmp(litos->filename,"Unsaved") != 0)
	{	
		GtkSourceLanguageManager *lm = gtk_source_language_manager_get_default();
	
		GtkSourceLanguage *lang = gtk_source_language_manager_guess_language(lm, litos->filename, NULL);
	
		gtk_source_buffer_set_language (litos->buffer, lang);
		gtk_source_buffer_set_highlight_syntax (litos->buffer, TRUE);
	}
}
