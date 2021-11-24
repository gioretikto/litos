#include "litos.h"

void save_file(gint page, struct lit *litos);
void save_as_dialog(struct lit *litos);
void menu_newtab (GtkWidget *widget, gpointer userData);
void monitor_change (GObject *gobject, GParamSpec *pspec, gpointer userData);
unsigned int saveornot_before_close(gint page, struct lit *litos);

void highlight_buffer(struct lit *litos);
GtkSourceView* currentTabSourceView(struct lit *litos);
GtkTextBuffer* get_current_buffer(struct lit *litos);

void freePage(int page, struct lit *litos)
{
	int i;
	int total_pages = gtk_notebook_get_n_pages(litos->notebook);

    g_free(litos->filename[page]);

	if (total_pages == 1)
        litos->filename[page] = NULL;

	else
	{
		for (i = page; i < total_pages; i++)
		{

			litos->filename[i] = litos->filename[i+1];

			litos->fileSaved[i] = litos->fileSaved[i+1];

		}
	}

	litos->fileSaved[total_pages-1] = TRUE;
	litos->filename[total_pages-1] = NULL;
}

void close_tab (GtkButton *button, gpointer userData)
{
	(void)button;

	struct lit *litos = (struct lit*)userData;

	gint page = gtk_notebook_get_current_page(litos->notebook);

	if (litos->fileSaved[page] == FALSE)
		saveornot_before_close(page, litos);

	else
	{
		if (gtk_notebook_get_n_pages(litos->notebook) == 1)
		{
			freePage(page, litos);
			gtk_notebook_remove_page(litos->notebook, page);
		    menu_newtab(NULL, litos);
		}
		
		else
		{
			freePage(page, litos);
			gtk_notebook_remove_page(litos->notebook, page);
		}
	}
}

void menu_save (GtkWidget *widget, gpointer userData)
{
	(void)widget;

	struct lit *litos = (struct lit*)userData;

	gint page = gtk_notebook_get_current_page(litos->notebook);

	if (litos->fileSaved[page] == TRUE)
		return;

	if (litos->filename[page] == NULL)
		save_as_dialog(litos);

	else
		save_file(page, litos);

	gtk_widget_grab_focus(GTK_WIDGET(currentTabSourceView(litos)));
}

void save_file(gint page, struct lit *litos)
{
	GtkTextIter start, end;

	gchar *content;

	GtkTextView *text_view = GTK_TEXT_VIEW(currentTabSourceView(litos));

	GtkTextBuffer *current_buffer = gtk_text_view_get_buffer (text_view);

	gtk_text_buffer_get_bounds (current_buffer, &start, &end);

	content = gtk_text_buffer_get_text (current_buffer, &start, &end, FALSE);

	if (!g_file_set_contents (litos->filename[page], content, -1, NULL))
		g_warning ("The file '%s' could not be written!", litos->filename[page]);

	else
	{
		litos->fileSaved[page] = TRUE;

		g_print("%s\n", litos->filename[page]);

		GtkWidget *label = gtk_label_new (NULL);

		const char *format = "<span color='black'>\%s</span>";

		char *markup = g_markup_printf_escaped (format, litos->filename[page]);

		gtk_label_set_markup (GTK_LABEL(label), markup);

    	gtk_notebook_set_tab_label_text(litos->notebook, gtk_notebook_get_nth_page(litos->notebook, page),litos->filename[page]);
	}

	g_free (content);
}

void save_as_file(GtkFileChooser *chooser, struct lit *litos)
{
	GFile *loc = gtk_file_chooser_get_file(chooser);

	GtkTextIter begin, end;

	GtkTextView *text_view = GTK_TEXT_VIEW(currentTabSourceView(litos));

	GtkTextBuffer *current_buffer = gtk_text_view_get_buffer (text_view);

	gtk_text_buffer_get_start_iter(current_buffer, &begin);

	gtk_text_buffer_get_end_iter(current_buffer, &end);

	char *contents = gtk_text_buffer_get_text(current_buffer, &begin, &end, 0);

	gint page = gtk_notebook_get_current_page(litos->notebook);

	litos->filename[page] = gtk_file_chooser_get_filename (chooser);

	if (!g_file_replace_contents(loc, contents, strlen(contents), NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, NULL))
		g_warning ("The file '%s' could not be written!", litos->filename[page]);

	else
		litos->fileSaved[page] = TRUE;

    gtk_notebook_set_tab_label_text(litos->notebook, gtk_notebook_get_nth_page(litos->notebook, page),litos->filename[page]);

	g_object_unref(loc);
}

void open_file(struct lit *litos, gboolean template)
{
    gboolean read_file_status;
   	GError *error;
   	char* contents;

	gint page = gtk_notebook_get_current_page(litos->notebook);

    read_file_status = g_file_get_contents(litos->filename[page], &contents, NULL, &error);

    if (read_file_status == FALSE)
    {
		g_error("error opening file: %s\n",error && error->message ? error->message : "No Detail");
		return;
    }

	GtkTextBuffer *current_buffer = get_current_buffer(litos);

	gtk_text_buffer_set_text(GTK_TEXT_BUFFER(current_buffer), contents, -1);

	char *filename = litos->filename[page];

	if (template)
	{
		litos->filename[page] = NULL;
		filename = "Untitled";
	}

	gtk_notebook_set_tab_label_text(
		litos->notebook,
		gtk_notebook_get_nth_page(
	        litos->notebook,
	        page
		),
		filename
	);

	if (litos->filename[page] != NULL)
		highlight_buffer(litos);

	gtk_widget_grab_focus(GTK_WIDGET(currentTabSourceView(litos)));

	litos->fileSaved[page] = TRUE;
}

void menu_newtab (GtkWidget *widget, gpointer userData)
{
	(void)widget;

	struct lit *litos = (struct lit*)userData;

    GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);

    GtkWidget *tabbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);

    GtkWidget *source_view = MyNewSourceview(litos);

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
		gtk_label_new("Untitled"),
		gtk_label_new("Untitled")
    );

	gtk_notebook_set_current_page(
		litos->notebook,
		gtk_notebook_get_n_pages(litos->notebook) - 1
    );

    gtk_notebook_set_tab_reorderable(litos->notebook, tabbox, TRUE);

	g_signal_connect (litos->buffer, "notify::text", G_CALLBACK (monitor_change), litos);

}

void monitor_change (GObject *gobject, GParamSpec *pspec, gpointer userData)	/*Function called when the file gets modified */
{
	(void)gobject;

	(void)pspec;

	struct lit *litos = (struct lit*)userData;

	gint page = gtk_notebook_get_current_page(litos->notebook);

	const gchar *filename = gtk_notebook_get_tab_label_text(litos->notebook,
								gtk_notebook_get_nth_page (litos->notebook, page)
							);

	if (litos->fileSaved[page] == TRUE)
	{
		GtkWidget *label = gtk_label_new (NULL);

		const char *format = "<span color='red'>\%s</span>";

		char *markup = g_markup_printf_escaped (format, filename);

		gtk_label_set_markup (GTK_LABEL(label), markup);

		gtk_notebook_set_tab_label (litos->notebook, gtk_notebook_get_nth_page(litos->notebook, page), label);
	}

	litos->fileSaved[page] = FALSE;
}
