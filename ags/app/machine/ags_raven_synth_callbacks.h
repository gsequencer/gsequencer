/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#ifndef __AGS_RAVEN_SYNTH_CALLBACKS_H__
#define __AGS_RAVEN_SYNTH_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/machine/ags_raven_synth.h>

G_BEGIN_DECLS

void ags_raven_synth_synth_0_oscillator_callback(GObject *gobject,
						 GParamSpec *pspec,
						 AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_0_octave_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_key_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_0_phase_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_volume_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_0_lfo_oscillator_callback(GObject *gobject,
						     GParamSpec *pspec,
						     AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_0_lfo_frequency_callback(GtkSpinButton *spin_button, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_lfo_depth_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_lfo_tuning_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_0_sync_enabled_callback(GtkButton *button, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_0_sync_relative_attack_factor_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_sync_attack_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_sync_phase_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_0_sync_relative_attack_factor_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_sync_attack_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_sync_phase_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_0_sync_relative_attack_factor_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_sync_attack_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_sync_phase_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_0_sync_relative_attack_factor_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_sync_attack_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_sync_phase_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_0_seq_tuning_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_4_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_5_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_6_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_7_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_8_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_9_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_10_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_11_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_12_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_13_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_14_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_15_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_0_seq_tuning_pingpong_callback(GtkCheckButton *button, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_tuning_lfo_frequency_callback(GtkSpinButton *spin_button, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_0_seq_volume_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_4_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_5_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_6_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_7_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_8_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_9_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_10_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_11_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_12_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_13_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_14_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_15_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_0_seq_volume_pingpong_callback(GtkCheckButton *button, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_0_seq_volume_lfo_frequency_callback(GtkSpinButton *spin_button, AgsRavenSynth *raven_synth);

void ags_raven_synth_low_pass_0_cut_off_frequency_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_low_pass_0_filter_gain_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_low_pass_0_no_clip_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_amplifier_0_amp_0_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth);
void ags_raven_synth_amplifier_0_amp_1_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth);
void ags_raven_synth_amplifier_0_amp_2_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth);
void ags_raven_synth_amplifier_0_amp_3_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth);

void ags_raven_synth_amplifier_0_filter_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_1_oscillator_callback(GObject *gobject,
						 GParamSpec *pspec,
						 AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_1_octave_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_key_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_1_phase_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_volume_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_1_lfo_oscillator_callback(GObject *gobject,
						     GParamSpec *pspec,
						     AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_1_lfo_frequency_callback(GtkSpinButton *spin_button, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_lfo_depth_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_lfo_tuning_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_1_sync_enabled_callback(GtkButton *button, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_1_sync_relative_attack_factor_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_sync_attack_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_sync_phase_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_1_sync_relative_attack_factor_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_sync_attack_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_sync_phase_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_1_sync_relative_attack_factor_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_sync_attack_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_sync_phase_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_1_sync_relative_attack_factor_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_sync_attack_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_sync_phase_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_1_seq_tuning_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_4_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_5_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_6_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_7_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_8_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_9_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_10_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_11_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_12_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_13_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_14_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_15_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_1_seq_tuning_pingpong_callback(GtkCheckButton *button, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_tuning_lfo_frequency_callback(GtkSpinButton *spin_button, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_1_seq_volume_0_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_1_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_2_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_3_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_4_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_5_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_6_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_7_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_8_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_9_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_10_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_11_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_12_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_13_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_14_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_15_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_synth_1_seq_volume_pingpong_callback(GtkCheckButton *button, AgsRavenSynth *raven_synth);
void ags_raven_synth_synth_1_seq_volume_lfo_frequency_callback(GtkSpinButton *spin_button, AgsRavenSynth *raven_synth);

void ags_raven_synth_low_pass_1_cut_off_frequency_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_low_pass_1_filter_gain_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_low_pass_1_no_clip_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_amplifier_1_amp_0_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth);
void ags_raven_synth_amplifier_1_amp_1_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth);
void ags_raven_synth_amplifier_1_amp_2_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth);
void ags_raven_synth_amplifier_1_amp_3_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth);

void ags_raven_synth_amplifier_1_filter_gain_callback(GtkRange *range, AgsRavenSynth *raven_synth);

void ags_raven_synth_pitch_type_callback(GObject *gobject,
					 GParamSpec *pspec,
					 AgsRavenSynth *raven_synth);
void ags_raven_synth_pitch_tuning_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_noise_gain_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_chorus_enabled_callback(GtkButton *button, AgsRavenSynth *raven_synth);

void ags_raven_synth_chorus_input_volume_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_chorus_output_volume_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_chorus_lfo_oscillator_callback(GObject *gobject,
						    GParamSpec *pspec,
						    AgsRavenSynth *raven_synth);
void ags_raven_synth_chorus_lfo_frequency_callback(GtkSpinButton *spin_button, AgsRavenSynth *raven_synth);

void ags_raven_synth_chorus_depth_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_chorus_mix_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_chorus_delay_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_volume_callback(GtkRange *range, AgsRavenSynth *raven_synth);

void ags_raven_synth_tremolo_enabled_callback(GtkCheckButton *button, AgsRavenSynth *raven_synth);

void ags_raven_synth_tremolo_gain_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_tremolo_lfo_depth_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_tremolo_lfo_freq_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_tremolo_tuning_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

void ags_raven_synth_vibrato_enabled_callback(GtkCheckButton *button, AgsRavenSynth *raven_synth);

void ags_raven_synth_vibrato_gain_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_vibrato_lfo_depth_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_vibrato_lfo_freq_callback(AgsDial *dial, AgsRavenSynth *raven_synth);
void ags_raven_synth_vibrato_tuning_callback(AgsDial *dial, AgsRavenSynth *raven_synth);

G_END_DECLS

#endif /*__AGS_RAVEN_SYNTH_CALLBACKS_H__*/
