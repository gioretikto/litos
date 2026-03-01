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
#include "litospage.h"
#include "litosappwin.h"

#define BIND_CHILD(x) gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), LitosAppWindow, x);

#define SCROLL_TO_MARK gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW(view), mark, 0, FALSE, 0.0,	0.0);

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
	gboolean quit_activated;

	// Dialog di salvataggio alla chiusura
	GtkWidget *save_dialog;
	GtkLabel  *save_dialog_label;

};

G_DEFINE_TYPE (LitosAppWindow, litos_app_window, GTK_TYPE_APPLICATION_WINDOW);

static gboolean litos_file_tabbox_equal(gconstpointer array_element, gconstpointer tabbox)
{
	const LitosFile *file = (const LitosFile *)array_element;

	if (!file || !tabbox)
	return FALSE;

	return litos_file_get_tabbox((LitosFile *)file) == GTK_WIDGET(tabbox);
}


guint litos_app_window_get_array_len(LitosAppWindow *win)
{
	return win->litosFileList->len;
}

gint litos_app_window_search_file(LitosAppWindow *win)
{
	if (!win || !win->notebook || litos_app_window_get_array_len(win) == 0)
		return -1;

	gint current_page = gtk_notebook_get_current_page(win->notebook);
	if (current_page < 0)
		return -1;

	GtkWidget *tabbox = gtk_notebook_get_nth_page(win->notebook, current_page);
	if (!tabbox)
		return -1;

	guint index;
	if (g_ptr_array_find_with_equal_func(win->litosFileList, tabbox, litos_file_tabbox_equal, &index))
		return (gint)index;

	return -1;
}


LitosFile *litos_app_window_current_file(LitosAppWindow *win)
{
	gint current_page = gtk_notebook_get_current_page(win->notebook);
	if (current_page < 0)
		return NULL;

	GtkWidget *tabbox = gtk_notebook_get_nth_page(win->notebook, current_page);
	if (!tabbox)
		return NULL;

	return g_object_get_data(G_OBJECT(tabbox), "litos-file");
}


static GtkSourceView* 
litos_app_window_current_tab_sourceView(LitosAppWindow *win)
{
    LitosFile *file = litos_app_window_current_file(win);
    if (!file) return NULL; // Ritorna NULL se non c'è un file attivo

    GtkWidget *view = litos_file_get_view(file);
    if (!GTK_IS_WIDGET(view)) return NULL;

    return GTK_SOURCE_VIEW(view);
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
        if (!view) return;

        buffer = gtk_source_search_context_get_buffer (win->search_context);

        // Otteniamo la posizione attuale per cercare la prossima occorrenza
        gtk_text_buffer_get_selection_bounds (GTK_TEXT_BUFFER (buffer), &match_start, &start);

        if (gtk_source_search_context_forward (win->search_context, &start, &match_start, &match_end, NULL))
        {
            // 1. Selezioniamo il range (questo colora il match con il colore "Selection")
            gtk_text_buffer_select_range (GTK_TEXT_BUFFER (buffer), &match_start, &match_end);

            // 2. Otteniamo il mark d'inserimento per lo scroll
            mark = gtk_text_buffer_get_insert(GTK_TEXT_BUFFER(buffer));

            // 3. SCROLL CENTRATO: Usiamo 0.5 per mettere il match al centro della vista
            // Invece della macro SCROLL_TO_MARK, usiamo la funzione esplicita:
            gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW(view), mark, 0.0, FALSE, 0.5, 0.5);
        }
    }
}

