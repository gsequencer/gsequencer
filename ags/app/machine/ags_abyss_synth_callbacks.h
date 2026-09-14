/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#ifndef __AGS_ABYSS_SYNTH_CALLBACKS_H__
#define __AGS_ABYSS_SYNTH_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/machine/ags_abyss_synth.h>

G_BEGIN_DECLS

void ags_abyss_synth_env_0_attack_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_0_decay_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_0_sustain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_0_release_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_env_0_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_0_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_env_1_attack_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_1_decay_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_1_sustain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_1_release_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_env_1_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_1_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_env_2_attack_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_2_decay_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_2_sustain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_2_release_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_env_2_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_2_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_env_3_attack_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_3_decay_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_3_sustain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_3_release_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_env_3_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_env_3_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_pink_noise_0_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_pink_noise_0_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_pink_noise_1_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_pink_noise_1_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_lfo_0_oscillator_callback(GObject *gobject,
					       GParamSpec *pspec,
					       AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_lfo_0_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_lfo_0_depth_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_lfo_0_tuning_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_lfo_1_oscillator_callback(GObject *gobject,
					       GParamSpec *pspec,
					       AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_lfo_1_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_lfo_1_depth_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_lfo_1_tuning_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_lfo_2_oscillator_callback(GObject *gobject,
					       GParamSpec *pspec,
					       AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_lfo_2_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_lfo_2_depth_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_lfo_2_tuning_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_lfo_3_oscillator_callback(GObject *gobject,
					       GParamSpec *pspec,
					       AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_lfo_3_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_lfo_3_depth_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_lfo_3_tuning_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_seq_0_modulation_0_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_modulation_1_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_modulation_2_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_modulation_3_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_modulation_4_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_modulation_5_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_modulation_6_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_modulation_7_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_modulation_8_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_modulation_9_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_modulation_10_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_modulation_11_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_modulation_12_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_modulation_13_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_modulation_14_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_modulation_15_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_seq_0_pingpong_callback(GtkCheckButton *button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_0_lfo_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_seq_1_modulation_0_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_modulation_1_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_modulation_2_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_modulation_3_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_modulation_4_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_modulation_5_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_modulation_6_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_modulation_7_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_modulation_8_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_modulation_9_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_modulation_10_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_modulation_11_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_modulation_12_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_modulation_13_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_modulation_14_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_modulation_15_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_seq_1_pingpong_callback(GtkCheckButton *button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_1_lfo_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_seq_2_modulation_0_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_modulation_1_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_modulation_2_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_modulation_3_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_modulation_4_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_modulation_5_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_modulation_6_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_modulation_7_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_modulation_8_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_modulation_9_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_modulation_10_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_modulation_11_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_modulation_12_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_modulation_13_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_modulation_14_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_modulation_15_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_seq_2_pingpong_callback(GtkCheckButton *button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_2_lfo_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_seq_3_modulation_0_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_modulation_1_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_modulation_2_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_modulation_3_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_modulation_4_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_modulation_5_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_modulation_6_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_modulation_7_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_modulation_8_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_modulation_9_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_modulation_10_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_modulation_11_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_modulation_12_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_modulation_13_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_modulation_14_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_modulation_15_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_seq_3_pingpong_callback(GtkCheckButton *button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_seq_3_lfo_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_0_oscillator_callback(GObject *gobject,
					       GParamSpec *pspec,
					       AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_0_octave_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_0_key_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_0_phase_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_0_volume_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_0_low_pass_0_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_0_low_pass_1_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_0_no_low_pass_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_1_oscillator_callback(GObject *gobject,
					       GParamSpec *pspec,
					       AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_1_octave_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_1_key_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_1_phase_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_1_volume_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_1_low_pass_0_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_1_low_pass_1_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_1_no_low_pass_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_2_oscillator_callback(GObject *gobject,
					       GParamSpec *pspec,
					       AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_2_octave_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_2_key_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_2_phase_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_2_volume_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_2_low_pass_0_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_2_low_pass_1_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_2_no_low_pass_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_3_oscillator_callback(GObject *gobject,
					       GParamSpec *pspec,
					       AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_3_octave_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_3_key_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_3_phase_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_3_volume_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_osc_3_low_pass_0_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_3_low_pass_1_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_osc_3_no_low_pass_callback(GtkCheckButton *check_button, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_ring_0_enabled_callback(GtkCheckButton *button, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_ring_0_tuning_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_ring_0_drive_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_ring_0_mix_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_ring_0_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_ring_1_enabled_callback(GtkCheckButton *button, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_ring_1_tuning_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_ring_1_drive_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_ring_1_mix_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_ring_1_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_pitch_tuning_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_volume_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_low_pass_0_cut_off_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_low_pass_0_filter_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_low_pass_0_no_clip_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_low_pass_1_cut_off_frequency_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_low_pass_1_filter_gain_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_low_pass_1_no_clip_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_amplifier_0_amp_0_gain_callback(GtkRange *range, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_amplifier_0_amp_1_gain_callback(GtkRange *range, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_amplifier_0_amp_2_gain_callback(GtkRange *range, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_amplifier_0_amp_3_gain_callback(GtkRange *range, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_amplifier_0_filter_gain_callback(GtkRange *range, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_chorus_enabled_callback(GtkButton *button, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_chorus_input_volume_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_chorus_output_volume_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_chorus_lfo_oscillator_callback(GObject *gobject,
						    GParamSpec *pspec,
						    AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_chorus_lfo_frequency_callback(GtkSpinButton *spin_button, AgsAbyssSynth *abyss_synth);

void ags_abyss_synth_chorus_depth_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_chorus_mix_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);
void ags_abyss_synth_chorus_delay_callback(AgsDial *dial, AgsAbyssSynth *abyss_synth);

G_END_DECLS

#endif /*__AGS_ABYSS_SYNTH_CALLBACKS_H__*/
