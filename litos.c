#include "litos.h"
#include <string.h>

GtkWidget* MyNewSourceview(struct lit *litos);
void menu_newtab (GtkWidget *widget, gpointer userData);
void monitor_change (GObject *gobject, GParamSpec *pspec, gpointer userData);
gboolean saveornot_before_close (gint page, struct lit *litos);
void highlight_buffer(struct lit *litos);
GtkSourceView* currentTabSourceView(struct lit *litos);
GtkTextBuffer* get_current_buffer(struct lit *litos);
static void open_file_complete (GObject *source_object, GAsyncResult *res, gpointer userData);
static void save_file (GObject *source_object, GAsyncResult *res, gpointer userData);
void action_save_as_dialog (GSimpleAction *action, GVariant *parameter, void* userData);
static void save_file_complete (GObject *source_object, GAsyncResult *result, gpointer userData);

static void changeLblColor(struct lit *litos)
{
	const char *format;

	const gchar *filename = gtk_notebook_get_tab_label_text(litos->notebook,
								gtk_notebook_get_nth_page (litos->notebook, litos->page)
							);

	GtkWidget *label = gtk_label_new (NULL);

	if (litos->fileSaved[litos->page] == TRUE)
	{
		format = "<span color='red'>\%s</span>";
		litos->fileSaved[litos->page] = FALSE;
	}

	else
		format = "<span color='black'>\%s</span>";

	const char *markup = g_markup_printf_escaped (format, filename);

	gtk_label_set_markup (GTK_LABEL(label), markup);

	gtk_notebook_set_tab_label (litos->notebook, gtk_notebook_get_nth_page(litos->notebook, litos->page), label);
}


void freePage(const int page, struct lit *litos)
{
	g_free(litos->filename[page]);

	int total_pages = gtk_notebook_get_n_pages(litos->notebook);

				/* If page 2 is closed move 3->2, 4->3, until last page */
	int i;

	for (i = page; i < total_pages; i++)
	{
		litos->filename[i] = litos->filename[i+1];

		litos->fileSaved[i] = litos->fileSaved[i+1];
	}

	total_pages--;

	litos->fileSaved[total_pages] = TRUE;
	litos->filename[total_pages] = NULL;
}

void clear_page_buffer(struct lit *litos)
{
	if (litos->filename[0] != NULL)
	{
		g_free(litos->filename[0]);
		litos->filename[0] = NULL;
	}

	gtk_notebook_set_tab_label_text(
		litos->notebook,
		gtk_notebook_get_nth_page(
	    litos->notebook,
	    0
		),
		"Untitled"
	);

	gtk_window_set_title (GTK_WINDOW (litos->window), "Untitled");

	GtkTextBuffer *buffer = get_current_buffer(litos);

	if ((gtk_text_buffer_get_char_count(buffer)) != 0)
	{		
		GtkTextIter start, end;
		gtk_text_buffer_get_start_iter (buffer, &start);
		gtk_text_buffer_get_end_iter (buffer, &end);
		gtk_text_buffer_delete (buffer, &start, &end);
		gtk_text_buffer_insert (buffer, &start, "", strlen(""));
		changeLblColor(litos);
	}

	litos->fileSaved[0] = TRUE;
}

gboolean close_tab (GtkButton *button, gpointer userData)
{
	(void)button;

	struct lit *litos = (struct lit*)userData;

	if (litos->fileSaved[litos->page] == FALSE)
		 return saveornot_before_close(litos->page, litos);

	else
	{
		if (gtk_notebook_get_n_pages(litos->notebook) == 1)
			clear_page_buffer(litos);

		else
		{
			freePage((int)litos->page, litos);
			gtk_notebook_remove_page(litos->notebook, litos->page);
		}
	}

	return TRUE;
}

void menu_save (gpointer userData)
{
	struct lit *litos = (struct lit*)userData;

	if (litos->fileSaved[litos->page] == TRUE)
		return;

	else if (litos->filename[litos->page] == NULL)
		action_save_as_dialog(NULL, NULL, litos);

	else
	{
		g_autoptr (GFile) file = g_file_new_for_path(litos->filename[litos->page]);
		save_file (G_OBJECT(file), NULL, litos);
	}
}

void on_save_as_response(GFile *file, struct lit *litos)
{
    if (!g_file_query_exists(file, NULL))
	{
		gtk_notebook_set_tab_label_text(
			litos->notebook,
			gtk_notebook_get_nth_page(
	        litos->notebook,
	        litos->page
			),
			g_file_get_basename (file)
		);

		g_file_create_async (file,
				G_FILE_CREATE_NONE,
				G_PRIORITY_DEFAULT,
				NULL,
				save_file, litos);
	}

	else
		save_file (G_OBJECT(file), NULL, litos);
}

