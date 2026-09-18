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

#include <ags/audio/fx/ags_fx_abyss_synth_audio.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_audio_signal.h>

#include <ags/i18n.h>

static void ags_fx_abyss_synth_audio_class_init(AgsFxAbyssSynthAudioClass *fx_abyss_synth_audio);
static void ags_fx_abyss_synth_audio_init(AgsFxAbyssSynthAudio *fx_abyss_synth_audio);
static void ags_fx_abyss_synth_audio_set_property(GObject *gobject,
						  guint prop_id,
						  const GValue *value,
						  GParamSpec *param_spec);
static void ags_fx_abyss_synth_audio_get_property(GObject *gobject,
						  guint prop_id,
						  GValue *value,
						  GParamSpec *param_spec);
static void ags_fx_abyss_synth_audio_dispose(GObject *gobject);
static void ags_fx_abyss_synth_audio_finalize(GObject *gobject);

static void ags_fx_abyss_synth_audio_notify_audio_callback(GObject *gobject,
							   GParamSpec *pspec,
							   gpointer user_data);
static void ags_fx_abyss_synth_audio_notify_buffer_size_callback(GObject *gobject,
								 GParamSpec *pspec,
								 gpointer user_data);
static void ags_fx_abyss_synth_audio_notify_format_callback(GObject *gobject,
							    GParamSpec *pspec,
							    gpointer user_data);
static void ags_fx_abyss_synth_audio_notify_samplerate_callback(GObject *gobject,
								GParamSpec *pspec,
								gpointer user_data);

static void ags_fx_abyss_synth_audio_set_audio_channels_callback(AgsAudio *audio,
								 guint audio_channels, guint audio_channels_old,
								 AgsFxAbyssSynthAudio *fx_abyss_synth_audio);

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_0_oscillator_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_0_octave_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_0_key_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_0_phase_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_0_volume_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_0_low_pass_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_1_oscillator_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_1_octave_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_1_key_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_1_phase_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_1_volume_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_1_low_pass_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_2_oscillator_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_2_octave_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_2_key_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_2_phase_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_2_volume_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_2_low_pass_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_3_oscillator_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_3_octave_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_3_key_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_3_phase_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_3_volume_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_osc_3_low_pass_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_ring_0_enabled_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_ring_0_tuning_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_ring_0_drive_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_ring_0_mix_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_ring_0_gain_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_ring_1_enabled_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_ring_1_tuning_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_ring_1_drive_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_ring_1_mix_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_ring_1_gain_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_pitch_tuning_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_volume_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_0_attack_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_0_decay_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_0_sustain_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_0_release_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_0_gain_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_0_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_0_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_1_attack_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_1_decay_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_1_sustain_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_1_release_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_1_gain_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_1_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_1_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_2_attack_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_2_decay_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_2_sustain_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_2_release_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_2_gain_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_2_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_2_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_3_attack_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_3_decay_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_3_sustain_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_3_release_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_3_gain_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_3_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_env_3_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_0_oscillator_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_0_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_0_depth_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_0_tuning_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_0_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_1_oscillator_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_1_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_1_depth_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_1_tuning_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_1_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_2_oscillator_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_2_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_2_depth_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_2_tuning_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_2_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_3_oscillator_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_3_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_3_depth_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_3_tuning_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_lfo_3_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_0_pingpong_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_0_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_0_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_1_pingpong_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_1_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_1_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_2_pingpong_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_2_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_2_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_3_pingpong_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_3_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_seq_3_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_pink_noise_0_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_pink_noise_0_gain_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_pink_noise_0_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_pink_noise_1_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_pink_noise_1_gain_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_synth_0_pink_noise_1_sends_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_low_pass_0_cut_off_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_low_pass_0_filter_gain_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_low_pass_0_no_clip_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_low_pass_1_cut_off_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_low_pass_1_filter_gain_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_low_pass_1_no_clip_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_amplifier_amp_gain_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_amplifier_filter_gain_plugin_port();

static AgsPluginPort* ags_fx_abyss_synth_audio_get_chorus_enabled_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_chorus_pitch_type_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_chorus_input_volume_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_chorus_output_volume_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_chorus_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_chorus_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_chorus_depth_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_chorus_mix_plugin_port();
static AgsPluginPort* ags_fx_abyss_synth_audio_get_chorus_delay_plugin_port();

