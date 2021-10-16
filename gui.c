#include "litos.h"

void close_tab(struct lit *litos);
void open_dialog(struct lit *litos);
void save_as_dialog(struct lit *litos);

void set_acels (GtkApplication *app);

void highlight_buffer(char *filename);
void init_find_replace_popover();
void about_dialog(void);
void init_find_replace_popover(GtkMenuButton *find_replace_button);
void menu_newtab(char *filename, struct lit *litos);
void menu_findreplaceall(void);
void menu_save(struct lit *litos);
void find_button_clicked ();
void my_grab_focus();

void monitor_change(struct lit *litos) { litos->changed[gtk_notebook_get_current_page(litos->notebook)] = TRUE;}
void action_quit_activated(GSimpleAction *action, GVariant *parameter, gpointer app);

void menu_newtab(char *filename, struct lit *litos)
{
    GtkWidget *scrolled_window;

    GtkWidget *tabbox;

    GtkWidget *source_view = MyNewSourceview(litos);

   	highlight_buffer(filename);

    tabbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);

    scrolled_window = gtk_scrolled_window_new (NULL, NULL);

    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
	GTK_POLICY_AUTOMATIC,
	GTK_POLICY_AUTOMATIC);

	gtk_widget_set_hexpand (scrolled_window, TRUE);

	GtkCssProvider *provider = gtk_css_provider_new ();
    gtk_css_provider_load_from_data (provider,
                                     "textview { font-family: Monospace; font-size: 11pt; }",
                                     -1,
                                     NULL);
    gtk_style_context_add_provider (gtk_widget_get_style_context (source_view),
                                    GTK_STYLE_PROVIDER (provider),
                                    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref (provider);

	gtk_container_add(GTK_CONTAINER(scrolled_window), source_view);

	gtk_container_add (GTK_CONTAINER(tabbox), scrolled_window);

	gtk_widget_show_all(GTK_WIDGET(tabbox));

	gtk_notebook_append_page_menu(
		litos->notebook,
		tabbox,
		gtk_label_new(filename),
		gtk_label_new(filename)
    );

    gtk_notebook_set_tab_reorderable(litos->notebook, tabbox, TRUE);

	gtk_notebook_set_current_page(
		litos->notebook,
		gtk_notebook_get_n_pages(litos->notebook) - 1
    );

	g_signal_connect (litos->buffer, "notify::text", G_CALLBACK (monitor_change), litos);
}

void create_popover (GtkWidget *parent, GtkPositionType pos, struct lit *litos)
{
	GtkWidget *popover;

	popover = gtk_popover_new (NULL);
	gtk_popover_set_position (GTK_POPOVER (popover), pos);

	gtk_menu_button_set_popover (GTK_MENU_BUTTON(parent), popover);
	gtk_container_set_border_width (GTK_CONTAINER (popover), 6);
  
  	GtkWidget *file_box, *new_tab_button, *open_button, *save_button, *save_as_button;

  	file_box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  	gtk_container_add (GTK_CONTAINER (popover), file_box);

	new_tab_button = gtk_button_new_with_label("New Tab");
	open_button = gtk_button_new_with_label("Open");
	save_button = gtk_button_new_with_label("Save");
	save_as_button = gtk_button_new_with_label("Save As");

	gtk_container_add(GTK_CONTAINER (file_box), open_button);
	gtk_container_add(GTK_CONTAINER (file_box), save_button);
	gtk_container_add(GTK_CONTAINER (file_box), save_as_button);

	g_signal_connect (new_tab_button, "clicked", G_CALLBACK (menu_newtab), litos);
	g_signal_connect (open_button, "clicked", G_CALLBACK (open_dialog), litos);
	g_signal_connect (save_button, "clicked", G_CALLBACK (menu_save), litos);

	gtk_widget_show_all (file_box);
}

void activate (GtkApplication* app, gpointer userData)
{
	struct lit *litos = (struct lit*)userData;

    GtkWidget *about_button, *find_replace_button, *close_tab_button;

	litos->window = gtk_application_window_new (app);
	litos->headbar = gtk_header_bar_new();

	litos->app = app;

	close_tab_button = gtk_button_new_with_label("Close Tab");
	about_button = gtk_button_new_with_label("About");

    gtk_header_bar_set_title (GTK_HEADER_BAR (litos->headbar), "Litos");
    gtk_window_set_title (GTK_WINDOW (litos->window), "Litos");    
	gtk_window_set_titlebar (GTK_WINDOW (litos->window), litos->headbar);
	gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (litos->headbar), TRUE);
	gtk_window_maximize (GTK_WINDOW (litos->window));

    GtkWidget *file_menu_button;
    
	file_menu_button = gtk_menu_button_new ();
	find_replace_button = gtk_menu_button_new();
	
	create_popover(file_menu_button, GTK_POS_TOP, litos);

	gtk_button_set_image (GTK_BUTTON (file_menu_button), gtk_image_new_from_icon_name ("open-menu-symbolic", GTK_ICON_SIZE_BUTTON));
	gtk_button_set_image (GTK_BUTTON (find_replace_button), gtk_image_new_from_icon_name ("edit-find-replace", GTK_ICON_SIZE_BUTTON));

	gtk_container_add(GTK_CONTAINER (litos->headbar), file_menu_button);
	gtk_container_add(GTK_CONTAINER (litos->headbar), close_tab_button);
	gtk_container_add(GTK_CONTAINER (litos->headbar), find_replace_button);
	gtk_container_add(GTK_CONTAINER (litos->headbar), about_button);

    litos->notebook = GTK_NOTEBOOK(gtk_notebook_new());

    gtk_container_add(GTK_CONTAINER(litos->window), GTK_WIDGET(litos->notebook));

    menu_newtab("unsaved", litos);

    my_grab_focus();
    
    init_find_replace_popover(GTK_MENU_BUTTON(find_replace_button));

    g_signal_connect (G_OBJECT (litos->window), "delete-event", G_CALLBACK (action_quit_activated), app);
   	g_signal_connect (close_tab_button, "clicked", G_CALLBACK (close_tab), litos);
    g_signal_connect (find_replace_button, "clicked", G_CALLBACK (find_button_clicked), NULL);
	g_signal_connect (about_button, "clicked", G_CALLBACK (about_dialog), NULL);
	
    set_acels(app);

    gtk_widget_show_all (litos->window);
}
