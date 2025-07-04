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

#include <ags/audio/fx/ags_fx_seq_synth_audio.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_fx_seq_synth_audio_class_init(AgsFxSeqSynthAudioClass *fx_seq_synth_audio);
void ags_fx_seq_synth_audio_init(AgsFxSeqSynthAudio *fx_seq_synth_audio);
void ags_fx_seq_synth_audio_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_fx_seq_synth_audio_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_fx_seq_synth_audio_dispose(GObject *gobject);
void ags_fx_seq_synth_audio_finalize(GObject *gobject);

void ags_fx_seq_synth_audio_notify_audio_callback(GObject *gobject,
						  GParamSpec *pspec,
						  gpointer user_data);
void ags_fx_seq_synth_audio_notify_buffer_size_callback(GObject *gobject,
							GParamSpec *pspec,
							gpointer user_data);
void ags_fx_seq_synth_audio_notify_format_callback(GObject *gobject,
						   GParamSpec *pspec,
						   gpointer user_data);
void ags_fx_seq_synth_audio_notify_samplerate_callback(GObject *gobject,
						       GParamSpec *pspec,
						       gpointer user_data);

void ags_fx_seq_synth_audio_set_audio_channels_callback(AgsAudio *audio,
							guint audio_channels, guint audio_channels_old,
							AgsFxSeqSynthAudio *fx_seq_synth_audio);

void ags_fx_seq_synth_audio_pitch_type_callback(AgsPort *port, GValue *value,
						AgsFxSeqSynthAudio *fx_seq_synth_audio);

static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_oscillator_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_octave_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_key_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_phase_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_volume_plugin_port();

static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_seq_tuning_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_seq_tuning_pingpong_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_seq_tuning_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_seq_volume_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_seq_volume_pingpong_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_seq_volume_lfo_frequency_plugin_port();

static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_sync_enabled_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_sync_relative_attack_factor_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_sync_attack_0_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_sync_phase_0_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_sync_attack_1_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_sync_phase_1_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_sync_attack_2_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_sync_phase_2_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_sync_attack_3_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_sync_phase_3_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_sync_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_sync_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_lfo_depth_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_0_lfo_tuning_plugin_port();

static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_oscillator_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_octave_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_key_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_phase_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_volume_plugin_port();

static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_seq_tuning_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_seq_tuning_pingpong_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_seq_tuning_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_seq_volume_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_seq_volume_pingpong_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_seq_volume_lfo_frequency_plugin_port();

static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_sync_enabled_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_sync_relative_attack_factor_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_sync_attack_0_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_sync_phase_0_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_sync_attack_1_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_sync_phase_1_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_sync_attack_2_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_sync_phase_2_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_sync_attack_3_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_sync_phase_3_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_sync_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_sync_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_lfo_depth_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_synth_1_lfo_tuning_plugin_port();

static AgsPluginPort* ags_fx_seq_synth_audio_get_noise_gain_plugin_port();

static AgsPluginPort* ags_fx_seq_synth_audio_get_pitch_type_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_pitch_tuning_plugin_port();

static AgsPluginPort* ags_fx_seq_synth_audio_get_chorus_enabled_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_chorus_pitch_type_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_chorus_input_volume_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_chorus_output_volume_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_chorus_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_chorus_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_chorus_depth_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_chorus_mix_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_chorus_delay_plugin_port();

static AgsPluginPort* ags_fx_seq_synth_audio_get_vibrato_enabled_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_vibrato_gain_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_vibrato_lfo_depth_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_vibrato_lfo_freq_plugin_port();
static AgsPluginPort* ags_fx_seq_synth_audio_get_vibrato_tuning_plugin_port();

