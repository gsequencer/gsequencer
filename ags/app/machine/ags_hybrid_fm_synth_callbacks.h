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

#ifndef __AGS_HYBRID_FM_SYNTH_CALLBACKS_H__ 
#define __AGS_HYBRID_FM_SYNTH_CALLBACKS_H__ 

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/machine/ags_hybrid_fm_synth.h>

G_BEGIN_DECLS

void ags_hybrid_fm_synth_synth_0_oscillator_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_synth_0_octave_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_synth_0_key_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_synth_0_phase_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_synth_0_volume_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_synth_0_lfo_oscillator_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_synth_0_lfo_frequency_callback(GtkSpinButton *spin_button, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_synth_0_lfo_depth_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_synth_0_lfo_tuning_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_synth_1_oscillator_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_synth_1_octave_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_synth_1_key_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_synth_1_phase_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_synth_1_volume_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_synth_1_lfo_oscillator_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_synth_1_lfo_frequency_callback(GtkSpinButton *spin_button, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_synth_1_lfo_depth_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_synth_1_lfo_tuning_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_synth_2_oscillator_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_synth_2_octave_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_synth_2_key_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_synth_2_phase_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_synth_2_volume_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_synth_2_lfo_oscillator_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_synth_2_lfo_frequency_callback(GtkSpinButton *spin_button, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_synth_2_lfo_depth_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_synth_2_lfo_tuning_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_sequencer_enabled_callback(GtkButton *button, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_sequencer_sign_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_pitch_tuning_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_noise_gain_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_low_pass_enabled_callback(GtkButton *button, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_low_pass_q_lin_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_low_pass_filter_gain_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_high_pass_enabled_callback(GtkButton *button, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_high_pass_q_lin_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_high_pass_filter_gain_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_chorus_enabled_callback(GtkButton *button, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_chorus_input_volume_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_chorus_output_volume_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_chorus_lfo_oscillator_callback(GtkComboBox *combo_box, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_chorus_lfo_frequency_callback(GtkSpinButton *spin_button, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_chorus_depth_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_chorus_mix_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);
void ags_hybrid_fm_synth_chorus_delay_callback(AgsDial *dial, AgsHybridFMSynth *hybrid_fm_synth);

void ags_hybrid_fm_synth_volume_callback(GtkRange *range, AgsHybridFMSynth *hybrid_fm_synth);

G_END_DECLS

#endif /*__AGS_HYBRID_FM_SYNTH_CALLBACKS_H__ */
