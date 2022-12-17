#include "litos.h"

GtkTextBuffer* get_current_buffer(struct lit *litos);
GtkSourceView* currentTabSourceView(struct lit *litos);

void clearSearchHighlight(GObject *gobject, GParamSpec *pspec, gpointer userData);

extern GtkWidget *search_entry, *replace_entry, *button_check_case;

void freeSearchContext(struct lit *litos);

extern GtkWidget *lbl_number_occurences;

void searchString(struct lit *litos, const char *stringToSearch)
{
	GtkSourceSearchSettings *settings = gtk_source_search_settings_new ();

	GtkSourceBuffer *buffer = GTK_SOURCE_BUFFER(get_current_buffer(litos));

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button_check_case)))
		gtk_source_search_settings_set_case_sensitive (settings, TRUE);

	else
		gtk_source_search_settings_set_case_sensitive (settings, FALSE);

	gtk_source_search_settings_set_search_text (settings, stringToSearch);

	if (litos->search_context != NULL)
		freeSearchContext(litos);

	else
		litos->search_context = gtk_source_search_context_new(GTK_SOURCE_BUFFER(get_current_buffer(litos)), settings);

	/* Count occurences */

	GtkTextIter iter, start, end;

	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(buffer), &iter);

	gtk_source_search_context_forward (litos->search_context, &iter, &start, &end, NULL);

	gint counter = gtk_source_search_context_get_occurrences_count (litos->search_context);

	char str[80];

	sprintf(str, "%d occurences", counter);

	gtk_label_set_label (
		GTK_LABEL(lbl_number_occurences),
		str
	);

	g_print("%d occurences\n", counter);
}

void highlightWord(struct lit *litos)
{
	GtkSourceBuffer *buffer = GTK_SOURCE_BUFFER(get_current_buffer(litos));

	GtkTextIter start_sel, end_sel;

	gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(buffer), &start_sel, &end_sel);

	GtkTextIter current_loc;

	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(buffer), &current_loc);

	gtk_source_search_context_forward (litos->search_context, &current_loc, &start_sel, &end_sel, NULL);

	gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(currentTabSourceView(litos)),
			&start_sel,
			0,
			TRUE,
			0.0,
			0.0);

	g_signal_connect (buffer, "notify::text", G_CALLBACK (clearSearchHighlight), litos); /* when file gets modified, remove search highlights*/
}

/* Called when Ctrl+F or search button are toggled */
void searchWord (GtkButton *button, gpointer userData)
{
	(void)button;

	struct lit *litos = (struct lit*)userData;

	const char *stringToSearch = NULL;

	GtkTextBuffer *buffer = get_current_buffer(litos);

	GtkTextIter start, end;

	if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) /* TRUE if some text is selected; places the bounds of the selection in start and end (if the selection has 																	   length 0, then start and end are filled in with the same value)*/
	{
		stringToSearch = gtk_text_buffer_get_text (buffer,			/* Extract the selected string from the buffer */
							&start,
							&end,
							FALSE);

		gtk_entry_set_text(GTK_ENTRY(search_entry), gtk_text_iter_get_text (&start, &end));
	}

	else
		stringToSearch = gtk_entry_get_text(GTK_ENTRY(search_entry));

	if (stringToSearch == NULL || *stringToSearch == '\0')
		return;

	searchString(litos, stringToSearch);

	highlightWord(litos);
}

void replaceButtonClicked (GtkButton *button, gpointer userData)
{
	(void)button;

	struct lit *litos = (struct lit*)userData;

	const gchar *stringToSearch = gtk_entry_get_text(GTK_ENTRY(search_entry));
	const gchar *replaceString = gtk_entry_get_text(GTK_ENTRY(replace_entry));

	if (stringToSearch != NULL && replaceString != NULL)
		searchString(litos, stringToSearch);

	else
		return;

	gtk_source_search_context_replace_all (litos->search_context,
			replaceString,
			-1,
			NULL);

	gtk_entry_set_text(GTK_ENTRY(replace_entry),"");

    /* Highlight the replaced string */

	searchString(litos, replaceString);  /* 1st set the search_context then highlight*/

	highlightWord(litos);
}

/* Called when Ctrl+B, Ctrl-i, etc is toggled */
void applyTags (struct lit *litos, const char *tag)
{
	char *string = NULL;

	char replaceString[350] = { 0 };

	GtkTextIter start_sel, end_sel;

	GtkTextBuffer *buffer = get_current_buffer(litos);

	if (gtk_text_buffer_get_selection_bounds(buffer, &start_sel, &end_sel))
	{
		string = gtk_text_buffer_get_text (buffer,
		                     &start_sel,
		                      &end_sel,
		                      FALSE);

		snprintf(replaceString, sizeof(replaceString), "<%s>%s</%s>", tag, string, tag);
		gtk_text_buffer_delete (buffer, &start_sel, &end_sel);
		gtk_text_buffer_insert (buffer, &start_sel, replaceString, (gint)strlen(replaceString));
	}

	else
	{
		snprintf(replaceString, sizeof(replaceString), "</%s>", tag);
		gtk_text_buffer_insert_at_cursor (buffer, replaceString,(gint)strlen(replaceString));
	}
}

/* Called when Ctrl+m, Ctrl+l is toggled: insert '−' character */
void insertChar (struct lit *litos, const char *insertChar)
{
	GtkTextBuffer *buffer = get_current_buffer(litos);

	gtk_text_buffer_insert_at_cursor (buffer, insertChar, (gint)strlen(insertChar));
}

void clearSearchHighlight(GObject *gobject, GParamSpec *pspec, gpointer userData)	/*Function called when the file gets modified to remove seach highlights */
{
	(void)pspec;

	struct lit *litos = (struct lit*)userData;

	if(litos->search_context != NULL)
	{
		g_signal_handlers_disconnect_by_func(gobject, G_CALLBACK(clearSearchHighlight), userData);
		freeSearchContext(litos);
	}
}
