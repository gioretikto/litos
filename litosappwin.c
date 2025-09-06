/*	Copyright (C) 2023-2024 Giovanni Resta <giovannirestadev@gmail.com>

	This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

#include "litosapp.h"
#include "litosfile.h"
#include "page.h"
#include "litosappwin.h"

#define BIND_CHILD(x) gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), LitosAppWindow, x);

#define SCROLL_TO_MARK gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW(view), mark, 0, FALSE, 0.0,	0.0);

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
	GtkWidget *btn_find_icon;
	GtkWidget *searchbar;
	GtkWidget *search_entry;
	GtkWidget *replace_entry;
	GtkWidget *about_btn;
	GtkWidget *btn_prev;
	GtkWidget *btn_next;
	GtkWidget *btn_check_case;
	GtkWidget *btn_replace;
	GtkWidget *lbl_number_occurences;
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

	return -1;
}

LitosFile * litos_app_window_current_file(LitosAppWindow *win)
{
	int index = litos_app_window_search_file(win);

	if (index == -1)
	{
		printf("The list is empty\n");
		return NULL;
	}

	else
		return g_ptr_array_index(win->litosFileList, index);
}

static GtkSourceView* litos_app_window_current_tab_sourceView(LitosAppWindow *win)
{
	LitosFile *file = litos_app_window_current_file(win);

	return GTK_SOURCE_VIEW(litos_file_get_view(file));
}

static void litos_app_window_prev_match(GtkWidget *close_btn G_GNUC_UNUSED, gpointer user_data)
{
	LitosAppWindow *win = LITOS_APP_WINDOW(user_data);

	if (win->search_context != NULL)
	{
		GtkTextMark* mark;
		GtkSourceBuffer *buffer;
		GtkTextIter selection_begin, selection_end;
		GtkTextIter match_start, match_end;

		GtkSourceView *view = litos_app_window_current_tab_sourceView(win);

		buffer = gtk_source_search_context_get_buffer (win->search_context);

		gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER (buffer),
			&selection_begin, &selection_end);

		if (gtk_source_search_context_backward(win->search_context, &selection_begin, &match_start, &match_end, NULL))
		{

			mark = gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer));

			SCROLL_TO_MARK

			gtk_text_buffer_select_range (GTK_TEXT_BUFFER (buffer), &match_end, &match_start);

			SCROLL_TO_MARK
		}
	}
}

static void litos_app_window_next_match(GtkWidget *close_btn G_GNUC_UNUSED, gpointer user_data)
{
	LitosAppWindow *win = LITOS_APP_WINDOW(user_data);

	if (win->search_context != NULL)
	{
		GtkTextMark* mark;
		GtkSourceBuffer *buffer;
		GtkTextIter start, match_start, match_end;

		GtkSourceView *view = litos_app_window_current_tab_sourceView(win);

		buffer = gtk_source_search_context_get_buffer (win->search_context);

		gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER (buffer),
					      &match_start,
					      &start);

		if (gtk_source_search_context_forward (win->search_context, &start, &match_start, &match_end, NULL))
		{

			mark = gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer));

			SCROLL_TO_MARK

			gtk_text_buffer_select_range (GTK_TEXT_BUFFER (buffer), &match_start, &match_end);

			SCROLL_TO_MARK
		}
	}

}

static void litow_app_window_update_match_label(GtkSourceSearchContext *context,
                                     GParamSpec *pspec G_GNUC_UNUSED,
                                     LitosAppWindow *win)
{
	gint count = gtk_source_search_context_get_occurrences_count(context);

	if (count >= 0){
		char buf[64];
		g_snprintf(buf, sizeof(buf), "%d matches", count);
		gtk_label_set_text(GTK_LABEL(win->lbl_number_occurences), buf);
	}

	else {
		// opzionale: mostra "..." finché il conteggio non è pronto
		gtk_label_set_text(GTK_LABEL(win->lbl_number_occurences), "...");
	}
}

static GtkSourceView *
litos_app_window_set_search_context(LitosAppWindow *win, const char *stringToSearch)
{
	GtkSourceView *source_view = litos_app_window_current_tab_sourceView(win);
	if (!GTK_IS_TEXT_VIEW(source_view)) {
		g_warning("source_view non valido o non inizializzato");
		return NULL;
	}

	GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(source_view));
	if (!GTK_IS_TEXT_BUFFER(buffer)) {
		g_warning("buffer non valido");
		return NULL;
	}

	GtkSourceSearchSettings *settings = gtk_source_search_settings_new();
	gtk_source_search_settings_set_search_text(settings, stringToSearch);

	gboolean case_sensitive = gtk_check_button_get_active(GTK_CHECK_BUTTON(win->btn_check_case));
	gtk_source_search_settings_set_case_sensitive(settings, case_sensitive);

	// Disconnetti e libera il vecchio search_context se esiste
	if (win->search_context != NULL) {
		g_signal_handlers_disconnect_by_func(win->search_context,
			G_CALLBACK(litow_app_window_update_match_label),
			win);
		g_object_unref(win->search_context);
		win->search_context = NULL;
	}

	win->search_context = gtk_source_search_context_new(GTK_SOURCE_BUFFER(buffer), settings);
	g_object_unref(settings); // settings non più necessario

	if (win->search_context == NULL) {
		g_warning("Impossibile creare GtkSourceSearchContext");
		return NULL;
	}

	g_signal_connect(win->search_context,
		"notify::occurrences-count",
		G_CALLBACK(litow_app_window_update_match_label),
		win);

	return source_view;
}


static void litos_app_window_replace_btn_clicked (GtkButton *button G_GNUC_UNUSED, gpointer userData)
{
	LitosAppWindow *win = LITOS_APP_WINDOW(userData);

	if (win->search_context == NULL)
		return;
		
	guint count_replaced = 0;

	const gchar *stringToSearch = gtk_editable_get_text(GTK_EDITABLE(win->search_entry));
	const gchar *replaceString = gtk_editable_get_text(GTK_EDITABLE(win->replace_entry));

	if (stringToSearch == NULL || replaceString == NULL)
		return;
		
	GtkSourceSearchSettings *settings = gtk_source_search_settings_new ();

	if (gtk_check_button_get_active(GTK_CHECK_BUTTON(win->btn_check_case)))
		gtk_source_search_settings_set_case_sensitive (settings, TRUE);

	else
		gtk_source_search_settings_set_case_sensitive (settings, FALSE);

	gtk_source_search_settings_set_search_text (settings, stringToSearch);

	/* Search and Highlight replaced string */

	count_replaced = gtk_source_search_context_replace_all (win->search_context,
		replaceString,
		-1,
		NULL);

	gtk_editable_set_text(GTK_EDITABLE(win->replace_entry),"");

	char str[80];

	sprintf(str, "%d replacement", count_replaced);

	gtk_label_set_label (GTK_LABEL(win->lbl_number_occurences), str);

	g_object_unref(settings);
}

