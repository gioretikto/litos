#include "litos.h"

GtkTextBuffer* get_current_buffer(struct lit *litos);
void ctrlF (GtkButton *button, gpointer userData);
void findButtonClicked (GtkButton *button, gpointer userData);
void replaceButtonClicked (GtkButton *button, gpointer userData);
void clearSearchHighlight(GObject *gobject, GParamSpec *pspec, gpointer userData);

GtkWidget *search_entry, *replace_entry, *check_case;

void createFindPopover(GtkMenuButton *find_menu_button, struct lit *litos)
{
	GtkWidget *vbox, *hbox_find, *hbox_replace, *label_find, *label_replace, *find_button, *replace_button, *popover;

	label_find = gtk_label_new ("Find:");
	label_replace = gtk_label_new ("Replace:");

	find_button = gtk_button_new_with_label("Find");
	replace_button = gtk_button_new_with_label("Replace All");

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	hbox_find = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	hbox_replace = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	check_case = gtk_check_button_new_with_mnemonic(_("_Match case"));

	search_entry = gtk_entry_new ();
	replace_entry = gtk_entry_new ();

	gtk_container_add (GTK_CONTAINER (vbox), hbox_find);
	gtk_container_add (GTK_CONTAINER (vbox), hbox_replace);

	gtk_container_add (GTK_CONTAINER (hbox_find), label_find);
	gtk_container_add (GTK_CONTAINER (hbox_find), search_entry);
	gtk_container_add (GTK_CONTAINER (hbox_find), find_button);
	gtk_container_add (GTK_CONTAINER (hbox_find), check_case);

	gtk_container_add (GTK_CONTAINER (hbox_replace), label_replace);
	gtk_container_add (GTK_CONTAINER (hbox_replace), replace_entry);
	gtk_container_add (GTK_CONTAINER (hbox_replace), replace_button);

	popover = gtk_popover_new (NULL);
	gtk_container_add (GTK_CONTAINER (popover), vbox);
	gtk_menu_button_set_popover (find_menu_button, popover);
	gtk_popover_set_position (GTK_POPOVER (popover), GTK_POS_RIGHT);
	
	gtk_widget_show_all (vbox);

	g_signal_connect (find_button, "clicked", G_CALLBACK (findButtonClicked), litos);
	g_signal_connect (replace_button, "clicked", G_CALLBACK (replaceButtonClicked), litos);
}

/* Called when Find button is clicked  */
void findButtonClicked (GtkButton *button, gpointer userData)
{
	(void)button;

	struct lit *litos = (struct lit*)userData;

	const gchar *searchString = NULL;

	GtkSourceSearchContext *search_context;
	GtkSourceSearchSettings *settings;

	searchString = gtk_entry_get_text(GTK_ENTRY(search_entry));
	gtk_entry_get_text(GTK_ENTRY(replace_entry));

	g_print("%s\n",searchString);

	if(searchString != NULL)
	{
		settings = gtk_source_search_settings_new ();

		search_context = gtk_source_search_context_new(GTK_SOURCE_BUFFER(get_current_buffer(litos)), settings);

		if(check_case)
			gtk_source_search_settings_set_case_sensitive
            	(settings,
                 	TRUE);
		else
			gtk_source_search_settings_set_case_sensitive
			(settings,
                   FALSE);

		gtk_source_search_settings_set_search_text (settings, searchString);

		g_signal_connect (GTK_SOURCE_BUFFER(get_current_buffer(litos)), "notify::text", G_CALLBACK (clearSearchHighlight), search_context);

		searchString = NULL;
	}
}

void replaceButtonClicked (GtkButton *button, gpointer userData)
{
	(void)button;

	struct lit *litos = (struct lit*)userData;

	GtkSourceSearchContext *search_context;
	GtkSourceSearchSettings *settings;

	settings = gtk_source_search_settings_new ();

	search_context = gtk_source_search_context_new(GTK_SOURCE_BUFFER(get_current_buffer(litos)), settings);

	const gchar *searchString = NULL;
	const gchar *replaceString = NULL;

	searchString = gtk_entry_get_text(GTK_ENTRY(search_entry));
	replaceString = gtk_entry_get_text(GTK_ENTRY(replace_entry));

	gtk_source_search_settings_set_search_text (settings, searchString);

	g_print("%s\n",searchString);
	g_print("%s\n",replaceString);

	gtk_source_search_context_replace_all (search_context,
                                       replaceString,
                                       -1,
                                       NULL);

	searchString = NULL;
	replaceString = NULL;
}

/* Called when Ctrl+F is toggled */
void ctrlF (GtkButton *button, gpointer userData)
{
	(void)button;

	struct lit *litos = (struct lit*)userData;

	GtkTextIter start_find, end_find;
    GtkTextIter start_sel, end_sel;
	GtkTextIter start_match, end_match;

	gchar *text;

	GtkTextBuffer *buffer = get_current_buffer(litos);

	gboolean selected = FALSE;

	selected = gtk_text_buffer_get_selection_bounds(buffer,
            &start_sel, &end_sel);

	if (selected)
	{
        gtk_text_buffer_get_start_iter(buffer, &start_find);
        gtk_text_buffer_get_end_iter(buffer, &end_find);

        text = (gchar *) gtk_text_buffer_get_text(buffer, &start_sel,
            &end_sel, FALSE);

        while (gtk_text_iter_forward_search(&start_find, text,
                GTK_TEXT_SEARCH_TEXT_ONLY |
                GTK_TEXT_SEARCH_VISIBLE_ONLY,
                &start_match, &end_match, NULL))
		{
			gtk_text_buffer_apply_tag_by_name(buffer, "gray_bg",
		          &start_match, &end_match);
		
			gint offset = gtk_text_iter_get_offset(&end_match);

			gtk_text_buffer_get_iter_at_offset(buffer,
				&start_find, offset);
        }

		g_free(text);
	}
}


void clearSearchHighlight(GObject *gobject, GParamSpec *pspec, gpointer userData)	/*Function called when the file gets modified */
{
	(void)pspec;

	g_print("OK\n");

	gtk_source_search_context_set_highlight
		(userData,
		FALSE);

	//g_clear_object(&userData);

	g_signal_handlers_disconnect_by_func(gobject, G_CALLBACK(clearSearchHighlight), userData);
}
