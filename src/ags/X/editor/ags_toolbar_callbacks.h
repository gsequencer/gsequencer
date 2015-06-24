/* This file is part of GSequencer.
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

#ifndef __AGS_TOOLBAR_CALLBACKS_H__
#define __AGS_TOOLBAR_CALLBACKS_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/editor/ags_toolbar.h>

gboolean ags_toolbar_destroy_callback(GtkObject *object, AgsToolbar *toolbar);
void ags_toolbar_show_callback(GtkWidget *widget, AgsToolbar *toolbar);

void ags_toolbar_position_callback(GtkToggleButton *toggle_button, AgsToolbar *toolbar);
void ags_toolbar_edit_callback(GtkToggleButton *toggle_button, AgsToolbar *toolbar);
void ags_toolbar_clear_callback(GtkToggleButton *toggle_button, AgsToolbar *toolbar);
void ags_toolbar_select_callback(GtkToggleButton *toggle_button, AgsToolbar *toolbar);

void ags_toolbar_copy_or_cut_callback(GtkWidget *widget, AgsToolbar *toolbar);
void ags_toolbar_paste_callback(GtkWidget *widget, AgsToolbar *toolbar);
void ags_toolbar_invert_callback(GtkWidget *widget, AgsToolbar *toolbar);

void ags_toolbar_zoom_callback(GtkComboBox *combo_box, AgsToolbar *toolbar);

void ags_toolbar_mode_callback(GtkWidget *widget, AgsToolbar *toolbar);

#endif /*__AGS_TOOLBAR_CALLBACKS_H__*/