static void 
litos_app_window_update_match_label (GtkSourceSearchContext *context,
                                     GParamSpec *pspec G_GNUC_UNUSED,
                                     LitosAppWindow *win)
{
    // Se il contesto è in fase di distruzione o non è quello attuale, ignora
    if (!context || context != win->search_context) return;

    GtkSourceBuffer *buffer = gtk_source_search_context_get_buffer(context);
    if (!buffer) return;

    gint count = gtk_source_search_context_get_occurrences_count(context);
    
    if (count >= 0) {
        char buf[64];
        g_snprintf(buf, sizeof(buf), "%d matches", count);
        gtk_label_set_text(GTK_LABEL(win->lbl_number_occurences), buf);
    } else {
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
			G_CALLBACK(litos_app_window_update_match_label),
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

	gtk_source_search_context_set_highlight (win->search_context, TRUE);

	g_signal_connect(win->search_context,
		"notify::occurrences-count",
		G_CALLBACK(litos_app_window_update_match_label),
		win);

	return source_view;
}

static void litos_app_window_replace_btn_clicked(GtkButton *button G_GNUC_UNUSED, gpointer userData)
{
	LitosAppWindow *win = LITOS_APP_WINDOW(userData);

	if (!win->search_context)
		return;

	const gchar *stringToSearch = gtk_editable_get_text(GTK_EDITABLE(win->search_entry));
	const gchar *replaceString  = gtk_editable_get_text(GTK_EDITABLE(win->replace_entry));

	if (!stringToSearch || !replaceString || stringToSearch[0] == '\0')
		return;

	guint count_replaced = gtk_source_search_context_replace_all(
		win->search_context,
		replaceString,
		-1,
		NULL
	);

	gtk_editable_set_text(GTK_EDITABLE(win->replace_entry), "");

	gchar str[80];
	g_snprintf(str, sizeof(str), "%d replacement%s", count_replaced, count_replaced == 1 ? "" : "s");
	gtk_label_set_text(GTK_LABEL(win->lbl_number_occurences), str);
}

/* a word is entered or modified in the seach bar */

static void 
litos_app_window_search_text_changed (GtkEditable *editable G_GNUC_UNUSED, LitosAppWindow *win)
{
    // 1. Recupera il testo in modo sicuro per GTK4
    const char *text = gtk_editable_get_text (GTK_EDITABLE (win->search_entry));
    
    // 2. Ottieni la view e il buffer del TAB ATTUALE
    GtkSourceView *view = litos_app_window_current_tab_sourceView(win);
    if (!view || !GTK_IS_TEXT_VIEW(view)) return;
    
    GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW(view));
    if (!GTK_IS_TEXT_BUFFER(buffer)) return;

    // 3. Gestione campo vuoto: pulisce l'evidenziazione e il conteggio
    if (text == NULL || text[0] == '\0') {
        if (win->search_context) {
            GtkSourceSearchSettings *settings = gtk_source_search_context_get_settings(win->search_context);
            gtk_source_search_settings_set_search_text(settings, NULL);
            litos_app_window_update_match_label(win->search_context, NULL, win);
        }
        return;
    }

    // 4. Crea o aggiorna il contesto di ricerca legato al buffer corrente
    litos_app_window_set_search_context(win, text);
    if (!win->search_context) return;

    // 5. Esegui la ricerca dalla cima del file
    GtkTextIter start, match_start, match_end;
    gtk_text_buffer_get_start_iter (buffer, &start);

    if (gtk_source_search_context_forward (win->search_context, &start, &match_start, &match_end, NULL)) {
        // Fondamentale: recupera il Mark d'inserimento SPECIFICO di questo buffer
        GtkTextMark *mark = gtk_text_buffer_get_insert (buffer); 
        
        // Seleziona il testo trovato
        gtk_text_buffer_select_range (buffer, &match_start, &match_end);
        
        // Verifica di sicurezza prima di scorrere (evita i crash del display)
        if (GTK_IS_TEXT_MARK(mark) && gtk_text_mark_get_buffer(mark) == buffer) {
            gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW(view), mark, 0.0, FALSE, 0.5, 0.5);
        }
    }
    
    // 6. Aggiorna la label con il numero di occorrenze
    litos_app_window_update_match_label(win->search_context, NULL, win);
}


