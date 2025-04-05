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

#include <ags/audio/fx/ags_fx_raven_synth_audio.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_audio_signal.h>

#include <ags/i18n.h>

void ags_fx_raven_synth_audio_class_init(AgsFxRavenSynthAudioClass *fx_raven_synth_audio);
void ags_fx_raven_synth_audio_init(AgsFxRavenSynthAudio *fx_raven_synth_audio);
void ags_fx_raven_synth_audio_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec);
void ags_fx_raven_synth_audio_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec);
void ags_fx_raven_synth_audio_dispose(GObject *gobject);
void ags_fx_raven_synth_audio_finalize(GObject *gobject);

void ags_fx_raven_synth_audio_notify_audio_callback(GObject *gobject,
						    GParamSpec *pspec,
						    gpointer user_data);
void ags_fx_raven_synth_audio_notify_buffer_size_callback(GObject *gobject,
							  GParamSpec *pspec,
							  gpointer user_data);
void ags_fx_raven_synth_audio_notify_format_callback(GObject *gobject,
						     GParamSpec *pspec,
						     gpointer user_data);
void ags_fx_raven_synth_audio_notify_samplerate_callback(GObject *gobject,
							 GParamSpec *pspec,
							 gpointer user_data);

void ags_fx_raven_synth_audio_set_audio_channels_callback(AgsAudio *audio,
							  guint audio_channels, guint audio_channels_old,
							  AgsFxRavenSynthAudio *fx_raven_synth_audio);

void ags_fx_raven_synth_audio_pitch_type_callback(AgsPort *port, GValue *value,
						  AgsFxRavenSynthAudio *fx_raven_synth_audio);

static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_oscillator_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_octave_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_key_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_phase_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_volume_plugin_port();

static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_seq_tuning_pingpong_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_seq_tuning_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_seq_volume_pingpong_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_seq_volume_lfo_frequency_plugin_port();

static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_sync_enabled_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_sync_relative_attack_factor_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_sync_attack_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_sync_phase_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_lfo_depth_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_synth_lfo_tuning_plugin_port();

static AgsPluginPort* ags_fx_raven_synth_audio_get_low_pass_cut_off_frequency_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_low_pass_filter_gain_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_low_pass_no_clip_plugin_port();

static AgsPluginPort* ags_fx_raven_synth_audio_get_amplifier_amp_gain_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_amplifier_filter_gain_plugin_port();

static AgsPluginPort* ags_fx_raven_synth_audio_get_noise_gain_plugin_port();

static AgsPluginPort* ags_fx_raven_synth_audio_get_pitch_type_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_pitch_tuning_plugin_port();

static AgsPluginPort* ags_fx_raven_synth_audio_get_chorus_enabled_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_chorus_pitch_type_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_chorus_input_volume_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_chorus_output_volume_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_chorus_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_chorus_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_chorus_depth_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_chorus_mix_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_chorus_delay_plugin_port();

static AgsPluginPort* ags_fx_raven_synth_audio_get_vibrato_enabled_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_vibrato_gain_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_vibrato_lfo_depth_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_vibrato_lfo_freq_plugin_port();
static AgsPluginPort* ags_fx_raven_synth_audio_get_vibrato_tuning_plugin_port();

