/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __AGS_LV2_BROWSER_CALLBACKS_H__
#define __AGS_LV2_BROWSER_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_lv2_browser.h>

G_BEGIN_DECLS

void ags_lv2_browser_plugin_filename_callback(GtkTreeView *tree_view,
					      GtkTreePath *path,
					      GtkTreeViewColumn *column,
					      AgsLv2Browser *lv2_browser);
void ags_lv2_browser_plugin_uri_callback(GtkTreeView *tree_view,
					 GtkTreePath *path,
					 GtkTreeViewColumn *column,
					 AgsLv2Browser *lv2_browser);

G_END_DECLS

#endif /*__AGS_LV2_BROWSER_CALLBACKS_H__*/