void litos_app_window_set_search_entry(LitosAppWindow *win)
{
	GtkTextIter start, end;
	LitosFile *file = litos_app_window_current_file(win);

	if (!file)
	return;

	GtkTextBuffer *buffer = litos_file_get_buffer(file);
	if (!GTK_IS_TEXT_BUFFER(buffer))
		return;

	if (gtk_text_buffer_get_selection_bounds(buffer, &start, &end)) {
		gchar *stringToSearch = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);
		gtk_editable_set_text(GTK_EDITABLE(win->search_entry), stringToSearch);
		g_free(stringToSearch); // evita il leak
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
		NULL,// GCancellable
		litos_app_window_save_response,
		win);
}

gboolean litos_app_window_save(LitosAppWindow *win, LitosFile *file)
{
	char *filename = litos_file_get_name(file);

	if (litos_file_get_gfile(file) == NULL) {
		litos_app_window_save_as_dialog(NULL, NULL, win);
		return FALSE; // Non ha salvato direttamente, ha aperto il dialogo
	}

	else {
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

void litos_app_window_update_title(LitosAppWindow *win)
{
	if (!win || !win->notebook)
		return;

	GtkWindow *window = GTK_WINDOW(win);

	gint current_page = gtk_notebook_get_current_page(win->notebook);
	GtkWidget *tabbox = (current_page >= 0)
		? gtk_notebook_get_nth_page(win->notebook, current_page)
		: NULL;

	// Recupera il file della tab corrente
	LitosFile *file = tabbox ? g_object_get_data(G_OBJECT(tabbox), "litos-file") : NULL;
	if (!file) {
		gtk_window_set_title(window, "Litos");
		return;
	}

	GtkWidget *lbl = litos_file_get_lbl(file);
	if (GTK_IS_LABEL(lbl)) {
		const gchar *name = litos_file_get_name(file) ? litos_file_get_name(file) : "Untitled";
		gboolean saved = litos_file_get_saved(file);

		// Aggiorna testo del label
		gchar *tab_title = saved ? g_strdup(name) : g_strdup_printf("*%s", name);
		gtk_label_set_text(GTK_LABEL(lbl), tab_title);
		g_free(tab_title);

		// Aggiorna colore via CSS usando API GTK 4
		if (!saved) {
			gtk_widget_remove_css_class(lbl, "saved-label");
			gtk_widget_add_css_class(lbl, "unsaved-label");
		} else {
			gtk_widget_remove_css_class(lbl, "unsaved-label");
			gtk_widget_add_css_class(lbl, "saved-label");
		}
	}

	// Se il file non ha un GFile (es. "Untitled"), non mostriamo path
	GFile *gfile = litos_file_get_gfile(file);
	if (!gfile) {
		gtk_window_set_title(window, "Litos");
		return;
	}

	// Costruisci il percorso da mostrare nella finestra
	gchar *filepath = g_file_get_path(gfile);
	const char *home = g_get_home_dir();
	gchar *display_path = g_str_has_prefix(filepath, home)
		? g_strconcat("~", filepath + strlen(home), NULL)
		: g_strdup(filepath);

	// Aggiorna il titolo della finestra con o senza asterisco
	if (!litos_file_get_saved(file)) {
		gchar *modified_title = g_strconcat("*", display_path, NULL);
		gtk_window_set_title(window, modified_title);
		g_free(modified_title);
	}
	else
		gtk_window_set_title(window, display_path);

	g_free(display_path);
	g_free(filepath);
}

static void litos_app_window_close_btn_clicked(GtkWidget *close_btn, gpointer user_data)
{
	LitosAppWindow *win = LITOS_APP_WINDOW(user_data);
	if (!LITOS_IS_APP_WINDOW(win))
	return;

	GtkNotebook *notebook = GTK_NOTEBOOK(win->notebook);
	GtkWidget *tab_label = gtk_widget_get_parent(close_btn); // Assumendo che il bottone sia figlio diretto del tab label

	gint page_num = -1;
	gint num_pages = gtk_notebook_get_n_pages(notebook);
	for (gint i = 0; i < num_pages; i++) {
		GtkWidget *tab = gtk_notebook_get_nth_page(notebook, i);
		GtkWidget *label = gtk_notebook_get_tab_label(notebook, tab);
		if (label == tab_label) {
		page_num = i;
		break;
		}
	}

	if (page_num != -1) {
		gtk_notebook_set_current_page(notebook, page_num);
		litos_app_window_remove_child(win);
	}
}

static void litos_app_window_remove_page(LitosAppWindow *win, LitosFile *file)
{
	if (!win || !file)
		return;

	GtkWidget *tabbox = litos_file_get_tabbox(file);
	if (!tabbox)
		return;

	gint page_num = gtk_notebook_page_num(GTK_NOTEBOOK(win->notebook), tabbox);
	if (page_num == -1)
		return;

	// Disconnetti i segnali dei pulsanti
	GtkWidget *close_btn = GTK_WIDGET(g_object_get_data(G_OBJECT(file), "close-button"));

	if (close_btn) {
		g_signal_handlers_disconnect_by_func(close_btn, G_CALLBACK(litos_app_window_close_btn_clicked), win);
		g_object_set_data(G_OBJECT(file), "close-button", NULL);
	}

	// Rimuovi la pagina dal notebook
	gtk_widget_set_visible(tabbox, FALSE);
	gtk_notebook_remove_page(GTK_NOTEBOOK(win->notebook), page_num);

	// Aggiorna lo stato interno
	g_ptr_array_remove(win->litosFileList, file);

	// Aggiorna il titolo della finestra
	litos_app_window_update_title(win);
}

static void
litos_app_window_on_close_clicked(GtkButton *button, gpointer user_data)
{
	LitosAppWindow *win = LITOS_APP_WINDOW(user_data);
	GtkWidget *dialog = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW);
	LitosFile *file = litos_app_window_current_file(win);

	if (!file)
		return;

	// Chiudi senza salvare
	litos_app_window_remove_page(win, file);

	// Nascondi il dialog invece di distruggerlo
	if (GTK_IS_WIDGET(dialog))
		gtk_widget_set_visible(dialog, FALSE);

	if (win->quit_activated)
	litos_app_window_quit(NULL, win);
}

static void
litos_app_window_on_save_clicked(GtkButton *button, gpointer user_data)
{
	LitosAppWindow *win = LITOS_APP_WINDOW(user_data);
	GtkWidget *dialog = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW);
	LitosFile *file = litos_app_window_current_file(win);

	if (!file)
		return;

	// Salva il file: eventuali errori vengono gestiti dentro litos_file_save()
	if (!litos_file_save(file, NULL))
		return; // se il salvataggio fallisce, non chiudere la tab né il dialog

	// Rimuove la tab dopo il salvataggio
	litos_app_window_remove_page(win, file);

	// Nascondi il dialog invece di distruggerlo
	if (GTK_IS_WIDGET(dialog))
		gtk_widget_set_visible(dialog, FALSE);

	// Se l'app era in chiusura, continua con la prossima tab
	if (win->quit_activated)
		litos_app_window_quit(NULL, win);
}


