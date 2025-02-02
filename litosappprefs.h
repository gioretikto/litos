/*	Copyright (C) 2023-2024 Giovanni Resta <giovannirestadev@gmail.com>

	This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __LITOSPPPREFS_H
#define __LITOSAPPPREFS_H

#include <gtk/gtk.h>
#include "litosappwin.h"


#define LITOS_APP_PREFS_TYPE (litos_app_prefs_get_type ())
G_DECLARE_FINAL_TYPE (LitosAppPrefs, litos_app_prefs, LITOS, APP_PREFS, GtkDialog)


LitosAppPrefs        *litos_app_prefs_new          (LitosAppWindow *win);


#endif /* __LITOSAPPPREFS_H */