/**
 * SECTION:ags_fx_abyss_synth_audio
 * @short_description: fx abyss synth audio
 * @title: AgsFxAbyssSynthAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_abyss_synth_audio.h
 *
 * The #AgsFxAbyssSynthAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_abyss_synth_audio_parent_class = NULL;

static const gchar *ags_fx_abyss_synth_audio_plugin_name = "ags-fx-abyss-synth";

static const gchar* ags_fx_abyss_synth_audio_specifier[] = {
  "./synth-0-osc-0-oscillator[0]",
  "./synth-0-osc-0-octave[0]",
  "./synth-0-osc-0-key[0]",
  "./synth-0-osc-0-phase[0]",
  "./synth-0-osc-0-volume[0]",
  "./synth-0-osc-0-low-pass-sends[0]",
  "./synth-0-osc-1-oscillator[0]",
  "./synth-0-osc-1-octave[0]",
  "./synth-0-osc-1-key[0]",
  "./synth-0-osc-1-phase[0]",
  "./synth-0-osc-1-volume[0]",
  "./synth-0-osc-1-low-pass-sends[0]",
  "./synth-0-osc-2-oscillator[0]",
  "./synth-0-osc-2-octave[0]",
  "./synth-0-osc-2-key[0]",
  "./synth-0-osc-2-phase[0]",
  "./synth-0-osc-2-volume[0]",
  "./synth-0-osc-2-low-pass-sends[0]",
  "./synth-0-osc-3-oscillator[0]",
  "./synth-0-osc-3-octave[0]",
  "./synth-0-osc-3-key[0]",
  "./synth-0-osc-3-phase[0]",
  "./synth-0-osc-3-volume[0]",
  "./synth-0-osc-3-low-pass-sends[0]",
  "./synth-0-ring-0-enabled[0]",
  "./synth-0-ring-0-tuning[0]",
  "./synth-0-ring-0-drive[0]",
  "./synth-0-ring-0-mix[0]",
  "./synth-0-ring-0-gain[0]",
  "./synth-0-ring-1-enabled[0]",
  "./synth-0-ring-1-tuning[0]",
  "./synth-0-ring-1-drive[0]",
  "./synth-0-ring-1-mix[0]",
  "./synth-0-ring-1-gain[0]",
  "./synth-0-pitch-tuning[0]",
  "./synth-0-volume[0]",
  "./synth-0-env-0-attack[0]",
  "./synth-0-env-0-decay[0]",
  "./synth-0-env-0-sustain[0]",
  "./synth-0-env-0-release[0]",
  "./synth-0-env-0-gain[0]",
  "./synth-0-env-0-frequency[0]",
  "./synth-0-env-0-sends[0]",
  "./synth-0-env-1-attack[0]",
  "./synth-0-env-1-decay[0]",
  "./synth-0-env-1-sustain[0]",
  "./synth-0-env-1-release[0]",
  "./synth-0-env-1-gain[0]",
  "./synth-0-env-1-frequency[0]",
  "./synth-0-env-1-sends[0]",
  "./synth-0-env-2-attack[0]",
  "./synth-0-env-2-decay[0]",
  "./synth-0-env-2-sustain[0]",
  "./synth-0-env-2-release[0]",
  "./synth-0-env-2-gain[0]",
  "./synth-0-env-2-frequency[0]",
  "./synth-0-env-2-sends[0]",
  "./synth-0-env-3-attack[0]",
  "./synth-0-env-3-decay[0]",
  "./synth-0-env-3-sustain[0]",
  "./synth-0-env-3-release[0]",
  "./synth-0-env-3-gain[0]",
  "./synth-0-env-3-frequency[0]",
  "./synth-0-env-3-sends[0]",
  "./synth-0-lfo-0-oscillator[0]",
  "./synth-0-lfo-0-frequency[0]",
  "./synth-0-lfo-0-depth[0]",
  "./synth-0-lfo-0-tuning[0]",
  "./synth-0-lfo-0-sends[0]",
  "./synth-0-lfo-1-oscillator[0]",
  "./synth-0-lfo-1-frequency[0]",
  "./synth-0-lfo-1-depth[0]",
  "./synth-0-lfo-1-tuning[0]",
  "./synth-0-lfo-1-sends[0]",
  "./synth-0-lfo-2-oscillator[0]",
  "./synth-0-lfo-2-frequency[0]",
  "./synth-0-lfo-2-depth[0]",
  "./synth-0-lfo-2-tuning[0]",
  "./synth-0-lfo-2-sends[0]",
  "./synth-0-lfo-3-oscillator[0]",
  "./synth-0-lfo-3-frequency[0]",
  "./synth-0-lfo-3-depth[0]",
  "./synth-0-lfo-3-tuning[0]",
  "./synth-0-lfo-3-sends[0]",
  "./synth-0-seq-0-modulation-0[0]",
  "./synth-0-seq-0-modulation-1[0]",
  "./synth-0-seq-0-modulation-2[0]",
  "./synth-0-seq-0-modulation-3[0]",
  "./synth-0-seq-0-modulation-4[0]",
  "./synth-0-seq-0-modulation-5[0]",
  "./synth-0-seq-0-modulation-6[0]",
  "./synth-0-seq-0-modulation-7[0]",
  "./synth-0-seq-0-modulation-8[0]",
  "./synth-0-seq-0-modulation-9[0]",
  "./synth-0-seq-0-modulation-10[0]",
  "./synth-0-seq-0-modulation-11[0]",
  "./synth-0-seq-0-modulation-12[0]",
  "./synth-0-seq-0-modulation-13[0]",
  "./synth-0-seq-0-modulation-14[0]",
  "./synth-0-seq-0-modulation-15[0]",
  "./synth-0-seq-0-pingpong[0]",
  "./synth-0-seq-0-lfo-frequency[0]",
  "./synth-0-seq-0-sends[0]",
  "./synth-0-seq-1-modulation-0[0]",
  "./synth-0-seq-1-modulation-1[0]",
  "./synth-0-seq-1-modulation-2[0]",
  "./synth-0-seq-1-modulation-3[0]",
  "./synth-0-seq-1-modulation-4[0]",
  "./synth-0-seq-1-modulation-5[0]",
  "./synth-0-seq-1-modulation-6[0]",
  "./synth-0-seq-1-modulation-7[0]",
  "./synth-0-seq-1-modulation-8[0]",
  "./synth-0-seq-1-modulation-9[0]",
  "./synth-0-seq-1-modulation-10[0]",
  "./synth-0-seq-1-modulation-11[0]",
  "./synth-0-seq-1-modulation-12[0]",
  "./synth-0-seq-1-modulation-13[0]",
  "./synth-0-seq-1-modulation-14[0]",
  "./synth-0-seq-1-modulation-15[0]",
  "./synth-0-seq-1-pingpong[0]",
  "./synth-0-seq-1-sends[0]",
  "./synth-0-seq-1-lfo-frequency[0]",
  "./synth-0-seq-2-modulation-0[0]",
  "./synth-0-seq-2-modulation-1[0]",
  "./synth-0-seq-2-modulation-2[0]",
  "./synth-0-seq-2-modulation-3[0]",
  "./synth-0-seq-2-modulation-4[0]",
  "./synth-0-seq-2-modulation-5[0]",
  "./synth-0-seq-2-modulation-6[0]",
  "./synth-0-seq-2-modulation-7[0]",
  "./synth-0-seq-2-modulation-8[0]",
  "./synth-0-seq-2-modulation-9[0]",
  "./synth-0-seq-2-modulation-10[0]",
  "./synth-0-seq-2-modulation-11[0]",
  "./synth-0-seq-2-modulation-12[0]",
  "./synth-0-seq-2-modulation-13[0]",
  "./synth-0-seq-2-modulation-14[0]",
  "./synth-0-seq-2-modulation-15[0]",
  "./synth-0-seq-2-pingpong[0]",
  "./synth-0-seq-2-lfo-frequency[0]",
  "./synth-0-seq-2-sends[0]",
  "./synth-0-seq-3-modulation-0[0]",
  "./synth-0-seq-3-modulation-1[0]",
  "./synth-0-seq-3-modulation-2[0]",
  "./synth-0-seq-3-modulation-3[0]",
  "./synth-0-seq-3-modulation-4[0]",
  "./synth-0-seq-3-modulation-5[0]",
  "./synth-0-seq-3-modulation-6[0]",
  "./synth-0-seq-3-modulation-7[0]",
  "./synth-0-seq-3-modulation-8[0]",
  "./synth-0-seq-3-modulation-9[0]",
  "./synth-0-seq-3-modulation-10[0]",
  "./synth-0-seq-3-modulation-11[0]",
  "./synth-0-seq-3-modulation-12[0]",
  "./synth-0-seq-3-modulation-13[0]",
  "./synth-0-seq-3-modulation-14[0]",
  "./synth-0-seq-3-modulation-15[0]",
  "./synth-0-seq-3-pingpong[0]",
  "./synth-0-seq-3-lfo-frequency[0]",
  "./synth-0-seq-3-sends[0]",
  "./synth-0-pink-noise-0-frequency[0]",
  "./synth-0-pink-noise-0-gain[0]",
  "./synth-0-pink-noise-0-sends[0]",
  "./synth-0-pink-noise-1-frequency[0]",
  "./synth-0-pink-noise-1-gain[0]",
  "./synth-0-pink-noise-1-sends[0]",
  "./low-pass-0-cut-off-frequency",
  "./low-pass-0-filter-gain",
  "./low-pass-0-no-clip",
  "./low-pass-1-cut-off-frequency",
  "./low-pass-1-filter-gain",
  "./low-pass-1-no-clip",
  "./amplifier-0-amp-0-gain",
  "./amplifier-0-amp-1-gain",
  "./amplifier-0-amp-2-gain",
  "./amplifier-0-amp-3-gain",
  "./amplifier-0-filter-gain",
  "./chorus-enabled[0]",
  "./chorus-pitch-type[0]",
  "./chorus-input-volume[0]",
  "./chorus-output-volume[0]",
  "./chorus-lfo-oscillator[0]",
  "./chorus-lfo-frequency[0]",
  "./chorus-depth[0]",
  "./chorus-mix[0]",
  "./chorus-delay[0]",
  NULL,
};

enum{
  PROP_0,
  PROP_SYNTH_0_OSC_0_OSCILLATOR,
  PROP_SYNTH_0_OSC_0_OCTAVE,
  PROP_SYNTH_0_OSC_0_KEY,
  PROP_SYNTH_0_OSC_0_PHASE,
  PROP_SYNTH_0_OSC_0_VOLUME,
  PROP_SYNTH_0_OSC_0_LOW_PASS_SENDS,
  PROP_SYNTH_0_OSC_1_OSCILLATOR,
  PROP_SYNTH_0_OSC_1_OCTAVE,
  PROP_SYNTH_0_OSC_1_KEY,
  PROP_SYNTH_0_OSC_1_PHASE,
  PROP_SYNTH_0_OSC_1_VOLUME,
  PROP_SYNTH_0_OSC_1_LOW_PASS_SENDS,
  PROP_SYNTH_0_OSC_2_OSCILLATOR,
  PROP_SYNTH_0_OSC_2_OCTAVE,
  PROP_SYNTH_0_OSC_2_KEY,
  PROP_SYNTH_0_OSC_2_PHASE,
  PROP_SYNTH_0_OSC_2_VOLUME,
  PROP_SYNTH_0_OSC_2_LOW_PASS_SENDS,
  PROP_SYNTH_0_OSC_3_OSCILLATOR,
  PROP_SYNTH_0_OSC_3_OCTAVE,
  PROP_SYNTH_0_OSC_3_KEY,
  PROP_SYNTH_0_OSC_3_PHASE,
  PROP_SYNTH_0_OSC_3_VOLUME,
  PROP_SYNTH_0_OSC_3_LOW_PASS_SENDS,
  PROP_SYNTH_0_RING_0_ENABLED,
  PROP_SYNTH_0_RING_0_TUNING,
  PROP_SYNTH_0_RING_0_DRIVE,
  PROP_SYNTH_0_RING_0_MIX,
  PROP_SYNTH_0_RING_0_GAIN,
  PROP_SYNTH_0_RING_1_ENABLED,
  PROP_SYNTH_0_RING_1_TUNING,
  PROP_SYNTH_0_RING_1_DRIVE,
  PROP_SYNTH_0_RING_1_MIX,
  PROP_SYNTH_0_RING_1_GAIN,
  PROP_SYNTH_0_PITCH_TUNING,
  PROP_SYNTH_0_VOLUME,
  PROP_SYNTH_0_ENV_0_ATTACK,
  PROP_SYNTH_0_ENV_0_DECAY,
  PROP_SYNTH_0_ENV_0_SUSTAIN,
  PROP_SYNTH_0_ENV_0_RELEASE,
  PROP_SYNTH_0_ENV_0_GAIN,
  PROP_SYNTH_0_ENV_0_FREQUENCY,
  PROP_SYNTH_0_ENV_0_SENDS,
  PROP_SYNTH_0_ENV_1_ATTACK,
  PROP_SYNTH_0_ENV_1_DECAY,
  PROP_SYNTH_0_ENV_1_SUSTAIN,
  PROP_SYNTH_0_ENV_1_RELEASE,
  PROP_SYNTH_0_ENV_1_GAIN,
  PROP_SYNTH_0_ENV_1_FREQUENCY,
  PROP_SYNTH_0_ENV_1_SENDS,
  PROP_SYNTH_0_ENV_2_ATTACK,
  PROP_SYNTH_0_ENV_2_DECAY,
  PROP_SYNTH_0_ENV_2_SUSTAIN,
  PROP_SYNTH_0_ENV_2_RELEASE,
  PROP_SYNTH_0_ENV_2_GAIN,
  PROP_SYNTH_0_ENV_2_FREQUENCY,
  PROP_SYNTH_0_ENV_2_SENDS,
  PROP_SYNTH_0_ENV_3_ATTACK,
  PROP_SYNTH_0_ENV_3_DECAY,
  PROP_SYNTH_0_ENV_3_SUSTAIN,
  PROP_SYNTH_0_ENV_3_RELEASE,
  PROP_SYNTH_0_ENV_3_GAIN,
  PROP_SYNTH_0_ENV_3_FREQUENCY,
  PROP_SYNTH_0_ENV_3_SENDS,
  PROP_SYNTH_0_LFO_0_OSCILLATOR,
  PROP_SYNTH_0_LFO_0_FREQUENCY,
  PROP_SYNTH_0_LFO_0_DEPTH,
  PROP_SYNTH_0_LFO_0_TUNING,
  PROP_SYNTH_0_LFO_0_SENDS,
  PROP_SYNTH_0_LFO_1_OSCILLATOR,
  PROP_SYNTH_0_LFO_1_FREQUENCY,
  PROP_SYNTH_0_LFO_1_DEPTH,
  PROP_SYNTH_0_LFO_1_TUNING,
  PROP_SYNTH_0_LFO_1_SENDS,
  PROP_SYNTH_0_LFO_2_OSCILLATOR,
  PROP_SYNTH_0_LFO_2_FREQUENCY,
  PROP_SYNTH_0_LFO_2_DEPTH,
  PROP_SYNTH_0_LFO_2_TUNING,
  PROP_SYNTH_0_LFO_2_SENDS,
  PROP_SYNTH_0_LFO_3_OSCILLATOR,
  PROP_SYNTH_0_LFO_3_FREQUENCY,
  PROP_SYNTH_0_LFO_3_DEPTH,
  PROP_SYNTH_0_LFO_3_TUNING,
  PROP_SYNTH_0_LFO_3_SENDS,
  PROP_SYNTH_0_SEQ_0_MODULATION_0,
  PROP_SYNTH_0_SEQ_0_MODULATION_1,
  PROP_SYNTH_0_SEQ_0_MODULATION_2,
  PROP_SYNTH_0_SEQ_0_MODULATION_3,
  PROP_SYNTH_0_SEQ_0_MODULATION_4,
  PROP_SYNTH_0_SEQ_0_MODULATION_5,
  PROP_SYNTH_0_SEQ_0_MODULATION_6,
  PROP_SYNTH_0_SEQ_0_MODULATION_7,
  PROP_SYNTH_0_SEQ_0_MODULATION_8,
  PROP_SYNTH_0_SEQ_0_MODULATION_9,
  PROP_SYNTH_0_SEQ_0_MODULATION_10,
  PROP_SYNTH_0_SEQ_0_MODULATION_11,
  PROP_SYNTH_0_SEQ_0_MODULATION_12,
  PROP_SYNTH_0_SEQ_0_MODULATION_13,
  PROP_SYNTH_0_SEQ_0_MODULATION_14,
  PROP_SYNTH_0_SEQ_0_MODULATION_15,
  PROP_SYNTH_0_SEQ_0_PINGPONG,
  PROP_SYNTH_0_SEQ_0_LFO_FREQUENCY,
  PROP_SYNTH_0_SEQ_0_SENDS,
  PROP_SYNTH_0_SEQ_1_MODULATION_0,
  PROP_SYNTH_0_SEQ_1_MODULATION_1,
  PROP_SYNTH_0_SEQ_1_MODULATION_2,
  PROP_SYNTH_0_SEQ_1_MODULATION_3,
  PROP_SYNTH_0_SEQ_1_MODULATION_4,
  PROP_SYNTH_0_SEQ_1_MODULATION_5,
  PROP_SYNTH_0_SEQ_1_MODULATION_6,
  PROP_SYNTH_0_SEQ_1_MODULATION_7,
  PROP_SYNTH_0_SEQ_1_MODULATION_8,
  PROP_SYNTH_0_SEQ_1_MODULATION_9,
  PROP_SYNTH_0_SEQ_1_MODULATION_10,
  PROP_SYNTH_0_SEQ_1_MODULATION_11,
  PROP_SYNTH_0_SEQ_1_MODULATION_12,
  PROP_SYNTH_0_SEQ_1_MODULATION_13,
  PROP_SYNTH_0_SEQ_1_MODULATION_14,
  PROP_SYNTH_0_SEQ_1_MODULATION_15,
  PROP_SYNTH_0_SEQ_1_PINGPONG,
  PROP_SYNTH_0_SEQ_1_LFO_FREQUENCY,
  PROP_SYNTH_0_SEQ_1_SENDS,
  PROP_SYNTH_0_SEQ_2_MODULATION_0,
  PROP_SYNTH_0_SEQ_2_MODULATION_1,
  PROP_SYNTH_0_SEQ_2_MODULATION_2,
  PROP_SYNTH_0_SEQ_2_MODULATION_3,
  PROP_SYNTH_0_SEQ_2_MODULATION_4,
  PROP_SYNTH_0_SEQ_2_MODULATION_5,
  PROP_SYNTH_0_SEQ_2_MODULATION_6,
  PROP_SYNTH_0_SEQ_2_MODULATION_7,
  PROP_SYNTH_0_SEQ_2_MODULATION_8,
  PROP_SYNTH_0_SEQ_2_MODULATION_9,
  PROP_SYNTH_0_SEQ_2_MODULATION_10,
  PROP_SYNTH_0_SEQ_2_MODULATION_11,
  PROP_SYNTH_0_SEQ_2_MODULATION_12,
  PROP_SYNTH_0_SEQ_2_MODULATION_13,
  PROP_SYNTH_0_SEQ_2_MODULATION_14,
  PROP_SYNTH_0_SEQ_2_MODULATION_15,
  PROP_SYNTH_0_SEQ_2_PINGPONG,
  PROP_SYNTH_0_SEQ_2_LFO_FREQUENCY,
  PROP_SYNTH_0_SEQ_2_SENDS,
  PROP_SYNTH_0_SEQ_3_MODULATION_0,
  PROP_SYNTH_0_SEQ_3_MODULATION_1,
  PROP_SYNTH_0_SEQ_3_MODULATION_2,
  PROP_SYNTH_0_SEQ_3_MODULATION_3,
  PROP_SYNTH_0_SEQ_3_MODULATION_4,
  PROP_SYNTH_0_SEQ_3_MODULATION_5,
  PROP_SYNTH_0_SEQ_3_MODULATION_6,
  PROP_SYNTH_0_SEQ_3_MODULATION_7,
  PROP_SYNTH_0_SEQ_3_MODULATION_8,
  PROP_SYNTH_0_SEQ_3_MODULATION_9,
  PROP_SYNTH_0_SEQ_3_MODULATION_10,
  PROP_SYNTH_0_SEQ_3_MODULATION_11,
  PROP_SYNTH_0_SEQ_3_MODULATION_12,
  PROP_SYNTH_0_SEQ_3_MODULATION_13,
  PROP_SYNTH_0_SEQ_3_MODULATION_14,
  PROP_SYNTH_0_SEQ_3_MODULATION_15,
  PROP_SYNTH_0_SEQ_3_PINGPONG,
  PROP_SYNTH_0_SEQ_3_LFO_FREQUENCY,
  PROP_SYNTH_0_SEQ_3_SENDS,
  PROP_SYNTH_0_PINK_NOISE_0_FREQUENCY,
  PROP_SYNTH_0_PINK_NOISE_0_GAIN,
  PROP_SYNTH_0_PINK_NOISE_0_SENDS,
  PROP_SYNTH_0_PINK_NOISE_1_FREQUENCY,
  PROP_SYNTH_0_PINK_NOISE_1_GAIN,
  PROP_SYNTH_0_PINK_NOISE_1_SENDS,
  PROP_LOW_PASS_0_CUT_OFF_FREQUENCY,
  PROP_LOW_PASS_0_FILTER_GAIN,
  PROP_LOW_PASS_0_NO_CLIP,
  PROP_LOW_PASS_1_CUT_OFF_FREQUENCY,
  PROP_LOW_PASS_1_FILTER_GAIN,
  PROP_LOW_PASS_1_NO_CLIP,
  PROP_AMPLIFIER_0_AMP_0_GAIN,
  PROP_AMPLIFIER_0_AMP_1_GAIN,
  PROP_AMPLIFIER_0_AMP_2_GAIN,
  PROP_AMPLIFIER_0_AMP_3_GAIN,
  PROP_AMPLIFIER_0_FILTER_GAIN,
  PROP_CHORUS_ENABLED,
  PROP_CHORUS_PITCH_TYPE,
  PROP_CHORUS_INPUT_VOLUME,
  PROP_CHORUS_OUTPUT_VOLUME,
  PROP_CHORUS_LFO_OSCILLATOR,
  PROP_CHORUS_LFO_FREQUENCY,
  PROP_CHORUS_DEPTH,
  PROP_CHORUS_MIX,
  PROP_CHORUS_DELAY,
};

GType
ags_fx_abyss_synth_audio_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_abyss_synth_audio = 0;

    static const GTypeInfo ags_fx_abyss_synth_audio_info = {
      sizeof (AgsFxAbyssSynthAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_abyss_synth_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxAbyssSynthAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_abyss_synth_audio_init,
    };

    ags_type_fx_abyss_synth_audio = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO,
							   "AgsFxAbyssSynthAudio",
							   &ags_fx_abyss_synth_audio_info,
							   0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_abyss_synth_audio);
  }

  return(g_define_type_id__static);
}

void
ags_fx_abyss_synth_audio_class_init(AgsFxAbyssSynthAudioClass *fx_abyss_synth_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_abyss_synth_audio_parent_class = g_type_class_peek_parent(fx_abyss_synth_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_abyss_synth_audio;

  gobject->set_property = ags_fx_abyss_synth_audio_set_property;
  gobject->get_property = ags_fx_abyss_synth_audio_get_property;

  gobject->dispose = ags_fx_abyss_synth_audio_dispose;
  gobject->finalize = ags_fx_abyss_synth_audio_finalize;

  /* properties */
  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-0-oscillator:
   *
   * The synth-0 osc-0 oscillator.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-0-oscillator",
				   i18n_pspec("synth-0 osc-0 oscillator of recall"),
				   i18n_pspec("The synth-0 osc-0's oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_0_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-0-octave:
   *
   * The synth-0 osc-0 octave.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-0-octave",
				   i18n_pspec("synth-0 osc-0 octave of recall"),
				   i18n_pspec("The synth-0 osc-0's octave"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_0_OCTAVE,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-0-key:
   *
   * The synth-0 osc-0 key.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-0-key",
				   i18n_pspec("synth-0 osc-0 key of recall"),
				   i18n_pspec("The synth-0 osc-0's key"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_0_KEY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-0-phase:
   *
   * The synth-0 osc-0 phase.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-0-phase",
				   i18n_pspec("synth-0 osc-0 phase of recall"),
				   i18n_pspec("The synth-0 osc-0's phase"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_0_PHASE,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-0-volume:
   *
   * The synth-0 osc-0 volume.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-0-volume",
				   i18n_pspec("synth-0 osc-0 volume of recall"),
				   i18n_pspec("The synth-0 osc-0's volume"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_0_VOLUME,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-0-low-pass-sends:
   *
   * The synth-0 osc-0 low-pass sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-0-low-pass-sends",
				   i18n_pspec("synth-0 osc-0 low-pass sends of recall"),
				   i18n_pspec("The synth-0 osc-0's low-pass sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_0_LOW_PASS_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-1-oscillator:
   *
   * The synth-0 osc-1 oscillator.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-1-oscillator",
				   i18n_pspec("synth-0 osc-1 oscillator of recall"),
				   i18n_pspec("The synth-0 osc-1's oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_1_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-1-octave:
   *
   * The synth-0 osc-1 octave.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-1-octave",
				   i18n_pspec("synth-0 osc-1 octave of recall"),
				   i18n_pspec("The synth-0 osc-1's octave"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_1_OCTAVE,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-1-key:
   *
   * The synth-0 osc-1 key.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-1-key",
				   i18n_pspec("synth-0 osc-1 key of recall"),
				   i18n_pspec("The synth-0 osc-1's key"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_1_KEY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-1-phase:
   *
   * The synth-0 osc-1 phase.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-1-phase",
				   i18n_pspec("synth-0 osc-1 phase of recall"),
				   i18n_pspec("The synth-0 osc-1's phase"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_1_PHASE,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-1-volume:
   *
   * The synth-0 osc-1 volume.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-1-volume",
				   i18n_pspec("synth-0 osc-1 volume of recall"),
				   i18n_pspec("The synth-0 osc-1's volume"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_1_VOLUME,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-1-low-pass-sends:
   *
   * The synth-0 osc-1 low-pass sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-1-low-pass-sends",
				   i18n_pspec("synth-0 osc-1 low-pass sends of recall"),
				   i18n_pspec("The synth-0 osc-1's low-pass sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_1_LOW_PASS_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-2-oscillator:
   *
   * The synth-0 osc-2 oscillator.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-2-oscillator",
				   i18n_pspec("synth-0 osc-2 oscillator of recall"),
				   i18n_pspec("The synth-0 osc-2's oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_2_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-2-octave:
   *
   * The synth-0 osc-2 octave.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-2-octave",
				   i18n_pspec("synth-0 osc-2 octave of recall"),
				   i18n_pspec("The synth-0 osc-2's octave"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_2_OCTAVE,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-2-key:
   *
   * The synth-0 osc-2 key.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-2-key",
				   i18n_pspec("synth-0 osc-2 key of recall"),
				   i18n_pspec("The synth-0 osc-2's key"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_2_KEY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-2-phase:
   *
   * The synth-0 osc-2 phase.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-2-phase",
				   i18n_pspec("synth-0 osc-2 phase of recall"),
				   i18n_pspec("The synth-0 osc-2's phase"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_2_PHASE,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-2-volume:
   *
   * The synth-0 osc-2 volume.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-2-volume",
				   i18n_pspec("synth-0 osc-2 volume of recall"),
				   i18n_pspec("The synth-0 osc-2's volume"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_2_VOLUME,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-2-low-pass-sends:
   *
   * The synth-0 osc-2 low-pass sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-2-low-pass-sends",
				   i18n_pspec("synth-0 osc-2 low-pass sends of recall"),
				   i18n_pspec("The synth-0 osc-2's low-pass sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_2_LOW_PASS_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-3-oscillator:
   *
   * The synth-0 osc-3 oscillator.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-3-oscillator",
				   i18n_pspec("synth-0 osc-3 oscillator of recall"),
				   i18n_pspec("The synth-0 osc-3's oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_3_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-3-octave:
   *
   * The synth-0 osc-3 octave.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-3-octave",
				   i18n_pspec("synth-0 osc-3 octave of recall"),
				   i18n_pspec("The synth-0 osc-3's octave"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_3_OCTAVE,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-3-key:
   *
   * The synth-0 osc-3 key.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-3-key",
				   i18n_pspec("synth-0 osc-3 key of recall"),
				   i18n_pspec("The synth-0 osc-3's key"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_3_KEY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-3-phase:
   *
   * The synth-0 osc-3 phase.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-3-phase",
				   i18n_pspec("synth-0 osc-3 phase of recall"),
				   i18n_pspec("The synth-0 osc-3's phase"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_3_PHASE,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-3-volume:
   *
   * The synth-0 osc-3 volume.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-3-volume",
				   i18n_pspec("synth-0 osc-3 volume of recall"),
				   i18n_pspec("The synth-0 osc-3's volume"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_3_VOLUME,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-osc-3-low-pass-sends:
   *
   * The synth-0 osc-3 low-pass sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-osc-3-low-pass-sends",
				   i18n_pspec("synth-0 osc-3 low-pass sends of recall"),
				   i18n_pspec("The synth-0 osc-3's low-pass sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSC_3_LOW_PASS_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-ring-0-enabled:
   *
   * The synth-0 ring-0 enabled.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-ring-0-enabled",
				   i18n_pspec("synth-0 ring-0 enabled of recall"),
				   i18n_pspec("The synth-0 ring-0's enabled"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_RING_0_ENABLED,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-ring-0-tuning:
   *
   * The synth-0 ring-0 tuning.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-ring-0-tuning",
				   i18n_pspec("synth-0 ring-0 tuning of recall"),
				   i18n_pspec("The synth-0 ring-0's tuning"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_RING_0_TUNING,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-ring-0-drive:
   *
   * The synth-0 ring-0 drive.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-ring-0-drive",
				   i18n_pspec("synth-0 ring-0 drive of recall"),
				   i18n_pspec("The synth-0 ring-0's drive"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_RING_0_DRIVE,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-ring-0-mix:
   *
   * The synth-0 ring-0 mix.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-ring-0-mix",
				   i18n_pspec("synth-0 ring-0 mix of recall"),
				   i18n_pspec("The synth-0 ring-0's mix"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_RING_0_MIX,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-ring-0-gain:
   *
   * The synth-0 ring-0 gain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-ring-0-gain",
				   i18n_pspec("synth-0 ring-0 gain of recall"),
				   i18n_pspec("The synth-0 ring-0's gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_RING_0_GAIN,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-ring-1-enabled:
   *
   * The synth-0 ring-1 enabled.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-ring-1-enabled",
				   i18n_pspec("synth-0 ring-1 enabled of recall"),
				   i18n_pspec("The synth-0 ring-1's enabled"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_RING_1_ENABLED,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-ring-1-tuning:
   *
   * The synth-0 ring-1 tuning.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-ring-1-tuning",
				   i18n_pspec("synth-0 ring-1 tuning of recall"),
				   i18n_pspec("The synth-0 ring-1's tuning"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_RING_1_TUNING,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-ring-1-drive:
   *
   * The synth-0 ring-1 drive.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-ring-1-drive",
				   i18n_pspec("synth-0 ring-1 drive of recall"),
				   i18n_pspec("The synth-0 ring-1's drive"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_RING_1_DRIVE,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-ring-1-mix:
   *
   * The synth-0 ring-1 mix.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-ring-1-mix",
				   i18n_pspec("synth-0 ring-1 mix of recall"),
				   i18n_pspec("The synth-0 ring-1's mix"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_RING_1_MIX,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-ring-1-gain:
   *
   * The synth-0 ring-1 gain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-ring-1-gain",
				   i18n_pspec("synth-0 ring-1 gain of recall"),
				   i18n_pspec("The synth-0 ring-1's gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_RING_1_GAIN,
				  param_spec);
  
  /**
   * AgsFxAbyssSynthAudio:synth-0-pitch-tuning:
   *
   * The synth 0 pitch tuning.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-pitch-tuning",
				   i18n_pspec("synth 0 pitch tuning of recall"),
				   i18n_pspec("The synth 0 pitch tuning"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_PITCH_TUNING,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-volume:
   *
   * The synth 0 volume.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-volume",
				   i18n_pspec("synth 0 volume of recall"),
				   i18n_pspec("The synth 0 volume"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_VOLUME,
				  param_spec);
  
  /**
   * AgsFxAbyssSynthAudio:synth-0-env-0-attack:
   *
   * The synth 0 env-0 attack.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-0-attack",
				   i18n_pspec("synth 0 env-0 attack of recall"),
				   i18n_pspec("The synth 0 env-0 attack"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_0_ATTACK,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-0-decay:
   *
   * The synth 0 env-0 decay.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-0-decay",
				   i18n_pspec("synth 0 env-0 decay of recall"),
				   i18n_pspec("The synth 0 env-0 decay"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_0_DECAY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-0-sustain:
   *
   * The synth 0 env-0 sustain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-0-sustain",
				   i18n_pspec("synth 0 env-0 sustain of recall"),
				   i18n_pspec("The synth 0 env-0 sustain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_0_SUSTAIN,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-0-release:
   *
   * The synth 0 env-0 release.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-0-release",
				   i18n_pspec("synth 0 env-0 release of recall"),
				   i18n_pspec("The synth 0 env-0 release"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_0_RELEASE,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-0-gain:
   *
   * The synth 0 env-0 gain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-0-gain",
				   i18n_pspec("synth 0 env-0 gain of recall"),
				   i18n_pspec("The synth 0 env-0 gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_0_GAIN,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-0-frequency:
   *
   * The synth 0 env-0 frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-0-frequency",
				   i18n_pspec("synth 0 env-0 frequency of recall"),
				   i18n_pspec("The synth 0 env-0 frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_0_FREQUENCY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-0-sends:
   *
   * The synth 0 env-0 sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-0-sends",
				   i18n_pspec("synth 0 env-0 sends of recall"),
				   i18n_pspec("The synth 0 env-0 sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_0_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-1-attack:
   *
   * The synth 0 env-1 attack.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-1-attack",
				   i18n_pspec("synth 0 env-1 attack of recall"),
				   i18n_pspec("The synth 0 env-1 attack"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_1_ATTACK,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-1-decay:
   *
   * The synth 0 env-1 decay.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-1-decay",
				   i18n_pspec("synth 0 env-1 decay of recall"),
				   i18n_pspec("The synth 0 env-1 decay"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_1_DECAY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-1-sustain:
   *
   * The synth 0 env-1 sustain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-1-sustain",
				   i18n_pspec("synth 0 env-1 sustain of recall"),
				   i18n_pspec("The synth 0 env-1 sustain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_1_SUSTAIN,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-1-release:
   *
   * The synth 0 env-1 release.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-1-release",
				   i18n_pspec("synth 0 env-1 release of recall"),
				   i18n_pspec("The synth 0 env-1 release"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_1_RELEASE,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-1-gain:
   *
   * The synth 0 env-1 gain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-1-gain",
				   i18n_pspec("synth 0 env-1 gain of recall"),
				   i18n_pspec("The synth 0 env-1 gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_1_GAIN,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-1-frequency:
   *
   * The synth 0 env-1 frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-1-frequency",
				   i18n_pspec("synth 0 env-1 frequency of recall"),
				   i18n_pspec("The synth 0 env-1 frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_1_FREQUENCY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-1-sends:
   *
   * The synth 0 env-1 sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-1-sends",
				   i18n_pspec("synth 0 env-1 sends of recall"),
				   i18n_pspec("The synth 0 env-1 sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_1_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-2-attack:
   *
   * The synth 0 env-2 attack.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-2-attack",
				   i18n_pspec("synth 0 env-2 attack of recall"),
				   i18n_pspec("The synth 0 env-2 attack"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_2_ATTACK,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-2-decay:
   *
   * The synth 0 env-2 decay.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-2-decay",
				   i18n_pspec("synth 0 env-2 decay of recall"),
				   i18n_pspec("The synth 0 env-2 decay"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_2_DECAY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-2-sustain:
   *
   * The synth 0 env-2 sustain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-2-sustain",
				   i18n_pspec("synth 0 env-2 sustain of recall"),
				   i18n_pspec("The synth 0 env-2 sustain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_2_SUSTAIN,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-2-release:
   *
   * The synth 0 env-2 release.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-2-release",
				   i18n_pspec("synth 0 env-2 release of recall"),
				   i18n_pspec("The synth 0 env-2 release"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_2_RELEASE,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-2-gain:
   *
   * The synth 0 env-2 gain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-2-gain",
				   i18n_pspec("synth 0 env-2 gain of recall"),
				   i18n_pspec("The synth 0 env-2 gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_2_GAIN,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-2-frequency:
   *
   * The synth 0 env-2 frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-2-frequency",
				   i18n_pspec("synth 0 env-2 frequency of recall"),
				   i18n_pspec("The synth 0 env-2 frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_2_FREQUENCY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-2-sends:
   *
   * The synth 0 env-2 sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-2-sends",
				   i18n_pspec("synth 0 env-2 sends of recall"),
				   i18n_pspec("The synth 0 env-2 sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_2_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-3-attack:
   *
   * The synth 0 env-3 attack.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-3-attack",
				   i18n_pspec("synth 0 env-3 attack of recall"),
				   i18n_pspec("The synth 0 env-3 attack"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_3_ATTACK,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-3-decay:
   *
   * The synth 0 env-3 decay.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-3-decay",
				   i18n_pspec("synth 0 env-3 decay of recall"),
				   i18n_pspec("The synth 0 env-3 decay"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_3_DECAY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-3-sustain:
   *
   * The synth 0 env-3 sustain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-3-sustain",
				   i18n_pspec("synth 0 env-3 sustain of recall"),
				   i18n_pspec("The synth 0 env-3 sustain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_3_SUSTAIN,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-3-release:
   *
   * The synth 0 env-3 release.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-3-release",
				   i18n_pspec("synth 0 env-3 release of recall"),
				   i18n_pspec("The synth 0 env-3 release"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_3_RELEASE,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-3-gain:
   *
   * The synth 0 env-3 gain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-3-gain",
				   i18n_pspec("synth 0 env-3 gain of recall"),
				   i18n_pspec("The synth 0 env-3 gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_3_GAIN,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-3-frequency:
   *
   * The synth 0 env-3 frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-3-frequency",
				   i18n_pspec("synth 0 env-3 frequency of recall"),
				   i18n_pspec("The synth 0 env-3 frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_3_FREQUENCY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-env-3-sends:
   *
   * The synth 0 env-3 sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-env-3-sends",
				   i18n_pspec("synth 0 env-3 sends of recall"),
				   i18n_pspec("The synth 0 env-3 sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_ENV_3_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-0-oscillator:
   *
   * The synth 0 lfo-0 oscillator.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-0-oscillator",
				   i18n_pspec("synth 0 lfo-0 oscillator of recall"),
				   i18n_pspec("The synth 0 lfo-0 oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_0_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-0-frequency:
   *
   * The synth 0 lfo-0 frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-0-frequency",
				   i18n_pspec("synth 0 lfo-0 frequency of recall"),
				   i18n_pspec("The synth 0 lfo-0 frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_0_FREQUENCY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-0-depth:
   *
   * The synth 0 lfo-0 depth.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-0-depth",
				   i18n_pspec("synth 0 lfo-0 depth of recall"),
				   i18n_pspec("The synth 0 lfo-0 depth"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_0_DEPTH,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-0-tuning:
   *
   * The synth 0 lfo-0 tuning.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-0-tuning",
				   i18n_pspec("synth 0 lfo-0 tuning of recall"),
				   i18n_pspec("The synth 0 lfo-0 tuning"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_0_TUNING,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-0-sends:
   *
   * The synth 0 lfo-0 sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-0-sends",
				   i18n_pspec("synth 0 lfo-0 sends of recall"),
				   i18n_pspec("The synth 0 lfo-0 sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_0_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-1-oscillator:
   *
   * The synth 0 lfo-1 oscillator.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-1-oscillator",
				   i18n_pspec("synth 0 lfo-1 oscillator of recall"),
				   i18n_pspec("The synth 0 lfo-1 oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_1_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-1-frequency:
   *
   * The synth 0 lfo-1 frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-1-frequency",
				   i18n_pspec("synth 0 lfo-1 frequency of recall"),
				   i18n_pspec("The synth 0 lfo-1 frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_1_FREQUENCY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-1-depth:
   *
   * The synth 0 lfo-1 depth.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-1-depth",
				   i18n_pspec("synth 0 lfo-1 depth of recall"),
				   i18n_pspec("The synth 0 lfo-1 depth"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_1_DEPTH,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-1-tuning:
   *
   * The synth 0 lfo-1 tuning.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-1-tuning",
				   i18n_pspec("synth 0 lfo-1 tuning of recall"),
				   i18n_pspec("The synth 0 lfo-1 tuning"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_1_TUNING,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-1-sends:
   *
   * The synth 0 lfo-1 sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-1-sends",
				   i18n_pspec("synth 0 lfo-1 sends of recall"),
				   i18n_pspec("The synth 0 lfo-1 sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_1_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-2-oscillator:
   *
   * The synth 0 lfo-2 oscillator.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-2-oscillator",
				   i18n_pspec("synth 0 lfo-2 oscillator of recall"),
				   i18n_pspec("The synth 0 lfo-2 oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_2_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-2-frequency:
   *
   * The synth 0 lfo-2 frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-2-frequency",
				   i18n_pspec("synth 0 lfo-2 frequency of recall"),
				   i18n_pspec("The synth 0 lfo-2 frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_2_FREQUENCY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-2-depth:
   *
   * The synth 0 lfo-2 depth.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-2-depth",
				   i18n_pspec("synth 0 lfo-2 depth of recall"),
				   i18n_pspec("The synth 0 lfo-2 depth"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_2_DEPTH,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-2-tuning:
   *
   * The synth 0 lfo-2 tuning.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-2-tuning",
				   i18n_pspec("synth 0 lfo-2 tuning of recall"),
				   i18n_pspec("The synth 0 lfo-2 tuning"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_2_TUNING,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-2-sends:
   *
   * The synth 0 lfo-2 sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-2-sends",
				   i18n_pspec("synth 0 lfo-2 sends of recall"),
				   i18n_pspec("The synth 0 lfo-2 sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_2_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-3-oscillator:
   *
   * The synth 0 lfo-3 oscillator.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-3-oscillator",
				   i18n_pspec("synth 0 lfo-3 oscillator of recall"),
				   i18n_pspec("The synth 0 lfo-3 oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_3_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-3-frequency:
   *
   * The synth 0 lfo-3 frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-3-frequency",
				   i18n_pspec("synth 0 lfo-3 frequency of recall"),
				   i18n_pspec("The synth 0 lfo-3 frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_3_FREQUENCY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-3-depth:
   *
   * The synth 0 lfo-3 depth.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-3-depth",
				   i18n_pspec("synth 0 lfo-3 depth of recall"),
				   i18n_pspec("The synth 0 lfo-3 depth"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_3_DEPTH,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-3-tuning:
   *
   * The synth 0 lfo-3 tuning.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-3-tuning",
				   i18n_pspec("synth 0 lfo-3 tuning of recall"),
				   i18n_pspec("The synth 0 lfo-3 tuning"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_3_TUNING,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-lfo-3-sends:
   *
   * The synth 0 lfo-3 sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-3-sends",
				   i18n_pspec("synth 0 lfo-3 sends of recall"),
				   i18n_pspec("The synth 0 lfo-3 sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_3_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-0:
   *
   * The synth-0 sequencer 0 modulation 0.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-0",
				   i18n_pspec("synth-0 seq 0 modulation 0 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 0"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_0,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-1:
   *
   * The synth-0 sequencer 0 modulation 1.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-1",
				   i18n_pspec("synth-0 seq 0 modulation 1 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 1"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_1,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-2:
   *
   * The synth-0 sequencer 0 modulation 2.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-2",
				   i18n_pspec("synth-0 seq 0 modulation 2 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 2"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_2,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-3:
   *
   * The synth-0 sequencer 0 modulation 3.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-3",
				   i18n_pspec("synth-0 seq 0 modulation 3 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 3"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_3,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-4:
   *
   * The synth-0 sequencer 0 modulation 4.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-4",
				   i18n_pspec("synth-0 seq 0 modulation 4 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 4"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_4,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-5:
   *
   * The synth-0 sequencer 0 modulation 5.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-5",
				   i18n_pspec("synth-0 seq 0 modulation 5 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 5"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_5,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-6:
   *
   * The synth-0 sequencer 0 modulation 6.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-6",
				   i18n_pspec("synth-0 seq 0 modulation 6 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 6"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_6,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-7:
   *
   * The synth-0 sequencer 0 modulation 7.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-7",
				   i18n_pspec("synth-0 seq 0 modulation 7 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 7"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_7,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-8:
   *
   * The synth-0 sequencer 0 modulation 8.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-8",
				   i18n_pspec("synth-0 seq 0 modulation 8 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 8"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_8,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-9:
   *
   * The synth-0 sequencer 0 modulation 9.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-9",
				   i18n_pspec("synth-0 seq 0 modulation 9 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 9"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_9,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-10:
   *
   * The synth-0 sequencer 0 modulation 10.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-10",
				   i18n_pspec("synth-0 seq 0 modulation 10 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 10"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_10,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-11:
   *
   * The synth-0 sequencer 0 modulation 11.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-11",
				   i18n_pspec("synth-0 seq 0 modulation 11 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 11"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_11,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-12:
   *
   * The synth-0 sequencer 0 modulation 12.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-12",
				   i18n_pspec("synth-0 seq 0 modulation 12 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 12"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_12,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-13:
   *
   * The synth-0 sequencer 0 modulation 13.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-13",
				   i18n_pspec("synth-0 seq 0 modulation 13 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 13"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_13,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-14:
   *
   * The synth-0 sequencer 0 modulation 14.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-14",
				   i18n_pspec("synth-0 seq 0 modulation 14 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 14"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_14,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-modulation-15:
   *
   * The synth-0 sequencer 0 modulation 15.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-modulation-15",
				   i18n_pspec("synth-0 seq 0 modulation 15 of recall"),
				   i18n_pspec("The synth-0's seq 0 modulation 15"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_MODULATION_15,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-pingpong:
   *
   * The synth-0 sequencer 0 pingpong.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-pingpong",
				   i18n_pspec("synth-0 seq 0 pingpong of recall"),
				   i18n_pspec("The synth-0's seq 0 pingpong"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_PINGPONG,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-lfo-frequency:
   *
   * The synth-0 sequencer 0 lfo frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-lfo-frequency",
				   i18n_pspec("synth-0 seq 0 LFO frequency of recall"),
				   i18n_pspec("The synth-0's seq 0 LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-0-sends:
   *
   * The synth-0 sequencer 0 sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-0-sends",
				   i18n_pspec("synth-0 seq 0 sends of recall"),
				   i18n_pspec("The synth-0's seq 0 sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_0_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-0:
   *
   * The synth-0 sequencer 0 modulation 0.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-0",
				   i18n_pspec("synth-0 seq 1 modulation 0 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 0"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_0,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-1:
   *
   * The synth-0 sequencer 0 modulation 1.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-1",
				   i18n_pspec("synth-0 seq 1 modulation 1 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 1"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_1,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-2:
   *
   * The synth-0 sequencer 0 modulation 2.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-2",
				   i18n_pspec("synth-0 seq 1 modulation 2 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 2"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_2,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-3:
   *
   * The synth-0 sequencer 0 modulation 3.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-3",
				   i18n_pspec("synth-0 seq 1 modulation 3 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 3"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_3,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-4:
   *
   * The synth-0 sequencer 0 modulation 4.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-4",
				   i18n_pspec("synth-0 seq 1 modulation 4 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 4"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_4,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-5:
   *
   * The synth-0 sequencer 0 modulation 5.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-5",
				   i18n_pspec("synth-0 seq 1 modulation 5 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 5"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_5,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-6:
   *
   * The synth-0 sequencer 0 modulation 6.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-6",
				   i18n_pspec("synth-0 seq 1 modulation 6 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 6"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_6,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-7:
   *
   * The synth-0 sequencer 0 modulation 7.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-7",
				   i18n_pspec("synth-0 seq 1 modulation 7 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 7"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_7,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-8:
   *
   * The synth-0 sequencer 0 modulation 8.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-8",
				   i18n_pspec("synth-0 seq 1 modulation 8 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 8"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_8,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-9:
   *
   * The synth-0 sequencer 0 modulation 9.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-9",
				   i18n_pspec("synth-0 seq 1 modulation 9 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 9"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_9,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-10:
   *
   * The synth-0 sequencer 0 modulation 10.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-10",
				   i18n_pspec("synth-0 seq 1 modulation 10 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 10"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_10,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-11:
   *
   * The synth-0 sequencer 0 modulation 11.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-11",
				   i18n_pspec("synth-0 seq 1 modulation 11 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 11"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_11,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-12:
   *
   * The synth-0 sequencer 0 modulation 12.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-12",
				   i18n_pspec("synth-0 seq 1 modulation 12 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 12"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_12,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-13:
   *
   * The synth-0 sequencer 0 modulation 13.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-13",
				   i18n_pspec("synth-0 seq 1 modulation 13 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 13"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_13,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-14:
   *
   * The synth-0 sequencer 0 modulation 14.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-14",
				   i18n_pspec("synth-0 seq 1 modulation 14 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 14"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_14,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-modulation-15:
   *
   * The synth-0 sequencer 0 modulation 15.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-modulation-15",
				   i18n_pspec("synth-0 seq 1 modulation 15 of recall"),
				   i18n_pspec("The synth-0's seq 1 modulation 15"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_MODULATION_15,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-pingpong:
   *
   * The synth-0 sequencer 0 pingpong.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-pingpong",
				   i18n_pspec("synth-0 seq 1 pingpong of recall"),
				   i18n_pspec("The synth-0's seq 1 pingpong"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_PINGPONG,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-lfo-frequency:
   *
   * The synth-0 sequencer 0 lfo frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-lfo-frequency",
				   i18n_pspec("synth-0 seq 1 LFO frequency of recall"),
				   i18n_pspec("The synth-0's seq 1 LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-1-sends:
   *
   * The synth-0 sequencer 0 sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-1-sends",
				   i18n_pspec("synth-0 seq 1 sends of recall"),
				   i18n_pspec("The synth-0's seq 1 sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_1_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-0:
   *
   * The synth-0 sequencer 0 modulation 0.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-0",
				   i18n_pspec("synth-0 seq 2 modulation 0 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 0"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_0,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-1:
   *
   * The synth-0 sequencer 0 modulation 1.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-1",
				   i18n_pspec("synth-0 seq 2 modulation 1 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 1"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_1,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-2:
   *
   * The synth-0 sequencer 0 modulation 2.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-2",
				   i18n_pspec("synth-0 seq 2 modulation 2 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 2"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_2,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-3:
   *
   * The synth-0 sequencer 0 modulation 3.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-3",
				   i18n_pspec("synth-0 seq 2 modulation 3 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 3"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_3,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-4:
   *
   * The synth-0 sequencer 0 modulation 4.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-4",
				   i18n_pspec("synth-0 seq 2 modulation 4 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 4"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_4,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-5:
   *
   * The synth-0 sequencer 0 modulation 5.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-5",
				   i18n_pspec("synth-0 seq 2 modulation 5 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 5"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_5,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-6:
   *
   * The synth-0 sequencer 0 modulation 6.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-6",
				   i18n_pspec("synth-0 seq 2 modulation 6 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 6"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_6,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-7:
   *
   * The synth-0 sequencer 0 modulation 7.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-7",
				   i18n_pspec("synth-0 seq 2 modulation 7 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 7"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_7,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-8:
   *
   * The synth-0 sequencer 0 modulation 8.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-8",
				   i18n_pspec("synth-0 seq 2 modulation 8 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 8"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_8,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-9:
   *
   * The synth-0 sequencer 0 modulation 9.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-9",
				   i18n_pspec("synth-0 seq 2 modulation 9 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 9"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_9,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-10:
   *
   * The synth-0 sequencer 0 modulation 10.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-10",
				   i18n_pspec("synth-0 seq 2 modulation 10 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 10"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_10,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-11:
   *
   * The synth-0 sequencer 0 modulation 11.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-11",
				   i18n_pspec("synth-0 seq 2 modulation 11 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 11"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_11,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-12:
   *
   * The synth-0 sequencer 0 modulation 12.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-12",
				   i18n_pspec("synth-0 seq 2 modulation 12 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 12"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_12,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-13:
   *
   * The synth-0 sequencer 0 modulation 13.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-13",
				   i18n_pspec("synth-0 seq 2 modulation 13 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 13"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_13,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-14:
   *
   * The synth-0 sequencer 0 modulation 14.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-14",
				   i18n_pspec("synth-0 seq 2 modulation 14 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 14"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_14,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-modulation-15:
   *
   * The synth-0 sequencer 0 modulation 15.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-modulation-15",
				   i18n_pspec("synth-0 seq 2 modulation 15 of recall"),
				   i18n_pspec("The synth-0's seq 2 modulation 15"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_MODULATION_15,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-pingpong:
   *
   * The synth-0 sequencer 0 pingpong.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-pingpong",
				   i18n_pspec("synth-0 seq 2 pingpong of recall"),
				   i18n_pspec("The synth-0's seq 2 pingpong"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_PINGPONG,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-lfo-frequency:
   *
   * The synth-0 sequencer 0 lfo frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-lfo-frequency",
				   i18n_pspec("synth-0 seq 2 LFO frequency of recall"),
				   i18n_pspec("The synth-0's seq 2 LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-2-sends:
   *
   * The synth-0 sequencer 0 sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-2-sends",
				   i18n_pspec("synth-0 seq 2 sends of recall"),
				   i18n_pspec("The synth-0's seq 2 sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_2_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-0:
   *
   * The synth-0 sequencer 0 modulation 0.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-0",
				   i18n_pspec("synth-0 seq 3 modulation 0 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 0"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_0,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-1:
   *
   * The synth-0 sequencer 0 modulation 1.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-1",
				   i18n_pspec("synth-0 seq 3 modulation 1 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 1"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_1,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-2:
   *
   * The synth-0 sequencer 0 modulation 2.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-2",
				   i18n_pspec("synth-0 seq 3 modulation 2 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 2"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_2,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-3:
   *
   * The synth-0 sequencer 0 modulation 3.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-3",
				   i18n_pspec("synth-0 seq 3 modulation 3 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 3"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_3,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-4:
   *
   * The synth-0 sequencer 0 modulation 4.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-4",
				   i18n_pspec("synth-0 seq 3 modulation 4 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 4"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_4,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-5:
   *
   * The synth-0 sequencer 0 modulation 5.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-5",
				   i18n_pspec("synth-0 seq 3 modulation 5 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 5"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_5,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-6:
   *
   * The synth-0 sequencer 0 modulation 6.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-6",
				   i18n_pspec("synth-0 seq 3 modulation 6 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 6"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_6,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-7:
   *
   * The synth-0 sequencer 0 modulation 7.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-7",
				   i18n_pspec("synth-0 seq 3 modulation 7 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 7"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_7,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-8:
   *
   * The synth-0 sequencer 0 modulation 8.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-8",
				   i18n_pspec("synth-0 seq 3 modulation 8 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 8"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_8,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-9:
   *
   * The synth-0 sequencer 0 modulation 9.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-9",
				   i18n_pspec("synth-0 seq 3 modulation 9 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 9"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_9,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-10:
   *
   * The synth-0 sequencer 0 modulation 10.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-10",
				   i18n_pspec("synth-0 seq 3 modulation 10 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 10"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_10,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-11:
   *
   * The synth-0 sequencer 0 modulation 11.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-11",
				   i18n_pspec("synth-0 seq 3 modulation 11 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 11"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_11,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-12:
   *
   * The synth-0 sequencer 0 modulation 12.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-12",
				   i18n_pspec("synth-0 seq 3 modulation 12 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 12"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_12,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-13:
   *
   * The synth-0 sequencer 0 modulation 13.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-13",
				   i18n_pspec("synth-0 seq 3 modulation 13 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 13"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_13,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-14:
   *
   * The synth-0 sequencer 0 modulation 14.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-14",
				   i18n_pspec("synth-0 seq 3 modulation 14 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 14"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_14,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-modulation-15:
   *
   * The synth-0 sequencer 0 modulation 15.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-modulation-15",
				   i18n_pspec("synth-0 seq 3 modulation 15 of recall"),
				   i18n_pspec("The synth-0's seq 3 modulation 15"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_MODULATION_15,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-pingpong:
   *
   * The synth-0 sequencer 0 pingpong.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-pingpong",
				   i18n_pspec("synth-0 seq 3 pingpong of recall"),
				   i18n_pspec("The synth-0's seq 3 pingpong"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_PINGPONG,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-lfo-frequency:
   *
   * The synth-0 sequencer 0 lfo frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-lfo-frequency",
				   i18n_pspec("synth-0 seq 3 LFO frequency of recall"),
				   i18n_pspec("The synth-0's seq 3 LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-seq-3-sends:
   *
   * The synth-0 sequencer 0 sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-seq-3-sends",
				   i18n_pspec("synth-0 seq 3 sends of recall"),
				   i18n_pspec("The synth-0's seq 3 sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_3_SENDS,
				  param_spec);
  
  /**
   * AgsFxAbyssSynthAudio:synth-0-pink-noise-0-frequency:
   *
   * The synth 0 pink noise 0 frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-pink-noise-0-frequency",
				   i18n_pspec("synth 0 pink noise 0 frequency of recall"),
				   i18n_pspec("The synth 0 pink noise 0 frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_PINK_NOISE_0_FREQUENCY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-pink-noise-0-gain:
   *
   * The synth 0 pink noise 0 gain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-pink-noise-0-gain",
				   i18n_pspec("synth 0 pink noise 0 gain of recall"),
				   i18n_pspec("The synth 0 pink noise 0 gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_PINK_NOISE_0_GAIN,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-pink-noise-0-sends:
   *
   * The synth 0 pink noise 0 sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-pink-noise-0-sends",
				   i18n_pspec("synth 0 pink noise 0 sends of recall"),
				   i18n_pspec("The synth 0 pink noise 0 sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_PINK_NOISE_0_SENDS,
				  param_spec);
  
  /**
   * AgsFxAbyssSynthAudio:synth-0-pink-noise-1-frequency:
   *
   * The synth 0 pink noise 1 frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-pink-noise-1-frequency",
				   i18n_pspec("synth 0 pink noise 1 frequency of recall"),
				   i18n_pspec("The synth 0 pink noise 1 frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_PINK_NOISE_1_FREQUENCY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-pink-noise-1-gain:
   *
   * The synth 0 pink noise 1 gain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-pink-noise-1-gain",
				   i18n_pspec("synth 0 pink noise 1 gain of recall"),
				   i18n_pspec("The synth 0 pink noise 1 gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_PINK_NOISE_1_GAIN,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:synth-0-pink-noise-1-sends:
   *
   * The synth 0 pink noise 1 sends.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("synth-0-pink-noise-1-sends",
				   i18n_pspec("synth 0 pink noise 1 sends of recall"),
				   i18n_pspec("The synth 0 pink noise 1 sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_PINK_NOISE_1_SENDS,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:low-pass-0-cut-off-frequency:
   *
   * The low-pass-0-cut-off-frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("low-pass-0-cut-off-frequency",
				   i18n_pspec("low-pass 0 cut off frequency of recall"),
				   i18n_pspec("The low-pass 0 cut off frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOW_PASS_0_CUT_OFF_FREQUENCY,
				  param_spec);
  
  /**
   * AgsFxAbyssSynthAudio:low-pass-0-filter-gain:
   *
   * The low-pass-0-filter-gain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("low-pass-0-filter-gain",
				   i18n_pspec("low-pass 0 filter gain of recall"),
				   i18n_pspec("The low-pass 0 filter gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOW_PASS_0_FILTER_GAIN,
				  param_spec);
  
  /**
   * AgsFxAbyssSynthAudio:low-pass-0-no-clip:
   *
   * The low-pass-0-no-clip.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("low-pass-0-no-clip",
				   i18n_pspec("low-pass 0 no-clip of recall"),
				   i18n_pspec("The low-pass 0 no clip"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOW_PASS_0_NO_CLIP,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:amplifier-0-amp-0-gain:
   *
   * The amplifier 0 amp-0 gain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("amplifier-0-amp-0-gain",
				   i18n_pspec("amplifier 0 amp-0 gain of recall"),
				   i18n_pspec("The amplifier 0 amp-0 gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AMPLIFIER_0_AMP_0_GAIN,
				  param_spec);
  
  /**
   * AgsFxAbyssSynthAudio:amplifier-0-amp-1-gain:
   *
   * The amplifier 0 amp-1 gain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("amplifier-0-amp-1-gain",
				   i18n_pspec("amplifier 0 amp-1 gain of recall"),
				   i18n_pspec("The amplifier 0 amp-1 gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AMPLIFIER_0_AMP_1_GAIN,
				  param_spec);
  
  /**
   * AgsFxAbyssSynthAudio:amplifier-0-amp-2-gain:
   *
   * The amplifier 0 amp-2 gain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("amplifier-0-amp-2-gain",
				   i18n_pspec("amplifier 0 amp-2 gain of recall"),
				   i18n_pspec("The amplifier 0 amp-2 gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AMPLIFIER_0_AMP_2_GAIN,
				  param_spec);
  
  /**
   * AgsFxAbyssSynthAudio:amplifier-0-amp-3-gain:
   *
   * The amplifier 0 amp-3 gain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("amplifier-0-amp-3-gain",
				   i18n_pspec("amplifier 0 amp-3 gain of recall"),
				   i18n_pspec("The amplifier 0 amp-3 gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AMPLIFIER_0_AMP_3_GAIN,
				  param_spec);
  
  /**
   * AgsFxAbyssSynthAudio:amplifier-0-filter-gain:
   *
   * The amplifier 0 filter gain.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("amplifier-0-filter-gain",
				   i18n_pspec("amplifier 0 filter gain of recall"),
				   i18n_pspec("The amplifier 0 filter gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AMPLIFIER_0_FILTER_GAIN,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:chorus-enabled:
   *
   * The chorus enabled.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("chorus-enabled",
				   i18n_pspec("chorus enabled of recall"),
				   i18n_pspec("The chorus enabled"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHORUS_ENABLED,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:chorus-pitch-type:
   *
   * The chorus pitch type.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("chorus-pitch-type",
				   i18n_pspec("chorus pitch type of recall"),
				   i18n_pspec("The chorus pitch type"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHORUS_PITCH_TYPE,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:chorus-input-volume:
   *
   * The chorus input volume.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("chorus-input-volume",
				   i18n_pspec("chorus input volume of recall"),
				   i18n_pspec("The chorus input volume"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHORUS_INPUT_VOLUME,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:chorus-output-volume:
   *
   * The chorus output volume.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("chorus-output-volume",
				   i18n_pspec("chorus output volume of recall"),
				   i18n_pspec("The chorus output volume"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHORUS_OUTPUT_VOLUME,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:chorus-lfo-oscillator:
   *
   * The chorus lfo oscillator.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("chorus-lfo-oscillator",
				   i18n_pspec("chorus lfo oscillator of recall"),
				   i18n_pspec("The chorus lfo oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHORUS_LFO_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:chorus-lfo-frequency:
   *
   * The chorus lfo frequency.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("chorus-lfo-frequency",
				   i18n_pspec("chorus lfo frequency of recall"),
				   i18n_pspec("The chorus lfo frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHORUS_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:chorus-depth:
   *
   * The chorus depth.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("chorus-depth",
				   i18n_pspec("chorus depth of recall"),
				   i18n_pspec("The chorus depth"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHORUS_DEPTH,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:chorus-mix:
   *
   * The chorus mix.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("chorus-mix",
				   i18n_pspec("chorus mix of recall"),
				   i18n_pspec("The chorus mix"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHORUS_MIX,
				  param_spec);

  /**
   * AgsFxAbyssSynthAudio:chorus-delay:
   *
   * The chorus delay.
   * 
   * Since: 9.4.0
   */
  param_spec = g_param_spec_object("chorus-delay",
				   i18n_pspec("chorus delay of recall"),
				   i18n_pspec("The chorus delay"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHORUS_DELAY,
				  param_spec);
}

void
ags_fx_abyss_synth_audio_init(AgsFxAbyssSynthAudio *fx_abyss_synth_audio)
{
  gint position;
  guint i;
  
  g_signal_connect(fx_abyss_synth_audio, "notify::audio",
		   G_CALLBACK(ags_fx_abyss_synth_audio_notify_audio_callback), NULL);

  g_signal_connect(fx_abyss_synth_audio, "notify::buffer-size",
		   G_CALLBACK(ags_fx_abyss_synth_audio_notify_buffer_size_callback), NULL);

  g_signal_connect(fx_abyss_synth_audio, "notify::format",
		   G_CALLBACK(ags_fx_abyss_synth_audio_notify_format_callback), NULL);

  g_signal_connect(fx_abyss_synth_audio, "notify::samplerate",
		   G_CALLBACK(ags_fx_abyss_synth_audio_notify_samplerate_callback), NULL);

  AGS_RECALL(fx_abyss_synth_audio)->name = "ags-fx-abyss-synth";
  AGS_RECALL(fx_abyss_synth_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_abyss_synth_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_abyss_synth_audio)->xml_type = "ags-fx-abyss-synth-audio";

  position = 0;

  /* synth-0 osc-0 oscillator */
  fx_abyss_synth_audio->synth_0_osc_0_oscillator = g_object_new(AGS_TYPE_PORT,
								"plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								"specifier", "./synth-0-osc-0-oscillator[0]",
								"control-port", "1/56",
								"port-value-is-pointer", FALSE,
								"port-value-type", G_TYPE_FLOAT,
								"port-value-size", sizeof(gfloat),
								"port-value-length", 1,
								NULL);
  
  fx_abyss_synth_audio->synth_0_osc_0_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_0_oscillator,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_0_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_0_oscillator);

  position++;

  /* synth-0 osc-0 octave */
  fx_abyss_synth_audio->synth_0_osc_0_octave = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-osc-0-octave[0]",
							    "control-port", "2/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_osc_0_octave->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_0_octave,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_0_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_0_octave);

  position++;

  /* synth-0 osc-0 key */
  fx_abyss_synth_audio->synth_0_osc_0_key = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							 "specifier", "./synth-0-osc-0-key[0]",
							 "control-port", "3/56",
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_abyss_synth_audio->synth_0_osc_0_key->port_value.ags_port_float = (gfloat) 2.0;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_0_key,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_0_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_0_key);

  position++;

  /* synth-0 osc-0 phase */
  fx_abyss_synth_audio->synth_0_osc_0_phase = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-osc-0-phase[0]",
							   "control-port", "4/56",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_osc_0_phase->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_0_phase,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_0_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_0_phase);

  position++;

  /* synth-0 osc-0 volume */
  fx_abyss_synth_audio->synth_0_osc_0_volume = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-osc-0-volume[0]",
							    "control-port", "5/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_osc_0_volume->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_0_volume,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_0_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_0_volume);

  position++;

  /* synth-0 osc-0 low-pass sends */
  fx_abyss_synth_audio->synth_0_osc_0_low_pass_sends = g_object_new(AGS_TYPE_PORT,
								    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								    "specifier", "./synth-0-osc-0-low-pass-sends[0]",
								    "control-port", "5/56",
								    "port-value-is-pointer", FALSE,
								    "port-value-type", G_TYPE_FLOAT,
								    "port-value-size", sizeof(gfloat),
								    "port-value-length", 1,
								    NULL);
  
  fx_abyss_synth_audio->synth_0_osc_0_low_pass_sends->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_0_low_pass_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_0_low_pass_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_0_low_pass_sends);

  position++;

  /* synth-0 osc-1 oscillator */
  fx_abyss_synth_audio->synth_0_osc_1_oscillator = g_object_new(AGS_TYPE_PORT,
								"plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								"specifier", "./synth-0-osc-1-oscillator[0]",
								"control-port", "6/56",
								"port-value-is-pointer", FALSE,
								"port-value-type", G_TYPE_FLOAT,
								"port-value-size", sizeof(gfloat),
								"port-value-length", 1,
								NULL);
  
  fx_abyss_synth_audio->synth_0_osc_1_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_1_oscillator,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_1_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_1_oscillator);

  position++;

  /* synth-0 osc-1 octave */
  fx_abyss_synth_audio->synth_0_osc_1_octave = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-osc-1-octave[0]",
							    "control-port", "7/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_osc_1_octave->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_1_octave,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_1_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_1_octave);

  position++;

  /* synth-0 osc-1 key */
  fx_abyss_synth_audio->synth_0_osc_1_key = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							 "specifier", "./synth-0-osc-1-key[0]",
							 "control-port", "8/56",
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_abyss_synth_audio->synth_0_osc_1_key->port_value.ags_port_float = (gfloat) 2.0;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_1_key,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_1_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_1_key);

  position++;

  /* synth-0 osc-1 phase */
  fx_abyss_synth_audio->synth_0_osc_1_phase = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-osc-1-phase[0]",
							   "control-port", "9/56",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_osc_1_phase->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_1_phase,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_1_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_1_phase);

  position++;

  /* synth-0 osc-1 volume */
  fx_abyss_synth_audio->synth_0_osc_1_volume = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-osc-1-volume[0]",
							    "control-port", "10/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_osc_1_volume->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_1_volume,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_1_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_1_volume);

  position++;

  /* synth-0 osc-1 low-pass sends */
  fx_abyss_synth_audio->synth_0_osc_1_low_pass_sends = g_object_new(AGS_TYPE_PORT,
								    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								    "specifier", "./synth-0-osc-1-low-pass-sends[0]",
								    "control-port", "5/56",
								    "port-value-is-pointer", FALSE,
								    "port-value-type", G_TYPE_FLOAT,
								    "port-value-size", sizeof(gfloat),
								    "port-value-length", 1,
								    NULL);
  
  fx_abyss_synth_audio->synth_0_osc_1_low_pass_sends->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_1_low_pass_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_1_low_pass_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_1_low_pass_sends);

  position++;

  /* synth-0 osc-2 oscillator */
  fx_abyss_synth_audio->synth_0_osc_2_oscillator = g_object_new(AGS_TYPE_PORT,
								"plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								"specifier", "./synth-0-osc-2-oscillator[0]",
								"control-port", "1/56",
								"port-value-is-pointer", FALSE,
								"port-value-type", G_TYPE_FLOAT,
								"port-value-size", sizeof(gfloat),
								"port-value-length", 1,
								NULL);
  
  fx_abyss_synth_audio->synth_0_osc_2_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_2_oscillator,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_2_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_2_oscillator);

  position++;

  /* synth-0 osc-2 octave */
  fx_abyss_synth_audio->synth_0_osc_2_octave = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-osc-2-octave[0]",
							    "control-port", "2/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_osc_2_octave->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_2_octave,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_2_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_2_octave);

  position++;

  /* synth-0 osc-2 key */
  fx_abyss_synth_audio->synth_0_osc_2_key = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							 "specifier", "./synth-0-osc-2-key[0]",
							 "control-port", "3/56",
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_abyss_synth_audio->synth_0_osc_2_key->port_value.ags_port_float = (gfloat) 2.0;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_2_key,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_2_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_2_key);

  position++;

  /* synth-0 osc-2 phase */
  fx_abyss_synth_audio->synth_0_osc_2_phase = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-osc-2-phase[0]",
							   "control-port", "4/56",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_osc_2_phase->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_2_phase,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_2_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_2_phase);

  position++;

  /* synth-0 osc-2 volume */
  fx_abyss_synth_audio->synth_0_osc_2_volume = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-osc-2-volume[0]",
							    "control-port", "5/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_osc_2_volume->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_2_volume,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_2_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_2_volume);

  position++;

  /* synth-0 osc-2 low-pass sends */
  fx_abyss_synth_audio->synth_0_osc_2_low_pass_sends = g_object_new(AGS_TYPE_PORT,
								    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								    "specifier", "./synth-0-osc-2-low-pass-sends[0]",
								    "control-port", "5/56",
								    "port-value-is-pointer", FALSE,
								    "port-value-type", G_TYPE_FLOAT,
								    "port-value-size", sizeof(gfloat),
								    "port-value-length", 1,
								    NULL);
  
  fx_abyss_synth_audio->synth_0_osc_2_low_pass_sends->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_2_low_pass_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_2_low_pass_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_2_low_pass_sends);

  position++;

  /* synth-0 osc-3 oscillator */
  fx_abyss_synth_audio->synth_0_osc_3_oscillator = g_object_new(AGS_TYPE_PORT,
								"plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								"specifier", "./synth-0-osc-3-oscillator[0]",
								"control-port", "1/56",
								"port-value-is-pointer", FALSE,
								"port-value-type", G_TYPE_FLOAT,
								"port-value-size", sizeof(gfloat),
								"port-value-length", 1,
								NULL);
  
  fx_abyss_synth_audio->synth_0_osc_3_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_3_oscillator,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_3_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_3_oscillator);

  position++;

  /* synth-0 osc-3 octave */
  fx_abyss_synth_audio->synth_0_osc_3_octave = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-osc-3-octave[0]",
							    "control-port", "2/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_osc_3_octave->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_3_octave,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_3_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_3_octave);

  position++;

  /* synth-0 osc-3 key */
  fx_abyss_synth_audio->synth_0_osc_3_key = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							 "specifier", "./synth-0-osc-3-key[0]",
							 "control-port", "3/56",
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_abyss_synth_audio->synth_0_osc_3_key->port_value.ags_port_float = (gfloat) 2.0;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_3_key,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_3_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_3_key);

  position++;

  /* synth-0 osc-3 phase */
  fx_abyss_synth_audio->synth_0_osc_3_phase = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-osc-3-phase[0]",
							   "control-port", "4/56",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_osc_3_phase->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_3_phase,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_3_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_3_phase);

  position++;

  /* synth-0 osc-3 volume */
  fx_abyss_synth_audio->synth_0_osc_3_volume = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-osc-3-volume[0]",
							    "control-port", "5/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_osc_3_volume->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_3_volume,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_3_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_3_volume);

  position++;

  /* synth-0 osc-3 low-pass sends */
  fx_abyss_synth_audio->synth_0_osc_3_low_pass_sends = g_object_new(AGS_TYPE_PORT,
								    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								    "specifier", "./synth-0-osc-3-low-pass-sends[0]",
								    "control-port", "5/56",
								    "port-value-is-pointer", FALSE,
								    "port-value-type", G_TYPE_FLOAT,
								    "port-value-size", sizeof(gfloat),
								    "port-value-length", 1,
								    NULL);
  
  fx_abyss_synth_audio->synth_0_osc_3_low_pass_sends->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_osc_3_low_pass_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_osc_3_low_pass_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_osc_3_low_pass_sends);

  position++;

  /* synth-0 ring-0 enabled */
  fx_abyss_synth_audio->synth_0_ring_0_enabled = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							      "specifier", "./synth-0-ring-0-enabled[0]",
							      "control-port", "5/56",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_abyss_synth_audio->synth_0_ring_0_enabled->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_ring_0_enabled,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_ring_0_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_ring_0_enabled);

  position++;

  /* synth-0 ring-0 tuning */
  fx_abyss_synth_audio->synth_0_ring_0_tuning = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							     "specifier", "./synth-0-ring-0-tuning[0]",
							     "control-port", "5/56",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_abyss_synth_audio->synth_0_ring_0_tuning->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_ring_0_tuning,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_ring_0_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_ring_0_tuning);

  position++;

  /* synth-0 ring-0 drive */
  fx_abyss_synth_audio->synth_0_ring_0_drive = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-ring-0-drive[0]",
							    "control-port", "5/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_ring_0_drive->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_ring_0_drive,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_ring_0_drive_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_ring_0_drive);

  position++;

  /* synth-0 ring-0 mix */
  fx_abyss_synth_audio->synth_0_ring_0_mix = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							  "specifier", "./synth-0-ring-0-mix[0]",
							  "control-port", "5/56",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_abyss_synth_audio->synth_0_ring_0_mix->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_ring_0_mix,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_ring_0_mix_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_ring_0_mix);

  position++;

  /* synth-0 ring-0 gain */
  fx_abyss_synth_audio->synth_0_ring_0_gain = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-ring-0-gain[0]",
							   "control-port", "5/56",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_ring_0_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_ring_0_gain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_ring_0_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_ring_0_gain);

  position++;

  /* synth-0 ring-1 enabled */
  fx_abyss_synth_audio->synth_0_ring_1_enabled = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							      "specifier", "./synth-0-ring-1-enabled[0]",
							      "control-port", "5/56",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_abyss_synth_audio->synth_0_ring_1_enabled->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_ring_1_enabled,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_ring_1_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_ring_1_enabled);

  position++;

  /* synth-0 ring-1 tuning */
  fx_abyss_synth_audio->synth_0_ring_1_tuning = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							     "specifier", "./synth-0-ring-1-tuning[0]",
							     "control-port", "5/56",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_abyss_synth_audio->synth_0_ring_1_tuning->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_ring_1_tuning,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_ring_1_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_ring_1_tuning);

  position++;

  /* synth-0 ring-1 drive */
  fx_abyss_synth_audio->synth_0_ring_1_drive = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-ring-1-drive[0]",
							    "control-port", "5/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_ring_1_drive->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_ring_1_drive,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_ring_1_drive_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_ring_1_drive);

  position++;

  /* synth-0 ring-1 mix */
  fx_abyss_synth_audio->synth_0_ring_1_mix = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							  "specifier", "./synth-0-ring-1-mix[0]",
							  "control-port", "5/56",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_abyss_synth_audio->synth_0_ring_1_mix->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_ring_1_mix,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_ring_1_mix_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_ring_1_mix);

  position++;

  /* synth-0 ring-1 gain */
  fx_abyss_synth_audio->synth_0_ring_1_gain = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-ring-1-gain[0]",
							   "control-port", "5/56",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_ring_1_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_ring_1_gain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_ring_1_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_ring_1_gain);

  position++;

  /* synth-0 pitch tuning */
  fx_abyss_synth_audio->synth_0_pitch_tuning = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./pitch-tuning[0]",
							    "control-port", "11/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_pitch_tuning->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_pitch_tuning,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_pitch_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_pitch_tuning);

  position++;

  /* synth-0 volume */
  fx_abyss_synth_audio->synth_0_volume = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
						      "specifier", "./volume[0]",
						      "control-port", "12/56",
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_abyss_synth_audio->synth_0_volume->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_volume,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_volume);

  position++;

  /* synth-0 env-0 attack */
  fx_abyss_synth_audio->synth_0_env_0_attack = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-env-0-attack[0]",
							    "control-port", "13/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_env_0_attack->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_0_attack,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_0_attack_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_0_attack);

  position++;

  /* synth-0 env-0 decay */
  fx_abyss_synth_audio->synth_0_env_0_decay = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-env-0-decay[0]",
							   "control-port", "14/56",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_env_0_decay->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_0_decay,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_0_decay_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_0_decay);

  position++;

  /* synth-0 env-0 sustain */
  fx_abyss_synth_audio->synth_0_env_0_sustain = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							     "specifier", "./synth-0-env-0-sustain[0]",
							     "control-port", "15/56",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_abyss_synth_audio->synth_0_env_0_sustain->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_0_sustain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_0_sustain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_0_sustain);

  position++;

  /* synth-0 env-0 release */
  fx_abyss_synth_audio->synth_0_env_0_release = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							     "specifier", "./synth-0-env-0-release[0]",
							     "control-port", "16/56",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_abyss_synth_audio->synth_0_env_0_release->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_0_release,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_0_release_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_0_release);

  position++;

  /* synth-0 env-0 gain */
  fx_abyss_synth_audio->synth_0_env_0_gain = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							  "specifier", "./synth-0-env-0-gain[0]",
							  "control-port", "17/56",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_abyss_synth_audio->synth_0_env_0_gain->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_0_gain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_0_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_0_gain);

  position++;

  /* synth-0 env-0 frequency */
  fx_abyss_synth_audio->synth_0_env_0_frequency = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							       "specifier", "./synth-0-env-0-frequency[0]",
							       "control-port", "18/56",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_abyss_synth_audio->synth_0_env_0_frequency->port_value.ags_port_float = (gfloat) 6.0;

  g_object_set(fx_abyss_synth_audio->synth_0_env_0_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_0_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_0_frequency);

  position++;

  /* synth-0 env-0 sends */
  fx_abyss_synth_audio->synth_0_env_0_sends = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-env-0-sends[0]",
							   "control-port", "19/56",
							   "port-value-is-pointer", TRUE,
							   "port-value-type", G_TYPE_INT64,
							   "port-value-size", sizeof(gint64),
							   "port-value-length", AGS_ABYSS_SYNTH_SENDS_COUNT,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_env_0_sends->port_value.ags_port_pointer = (gint *) g_malloc(AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  memset(fx_abyss_synth_audio->synth_0_env_0_sends->port_value.ags_port_pointer, 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
  
  g_object_set(fx_abyss_synth_audio->synth_0_env_0_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_0_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_0_sends);

  position++;
  
  /* synth-0 env-1 attack */
  fx_abyss_synth_audio->synth_0_env_1_attack = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-env-1-attack[0]",
							    "control-port", "20/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_env_1_attack->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_1_attack,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_1_attack_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_1_attack);

  position++;

  /* synth-0 env-1 decay */
  fx_abyss_synth_audio->synth_0_env_1_decay = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-env-1-decay[0]",
							   "control-port", "21/56",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_env_1_decay->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_1_decay,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_1_decay_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_1_decay);

  position++;

  /* synth-0 env-1 sustain */
  fx_abyss_synth_audio->synth_0_env_1_sustain = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							     "specifier", "./synth-0-env-1-sustain[0]",
							     "control-port", "22/56",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_abyss_synth_audio->synth_0_env_1_sustain->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_1_sustain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_1_sustain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_1_sustain);

  position++;

  /* synth-0 env-1 release */
  fx_abyss_synth_audio->synth_0_env_1_release = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							     "specifier", "./synth-0-env-1-release[0]",
							     "control-port", "23/56",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_abyss_synth_audio->synth_0_env_1_release->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_1_release,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_1_release_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_1_release);

  position++;

  /* synth-0 env-1 gain */
  fx_abyss_synth_audio->synth_0_env_1_gain = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							  "specifier", "./synth-0-env-1-gain[0]",
							  "control-port", "24/56",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_abyss_synth_audio->synth_0_env_1_gain->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_1_gain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_1_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_1_gain);

  position++;

  /* synth-0 env-1 frequency */
  fx_abyss_synth_audio->synth_0_env_1_frequency = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							       "specifier", "./synth-0-env-1-frequency[0]",
							       "control-port", "25/56",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_abyss_synth_audio->synth_0_env_1_frequency->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_1_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_1_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_1_frequency);

  position++;

  /* synth-0 env-1 sends */
  fx_abyss_synth_audio->synth_0_env_1_sends = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-env-1-sends[0]",
							   "control-port", "26/56",
							   "port-value-is-pointer", TRUE,
							   "port-value-type", G_TYPE_INT64,
							   "port-value-size", sizeof(gint64),
							   "port-value-length", AGS_ABYSS_SYNTH_SENDS_COUNT,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_env_1_sends->port_value.ags_port_pointer = (gint *) g_malloc(AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  memset(fx_abyss_synth_audio->synth_0_env_1_sends->port_value.ags_port_pointer, 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  g_object_set(fx_abyss_synth_audio->synth_0_env_1_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_1_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_1_sends);

  position++;
  
  /* synth-0 env-2 attack */
  fx_abyss_synth_audio->synth_0_env_2_attack = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-env-2-attack[0]",
							    "control-port", "13/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_env_2_attack->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_2_attack,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_2_attack_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_2_attack);

  position++;

  /* synth-0 env-2 decay */
  fx_abyss_synth_audio->synth_0_env_2_decay = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-env-2-decay[0]",
							   "control-port", "14/56",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_env_2_decay->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_2_decay,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_2_decay_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_2_decay);

  position++;

  /* synth-0 env-2 sustain */
  fx_abyss_synth_audio->synth_0_env_2_sustain = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							     "specifier", "./synth-0-env-2-sustain[0]",
							     "control-port", "15/56",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_abyss_synth_audio->synth_0_env_2_sustain->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_2_sustain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_2_sustain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_2_sustain);

  position++;

  /* synth-0 env-2 release */
  fx_abyss_synth_audio->synth_0_env_2_release = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							     "specifier", "./synth-0-env-2-release[0]",
							     "control-port", "16/56",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_abyss_synth_audio->synth_0_env_2_release->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_2_release,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_2_release_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_2_release);

  position++;

  /* synth-0 env-2 gain */
  fx_abyss_synth_audio->synth_0_env_2_gain = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							  "specifier", "./synth-0-env-2-gain[0]",
							  "control-port", "17/56",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_abyss_synth_audio->synth_0_env_2_gain->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_2_gain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_2_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_2_gain);

  position++;

  /* synth-0 env-2 frequency */
  fx_abyss_synth_audio->synth_0_env_2_frequency = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							       "specifier", "./synth-0-env-2-frequency[0]",
							       "control-port", "18/56",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_abyss_synth_audio->synth_0_env_2_frequency->port_value.ags_port_float = (gfloat) 6.0;

  g_object_set(fx_abyss_synth_audio->synth_0_env_2_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_2_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_2_frequency);

  position++;

  /* synth-0 env-2 sends */
  fx_abyss_synth_audio->synth_0_env_2_sends = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-env-2-sends[0]",
							   "control-port", "19/56",
							   "port-value-is-pointer", TRUE,
							   "port-value-type", G_TYPE_INT64,
							   "port-value-size", sizeof(gint64),
							   "port-value-length", AGS_ABYSS_SYNTH_SENDS_COUNT,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_env_2_sends->port_value.ags_port_pointer = (gint *) g_malloc(AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  memset(fx_abyss_synth_audio->synth_0_env_2_sends->port_value.ags_port_pointer, 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
  
  g_object_set(fx_abyss_synth_audio->synth_0_env_2_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_2_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_2_sends);

  position++;

  /* synth-0 env-3 attack */
  fx_abyss_synth_audio->synth_0_env_3_attack = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-env-3-attack[0]",
							    "control-port", "13/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_env_3_attack->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_3_attack,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_3_attack_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_3_attack);

  position++;

  /* synth-0 env-3 decay */
  fx_abyss_synth_audio->synth_0_env_3_decay = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-env-3-decay[0]",
							   "control-port", "14/56",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_env_3_decay->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_3_decay,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_3_decay_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_3_decay);

  position++;

  /* synth-0 env-3 sustain */
  fx_abyss_synth_audio->synth_0_env_3_sustain = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							     "specifier", "./synth-0-env-3-sustain[0]",
							     "control-port", "15/56",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_abyss_synth_audio->synth_0_env_3_sustain->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_3_sustain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_3_sustain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_3_sustain);

  position++;

  /* synth-0 env-3 release */
  fx_abyss_synth_audio->synth_0_env_3_release = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							     "specifier", "./synth-0-env-3-release[0]",
							     "control-port", "16/56",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_abyss_synth_audio->synth_0_env_3_release->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_3_release,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_3_release_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_3_release);

  position++;

  /* synth-0 env-3 gain */
  fx_abyss_synth_audio->synth_0_env_3_gain = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							  "specifier", "./synth-0-env-3-gain[0]",
							  "control-port", "17/56",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_abyss_synth_audio->synth_0_env_3_gain->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_env_3_gain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_3_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_3_gain);

  position++;

  /* synth-0 env-3 frequency */
  fx_abyss_synth_audio->synth_0_env_3_frequency = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							       "specifier", "./synth-0-env-3-frequency[0]",
							       "control-port", "18/56",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_abyss_synth_audio->synth_0_env_3_frequency->port_value.ags_port_float = (gfloat) 6.0;

  g_object_set(fx_abyss_synth_audio->synth_0_env_3_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_3_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_3_frequency);

  position++;

  /* synth-0 env-3 sends */
  fx_abyss_synth_audio->synth_0_env_3_sends = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-env-3-sends[0]",
							   "control-port", "19/56",
							   "port-value-is-pointer", TRUE,
							   "port-value-type", G_TYPE_INT64,
							   "port-value-size", sizeof(gint64),
							   "port-value-length", AGS_ABYSS_SYNTH_SENDS_COUNT,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_env_3_sends->port_value.ags_port_pointer = (gint *) g_malloc(AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  memset(fx_abyss_synth_audio->synth_0_env_3_sends->port_value.ags_port_pointer, 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
  
  g_object_set(fx_abyss_synth_audio->synth_0_env_3_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_env_3_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_env_3_sends);

  position++;

  /* synth-0 lfo-0 oscillator */
  fx_abyss_synth_audio->synth_0_lfo_0_oscillator = g_object_new(AGS_TYPE_PORT,
								"plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								"specifier", "./synth-0-lfo-0-oscillator[0]",
								"control-port", "27/56",
								"port-value-is-pointer", FALSE,
								"port-value-type", G_TYPE_FLOAT,
								"port-value-size", sizeof(gfloat),
								"port-value-length", 1,
								NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_0_oscillator->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_0_oscillator,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_0_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_0_oscillator);

  position++;
  
  /* synth-0 lfo-0 frequency */
  fx_abyss_synth_audio->synth_0_lfo_0_frequency = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							       "specifier", "./synth-0-lfo-0-frequency[0]",
							       "control-port", "28/56",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_0_frequency->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_0_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_0_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_0_frequency);

  position++;
  
  /* synth-0 lfo-0 depth */
  fx_abyss_synth_audio->synth_0_lfo_0_depth = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-lfo-0-depth[0]",
							   "control-port", "29/56",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_0_depth->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_0_depth,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_0_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_0_depth);

  position++;
  
  /* synth-0 lfo-0 tuning */
  fx_abyss_synth_audio->synth_0_lfo_0_tuning = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-lfo-0-tuning[0]",
							    "control-port", "30/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_0_tuning->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_0_tuning,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_0_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_0_tuning);

  position++;
  
  /* synth-0 lfo-0 sends */
  fx_abyss_synth_audio->synth_0_lfo_0_sends = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-lfo-0-sends[0]",
							   "control-port", "31/56",
							   "port-value-is-pointer", TRUE,
							   "port-value-type", G_TYPE_INT64,
							   "port-value-size", sizeof(gint64),
							   "port-value-length", AGS_ABYSS_SYNTH_SENDS_COUNT,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_0_sends->port_value.ags_port_pointer = (gint64 *) g_malloc(AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  memset(fx_abyss_synth_audio->synth_0_lfo_0_sends->port_value.ags_port_pointer, 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_0_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_0_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_0_sends);

  position++;

  /* synth-0 lfo-1 oscillator */
  fx_abyss_synth_audio->synth_0_lfo_1_oscillator = g_object_new(AGS_TYPE_PORT,
								"plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								"specifier", "./synth-0-lfo-1-oscillator[0]",
								"control-port", "32/56",
								"port-value-is-pointer", FALSE,
								"port-value-type", G_TYPE_FLOAT,
								"port-value-size", sizeof(gfloat),
								"port-value-length", 1,
								NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_1_oscillator->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_1_oscillator,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_1_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_1_oscillator);

  position++;
  
  /* synth-0 lfo-1 frequency */
  fx_abyss_synth_audio->synth_0_lfo_1_frequency = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							       "specifier", "./synth-0-lfo-1-frequency[0]",
							       "control-port", "33/56",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_1_frequency->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_1_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_1_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_1_frequency);

  position++;
  
  /* synth-0 lfo-1 depth */
  fx_abyss_synth_audio->synth_0_lfo_1_depth = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-lfo-1-depth[0]",
							   "control-port", "34/56",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_1_depth->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_1_depth,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_1_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_1_depth);

  position++;
  
  /* synth-0 lfo-1 tuning */
  fx_abyss_synth_audio->synth_0_lfo_1_tuning = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-lfo-1-tuning[0]",
							    "control-port", "35/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_1_tuning->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_1_tuning,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_1_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_1_tuning);

  position++;
  
  /* synth-0 lfo-1 sends */
  fx_abyss_synth_audio->synth_0_lfo_1_sends = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-lfo-1-sends[0]",
							   "control-port", "36/56",
							   "port-value-is-pointer", TRUE,
							   "port-value-type", G_TYPE_INT64,
							   "port-value-size", sizeof(gint64),
							   "port-value-length", AGS_ABYSS_SYNTH_SENDS_COUNT,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_1_sends->port_value.ags_port_pointer = (gint64 *) g_malloc(AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  memset(fx_abyss_synth_audio->synth_0_lfo_1_sends->port_value.ags_port_pointer, 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_1_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_1_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_1_sends);

  position++;
  
  /* synth-0 lfo-2 oscillator */
  fx_abyss_synth_audio->synth_0_lfo_2_oscillator = g_object_new(AGS_TYPE_PORT,
								"plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								"specifier", "./synth-0-lfo-2-oscillator[0]",
								"control-port", "27/56",
								"port-value-is-pointer", FALSE,
								"port-value-type", G_TYPE_FLOAT,
								"port-value-size", sizeof(gfloat),
								"port-value-length", 1,
								NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_2_oscillator->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_2_oscillator,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_2_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_2_oscillator);

  position++;
  
  /* synth-0 lfo-2 frequency */
  fx_abyss_synth_audio->synth_0_lfo_2_frequency = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							       "specifier", "./synth-0-lfo-2-frequency[0]",
							       "control-port", "28/56",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_2_frequency->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_2_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_2_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_2_frequency);

  position++;
  
  /* synth-0 lfo-2 depth */
  fx_abyss_synth_audio->synth_0_lfo_2_depth = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-lfo-2-depth[0]",
							   "control-port", "29/56",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_2_depth->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_2_depth,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_2_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_2_depth);

  position++;
  
  /* synth-0 lfo-2 tuning */
  fx_abyss_synth_audio->synth_0_lfo_2_tuning = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-lfo-2-tuning[0]",
							    "control-port", "30/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_2_tuning->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_2_tuning,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_2_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_2_tuning);

  position++;
  
  /* synth-0 lfo-2 sends */
  fx_abyss_synth_audio->synth_0_lfo_2_sends = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-lfo-2-sends[0]",
							   "control-port", "31/56",
							   "port-value-is-pointer", TRUE,
							   "port-value-type", G_TYPE_INT64,
							   "port-value-size", sizeof(gint64),
							   "port-value-length", AGS_ABYSS_SYNTH_SENDS_COUNT,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_2_sends->port_value.ags_port_pointer = (gint64 *) g_malloc(AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  memset(fx_abyss_synth_audio->synth_0_lfo_2_sends->port_value.ags_port_pointer, 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_2_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_2_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_2_sends);

  position++;

  /* synth-0 lfo-3 oscillator */
  fx_abyss_synth_audio->synth_0_lfo_3_oscillator = g_object_new(AGS_TYPE_PORT,
								"plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								"specifier", "./synth-0-lfo-3-oscillator[0]",
								"control-port", "27/56",
								"port-value-is-pointer", FALSE,
								"port-value-type", G_TYPE_FLOAT,
								"port-value-size", sizeof(gfloat),
								"port-value-length", 1,
								NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_3_oscillator->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_3_oscillator,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_3_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_3_oscillator);

  position++;
  
  /* synth-0 lfo-3 frequency */
  fx_abyss_synth_audio->synth_0_lfo_3_frequency = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							       "specifier", "./synth-0-lfo-3-frequency[0]",
							       "control-port", "28/56",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_3_frequency->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_3_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_3_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_3_frequency);

  position++;
  
  /* synth-0 lfo-3 depth */
  fx_abyss_synth_audio->synth_0_lfo_3_depth = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-lfo-3-depth[0]",
							   "control-port", "29/56",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_3_depth->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_3_depth,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_3_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_3_depth);

  position++;
  
  /* synth-0 lfo-3 tuning */
  fx_abyss_synth_audio->synth_0_lfo_3_tuning = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-lfo-3-tuning[0]",
							    "control-port", "30/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_3_tuning->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_3_tuning,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_3_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_3_tuning);

  position++;
  
  /* synth-0 lfo-3 sends */
  fx_abyss_synth_audio->synth_0_lfo_3_sends = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-lfo-3-sends[0]",
							   "control-port", "31/56",
							   "port-value-is-pointer", TRUE,
							   "port-value-type", G_TYPE_INT64,
							   "port-value-size", sizeof(gint64),
							   "port-value-length", AGS_ABYSS_SYNTH_SENDS_COUNT,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_lfo_3_sends->port_value.ags_port_pointer = (gint64 *) g_malloc(AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  memset(fx_abyss_synth_audio->synth_0_lfo_3_sends->port_value.ags_port_pointer, 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  g_object_set(fx_abyss_synth_audio->synth_0_lfo_3_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_lfo_3_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_lfo_3_sends);

  position++;

  /* synth-0 seq modulation 0 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_0 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-0-modulation-0[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_0->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_0,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_0);

  position++;

  /* synth-0 seq modulation 1 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_1 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-0-modulation-1[0]",
							    "control-port", "17/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_1->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_1,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_1);

  position++;

  /* synth-0 seq modulation 2 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_2 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-0-modulation-2[0]",
							    "control-port", "18/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_2->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_2,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_2);

  position++;

  /* synth-0 seq modulation 3 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_3 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-0-modulation-3[0]",
							    "control-port", "19/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_3->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_3,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_3);

  position++;

  /* synth-0 seq modulation 4 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_4 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-0-modulation-4[0]",
							    "control-port", "20/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_4->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_4,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_4);

  position++;

  /* synth-0 seq modulation 5 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_5 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-0-modulation-5[0]",
							    "control-port", "21/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_5->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_5,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_5);

  position++;

  /* synth-0 seq modulation 6 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_6 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-0-modulation-6[0]",
							    "control-port", "22/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_6->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_6,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_6);

  position++;

  /* synth-0 seq modulation 7 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_7 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-0-modulation-7[0]",
							    "control-port", "23/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_7->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_7,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_7);

  position++;

  /* synth-0 seq modulation 8 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_8 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-0-modulation-8[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_8->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_8,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_8);

  position++;

  /* synth-0 seq modulation 9 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_9 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-0-modulation-9[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_9->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_9,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_9);

  position++;

  /* synth-0 seq modulation 10 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_10 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-0-modulation-10[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_10->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_10,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_10);

  position++;

  /* synth-0 seq modulation 11 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_11 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-0-modulation-11[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_11->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_11,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_11);

  position++;

  /* synth-0 seq modulation 12 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_12 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-0-modulation-12[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_12->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_12,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_12);

  position++;

  /* synth-0 seq modulation 13 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_13 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-0-modulation-13[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_13->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_13,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_13);

  position++;

  /* synth-0 seq modulation 14 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_14 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-0-modulation-14[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_14->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_14,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_14);

  position++;

  /* synth-0 seq modulation 15 */
  fx_abyss_synth_audio->synth_0_seq_0_modulation_15 = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							     "specifier", "./synth-0-seq-0-modulation-15[0]",
							     "control-port", "16/121", 
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_modulation_15->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_modulation_15,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_modulation_15);

  position++;

  /* synth-0 seq 0 pingpong */
  fx_abyss_synth_audio->synth_0_seq_0_pingpong = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							      "specifier", "./synth-0-seq-0-pingpong[0]",
							      "control-port", "24/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_pingpong->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_pingpong,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_pingpong_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_pingpong);

  position++;

  /* synth-0 seq 0 LFO frequency */
  fx_abyss_synth_audio->synth_0_seq_0_lfo_frequency = g_object_new(AGS_TYPE_PORT,
								   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								   "specifier", "./synth-0-seq-0-lfo-frequency[0]",
								   "control-port", "25/121",
								   "port-value-is-pointer", FALSE,
								   "port-value-type", G_TYPE_FLOAT,
								   "port-value-size", sizeof(gfloat),
								   "port-value-length", 1,
								   NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_lfo_frequency->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_lfo_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_lfo_frequency);

  position++;

  /* synth-0 seq-0 sends */
  fx_abyss_synth_audio->synth_0_seq_0_sends = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-seq-0-sends[0]",
							   "control-port", "31/56",
							   "port-value-is-pointer", TRUE,
							   "port-value-type", G_TYPE_INT64,
							   "port-value-size", sizeof(gint64),
							   "port-value-length", AGS_ABYSS_SYNTH_SENDS_COUNT,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_seq_0_sends->port_value.ags_port_pointer = (gint64 *) g_malloc(AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  memset(fx_abyss_synth_audio->synth_0_seq_0_sends->port_value.ags_port_pointer, 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  g_object_set(fx_abyss_synth_audio->synth_0_seq_0_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_0_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_0_sends);

  position++;

  /* synth-0 seq modulation 0 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_0 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-1-modulation-0[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_0->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_0,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_0);

  position++;

  /* synth-0 seq modulation 1 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_1 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-1-modulation-1[0]",
							    "control-port", "17/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_1->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_1,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_1);

  position++;

  /* synth-0 seq modulation 2 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_2 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-1-modulation-2[0]",
							    "control-port", "18/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_2->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_2,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_2);

  position++;

  /* synth-0 seq modulation 3 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_3 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-1-modulation-3[0]",
							    "control-port", "19/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_3->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_3,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_3);

  position++;

  /* synth-0 seq modulation 4 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_4 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-1-modulation-4[0]",
							    "control-port", "20/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_4->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_4,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_4);

  position++;

  /* synth-0 seq modulation 5 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_5 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-1-modulation-5[0]",
							    "control-port", "21/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_5->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_5,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_5);

  position++;

  /* synth-0 seq modulation 6 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_6 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-1-modulation-6[0]",
							    "control-port", "22/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_6->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_6,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_6);

  position++;

  /* synth-0 seq modulation 7 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_7 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-1-modulation-7[0]",
							    "control-port", "23/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_7->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_7,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_7);

  position++;

  /* synth-0 seq modulation 8 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_8 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-1-modulation-8[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_8->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_8,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_8);

  position++;

  /* synth-0 seq modulation 9 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_9 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-1-modulation-9[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_9->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_9,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_9);

  position++;

  /* synth-0 seq modulation 10 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_10 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-1-modulation-10[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_10->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_10,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_10);

  position++;

  /* synth-0 seq modulation 11 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_11 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-1-modulation-11[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_11->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_11,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_11);

  position++;

  /* synth-0 seq modulation 12 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_12 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-1-modulation-12[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_12->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_12,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_12);

  position++;

  /* synth-0 seq modulation 13 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_13 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-1-modulation-13[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_13->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_13,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_13);

  position++;

  /* synth-0 seq modulation 14 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_14 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-1-modulation-14[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_14->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_14,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_14);

  position++;

  /* synth-0 seq modulation 15 */
  fx_abyss_synth_audio->synth_0_seq_1_modulation_15 = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							     "specifier", "./synth-0-seq-1-modulation-15[0]",
							     "control-port", "16/121", 
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_modulation_15->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_modulation_15,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_modulation_15);

  position++;

  /* synth-0 seq 0 pingpong */
  fx_abyss_synth_audio->synth_0_seq_1_pingpong = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							      "specifier", "./synth-0-seq-1-pingpong[0]",
							      "control-port", "24/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_pingpong->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_pingpong,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_pingpong_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_pingpong);

  position++;

  /* synth-0 seq 0 LFO frequency */
  fx_abyss_synth_audio->synth_0_seq_1_lfo_frequency = g_object_new(AGS_TYPE_PORT,
								   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								   "specifier", "./synth-0-seq-1-lfo-frequency[0]",
								   "control-port", "25/121",
								   "port-value-is-pointer", FALSE,
								   "port-value-type", G_TYPE_FLOAT,
								   "port-value-size", sizeof(gfloat),
								   "port-value-length", 1,
								   NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_lfo_frequency->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_lfo_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_lfo_frequency);

  position++;

  /* synth-0 seq-1 sends */
  fx_abyss_synth_audio->synth_0_seq_1_sends = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-seq-1-sends[0]",
							   "control-port", "31/56",
							   "port-value-is-pointer", TRUE,
							   "port-value-type", G_TYPE_INT64,
							   "port-value-size", sizeof(gint64),
							   "port-value-length", AGS_ABYSS_SYNTH_SENDS_COUNT,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_seq_1_sends->port_value.ags_port_pointer = (gint64 *) g_malloc(AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  memset(fx_abyss_synth_audio->synth_0_seq_1_sends->port_value.ags_port_pointer, 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  g_object_set(fx_abyss_synth_audio->synth_0_seq_1_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_1_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_1_sends);

  position++;

  /* synth-0 seq modulation 0 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_0 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-2-modulation-0[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_0->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_0,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_0);

  position++;

  /* synth-0 seq modulation 1 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_1 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-2-modulation-1[0]",
							    "control-port", "17/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_1->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_1,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_1);

  position++;

  /* synth-0 seq modulation 2 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_2 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-2-modulation-2[0]",
							    "control-port", "18/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_2->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_2,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_2);

  position++;

  /* synth-0 seq modulation 3 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_3 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-2-modulation-3[0]",
							    "control-port", "19/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_3->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_3,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_3);

  position++;

  /* synth-0 seq modulation 4 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_4 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-2-modulation-4[0]",
							    "control-port", "20/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_4->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_4,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_4);

  position++;

  /* synth-0 seq modulation 5 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_5 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-2-modulation-5[0]",
							    "control-port", "21/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_5->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_5,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_5);

  position++;

  /* synth-0 seq modulation 6 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_6 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-2-modulation-6[0]",
							    "control-port", "22/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_6->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_6,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_6);

  position++;

  /* synth-0 seq modulation 7 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_7 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-2-modulation-7[0]",
							    "control-port", "23/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_7->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_7,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_7);

  position++;

  /* synth-0 seq modulation 8 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_8 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-2-modulation-8[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_8->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_8,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_8);

  position++;

  /* synth-0 seq modulation 9 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_9 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-2-modulation-9[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_9->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_9,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_9);

  position++;

  /* synth-0 seq modulation 10 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_10 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-2-modulation-10[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_10->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_10,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_10);

  position++;

  /* synth-0 seq modulation 11 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_11 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-2-modulation-11[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_11->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_11,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_11);

  position++;

  /* synth-0 seq modulation 12 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_12 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-2-modulation-12[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_12->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_12,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_12);

  position++;

  /* synth-0 seq modulation 13 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_13 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-2-modulation-13[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_13->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_13,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_13);

  position++;

  /* synth-0 seq modulation 14 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_14 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-2-modulation-14[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_14->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_14,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_14);

  position++;

  /* synth-0 seq modulation 15 */
  fx_abyss_synth_audio->synth_0_seq_2_modulation_15 = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							     "specifier", "./synth-0-seq-2-modulation-15[0]",
							     "control-port", "16/121", 
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_modulation_15->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_modulation_15,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_modulation_15);

  position++;

  /* synth-0 seq 0 pingpong */
  fx_abyss_synth_audio->synth_0_seq_2_pingpong = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							      "specifier", "./synth-0-seq-2-pingpong[0]",
							      "control-port", "24/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_pingpong->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_pingpong,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_pingpong_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_pingpong);

  position++;

  /* synth-0 seq 0 LFO frequency */
  fx_abyss_synth_audio->synth_0_seq_2_lfo_frequency = g_object_new(AGS_TYPE_PORT,
								   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								   "specifier", "./synth-0-seq-2-lfo-frequency[0]",
								   "control-port", "25/121",
								   "port-value-is-pointer", FALSE,
								   "port-value-type", G_TYPE_FLOAT,
								   "port-value-size", sizeof(gfloat),
								   "port-value-length", 1,
								   NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_lfo_frequency->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_lfo_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_lfo_frequency);

  position++;

  /* synth-0 seq-2 sends */
  fx_abyss_synth_audio->synth_0_seq_2_sends = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-seq-2-sends[0]",
							   "control-port", "31/56",
							   "port-value-is-pointer", TRUE,
							   "port-value-type", G_TYPE_INT64,
							   "port-value-size", sizeof(gint64),
							   "port-value-length", AGS_ABYSS_SYNTH_SENDS_COUNT,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_seq_2_sends->port_value.ags_port_pointer = (gint64 *) g_malloc(AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  memset(fx_abyss_synth_audio->synth_0_seq_2_sends->port_value.ags_port_pointer, 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  g_object_set(fx_abyss_synth_audio->synth_0_seq_2_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_2_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_2_sends);

  position++;

  /* synth-0 seq modulation 0 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_0 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-3-modulation-0[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_0->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_0,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_0);

  position++;

  /* synth-0 seq modulation 1 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_1 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-3-modulation-1[0]",
							    "control-port", "17/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_1->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_1,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_1);

  position++;

  /* synth-0 seq modulation 2 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_2 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-3-modulation-2[0]",
							    "control-port", "18/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_2->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_2,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_2);

  position++;

  /* synth-0 seq modulation 3 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_3 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-3-modulation-3[0]",
							    "control-port", "19/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_3->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_3,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_3);

  position++;

  /* synth-0 seq modulation 4 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_4 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-3-modulation-4[0]",
							    "control-port", "20/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_4->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_4,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_4);

  position++;

  /* synth-0 seq modulation 5 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_5 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-3-modulation-5[0]",
							    "control-port", "21/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_5->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_5,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_5);

  position++;

  /* synth-0 seq modulation 6 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_6 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-3-modulation-6[0]",
							    "control-port", "22/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_6->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_6,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_6);

  position++;

  /* synth-0 seq modulation 7 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_7 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-3-modulation-7[0]",
							    "control-port", "23/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_7->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_7,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_7);

  position++;

  /* synth-0 seq modulation 8 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_8 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-3-modulation-8[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_8->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_8,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_8);

  position++;

  /* synth-0 seq modulation 9 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_9 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-3-modulation-9[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_9->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_9,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_9);

  position++;

  /* synth-0 seq modulation 10 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_10 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-3-modulation-10[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_10->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_10,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_10);

  position++;

  /* synth-0 seq modulation 11 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_11 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-3-modulation-11[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_11->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_11,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_11);

  position++;

  /* synth-0 seq modulation 12 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_12 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-3-modulation-12[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_12->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_12,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_12);

  position++;

  /* synth-0 seq modulation 13 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_13 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-3-modulation-13[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_13->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_13,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_13);

  position++;

  /* synth-0 seq modulation 14 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_14 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-3-modulation-14[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_14->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_14,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_14);

  position++;

  /* synth-0 seq modulation 15 */
  fx_abyss_synth_audio->synth_0_seq_3_modulation_15 = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							     "specifier", "./synth-0-seq-3-modulation-15[0]",
							     "control-port", "16/121", 
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_modulation_15->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_modulation_15,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_modulation_15);

  position++;

  /* synth-0 seq 0 pingpong */
  fx_abyss_synth_audio->synth_0_seq_3_pingpong = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							      "specifier", "./synth-0-seq-3-pingpong[0]",
							      "control-port", "24/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_pingpong->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_pingpong,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_pingpong_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_pingpong);

  position++;

  /* synth-0 seq 0 LFO frequency */
  fx_abyss_synth_audio->synth_0_seq_3_lfo_frequency = g_object_new(AGS_TYPE_PORT,
								   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								   "specifier", "./synth-0-seq-3-lfo-frequency[0]",
								   "control-port", "25/121",
								   "port-value-is-pointer", FALSE,
								   "port-value-type", G_TYPE_FLOAT,
								   "port-value-size", sizeof(gfloat),
								   "port-value-length", 1,
								   NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_lfo_frequency->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_lfo_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_lfo_frequency);

  position++;

  /* synth-0 seq-3 sends */
  fx_abyss_synth_audio->synth_0_seq_3_sends = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-seq-3-sends[0]",
							   "control-port", "31/56",
							   "port-value-is-pointer", TRUE,
							   "port-value-type", G_TYPE_INT64,
							   "port-value-size", sizeof(gint64),
							   "port-value-length", AGS_ABYSS_SYNTH_SENDS_COUNT,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_seq_3_sends->port_value.ags_port_pointer = (gint64 *) g_malloc(AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  memset(fx_abyss_synth_audio->synth_0_seq_3_sends->port_value.ags_port_pointer, 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  g_object_set(fx_abyss_synth_audio->synth_0_seq_3_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_seq_3_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_seq_3_sends);

  position++;
  
  /* synth-0 pink noise 0 frequency */
  fx_abyss_synth_audio->synth_0_pink_noise_0_frequency = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							       "specifier", "./synth-0-pink-noise-0-frequency[0]",
							       "control-port", "37/56",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_abyss_synth_audio->synth_0_pink_noise_0_frequency->port_value.ags_port_float = (gfloat) 220.0;

  g_object_set(fx_abyss_synth_audio->synth_0_pink_noise_0_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_pink_noise_0_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_pink_noise_0_frequency);

  position++;

  /* synth-0 pink noise 0 gain */
  fx_abyss_synth_audio->synth_0_pink_noise_0_gain = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							  "specifier", "./synth-0-pink-noise-0-gain[0]",
							  "control-port", "38/56",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_abyss_synth_audio->synth_0_pink_noise_0_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_pink_noise_0_gain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_pink_noise_0_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_pink_noise_0_gain);

  position++;

  /* synth-0 pink noise 0 sends */
  fx_abyss_synth_audio->synth_0_pink_noise_0_sends = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-pink-noise-0-sends[0]",
							   "control-port", "39/56",
							   "port-value-is-pointer", TRUE,
							   "port-value-type", G_TYPE_INT64,
							   "port-value-size", sizeof(gint64),
							   "port-value-length", AGS_ABYSS_SYNTH_SENDS_COUNT,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_pink_noise_0_sends->port_value.ags_port_pointer = (gint *) g_malloc(AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  memset(fx_abyss_synth_audio->synth_0_pink_noise_0_sends->port_value.ags_port_pointer, 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  g_object_set(fx_abyss_synth_audio->synth_0_pink_noise_0_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_pink_noise_0_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_pink_noise_0_sends);

  position++;
  
  /* synth-0 pink noise 1 frequency */
  fx_abyss_synth_audio->synth_0_pink_noise_1_frequency = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							       "specifier", "./synth-0-pink-noise-1-frequency[0]",
							       "control-port", "37/56",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_abyss_synth_audio->synth_0_pink_noise_1_frequency->port_value.ags_port_float = (gfloat) 220.0;

  g_object_set(fx_abyss_synth_audio->synth_0_pink_noise_1_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_pink_noise_1_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_pink_noise_1_frequency);

  position++;

  /* synth-0 pink noise 1 gain */
  fx_abyss_synth_audio->synth_0_pink_noise_1_gain = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							  "specifier", "./synth-0-pink-noise-1-gain[0]",
							  "control-port", "38/56",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_abyss_synth_audio->synth_0_pink_noise_1_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->synth_0_pink_noise_1_gain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_pink_noise_1_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_pink_noise_1_gain);

  position++;

  /* synth-0 pink noise 1 sends */
  fx_abyss_synth_audio->synth_0_pink_noise_1_sends = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./synth-0-pink-noise-1-sends[0]",
							   "control-port", "39/56",
							   "port-value-is-pointer", TRUE,
							   "port-value-type", G_TYPE_INT64,
							   "port-value-size", sizeof(gint64),
							   "port-value-length", AGS_ABYSS_SYNTH_SENDS_COUNT,
							   NULL);
  
  fx_abyss_synth_audio->synth_0_pink_noise_1_sends->port_value.ags_port_pointer = (gint *) g_malloc(AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  memset(fx_abyss_synth_audio->synth_0_pink_noise_1_sends->port_value.ags_port_pointer, 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

  g_object_set(fx_abyss_synth_audio->synth_0_pink_noise_1_sends,
	       "plugin-port", ags_fx_abyss_synth_audio_get_synth_0_pink_noise_1_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->synth_0_pink_noise_1_sends);

  position++;
  
  /* low-pass 0 cut off frequency */
  fx_abyss_synth_audio->low_pass_0_cut_off_frequency = g_object_new(AGS_TYPE_PORT,
								    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								    "specifier", "./low-pass-0-cut-off-frequency[0]",
								    "control-port", "40/56",
								    "port-value-is-pointer", FALSE,
								    "port-value-type", G_TYPE_FLOAT,
								    "port-value-size", sizeof(gfloat),
								    "port-value-length", 1,
								    NULL);
  
  fx_abyss_synth_audio->low_pass_0_cut_off_frequency->port_value.ags_port_float = (gfloat) 2000.0;

  g_object_set(fx_abyss_synth_audio->low_pass_0_cut_off_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_low_pass_0_cut_off_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->low_pass_0_cut_off_frequency);

  position++;
  
  /* low-pass 0 filter gain */
  fx_abyss_synth_audio->low_pass_0_filter_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							      "specifier", "./low-pass-0-filter-gain[0]",
							      "control-port", "41/56",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_abyss_synth_audio->low_pass_0_filter_gain->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->low_pass_0_filter_gain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_low_pass_0_filter_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->low_pass_0_filter_gain);

  position++;
  
  /* low-pass 0 no-clip */
  fx_abyss_synth_audio->low_pass_0_no_clip = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							  "specifier", "./low-pass-0-no-clip[0]",
							  "control-port", "42/56",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_abyss_synth_audio->low_pass_0_no_clip->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->low_pass_0_no_clip,
	       "plugin-port", ags_fx_abyss_synth_audio_get_low_pass_0_no_clip_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->low_pass_0_no_clip);

  position++;

  /* low-pass 1 cut off frequency */
  fx_abyss_synth_audio->low_pass_1_cut_off_frequency = g_object_new(AGS_TYPE_PORT,
								    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
								    "specifier", "./low-pass-1-cut-off-frequency[0]",
								    "control-port", "40/56",
								    "port-value-is-pointer", FALSE,
								    "port-value-type", G_TYPE_FLOAT,
								    "port-value-size", sizeof(gfloat),
								    "port-value-length", 1,
								    NULL);
  
  fx_abyss_synth_audio->low_pass_1_cut_off_frequency->port_value.ags_port_float = (gfloat) 2000.0;

  g_object_set(fx_abyss_synth_audio->low_pass_1_cut_off_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_low_pass_1_cut_off_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->low_pass_1_cut_off_frequency);

  position++;
  
  /* low-pass 1 filter gain */
  fx_abyss_synth_audio->low_pass_1_filter_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							      "specifier", "./low-pass-1-filter-gain[0]",
							      "control-port", "41/56",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_abyss_synth_audio->low_pass_1_filter_gain->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->low_pass_1_filter_gain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_low_pass_1_filter_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->low_pass_1_filter_gain);

  position++;
  
  /* low-pass 1 no-clip */
  fx_abyss_synth_audio->low_pass_1_no_clip = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							  "specifier", "./low-pass-1-no-clip[0]",
							  "control-port", "42/56",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_abyss_synth_audio->low_pass_1_no_clip->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->low_pass_1_no_clip,
	       "plugin-port", ags_fx_abyss_synth_audio_get_low_pass_1_no_clip_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->low_pass_1_no_clip);

  position++;

  /* amplifier 0 amp-0 gain */
  fx_abyss_synth_audio->amplifier_0_amp_0_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							      "specifier", "./amplifier-0-amp-0-gain[0]",
							      "control-port", "43/56",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_abyss_synth_audio->amplifier_0_amp_0_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->amplifier_0_amp_0_gain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_amplifier_amp_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->amplifier_0_amp_0_gain);

  position++;

  /* amplifier 0 amp-1 gain */
  fx_abyss_synth_audio->amplifier_0_amp_1_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							      "specifier", "./amplifier-0-amp-1-gain[0]",
							      "control-port", "44/56",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_abyss_synth_audio->amplifier_0_amp_1_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->amplifier_0_amp_1_gain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_amplifier_amp_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->amplifier_0_amp_1_gain);

  position++;

  /* amplifier 0 amp-2 gain */
  fx_abyss_synth_audio->amplifier_0_amp_2_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							      "specifier", "./amplifier-0-amp-2-gain[0]",
							      "control-port", "45/56",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_abyss_synth_audio->amplifier_0_amp_2_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->amplifier_0_amp_2_gain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_amplifier_amp_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->amplifier_0_amp_2_gain);

  position++;

  /* amplifier 0 amp-3 gain */
  fx_abyss_synth_audio->amplifier_0_amp_3_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							      "specifier", "./amplifier-0-amp-3-gain[0]",
							      "control-port", "46/56",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_abyss_synth_audio->amplifier_0_amp_3_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->amplifier_0_amp_3_gain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_amplifier_amp_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->amplifier_0_amp_3_gain);

  position++;

  /* amplifier 0 filter gain */
  fx_abyss_synth_audio->amplifier_0_filter_gain = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							       "specifier", "./amplifier-0-filter-gain[0]",
							       "control-port", "47/56",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_abyss_synth_audio->amplifier_0_filter_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->amplifier_0_filter_gain,
	       "plugin-port", ags_fx_abyss_synth_audio_get_amplifier_filter_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->amplifier_0_filter_gain);

  position++;

  /* chorus enabled */
  fx_abyss_synth_audio->chorus_enabled = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
						      "specifier", "./chorus-enabled[0]",
						      "control-port", "48/56",
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_abyss_synth_audio->chorus_enabled->port_value.ags_port_float = (gfloat) TRUE;

  g_object_set(fx_abyss_synth_audio->chorus_enabled,
	       "plugin-port", ags_fx_abyss_synth_audio_get_chorus_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->chorus_enabled);

  position++;

  /* chorus pitch type */
  fx_abyss_synth_audio->chorus_pitch_type = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							 "specifier", "./chorus-pitch-type[0]",
							 "control-port", "49/56",
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_abyss_synth_audio->chorus_pitch_type->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->chorus_pitch_type,
	       "plugin-port", ags_fx_abyss_synth_audio_get_chorus_pitch_type_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->chorus_pitch_type);

  position++;

  /* chorus input volume */
  fx_abyss_synth_audio->chorus_input_volume = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							   "specifier", "./chorus-input-volume[0]",
							   "control-port", "50/56",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_abyss_synth_audio->chorus_input_volume->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->chorus_input_volume,
	       "plugin-port", ags_fx_abyss_synth_audio_get_chorus_input_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->chorus_input_volume);

  position++;

  /* chorus output volume */
  fx_abyss_synth_audio->chorus_output_volume = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./chorus-output-volume[0]",
							    "control-port", "51/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->chorus_output_volume->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_abyss_synth_audio->chorus_output_volume,
	       "plugin-port", ags_fx_abyss_synth_audio_get_chorus_output_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->chorus_output_volume);

  position++;

  /* chorus LFO oscillator */
  fx_abyss_synth_audio->chorus_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							     "specifier", "./chorus-lfo-oscillator[0]",
							     "control-port", "52/56",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_abyss_synth_audio->chorus_lfo_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_abyss_synth_audio->chorus_lfo_oscillator,
	       "plugin-port", ags_fx_abyss_synth_audio_get_chorus_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->chorus_lfo_oscillator);

  position++;

  /* chorus LFO frequency */
  fx_abyss_synth_audio->chorus_lfo_frequency = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
							    "specifier", "./chorus-lfo-frequency[0]",
							    "control-port", "53/56",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_abyss_synth_audio->chorus_lfo_frequency->port_value.ags_port_float = (gfloat) 0.01;

  g_object_set(fx_abyss_synth_audio->chorus_lfo_frequency,
	       "plugin-port", ags_fx_abyss_synth_audio_get_chorus_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->chorus_lfo_frequency);

  position++;

  /* chorus depth */
  fx_abyss_synth_audio->chorus_depth = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
						    "specifier", "./chorus-depth[0]",
						    "control-port", "54/56",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_abyss_synth_audio->chorus_depth->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->chorus_depth,
	       "plugin-port", ags_fx_abyss_synth_audio_get_chorus_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->chorus_depth);

  position++;

  /* chorus mix */
  fx_abyss_synth_audio->chorus_mix = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
						  "specifier", "./chorus-mix[0]",
						  "control-port", "55/56",
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 1,
						  NULL);
  
  fx_abyss_synth_audio->chorus_mix->port_value.ags_port_float = (gfloat) 0.5;

  g_object_set(fx_abyss_synth_audio->chorus_mix,
	       "plugin-port", ags_fx_abyss_synth_audio_get_chorus_mix_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->chorus_mix);

  position++;

  /* chorus delay */
  fx_abyss_synth_audio->chorus_delay = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_abyss_synth_audio_plugin_name,
						    "specifier", "./chorus-delay[0]",
						    "control-port", "56/56",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_abyss_synth_audio->chorus_delay->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_abyss_synth_audio->chorus_delay,
	       "plugin-port", ags_fx_abyss_synth_audio_get_chorus_delay_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_abyss_synth_audio,
		      fx_abyss_synth_audio->chorus_delay);

  position++;

  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      fx_abyss_synth_audio->scope_data[i] = ags_fx_abyss_synth_audio_scope_data_alloc();
      
      fx_abyss_synth_audio->scope_data[i]->parent = fx_abyss_synth_audio;
    }else{
      fx_abyss_synth_audio->scope_data[i] = NULL;
    }
  }
}