/**
 * SECTION:ags_fx_raven_synth_audio
 * @short_description: fx raven synth audio
 * @title: AgsFxRavenSynthAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_raven_synth_audio.h
 *
 * The #AgsFxRavenSynthAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_raven_synth_audio_parent_class = NULL;

const gchar *ags_fx_raven_synth_audio_plugin_name = "ags-fx-raven-synth";

const gchar* ags_fx_raven_synth_audio_specifier[] = {
  "./synth-0-oscillator[0]",
  "./synth-0-octave[0]",
  "./synth-0-key[0]",
  "./synth-0-phase[0]",
  "./synth-0-volume[0]",
  "./synth-0-seq-tuning-0[0]",
  "./synth-0-seq-tuning-1[0]",
  "./synth-0-seq-tuning-2[0]",
  "./synth-0-seq-tuning-3[0]",
  "./synth-0-seq-tuning-4[0]",
  "./synth-0-seq-tuning-5[0]",
  "./synth-0-seq-tuning-6[0]",
  "./synth-0-seq-tuning-7[0]",
  "./synth-0-seq-tuning-8[0]",
  "./synth-0-seq-tuning-9[0]",
  "./synth-0-seq-tuning-10[0]",
  "./synth-0-seq-tuning-11[0]",
  "./synth-0-seq-tuning-12[0]",
  "./synth-0-seq-tuning-13[0]",
  "./synth-0-seq-tuning-14[0]",
  "./synth-0-seq-tuning-15[0]",
  "./synth-0-seq-tuning-pingpong[0]",
  "./synth-0-seq-tuning-lfo-frequency[0]",
  "./synth-0-seq-volume-0[0]",
  "./synth-0-seq-volume-1[0]",
  "./synth-0-seq-volume-2[0]",
  "./synth-0-seq-volume-3[0]",
  "./synth-0-seq-volume-4[0]",
  "./synth-0-seq-volume-5[0]",
  "./synth-0-seq-volume-6[0]",
  "./synth-0-seq-volume-7[0]",
  "./synth-0-seq-volume-8[0]",
  "./synth-0-seq-volume-9[0]",
  "./synth-0-seq-volume-10[0]",
  "./synth-0-seq-volume-11[0]",
  "./synth-0-seq-volume-12[0]",
  "./synth-0-seq-volume-13[0]",
  "./synth-0-seq-volume-14[0]",
  "./synth-0-seq-volume-15[0]",
  "./synth-0-seq-volume-pingpong[0]",
  "./synth-0-seq-volume-lfo-frequency[0]",
  "./synth-0-sync-enabled[0]",
  "./synth-0-sync-relative-factor[0]",
  "./synth-0-sync-attack-0[0]",
  "./synth-0-sync-phase-0[0]",
  "./synth-0-sync-attack-1[0]",
  "./synth-0-sync-phase-1[0]",
  "./synth-0-sync-attack-2[0]",
  "./synth-0-sync-phase-2[0]",
  "./synth-0-sync-attack-3[0]",
  "./synth-0-sync-phase-3[0]",
  "./synth-0-sync-lfo-oscillator[0]",
  "./synth-0-sync-lfo-frequency[0]",
  "./synth-0-lfo-oscillator[0]",
  "./synth-0-lfo-frequency[0]",
  "./synth-0-lfo-depth[0]",
  "./synth-0-lfo-tuning[0]",
  "./synth-1-oscillator[0]",
  "./synth-1-octave[0]",
  "./synth-1-key[0]",
  "./synth-1-phase[0]",
  "./synth-1-volume[0]",
  "./synth-1-seq-tuning-0[0]",
  "./synth-1-seq-tuning-1[0]",
  "./synth-1-seq-tuning-2[0]",
  "./synth-1-seq-tuning-3[0]",
  "./synth-1-seq-tuning-4[0]",
  "./synth-1-seq-tuning-5[0]",
  "./synth-1-seq-tuning-6[0]",
  "./synth-1-seq-tuning-7[0]",
  "./synth-1-seq-tuning-8[0]",
  "./synth-1-seq-tuning-9[0]",
  "./synth-1-seq-tuning-10[0]",
  "./synth-1-seq-tuning-11[0]",
  "./synth-1-seq-tuning-12[0]",
  "./synth-1-seq-tuning-13[0]",
  "./synth-1-seq-tuning-14[0]",
  "./synth-1-seq-tuning-15[0]",
  "./synth-1-seq-tuning-pingpong[0]",
  "./synth-1-seq-tuning-lfo-frequency[0]",
  "./synth-1-seq-volume-0[0]",
  "./synth-1-seq-volume-1[0]",
  "./synth-1-seq-volume-2[0]",
  "./synth-1-seq-volume-3[0]",
  "./synth-1-seq-volume-4[0]",
  "./synth-1-seq-volume-5[0]",
  "./synth-1-seq-volume-6[0]",
  "./synth-1-seq-volume-7[0]",
  "./synth-1-seq-volume-8[0]",
  "./synth-1-seq-volume-9[0]",
  "./synth-1-seq-volume-10[0]",
  "./synth-1-seq-volume-11[0]",
  "./synth-1-seq-volume-12[0]",
  "./synth-1-seq-volume-13[0]",
  "./synth-1-seq-volume-14[0]",
  "./synth-1-seq-volume-15[0]",
  "./synth-1-seq-volume-pingpong[0]",
  "./synth-1-seq-volume-lfo-frequency[0]",
  "./synth-1-sync-enabled[0]",
  "./synth-1-sync-relative-factor[0]",
  "./synth-1-sync-attack-0[0]",
  "./synth-1-sync-phase-0[0]",
  "./synth-1-sync-attack-1[0]",
  "./synth-1-sync-phase-1[0]",
  "./synth-1-sync-attack-2[0]",
  "./synth-1-sync-phase-2[0]",
  "./synth-1-sync-attack-3[0]",
  "./synth-1-sync-phase-3[0]",
  "./synth-1-sync-lfo-oscillator[0]",
  "./synth-1-sync-lfo-frequency[0]",
  "./synth-1-lfo-oscillator[0]",
  "./synth-1-lfo-frequency[0]",
  "./synth-1-lfo-depth[0]",
  "./synth-1-lfo-tuning[0]",
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
  "./amplifier-1-amp-0-gain",
  "./amplifier-1-amp-1-gain",
  "./amplifier-1-amp-2-gain",
  "./amplifier-1-amp-3-gain",
  "./amplifier-1-filter-gain",
  "./noise-gain[0]",
  "./pitch-type[0]",
  "./pitch-tuning[0]",
  "./chorus-enabled[0]",
  "./chorus-pitch-type[0]",
  "./chorus-input-volume[0]",
  "./chorus-output-volume[0]",
  "./chorus-lfo-oscillator[0]",
  "./chorus-lfo-frequency[0]",
  "./chorus-depth[0]",
  "./chorus-mix[0]",
  "./chorus-delay[0]",
  "./vibrato-enabled[0]",
  "./vibrato-gain[0]",
  "./vibrato-lfo-depth[0]",
  "./vibrato-lfo-freq[0]",
  "./vibrato-tuning[0]",
  NULL,
};

enum{
  PROP_0,
  PROP_SYNTH_0_OSCILLATOR,
  PROP_SYNTH_0_OCTAVE,
  PROP_SYNTH_0_KEY,
  PROP_SYNTH_0_PHASE,
  PROP_SYNTH_0_VOLUME,
  PROP_SYNTH_0_SEQ_TUNING_0,
  PROP_SYNTH_0_SEQ_TUNING_1,
  PROP_SYNTH_0_SEQ_TUNING_2,
  PROP_SYNTH_0_SEQ_TUNING_3,
  PROP_SYNTH_0_SEQ_TUNING_4,
  PROP_SYNTH_0_SEQ_TUNING_5,
  PROP_SYNTH_0_SEQ_TUNING_6,
  PROP_SYNTH_0_SEQ_TUNING_7,
  PROP_SYNTH_0_SEQ_TUNING_8,
  PROP_SYNTH_0_SEQ_TUNING_9,
  PROP_SYNTH_0_SEQ_TUNING_10,
  PROP_SYNTH_0_SEQ_TUNING_11,
  PROP_SYNTH_0_SEQ_TUNING_12,
  PROP_SYNTH_0_SEQ_TUNING_13,
  PROP_SYNTH_0_SEQ_TUNING_14,
  PROP_SYNTH_0_SEQ_TUNING_15,
  PROP_SYNTH_0_SEQ_TUNING_PINGPONG,
  PROP_SYNTH_0_SEQ_TUNING_LFO_FREQUENCY,
  PROP_SYNTH_0_SEQ_VOLUME_0,
  PROP_SYNTH_0_SEQ_VOLUME_1,
  PROP_SYNTH_0_SEQ_VOLUME_2,
  PROP_SYNTH_0_SEQ_VOLUME_3,
  PROP_SYNTH_0_SEQ_VOLUME_4,
  PROP_SYNTH_0_SEQ_VOLUME_5,
  PROP_SYNTH_0_SEQ_VOLUME_6,
  PROP_SYNTH_0_SEQ_VOLUME_7,
  PROP_SYNTH_0_SEQ_VOLUME_8,
  PROP_SYNTH_0_SEQ_VOLUME_9,
  PROP_SYNTH_0_SEQ_VOLUME_10,
  PROP_SYNTH_0_SEQ_VOLUME_11,
  PROP_SYNTH_0_SEQ_VOLUME_12,
  PROP_SYNTH_0_SEQ_VOLUME_13,
  PROP_SYNTH_0_SEQ_VOLUME_14,
  PROP_SYNTH_0_SEQ_VOLUME_15,
  PROP_SYNTH_0_SEQ_VOLUME_PINGPONG,
  PROP_SYNTH_0_SEQ_VOLUME_LFO_FREQUENCY,
  PROP_SYNTH_0_SYNC_ENABLED,
  PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_0,
  PROP_SYNTH_0_SYNC_ATTACK_0,
  PROP_SYNTH_0_SYNC_PHASE_0,
  PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_1,
  PROP_SYNTH_0_SYNC_ATTACK_1,
  PROP_SYNTH_0_SYNC_PHASE_1,
  PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_2,
  PROP_SYNTH_0_SYNC_ATTACK_2,
  PROP_SYNTH_0_SYNC_PHASE_2,
  PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_3,
  PROP_SYNTH_0_SYNC_ATTACK_3,
  PROP_SYNTH_0_SYNC_PHASE_3,
  PROP_SYNTH_0_LFO_OSCILLATOR,
  PROP_SYNTH_0_LFO_FREQUENCY,
  PROP_SYNTH_0_LFO_DEPTH,
  PROP_SYNTH_0_LFO_TUNING,
  PROP_SYNTH_1_OSCILLATOR,
  PROP_SYNTH_1_OCTAVE,
  PROP_SYNTH_1_KEY,
  PROP_SYNTH_1_PHASE,
  PROP_SYNTH_1_VOLUME,
  PROP_SYNTH_1_SEQ_TUNING_0,
  PROP_SYNTH_1_SEQ_TUNING_1,
  PROP_SYNTH_1_SEQ_TUNING_2,
  PROP_SYNTH_1_SEQ_TUNING_3,
  PROP_SYNTH_1_SEQ_TUNING_4,
  PROP_SYNTH_1_SEQ_TUNING_5,
  PROP_SYNTH_1_SEQ_TUNING_6,
  PROP_SYNTH_1_SEQ_TUNING_7,
  PROP_SYNTH_1_SEQ_TUNING_8,
  PROP_SYNTH_1_SEQ_TUNING_9,
  PROP_SYNTH_1_SEQ_TUNING_10,
  PROP_SYNTH_1_SEQ_TUNING_11,
  PROP_SYNTH_1_SEQ_TUNING_12,
  PROP_SYNTH_1_SEQ_TUNING_13,
  PROP_SYNTH_1_SEQ_TUNING_14,
  PROP_SYNTH_1_SEQ_TUNING_15,
  PROP_SYNTH_1_SEQ_TUNING_PINGPONG,
  PROP_SYNTH_1_SEQ_TUNING_LFO_FREQUENCY,
  PROP_SYNTH_1_SEQ_VOLUME_0,
  PROP_SYNTH_1_SEQ_VOLUME_1,
  PROP_SYNTH_1_SEQ_VOLUME_2,
  PROP_SYNTH_1_SEQ_VOLUME_3,
  PROP_SYNTH_1_SEQ_VOLUME_4,
  PROP_SYNTH_1_SEQ_VOLUME_5,
  PROP_SYNTH_1_SEQ_VOLUME_6,
  PROP_SYNTH_1_SEQ_VOLUME_7,
  PROP_SYNTH_1_SEQ_VOLUME_8,
  PROP_SYNTH_1_SEQ_VOLUME_9,
  PROP_SYNTH_1_SEQ_VOLUME_10,
  PROP_SYNTH_1_SEQ_VOLUME_11,
  PROP_SYNTH_1_SEQ_VOLUME_12,
  PROP_SYNTH_1_SEQ_VOLUME_13,
  PROP_SYNTH_1_SEQ_VOLUME_14,
  PROP_SYNTH_1_SEQ_VOLUME_15,
  PROP_SYNTH_1_SEQ_VOLUME_PINGPONG,
  PROP_SYNTH_1_SEQ_VOLUME_LFO_FREQUENCY,
  PROP_SYNTH_1_SYNC_ENABLED,
  PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_0,
  PROP_SYNTH_1_SYNC_ATTACK_0,
  PROP_SYNTH_1_SYNC_PHASE_0,
  PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_1,
  PROP_SYNTH_1_SYNC_ATTACK_1,
  PROP_SYNTH_1_SYNC_PHASE_1,
  PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_2,
  PROP_SYNTH_1_SYNC_ATTACK_2,
  PROP_SYNTH_1_SYNC_PHASE_2,
  PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_3,
  PROP_SYNTH_1_SYNC_ATTACK_3,
  PROP_SYNTH_1_SYNC_PHASE_3,
  PROP_SYNTH_1_LFO_OSCILLATOR,
  PROP_SYNTH_1_LFO_FREQUENCY,
  PROP_SYNTH_1_LFO_DEPTH,
  PROP_SYNTH_1_LFO_TUNING,
  PROP_LOW_PASS_0_CUT_OFF_FREQUENCY,
  PROP_LOW_PASS_0_FILTER_GAIN,
  PROP_LOW_PASS_0_NO_CLIP,
  PROP_AMPLIFIER_0_AMP_0_GAIN,
  PROP_AMPLIFIER_0_AMP_1_GAIN,
  PROP_AMPLIFIER_0_AMP_2_GAIN,
  PROP_AMPLIFIER_0_AMP_3_GAIN,
  PROP_AMPLIFIER_0_FILTER_GAIN,
  PROP_LOW_PASS_1_CUT_OFF_FREQUENCY,
  PROP_LOW_PASS_1_FILTER_GAIN,
  PROP_LOW_PASS_1_NO_CLIP,
  PROP_AMPLIFIER_1_AMP_0_GAIN,
  PROP_AMPLIFIER_1_AMP_1_GAIN,
  PROP_AMPLIFIER_1_AMP_2_GAIN,
  PROP_AMPLIFIER_1_AMP_3_GAIN,
  PROP_AMPLIFIER_1_FILTER_GAIN,
  PROP_NOISE_GAIN,
  PROP_PITCH_TYPE,
  PROP_PITCH_TUNING,
  PROP_CHORUS_ENABLED,
  PROP_CHORUS_PITCH_TYPE,
  PROP_CHORUS_INPUT_VOLUME,
  PROP_CHORUS_OUTPUT_VOLUME,
  PROP_CHORUS_LFO_OSCILLATOR,
  PROP_CHORUS_LFO_FREQUENCY,
  PROP_CHORUS_DEPTH,
  PROP_CHORUS_MIX,
  PROP_CHORUS_DELAY,
  PROP_VIBRATO_ENABLED,
  PROP_VIBRATO_GAIN,
  PROP_VIBRATO_LFO_DEPTH,
  PROP_VIBRATO_LFO_FREQ,
  PROP_VIBRATO_TUNING,
};

GType
ags_fx_raven_synth_audio_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_raven_synth_audio = 0;

    static const GTypeInfo ags_fx_raven_synth_audio_info = {
      sizeof (AgsFxRavenSynthAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_raven_synth_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxRavenSynthAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_raven_synth_audio_init,
    };

    ags_type_fx_raven_synth_audio = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO,
							   "AgsFxRavenSynthAudio",
							   &ags_fx_raven_synth_audio_info,
							   0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_raven_synth_audio);
  }

  return(g_define_type_id__static);
}

void
ags_fx_raven_synth_audio_class_init(AgsFxRavenSynthAudioClass *fx_raven_synth_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_raven_synth_audio_parent_class = g_type_class_peek_parent(fx_raven_synth_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_raven_synth_audio;

  gobject->set_property = ags_fx_raven_synth_audio_set_property;
  gobject->get_property = ags_fx_raven_synth_audio_get_property;

  gobject->dispose = ags_fx_raven_synth_audio_dispose;
  gobject->finalize = ags_fx_raven_synth_audio_finalize;

  /* properties */
  /**
   * AgsFxRavenSynthAudio:synth-0-oscillator:
   *
   * The synth-0 oscillator.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-oscillator",
				   i18n_pspec("synth-0 oscillator of recall"),
				   i18n_pspec("The synth-0's oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-octave:
   *
   * The synth-0 octave.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-octave",
				   i18n_pspec("synth-0 octave of recall"),
				   i18n_pspec("The synth-0's octave"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_OCTAVE,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-key:
   *
   * The synth-0 key.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-key",
				   i18n_pspec("synth-0 key of recall"),
				   i18n_pspec("The synth-0's key"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_KEY,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-phase:
   *
   * The synth-0 phase.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-phase",
				   i18n_pspec("synth-0 phase of recall"),
				   i18n_pspec("The synth-0's phase"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_PHASE,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-volume:
   *
   * The synth-0 volume.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-volume",
				   i18n_pspec("synth-0 volume of recall"),
				   i18n_pspec("The synth-0's volume"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_VOLUME,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-0:
   *
   * The synth-0 sequencer tuning 0.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-0",
				   i18n_pspec("synth-0 seq tuning 0 of recall"),
				   i18n_pspec("The synth-0's seq tuning 0"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_0,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-1:
   *
   * The synth-0 sequencer tuning 1.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-1",
				   i18n_pspec("synth-0 seq tuning 1 of recall"),
				   i18n_pspec("The synth-0's seq tuning 1"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_1,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-2:
   *
   * The synth-0 sequencer tuning 2.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-2",
				   i18n_pspec("synth-0 seq tuning 2 of recall"),
				   i18n_pspec("The synth-0's seq tuning 2"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_2,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-3:
   *
   * The synth-0 sequencer tuning 3.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-3",
				   i18n_pspec("synth-0 seq tuning 3 of recall"),
				   i18n_pspec("The synth-0's seq tuning 3"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_3,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-4:
   *
   * The synth-0 sequencer tuning 4.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-4",
				   i18n_pspec("synth-0 seq tuning 4 of recall"),
				   i18n_pspec("The synth-0's seq tuning 4"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_4,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-5:
   *
   * The synth-0 sequencer tuning 5.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-5",
				   i18n_pspec("synth-0 seq tuning 5 of recall"),
				   i18n_pspec("The synth-0's seq tuning 5"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_5,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-6:
   *
   * The synth-0 sequencer tuning 6.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-6",
				   i18n_pspec("synth-0 seq tuning 6 of recall"),
				   i18n_pspec("The synth-0's seq tuning 6"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_6,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-7:
   *
   * The synth-0 sequencer tuning 7.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-7",
				   i18n_pspec("synth-0 seq tuning 7 of recall"),
				   i18n_pspec("The synth-0's seq tuning 7"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_7,
				  param_spec);
  
  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-8:
   *
   * The synth-0 sequencer tuning 8.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-8",
				   i18n_pspec("synth-0 seq tuning 8 of recall"),
				   i18n_pspec("The synth-0's seq tuning 8"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_8,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-9:
   *
   * The synth-0 sequencer tuning 9.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-9",
				   i18n_pspec("synth-0 seq tuning 9 of recall"),
				   i18n_pspec("The synth-0's seq tuning 9"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_9,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-10:
   *
   * The synth-0 sequencer tuning 10.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-10",
				   i18n_pspec("synth-0 seq tuning 10 of recall"),
				   i18n_pspec("The synth-0's seq tuning 10"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_10,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-11:
   *
   * The synth-0 sequencer tuning 11.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-11",
				   i18n_pspec("synth-0 seq tuning 11 of recall"),
				   i18n_pspec("The synth-0's seq tuning 11"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_11,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-12:
   *
   * The synth-0 sequencer tuning 12.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-12",
				   i18n_pspec("synth-0 seq tuning 12 of recall"),
				   i18n_pspec("The synth-0's seq tuning 12"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_12,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-13:
   *
   * The synth-0 sequencer tuning 13.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-13",
				   i18n_pspec("synth-0 seq tuning 13 of recall"),
				   i18n_pspec("The synth-0's seq tuning 13"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_13,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-14:
   *
   * The synth-0 sequencer tuning 14.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-14",
				   i18n_pspec("synth-0 seq tuning 14 of recall"),
				   i18n_pspec("The synth-0's seq tuning 14"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_14,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-15:
   *
   * The synth-0 sequencer tuning 15.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-15",
				   i18n_pspec("synth-0 seq tuning 15 of recall"),
				   i18n_pspec("The synth-0's seq tuning 15"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_15,
				  param_spec);
  
  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-pingpong:
   *
   * The synth-0 sequencer tuning pingpong.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-pingpong",
				   i18n_pspec("synth-0 seq tuning pingpong of recall"),
				   i18n_pspec("The synth-0's seq tuning pingpong"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_PINGPONG,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-tuning-lfo-frequency:
   *
   * The synth-0 sequencer tuning lfo-frequency.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-tuning-lfo-frequency",
				   i18n_pspec("synth-0 seq tuning LFO frequency of recall"),
				   i18n_pspec("The synth-0's seq tuning LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_TUNING_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-0:
   *
   * The synth-0 sequencer volume 0.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-0",
				   i18n_pspec("synth-0 seq volume 0 of recall"),
				   i18n_pspec("The synth-0's seq volume 0"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_0,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-1:
   *
   * The synth-0 sequencer volume 1.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-1",
				   i18n_pspec("synth-0 seq volume 1 of recall"),
				   i18n_pspec("The synth-0's seq volume 1"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_1,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-2:
   *
   * The synth-0 sequencer volume 2.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-2",
				   i18n_pspec("synth-0 seq volume 2 of recall"),
				   i18n_pspec("The synth-0's seq volume 2"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_2,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-3:
   *
   * The synth-0 sequencer volume 3.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-3",
				   i18n_pspec("synth-0 seq volume 3 of recall"),
				   i18n_pspec("The synth-0's seq volume 3"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_3,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-4:
   *
   * The synth-0 sequencer volume 4.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-4",
				   i18n_pspec("synth-0 seq volume 4 of recall"),
				   i18n_pspec("The synth-0's seq volume 4"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_4,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-5:
   *
   * The synth-0 sequencer volume 5.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-5",
				   i18n_pspec("synth-0 seq volume 5 of recall"),
				   i18n_pspec("The synth-0's seq volume 5"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_5,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-6:
   *
   * The synth-0 sequencer volume 6.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-6",
				   i18n_pspec("synth-0 seq volume 6 of recall"),
				   i18n_pspec("The synth-0's seq volume 6"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_6,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-7:
   *
   * The synth-0 sequencer volume 7.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-7",
				   i18n_pspec("synth-0 seq volume 7 of recall"),
				   i18n_pspec("The synth-0's seq volume 7"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_7,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-8:
   *
   * The synth-0 sequencer volume 8.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-8",
				   i18n_pspec("synth-0 seq volume 8 of recall"),
				   i18n_pspec("The synth-0's seq volume 8"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_8,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-9:
   *
   * The synth-0 sequencer volume 9.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-9",
				   i18n_pspec("synth-0 seq volume 9 of recall"),
				   i18n_pspec("The synth-0's seq volume 9"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_9,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-10:
   *
   * The synth-0 sequencer volume 10.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-10",
				   i18n_pspec("synth-0 seq volume 10 of recall"),
				   i18n_pspec("The synth-0's seq volume 10"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_10,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-11:
   *
   * The synth-0 sequencer volume 11.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-11",
				   i18n_pspec("synth-0 seq volume 11 of recall"),
				   i18n_pspec("The synth-0's seq volume 11"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_11,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-12:
   *
   * The synth-0 sequencer volume 12.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-12",
				   i18n_pspec("synth-0 seq volume 12 of recall"),
				   i18n_pspec("The synth-0's seq volume 12"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_12,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-13:
   *
   * The synth-0 sequencer volume 13.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-13",
				   i18n_pspec("synth-0 seq volume 13 of recall"),
				   i18n_pspec("The synth-0's seq volume 13"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_13,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-14:
   *
   * The synth-0 sequencer volume 14.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-14",
				   i18n_pspec("synth-0 seq volume 14 of recall"),
				   i18n_pspec("The synth-0's seq volume 14"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_14,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-15:
   *
   * The synth-0 sequencer volume 15.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-15",
				   i18n_pspec("synth-0 seq volume 15 of recall"),
				   i18n_pspec("The synth-0's seq volume 15"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_15,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-pingpong:
   *
   * The synth-0 sequencer volume pingpong.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-pingpong",
				   i18n_pspec("synth-0 seq volume pingpong of recall"),
				   i18n_pspec("The synth-0's seq volume pingpong"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_PINGPONG,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-seq-volume-lfo-frequency:
   *
   * The synth-0 sequencer volume lfo-frequency.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-seq-volume-lfo-frequency",
				   i18n_pspec("synth-0 seq volume LFO frequency of recall"),
				   i18n_pspec("The synth-0's seq volume LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SEQ_VOLUME_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-sync-enabled:
   *
   * The synth-0 sync enabled.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-sync-enabled",
				   i18n_pspec("synth-0 sync enabled of recall"),
				   i18n_pspec("The synth-0's sync enabled"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_ENABLED,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-sync-relative-attack-factor-0:
   *
   * The synth-0 sync relative attack factor 0.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-sync-relative-attack-factor-0",
				   i18n_pspec("synth-0 sync relative attack factor 0 of recall"),
				   i18n_pspec("The synth-0's sync relative attack factor 0"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_0,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-sync-attack-0:
   *
   * The synth-0 sync attack-0.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-sync-attack-0",
				   i18n_pspec("synth-0 sync attack-0 of recall"),
				   i18n_pspec("The synth-0's sync attack-0"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_ATTACK_0,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-sync-phase-0:
   *
   * The synth-0 sync phase-0.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-sync-phase-0",
				   i18n_pspec("synth-0 sync phase-0 of recall"),
				   i18n_pspec("The synth-0's sync phase-0"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_PHASE_0,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-sync-relative-attack-factor-1:
   *
   * The synth-0 sync relative attack factor 1.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-sync-relative-attack-factor-1",
				   i18n_pspec("synth-0 sync relative attack factor 1 of recall"),
				   i18n_pspec("The synth-0's sync relative attack factor 1"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_1,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-sync-attack-1:
   *
   * The synth-0 sync attack-1.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-sync-attack-1",
				   i18n_pspec("synth-0 sync attack-1 of recall"),
				   i18n_pspec("The synth-0's sync attack-1"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_ATTACK_1,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-sync-phase-1:
   *
   * The synth-0 sync phase-1.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-sync-phase-1",
				   i18n_pspec("synth-0 sync phase-1 of recall"),
				   i18n_pspec("The synth-0's sync phase-1"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_PHASE_1,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-sync-relative-attack-factor-1:
   *
   * The synth-0 sync relative attack factor 1.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-sync-relative-attack-factor-2",
				   i18n_pspec("synth-0 sync relative attack factor 2 of recall"),
				   i18n_pspec("The synth-0's sync relative attack factor 2"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_2,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-sync-attack-2:
   *
   * The synth-0 sync attack-2.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-sync-attack-2",
				   i18n_pspec("synth-0 sync attack-2 of recall"),
				   i18n_pspec("The synth-0's sync attack-2"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_ATTACK_2,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-sync-phase-2:
   *
   * The synth-0 sync phase-2.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-sync-phase-2",
				   i18n_pspec("synth-0 sync phase-2 of recall"),
				   i18n_pspec("The synth-0's sync phase-2"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_PHASE_2,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-sync-relative-attack-factor-3:
   *
   * The synth-0 sync relative attack factor 3.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-sync-relative-attack-factor-3",
				   i18n_pspec("synth-0 sync relative attack factor 3 of recall"),
				   i18n_pspec("The synth-0's sync relative attack factor 3"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_3,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-sync-attack-3:
   *
   * The synth-0 sync attack-3.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-sync-attack-3",
				   i18n_pspec("synth-0 sync attack-3 of recall"),
				   i18n_pspec("The synth-0's sync attack-3"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_ATTACK_3,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-sync-phase-3:
   *
   * The synth-0 sync phase-3.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-sync-phase-3",
				   i18n_pspec("synth-0 sync phase-3 of recall"),
				   i18n_pspec("The synth-0's sync phase-3"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_PHASE_3,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-lfo-oscillator:
   *
   * The synth-0 lfo-oscillator.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-oscillator",
				   i18n_pspec("synth-0 LFO oscillator of recall"),
				   i18n_pspec("The synth-0's LFO oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-lfo-frequency:
   *
   * The synth-0 LFO frequency.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-frequency",
				   i18n_pspec("synth-0 LFO frequency of recall"),
				   i18n_pspec("The synth-0's LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-lfo-depth:
   *
   * The synth-0 LFO depth.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-depth",
				   i18n_pspec("synth-0 LFO depth of recall"),
				   i18n_pspec("The synth-0's LFO depth"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_DEPTH,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-0-lfo-tuning:
   *
   * The synth-0 LFO tuning.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-tuning",
				   i18n_pspec("synth-0 LFO tuning of recall"),
				   i18n_pspec("The synth-0's LFO tuning"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_LFO_TUNING,
				  param_spec); 

  /**
   * AgsFxRavenSynthAudio:synth-1-oscillator:
   *
   * The synth-1 oscillator.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-oscillator",
				   i18n_pspec("synth-1 oscillator of recall"),
				   i18n_pspec("The synth-1's oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-octave:
   *
   * The synth-1 octave.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-octave",
				   i18n_pspec("synth-1 octave of recall"),
				   i18n_pspec("The synth-1's octave"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_OCTAVE,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-key:
   *
   * The synth-1 key.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-key",
				   i18n_pspec("synth-1 key of recall"),
				   i18n_pspec("The synth-1's key"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_KEY,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-phase:
   *
   * The synth-1 phase.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-phase",
				   i18n_pspec("synth-1 phase of recall"),
				   i18n_pspec("The synth-1's phase"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_PHASE,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-volume:
   *
   * The synth-1 volume.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-volume",
				   i18n_pspec("synth-1 volume of recall"),
				   i18n_pspec("The synth-1's volume"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_VOLUME,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-0:
   *
   * The synth-1 sequencer tuning 0.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-0",
				   i18n_pspec("synth-1 seq tuning 0 of recall"),
				   i18n_pspec("The synth-1's seq tuning 0"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_0,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-1:
   *
   * The synth-1 sequencer tuning 1.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-1",
				   i18n_pspec("synth-1 seq tuning 1 of recall"),
				   i18n_pspec("The synth-1's seq tuning 1"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_1,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-2:
   *
   * The synth-1 sequencer tuning 2.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-2",
				   i18n_pspec("synth-1 seq tuning 2 of recall"),
				   i18n_pspec("The synth-1's seq tuning 2"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_2,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-3:
   *
   * The synth-1 sequencer tuning 3.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-3",
				   i18n_pspec("synth-1 seq tuning 3 of recall"),
				   i18n_pspec("The synth-1's seq tuning 3"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_3,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-4:
   *
   * The synth-1 sequencer tuning 4.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-4",
				   i18n_pspec("synth-1 seq tuning 4 of recall"),
				   i18n_pspec("The synth-1's seq tuning 4"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_4,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-5:
   *
   * The synth-1 sequencer tuning 5.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-5",
				   i18n_pspec("synth-1 seq tuning 5 of recall"),
				   i18n_pspec("The synth-1's seq tuning 5"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_5,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-6:
   *
   * The synth-1 sequencer tuning 6.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-6",
				   i18n_pspec("synth-1 seq tuning 6 of recall"),
				   i18n_pspec("The synth-1's seq tuning 6"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_6,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-7:
   *
   * The synth-1 sequencer tuning 7.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-7",
				   i18n_pspec("synth-1 seq tuning 7 of recall"),
				   i18n_pspec("The synth-1's seq tuning 7"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_7,
				  param_spec);
  
  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-8:
   *
   * The synth-1 sequencer tuning 8.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-8",
				   i18n_pspec("synth-1 seq tuning 8 of recall"),
				   i18n_pspec("The synth-1's seq tuning 8"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_8,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-9:
   *
   * The synth-1 sequencer tuning 9.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-9",
				   i18n_pspec("synth-1 seq tuning 9 of recall"),
				   i18n_pspec("The synth-1's seq tuning 9"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_9,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-10:
   *
   * The synth-1 sequencer tuning 10.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-10",
				   i18n_pspec("synth-1 seq tuning 10 of recall"),
				   i18n_pspec("The synth-1's seq tuning 10"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_10,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-11:
   *
   * The synth-1 sequencer tuning 11.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-11",
				   i18n_pspec("synth-1 seq tuning 11 of recall"),
				   i18n_pspec("The synth-1's seq tuning 11"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_11,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-12:
   *
   * The synth-1 sequencer tuning 12.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-12",
				   i18n_pspec("synth-1 seq tuning 12 of recall"),
				   i18n_pspec("The synth-1's seq tuning 12"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_12,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-13:
   *
   * The synth-1 sequencer tuning 13.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-13",
				   i18n_pspec("synth-1 seq tuning 13 of recall"),
				   i18n_pspec("The synth-1's seq tuning 13"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_13,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-14:
   *
   * The synth-1 sequencer tuning 14.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-14",
				   i18n_pspec("synth-1 seq tuning 14 of recall"),
				   i18n_pspec("The synth-1's seq tuning 14"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_14,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-15:
   *
   * The synth-1 sequencer tuning 15.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-15",
				   i18n_pspec("synth-1 seq tuning 15 of recall"),
				   i18n_pspec("The synth-1's seq tuning 15"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_15,
				  param_spec);
  
  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-pingpong:
   *
   * The synth-1 sequencer tuning pingpong.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-pingpong",
				   i18n_pspec("synth-1 seq tuning pingpong of recall"),
				   i18n_pspec("The synth-1's seq tuning pingpong"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_PINGPONG,
				  param_spec);
  
  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-0:
   *
   * The synth-1 sequencer volume 0.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-0",
				   i18n_pspec("synth-1 seq volume 0 of recall"),
				   i18n_pspec("The synth-1's seq volume 0"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_0,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-1:
   *
   * The synth-1 sequencer volume 1.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-1",
				   i18n_pspec("synth-1 seq volume 1 of recall"),
				   i18n_pspec("The synth-1's seq volume 1"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_1,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-2:
   *
   * The synth-1 sequencer volume 2.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-2",
				   i18n_pspec("synth-1 seq volume 2 of recall"),
				   i18n_pspec("The synth-1's seq volume 2"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_2,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-3:
   *
   * The synth-1 sequencer volume 3.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-3",
				   i18n_pspec("synth-1 seq volume 3 of recall"),
				   i18n_pspec("The synth-1's seq volume 3"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_3,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-4:
   *
   * The synth-1 sequencer volume 4.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-4",
				   i18n_pspec("synth-1 seq volume 4 of recall"),
				   i18n_pspec("The synth-1's seq volume 4"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_4,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-5:
   *
   * The synth-1 sequencer volume 5.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-5",
				   i18n_pspec("synth-1 seq volume 5 of recall"),
				   i18n_pspec("The synth-1's seq volume 5"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_5,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-6:
   *
   * The synth-1 sequencer volume 6.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-6",
				   i18n_pspec("synth-1 seq volume 6 of recall"),
				   i18n_pspec("The synth-1's seq volume 6"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_6,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-7:
   *
   * The synth-1 sequencer volume 7.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-7",
				   i18n_pspec("synth-1 seq volume 7 of recall"),
				   i18n_pspec("The synth-1's seq volume 7"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_7,
				  param_spec);


  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-8:
   *
   * The synth-1 sequencer volume 8.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-8",
				   i18n_pspec("synth-1 seq volume 8 of recall"),
				   i18n_pspec("The synth-1's seq volume 8"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_8,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-9:
   *
   * The synth-1 sequencer volume 9.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-9",
				   i18n_pspec("synth-1 seq volume 9 of recall"),
				   i18n_pspec("The synth-1's seq volume 9"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_9,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-10:
   *
   * The synth-1 sequencer volume 10.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-10",
				   i18n_pspec("synth-1 seq volume 10 of recall"),
				   i18n_pspec("The synth-1's seq volume 10"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_10,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-11:
   *
   * The synth-1 sequencer volume 11.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-11",
				   i18n_pspec("synth-1 seq volume 11 of recall"),
				   i18n_pspec("The synth-1's seq volume 11"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_11,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-12:
   *
   * The synth-1 sequencer volume 12.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-12",
				   i18n_pspec("synth-1 seq volume 12 of recall"),
				   i18n_pspec("The synth-1's seq volume 12"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_12,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-13:
   *
   * The synth-1 sequencer volume 13.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-13",
				   i18n_pspec("synth-1 seq volume 13 of recall"),
				   i18n_pspec("The synth-1's seq volume 13"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_13,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-14:
   *
   * The synth-1 sequencer volume 14.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-14",
				   i18n_pspec("synth-1 seq volume 14 of recall"),
				   i18n_pspec("The synth-1's seq volume 14"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_14,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-15:
   *
   * The synth-1 sequencer volume 15.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-15",
				   i18n_pspec("synth-1 seq volume 15 of recall"),
				   i18n_pspec("The synth-1's seq volume 15"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_15,
				  param_spec);
  
  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-pingpong:
   *
   * The synth-1 sequencer volume pingpong.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-pingpong",
				   i18n_pspec("synth-1 seq volume pingpong of recall"),
				   i18n_pspec("The synth-1's seq volume pingpong"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_PINGPONG,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-volume-lfo-frequency:
   *
   * The synth-1 sequencer volume lfo-frequency.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-volume-lfo-frequency",
				   i18n_pspec("synth-1 seq volume LFO frequency of recall"),
				   i18n_pspec("The synth-1's seq volume LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_VOLUME_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-seq-tuning-lfo-frequency:
   *
   * The synth-1 sequencer tuning lfo-frequency.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-seq-tuning-lfo-frequency",
				   i18n_pspec("synth-1 seq tuning LFO frequency of recall"),
				   i18n_pspec("The synth-1's seq tuning LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SEQ_TUNING_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-sync-enabled:
   *
   * The synth-1 sync enabled.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-sync-enabled",
				   i18n_pspec("synth-1 sync enabled of recall"),
				   i18n_pspec("The synth-1's sync enabled"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_ENABLED,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-sync-relative-attack-factor-0:
   *
   * The synth-1 sync relative attack factor 0.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-sync-relative-attack-factor-0",
				   i18n_pspec("synth-1 sync relative attack factor 0 of recall"),
				   i18n_pspec("The synth-1's sync relative attack factor 0"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_0,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-sync-attack-0:
   *
   * The synth-1 sync attack-0.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-sync-attack-0",
				   i18n_pspec("synth-1 sync attack-0 of recall"),
				   i18n_pspec("The synth-1's sync attack-0"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_ATTACK_0,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-sync-phase-0:
   *
   * The synth-1 sync phase-0.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-sync-phase-0",
				   i18n_pspec("synth-1 sync phase-0 of recall"),
				   i18n_pspec("The synth-1's sync phase-0"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_PHASE_0,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-sync-relative-attack-factor-1:
   *
   * The synth-1 sync relative attack factor 1.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-sync-relative-attack-factor-1",
				   i18n_pspec("synth-1 sync relative attack factor 1 of recall"),
				   i18n_pspec("The synth-1's sync relative attack factor 1"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_1,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-sync-attack-1:
   *
   * The synth-1 sync attack-1.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-sync-attack-1",
				   i18n_pspec("synth-1 sync attack-1 of recall"),
				   i18n_pspec("The synth-1's sync attack-1"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_ATTACK_1,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-sync-phase-1:
   *
   * The synth-1 sync phase-1.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-sync-phase-1",
				   i18n_pspec("synth-1 sync phase-1 of recall"),
				   i18n_pspec("The synth-1's sync phase-1"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_PHASE_1,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-sync-relative-attack-factor-2:
   *
   * The synth-1 sync relative attack factor 2.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-sync-relative-attack-factor-2",
				   i18n_pspec("synth-1 sync relative attack factor 2 of recall"),
				   i18n_pspec("The synth-1's sync relative attack factor 2"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_2,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-sync-attack-2:
   *
   * The synth-1 sync attack-2.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-sync-attack-2",
				   i18n_pspec("synth-1 sync attack-2 of recall"),
				   i18n_pspec("The synth-1's sync attack-2"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_ATTACK_2,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-sync-phase-2:
   *
   * The synth-1 sync phase-2.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-sync-phase-2",
				   i18n_pspec("synth-1 sync phase-2 of recall"),
				   i18n_pspec("The synth-1's sync phase-2"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_PHASE_2,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-sync-relative-attack-factor-3:
   *
   * The synth-1 sync relative attack factor 3.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-sync-relative-attack-factor-3",
				   i18n_pspec("synth-1 sync relative attack factor 3 of recall"),
				   i18n_pspec("The synth-1's sync relative attack factor 3"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_3,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-sync-attack-3:
   *
   * The synth-1 sync attack-3.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-sync-attack-3",
				   i18n_pspec("synth-1 sync attack-3 of recall"),
				   i18n_pspec("The synth-1's sync attack-3"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_ATTACK_3,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-sync-phase-3:
   *
   * The synth-1 sync phase-3.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-sync-phase-3",
				   i18n_pspec("synth-1 sync phase-3 of recall"),
				   i18n_pspec("The synth-1's sync phase-3"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_PHASE_3,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-lfo-oscillator:
   *
   * The synth-1 lfo-oscillator.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-lfo-oscillator",
				   i18n_pspec("synth-1 LFO oscillator of recall"),
				   i18n_pspec("The synth-1's LFO oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_LFO_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-lfo-frequency:
   *
   * The synth-1 LFO frequency.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-lfo-frequency",
				   i18n_pspec("synth-1 LFO frequency of recall"),
				   i18n_pspec("The synth-1's LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-lfo-depth:
   *
   * The synth-1 LFO depth.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-lfo-depth",
				   i18n_pspec("synth-1 LFO depth of recall"),
				   i18n_pspec("The synth-1's LFO depth"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_LFO_DEPTH,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:synth-1-lfo-tuning:
   *
   * The synth-1 LFO tuning.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("synth-1-lfo-tuning",
				   i18n_pspec("synth-1 LFO tuning of recall"),
				   i18n_pspec("The synth-1's LFO tuning"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_LFO_TUNING,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:low-pass-0-cut-off-frequency:
   *
   * The low-pass-0-cut-off-frequency.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:low-pass-0-filter-gain:
   *
   * The low-pass-0-filter-gain.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:low-pass-0-no-clip:
   *
   * The low-pass-0-no-clip.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:low-pass-1-cut-off-frequency:
   *
   * The low-pass-1-cut-off-frequency.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("low-pass-1-cut-off-frequency",
				   i18n_pspec("low-pass 1 cut off frequency of recall"),
				   i18n_pspec("The low-pass 1 cut off frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOW_PASS_1_CUT_OFF_FREQUENCY,
				  param_spec);
  
  /**
   * AgsFxRavenSynthAudio:low-pass-1-filter-gain:
   *
   * The low-pass-1-filter-gain.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("low-pass-1-filter-gain",
				   i18n_pspec("low-pass 1 filter gain of recall"),
				   i18n_pspec("The low-pass 1 filter gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOW_PASS_1_FILTER_GAIN,
				  param_spec);
  
  /**
   * AgsFxRavenSynthAudio:low-pass-1-no-clip:
   *
   * The low-pass-1-no-clip.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("low-pass-1-no-clip",
				   i18n_pspec("low-pass 1 no-clip of recall"),
				   i18n_pspec("The low-pass 1 no clip"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOW_PASS_1_NO_CLIP,
				  param_spec);
  
  /**
   * AgsFxRavenSynthAudio:amplifier-0-amp-0-gain:
   *
   * The amplifier 0 amp-0 gain.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:amplifier-0-amp-1-gain:
   *
   * The amplifier 0 amp-1 gain.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:amplifier-0-amp-2-gain:
   *
   * The amplifier 0 amp-2 gain.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:amplifier-0-amp-3-gain:
   *
   * The amplifier 0 amp-3 gain.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:amplifier-0-filter-gain:
   *
   * The amplifier 0 filter gain.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:amplifier-1-amp-0-gain:
   *
   * The amplifier 1 amp-0 gain.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("amplifier-1-amp-0-gain",
				   i18n_pspec("amplifier 1 amp-0 gain of recall"),
				   i18n_pspec("The amplifier 1 amp-0 gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AMPLIFIER_1_AMP_0_GAIN,
				  param_spec);
  
  /**
   * AgsFxRavenSynthAudio:amplifier-1-amp-1-gain:
   *
   * The amplifier 1 amp-1 gain.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("amplifier-1-amp-1-gain",
				   i18n_pspec("amplifier 1 amp-1 gain of recall"),
				   i18n_pspec("The amplifier 1 amp-1 gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AMPLIFIER_1_AMP_1_GAIN,
				  param_spec);
  
  /**
   * AgsFxRavenSynthAudio:amplifier-1-amp-2-gain:
   *
   * The amplifier 1 amp-2 gain.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("amplifier-1-amp-2-gain",
				   i18n_pspec("amplifier 1 amp-2 gain of recall"),
				   i18n_pspec("The amplifier 1 amp-2 gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AMPLIFIER_1_AMP_2_GAIN,
				  param_spec);
  
  /**
   * AgsFxRavenSynthAudio:amplifier-1-amp-3-gain:
   *
   * The amplifier 1 amp-3 gain.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("amplifier-1-amp-3-gain",
				   i18n_pspec("amplifier 1 amp-3 gain of recall"),
				   i18n_pspec("The amplifier 1 amp-3 gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AMPLIFIER_1_AMP_3_GAIN,
				  param_spec);
  
  /**
   * AgsFxRavenSynthAudio:amplifier-1-filter-gain:
   *
   * The amplifier 1 filter gain.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("amplifier-1-filter-gain",
				   i18n_pspec("amplifier 1 filter gain of recall"),
				   i18n_pspec("The amplifier 1 filter gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AMPLIFIER_1_FILTER_GAIN,
				  param_spec);
  
  /**
   * AgsFxRavenSynthAudio:noise-gain:
   *
   * The noise gain.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("noise-gain",
				   i18n_pspec("noise gain of recall"),
				   i18n_pspec("The noise gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOISE_GAIN,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:pitch-type:
   *
   * The pitch type.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("pitch-type",
				   i18n_pspec("pitch type of recall"),
				   i18n_pspec("The pitch type"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PITCH_TYPE,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:pitch-tuning:
   *
   * The pitch tuning.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("pitch-tuning",
				   i18n_pspec("pitch tuning of recall"),
				   i18n_pspec("The pitch tuning"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PITCH_TUNING,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:chorus-enabled:
   *
   * The chorus enabled.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:chorus-pitch-type:
   *
   * The chorus pitch type.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:chorus-input-volume:
   *
   * The chorus input volume.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:chorus-output-volume:
   *
   * The chorus output volume.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:chorus-lfo-oscillator:
   *
   * The chorus lfo oscillator.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:chorus-lfo-frequency:
   *
   * The chorus lfo frequency.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:chorus-depth:
   *
   * The chorus depth.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:chorus-mix:
   *
   * The chorus mix.
   * 
   * Since: 7.7.0
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
   * AgsFxRavenSynthAudio:chorus-delay:
   *
   * The chorus delay.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("chorus-delay",
				   i18n_pspec("chorus delay of recall"),
				   i18n_pspec("The chorus delay"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHORUS_DELAY,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:vibrato-enabled:
   *
   * The vibrato enabled.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("vibrato-enabled",
				   i18n_pspec("vibrato enabled of recall"),
				   i18n_pspec("The vibrato enabled"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VIBRATO_ENABLED,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:vibrato-gain:
   *
   * The vibrato gain.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("vibrato-gain",
				   i18n_pspec("vibrato gain of recall"),
				   i18n_pspec("The vibrato gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VIBRATO_GAIN,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:vibrato-lfo-depth:
   *
   * The vibrato LFO depth.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("vibrato-lfo-depth",
				   i18n_pspec("vibrato LFO depth of recall"),
				   i18n_pspec("The vibrato LFO depth"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VIBRATO_LFO_DEPTH,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:vibrato-lfo-freq:
   *
   * The vibrato LFO freq.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("vibrato-lfo-freq",
				   i18n_pspec("vibrato LFO freq of recall"),
				   i18n_pspec("The vibrato LFO freq"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VIBRATO_LFO_FREQ,
				  param_spec);

  /**
   * AgsFxRavenSynthAudio:vibrato-tuning:
   *
   * The vibrato tuning.
   * 
   * Since: 7.7.0
   */
  param_spec = g_param_spec_object("vibrato-tuning",
				   i18n_pspec("vibrato tuning of recall"),
				   i18n_pspec("The vibrato tuning"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VIBRATO_TUNING,
				  param_spec);
}

