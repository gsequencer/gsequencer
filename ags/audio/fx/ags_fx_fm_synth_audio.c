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

#include <ags/audio/fx/ags_fx_fm_synth_audio.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_fx_fm_synth_audio_class_init(AgsFxFMSynthAudioClass *fx_fm_synth_audio);
void ags_fx_fm_synth_audio_init(AgsFxFMSynthAudio *fx_fm_synth_audio);
void ags_fx_fm_synth_audio_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_fx_fm_synth_audio_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_fx_fm_synth_audio_dispose(GObject *gobject);
void ags_fx_fm_synth_audio_finalize(GObject *gobject);

void ags_fx_fm_synth_audio_notify_audio_callback(GObject *gobject,
						 GParamSpec *pspec,
						 gpointer user_data);
void ags_fx_fm_synth_audio_notify_buffer_size_callback(GObject *gobject,
						       GParamSpec *pspec,
						       gpointer user_data);
void ags_fx_fm_synth_audio_notify_format_callback(GObject *gobject,
						  GParamSpec *pspec,
						  gpointer user_data);
void ags_fx_fm_synth_audio_notify_samplerate_callback(GObject *gobject,
						      GParamSpec *pspec,
						      gpointer user_data);

void ags_fx_fm_synth_audio_set_audio_channels_callback(AgsAudio *audio,
						       guint audio_channels, guint audio_channels_old,
						       AgsFxFMSynthAudio *fx_fm_synth_audio);

static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_0_oscillator_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_0_octave_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_0_key_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_0_phase_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_0_volume_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_0_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_0_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_0_lfo_depth_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_0_lfo_tuning_plugin_port();

static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_1_oscillator_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_1_octave_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_1_key_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_1_phase_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_1_volume_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_1_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_1_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_1_lfo_depth_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_1_lfo_tuning_plugin_port();

static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_2_oscillator_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_2_octave_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_2_key_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_2_phase_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_2_volume_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_2_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_2_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_2_lfo_depth_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_synth_2_lfo_tuning_plugin_port();

static AgsPluginPort* ags_fx_fm_synth_audio_get_sequencer_enabled_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_sequencer_sign_plugin_port();

static AgsPluginPort* ags_fx_fm_synth_audio_get_noise_gain_plugin_port();

static AgsPluginPort* ags_fx_fm_synth_audio_get_pitch_tuning_plugin_port();

static AgsPluginPort* ags_fx_fm_synth_audio_get_chorus_enabled_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_chorus_input_volume_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_chorus_output_volume_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_chorus_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_chorus_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_chorus_depth_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_chorus_mix_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_chorus_delay_plugin_port();

static AgsPluginPort* ags_fx_fm_synth_audio_get_low_pass_enabled_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_low_pass_q_lin_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_low_pass_filter_gain_plugin_port();

static AgsPluginPort* ags_fx_fm_synth_audio_get_high_pass_enabled_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_high_pass_q_lin_plugin_port();
static AgsPluginPort* ags_fx_fm_synth_audio_get_high_pass_filter_gain_plugin_port();