void
ags_fx_abyss_synth_audio_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec)
{
  AgsFxAbyssSynthAudio *fx_abyss_synth_audio;

  GRecMutex *recall_mutex;

  fx_abyss_synth_audio = AGS_FX_ABYSS_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_abyss_synth_audio);

  switch(prop_id){
  case PROP_SYNTH_0_OSC_0_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_0_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_0_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_0_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_0_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_OCTAVE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_0_octave){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_0_octave != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_0_octave));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_0_octave = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_KEY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_0_key){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_0_key != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_0_key));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_0_key = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_PHASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_0_phase){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_0_phase != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_0_phase));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_0_phase = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_0_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_0_volume != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_0_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_0_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_LOW_PASS_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_0_low_pass_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_0_low_pass_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_0_low_pass_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_0_low_pass_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_1_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_1_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_1_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_1_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_OCTAVE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_1_octave){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_1_octave != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_1_octave));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_1_octave = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_KEY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_1_key){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_1_key != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_1_key));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_1_key = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_PHASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_1_phase){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_1_phase != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_1_phase));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_1_phase = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_1_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_1_volume != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_1_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_1_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_LOW_PASS_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_1_low_pass_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_1_low_pass_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_1_low_pass_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_1_low_pass_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_2_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_2_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_2_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_2_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_2_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_2_OCTAVE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_2_octave){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_2_octave != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_2_octave));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_2_octave = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_2_KEY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_2_key){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_2_key != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_2_key));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_2_key = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_2_PHASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_2_phase){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_2_phase != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_2_phase));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_2_phase = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_2_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_2_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_2_volume != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_2_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_2_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_2_LOW_PASS_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_2_low_pass_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_2_low_pass_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_2_low_pass_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_2_low_pass_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_3_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_3_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_3_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_3_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_3_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_3_OCTAVE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_3_octave){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_3_octave != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_3_octave));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_3_octave = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_3_KEY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_3_key){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_3_key != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_3_key));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_3_key = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_3_PHASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_3_phase){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_3_phase != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_3_phase));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_3_phase = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_3_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_3_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_3_volume != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_3_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_3_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_3_LOW_PASS_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_osc_3_low_pass_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_osc_3_low_pass_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_osc_3_low_pass_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_osc_3_low_pass_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_0_ENABLED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_ring_0_enabled){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_ring_0_enabled != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_ring_0_enabled));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_ring_0_enabled = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_0_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_ring_0_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_ring_0_tuning != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_ring_0_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_ring_0_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_0_DRIVE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_ring_0_drive){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_ring_0_drive != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_ring_0_drive));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_ring_0_drive = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_0_MIX:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_ring_0_mix){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_ring_0_mix != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_ring_0_mix));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_ring_0_mix = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_0_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_ring_0_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_ring_0_gain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_ring_0_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_ring_0_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_1_ENABLED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_ring_1_enabled){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_ring_1_enabled != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_ring_1_enabled));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_ring_1_enabled = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_1_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_ring_1_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_ring_1_tuning != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_ring_1_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_ring_1_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_1_DRIVE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_ring_1_drive){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_ring_1_drive != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_ring_1_drive));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_ring_1_drive = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_1_MIX:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_ring_1_mix){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_ring_1_mix != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_ring_1_mix));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_ring_1_mix = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_1_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_ring_1_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_ring_1_gain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_ring_1_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_ring_1_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PITCH_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_pitch_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_pitch_tuning != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_pitch_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_pitch_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_volume != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_ATTACK:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_0_attack){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_0_attack != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_0_attack));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_0_attack = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_DECAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_0_decay){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_0_decay != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_0_decay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_0_decay = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_SUSTAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_0_sustain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_0_sustain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_0_sustain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_0_sustain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_RELEASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_0_release){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_0_release != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_0_release));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_0_release = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_0_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_0_gain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_0_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_0_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_0_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_0_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_0_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_0_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_0_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_0_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_0_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_0_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_ATTACK:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_1_attack){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_1_attack != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_1_attack));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_1_attack = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_DECAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_1_decay){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_1_decay != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_1_decay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_1_decay = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_SUSTAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_1_sustain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_1_sustain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_1_sustain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_1_sustain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_RELEASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_1_release){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_1_release != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_1_release));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_1_release = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_1_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_1_gain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_1_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_1_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_1_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_1_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_1_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_1_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_1_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_1_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_1_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_1_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_2_ATTACK:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_2_attack){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_2_attack != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_2_attack));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_2_attack = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_2_DECAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_2_decay){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_2_decay != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_2_decay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_2_decay = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_2_SUSTAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_2_sustain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_2_sustain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_2_sustain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_2_sustain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_2_RELEASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_2_release){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_2_release != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_2_release));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_2_release = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_2_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_2_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_2_gain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_2_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_2_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_2_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_2_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_2_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_2_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_2_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_2_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_2_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_2_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_2_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_2_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_3_ATTACK:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_3_attack){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_3_attack != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_3_attack));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_3_attack = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_3_DECAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_3_decay){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_3_decay != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_3_decay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_3_decay = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_3_SUSTAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_3_sustain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_3_sustain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_3_sustain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_3_sustain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_3_RELEASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_3_release){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_3_release != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_3_release));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_3_release = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_3_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_3_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_3_gain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_3_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_3_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_3_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_3_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_3_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_3_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_3_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_3_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_env_3_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_env_3_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_env_3_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_env_3_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_0_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_0_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_0_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_0_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_0_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_0_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_0_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_0_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_0_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_0_depth != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_0_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_0_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_0_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_0_tuning != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_0_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_0_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_0_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_0_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_0_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_0_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_1_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_1_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_1_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_1_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_1_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_1_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_1_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_1_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_1_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_1_depth != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_1_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_1_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_1_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_1_tuning != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_1_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_1_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_1_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_1_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_1_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_1_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_2_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_2_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_2_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_2_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_2_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_2_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_2_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_2_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_2_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_2_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_2_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_2_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_2_depth != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_2_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_2_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_2_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_2_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_2_tuning != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_2_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_2_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_2_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_2_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_2_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_2_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_2_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_3_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_3_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_3_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_3_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_3_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_3_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_3_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_3_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_3_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_3_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_3_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_3_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_3_depth != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_3_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_3_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_3_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_3_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_3_tuning != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_3_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_3_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_3_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_lfo_3_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_lfo_3_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_lfo_3_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_lfo_3_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_0 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_1 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_2 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_3 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_4:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_4){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_4 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_4));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_4 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_5:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_5){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_5 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_5));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_5 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_6:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_6){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_6 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_6));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_6 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_7:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_7){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_7 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_7));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_7 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_8:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_8){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_8 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_8));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_8 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_9:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_9){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_9 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_9));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_9 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_10:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_10){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_10 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_10));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_10 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_11:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_11){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_11 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_11));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_11 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_12:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_12){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_12 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_12));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_12 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_13:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_13){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_13 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_13));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_13 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_14:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_14){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_14 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_14));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_14 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_15:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_modulation_15){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_modulation_15 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_modulation_15));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_modulation_15 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_PINGPONG:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_pingpong){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_pingpong != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_pingpong));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_pingpong = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_0_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_0_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_0_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_0_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_0 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_1 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_2 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_3 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_4:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_4){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_4 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_4));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_4 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_5:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_5){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_5 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_5));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_5 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_6:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_6){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_6 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_6));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_6 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_7:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_7){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_7 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_7));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_7 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_8:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_8){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_8 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_8));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_8 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_9:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_9){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_9 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_9));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_9 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_10:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_10){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_10 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_10));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_10 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_11:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_11){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_11 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_11));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_11 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_12:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_12){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_12 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_12));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_12 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_13:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_13){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_13 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_13));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_13 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_14:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_14){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_14 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_14));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_14 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_15:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_modulation_15){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_modulation_15 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_modulation_15));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_modulation_15 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_PINGPONG:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_pingpong){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_pingpong != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_pingpong));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_pingpong = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_1_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_1_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_1_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_1_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_0 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_1 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_2 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_3 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_4:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_4){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_4 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_4));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_4 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_5:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_5){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_5 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_5));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_5 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_6:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_6){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_6 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_6));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_6 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_7:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_7){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_7 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_7));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_7 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_8:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_8){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_8 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_8));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_8 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_9:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_9){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_9 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_9));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_9 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_10:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_10){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_10 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_10));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_10 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_11:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_11){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_11 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_11));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_11 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_12:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_12){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_12 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_12));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_12 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_13:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_13){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_13 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_13));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_13 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_14:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_14){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_14 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_14));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_14 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_15:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_modulation_15){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_modulation_15 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_modulation_15));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_modulation_15 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_PINGPONG:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_pingpong){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_pingpong != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_pingpong));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_pingpong = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_2_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_2_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_2_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_2_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_0 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_1 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_2 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_3 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_4:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_4){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_4 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_4));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_4 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_5:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_5){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_5 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_5));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_5 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_6:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_6){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_6 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_6));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_6 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_7:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_7){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_7 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_7));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_7 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_8:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_8){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_8 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_8));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_8 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_9:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_9){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_9 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_9));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_9 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_10:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_10){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_10 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_10));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_10 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_11:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_11){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_11 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_11));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_11 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_12:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_12){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_12 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_12));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_12 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_13:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_13){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_13 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_13));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_13 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_14:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_14){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_14 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_14));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_14 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_15:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_modulation_15){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_modulation_15 != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_modulation_15));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_modulation_15 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_PINGPONG:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_pingpong){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_pingpong != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_pingpong));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_pingpong = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_seq_3_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_seq_3_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_seq_3_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_seq_3_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PINK_NOISE_0_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_pink_noise_0_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_pink_noise_0_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_pink_noise_0_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_pink_noise_0_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PINK_NOISE_0_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_pink_noise_0_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_pink_noise_0_gain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_pink_noise_0_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_pink_noise_0_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PINK_NOISE_0_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_pink_noise_0_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_pink_noise_0_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_pink_noise_0_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_pink_noise_0_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PINK_NOISE_1_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_pink_noise_1_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_pink_noise_1_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_pink_noise_1_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_pink_noise_1_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PINK_NOISE_1_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_pink_noise_1_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_pink_noise_1_gain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_pink_noise_1_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_pink_noise_1_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PINK_NOISE_1_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->synth_0_pink_noise_1_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->synth_0_pink_noise_1_sends != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->synth_0_pink_noise_1_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->synth_0_pink_noise_1_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_0_CUT_OFF_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->low_pass_0_cut_off_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->low_pass_0_cut_off_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->low_pass_0_cut_off_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->low_pass_0_cut_off_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_0_FILTER_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->low_pass_0_filter_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->low_pass_0_filter_gain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->low_pass_0_filter_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->low_pass_0_filter_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_0_NO_CLIP:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->low_pass_0_no_clip){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->low_pass_0_no_clip != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->low_pass_0_no_clip));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->low_pass_0_no_clip = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_1_CUT_OFF_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->low_pass_1_cut_off_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->low_pass_1_cut_off_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->low_pass_1_cut_off_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->low_pass_1_cut_off_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_1_FILTER_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->low_pass_1_filter_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->low_pass_1_filter_gain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->low_pass_1_filter_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->low_pass_1_filter_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_1_NO_CLIP:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->low_pass_1_no_clip){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->low_pass_1_no_clip != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->low_pass_1_no_clip));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->low_pass_1_no_clip = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_0_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->amplifier_0_amp_0_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->amplifier_0_amp_0_gain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->amplifier_0_amp_0_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->amplifier_0_amp_0_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_1_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->amplifier_0_amp_1_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->amplifier_0_amp_1_gain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->amplifier_0_amp_1_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->amplifier_0_amp_1_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_2_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->amplifier_0_amp_2_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->amplifier_0_amp_2_gain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->amplifier_0_amp_2_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->amplifier_0_amp_2_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_3_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->amplifier_0_amp_3_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->amplifier_0_amp_3_gain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->amplifier_0_amp_3_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->amplifier_0_amp_3_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_FILTER_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->amplifier_0_filter_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->amplifier_0_filter_gain != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->amplifier_0_filter_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->amplifier_0_filter_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_ENABLED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->chorus_enabled){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->chorus_enabled != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->chorus_enabled));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->chorus_enabled = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_PITCH_TYPE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->chorus_pitch_type){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->chorus_pitch_type != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->chorus_pitch_type));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->chorus_pitch_type = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_INPUT_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->chorus_input_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->chorus_input_volume != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->chorus_input_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->chorus_input_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_OUTPUT_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->chorus_output_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->chorus_output_volume != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->chorus_output_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->chorus_output_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_LFO_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->chorus_lfo_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->chorus_lfo_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->chorus_lfo_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->chorus_lfo_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->chorus_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->chorus_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->chorus_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->chorus_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->chorus_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->chorus_depth != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->chorus_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->chorus_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_MIX:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->chorus_mix){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->chorus_mix != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->chorus_mix));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->chorus_mix = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_DELAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_abyss_synth_audio->chorus_delay){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_abyss_synth_audio->chorus_delay != NULL){
	g_object_unref(G_OBJECT(fx_abyss_synth_audio->chorus_delay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_abyss_synth_audio->chorus_delay = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_abyss_synth_audio_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec)
{
  AgsFxAbyssSynthAudio *fx_abyss_synth_audio;

  GRecMutex *recall_mutex;

  fx_abyss_synth_audio = AGS_FX_ABYSS_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_abyss_synth_audio);

  switch(prop_id){
  case PROP_SYNTH_0_OSC_0_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_0_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_OCTAVE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_0_octave);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_KEY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_0_key);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_PHASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_0_phase);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_0_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_LOW_PASS_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_0_low_pass_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_1_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_OCTAVE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_1_octave);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_KEY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_1_key);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_PHASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_1_phase);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_1_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_LOW_PASS_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_1_low_pass_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_2_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_2_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_2_OCTAVE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_2_octave);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_2_KEY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_2_key);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_2_PHASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_2_phase);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_2_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_2_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_2_LOW_PASS_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_2_low_pass_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_3_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_3_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_3_OCTAVE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_3_octave);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_3_KEY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_3_key);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_3_PHASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_3_phase);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_3_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_3_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_3_LOW_PASS_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_osc_3_low_pass_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_0_ENABLED:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_ring_0_enabled);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_0_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_ring_0_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_0_DRIVE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_ring_0_drive);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_0_MIX:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_ring_0_mix);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_0_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_ring_0_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_1_ENABLED:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_ring_1_enabled);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_1_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_ring_1_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_1_DRIVE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_ring_1_drive);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_1_MIX:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_ring_1_mix);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_RING_1_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_ring_1_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PITCH_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_pitch_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_0_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_0_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_0_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_0_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_0_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_1_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_1_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_1_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_1_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_1_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_2_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_2_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_2_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_2_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_2_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_2_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_2_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_2_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_2_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_2_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_3_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_3_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_3_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_3_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_3_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_3_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_3_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_3_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_3_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_lfo_3_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_ATTACK:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_0_attack);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_DECAY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_0_decay);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_SUSTAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_0_sustain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_RELEASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_0_release);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_0_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_0_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_0_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_ATTACK:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_1_attack);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_DECAY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_1_decay);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_SUSTAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_1_sustain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_RELEASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_1_release);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_1_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_1_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_1_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_2_ATTACK:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_2_attack);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_2_DECAY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_2_decay);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_2_SUSTAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_2_sustain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_2_RELEASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_2_release);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_2_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_2_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_2_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_2_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_2_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_2_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_3_ATTACK:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_3_attack);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_3_DECAY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_3_decay);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_3_SUSTAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_3_sustain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_3_RELEASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_3_release);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_3_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_3_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_3_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_3_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_3_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_env_3_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_4:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_4);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_5:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_5);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_6:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_6);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_7:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_7);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_8:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_8);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_9:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_9);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_10:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_10);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_11:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_11);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_12:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_12);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_13:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_13);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_14:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_14);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_MODULATION_15:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_modulation_15);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_PINGPONG:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_pingpong);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_0_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_0_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_4:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_4);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_5:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_5);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_6:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_6);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_7:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_7);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_8:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_8);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_9:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_9);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_10:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_10);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_11:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_11);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_12:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_12);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_13:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_13);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_14:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_14);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_MODULATION_15:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_modulation_15);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_PINGPONG:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_pingpong);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_1_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_1_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_4:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_4);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_5:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_5);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_6:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_6);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_7:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_7);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_8:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_8);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_9:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_9);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_10:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_10);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_11:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_11);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_12:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_12);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_13:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_13);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_14:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_14);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_MODULATION_15:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_modulation_15);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_PINGPONG:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_pingpong);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_2_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_2_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_4:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_4);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_5:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_5);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_6:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_6);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_7:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_7);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_8:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_8);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_9:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_9);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_10:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_10);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_11:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_11);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_12:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_12);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_13:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_13);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_14:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_14);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_MODULATION_15:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_modulation_15);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_PINGPONG:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_pingpong);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_3_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_seq_3_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PINK_NOISE_0_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_pink_noise_0_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PINK_NOISE_0_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_pink_noise_0_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PINK_NOISE_0_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_pink_noise_0_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PINK_NOISE_1_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_pink_noise_1_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PINK_NOISE_1_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_pink_noise_1_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PINK_NOISE_1_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->synth_0_pink_noise_1_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_0_CUT_OFF_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->low_pass_0_cut_off_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_0_FILTER_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->low_pass_0_filter_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_0_NO_CLIP:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->low_pass_0_no_clip);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_1_CUT_OFF_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->low_pass_1_cut_off_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_1_FILTER_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->low_pass_1_filter_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_1_NO_CLIP:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->low_pass_1_no_clip);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_0_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->amplifier_0_amp_0_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_1_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->amplifier_0_amp_1_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_2_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->amplifier_0_amp_2_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_3_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->amplifier_0_amp_3_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_FILTER_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->amplifier_0_filter_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_ENABLED:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->chorus_enabled);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_PITCH_TYPE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->chorus_pitch_type);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_INPUT_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->chorus_input_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_OUTPUT_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->chorus_output_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_LFO_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->chorus_lfo_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->chorus_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->chorus_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_MIX:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->chorus_mix);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_DELAY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_abyss_synth_audio->chorus_delay);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_abyss_synth_audio_dispose(GObject *gobject)
{
  AgsFxAbyssSynthAudio *fx_abyss_synth_audio;
  
  fx_abyss_synth_audio = AGS_FX_ABYSS_SYNTH_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_abyss_synth_audio_parent_class)->dispose(gobject);
}