static void
litos_app_window_on_cancel_clicked(GtkButton *button G_GNUC_UNUSED, gpointer user_data G_GNUC_UNUSED)
{
	GtkWidget *dialog = gtk_widget_get_ancestor(GTK_WIDGET(button), GTK_TYPE_WINDOW);

	if (GTK_IS_WIDGET(dialog))
		gtk_widget_set_visible(dialog, FALSE);
}

void
litos_app_window_saveornot_dialog(LitosAppWindow *win, LitosFile *file)
{
	gchar *message = g_strdup_printf("Save changes to document %s before closing?", litos_file_get_name(file));
	gtk_label_set_text(win->save_dialog_label, message);
	g_free(message);

	gtk_window_present(GTK_WINDOW(win->save_dialog));
}

/* Salva la sessione così da riaprire i files lasciati aperti al prossimo riavvio */

void litos_app_window_save_session (LitosAppWindow *win) 
{
    // Verifica che l'array esista per evitare crash
    if (!win->litosFileList) return;

    GPtrArray *paths = g_ptr_array_new_with_free_func (g_free);

    for (guint i = 0; i < win->litosFileList->len; i++) {
        LitosFile *file = g_ptr_array_index (win->litosFileList, i);
        GFile *gf = litos_file_get_gfile (file);
        
        if (gf) {
            gchar *path = g_file_get_path (gf);
            if (path) {
                g_ptr_array_add (paths, path);
            }
        }
    }
    // Terminatore obbligatorio per GSettings
    g_ptr_array_add (paths, NULL); 
    
    // Scrive la lista nelle impostazioni
    g_settings_set_strv (win->settings, "last-session-files", (const gchar * const *)paths->pdata);
    
    /* FONDAMENTALE: Forza la scrittura immediata su disco prima che l'app muoia */
    g_settings_sync (); 
    
    g_ptr_array_unref (paths);
}