void
ags_fx_raven_synth_audio_init(AgsFxRavenSynthAudio *fx_raven_synth_audio)
{
  gint position;
  guint i;
  
  g_signal_connect(fx_raven_synth_audio, "notify::audio",
		   G_CALLBACK(ags_fx_raven_synth_audio_notify_audio_callback), NULL);

  g_signal_connect(fx_raven_synth_audio, "notify::buffer-size",
		   G_CALLBACK(ags_fx_raven_synth_audio_notify_buffer_size_callback), NULL);

  g_signal_connect(fx_raven_synth_audio, "notify::format",
		   G_CALLBACK(ags_fx_raven_synth_audio_notify_format_callback), NULL);

  g_signal_connect(fx_raven_synth_audio, "notify::samplerate",
		   G_CALLBACK(ags_fx_raven_synth_audio_notify_samplerate_callback), NULL);

  AGS_RECALL(fx_raven_synth_audio)->name = "ags-fx-raven-synth";
  AGS_RECALL(fx_raven_synth_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_raven_synth_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_raven_synth_audio)->xml_type = "ags-fx-raven-synth-audio";

  position = 0;

  /* synth-0 oscillator */
  fx_raven_synth_audio->synth_0_oscillator = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							  "specifier", "./synth-0-oscillator[0]",
							  "control-port", "1/121",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_raven_synth_audio->synth_0_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_raven_synth_audio->synth_0_oscillator,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_oscillator);

  position++;

  /* synth-0 octave */
  fx_raven_synth_audio->synth_0_octave = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						      "specifier", "./synth-0-octave[0]",
						      "control-port", "2/121",
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_raven_synth_audio->synth_0_octave->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_octave,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_octave);

  position++;

  /* synth-0 key */
  fx_raven_synth_audio->synth_0_key = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						   "specifier", "./synth-0-key[0]",
						   "control-port", "3/121",
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_raven_synth_audio->synth_0_key->port_value.ags_port_float = (gfloat) 2.0;

  g_object_set(fx_raven_synth_audio->synth_0_key,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_key);

  position++;

  /* synth-0 phase */
  fx_raven_synth_audio->synth_0_phase = g_object_new(AGS_TYPE_PORT,
						     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						     "specifier", "./synth-0-phase[0]",
						     "control-port", "4/121",
						     "port-value-is-pointer", FALSE,
						     "port-value-type", G_TYPE_FLOAT,
						     "port-value-size", sizeof(gfloat),
						     "port-value-length", 1,
						     NULL);
  
  fx_raven_synth_audio->synth_0_phase->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_phase,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_phase);

  position++;

  /* synth-0 volume */
  fx_raven_synth_audio->synth_0_volume = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						      "specifier", "./synth-0-volume[0]",
						      "control-port", "5/121",
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_raven_synth_audio->synth_0_volume->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_raven_synth_audio->synth_0_volume,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_volume);

  position++;

  /* synth-0 seq tuning 0 */
  fx_raven_synth_audio->synth_0_seq_tuning_0 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-0[0]",
							    "control-port", "6/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_0->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_0,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_0);

  position++;

  /* synth-0 seq tuning 1 */
  fx_raven_synth_audio->synth_0_seq_tuning_1 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-1[0]",
							    "control-port", "7/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_1->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_1,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_1);

  position++;

  /* synth-0 seq tuning 2 */
  fx_raven_synth_audio->synth_0_seq_tuning_2 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-2[0]",
							    "control-port", "8/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_2->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_2,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_2);

  position++;

  /* synth-0 seq tuning 3 */
  fx_raven_synth_audio->synth_0_seq_tuning_3 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-3[0]",
							    "control-port", "9/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_3->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_3,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_3);

  position++;

  /* synth-0 seq tuning 4 */
  fx_raven_synth_audio->synth_0_seq_tuning_4 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-4[0]",
							    "control-port", "10/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_4->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_4,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_4);

  position++;

  /* synth-0 seq tuning 5 */
  fx_raven_synth_audio->synth_0_seq_tuning_5 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-5[0]",
							    "control-port", "11/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_5->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_5,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_5);

  position++;

  /* synth-0 seq tuning 6 */
  fx_raven_synth_audio->synth_0_seq_tuning_6 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-6[0]",
							    "control-port", "12/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_6->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_6,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_6);

  position++;

  /* synth-0 seq tuning 7 */
  fx_raven_synth_audio->synth_0_seq_tuning_7 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-7[0]",
							    "control-port", "13/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_7->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_7,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_7);

  position++;

  /* synth-0 seq tuning 8 */
  fx_raven_synth_audio->synth_0_seq_tuning_8 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-8[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_8->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_8,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_8);

  position++;

  /* synth-0 seq tuning 9 */
  fx_raven_synth_audio->synth_0_seq_tuning_9 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-9[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_9->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_9,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_9);

  position++;

  /* synth-0 seq tuning 10 */
  fx_raven_synth_audio->synth_0_seq_tuning_10 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-10[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_10->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_10,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_10);

  position++;

  /* synth-0 seq tuning 11 */
  fx_raven_synth_audio->synth_0_seq_tuning_11 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-11[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_11->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_11,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_11);

  position++;

  /* synth-0 seq tuning 12 */
  fx_raven_synth_audio->synth_0_seq_tuning_12 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-12[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_12->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_12,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_12);

  position++;

  /* synth-0 seq tuning 13 */
  fx_raven_synth_audio->synth_0_seq_tuning_13 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-0[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_13->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_13,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_13);

  position++;

  /* synth-0 seq tuning 14 */
  fx_raven_synth_audio->synth_0_seq_tuning_14 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-14[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_14->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_14,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_14);

  position++;

  /* synth-0 seq tuning 15 */
  fx_raven_synth_audio->synth_0_seq_tuning_15 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-tuning-15[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_15->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_15,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_15);

  position++;

  /* synth-0 seq tuning pingpong */
  fx_raven_synth_audio->synth_0_seq_tuning_pingpong = g_object_new(AGS_TYPE_PORT,
								   "plugin-name", ags_fx_raven_synth_audio_plugin_name,
								   "specifier", "./synth-0-seq-tuning-pingpong[0]",
								   "control-port", "14/121",
								   "port-value-is-pointer", FALSE,
								   "port-value-type", G_TYPE_FLOAT,
								   "port-value-size", sizeof(gfloat),
								   "port-value-length", 1,
								   NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_pingpong->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_pingpong,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_pingpong_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_pingpong);

  position++;

  /* synth-0 seq tuning LFO frequency */
  fx_raven_synth_audio->synth_0_seq_tuning_lfo_frequency = g_object_new(AGS_TYPE_PORT,
									"plugin-name", ags_fx_raven_synth_audio_plugin_name,
									"specifier", "./synth-0-seq-tuning-lfo-frequency[0]",
									"control-port", "15/121",
									"port-value-is-pointer", FALSE,
									"port-value-type", G_TYPE_FLOAT,
									"port-value-size", sizeof(gfloat),
									"port-value-length", 1,
									NULL);
  
  fx_raven_synth_audio->synth_0_seq_tuning_lfo_frequency->port_value.ags_port_float = (gfloat) 8.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_tuning_lfo_frequency,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_tuning_lfo_frequency);

  position++;

  /* synth-0 seq volume 0 */
  fx_raven_synth_audio->synth_0_seq_volume_0 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-0[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_0->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_0,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_0);

  position++;

  /* synth-0 seq volume 1 */
  fx_raven_synth_audio->synth_0_seq_volume_1 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-1[0]",
							    "control-port", "17/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_1->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_1,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_1);

  position++;

  /* synth-0 seq volume 2 */
  fx_raven_synth_audio->synth_0_seq_volume_2 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-2[0]",
							    "control-port", "18/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_2->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_2,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_2);

  position++;

  /* synth-0 seq volume 3 */
  fx_raven_synth_audio->synth_0_seq_volume_3 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-3[0]",
							    "control-port", "19/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_3->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_3,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_3);

  position++;

  /* synth-0 seq volume 4 */
  fx_raven_synth_audio->synth_0_seq_volume_4 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-4[0]",
							    "control-port", "20/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_4->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_4,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_4);

  position++;

  /* synth-0 seq volume 5 */
  fx_raven_synth_audio->synth_0_seq_volume_5 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-5[0]",
							    "control-port", "21/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_5->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_5,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_5);

  position++;

  /* synth-0 seq volume 6 */
  fx_raven_synth_audio->synth_0_seq_volume_6 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-6[0]",
							    "control-port", "22/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_6->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_6,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_6);

  position++;

  /* synth-0 seq volume 7 */
  fx_raven_synth_audio->synth_0_seq_volume_7 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-7[0]",
							    "control-port", "23/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_7->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_7,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_7);

  position++;

  /* synth-0 seq volume 8 */
  fx_raven_synth_audio->synth_0_seq_volume_8 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-8[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_8->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_8,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_8);

  position++;

  /* synth-0 seq volume 9 */
  fx_raven_synth_audio->synth_0_seq_volume_9 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-9[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_9->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_9,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_9);

  position++;

  /* synth-0 seq volume 10 */
  fx_raven_synth_audio->synth_0_seq_volume_10 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-10[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_10->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_10,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_10);

  position++;

  /* synth-0 seq volume 11 */
  fx_raven_synth_audio->synth_0_seq_volume_11 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-11[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_11->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_11,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_11);

  position++;

  /* synth-0 seq volume 12 */
  fx_raven_synth_audio->synth_0_seq_volume_12 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-12[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_12->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_12,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_12);

  position++;

  /* synth-0 seq volume 13 */
  fx_raven_synth_audio->synth_0_seq_volume_13 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-13[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_13->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_13,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_13);

  position++;

  /* synth-0 seq volume 14 */
  fx_raven_synth_audio->synth_0_seq_volume_14 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-14[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_14->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_14,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_14);

  position++;

  /* synth-0 seq volume 15 */
  fx_raven_synth_audio->synth_0_seq_volume_15 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-seq-volume-15[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_15->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_15,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_15);

  position++;

  /* synth-0 seq volume pingpong */
  fx_raven_synth_audio->synth_0_seq_volume_pingpong = g_object_new(AGS_TYPE_PORT,
								   "plugin-name", ags_fx_raven_synth_audio_plugin_name,
								   "specifier", "./synth-0-seq-volume-pingpong[0]",
								   "control-port", "24/121",
								   "port-value-is-pointer", FALSE,
								   "port-value-type", G_TYPE_FLOAT,
								   "port-value-size", sizeof(gfloat),
								   "port-value-length", 1,
								   NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_pingpong->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_pingpong,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_pingpong_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_pingpong);

  position++;

  /* synth-0 seq volume LFO frequency */
  fx_raven_synth_audio->synth_0_seq_volume_lfo_frequency = g_object_new(AGS_TYPE_PORT,
									"plugin-name", ags_fx_raven_synth_audio_plugin_name,
									"specifier", "./synth-0-seq-volume-lfo-frequency[0]",
									"control-port", "25/121",
									"port-value-is-pointer", FALSE,
									"port-value-type", G_TYPE_FLOAT,
									"port-value-size", sizeof(gfloat),
									"port-value-length", 1,
									NULL);
  
  fx_raven_synth_audio->synth_0_seq_volume_lfo_frequency->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_seq_volume_lfo_frequency,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_seq_volume_lfo_frequency);

  position++;
  
  /* synth-0 sync enabled */
  fx_raven_synth_audio->synth_0_sync_enabled = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-sync-enabled[0]",
							    "control-port", "26/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_sync_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_raven_synth_audio->synth_0_sync_enabled,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_sync_enabled);

  position++;

  /* synth-0 sync relative attack factor 0 */
  fx_raven_synth_audio->synth_0_sync_relative_attack_factor_0 = g_object_new(AGS_TYPE_PORT,
									     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
									     "specifier", "./synth-0-sync-relative-factor-0[0]",
									     "control-port", "27/121",
									     "port-value-is-pointer", FALSE,
									     "port-value-type", G_TYPE_FLOAT,
									     "port-value-size", sizeof(gfloat),
									     "port-value-length", 1,
									     NULL);
  
  fx_raven_synth_audio->synth_0_sync_relative_attack_factor_0->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_sync_relative_attack_factor_0,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_relative_attack_factor_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_sync_relative_attack_factor_0);

  position++;

  /* synth-0 sync attack-0 */
  fx_raven_synth_audio->synth_0_sync_attack_0 = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							     "specifier", "./synth-0-sync-attack-0[0]",
							     "control-port", "28/121",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_raven_synth_audio->synth_0_sync_attack_0->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_sync_attack_0,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_attack_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_sync_attack_0);

  position++;

  /* synth-0 sync phase-0 */
  fx_raven_synth_audio->synth_0_sync_phase_0 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-sync-phase-0[0]",
							    "control-port", "29/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_sync_phase_0->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_sync_phase_0,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_sync_phase_0);

  position++;

  /* synth-0 sync relative attack factor 1 */
  fx_raven_synth_audio->synth_0_sync_relative_attack_factor_1 = g_object_new(AGS_TYPE_PORT,
									     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
									     "specifier", "./synth-0-sync-relative-factor-1[0]",
									     "control-port", "30/121",
									     "port-value-is-pointer", FALSE,
									     "port-value-type", G_TYPE_FLOAT,
									     "port-value-size", sizeof(gfloat),
									     "port-value-length", 1,
									     NULL);
  
  fx_raven_synth_audio->synth_0_sync_relative_attack_factor_1->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_sync_relative_attack_factor_1,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_relative_attack_factor_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_sync_relative_attack_factor_1);

  position++;

  /* synth-0 sync attack-1 */
  fx_raven_synth_audio->synth_0_sync_attack_1 = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							     "specifier", "./synth-0-sync-attack-1[0]",
							     "control-port", "31/121",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_raven_synth_audio->synth_0_sync_attack_1->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_sync_attack_1,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_attack_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_sync_attack_1);

  position++;

  /* synth-0 sync phase-1 */
  fx_raven_synth_audio->synth_0_sync_phase_1 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-sync-phase-1[0]",
							    "control-port", "32/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_sync_phase_1->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_sync_phase_1,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_sync_phase_1);

  position++;

  /* synth-0 sync relative attack factor 2 */
  fx_raven_synth_audio->synth_0_sync_relative_attack_factor_2 = g_object_new(AGS_TYPE_PORT,
									     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
									     "specifier", "./synth-0-sync-relative-factor-2[0]",
									     "control-port", "33/121",
									     "port-value-is-pointer", FALSE,
									     "port-value-type", G_TYPE_FLOAT,
									     "port-value-size", sizeof(gfloat),
									     "port-value-length", 1,
									     NULL);
  
  fx_raven_synth_audio->synth_0_sync_relative_attack_factor_2->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_sync_relative_attack_factor_2,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_relative_attack_factor_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_sync_relative_attack_factor_2);

  position++;

  /* synth-0 sync attack-2 */
  fx_raven_synth_audio->synth_0_sync_attack_2 = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							     "specifier", "./synth-0-sync-attack-2[0]",
							     "control-port", "34/121",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_raven_synth_audio->synth_0_sync_attack_2->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_sync_attack_2,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_attack_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_sync_attack_2);

  position++;

  /* synth-0 sync phase-2 */
  fx_raven_synth_audio->synth_0_sync_phase_2 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-sync-phase-2[0]",
							    "control-port", "35/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_sync_phase_2->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_sync_phase_2,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_sync_phase_2);

  position++;

  /* synth-0 sync relative attack factor 3 */
  fx_raven_synth_audio->synth_0_sync_relative_attack_factor_3 = g_object_new(AGS_TYPE_PORT,
									     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
									     "specifier", "./synth-0-sync-relative-factor-3[0]",
									     "control-port", "36/121",
									     "port-value-is-pointer", FALSE,
									     "port-value-type", G_TYPE_FLOAT,
									     "port-value-size", sizeof(gfloat),
									     "port-value-length", 1,
									     NULL);
  
  fx_raven_synth_audio->synth_0_sync_relative_attack_factor_3->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_0_sync_relative_attack_factor_3,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_relative_attack_factor_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_sync_relative_attack_factor_3);

  position++;

  /* synth-0 sync attack-3 */
  fx_raven_synth_audio->synth_0_sync_attack_3 = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							     "specifier", "./synth-0-sync-attack-3[0]",
							     "control-port", "37/121",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_raven_synth_audio->synth_0_sync_attack_3->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_sync_attack_3,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_attack_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_sync_attack_3);

  position++;

  /* synth-0 sync phase-3 */
  fx_raven_synth_audio->synth_0_sync_phase_3 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-0-sync-phase-3[0]",
							    "control-port", "38/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_0_sync_phase_3->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_0_sync_phase_3,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_sync_phase_3);

  position++;

  /* synth-0 LFO oscillator */
  fx_raven_synth_audio->synth_0_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							      "specifier", "./synth-0-lfo-oscillator[0]",
							      "control-port", "41/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_raven_synth_audio->synth_0_lfo_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_raven_synth_audio->synth_0_lfo_oscillator,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_lfo_oscillator);

  position++;

  /* synth-0 LFO frequency */
  fx_raven_synth_audio->synth_0_lfo_frequency = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							     "specifier", "./synth-0-lfo-frequency[0]",
							     "control-port", "42/121",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_raven_synth_audio->synth_0_lfo_frequency->port_value.ags_port_float = (gfloat) AGS_RAVEN_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY;

  g_object_set(fx_raven_synth_audio->synth_0_lfo_frequency,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_lfo_frequency);

  position++;

  /* synth-0 LFO depth */
  fx_raven_synth_audio->synth_0_lfo_depth = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							 "specifier", "./synth-0-lfo-depth[0]",
							 "control-port", "43/121",
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_raven_synth_audio->synth_0_lfo_depth->port_value.ags_port_float = (gfloat) AGS_RAVEN_SYNTH_UTIL_DEFAULT_LFO_DEPTH;

  g_object_set(fx_raven_synth_audio->synth_0_lfo_depth,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_lfo_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_lfo_depth);

  position++;

  /* synth-0 LFO tuning */
  fx_raven_synth_audio->synth_0_lfo_tuning = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							  "specifier", "./synth-0-lfo-tuning[0]",
							  "control-port", "44/121",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_raven_synth_audio->synth_0_lfo_tuning->port_value.ags_port_float = (gfloat) AGS_RAVEN_SYNTH_UTIL_DEFAULT_TUNING;

  g_object_set(fx_raven_synth_audio->synth_0_lfo_tuning,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_lfo_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_0_lfo_tuning);

  position++;

  /* synth-1 oscillator */
  fx_raven_synth_audio->synth_1_oscillator = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							  "specifier", "./synth-1-oscillator[0]",
							  "control-port", "45/121",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_raven_synth_audio->synth_1_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_raven_synth_audio->synth_1_oscillator,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_oscillator);

  position++;
  
  /* synth-1 octave */
  fx_raven_synth_audio->synth_1_octave = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						      "specifier", "./synth-1-octave[0]",
						      "control-port", "46/121",
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_raven_synth_audio->synth_1_octave->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_octave,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_octave);

  position++;

  /* synth-1 key */
  fx_raven_synth_audio->synth_1_key = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						   "specifier", "./synth-1-key[0]",
						   "control-port", "47/121",
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_raven_synth_audio->synth_1_key->port_value.ags_port_float = (gfloat) 2.0;

  g_object_set(fx_raven_synth_audio->synth_1_key,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_key);

  position++;

  /* synth-1 phase */
  fx_raven_synth_audio->synth_1_phase = g_object_new(AGS_TYPE_PORT,
						     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						     "specifier", "./synth-1-phase[0]",
						     "control-port", "48/121",
						     "port-value-is-pointer", FALSE,
						     "port-value-type", G_TYPE_FLOAT,
						     "port-value-size", sizeof(gfloat),
						     "port-value-length", 1,
						     NULL);
  
  fx_raven_synth_audio->synth_1_phase->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_phase,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_phase);

  position++;

  /* synth-1 volume */
  fx_raven_synth_audio->synth_1_volume = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						      "specifier", "./synth-1-volume[0]",
						      "control-port", "49/121",
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_raven_synth_audio->synth_1_volume->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_raven_synth_audio->synth_1_volume,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_volume);

  position++;

  /* synth-1 seq tuning 0 */
  fx_raven_synth_audio->synth_1_seq_tuning_0 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-0[0]",
							    "control-port", "50/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_0->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_0,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_0);

  position++;

  /* synth-1 seq tuning 1 */
  fx_raven_synth_audio->synth_1_seq_tuning_1 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-1[0]",
							    "control-port", "51/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_1->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_1,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_1);

  position++;

  /* synth-1 seq tuning 2 */
  fx_raven_synth_audio->synth_1_seq_tuning_2 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-2[0]",
							    "control-port", "52/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_2->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_2,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_2);

  position++;

  /* synth-1 seq tuning 3 */
  fx_raven_synth_audio->synth_1_seq_tuning_3 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-3[0]",
							    "control-port", "53/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_3->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_3,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_3);

  position++;

  /* synth-1 seq tuning 4 */
  fx_raven_synth_audio->synth_1_seq_tuning_4 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-4[0]",
							    "control-port", "54/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_4->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_4,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_4);

  position++;

  /* synth-1 seq tuning 5 */
  fx_raven_synth_audio->synth_1_seq_tuning_5 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-5[0]",
							    "control-port", "55/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_5->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_5,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_5);

  position++;

  /* synth-1 seq tuning 6 */
  fx_raven_synth_audio->synth_1_seq_tuning_6 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-6[0]",
							    "control-port", "56/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_6->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_6,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_6);

  position++;

  /* synth-1 seq tuning 7 */
  fx_raven_synth_audio->synth_1_seq_tuning_7 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-7[0]",
							    "control-port", "57/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_7->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_7,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_7);

  position++;

  /* synth-1 seq tuning 8 */
  fx_raven_synth_audio->synth_1_seq_tuning_8 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-8[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_8->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_8,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_8);

  position++;

  /* synth-1 seq tuning 9 */
  fx_raven_synth_audio->synth_1_seq_tuning_9 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-9[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_9->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_9,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_9);

  position++;

  /* synth-1 seq tuning 10 */
  fx_raven_synth_audio->synth_1_seq_tuning_10 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-10[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_10->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_10,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_10);

  position++;

  /* synth-1 seq tuning 11 */
  fx_raven_synth_audio->synth_1_seq_tuning_11 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-11[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_11->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_11,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_11);

  position++;

  /* synth-1 seq tuning 12 */
  fx_raven_synth_audio->synth_1_seq_tuning_12 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-12[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_12->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_12,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_12);

  position++;

  /* synth-1 seq tuning 13 */
  fx_raven_synth_audio->synth_1_seq_tuning_13 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-13[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_13->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_13,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_13);

  position++;

  /* synth-1 seq tuning 14 */
  fx_raven_synth_audio->synth_1_seq_tuning_14 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-14[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_14->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_14,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_14);

  position++;

  /* synth-1 seq tuning 15 */
  fx_raven_synth_audio->synth_1_seq_tuning_15 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-tuning-15[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_15->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_15,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_15);

  position++;

  /* synth-1 seq tuning pingpong */
  fx_raven_synth_audio->synth_1_seq_tuning_pingpong = g_object_new(AGS_TYPE_PORT,
								   "plugin-name", ags_fx_raven_synth_audio_plugin_name,
								   "specifier", "./synth-1-seq-tuning-pingpong[0]",
								   "control-port", "58/121",
								   "port-value-is-pointer", FALSE,
								   "port-value-type", G_TYPE_FLOAT,
								   "port-value-size", sizeof(gfloat),
								   "port-value-length", 1,
								   NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_pingpong->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_pingpong,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_pingpong_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_pingpong);

  position++;

  /* synth-1 seq tuning LFO frequency */
  fx_raven_synth_audio->synth_1_seq_tuning_lfo_frequency = g_object_new(AGS_TYPE_PORT,
									"plugin-name", ags_fx_raven_synth_audio_plugin_name,
									"specifier", "./synth-1-seq-tuning-lfo-frequency[0]",
									"control-port", "59/121",
									"port-value-is-pointer", FALSE,
									"port-value-type", G_TYPE_FLOAT,
									"port-value-size", sizeof(gfloat),
									"port-value-length", 1,
									NULL);
  
  fx_raven_synth_audio->synth_1_seq_tuning_lfo_frequency->port_value.ags_port_float = (gfloat) 8.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_tuning_lfo_frequency,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_tuning_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_tuning_lfo_frequency);

  position++;

  /* synth-1 seq volume 0 */
  fx_raven_synth_audio->synth_1_seq_volume_0 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-0[0]",
							    "control-port", "60/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_0->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_0,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_0);

  position++;

  /* synth-1 seq volume 1 */
  fx_raven_synth_audio->synth_1_seq_volume_1 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-1[0]",
							    "control-port", "61/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_1->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_1,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_1);

  position++;

  /* synth-1 seq volume 2 */
  fx_raven_synth_audio->synth_1_seq_volume_2 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-2[0]",
							    "control-port", "62/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_2->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_2,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_2);

  position++;

  /* synth-1 seq volume 3 */
  fx_raven_synth_audio->synth_1_seq_volume_3 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-3[0]",
							    "control-port", "63/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_3->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_3,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_3);

  position++;

  /* synth-1 seq volume 4 */
  fx_raven_synth_audio->synth_1_seq_volume_4 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-4[0]",
							    "control-port", "64/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_4->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_4,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_4);

  position++;

  /* synth-1 seq volume 5 */
  fx_raven_synth_audio->synth_1_seq_volume_5 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-5[0]",
							    "control-port", "65/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_5->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_5,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_5);

  position++;

  /* synth-1 seq volume 6 */
  fx_raven_synth_audio->synth_1_seq_volume_6 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-6[0]",
							    "control-port", "66/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_6->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_6,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_6);

  position++;

  /* synth-1 seq volume 7 */
  fx_raven_synth_audio->synth_1_seq_volume_7 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-7[0]",
							    "control-port", "67/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_7->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_7,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_7);

  position++;

  /* synth-1 seq volume 8 */
  fx_raven_synth_audio->synth_1_seq_volume_8 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-8[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_8->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_8,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_8);

  position++;

  /* synth-1 seq volume 9 */
  fx_raven_synth_audio->synth_1_seq_volume_9 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-9[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_9->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_9,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_9);

  position++;

  /* synth-1 seq volume 10 */
  fx_raven_synth_audio->synth_1_seq_volume_10 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-10[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_10->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_10,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_10);

  position++;

  /* synth-1 seq volume 11 */
  fx_raven_synth_audio->synth_1_seq_volume_11 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-11[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_11->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_11,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_11);

  position++;

  /* synth-1 seq volume 12 */
  fx_raven_synth_audio->synth_1_seq_volume_12 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-12[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_12->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_12,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_12);

  position++;

  /* synth-1 seq volume 13 */
  fx_raven_synth_audio->synth_1_seq_volume_13 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-13[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_13->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_13,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_13);

  position++;

  /* synth-1 seq volume 14 */
  fx_raven_synth_audio->synth_1_seq_volume_14 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-14[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_14->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_14,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_14);

  position++;

  /* synth-1 seq volume 15 */
  fx_raven_synth_audio->synth_1_seq_volume_15 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-seq-volume-15[0]",
							    "control-port", "16/121", 
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_15->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_15,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_15);

  position++;

  /* synth-1 seq volume pingpong */
  fx_raven_synth_audio->synth_1_seq_volume_pingpong = g_object_new(AGS_TYPE_PORT,
								   "plugin-name", ags_fx_raven_synth_audio_plugin_name,
								   "specifier", "./synth-1-seq-volume-pingpong[0]",
								   "control-port", "68/121",
								   "port-value-is-pointer", FALSE,
								   "port-value-type", G_TYPE_FLOAT,
								   "port-value-size", sizeof(gfloat),
								   "port-value-length", 1,
								   NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_pingpong->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_pingpong,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_pingpong_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_pingpong);

  position++;

  /* synth-1 seq volume LFO frequency */
  fx_raven_synth_audio->synth_1_seq_volume_lfo_frequency = g_object_new(AGS_TYPE_PORT,
									"plugin-name", ags_fx_raven_synth_audio_plugin_name,
									"specifier", "./synth-1-seq-volume-lfo-frequency[0]",
									"control-port", "69/121",
									"port-value-is-pointer", FALSE,
									"port-value-type", G_TYPE_FLOAT,
									"port-value-size", sizeof(gfloat),
									"port-value-length", 1,
									NULL);
  
  fx_raven_synth_audio->synth_1_seq_volume_lfo_frequency->port_value.ags_port_float = (gfloat) 8.0;

  g_object_set(fx_raven_synth_audio->synth_1_seq_volume_lfo_frequency,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_seq_volume_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_seq_volume_lfo_frequency);

  position++;

  /* synth-1 sync enabled */
  fx_raven_synth_audio->synth_1_sync_enabled = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-sync-enabled[0]",
							    "control-port", "70/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_sync_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_raven_synth_audio->synth_1_sync_enabled,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_sync_enabled);

  position++;

  /* synth-1 sync relative attack factor 0 */
  fx_raven_synth_audio->synth_1_sync_relative_attack_factor_0 = g_object_new(AGS_TYPE_PORT,
									     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
									     "specifier", "./synth-1-sync-relative-factor-0[0]",
									     "control-port", "71/121",
									     "port-value-is-pointer", FALSE,
									     "port-value-type", G_TYPE_FLOAT,
									     "port-value-size", sizeof(gfloat),
									     "port-value-length", 1,
									     NULL);
  
  fx_raven_synth_audio->synth_1_sync_relative_attack_factor_0->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_sync_relative_attack_factor_0,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_relative_attack_factor_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_sync_relative_attack_factor_0);

  position++;

  /* synth-1 sync attack-0 */
  fx_raven_synth_audio->synth_1_sync_attack_0 = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							     "specifier", "./synth-1-sync-attack-0[0]",
							     "control-port", "72/121",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_raven_synth_audio->synth_1_sync_attack_0->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_sync_attack_0,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_attack_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_sync_attack_0);

  position++;

  /* synth-1 sync phase-0 */
  fx_raven_synth_audio->synth_1_sync_phase_0 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-sync-phase-0[0]",
							    "control-port", "73/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_sync_phase_0->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_sync_phase_0,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_sync_phase_0);

  position++;

  /* synth-1 sync relative attack factor 1 */
  fx_raven_synth_audio->synth_1_sync_relative_attack_factor_1 = g_object_new(AGS_TYPE_PORT,
									     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
									     "specifier", "./synth-1-sync-relative-factor-1[0]",
									     "control-port", "74/121",
									     "port-value-is-pointer", FALSE,
									     "port-value-type", G_TYPE_FLOAT,
									     "port-value-size", sizeof(gfloat),
									     "port-value-length", 1,
									     NULL);
  
  fx_raven_synth_audio->synth_1_sync_relative_attack_factor_1->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_sync_relative_attack_factor_1,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_relative_attack_factor_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_sync_relative_attack_factor_1);

  position++;

  /* synth-1 sync attack-1 */
  fx_raven_synth_audio->synth_1_sync_attack_1 = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							     "specifier", "./synth-1-sync-attack-1[0]",
							     "control-port", "75/121",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_raven_synth_audio->synth_1_sync_attack_1->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_sync_attack_1,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_attack_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_sync_attack_1);

  position++;

  /* synth-1 sync phase-1 */
  fx_raven_synth_audio->synth_1_sync_phase_1 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-sync-phase-1[0]",
							    "control-port", "76/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_sync_phase_1->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_sync_phase_1,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_sync_phase_1);

  position++;

  /* synth-1 sync relative attack factor 2 */
  fx_raven_synth_audio->synth_1_sync_relative_attack_factor_2 = g_object_new(AGS_TYPE_PORT,
									     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
									     "specifier", "./synth-1-sync-relative-factor-2[0]",
									     "control-port", "77/121",
									     "port-value-is-pointer", FALSE,
									     "port-value-type", G_TYPE_FLOAT,
									     "port-value-size", sizeof(gfloat),
									     "port-value-length", 1,
									     NULL);
  
  fx_raven_synth_audio->synth_1_sync_relative_attack_factor_2->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_sync_relative_attack_factor_2,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_relative_attack_factor_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_sync_relative_attack_factor_2);

  position++;

  /* synth-1 sync attack-2 */
  fx_raven_synth_audio->synth_1_sync_attack_2 = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							     "specifier", "./synth-1-sync-attack-2[0]",
							     "control-port", "78/121",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_raven_synth_audio->synth_1_sync_attack_2->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_sync_attack_2,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_attack_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_sync_attack_2);

  position++;

  /* synth-1 sync phase-2 */
  fx_raven_synth_audio->synth_1_sync_phase_2 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-sync-phase-2[0]",
							    "control-port", "79/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_sync_phase_2->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_sync_phase_2,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_sync_phase_2);

  position++;

  /* synth-1 sync relative attack factor 3 */
  fx_raven_synth_audio->synth_1_sync_relative_attack_factor_3 = g_object_new(AGS_TYPE_PORT,
									     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
									     "specifier", "./synth-1-sync-relative-factor-3[0]",
									     "control-port", "80/121",
									     "port-value-is-pointer", FALSE,
									     "port-value-type", G_TYPE_FLOAT,
									     "port-value-size", sizeof(gfloat),
									     "port-value-length", 1,
									     NULL);
  
  fx_raven_synth_audio->synth_1_sync_relative_attack_factor_3->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->synth_1_sync_relative_attack_factor_3,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_relative_attack_factor_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_sync_relative_attack_factor_3);

  position++;

  /* synth-1 sync attack-3 */
  fx_raven_synth_audio->synth_1_sync_attack_3 = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							     "specifier", "./synth-1-sync-attack-3[0]",
							     "control-port", "81/121",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_raven_synth_audio->synth_1_sync_attack_3->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_sync_attack_3,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_attack_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_sync_attack_3);

  position++;

  /* synth-1 sync phase-3 */
  fx_raven_synth_audio->synth_1_sync_phase_3 = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./synth-1-sync-phase-3[0]",
							    "control-port", "82/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->synth_1_sync_phase_3->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->synth_1_sync_phase_3,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_sync_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_sync_phase_3);

  position++;

  /* synth-1 LFO oscillator */
  fx_raven_synth_audio->synth_1_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							      "specifier", "./synth-1-lfo-oscillator[0]",
							      "control-port", "85/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_raven_synth_audio->synth_1_lfo_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_raven_synth_audio->synth_1_lfo_oscillator,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_lfo_oscillator);

  position++;

  /* synth-1 LFO frequency */
  fx_raven_synth_audio->synth_1_lfo_frequency = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							     "specifier", "./synth-1-lfo-frequency[0]",
							     "control-port", "86/121",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_raven_synth_audio->synth_1_lfo_frequency->port_value.ags_port_float = (gfloat) AGS_RAVEN_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY;

  g_object_set(fx_raven_synth_audio->synth_1_lfo_frequency,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_lfo_frequency);

  position++;

  /* synth-1 LFO depth */
  fx_raven_synth_audio->synth_1_lfo_depth = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							 "specifier", "./synth-1-lfo-depth[0]",
							 "control-port", "87/121",
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_raven_synth_audio->synth_1_lfo_depth->port_value.ags_port_float = (gfloat) AGS_RAVEN_SYNTH_UTIL_DEFAULT_LFO_DEPTH;

  g_object_set(fx_raven_synth_audio->synth_1_lfo_depth,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_lfo_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_lfo_depth);

  position++;

  /* synth-1 LFO tuning */
  fx_raven_synth_audio->synth_1_lfo_tuning = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							  "specifier", "./synth-1-lfo-tuning[0]",
							  "control-port", "88/121",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_raven_synth_audio->synth_1_lfo_tuning->port_value.ags_port_float = (gfloat) AGS_RAVEN_SYNTH_UTIL_DEFAULT_TUNING;

  g_object_set(fx_raven_synth_audio->synth_1_lfo_tuning,
	       "plugin-port", ags_fx_raven_synth_audio_get_synth_lfo_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->synth_1_lfo_tuning);

  position++;
  
  /* low-pass 0 cut off frequency */
  fx_raven_synth_audio->low_pass_0_cut_off_frequency = g_object_new(AGS_TYPE_PORT,
								    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
								    "specifier", "./low-pass-0-cut-off-frequency[0]",
								    "control-port", "89/121",
								    "port-value-is-pointer", FALSE,
								    "port-value-type", G_TYPE_FLOAT,
								    "port-value-size", sizeof(gfloat),
								    "port-value-length", 1,
								    NULL);
  
  fx_raven_synth_audio->low_pass_0_cut_off_frequency->port_value.ags_port_float = (gfloat) 2000.0;

  g_object_set(fx_raven_synth_audio->low_pass_0_cut_off_frequency,
	       "plugin-port", ags_fx_raven_synth_audio_get_low_pass_cut_off_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->low_pass_0_cut_off_frequency);

  position++;
  
  /* low-pass 0 filter gain */
  fx_raven_synth_audio->low_pass_0_filter_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							      "specifier", "./low-pass-0-filter-gain[0]",
							      "control-port", "90/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_raven_synth_audio->low_pass_0_filter_gain->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->low_pass_0_filter_gain,
	       "plugin-port", ags_fx_raven_synth_audio_get_low_pass_filter_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->low_pass_0_filter_gain);

  position++;
  
  /* low-pass 0 no-clip */
  fx_raven_synth_audio->low_pass_0_no_clip = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							  "specifier", "./low-pass-0-no-clip[0]",
							  "control-port", "91/121",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_raven_synth_audio->low_pass_0_no_clip->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->low_pass_0_no_clip,
	       "plugin-port", ags_fx_raven_synth_audio_get_low_pass_no_clip_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->low_pass_0_no_clip);

  position++;
  
  /* low-pass 1 cut off frequency */
  fx_raven_synth_audio->low_pass_1_cut_off_frequency = g_object_new(AGS_TYPE_PORT,
								    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
								    "specifier", "./low-pass-1-cut-off-frequency[0]",
								    "control-port", "92/121",
								    "port-value-is-pointer", FALSE,
								    "port-value-type", G_TYPE_FLOAT,
								    "port-value-size", sizeof(gfloat),
								    "port-value-length", 1,
								    NULL);
  
  fx_raven_synth_audio->low_pass_1_cut_off_frequency->port_value.ags_port_float = (gfloat) 2000.0;

  g_object_set(fx_raven_synth_audio->low_pass_1_cut_off_frequency,
	       "plugin-port", ags_fx_raven_synth_audio_get_low_pass_cut_off_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->low_pass_1_cut_off_frequency);

  position++;
  
  /* low-pass 1 filter gain */
  fx_raven_synth_audio->low_pass_1_filter_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							      "specifier", "./low-pass-1-filter-gain[0]",
							      "control-port", "93/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_raven_synth_audio->low_pass_1_filter_gain->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->low_pass_1_filter_gain,
	       "plugin-port", ags_fx_raven_synth_audio_get_low_pass_filter_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->low_pass_1_filter_gain);

  position++;
  
  /* low-pass 1 no-clip */
  fx_raven_synth_audio->low_pass_1_no_clip = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							  "specifier", "./low-pass-1-no-clip[0]",
							  "control-port", "94/121",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_raven_synth_audio->low_pass_1_no_clip->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->low_pass_1_no_clip,
	       "plugin-port", ags_fx_raven_synth_audio_get_low_pass_no_clip_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->low_pass_1_no_clip);

  position++;

  /* amplifier 0 amp-0 gain */
  fx_raven_synth_audio->amplifier_0_amp_0_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							      "specifier", "./amplifier-0-amp-0-gain[0]",
							      "control-port", "95/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_raven_synth_audio->amplifier_0_amp_0_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->amplifier_0_amp_0_gain,
	       "plugin-port", ags_fx_raven_synth_audio_get_amplifier_amp_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->amplifier_0_amp_0_gain);

  position++;

  /* amplifier 0 amp-1 gain */
  fx_raven_synth_audio->amplifier_0_amp_1_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							      "specifier", "./amplifier-0-amp-1-gain[0]",
							      "control-port", "96/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_raven_synth_audio->amplifier_0_amp_1_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->amplifier_0_amp_1_gain,
	       "plugin-port", ags_fx_raven_synth_audio_get_amplifier_amp_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->amplifier_0_amp_1_gain);

  position++;

  /* amplifier 0 amp-2 gain */
  fx_raven_synth_audio->amplifier_0_amp_2_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							      "specifier", "./amplifier-0-amp-2-gain[0]",
							      "control-port", "97/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_raven_synth_audio->amplifier_0_amp_2_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->amplifier_0_amp_2_gain,
	       "plugin-port", ags_fx_raven_synth_audio_get_amplifier_amp_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->amplifier_0_amp_2_gain);

  position++;

  /* amplifier 0 amp-3 gain */
  fx_raven_synth_audio->amplifier_0_amp_3_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							      "specifier", "./amplifier-0-amp-3-gain[0]",
							      "control-port", "98/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_raven_synth_audio->amplifier_0_amp_3_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->amplifier_0_amp_3_gain,
	       "plugin-port", ags_fx_raven_synth_audio_get_amplifier_amp_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->amplifier_0_amp_3_gain);

  position++;

  /* amplifier 0 filter gain */
  fx_raven_synth_audio->amplifier_0_filter_gain = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							       "specifier", "./amplifier-0-filter-gain[0]",
							       "control-port", "99/121",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_raven_synth_audio->amplifier_0_filter_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->amplifier_0_filter_gain,
	       "plugin-port", ags_fx_raven_synth_audio_get_amplifier_filter_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->amplifier_0_filter_gain);

  position++;

  /* amplifier 1 amp-0 gain */
  fx_raven_synth_audio->amplifier_1_amp_0_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							      "specifier", "./amplifier-1-amp-0-gain[0]",
							      "control-port", "100/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_raven_synth_audio->amplifier_1_amp_0_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->amplifier_1_amp_0_gain,
	       "plugin-port", ags_fx_raven_synth_audio_get_amplifier_amp_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->amplifier_1_amp_0_gain);

  position++;

  /* amplifier 1 amp-1 gain */
  fx_raven_synth_audio->amplifier_1_amp_1_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							      "specifier", "./amplifier-1-amp-1-gain[0]",
							      "control-port", "101/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_raven_synth_audio->amplifier_1_amp_1_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->amplifier_1_amp_1_gain,
	       "plugin-port", ags_fx_raven_synth_audio_get_amplifier_amp_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->amplifier_1_amp_1_gain);

  position++;

  /* amplifier 1 amp-2 gain */
  fx_raven_synth_audio->amplifier_1_amp_2_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							      "specifier", "./amplifier-1-amp-2-gain[0]",
							      "control-port", "102/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_raven_synth_audio->amplifier_1_amp_2_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->amplifier_1_amp_2_gain,
	       "plugin-port", ags_fx_raven_synth_audio_get_amplifier_amp_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->amplifier_1_amp_2_gain);

  position++;

  /* amplifier 1 amp-3 gain */
  fx_raven_synth_audio->amplifier_1_amp_3_gain = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							      "specifier", "./amplifier-1-amp-3-gain[0]",
							      "control-port", "103/121",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_raven_synth_audio->amplifier_1_amp_3_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->amplifier_1_amp_3_gain,
	       "plugin-port", ags_fx_raven_synth_audio_get_amplifier_amp_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->amplifier_1_amp_3_gain);

  position++;

  /* amplifier 1 filter gain */
  fx_raven_synth_audio->amplifier_1_filter_gain = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							       "specifier", "./amplifier-1-filter-gain[0]",
							       "control-port", "104/121",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_raven_synth_audio->amplifier_1_filter_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->amplifier_1_filter_gain,
	       "plugin-port", ags_fx_raven_synth_audio_get_amplifier_filter_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->amplifier_1_filter_gain);

  position++;

  /* noise gain */
  fx_raven_synth_audio->noise_gain = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						  "specifier", "./noise-gain[0]",
						  "control-port", "105/121",
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 1,
						  NULL);
  
  fx_raven_synth_audio->noise_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->noise_gain,
	       "plugin-port", ags_fx_raven_synth_audio_get_noise_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->noise_gain);

  position++;

  /* pitch type */
  fx_raven_synth_audio->pitch_type = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						  "specifier", "./pitch-type[0]",
						  "control-port", "106/121",
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 1,
						  NULL);
  
  fx_raven_synth_audio->pitch_type->port_value.ags_port_float = (gfloat) AGS_PITCH_TYPE_FLUID_INTERPOLATE_4TH_ORDER;

  g_object_set(fx_raven_synth_audio->pitch_type,
	       "plugin-port", ags_fx_raven_synth_audio_get_pitch_type_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->pitch_type);

  position++;

  g_signal_connect_after(fx_raven_synth_audio->pitch_type, "safe-write",
			 G_CALLBACK(ags_fx_raven_synth_audio_pitch_type_callback), fx_raven_synth_audio);

  /* pitch tuning */
  fx_raven_synth_audio->pitch_tuning = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						    "specifier", "./pitch-tuning[0]",
						    "control-port", "107/121",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_raven_synth_audio->pitch_tuning->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->pitch_tuning,
	       "plugin-port", ags_fx_raven_synth_audio_get_pitch_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->pitch_tuning);

  position++;

  /* chorus enabled */
  fx_raven_synth_audio->chorus_enabled = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						      "specifier", "./chorus-enabled[0]",
						      "control-port", "108/121",
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_raven_synth_audio->chorus_enabled->port_value.ags_port_float = (gfloat) TRUE;

  g_object_set(fx_raven_synth_audio->chorus_enabled,
	       "plugin-port", ags_fx_raven_synth_audio_get_chorus_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->chorus_enabled);

  position++;

  /* chorus pitch type */
  fx_raven_synth_audio->chorus_pitch_type = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							 "specifier", "./chorus-pitch-type[0]",
							 "control-port", "109/121",
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_raven_synth_audio->chorus_pitch_type->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->chorus_pitch_type,
	       "plugin-port", ags_fx_raven_synth_audio_get_chorus_pitch_type_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->chorus_pitch_type);

  position++;

  /* chorus input volume */
  fx_raven_synth_audio->chorus_input_volume = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							   "specifier", "./chorus-input-volume[0]",
							   "control-port", "110/121",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_raven_synth_audio->chorus_input_volume->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->chorus_input_volume,
	       "plugin-port", ags_fx_raven_synth_audio_get_chorus_input_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->chorus_input_volume);

  position++;

  /* chorus output volume */
  fx_raven_synth_audio->chorus_output_volume = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./chorus-output-volume[0]",
							    "control-port", "111/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->chorus_output_volume->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->chorus_output_volume,
	       "plugin-port", ags_fx_raven_synth_audio_get_chorus_output_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->chorus_output_volume);

  position++;

  /* chorus LFO oscillator */
  fx_raven_synth_audio->chorus_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							     "specifier", "./chorus-lfo-oscillator[0]",
							     "control-port", "112/121",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_raven_synth_audio->chorus_lfo_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_raven_synth_audio->chorus_lfo_oscillator,
	       "plugin-port", ags_fx_raven_synth_audio_get_chorus_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->chorus_lfo_oscillator);

  position++;

  /* chorus LFO frequency */
  fx_raven_synth_audio->chorus_lfo_frequency = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							    "specifier", "./chorus-lfo-frequency[0]",
							    "control-port", "113/121",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_raven_synth_audio->chorus_lfo_frequency->port_value.ags_port_float = (gfloat) 0.01;

  g_object_set(fx_raven_synth_audio->chorus_lfo_frequency,
	       "plugin-port", ags_fx_raven_synth_audio_get_chorus_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->chorus_lfo_frequency);

  position++;

  /* chorus depth */
  fx_raven_synth_audio->chorus_depth = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						    "specifier", "./chorus-depth[0]",
						    "control-port", "114/121",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_raven_synth_audio->chorus_depth->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->chorus_depth,
	       "plugin-port", ags_fx_raven_synth_audio_get_chorus_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->chorus_depth);

  position++;

  /* chorus mix */
  fx_raven_synth_audio->chorus_mix = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						  "specifier", "./chorus-mix[0]",
						  "control-port", "115/121",
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 1,
						  NULL);
  
  fx_raven_synth_audio->chorus_mix->port_value.ags_port_float = (gfloat) 0.5;

  g_object_set(fx_raven_synth_audio->chorus_mix,
	       "plugin-port", ags_fx_raven_synth_audio_get_chorus_mix_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->chorus_mix);

  position++;

  /* chorus delay */
  fx_raven_synth_audio->chorus_delay = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						    "specifier", "./chorus-delay[0]",
						    "control-port", "116/121",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_raven_synth_audio->chorus_delay->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->chorus_delay,
	       "plugin-port", ags_fx_raven_synth_audio_get_chorus_delay_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->chorus_delay);

  position++;

  /* vibrato enabled */
  fx_raven_synth_audio->vibrato_enabled = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						       "specifier", "./vibrato-enabled[0]",
						       "control-port", "117/121",
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_raven_synth_audio->vibrato_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_raven_synth_audio->vibrato_enabled,
	       "plugin-port", ags_fx_raven_synth_audio_get_vibrato_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->vibrato_enabled);

  position++;

  /* vibrato gain */
  fx_raven_synth_audio->vibrato_gain = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						    "specifier", "./vibrato-gain[0]",
						    "control-port", "118/121",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_raven_synth_audio->vibrato_gain->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->vibrato_gain,
	       "plugin-port", ags_fx_raven_synth_audio_get_vibrato_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->vibrato_gain);

  position++;
  
  /* vibrato LFO depth */
  fx_raven_synth_audio->vibrato_lfo_depth = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_raven_synth_audio_plugin_name,
							 "specifier", "./vibrato-lfo-depth[0]",
							 "control-port", "119/121",
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_raven_synth_audio->vibrato_lfo_depth->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_raven_synth_audio->vibrato_lfo_depth,
	       "plugin-port", ags_fx_raven_synth_audio_get_vibrato_lfo_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->vibrato_lfo_depth);

  position++;
  
  /* vibrato LFO freq */
  fx_raven_synth_audio->vibrato_lfo_freq = g_object_new(AGS_TYPE_PORT,
							"plugin-name", ags_fx_raven_synth_audio_plugin_name,
							"specifier", "./vibrato-lfo-freq[0]",
							"control-port", "120/121",
							"port-value-is-pointer", FALSE,
							"port-value-type", G_TYPE_FLOAT,
							"port-value-size", sizeof(gfloat),
							"port-value-length", 1,
							NULL);
  
  fx_raven_synth_audio->vibrato_lfo_freq->port_value.ags_port_float = (gfloat) 8.172;

  g_object_set(fx_raven_synth_audio->vibrato_lfo_freq,
	       "plugin-port", ags_fx_raven_synth_audio_get_vibrato_lfo_freq_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->vibrato_lfo_freq);

  position++;

  /* vibrato tuning */
  fx_raven_synth_audio->vibrato_tuning = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_raven_synth_audio_plugin_name,
						      "specifier", "./vibrato-tuning[0]",
						      "control-port", "121/121",
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_raven_synth_audio->vibrato_tuning->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_raven_synth_audio->vibrato_tuning,
	       "plugin-port", ags_fx_raven_synth_audio_get_vibrato_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_raven_synth_audio,
		      fx_raven_synth_audio->vibrato_tuning);

  position++;
  
  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      fx_raven_synth_audio->scope_data[i] = ags_fx_raven_synth_audio_scope_data_alloc();
      
      fx_raven_synth_audio->scope_data[i]->parent = fx_raven_synth_audio;
    }else{
      fx_raven_synth_audio->scope_data[i] = NULL;
    }
  }
}

