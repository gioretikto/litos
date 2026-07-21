#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gtksourceview/gtksource.h>

#include "litospage.h"

static GtkWidget *litos_page_create_source_view(void)
{
	GtkSourceBuffer *source_buffer;
	GtkWidget *source_view;

	source_buffer = gtk_source_buffer_new(NULL);

	source_view =
		gtk_source_view_new_with_buffer(source_buffer);

	g_object_unref(source_buffer);

	gtk_source_view_set_show_line_numbers(
		GTK_SOURCE_VIEW(source_view),
		TRUE);

	gtk_source_view_set_auto_indent(
		GTK_SOURCE_VIEW(source_view),
		TRUE);

	gtk_source_view_set_tab_width(
		GTK_SOURCE_VIEW(source_view),
		4);

	gtk_text_view_set_wrap_mode(
		GTK_TEXT_VIEW(source_view),
		GTK_WRAP_WORD_CHAR);

	return source_view;
}

static void litos_page_create_widgets(struct Page *page)
{
	page->view = litos_page_create_source_view();

	page->buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(page->view));

	page->scrolled = gtk_scrolled_window_new();

	gtk_widget_set_hexpand(page->scrolled, TRUE);
	gtk_widget_set_vexpand(page->scrolled, TRUE);

	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(page->scrolled), page->view);

	page->lbl = gtk_label_new(page->name);

	gtk_label_set_single_line_mode(GTK_LABEL(page->lbl), TRUE);
	gtk_label_set_ellipsize(GTK_LABEL(page->lbl), PANGO_ELLIPSIZE_END);
	gtk_label_set_xalign(GTK_LABEL(page->lbl), 0.0);

	/* lascia spazio alla label dentro la tab */
	gtk_widget_set_hexpand(page->lbl, TRUE);
	gtk_widget_set_halign(page->lbl, GTK_ALIGN_FILL);

	/* Contenitore della tab */
	page->tabbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);

	page->tab_label_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

	gtk_widget_set_hexpand(page->tab_label_box, FALSE);
	gtk_widget_set_halign(page->tab_label_box, GTK_ALIGN_FILL);
}

struct Page litos_page_new_empty(const gchar *name)
{
	struct Page page = {0};

	page.name = g_strdup(name);
	page.gf = NULL;

	litos_page_create_widgets(&page);

	return page;
}

struct Page litos_page_new_from_file(GFile *gf)
{
	struct Page page = {0};

	page.gf = gf ? g_object_ref(gf) : NULL;

	if (gf)
		page.name = g_file_get_basename(gf);
	else
		page.name = g_strdup("");

	litos_page_create_widgets(&page);

	return page;
}
