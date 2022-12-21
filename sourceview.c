#include "litos.h"

GtkSourceView* currentTabSourceView(struct lit *litos)
{

	GList *children = gtk_container_get_children(GTK_CONTAINER(gtk_notebook_get_nth_page(
		litos->notebook,
		(gint)litos->page
	)));

	return GTK_SOURCE_VIEW(gtk_bin_get_child(GTK_BIN(g_list_nth_data(children, 0))));
}

GtkWidget* MyNewSourceview(struct lit *litos)
{
	GtkWidget *source_view;

	litos->source_buffer = gtk_source_buffer_new (NULL);

	source_view = gtk_source_view_new_with_buffer (litos->source_buffer);

	gtk_source_buffer_set_style_scheme (litos->source_buffer,
	gtk_source_style_scheme_manager_get_scheme (
	gtk_source_style_scheme_manager_get_default (), "solarized-dark"));

	gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW((source_view)), TRUE);

	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(source_view), GTK_WRAP_WORD_CHAR);

	return source_view;
}

GtkTextBuffer* get_current_buffer(struct lit *litos)
{
	GtkSourceView *source_view;
	GtkTextBuffer *current_buffer;

	source_view = currentTabSourceView(litos);

  	current_buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(source_view));
  	
  	return current_buffer;
}

void highlight_buffer(struct lit *litos) /* Apply different font styles depending on file extension .html .c, etc */
{
	GtkSourceLanguageManager *lm = gtk_source_language_manager_get_default();

	GtkSourceLanguage *lang;

	if (litos->filename[litos->page] == NULL)
		lang = gtk_source_language_manager_get_language(lm,"html");
	else
		lang = gtk_source_language_manager_guess_language(lm, litos->filename[litos->page], NULL);
		
	gtk_source_buffer_set_language (litos->source_buffer, lang);

	if (lang != NULL)
		gtk_source_buffer_set_highlight_syntax (litos->source_buffer, TRUE);
}