/**
 * SECTION:ags_fx_fm_synth_audio
 * @short_description: fx FM synth audio
 * @title: AgsFxFMSynthAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_fm_synth_audio.h
 *
 * The #AgsFxFMSynthAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_fm_synth_audio_parent_class = NULL;

const gchar *ags_fx_fm_synth_audio_plugin_name = "ags-fx-fm-synth";

const gchar* ags_fx_fm_synth_audio_specifier[] = {
  "./synth-0-oscillator[0]",
  "./synth-0-octave[0]",
  "./synth-0-key[0]",
  "./synth-0-phase[0]",
  "./synth-0-volume[0]",
  "./synth-0-lfo-oscillator[0]",
  "./synth-0-lfo-frequency[0]",
  "./synth-0-lfo-depth[0]",
  "./synth-0-lfo-tuning[0]",
  "./synth-1-oscillator[0]",
  "./synth-1-octave[0]",
  "./synth-1-key[0]",
  "./synth-1-phase[0]",
  "./synth-1-volume[0]",
  "./synth-1-lfo-oscillator[0]",
  "./synth-1-lfo-frequency[0]",
  "./synth-1-lfo-depth[0]",
  "./synth-1-lfo-tuning[0]",
  "./synth-2-oscillator[0]",
  "./synth-2-octave[0]",
  "./synth-2-key[0]",
  "./synth-2-phase[0]",
  "./synth-2-volume[0]",
  "./synth-2-lfo-oscillator[0]",
  "./synth-2-lfo-frequency[0]",
  "./synth-2-lfo-depth[0]",
  "./synth-2-lfo-tuning[0]",
  "./sequencer-enabled[0]",
  "./sequencer-sign[0]",
  "./noise-gain[0]",
  "./pitch-tuning[0]",
  "./chorus-enabled[0]",
  "./chorus-input-volume[0]",
  "./chorus-output-volume[0]",
  "./chorus-lfo-oscillator[0]",
  "./chorus-lfo-frequency[0]",
  "./chorus-depth[0]",
  "./chorus-mix[0]",
  "./chorus-delay[0]",
  "./low-pass-enabled[0]",
  "./low-pass-q-lin[0]",
  "./low-pass-filter-gain[0]",
  "./high-pass-enabled[0]",
  "./high-pass-q-lin[0]",
  "./high-pass-filter-gain[0]",
  NULL,
};

const gchar* ags_fx_fm_synth_audio_control_port[] = {
  "1/44",
  "2/44",
  "3/44",
  "4/44",
  "5/44",
  "6/44",
  "7/44",
  "8/44",
  "9/44",
  "10/44",
  "11/44",
  "12/44",
  "13/44",
  "14/44",
  "15/44",
  "16/44",
  "17/44",
  "18/44",
  "19/44",
  "20/44",
  "21/44",
  "22/44",
  "23/44",
  "24/44",
  "25/44",
  "26/44",
  "27/44",
  "28/44",
  "29/44",
  "30/44",
  "31/44",
  "32/44",
  "33/44",
  "34/44",
  "35/44",
  "36/44",
  "37/44",
  "38/44",
  "39/44",
  "40/44",
  "41/44",
  "42/44",
  "43/44",
  "44/44",
  NULL,
};

enum{
  PROP_0,
  PROP_SYNTH_0_OSCILLATOR,
  PROP_SYNTH_0_OCTAVE,
  PROP_SYNTH_0_KEY,
  PROP_SYNTH_0_PHASE,
  PROP_SYNTH_0_VOLUME,
  PROP_SYNTH_0_LFO_OSCILLATOR,
  PROP_SYNTH_0_LFO_FREQUENCY,
  PROP_SYNTH_0_LFO_DEPTH,
  PROP_SYNTH_0_LFO_TUNING,
  PROP_SYNTH_1_OSCILLATOR,
  PROP_SYNTH_1_OCTAVE,
  PROP_SYNTH_1_KEY,
  PROP_SYNTH_1_PHASE,
  PROP_SYNTH_1_VOLUME,
  PROP_SYNTH_1_LFO_OSCILLATOR,
  PROP_SYNTH_1_LFO_FREQUENCY,
  PROP_SYNTH_1_LFO_DEPTH,
  PROP_SYNTH_1_LFO_TUNING,
  PROP_SYNTH_2_OSCILLATOR,
  PROP_SYNTH_2_OCTAVE,
  PROP_SYNTH_2_KEY,
  PROP_SYNTH_2_PHASE,
  PROP_SYNTH_2_VOLUME,
  PROP_SYNTH_2_LFO_OSCILLATOR,
  PROP_SYNTH_2_LFO_FREQUENCY,
  PROP_SYNTH_2_LFO_DEPTH,
  PROP_SYNTH_2_LFO_TUNING,
  PROP_SEQUENCER_ENABLED,
  PROP_SEQUENCER_SIGN,
  PROP_NOISE_GAIN,
  PROP_PITCH_TUNING,
  PROP_CHORUS_ENABLED,
  PROP_CHORUS_INPUT_VOLUME,
  PROP_CHORUS_OUTPUT_VOLUME,
  PROP_CHORUS_LFO_OSCILLATOR,
  PROP_CHORUS_LFO_FREQUENCY,
  PROP_CHORUS_DEPTH,
  PROP_CHORUS_MIX,
  PROP_CHORUS_DELAY,
  PROP_LOW_PASS_ENABLED,
  PROP_LOW_PASS_Q_LIN,
  PROP_LOW_PASS_FILTER_GAIN,
  PROP_HIGH_PASS_ENABLED,
  PROP_HIGH_PASS_Q_LIN,
  PROP_HIGH_PASS_FILTER_GAIN,  
};

GType
ags_fx_fm_synth_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_fm_synth_audio = 0;

    static const GTypeInfo ags_fx_fm_synth_audio_info = {
      sizeof (AgsFxFMSynthAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_fm_synth_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxFMSynthAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_fm_synth_audio_init,
    };

    ags_type_fx_fm_synth_audio = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO,
							"AgsFxFMSynthAudio",
							&ags_fx_fm_synth_audio_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_fm_synth_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_fm_synth_audio_class_init(AgsFxFMSynthAudioClass *fx_fm_synth_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_fm_synth_audio_parent_class = g_type_class_peek_parent(fx_fm_synth_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_fm_synth_audio;

  gobject->set_property = ags_fx_fm_synth_audio_set_property;
  gobject->get_property = ags_fx_fm_synth_audio_get_property;

  gobject->dispose = ags_fx_fm_synth_audio_dispose;
  gobject->finalize = ags_fx_fm_synth_audio_finalize;

  /* properties */
  /**
   * AgsFxFMSynthAudio:synth-0-oscillator:
   *
   * The synth-0 oscillator.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:synth-0-octave:
   *
   * The synth-0 octave.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:synth-0-key:
   *
   * The synth-0 key.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:synth-0-phase:
   *
   * The synth-0 phase.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:synth-0-volume:
   *
   * The synth-0 volume.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:synth-0-lfo-oscillator:
   *
   * The synth-0 lfo-oscillator.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:synth-0-lfo-frequency:
   *
   * The synth-0 LFO frequency.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:synth-0-lfo-depth:
   *
   * The synth-0 LFO depth.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:synth-0-lfo-tuning:
   *
   * The synth-0 LFO tuning.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:synth-0-oscillator:
   *
   * The synth-0 oscillator.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-oscillator",
				   i18n_pspec("synth-0 oscillator of recall"),
				   i18n_pspec("The synth-0's oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-octave:
   *
   * The synth-0 octave.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-octave",
				   i18n_pspec("synth-0 octave of recall"),
				   i18n_pspec("The synth-0's octave"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_OCTAVE,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-key:
   *
   * The synth-0 key.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-key",
				   i18n_pspec("synth-0 key of recall"),
				   i18n_pspec("The synth-0's key"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_KEY,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-phase:
   *
   * The synth-0 phase.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-phase",
				   i18n_pspec("synth-0 phase of recall"),
				   i18n_pspec("The synth-0's phase"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_PHASE,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-volume:
   *
   * The synth-0 volume.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-volume",
				   i18n_pspec("synth-0 volume of recall"),
				   i18n_pspec("The synth-0's volume"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_VOLUME,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-lfo-oscillator:
   *
   * The synth-0 lfo-oscillator.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-oscillator",
				   i18n_pspec("synth-0 LFO oscillator of recall"),
				   i18n_pspec("The synth-0's LFO oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_LFO_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-lfo-frequency:
   *
   * The synth-0 LFO frequency.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-frequency",
				   i18n_pspec("synth-0 LFO frequency of recall"),
				   i18n_pspec("The synth-0's LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-lfo-depth:
   *
   * The synth-0 LFO depth.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-depth",
				   i18n_pspec("synth-0 LFO depth of recall"),
				   i18n_pspec("The synth-0's LFO depth"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_LFO_DEPTH,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-lfo-tuning:
   *
   * The synth-0 LFO tuning.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-tuning",
				   i18n_pspec("synth-0 LFO tuning of recall"),
				   i18n_pspec("The synth-0's LFO tuning"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_1_LFO_TUNING,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-oscillator:
   *
   * The synth-0 oscillator.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-oscillator",
				   i18n_pspec("synth-0 oscillator of recall"),
				   i18n_pspec("The synth-0's oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_2_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-octave:
   *
   * The synth-0 octave.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-octave",
				   i18n_pspec("synth-0 octave of recall"),
				   i18n_pspec("The synth-0's octave"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_2_OCTAVE,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-key:
   *
   * The synth-0 key.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-key",
				   i18n_pspec("synth-0 key of recall"),
				   i18n_pspec("The synth-0's key"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_2_KEY,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-phase:
   *
   * The synth-0 phase.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-phase",
				   i18n_pspec("synth-0 phase of recall"),
				   i18n_pspec("The synth-0's phase"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_2_PHASE,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-volume:
   *
   * The synth-0 volume.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-volume",
				   i18n_pspec("synth-0 volume of recall"),
				   i18n_pspec("The synth-0's volume"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_2_VOLUME,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-lfo-oscillator:
   *
   * The synth-0 lfo-oscillator.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-oscillator",
				   i18n_pspec("synth-0 LFO oscillator of recall"),
				   i18n_pspec("The synth-0's LFO oscillator"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_2_LFO_OSCILLATOR,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-lfo-frequency:
   *
   * The synth-0 LFO frequency.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-frequency",
				   i18n_pspec("synth-0 LFO frequency of recall"),
				   i18n_pspec("The synth-0's LFO frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_2_LFO_FREQUENCY,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-lfo-depth:
   *
   * The synth-0 LFO depth.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-depth",
				   i18n_pspec("synth-0 LFO depth of recall"),
				   i18n_pspec("The synth-0's LFO depth"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_2_LFO_DEPTH,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:synth-0-lfo-tuning:
   *
   * The synth-0 LFO tuning.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("synth-0-lfo-tuning",
				   i18n_pspec("synth-0 LFO tuning of recall"),
				   i18n_pspec("The synth-0's LFO tuning"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_2_LFO_TUNING,
				  param_spec);


  /**
   * AgsFxFMSynthAudio:sequencer-enabled:
   *
   * The sequencer enabled.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("sequencer-enabled",
				   i18n_pspec("sequencer enabled of recall"),
				   i18n_pspec("The sequencer enabled"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_ENABLED,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:sequencer-sign:
   *
   * The sequencer sign.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("sequencer-sign",
				   i18n_pspec("sequencer sign of recall"),
				   i18n_pspec("The sequencer sign"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_SIGN,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:noise-gain:
   *
   * The noise gain.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:pitch-tuning:
   *
   * The pitch tuning.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:chorus-enabled:
   *
   * The chorus enabled.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:chorus-input-volume:
   *
   * The chorus input volume.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:chorus-output-volume:
   *
   * The chorus output volume.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:chorus-lfo-oscillator:
   *
   * The chorus lfo oscillator.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:chorus-lfo-frequency:
   *
   * The chorus lfo frequency.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:chorus-depth:
   *
   * The chorus depth.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:chorus-mix:
   *
   * The chorus mix.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:chorus-delay:
   *
   * The chorus delay.
   * 
   * Since: 3.15.0
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
   * AgsFxFMSynthAudio:low-pass-enabled:
   *
   * The low pass enabled.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("low-pass-enabled",
				   i18n_pspec("low pass enabled of recall"),
				   i18n_pspec("The low pass enabled"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOW_PASS_ENABLED,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:low-pass-q-lin:
   *
   * The low pass q-lin.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("low-pass-q-lin",
				   i18n_pspec("low pass q-lin of recall"),
				   i18n_pspec("The low pass q-lin"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOW_PASS_Q_LIN,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:low-pass-filter-gain:
   *
   * The low pass filter gain.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("low-pass-filter-gain",
				   i18n_pspec("low pass filter gain of recall"),
				   i18n_pspec("The low pass filter gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LOW_PASS_FILTER_GAIN,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:high-pass-enabled:
   *
   * The high pass enabled.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("high-pass-enabled",
				   i18n_pspec("high pass enabled of recall"),
				   i18n_pspec("The high pass enabled"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_HIGH_PASS_ENABLED,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:high-pass-q-lin:
   *
   * The high pass q-lin.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("high-pass-q-lin",
				   i18n_pspec("high pass q-lin of recall"),
				   i18n_pspec("The high pass q-lin"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_HIGH_PASS_Q_LIN,
				  param_spec);

  /**
   * AgsFxFMSynthAudio:high-pass-filter-gain:
   *
   * The high pass filter gain.
   * 
   * Since: 3.15.0
   */
  param_spec = g_param_spec_object("high-pass-filter-gain",
				   i18n_pspec("high pass filter gain of recall"),
				   i18n_pspec("The high pass filter gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_HIGH_PASS_FILTER_GAIN,
				  param_spec);
}

