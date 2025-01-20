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

#ifndef __AGS_QUANTUM_SYNTH_CALLBACKS_H__
#define __AGS_QUANTUM_SYNTH_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/machine/ags_quantum_synth.h>

G_BEGIN_DECLS

void ags_quantum_synth_synth_0_oscillator_callback(GObject *gobject,
						   GParamSpec *pspec,
						   AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_0_octave_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_key_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_0_phase_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_volume_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_0_seq_tuning_0_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_seq_tuning_1_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_seq_tuning_2_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_seq_tuning_3_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_seq_tuning_4_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_seq_tuning_5_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_seq_tuning_6_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_seq_tuning_7_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_0_seq_tuning_pingpong_callback(GtkCheckButton *button, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_0_seq_volume_0_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_seq_volume_1_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_seq_volume_2_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_seq_volume_3_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_seq_volume_4_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_seq_volume_5_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_seq_volume_6_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_seq_volume_7_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_0_seq_volume_pingpong_callback(GtkCheckButton *button, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_0_sync_enabled_callback(GtkButton *button, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_0_sync_relative_attack_factor_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_0_sync_attack_0_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_sync_phase_0_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_0_sync_attack_1_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_sync_phase_1_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_0_sync_attack_2_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_sync_phase_2_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_0_sync_attack_3_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_sync_phase_3_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_0_sync_lfo_oscillator_callback(GObject *gobject,
							    GParamSpec *pspec,
							    AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_sync_lfo_frequency_callback(GtkSpinButton *spin_button, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_0_lfo_oscillator_callback(GObject *gobject,
						       GParamSpec *pspec,
						       AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_0_lfo_frequency_callback(GtkSpinButton *spin_button, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_lfo_depth_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_0_lfo_tuning_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_oscillator_callback(GObject *gobject,
						   GParamSpec *pspec,
						   AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_octave_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_key_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_phase_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_volume_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_seq_tuning_0_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_seq_tuning_1_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_seq_tuning_2_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_seq_tuning_3_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_seq_tuning_4_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_seq_tuning_5_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_seq_tuning_6_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_seq_tuning_7_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_seq_tuning_pingpong_callback(GtkCheckButton *button, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_seq_volume_0_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_seq_volume_1_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_seq_volume_2_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_seq_volume_3_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_seq_volume_4_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_seq_volume_5_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_seq_volume_6_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_seq_volume_7_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_seq_volume_pingpong_callback(GtkCheckButton *button, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_sync_enabled_callback(GtkButton *button, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_sync_relative_attack_factor_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_sync_attack_0_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_sync_phase_0_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_sync_attack_1_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_sync_phase_1_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_sync_attack_2_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_sync_phase_2_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_sync_attack_3_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_sync_phase_3_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_sync_lfo_oscillator_callback(GObject *gobject,
							    GParamSpec *pspec,
							    AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_sync_lfo_frequency_callback(GtkSpinButton *spin_button, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_lfo_oscillator_callback(GObject *gobject,
						       GParamSpec *pspec,
						       AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_synth_1_lfo_frequency_callback(GtkSpinButton *spin_button, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_lfo_depth_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_synth_1_lfo_tuning_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_pitch_type_callback(GObject *gobject,
					   GParamSpec *pspec,
					   AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_pitch_tuning_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_noise_gain_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_chorus_enabled_callback(GtkButton *button, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_chorus_input_volume_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_chorus_output_volume_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_chorus_lfo_oscillator_callback(GObject *gobject,
						      GParamSpec *pspec,
						      AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_chorus_lfo_frequency_callback(GtkSpinButton *spin_button, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_chorus_depth_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_chorus_mix_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_chorus_delay_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_volume_callback(GtkRange *range, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_tremolo_enabled_callback(GtkCheckButton *button, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_tremolo_gain_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_tremolo_lfo_depth_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_tremolo_lfo_freq_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_tremolo_tuning_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_vibrato_enabled_callback(GtkCheckButton *button, AgsQuantumSynth *quantum_synth);

void ags_quantum_synth_vibrato_gain_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_vibrato_lfo_depth_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_vibrato_lfo_freq_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);
void ags_quantum_synth_vibrato_tuning_callback(AgsDial *dial, AgsQuantumSynth *quantum_synth);

G_END_DECLS

#endif /*__AGS_QUANTUM_SYNTH_CALLBACKS_H__*/