void
ags_fx_abyss_synth_audio_finalize(GObject *gobject)
{
  AgsFxAbyssSynthAudio *fx_abyss_synth_audio;

  guint i;
  
  fx_abyss_synth_audio = AGS_FX_ABYSS_SYNTH_AUDIO(gobject);

  
  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      ags_fx_abyss_synth_audio_scope_data_free(fx_abyss_synth_audio->scope_data[i]);
    }
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_abyss_synth_audio_parent_class)->finalize(gobject);
}

void
ags_fx_abyss_synth_audio_notify_audio_callback(GObject *gobject,
					       GParamSpec *pspec,
					       gpointer user_data)
{
  AgsAudio *audio;
  AgsFxAbyssSynthAudio *fx_abyss_synth_audio;

  fx_abyss_synth_audio = AGS_FX_ABYSS_SYNTH_AUDIO(gobject);

  /* get audio */
  audio = NULL;

  g_object_get(fx_abyss_synth_audio,
	       "audio", &audio,
	       NULL);

  g_signal_connect_after((GObject *) audio, "set-audio-channels",
			 G_CALLBACK(ags_fx_abyss_synth_audio_set_audio_channels_callback), fx_abyss_synth_audio);

  if(audio != NULL){
    g_object_unref(audio);
  }
}

