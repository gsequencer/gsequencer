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

#ifndef __AGS_SHEET_EDITOR_CALLBACKS_H__
#define __AGS_SHEET_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/app/ags_sheet_editor.h>
#include <ags/app/ags_machine.h>

G_BEGIN_DECLS

void ags_sheet_editor_machine_changed_callback(AgsMachineSelector *machine_selector, AgsMachine *machine,
					       AgsSheetEditor *sheet_editor);

void ags_sheet_editor_resize_audio_channels_callback(AgsMachine *machine, 
						     guint audio_channels, guint audio_channels_old,
						     AgsSheetEditor *sheet_editor);
void ags_sheet_editor_resize_pads_callback(AgsMachine *machine, GType channel_type,
					   guint pads, guint pads_old,
					   AgsSheetEditor *sheet_editor);

G_END_DECLS

#endif /*__AGS_SHEET_EDITOR_CALLBACKS_H__*/
