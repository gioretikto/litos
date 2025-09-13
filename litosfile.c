/*	Copyright (C) 2023-2024 Giovanni Resta <giovannirestadev@gmail.com>

	This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

#include "litosfile.h"
#include "litospage.h"
#include "litosappwin.h"
#include "litosapp.h"

GtkSourceStyleScheme *litos_app_get_style_scheme(LitosApp *app);

struct _LitosFile
{
	GObject parent;

	GFile* gfile;

	GtkWidget *scrolled;

	GtkWidget *lbl;

	GtkWidget* tabbox;

	GtkWidget* close_btn_box;

	/*GtkSourceView*/

	GtkWidget *view;

	/* the text buffer of the file */
	GtkTextBuffer *buffer;

	/*filename */
	gchar *name;

	gboolean saved;
};

G_DEFINE_TYPE (LitosFile, litos_file, G_TYPE_OBJECT)

static void
litos_file_init (LitosFile *file)
{
	file->gfile = NULL;

	file->buffer = NULL;

	file->name = g_strdup("");

	file->lbl = NULL;

	file->saved = TRUE;

	file->scrolled = NULL;
}

typedef enum
{
	PROP_SAVED = 1,
	N_PROPERTIES

} LitosFileProperty;

static GParamSpec *obj_properties[N_PROPERTIES] = { NULL, };

void litos_file_set_unsaved(LitosFile *file)
{
	file->saved = FALSE;
	g_object_notify_by_pspec (G_OBJECT (file), obj_properties[PROP_SAVED]); //Emission of signal for the "saved" property
}

static void litos_file_buffer_monitor_change(GtkTextBuffer *buffer G_GNUC_UNUSED,
 gpointer userdata)
{
	LitosFile *file = LITOS_FILE(userdata);
	litos_file_set_unsaved(file);
	g_object_notify_by_pspec(G_OBJECT(file), obj_properties[PROP_SAVED]);
}

void litos_file_reset_gfile(LitosFile *file)
{
	g_clear_object(&file->gfile);
}

static void litos_file_dispose(GObject *object)
{
	LitosFile *file = LITOS_FILE(object);

	// Disconnette il segnale sul buffer
	if (GTK_IS_TEXT_BUFFER(file->buffer))
	g_signal_handlers_disconnect_by_func(file->buffer, litos_file_buffer_monitor_change, file);

	// Libera la stringa del nome
	g_free(file->name);
	file->name = NULL;

	// Rilascia l'oggetto GFile in modo sicuro
	litos_file_reset_gfile(file);

	// I widget GTK sono gestiti dai container: non fare unref

	// Chiamata al dispose della superclasse
	G_OBJECT_CLASS(litos_file_parent_class)->dispose(object);
}