void
ags_fx_abyss_synth_audio_notify_buffer_size_callback(GObject *gobject,
						     GParamSpec *pspec,
						     gpointer user_data)
{
  AgsFxAbyssSynthAudio *fx_abyss_synth_audio;

  guint buffer_size;
  guint format;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_abyss_synth_audio = AGS_FX_ABYSS_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_abyss_synth_audio);

  /* get buffer size */
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  format =  AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  g_object_get(fx_abyss_synth_audio,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxAbyssSynthAudioScopeData *scope_data;

    scope_data = fx_abyss_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxAbyssSynthAudioChannelData *channel_data;

	AgsAbyssSynthUtil *abyss_synth_util_0;
	
	gpointer destination;

	channel_data = scope_data->channel_data[j];

	abyss_synth_util_0 = channel_data->abyss_synth_util_0;

	/* synth buffer */
	ags_stream_free(channel_data->synth_buffer_0);

	channel_data->synth_buffer_0 = NULL;

	if(buffer_size > 0){
	  channel_data->synth_buffer_0 = ags_stream_alloc(buffer_size,
							  format);
	}
	
	ags_amplifier_util_set_buffer_length(channel_data->amplifier_util_0,
					     buffer_size);
	
	/* free chorus destination */
	destination = ags_chorus_util_get_destination(channel_data->chorus_util);
	
	ags_stream_free(destination);

	ags_chorus_util_set_destination(channel_data->chorus_util,
					NULL);

	/* alloc chorus destination */
	if(buffer_size > 0){
	  destination = ags_stream_alloc(buffer_size,
					 ags_chorus_util_get_format(channel_data->chorus_util));
	  
	  ags_chorus_util_set_destination(channel_data->chorus_util,
					  destination);
	}
	
	ags_chorus_util_set_buffer_length(channel_data->chorus_util,
					  buffer_size);
		
	/* set buffer length */
	ags_abyss_synth_util_set_buffer_length(abyss_synth_util_0,
					       buffer_size);
      }
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_abyss_synth_audio_notify_format_callback(GObject *gobject,
						GParamSpec *pspec,
						gpointer user_data)
{
  AgsFxAbyssSynthAudio *fx_abyss_synth_audio;

  guint buffer_size;
  guint format;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_abyss_synth_audio = AGS_FX_ABYSS_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_abyss_synth_audio);

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  format =  AGS_SOUNDCARD_DEFAULT_FORMAT;

  g_object_get(fx_abyss_synth_audio,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxAbyssSynthAudioScopeData *scope_data;

    scope_data = fx_abyss_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxAbyssSynthAudioChannelData *channel_data;

	AgsAbyssSynthUtil *abyss_synth_util_0;
	
	gpointer destination;
	
	channel_data = scope_data->channel_data[j];

	abyss_synth_util_0 = channel_data->abyss_synth_util_0;

	ags_abyss_synth_util_set_format(abyss_synth_util_0,
					format);

	/* synth buffer */
	ags_stream_free(channel_data->synth_buffer_0);

	channel_data->synth_buffer_0 = NULL;

	if(buffer_size > 0){
	  channel_data->synth_buffer_0 = ags_stream_alloc(buffer_size,
							  format);
	}

	ags_amplifier_util_set_format(channel_data->amplifier_util_0,
				      format);

	/* free chorus destination */
	destination = ags_chorus_util_get_destination(channel_data->chorus_util);
	
	ags_stream_free(destination);

	ags_chorus_util_set_destination(channel_data->chorus_util,
					NULL);

	/* alloc chorus destination */
	if(buffer_size > 0){
	  destination = ags_stream_alloc(buffer_size,
					 format);
	  
	  ags_chorus_util_set_destination(channel_data->chorus_util,
					  destination);
	}
	
	ags_chorus_util_set_format(channel_data->chorus_util,
				   format);	
      }
    }
  }

  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_abyss_synth_audio_notify_samplerate_callback(GObject *gobject,
						    GParamSpec *pspec,
						    gpointer user_data)
{
  AgsFxAbyssSynthAudio *fx_abyss_synth_audio;

  guint samplerate;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_abyss_synth_audio = AGS_FX_ABYSS_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_abyss_synth_audio);

  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  g_object_get(fx_abyss_synth_audio,
	       "samplerate", &samplerate,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxAbyssSynthAudioScopeData *scope_data;

    scope_data = fx_abyss_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxAbyssSynthAudioChannelData *channel_data;

	AgsAbyssSynthUtil *abyss_synth_util_0;

	channel_data = scope_data->channel_data[j];

	abyss_synth_util_0 = channel_data->abyss_synth_util_0;

	ags_abyss_synth_util_set_samplerate(abyss_synth_util_0,
					    samplerate);
	
	ags_amplifier_util_set_samplerate(channel_data->amplifier_util_0,
					  samplerate);

	ags_chorus_util_set_samplerate(channel_data->chorus_util,
				       samplerate);
      }
    }
  }

  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_abyss_synth_audio_set_audio_channels_callback(AgsAudio *audio,
						     guint audio_channels, guint audio_channels_old,
						     AgsFxAbyssSynthAudio *fx_abyss_synth_audio)
{
  guint input_pads;
  guint output_port_count, input_port_count;
  guint buffer_size;
  guint format;
  guint samplerate;
  guint i, j, k;

  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_abyss_synth_audio);

  input_pads = 0;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  g_object_get(audio,
	       "input-pads", &input_pads,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       "samplerate", &samplerate,
	       NULL);
    
  /* allocate channel data */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxAbyssSynthAudioScopeData *scope_data;

    scope_data = fx_abyss_synth_audio->scope_data[i];

    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      if(scope_data->audio_channels > audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxAbyssSynthAudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];
	
	  ags_fx_abyss_synth_audio_channel_data_free(channel_data);
	}
      }
      
      if(scope_data->channel_data == NULL){
	scope_data->channel_data = (AgsFxAbyssSynthAudioChannelData **) g_malloc(audio_channels * sizeof(AgsFxAbyssSynthAudioChannelData *)); 
      }else{
	scope_data->channel_data = (AgsFxAbyssSynthAudioChannelData **) g_realloc(scope_data->channel_data,
										  audio_channels * sizeof(AgsFxAbyssSynthAudioChannelData *)); 
      }

      if(scope_data->audio_channels < audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxAbyssSynthAudioChannelData *channel_data;

	  AgsAbyssSynthUtil *abyss_synth_util_0;

	  gpointer destination;  

	  channel_data =
	    scope_data->channel_data[j] = ags_fx_abyss_synth_audio_channel_data_alloc();

	  abyss_synth_util_0 = channel_data->abyss_synth_util_0;

	  channel_data->synth_buffer_0 = ags_stream_alloc(buffer_size,
							  format);

	  ags_abyss_synth_util_set_buffer_length(abyss_synth_util_0,
						 buffer_size);
	  ags_abyss_synth_util_set_format(channel_data->abyss_synth_util_0,
					  format);		
	  ags_abyss_synth_util_set_samplerate(channel_data->abyss_synth_util_0,
					      samplerate);

	  ags_amplifier_util_set_buffer_length(channel_data->amplifier_util_0,
					       buffer_size);
	  ags_amplifier_util_set_format(channel_data->amplifier_util_0,
					format);
	  ags_amplifier_util_set_samplerate(channel_data->amplifier_util_0,
					    samplerate);

	  ags_chorus_util_set_buffer_length(channel_data->chorus_util,
					    buffer_size);
	  ags_chorus_util_set_format(channel_data->chorus_util,
				     format);
	  ags_chorus_util_set_samplerate(channel_data->chorus_util,
					 samplerate);

	  /* alloc chorus destination */
	  destination = ags_stream_alloc(buffer_size,
					 format);

	  ags_chorus_util_set_destination(channel_data->chorus_util,
					  destination);	  

	  for(k = 0; k < AGS_SEQUENCER_MAX_MIDI_KEYS; k++){
	    AgsFxAbyssSynthAudioInputData *input_data;

	    input_data = channel_data->input_data[k];

	    //TODO:JK: implement me
	  }
	}
      }
      
      scope_data->audio_channels = audio_channels;
    }
  }  

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_abyss_synth_audio_scope_data_alloc:
 * 
 * Allocate #AgsFxAbyssSynthAudioScopeData-struct
 * 
 * Returns: the new #AgsFxAbyssSynthAudioScopeData-struct
 * 
 * Since: 9.4.0
 */
