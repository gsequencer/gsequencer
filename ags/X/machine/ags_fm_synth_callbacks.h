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

#ifndef __AGS_FM_SYNTH_CALLBACKS_H__ 
#define __AGS_FM_SYNTH_CALLBACKS_H__ 

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/machine/ags_fm_synth.h>

void ags_fm_synth_parent_set_callback(GtkWidget *widget, GtkObject *old_parent, AgsFMSynth *fm_synth);

void ags_fm_synth_auto_update_callback(GtkToggleButton *toggle, AgsFMSynth *fm_synth);
void ags_fm_synth_update_callback(GtkButton *button, AgsFMSynth *fm_synth);

void ags_fm_synth_lower_callback(GtkSpinButton *spin_button, AgsFMSynth *fm_synth);

void ags_fm_synth_enable_lfo_callback(GtkToggleButton *toggle, AgsFMSynth *fm_synth);

void ags_fm_synth_lfo_freq_callback(GtkSpinButton *spin_button, AgsFMSynth *fm_synth);
void ags_fm_synth_lfo_phase_callback(GtkSpinButton *spin_button, AgsFMSynth *fm_synth);
void ags_fm_synth_lfo_depth_callback(GtkSpinButton *spin_button, AgsFMSynth *fm_synth);
void ags_fm_synth_lfo_tunint_callback(GtkSpinButton *spin_button, AgsFMSynth *fm_synth);

#endif /*__AGS_FM_SYNTH_CALLBACKS_H__ */
