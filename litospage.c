#include "litospage.h"
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gtksourceview/gtksource.h>

GtkWidget* MyNewSourceview()
{
	GtkWidget *source_view = NULL;
	GtkSourceBuffer *source_buffer = gtk_source_buffer_new(NULL);

	GtkSourceStyleSchemeManager *scheme_manager = gtk_source_style_scheme_manager_get_default();

	// Usa il nome dello schema desiderato
	const gchar *scheme_name = "Adwaita-dark"; // oppure "solarized-dark", "kate", ecc.

	GtkSourceStyleScheme *scheme = gtk_source_style_scheme_manager_get_scheme(scheme_manager, scheme_name);

	source_view = gtk_source_view_new_with_buffer(source_buffer);

	if (scheme != NULL)
		gtk_source_buffer_set_style_scheme(source_buffer, scheme);

	else
		g_warning("Schema '%s' non trovato. Verrà usato lo stile predefinito.", scheme_name);

	gtk_source_view_set_show_line_numbers(GTK_SOURCE_VIEW(source_view), TRUE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(source_view), GTK_WRAP_WORD_CHAR);

	return source_view;
}

struct Page litos_page_new_empty(const gchar *name)
{
	struct Page page = {0};

	page.name = g_strdup(name);

	page.gf = NULL;

	page.view = MyNewSourceview();
	page.buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(page.view));

	page.scrolled = gtk_scrolled_window_new();
	gtk_widget_set_hexpand(page.scrolled, TRUE);
	gtk_widget_set_vexpand(page.scrolled, TRUE);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(page.scrolled), page.view);

	page.lbl = gtk_label_new(page.name);
	page.tabbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	page.close_btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	return page;
}

struct Page litos_page_new_from_file(GFile *gf)
{
	struct Page page = {0};

	page.gf = gf;
	page.name = g_file_get_basename(gf);

	page.view = MyNewSourceview();
	page.buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(page.view));

	page.scrolled = gtk_scrolled_window_new();
	gtk_widget_set_hexpand(page.scrolled, TRUE);
	gtk_widget_set_vexpand(page.scrolled, TRUE);
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(page.scrolled), page.view);

	page.lbl = gtk_label_new(page.name);
	page.tabbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
	page.close_btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	return page;
}

