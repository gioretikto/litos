#include "litos.h"

void menu_newtab (GtkWidget *widget, gpointer userData);
void menu_save (GtkWidget *widget, gpointer userData);
void close_tab (GtkButton *button, gpointer userData);
void applyTags(struct lit *litos, char *what_tag);
unsigned int saveornot_before_close(gint page, struct lit *litos);
void open_dialog (GtkWidget *widget, gpointer userData);
void ctrlF (GtkButton *button, gpointer userData);

void action_find_selection(GSimpleAction *action, GVariant *parameter, gpointer userData) {(void)userData; (void)action; (void)parameter; ctrlF(NULL, userData);}

void action_apply_bold(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	(void)parameter;
	char *tag = "b";
	applyTags(userData, tag);
}

void action_apply_italic(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	(void)parameter;
	char *tag = "i";
	applyTags(userData, tag);
}

void action_apply_sup_tag(GSimpleAction *action, GVariant *parameter, gpointer userData)
{
	(void)action;
	(void)parameter;
	char *tag = "sup";
	applyTags(userData, tag);
}

void action_save_dialog(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; menu_save(NULL, userData);}
void action_new_tab(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; menu_newtab (NULL, userData);}
void action_close_tab(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; close_tab(NULL, userData);}
void action_open_dialog(GSimpleAction *action, GVariant *parameter, void* userData) { (void)action; (void)parameter; open_dialog(NULL, userData);}

void action_quit_activated(GSimpleAction *action, GVariant *parameter, void* userData)
{
	(void)action;
	(void)parameter;

	gint i;

	unsigned int res = CLOSE;
	
	struct lit *litos = (struct lit*)userData;

	for (i = 0; i < gtk_notebook_get_n_pages(litos->notebook); i++)
	{
 		if (litos->fileSaved[i] == FALSE)
 		{
			res = saveornot_before_close(i, litos);

		    if (res == CLOSE)
		    	i--;
		}
 	}

	if (res == CLOSE || res == SAVE)
		g_application_quit (G_APPLICATION (litos->app));
}

void set_acels (struct lit *litos)
{
	long unsigned int i;

	const GActionEntry app_entries[] = {
		{"new", action_new_tab, NULL, NULL, NULL, {0,0,0}},
		{"open", action_open_dialog, NULL, NULL, NULL, {0,0,0}},
		{"save", action_save_dialog, NULL, NULL, NULL, {0,0,0}},
		{"find_selection", action_find_selection, NULL, NULL, NULL, {0,0,0}},
		{"bold", action_apply_bold, NULL, NULL, NULL, {0,0,0}},
		{"italic", action_apply_italic, NULL, NULL, NULL, {0,0,0}},
		{"sup", action_apply_sup_tag, NULL, NULL, NULL, {0,0,0}},
		{"close_tab", action_close_tab, NULL, NULL, NULL, {0,0,0}},
		{"quit", action_quit_activated, NULL, NULL, NULL, {0,0,0}}
	};

	struct {
	  const gchar *action;
	  const gchar *accels[2];
	} action_accels[] = {
	  { "app.new", { "<Control>n", NULL} },
	  { "app.open", { "<Control>o", NULL} },
	  { "app.bold", { "<Control>b", NULL} },
	  { "app.italic", { "<Control>i", NULL} },
	  { "app.sup", { "<Control>p", NULL} },
	  { "app.close_tab", { "<Control>w", NULL} },
	  { "app.quit", { "<Control>q", NULL} },
	  { "app.save", { "<Control>s", NULL} },
	  { "app.find_selection", { "<Control>f", NULL} },
	};

	g_action_map_add_action_entries(G_ACTION_MAP(litos->app), app_entries, G_N_ELEMENTS(app_entries), litos);

	for (i = 0; i < G_N_ELEMENTS(action_accels); i++)
		gtk_application_set_accels_for_action(GTK_APPLICATION(litos->app), action_accels[i].action, action_accels[i].accels);
}
