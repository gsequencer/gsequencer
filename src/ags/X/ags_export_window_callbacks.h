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

#ifndef __AGS_EXPORT_WINDOW_CALLBACKS_H__
#define __AGS_EXPORT_WINDOW_CALLBACKS_H__

#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_export_window.h>

void ags_export_window_file_chooser_button_callback(GtkWidget *file_chooser_button,
						    AgsExportWindow *export_window);

void ags_export_window_tact_callback(GtkWidget *spin_button,
				     AgsExportWindow *export_window);

void ags_export_window_export_callback(GtkWidget *toggle_button,
				       AgsExportWindow *export_window);

#endif /*__AGS_EXPORT_WINDOW_CALLBACKS_H__*/