void
ags_fx_raven_synth_audio_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec)
{
  AgsFxRavenSynthAudio *fx_raven_synth_audio;

  GRecMutex *recall_mutex;

  fx_raven_synth_audio = AGS_FX_RAVEN_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_raven_synth_audio);

  switch(prop_id){
  case PROP_SYNTH_0_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OCTAVE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_octave){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_octave != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_octave));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_octave = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_KEY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_key){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_key != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_key));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_key = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PHASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_phase){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_phase != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_phase));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_phase = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_volume != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_0 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_1 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_2 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_3 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_4:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_4){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_4 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_4));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_4 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_5:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_5){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_5 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_5));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_5 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_6:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_6){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_6 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_6));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_6 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_7:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_7){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_7 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_7));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_7 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_8:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_8){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_8 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_8));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_8 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_9:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_9){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_9 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_9));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_9 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_10:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_10){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_10 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_10));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_10 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_11:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_11){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_11 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_11));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_11 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_12:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_12){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_12 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_12));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_12 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_13:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_13){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_13 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_13));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_13 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_14:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_14){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_14 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_14));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_14 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_15:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_15){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_15 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_15));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_15 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_PINGPONG:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_pingpong){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_pingpong != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_pingpong));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_pingpong = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_tuning_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_tuning_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_tuning_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_tuning_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_0 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_1 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_2 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_3 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_4:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_4){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_4 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_4));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_4 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_5:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_5){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_5 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_5));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_5 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_6:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_6){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_6 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_6));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_6 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_7:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_7){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_7 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_7));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_7 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_8:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_8){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_8 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_8));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_8 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_9:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_9){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_9 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_9));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_9 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_10:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_10){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_10 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_10));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_10 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_11:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_11){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_11 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_11));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_11 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_12:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_12){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_12 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_12));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_12 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_13:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_13){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_13 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_13));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_13 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_14:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_14){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_14 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_14));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_14 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_15:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_15){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_15 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_15));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_15 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_PINGPONG:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_pingpong){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_pingpong != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_pingpong));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_pingpong = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_seq_volume_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_seq_volume_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_seq_volume_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_seq_volume_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ENABLED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_sync_enabled){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_sync_enabled != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_sync_enabled));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_sync_enabled = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_sync_relative_attack_factor_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_sync_relative_attack_factor_0 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_sync_relative_attack_factor_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_sync_relative_attack_factor_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_sync_relative_attack_factor_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_sync_relative_attack_factor_1 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_sync_relative_attack_factor_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_sync_relative_attack_factor_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_sync_relative_attack_factor_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_sync_relative_attack_factor_2 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_sync_relative_attack_factor_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_sync_relative_attack_factor_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_sync_relative_attack_factor_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_sync_relative_attack_factor_3 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_sync_relative_attack_factor_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_sync_relative_attack_factor_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_sync_attack_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_sync_attack_0 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_sync_attack_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_sync_attack_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_sync_phase_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_sync_phase_0 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_sync_phase_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_sync_phase_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_sync_attack_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_sync_attack_1 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_sync_attack_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_sync_attack_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_sync_phase_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_sync_phase_1 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_sync_phase_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_sync_phase_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_sync_attack_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_sync_attack_2 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_sync_attack_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_sync_attack_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_sync_phase_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_sync_phase_2 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_sync_phase_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_sync_phase_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_sync_attack_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_sync_attack_3 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_sync_attack_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_sync_attack_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_sync_phase_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_sync_phase_3 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_sync_phase_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_sync_phase_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_lfo_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_lfo_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_lfo_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_lfo_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_lfo_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_lfo_depth != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_lfo_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_lfo_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_0_lfo_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_0_lfo_tuning != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_0_lfo_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_0_lfo_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_0_CUT_OFF_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->low_pass_0_cut_off_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->low_pass_0_cut_off_frequency != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->low_pass_0_cut_off_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->low_pass_0_cut_off_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_0_FILTER_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->low_pass_0_filter_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->low_pass_0_filter_gain != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->low_pass_0_filter_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->low_pass_0_filter_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_0_NO_CLIP:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->low_pass_0_no_clip){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->low_pass_0_no_clip != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->low_pass_0_no_clip));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->low_pass_0_no_clip = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_0_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->amplifier_0_amp_0_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->amplifier_0_amp_0_gain != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->amplifier_0_amp_0_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->amplifier_0_amp_0_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_1_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->amplifier_0_amp_1_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->amplifier_0_amp_1_gain != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->amplifier_0_amp_1_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->amplifier_0_amp_1_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_2_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->amplifier_0_amp_2_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->amplifier_0_amp_2_gain != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->amplifier_0_amp_2_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->amplifier_0_amp_2_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_3_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->amplifier_0_amp_3_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->amplifier_0_amp_3_gain != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->amplifier_0_amp_3_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->amplifier_0_amp_3_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_FILTER_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->amplifier_0_filter_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->amplifier_0_filter_gain != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->amplifier_0_filter_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->amplifier_0_filter_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_OCTAVE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_octave){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_octave != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_octave));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_octave = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_KEY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_key){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_key != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_key));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_key = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_PHASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_phase){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_phase != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_phase));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_phase = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_volume != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_0 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_1 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_2 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_3 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_4:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_4){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_4 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_4));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_4 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_5:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_5){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_5 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_5));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_5 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_6:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_6){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_6 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_6));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_6 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_7:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_7){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_7 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_7));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_7 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_8:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_8){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_8 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_8));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_8 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_9:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_9){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_9 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_9));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_9 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_10:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_10){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_10 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_10));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_10 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_11:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_11){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_11 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_11));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_11 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_12:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_12){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_12 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_12));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_12 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_13:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_13){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_13 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_13));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_13 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_14:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_14){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_14 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_14));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_14 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_15:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_15){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_15 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_15));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_15 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_PINGPONG:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_pingpong){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_pingpong != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_pingpong));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_pingpong = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_tuning_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_tuning_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_tuning_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_tuning_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_0 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_1 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_2 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_3 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_4:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_4){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_4 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_4));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_4 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_5:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_5){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_5 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_5));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_5 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_6:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_6){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_6 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_6));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_6 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_7:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_7){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_7 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_7));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_7 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_8:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_8){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_8 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_8));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_8 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_9:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_9){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_9 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_9));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_9 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_10:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_10){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_10 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_10));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_10 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_11:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_11){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_11 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_11));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_11 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_12:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_12){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_12 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_12));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_12 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_13:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_13){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_13 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_13));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_13 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_14:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_14){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_14 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_14));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_14 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_15:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_15){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_15 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_15));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_15 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_PINGPONG:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_pingpong){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_pingpong != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_pingpong));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_pingpong = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_seq_volume_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_seq_volume_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_seq_volume_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_seq_volume_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ENABLED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_sync_enabled){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_sync_enabled != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_sync_enabled));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_sync_enabled = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_sync_relative_attack_factor_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_sync_relative_attack_factor_0 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_sync_relative_attack_factor_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_sync_relative_attack_factor_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_sync_relative_attack_factor_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_sync_relative_attack_factor_1 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_sync_relative_attack_factor_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_sync_relative_attack_factor_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_sync_relative_attack_factor_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_sync_relative_attack_factor_2 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_sync_relative_attack_factor_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_sync_relative_attack_factor_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_sync_relative_attack_factor_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_sync_relative_attack_factor_3 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_sync_relative_attack_factor_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_sync_relative_attack_factor_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_sync_attack_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_sync_attack_0 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_sync_attack_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_sync_attack_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_sync_phase_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_sync_phase_0 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_sync_phase_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_sync_phase_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_sync_attack_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_sync_attack_1 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_sync_attack_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_sync_attack_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_sync_phase_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_sync_phase_1 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_sync_phase_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_sync_phase_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_sync_attack_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_sync_attack_2 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_sync_attack_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_sync_attack_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_sync_phase_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_sync_phase_2 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_sync_phase_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_sync_phase_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_sync_attack_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_sync_attack_3 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_sync_attack_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_sync_attack_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_sync_phase_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_sync_phase_3 != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_sync_phase_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_sync_phase_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_lfo_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_lfo_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_lfo_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_lfo_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_lfo_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_lfo_depth != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_lfo_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_lfo_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->synth_1_lfo_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->synth_1_lfo_tuning != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->synth_1_lfo_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->synth_1_lfo_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_1_CUT_OFF_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->low_pass_1_cut_off_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->low_pass_1_cut_off_frequency != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->low_pass_1_cut_off_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->low_pass_1_cut_off_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_1_FILTER_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->low_pass_1_filter_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->low_pass_1_filter_gain != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->low_pass_1_filter_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->low_pass_1_filter_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_1_NO_CLIP:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->low_pass_1_no_clip){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->low_pass_1_no_clip != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->low_pass_1_no_clip));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->low_pass_1_no_clip = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_1_AMP_0_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->amplifier_1_amp_0_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->amplifier_1_amp_0_gain != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->amplifier_1_amp_0_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->amplifier_1_amp_0_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_1_AMP_1_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->amplifier_1_amp_1_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->amplifier_1_amp_1_gain != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->amplifier_1_amp_1_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->amplifier_1_amp_1_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_1_AMP_2_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->amplifier_1_amp_2_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->amplifier_1_amp_2_gain != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->amplifier_1_amp_2_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->amplifier_1_amp_2_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_1_AMP_3_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->amplifier_1_amp_3_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->amplifier_1_amp_3_gain != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->amplifier_1_amp_3_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->amplifier_1_amp_3_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_1_FILTER_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->amplifier_1_filter_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->amplifier_1_filter_gain != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->amplifier_1_filter_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->amplifier_1_filter_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_NOISE_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->noise_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->noise_gain != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->noise_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->noise_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PITCH_TYPE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->pitch_type){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->pitch_type != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->pitch_type));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->pitch_type = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PITCH_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->pitch_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->pitch_tuning != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->pitch_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->pitch_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_ENABLED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->chorus_enabled){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->chorus_enabled != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->chorus_enabled));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->chorus_enabled = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_PITCH_TYPE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->chorus_pitch_type){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->chorus_pitch_type != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->chorus_pitch_type));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->chorus_pitch_type = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_INPUT_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->chorus_input_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->chorus_input_volume != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->chorus_input_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->chorus_input_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_OUTPUT_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->chorus_output_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->chorus_output_volume != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->chorus_output_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->chorus_output_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_LFO_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->chorus_lfo_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->chorus_lfo_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->chorus_lfo_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->chorus_lfo_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->chorus_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->chorus_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->chorus_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->chorus_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->chorus_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->chorus_depth != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->chorus_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->chorus_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_MIX:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->chorus_mix){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->chorus_mix != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->chorus_mix));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->chorus_mix = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_DELAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->chorus_delay){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->chorus_delay != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->chorus_delay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->chorus_delay = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_ENABLED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->vibrato_enabled){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->vibrato_enabled != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->vibrato_enabled));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->vibrato_enabled = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->vibrato_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->vibrato_gain != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->vibrato_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->vibrato_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_LFO_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->vibrato_lfo_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->vibrato_lfo_depth != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->vibrato_lfo_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->vibrato_lfo_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_LFO_FREQ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->vibrato_lfo_freq){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->vibrato_lfo_freq != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->vibrato_lfo_freq));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->vibrato_lfo_freq = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_raven_synth_audio->vibrato_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_raven_synth_audio->vibrato_tuning != NULL){
	g_object_unref(G_OBJECT(fx_raven_synth_audio->vibrato_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_raven_synth_audio->vibrato_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_raven_synth_audio_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec)
{
  AgsFxRavenSynthAudio *fx_raven_synth_audio;

  GRecMutex *recall_mutex;

  fx_raven_synth_audio = AGS_FX_RAVEN_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_raven_synth_audio);

  switch(prop_id){
  case PROP_SYNTH_0_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OCTAVE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_octave);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_KEY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_key);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PHASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_phase);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_4:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_4);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_5:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_5);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_6:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_6);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_7:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_7);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_8:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_8);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_9:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_9);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_10:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_10);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_11:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_11);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_12:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_12);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_13:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_13);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_14:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_14);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_15:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_15);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_PINGPONG:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_pingpong);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_tuning_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_4:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_4);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_5:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_5);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_6:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_6);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_7:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_7);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_8:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_8);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_9:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_9);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_10:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_10);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_11:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_11);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_12:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_12);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_13:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_13);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_14:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_14);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_15:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_15);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_PINGPONG:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_pingpong);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_seq_volume_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ENABLED:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_sync_enabled);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_sync_relative_attack_factor_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_sync_relative_attack_factor_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_sync_relative_attack_factor_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_sync_relative_attack_factor_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_sync_attack_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_sync_phase_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_sync_attack_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_sync_phase_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_sync_attack_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_sync_phase_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_sync_attack_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_sync_phase_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_lfo_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_lfo_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_0_lfo_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_0_CUT_OFF_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->low_pass_0_cut_off_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_0_FILTER_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->low_pass_0_filter_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_0_NO_CLIP:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->low_pass_0_no_clip);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_0_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->amplifier_0_amp_0_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_1_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->amplifier_0_amp_1_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_2_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->amplifier_0_amp_2_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_AMP_3_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->amplifier_0_amp_3_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_0_FILTER_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->amplifier_0_filter_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_OCTAVE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_octave);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_KEY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_key);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_PHASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_phase);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_4:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_4);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_5:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_5);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_6:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_6);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_7:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_7);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_8:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_8);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_9:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_9);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_10:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_10);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_11:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_11);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_12:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_12);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_13:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_13);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_14:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_14);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_15:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_15);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_PINGPONG:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_pingpong);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_tuning_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_4:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_4);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_5:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_5);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_6:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_6);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_7:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_7);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_8:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_8);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_9:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_9);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_10:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_10);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_11:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_11);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_12:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_12);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_13:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_13);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_14:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_14);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_15:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_15);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_PINGPONG:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_pingpong);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_seq_volume_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ENABLED:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_sync_enabled);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_sync_relative_attack_factor_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_sync_relative_attack_factor_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_sync_relative_attack_factor_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_sync_relative_attack_factor_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_sync_attack_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_sync_phase_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_sync_attack_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_sync_phase_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_sync_attack_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_sync_phase_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_sync_attack_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_sync_phase_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_lfo_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_lfo_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->synth_1_lfo_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_1_CUT_OFF_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->low_pass_1_cut_off_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_1_FILTER_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->low_pass_1_filter_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_LOW_PASS_1_NO_CLIP:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->low_pass_1_no_clip);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_1_AMP_0_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->amplifier_1_amp_0_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_1_AMP_1_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->amplifier_1_amp_1_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_1_AMP_2_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->amplifier_1_amp_2_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_1_AMP_3_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->amplifier_1_amp_3_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_AMPLIFIER_1_FILTER_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->amplifier_1_filter_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_NOISE_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->noise_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PITCH_TYPE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->pitch_type);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PITCH_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->pitch_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_ENABLED:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->chorus_enabled);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_PITCH_TYPE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->chorus_pitch_type);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_INPUT_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->chorus_input_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_OUTPUT_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->chorus_output_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_LFO_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->chorus_lfo_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->chorus_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->chorus_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_MIX:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->chorus_mix);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_DELAY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->chorus_delay);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_ENABLED:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->vibrato_enabled);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->vibrato_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_LFO_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->vibrato_lfo_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_LFO_FREQ:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->vibrato_lfo_freq);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_raven_synth_audio->vibrato_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_raven_synth_audio_dispose(GObject *gobject)
{
  AgsFxRavenSynthAudio *fx_raven_synth_audio;
  
  fx_raven_synth_audio = AGS_FX_RAVEN_SYNTH_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_raven_synth_audio_parent_class)->dispose(gobject);
}