AgsFxAbyssSynthAudioScopeData*
ags_fx_abyss_synth_audio_scope_data_alloc()
{
  AgsFxAbyssSynthAudioScopeData *scope_data;

  scope_data = (AgsFxAbyssSynthAudioScopeData *) g_malloc(sizeof(AgsFxAbyssSynthAudioScopeData));

  g_rec_mutex_init(&(scope_data->strct_mutex));
  
  scope_data->parent = NULL;
  
  scope_data->audio_channels = 0;

  scope_data->channel_data = NULL;
  
  return(scope_data);
}

/**
 * ags_fx_abyss_synth_audio_scope_data_free:
 * @scope_data: the #AgsFxAbyssSynthAudioScopeData-struct
 * 
 * Free @scope_data.
 * 
 * Since: 9.4.0
 */
void
ags_fx_abyss_synth_audio_scope_data_free(AgsFxAbyssSynthAudioScopeData *scope_data)
{
  guint i;

  if(scope_data == NULL){
    return;
  }

  for(i = 0; i < scope_data->audio_channels; i++){
    ags_fx_abyss_synth_audio_channel_data_free(scope_data->channel_data[i]);
  }

  g_free(scope_data);
}

/**
 * ags_fx_abyss_synth_audio_channel_data_alloc:
 * 
 * Allocate #AgsFxAbyssSynthAudioChannelData-struct
 * 
 * Returns: the new #AgsFxAbyssSynthAudioChannelData-struct
 * 
 * Since: 9.4.0
 */
