#include "litos.h"
#define CANCEL 3

void open_file(struct lit *litos, gboolean template);
void menu_save (GtkWidget *widget, gpointer userData);
void save_as_dialog(struct lit *litos);
void save_as_file(GtkFileChooser *chooser, struct lit *litos);
void save_file(struct lit *litos);
void open_dialog (GtkWidget *widget, gpointer userData);
GtkSourceView* currentTabSourceView(struct lit *litos);
void menu_newtab (GtkWidget *widget, gpointer userData);

GtkTextBuffer* get_current_buffer(struct lit *litos);

void freePage(int page, struct lit *litos)
{
	if (litos->filename[page] != NULL)
	{
		g_free(litos->filename[page]);
		litos->filename[page] = NULL;
	}
}

unsigned int saveornot_before_close(gint page, struct lit *litos)
{
	GtkWidget *message_dialog;

	gint res;

	const gchar *filename = gtk_notebook_get_tab_label_text(
								litos->notebook,
								gtk_notebook_get_nth_page (litos->notebook, page)
							);

	message_dialog = gtk_message_dialog_new(GTK_WINDOW(litos->window), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING,
                      GTK_BUTTONS_NONE, "Save changes to document %s before closing?", filename);

	gtk_dialog_add_buttons (GTK_DIALOG(message_dialog), "Close without Saving", GTK_RESPONSE_REJECT,
                                                      "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT,  NULL);

	res = gtk_dialog_run(GTK_DIALOG(message_dialog));

	gtk_widget_destroy(message_dialog);

	switch (res)
	{
		case GTK_RESPONSE_ACCEPT:
			menu_save(NULL, litos);
			freePage(page, litos);
			return SAVE;
			break;

		case GTK_RESPONSE_REJECT:
			gtk_notebook_remove_page(litos->notebook, page);
			freePage(page, litos);
			return CLOSE;
			break;

		case GTK_RESPONSE_CANCEL:
			return CANCEL;
			break;

		default: /*close bottun was pressed*/
			g_print("The bottun(Close without Saving/Cancel/Save) was not pressed.");
	}

	if (litos->filename[page] != NULL)
	{
		g_free(litos->filename[page]);
		litos->filename[page] = NULL;
	}

	return 0;
}

void open_dialog (GtkWidget *widget, gpointer userData)
{
	(void)widget;

	struct lit *litos = (struct lit*)userData;

	GtkWidget *dialog;

	gint res;

	dialog = gtk_file_chooser_dialog_new ("Open File",
                                      GTK_WINDOW(litos->window),
                                      GTK_FILE_CHOOSER_ACTION_OPEN,
                                      _("_Cancel"),
                                      GTK_RESPONSE_CANCEL,
                                      _("_Open"),
                                      GTK_RESPONSE_ACCEPT,
                                      NULL);

	res = gtk_dialog_run (GTK_DIALOG (dialog));

	if (res == GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		GtkTextBuffer *current_buffer = get_current_buffer(litos);

    	if ((gtk_text_buffer_get_char_count(current_buffer)) != 0)
		    	menu_newtab(NULL, litos);

		litos->filename[gtk_notebook_get_current_page(litos->notebook)] = gtk_file_chooser_get_filename (chooser);
   		open_file (litos, FALSE);
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

	gint res;

	if ((gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), g_get_user_special_dir (G_USER_DIRECTORY_TEMPLATES))) == FALSE)
		fprintf(stderr, "error opening TEMPLATES directory chek wheter $HOME/.config/user-dirs.dirs contains XDG_TEMPLATES_DIR=\"$HOME/Templates\"");

	res = gtk_dialog_run (GTK_DIALOG (dialog));

	if (res == GTK_RESPONSE_ACCEPT)
	{
		GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
		GtkTextBuffer *current_buffer = get_current_buffer(litos);

    	if ((gtk_text_buffer_get_char_count(current_buffer)) != 0)
		    	menu_newtab(NULL, litos);

		litos->filename[gtk_notebook_get_current_page(litos->notebook)] = gtk_file_chooser_get_filename (chooser);
   		open_file (litos, TRUE);
	}

	gtk_widget_destroy (dialog);
}

void save_as_dialog(struct lit *litos)
{
	GtkWidget *dialog;

	GtkFileChooser *chooser;

	gint res;

	dialog = gtk_file_chooser_dialog_new ("Save File",
		                                  GTK_WINDOW(litos->window),
		                                  GTK_FILE_CHOOSER_ACTION_SAVE,
		                                  _("_Cancel"),
		                                  GTK_RESPONSE_CANCEL,
		                                  _("_Save"),
		                                  GTK_RESPONSE_ACCEPT,
		                                  NULL);
	chooser = GTK_FILE_CHOOSER (dialog);

	gtk_file_chooser_set_do_overwrite_confirmation (chooser, TRUE);

	res = gtk_dialog_run (GTK_DIALOG (dialog));

	if (res == GTK_RESPONSE_ACCEPT)
		save_as_file (chooser, litos);

	gtk_widget_destroy (dialog);
}

void about_dialog (GtkButton *button, gpointer userData)
{
	(void)button;
	(void)userData;

	const gchar *authors[] = {"Giovanni Resta", "giovannirestadev@gmail.com", NULL};
	
	gtk_show_about_dialog (NULL,
                       "program-name", "Litos",
                       "version", "0.0.1",
    				   "license-type", GTK_LICENSE_GPL_3_0,
    				   "website", "https://github.com/gioretikto/litos",
					   "authors", authors,
    				   "logo-icon-name", "start-here",
                       "title", ("Litos"),
                       NULL);
}
