#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

#include "litosapp.h"
#include "litosappwin.h"
#include "litosfile.h"
#include "page.h"

GFile *litos_file_get_gfile(LitosFile* file);
gboolean litos_file_save(LitosFile *file, GError *error);
void litos_file_save_as(LitosFile* file, GFile *new_file);
gchar *litos_file_get_name(LitosFile *file);
void litos_file_highlight_buffer(LitosFile *file);

LitosFile * litos_file_set(struct Page *page);
gboolean litos_file_get_saved(LitosFile *file);
GtkWidget * litos_file_get_view(LitosFile *file);
GtkWidget * litos_file_get_lbl(LitosFile *file);
GtkWidget * litos_file_get_tabbox(LitosFile *file);
gboolean litos_file_load (LitosFile *file, GError **error);

GSettings *litos_app_get_settings(LitosApp *app);
GtkCssProvider * litos_app_get_css_provider(LitosApp *app);
GSettings *litos_app_get_settings(LitosApp *app);

GtkWidget* MyNewSourceview();

gboolean litos_app_window_remove_child(LitosAppWindow *win);

struct _LitosAppWindow
{
	GtkApplicationWindow parent;

	GSettings *settings;
	GtkNotebook *notebook;
	GtkWidget *gears;
	GtkWidget *search;
	GtkWidget *searchbar;
	GtkWidget *about;
	GtkWidget *up_search;
	GtkWidget *down_search;
	GtkWidget *button_check_case;
	GtkSourceSearchContext *search_context;
	GPtrArray *litosFileList;
	gboolean quit;
};

G_DEFINE_TYPE (LitosAppWindow, litos_app_window, GTK_TYPE_APPLICATION_WINDOW);

static gboolean func (gconstpointer array_element, gconstpointer tabbox)
{
	return litos_file_get_tabbox ((LITOS_FILE((void*)array_element))) == tabbox;
}

guint litos_app_window_get_array_len(LitosAppWindow *win)
{
	return win->litosFileList->len;
}

guint litos_app_window_search_file(LitosAppWindow *win)
{
	guint index;

	if (litos_app_window_get_array_len(win) != 0)
	{
		gint current_page = gtk_notebook_get_current_page (win->notebook);

		GtkWidget *tabbox = gtk_notebook_get_nth_page (win->notebook, current_page);

		if (g_ptr_array_find_with_equal_func(win->litosFileList, tabbox, func, &index))
			return index;

		else
			return -1;
	}
}

LitosFile * litos_app_window_current_file(LitosAppWindow *win)
{
	guint index = litos_app_window_search_file(win);

	if (index == -1)
	{
		printf("The list is empty\n");
		return NULL;
	}

	else
		return g_ptr_array_index(win->litosFileList, index);
}

static
GtkSourceView* currentTabSourceView(LitosAppWindow *win)
{
	LitosFile *file = litos_app_window_current_file(win);

	return GTK_SOURCE_VIEW(litos_file_get_view(file));
}

static void
next_match(GtkWidget *close_btn, gpointer user_data)
{
	LitosAppWindow *win = LITOS_APP_WINDOW(user_data);

	if (win->search_context != NULL)
	{
		GtkTextIter match_start, match_end;

		GtkSourceView *view = currentTabSourceView(win);

		GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(view));
		gtk_text_buffer_select_range (buffer, &match_start, &match_end);

		GtkTextMark *insert;

		gtk_text_buffer_select_range (buffer,
				      &match_start,
				      &match_end);

		insert = gtk_text_buffer_get_insert (buffer);

		gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (view), insert);	
	}	
}

static GtkSourceView *
litos_app_window_set_search_context(LitosAppWindow *win, const char *stringToSearch)
{
	GtkSourceSearchSettings *settings = gtk_source_search_settings_new ();

	GtkSourceView *source_view = currentTabSourceView(win);

	GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(source_view));

	gtk_source_search_settings_set_search_text (settings, stringToSearch);

	win->search_context = gtk_source_search_context_new(GTK_SOURCE_BUFFER(buffer), settings);

	return source_view;
}

static void
search_text_changed (GtkEntry *entry,
                     LitosAppWindow *win)
{
	const char *text;
	GtkTextBuffer *buffer;
	GtkTextIter start, match_start, match_end;
	GtkTextMark* mark;

	if (win->search_context != NULL)
	{
		g_object_unref(win->search_context);
		win->search_context = NULL;
	}

	text = gtk_editable_get_text (GTK_EDITABLE (entry));

	if (text[0] == '\0')
		return;

	GtkSourceView *view = litos_app_window_set_search_context(win, text);

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(view));

	mark = gtk_text_buffer_get_insert(buffer);

	/* Very simple-minded search implementation */

	gtk_text_buffer_get_start_iter (buffer, &start);

	if (gtk_source_search_context_forward (win->search_context, &start, &match_start, &match_end, FALSE))
	{
		GtkTextMark *insert;

		gtk_text_buffer_select_range (buffer,
					      &match_start,
					      &match_end);

		insert = gtk_text_buffer_get_insert (buffer);

		/*gtk_text_view_scroll_mark_onscreen (GTK_TEXT_VIEW (view),
						    insert);
		gtk_text_buffer_select_range (buffer, &match_start, &match_end);
		gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW(view), &match_start,
				0.0, FALSE, 0.0, 0.0);*/
		gtk_text_buffer_move_mark (buffer, mark, &match_end);
	}
}