void
ags_fx_fm_synth_audio_init(AgsFxFMSynthAudio *fx_fm_synth_audio)
{
  guint i;
  
  g_signal_connect(fx_fm_synth_audio, "notify::audio",
		   G_CALLBACK(ags_fx_fm_synth_audio_notify_audio_callback), NULL);

  g_signal_connect(fx_fm_synth_audio, "notify::buffer-size",
		   G_CALLBACK(ags_fx_fm_synth_audio_notify_buffer_size_callback), NULL);

  g_signal_connect(fx_fm_synth_audio, "notify::format",
		   G_CALLBACK(ags_fx_fm_synth_audio_notify_format_callback), NULL);

  g_signal_connect(fx_fm_synth_audio, "notify::samplerate",
		   G_CALLBACK(ags_fx_fm_synth_audio_notify_samplerate_callback), NULL);

  AGS_RECALL(fx_fm_synth_audio)->name = "ags-fx-fm-synth";
  AGS_RECALL(fx_fm_synth_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_fm_synth_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_fm_synth_audio)->xml_type = "ags-fx-fm-synth-audio";

  /* synth-0 oscillator */
  fx_fm_synth_audio->synth_0_oscillator = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						       "specifier", ags_fx_fm_synth_audio_specifier[0],
						       "control-port", ags_fx_fm_synth_audio_control_port[0],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_fm_synth_audio->synth_0_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_fm_synth_audio->synth_0_oscillator,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_0_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_0_oscillator);

  /* synth-0 octave */
  fx_fm_synth_audio->synth_0_octave = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						   "specifier", ags_fx_fm_synth_audio_specifier[1],
						   "control-port", ags_fx_fm_synth_audio_control_port[1],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_fm_synth_audio->synth_0_octave->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->synth_0_octave,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_0_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_0_octave);

  /* synth-0 key */
  fx_fm_synth_audio->synth_0_key = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_fm_synth_audio_plugin_name,
						"specifier", ags_fx_fm_synth_audio_specifier[2],
						"control-port", ags_fx_fm_synth_audio_control_port[2],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 1,
						NULL);
  
  fx_fm_synth_audio->synth_0_key->port_value.ags_port_float = (gfloat) 2.0;

  g_object_set(fx_fm_synth_audio->synth_0_key,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_0_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_0_key);

  /* synth-0 phase */
  fx_fm_synth_audio->synth_0_phase = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						  "specifier", ags_fx_fm_synth_audio_specifier[3],
						  "control-port", ags_fx_fm_synth_audio_control_port[3],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 1,
						  NULL);
  
  fx_fm_synth_audio->synth_0_phase->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->synth_0_phase,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_0_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_0_phase);

  /* synth-0 volume */
  fx_fm_synth_audio->synth_0_volume = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						   "specifier", ags_fx_fm_synth_audio_specifier[4],
						   "control-port", ags_fx_fm_synth_audio_control_port[4],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_fm_synth_audio->synth_0_volume->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_fm_synth_audio->synth_0_volume,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_0_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_0_volume);

  /* synth-0 LFO oscillator */
  fx_fm_synth_audio->synth_0_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_fm_synth_audio_plugin_name,
							   "specifier", ags_fx_fm_synth_audio_specifier[5],
							   "control-port", ags_fx_fm_synth_audio_control_port[5],
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_fm_synth_audio->synth_0_lfo_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_fm_synth_audio->synth_0_lfo_oscillator,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_0_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_0_lfo_oscillator);

  /* synth-0 LFO frequency */
  fx_fm_synth_audio->synth_0_lfo_frequency = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_fm_synth_audio_plugin_name,
							  "specifier", ags_fx_fm_synth_audio_specifier[6],
							  "control-port", ags_fx_fm_synth_audio_control_port[6],
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_fm_synth_audio->synth_0_lfo_frequency->port_value.ags_port_float = (gfloat) 0.01;

  g_object_set(fx_fm_synth_audio->synth_0_lfo_frequency,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_0_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_0_lfo_frequency);

  /* synth-0 LFO depth */
  fx_fm_synth_audio->synth_0_lfo_depth = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						      "specifier", ags_fx_fm_synth_audio_specifier[7],
						      "control-port", ags_fx_fm_synth_audio_control_port[7],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_fm_synth_audio->synth_0_lfo_depth->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_fm_synth_audio->synth_0_lfo_depth,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_0_lfo_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_0_lfo_depth);

  /* synth-0 LFO tuning */
  fx_fm_synth_audio->synth_0_lfo_tuning = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						       "specifier", ags_fx_fm_synth_audio_specifier[8],
						       "control-port", ags_fx_fm_synth_audio_control_port[8],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_fm_synth_audio->synth_0_lfo_tuning->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->synth_0_lfo_tuning,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_0_lfo_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_0_lfo_tuning);

  /* synth-1 oscillator */
  fx_fm_synth_audio->synth_1_oscillator = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						       "specifier", ags_fx_fm_synth_audio_specifier[9],
						       "control-port", ags_fx_fm_synth_audio_control_port[9],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_fm_synth_audio->synth_1_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_fm_synth_audio->synth_1_oscillator,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_1_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_1_oscillator);
  
  /* synth-1 octave */
  fx_fm_synth_audio->synth_1_octave = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						   "specifier", ags_fx_fm_synth_audio_specifier[10],
						   "control-port", ags_fx_fm_synth_audio_control_port[10],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_fm_synth_audio->synth_1_octave->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->synth_1_octave,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_1_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_1_octave);

  /* synth-1 key */
  fx_fm_synth_audio->synth_1_key = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_fm_synth_audio_plugin_name,
						"specifier", ags_fx_fm_synth_audio_specifier[11],
						"control-port", ags_fx_fm_synth_audio_control_port[11],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 1,
						NULL);
  
  fx_fm_synth_audio->synth_1_key->port_value.ags_port_float = (gfloat) 2.0;

  g_object_set(fx_fm_synth_audio->synth_1_key,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_1_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_1_key);

  /* synth-1 phase */
  fx_fm_synth_audio->synth_1_phase = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						  "specifier", ags_fx_fm_synth_audio_specifier[12],
						  "control-port", ags_fx_fm_synth_audio_control_port[12],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 1,
						  NULL);
  
  fx_fm_synth_audio->synth_1_phase->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->synth_1_phase,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_1_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_1_phase);

  /* synth-1 volume */
  fx_fm_synth_audio->synth_1_volume = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						   "specifier", ags_fx_fm_synth_audio_specifier[13],
						   "control-port", ags_fx_fm_synth_audio_control_port[13],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_fm_synth_audio->synth_1_volume->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_fm_synth_audio->synth_1_volume,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_1_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_1_volume);

  /* synth-1 LFO oscillator */
  fx_fm_synth_audio->synth_1_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_fm_synth_audio_plugin_name,
							   "specifier", ags_fx_fm_synth_audio_specifier[14],
							   "control-port", ags_fx_fm_synth_audio_control_port[14],
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_fm_synth_audio->synth_1_lfo_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_fm_synth_audio->synth_1_lfo_oscillator,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_1_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_1_lfo_oscillator);

  /* synth-1 LFO frequency */
  fx_fm_synth_audio->synth_1_lfo_frequency = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_fm_synth_audio_plugin_name,
							  "specifier", ags_fx_fm_synth_audio_specifier[15],
							  "control-port", ags_fx_fm_synth_audio_control_port[15],
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_fm_synth_audio->synth_1_lfo_frequency->port_value.ags_port_float = (gfloat) 0.01;

  g_object_set(fx_fm_synth_audio->synth_1_lfo_frequency,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_1_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_1_lfo_frequency);

  /* synth-1 LFO depth */
  fx_fm_synth_audio->synth_1_lfo_depth = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						      "specifier", ags_fx_fm_synth_audio_specifier[16],
						      "control-port", ags_fx_fm_synth_audio_control_port[16],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_fm_synth_audio->synth_1_lfo_depth->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_fm_synth_audio->synth_1_lfo_depth,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_1_lfo_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_1_lfo_depth);

  /* synth-1 LFO tuning */
  fx_fm_synth_audio->synth_1_lfo_tuning = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						       "specifier", ags_fx_fm_synth_audio_specifier[17],
						       "control-port", ags_fx_fm_synth_audio_control_port[17],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_fm_synth_audio->synth_1_lfo_tuning->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->synth_1_lfo_tuning,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_1_lfo_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_1_lfo_tuning);

  /* synth-2 octave */
  fx_fm_synth_audio->synth_2_octave = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						   "specifier", ags_fx_fm_synth_audio_specifier[18],
						   "control-port", ags_fx_fm_synth_audio_control_port[18],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_fm_synth_audio->synth_2_octave->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->synth_2_octave,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_2_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_2_octave);

  /* synth-2 key */
  fx_fm_synth_audio->synth_2_key = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_fm_synth_audio_plugin_name,
						"specifier", ags_fx_fm_synth_audio_specifier[19],
						"control-port", ags_fx_fm_synth_audio_control_port[19],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 1,
						NULL);
  
  fx_fm_synth_audio->synth_2_key->port_value.ags_port_float = (gfloat) 2.0;

  g_object_set(fx_fm_synth_audio->synth_2_key,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_2_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_2_key);

  /* synth-2 phase */
  fx_fm_synth_audio->synth_2_phase = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						  "specifier", ags_fx_fm_synth_audio_specifier[20],
						  "control-port", ags_fx_fm_synth_audio_control_port[20],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 1,
						  NULL);
  
  fx_fm_synth_audio->synth_2_phase->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->synth_2_phase,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_2_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_2_phase);

  /* synth-2 volume */
  fx_fm_synth_audio->synth_2_volume = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						   "specifier", ags_fx_fm_synth_audio_specifier[21],
						   "control-port", ags_fx_fm_synth_audio_control_port[21],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_fm_synth_audio->synth_2_volume->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_fm_synth_audio->synth_2_volume,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_2_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_2_volume);

  /* synth-2 LFO oscillator */
  fx_fm_synth_audio->synth_2_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_fm_synth_audio_plugin_name,
							   "specifier", ags_fx_fm_synth_audio_specifier[22],
							   "control-port", ags_fx_fm_synth_audio_control_port[22],
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_fm_synth_audio->synth_2_lfo_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_fm_synth_audio->synth_2_lfo_oscillator,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_2_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_2_lfo_oscillator);

  /* synth-2 LFO frequency */
  fx_fm_synth_audio->synth_2_lfo_frequency = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_fm_synth_audio_plugin_name,
							  "specifier", ags_fx_fm_synth_audio_specifier[23],
							  "control-port", ags_fx_fm_synth_audio_control_port[23],
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_fm_synth_audio->synth_2_lfo_frequency->port_value.ags_port_float = (gfloat) 0.01;

  g_object_set(fx_fm_synth_audio->synth_2_lfo_frequency,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_2_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_2_lfo_frequency);

  /* synth-2 LFO depth */
  fx_fm_synth_audio->synth_2_lfo_depth = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						      "specifier", ags_fx_fm_synth_audio_specifier[24],
						      "control-port", ags_fx_fm_synth_audio_control_port[24],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_fm_synth_audio->synth_2_lfo_depth->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_fm_synth_audio->synth_2_lfo_depth,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_2_lfo_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_2_lfo_depth);

  /* synth-2 LFO tuning */
  fx_fm_synth_audio->synth_2_lfo_tuning = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						       "specifier", ags_fx_fm_synth_audio_specifier[25],
						       "control-port", ags_fx_fm_synth_audio_control_port[25],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_fm_synth_audio->synth_2_lfo_tuning->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->synth_2_lfo_tuning,
	       "plugin-port", ags_fx_fm_synth_audio_get_synth_2_lfo_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->synth_2_lfo_tuning);
  
  /* sequencer enabled */
  fx_fm_synth_audio->sequencer_enabled = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						      "specifier", ags_fx_fm_synth_audio_specifier[26],
						      "control-port", ags_fx_fm_synth_audio_control_port[26],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_fm_synth_audio->sequencer_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_fm_synth_audio->sequencer_enabled,
	       "plugin-port", ags_fx_fm_synth_audio_get_sequencer_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->sequencer_enabled);

  /* sequencer sign */
  fx_fm_synth_audio->sequencer_sign = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						   "specifier", ags_fx_fm_synth_audio_specifier[27],
						   "control-port", ags_fx_fm_synth_audio_control_port[27],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_fm_synth_audio->sequencer_sign->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->sequencer_sign,
	       "plugin-port", ags_fx_fm_synth_audio_get_sequencer_sign_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->sequencer_sign);

  /* noise gain */
  fx_fm_synth_audio->noise_gain = g_object_new(AGS_TYPE_PORT,
					       "plugin-name", ags_fx_fm_synth_audio_plugin_name,
					       "specifier", ags_fx_fm_synth_audio_specifier[28],
					       "control-port", ags_fx_fm_synth_audio_control_port[28],
					       "port-value-is-pointer", FALSE,
					       "port-value-type", G_TYPE_FLOAT,
					       "port-value-size", sizeof(gfloat),
					       "port-value-length", 1,
					       NULL);
  
  fx_fm_synth_audio->noise_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->noise_gain,
	       "plugin-port", ags_fx_fm_synth_audio_get_noise_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->noise_gain);

  /* pitch tuning */
  fx_fm_synth_audio->pitch_tuning = g_object_new(AGS_TYPE_PORT,
						 "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						 "specifier", ags_fx_fm_synth_audio_specifier[29],
						 "control-port", ags_fx_fm_synth_audio_control_port[29],
						 "port-value-is-pointer", FALSE,
						 "port-value-type", G_TYPE_FLOAT,
						 "port-value-size", sizeof(gfloat),
						 "port-value-length", 1,
						 NULL);
  
  fx_fm_synth_audio->pitch_tuning->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->pitch_tuning,
	       "plugin-port", ags_fx_fm_synth_audio_get_pitch_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->pitch_tuning);

  /* chorus enabled */
  fx_fm_synth_audio->chorus_enabled = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						   "specifier", ags_fx_fm_synth_audio_specifier[30],
						   "control-port", ags_fx_fm_synth_audio_control_port[30],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_fm_synth_audio->chorus_enabled->port_value.ags_port_float = (gfloat) TRUE;

  g_object_set(fx_fm_synth_audio->chorus_enabled,
	       "plugin-port", ags_fx_fm_synth_audio_get_chorus_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->chorus_enabled);

  /* chorus input volume */
  fx_fm_synth_audio->chorus_input_volume = g_object_new(AGS_TYPE_PORT,
							"plugin-name", ags_fx_fm_synth_audio_plugin_name,
							"specifier", ags_fx_fm_synth_audio_specifier[31],
							"control-port", ags_fx_fm_synth_audio_control_port[31],
							"port-value-is-pointer", FALSE,
							"port-value-type", G_TYPE_FLOAT,
							"port-value-size", sizeof(gfloat),
							"port-value-length", 1,
							NULL);
  
  fx_fm_synth_audio->chorus_input_volume->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_fm_synth_audio->chorus_input_volume,
	       "plugin-port", ags_fx_fm_synth_audio_get_chorus_input_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->chorus_input_volume);

  /* chorus output volume */
  fx_fm_synth_audio->chorus_output_volume = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_fm_synth_audio_plugin_name,
							 "specifier", ags_fx_fm_synth_audio_specifier[32],
							 "control-port", ags_fx_fm_synth_audio_control_port[32],
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_fm_synth_audio->chorus_output_volume->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_fm_synth_audio->chorus_output_volume,
	       "plugin-port", ags_fx_fm_synth_audio_get_chorus_output_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->chorus_output_volume);

  /* chorus LFO oscillator */
  fx_fm_synth_audio->chorus_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_fm_synth_audio_plugin_name,
							  "specifier", ags_fx_fm_synth_audio_specifier[33],
							  "control-port", ags_fx_fm_synth_audio_control_port[33],
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_fm_synth_audio->chorus_lfo_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_fm_synth_audio->chorus_lfo_oscillator,
	       "plugin-port", ags_fx_fm_synth_audio_get_chorus_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->chorus_lfo_oscillator);

  /* chorus LFO frequency */
  fx_fm_synth_audio->chorus_lfo_frequency = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_fm_synth_audio_plugin_name,
							 "specifier", ags_fx_fm_synth_audio_specifier[34],
							 "control-port", ags_fx_fm_synth_audio_control_port[34],
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_fm_synth_audio->chorus_lfo_frequency->port_value.ags_port_float = (gfloat) 10.0;

  g_object_set(fx_fm_synth_audio->chorus_lfo_frequency,
	       "plugin-port", ags_fx_fm_synth_audio_get_chorus_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->chorus_lfo_frequency);

  /* chorus depth */
  fx_fm_synth_audio->chorus_depth = g_object_new(AGS_TYPE_PORT,
						 "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						 "specifier", ags_fx_fm_synth_audio_specifier[35],
						 "control-port", ags_fx_fm_synth_audio_control_port[35],
						 "port-value-is-pointer", FALSE,
						 "port-value-type", G_TYPE_FLOAT,
						 "port-value-size", sizeof(gfloat),
						 "port-value-length", 1,
						 NULL);
  
  fx_fm_synth_audio->chorus_depth->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->chorus_depth,
	       "plugin-port", ags_fx_fm_synth_audio_get_chorus_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->chorus_depth);

  /* chorus mix */
  fx_fm_synth_audio->chorus_mix = g_object_new(AGS_TYPE_PORT,
					       "plugin-name", ags_fx_fm_synth_audio_plugin_name,
					       "specifier", ags_fx_fm_synth_audio_specifier[36],
					       "control-port", ags_fx_fm_synth_audio_control_port[36],
					       "port-value-is-pointer", FALSE,
					       "port-value-type", G_TYPE_FLOAT,
					       "port-value-size", sizeof(gfloat),
					       "port-value-length", 1,
					       NULL);
  
  fx_fm_synth_audio->chorus_mix->port_value.ags_port_float = (gfloat) 0.5;

  g_object_set(fx_fm_synth_audio->chorus_mix,
	       "plugin-port", ags_fx_fm_synth_audio_get_chorus_mix_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->chorus_mix);

  /* chorus delay */
  fx_fm_synth_audio->chorus_delay = g_object_new(AGS_TYPE_PORT,
						 "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						 "specifier", ags_fx_fm_synth_audio_specifier[37],
						 "control-port", ags_fx_fm_synth_audio_control_port[37],
						 "port-value-is-pointer", FALSE,
						 "port-value-type", G_TYPE_FLOAT,
						 "port-value-size", sizeof(gfloat),
						 "port-value-length", 1,
						 NULL);
  
  fx_fm_synth_audio->chorus_delay->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->chorus_delay,
	       "plugin-port", ags_fx_fm_synth_audio_get_chorus_delay_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->chorus_delay);

  /* low-pass enabled */
  fx_fm_synth_audio->low_pass_enabled = g_object_new(AGS_TYPE_PORT,
						     "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						     "specifier", ags_fx_fm_synth_audio_specifier[38],
						     "control-port", ags_fx_fm_synth_audio_control_port[39],
						     "port-value-is-pointer", FALSE,
						     "port-value-type", G_TYPE_FLOAT,
						     "port-value-size", sizeof(gfloat),
						     "port-value-length", 1,
						     NULL);
  
  fx_fm_synth_audio->low_pass_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_fm_synth_audio->low_pass_enabled,
	       "plugin-port", ags_fx_fm_synth_audio_get_low_pass_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->low_pass_enabled);

  /* low-pass q-lin */
  fx_fm_synth_audio->low_pass_q_lin = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						   "specifier", ags_fx_fm_synth_audio_specifier[39],
						   "control-port", ags_fx_fm_synth_audio_control_port[39],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_fm_synth_audio->low_pass_q_lin->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->low_pass_q_lin,
	       "plugin-port", ags_fx_fm_synth_audio_get_low_pass_q_lin_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->low_pass_q_lin);

  /* low-pass filter-gain */
  fx_fm_synth_audio->low_pass_filter_gain = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_fm_synth_audio_plugin_name,
							 "specifier", ags_fx_fm_synth_audio_specifier[40],
							 "control-port", ags_fx_fm_synth_audio_control_port[40],
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_fm_synth_audio->low_pass_filter_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->low_pass_filter_gain,
	       "plugin-port", ags_fx_fm_synth_audio_get_low_pass_filter_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->low_pass_filter_gain);

  /* high-pass enabled */
  fx_fm_synth_audio->high_pass_enabled = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						      "specifier", ags_fx_fm_synth_audio_specifier[41],
						      "control-port", ags_fx_fm_synth_audio_control_port[41],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_fm_synth_audio->high_pass_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_fm_synth_audio->high_pass_enabled,
	       "plugin-port", ags_fx_fm_synth_audio_get_high_pass_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->high_pass_enabled);

  /* high-pass q-lin */
  fx_fm_synth_audio->high_pass_q_lin = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_fm_synth_audio_plugin_name,
						    "specifier", ags_fx_fm_synth_audio_specifier[42],
						    "control-port", ags_fx_fm_synth_audio_control_port[42],
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_fm_synth_audio->high_pass_q_lin->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->high_pass_q_lin,
	       "plugin-port", ags_fx_fm_synth_audio_get_high_pass_q_lin_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->high_pass_q_lin);

  /* high-pass filter-gain */
  fx_fm_synth_audio->high_pass_filter_gain = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_fm_synth_audio_plugin_name,
							  "specifier", ags_fx_fm_synth_audio_specifier[43],
							  "control-port", ags_fx_fm_synth_audio_control_port[43],
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_fm_synth_audio->high_pass_filter_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_fm_synth_audio->high_pass_filter_gain,
	       "plugin-port", ags_fx_fm_synth_audio_get_high_pass_filter_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_fm_synth_audio,
		      fx_fm_synth_audio->high_pass_filter_gain);

  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      fx_fm_synth_audio->scope_data[i] = ags_fx_fm_synth_audio_scope_data_alloc();
      
      fx_fm_synth_audio->scope_data[i]->parent = fx_fm_synth_audio;
    }else{
      fx_fm_synth_audio->scope_data[i] = NULL;
    }
  }
}

