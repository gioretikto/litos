#include "litos.h"

gboolean close_tab (GtkButton *button, gpointer userData);
void set_acels (struct lit *litos);
void about_dialog (GtkButton *button, gpointer userData);
void menu_newtab (GtkWidget *widget, gpointer userData);
void createFilePopover (GtkWidget *parent, GtkPositionType pos, struct lit *litos);
void createSearchPopover(GtkMenuButton *search_menu_button, struct lit *litos);
GtkSourceView* currentTabSourceView(struct lit *litos);
void freePage(int page, struct lit *litos);

void spellCheck(struct lit *litos);

gboolean on_delete_event (GtkWidget *widget,
         GdkEvent  *event,
         gpointer   userData)
{
	(void)widget;
	(void)event;

	struct lit *litos = (struct lit*)userData;

	const int last_page = gtk_notebook_get_n_pages(litos->notebook) - 1;

	gint i;

	for (i = last_page; i >= 0; i--)
	{
 		if (litos->fileSaved[i] == FALSE)
		{
			if (close_tab(NULL, litos))
				return TRUE; /* Returning TRUE means we don't want the window to be destroyed*/
		}

		else
		{
			freePage(i, litos);
			gtk_notebook_remove_page(litos->notebook, i);
		}			
 	}

	return FALSE;
}

void swap(struct lit *litos, const int a, const int b)
{
	char *filename_tmp = litos->filename[a];

	litos->filename[a] = litos->filename[b];

	litos->filename[b] = filename_tmp;

	_Bool fileSaved_tmp = litos->fileSaved[a];

	litos->fileSaved[a] = litos->fileSaved[b];

	litos->fileSaved[b] = fileSaved_tmp;
}

static void page_reordered_cb (
  GtkNotebook* self,
  GtkWidget* child,
  guint page_num,
  gpointer userData
)
{
	(void) self;
	(void) child;

	struct lit *litos = (struct lit*)userData;

	int i;
	const int end_page = (int)page_num;

	if (litos->page > end_page)
		for (i = litos->page; i > end_page; i--) { swap(litos, i - 1, i); }

	else
		for(i = litos->page; i < end_page; i++) { swap(litos, i + 1, i); }

	litos->page = end_page;

	gtk_window_set_title (GTK_WINDOW (litos->window), litos->filename[end_page]);
}

void switchPage_cb(GtkNotebook *notebook, gpointer page, const guint page_num, gpointer userData)
{
	(void) notebook;
	(void) page;

	struct lit *litos = (struct lit*)userData;

	litos->page = (int)page_num;

	gtk_window_set_title (GTK_WINDOW (litos->window), litos->filename[page_num]);
}

void activate_cb (GtkApplication* app, gpointer userData)
{
	struct lit *litos = (struct lit*)userData;

	if (litos->window != NULL)
		return;

	litos->window = gtk_application_window_new (app);

	GtkWidget *about_button, *search_menu_button, *close_tab_button, *file_menu_button, *spell_button;

	litos->app = app;

	litos->headbar = gtk_header_bar_new();

	spell_button = gtk_button_new_with_label("Check Spell");
	close_tab_button = gtk_button_new_with_label("Close Tab");
	about_button = gtk_button_new_with_label("About");

	gtk_header_bar_set_title (GTK_HEADER_BAR (litos->headbar), "Litos");
	gtk_window_set_title (GTK_WINDOW (litos->window), "Litos");    
	gtk_window_set_titlebar (GTK_WINDOW (litos->window), litos->headbar);
	gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (litos->headbar), TRUE);
	gtk_window_maximize (GTK_WINDOW (litos->window));

	file_menu_button = gtk_menu_button_new ();
	search_menu_button = gtk_menu_button_new ();

	createFilePopover(file_menu_button, GTK_POS_TOP, litos);
	createSearchPopover(GTK_MENU_BUTTON(search_menu_button), litos);

	gtk_button_set_image (GTK_BUTTON (file_menu_button), gtk_image_new_from_icon_name ("open-menu-symbolic", GTK_ICON_SIZE_BUTTON));
	gtk_button_set_image (GTK_BUTTON (search_menu_button), gtk_image_new_from_icon_name ("edit-find-replace", GTK_ICON_SIZE_BUTTON));

	gtk_container_add(GTK_CONTAINER (litos->headbar), file_menu_button);
	gtk_container_add(GTK_CONTAINER (litos->headbar), search_menu_button);
	gtk_container_add(GTK_CONTAINER (litos->headbar), spell_button);
	gtk_container_add(GTK_CONTAINER (litos->headbar), close_tab_button);
	gtk_container_add(GTK_CONTAINER (litos->headbar), about_button);

	litos->notebook = GTK_NOTEBOOK(gtk_notebook_new());

	gtk_container_add(GTK_CONTAINER(litos->window), GTK_WIDGET(litos->notebook));

	set_acels(litos);

	menu_newtab(NULL, litos);

	gtk_widget_grab_focus(GTK_WIDGET(currentTabSourceView(litos)));

	g_signal_connect (litos->window, "delete-event", G_CALLBACK (on_delete_event), litos);
   	g_signal_connect (close_tab_button, "clicked", G_CALLBACK (close_tab), litos);
	g_signal_connect (about_button, "clicked", G_CALLBACK (about_dialog), NULL);
	g_signal_connect (spell_button, "clicked", G_CALLBACK (spellCheck), litos);
	g_signal_connect (litos->notebook, "switch-page", G_CALLBACK(switchPage_cb), litos);
	g_signal_connect (litos->notebook, "page-reordered", G_CALLBACK (page_reordered_cb), litos);

	gtk_widget_show_all (litos->window);
}
