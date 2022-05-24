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

#ifndef __AGS_SYNCSYNTH_CALLBACKS_H__ 
#define __AGS_SYNCSYNTH_CALLBACKS_H__ 

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/machine/ags_syncsynth.h>

G_BEGIN_DECLS

void ags_syncsynth_samplerate_changed_callback(AgsMachine *machine,
					       guint samplerate, guint old_samplerate,
					       gpointer user_data);

void ags_syncsynth_oscillator_control_changed_callback(AgsOscillator *oscillator,
						       AgsSyncsynth *syncsynth);

void ags_syncsynth_add_callback(GtkButton *button, AgsSyncsynth *syncsynth);
void ags_syncsynth_remove_callback(GtkButton *button, AgsSyncsynth *syncsynth);

void ags_syncsynth_auto_update_callback(GtkToggleButton *toggle, AgsSyncsynth *syncsynth);
void ags_syncsynth_update_callback(GtkButton *button, AgsSyncsynth *syncsynth);

void ags_syncsynth_lower_callback(GtkSpinButton *spin_button, AgsSyncsynth *syncsynth);
void ags_syncsynth_loop_start_callback(GtkSpinButton *spin_button, AgsSyncsynth *syncsynth);
void ags_syncsynth_loop_end_callback(GtkSpinButton *spin_button, AgsSyncsynth *syncsynth);

void ags_syncsynth_volume_callback(GtkRange *range, AgsSyncsynth *syncsynth);

G_END_DECLS

#endif /*__AGS_SYNCSYNTH_CALLBACKS_H__ */