/* a word is entered or modified in the seach bar */

static void litos_app_window_search_text_changed (GtkEntry *entry, LitosAppWindow *win)
{
	const char *text;
	GtkTextBuffer *buffer;
	GtkTextIter start, match_start, match_end;
	GtkTextMark* mark;

	if (win->search_context != NULL)
	{
		g_signal_handlers_disconnect_by_func(win->search_context,
			G_CALLBACK(litow_app_window_update_match_label),
			win);
		g_object_unref(win->search_context);
		win->search_context = NULL;
	}

	text = gtk_editable_get_text (GTK_EDITABLE (entry));

	if (text[0] == '\0')
		return;

	GtkSourceView *view = litos_app_window_set_search_context(win, text);

	if (view == NULL)
		return;

	buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(view));

	mark = gtk_text_buffer_get_insert(buffer);

	SCROLL_TO_MARK

	/* Very simple-minded search implementation */

	gtk_text_buffer_get_start_iter (buffer, &start);

	if (gtk_source_search_context_forward (win->search_context, &start, &match_start, &match_end, FALSE))
	{
		gtk_text_buffer_move_mark (buffer, mark, &match_end);

		gtk_text_buffer_select_range (buffer,
					      &match_start,
					      &match_end);
	}
	
	gint counter = gtk_source_search_context_get_occurrences_count (win->search_context);

	char str[80];

	sprintf(str, "%d occurences", counter);

	gtk_label_set_label (GTK_LABEL(win->lbl_number_occurences),str);
}

void litos_app_window_set_search_entry(LitosAppWindow *win)
{
	GtkTextIter start, end;
	LitosFile *file = litos_app_window_current_file(win);

	GtkTextBuffer *buffer = litos_file_get_buffer(file);

	const char *stringToSearch = NULL;

	if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) 
	{
		stringToSearch = gtk_text_buffer_get_text (buffer,			
							&start,
							&end,
							FALSE);

		gtk_editable_set_text(GTK_EDITABLE(win->search_entry),stringToSearch);
	}
}

