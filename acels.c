#include "litos.h"

void menu_newtab (GtkWidget *widget, gpointer userData);
void menu_save (gpointer userData);
gboolean close_tab (GtkButton *button, gpointer userData);
void applyTags(struct lit *litos, char *what_tag);
unsigned int saveornot_before_close(gint page, struct lit *litos);
void open_dialog (GtkWidget *widget, gpointer userData);
void ctrlF (GtkButton *button, gpointer userData);
void insertChar (struct lit *litos, const char *insertChar);
void on_save_as_response(GFile *file, struct lit *litos);
gboolean on_delete_event (GtkWidget *widget, GdkEvent  *event, gpointer userData);

void action_remove_highlight(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	(void)parameter;

	struct lit *litos = (struct lit*)userData;

	if (litos->search_context != NULL)
	{
		gtk_source_search_context_set_highlight
			(litos->search_context,
			FALSE);

		litos->search_context = NULL;
	}
}

void action_find_selection(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; ctrlF(NULL, userData);}

void action_insert_minus(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "−");}

void action_insert_bond(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "—");}

void action_insert_dot(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "⋅");}

void action_insert_arrow(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "⟶⟼⇒");}

void action_insert_arrow_left(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "⇌⟵⇐");}

void action_apply_bold(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	(void)parameter;
	applyTags(userData, "b");
}

void action_apply_heading(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	(void)parameter;
	applyTags(userData, "h2");
}

void action_apply_italic(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	(void)parameter;
	applyTags(userData, "i");
}

void action_apply_br_tag(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	(void)parameter;
	applyTags(userData, "br");
}

void action_apply_sup_tag(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	(void)parameter;
	applyTags(userData, "sup");
}

void action_apply_sub_tag(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	(void)parameter;
	applyTags(userData, "sub");
}

void action_insert_endlist_tag(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "</li>");}

void action_insert_space_tag(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "&emsp;■□");}

void action_insert_div_tag(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; insertChar(userData, "<div class=\"eq\">\n<p>\n</div>");}

void action_save_dialog(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; menu_save(userData);}

void action_new_tab(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; menu_newtab (NULL, userData);}

void action_close_tab(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; close_tab(NULL, userData);}

void action_open_dialog(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; open_dialog(NULL, userData);}

void action_quit_activated(GSimpleAction *action, GVariant *parameter, void* userData)
{
	(void)action;
	(void)parameter;

	on_delete_event(NULL, NULL, userData);
}

void action_save_as_dialog (GSimpleAction *action, GVariant *parameter, void* userData)
{
	(void)action;
	(void)parameter;

	struct lit *litos = (struct lit*)userData;

	GtkWidget *dialog = gtk_file_chooser_dialog_new ("Save File",
		                                  GTK_WINDOW(litos->window),
		                                  GTK_FILE_CHOOSER_ACTION_SAVE,
		                                  _("_Cancel"),
		                                  GTK_RESPONSE_CANCEL,
		                                  _("_Save"),
		                                  GTK_RESPONSE_ACCEPT,
		                                  NULL);

	GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);

	gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

	gint res = gtk_dialog_run (GTK_DIALOG (dialog));

	if (res == GTK_RESPONSE_ACCEPT)
	{
		g_autoptr (GFile) file = gtk_file_chooser_get_file(chooser);
		on_save_as_response (file, litos);
	}

	gtk_widget_destroy (dialog);
}

void set_acels (struct lit *litos)
{
	long unsigned int i;

	const GActionEntry app_entries[] = {
		{"new", action_new_tab, NULL, NULL, NULL, {0,0,0}},
		{"esc", action_remove_highlight, NULL, NULL, NULL, {0,0,0}},
		{"open", action_open_dialog, NULL, NULL, NULL, {0,0,0}},
		{"save", action_save_dialog, NULL, NULL, NULL, {0,0,0}},
		{"save_as", action_save_as_dialog, NULL, NULL, NULL, {0,0,0}},
		{"find_selection", action_find_selection, NULL, NULL, NULL, {0,0,0}},
		{"bold", action_apply_bold, NULL, NULL, NULL, {0,0,0}},
		{"italic", action_apply_italic, NULL, NULL, NULL, {0,0,0}},
		{"h2", action_apply_heading, NULL, NULL, NULL, {0,0,0}},
		{"minus", action_insert_minus, NULL, NULL, NULL, {0,0,0}},
		{"div", action_insert_div_tag, NULL, NULL, NULL, {0,0,0}},
		{"arrow", action_insert_arrow, NULL, NULL, NULL, {0,0,0}},
		{"arrow_left", action_insert_arrow_left, NULL, NULL, NULL, {0,0,0}},
		{"dot", action_insert_dot, NULL, NULL, NULL, {0,0,0}},
		{"list", action_insert_endlist_tag, NULL, NULL, NULL, {0,0,0}},
		{"bond", action_insert_bond, NULL, NULL, NULL, {0,0,0}},
		{"sup", action_apply_sup_tag, NULL, NULL, NULL, {0,0,0}},
		{"sub", action_apply_sub_tag, NULL, NULL, NULL, {0,0,0}},
		{"br", action_apply_br_tag, NULL, NULL, NULL, {0,0,0}},
		{"space", action_insert_space_tag, NULL, NULL, NULL, {0,0,0}},
		{"close_tab", action_close_tab, NULL, NULL, NULL, {0,0,0}},
		{"quit", action_quit_activated, NULL, NULL, NULL, {0,0,0}}
	};

	struct {
	  const gchar *action;
	  const gchar *accels[2];
	} action_accels[] = {
	  { "app.new", { "<Control>n", NULL} },
	  { "app.esc", { "Escape", NULL} },
	  { "app.open", { "<Control>o", NULL} },
	  { "app.bold", { "<Control>b", NULL} },
	  { "app.h2", { "<Control>h", NULL} },
	  { "app.italic", { "<Control>i", NULL} },
	  { "app.div", { "<Control>g", NULL} },
	  { "app.minus", { "<Control>m", NULL} },
	  { "app.dot", { "<Control>d", NULL} },
	  { "app.list", { "<Control>l", NULL} },
	  { "app.arrow", { "<Control>t", NULL} },
	  { "app.bond", { "<Shift><Control>l", NULL} },
	  { "app.sup", { "<Control>p", NULL} },
	  { "app.arrow_left", { "<Control>y", NULL} },
	  { "app.sub", { "<Control>u", NULL} },
	  { "app.br", { "<Control>r", NULL} },
	  { "app.space", { "<Control>e", NULL} },
	  { "app.close_tab", { "<Control>w", NULL} },
	  { "app.quit", { "<Control>q", NULL} },
	  { "app.save", { "<Control>s", NULL} },
	  { "app.save_as", { "<Shift><Control>s", NULL} },
	  { "app.find_selection", { "<Control>f", NULL} },
	};

	g_action_map_add_action_entries(G_ACTION_MAP(litos->app), app_entries, G_N_ELEMENTS(app_entries), litos);

	for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
		gtk_application_set_accels_for_action(GTK_APPLICATION(litos->app), action_accels[i].action, action_accels[i].accels);
}