void
ags_fx_fm_synth_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsFxFMSynthAudio *fx_fm_synth_audio;

  GRecMutex *recall_mutex;

  fx_fm_synth_audio = AGS_FX_FM_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_fm_synth_audio);

  switch(prop_id){
  case PROP_SYNTH_0_OSCILLATOR:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_0_oscillator){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_0_oscillator != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_0_oscillator));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_0_oscillator = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_OCTAVE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_0_octave){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_0_octave != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_0_octave));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_0_octave = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_KEY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_0_key){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_0_key != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_0_key));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_0_key = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_PHASE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_0_phase){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_0_phase != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_0_phase));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_0_phase = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_VOLUME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_0_volume){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_0_volume != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_0_volume));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_0_volume = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_LFO_OSCILLATOR:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_0_lfo_oscillator){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_0_lfo_oscillator != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_0_lfo_oscillator));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_0_lfo_oscillator = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_LFO_FREQUENCY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_0_lfo_frequency){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_0_lfo_frequency != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_0_lfo_frequency));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_0_lfo_frequency = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_LFO_DEPTH:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_0_lfo_depth){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_0_lfo_depth != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_0_lfo_depth));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_0_lfo_depth = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_LFO_TUNING:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_0_lfo_tuning){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_0_lfo_tuning != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_0_lfo_tuning));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_0_lfo_tuning = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_OSCILLATOR:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_1_oscillator){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_1_oscillator != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_1_oscillator));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_1_oscillator = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_OCTAVE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_1_octave){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_1_octave != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_1_octave));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_1_octave = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_KEY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_1_key){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_1_key != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_1_key));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_1_key = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_PHASE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_1_phase){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_1_phase != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_1_phase));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_1_phase = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_VOLUME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_1_volume){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_1_volume != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_1_volume));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_1_volume = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_LFO_OSCILLATOR:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_1_lfo_oscillator){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_1_lfo_oscillator != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_1_lfo_oscillator));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_1_lfo_oscillator = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_LFO_FREQUENCY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_1_lfo_frequency){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_1_lfo_frequency != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_1_lfo_frequency));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_1_lfo_frequency = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_LFO_DEPTH:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_1_lfo_depth){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_1_lfo_depth != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_1_lfo_depth));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_1_lfo_depth = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_LFO_TUNING:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_1_lfo_tuning){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_1_lfo_tuning != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_1_lfo_tuning));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_1_lfo_tuning = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_OSCILLATOR:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_2_oscillator){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_2_oscillator != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_2_oscillator));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_2_oscillator = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_OCTAVE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_2_octave){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_2_octave != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_2_octave));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_2_octave = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_KEY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_2_key){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_2_key != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_2_key));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_2_key = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_PHASE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_2_phase){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_2_phase != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_2_phase));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_2_phase = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_VOLUME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_2_volume){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_2_volume != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_2_volume));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_2_volume = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_LFO_OSCILLATOR:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_2_lfo_oscillator){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_2_lfo_oscillator != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_2_lfo_oscillator));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_2_lfo_oscillator = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_LFO_FREQUENCY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_2_lfo_frequency){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_2_lfo_frequency != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_2_lfo_frequency));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_2_lfo_frequency = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_LFO_DEPTH:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_2_lfo_depth){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_2_lfo_depth != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_2_lfo_depth));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_2_lfo_depth = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_LFO_TUNING:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->synth_2_lfo_tuning){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->synth_2_lfo_tuning != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->synth_2_lfo_tuning));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->synth_2_lfo_tuning = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SEQUENCER_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->sequencer_enabled){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->sequencer_enabled != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->sequencer_enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->sequencer_enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SEQUENCER_SIGN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->sequencer_sign){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->sequencer_sign != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->sequencer_sign));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->sequencer_sign = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_NOISE_GAIN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->noise_gain){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->noise_gain != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->noise_gain));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->noise_gain = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PITCH_TUNING:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->pitch_tuning){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->pitch_tuning != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->pitch_tuning));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->pitch_tuning = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->chorus_enabled){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->chorus_enabled != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->chorus_enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->chorus_enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_INPUT_VOLUME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->chorus_input_volume){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->chorus_input_volume != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->chorus_input_volume));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->chorus_input_volume = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_OUTPUT_VOLUME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->chorus_output_volume){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->chorus_output_volume != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->chorus_output_volume));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->chorus_output_volume = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_LFO_OSCILLATOR:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->chorus_lfo_oscillator){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->chorus_lfo_oscillator != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->chorus_lfo_oscillator));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->chorus_lfo_oscillator = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_LFO_FREQUENCY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->chorus_lfo_frequency){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->chorus_lfo_frequency != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->chorus_lfo_frequency));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->chorus_lfo_frequency = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_DEPTH:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->chorus_depth){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->chorus_depth != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->chorus_depth));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->chorus_depth = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_MIX:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->chorus_mix){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->chorus_mix != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->chorus_mix));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->chorus_mix = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_DELAY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->chorus_delay){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->chorus_delay != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->chorus_delay));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->chorus_delay = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LOW_PASS_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->low_pass_enabled){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->low_pass_enabled != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->low_pass_enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->low_pass_enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LOW_PASS_Q_LIN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->low_pass_q_lin){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->low_pass_q_lin != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->low_pass_q_lin));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->low_pass_q_lin = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LOW_PASS_FILTER_GAIN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->low_pass_filter_gain){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->low_pass_filter_gain != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->low_pass_filter_gain));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->low_pass_filter_gain = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_HIGH_PASS_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->high_pass_enabled){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->high_pass_enabled != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->high_pass_enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->high_pass_enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_HIGH_PASS_Q_LIN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->high_pass_q_lin){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->high_pass_q_lin != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->high_pass_q_lin));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->high_pass_q_lin = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_HIGH_PASS_FILTER_GAIN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_fm_synth_audio->high_pass_filter_gain){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_fm_synth_audio->high_pass_filter_gain != NULL){
      g_object_unref(G_OBJECT(fx_fm_synth_audio->high_pass_filter_gain));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_fm_synth_audio->high_pass_filter_gain = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_fm_synth_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsFxFMSynthAudio *fx_fm_synth_audio;

  GRecMutex *recall_mutex;

  fx_fm_synth_audio = AGS_FX_FM_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_fm_synth_audio);

  switch(prop_id){
  case PROP_SYNTH_0_OSCILLATOR:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_0_oscillator);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_OCTAVE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_0_octave);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_KEY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_0_key);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_PHASE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_0_phase);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_VOLUME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_0_volume);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_LFO_OSCILLATOR:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_0_lfo_oscillator);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_LFO_FREQUENCY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_0_lfo_frequency);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_LFO_DEPTH:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_0_lfo_depth);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_LFO_TUNING:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_0_lfo_tuning);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_OSCILLATOR:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_1_oscillator);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_OCTAVE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_1_octave);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_KEY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_1_key);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_PHASE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_1_phase);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_VOLUME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_1_volume);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_LFO_OSCILLATOR:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_1_lfo_oscillator);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_LFO_FREQUENCY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_1_lfo_frequency);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_LFO_DEPTH:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_1_lfo_depth);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_LFO_TUNING:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_1_lfo_tuning);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_OSCILLATOR:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_2_oscillator);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_OCTAVE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_2_octave);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_KEY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_2_key);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_PHASE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_2_phase);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_VOLUME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_2_volume);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_LFO_OSCILLATOR:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_2_lfo_oscillator);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_LFO_FREQUENCY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_2_lfo_frequency);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_LFO_DEPTH:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_2_lfo_depth);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_2_LFO_TUNING:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->synth_2_lfo_tuning);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SEQUENCER_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->sequencer_enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SEQUENCER_SIGN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->sequencer_sign);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PITCH_TUNING:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->pitch_tuning);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_NOISE_GAIN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->noise_gain);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->chorus_enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_INPUT_VOLUME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->chorus_input_volume);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_OUTPUT_VOLUME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->chorus_output_volume);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_LFO_OSCILLATOR:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->chorus_lfo_oscillator);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_LFO_FREQUENCY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->chorus_lfo_frequency);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_DEPTH:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->chorus_depth);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_MIX:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->chorus_mix);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_DELAY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->chorus_delay);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LOW_PASS_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->low_pass_enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LOW_PASS_Q_LIN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->low_pass_q_lin);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LOW_PASS_FILTER_GAIN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->low_pass_filter_gain);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_HIGH_PASS_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->high_pass_enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_HIGH_PASS_Q_LIN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->high_pass_q_lin);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_HIGH_PASS_FILTER_GAIN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_fm_synth_audio->high_pass_filter_gain);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_fm_synth_audio_dispose(GObject *gobject)
{
  AgsFxFMSynthAudio *fx_fm_synth_audio;
  
  fx_fm_synth_audio = AGS_FX_FM_SYNTH_AUDIO(gobject);

  //TODO:JK: implement me

  /* call parent */
  G_OBJECT_CLASS(ags_fx_fm_synth_audio_parent_class)->dispose(gobject);
}