void litos_app_window_Esc(LitosAppWindow *win)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(LITOS_APP_WINDOW(win)->btn_find_icon), FALSE);
	gtk_editable_set_text(GTK_EDITABLE(win->search_entry),"");
}

void litos_app_window_ctrl_f(LitosAppWindow *win)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(LITOS_APP_WINDOW(win)->btn_find_icon), TRUE);
	gtk_widget_grab_focus(LITOS_APP_WINDOW(win)->search_entry);
	litos_app_window_set_search_entry(win);
}

static void litos_app_window_visible_child_changed (GObject *notebook,
			GParamSpec *pspec G_GNUC_UNUSED,
			LitosAppWindow *win)
{
	if (gtk_widget_in_destruction (GTK_WIDGET (notebook)))
		return;

	gtk_search_bar_set_search_mode (GTK_SEARCH_BAR (win->searchbar), FALSE);
}

static void litos_app_window_save_response(GObject *source_object, GAsyncResult *res, gpointer win)
{
	GtkFileDialog *dialog = GTK_FILE_DIALOG(source_object);

	GError *error = NULL;
	GFile *gfile = gtk_file_dialog_save_finish(dialog, res, &error);

	if (gfile == NULL) {
	// Se gfile è NULL, controlla se è stato annullato
		if (error != NULL) {
			if (!g_error_matches(error, G_IO_ERROR, G_IO_ERROR_CANCELLED)) {
				/* Solo se NON è stato annullato, show the warning */
				g_warning("Errore durante il salvataggio: %s", error->message);
			}
			g_error_free(error);
		}
		return;
	}

	// Salvataggio riuscito
	LitosFile *file = litos_app_window_current_file(win);
	litos_file_save_as(file, gfile);
	gtk_label_set_text(GTK_LABEL(litos_file_get_lbl(file)), litos_file_get_name(file));
}

static void litos_app_window_save_as_dialog(GSimpleAction *action G_GNUC_UNUSED, GVariant *parameter G_GNUC_UNUSED, gpointer win)
{
    GtkFileDialog *dialog = gtk_file_dialog_new();

	gtk_file_dialog_save(dialog,
		GTK_WINDOW(win),
		NULL,        // GCancellable
		litos_app_window_save_response,
		win);
}

gboolean litos_app_window_save(LitosAppWindow *win, LitosFile *file)
{
    char *filename = litos_file_get_name(file);

    if (litos_file_get_gfile(file) == NULL) {
        litos_app_window_save_as_dialog(NULL, NULL, win);
        return FALSE; // Non ha salvato direttamente, ha aperto il dialogo
    } else {
        GError *error = NULL;

        if (!litos_file_save(file, &error)) {
            char *message = g_strdup_printf("ERROR: Can't save %s.\n%s", filename, error->message);

            GtkAlertDialog *alert = gtk_alert_dialog_new(message);
            gtk_alert_dialog_show(alert, GTK_WINDOW(win));

            g_free(message);
            g_error_free(error);
            return FALSE;
        }

        return TRUE;
    }
}


static void litos_app_window_remove_page(LitosAppWindow *win, LitosFile *file)
{
    GtkWidget *tabbox = litos_file_get_tabbox(file);
    if (!tabbox)
        return;

    gint page_num = gtk_notebook_page_num(GTK_NOTEBOOK(win->notebook), tabbox);
    if (page_num == -1)
        return;

    // Nascondi il widget per evitare warning GTK sul layout
    gtk_widget_set_visible(tabbox, FALSE);

    // Rimuovi la pagina dal notebook
    gtk_notebook_remove_page(GTK_NOTEBOOK(win->notebook), page_num);

    // Rimuovi il file dalla lista interna
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
			litos_app_window_save(win,file);
			litos_app_window_remove_page(win,file);
			break;

		case GTK_RESPONSE_CANCEL:
			win->quit = FALSE;
			break;

		case GTK_RESPONSE_REJECT:  // Close without saving
			litos_app_window_remove_page(win,file);
			break;

		default:
			win->quit = FALSE;
			g_print("The button(Close without Saving/Cancel/Save) was not pressed.");
	}

	gtk_window_destroy(GTK_WINDOW(dialog));

	if (win->quit == TRUE)
	{
		if (win->litosFileList->len == 0)
		{
			g_application_quit(G_APPLICATION(app));
		}

		else
		{
			litos_app_window_remove_child(win);
			// **Richiamo ricorsivo per chiudere eventuali altre tab**
			litos_app_window_quit(NULL, win);
		}
	}
}