/**
 * SECTION:ags_fx_seq_synth_audio
 * @short_description: fx seq synth audio
 * @title: AgsFxSeqSynthAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_seq_synth_audio.h
 *
 * The #AgsFxSeqSynthAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_seq_synth_audio_parent_class = NULL;

const gchar *ags_fx_seq_synth_audio_plugin_name = "ags-fx-seq-synth";

const gchar* ags_fx_seq_synth_audio_specifier[] = {
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
  PROP_SYNTH_0_SEQ_VOLUME_PINGPONG,
  PROP_SYNTH_0_SEQ_VOLUME_LFO_FREQUENCY,
  PROP_SYNTH_0_SYNC_ENABLED,
  PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR,
  PROP_SYNTH_0_SYNC_ATTACK_0,
  PROP_SYNTH_0_SYNC_PHASE_0,
  PROP_SYNTH_0_SYNC_ATTACK_1,
  PROP_SYNTH_0_SYNC_PHASE_1,
  PROP_SYNTH_0_SYNC_ATTACK_2,
  PROP_SYNTH_0_SYNC_PHASE_2,
  PROP_SYNTH_0_SYNC_ATTACK_3,
  PROP_SYNTH_0_SYNC_PHASE_3,
  PROP_SYNTH_0_SYNC_LFO_OSCILLATOR,
  PROP_SYNTH_0_SYNC_LFO_FREQUENCY,
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
  PROP_SYNTH_1_SEQ_VOLUME_PINGPONG,
  PROP_SYNTH_1_SEQ_VOLUME_LFO_FREQUENCY,
  PROP_SYNTH_1_SYNC_ENABLED,
  PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR,
  PROP_SYNTH_1_SYNC_ATTACK_0,
  PROP_SYNTH_1_SYNC_PHASE_0,
  PROP_SYNTH_1_SYNC_ATTACK_1,
  PROP_SYNTH_1_SYNC_PHASE_1,
  PROP_SYNTH_1_SYNC_ATTACK_2,
  PROP_SYNTH_1_SYNC_PHASE_2,
  PROP_SYNTH_1_SYNC_ATTACK_3,
  PROP_SYNTH_1_SYNC_PHASE_3,
  PROP_SYNTH_1_SYNC_LFO_OSCILLATOR,
  PROP_SYNTH_1_SYNC_LFO_FREQUENCY,
  PROP_SYNTH_1_LFO_OSCILLATOR,
  PROP_SYNTH_1_LFO_FREQUENCY,
  PROP_SYNTH_1_LFO_DEPTH,
  PROP_SYNTH_1_LFO_TUNING,
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
ags_fx_seq_synth_audio_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_seq_synth_audio = 0;

    static const GTypeInfo ags_fx_seq_synth_audio_info = {
      sizeof (AgsFxSeqSynthAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_seq_synth_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxSeqSynthAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_seq_synth_audio_init,
    };

    ags_type_fx_seq_synth_audio = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO,
							 "AgsFxSeqSynthAudio",
							 &ags_fx_seq_synth_audio_info,
							 0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_seq_synth_audio);
  }

  return(g_define_type_id__static);
}

void
ags_fx_seq_synth_audio_class_init(AgsFxSeqSynthAudioClass *fx_seq_synth_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_seq_synth_audio_parent_class = g_type_class_peek_parent(fx_seq_synth_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_seq_synth_audio;

  gobject->set_property = ags_fx_seq_synth_audio_set_property;
  gobject->get_property = ags_fx_seq_synth_audio_get_property;

  gobject->dispose = ags_fx_seq_synth_audio_dispose;
  gobject->finalize = ags_fx_seq_synth_audio_finalize;

  /* properties */
  /**
   * AgsFxSeqSynthAudio:synth-0-oscillator:
   *
   * The synth-0 oscillator.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-octave:
   *
   * The synth-0 octave.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-key:
   *
   * The synth-0 key.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-phase:
   *
   * The synth-0 phase.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-volume:
   *
   * The synth-0 volume.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-tuning-0:
   *
   * The synth-0 sequencer tuning 0.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-tuning-1:
   *
   * The synth-0 sequencer tuning 1.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-tuning-2:
   *
   * The synth-0 sequencer tuning 2.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-tuning-3:
   *
   * The synth-0 sequencer tuning 3.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-tuning-4:
   *
   * The synth-0 sequencer tuning 4.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-tuning-5:
   *
   * The synth-0 sequencer tuning 5.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-tuning-6:
   *
   * The synth-0 sequencer tuning 6.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-tuning-7:
   *
   * The synth-0 sequencer tuning 7.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-tuning-pingpong:
   *
   * The synth-0 sequencer tuning pingpong.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-tuning-lfo-frequency:
   *
   * The synth-0 sequencer tuning lfo-frequency.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-volume-0:
   *
   * The synth-0 sequencer volume 0.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-volume-1:
   *
   * The synth-0 sequencer volume 1.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-volume-2:
   *
   * The synth-0 sequencer volume 2.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-volume-3:
   *
   * The synth-0 sequencer volume 3.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-volume-4:
   *
   * The synth-0 sequencer volume 4.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-volume-5:
   *
   * The synth-0 sequencer volume 5.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-volume-6:
   *
   * The synth-0 sequencer volume 6.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-volume-7:
   *
   * The synth-0 sequencer volume 7.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-volume-pingpong:
   *
   * The synth-0 sequencer volume pingpong.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-seq-volume-lfo-frequency:
   *
   * The synth-0 sequencer volume lfo-frequency.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-sync-enabled:
   *
   * The synth-0 sync enabled.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-sync-relative-attack-factor:
   *
   * The synth-0 sync relative attack factor.
   * 
   * Since: 7.5.0
   */
  param_spec = g_param_spec_object("synth-0-sync-relative-attack-factor",
				   i18n_pspec("synth-0 sync relative attack factor of recall"),
				   i18n_pspec("The synth-0's sync relative attack factor"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR,
				  param_spec);

  /**
   * AgsFxSeqSynthAudio:synth-0-sync-attack-0:
   *
   * The synth-0 sync attack-0.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-sync-phase-0:
   *
   * The synth-0 sync phase-0.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-sync-attack-1:
   *
   * The synth-0 sync attack-1.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-sync-phase-1:
   *
   * The synth-0 sync phase-1.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-sync-attack-2:
   *
   * The synth-0 sync attack-2.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-sync-phase-2:
   *
   * The synth-0 sync phase-2.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-sync-attack-3:
   *
   * The synth-0 sync attack-3.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-sync-phase-3:
   *
   * The synth-0 sync phase-3.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-sync-lfo-oscillator:
   *
   * The synth-0 sync LFO oscillator.
   * 
   * Since: 7.5.0
   */
  param_spec = g_param_spec_object("synth-0-sync-lfo-oscillator",
				   i18n_pspec("synth-0 sync LFO oscillator of recall"),
				   i18n_pspec("The synth-0's sync LFO oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_LFO_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxSeqSynthAudio:synth-0-sync-lfo-frequency:
   *
   * The synth-0 sync LFO frequency.
   * 
   * Since: 7.5.0
   */
  param_spec = g_param_spec_object("synth-0-sync-lfo-frequency",
				   i18n_pspec("synth-0 sync LFO frequency of recall"),
				   i18n_pspec("The synth-0's sync LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_SYNC_LFO_FREQUENCY,
				  param_spec);
  
  /**
   * AgsFxSeqSynthAudio:synth-0-lfo-oscillator:
   *
   * The synth-0 lfo-oscillator.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-lfo-frequency:
   *
   * The synth-0 LFO frequency.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-lfo-depth:
   *
   * The synth-0 LFO depth.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-0-lfo-tuning:
   *
   * The synth-0 LFO tuning.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-oscillator:
   *
   * The synth-1 oscillator.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-octave:
   *
   * The synth-1 octave.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-key:
   *
   * The synth-1 key.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-phase:
   *
   * The synth-1 phase.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-volume:
   *
   * The synth-1 volume.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-tuning-0:
   *
   * The synth-1 sequencer tuning 0.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-tuning-1:
   *
   * The synth-1 sequencer tuning 1.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-tuning-2:
   *
   * The synth-1 sequencer tuning 2.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-tuning-3:
   *
   * The synth-1 sequencer tuning 3.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-tuning-4:
   *
   * The synth-1 sequencer tuning 4.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-tuning-5:
   *
   * The synth-1 sequencer tuning 5.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-tuning-6:
   *
   * The synth-1 sequencer tuning 6.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-tuning-7:
   *
   * The synth-1 sequencer tuning 7.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-tuning-pingpong:
   *
   * The synth-1 sequencer tuning pingpong.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-volume-0:
   *
   * The synth-1 sequencer volume 0.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-volume-1:
   *
   * The synth-1 sequencer volume 1.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-volume-2:
   *
   * The synth-1 sequencer volume 2.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-volume-3:
   *
   * The synth-1 sequencer volume 3.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-volume-4:
   *
   * The synth-1 sequencer volume 4.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-volume-5:
   *
   * The synth-1 sequencer volume 5.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-volume-6:
   *
   * The synth-1 sequencer volume 6.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-volume-7:
   *
   * The synth-1 sequencer volume 7.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-volume-pingpong:
   *
   * The synth-1 sequencer volume pingpong.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-volume-lfo-frequency:
   *
   * The synth-1 sequencer volume lfo-frequency.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-seq-tuning-lfo-frequency:
   *
   * The synth-1 sequencer tuning lfo-frequency.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-sync-enabled:
   *
   * The synth-1 sync enabled.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-sync-relative-attack-factor:
   *
   * The synth-1 sync relative attack factor.
   * 
   * Since: 7.5.0
   */
  param_spec = g_param_spec_object("synth-1-sync-relative-attack-factor",
				   i18n_pspec("synth-1 sync relative attack factor of recall"),
				   i18n_pspec("The synth-1's sync relative attack factor"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR,
				  param_spec);

  /**
   * AgsFxSeqSynthAudio:synth-1-sync-attack-0:
   *
   * The synth-1 sync attack-0.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-sync-phase-0:
   *
   * The synth-1 sync phase-0.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-sync-attack-1:
   *
   * The synth-1 sync attack-1.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-sync-phase-1:
   *
   * The synth-1 sync phase-1.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-sync-attack-2:
   *
   * The synth-1 sync attack-2.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-sync-phase-2:
   *
   * The synth-1 sync phase-2.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-sync-attack-3:
   *
   * The synth-1 sync attack-3.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-sync-phase-3:
   *
   * The synth-1 sync phase-3.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-sync-lfo-oscillator:
   *
   * The synth-1 sync LFO oscillator.
   * 
   * Since: 7.5.0
   */
  param_spec = g_param_spec_object("synth-1-sync-lfo-oscillator",
				   i18n_pspec("synth-1 sync LFO oscillator of recall"),
				   i18n_pspec("The synth-1's sync LFO oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_LFO_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxSeqSynthAudio:synth-1-sync-lfo-frequency:
   *
   * The synth-1 sync LFO frequency.
   * 
   * Since: 7.5.0
   */
  param_spec = g_param_spec_object("synth-1-sync-lfo-frequency",
				   i18n_pspec("synth-1 sync LFO frequency of recall"),
				   i18n_pspec("The synth-1's sync LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_SYNC_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsFxSeqSynthAudio:synth-1-lfo-oscillator:
   *
   * The synth-1 lfo-oscillator.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-lfo-frequency:
   *
   * The synth-1 LFO frequency.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-lfo-depth:
   *
   * The synth-1 LFO depth.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:synth-1-lfo-tuning:
   *
   * The synth-1 LFO tuning.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:noise-gain:
   *
   * The noise gain.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:pitch-type:
   *
   * The pitch type.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:pitch-tuning:
   *
   * The pitch tuning.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:chorus-enabled:
   *
   * The chorus enabled.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:chorus-pitch-type:
   *
   * The chorus pitch type.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:chorus-input-volume:
   *
   * The chorus input volume.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:chorus-output-volume:
   *
   * The chorus output volume.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:chorus-lfo-oscillator:
   *
   * The chorus lfo oscillator.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:chorus-lfo-frequency:
   *
   * The chorus lfo frequency.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:chorus-depth:
   *
   * The chorus depth.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:chorus-mix:
   *
   * The chorus mix.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:chorus-delay:
   *
   * The chorus delay.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:vibrato-enabled:
   *
   * The vibrato enabled.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:vibrato-gain:
   *
   * The vibrato gain.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:vibrato-lfo-depth:
   *
   * The vibrato LFO depth.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:vibrato-lfo-freq:
   *
   * The vibrato LFO freq.
   * 
   * Since: 7.5.0
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
   * AgsFxSeqSynthAudio:vibrato-tuning:
   *
   * The vibrato tuning.
   * 
   * Since: 7.5.0
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
ags_fx_seq_synth_audio_init(AgsFxSeqSynthAudio *fx_seq_synth_audio)
{
  gint position;
  guint i;
  
  g_signal_connect(fx_seq_synth_audio, "notify::audio",
		   G_CALLBACK(ags_fx_seq_synth_audio_notify_audio_callback), NULL);

  g_signal_connect(fx_seq_synth_audio, "notify::buffer-size",
		   G_CALLBACK(ags_fx_seq_synth_audio_notify_buffer_size_callback), NULL);

  g_signal_connect(fx_seq_synth_audio, "notify::format",
		   G_CALLBACK(ags_fx_seq_synth_audio_notify_format_callback), NULL);

  g_signal_connect(fx_seq_synth_audio, "notify::samplerate",
		   G_CALLBACK(ags_fx_seq_synth_audio_notify_samplerate_callback), NULL);

  AGS_RECALL(fx_seq_synth_audio)->name = "ags-fx-seq-synth";
  AGS_RECALL(fx_seq_synth_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_seq_synth_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_seq_synth_audio)->xml_type = "ags-fx-seq-synth-audio";

  position = 0;

  /* synth-0 oscillator */
  fx_seq_synth_audio->synth_0_oscillator = g_object_new(AGS_TYPE_PORT,
							"plugin-name", ags_fx_seq_synth_audio_plugin_name,
							"specifier", "./synth-0-oscillator[0]",
							"control-port", "1/99",
							"port-value-is-pointer", FALSE,
							"port-value-type", G_TYPE_FLOAT,
							"port-value-size", sizeof(gfloat),
							"port-value-length", 1,
							NULL);
  
  fx_seq_synth_audio->synth_0_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_seq_synth_audio->synth_0_oscillator,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_oscillator);

  position++;

  /* synth-0 octave */
  fx_seq_synth_audio->synth_0_octave = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-octave[0]",
						    "control-port", "2/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_octave->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_octave,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_octave);

  position++;

  /* synth-0 key */
  fx_seq_synth_audio->synth_0_key = g_object_new(AGS_TYPE_PORT,
						 "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						 "specifier", "./synth-0-key[0]",
						 "control-port", "3/99",
						 "port-value-is-pointer", FALSE,
						 "port-value-type", G_TYPE_FLOAT,
						 "port-value-size", sizeof(gfloat),
						 "port-value-length", 1,
						 NULL);
  
  fx_seq_synth_audio->synth_0_key->port_value.ags_port_float = (gfloat) 2.0;

  g_object_set(fx_seq_synth_audio->synth_0_key,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_key);

  position++;

  /* synth-0 phase */
  fx_seq_synth_audio->synth_0_phase = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						   "specifier", "./synth-0-phase[0]",
						   "control-port", "4/99",
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_seq_synth_audio->synth_0_phase->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_phase,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_phase);

  position++;

  /* synth-0 volume */
  fx_seq_synth_audio->synth_0_volume = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-volume[0]",
						    "control-port", "5/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_volume->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_seq_synth_audio->synth_0_volume,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_volume);

  position++;

  /* synth-0 seq tuning 0 */
  fx_seq_synth_audio->synth_0_seq_tuning_0 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-tuning-0[0]",
						    "control-port", "5/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_tuning_0->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_tuning_0,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_tuning_0);

  position++;

  /* synth-0 seq tuning 1 */
  fx_seq_synth_audio->synth_0_seq_tuning_1 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-tuning-1[0]",
						    "control-port", "6/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_tuning_1->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_tuning_1,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_tuning_1);

  position++;

  /* synth-0 seq tuning 2 */
  fx_seq_synth_audio->synth_0_seq_tuning_2 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-tuning-2[0]",
						    "control-port", "7/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_tuning_2->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_tuning_2,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_tuning_2);

  position++;

  /* synth-0 seq tuning 3 */
  fx_seq_synth_audio->synth_0_seq_tuning_3 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-tuning-3[0]",
						    "control-port", "8/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_tuning_3->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_tuning_3,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_tuning_3);

  position++;

  /* synth-0 seq tuning 4 */
  fx_seq_synth_audio->synth_0_seq_tuning_4 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-tuning-4[0]",
						    "control-port", "9/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_tuning_4->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_tuning_4,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_tuning_4);

  position++;

  /* synth-0 seq tuning 5 */
  fx_seq_synth_audio->synth_0_seq_tuning_5 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-tuning-5[0]",
						    "control-port", "10/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_tuning_5->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_tuning_5,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_tuning_5);

  position++;

  /* synth-0 seq tuning 6 */
  fx_seq_synth_audio->synth_0_seq_tuning_6 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-tuning-6[0]",
						    "control-port", "11/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_tuning_6->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_tuning_6,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_tuning_6);

  position++;

  /* synth-0 seq tuning 7 */
  fx_seq_synth_audio->synth_0_seq_tuning_7 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-tuning-7[0]",
						    "control-port", "12/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_tuning_7->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_tuning_7,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_tuning_7);

  position++;

  /* synth-0 seq tuning pingpong */
  fx_seq_synth_audio->synth_0_seq_tuning_pingpong = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-tuning-pingpong[0]",
						    "control-port", "13/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_tuning_pingpong->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_seq_synth_audio->synth_0_seq_tuning_pingpong,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_tuning_pingpong_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_tuning_pingpong);

  position++;

  /* synth-0 seq tuning LFO frequency */
  fx_seq_synth_audio->synth_0_seq_tuning_lfo_frequency = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-tuning-lfo-frequency[0]",
						    "control-port", "14/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_tuning_lfo_frequency->port_value.ags_port_float = (gfloat) 8.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_tuning_lfo_frequency,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_tuning_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_tuning_lfo_frequency);

  position++;

  /* synth-0 seq volume 0 */
  fx_seq_synth_audio->synth_0_seq_volume_0 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-volume-0[0]",
						    "control-port", "15/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_volume_0->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_volume_0,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_volume_0);

  position++;

  /* synth-0 seq volume 1 */
  fx_seq_synth_audio->synth_0_seq_volume_1 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-volume-1[0]",
						    "control-port", "16/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_volume_1->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_volume_1,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_volume_1);

  position++;

  /* synth-0 seq volume 2 */
  fx_seq_synth_audio->synth_0_seq_volume_2 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-volume-2[0]",
						    "control-port", "17/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_volume_2->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_volume_2,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_volume_2);

  position++;

  /* synth-0 seq volume 3 */
  fx_seq_synth_audio->synth_0_seq_volume_3 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-volume-3[0]",
						    "control-port", "18/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_volume_3->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_volume_3,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_volume_3);

  position++;

  /* synth-0 seq volume 4 */
  fx_seq_synth_audio->synth_0_seq_volume_4 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-volume-4[0]",
						    "control-port", "19/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_volume_4->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_volume_4,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_volume_4);

  position++;

  /* synth-0 seq volume 5 */
  fx_seq_synth_audio->synth_0_seq_volume_5 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-volume-5[0]",
						    "control-port", "20/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_volume_5->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_volume_5,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_volume_5);

  position++;

  /* synth-0 seq volume 6 */
  fx_seq_synth_audio->synth_0_seq_volume_6 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-volume-6[0]",
						    "control-port", "21/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_volume_6->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_volume_6,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_volume_6);

  position++;

  /* synth-0 seq volume 7 */
  fx_seq_synth_audio->synth_0_seq_volume_7 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-volume-7[0]",
						    "control-port", "22/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_volume_7->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_volume_7,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_volume_7);

  position++;

  /* synth-0 seq volume pingpong */
  fx_seq_synth_audio->synth_0_seq_volume_pingpong = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-volume-pingpong[0]",
						    "control-port", "23/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_volume_pingpong->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_seq_synth_audio->synth_0_seq_volume_pingpong,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_volume_pingpong_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_volume_pingpong);

  position++;

  /* synth-0 seq volume LFO frequency */
  fx_seq_synth_audio->synth_0_seq_volume_lfo_frequency = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-0-seq-volume-lfo-frequency[0]",
						    "control-port", "24/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_0_seq_volume_lfo_frequency->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_seq_volume_lfo_frequency,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_seq_volume_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_seq_volume_lfo_frequency);

  position++;
  
  /* synth-0 sync enabled */
  fx_seq_synth_audio->synth_0_sync_enabled = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							  "specifier", "./synth-0-sync-enabled[0]",
							  "control-port", "25/99",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_seq_synth_audio->synth_0_sync_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_seq_synth_audio->synth_0_sync_enabled,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_sync_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_sync_enabled);

  position++;

  /* synth-0 sync relative attack factor */
  fx_seq_synth_audio->synth_0_sync_relative_attack_factor = g_object_new(AGS_TYPE_PORT,
									 "plugin-name", ags_fx_seq_synth_audio_plugin_name,
									 "specifier", "./synth-0-sync-relative-factor[0]",
									 "control-port", "26/99",
									 "port-value-is-pointer", FALSE,
									 "port-value-type", G_TYPE_FLOAT,
									 "port-value-size", sizeof(gfloat),
									 "port-value-length", 1,
									 NULL);
  
  fx_seq_synth_audio->synth_0_sync_relative_attack_factor->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_seq_synth_audio->synth_0_sync_relative_attack_factor,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_sync_relative_attack_factor_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_sync_relative_attack_factor);

  position++;

  /* synth-0 sync attack-0 */
  fx_seq_synth_audio->synth_0_sync_attack_0 = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							   "specifier", "./synth-0-sync-attack-0[0]",
							   "control-port", "27/99",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_seq_synth_audio->synth_0_sync_attack_0->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_sync_attack_0,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_sync_attack_0_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_sync_attack_0);

  position++;

  /* synth-0 sync phase-0 */
  fx_seq_synth_audio->synth_0_sync_phase_0 = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							  "specifier", "./synth-0-sync-phase-0[0]",
							  "control-port", "28/99",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_seq_synth_audio->synth_0_sync_phase_0->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_sync_phase_0,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_sync_phase_0_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_sync_phase_0);

  position++;

  /* synth-0 sync attack-1 */
  fx_seq_synth_audio->synth_0_sync_attack_1 = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							   "specifier", "./synth-0-sync-attack-1[0]",
							   "control-port", "29/99",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_seq_synth_audio->synth_0_sync_attack_1->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_sync_attack_1,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_sync_attack_1_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_sync_attack_1);

  position++;

  /* synth-0 sync phase-1 */
  fx_seq_synth_audio->synth_0_sync_phase_1 = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							  "specifier", "./synth-0-sync-phase-1[0]",
							  "control-port", "30/99",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_seq_synth_audio->synth_0_sync_phase_1->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_sync_phase_1,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_sync_phase_1_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_sync_phase_1);

  position++;

  /* synth-0 sync attack-2 */
  fx_seq_synth_audio->synth_0_sync_attack_2 = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							   "specifier", "./synth-0-sync-attack-2[0]",
							   "control-port", "31/99",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_seq_synth_audio->synth_0_sync_attack_2->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_sync_attack_2,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_sync_attack_2_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_sync_attack_2);

  position++;

  /* synth-0 sync phase-2 */
  fx_seq_synth_audio->synth_0_sync_phase_2 = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							  "specifier", "./synth-0-sync-phase-2[0]",
							  "control-port", "32/99",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_seq_synth_audio->synth_0_sync_phase_2->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_sync_phase_2,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_sync_phase_2_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_sync_phase_2);

  position++;

  /* synth-0 sync attack-3 */
  fx_seq_synth_audio->synth_0_sync_attack_3 = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							   "specifier", "./synth-0-sync-attack-3[0]",
							   "control-port", "33/99",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_seq_synth_audio->synth_0_sync_attack_3->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_sync_attack_3,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_sync_attack_3_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_sync_attack_3);

  position++;

  /* synth-0 sync phase-3 */
  fx_seq_synth_audio->synth_0_sync_phase_3 = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							  "specifier", "./synth-0-sync-phase-3[0]",
							  "control-port", "34/99",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_seq_synth_audio->synth_0_sync_phase_3->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_0_sync_phase_3,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_sync_phase_3_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_sync_phase_3);

  position++;

  /* synth-0 sync LFO oscillator */
  fx_seq_synth_audio->synth_0_sync_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
								 "plugin-name", ags_fx_seq_synth_audio_plugin_name,
								 "specifier", "./synth-0-sync-lfo-oscillator[0]",
								 "control-port", "35/99",
								 "port-value-is-pointer", FALSE,
								 "port-value-type", G_TYPE_FLOAT,
								 "port-value-size", sizeof(gfloat),
								 "port-value-length", 1,
								 NULL);
  
  fx_seq_synth_audio->synth_0_sync_lfo_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_seq_synth_audio->synth_0_sync_lfo_oscillator,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_sync_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_sync_lfo_oscillator);

  position++;

  /* synth-0 sync LFO frequency */
  fx_seq_synth_audio->synth_0_sync_lfo_frequency = g_object_new(AGS_TYPE_PORT,
								"plugin-name", ags_fx_seq_synth_audio_plugin_name,
								"specifier", "./synth-0-sync-lfo-frequency[0]",
								"control-port", "36/99",
								"port-value-is-pointer", FALSE,
								"port-value-type", G_TYPE_FLOAT,
								"port-value-size", sizeof(gfloat),
								"port-value-length", 1,
								NULL);
  
  fx_seq_synth_audio->synth_0_sync_lfo_frequency->port_value.ags_port_float = (gfloat) 10.0;

  g_object_set(fx_seq_synth_audio->synth_0_sync_lfo_frequency,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_sync_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_sync_lfo_frequency);

  position++;

  /* synth-0 LFO oscillator */
  fx_seq_synth_audio->synth_0_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							    "specifier", "./synth-0-lfo-oscillator[0]",
							    "control-port", "37/99",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_seq_synth_audio->synth_0_lfo_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_seq_synth_audio->synth_0_lfo_oscillator,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_lfo_oscillator);

  position++;

  /* synth-0 LFO frequency */
  fx_seq_synth_audio->synth_0_lfo_frequency = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							   "specifier", "./synth-0-lfo-frequency[0]",
							   "control-port", "38/99",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_seq_synth_audio->synth_0_lfo_frequency->port_value.ags_port_float = (gfloat) AGS_SEQ_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY;

  g_object_set(fx_seq_synth_audio->synth_0_lfo_frequency,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_lfo_frequency);

  position++;

  /* synth-0 LFO depth */
  fx_seq_synth_audio->synth_0_lfo_depth = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						       "specifier", "./synth-0-lfo-depth[0]",
						       "control-port", "39/99",
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_seq_synth_audio->synth_0_lfo_depth->port_value.ags_port_float = (gfloat) AGS_SEQ_SYNTH_UTIL_DEFAULT_LFO_DEPTH;

  g_object_set(fx_seq_synth_audio->synth_0_lfo_depth,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_lfo_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_lfo_depth);

  position++;

  /* synth-0 LFO tuning */
  fx_seq_synth_audio->synth_0_lfo_tuning = g_object_new(AGS_TYPE_PORT,
							"plugin-name", ags_fx_seq_synth_audio_plugin_name,
							"specifier", "./synth-0-lfo-tuning[0]",
							"control-port", "40/99",
							"port-value-is-pointer", FALSE,
							"port-value-type", G_TYPE_FLOAT,
							"port-value-size", sizeof(gfloat),
							"port-value-length", 1,
							NULL);
  
  fx_seq_synth_audio->synth_0_lfo_tuning->port_value.ags_port_float = (gfloat) AGS_SEQ_SYNTH_UTIL_DEFAULT_TUNING;

  g_object_set(fx_seq_synth_audio->synth_0_lfo_tuning,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_0_lfo_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_0_lfo_tuning);

  position++;

  /* synth-1 oscillator */
  fx_seq_synth_audio->synth_1_oscillator = g_object_new(AGS_TYPE_PORT,
							"plugin-name", ags_fx_seq_synth_audio_plugin_name,
							"specifier", "./synth-1-oscillator[0]",
							"control-port", "41/99",
							"port-value-is-pointer", FALSE,
							"port-value-type", G_TYPE_FLOAT,
							"port-value-size", sizeof(gfloat),
							"port-value-length", 1,
							NULL);
  
  fx_seq_synth_audio->synth_1_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_seq_synth_audio->synth_1_oscillator,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_oscillator);

  position++;
  
  /* synth-1 octave */
  fx_seq_synth_audio->synth_1_octave = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-octave[0]",
						    "control-port", "42/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_octave->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_octave,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_octave);

  position++;

  /* synth-1 key */
  fx_seq_synth_audio->synth_1_key = g_object_new(AGS_TYPE_PORT,
						 "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						 "specifier", "./synth-1-key[0]",
						 "control-port", "43/99",
						 "port-value-is-pointer", FALSE,
						 "port-value-type", G_TYPE_FLOAT,
						 "port-value-size", sizeof(gfloat),
						 "port-value-length", 1,
						 NULL);
  
  fx_seq_synth_audio->synth_1_key->port_value.ags_port_float = (gfloat) 2.0;

  g_object_set(fx_seq_synth_audio->synth_1_key,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_key);

  position++;

  /* synth-1 phase */
  fx_seq_synth_audio->synth_1_phase = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						   "specifier", "./synth-1-phase[0]",
						   "control-port", "44/99",
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_seq_synth_audio->synth_1_phase->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_phase,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_phase);

  position++;

  /* synth-1 volume */
  fx_seq_synth_audio->synth_1_volume = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-volume[0]",
						    "control-port", "45/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_volume->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_seq_synth_audio->synth_1_volume,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_volume);

  position++;


  /* synth-1 seq tuning 0 */
  fx_seq_synth_audio->synth_1_seq_tuning_0 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-tuning-0[0]",
						    "control-port", "46/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_tuning_0->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_tuning_0,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_tuning_0);

  position++;

  /* synth-1 seq tuning 1 */
  fx_seq_synth_audio->synth_1_seq_tuning_1 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-tuning-1[0]",
						    "control-port", "47/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_tuning_1->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_tuning_1,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_tuning_1);

  position++;

  /* synth-1 seq tuning 2 */
  fx_seq_synth_audio->synth_1_seq_tuning_2 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-tuning-2[0]",
						    "control-port", "48/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_tuning_2->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_tuning_2,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_tuning_2);

  position++;

  /* synth-1 seq tuning 3 */
  fx_seq_synth_audio->synth_1_seq_tuning_3 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-tuning-3[0]",
						    "control-port", "49/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_tuning_3->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_tuning_3,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_tuning_3);

  position++;

  /* synth-1 seq tuning 4 */
  fx_seq_synth_audio->synth_1_seq_tuning_4 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-tuning-4[0]",
						    "control-port", "50/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_tuning_4->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_tuning_4,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_tuning_4);

  position++;

  /* synth-1 seq tuning 5 */
  fx_seq_synth_audio->synth_1_seq_tuning_5 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-tuning-5[0]",
						    "control-port", "51/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_tuning_5->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_tuning_5,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_tuning_5);

  position++;

  /* synth-1 seq tuning 6 */
  fx_seq_synth_audio->synth_1_seq_tuning_6 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-tuning-6[0]",
						    "control-port", "52/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_tuning_6->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_tuning_6,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_tuning_6);

  position++;

  /* synth-1 seq tuning 7 */
  fx_seq_synth_audio->synth_1_seq_tuning_7 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-tuning-7[0]",
						    "control-port", "53/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_tuning_7->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_tuning_7,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_tuning_7);

  position++;

  /* synth-1 seq tuning pingpong */
  fx_seq_synth_audio->synth_1_seq_tuning_pingpong = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-tuning-pingpong[0]",
						    "control-port", "54/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_tuning_pingpong->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_seq_synth_audio->synth_1_seq_tuning_pingpong,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_tuning_pingpong_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_tuning_pingpong);

  position++;

  /* synth-1 seq tuning LFO frequency */
  fx_seq_synth_audio->synth_1_seq_tuning_lfo_frequency = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-tuning-lfo-frequency[0]",
						    "control-port", "55/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_tuning_lfo_frequency->port_value.ags_port_float = (gfloat) 8.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_tuning_lfo_frequency,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_tuning_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_tuning_lfo_frequency);

  position++;

  /* synth-1 seq volume 0 */
  fx_seq_synth_audio->synth_1_seq_volume_0 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-volume-0[0]",
						    "control-port", "56/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_volume_0->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_volume_0,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_volume_0);

  position++;

  /* synth-1 seq volume 1 */
  fx_seq_synth_audio->synth_1_seq_volume_1 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-volume-1[0]",
						    "control-port", "57/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_volume_1->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_volume_1,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_volume_1);

  position++;

  /* synth-1 seq volume 2 */
  fx_seq_synth_audio->synth_1_seq_volume_2 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-volume-2[0]",
						    "control-port", "58/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_volume_2->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_volume_2,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_volume_2);

  position++;

  /* synth-1 seq volume 3 */
  fx_seq_synth_audio->synth_1_seq_volume_3 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-volume-3[0]",
						    "control-port", "59/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_volume_3->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_volume_3,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_volume_3);

  position++;

  /* synth-1 seq volume 4 */
  fx_seq_synth_audio->synth_1_seq_volume_4 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-volume-4[0]",
						    "control-port", "60/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_volume_4->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_volume_4,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_volume_4);

  position++;

  /* synth-1 seq volume 5 */
  fx_seq_synth_audio->synth_1_seq_volume_5 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-volume-5[0]",
						    "control-port", "61/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_volume_5->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_volume_5,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_volume_5);

  position++;

  /* synth-1 seq volume 6 */
  fx_seq_synth_audio->synth_1_seq_volume_6 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-volume-6[0]",
						    "control-port", "62/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_volume_6->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_volume_6,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_volume_6);

  position++;

  /* synth-1 seq volume 7 */
  fx_seq_synth_audio->synth_1_seq_volume_7 = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-volume-7[0]",
						    "control-port", "63/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_volume_7->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_volume_7,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_volume_7);

  position++;

  /* synth-1 seq volume pingpong */
  fx_seq_synth_audio->synth_1_seq_volume_pingpong = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-volume-pingpong[0]",
						    "control-port", "64/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_volume_pingpong->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_seq_synth_audio->synth_1_seq_volume_pingpong,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_volume_pingpong_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_volume_pingpong);

  position++;

  /* synth-1 seq volume LFO frequency */
  fx_seq_synth_audio->synth_1_seq_volume_lfo_frequency = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./synth-1-seq-volume-lfo-frequency[0]",
						    "control-port", "65/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->synth_1_seq_volume_lfo_frequency->port_value.ags_port_float = (gfloat) 8.0;

  g_object_set(fx_seq_synth_audio->synth_1_seq_volume_lfo_frequency,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_seq_volume_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_seq_volume_lfo_frequency);

  position++;

  /* synth-1 sync enabled */
  fx_seq_synth_audio->synth_1_sync_enabled = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							  "specifier", "./synth-1-sync-enabled[0]",
							  "control-port", "66/99",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_seq_synth_audio->synth_1_sync_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_seq_synth_audio->synth_1_sync_enabled,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_sync_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_sync_enabled);

  position++;

  /* synth-1 sync relative attack factor */
  fx_seq_synth_audio->synth_1_sync_relative_attack_factor = g_object_new(AGS_TYPE_PORT,
									 "plugin-name", ags_fx_seq_synth_audio_plugin_name,
									 "specifier", "./synth-1-sync-relative-factor[0]",
									 "control-port", "67/99",
									 "port-value-is-pointer", FALSE,
									 "port-value-type", G_TYPE_FLOAT,
									 "port-value-size", sizeof(gfloat),
									 "port-value-length", 1,
									 NULL);
  
  fx_seq_synth_audio->synth_1_sync_relative_attack_factor->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_seq_synth_audio->synth_1_sync_relative_attack_factor,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_sync_relative_attack_factor_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_sync_relative_attack_factor);

  position++;

  /* synth-1 sync attack-0 */
  fx_seq_synth_audio->synth_1_sync_attack_0 = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							   "specifier", "./synth-1-sync-attack-0[0]",
							   "control-port", "68/99",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_seq_synth_audio->synth_1_sync_attack_0->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_sync_attack_0,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_sync_attack_0_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_sync_attack_0);

  position++;

  /* synth-1 sync phase-0 */
  fx_seq_synth_audio->synth_1_sync_phase_0 = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							  "specifier", "./synth-1-sync-phase-0[0]",
							  "control-port", "69/99",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_seq_synth_audio->synth_1_sync_phase_0->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_sync_phase_0,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_sync_phase_0_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_sync_phase_0);

  position++;

  /* synth-1 sync attack-1 */
  fx_seq_synth_audio->synth_1_sync_attack_1 = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							   "specifier", "./synth-1-sync-attack-1[0]",
							   "control-port", "70/99",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_seq_synth_audio->synth_1_sync_attack_1->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_sync_attack_1,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_sync_attack_1_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_sync_attack_1);

  position++;

  /* synth-1 sync phase-1 */
  fx_seq_synth_audio->synth_1_sync_phase_1 = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							  "specifier", "./synth-1-sync-phase-1[0]",
							  "control-port", "71/99",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_seq_synth_audio->synth_1_sync_phase_1->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_sync_phase_1,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_sync_phase_1_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_sync_phase_1);

  position++;

  /* synth-1 sync attack-2 */
  fx_seq_synth_audio->synth_1_sync_attack_2 = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							   "specifier", "./synth-1-sync-attack-2[0]",
							   "control-port", "72/99",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_seq_synth_audio->synth_1_sync_attack_2->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_sync_attack_2,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_sync_attack_2_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_sync_attack_2);

  position++;

  /* synth-1 sync phase-2 */
  fx_seq_synth_audio->synth_1_sync_phase_2 = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							  "specifier", "./synth-1-sync-phase-2[0]",
							  "control-port", "73/99",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_seq_synth_audio->synth_1_sync_phase_2->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_sync_phase_2,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_sync_phase_2_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_sync_phase_2);

  position++;

  /* synth-1 sync attack-3 */
  fx_seq_synth_audio->synth_1_sync_attack_3 = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							   "specifier", "./synth-1-sync-attack-3[0]",
							   "control-port", "74/99",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_seq_synth_audio->synth_1_sync_attack_3->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_sync_attack_3,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_sync_attack_3_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_sync_attack_3);

  position++;

  /* synth-1 sync phase-3 */
  fx_seq_synth_audio->synth_1_sync_phase_3 = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							  "specifier", "./synth-1-sync-phase-3[0]",
							  "control-port", "75/99",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_seq_synth_audio->synth_1_sync_phase_3->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->synth_1_sync_phase_3,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_sync_phase_3_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_sync_phase_3);

  position++;

  /* synth-1 sync LFO oscillator */
  fx_seq_synth_audio->synth_1_sync_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
								 "plugin-name", ags_fx_seq_synth_audio_plugin_name,
								 "specifier", "./synth-1-sync-lfo-oscillator[0]",
								 "control-port", "76/99",
								 "port-value-is-pointer", FALSE,
								 "port-value-type", G_TYPE_FLOAT,
								 "port-value-size", sizeof(gfloat),
								 "port-value-length", 1,
								 NULL);
  
  fx_seq_synth_audio->synth_1_sync_lfo_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_seq_synth_audio->synth_1_sync_lfo_oscillator,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_sync_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_sync_lfo_oscillator);

  position++;

  /* synth-1 sync LFO frequency */
  fx_seq_synth_audio->synth_1_sync_lfo_frequency = g_object_new(AGS_TYPE_PORT,
								"plugin-name", ags_fx_seq_synth_audio_plugin_name,
								"specifier", "./synth-1-sync-lfo-frequency[0]",
								"control-port", "77/99",
								"port-value-is-pointer", FALSE,
								"port-value-type", G_TYPE_FLOAT,
								"port-value-size", sizeof(gfloat),
								"port-value-length", 1,
								NULL);
  
  fx_seq_synth_audio->synth_1_sync_lfo_frequency->port_value.ags_port_float = (gfloat) 10.0;

  g_object_set(fx_seq_synth_audio->synth_1_sync_lfo_frequency,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_sync_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_sync_lfo_frequency);

  position++;

  /* synth-1 LFO oscillator */
  fx_seq_synth_audio->synth_1_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							    "specifier", "./synth-1-lfo-oscillator[0]",
							    "control-port", "78/99",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_seq_synth_audio->synth_1_lfo_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_seq_synth_audio->synth_1_lfo_oscillator,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_lfo_oscillator);

  position++;

  /* synth-1 LFO frequency */
  fx_seq_synth_audio->synth_1_lfo_frequency = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							   "specifier", "./synth-1-lfo-frequency[0]",
							   "control-port", "79/99",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_seq_synth_audio->synth_1_lfo_frequency->port_value.ags_port_float = (gfloat) AGS_SEQ_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY;

  g_object_set(fx_seq_synth_audio->synth_1_lfo_frequency,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_lfo_frequency);

  position++;

  /* synth-1 LFO depth */
  fx_seq_synth_audio->synth_1_lfo_depth = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						       "specifier", "./synth-1-lfo-depth[0]",
						       "control-port", "80/99",
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_seq_synth_audio->synth_1_lfo_depth->port_value.ags_port_float = (gfloat) AGS_SEQ_SYNTH_UTIL_DEFAULT_LFO_DEPTH;

  g_object_set(fx_seq_synth_audio->synth_1_lfo_depth,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_lfo_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_lfo_depth);

  position++;

  /* synth-1 LFO tuning */
  fx_seq_synth_audio->synth_1_lfo_tuning = g_object_new(AGS_TYPE_PORT,
							"plugin-name", ags_fx_seq_synth_audio_plugin_name,
							"specifier", "./synth-1-lfo-tuning[0]",
							"control-port", "81/99",
							"port-value-is-pointer", FALSE,
							"port-value-type", G_TYPE_FLOAT,
							"port-value-size", sizeof(gfloat),
							"port-value-length", 1,
							NULL);
  
  fx_seq_synth_audio->synth_1_lfo_tuning->port_value.ags_port_float = (gfloat) AGS_SEQ_SYNTH_UTIL_DEFAULT_TUNING;

  g_object_set(fx_seq_synth_audio->synth_1_lfo_tuning,
	       "plugin-port", ags_fx_seq_synth_audio_get_synth_1_lfo_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->synth_1_lfo_tuning);

  position++;

  /* noise gain */
  fx_seq_synth_audio->noise_gain = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_seq_synth_audio_plugin_name,
						"specifier", "./noise-gain[0]",
						"control-port", "82/99",
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 1,
						NULL);
  
  fx_seq_synth_audio->noise_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->noise_gain,
	       "plugin-port", ags_fx_seq_synth_audio_get_noise_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->noise_gain);

  position++;

  /* pitch type */
  fx_seq_synth_audio->pitch_type = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_seq_synth_audio_plugin_name,
						"specifier", "./pitch-type[0]",
						"control-port", "83/99",
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 1,
						NULL);
  
  fx_seq_synth_audio->pitch_type->port_value.ags_port_float = (gfloat) AGS_PITCH_TYPE_FLUID_INTERPOLATE_4TH_ORDER;

  g_object_set(fx_seq_synth_audio->pitch_type,
	       "plugin-port", ags_fx_seq_synth_audio_get_pitch_type_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->pitch_type);

  position++;

  g_signal_connect_after(fx_seq_synth_audio->pitch_type, "safe-write",
			 G_CALLBACK(ags_fx_seq_synth_audio_pitch_type_callback), fx_seq_synth_audio);

  /* pitch tuning */
  fx_seq_synth_audio->pitch_tuning = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						  "specifier", "./pitch-tuning[0]",
						  "control-port", "84/99",
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 1,
						  NULL);
  
  fx_seq_synth_audio->pitch_tuning->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->pitch_tuning,
	       "plugin-port", ags_fx_seq_synth_audio_get_pitch_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->pitch_tuning);

  position++;

  /* chorus enabled */
  fx_seq_synth_audio->chorus_enabled = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./chorus-enabled[0]",
						    "control-port", "85/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->chorus_enabled->port_value.ags_port_float = (gfloat) TRUE;

  g_object_set(fx_seq_synth_audio->chorus_enabled,
	       "plugin-port", ags_fx_seq_synth_audio_get_chorus_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->chorus_enabled);

  position++;

  /* chorus pitch type */
  fx_seq_synth_audio->chorus_pitch_type = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						       "specifier", "./chorus-pitch-type[0]",
						       "control-port", "86/99",
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_seq_synth_audio->chorus_pitch_type->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->chorus_pitch_type,
	       "plugin-port", ags_fx_seq_synth_audio_get_chorus_pitch_type_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->chorus_pitch_type);

  position++;

  /* chorus input volume */
  fx_seq_synth_audio->chorus_input_volume = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							 "specifier", "./chorus-input-volume[0]",
							 "control-port", "87/99",
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_seq_synth_audio->chorus_input_volume->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_seq_synth_audio->chorus_input_volume,
	       "plugin-port", ags_fx_seq_synth_audio_get_chorus_input_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->chorus_input_volume);

  position++;

  /* chorus output volume */
  fx_seq_synth_audio->chorus_output_volume = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							  "specifier", "./chorus-output-volume[0]",
							  "control-port", "88/99",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_seq_synth_audio->chorus_output_volume->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_seq_synth_audio->chorus_output_volume,
	       "plugin-port", ags_fx_seq_synth_audio_get_chorus_output_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->chorus_output_volume);

  position++;

  /* chorus LFO oscillator */
  fx_seq_synth_audio->chorus_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							   "specifier", "./chorus-lfo-oscillator[0]",
							   "control-port", "89/99",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_seq_synth_audio->chorus_lfo_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_seq_synth_audio->chorus_lfo_oscillator,
	       "plugin-port", ags_fx_seq_synth_audio_get_chorus_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->chorus_lfo_oscillator);

  position++;

  /* chorus LFO frequency */
  fx_seq_synth_audio->chorus_lfo_frequency = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
							  "specifier", "./chorus-lfo-frequency[0]",
							  "control-port", "90/99",
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_seq_synth_audio->chorus_lfo_frequency->port_value.ags_port_float = (gfloat) 0.01;

  g_object_set(fx_seq_synth_audio->chorus_lfo_frequency,
	       "plugin-port", ags_fx_seq_synth_audio_get_chorus_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->chorus_lfo_frequency);

  position++;

  /* chorus depth */
  fx_seq_synth_audio->chorus_depth = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						  "specifier", "./chorus-depth[0]",
						  "control-port", "91/99",
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 1,
						  NULL);
  
  fx_seq_synth_audio->chorus_depth->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->chorus_depth,
	       "plugin-port", ags_fx_seq_synth_audio_get_chorus_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->chorus_depth);

  position++;

  /* chorus mix */
  fx_seq_synth_audio->chorus_mix = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_seq_synth_audio_plugin_name,
						"specifier", "./chorus-mix[0]",
						"control-port", "92/99",
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 1,
						NULL);
  
  fx_seq_synth_audio->chorus_mix->port_value.ags_port_float = (gfloat) 0.5;

  g_object_set(fx_seq_synth_audio->chorus_mix,
	       "plugin-port", ags_fx_seq_synth_audio_get_chorus_mix_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->chorus_mix);

  position++;

  /* chorus delay */
  fx_seq_synth_audio->chorus_delay = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						  "specifier", "./chorus-delay[0]",
						  "control-port", "93/99",
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 1,
						  NULL);
  
  fx_seq_synth_audio->chorus_delay->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->chorus_delay,
	       "plugin-port", ags_fx_seq_synth_audio_get_chorus_delay_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->chorus_delay);

  position++;

  /* vibrato enabled */
  fx_seq_synth_audio->vibrato_enabled = g_object_new(AGS_TYPE_PORT,
						     "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						     "specifier", "./vibrato-enabled[0]",
						     "control-port", "94/99",
						     "port-value-is-pointer", FALSE,
						     "port-value-type", G_TYPE_FLOAT,
						     "port-value-size", sizeof(gfloat),
						     "port-value-length", 1,
						     NULL);
  
  fx_seq_synth_audio->vibrato_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_seq_synth_audio->vibrato_enabled,
	       "plugin-port", ags_fx_seq_synth_audio_get_vibrato_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->vibrato_enabled);

  position++;

  /* vibrato gain */
  fx_seq_synth_audio->vibrato_gain = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						  "specifier", "./vibrato-gain[0]",
						  "control-port", "95/99",
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 1,
						  NULL);
  
  fx_seq_synth_audio->vibrato_gain->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_seq_synth_audio->vibrato_gain,
	       "plugin-port", ags_fx_seq_synth_audio_get_vibrato_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->vibrato_gain);

  position++;
  
  /* vibrato LFO depth */
  fx_seq_synth_audio->vibrato_lfo_depth = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						       "specifier", "./vibrato-lfo-depth[0]",
						       "control-port", "96/99",
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_seq_synth_audio->vibrato_lfo_depth->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_seq_synth_audio->vibrato_lfo_depth,
	       "plugin-port", ags_fx_seq_synth_audio_get_vibrato_lfo_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->vibrato_lfo_depth);

  position++;
  
  /* vibrato LFO freq */
  fx_seq_synth_audio->vibrato_lfo_freq = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						      "specifier", "./vibrato-lfo-freq[0]",
						      "control-port", "98/99",
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_seq_synth_audio->vibrato_lfo_freq->port_value.ags_port_float = (gfloat) 8.172;

  g_object_set(fx_seq_synth_audio->vibrato_lfo_freq,
	       "plugin-port", ags_fx_seq_synth_audio_get_vibrato_lfo_freq_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->vibrato_lfo_freq);

  position++;

  /* vibrato tuning */
  fx_seq_synth_audio->vibrato_tuning = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_seq_synth_audio_plugin_name,
						    "specifier", "./vibrato-tuning[0]",
						    "control-port", "99/99",
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_seq_synth_audio->vibrato_tuning->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_seq_synth_audio->vibrato_tuning,
	       "plugin-port", ags_fx_seq_synth_audio_get_vibrato_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_seq_synth_audio,
		      fx_seq_synth_audio->vibrato_tuning);

  position++;
  
  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      fx_seq_synth_audio->scope_data[i] = ags_fx_seq_synth_audio_scope_data_alloc();
      
      fx_seq_synth_audio->scope_data[i]->parent = fx_seq_synth_audio;
    }else{
      fx_seq_synth_audio->scope_data[i] = NULL;
    }
  }
}