static void litos_file_set_property (GObject *object,
  guint  property_id,
  const GValue *value,
  GParamSpec *pspec)
{
	LitosFile *self = LITOS_FILE (object);

	switch ((LitosFileProperty) property_id)
	{
		case PROP_SAVED:
			self->saved = g_value_get_boolean (value);
			break;

		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
litos_file_get_property (GObject *object, guint property_id, GValue *value, GParamSpec *pspec)
{
	LitosFile *self = LITOS_FILE (object);

	switch ((LitosFileProperty) property_id)
	{
		case PROP_SAVED:
			g_value_set_boolean (value, self->saved);
			break;

		default:
			/* We don't have any other property... */
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void
litos_file_class_init (LitosFileClass *class)
{

	/*Nel sistema GObject, il metodo dispose non viene mai chiamato manualmente.
	Viene invocato automaticamente dal sistema quando fai: g_object_unref(file); */

	G_OBJECT_CLASS (class)->dispose = litos_file_dispose;
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = litos_file_set_property;
	object_class->get_property = litos_file_get_property;

	obj_properties[PROP_SAVED] =
		g_param_spec_boolean ("saved",
		 "Save",
		 "File status",
		 FALSE	/* default value */,
		 G_PARAM_READWRITE);

	g_object_class_install_properties (object_class,
		N_PROPERTIES,
		obj_properties);
}

LitosFile *litos_file_new()
{
	return g_object_new (LITOS_TYPE_FILE, NULL);
}

GtkWidget * litos_file_get_lbl(LitosFile *file)
{
	return file->lbl;
}

GtkWidget * litos_file_get_view(LitosFile *file)
{
	return file->view;
}

GFile *litos_file_get_gfile(LitosFile* file)
{
	return file->gfile;
}

gchar *litos_file_get_name(LitosFile *file)
{
	return file->name;
}

GtkTextBuffer *litos_file_get_buffer(LitosFile *file)
{
	return file->buffer;
}

gboolean litos_file_get_saved(LitosFile *file)
{
	return file->saved;
}

GtkWidget * litos_file_get_tabbox(LitosFile *file)
{
	return file->tabbox;
}

void litos_file_set_tabbox(LitosFile *file, GtkWidget *tabbox)
{
file->tabbox = tabbox;
}

void litos_file_set_saved(LitosFile *file)
{
	file->saved = TRUE;
}

LitosFile *litos_file_set(struct Page *page)
{
	// Crea un nuovo oggetto LitosFile
	LitosFile *file = litos_file_new();

	// Salva i puntatori (senza aumentare il refcount)
	file->gfile = page->gf;
	file->scrolled  = page->scrolled;
	file->tabbox= page->tabbox;
	file->close_btn_box = page->close_btn_box;
	file->view  = page->view;
	file->lbl   = page->lbl;
	file->buffer= page->buffer;

	// Fai ref solo su gfile, se necessario
	if (G_IS_OBJECT(file->gfile))
		g_object_ref(file->gfile);

	// Copia il nome del file, se presente
	if (file->name)
		g_free(file->name);

	if (page->name) {
		file->name = page->name;
		page->name = NULL;
	} else if (G_IS_FILE(page->gf)) {
		file->name = g_file_get_basename(page->gf); // va liberato in destroy
	} else {
	    file->name = NULL;
	}

	// Collega il segnale sul buffer, se è valido
	if (GTK_IS_TEXT_BUFFER(file->buffer))
		g_signal_connect(file->buffer, "changed", G_CALLBACK(litos_file_buffer_monitor_change), file);

	return file;
}

void
litos_file_highlight_buffer (LitosFile *file, LitosApp *app)
{
	/* Controlli difensivi su file e buffer */
	if (!file || !file->buffer) {
		g_warning ("litos_file_highlight_buffer: file o buffer NULL");
		return;
	}

	/* Verifica che sia davvero un GtkSourceBuffer */
	if (!GTK_SOURCE_IS_BUFFER (file->buffer)) {
		g_warning ("litos_file_highlight_buffer: il buffer non è un GtkSourceBuffer, salto evidenziazione");
		return;
	}

	GtkSourceBuffer *source_buffer = GTK_SOURCE_BUFFER (file->buffer);
	GtkSourceLanguageManager *lm = gtk_source_language_manager_get_default ();
	GtkSourceLanguage *lang = NULL;
	gchar *content_type = NULL;

	/* Prova a determinare la lingua dal nome file e dal content type */
	if (file->name && *file->name) {
		gchar *path = g_file_get_path (file->gfile);
		if (path) {
			/* content_type è una nuova stringa da liberare */
			content_type = g_content_type_guess (path, NULL, 0, NULL);
			g_free (path);
		}

		lang = gtk_source_language_manager_guess_language (lm, file->name, content_type);

		/* libera content_type se allocata */
		g_free (content_type);
	}

	/* Fallback a HTML se non riconosciuto */
	if (!lang)
		lang = gtk_source_language_manager_get_language (lm, "html");

	/* Imposta sintassi solo se abbiamo una lingua valida */
	if (lang) {
		gtk_source_buffer_set_language (source_buffer, lang);
		gtk_source_buffer_set_highlight_syntax (source_buffer, TRUE);
	} else {
		gtk_source_buffer_set_highlight_syntax (source_buffer, FALSE);
	}

	/* Applica lo schema di stile centralizzato se disponibile */
	GtkSourceStyleScheme *scheme = litos_app_get_style_scheme (app);
	if (scheme)
		gtk_source_buffer_set_style_scheme (source_buffer, scheme);
	}

gboolean litos_file_load(LitosFile *file, GError **error)
{
	char *contents = NULL;
	gsize length = 0;

	if (!file || !file->gfile || !file->buffer)
		return FALSE;

	if (!g_file_load_contents(file->gfile, NULL, &contents, &length, NULL, error))
		return FALSE;

	gchar *path = g_file_get_path(file->gfile);
	gchar *content_type = g_content_type_guess(path, (const guchar *)contents, length, NULL);
	g_free(path);

	// Accetta solo file testuali
	if (!g_content_type_is_a(content_type, "text/plain")) {
		g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
		"Il file non è di tipo testuale (%s)", content_type);
		g_free(content_type);
		g_free(contents);
		return FALSE;
	}

	// Verifica che il contenuto sia UTF-8 valido
	if (!g_utf8_validate(contents, length, NULL)) {
			g_set_error(error, G_FILE_ERROR, G_FILE_ERROR_FAILED,
		"Il contenuto del file non è UTF-8 valido.");
		g_free(content_type);
		g_free(contents);
		return FALSE;
	}

	gtk_text_buffer_set_text(file->buffer, contents, length);
	g_object_notify_by_pspec(G_OBJECT(file), obj_properties[PROP_SAVED]);
	file->saved = TRUE;

	g_free(content_type);
	g_free(contents);

	return TRUE;
}


gboolean litos_file_save(LitosFile *file, GError **error)
{
	if (file->gfile != NULL)
	{
		GtkTextIter start_iter;
		GtkTextIter end_iter;

		gtk_text_buffer_get_bounds(file->buffer, &start_iter, &end_iter);
		char *contents = gtk_text_buffer_get_text(file->buffer, &start_iter, &end_iter, TRUE);

		gboolean success = g_file_replace_contents(file->gfile,
			contents,
			strlen(contents),
			NULL,
			FALSE,
			G_FILE_CREATE_NONE,
			NULL,
			NULL,
			error);

		g_free(contents);

		if (!success)
			return FALSE;

		file->saved = TRUE;
		g_object_notify_by_pspec(G_OBJECT(file), obj_properties[PROP_SAVED]);

		litos_app_window_initialize_star_if_needed(file);
	}

	return TRUE;
}

void litos_file_save_as(LitosFile* file, GFile *new_file)
{
	g_set_object(&file->gfile, new_file);

	g_free (file->name);

	if (new_file != NULL)
		file->name = g_file_get_basename(new_file);
	else
		file->name = NULL;

	litos_file_save(file, NULL);
}