void
ags_fx_fm_synth_audio_finalize(GObject *gobject)
{
  AgsFxFMSynthAudio *fx_fm_synth_audio;
  
  fx_fm_synth_audio = AGS_FX_FM_SYNTH_AUDIO(gobject);

  //TODO:JK: implement me

  /* call parent */
  G_OBJECT_CLASS(ags_fx_fm_synth_audio_parent_class)->finalize(gobject);
}

void
ags_fx_fm_synth_audio_notify_audio_callback(GObject *gobject,
					    GParamSpec *pspec,
					    gpointer user_data)
{
  AgsAudio *audio;
  AgsFxFMSynthAudio *fx_fm_synth_audio;

  fx_fm_synth_audio = AGS_FX_FM_SYNTH_AUDIO(gobject);

  /* get audio */
  audio = NULL;

  g_object_get(fx_fm_synth_audio,
	       "audio", &audio,
	       NULL);

  g_signal_connect_after((GObject *) audio, "set-audio-channels",
			 G_CALLBACK(ags_fx_fm_synth_audio_set_audio_channels_callback), fx_fm_synth_audio);

  if(audio != NULL){
    g_object_unref(audio);
  }
}

void
ags_fx_fm_synth_audio_notify_buffer_size_callback(GObject *gobject,
						  GParamSpec *pspec,
						  gpointer user_data)
{
  AgsFxFMSynthAudio *fx_fm_synth_audio;

  guint buffer_size;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_fm_synth_audio = AGS_FX_FM_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_fm_synth_audio);

  /* get buffer size */
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  g_object_get(fx_fm_synth_audio,
	       "buffer-size", &buffer_size,
	       NULL);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxFMSynthAudioScopeData *scope_data;

    scope_data = fx_fm_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxFMSynthAudioChannelData *channel_data;

	channel_data = scope_data->channel_data[j];

	channel_data->noise_util.buffer_length = buffer_size;

	channel_data->hq_pitch_linear_interpolate_util.buffer_length = buffer_size;

	channel_data->hq_pitch_util.buffer_length = buffer_size;

	channel_data->chorus_hq_pitch_util.buffer_length = buffer_size;

	channel_data->chorus_linear_interpolate_util.buffer_length = buffer_size;
	
	channel_data->chorus_util.buffer_length = buffer_size;
      }
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_fm_synth_audio_notify_format_callback(GObject *gobject,
					     GParamSpec *pspec,
					     gpointer user_data)
{
  AgsFxFMSynthAudio *fx_fm_synth_audio;

  guint format;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_fm_synth_audio = AGS_FX_FM_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_fm_synth_audio);

  format =  AGS_SOUNDCARD_DEFAULT_FORMAT;

  g_object_get(fx_fm_synth_audio,
	       "format", &format,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxFMSynthAudioScopeData *scope_data;

    scope_data = fx_fm_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxFMSynthAudioChannelData *channel_data;

	channel_data = scope_data->channel_data[j];

	channel_data->hq_pitch_linear_interpolate_util.format = format;

	ags_stream_free(channel_data->hq_pitch_util.destination);
	
	channel_data->hq_pitch_util.destination = ags_stream_alloc(AGS_FX_FM_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
								   format);
	
	ags_stream_free(channel_data->hq_pitch_util.low_mix_buffer);	
	ags_stream_free(channel_data->hq_pitch_util.new_mix_buffer);
	
	channel_data->hq_pitch_util.low_mix_buffer = ags_stream_alloc(AGS_FX_FM_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
								      format);
	channel_data->hq_pitch_util.new_mix_buffer = ags_stream_alloc(AGS_FX_FM_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
								      format);

	channel_data->hq_pitch_util.format = format;

	channel_data->chorus_hq_pitch_util.format = format;

	ags_stream_free(channel_data->chorus_hq_pitch_util.low_mix_buffer);
	ags_stream_free(channel_data->chorus_hq_pitch_util.new_mix_buffer);

	channel_data->chorus_hq_pitch_util.low_mix_buffer = ags_stream_alloc(AGS_FX_FM_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									     format);
	channel_data->chorus_hq_pitch_util.new_mix_buffer = ags_stream_alloc(AGS_FX_FM_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									     format);
	
	channel_data->chorus_linear_interpolate_util.format = format;

	ags_stream_free(channel_data->chorus_util.destination);
	ags_stream_free(channel_data->chorus_hq_pitch_util.destination);

	channel_data->chorus_util.destination = ags_stream_alloc(AGS_FX_FM_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
								 format);

	channel_data->chorus_hq_pitch_util.destination =
	  channel_data->chorus_util.pitch_mix_buffer = ags_stream_alloc(AGS_FX_FM_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									format);
	
	channel_data->chorus_util.format = format;
      }
    }
  }

  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_fm_synth_audio_notify_samplerate_callback(GObject *gobject,
						 GParamSpec *pspec,
						 gpointer user_data)
{
  AgsFxFMSynthAudio *fx_fm_synth_audio;

  guint samplerate;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_fm_synth_audio = AGS_FX_FM_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_fm_synth_audio);

  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  g_object_get(fx_fm_synth_audio,
	       "samplerate", &samplerate,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxFMSynthAudioScopeData *scope_data;

    scope_data = fx_fm_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxFMSynthAudioChannelData *channel_data;

	channel_data = scope_data->channel_data[j];

	channel_data->hq_pitch_linear_interpolate_util.samplerate = samplerate;

	channel_data->hq_pitch_util.samplerate = samplerate;

	channel_data->chorus_hq_pitch_util.samplerate = samplerate;

	channel_data->chorus_linear_interpolate_util.samplerate = samplerate;
	
	channel_data->chorus_util.samplerate = samplerate;
      }
    }
  }

  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_fm_synth_audio_set_audio_channels_callback(AgsAudio *audio,
						  guint audio_channels, guint audio_channels_old,
						  AgsFxFMSynthAudio *fx_fm_synth_audio)
{
  guint input_pads;
  guint output_port_count, input_port_count;
  guint buffer_size;
  guint format;
  guint samplerate;
  guint i, j, k;

  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_fm_synth_audio);

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
    AgsFxFMSynthAudioScopeData *scope_data;

    scope_data = fx_fm_synth_audio->scope_data[i];

    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      if(scope_data->audio_channels > audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxFMSynthAudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];
	
	  ags_fx_fm_synth_audio_channel_data_free(channel_data);
	}
      }
      
      if(scope_data->channel_data == NULL){
	scope_data->channel_data = (AgsFxFMSynthAudioChannelData **) g_malloc(audio_channels * sizeof(AgsFxFMSynthAudioChannelData *)); 
      }else{
	scope_data->channel_data = (AgsFxFMSynthAudioChannelData **) g_realloc(scope_data->channel_data,
									     audio_channels * sizeof(AgsFxFMSynthAudioChannelData *)); 
      }

      if(scope_data->audio_channels < audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxFMSynthAudioChannelData *channel_data;

	  channel_data =
	    scope_data->channel_data[j] = ags_fx_fm_synth_audio_channel_data_alloc();

	  channel_data->hq_pitch_linear_interpolate_util.buffer_length = buffer_size;
	  channel_data->hq_pitch_linear_interpolate_util.format = format;
	  channel_data->hq_pitch_linear_interpolate_util.samplerate = samplerate;

	  channel_data->hq_pitch_util.destination = ags_stream_alloc(AGS_FX_FM_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
								     format);

	  channel_data->hq_pitch_util.low_mix_buffer = ags_stream_alloc(AGS_FX_FM_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									format);
	  channel_data->hq_pitch_util.new_mix_buffer = ags_stream_alloc(AGS_FX_FM_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									format);

	  channel_data->hq_pitch_util.buffer_length = buffer_size;
	  channel_data->hq_pitch_util.format = format;
	  channel_data->hq_pitch_util.samplerate = samplerate;
	  
	  channel_data->chorus_hq_pitch_util.buffer_length = buffer_size;
	  channel_data->chorus_hq_pitch_util.format = format;
	  channel_data->chorus_hq_pitch_util.samplerate = samplerate;

	  channel_data->chorus_hq_pitch_util.low_mix_buffer = ags_stream_alloc(AGS_FX_FM_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									       format);
	  channel_data->chorus_hq_pitch_util.new_mix_buffer = ags_stream_alloc(AGS_FX_FM_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									       format);

	  channel_data->chorus_linear_interpolate_util.buffer_length = buffer_size;
	  channel_data->chorus_linear_interpolate_util.format = format;
	  channel_data->chorus_linear_interpolate_util.samplerate = samplerate;

	  channel_data->chorus_linear_interpolate_util.factor = 1.0;
	  
	  channel_data->chorus_util.destination = ags_stream_alloc(AGS_FX_FM_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
								   format);

	  channel_data->chorus_hq_pitch_util.destination =
	    channel_data->chorus_util.pitch_mix_buffer = ags_stream_alloc(AGS_FX_FM_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									  format);
	  
	  channel_data->chorus_util.buffer_length = buffer_size;
	  channel_data->chorus_util.format = format;
	  channel_data->chorus_util.samplerate = samplerate;

	  for(k = 0; k < AGS_SEQUENCER_MAX_MIDI_KEYS; k++){
	    AgsFxFMSynthAudioInputData *input_data;

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
 * ags_fx_fm_synth_audio_scope_data_alloc:
 * 
 * Allocate #AgsFxFMSynthAudioScopeData-struct
 * 
 * Returns: the new #AgsFxFMSynthAudioScopeData-struct
 * 
 * Since: 3.14.0
 */
AgsFxFMSynthAudioScopeData*
ags_fx_fm_synth_audio_scope_data_alloc()
{
  AgsFxFMSynthAudioScopeData *scope_data;

  scope_data = (AgsFxFMSynthAudioScopeData *) g_malloc(sizeof(AgsFxFMSynthAudioScopeData));

  g_rec_mutex_init(&(scope_data->strct_mutex));
  
  scope_data->parent = NULL;
  
  scope_data->audio_channels = 0;

  scope_data->channel_data = NULL;
  
  return(scope_data);
}

/**
 * ags_fx_fm_synth_audio_scope_data_free:
 * @scope_data: the #AgsFxFMSynthAudioScopeData-struct
 * 
 * Free @scope_data.
 * 
 * Since: 3.14.0
 */
void
ags_fx_fm_synth_audio_scope_data_free(AgsFxFMSynthAudioScopeData *scope_data)
{
  guint i;

  if(scope_data == NULL){
    return;
  }

  for(i = 0; i < scope_data->audio_channels; i++){
    ags_fx_fm_synth_audio_channel_data_free(scope_data->channel_data[i]);
  }

  g_free(scope_data);
}

/**
 * ags_fx_fm_synth_audio_channel_data_alloc:
 * 
 * Allocate #AgsFxFMSynthAudioChannelData-struct
 * 
 * Returns: the new #AgsFxFMSynthAudioChannelData-struct
 * 
 * Since: 3.14.0
 */
AgsFxFMSynthAudioChannelData*
ags_fx_fm_synth_audio_channel_data_alloc()
{
  AgsFxFMSynthAudioChannelData *channel_data;

  guint i;
  
  channel_data = (AgsFxFMSynthAudioChannelData *) g_malloc(sizeof(AgsFxFMSynthAudioChannelData));

  g_rec_mutex_init(&(channel_data->strct_mutex));

  channel_data->parent = NULL;

  /* noise util */
  channel_data->noise_util.source = NULL;
  channel_data->noise_util.source_stride = 1;
  
  channel_data->noise_util.destination = NULL;
  channel_data->noise_util.destination_stride = 1;
 
  channel_data->noise_util.buffer_length = 0;
  channel_data->noise_util.format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  channel_data->noise_util.samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  channel_data->noise_util.mode = AGS_NOISE_UTIL_PINK_NOISE;

  channel_data->noise_util.frequency = AGS_NOISE_UTIL_DEFAULT_FREQUENCY;
  
  channel_data->noise_util.volume = 0.0;

  /* HQ pitch util */
  channel_data->hq_pitch_linear_interpolate_util.source = NULL;
  channel_data->hq_pitch_linear_interpolate_util.source_stride = 1;
	  
  channel_data->hq_pitch_linear_interpolate_util.destination = NULL;
  channel_data->hq_pitch_linear_interpolate_util.destination_stride = 1;
	  
  channel_data->hq_pitch_linear_interpolate_util.buffer_length = 0;
  channel_data->hq_pitch_linear_interpolate_util.format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  channel_data->hq_pitch_linear_interpolate_util.samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  channel_data->hq_pitch_linear_interpolate_util.factor = 1.0;

  channel_data->hq_pitch_util.source = NULL;
  channel_data->hq_pitch_util.source_stride = 1;
	  
  channel_data->hq_pitch_util.destination = NULL;
  channel_data->hq_pitch_util.destination_stride = 1;
  
  channel_data->hq_pitch_util.low_mix_buffer = NULL;
  channel_data->hq_pitch_util.new_mix_buffer = NULL;

  channel_data->hq_pitch_util.buffer_length = 0;
  channel_data->hq_pitch_util.format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  channel_data->hq_pitch_util.samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  channel_data->hq_pitch_util.base_key = 0.0;
  channel_data->hq_pitch_util.tuning = 0.0;

  channel_data->hq_pitch_util.linear_interpolate_util = &(channel_data->hq_pitch_linear_interpolate_util);    

  /* chorus util */
  channel_data->chorus_linear_interpolate_util.source = NULL;
  channel_data->chorus_linear_interpolate_util.source_stride = 1;

  channel_data->chorus_linear_interpolate_util.destination = NULL;
  channel_data->chorus_linear_interpolate_util.destination_stride = 1;

  channel_data->chorus_linear_interpolate_util.buffer_length = 0;
  channel_data->chorus_linear_interpolate_util.format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  channel_data->chorus_linear_interpolate_util.samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  channel_data->chorus_linear_interpolate_util.factor = 1.0;
	  
  channel_data->chorus_hq_pitch_util.source = NULL;
  channel_data->chorus_hq_pitch_util.source_stride = 1;
	  
  channel_data->chorus_hq_pitch_util.destination = NULL;
  channel_data->chorus_hq_pitch_util.destination_stride = 1;

  channel_data->chorus_hq_pitch_util.low_mix_buffer = NULL;
  channel_data->chorus_hq_pitch_util.new_mix_buffer = NULL;
	  
  channel_data->chorus_hq_pitch_util.buffer_length = 0;
  channel_data->chorus_hq_pitch_util.format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  channel_data->chorus_hq_pitch_util.samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  channel_data->chorus_hq_pitch_util.base_key = 0.0;
  channel_data->chorus_hq_pitch_util.tuning = 0.0;

  channel_data->chorus_hq_pitch_util.linear_interpolate_util = &(channel_data->chorus_linear_interpolate_util);

  channel_data->chorus_util.source = NULL;
  channel_data->chorus_util.source_stride = 1;
	  
  channel_data->chorus_util.destination = NULL;
  channel_data->chorus_util.destination_stride = 1;

  channel_data->chorus_util.buffer_length = 0;
  channel_data->chorus_util.format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  channel_data->chorus_util.samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  channel_data->chorus_util.pitch_mix_buffer = NULL;

  channel_data->chorus_util.hq_pitch_util = &(channel_data->chorus_hq_pitch_util);

  /* low pass filter util */
  channel_data->low_pass_filter.filter_type = AGS_FLUID_IIR_LOWPASS;
	  
  /* high pass filter util */
  channel_data->high_pass_filter.filter_type = AGS_FLUID_IIR_HIGHPASS;
  
  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    channel_data->input_data[i] = ags_fx_fm_synth_audio_input_data_alloc();

    channel_data->input_data[i]->parent = channel_data;
  }

  return(channel_data);
}

/**
 * ags_fx_fm_synth_audio_channel_data_free:
 * @channel_data: the #AgsFxFMSynthAudioChannelData-struct
 * 
 * Free @channel_data.
 * 
 * Since: 3.14.0
 */
void
ags_fx_fm_synth_audio_channel_data_free(AgsFxFMSynthAudioChannelData *channel_data)
{
  guint i;

  if(channel_data == NULL){
    return;
  }
  
  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    ags_fx_fm_synth_audio_input_data_free(channel_data->input_data[i]);
  }

  g_free(channel_data);
}

