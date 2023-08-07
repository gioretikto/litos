#include "litos.h"
#include <ctype.h>

GtkTextBuffer* get_current_buffer(struct lit *litos);
GtkSourceView* currentTabSourceView(struct lit *litos);

extern GtkWidget *search_entry, *replace_entry, *button_check_case;

extern GtkWidget *lbl_number_occurences;

extern GtkSourceBuffer *highlightSearchBuffer;
void countOccurences(gpointer search_context);
void highlightWord(struct lit *litos);

void clearSearchHighlight(GObject *gobject, GParamSpec *pspec, gpointer search_context)	/*Function called when the file gets modified to remove seach highlights */
{
	(void)pspec;

	if (gtk_source_search_context_get_highlight(search_context))
	{
		gtk_source_search_context_set_highlight
			(search_context,
			FALSE);

		printf("ptr = %p\n", (void *)search_context);
	}

	g_signal_handlers_disconnect_by_func(gobject, G_CALLBACK(clearSearchHighlight), search_context);
}

void clearSearchContext (struct lit *litos)
{
	if (litos->search_context != NULL)			/* Before making a new search clear the previous search context */
	{
		if (gtk_source_search_context_get_highlight(litos->search_context))
			clearSearchHighlight(G_OBJECT(highlightSearchBuffer), NULL, litos->search_context);

		g_object_unref(litos->search_context);
		litos->search_context = NULL;
	}
}

void searchString(struct lit *litos, const char *stringToSearch)
{
	GtkSourceSearchSettings *settings = gtk_source_search_settings_new ();

	clearSearchContext(litos);	/* Before making a new search clear the previous search context */

	highlightSearchBuffer = GTK_SOURCE_BUFFER(get_current_buffer(litos));

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button_check_case)))
		gtk_source_search_settings_set_case_sensitive (settings, TRUE);

	else
		gtk_source_search_settings_set_case_sensitive (settings, FALSE);

	gtk_source_search_settings_set_search_text (settings, stringToSearch);

	litos->search_context = gtk_source_search_context_new(highlightSearchBuffer, settings);
}

/* Called when Ctrl+F or search button are toggled */
void searchWord (GtkButton *button, gpointer userData)
{
	(void)button;

	struct lit *litos = (struct lit*)userData;

	const char *stringToSearch = NULL;

	GtkTextIter start, end;

	GtkTextBuffer *buffer = get_current_buffer(litos);

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

    /* Highlight the replaced string */

	searchString(litos, stringToSearch);	/* 1st set the search_context*/

	highlightWord(litos);					/* then highlight */

	countOccurences(litos->search_context);
}

void replaceButtonClicked (GtkButton *button, gpointer userData)
{
	(void)button;

	struct lit *litos = (struct lit*)userData;

	guint count_replaced = 0;

	const gchar *stringToSearch = gtk_entry_get_text(GTK_ENTRY(search_entry));
	const gchar *replaceString = gtk_entry_get_text(GTK_ENTRY(replace_entry));

	if (stringToSearch == NULL || replaceString == NULL)
		return;

	/* Search and Highlight replaced string */

	count_replaced = gtk_source_search_context_replace_all (litos->search_context,
			replaceString,
			-1,
			NULL);

	if (!isspace(replaceString[0])) /* Don't search and highlight strings with spaces */
	{
		searchString(litos, stringToSearch);

		GtkSourceSearchSettings *settings = gtk_source_search_settings_new ();

		clearSearchContext(litos);

		gtk_source_search_settings_set_case_sensitive (settings, TRUE);

		gtk_source_search_settings_set_search_text (settings, replaceString);

		highlightSearchBuffer = GTK_SOURCE_BUFFER(get_current_buffer(litos));

		litos->search_context = gtk_source_search_context_new(highlightSearchBuffer, settings);

		highlightWord(litos);
	}

	gtk_entry_set_text(GTK_ENTRY(replace_entry),"");

	char str[80];

	sprintf(str, "%d replacement", count_replaced);

	gtk_label_set_label (
		GTK_LABEL(lbl_number_occurences),
		str
	);

	g_print("%d replacement\n", count_replaced);
}

void highlightWord(struct lit *litos)
{
	GtkTextBuffer *buffer = GTK_TEXT_BUFFER(highlightSearchBuffer);

	GtkTextIter start_sel, end_sel;

	gtk_text_buffer_get_selection_bounds(buffer, &start_sel, &end_sel);

	GtkTextIter current_loc;

	gtk_text_buffer_get_start_iter (buffer, &current_loc);

	gtk_source_search_context_forward (litos->search_context, &current_loc, &start_sel, &end_sel, NULL);

	gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(currentTabSourceView(litos)),
			&start_sel,
			0,
			TRUE,
			0.0,
			0.0);

	g_signal_connect (highlightSearchBuffer, "notify::text", G_CALLBACK (clearSearchHighlight), litos->search_context); /* when file gets modified, remove search highlights*/
}

void countOccurences(gpointer search_context) /* Count occurences */
{
	GtkTextIter iter, start, end;

	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(highlightSearchBuffer), &iter);

	gtk_source_search_context_forward (search_context, &iter, &start, &end, NULL);

	gint counter = gtk_source_search_context_get_occurrences_count (search_context);

	char str[80];

	sprintf(str, "%d occurences", counter);

	gtk_label_set_label (
		GTK_LABEL(lbl_number_occurences),
		str
	);

	g_print("%d occurences\n", counter);
}