void
ags_fx_raven_synth_audio_finalize(GObject *gobject)
{
  AgsFxRavenSynthAudio *fx_raven_synth_audio;

  guint i;
  
  fx_raven_synth_audio = AGS_FX_RAVEN_SYNTH_AUDIO(gobject);

  
  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      ags_fx_raven_synth_audio_scope_data_free(fx_raven_synth_audio->scope_data[i]);
    }
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_raven_synth_audio_parent_class)->finalize(gobject);
}

void
ags_fx_raven_synth_audio_notify_audio_callback(GObject *gobject,
					       GParamSpec *pspec,
					       gpointer user_data)
{
  AgsAudio *audio;
  AgsFxRavenSynthAudio *fx_raven_synth_audio;

  fx_raven_synth_audio = AGS_FX_RAVEN_SYNTH_AUDIO(gobject);

  /* get audio */
  audio = NULL;

  g_object_get(fx_raven_synth_audio,
	       "audio", &audio,
	       NULL);

  g_signal_connect_after((GObject *) audio, "set-audio-channels",
			 G_CALLBACK(ags_fx_raven_synth_audio_set_audio_channels_callback), fx_raven_synth_audio);

  if(audio != NULL){
    g_object_unref(audio);
  }
}

void
ags_fx_raven_synth_audio_notify_buffer_size_callback(GObject *gobject,
						     GParamSpec *pspec,
						     gpointer user_data)
{
  AgsFxRavenSynthAudio *fx_raven_synth_audio;

  guint buffer_size;
  guint format;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_raven_synth_audio = AGS_FX_RAVEN_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_raven_synth_audio);

  /* get buffer size */
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  format =  AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  g_object_get(fx_raven_synth_audio,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxRavenSynthAudioScopeData *scope_data;

    scope_data = fx_raven_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxRavenSynthAudioChannelData *channel_data;

	gpointer destination;

	channel_data = scope_data->channel_data[j];
 
	/* free chorus destination */
	destination = ags_chorus_util_get_destination(channel_data->chorus_util);
	
	ags_stream_free(destination);

	/* alloc chorus destination */
	destination = NULL;

	if(buffer_size > 0){
	  destination = ags_stream_alloc(buffer_size,
					 ags_chorus_util_get_format(channel_data->chorus_util));
	}
	
	ags_chorus_util_set_destination(channel_data->chorus_util,
					destination);

	/* synth buffer */
	ags_stream_free(channel_data->synth_buffer_0);
	ags_stream_free(channel_data->synth_buffer_1);

	channel_data->synth_buffer_0 = NULL;
	channel_data->synth_buffer_1 = NULL;

	if(buffer_size > 0){
	  channel_data->synth_buffer_0 = ags_stream_alloc(buffer_size,
							  format);
	  
	  channel_data->synth_buffer_1 = ags_stream_alloc(buffer_size,
							  format);
	}
	
	/* set buffer length */
	ags_raven_synth_util_set_buffer_length(channel_data->raven_synth_0,
					       buffer_size);
	
	ags_raven_synth_util_set_buffer_length(channel_data->raven_synth_1,
					       buffer_size);

	ags_low_pass_filter_util_set_buffer_length(channel_data->low_pass_filter_util_0,
						   buffer_size);
	
	ags_low_pass_filter_util_set_buffer_length(channel_data->low_pass_filter_util_1,
						   buffer_size);
	
	ags_amplifier_util_set_buffer_length(channel_data->amplifier_util_0,
					     buffer_size);
	
	ags_amplifier_util_set_buffer_length(channel_data->amplifier_util_1,
					     buffer_size);

	ags_noise_util_set_buffer_length(channel_data->noise_util,
					 buffer_size);

	ags_common_pitch_util_set_buffer_length(channel_data->pitch_util,
						channel_data->pitch_type,
						buffer_size);

	ags_chorus_util_set_buffer_length(channel_data->chorus_util,
					  buffer_size);
      }
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_raven_synth_audio_notify_format_callback(GObject *gobject,
						GParamSpec *pspec,
						gpointer user_data)
{
  AgsFxRavenSynthAudio *fx_raven_synth_audio;

  guint buffer_size;
  guint format;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_raven_synth_audio = AGS_FX_RAVEN_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_raven_synth_audio);

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  format =  AGS_SOUNDCARD_DEFAULT_FORMAT;

  g_object_get(fx_raven_synth_audio,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxRavenSynthAudioScopeData *scope_data;

    scope_data = fx_raven_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxRavenSynthAudioChannelData *channel_data;

	gpointer destination;
	
	channel_data = scope_data->channel_data[j];

	ags_raven_synth_util_set_format(channel_data->raven_synth_0,
					format);

	ags_raven_synth_util_set_format(channel_data->raven_synth_1,
					format);

	ags_low_pass_filter_util_set_format(channel_data->low_pass_filter_util_0,
					    format);

	ags_low_pass_filter_util_set_format(channel_data->low_pass_filter_util_1,
					    format);

	ags_amplifier_util_set_format(channel_data->amplifier_util_0,
				      format);

	ags_amplifier_util_set_format(channel_data->amplifier_util_1,
				      format);
	
	/* free chorus destination */
	destination = ags_chorus_util_get_destination(channel_data->chorus_util);
	
	ags_stream_free(destination);

	/* alloc chorus destination */
	destination = NULL;

	if(ags_chorus_util_get_buffer_length(channel_data->chorus_util) > 0){
	  destination = ags_stream_alloc(ags_chorus_util_get_buffer_length(channel_data->chorus_util),
					 format);
	}
	
	ags_chorus_util_set_destination(channel_data->chorus_util,
					destination);

	/* synth buffer */
	ags_stream_free(channel_data->synth_buffer_0);
	ags_stream_free(channel_data->synth_buffer_1);

	channel_data->synth_buffer_0 = NULL;
	channel_data->synth_buffer_1 = NULL;

	if(buffer_size > 0){
	  channel_data->synth_buffer_0 = ags_stream_alloc(buffer_size,
							  format);

	  channel_data->synth_buffer_1 = ags_stream_alloc(buffer_size,
							  format);
	}

	/*  */
	ags_noise_util_set_format(channel_data->noise_util,
				  format);

	ags_common_pitch_util_set_format(channel_data->pitch_util,
					 channel_data->pitch_type,
					 format);

	ags_chorus_util_set_format(channel_data->chorus_util,
				   format);
      }
    }
  }

  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_raven_synth_audio_notify_samplerate_callback(GObject *gobject,
						    GParamSpec *pspec,
						    gpointer user_data)
{
  AgsFxRavenSynthAudio *fx_raven_synth_audio;

  guint samplerate;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_raven_synth_audio = AGS_FX_RAVEN_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_raven_synth_audio);

  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  g_object_get(fx_raven_synth_audio,
	       "samplerate", &samplerate,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxRavenSynthAudioScopeData *scope_data;

    scope_data = fx_raven_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxRavenSynthAudioChannelData *channel_data;

	channel_data = scope_data->channel_data[j];

	ags_raven_synth_util_set_samplerate(channel_data->raven_synth_0,
					    samplerate);

	ags_raven_synth_util_set_samplerate(channel_data->raven_synth_1,
					    samplerate);
	
	ags_low_pass_filter_util_set_samplerate(channel_data->low_pass_filter_util_0,
						samplerate);

	ags_low_pass_filter_util_set_samplerate(channel_data->low_pass_filter_util_1,
						samplerate);

	ags_amplifier_util_set_samplerate(channel_data->amplifier_util_0,
					  samplerate);

	ags_amplifier_util_set_samplerate(channel_data->amplifier_util_1,
					  samplerate);
	
	ags_noise_util_set_samplerate(channel_data->noise_util,
				      samplerate);

	ags_common_pitch_util_set_samplerate(channel_data->pitch_util,
					     channel_data->pitch_type,
					     samplerate);

	ags_chorus_util_set_samplerate(channel_data->chorus_util,
				       samplerate);

	ags_chorus_util_set_samplerate(channel_data->chorus_util,
				       samplerate);
      }
    }
  }

  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_raven_synth_audio_set_audio_channels_callback(AgsAudio *audio,
						     guint audio_channels, guint audio_channels_old,
						     AgsFxRavenSynthAudio *fx_raven_synth_audio)
{
  guint input_pads;
  guint output_port_count, input_port_count;
  guint buffer_size;
  guint format;
  guint samplerate;
  guint i, j, k;

  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_raven_synth_audio);

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
    AgsFxRavenSynthAudioScopeData *scope_data;

    scope_data = fx_raven_synth_audio->scope_data[i];

    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      if(scope_data->audio_channels > audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxRavenSynthAudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];
	
	  ags_fx_raven_synth_audio_channel_data_free(channel_data);
	}
      }
      
      if(scope_data->channel_data == NULL){
	scope_data->channel_data = (AgsFxRavenSynthAudioChannelData **) g_malloc(audio_channels * sizeof(AgsFxRavenSynthAudioChannelData *)); 
      }else{
	scope_data->channel_data = (AgsFxRavenSynthAudioChannelData **) g_realloc(scope_data->channel_data,
										  audio_channels * sizeof(AgsFxRavenSynthAudioChannelData *)); 
      }

      if(scope_data->audio_channels < audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxRavenSynthAudioChannelData *channel_data;

	  gpointer destination;  

	  channel_data =
	    scope_data->channel_data[j] = ags_fx_raven_synth_audio_channel_data_alloc();

	  channel_data->synth_buffer_0 = ags_stream_alloc(buffer_size,
							  format);

	  channel_data->synth_buffer_1 = ags_stream_alloc(buffer_size,
							  format);

	  ags_raven_synth_util_set_buffer_length(channel_data->raven_synth_0,
					       buffer_size);
	  ags_raven_synth_util_set_format(channel_data->raven_synth_0,
					format);		
	  ags_raven_synth_util_set_samplerate(channel_data->raven_synth_0,
					    samplerate);

	  ags_raven_synth_util_set_buffer_length(channel_data->raven_synth_1,
					       buffer_size);
	  ags_raven_synth_util_set_format(channel_data->raven_synth_1,
					format);		
	  ags_raven_synth_util_set_samplerate(channel_data->raven_synth_1,
					    samplerate);
	  
	  ags_noise_util_set_buffer_length(channel_data->noise_util,
					   buffer_size);					 
	  ags_noise_util_set_format(channel_data->noise_util,
				    format);					 
	  ags_noise_util_set_samplerate(channel_data->noise_util,
					samplerate);

	  ags_common_pitch_util_set_buffer_length(channel_data->pitch_util,
						  channel_data->pitch_type,
						  buffer_size);
	  ags_common_pitch_util_set_format(channel_data->pitch_util,
					   channel_data->pitch_type,
					   format);
	  ags_common_pitch_util_set_samplerate(channel_data->pitch_util,
					       channel_data->pitch_type,
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
	    AgsFxRavenSynthAudioInputData *input_data;

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

void
ags_fx_raven_synth_audio_pitch_type_callback(AgsPort *port, GValue *value,
					     AgsFxRavenSynthAudio *fx_raven_synth_audio)
{
  AgsPitchTypeMode pitch_type_mode;

  gpointer pitch_util;
  gpointer tmp_pitch_util;
  gpointer source;
  gpointer destination;    

  GType pitch_type;  
  GType tmp_pitch_type;

  guint source_stride;
  guint destination_stride;
  guint buffer_length;
  AgsSoundcardFormat format;
  guint samplerate;
  gdouble base_key;
  gdouble tuning;
  guint i, j;
  gboolean success;

  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_raven_synth_audio);

  /* reset pitch util */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxRavenSynthAudioScopeData *scope_data;

    scope_data = fx_raven_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxRavenSynthAudioChannelData *channel_data;
	
	channel_data = scope_data->channel_data[j];	

	pitch_type_mode = (AgsPitchTypeMode) g_value_get_float(value);
  
	pitch_type = G_TYPE_NONE;

	switch(pitch_type_mode){
	case AGS_PITCH_TYPE_FLUID_INTERPOLATE_NONE:
	  {
	    pitch_type = AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL;
	  }
	  break;
	case AGS_PITCH_TYPE_FLUID_INTERPOLATE_LINEAR:
	  {
	    pitch_type = AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL;
	  }
	  break;
	case AGS_PITCH_TYPE_FLUID_INTERPOLATE_4TH_ORDER:
	  {
	    pitch_type = AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL;
	  }
	  break;
	case AGS_PITCH_TYPE_FLUID_INTERPOLATE_7TH_ORDER:
	  {
	    pitch_type = AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL;
	  }
	  break;
	case AGS_PITCH_TYPE_INTERPOLATE_2X_ALIAS:
	  {
	    pitch_type = AGS_TYPE_PITCH_2X_ALIAS_UTIL;
	  }
	  break;
	case AGS_PITCH_TYPE_INTERPOLATE_4X_ALIAS:
	  {
	    pitch_type = AGS_TYPE_PITCH_4X_ALIAS_UTIL;
	  }
	  break;
	case AGS_PITCH_TYPE_INTERPOLATE_16X_ALIAS:
	  {
	    pitch_type = AGS_TYPE_PITCH_16X_ALIAS_UTIL;
	  }
	  break;
	}

	tmp_pitch_type = channel_data->pitch_type;  
	tmp_pitch_util = channel_data->pitch_util;

	source = NULL;
	destination = NULL;
  
	success = FALSE;

	if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
	  success = TRUE;

	  pitch_util = 
	    channel_data->pitch_util = ags_fluid_interpolate_none_util_alloc();
	}else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
	  success = TRUE;
    
	  pitch_util = 
	    channel_data->pitch_util = ags_fluid_interpolate_linear_util_alloc();
	}else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
	  success = TRUE;
    
	  pitch_util = 
	    channel_data->pitch_util = ags_fluid_interpolate_4th_order_util_alloc();
	}else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
	  success = TRUE;
    
	  pitch_util = 
	    channel_data->pitch_util = ags_fluid_interpolate_7th_order_util_alloc();
	}else if(pitch_type == AGS_TYPE_PITCH_2X_ALIAS_UTIL){
	  success = TRUE;
    
	  pitch_util = 
	    channel_data->pitch_util = ags_pitch_2x_alias_util_alloc();
	}else if(pitch_type == AGS_TYPE_PITCH_4X_ALIAS_UTIL){
	  success = TRUE;
    
	  pitch_util = 
	    channel_data->pitch_util = ags_pitch_4x_alias_util_alloc();
	}else if(pitch_type == AGS_TYPE_PITCH_16X_ALIAS_UTIL){
	  success = TRUE;
    
	  pitch_util = 
	    channel_data->pitch_util = ags_pitch_16x_alias_util_alloc();
	}
  
	if(success){
	  channel_data->pitch_type = pitch_type;

	  source_stride = 1;
    
	  destination_stride = 1;

	  buffer_length = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
	  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
	  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

	  base_key = 0.0;
	  tuning = 0.0;
    
	  if(tmp_pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
	    source = ags_fluid_interpolate_none_util_get_source(tmp_pitch_util);
	    source_stride = ags_fluid_interpolate_none_util_get_source_stride(tmp_pitch_util);

	    destination = ags_fluid_interpolate_none_util_get_destination(tmp_pitch_util);
	    destination_stride = ags_fluid_interpolate_none_util_get_destination_stride(tmp_pitch_util);
      
	    buffer_length = ags_fluid_interpolate_none_util_get_buffer_length(tmp_pitch_util);
	    format = ags_fluid_interpolate_none_util_get_format(tmp_pitch_util);
	    samplerate = ags_fluid_interpolate_none_util_get_samplerate(tmp_pitch_util);

	    base_key = ags_fluid_interpolate_none_util_get_base_key(tmp_pitch_util);
	    tuning = ags_fluid_interpolate_none_util_get_tuning(tmp_pitch_util);

	    ags_fluid_interpolate_none_util_set_source(tmp_pitch_util,
						       NULL);
      
	    ags_fluid_interpolate_none_util_set_destination(tmp_pitch_util,
							    NULL);

	    ags_fluid_interpolate_none_util_free(tmp_pitch_util);
	  }else if(tmp_pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
	    source = ags_fluid_interpolate_linear_util_get_source(tmp_pitch_util);
	    source_stride = ags_fluid_interpolate_linear_util_get_source_stride(tmp_pitch_util);

	    destination = ags_fluid_interpolate_linear_util_get_destination(tmp_pitch_util);
	    destination_stride = ags_fluid_interpolate_linear_util_get_destination_stride(tmp_pitch_util);
      
	    buffer_length = ags_fluid_interpolate_linear_util_get_buffer_length(tmp_pitch_util);
	    format = ags_fluid_interpolate_linear_util_get_format(tmp_pitch_util);
	    samplerate = ags_fluid_interpolate_linear_util_get_samplerate(tmp_pitch_util);

	    base_key = ags_fluid_interpolate_linear_util_get_base_key(tmp_pitch_util);
	    tuning = ags_fluid_interpolate_linear_util_get_tuning(tmp_pitch_util);

	    ags_fluid_interpolate_linear_util_set_source(tmp_pitch_util,
							 NULL);

	    ags_fluid_interpolate_linear_util_set_destination(tmp_pitch_util,
							      NULL);

	    ags_fluid_interpolate_linear_util_free(tmp_pitch_util);
	  }else if(tmp_pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
	    source = ags_fluid_interpolate_4th_order_util_get_source(tmp_pitch_util);
	    source_stride = ags_fluid_interpolate_4th_order_util_get_source_stride(tmp_pitch_util);

	    destination = ags_fluid_interpolate_4th_order_util_get_destination(tmp_pitch_util);
	    destination_stride = ags_fluid_interpolate_4th_order_util_get_destination_stride(tmp_pitch_util);
      
	    buffer_length = ags_fluid_interpolate_4th_order_util_get_buffer_length(tmp_pitch_util);
	    format = ags_fluid_interpolate_4th_order_util_get_format(tmp_pitch_util);
	    samplerate = ags_fluid_interpolate_4th_order_util_get_samplerate(tmp_pitch_util);

	    base_key = ags_fluid_interpolate_4th_order_util_get_base_key(tmp_pitch_util);
	    tuning = ags_fluid_interpolate_4th_order_util_get_tuning(tmp_pitch_util);

	    ags_fluid_interpolate_4th_order_util_set_source(tmp_pitch_util,
							    NULL);

	    ags_fluid_interpolate_4th_order_util_set_destination(tmp_pitch_util,
								 NULL);

	    ags_fluid_interpolate_4th_order_util_free(tmp_pitch_util);
	  }else if(tmp_pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
	    source = ags_fluid_interpolate_7th_order_util_get_source(tmp_pitch_util);
	    source_stride = ags_fluid_interpolate_7th_order_util_get_source_stride(tmp_pitch_util);

	    destination = ags_fluid_interpolate_7th_order_util_get_destination(tmp_pitch_util);
	    destination_stride = ags_fluid_interpolate_7th_order_util_get_destination_stride(tmp_pitch_util);
      
	    buffer_length = ags_fluid_interpolate_7th_order_util_get_buffer_length(tmp_pitch_util);
	    format = ags_fluid_interpolate_7th_order_util_get_format(tmp_pitch_util);
	    samplerate = ags_fluid_interpolate_7th_order_util_get_samplerate(tmp_pitch_util);

	    base_key = ags_fluid_interpolate_7th_order_util_get_base_key(tmp_pitch_util);
	    tuning = ags_fluid_interpolate_7th_order_util_get_tuning(tmp_pitch_util);

	    ags_fluid_interpolate_7th_order_util_set_source(tmp_pitch_util,
							    NULL);

	    ags_fluid_interpolate_7th_order_util_set_destination(tmp_pitch_util,
								 NULL);

	    ags_fluid_interpolate_7th_order_util_free(tmp_pitch_util);
	  }else if(tmp_pitch_type == AGS_TYPE_PITCH_2X_ALIAS_UTIL){
	    source = ags_pitch_2x_alias_util_get_source(tmp_pitch_util);
	    source_stride = ags_pitch_2x_alias_util_get_source_stride(tmp_pitch_util);

	    destination = ags_pitch_2x_alias_util_get_destination(tmp_pitch_util);
	    destination_stride = ags_pitch_2x_alias_util_get_destination_stride(tmp_pitch_util);
      
	    buffer_length = ags_pitch_2x_alias_util_get_buffer_length(tmp_pitch_util);
	    format = ags_pitch_2x_alias_util_get_format(tmp_pitch_util);
	    samplerate = ags_pitch_2x_alias_util_get_samplerate(tmp_pitch_util);

	    base_key = ags_pitch_2x_alias_util_get_base_key(tmp_pitch_util);
	    tuning = ags_pitch_2x_alias_util_get_tuning(tmp_pitch_util);

	    ags_pitch_2x_alias_util_set_source(tmp_pitch_util,
					       NULL);

	    ags_pitch_2x_alias_util_set_destination(tmp_pitch_util,
						    NULL);

	    ags_pitch_2x_alias_util_free(tmp_pitch_util);
	  }else if(tmp_pitch_type == AGS_TYPE_PITCH_4X_ALIAS_UTIL){
	    source = ags_pitch_4x_alias_util_get_source(tmp_pitch_util);
	    source_stride = ags_pitch_4x_alias_util_get_source_stride(tmp_pitch_util);

	    destination = ags_pitch_4x_alias_util_get_destination(tmp_pitch_util);
	    destination_stride = ags_pitch_4x_alias_util_get_destination_stride(tmp_pitch_util);
      
	    buffer_length = ags_pitch_4x_alias_util_get_buffer_length(tmp_pitch_util);
	    format = ags_pitch_4x_alias_util_get_format(tmp_pitch_util);
	    samplerate = ags_pitch_4x_alias_util_get_samplerate(tmp_pitch_util);

	    base_key = ags_pitch_4x_alias_util_get_base_key(tmp_pitch_util);
	    tuning = ags_pitch_4x_alias_util_get_tuning(tmp_pitch_util);

	    ags_pitch_4x_alias_util_set_source(tmp_pitch_util,
					       NULL);

	    ags_pitch_4x_alias_util_set_destination(tmp_pitch_util,
						    NULL);

	    ags_pitch_4x_alias_util_free(tmp_pitch_util);
	  }else if(tmp_pitch_type == AGS_TYPE_PITCH_16X_ALIAS_UTIL){
	    source = ags_pitch_16x_alias_util_get_source(tmp_pitch_util);
	    source_stride = ags_pitch_16x_alias_util_get_source_stride(tmp_pitch_util);

	    destination = ags_pitch_16x_alias_util_get_destination(tmp_pitch_util);
	    destination_stride = ags_pitch_16x_alias_util_get_destination_stride(tmp_pitch_util);
      
	    buffer_length = ags_pitch_16x_alias_util_get_buffer_length(tmp_pitch_util);
	    format = ags_pitch_16x_alias_util_get_format(tmp_pitch_util);
	    samplerate = ags_pitch_16x_alias_util_get_samplerate(tmp_pitch_util);

	    base_key = ags_pitch_16x_alias_util_get_base_key(tmp_pitch_util);
	    tuning = ags_pitch_16x_alias_util_get_tuning(tmp_pitch_util);

	    ags_pitch_16x_alias_util_set_source(tmp_pitch_util,
						NULL);

	    ags_pitch_16x_alias_util_set_destination(tmp_pitch_util,
						     NULL);

	    ags_pitch_16x_alias_util_free(tmp_pitch_util);
	  }

	  /* set source and destination */
	  if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL){
	    ags_fluid_interpolate_none_util_set_source(pitch_util,
						       source);
	    ags_fluid_interpolate_none_util_set_source_stride(pitch_util,
							      source_stride);
      
	    ags_fluid_interpolate_none_util_set_destination(pitch_util,
							    destination);
	    ags_fluid_interpolate_none_util_set_destination_stride(pitch_util,
								   destination_stride);      

	    ags_fluid_interpolate_none_util_set_buffer_length(pitch_util,
							      buffer_length);      
	    ags_fluid_interpolate_none_util_set_format(pitch_util,
						       format);      
	    ags_fluid_interpolate_none_util_set_samplerate(pitch_util,
							   samplerate);      

	    ags_fluid_interpolate_none_util_set_base_key(pitch_util,
							 base_key);      
	    ags_fluid_interpolate_none_util_set_tuning(pitch_util,
						       tuning);      
	  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL){
	    ags_fluid_interpolate_linear_util_set_source(pitch_util,
							 source);
	    ags_fluid_interpolate_linear_util_set_source_stride(pitch_util,
								source_stride);
      
	    ags_fluid_interpolate_linear_util_set_destination(pitch_util,
							      destination);
	    ags_fluid_interpolate_linear_util_set_destination_stride(pitch_util,
								     destination_stride);      

	    ags_fluid_interpolate_linear_util_set_buffer_length(pitch_util,
								buffer_length);      
	    ags_fluid_interpolate_linear_util_set_format(pitch_util,
							 format);      
	    ags_fluid_interpolate_linear_util_set_samplerate(pitch_util,
							     samplerate);      

	    ags_fluid_interpolate_linear_util_set_base_key(pitch_util,
							   base_key);      
	    ags_fluid_interpolate_linear_util_set_tuning(pitch_util,
							 tuning);      
	  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL){
	    ags_fluid_interpolate_4th_order_util_set_source(pitch_util,
							    source);
	    ags_fluid_interpolate_4th_order_util_set_source_stride(pitch_util,
								   source_stride);
      
	    ags_fluid_interpolate_4th_order_util_set_destination(pitch_util,
								 destination);
	    ags_fluid_interpolate_4th_order_util_set_destination_stride(pitch_util,
									destination_stride);      

	    ags_fluid_interpolate_4th_order_util_set_buffer_length(pitch_util,
								   buffer_length);      
	    ags_fluid_interpolate_4th_order_util_set_format(pitch_util,
							    format);      
	    ags_fluid_interpolate_4th_order_util_set_samplerate(pitch_util,
								samplerate);      

	    ags_fluid_interpolate_4th_order_util_set_base_key(pitch_util,
							      base_key);      
	    ags_fluid_interpolate_4th_order_util_set_tuning(pitch_util,
							    tuning);      
	  }else if(pitch_type == AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL){
	    ags_fluid_interpolate_7th_order_util_set_source(pitch_util,
							    source);
	    ags_fluid_interpolate_7th_order_util_set_source_stride(pitch_util,
								   source_stride);
      
	    ags_fluid_interpolate_7th_order_util_set_destination(pitch_util,
								 destination);
	    ags_fluid_interpolate_7th_order_util_set_destination_stride(pitch_util,
									destination_stride);      

	    ags_fluid_interpolate_7th_order_util_set_buffer_length(pitch_util,
								   buffer_length);      
	    ags_fluid_interpolate_7th_order_util_set_format(pitch_util,
							    format);      
	    ags_fluid_interpolate_7th_order_util_set_samplerate(pitch_util,
								samplerate);      

	    ags_fluid_interpolate_7th_order_util_set_base_key(pitch_util,
							      base_key);      
	    ags_fluid_interpolate_7th_order_util_set_tuning(pitch_util,
							    tuning);      
	  }else if(pitch_type == AGS_TYPE_PITCH_2X_ALIAS_UTIL){
	    ags_pitch_2x_alias_util_set_source(pitch_util,
					       source);
	    ags_pitch_2x_alias_util_set_source_stride(pitch_util,
						      source_stride);
      
	    ags_pitch_2x_alias_util_set_destination(pitch_util,
						    destination);
	    ags_pitch_2x_alias_util_set_destination_stride(pitch_util,
							   destination_stride);      

	    ags_pitch_2x_alias_util_set_buffer_length(pitch_util,
						      buffer_length);      
	    ags_pitch_2x_alias_util_set_format(pitch_util,
					       format);      
	    ags_pitch_2x_alias_util_set_samplerate(pitch_util,
						   samplerate);      

	    ags_pitch_2x_alias_util_set_base_key(pitch_util,
						 base_key);      
	    ags_pitch_2x_alias_util_set_tuning(pitch_util,
					       tuning);      
	  }else if(pitch_type == AGS_TYPE_PITCH_4X_ALIAS_UTIL){
	    ags_pitch_4x_alias_util_set_source(pitch_util,
					       source);
	    ags_pitch_4x_alias_util_set_source_stride(pitch_util,
						      source_stride);
      
	    ags_pitch_4x_alias_util_set_destination(pitch_util,
						    destination);
	    ags_pitch_4x_alias_util_set_destination_stride(pitch_util,
							   destination_stride);      

	    ags_pitch_4x_alias_util_set_buffer_length(pitch_util,
						      buffer_length);      
	    ags_pitch_4x_alias_util_set_format(pitch_util,
					       format);      
	    ags_pitch_4x_alias_util_set_samplerate(pitch_util,
						   samplerate);      

	    ags_pitch_4x_alias_util_set_base_key(pitch_util,
						 base_key);      
	    ags_pitch_4x_alias_util_set_tuning(pitch_util,
					       tuning);      
	  }else if(pitch_type == AGS_TYPE_PITCH_16X_ALIAS_UTIL){
	    ags_pitch_16x_alias_util_set_source(pitch_util,
						source);
	    ags_pitch_16x_alias_util_set_source_stride(pitch_util,
						       source_stride);
      
	    ags_pitch_16x_alias_util_set_destination(pitch_util,
						     destination);
	    ags_pitch_16x_alias_util_set_destination_stride(pitch_util,
							    destination_stride);      

	    ags_pitch_16x_alias_util_set_buffer_length(pitch_util,
						       buffer_length);      
	    ags_pitch_16x_alias_util_set_format(pitch_util,
						format);      
	    ags_pitch_16x_alias_util_set_samplerate(pitch_util,
						    samplerate);      

	    ags_pitch_16x_alias_util_set_base_key(pitch_util,
						  base_key);      
	    ags_pitch_16x_alias_util_set_tuning(pitch_util,
						tuning);      
	  }
	}
      }
    }
  }  
 
  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_raven_synth_audio_chorus_pitch_type_callback(AgsPort *port, GValue *value,
						    AgsFxRavenSynthAudio *fx_raven_synth_audio)
{
  AgsPitchTypeMode pitch_type;

  GType pitch_gtype;
  
  guint i, j;

  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_raven_synth_audio);

  /* reset pitch util */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxRavenSynthAudioScopeData *scope_data;

    scope_data = fx_raven_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxRavenSynthAudioChannelData *channel_data;
	
	channel_data = scope_data->channel_data[j];	

	pitch_type = (AgsPitchTypeMode) g_value_get_float(value);

	pitch_gtype = G_TYPE_NONE;
  
	switch(pitch_type){
	case AGS_PITCH_TYPE_FLUID_INTERPOLATE_NONE:
	  {
	    pitch_gtype = AGS_TYPE_FLUID_INTERPOLATE_NONE_UTIL;
	  }
	  break;
	case AGS_PITCH_TYPE_FLUID_INTERPOLATE_LINEAR:
	  {
	    pitch_gtype = AGS_TYPE_FLUID_INTERPOLATE_LINEAR_UTIL;
	  }
	  break;
	case AGS_PITCH_TYPE_FLUID_INTERPOLATE_4TH_ORDER:
	  {
	    pitch_gtype = AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL;
	  }
	  break;
	case AGS_PITCH_TYPE_FLUID_INTERPOLATE_7TH_ORDER:
	  {
	    pitch_gtype = AGS_TYPE_FLUID_INTERPOLATE_7TH_ORDER_UTIL;
	  }
	  break;
	case AGS_PITCH_TYPE_INTERPOLATE_2X_ALIAS:
	  {
	    pitch_gtype = AGS_TYPE_PITCH_2X_ALIAS_UTIL;
	  }
	  break;
	case AGS_PITCH_TYPE_INTERPOLATE_4X_ALIAS:
	  {
	    pitch_gtype = AGS_TYPE_PITCH_4X_ALIAS_UTIL;
	  }
	  break;
	case AGS_PITCH_TYPE_INTERPOLATE_16X_ALIAS:
	  {
	    pitch_gtype = AGS_TYPE_PITCH_16X_ALIAS_UTIL;
	  }
	  break;
	}

	ags_chorus_util_set_pitch_type(channel_data->chorus_util,
				       pitch_gtype);
      }
    }
  }  
 
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_raven_synth_audio_scope_data_alloc:
 * 
 * Allocate #AgsFxRavenSynthAudioScopeData-struct
 * 
 * Returns: the new #AgsFxRavenSynthAudioScopeData-struct
 * 
 * Since: 7.7.0
 */