void
ags_fx_seq_synth_audio_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsFxSeqSynthAudio *fx_seq_synth_audio;

  GRecMutex *recall_mutex;

  fx_seq_synth_audio = AGS_FX_SEQ_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_seq_synth_audio);

  switch(prop_id){
  case PROP_SYNTH_0_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OCTAVE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_octave){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_octave != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_octave));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_octave = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_KEY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_key){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_key != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_key));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_key = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PHASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_phase){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_phase != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_phase));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_phase = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_volume != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_tuning_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_tuning_0 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_tuning_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_tuning_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_tuning_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_tuning_1 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_tuning_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_tuning_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_tuning_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_tuning_2 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_tuning_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_tuning_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_tuning_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_tuning_3 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_tuning_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_tuning_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_4:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_tuning_4){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_tuning_4 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_tuning_4));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_tuning_4 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_5:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_tuning_5){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_tuning_5 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_tuning_5));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_tuning_5 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_6:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_tuning_6){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_tuning_6 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_tuning_6));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_tuning_6 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_7:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_tuning_7){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_tuning_7 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_tuning_7));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_tuning_7 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_PINGPONG:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_tuning_pingpong){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_tuning_pingpong != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_tuning_pingpong));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_tuning_pingpong = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_tuning_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_tuning_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_tuning_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_tuning_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_volume_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_volume_0 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_volume_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_volume_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_volume_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_volume_1 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_volume_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_volume_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_volume_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_volume_2 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_volume_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_volume_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_volume_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_volume_3 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_volume_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_volume_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_4:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_volume_4){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_volume_4 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_volume_4));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_volume_4 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_5:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_volume_5){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_volume_5 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_volume_5));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_volume_5 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_6:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_volume_6){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_volume_6 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_volume_6));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_volume_6 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_7:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_volume_7){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_volume_7 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_volume_7));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_volume_7 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_PINGPONG:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_volume_pingpong){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_volume_pingpong != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_volume_pingpong));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_volume_pingpong = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_seq_volume_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_seq_volume_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_seq_volume_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_seq_volume_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ENABLED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_sync_enabled){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_sync_enabled != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_sync_enabled));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_sync_enabled = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_sync_relative_attack_factor){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_sync_relative_attack_factor != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_sync_relative_attack_factor));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_sync_relative_attack_factor = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_sync_attack_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_sync_attack_0 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_sync_attack_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_sync_attack_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_sync_phase_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_sync_phase_0 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_sync_phase_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_sync_phase_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_sync_attack_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_sync_attack_1 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_sync_attack_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_sync_attack_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_sync_phase_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_sync_phase_1 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_sync_phase_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_sync_phase_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_sync_attack_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_sync_attack_2 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_sync_attack_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_sync_attack_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_sync_phase_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_sync_phase_2 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_sync_phase_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_sync_phase_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_sync_attack_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_sync_attack_3 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_sync_attack_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_sync_attack_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_sync_phase_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_sync_phase_3 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_sync_phase_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_sync_phase_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_LFO_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_sync_lfo_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_sync_lfo_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_sync_lfo_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_sync_lfo_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_sync_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_sync_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_sync_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_sync_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_lfo_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_lfo_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_lfo_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_lfo_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_lfo_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_lfo_depth != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_lfo_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_lfo_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_0_lfo_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_0_lfo_tuning != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_0_lfo_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_0_lfo_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_OCTAVE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_octave){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_octave != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_octave));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_octave = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_KEY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_key){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_key != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_key));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_key = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_PHASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_phase){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_phase != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_phase));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_phase = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_volume != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_tuning_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_tuning_0 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_tuning_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_tuning_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_tuning_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_tuning_1 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_tuning_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_tuning_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_tuning_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_tuning_2 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_tuning_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_tuning_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_tuning_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_tuning_3 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_tuning_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_tuning_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_4:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_tuning_4){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_tuning_4 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_tuning_4));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_tuning_4 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_5:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_tuning_5){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_tuning_5 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_tuning_5));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_tuning_5 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_6:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_tuning_6){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_tuning_6 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_tuning_6));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_tuning_6 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_7:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_tuning_7){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_tuning_7 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_tuning_7));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_tuning_7 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_PINGPONG:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_tuning_pingpong){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_tuning_pingpong != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_tuning_pingpong));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_tuning_pingpong = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_tuning_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_tuning_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_tuning_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_tuning_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_volume_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_volume_0 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_volume_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_volume_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_volume_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_volume_1 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_volume_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_volume_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_volume_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_volume_2 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_volume_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_volume_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_volume_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_volume_3 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_volume_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_volume_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_4:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_volume_4){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_volume_4 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_volume_4));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_volume_4 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_5:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_volume_5){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_volume_5 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_volume_5));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_volume_5 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_6:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_volume_6){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_volume_6 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_volume_6));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_volume_6 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_7:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_volume_7){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_volume_7 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_volume_7));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_volume_7 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_PINGPONG:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_volume_pingpong){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_volume_pingpong != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_volume_pingpong));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_volume_pingpong = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_seq_volume_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_seq_volume_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_seq_volume_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_seq_volume_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ENABLED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_sync_enabled){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_sync_enabled != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_sync_enabled));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_sync_enabled = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_sync_relative_attack_factor){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_sync_relative_attack_factor != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_sync_relative_attack_factor));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_sync_relative_attack_factor = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_sync_attack_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_sync_attack_0 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_sync_attack_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_sync_attack_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_0:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_sync_phase_0){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_sync_phase_0 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_sync_phase_0));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_sync_phase_0 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_sync_attack_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_sync_attack_1 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_sync_attack_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_sync_attack_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_1:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_sync_phase_1){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_sync_phase_1 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_sync_phase_1));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_sync_phase_1 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_sync_attack_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_sync_attack_2 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_sync_attack_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_sync_attack_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_2:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_sync_phase_2){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_sync_phase_2 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_sync_phase_2));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_sync_phase_2 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_sync_attack_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_sync_attack_3 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_sync_attack_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_sync_attack_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_3:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_sync_phase_3){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_sync_phase_3 != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_sync_phase_3));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_sync_phase_3 = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_LFO_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_sync_lfo_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_sync_lfo_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_sync_lfo_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_sync_lfo_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_sync_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_sync_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_sync_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_sync_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_lfo_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_lfo_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_lfo_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_lfo_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_lfo_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_lfo_depth != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_lfo_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_lfo_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->synth_1_lfo_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->synth_1_lfo_tuning != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->synth_1_lfo_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->synth_1_lfo_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_NOISE_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->noise_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->noise_gain != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->noise_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->noise_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PITCH_TYPE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->pitch_type){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->pitch_type != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->pitch_type));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->pitch_type = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PITCH_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->pitch_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->pitch_tuning != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->pitch_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->pitch_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_ENABLED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->chorus_enabled){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->chorus_enabled != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->chorus_enabled));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->chorus_enabled = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_PITCH_TYPE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->chorus_pitch_type){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->chorus_pitch_type != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->chorus_pitch_type));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->chorus_pitch_type = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_INPUT_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->chorus_input_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->chorus_input_volume != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->chorus_input_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->chorus_input_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_OUTPUT_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->chorus_output_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->chorus_output_volume != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->chorus_output_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->chorus_output_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_LFO_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->chorus_lfo_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->chorus_lfo_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->chorus_lfo_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->chorus_lfo_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_LFO_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->chorus_lfo_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->chorus_lfo_frequency != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->chorus_lfo_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->chorus_lfo_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->chorus_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->chorus_depth != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->chorus_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->chorus_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_MIX:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->chorus_mix){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->chorus_mix != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->chorus_mix));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->chorus_mix = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_DELAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->chorus_delay){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->chorus_delay != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->chorus_delay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->chorus_delay = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_ENABLED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->vibrato_enabled){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->vibrato_enabled != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->vibrato_enabled));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->vibrato_enabled = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->vibrato_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->vibrato_gain != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->vibrato_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->vibrato_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_LFO_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->vibrato_lfo_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->vibrato_lfo_depth != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->vibrato_lfo_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->vibrato_lfo_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_LFO_FREQ:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->vibrato_lfo_freq){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->vibrato_lfo_freq != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->vibrato_lfo_freq));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->vibrato_lfo_freq = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_seq_synth_audio->vibrato_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_seq_synth_audio->vibrato_tuning != NULL){
	g_object_unref(G_OBJECT(fx_seq_synth_audio->vibrato_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_seq_synth_audio->vibrato_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_seq_synth_audio_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsFxSeqSynthAudio *fx_seq_synth_audio;

  GRecMutex *recall_mutex;

  fx_seq_synth_audio = AGS_FX_SEQ_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_seq_synth_audio);

  switch(prop_id){
  case PROP_SYNTH_0_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OCTAVE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_octave);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_KEY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_key);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PHASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_phase);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_tuning_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_tuning_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_tuning_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_tuning_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_4:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_tuning_4);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_5:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_tuning_5);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_6:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_tuning_6);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_7:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_tuning_7);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_PINGPONG:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_tuning_pingpong);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_TUNING_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_tuning_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_volume_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_volume_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_volume_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_volume_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_4:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_volume_4);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_5:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_volume_5);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_6:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_volume_6);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_7:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_volume_7);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_PINGPONG:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_volume_pingpong);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SEQ_VOLUME_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_seq_volume_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ENABLED:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_sync_enabled);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_RELATIVE_ATTACK_FACTOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_sync_relative_attack_factor);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_sync_attack_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_sync_phase_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_sync_attack_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_sync_phase_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_sync_attack_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_sync_phase_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_ATTACK_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_sync_attack_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_PHASE_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_sync_phase_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_LFO_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_sync_lfo_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_SYNC_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_sync_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_lfo_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_lfo_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_0_lfo_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_OCTAVE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_octave);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_KEY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_key);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_PHASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_phase);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_tuning_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_tuning_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_tuning_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_tuning_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_4:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_tuning_4);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_5:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_tuning_5);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_6:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_tuning_6);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_7:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_tuning_7);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_PINGPONG:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_tuning_pingpong);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_TUNING_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_tuning_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_volume_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_volume_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_volume_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_volume_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_4:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_volume_4);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_5:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_volume_5);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_6:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_volume_6);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_7:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_volume_7);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_PINGPONG:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_volume_pingpong);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SEQ_VOLUME_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_seq_volume_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ENABLED:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_sync_enabled);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_sync_relative_attack_factor);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_sync_attack_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_0:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_sync_phase_0);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_sync_attack_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_1:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_sync_phase_1);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_sync_attack_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_2:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_sync_phase_2);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_ATTACK_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_sync_attack_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_PHASE_3:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_sync_phase_3);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_LFO_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_sync_lfo_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_SYNC_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_sync_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_lfo_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_lfo_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_1_LFO_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->synth_1_lfo_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_NOISE_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->noise_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PITCH_TYPE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->pitch_type);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PITCH_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->pitch_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_ENABLED:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->chorus_enabled);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_PITCH_TYPE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->chorus_pitch_type);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_INPUT_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->chorus_input_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_OUTPUT_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->chorus_output_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_LFO_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->chorus_lfo_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_LFO_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->chorus_lfo_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->chorus_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_MIX:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->chorus_mix);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_CHORUS_DELAY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->chorus_delay);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_ENABLED:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->vibrato_enabled);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->vibrato_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_LFO_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->vibrato_lfo_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_LFO_FREQ:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->vibrato_lfo_freq);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_VIBRATO_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_seq_synth_audio->vibrato_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_seq_synth_audio_dispose(GObject *gobject)
{
  AgsFxSeqSynthAudio *fx_seq_synth_audio;
  
  fx_seq_synth_audio = AGS_FX_SEQ_SYNTH_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_seq_synth_audio_parent_class)->dispose(gobject);
}

