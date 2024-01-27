#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

#include "litosfile.h"
#include "page.h"

void litos_file_set_unsaved(LitosFile *file);

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

	file->name = NULL;

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

static void _buffer_monitor_change(GObject *gobject, GParamSpec *pspec, gpointer userdata)
{
	LitosFile *file = LITOS_FILE(userdata);
	litos_file_set_unsaved(file);
	g_object_notify_by_pspec (G_OBJECT (file), obj_properties[PROP_SAVED]);
}

static void
litos_file_dispose (GObject *object)
{
	LitosFile *file = LITOS_FILE (object);

	g_signal_handlers_disconnect_by_func(file->buffer, _buffer_monitor_change, file);

	g_free (file->name);

	G_OBJECT_CLASS (litos_file_parent_class)->dispose (object);
}

static void
litos_file_set_property (GObject *object,
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
litos_file_get_property (GObject *object,
                          guint property_id,
                          GValue *value,
                          GParamSpec *pspec)
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
	G_OBJECT_CLASS (class)->dispose = litos_file_dispose;
	GObjectClass *object_class = G_OBJECT_CLASS (class);

	object_class->set_property = litos_file_set_property;
	object_class->get_property = litos_file_get_property;

	obj_properties[PROP_SAVED] =
		g_param_spec_string ("saved",
		                 "Save",
		                 "File status",
		                 NULL  /* default value */,
		                 G_PARAM_READWRITE);

	g_object_class_install_properties (object_class,
		N_PROPERTIES,
		obj_properties);
}

LitosFile *litos_file_new()
{
	return g_object_new (LITOS_TYPE_FILE, NULL);
}

void litos_file_set_gfile(LitosFile *file, GFile *gfile)
{
	file->gfile = gfile;
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
	g_object_ref(file->gfile);
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

void litos_file_reset_gfile(LitosFile *file)
{
	file->gfile = NULL;
}

GtkWidget * litos_file_get_tabbox(LitosFile *file)
{
	return file->tabbox;
}

void litos_file_set_saved(LitosFile *file)
{
	file->saved = TRUE;
}

void litos_file_set_unsaved(LitosFile *file)
{
	file->saved = FALSE;
	g_object_notify_by_pspec (G_OBJECT (file), obj_properties[PROP_SAVED]);
}

LitosFile * litos_file_set(struct Page *page)
{
	LitosFile *file = litos_file_new();

	file->gfile = page->gf;
	g_object_ref(page->gf);
	
	file->name = page->name;
	file->scrolled = page->scrolled;
	file->tabbox = page->tabbox;
	file->close_btn_box = page->close_btn_box;
	file->view = page->view;
	file->lbl = page->lbl;
	file->buffer = page->buffer;

	g_signal_connect (file->buffer, "notify::text", G_CALLBACK (_buffer_monitor_change), file);

	return file;
}

void litos_file_highlight_buffer(LitosFile *file) /* Apply different font styles depending on file extension .html .c, etc */
{
	GtkSourceLanguageManager *lm = gtk_source_language_manager_get_default();

	GtkSourceLanguage *lang;

	GtkSourceBuffer *source_buffer = GTK_SOURCE_BUFFER(file->buffer);

	if ((lang = gtk_source_language_manager_guess_language(lm, file->name, NULL)) == NULL)
		lang = gtk_source_language_manager_get_language(lm,"html");

	if (lang != NULL)
	{	gtk_source_buffer_set_language (source_buffer, lang);
		gtk_source_buffer_set_highlight_syntax (source_buffer, TRUE);
	}

	GtkSourceStyleSchemeManager *scheme_manager = gtk_source_style_scheme_manager_get_default();
	const gchar * const* schemes = gtk_source_style_scheme_manager_get_scheme_ids(scheme_manager);

	if (schemes != NULL && schemes[0] != NULL)
	{
		GtkSourceStyleScheme *scheme = gtk_source_style_scheme_manager_get_scheme(scheme_manager,
			schemes[8]);

		if (scheme != NULL)
			gtk_source_buffer_set_style_scheme(source_buffer, scheme);
	}
}

gboolean litos_file_load (LitosFile *file, GError **error)
{
	char *contents;
	gsize length;

	if (g_file_load_contents (file->gfile, NULL, &contents, &length, NULL, error))
	{
		gtk_text_buffer_set_text (file->buffer, contents, length);
		g_object_notify_by_pspec (G_OBJECT (file), obj_properties[PROP_SAVED]);
		file->saved = TRUE;
		g_free (contents);
		return TRUE;
	}

	else
		return FALSE;
}

gboolean litos_file_save(LitosFile *file, GError *error)
{
	if (file->gfile != NULL)
	{
		char *contents;
		GtkTextIter start_iter;
		GtkTextIter end_iter;

		gtk_text_buffer_get_bounds(file->buffer, &start_iter, &end_iter);
		contents = gtk_text_buffer_get_text(file->buffer, &start_iter, &end_iter, TRUE);

		if (!g_file_replace_contents(file->gfile, contents, strlen(contents), NULL, TRUE, G_FILE_CREATE_NONE, NULL, NULL, &error))
		{
			g_free(contents);
			return FALSE;
		}

		else
		{
			file->saved = TRUE;
			g_object_notify_by_pspec (G_OBJECT (file), obj_properties[PROP_SAVED]);
			g_free(contents);
		}
	}

	return TRUE;
}

void litos_file_save_as(LitosFile* file, GFile *new_file)
{
	if (new_file != NULL)
		g_object_ref(new_file);

	file->gfile = new_file;

	g_free (file->name);

	file->name = g_file_get_basename(new_file);

	litos_file_save(file, NULL);
}
