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

#ifndef __AGS_FM_SYNCSYNTH_CALLBACKS_H__ 
#define __AGS_FM_SYNCSYNTH_CALLBACKS_H__ 

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/GSequencer/machine/ags_fm_syncsynth.h>

G_BEGIN_DECLS

void ags_fm_syncsynth_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsFMSyncsynth *fm_syncsynth);

void ags_fm_syncsynth_samplerate_changed_callback(AgsMachine *machine,
						  guint samplerate, guint old_samplerate,
						  gpointer user_data);

void ags_fm_syncsynth_fm_oscillator_control_changed_callback(AgsFMOscillator *fm_oscillator,
							     AgsFMSyncsynth *fm_syncsynth);

void ags_fm_syncsynth_add_callback(GtkButton *button, AgsFMSyncsynth *fm_syncsynth);
void ags_fm_syncsynth_remove_callback(GtkButton *button, AgsFMSyncsynth *fm_syncsynth);

void ags_fm_syncsynth_auto_update_callback(GtkToggleButton *toggle, AgsFMSyncsynth *fm_syncsynth);
void ags_fm_syncsynth_update_callback(GtkButton *button, AgsFMSyncsynth *fm_syncsynth);

void ags_fm_syncsynth_lower_callback(GtkSpinButton *spin_button, AgsFMSyncsynth *fm_syncsynth);
void ags_fm_syncsynth_loop_start_callback(GtkSpinButton *spin_button, AgsFMSyncsynth *fm_syncsynth);
void ags_fm_syncsynth_loop_end_callback(GtkSpinButton *spin_button, AgsFMSyncsynth *fm_syncsynth);

void ags_fm_syncsynth_enable_lfo_callback(GtkToggleButton *toggle, AgsFMSyncsynth *fm_syncsynth);

void ags_fm_syncsynth_lfo_freq_callback(GtkSpinButton *spin_button, AgsFMSyncsynth *fm_syncsynth);
void ags_fm_syncsynth_lfo_phase_callback(GtkSpinButton *spin_button, AgsFMSyncsynth *fm_syncsynth);
void ags_fm_syncsynth_lfo_depth_callback(GtkSpinButton *spin_button, AgsFMSyncsynth *fm_syncsynth);
void ags_fm_syncsynth_lfo_tunint_callback(GtkSpinButton *spin_button, AgsFMSyncsynth *fm_syncsynth);

void ags_fm_syncsynth_volume_callback(GtkRange *range, AgsFMSyncsynth *fm_syncsynth);

G_END_DECLS

#endif /*__AGS_FM_SYNCSYNTH_CALLBACKS_H__ */
