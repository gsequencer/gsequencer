/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_RAMP_MARKER_POPOVER_CALLBACKS_H__
#define __AGS_RAMP_MARKER_POPOVER_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/app/editor/ags_ramp_marker_popover.h>

G_BEGIN_DECLS

void ags_ramp_marker_popover_control_name_callback(GtkComboBox *combo_box,
						   AgsRampMarkerPopover *ramp_marker_popover);

void ags_ramp_marker_popover_machine_changed_callback(AgsCompositeEditor *composite_editor,
						      AgsMachine *machine,
						      AgsRampMarkerPopover *ramp_marker_popover);

G_END_DECLS

#endif /*__AGS_RAMP_MARKER_POPOVER_CALLBACKS_H__*/
