#include "litos.h"

void close_tab (GtkButton *button, gpointer userData);
void set_acels (struct lit *litos);
void about_dialog (GtkButton *button, gpointer userData);
void menu_newtab (GtkWidget *widget, gpointer userData);

void createFilePopover (GtkWidget *parent, GtkPositionType pos, struct lit *litos);
void createFindPopover(GtkMenuButton *find_menu_button, struct lit *litos);
void action_quit_activated(GSimpleAction *action, GVariant *parameter, gpointer app);
GtkSourceView* tab_get_sourceview(int page, struct lit *litos);

void activate (GtkApplication* app, gpointer userData)
{
	struct lit *litos = (struct lit*)userData;

    GtkWidget *about_button, *find_menu_button, *close_tab_button, *file_menu_button;

	litos->app = app;

	litos->window = gtk_application_window_new (app);
	litos->headbar = gtk_header_bar_new();

	close_tab_button = gtk_button_new_with_label("Close Tab");
	about_button = gtk_button_new_with_label("About");

    gtk_header_bar_set_title (GTK_HEADER_BAR (litos->headbar), "Litos");
    gtk_window_set_title (GTK_WINDOW (litos->window), "Litos");    
	gtk_window_set_titlebar (GTK_WINDOW (litos->window), litos->headbar);
	gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (litos->headbar), TRUE);
	gtk_window_maximize (GTK_WINDOW (litos->window));

	file_menu_button = gtk_menu_button_new ();
	find_menu_button = gtk_menu_button_new ();
	
	createFilePopover(file_menu_button, GTK_POS_TOP, litos);
    createFindPopover(GTK_MENU_BUTTON(find_menu_button), litos);

	gtk_button_set_image (GTK_BUTTON (file_menu_button), gtk_image_new_from_icon_name ("open-menu-symbolic", GTK_ICON_SIZE_BUTTON));
	gtk_button_set_image (GTK_BUTTON (find_menu_button), gtk_image_new_from_icon_name ("edit-find-replace", GTK_ICON_SIZE_BUTTON));

	gtk_container_add(GTK_CONTAINER (litos->headbar), file_menu_button);
	gtk_container_add(GTK_CONTAINER (litos->headbar), find_menu_button);
	gtk_container_add(GTK_CONTAINER (litos->headbar), close_tab_button);
	gtk_container_add(GTK_CONTAINER (litos->headbar), about_button);

    litos->notebook = GTK_NOTEBOOK(gtk_notebook_new());

    gtk_container_add(GTK_CONTAINER(litos->window), GTK_WIDGET(litos->notebook));

    menu_newtab(NULL, litos);

	gtk_widget_grab_focus(GTK_WIDGET(tab_get_sourceview(CURRENT_PAGE, litos)));

    g_signal_connect (G_OBJECT (litos->window), "delete-event", G_CALLBACK (action_quit_activated), litos);
   	g_signal_connect (close_tab_button, "clicked", G_CALLBACK (close_tab), litos);
	g_signal_connect (about_button, "clicked", G_CALLBACK (about_dialog), NULL);
	
    set_acels(litos);

    gtk_widget_show_all (litos->window);
}