gboolean litos_app_window_quit(GtkWindow *window G_GNUC_UNUSED, gpointer user_data)
{
    LitosAppWindow *win = LITOS_APP_WINDOW(user_data);
    GtkApplication *app = gtk_window_get_application(GTK_WINDOW(win));

    /* 1. SALVA LA SESSIONE ORA: prima che i file vengano chiusi/rimossi */
    if (!win->quit_activated) {
        litos_app_window_save_session(win);
    }

    if (litos_app_window_get_array_len(win) == 0) {
        /* Se non ci sono più file, esce definitivamente */
        g_application_quit(G_APPLICATION(app));
        return FALSE;
    }

    win->quit_activated = TRUE;

    // Prova a chiudere il file corrente (gestisce i dialog se non salvato)
    if (!litos_app_window_remove_child(win))
        return TRUE; 

    // Ricorsione: passa al file successivo
    return litos_app_window_quit(NULL, win);
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

static void
litos_app_window_init_save_dialog(LitosAppWindow *win)
{
	GtkWidget *main_box, *message_box, *image, *button_box;
	GtkWidget *btn_close, *btn_cancel, *btn_save;

	win->save_dialog = gtk_window_new();
	gtk_window_set_transient_for(GTK_WINDOW(win->save_dialog), GTK_WINDOW(win));
	gtk_window_set_modal(GTK_WINDOW(win->save_dialog), TRUE);
	gtk_window_set_title(GTK_WINDOW(win->save_dialog), "Save changes?");
	gtk_window_set_resizable(GTK_WINDOW(win->save_dialog), FALSE);
	gtk_window_set_hide_on_close(GTK_WINDOW(win->save_dialog), TRUE);

	main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
	gtk_window_set_child(GTK_WINDOW(win->save_dialog), main_box);

	message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
	gtk_box_append(GTK_BOX(main_box), message_box);

	image = gtk_image_new_from_icon_name("dialog-warning-symbolic");
	gtk_box_append(GTK_BOX(message_box), image);

	win->save_dialog_label = GTK_LABEL(gtk_label_new(""));
	gtk_label_set_wrap(win->save_dialog_label, TRUE);
	gtk_widget_set_halign(GTK_WIDGET(win->save_dialog_label), GTK_ALIGN_START);
	gtk_widget_set_valign(GTK_WIDGET(win->save_dialog_label), GTK_ALIGN_CENTER);
	gtk_box_append(GTK_BOX(message_box), GTK_WIDGET(win->save_dialog_label));

	button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
	gtk_widget_set_halign(button_box, GTK_ALIGN_END);
	gtk_box_append(GTK_BOX(main_box), button_box);

	btn_close = gtk_button_new_with_label("Close without Saving");
	gtk_box_append(GTK_BOX(button_box), btn_close);
	g_signal_connect(btn_close, "clicked", G_CALLBACK(litos_app_window_on_close_clicked), win);

	btn_cancel = gtk_button_new_with_label("Cancel");
	gtk_box_append(GTK_BOX(button_box), btn_cancel);
	g_signal_connect(btn_cancel, "clicked", G_CALLBACK(litos_app_window_on_cancel_clicked), win);

	btn_save = gtk_button_new_with_label("Save");
	gtk_box_append(GTK_BOX(button_box), btn_save);
	g_signal_connect(btn_save, "clicked", G_CALLBACK(litos_app_window_on_save_clicked), win);
}

static void
litos_app_window_on_switch_page (GtkNotebook *nb G_GNUC_UNUSED, 
                                 GtkWidget   *page G_GNUC_UNUSED, 
                                 guint        page_num G_GNUC_UNUSED, 
                                 gpointer     user_data)
{
    LitosAppWindow *win = LITOS_APP_WINDOW (user_data);

    // 1. Distruggi il vecchio contesto legato al file precedente
    if (win->search_context != NULL) {
        g_signal_handlers_disconnect_by_func (win->search_context,
            G_CALLBACK (litos_app_window_update_match_label), win);
        g_clear_object (&win->search_context);
    }

    // 2. Se la barra è visibile, riavvia la ricerca sul nuovo buffer
    if (gtk_widget_get_visible (win->searchbar)) {
        litos_app_window_search_text_changed (GTK_EDITABLE (win->search_entry), win);
    }
}

static void litos_app_window_init (LitosAppWindow *win)
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
	win->quit_activated = FALSE;
	win->search_context = NULL;
	win->litosFileList = g_ptr_array_new_full(0, g_object_unref);

	// Connessioni dei segnali
	g_signal_connect(GTK_WINDOW(win), "close-request", G_CALLBACK(litos_app_window_quit), win);
	g_signal_connect(win->btn_prev, "clicked", G_CALLBACK(litos_app_window_prev_match), win);
	g_signal_connect(win->btn_next, "clicked", G_CALLBACK(litos_app_window_next_match), win);
	g_signal_connect(win->btn_replace, "clicked", G_CALLBACK(litos_app_window_replace_btn_clicked), win);
	g_signal_connect_data (win->notebook, "switch-page", G_CALLBACK (litos_app_window_update_title), win, NULL, G_CONNECT_SWAPPED | G_CONNECT_AFTER); /* to update the title with file path*/
	g_signal_connect_after (win->notebook, "switch-page", G_CALLBACK (litos_app_window_on_switch_page), win);


	litos_app_window_init_save_dialog(win);

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

	// Disconnessione e rilascio del GtkSourceSearchContext, se presente
	if (win->search_context) {
		g_signal_handlers_disconnect_by_func(
		win->search_context,
		G_CALLBACK(litos_app_window_update_match_label),
		win
		);
		g_clear_object(&win->search_context);
	}

	g_clear_object (&win->settings);

	g_ptr_array_unref(win->litosFileList);

	G_OBJECT_CLASS (litos_app_window_parent_class)->dispose (object);
}