AgsFxRavenSynthAudioScopeData*
ags_fx_raven_synth_audio_scope_data_alloc()
{
  AgsFxRavenSynthAudioScopeData *scope_data;

  scope_data = (AgsFxRavenSynthAudioScopeData *) g_malloc(sizeof(AgsFxRavenSynthAudioScopeData));

  g_rec_mutex_init(&(scope_data->strct_mutex));
  
  scope_data->parent = NULL;
  
  scope_data->audio_channels = 0;

  scope_data->channel_data = NULL;
  
  return(scope_data);
}

/**
 * ags_fx_raven_synth_audio_scope_data_free:
 * @scope_data: the #AgsFxRavenSynthAudioScopeData-struct
 * 
 * Free @scope_data.
 * 
 * Since: 7.7.0
 */
void
ags_fx_raven_synth_audio_scope_data_free(AgsFxRavenSynthAudioScopeData *scope_data)
{
  guint i;

  if(scope_data == NULL){
    return;
  }

  for(i = 0; i < scope_data->audio_channels; i++){
    ags_fx_raven_synth_audio_channel_data_free(scope_data->channel_data[i]);
  }

  g_free(scope_data);
}

/**
 * ags_fx_raven_synth_audio_channel_data_alloc:
 * 
 * Allocate #AgsFxRavenSynthAudioChannelData-struct
 * 
 * Returns: the new #AgsFxRavenSynthAudioChannelData-struct
 * 
 * Since: 7.7.0
 */