static void
on_save_clicked(GtkButton *button, gpointer user_data)
{
	GtkWidget *dialog = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW);
	litos_app_window_saveornot_dialog_cb(dialog, GTK_RESPONSE_ACCEPT, user_data);
}

static void
on_cancel_clicked(GtkButton *button, gpointer user_data)
{
	GtkWidget *dialog = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW);
	litos_app_window_saveornot_dialog_cb(dialog, GTK_RESPONSE_CANCEL, user_data);
}

static void
on_close_clicked(GtkButton *button, gpointer user_data)
{
	GtkWidget *dialog = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW);
	litos_app_window_saveornot_dialog_cb(dialog, GTK_RESPONSE_REJECT, user_data);
}

static void
litos_app_window_saveornot_dialog(LitosAppWindow *win, LitosFile *file)
{
	GtkWidget *dialog;
	GtkWidget *main_box, *message_box, *image, *label, *button_box;
	GtkWidget *btn_close, *btn_cancel, *btn_save;
	gchar *message;

	// Crea una nuova finestra come dialogo
	dialog = gtk_window_new();
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(win));
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	gtk_window_set_title(GTK_WINDOW(dialog), "Save changes?");
	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
	gtk_window_set_hide_on_close(GTK_WINDOW(dialog), TRUE);

	// Box verticale principale
	main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
	gtk_widget_set_margin_top(main_box, 12);
	gtk_widget_set_margin_bottom(main_box, 12);
	gtk_widget_set_margin_start(main_box, 12);
	gtk_widget_set_margin_end(main_box, 12);
	gtk_window_set_child(GTK_WINDOW(dialog), main_box);

	// Box orizzontale per icona + messaggio
	message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
	gtk_box_append(GTK_BOX(main_box), message_box);

	image = gtk_image_new_from_icon_name("dialog-warning-symbolic");
	gtk_box_append(GTK_BOX(message_box), image);

	message = g_strdup_printf("Save changes to document %s before closing?", litos_file_get_name(file));
	label = gtk_label_new(message);
	g_free(message);
	gtk_label_set_wrap(GTK_LABEL(label), TRUE);
	gtk_widget_set_halign(label, GTK_ALIGN_START);
	gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
	gtk_box_append(GTK_BOX(message_box), label);

	// Box pulsanti
	button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	gtk_widget_set_halign(button_box, GTK_ALIGN_END);
	gtk_box_append(GTK_BOX(main_box), button_box);

	// Pulsante "Close without Saving"
	btn_close = gtk_button_new_with_label("Close without Saving");
	gtk_box_append(GTK_BOX(button_box), btn_close);
	g_signal_connect(btn_close, "clicked", G_CALLBACK(on_close_clicked), win);

	// Pulsante "Cancel"
	btn_cancel = gtk_button_new_with_label("Cancel");
	gtk_box_append(GTK_BOX(button_box), btn_cancel);
	g_signal_connect(btn_cancel, "clicked", G_CALLBACK(on_cancel_clicked), win);

	// Pulsante "Save"
	btn_save = gtk_button_new_with_label("Save");
	gtk_box_append(GTK_BOX(button_box), btn_save);
	g_signal_connect(btn_save, "clicked", G_CALLBACK(on_save_clicked), win);

	// Mostra la finestra
	gtk_window_present(GTK_WINDOW(dialog));
}


gboolean litos_app_window_remove_child(LitosAppWindow *win)
{
	GtkWidget *tabbox = gtk_notebook_get_nth_page(win->notebook, gtk_notebook_get_current_page(win->notebook));

	if (tabbox != NULL)
	{
		LitosFile *file = litos_app_window_current_file(win);

		if (litos_file_get_saved(file))
		{
			litos_app_window_remove_page(win, file);
			return TRUE;
		}

		else
		{
			// Mostra la finestra di dialogo e ritorna FALSE per interrompere il ciclo
			litos_app_window_saveornot_dialog(win, file);
			return FALSE;
		}
	}

	return FALSE;
}


gboolean litos_app_window_quit(GtkWindow *window G_GNUC_UNUSED, gpointer user_data)
{
	LitosAppWindow *win = LITOS_APP_WINDOW(user_data);
	GtkApplication *app = gtk_window_get_application(GTK_WINDOW(win));

	if (litos_app_window_get_array_len(win) == 0) {
		g_application_quit(G_APPLICATION(app));
		return FALSE;
	}

	win->quit = TRUE;

	// Inizia a chiudere un file per volta
	if (!litos_app_window_remove_child(win)) {
		// Se il file non è salvato, è stato aperto il dialog → aspetta callback
		return TRUE;
	}

	// Se era già salvato, prova subito col prossimo
	return litos_app_window_quit(NULL, win);
}


