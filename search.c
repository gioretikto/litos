#include "litos.h"

GtkTextBuffer* get_current_buffer(struct lit *litos);
void ctrlF (GtkButton *button, gpointer userData);
void findButtonClicked (GtkButton *button, gpointer userData);
void my_grab_focus(struct lit *litos);
void replaceButtonClicked (GtkButton *button, gpointer userData);

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

	searchString = gtk_entry_get_text(GTK_ENTRY(search_entry));
	gtk_entry_get_text(GTK_ENTRY(replace_entry));

	gint page = gtk_notebook_get_current_page(litos->notebook);

	g_print("%s\n",searchString);

	if(searchString != NULL)
	{
		GtkTextIter start_match, end_match;
		GtkTextIter start_find, end_find;

		GtkTextBuffer *buffer = get_current_buffer(litos);

		gtk_text_buffer_get_start_iter(buffer, &start_find);
		gtk_text_buffer_get_end_iter(buffer, &end_find);

		GtkTextSearchFlags caseType = 0;

		if(!check_case)
			caseType = GTK_TEXT_SEARCH_CASE_INSENSITIVE;

		while (gtk_text_iter_forward_search(&start_find, searchString,
		            GTK_TEXT_SEARCH_TEXT_ONLY |
					caseType |
		            GTK_TEXT_SEARCH_VISIBLE_ONLY,
		            &start_match, &end_match, NULL))
			{
				gtk_text_buffer_apply_tag_by_name(buffer, "gray_bg",
				      &start_match, &end_match);

				gint offset = gtk_text_iter_get_offset(&end_match);

				gtk_text_buffer_get_iter_at_offset(buffer,
					&start_find, offset);
		    }
	}

	litos->search[page] = TRUE;

	searchString = NULL;
}

void replaceButtonClicked (GtkButton *button, gpointer userData)
{
	(void)button;

	struct lit *litos = (struct lit*)userData;

	GtkSourceSearchSettings *settings = gtk_source_search_settings_new ();

	GtkSourceSearchContext *search_context = gtk_source_search_context_new(litos->buffer, settings);

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
	gint page = gtk_notebook_get_current_page(litos->notebook);

	GtkTextBuffer *buffer = get_current_buffer(litos);

	gboolean selected = FALSE;

	selected = gtk_text_buffer_get_selection_bounds(buffer,
            &start_sel, &end_sel);

	if (selected && litos->search[page] == FALSE)
	{
		litos->search[page] = TRUE;
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


void find_clear_tags(struct lit *litos)
{
	gint page = gtk_notebook_get_current_page(litos->notebook);

	if(litos->search[page] == TRUE)
	{
		litos->search[page] = FALSE;

		GtkTextBuffer *buffer = get_current_buffer(litos);
		GtkTextIter start_find, end_find;
	  	gtk_text_buffer_get_start_iter(buffer, &start_find);
		gtk_text_buffer_get_end_iter(buffer, &end_find);

		gtk_text_buffer_remove_tag_by_name(buffer, "gray_bg",
		          &start_find, &end_find);
	}
}
