/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_COMPOSITE_EDITOR_CALLBACKS_H__
#define __AGS_COMPOSITE_EDITOR_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/app/editor/ags_machine_selector.h>

G_BEGIN_DECLS

void ags_composite_editor_edit_viewport_vadjustment_changed_callback(GtkAdjustment *adjustment,
								     AgsCompositeEditor *composite_editor);

void ags_composite_editor_edit_vadjustment_value_changed_callback(GtkAdjustment *adjustment,
								  AgsCompositeEditor *composite_editor);

void ags_composite_editor_automation_edit_hadjustment_changed_callback(GtkAdjustment *adjustment,
								       AgsCompositeEditor *composite_editor);

void ags_composite_editor_edit_hadjustment_value_changed_callback(GtkAdjustment *adjustment,
								  AgsCompositeEditor *composite_editor);

void ags_composite_editor_wave_edit_hadjustment_changed_callback(GtkAdjustment *adjustment,
								 AgsCompositeEditor *composite_editor);

void ags_composite_editor_machine_selector_changed_callback(AgsMachineSelector *machine_selector,
							    AgsMachine *machine,
							    AgsCompositeEditor *composite_editor);

void ags_composite_editor_resize_audio_channels_callback(AgsMachine *machine, 
							 guint audio_channels, guint audio_channels_old,
							 AgsCompositeEditor *composite_editor);
void ags_composite_editor_resize_pads_callback(AgsMachine *machine, GType channel_type,
					       guint pads, guint pads_old,
					       AgsCompositeEditor *composite_editor);

void ags_composite_editor_automation_edit_hscrollbar_value_changed(GtkAdjustment *adjustment,
								   AgsCompositeEditor *composite_editor);

void ags_composite_editor_wave_edit_hscrollbar_value_changed(GtkAdjustment *adjustment,
							     AgsCompositeEditor *composite_editor);


G_END_DECLS

#endif /*__AGS_COMPOSITE_EDITOR_CALLBACKS_H__*/