static void save_file (GObject *source_object, GAsyncResult *result, gpointer userData)
{
	(void) result;

	GFile *file = G_FILE (source_object);

	struct lit *litos = (struct lit*)userData;

	GtkTextBuffer *buffer = get_current_buffer(litos);

	/* Retrieve the iterator at the start of the buffer */
	GtkTextIter start;
	gtk_text_buffer_get_start_iter (buffer, &start);

	/* Retrieve the iterator at the end of the buffer */
	GtkTextIter end;
	gtk_text_buffer_get_end_iter (buffer, &end);

	/* Retrieve all the visible text between the two bounds */
	char *text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

	/* If there is nothing to save, return early */
	if (text == NULL || *text == '\0')
		return;

	/* Ensure that the file is encoded with UTF-8 */
	if (!g_utf8_validate (text, (gssize)strlen (text), NULL))
	{
		g_printerr ("Unable to save the contents in “%s”: "
			"the text is not encoded with UTF-8\n",
		g_file_peek_path (file));

		return;
	}

	GBytes* bytes = g_bytes_new_take (text, strlen (text));

	/* Start the asynchronous operation to save the data into the file */
	g_file_replace_contents_bytes_async (file,
			bytes,
			NULL,
			FALSE,
			G_FILE_CREATE_NONE,
			NULL,
			save_file_complete,
			litos);

	g_bytes_unref (bytes);
}

static void save_file_complete (GObject *source_object, GAsyncResult *result, gpointer userData)
{
	GFile *file = G_FILE (source_object);

	struct lit *litos = (struct lit*)userData;

	g_autoptr (GError) error = NULL;

	g_file_replace_contents_finish (file, result, NULL, &error);

	if (error != NULL)
	{
		g_printerr ("Unable to save “%s”: %s\n",
			g_file_peek_path (file),
			error->message);
    }

	else
	{
		changeLblColor(litos);

		litos->fileSaved[litos->page] = TRUE;

		if (litos->filename[litos->page] == NULL)
			litos->filename[litos->page] = g_file_get_path(file);

		gtk_window_set_title (GTK_WINDOW (litos->window), litos->filename[litos->page]);

		gtk_widget_grab_focus(GTK_WIDGET(currentTabSourceView(litos)));
	}
}

void open_file (GFile *file, gpointer userData, gboolean template)
{
	struct lit *litos = (struct lit*)userData;

    GtkTextBuffer *buffer = get_current_buffer(litos);

	if ((gtk_text_buffer_get_char_count(buffer)) != 0)
		menu_newtab(NULL, litos);

	if (template == FALSE)
		litos->filename[litos->page] = g_file_get_path (file);

	g_file_load_contents_async (file,
			NULL,
			(GAsyncReadyCallback) open_file_complete,
			litos);
}

static void open_file_complete (GObject *source_object, GAsyncResult *res, gpointer userData)
{
	GFile *file = G_FILE (source_object);

	struct lit *litos = (struct lit*)userData;

	g_autofree char *contents = NULL;

	gsize length = 0;

	g_autoptr (GError) error = NULL;

	/* Complete the asynchronous operation; this function will either
	 give you the contents of the file as a byte array, or will
	 set the error argument */
	g_file_load_contents_finish (file,
				res,
				&contents,
				&length,
				NULL,
				&error);

	/* In case of error, print a warning to the standard error output */
	if (error != NULL)
	{
		g_printerr ("Unable to open “%s”: %s\n",
			g_file_peek_path (file),
			error->message);

		return;
    }

	/* Now that you have the contents of the file, you can display them in the GtkTextView widget. */

	/* Ensure that the file is encoded with UTF-8 */
	if (!g_utf8_validate (contents, (gssize)length, NULL))
	{
		g_printerr ("Unable to load the contents of “%s”: "
			"the file is not encoded with UTF-8\n",
		g_file_peek_path (file));

		return;
	}

	/* Retrieve the GtkTextBuffer instance that stores the
	 text displayed by the GtkTextView widget */

	GtkTextBuffer *current_buffer = get_current_buffer(litos);

	/* Set the text using the contents of the file */
	gtk_text_buffer_set_text (current_buffer, contents, (gint)length);

	/* Reposition the cursor so it's at the start of the text */
	GtkTextIter start;
	gtk_text_buffer_get_start_iter (current_buffer, &start);
	gtk_text_buffer_place_cursor (current_buffer, &start);

	gtk_widget_grab_focus(GTK_WIDGET(currentTabSourceView(litos)));

	gtk_window_set_title (GTK_WINDOW (litos->window), g_file_peek_path (file));

	highlight_buffer(litos);

	litos->fileSaved[litos->page] = TRUE;

	if (litos->filename[litos->page] == NULL)
		printf("File name is NULL\n");
	else
		printf("Filename is %s\n", g_file_peek_path (file));

	gtk_notebook_set_tab_label_text(
		litos->notebook,
		gtk_notebook_get_nth_page(
	    litos->notebook,
	    litos->page
		),
		g_file_get_basename (file)
	);
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
							"textview { font-family: Monospace; font-size: 11pt;}",
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
		gtk_notebook_get_n_pages(litos->notebook)-1
	);

	gtk_notebook_set_tab_reorderable(litos->notebook, tabbox, TRUE);

	g_signal_connect (litos->buffer, "notify::text", G_CALLBACK (monitor_change), litos);
}

void monitor_change (GObject *gobject, GParamSpec *pspec, gpointer userData)	/*Function called when the file gets modified */
{
	(void)gobject;

	(void)pspec;

	struct lit *litos = (struct lit*)userData;

	if (litos->fileSaved[litos->page] == TRUE)
		changeLblColor(litos);
}