static void
visible_child_changed (GObject *notebook,
			GParamSpec *pspec,
			LitosAppWindow *win)
{
	if (gtk_widget_in_destruction (GTK_WIDGET (notebook)))
		return;

	gtk_search_bar_set_search_mode (GTK_SEARCH_BAR (win->searchbar), FALSE);
}

static void
litos_app_window_save_finalize (GtkWidget *dialog, gint response, gpointer win)
{
	GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);

	if (response == GTK_RESPONSE_ACCEPT)
	{
		LitosFile *file = litos_app_window_current_file(win);

		g_autoptr (GFile) gfile = gtk_file_chooser_get_file(chooser);

		litos_file_save_as (file, gfile);

		gtk_label_set_text (GTK_LABEL(litos_file_get_lbl(file)), litos_file_get_name(file));
	}

	gtk_window_destroy (GTK_WINDOW (dialog));
}

static void
litos_app_window_save_as_dialog (GSimpleAction *action, GVariant *parameter, gpointer win)
{
	GtkWidget *dialog = gtk_file_chooser_dialog_new ("Save File",
		                                  NULL,
		                                  GTK_FILE_CHOOSER_ACTION_SAVE,
		                                 ("_Cancel"),
		                                  GTK_RESPONSE_CANCEL,
		                                 ("_Save"),
		                                  GTK_RESPONSE_ACCEPT,
		                                  NULL);

	gtk_window_set_transient_for(GTK_WINDOW(dialog), win);

	gtk_widget_show(dialog);

	g_signal_connect (dialog, "response", G_CALLBACK (litos_app_window_save_finalize), win);
}

gboolean litos_app_window_save(LitosAppWindow *win, LitosFile *file)
{
	char *filename = litos_file_get_name(file);

	if (litos_file_get_gfile(file) == NULL)
		litos_app_window_save_as_dialog(NULL, NULL, win);

	else
	{
		GError *error = NULL;

		if (!litos_file_save(file,error))
		{
			GtkWidget *message_dialog;
			message_dialog = gtk_message_dialog_new(GTK_WINDOW(win), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
			GTK_BUTTONS_CLOSE, "ERROR : Can't save %s.\n %s", filename, error->message);
			gtk_widget_show(message_dialog);
			g_error_free(error);
			return FALSE;
		}

		else
			return TRUE;
	}
}

static void
litos_app_window_remove_page(LitosAppWindow *win, LitosFile *file)
{
	gtk_notebook_remove_page (win->notebook,gtk_notebook_get_current_page(win->notebook));
	g_ptr_array_remove(win->litosFileList, file);
}

static void
litos_app_window_saveornot_dialog_cb(GtkWidget *dialog, int response, gpointer window)
{
	GtkApplication *app = gtk_window_get_application(window);

	LitosAppWindow *win = LITOS_APP_WINDOW(window);

	LitosFile *file = litos_app_window_current_file(win);

	switch (response)
	{
		case GTK_RESPONSE_ACCEPT:
			litos_app_window_remove_page(win,file);		
			break;

		case GTK_RESPONSE_CANCEL:
			win->quit = FALSE;
			break;

		case GTK_RESPONSE_REJECT:
			litos_app_window_remove_page(win,file);
			break;

		default: /*close bottun was pressed*/
			win->quit = FALSE;
			g_print("The bottun(Close without Saving/Cancel/Save) was not pressed.");
	}

	gtk_window_destroy (GTK_WINDOW (dialog));

	if (win->quit == TRUE)
	{
		if (win->litosFileList->len == 0)
			g_application_quit (G_APPLICATION (app));
		else
			litos_app_window_remove_child(win);
	}
}

static void
litos_app_window_saveornot_dialog(LitosAppWindow *win, LitosFile *file)
{
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new(GTK_WINDOW(win), GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING,
		      GTK_BUTTONS_NONE, "Save changes to document %s before closing?", litos_file_get_name(file));

	gtk_dialog_add_buttons (GTK_DIALOG(dialog), "Close without Saving", GTK_RESPONSE_REJECT,
		                                      "Cancel", GTK_RESPONSE_CANCEL, "Save", GTK_RESPONSE_ACCEPT,  NULL);

	gtk_widget_show(dialog);

	g_signal_connect (dialog, "response", G_CALLBACK (litos_app_window_saveornot_dialog_cb), win);
}

