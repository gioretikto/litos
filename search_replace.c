#include "litos.h"

GtkTextBuffer* get_current_buffer(struct lit *litos);
GtkSourceView* currentTabSourceView(struct lit *litos);
void ctrlF (GtkButton *button, gpointer userData);

void clearSearchHighlight(GObject *gobject, GParamSpec *pspec, gpointer userData);

extern GtkWidget *search_entry, *replace_entry, *button_check_case;

void highlightWord(struct lit *litos, const gchar *searchString)
{
	if (litos->search_context != NULL)
	{
		gtk_source_search_context_set_highlight
				(litos->search_context,
				FALSE);

		litos->search_context = NULL;
	}

	GtkSourceBuffer *buffer = GTK_SOURCE_BUFFER(get_current_buffer(litos));

	GtkTextIter start_sel, end_sel;

	GtkSourceSearchSettings *settings = gtk_source_search_settings_new ();

	litos->search_context = gtk_source_search_context_new(buffer, settings);

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button_check_case)))
		gtk_source_search_settings_set_case_sensitive (settings, TRUE);
	else
		gtk_source_search_settings_set_case_sensitive (settings, FALSE);

	gtk_source_search_settings_set_search_text (settings, searchString);

	gtk_text_buffer_get_selection_bounds(GTK_TEXT_BUFFER(buffer), &start_sel, &end_sel);

	GtkTextIter current_loc;

	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER(buffer), &current_loc);

	gtk_source_search_context_forward2 (litos->search_context, &current_loc, &start_sel, &end_sel, NULL);

	gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(currentTabSourceView(litos)),
	                         &start_sel,
	                         0,
	                         TRUE,
	                         0.0,
	                         0.0);

	g_signal_connect (buffer, "notify::text", G_CALLBACK (clearSearchHighlight), litos->search_context);

}

void findButtonClicked (GtkButton *button, gpointer userData)
{
	(void)button;

	struct lit *litos = (struct lit*)userData;

	const gchar *searchString = NULL;

	searchString = gtk_entry_get_text(GTK_ENTRY(search_entry));

	if (searchString != NULL)
		highlightWord(litos, searchString);
}

void replaceButtonClicked (GtkButton *button, gpointer userData)
{
	(void)button;

	struct lit *litos = (struct lit*)userData;

	GtkSourceSearchSettings *settings = gtk_source_search_settings_new ();

	if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button_check_case)))
		gtk_source_search_settings_set_case_sensitive (settings, TRUE);

	else
		gtk_source_search_settings_set_case_sensitive (settings, FALSE);

	litos->search_context = gtk_source_search_context_new(GTK_SOURCE_BUFFER(get_current_buffer(litos)), settings);

	const gchar *searchString = gtk_entry_get_text(GTK_ENTRY(search_entry));
	const gchar *replaceString = gtk_entry_get_text(GTK_ENTRY(replace_entry));

	gtk_source_search_settings_set_search_text (settings, searchString);

	gtk_source_search_context_replace_all (litos->search_context,
                                       replaceString,
                                       -1,
                                       NULL);

	highlightWord(litos, replaceString);
}

/* Called when Ctrl+F is toggled */
void ctrlF (GtkButton *button, gpointer userData)
{
	(void)button;

	struct lit *litos = (struct lit*)userData;

	const gchar *searchString = NULL;

	GtkTextBuffer *buffer = get_current_buffer(litos);

	GtkTextIter start_sel, end_sel;

	if (gtk_text_buffer_get_selection_bounds(buffer, &start_sel, &end_sel))
	{

		searchString = gtk_text_buffer_get_text (buffer,
		                     &start_sel,
		                      &end_sel,
		                      FALSE);

		gtk_entry_set_text(GTK_ENTRY(search_entry), gtk_text_iter_get_text (&start_sel, &end_sel));

		highlightWord(litos, searchString);

	}
}


/* Called when Ctrl+B, Ctrl-i, etc is toggled */
void applyTags (struct lit *litos, char *tag)
{
	char *string = NULL;

	char replaceString[100] = { 0 };

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
		if (*tag == 'b')
			gtk_text_buffer_insert_at_cursor (buffer, "<br>", (gint)strlen("<br>"));
		else
		{
			snprintf(replaceString, sizeof(replaceString), "</%s>", tag);
			gtk_text_buffer_insert_at_cursor (buffer, replaceString,(gint)strlen(replaceString));
		}
	}		
}

/* Called when Ctrl+m, Ctrl+l is toggled */
void insertChar (struct lit *litos, const char *insertChar)
{
	GtkTextBuffer *buffer = get_current_buffer(litos);

	gtk_text_buffer_insert_at_cursor (buffer, insertChar, (gint)strlen(insertChar));
}

void clearSearchHighlight(GObject *gobject, GParamSpec *pspec, gpointer userData)	/*Function called when the file gets modified */
{
	(void)pspec;

	gtk_source_search_context_set_highlight
		(userData,
		FALSE);

	userData = NULL;

	g_signal_handlers_disconnect_by_func(gobject, G_CALLBACK(clearSearchHighlight), userData);
}