void litos_app_window_about_dialog (GtkButton *button G_GNUC_UNUSED, gpointer userData G_GNUC_UNUSED)
{
	const gchar *authors[] = {"Giovanni Resta", "giovannirestadev@gmail.com", NULL};

	gtk_show_about_dialog (NULL,
			"program-name", "Litos",
			"version", "4.3.1",
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

void litos_app_window_error_dialog(GtkWindow *parent, GError *error, const char *filename)
{
	GtkWidget *dialog;
	GtkWidget *box;
	GtkWidget *label;
	GtkWidget *button;
	gchar *message;

	// Crea una nuova finestra
	dialog = gtk_window_new();
	gtk_window_set_title(GTK_WINDOW(dialog), "Errore");
	gtk_window_set_transient_for(GTK_WINDOW(dialog), parent);
	gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
	gtk_window_set_resizable(GTK_WINDOW(dialog), FALSE);
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

	// Connetti il segnale per chiudere la finestra
	g_signal_connect_swapped(button, "clicked", G_CALLBACK(gtk_window_close), dialog);

	// Mostra la finestra
	gtk_window_present(GTK_WINDOW(dialog));

	// Cleanup
	g_error_free(error);
}

static void litos_app_window_saved_notify_cb(GObject *gobject G_GNUC_UNUSED,
 GParamSpec *pspec G_GNUC_UNUSED, gpointer user_data)
{
	LitosAppWindow *win = LITOS_APP_WINDOW(user_data);
	if (!win)
		return;

	litos_app_window_update_title(win);
}

static LitosFile *
litos_app_window_new_tab(LitosAppWindow *win, struct Page *page)
{
	LitosFile *file = litos_file_set(page);

	g_signal_connect(G_OBJECT(file), "notify::saved", G_CALLBACK(litos_app_window_saved_notify_cb), win);

	// Pulsante di chiusura
	GtkWidget *close_btn = gtk_button_new();
	GtkWidget *close_icon = gtk_image_new_from_icon_name("window-close-symbolic");
	gtk_button_set_child(GTK_BUTTON(close_btn), close_icon);
	gtk_widget_set_focusable(close_btn, FALSE);

	//Insert widgets in container box

	gtk_box_append(GTK_BOX(page->close_btn_box), page->lbl);
	gtk_box_append(GTK_BOX(page->close_btn_box), close_btn);
	gtk_box_append(GTK_BOX(page->tabbox), page->scrolled);

	// Aggiunta al notebook
	gint page_num = gtk_notebook_append_page_menu(
		win->notebook,
		page->tabbox,
		page->close_btn_box,
		NULL);

	gtk_notebook_set_current_page(win->notebook, page_num);
	gtk_notebook_set_tab_reorderable(win->notebook, page->tabbox, TRUE);

	// Applica stile ridotto se ci sono più di 6 tab
	if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(win->notebook)) > 6)
		gtk_widget_add_css_class(page->lbl, "tab-label-small");

	// Associazione del file
	g_ptr_array_add(win->litosFileList, file);
	g_object_set_data(G_OBJECT(page->tabbox), "litos-file", file);
	litos_file_set_tabbox(file, page->tabbox);

	g_object_set_data(G_OBJECT(close_btn), "litos-file", file);
	g_signal_connect(close_btn, "clicked", G_CALLBACK(litos_app_window_close_btn_clicked), win);

	litos_app_window_update_title(win);

	return file;
}

