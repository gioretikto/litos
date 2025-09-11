/*	Copyright (C) 2023-2024 Giovanni Resta <giovannirestadev@gmail.com>

	This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __LITOSAPPWIN_H
#define __LITOSAPPWIN_H

//Forward declarations
typedef struct _LitosAppWindow LitosAppWindow;
typedef struct _LitosFile LitosFile;
typedef struct _LitosApp LitosApp;

void litos_app_window_ctrl_f(LitosAppWindow *win);
LitosFile * litos_app_window_current_file(LitosAppWindow *win);
void litos_app_window_Esc(LitosAppWindow *win);
guint litos_app_window_get_array_len(LitosAppWindow *win);
LitosFile * litos_app_window_get_file(LitosAppWindow *win, int *i);
GtkNotebook * litos_app_window_get_nb(LitosAppWindow *win);
void litos_app_window_initialize_star_if_needed(LitosFile *file);
LitosFile * litos_app_window_open(LitosAppWindow *win, GFile *gf);
void litos_app_window_new_file(LitosAppWindow *win);
gboolean litos_app_window_save(LitosAppWindow *win, LitosFile *file);
void litos_app_window_save_as(LitosAppWindow *app);
gboolean litos_app_window_remove_child(LitosAppWindow *win);
void litos_app_window_update_font();

#define LITOS_APP_WINDOW_TYPE (litos_app_window_get_type ())
G_DECLARE_FINAL_TYPE (LitosAppWindow, litos_app_window, LITOS, APP_WINDOW, GtkApplicationWindow)

LitosAppWindow       *litos_app_window_new          (LitosApp *app);

#endif /* __LITOSAPPWIN_H */
