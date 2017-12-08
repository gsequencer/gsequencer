/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_NOTATION_EDITOR_CALLBACKS_H__
#define __AGS_NOTATION_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/ags_notation_editor.h>
#include <ags/X/ags_machine.h>

void ags_notation_editor_machine_changed_callback(AgsMachineSelector *machine_selector, AgsMachine *machine,
						  AgsNotationEditor *notation_editor);

void ags_notation_editor_set_pads_callback(AgsAudio *audio, GType channel_type,
					   guint pads, guint pads_old,
					   AgsNotationEditor *notation_editor);

void ags_notation_editor_init_channel_launch_callback(AgsTask *task, AgsNote *note);

#endif /*__AGS_NOTATION_EDITOR_CALLBACKS_H__*/