void
ags_fx_seq_synth_audio_finalize(GObject *gobject)
{
  AgsFxSeqSynthAudio *fx_seq_synth_audio;

  guint i;
  
  fx_seq_synth_audio = AGS_FX_SEQ_SYNTH_AUDIO(gobject);

  
  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      ags_fx_seq_synth_audio_scope_data_free(fx_seq_synth_audio->scope_data[i]);
    }
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_seq_synth_audio_parent_class)->finalize(gobject);
}

void
ags_fx_seq_synth_audio_notify_audio_callback(GObject *gobject,
					     GParamSpec *pspec,
					     gpointer user_data)
{
  AgsAudio *audio;
  AgsFxSeqSynthAudio *fx_seq_synth_audio;

  fx_seq_synth_audio = AGS_FX_SEQ_SYNTH_AUDIO(gobject);

  /* get audio */
  audio = NULL;

  g_object_get(fx_seq_synth_audio,
	       "audio", &audio,
	       NULL);

  g_signal_connect_after((GObject *) audio, "set-audio-channels",
			 G_CALLBACK(ags_fx_seq_synth_audio_set_audio_channels_callback), fx_seq_synth_audio);

  if(audio != NULL){
    g_object_unref(audio);
  }
}

void
ags_fx_seq_synth_audio_notify_buffer_size_callback(GObject *gobject,
						   GParamSpec *pspec,
						   gpointer user_data)
{
  AgsFxSeqSynthAudio *fx_seq_synth_audio;

  guint buffer_size;
  guint format;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_seq_synth_audio = AGS_FX_SEQ_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_seq_synth_audio);

  /* get buffer size and format */
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format =  AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  g_object_get(fx_seq_synth_audio,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxSeqSynthAudioScopeData *scope_data;

    scope_data = fx_seq_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxSeqSynthAudioChannelData *channel_data;

	gpointer destination;

	channel_data = scope_data->channel_data[j];
 
	/* free chorus destination */
	destination = ags_chorus_util_get_destination(channel_data->chorus_util);
	
	ags_stream_free(destination);

	/* alloc chorus destination */
	destination = ags_stream_alloc(buffer_size,
				       ags_chorus_util_get_format(channel_data->chorus_util));
	
	ags_chorus_util_set_destination(channel_data->chorus_util,
					destination);

	/* set buffer length */
	ags_seq_synth_util_set_buffer_length(channel_data->seq_synth_0,
					     buffer_size);
	
	ags_seq_synth_util_set_buffer_length(channel_data->seq_synth_1,
					     buffer_size);
	
	ags_noise_util_set_buffer_length(channel_data->noise_util,
					 buffer_size);

	ags_common_pitch_util_set_buffer_length(channel_data->pitch_util,
						channel_data->pitch_type,
						buffer_size);

	/* pitch buffer */
	ags_stream_free(channel_data->pitch_buffer);
  
	channel_data->pitch_buffer = ags_stream_alloc(buffer_size,
						      format);
	
	ags_chorus_util_set_buffer_length(channel_data->chorus_util,
					  buffer_size);
      }
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_seq_synth_audio_notify_format_callback(GObject *gobject,
					      GParamSpec *pspec,
					      gpointer user_data)
{
  AgsFxSeqSynthAudio *fx_seq_synth_audio;

  guint buffer_size;
  guint format;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_seq_synth_audio = AGS_FX_SEQ_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_seq_synth_audio);

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format =  AGS_SOUNDCARD_DEFAULT_FORMAT;

  g_object_get(fx_seq_synth_audio,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxSeqSynthAudioScopeData *scope_data;

    scope_data = fx_seq_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxSeqSynthAudioChannelData *channel_data;

	gpointer destination;

	channel_data = scope_data->channel_data[j];

	ags_seq_synth_util_set_format(channel_data->seq_synth_0,
				      format);

	ags_seq_synth_util_set_format(channel_data->seq_synth_1,
				      format);
	
	/* free chorus destination */
	destination = ags_chorus_util_get_destination(channel_data->chorus_util);
	
	ags_stream_free(destination);

	/* alloc chorus destination */
	destination = ags_stream_alloc(ags_chorus_util_get_buffer_length(channel_data->chorus_util),
				       format);
	
	ags_chorus_util_set_destination(channel_data->chorus_util,
					destination);

	ags_noise_util_set_format(channel_data->noise_util,
				  format);

	ags_common_pitch_util_set_format(channel_data->pitch_util,
					 channel_data->pitch_type,
					 format);

	/* pitch buffer */
	ags_stream_free(channel_data->pitch_buffer);
  
	channel_data->pitch_buffer = ags_stream_alloc(buffer_size,
						      format);
	
	ags_chorus_util_set_format(channel_data->chorus_util,
				   format);
      }
    }
  }

  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_seq_synth_audio_notify_samplerate_callback(GObject *gobject,
						  GParamSpec *pspec,
						  gpointer user_data)
{
  AgsFxSeqSynthAudio *fx_seq_synth_audio;

  guint samplerate;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_seq_synth_audio = AGS_FX_SEQ_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_seq_synth_audio);

  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  g_object_get(fx_seq_synth_audio,
	       "samplerate", &samplerate,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxSeqSynthAudioScopeData *scope_data;

    scope_data = fx_seq_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxSeqSynthAudioChannelData *channel_data;

	channel_data = scope_data->channel_data[j];

	ags_seq_synth_util_set_samplerate(channel_data->seq_synth_0,
					  samplerate);

	ags_seq_synth_util_set_samplerate(channel_data->seq_synth_1,
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
ags_fx_seq_synth_audio_set_audio_channels_callback(AgsAudio *audio,
						   guint audio_channels, guint audio_channels_old,
						   AgsFxSeqSynthAudio *fx_seq_synth_audio)
{
  guint input_pads;
  guint output_port_count, input_port_count;
  guint buffer_size;
  guint format;
  guint samplerate;
  guint i, j, k;

  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_seq_synth_audio);

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
    AgsFxSeqSynthAudioScopeData *scope_data;

    scope_data = fx_seq_synth_audio->scope_data[i];

    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      if(scope_data->audio_channels > audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxSeqSynthAudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];
	
	  ags_fx_seq_synth_audio_channel_data_free(channel_data);
	}
      }
      
      if(scope_data->channel_data == NULL){
	scope_data->channel_data = (AgsFxSeqSynthAudioChannelData **) g_malloc(audio_channels * sizeof(AgsFxSeqSynthAudioChannelData *)); 
      }else{
	scope_data->channel_data = (AgsFxSeqSynthAudioChannelData **) g_realloc(scope_data->channel_data,
										audio_channels * sizeof(AgsFxSeqSynthAudioChannelData *)); 
      }

      if(scope_data->audio_channels < audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxSeqSynthAudioChannelData *channel_data;

	  gpointer destination;  

	  channel_data =
	    scope_data->channel_data[j] = ags_fx_seq_synth_audio_channel_data_alloc();

	  ags_seq_synth_util_set_buffer_length(channel_data->seq_synth_0,
					       buffer_size);
	  ags_seq_synth_util_set_format(channel_data->seq_synth_0,
					format);		
	  ags_seq_synth_util_set_samplerate(channel_data->seq_synth_0,
					    samplerate);

	  ags_seq_synth_util_set_buffer_length(channel_data->seq_synth_1,
					       buffer_size);
	  ags_seq_synth_util_set_format(channel_data->seq_synth_1,
					format);		
	  ags_seq_synth_util_set_samplerate(channel_data->seq_synth_1,
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

	  channel_data->pitch_buffer = ags_stream_alloc(buffer_size,
							format);

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
	    AgsFxSeqSynthAudioInputData *input_data;

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
ags_fx_seq_synth_audio_pitch_type_callback(AgsPort *port, GValue *value,
					   AgsFxSeqSynthAudio *fx_seq_synth_audio)
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
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_seq_synth_audio);

  /* reset pitch util */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxSeqSynthAudioScopeData *scope_data;

    scope_data = fx_seq_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxSeqSynthAudioChannelData *channel_data;
	
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
ags_fx_seq_synth_audio_chorus_pitch_type_callback(AgsPort *port, GValue *value,
						  AgsFxSeqSynthAudio *fx_seq_synth_audio)
{
  AgsPitchTypeMode pitch_type;

  GType pitch_gtype;
  
  guint i, j;

  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_seq_synth_audio);

  /* reset pitch util */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxSeqSynthAudioScopeData *scope_data;

    scope_data = fx_seq_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxSeqSynthAudioChannelData *channel_data;
	
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
 * ags_fx_seq_synth_audio_scope_data_alloc:
 * 
 * Allocate #AgsFxSeqSynthAudioScopeData-struct
 * 
 * Returns: the new #AgsFxSeqSynthAudioScopeData-struct
 * 
 * Since: 7.5.0
 */
AgsFxSeqSynthAudioScopeData*
ags_fx_seq_synth_audio_scope_data_alloc()
{
  AgsFxSeqSynthAudioScopeData *scope_data;

  scope_data = (AgsFxSeqSynthAudioScopeData *) g_malloc(sizeof(AgsFxSeqSynthAudioScopeData));

  g_rec_mutex_init(&(scope_data->strct_mutex));
  
  scope_data->parent = NULL;
  
  scope_data->audio_channels = 0;

  scope_data->channel_data = NULL;
  
  return(scope_data);
}

/**
 * ags_fx_seq_synth_audio_scope_data_free:
 * @scope_data: the #AgsFxSeqSynthAudioScopeData-struct
 * 
 * Free @scope_data.
 * 
 * Since: 7.5.0
 */
void
ags_fx_seq_synth_audio_scope_data_free(AgsFxSeqSynthAudioScopeData *scope_data)
{
  guint i;

  if(scope_data == NULL){
    return;
  }

  for(i = 0; i < scope_data->audio_channels; i++){
    ags_fx_seq_synth_audio_channel_data_free(scope_data->channel_data[i]);
  }

  g_free(scope_data);
}

/**
 * ags_fx_seq_synth_audio_channel_data_alloc:
 * 
 * Allocate #AgsFxSeqSynthAudioChannelData-struct
 * 
 * Returns: the new #AgsFxSeqSynthAudioChannelData-struct
 * 
 * Since: 7.5.0
 */
AgsFxSeqSynthAudioChannelData*
ags_fx_seq_synth_audio_channel_data_alloc()
{
  AgsFxSeqSynthAudioChannelData *channel_data;

  guint i;
  
  channel_data = (AgsFxSeqSynthAudioChannelData *) g_malloc(sizeof(AgsFxSeqSynthAudioChannelData));

  g_rec_mutex_init(&(channel_data->strct_mutex));

  channel_data->parent = NULL;

  /* seq synth util */
  channel_data->seq_synth_0 = ags_seq_synth_util_alloc();
  channel_data->seq_synth_1 = ags_seq_synth_util_alloc();

  /* noise util */
  channel_data->noise_util = ags_noise_util_alloc();

  /* pitch util */
  channel_data->pitch_type = AGS_TYPE_FLUID_INTERPOLATE_4TH_ORDER_UTIL;
  channel_data->pitch_util = ags_fluid_interpolate_4th_order_util_alloc();

  channel_data->pitch_buffer = NULL;

  /* chorus util */
  channel_data->chorus_util = ags_chorus_util_alloc();
  
  /* chorus util */
  channel_data->chorus_util = ags_chorus_util_alloc();
  
  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    channel_data->input_data[i] = ags_fx_seq_synth_audio_input_data_alloc();

    channel_data->input_data[i]->parent = channel_data;
  }

  return(channel_data);
}