AgsFxAbyssSynthAudioChannelData*
ags_fx_abyss_synth_audio_channel_data_alloc()
{
  AgsFxAbyssSynthAudioChannelData *channel_data;

  guint i;
  
  channel_data = (AgsFxAbyssSynthAudioChannelData *) g_malloc(sizeof(AgsFxAbyssSynthAudioChannelData));

  g_rec_mutex_init(&(channel_data->strct_mutex));

  channel_data->parent = NULL;

  channel_data->synth_buffer_0 = NULL;

  /* abyss synth util */
  channel_data->abyss_synth_util_0 = ags_abyss_synth_util_alloc();

  /* amplifier util */
  channel_data->amplifier_util_0 = ags_amplifier_util_alloc();

  /* chorus util */
  channel_data->chorus_util = ags_chorus_util_alloc();

  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    channel_data->input_data[i] = ags_fx_abyss_synth_audio_input_data_alloc();

    channel_data->input_data[i]->parent = channel_data;
  }

  return(channel_data);
}

/**
 * ags_fx_abyss_synth_audio_channel_data_free:
 * @channel_data: the #AgsFxAbyssSynthAudioChannelData-struct
 * 
 * Free @channel_data.
 * 
 * Since: 9.4.0
 */
void
ags_fx_abyss_synth_audio_channel_data_free(AgsFxAbyssSynthAudioChannelData *channel_data)
{
  guint i;

  if(channel_data == NULL){
    return;
  }

  /* abyss synth util */
  ags_abyss_synth_util_free(channel_data->abyss_synth_util_0);
  
  /* amplifier util */
  ags_amplifier_util_free(channel_data->amplifier_util_0);
  
  /* chorus util */
  ags_chorus_util_free(channel_data->chorus_util);

  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    ags_fx_abyss_synth_audio_input_data_free(channel_data->input_data[i]);
  }

  g_free(channel_data);
}

