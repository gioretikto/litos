#include "litos.h"

void open_file (GFile *file, gpointer userData);
void menu_save (gpointer userData);
void save_as_dialog(struct lit *litos);
void save_as_file(GtkFileChooser *chooser, struct lit *litos);
GtkSourceView* currentTabSourceView(struct lit *litos);
void menu_newtab (GtkWidget *widget, gpointer userData);
void freePage(int page, struct lit *litos);
GtkTextBuffer* get_current_buffer(struct lit *litos);
void clear_page_buffer(struct lit *litos);

gboolean saveornot_before_close(const gint page, struct lit *litos)
{
	GtkWidget *message_dialog;

	message_dialog = gtk_message_dialog_new(GTK_WINDOW(litos->window), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING,
                      GTK_BUTTONS_NONE, "Save changes to document %s before closing?", litos->filename[page]);

	gtk_dialog_add_buttons (GTK_DIALOG(message_dialog), "Close without Saving", GTK_RESPONSE_REJECT,
                                                      "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT,  NULL);

	gint res = gtk_dialog_run(GTK_DIALOG(message_dialog));

	gtk_widget_destroy(message_dialog);

	const int totalPages = gtk_notebook_get_n_pages(litos->notebook);

	switch (res)
	{
		case GTK_RESPONSE_CANCEL:
			return TRUE;

		case GTK_RESPONSE_ACCEPT:

			menu_save(litos);

			return TRUE;

		case GTK_RESPONSE_REJECT:

	   		if (totalPages == 1)
				clear_page_buffer(litos);

			else
			{
				freePage(page, litos);
				gtk_notebook_remove_page(litos->notebook, page);
			}

			return FALSE;

		default: /*close bottun was pressed*/
			g_print("The bottun(Close without Saving/Cancel/Save) was not pressed.");
	}

	return TRUE;
}

void open_dialog (GtkWidget *widget, gpointer userData)
{
    (void)widget;

    struct lit *litos = (struct lit*)userData;
 
    gint response;

    GtkWidget *dialog;

    dialog = gtk_file_chooser_dialog_new ("Open File",
                                      GTK_WINDOW(litos->window),
                                      GTK_FILE_CHOOSER_ACTION_OPEN,
                                      _("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      _("_Open"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

    if (litos->filename[litos->page] != NULL)
    { 
        /* To let Open dialog show the files within current DIR of file already opened*/

        gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER (dialog), g_path_get_dirname(litos->filename[litos->page]));
    }

    response = gtk_dialog_run (GTK_DIALOG (dialog));

    if (response == GTK_RESPONSE_ACCEPT)
    {
        GtkTextBuffer *buffer = get_current_buffer(litos);
        GFile *file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));

		if (litos->filename[0] != NULL)
		{
			int total_pages = gtk_notebook_get_n_pages(litos->notebook);

			int i;

			for (i = 0; i < total_pages; i++)	/* Check whether the file is already opened: in positive case focus on its tab */
			{
				if (litos->filename[i] == NULL)
					continue;

				else
				{
						if (strcmp(litos->filename[i],g_file_get_path (file)) == 0)
						{
							gtk_notebook_set_current_page (litos->notebook,i);

							gtk_widget_grab_focus(GTK_WIDGET(currentTabSourceView(litos)));

							gtk_widget_destroy (dialog);

							return;
						}
					}
				}
		}
 
        if ((gtk_text_buffer_get_char_count(buffer)) != 0)
		{
			menu_newtab(NULL, litos);
			litos->filename[litos->page] = g_file_get_path(file);
		}

		else
			litos->filename[litos->page] = g_file_get_path(file);	

		g_object_unref(file);
        open_file (file, litos);
    }

    gtk_widget_destroy (dialog);
}

void openFromTemplate (GtkWidget *widget, gpointer userData)
{
	(void)widget;

	struct lit *litos = (struct lit*)userData;

	GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open File",
                                      GTK_WINDOW(litos->window),
                                      GTK_FILE_CHOOSER_ACTION_OPEN,
                                      _("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      _("_Open"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

	if ((gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), g_get_user_special_dir (G_USER_DIRECTORY_TEMPLATES))) == FALSE)
		fprintf(stderr, "error opening TEMPLATES directory chek wheter $HOME/.config/user-dirs.dirs contains XDG_TEMPLATES_DIR=\"$HOME/Templates\"");

	gint response = gtk_dialog_run (GTK_DIALOG (dialog));

	if (response == GTK_RESPONSE_ACCEPT)
	{
		GtkTextBuffer *buffer = get_current_buffer(litos);

    	if ((gtk_text_buffer_get_char_count(buffer)) != 0)
			menu_newtab(NULL, litos);

		GFile *file = gtk_file_chooser_get_file (GTK_FILE_CHOOSER (dialog));

		g_object_unref(file);

		litos->isTemplate = TRUE;

		open_file (file, litos);
    }

	gtk_widget_destroy (dialog);
}

void about_dialog (GtkButton *button, gpointer userData)
{
	(void)button;
	(void)userData;

	const gchar *authors[] = {"Giovanni Resta", "giovannirestadev@gmail.com", NULL};

	gtk_show_about_dialog (NULL,
			"program-name", "Litos",
			"version", VERSION,
			"license-type", GTK_LICENSE_GPL_3_0,
			"website", "https://github.com/gioretikto/litos",
			"authors", authors,
			"logo-icon-name", "start-here",
			"title", ("Litos"),
			NULL);
}
