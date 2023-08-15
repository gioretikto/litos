#include "litos.h"

void action_save_as_dialog (GSimpleAction *action, GVariant *parameter, void* userData);
void menu_newtab (GtkWidget *widget, gpointer userData);
void menu_save (gpointer userData);
gboolean close_tab (GtkButton *button, gpointer userData);
unsigned int saveornot_before_close(gint page, struct lit *litos);
void open_dialog (GtkWidget *widget, gpointer userData);
void searchWord (GtkButton *button, gpointer userData);
void insertChar (struct lit *litos, const char *insertChar);
void clearSearchHighlight(GObject *gobject, GParamSpec *pspec, gpointer userData);
gboolean on_delete_event (GtkWidget *widget, GdkEvent  *event, gpointer userData);
GtkTextBuffer* get_current_buffer(struct lit *litos);

extern GtkSourceBuffer *highlightSearchBuffer;

/* Called when Ctrl+m, Ctrl+l is toggled: insert '−' character */
void insertChar (struct lit *litos, const char *insertChar)
{
	GtkTextBuffer *buffer = get_current_buffer(litos);

	gtk_text_buffer_insert_at_cursor (buffer, insertChar, (gint)strlen(insertChar));
}

/* Called when Ctrl+B, Ctrl+i, etc is toggled */
void insertHtmlTags (struct lit *litos, const char *tag)
{
	char *string = NULL;

	char replaceString[350] = { 0 };

	GtkTextIter start_sel, end_sel;

	GtkTextBuffer *buffer = get_current_buffer(litos);

	if (gtk_text_buffer_get_selection_bounds(buffer, &start_sel, &end_sel))
	{
		string = gtk_text_buffer_get_text (buffer,
							&start_sel,
							&end_sel,
							FALSE);

		snprintf(replaceString, sizeof(replaceString), "<%s>%s</%s>", tag, string, tag);
		gtk_text_buffer_delete (buffer, &start_sel, &end_sel);
		gtk_text_buffer_insert (buffer, &start_sel, replaceString, (gint)strlen(replaceString));
	}

	else
	{
		snprintf(replaceString, sizeof(replaceString), "</%s>", tag);
		gtk_text_buffer_insert_at_cursor (buffer, replaceString,(gint)strlen(replaceString));
	}
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
		for(i = 0; i < litos->search_context2->len; i++)
			gtk_source_search_context_set_highlight (g_ptr_array_index (litos->search_context2,i),FALSE);

		g_ptr_array_remove_range(litos->search_context2, 0, litos->search_context2->len);
	}
}

void action_search_selection(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; searchWord(NULL, userData);}

void action_insert_minus(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "−");}

void action_insert_bond(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "—");}

void action_insert_dot(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "⋅");}

void action_insert_arrow(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "⟶⟼⇒");}

void action_insert_arrow_left(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "⇌⟵⇐");}

void action_apply_bold(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	(void)parameter;
	insertHtmlTags(userData, "b");
}

void action_apply_heading(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	guint32 headvalue;
	char heading[2];
	g_variant_get (parameter, "u", &headvalue);
	snprintf(heading,3, "h%d", headvalue);
	insertHtmlTags(userData, heading);
}

void action_apply_italic(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	(void)parameter;
	insertHtmlTags(userData, "i");
}

void action_insert_br_tag(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "<br>");}

void action_apply_sup_tag(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	(void)parameter;
	insertHtmlTags(userData, "sup");
}

void action_apply_sub_tag(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	(void)parameter;
	insertHtmlTags(userData, "sub");
}

void action_insert_endlist_tag(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "</li>");}

void action_insert_endanchor_tag(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "</a>");}

void action_insert_endparagraph_tag(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "</p>");}

void action_insert_space_tag(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "&emsp;■□");}

void action_insert_div_tag(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "<div class=\"eq\">\n<p>this</p>\n</div>");}

void action_insert_href(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "<a href=\"link.html\">this</a>");}

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
		{"new", action_new_tab, NULL, NULL, NULL, {0,0,0}},
		{"esc", EscButtonPressed, NULL, NULL, NULL, {0,0,0}},
		{"open", action_open_dialog, NULL, NULL, NULL, {0,0,0}},
		{"save", action_save_dialog, NULL, NULL, NULL, {0,0,0}},
		{"save_as", action_save_as_dialog, NULL, NULL, NULL, {0,0,0}},
		{"search", action_search_selection, NULL, NULL, NULL, {0,0,0}},
		{"bold", action_apply_bold, NULL, NULL, NULL, {0,0,0}},
		{"italic", action_apply_italic, NULL, NULL, NULL, {0,0,0}},
		{"h2", action_apply_heading, "u", NULL, NULL, {0,0,0}},
		{"h3", action_apply_heading, "u", NULL, NULL, {0,0,0}},
		{"minus", action_insert_minus, NULL, NULL, NULL, {0,0,0}},
		{"div", action_insert_div_tag, NULL, NULL, NULL, {0,0,0}},
		{"href", action_insert_href, NULL, NULL, NULL, {0,0,0}},
		{"arrow", action_insert_arrow, NULL, NULL, NULL, {0,0,0}},
		{"arrow_left", action_insert_arrow_left, NULL, NULL, NULL, {0,0,0}},
		{"dot", action_insert_dot, NULL, NULL, NULL, {0,0,0}},
		{"list", action_insert_endlist_tag, NULL, NULL, NULL, {0,0,0}},
		{"a", action_insert_endanchor_tag, NULL, NULL, NULL, {0,0,0}},
		{"p", action_insert_endparagraph_tag, NULL, NULL, NULL, {0,0,0}},
		{"bond", action_insert_bond, NULL, NULL, NULL, {0,0,0}},
		{"sup", action_apply_sup_tag, NULL, NULL, NULL, {0,0,0}},
		{"sub", action_apply_sub_tag, NULL, NULL, NULL, {0,0,0}},
		{"br", action_insert_br_tag, NULL, NULL, NULL, {0,0,0}},
		{"space", action_insert_space_tag, NULL, NULL, NULL, {0,0,0}},
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
	  { "app.bold", { "<Control>b", NULL} },
	  { "app.h2(uint32 2)", { "<Control>2", NULL} },
	  { "app.h3(uint32 3)", { "<Control>3", NULL} },
	  { "app.italic", { "<Control>i", NULL} },
	  { "app.div", { "<Control>g", NULL} },
	  { "app.minus", { "<Control>m", NULL} },
	  { "app.dot", { "<Control>d", NULL} },
	  { "app.list", { "<Control>l", NULL} },
	  { "app.a", { "<Control>h", NULL} },
	  { "app.href", { "<Control><Shift>h", NULL} },
	  { "app.p", { "<Control>p", NULL} },
	  { "app.arrow", { "<Control>t", NULL} },
	  { "app.bond", { "<Shift><Control>l", NULL} },
	  { "app.sup", { "<Control><Shift>p", NULL} },
	  { "app.arrow_left", { "<Control>y", NULL} },
	  { "app.sub", { "<Control>u", NULL} },
	  { "app.br", { "<Control>r", NULL} },
	  { "app.space", { "<Control>e", NULL} },
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