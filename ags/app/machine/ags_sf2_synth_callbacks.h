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

#ifndef __AGS_SF2_SYNTH_CALLBACKS_H__ 
#define __AGS_SF2_SYNTH_CALLBACKS_H__ 

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/machine/ags_sf2_synth.h>

G_BEGIN_DECLS

void ags_sf2_synth_destroy_callback(GtkWidget *widget, AgsSF2Synth *sf2_synth);

void ags_sf2_synth_open_clicked_callback(GtkWidget *widget, AgsSF2Synth *sf2_synth);

void ags_sf2_synth_bank_tree_view_callback(GtkTreeView *tree_view,
					   GtkTreePath *path,
					   GtkTreeViewColumn *column,
					   AgsSF2Synth *sf2_synth);

void ags_sf2_synth_program_tree_view_callback(GtkTreeView *tree_view,
					      GtkTreePath *path,
					      GtkTreeViewColumn *column,
					      AgsSF2Synth *sf2_synth);

void ags_sf2_synth_synth_octave_callback(AgsDial *dial, AgsSF2Synth *sf2_synth);
void ags_sf2_synth_synth_key_callback(AgsDial *dial, AgsSF2Synth *sf2_synth);

void ags_sf2_synth_synth_volume_callback(AgsDial *dial, AgsSF2Synth *sf2_synth);

void ags_sf2_synth_chorus_enabled_callback(GtkButton *button, AgsSF2Synth *sf2_synth);

void ags_sf2_synth_chorus_input_volume_callback(AgsDial *dial, AgsSF2Synth *sf2_synth);
void ags_sf2_synth_chorus_output_volume_callback(AgsDial *dial, AgsSF2Synth *sf2_synth);

void ags_sf2_synth_chorus_lfo_oscillator_callback(GtkComboBox *combo_box, AgsSF2Synth *sf2_synth);
void ags_sf2_synth_chorus_lfo_frequency_callback(GtkSpinButton *spin_button, AgsSF2Synth *sf2_synth);

void ags_sf2_synth_chorus_depth_callback(AgsDial *dial, AgsSF2Synth *sf2_synth);
void ags_sf2_synth_chorus_mix_callback(AgsDial *dial, AgsSF2Synth *sf2_synth);
void ags_sf2_synth_chorus_delay_callback(AgsDial *dial, AgsSF2Synth *sf2_synth);

G_END_DECLS

#endif /*__AGS_SF2_SYNTH_CALLBACKS_H__ */