gboolean litos_app_window_remove_child(LitosAppWindow *win)
{
	GtkWidget *tabbox = gtk_notebook_get_nth_page (win->notebook, gtk_notebook_get_current_page ((win->notebook)));

	if (tabbox != NULL)
	{
		LitosFile *file = litos_app_window_current_file(win);

		if (litos_file_get_saved(file))
		{
			litos_app_window_remove_page(win,file);
			return TRUE;
		}

		else
			litos_app_window_saveornot_dialog(win, file);
	}

	return FALSE;	
}

gboolean litos_app_window_quit (GtkWindow *window, gpointer user_data)
{
	LitosAppWindow *win = LITOS_APP_WINDOW(user_data);
	GtkApplication *app = gtk_window_get_application(window);

	if (litos_app_window_get_array_len(win) == 0)
		g_application_quit (G_APPLICATION (app));
	
	else
	{
		win->quit = TRUE;
		while (win->quit == TRUE && litos_app_window_remove_child(win))
			;
	}

	return TRUE;
}

static void
litos_app_window_update_font ()
{
	LitosApp *app = LITOS_APP(g_application_get_default());

	PangoFontDescription *font_desc;
	gchar *font, *css_string;
	GtkCssProvider *css_provider;

	font = g_settings_get_string (litos_app_get_settings(app),"font");

	font_desc = pango_font_description_from_string (font);
	css_provider = litos_app_get_css_provider(app);
	g_free (font);

	/* update font */
	css_string = g_strdup_printf ("textview { font: %dpt %s; }",
			pango_font_description_get_size (font_desc) / PANGO_SCALE,
			pango_font_description_get_family (font_desc));
	gtk_css_provider_load_from_data (css_provider, css_string, -1);

	gtk_style_context_add_provider_for_display (gdk_display_get_default(),
                                              GTK_STYLE_PROVIDER (css_provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

	/* cleanup */
	pango_font_description_free (font_desc);
	g_free (css_string);
}

static void
litos_app_window_init (LitosAppWindow *win)
{
	GtkBuilder *builder;
	GMenuModel *menu;
	LitosApp *app = LITOS_APP(g_application_get_default());

	gtk_widget_init_template (GTK_WIDGET (win));

	builder = gtk_builder_new_from_resource ("/org/gtk/litos/gears-menu.ui");
	menu = G_MENU_MODEL (gtk_builder_get_object (builder, "menu"));
	gtk_menu_button_set_menu_model (GTK_MENU_BUTTON (win->gears), menu);
	g_object_unref (builder);

	win->settings = g_settings_new ("org.gtk.litos");
	gtk_widget_set_sensitive (win->search, TRUE);

	win->quit = FALSE;
	win->search_context = NULL;
	win->litosFileList = g_ptr_array_new_full(0, g_object_unref);

	g_signal_connect (litos_app_get_settings(app), "changed::font-desc", G_CALLBACK (litos_app_window_update_font), NULL);

	g_signal_connect (GTK_WINDOW(win), "close-request", G_CALLBACK (litos_app_window_quit), win);
	g_signal_connect (win->down_search, "clicked", G_CALLBACK(next_match), win);
	//g_signal_connect (win->up_search, "clicked", G_CALLBACK(previous_match), win);
	
	g_object_bind_property (win->search, "active",
		win->searchbar, "search-mode-enabled",
		G_BINDING_BIDIRECTIONAL);
}

static void
litos_app_window_dispose (GObject *object)
{
	LitosAppWindow *win;

	win = LITOS_APP_WINDOW (object);

	g_clear_object (&win->settings);

	g_ptr_array_unref(win->litosFileList);

	G_OBJECT_CLASS (litos_app_window_parent_class)->dispose (object);
}

void about_dialog (GtkButton *button, gpointer userData)
{
	const gchar *authors[] = {"Giovanni Resta", "giovannirestadev@gmail.com", NULL};

	gtk_show_about_dialog (NULL,
			"program-name", "Litos",
			"version", "4.0.0",
			"license-type", GTK_LICENSE_GPL_3_0,
			"website", "https://github.com/gioretikto/litos",
			"authors", authors,
			"logo-icon-name", "start-here",
			"title", ("Litos"),
			NULL);
}

static void
litos_app_window_class_init (LitosAppWindowClass *class)
{
	G_OBJECT_CLASS (class)->dispose = litos_app_window_dispose;

	gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
						"/org/gtk/litos/window.ui");
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), LitosAppWindow, notebook);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), LitosAppWindow, gears);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), LitosAppWindow, search);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), LitosAppWindow, searchbar);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), LitosAppWindow, about);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), LitosAppWindow, up_search);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), LitosAppWindow, down_search);
	//gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), LitosAppWindow, button_check_case);

	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), about_dialog);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), search_text_changed);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), visible_child_changed);
}