AgsFxRavenSynthAudioChannelData*
ags_fx_raven_synth_audio_channel_data_alloc()
{
  AgsFxRavenSynthAudioChannelData *channel_data;

  guint i;
  
  channel_data = (AgsFxRavenSynthAudioChannelData *) g_malloc(sizeof(AgsFxRavenSynthAudioChannelData));

  g_rec_mutex_init(&(channel_data->strct_mutex));

  channel_data->parent = NULL;

  channel_data->synth_buffer_0 = NULL;
  channel_data->synth_buffer_1 = NULL;

  /* seq synth util */
  channel_data->raven_synth_0 = ags_raven_synth_util_alloc();
  channel_data->raven_synth_1 = ags_raven_synth_util_alloc();

  /* low-pass filter util */
  channel_data->low_pass_filter_util_0 = ags_low_pass_filter_util_alloc();
  channel_data->low_pass_filter_util_1 = ags_low_pass_filter_util_alloc();

  /* amplifier util */
  channel_data->amplifier_util_0 = ags_amplifier_util_alloc();
  channel_data->amplifier_util_1 = ags_amplifier_util_alloc();

  /* noise util */
  channel_data->noise_util = ags_noise_util_alloc();

  /* pitch util */
  channel_data->pitch_type = AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL;
  channel_data->pitch_util = ags_fluid_interpolate_4th_order_util_alloc();

  /* chorus util */
  channel_data->chorus_util = ags_chorus_util_alloc();
  
  /* chorus util */
  channel_data->chorus_util = ags_chorus_util_alloc();
  
  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    channel_data->input_data[i] = ags_fx_raven_synth_audio_input_data_alloc();

    channel_data->input_data[i]->parent = channel_data;
  }

  return(channel_data);
}

