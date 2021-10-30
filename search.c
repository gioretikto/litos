#include "litos.h"

GtkTextBuffer* get_current_buffer(struct lit *litos);
void ctrlF (GtkButton *button, gpointer userData);

void clearSearchHighlight(GObject *gobject, GParamSpec *pspec, gpointer userData);

extern GtkWidget *search_entry, *replace_entry, *check_case;

/* Called when Find button is clicked  */
void findButtonClicked (GtkButton *button, gpointer userData)
{
	(void)button;

	struct lit *litos = (struct lit*)userData;

	const gchar *searchString = NULL;

	GtkSourceSearchContext *search_context;
	GtkSourceSearchSettings *settings;

	searchString = gtk_entry_get_text(GTK_ENTRY(search_entry));

	g_print("%s\n",searchString);

	if (searchString != NULL)
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

	const gchar *searchString = NULL;

    GtkTextIter start_sel, end_sel;

	GtkTextBuffer *buffer = get_current_buffer(litos);

	if(gtk_text_buffer_get_selection_bounds(buffer, &start_sel, &end_sel))
	{
		GtkSourceSearchSettings *settings;

		settings = gtk_source_search_settings_new ();

		GtkSourceSearchContext *search_context = gtk_source_search_context_new(GTK_SOURCE_BUFFER(buffer), settings);

		searchString = gtk_text_buffer_get_text (buffer,
		                     &start_sel,
		                      &end_sel,
		                      FALSE);

		gtk_source_search_settings_set_search_text (settings, searchString);

		g_print("%s\n",searchString);

		g_signal_connect (GTK_SOURCE_BUFFER(get_current_buffer(litos)), "notify::text", G_CALLBACK (clearSearchHighlight), search_context);

	}
}

/* Called when Ctrl+B, Ctrl-i, etc is toggled */
void applyTags (struct lit *litos)
{
	char *string = NULL;

    GtkTextIter start_sel, end_sel;

	GtkTextBuffer *buffer = get_current_buffer(litos);

	if (gtk_text_buffer_get_selection_bounds(buffer, &start_sel, &end_sel))
	{
		string = gtk_text_buffer_get_text (buffer,
		                     &start_sel,
		                      &end_sel,
		                      FALSE);

		char replaceString[100] = { 0 };

		snprintf(replaceString, sizeof(replaceString), "<%c>%s</%c>", litos->tag, string, litos->tag);
		gtk_text_buffer_delete (buffer, &start_sel, &end_sel);
		gtk_text_buffer_insert (buffer, &start_sel, replaceString, (gint)strlen(replaceString));

	}
}


void clearSearchHighlight(GObject *gobject, GParamSpec *pspec, gpointer userData)	/*Function called when the file gets modified */
{
	(void)pspec;

	g_print("OK\n");

	gtk_source_search_context_set_highlight
		(userData,
		FALSE);

	g_signal_handlers_disconnect_by_func(gobject, G_CALLBACK(clearSearchHighlight), userData);
}
