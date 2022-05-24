/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_TIME_STRETCH_BUFFER_DIALOG_CALLBACKS_H__
#define __AGS_TIME_STRETCH_BUFFER_DIALOG_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_time_stretch_buffer_dialog.h>

G_BEGIN_DECLS

void ags_time_stretch_buffer_dialog_response_callback(GtkWidget *dialog, gint response,
						      AgsTimeStretchBufferDialog *time_stretch_buffer_dialog);

G_END_DECLS

#endif /*__AGS_TIME_STRETCH_BUFFER_DIALOG_CALLBACKS_H__*/