void litos_app_window_update_font ()
{
	LitosApp *app = LITOS_APP(g_application_get_default());

	PangoFontDescription *font_desc;
	gchar *font, *css_string;
	GtkCssProvider *css_provider;

	font = g_settings_get_string(litos_app_get_settings(app), "font");

	font_desc = pango_font_description_from_string(font);
	css_provider = litos_app_get_css_provider(app);
	g_free(font);

	// Costruisci la stringa CSS
	css_string = g_strdup_printf(
		"textview { font: %dpt %s; }",
		pango_font_description_get_size(font_desc) / PANGO_SCALE,
		pango_font_description_get_family(font_desc)
	);

	// Usa la nuova funzione non deprecata
	gtk_css_provider_load_from_string(css_provider, css_string);

	gtk_style_context_add_provider_for_display(
		gdk_display_get_default(),
		GTK_STYLE_PROVIDER(css_provider),
		GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
	);

	// Cleanup
	pango_font_description_free(font_desc);
	g_free(css_string);
}


static void
litos_app_window_init (LitosAppWindow *win)
{
	GtkBuilder *builder;
	GMenuModel *menu;

	// load the template from the .ui file
	gtk_widget_init_template(GTK_WIDGET(win));

	// Costruisci il menu degli ingranaggi
	builder = gtk_builder_new_from_resource("/org/gtk/litos/gears-menu.ui");
	menu = G_MENU_MODEL(gtk_builder_get_object(builder, "menu"));
	gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(win->gears), menu);
	g_object_unref(builder);

	// Imposta le proprietà iniziali della finestra
	win->settings = g_settings_new("org.gtk.litos");
	gtk_widget_set_sensitive(win->btn_find_icon, TRUE);
	win->quit = FALSE;
	win->search_context = NULL;
	win->litosFileList = g_ptr_array_new_full(0, g_object_unref);

	// Applica il font personalizzato
	litos_app_window_update_font();

	// Connessioni dei segnali
	g_signal_connect(GTK_WINDOW(win), "close-request", G_CALLBACK(litos_app_window_quit), win);
	g_signal_connect(win->btn_prev, "clicked", G_CALLBACK(litos_app_window_prev_match), win);
	g_signal_connect(win->btn_next, "clicked", G_CALLBACK(litos_app_window_next_match), win);
	g_signal_connect(win->btn_replace, "clicked", G_CALLBACK(litos_app_window_replace_btn_clicked), win);

	// Focus automatico sulla barra di ricerca
	gtk_widget_set_can_focus(win->search_entry, TRUE);

	// Associa lo stato del pulsante alla visibilità della barra di ricerca
	g_object_bind_property(win->btn_find_icon, "active",
		win->searchbar, "search-mode-enabled",
		G_BINDING_BIDIRECTIONAL);
}


static void litos_app_window_dispose (GObject *object)
{
	LitosAppWindow *win = LITOS_APP_WINDOW (object);

	g_clear_object (&win->settings);

	g_ptr_array_unref(win->litosFileList);

	G_OBJECT_CLASS (litos_app_window_parent_class)->dispose (object);
}

void litos_app_window_about_dialog (GtkButton *button G_GNUC_UNUSED, gpointer userData G_GNUC_UNUSED)
{
	const gchar *authors[] = {"Giovanni Resta", "giovannirestadev@gmail.com", NULL};

	gtk_show_about_dialog (NULL,
			"program-name", "Litos",
			"version", "4.2.1",
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

	BIND_CHILD (notebook)
	BIND_CHILD (gears)
	BIND_CHILD (btn_find_icon)
	BIND_CHILD (searchbar)
	BIND_CHILD (search_entry)
	BIND_CHILD (replace_entry)
	BIND_CHILD (about_btn)
	BIND_CHILD (btn_prev)
	BIND_CHILD (btn_next)
	BIND_CHILD (lbl_number_occurences)
	BIND_CHILD (btn_replace)
	BIND_CHILD (btn_check_case)

	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), litos_app_window_about_dialog);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), litos_app_window_search_text_changed);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), litos_app_window_visible_child_changed);
}

LitosAppWindow * litos_app_window_new (LitosApp *app)
{
	return g_object_new (LITOS_APP_WINDOW_TYPE, "application", app, NULL);
}