/**
 * ags_fx_seq_synth_audio_channel_data_free:
 * @channel_data: the #AgsFxSeqSynthAudioChannelData-struct
 * 
 * Free @channel_data.
 * 
 * Since: 7.5.0
 */
void
ags_fx_seq_synth_audio_channel_data_free(AgsFxSeqSynthAudioChannelData *channel_data)
{
  guint i;

  if(channel_data == NULL){
    return;
  }

  /* seq synth util */
  ags_seq_synth_util_free(channel_data->seq_synth_0);
  ags_seq_synth_util_free(channel_data->seq_synth_1);
  
  /* pitch buffer */
  ags_stream_free(channel_data->pitch_buffer);
  
  /* chorus util */
  ags_chorus_util_free(channel_data->chorus_util);
  
  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    ags_fx_seq_synth_audio_input_data_free(channel_data->input_data[i]);
  }

  g_free(channel_data);
}

/**
 * ags_fx_seq_synth_audio_input_data_alloc:
 * 
 * Allocate #AgsFxSeqSynthAudioInputData-struct
 * 
 * Returns: the new #AgsFxSeqSynthAudioInputData-struct
 * 
 * Since: 7.5.0
 */
AgsFxSeqSynthAudioInputData*
ags_fx_seq_synth_audio_input_data_alloc()
{
  AgsFxSeqSynthAudioInputData *input_data;

  input_data = (AgsFxSeqSynthAudioInputData *) g_malloc(sizeof(AgsFxSeqSynthAudioInputData));

  g_rec_mutex_init(&(input_data->strct_mutex));

  input_data->parent = NULL;

  input_data->key_on = 0;
  
  return(input_data);
}

