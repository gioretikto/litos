#include "litos.h"

void action_save_as_dialog (GSimpleAction *action, GVariant *parameter, void* userData);
void menu_newtab (GtkWidget *widget, gpointer userData);
void menu_save (gpointer userData);
gboolean close_tab (GtkButton *button, gpointer userData);
unsigned int saveornot_before_close(gint page, struct lit *litos);
void open_dialog (GtkWidget *widget, gpointer userData);
void searchWord (GtkButton *button, gpointer userData);
gboolean on_delete_event (GtkWidget *widget, GdkEvent  *event, gpointer userData);
GtkTextBuffer* get_current_buffer(struct lit *litos);
void clearSearchHighlight(GObject *gobject, GParamSpec *pspec, gpointer userData);

extern GtkSourceBuffer *highlightSearchBuffer;

/* Called when Ctrl+B, Ctrl+i, etc is toggled */
void insertChar (GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;

	struct lit *litos = (struct lit*)userData;

	gchar *insertChar;

	g_variant_get (parameter, "s", &insertChar);

	GtkTextBuffer *buffer = get_current_buffer(litos);

	gtk_text_buffer_insert_at_cursor (buffer, insertChar, (gint)strlen(insertChar));

	g_free(insertChar);
}

/* Called when Ctrl+B, Ctrl+i, etc is toggled */
void insertHtmlTags (GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;

	struct lit *litos = (struct lit*)userData;

	char *string = NULL;

	char replaceString[350] = { 0 };

	gchar *tag;

	g_variant_get (parameter, "s", &tag);

	GtkTextIter start_sel, end_sel;

	GtkTextBuffer *buffer = get_current_buffer(litos);

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
		snprintf(replaceString, sizeof(replaceString), "</%c>", tag[1]);
		gtk_text_buffer_insert_at_cursor (buffer, replaceString,(gint)strlen(replaceString));
	}
	g_free(tag);
}

void EscButtonPressed(GSimpleAction *action, GVariant *parameter, gpointer userData)		/* Remove highlights: Called when ESC is pressed */
{
	(void)action;
	(void)parameter;

	unsigned int i;

	struct lit *litos = (struct lit*)userData;

	if (litos->search_context != NULL)
		clearSearchHighlight(G_OBJECT(highlightSearchBuffer), NULL, litos->search_context);

	if (litos->search_context2->len != 0)
	{
		for (i = 0; i < litos->search_context2->len; i++)
			gtk_source_search_context_set_highlight (g_ptr_array_index (litos->search_context2,i),FALSE);

		g_ptr_array_remove_range(litos->search_context2, 0, litos->search_context2->len);
	}
}

void action_search_selection(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; searchWord(NULL, userData);}

void action_save_dialog(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; menu_save(userData);}

void action_new_tab(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; menu_newtab (NULL, userData);}

void action_close_tab(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; close_tab(NULL, userData);}

void action_open_dialog(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; open_dialog(NULL, userData);}

void action_quit_activated(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	(void)parameter;

	gtk_window_close(GTK_WINDOW(((struct lit*)userData)->window)); /* it will trigger "close-request" calling on_delete_event(NULL, NULL, userData); */
}

void set_acels (struct lit *litos)
{
	long unsigned int i;

	/* map actions to callbacks */
	const GActionEntry app_entries[] = {
		{"insert_html", insertHtmlTags, "s", NULL, NULL, {0,0,0}},
		{"insert_char", insertChar, "s", NULL, NULL, {0,0,0}},
		{"new", action_new_tab, NULL, NULL, NULL, {0,0,0}},
		{"esc", EscButtonPressed, NULL, NULL, NULL, {0,0,0}},
		{"open", action_open_dialog, NULL, NULL, NULL, {0,0,0}},
		{"save", action_save_dialog, NULL, NULL, NULL, {0,0,0}},
		{"save_as", action_save_as_dialog, NULL, NULL, NULL, {0,0,0}},
		{"search", action_search_selection, NULL, NULL, NULL, {0,0,0}},
		{"close_tab", action_close_tab, NULL, NULL, NULL, {0,0,0}},
		{"quit", action_quit_activated, NULL, NULL, NULL, {0,0,0}}
	};

	/* define keyboard accelerators*/
	struct {
	  const gchar *action;
	  const gchar *accels[2];
	} action_accels[] = {
	  { "app.new", { "<Control>n", NULL} },
	  { "app.esc", { "Escape", NULL} },
	  { "app.open", { "<Control>o", NULL} },
	  { "app.insert_html(\"<b>%s</b>\")", { "<Control>b", NULL} },
	  { "app.insert_html(\"<i>%s</i>\")", { "<Control>i", NULL} },
	  { "app.insert_html(\"<h2>%s</h2>\")", { "<Control>2", NULL} },
	  { "app.insert_html(\"<h3>%s</h3>\")", { "<Control>3", NULL} },
	  { "app.insert_html('<a href=\"this.html\">%s</a>')", { "<Control>h", NULL} },
	  { "app.insert_html(\"<p>%s</p>\")", { "<Control>p", NULL} },
	  { "app.insert_html(\"<li>%s</li>\")", { "<Control>l", NULL} },
	  { "app.insert_html(\"<sup>%s</sup>\")", { "<Control><Shift>p", NULL} },
	  { "app.insert_html(\"<sub>%s</sub>\")", { "<Control>u", NULL} },
	  { "app.insert_char('<div class=\"eq\">\n<p>this</p>\n</div>\')", { "<Control>g", NULL} },
	  { "app.insert_char(\"−\")", { "<Control>m", NULL} },
	  { "app.insert_char(\"⋅\")", { "<Control>d", NULL} },
	  { "app.insert_char(\"⟶⟼⇒\")", { "<Control>t", NULL} },
	  { "app.insert_char(\"⇌⟵⇐\")", { "<Control>y", NULL} },
	  { "app.insert_char(\"<br>\")", { "<Control>r", NULL} },
	  { "app.insert_char(\"&emsp;■□\")", { "<Control>e", NULL} },
	  { "app.close_tab", { "<Control>w", NULL} },
	  { "app.quit", { "<Control>q", NULL} },
	  { "app.save", { "<Control>s", NULL} },
	  { "app.save_as", { "<Shift><Control>s", NULL} },
	  { "app.search", { "<Control>f", NULL} },
	};

	g_action_map_add_action_entries(G_ACTION_MAP(litos->app), app_entries, G_N_ELEMENTS(app_entries), litos);

	for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
		gtk_application_set_accels_for_action(GTK_APPLICATION(litos->app), action_accels[i].action, action_accels[i].accels);
}