LitosAppWindow *
litos_app_window_new (LitosApp *app)
{
	return g_object_new (LITOS_APP_WINDOW_TYPE, "application", app, NULL);
}

void litos_app_window_save_as(LitosAppWindow *win)
{
	litos_app_window_save_as_dialog(NULL, NULL, win);
}

static void
close_btn_clicked(GtkWidget *close_btn, gpointer user_data)
{
	LitosAppWindow *win = LITOS_APP_WINDOW(user_data);
	litos_app_window_remove_child(win);

	g_signal_handlers_disconnect_by_func(close_btn, close_btn_clicked, win);
}

static void
lblToColor(LitosAppWindow *win, LitosFile* file, const char *color)
{
	const char *markup = g_markup_printf_escaped ("<span color='%s'>\%s</span>", color, litos_file_get_name(file));

	gtk_label_set_markup (GTK_LABEL(litos_file_get_lbl(file)), markup);
}

static void
_file_monitor_saved_change(GObject *gobject, GParamSpec *pspec, gpointer win)
{
	LitosAppWindow *lwin = LITOS_APP_WINDOW(win);
	LitosFile *file = LITOS_FILE(gobject);

	if (litos_file_get_saved(file) == FALSE)
		lblToColor(lwin, file, "red");

	else
		lblToColor(lwin, file, "gray");
}

void litos_app_error_dialog(GtkWindow *window, GError *error, char *filename)
{
	GtkWidget *message_dialog;
	message_dialog = gtk_message_dialog_new(window, GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
	GTK_BUTTONS_CLOSE, "ERROR : Can't load %s.\n %s", filename, error->message);
	gtk_widget_show(message_dialog);
	g_error_free(error);
}

static LitosFile *
litos_app_window_new_tab(LitosAppWindow *win, struct Page *page)
{
	GtkWidget *close_btn, *lbl;

	page->tabbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	page->close_btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
	page->scrolled = gtk_scrolled_window_new ();
	page->view = MyNewSourceview();

	close_btn = gtk_button_new_from_icon_name ("window-close-symbolic");

	gtk_widget_set_hexpand (page->scrolled, TRUE);
	gtk_widget_set_vexpand (page->scrolled, TRUE);

	gtk_scrolled_window_set_child (GTK_SCROLLED_WINDOW (page->scrolled), page->view);

	page->buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (page->view));

	page->lbl = gtk_label_new(page->name);

	LitosFile *file = litos_file_set(page);

	gtk_box_append (GTK_BOX(page->close_btn_box), page->lbl);
	gtk_box_append (GTK_BOX(page->close_btn_box), close_btn);
	gtk_box_append (GTK_BOX(page->tabbox), page->scrolled);

	gtk_notebook_set_current_page (
		win->notebook,
		gtk_notebook_append_page_menu (win->notebook, page->tabbox, page->close_btn_box, page->close_btn_box)
	);

	litos_app_window_update_font();

	gtk_widget_grab_focus(GTK_WIDGET(page->view));

	gtk_notebook_set_tab_reorderable(win->notebook, page->tabbox, TRUE);

	g_signal_connect(close_btn, "clicked", 
	    G_CALLBACK(close_btn_clicked), win);

	g_ptr_array_add(win->litosFileList, file);

	return file;
}

LitosFile * litos_app_window_open(LitosAppWindow *win, GFile *gf)
{
	struct Page page;

	GError *error = NULL;

	page.name = g_file_get_basename(gf);

	page.gf = gf;

	LitosFile *file = litos_app_window_new_tab(win,&page);

	if (!litos_file_load(file,&error))
	{
		litos_app_error_dialog(GTK_WINDOW(win), error, page.name);
		return NULL;
	}

	else
	{
		litos_file_highlight_buffer(file);

		g_signal_connect(G_OBJECT(file), "notify::saved", G_CALLBACK (_file_monitor_saved_change), win);

		return file;
	}
}

LitosFile * litos_app_window_new_file(LitosAppWindow *win)
{
	int file_index = litos_app_window_get_array_len(win) + 1;

	struct Page page;

	page.name = g_strdup_printf("Untitled %d", file_index);

	page.gf = NULL;

	LitosFile *file = litos_app_window_new_tab(win,&page);

	g_signal_connect(G_OBJECT(file), "notify::saved", G_CALLBACK (_file_monitor_saved_change), win);

	return file;
}

LitosFile * litos_app_window_get_file(LitosAppWindow *win, int *i)
{
	return g_ptr_array_index(win->litosFileList,*i);
}

GtkNotebook * litos_app_window_get_nb(LitosAppWindow *win)
{
	return win->notebook;
}
