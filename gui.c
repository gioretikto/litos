#include "litos.h"

void monitor_change() { changed[gtk_notebook_get_current_page(notebook)] = TRUE;}
void action_quit_activated(GSimpleAction *action, GVariant *parameter, gpointer app);
void create_menu (GtkApplication *app);

void highlight_buffer(char *filename);

_Bool changed[10] = {FALSE};

void menu_newtab(char *filename)
{
    GtkWidget *scrolled_window;
    GtkWidget *tabbox;

    GtkWidget *source_view = new_sourceview();

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
		notebook,
		tabbox,
		gtk_label_new(filename),
		gtk_label_new(filename)
    );

    gtk_notebook_set_tab_reorderable(notebook, tabbox, TRUE);

	gtk_notebook_set_current_page(
		notebook,
		gtk_notebook_get_n_pages(notebook) - 1
    );

	g_signal_connect (buffer, "notify::text", G_CALLBACK (monitor_change), NULL);

}

void activate (GtkApplication* app, gpointer user_data)
{
	window = gtk_application_window_new (app);
	gtk_window_set_title (GTK_WINDOW (window), "Litos");
	gtk_window_maximize (GTK_WINDOW(window));

	(void)user_data;

    notebook = GTK_NOTEBOOK(gtk_notebook_new());

    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(notebook));

    create_menu(app);

    menu_newtab("unsaved");

    g_signal_connect (G_OBJECT (window), "delete-event", G_CALLBACK (action_quit_activated), app);

    gtk_widget_show_all (window);
}