/**
 * ags_fx_raven_synth_audio_channel_data_free:
 * @channel_data: the #AgsFxRavenSynthAudioChannelData-struct
 * 
 * Free @channel_data.
 * 
 * Since: 7.7.0
 */
void
ags_fx_raven_synth_audio_channel_data_free(AgsFxRavenSynthAudioChannelData *channel_data)
{
  guint i;

  if(channel_data == NULL){
    return;
  }

  /* seq synth util */
  ags_raven_synth_util_free(channel_data->raven_synth_0);
  ags_raven_synth_util_free(channel_data->raven_synth_1);
  
  /* chorus util */
  ags_chorus_util_free(channel_data->chorus_util);
  
  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    ags_fx_raven_synth_audio_input_data_free(channel_data->input_data[i]);
  }

  g_free(channel_data);
}

/**
 * ags_fx_raven_synth_audio_input_data_alloc:
 * 
 * Allocate #AgsFxRavenSynthAudioInputData-struct
 * 
 * Returns: the new #AgsFxRavenSynthAudioInputData-struct
 * 
 * Since: 7.7.0
 */
AgsFxRavenSynthAudioInputData*
ags_fx_raven_synth_audio_input_data_alloc()
{
  AgsFxRavenSynthAudioInputData *input_data;

  input_data = (AgsFxRavenSynthAudioInputData *) g_malloc(sizeof(AgsFxRavenSynthAudioInputData));

  g_rec_mutex_init(&(input_data->strct_mutex));

  input_data->parent = NULL;

  input_data->key_on = 0;
  
  return(input_data);
}

/**
 * ags_fx_raven_synth_audio_input_data_free:
 * @input_data: the #AgsFxRavenSynthAudioInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 7.7.0
 */
void
ags_fx_raven_synth_audio_input_data_free(AgsFxRavenSynthAudioInputData *input_data)
{
  if(input_data == NULL){
    return;
  }

  g_free(input_data);
}

static AgsPluginPort*
ags_fx_raven_synth_audio_get_synth_oscillator_plugin_port()
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
ags_fx_raven_synth_audio_get_synth_octave_plugin_port()
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
ags_fx_raven_synth_audio_get_synth_key_plugin_port()
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
ags_fx_raven_synth_audio_get_synth_phase_plugin_port()
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
ags_fx_raven_synth_audio_get_synth_volume_plugin_port()
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
ags_fx_raven_synth_audio_get_synth_seq_tuning_plugin_port()
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
ags_fx_raven_synth_audio_get_synth_seq_tuning_pingpong_plugin_port()
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
ags_fx_raven_synth_audio_get_synth_seq_tuning_lfo_frequency_plugin_port()
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
ags_fx_raven_synth_audio_get_synth_seq_volume_plugin_port()
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
ags_fx_raven_synth_audio_get_synth_seq_volume_pingpong_plugin_port()
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
ags_fx_raven_synth_audio_get_synth_seq_volume_lfo_frequency_plugin_port()
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
ags_fx_raven_synth_audio_get_synth_sync_enabled_plugin_port()
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
ags_fx_raven_synth_audio_get_synth_sync_relative_attack_factor_plugin_port()
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
ags_fx_raven_synth_audio_get_synth_sync_attack_plugin_port()
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
		      12.0 * M_PI);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_raven_synth_audio_get_synth_sync_phase_plugin_port()
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
ags_fx_raven_synth_audio_get_synth_lfo_oscillator_plugin_port()
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
ags_fx_raven_synth_audio_get_synth_lfo_frequency_plugin_port()
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
		      AGS_RAVEN_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_raven_synth_audio_get_synth_lfo_depth_plugin_port()
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
		      AGS_RAVEN_SYNTH_UTIL_DEFAULT_LFO_DEPTH);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_raven_synth_audio_get_synth_lfo_tuning_plugin_port()
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
ags_fx_raven_synth_audio_get_low_pass_cut_off_frequency_plugin_port()
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
ags_fx_raven_synth_audio_get_low_pass_filter_gain_plugin_port()
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
ags_fx_raven_synth_audio_get_low_pass_no_clip_plugin_port()
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
ags_fx_raven_synth_audio_get_amplifier_amp_gain_plugin_port()
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
ags_fx_raven_synth_audio_get_amplifier_filter_gain_plugin_port()
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
ags_fx_raven_synth_audio_get_noise_gain_plugin_port()
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
ags_fx_raven_synth_audio_get_pitch_type_plugin_port()
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
		      6.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_raven_synth_audio_get_pitch_tuning_plugin_port()
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
ags_fx_raven_synth_audio_get_chorus_enabled_plugin_port()
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
ags_fx_raven_synth_audio_get_chorus_pitch_type_plugin_port()
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
ags_fx_raven_synth_audio_get_chorus_input_volume_plugin_port()
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
ags_fx_raven_synth_audio_get_chorus_output_volume_plugin_port()
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
ags_fx_raven_synth_audio_get_chorus_lfo_oscillator_plugin_port()
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
ags_fx_raven_synth_audio_get_chorus_lfo_frequency_plugin_port()
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
ags_fx_raven_synth_audio_get_chorus_depth_plugin_port()
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
ags_fx_raven_synth_audio_get_chorus_mix_plugin_port()
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
ags_fx_raven_synth_audio_get_chorus_delay_plugin_port()
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
ags_fx_raven_synth_audio_get_vibrato_enabled_plugin_port()
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
ags_fx_raven_synth_audio_get_vibrato_gain_plugin_port()
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
ags_fx_raven_synth_audio_get_vibrato_lfo_depth_plugin_port()
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
ags_fx_raven_synth_audio_get_vibrato_lfo_freq_plugin_port()
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
		      8.172);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      10.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_raven_synth_audio_get_vibrato_tuning_plugin_port()
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

/**
 * ags_fx_raven_synth_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxRavenSynthAudio
 *
 * Returns: the new #AgsFxRavenSynthAudio
 *
 * Since: 7.7.0
 */
AgsFxRavenSynthAudio*
ags_fx_raven_synth_audio_new(AgsAudio *audio)
{
  AgsFxRavenSynthAudio *fx_raven_synth_audio;

  fx_raven_synth_audio = (AgsFxRavenSynthAudio *) g_object_new(AGS_TYPE_FX_RAVEN_SYNTH_AUDIO,
							       "audio", audio,
							       NULL);
  
  return(fx_raven_synth_audio);
}
