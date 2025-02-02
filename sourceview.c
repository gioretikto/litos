/*	Copyright (C) 2023-2024 Giovanni Resta <giovannirestadev@gmail.com>

	This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>

#include "litosappwin.h"

GtkWidget* MyNewSourceview()
{
	GtkWidget *source_view;

	GtkSourceBuffer *source_buffer = gtk_source_buffer_new (NULL);

	GtkSourceStyleSchemeManager *scheme_manager = gtk_source_style_scheme_manager_get_default();
	const gchar * const* schemes = gtk_source_style_scheme_manager_get_scheme_ids(scheme_manager);

	if (schemes != NULL && schemes[0] != NULL)
	{
		GtkSourceStyleScheme *scheme = gtk_source_style_scheme_manager_get_scheme(scheme_manager, schemes[1]);

		if (scheme != NULL)
		{
			source_view = gtk_source_view_new_with_buffer (source_buffer);
			gtk_source_buffer_set_style_scheme(source_buffer, scheme);
		}
	}

	gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW((source_view)), TRUE);

	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(source_view), GTK_WRAP_WORD_CHAR);

	return source_view;
}