/**
 * ags_fx_seq_synth_audio_input_data_free:
 * @input_data: the #AgsFxSeqSynthAudioInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 7.5.0
 */
void
ags_fx_seq_synth_audio_input_data_free(AgsFxSeqSynthAudioInputData *input_data)
{
  if(input_data == NULL){
    return;
  }

  g_free(input_data);
}

static AgsPluginPort*
ags_fx_seq_synth_audio_get_synth_0_oscillator_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_octave_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_key_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_phase_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_volume_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_seq_tuning_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_seq_tuning_pingpong_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_seq_tuning_lfo_frequency_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_seq_volume_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_seq_volume_pingpong_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_seq_volume_lfo_frequency_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_sync_enabled_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_sync_relative_attack_factor_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_sync_attack_0_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_sync_phase_0_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_sync_attack_1_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_sync_phase_1_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_sync_attack_2_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_sync_phase_2_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_sync_attack_3_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_sync_phase_3_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_sync_lfo_oscillator_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_sync_lfo_frequency_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_lfo_oscillator_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_0_lfo_frequency_plugin_port()
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
		      AGS_SEQ_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_seq_synth_audio_get_synth_0_lfo_depth_plugin_port()
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
		      AGS_SEQ_SYNTH_UTIL_DEFAULT_LFO_DEPTH);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_seq_synth_audio_get_synth_0_lfo_tuning_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_oscillator_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_octave_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_key_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_phase_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_volume_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_seq_tuning_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_seq_tuning_pingpong_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_seq_tuning_lfo_frequency_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_seq_volume_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_seq_volume_pingpong_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_seq_volume_lfo_frequency_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_sync_enabled_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_sync_relative_attack_factor_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_sync_attack_0_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_sync_phase_0_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_sync_attack_1_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_sync_phase_1_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_sync_attack_2_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_sync_phase_2_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_sync_attack_3_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_sync_phase_3_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_sync_lfo_oscillator_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_sync_lfo_frequency_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_lfo_oscillator_plugin_port()
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
ags_fx_seq_synth_audio_get_synth_1_lfo_frequency_plugin_port()
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
		      AGS_SEQ_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_seq_synth_audio_get_synth_1_lfo_depth_plugin_port()
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
		      AGS_SEQ_SYNTH_UTIL_DEFAULT_LFO_DEPTH);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_seq_synth_audio_get_synth_1_lfo_tuning_plugin_port()
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
ags_fx_seq_synth_audio_get_noise_gain_plugin_port()
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
ags_fx_seq_synth_audio_get_pitch_type_plugin_port()
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
ags_fx_seq_synth_audio_get_pitch_tuning_plugin_port()
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
ags_fx_seq_synth_audio_get_chorus_enabled_plugin_port()
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
ags_fx_seq_synth_audio_get_chorus_pitch_type_plugin_port()
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
ags_fx_seq_synth_audio_get_chorus_input_volume_plugin_port()
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
ags_fx_seq_synth_audio_get_chorus_output_volume_plugin_port()
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
ags_fx_seq_synth_audio_get_chorus_lfo_oscillator_plugin_port()
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
ags_fx_seq_synth_audio_get_chorus_lfo_frequency_plugin_port()
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
ags_fx_seq_synth_audio_get_chorus_depth_plugin_port()
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
ags_fx_seq_synth_audio_get_chorus_mix_plugin_port()
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
ags_fx_seq_synth_audio_get_chorus_delay_plugin_port()
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
ags_fx_seq_synth_audio_get_vibrato_enabled_plugin_port()
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
ags_fx_seq_synth_audio_get_vibrato_gain_plugin_port()
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
ags_fx_seq_synth_audio_get_vibrato_lfo_depth_plugin_port()
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
ags_fx_seq_synth_audio_get_vibrato_lfo_freq_plugin_port()
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
ags_fx_seq_synth_audio_get_vibrato_tuning_plugin_port()
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
 * ags_fx_seq_synth_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxSeqSynthAudio
 *
 * Returns: the new #AgsFxSeqSynthAudio
 *
 * Since: 7.5.0
 */
AgsFxSeqSynthAudio*
ags_fx_seq_synth_audio_new(AgsAudio *audio)
{
  AgsFxSeqSynthAudio *fx_seq_synth_audio;

  fx_seq_synth_audio = (AgsFxSeqSynthAudio *) g_object_new(AGS_TYPE_FX_SEQ_SYNTH_AUDIO,
							   "audio", audio,
							   NULL);
  
  return(fx_seq_synth_audio);
}