void litos_app_window_save_as(LitosAppWindow *win)
{
	if (!win->notebook || gtk_notebook_get_n_pages(GTK_NOTEBOOK(win->notebook)) == 0) {
		g_warning("No tab opened: impossible execute 'Save as'.");
		return;
	}

	litos_app_window_save_as_dialog(NULL, NULL, win);
}

static void litos_app_window_close_btn_clicked(GtkWidget *close_btn, gpointer user_data)
{
	LitosAppWindow *win = LITOS_APP_WINDOW(user_data);
	if (!LITOS_IS_APP_WINDOW(win))
		return;

	GtkNotebook *notebook = GTK_NOTEBOOK(win->notebook);

	// Trova il tab associato al bottone cliccato
	gint num_pages = gtk_notebook_get_n_pages(notebook);
	for (gint i = 0; i < num_pages; i++) {
		GtkWidget *tab = gtk_notebook_get_nth_page(notebook, i);
		GtkWidget *tab_label = gtk_notebook_get_tab_label(notebook, tab);
		if (gtk_widget_is_ancestor(close_btn, tab_label)) {
			gtk_notebook_set_current_page(notebook, i); // seleziona il tab
			litos_app_window_remove_child(win);         // rimuove il tab attivo
			break;
		}
	}
}

static void litos_app_window_lblToColor(LitosAppWindow *win G_GNUC_UNUSED, LitosFile* file, const char *color)
{
	char *markup = g_markup_printf_escaped ("<span color='%s'>\%s</span>", color, litos_file_get_name(file));

	gtk_label_set_markup (GTK_LABEL(litos_file_get_lbl(file)), markup);

	g_free(markup);
}

static void litos_app_window_monitor_saved_change(GObject *gobject, GParamSpec *pspec G_GNUC_UNUSED, gpointer win)
{
	LitosAppWindow *lwin = LITOS_APP_WINDOW(win);
	LitosFile *file = LITOS_FILE(gobject);

	if (litos_file_get_saved(file) == FALSE)
		litos_app_window_lblToColor(lwin, file, "red");

	else
		litos_app_window_lblToColor(lwin, file, "gray");
}

void litos_app_window_error_dialog(GtkWindow *parent, GError *error, const char *filename)
{
	GtkWidget *dialog;
	GtkWidget *box;
	GtkWidget *label;
	GtkWidget *button;
	gchar *message;

	// Crea una finestra di dialogo come sottoclasse di GtkWindow
	dialog = gtk_window_new();
	gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	gtk_window_set_title(GTK_WINDOW(dialog), "Errore");
	gtk_window_set_default_size(GTK_WINDOW(dialog), 300, 100);

	// Crea un contenitore verticale
	box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_widget_set_margin_top(box, 15);
	gtk_widget_set_margin_bottom(box, 15);
	gtk_widget_set_margin_start(box, 15);
	gtk_widget_set_margin_end(box, 15);
	gtk_window_set_child(GTK_WINDOW(dialog), box);

	// Crea il messaggio
	message = g_strdup_printf("ERROR: Can't load %s.\n%s", filename, error->message);
	label = gtk_label_new(message);
	gtk_label_set_wrap(GTK_LABEL(label), TRUE);
	gtk_box_append(GTK_BOX(box), label);
	g_free(message);

	// Crea il pulsante di chiusura
	button = gtk_button_new_with_label("Chiudi");
	gtk_box_append(GTK_BOX(box), button);
	g_signal_connect(button, "clicked", G_CALLBACK(gtk_window_destroy), dialog);

	// Mostra la finestra
	gtk_window_present(GTK_WINDOW(dialog));

	// Cleanup
	g_error_free(error);
}

void update_star_button(GtkButton *button, gboolean is_favorite)
{
    const char *icon_name = is_favorite ? "starred-symbolic" : "non-starred-symbolic";
    const char *tooltip = is_favorite ? "Rimuovi dai preferiti" : "Aggiungi ai preferiti";

    GtkWidget *new_icon = gtk_image_new_from_icon_name(icon_name);
    gtk_button_set_child(button, new_icon);
    gtk_widget_set_tooltip_text(GTK_WIDGET(button), tooltip);
}


static gboolean uri_in_list(const gchar **list, const gchar *uri)
{
    for (int i = 0; list && list[i]; i++) {
        if (g_strcmp0(list[i], uri) == 0)
            return TRUE;
    }
    return FALSE;
}


