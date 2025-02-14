/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_EXPORT_WINDOW_CALLBACKS_H__
#define __AGS_EXPORT_WINDOW_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_export_window.h>

G_BEGIN_DECLS

gboolean ags_export_window_close_request_callback(GtkWindow *window, gpointer user_data);

void ags_export_window_add_export_soundcard_callback(GtkWidget *button,
						     AgsExportWindow *export_window);
void ags_export_window_remove_export_soundcard_callback(GtkWidget *button,
							AgsExportWindow *export_window);

void ags_export_window_tact_callback(GtkWidget *spin_button,
				     AgsExportWindow *export_window);

void ags_export_window_export_callback(GObject *gobject,
				       GParamSpec *pspec,
				       AgsExportWindow *export_window);

void ags_export_window_update_ui_callback(AgsApplicationContext *application_context,
					  AgsExportWindow *export_window);

void ags_export_window_stop_callback(AgsThread *thread,
				     AgsExportWindow *export_window);

G_END_DECLS

#endif /*__AGS_EXPORT_WINDOW_CALLBACKS_H__*/
