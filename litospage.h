/*	Copyright (C) 2023-2024 Giovanni Resta <giovannirestadev@gmail.com>

	This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LITOS_PAGE_H
#define LITOS_PAGE_H

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include <gio/gio.h>

struct Page {
    gchar *name;
    GFile *gf;
    GtkWidget *view;
    GtkWidget *scrolled;
    GtkWidget *lbl;
    GtkWidget *tabbox;
    GtkWidget *close_btn_box;
    GtkTextBuffer *buffer;
};

struct Page litos_page_new_empty(const gchar *name);
struct Page litos_page_new_from_file(GFile *gf);
struct Page litos_page_new_from_template(const gchar *name, const gchar *template_text);

#endif