static gchar **add_uri_to_list(const gchar **list, const gchar *uri)
{
    GPtrArray *array = g_ptr_array_new_with_free_func(g_free);
    for (int i = 0; list && list[i]; i++)
        g_ptr_array_add(array, g_strdup(list[i]));

    if (uri && g_utf8_validate(uri, -1, NULL))
        g_ptr_array_add(array, g_strdup(uri));

    g_ptr_array_add(array, NULL);
    return (gchar **)g_ptr_array_free(array, FALSE);
}

static gchar **remove_uri_from_list(const gchar **list, const gchar *uri)
{
    GPtrArray *array = g_ptr_array_new_with_free_func(g_free);
    for (int i = 0; list && list[i]; i++) {
        if (g_strcmp0(list[i], uri) != 0)
            g_ptr_array_add(array, g_strdup(list[i]));
    }

    g_ptr_array_add(array, NULL);
    return (gchar **)g_ptr_array_free(array, FALSE);
}


void litos_app_window_save_favorites(LitosAppWindow *win)
{
    GPtrArray *valid_uris = g_ptr_array_new_with_free_func(g_free);

    for (guint i = 0; i < win->litosFileList->len; i++) {
        LitosFile *file = g_ptr_array_index(win->litosFileList, i);
        GFile *gf = litos_file_get_gfile(file);

        if (!gf)
            continue;

        gchar *uri = g_file_get_uri(gf);

        if (uri && g_utf8_validate(uri, -1, NULL)) {
            g_ptr_array_add(valid_uris, g_strdup(uri));
        } else {
            g_warning("URI non valido, ignorato: %s", uri ? uri : "(null)");
        }

        g_free(uri);
    }

    g_ptr_array_add(valid_uris, NULL); // terminatore per strv
    g_settings_set_strv(win->settings, "favorite-files", (const gchar * const *)valid_uris->pdata);
    g_ptr_array_free(valid_uris, TRUE);
}


void litos_app_window_star_btn_clicked(GtkButton *button, gpointer user_data)
{
    LitosAppWindow *win = LITOS_APP_WINDOW(user_data);
    LitosFile *file = g_object_get_data(G_OBJECT(button), "litos-file");

    GFile *gf = litos_file_get_gfile(file);
    gchar *uri = gf ? g_file_get_uri(gf) : NULL;

    if (!uri || !g_utf8_validate(uri, -1, NULL)) {
        g_warning("URI non valido o NULL");
        g_free(uri);
        return;
    }

    gchar **favorites = g_settings_get_strv(win->settings, "favorite-files");

    gboolean is_fav = uri_in_list((const gchar **)favorites, uri);
    gchar **updated = is_fav
        ? remove_uri_from_list((const gchar **)favorites, uri)
        : add_uri_to_list((const gchar **)favorites, uri);

    g_settings_set_strv(win->settings, "favorite-files", (const gchar * const *)updated);
    update_star_button(button, !is_fav);

    g_strfreev(favorites);
    g_strfreev(updated);
    g_free(uri);
}


static gboolean is_favorite(const char *uri)
{
    if (!uri || !g_utf8_validate(uri, -1, NULL))
        return FALSE;

    GSettings *settings = g_settings_new("org.gtk.litos");
    gchar **favorites = g_settings_get_strv(settings, "favorite-files");

    gboolean found = uri_in_list((const gchar **)favorites, uri);

    g_strfreev(favorites);
    g_object_unref(settings);
    return found;
}


