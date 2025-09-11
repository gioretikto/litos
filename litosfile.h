/*	Copyright (C) 2023-2024 Giovanni Resta <giovannirestadev@gmail.com>

	This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _LITOS_FILE_H
#define _LITOS_FILE_H

G_BEGIN_DECLS

#define LITOS_TYPE_FILE (litos_file_get_type ())

// forward declarations
typedef struct _LitosFile LitosFile;
typedef struct _LitosApp LitosApp;

struct Page;

gboolean litos_file_load (LitosFile *file, GError **error);
gboolean litos_file_save(LitosFile *file, GError **error);
void litos_file_save_as(LitosFile* file, GFile *new_file);
GtkTextBuffer *litos_file_get_buffer(LitosFile *file);
GFile *litos_file_get_gfile(LitosFile* file);
GtkWidget * litos_file_get_lbl(LitosFile *file);
gchar *litos_file_get_name(LitosFile *file);
gboolean litos_file_get_saved(LitosFile *file);
GtkWidget * litos_file_get_tabbox(LitosFile *file);
GtkWidget * litos_file_get_view(LitosFile *file);
void litos_file_highlight_buffer(LitosFile *file, LitosApp *app);
void litos_file_reset_gfile(LitosFile *file);
LitosFile * litos_file_set(struct Page *page);
void litos_file_set_tabbox(LitosFile *file, GtkWidget *tabbox);
void litos_file_set_unsaved(LitosFile *file);

G_DECLARE_FINAL_TYPE(LitosFile, litos_file, LITOS, FILE, GObject)

LitosFile *          litos_file_new           ();

G_END_DECLS

#endif