LitosFile * litos_app_window_open(LitosAppWindow *win, GFile *gf)
{
	GError *error = NULL;
	gchar *basename = g_file_get_basename(gf); // va sempre liberata

	// Crea la Page
	struct Page page = litos_page_new_from_file(gf);

	// Crea il LitosFile e carica il contenuto
	LitosFile *file = litos_file_set(&page);
	if (!litos_file_load(file, &error)) {
		litos_app_window_error_dialog(GTK_WINDOW(win), error, basename);
		g_object_unref(file);
		g_free(basename); // liberata in caso di errore
		return NULL;
	}

	// Applica evidenziazione con schema centralizzato
	LitosApp *app = LITOS_APP(gtk_window_get_application(GTK_WINDOW(win)));
	litos_file_highlight_buffer(file, app);

	g_free(basename); // liberata anche in caso di successo

	// Crea la tab
	return litos_app_window_new_tab(win, &page);
}

void litos_app_window_new_file(LitosAppWindow *win)
{
	int file_index = litos_app_window_get_array_len(win) + 1;
	gchar *name = g_strdup_printf("Untitled %d", file_index);

	struct Page page = litos_page_new_empty(name);
	g_free(name);

	LitosFile *file = litos_app_window_new_tab(win, &page);

	// Tab vuota: non salvata
	litos_file_set_unsaved(file); // imposta saved = FALSE e notifica
}

LitosFile * litos_app_window_get_file(LitosAppWindow *win, int *i)
{
	return g_ptr_array_index(win->litosFileList,*i);
}

GtkNotebook * litos_app_window_get_nb(LitosAppWindow *win)
{
	return win->notebook;
}

void litos_app_window_restore_session (LitosAppWindow *win)
{
    // Leggiamo la nuova chiave "last-session-files"
    gchar **files = g_settings_get_strv (win->settings, "last-session-files");

    if (files) {
        for (int i = 0; files[i] != NULL; i++) {
            // Usiamo g_file_new_for_path perché abbiamo salvato path assoluti, non URI
            GFile *gf = g_file_new_for_path (files[i]);
            
            if (g_file_query_exists (gf, NULL)) {
                litos_app_window_open (win, gf);
                litos_app_window_update_title (win);
            }
            g_object_unref (gf);
        }
        g_strfreev (files);
    }
}