static LitosFile *
litos_app_window_new_tab(LitosAppWindow *win, struct Page *page)
{
    GtkWidget *close_btn;
    GtkWidget *star_btn;

    // 1. Inizializza i contenitori
    page->tabbox        = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 1);
    page->close_btn_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    page->scrolled      = gtk_scrolled_window_new();

    // 2. Crea la view con il buffer
    page->view = MyNewSourceview();
    if (!page->view) {
        g_warning("MyNewSourceview() ha restituito NULL");
        return NULL;
    }
    page->buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(page->view));

    // 3. Inserisci la view nello scrolled window
    gtk_widget_set_hexpand(page->scrolled, TRUE);
    gtk_widget_set_vexpand(page->scrolled, TRUE);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(page->scrolled), page->view);

    // 4. Crea label e bottone di chiusura
    page->lbl = gtk_label_new(page->name ? page->name : "Senza nome");
    close_btn = gtk_button_new();
    GtkWidget *close_icon = gtk_image_new_from_icon_name("window-close-symbolic");
    gtk_button_set_child(GTK_BUTTON(close_btn), close_icon);

    // 4b. Crea bottone stella
    star_btn = gtk_button_new();
    gtk_widget_set_tooltip_text(star_btn, "Aggiungi ai preferiti");

    // 5. Costruisci la tab
    gtk_box_append(GTK_BOX(page->close_btn_box), page->lbl);
    gtk_box_append(GTK_BOX(page->close_btn_box), star_btn);
    gtk_box_append(GTK_BOX(page->close_btn_box), close_btn);
    gtk_box_append(GTK_BOX(page->tabbox), page->scrolled);

    // 6. Aggiungi la tab al notebook
    gint page_num = gtk_notebook_append_page_menu(
        win->notebook,
        page->tabbox,
        page->close_btn_box,
        page->close_btn_box
    );
    gtk_notebook_set_current_page(win->notebook, page_num);
    gtk_notebook_set_tab_reorderable(win->notebook, page->tabbox, TRUE);

    // 7. Registra il file
    LitosFile *file = litos_file_set(page);
    g_ptr_array_add(win->litosFileList, file);

    // 8. Collega il file al bottone di chiusura
    g_object_set_data(G_OBJECT(close_btn), "litos-file", file);
    g_signal_connect(close_btn, "clicked",
                     G_CALLBACK(litos_app_window_close_btn_clicked), win);

    // 9. Collega il file al bottone stella
    g_object_set_data(G_OBJECT(star_btn), "litos-file", file);
    g_signal_connect(star_btn, "clicked",
                     G_CALLBACK(litos_app_window_star_btn_clicked), win);

    // 10. Salva il bottone stella nel file per uso futuro
    g_object_set_data(G_OBJECT(file), "star-button", star_btn);
    g_object_set_data(G_OBJECT(file), "star-initialized", GINT_TO_POINTER(FALSE));

    // 11. Se il file è già salvato, inizializza subito la stella
    GFile *gf = litos_file_get_gfile(file);
    gchar *uri = gf ? g_file_get_uri(gf) : NULL;

    if (uri && g_utf8_validate(uri, -1, NULL)) {
        gboolean is_fav = is_favorite(uri);
        update_star_button(GTK_BUTTON(star_btn), is_fav);
        g_object_set_data(G_OBJECT(file), "star-initialized", GINT_TO_POINTER(TRUE));
        g_free(uri);
    }

    // 12. Focus sul contenuto
    gtk_widget_grab_focus(page->view);

    return file;
}

void litos_app_window_initialize_star_if_needed(LitosFile *file)
{
    if (GPOINTER_TO_INT(g_object_get_data(G_OBJECT(file), "star-initialized")))
        return;

    GtkButton *star_btn = GTK_BUTTON(g_object_get_data(G_OBJECT(file), "star-button"));
    if (!star_btn)
        return;

    GFile *gf = litos_file_get_gfile(file);
    gchar *uri = gf ? g_file_get_uri(gf) : NULL;

    if (uri && g_utf8_validate(uri, -1, NULL)) {
        gboolean is_fav = is_favorite(uri);
        update_star_button(star_btn, is_fav);
        g_object_set_data(G_OBJECT(file), "star-initialized", GINT_TO_POINTER(TRUE));
    }

    g_free(uri);
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
		litos_app_window_error_dialog(GTK_WINDOW(win), error, page.name);
		return NULL;
	}

	else
	{
		litos_file_highlight_buffer(file);
		g_signal_connect(G_OBJECT(file), "notify::saved", G_CALLBACK (litos_app_window_monitor_saved_change), win);
		return file;
	}
}

void litos_app_window_new_file(LitosAppWindow *win)
{
	int file_index = litos_app_window_get_array_len(win) + 1;

	struct Page page;

	page.name = g_strdup_printf("Untitled %d", file_index);

	page.gf = NULL;

	LitosFile *file = litos_app_window_new_tab(win,&page);

	g_signal_connect(G_OBJECT(file), "notify::saved", G_CALLBACK (litos_app_window_monitor_saved_change), win);
}

LitosFile * litos_app_window_get_file(LitosAppWindow *win, int *i)
{
	return g_ptr_array_index(win->litosFileList,*i);
}

GtkNotebook * litos_app_window_get_nb(LitosAppWindow *win)
{
	return win->notebook;
}

void litos_app_window_open_favorites(LitosAppWindow *win)
{
    gchar **favorites = g_settings_get_strv(win->settings, "favorite-files");

    for (int i = 0; favorites && favorites[i]; i++) {
        GFile *gf = g_file_new_for_uri(favorites[i]);
        litos_app_window_open(win, gf);
        g_object_unref(gf);
    }

    g_strfreev(favorites);
}