/**
 * ags_fx_fm_synth_audio_input_data_alloc:
 * 
 * Allocate #AgsFxFMSynthAudioInputData-struct
 * 
 * Returns: the new #AgsFxFMSynthAudioInputData-struct
 * 
 * Since: 3.14.0
 */
AgsFxFMSynthAudioInputData*
ags_fx_fm_synth_audio_input_data_alloc()
{
  AgsFxFMSynthAudioInputData *input_data;

  input_data = (AgsFxFMSynthAudioInputData *) g_malloc(sizeof(AgsFxFMSynthAudioInputData));

  g_rec_mutex_init(&(input_data->strct_mutex));

  input_data->parent = NULL;

  input_data->key_on = 0;
  
  return(input_data);
}

/**
 * ags_fx_fm_synth_audio_input_data_free:
 * @input_data: the #AgsFxFMSynthAudioInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 3.14.0
 */
void
ags_fx_fm_synth_audio_input_data_free(AgsFxFMSynthAudioInputData *input_data)
{
  if(input_data == NULL){
    return;
  }

  g_free(input_data);
}

static AgsPluginPort*
ags_fx_fm_synth_audio_get_synth_0_oscillator_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_0_octave_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_0_key_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_0_phase_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_0_volume_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_0_lfo_oscillator_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_0_lfo_frequency_plugin_port()
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
		      0.01);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      12.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_fm_synth_audio_get_synth_0_lfo_depth_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_0_lfo_tuning_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_1_oscillator_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_1_octave_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_1_key_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_1_phase_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_1_volume_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_1_lfo_oscillator_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_1_lfo_frequency_plugin_port()
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
		      0.01);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      12.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_fm_synth_audio_get_synth_1_lfo_depth_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_1_lfo_tuning_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_2_oscillator_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_2_octave_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_2_key_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_2_phase_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_2_volume_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_2_lfo_oscillator_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_2_lfo_frequency_plugin_port()
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
		      0.01);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      12.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_fm_synth_audio_get_synth_2_lfo_depth_plugin_port()
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
ags_fx_fm_synth_audio_get_synth_2_lfo_tuning_plugin_port()
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
ags_fx_fm_synth_audio_get_sequencer_enabled_plugin_port()
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
		      10.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_fm_synth_audio_get_sequencer_sign_plugin_port()
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
		      12.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_fm_synth_audio_get_noise_gain_plugin_port()
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
ags_fx_fm_synth_audio_get_pitch_tuning_plugin_port()
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
ags_fx_fm_synth_audio_get_chorus_enabled_plugin_port()
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
ags_fx_fm_synth_audio_get_chorus_input_volume_plugin_port()
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
ags_fx_fm_synth_audio_get_chorus_output_volume_plugin_port()
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
ags_fx_fm_synth_audio_get_chorus_lfo_oscillator_plugin_port()
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
ags_fx_fm_synth_audio_get_chorus_lfo_frequency_plugin_port()
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
ags_fx_fm_synth_audio_get_chorus_depth_plugin_port()
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
ags_fx_fm_synth_audio_get_chorus_mix_plugin_port()
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
ags_fx_fm_synth_audio_get_chorus_delay_plugin_port()
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
ags_fx_fm_synth_audio_get_low_pass_enabled_plugin_port()
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
ags_fx_fm_synth_audio_get_low_pass_q_lin_plugin_port()
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
ags_fx_fm_synth_audio_get_low_pass_filter_gain_plugin_port()
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
ags_fx_fm_synth_audio_get_high_pass_enabled_plugin_port()
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
ags_fx_fm_synth_audio_get_high_pass_q_lin_plugin_port()
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
ags_fx_fm_synth_audio_get_high_pass_filter_gain_plugin_port()
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
 * ags_fx_fm_synth_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxFMSynthAudio
 *
 * Returns: the new #AgsFxFMSynthAudio
 *
 * Since: 3.14.0
 */
AgsFxFMSynthAudio*
ags_fx_fm_synth_audio_new(AgsAudio *audio)
{
  AgsFxFMSynthAudio *fx_fm_synth_audio;

  fx_fm_synth_audio = (AgsFxFMSynthAudio *) g_object_new(AGS_TYPE_FX_FM_SYNTH_AUDIO,
							 "audio", audio,
							 NULL);

  return(fx_fm_synth_audio);
}