/**
 * ags_fx_abyss_synth_audio_input_data_alloc:
 * 
 * Allocate #AgsFxAbyssSynthAudioInputData-struct
 * 
 * Returns: the new #AgsFxAbyssSynthAudioInputData-struct
 * 
 * Since: 9.4.0
 */
AgsFxAbyssSynthAudioInputData*
ags_fx_abyss_synth_audio_input_data_alloc()
{
  AgsFxAbyssSynthAudioInputData *input_data;

  input_data = (AgsFxAbyssSynthAudioInputData *) g_malloc(sizeof(AgsFxAbyssSynthAudioInputData));

  g_rec_mutex_init(&(input_data->strct_mutex));

  input_data->parent = NULL;

  input_data->key_on = 0;
  
  return(input_data);
}

/**
 * ags_fx_abyss_synth_audio_input_data_free:
 * @input_data: the #AgsFxAbyssSynthAudioInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 9.4.0
 */
void
ags_fx_abyss_synth_audio_input_data_free(AgsFxAbyssSynthAudioInputData *input_data)
{
  if(input_data == NULL){
    return;
  }

  g_free(input_data);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_0_oscillator_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_INTEGER |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      (gfloat) (AGS_SYNTH_OSCILLATOR_LAST - 1));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_0_octave_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      -6.0);
    g_value_set_float(plugin_port->upper_value,
		      6.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_0_key_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      2.0);
    g_value_set_float(plugin_port->lower_value,
		      -12.0);
    g_value_set_float(plugin_port->upper_value,
		      12.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_0_phase_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      2.0 * M_PI);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_0_volume_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_0_low_pass_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      2.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      2.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_1_oscillator_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_INTEGER |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      (gfloat) (AGS_SYNTH_OSCILLATOR_LAST - 1));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_1_octave_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      -6.0);
    g_value_set_float(plugin_port->upper_value,
		      6.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_1_key_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      2.0);
    g_value_set_float(plugin_port->lower_value,
		      -12.0);
    g_value_set_float(plugin_port->upper_value,
		      12.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_1_phase_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      2.0 * M_PI);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_1_volume_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_1_low_pass_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      2.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      2.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_2_oscillator_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_INTEGER |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      (gfloat) (AGS_SYNTH_OSCILLATOR_LAST - 1));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_2_octave_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      -6.0);
    g_value_set_float(plugin_port->upper_value,
		      6.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_2_key_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      2.0);
    g_value_set_float(plugin_port->lower_value,
		      -12.0);
    g_value_set_float(plugin_port->upper_value,
		      12.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_2_phase_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      2.0 * M_PI);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_2_volume_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_2_low_pass_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      2.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      2.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_3_oscillator_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_INTEGER |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      (gfloat) (AGS_SYNTH_OSCILLATOR_LAST - 1));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_3_octave_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      -6.0);
    g_value_set_float(plugin_port->upper_value,
		      6.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_3_key_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      2.0);
    g_value_set_float(plugin_port->lower_value,
		      -12.0);
    g_value_set_float(plugin_port->upper_value,
		      12.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_3_phase_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      2.0 * M_PI);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_3_volume_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_osc_3_low_pass_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      2.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      2.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_ring_0_enabled_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL |
			   AGS_PLUGIN_PORT_TOGGLED);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_ring_0_tuning_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      -1200.0);
    g_value_set_float(plugin_port->upper_value,
		      1200.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_ring_0_drive_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_ring_0_mix_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_ring_0_gain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_ring_1_enabled_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL |
			   AGS_PLUGIN_PORT_TOGGLED);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_ring_1_tuning_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      -1200.0);
    g_value_set_float(plugin_port->upper_value,
		      1200.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_ring_1_drive_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_ring_1_mix_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_ring_1_gain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_pitch_tuning_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      -1200.0);
    g_value_set_float(plugin_port->upper_value,
		      1200.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_volume_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_0_attack_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_0_decay_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_0_sustain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_0_release_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_0_gain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_0_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_0_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    static gint64 sends_default[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_lower[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_upper[AGS_ABYSS_SYNTH_SENDS_COUNT];

    guint i;
    
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    memset(&(sends_default[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(sends_lower[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    // memset(&(sends_upper[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

    for(i = 0; i < AGS_ABYSS_SYNTH_SENDS_COUNT; i++){
      sends_upper[i] = (1L << i);
    }
    
    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_POINTER);

    g_value_set_pointer(plugin_port->default_value,
			&(sends_default[0]));
    g_value_set_pointer(plugin_port->lower_value,
			&(sends_lower[0]));
    g_value_set_pointer(plugin_port->upper_value,
			&(sends_upper[0]));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_1_attack_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_1_decay_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_1_sustain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_1_release_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_1_gain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_1_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_1_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    static gint64 sends_default[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_lower[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_upper[AGS_ABYSS_SYNTH_SENDS_COUNT];

    guint i;
    
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    memset(&(sends_default[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(sends_lower[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    // memset(&(sends_upper[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

    for(i = 0; i < AGS_ABYSS_SYNTH_SENDS_COUNT; i++){
      sends_upper[i] = (1L << i);
    }
    

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_POINTER);

    g_value_set_pointer(plugin_port->default_value,
			&(sends_default[0]));
    g_value_set_pointer(plugin_port->lower_value,
			&(sends_lower[0]));
    g_value_set_pointer(plugin_port->upper_value,
			&(sends_upper[0]));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_2_attack_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_2_decay_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_2_sustain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_2_release_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_2_gain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_2_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_2_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    static gint64 sends_default[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_lower[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_upper[AGS_ABYSS_SYNTH_SENDS_COUNT];

    guint i;
    
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    memset(&(sends_default[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(sends_lower[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    // memset(&(sends_upper[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

    for(i = 0; i < AGS_ABYSS_SYNTH_SENDS_COUNT; i++){
      sends_upper[i] = (1L << i);
    }
    
    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_POINTER);

    g_value_set_pointer(plugin_port->default_value,
			&(sends_default[0]));
    g_value_set_pointer(plugin_port->lower_value,
			&(sends_lower[0]));
    g_value_set_pointer(plugin_port->upper_value,
			&(sends_upper[0]));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_3_attack_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_3_decay_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_3_sustain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_3_release_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_3_gain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_3_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_env_3_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    static gint64 sends_default[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_lower[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_upper[AGS_ABYSS_SYNTH_SENDS_COUNT];

    guint i;
    
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    memset(&(sends_default[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(sends_lower[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    // memset(&(sends_upper[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

    for(i = 0; i < AGS_ABYSS_SYNTH_SENDS_COUNT; i++){
      sends_upper[i] = (1L << i);
    }
    
    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_POINTER);

    g_value_set_pointer(plugin_port->default_value,
			&(sends_default[0]));
    g_value_set_pointer(plugin_port->lower_value,
			&(sends_lower[0]));
    g_value_set_pointer(plugin_port->upper_value,
			&(sends_upper[0]));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_0_oscillator_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      (gfloat) AGS_SYNTH_OSCILLATOR_SIN);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      (gfloat) AGS_SYNTH_OSCILLATOR_LAST - 1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_0_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_0_depth_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_DEPTH);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_0_tuning_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_TUNING);
    g_value_set_float(plugin_port->lower_value,
		      -1200.0);
    g_value_set_float(plugin_port->upper_value,
		      1200.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_0_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    static gint64 sends_default[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_lower[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_upper[AGS_ABYSS_SYNTH_SENDS_COUNT];

    guint i;
    
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    memset(&(sends_default[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(sends_lower[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    // memset(&(sends_upper[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

    for(i = 0; i < AGS_ABYSS_SYNTH_SENDS_COUNT; i++){
      sends_upper[i] = (1L << i);
    }
    

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_POINTER);

    g_value_set_pointer(plugin_port->default_value,
			&(sends_default[0]));
    g_value_set_pointer(plugin_port->lower_value,
			&(sends_lower[0]));
    g_value_set_pointer(plugin_port->upper_value,
			&(sends_upper[0]));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_1_oscillator_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      (gfloat) AGS_SYNTH_OSCILLATOR_SIN);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      (gfloat) AGS_SYNTH_OSCILLATOR_LAST - 1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_1_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_1_depth_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_DEPTH);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_1_tuning_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_TUNING);
    g_value_set_float(plugin_port->lower_value,
		      -1200.0);
    g_value_set_float(plugin_port->upper_value,
		      1200.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_1_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    static gint64 sends_default[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_lower[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_upper[AGS_ABYSS_SYNTH_SENDS_COUNT];

    guint i;
    
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    memset(&(sends_default[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(sends_lower[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    // memset(&(sends_upper[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

    for(i = 0; i < AGS_ABYSS_SYNTH_SENDS_COUNT; i++){
      sends_upper[i] = (1L << i);
    }
    

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_POINTER);

    g_value_set_pointer(plugin_port->default_value,
			&(sends_default[0]));
    g_value_set_pointer(plugin_port->lower_value,
			&(sends_lower[0]));
    g_value_set_pointer(plugin_port->upper_value,
			&(sends_upper[0]));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_2_oscillator_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      (gfloat) AGS_SYNTH_OSCILLATOR_SIN);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      (gfloat) AGS_SYNTH_OSCILLATOR_LAST - 1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_2_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_2_depth_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_DEPTH);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_2_tuning_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_TUNING);
    g_value_set_float(plugin_port->lower_value,
		      -1200.0);
    g_value_set_float(plugin_port->upper_value,
		      1200.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_2_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    static gint64 sends_default[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_lower[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_upper[AGS_ABYSS_SYNTH_SENDS_COUNT];

    guint i;
    
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    memset(&(sends_default[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(sends_lower[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    // memset(&(sends_upper[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

    for(i = 0; i < AGS_ABYSS_SYNTH_SENDS_COUNT; i++){
      sends_upper[i] = (1L << i);
    }
    

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_POINTER);

    g_value_set_pointer(plugin_port->default_value,
			&(sends_default[0]));
    g_value_set_pointer(plugin_port->lower_value,
			&(sends_lower[0]));
    g_value_set_pointer(plugin_port->upper_value,
			&(sends_upper[0]));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_3_oscillator_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      (gfloat) AGS_SYNTH_OSCILLATOR_SIN);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      (gfloat) AGS_SYNTH_OSCILLATOR_LAST - 1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_3_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_3_depth_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_DEPTH);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_3_tuning_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      AGS_ABYSS_SYNTH_UTIL_DEFAULT_LFO_TUNING);
    g_value_set_float(plugin_port->lower_value,
		      -1200.0);
    g_value_set_float(plugin_port->upper_value,
		      1200.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_lfo_3_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    static gint64 sends_default[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_lower[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_upper[AGS_ABYSS_SYNTH_SENDS_COUNT];

    guint i;
    
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    memset(&(sends_default[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(sends_lower[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    // memset(&(sends_upper[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

    for(i = 0; i < AGS_ABYSS_SYNTH_SENDS_COUNT; i++){
      sends_upper[i] = (1L << i);
    }
    

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_POINTER);

    g_value_set_pointer(plugin_port->default_value,
			&(sends_default[0]));
    g_value_set_pointer(plugin_port->lower_value,
			&(sends_lower[0]));
    g_value_set_pointer(plugin_port->upper_value,
			&(sends_upper[0]));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_0_modulation_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      -1.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_0_pingpong_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL |
			   AGS_PLUGIN_PORT_TOGGLED);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_0_lfo_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      8.0);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_0_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    static gint64 sends_default[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_lower[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_upper[AGS_ABYSS_SYNTH_SENDS_COUNT];

    guint i;
    
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    memset(&(sends_default[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(sends_lower[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    // memset(&(sends_upper[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

    for(i = 0; i < AGS_ABYSS_SYNTH_SENDS_COUNT; i++){
      sends_upper[i] = (1L << i);
    }
    

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_POINTER);

    g_value_set_pointer(plugin_port->default_value,
			&(sends_default[0]));
    g_value_set_pointer(plugin_port->lower_value,
			&(sends_lower[0]));
    g_value_set_pointer(plugin_port->upper_value,
			&(sends_upper[0]));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_1_modulation_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      -1.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_1_pingpong_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL |
			   AGS_PLUGIN_PORT_TOGGLED);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_1_lfo_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      8.0);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_1_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    static gint64 sends_default[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_lower[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_upper[AGS_ABYSS_SYNTH_SENDS_COUNT];

    guint i;
    
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    memset(&(sends_default[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(sends_lower[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    // memset(&(sends_upper[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

    for(i = 0; i < AGS_ABYSS_SYNTH_SENDS_COUNT; i++){
      sends_upper[i] = (1L << i);
    }
    

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_POINTER);

    g_value_set_pointer(plugin_port->default_value,
			&(sends_default[0]));
    g_value_set_pointer(plugin_port->lower_value,
			&(sends_lower[0]));
    g_value_set_pointer(plugin_port->upper_value,
			&(sends_upper[0]));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_2_modulation_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      -1.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_2_pingpong_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL |
			   AGS_PLUGIN_PORT_TOGGLED);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_2_lfo_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      8.0);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_2_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    static gint64 sends_default[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_lower[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_upper[AGS_ABYSS_SYNTH_SENDS_COUNT];

    guint i;
    
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    memset(&(sends_default[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(sends_lower[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    // memset(&(sends_upper[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

    for(i = 0; i < AGS_ABYSS_SYNTH_SENDS_COUNT; i++){
      sends_upper[i] = (1L << i);
    }
    

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_POINTER);

    g_value_set_pointer(plugin_port->default_value,
			&(sends_default[0]));
    g_value_set_pointer(plugin_port->lower_value,
			&(sends_lower[0]));
    g_value_set_pointer(plugin_port->upper_value,
			&(sends_upper[0]));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_3_modulation_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      -1.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_3_pingpong_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL |
			   AGS_PLUGIN_PORT_TOGGLED);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_3_lfo_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      8.0);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_seq_3_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    static gint64 sends_default[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_lower[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_upper[AGS_ABYSS_SYNTH_SENDS_COUNT];

    guint i;
    
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    memset(&(sends_default[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(sends_lower[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    // memset(&(sends_upper[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

    for(i = 0; i < AGS_ABYSS_SYNTH_SENDS_COUNT; i++){
      sends_upper[i] = (1L << i);
    }
    

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_POINTER);

    g_value_set_pointer(plugin_port->default_value,
			&(sends_default[0]));
    g_value_set_pointer(plugin_port->lower_value,
			&(sends_lower[0]));
    g_value_set_pointer(plugin_port->upper_value,
			&(sends_upper[0]));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_pink_noise_0_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      220.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1760.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_pink_noise_0_gain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_pink_noise_0_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    static gint64 sends_default[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_lower[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_upper[AGS_ABYSS_SYNTH_SENDS_COUNT];

    guint i;
    
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    memset(&(sends_default[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(sends_lower[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    // memset(&(sends_upper[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

    for(i = 0; i < AGS_ABYSS_SYNTH_SENDS_COUNT; i++){
      sends_upper[i] = (1L << i);
    }
    

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_POINTER);

    g_value_set_pointer(plugin_port->default_value,
			&(sends_default[0]));
    g_value_set_pointer(plugin_port->lower_value,
			&(sends_lower[0]));
    g_value_set_pointer(plugin_port->upper_value,
			&(sends_upper[0]));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_pink_noise_1_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      220.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1760.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_pink_noise_1_gain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_synth_0_pink_noise_1_sends_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    static gint64 sends_default[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_lower[AGS_ABYSS_SYNTH_SENDS_COUNT];
    static gint64 sends_upper[AGS_ABYSS_SYNTH_SENDS_COUNT];

    guint i;
    
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    memset(&(sends_default[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    memset(&(sends_lower[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));
    // memset(&(sends_upper[0]), 0, AGS_ABYSS_SYNTH_SENDS_COUNT * sizeof(gint64));

    for(i = 0; i < AGS_ABYSS_SYNTH_SENDS_COUNT; i++){
      sends_upper[i] = (1L << i);
    }
    

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_POINTER);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_POINTER);

    g_value_set_pointer(plugin_port->default_value,
			&(sends_default[0]));
    g_value_set_pointer(plugin_port->lower_value,
			&(sends_lower[0]));
    g_value_set_pointer(plugin_port->upper_value,
			&(sends_upper[0]));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_chorus_enabled_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL |
			   AGS_PLUGIN_PORT_TOGGLED);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_chorus_pitch_type_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      (gfloat) AGS_PITCH_TYPE_FLUID_INTERPOLATE_4TH_ORDER);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      6.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_chorus_input_volume_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_chorus_output_volume_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_chorus_lfo_oscillator_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_INTEGER |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      (gfloat) (AGS_SYNTH_OSCILLATOR_LAST - 1));
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_chorus_lfo_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      10.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_chorus_depth_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_chorus_mix_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.5);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_low_pass_0_cut_off_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      2000.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      22000.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_low_pass_0_filter_gain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_low_pass_0_no_clip_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_low_pass_1_cut_off_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      2000.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      22000.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_low_pass_1_filter_gain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      1.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_low_pass_1_no_clip_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_amplifier_amp_gain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_amplifier_filter_gain_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_abyss_synth_audio_get_chorus_delay_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

/**
 * ags_fx_abyss_synth_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxAbyssSynthAudio
 *
 * Returns: the new #AgsFxAbyssSynthAudio
 *
 * Since: 9.4.0
 */
AgsFxAbyssSynthAudio*
ags_fx_abyss_synth_audio_new(AgsAudio *audio)
{
  AgsFxAbyssSynthAudio *fx_abyss_synth_audio;

  fx_abyss_synth_audio = (AgsFxAbyssSynthAudio *) g_object_new(AGS_TYPE_FX_ABYSS_SYNTH_AUDIO,
							       "audio", audio,
							       NULL);
  
  return(fx_abyss_synth_audio);
}
