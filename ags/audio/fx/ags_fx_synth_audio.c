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

#include <ags/audio/fx/ags_fx_synth_audio.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_fx_synth_audio_class_init(AgsFxSynthAudioClass *fx_synth_audio);
void ags_fx_synth_audio_init(AgsFxSynthAudio *fx_synth_audio);
void ags_fx_synth_audio_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_fx_synth_audio_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_fx_synth_audio_dispose(GObject *gobject);
void ags_fx_synth_audio_finalize(GObject *gobject);

static AgsPluginPort* ags_fx_synth_audio_get_synth_0_oscillator_mode_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_octave_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_key_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_phase_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_volume_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_sync_enabled_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_sync_relative_attack_factor_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_sync_attack_0_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_sync_phase_0_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_sync_attack_1_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_sync_phase_1_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_sync_attack_2_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_sync_phase_2_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_sync_lfo_oscillator_mode_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_0_sync_lfo_oscillator_frequency_plugin_port();

static AgsPluginPort* ags_fx_synth_audio_get_synth_1_oscillator_mode_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_octave_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_key_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_phase_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_volume_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_sync_enabled_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_sync_relative_attack_factor_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_sync_attack_0_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_sync_phase_0_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_sync_attack_1_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_sync_phase_1_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_sync_attack_2_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_sync_phase_2_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_sync_lfo_oscillator_mode_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_synth_1_sync_lfo_oscillator_frequency_plugin_port();

static AgsPluginPort* ags_fx_synth_audio_get_sequencer_enabled_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_sequencer_sign_plugin_port();

static AgsPluginPort* ags_fx_synth_audio_get_pitch_tuning_plugin_port();

static AgsPluginPort* ags_fx_synth_audio_get_noise_gain_plugin_port();

static AgsPluginPort* ags_fx_synth_audio_get_chorus_enabled_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_chorus_input_volume_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_chorus_output_volume_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_chorus_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_chorus_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_chorus_depth_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_chorus_mix_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_chorus_delay_plugin_port();

static AgsPluginPort* ags_fx_synth_audio_get_low_pass_enabled__plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_low_pass_q_lin_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_low_pass_filter_gain_plugin_port();

static AgsPluginPort* ags_fx_synth_audio_get_high_pass_enabled__plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_high_pass_q_lin_plugin_port();
static AgsPluginPort* ags_fx_synth_audio_get_high_pass_filter_gain_plugin_port();

/**
 * SECTION:ags_fx_synth_audio
 * @short_description: fx synth audio
 * @title: AgsFxSynthAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_synth_audio.h
 *
 * The #AgsFxSynthAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_synth_audio_parent_class = NULL;

const gchar *ags_fx_synth_audio_plugin_name = "ags-fx-synth";

const gchar* ags_fx_synth_audio_specifier[] = {
  "./synth-0-oscillator[0]",
  "./synth-0-octave[0]",
  "./synth-0-key[0]",
  "./synth-0-phase[0]",
  "./synth-0-volume[0]",
  "./synth-0-sync-enabled[0]",
  "./synth-0-sync-relative-factor[0]",
  "./synth-0-sync-attack-0[0]",
  "./synth-0-sync-phase-0[0]",
  "./synth-0-sync-attack-1[0]",
  "./synth-0-sync-phase-1[0]",
  "./synth-0-sync-attack-2[0]",
  "./synth-0-sync-phase-2[0]",
  "./synth-0-sync-lfo-oscillator[0]",
  "./synth-0-sync-lfo-frequency[0]",
  "./synth-1-oscillator[0]",
  "./synth-1-octave[0]",
  "./synth-1-key[0]",
  "./synth-1-phase[0]",
  "./synth-1-volume[0]",
  "./synth-1-sync-enabled[0]",
  "./synth-1-sync-relative-factor[0]",
  "./synth-1-sync-attack-0[0]",
  "./synth-1-sync-phase-0[0]",
  "./synth-1-sync-attack-1[0]",
  "./synth-1-sync-phase-1[0]",
  "./synth-1-sync-attack-2[0]",
  "./synth-1-sync-phase-2[0]",
  "./synth-1-sync-lfo-oscillator[0]",
  "./synth-1-sync-lfo-frequency[0]",
  "./sequencer-enabled[0]",
  "./sequencer-sign[0]",
  "./pitch-tuning[0]",
  "./noise-gain[0]",
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

const gchar* ags_fx_synth_audio_control_port[] = {
  "1/48",
  "2/48",
  "3/48",
  "4/48",
  "5/48",
  "6/48",
  "7/48",
  "8/48",
  "9/48",
  "10/48",
  "11/48",
  "12/48",
  "13/48",
  "14/48",
  "15/48",
  "16/48",
  "17/48",
  "18/48",
  "19/48",
  "20/48",
  "21/48",
  "22/48",
  "23/48",
  "24/48",
  "25/48",
  "26/48",
  "27/48",
  "28/48",
  "29/48",
  "30/48",
  "31/48",
  "32/48",
  "33/48",
  "34/48",
  "35/48",
  "36/48",
  "37/48",
  "38/48",
  "39/48",
  "40/48",
  "41/48",
  "42/48",
  "43/48",
  "44/48",
  "45/48",
  "46/48",
  "47/48",
  "48/48",
  NULL,
};

enum{
  PROP_0,
  PROP_SYNTH_O_OSCILLATOR_MODE,
  PROP_SYNTH_O_OCTAVE,
  PROP_SYNTH_O_KEY,
  PROP_SYNTH_O_VOLUME,
  PROP_SYNTH_O_PHASE,
  PROP_SYNTH_O_SYNC_ENABLED,
  PROP_SYNTH_O_SYNC_RELATIVE_ATTACK_FACTOR,
  PROP_SYNTH_O_SYNC_ATTACK_0,
  PROP_SYNTH_O_SYNC_PHASE_0,
  PROP_SYNTH_O_SYNC_ATTACK_1,
  PROP_SYNTH_O_SYNC_PHASE_1,
  PROP_SYNTH_O_SYNC_ATTACK_2,
  PROP_SYNTH_O_SYNC_PHASE_2,
  PROP_SYNTH_O_SYNC_LFO_OSCILLATOR_MODE,
  PROP_SYNTH_O_SYNC_LFO_OSCILLATOR_FREQUENCY,
  PROP_SYNTH_1_OSCILLATOR_MODE,
  PROP_SYNTH_1_OCTAVE,
  PROP_SYNTH_1_KEY,
  PROP_SYNTH_1_VOLUME,
  PROP_SYNTH_1_PHASE,
  PROP_SYNTH_1_SYNC_ENABLED,
  PROP_SYNTH_1_SYNC_RELATIVE_ATTACK_FACTOR,
  PROP_SYNTH_1_SYNC_ATTACK_0,
  PROP_SYNTH_1_SYNC_PHASE_0,
  PROP_SYNTH_1_SYNC_ATTACK_1,
  PROP_SYNTH_1_SYNC_PHASE_1,
  PROP_SYNTH_1_SYNC_ATTACK_2,
  PROP_SYNTH_1_SYNC_PHASE_2,
  PROP_SYNTH_1_SYNC_LFO_OSCILLATOR_MODE,
  PROP_SYNTH_1_SYNC_LFO_OSCILLATOR_FREQUENCY,
  PROP_SEQUENCER_ENABLED,
  PROP_SEQUENCER_SIGN,
  PROP_PITCH_TUNING,
  PROP_NOISE_GAIN,
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
ags_fx_synth_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_synth_audio = 0;

    static const GTypeInfo ags_fx_synth_audio_info = {
      sizeof (AgsFxSynthAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_synth_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxSynthAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_synth_audio_init,
    };

    ags_type_fx_synth_audio = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO,
						     "AgsFxSynthAudio",
						     &ags_fx_synth_audio_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_synth_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_synth_audio_class_init(AgsFxSynthAudioClass *fx_synth_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_synth_audio_parent_class = g_type_class_peek_parent(fx_synth_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_synth_audio;

  gobject->set_property = ags_fx_synth_audio_set_property;
  gobject->get_property = ags_fx_synth_audio_get_property;

  gobject->dispose = ags_fx_synth_audio_dispose;
  gobject->finalize = ags_fx_synth_audio_finalize;

  /* properties */
  /**
   * AgsFxSynthAudio:synth-0-oscillator:
   *
   * The synth-0 oscillator.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:synth-0-octave:
   *
   * The synth-0 octave.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:synth-0-key:
   *
   * The synth-0 key.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:synth-0-phase:
   *
   * The synth-0 phase.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:synth-0-volume:
   *
   * The synth-0 volume.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:synth-0-sync-enabled:
   *
   * The synth-0 sync enabled.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:synth-0-sync-relative-attack-factor:
   *
   * The synth-0 sync relative attack factor.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:synth-0-sync-attack-0:
   *
   * The synth-0 sync attack-0.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:synth-0-sync-phase-0:
   *
   * The synth-0 sync phase-0.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:synth-0-sync-attack-1:
   *
   * The synth-0 sync attack-1.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:synth-0-sync-phase-1:
   *
   * The synth-0 sync phase-1.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:synth-0-sync-attack-2:
   *
   * The synth-0 sync attack-2.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:synth-0-sync-phase-2:
   *
   * The synth-0 sync phase-2.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:synth-0-sync-lfo-oscillator:
   *
   * The synth-0 sync LFO oscillator.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:synth-0-sync-lfo-frequency:
   *
   * The synth-0 sync LFO frequency.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:sequencer-enabled:
   *
   * The sequencer enabled.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:sequencer-sign:
   *
   * The sequencer sign.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:pitch-tuning:
   *
   * The pitch tuning.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:noise-gain:
   *
   * The noise gain.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:chorus-enabled:
   *
   * The chorus enabled.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:chorus-input-volume:
   *
   * The chorus input volume.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:chorus-output-volume:
   *
   * The chorus output volume.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:chorus-lfo-oscillator:
   *
   * The chorus lfo oscillator.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:chorus-lfo-frequency:
   *
   * The chorus lfo frequency.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:chorus-depth:
   *
   * The chorus depth.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:chorus-mix:
   *
   * The chorus mix.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:chorus-delay:
   *
   * The chorus delay.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:low-pass-enabled:
   *
   * The low pass enabled.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:low-pass-q-lin:
   *
   * The low pass q-lin.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:low-pass-filter-gain:
   *
   * The low pass filter gain.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:high-pass-enabled:
   *
   * The high pass enabled.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:high-pass-q-lin:
   *
   * The high pass q-lin.
   * 
   * Since: 3.14.0
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
   * AgsFxSynthAudio:high-pass-filter-gain:
   *
   * The high pass filter gain.
   * 
   * Since: 3.14.0
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
ags_fx_synth_audio_init(AgsFxSynthAudio *fx_synth_audio)
{
  AGS_RECALL(fx_synth_audio)->name = "ags-fx-synth";
  AGS_RECALL(fx_synth_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_synth_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_synth_audio)->xml_type = "ags-fx-synth-audio";

  /* synth-0 oscillator */
  fx_synth_audio->synth_0_oscillator = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_synth_audio_plugin_name,
						    "specifier", ags_fx_synth_audio_specifier[0],
						    "control-port", ags_fx_synth_audio_control_port[0],
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_synth_audio->synth_0_oscillator->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_0_oscillator,
	       "plugin-port", ags_fx_synth_audio_get_synth_0_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_0_oscillator);

  /* synth-0 octave */
  fx_synth_audio->synth_0_octave = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_synth_audio_plugin_name,
						"specifier", ags_fx_synth_audio_specifier[1],
						"control-port", ags_fx_synth_audio_control_port[1],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 1,
						NULL);
  
  fx_synth_audio->synth_0_octave->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_0_octave,
	       "plugin-port", ags_fx_synth_audio_get_synth_0_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_0_octave);

  /* synth-0 key */
  fx_synth_audio->synth_0_key = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_fx_synth_audio_plugin_name,
					     "specifier", ags_fx_synth_audio_specifier[2],
					     "control-port", ags_fx_synth_audio_control_port[2],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_FLOAT,
					     "port-value-size", sizeof(gfloat),
					     "port-value-length", 1,
					     NULL);
  
  fx_synth_audio->synth_0_key->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_0_key,
	       "plugin-port", ags_fx_synth_audio_get_synth_0_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_0_key);

  /* synth-0 phase */
  fx_synth_audio->synth_0_phase = g_object_new(AGS_TYPE_PORT,
					       "plugin-name", ags_fx_synth_audio_plugin_name,
					       "specifier", ags_fx_synth_audio_specifier[3],
					       "control-port", ags_fx_synth_audio_control_port[3],
					       "port-value-is-pointer", FALSE,
					       "port-value-type", G_TYPE_FLOAT,
					       "port-value-size", sizeof(gfloat),
					       "port-value-length", 1,
					       NULL);
  
  fx_synth_audio->synth_0_phase->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_0_phase,
	       "plugin-port", ags_fx_synth_audio_get_synth_0_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_0_phase);

  /* synth-0 volume */
  fx_synth_audio->synth_0_volume = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_synth_audio_plugin_name,
						"specifier", ags_fx_synth_audio_specifier[4],
						"control-port", ags_fx_synth_audio_control_port[4],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 1,
						NULL);
  
  fx_synth_audio->synth_0_volume->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_0_volume,
	       "plugin-port", ags_fx_synth_audio_get_synth_0_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_0_volume);

  /* synth-0 sync enabled */
  fx_synth_audio->synth_0_sync_enabled = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_synth_audio_plugin_name,
						      "specifier", ags_fx_synth_audio_specifier[5],
						      "control-port", ags_fx_synth_audio_control_port[5],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_synth_audio->synth_0_sync_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_0_sync_enabled,
	       "plugin-port", ags_fx_synth_audio_get_synth_0_sync_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_0_sync_enabled);

  /* synth-0 sync relative attack factor */
  fx_synth_audio->synth_0_sync_relative_attack_factor = g_object_new(AGS_TYPE_PORT,
								     "plugin-name", ags_fx_synth_audio_plugin_name,
								     "specifier", ags_fx_synth_audio_specifier[6],
								     "control-port", ags_fx_synth_audio_control_port[6],
								     "port-value-is-pointer", FALSE,
								     "port-value-type", G_TYPE_FLOAT,
								     "port-value-size", sizeof(gfloat),
								     "port-value-length", 1,
								     NULL);
  
  fx_synth_audio->synth_0_sync_relative_attack_factor->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_0_sync_relative_attack_factor,
	       "plugin-port", ags_fx_synth_audio_get_synth_0_sync_relative_attack_factor_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_0_sync_relative_attack_factor);

  /* synth-0 sync attack-0 */
  fx_synth_audio->synth_0_sync_attack_0 = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_synth_audio_plugin_name,
						       "specifier", ags_fx_synth_audio_specifier[7],
						       "control-port", ags_fx_synth_audio_control_port[7],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_synth_audio->synth_0_sync_attack_0->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_0_sync_attack_0,
	       "plugin-port", ags_fx_synth_audio_get_synth_0_sync_attack_0_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_0_sync_attack_0);

  /* synth-0 sync phase-0 */
  fx_synth_audio->synth_0_sync_phase_0 = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_synth_audio_plugin_name,
						      "specifier", ags_fx_synth_audio_specifier[8],
						      "control-port", ags_fx_synth_audio_control_port[8],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_synth_audio->synth_0_sync_phase_0->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_0_sync_phase_0,
	       "plugin-port", ags_fx_synth_audio_get_synth_0_sync_phase_0_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_0_sync_phase_0);

  /* synth-0 sync attack-1 */
  fx_synth_audio->synth_0_sync_attack_1 = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_synth_audio_plugin_name,
						       "specifier", ags_fx_synth_audio_specifier[9],
						       "control-port", ags_fx_synth_audio_control_port[9],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_synth_audio->synth_0_sync_attack_1->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_0_sync_attack_1,
	       "plugin-port", ags_fx_synth_audio_get_synth_0_sync_attack_1_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_0_sync_attack_1);

  /* synth-0 sync phase-1 */
  fx_synth_audio->synth_0_sync_phase_1 = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_synth_audio_plugin_name,
						      "specifier", ags_fx_synth_audio_specifier[10],
						      "control-port", ags_fx_synth_audio_control_port[10],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_synth_audio->synth_0_sync_phase_1->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_0_sync_phase_1,
	       "plugin-port", ags_fx_synth_audio_get_synth_0_sync_phase_1_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_0_sync_phase_1);


  /* synth-0 sync attack-2 */
  fx_synth_audio->synth_0_sync_attack_2 = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_synth_audio_plugin_name,
						       "specifier", ags_fx_synth_audio_specifier[11],
						       "control-port", ags_fx_synth_audio_control_port[11],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 2,
						       NULL);
  
  fx_synth_audio->synth_0_sync_attack_2->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_0_sync_attack_2,
	       "plugin-port", ags_fx_synth_audio_get_synth_0_sync_attack_2_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_0_sync_attack_2);

  /* synth-0 sync phase-2 */
  fx_synth_audio->synth_0_sync_phase_2 = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_synth_audio_plugin_name,
						      "specifier", ags_fx_synth_audio_specifier[12],
						      "control-port", ags_fx_synth_audio_control_port[12],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 2,
						      NULL);
  
  fx_synth_audio->synth_0_sync_phase_2->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_0_sync_phase_2,
	       "plugin-port", ags_fx_synth_audio_get_synth_0_sync_phase_2_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_0_sync_phase_2);

  /* synth-0 sync LFO oscillator */
  fx_synth_audio->synth_0_sync_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_synth_audio_plugin_name,
							     "specifier", ags_fx_synth_audio_specifier[13],
							     "control-port", ags_fx_synth_audio_control_port[13],
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 2,
							     NULL);
  
  fx_synth_audio->synth_0_sync_lfo_oscillator->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_0_sync_lfo_oscillator,
	       "plugin-port", ags_fx_synth_audio_get_synth_0_sync_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_0_sync_lfo_oscillator);

  /* synth-0 sync LFO frequency */
  fx_synth_audio->synth_0_sync_lfo_frequency = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_synth_audio_plugin_name,
							    "specifier", ags_fx_synth_audio_specifier[14],
							    "control-port", ags_fx_synth_audio_control_port[14],
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 2,
							    NULL);
  
  fx_synth_audio->synth_0_sync_lfo_frequency->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_0_sync_lfo_frequency,
	       "plugin-port", ags_fx_synth_audio_get_synth_0_sync_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_0_sync_lfo_frequency);

  /* synth-1 oscillator */
  fx_synth_audio->synth_1_oscillator = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_synth_audio_plugin_name,
						    "specifier", ags_fx_synth_audio_specifier[15],
						    "control-port", ags_fx_synth_audio_control_port[15],
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_synth_audio->synth_1_oscillator->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_1_oscillator,
	       "plugin-port", ags_fx_synth_audio_get_synth_1_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_1_oscillator);

  /* synth-1 octave */
  fx_synth_audio->synth_1_octave = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_synth_audio_plugin_name,
						"specifier", ags_fx_synth_audio_specifier[16],
						"control-port", ags_fx_synth_audio_control_port[16],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 1,
						NULL);
  
  fx_synth_audio->synth_1_octave->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_1_octave,
	       "plugin-port", ags_fx_synth_audio_get_synth_1_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_1_octave);

  /* synth-1 key */
  fx_synth_audio->synth_1_key = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_fx_synth_audio_plugin_name,
					     "specifier", ags_fx_synth_audio_specifier[17],
					     "control-port", ags_fx_synth_audio_control_port[17],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_FLOAT,
					     "port-value-size", sizeof(gfloat),
					     "port-value-length", 1,
					     NULL);
  
  fx_synth_audio->synth_1_key->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_1_key,
	       "plugin-port", ags_fx_synth_audio_get_synth_1_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_1_key);

  /* synth-1 phase */
  fx_synth_audio->synth_1_phase = g_object_new(AGS_TYPE_PORT,
					       "plugin-name", ags_fx_synth_audio_plugin_name,
					       "specifier", ags_fx_synth_audio_specifier[18],
					       "control-port", ags_fx_synth_audio_control_port[18],
					       "port-value-is-pointer", FALSE,
					       "port-value-type", G_TYPE_FLOAT,
					       "port-value-size", sizeof(gfloat),
					       "port-value-length", 1,
					       NULL);
  
  fx_synth_audio->synth_1_phase->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_1_phase,
	       "plugin-port", ags_fx_synth_audio_get_synth_1_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_1_phase);

  /* synth-1 volume */
  fx_synth_audio->synth_1_volume = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_synth_audio_plugin_name,
						"specifier", ags_fx_synth_audio_specifier[19],
						"control-port", ags_fx_synth_audio_control_port[19],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 1,
						NULL);
  
  fx_synth_audio->synth_1_volume->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_1_volume,
	       "plugin-port", ags_fx_synth_audio_get_synth_1_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_1_volume);

  /* synth-1 sync enabled */
  fx_synth_audio->synth_1_sync_enabled = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_synth_audio_plugin_name,
						      "specifier", ags_fx_synth_audio_specifier[20],
						      "control-port", ags_fx_synth_audio_control_port[20],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_synth_audio->synth_1_sync_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_1_sync_enabled,
	       "plugin-port", ags_fx_synth_audio_get_synth_1_sync_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_1_sync_enabled);

  /* synth-1 sync relative attack factor */
  fx_synth_audio->synth_1_sync_relative_attack_factor = g_object_new(AGS_TYPE_PORT,
								     "plugin-name", ags_fx_synth_audio_plugin_name,
								     "specifier", ags_fx_synth_audio_specifier[21],
								     "control-port", ags_fx_synth_audio_control_port[21],
								     "port-value-is-pointer", FALSE,
								     "port-value-type", G_TYPE_FLOAT,
								     "port-value-size", sizeof(gfloat),
								     "port-value-length", 1,
								     NULL);
  
  fx_synth_audio->synth_1_sync_relative_attack_factor->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_1_sync_relative_attack_factor,
	       "plugin-port", ags_fx_synth_audio_get_synth_1_sync_relative_attack_factor_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_1_sync_relative_attack_factor);

  /* synth-1 sync attack-0 */
  fx_synth_audio->synth_1_sync_attack_0 = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_synth_audio_plugin_name,
						       "specifier", ags_fx_synth_audio_specifier[22],
						       "control-port", ags_fx_synth_audio_control_port[22],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_synth_audio->synth_1_sync_attack_0->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_1_sync_attack_0,
	       "plugin-port", ags_fx_synth_audio_get_synth_1_sync_attack_0_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_1_sync_attack_0);

  /* synth-1 sync phase-0 */
  fx_synth_audio->synth_1_sync_phase_0 = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_synth_audio_plugin_name,
						      "specifier", ags_fx_synth_audio_specifier[23],
						      "control-port", ags_fx_synth_audio_control_port[23],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_synth_audio->synth_1_sync_phase_0->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_1_sync_phase_0,
	       "plugin-port", ags_fx_synth_audio_get_synth_1_sync_phase_0_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_1_sync_phase_0);

  /* synth-1 sync attack-1 */
  fx_synth_audio->synth_1_sync_attack_1 = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_synth_audio_plugin_name,
						       "specifier", ags_fx_synth_audio_specifier[24],
						       "control-port", ags_fx_synth_audio_control_port[24],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_synth_audio->synth_1_sync_attack_1->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_1_sync_attack_1,
	       "plugin-port", ags_fx_synth_audio_get_synth_1_sync_attack_1_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_1_sync_attack_1);

  /* synth-1 sync phase-1 */
  fx_synth_audio->synth_1_sync_phase_1 = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_synth_audio_plugin_name,
						      "specifier", ags_fx_synth_audio_specifier[25],
						      "control-port", ags_fx_synth_audio_control_port[25],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_synth_audio->synth_1_sync_phase_1->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_1_sync_phase_1,
	       "plugin-port", ags_fx_synth_audio_get_synth_1_sync_phase_1_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_1_sync_phase_1);


  /* synth-1 sync attack-2 */
  fx_synth_audio->synth_1_sync_attack_2 = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_synth_audio_plugin_name,
						       "specifier", ags_fx_synth_audio_specifier[26],
						       "control-port", ags_fx_synth_audio_control_port[26],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 2,
						       NULL);
  
  fx_synth_audio->synth_1_sync_attack_2->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_1_sync_attack_2,
	       "plugin-port", ags_fx_synth_audio_get_synth_1_sync_attack_2_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_1_sync_attack_2);

  /* synth-1 sync phase-2 */
  fx_synth_audio->synth_1_sync_phase_2 = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_synth_audio_plugin_name,
						      "specifier", ags_fx_synth_audio_specifier[27],
						      "control-port", ags_fx_synth_audio_control_port[27],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 2,
						      NULL);
  
  fx_synth_audio->synth_1_sync_phase_2->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_1_sync_phase_2,
	       "plugin-port", ags_fx_synth_audio_get_synth_1_sync_phase_2_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_1_sync_phase_2);

  /* synth-1 sync LFO oscillator */
  fx_synth_audio->synth_1_sync_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_synth_audio_plugin_name,
							     "specifier", ags_fx_synth_audio_specifier[28],
							     "control-port", ags_fx_synth_audio_control_port[28],
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 2,
							     NULL);
  
  fx_synth_audio->synth_1_sync_lfo_oscillator->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_1_sync_lfo_oscillator,
	       "plugin-port", ags_fx_synth_audio_get_synth_1_sync_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_1_sync_lfo_oscillator);

  /* synth-1 sync LFO frequency */
  fx_synth_audio->synth_1_sync_lfo_frequency = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_synth_audio_plugin_name,
							    "specifier", ags_fx_synth_audio_specifier[29],
							    "control-port", ags_fx_synth_audio_control_port[29],
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 2,
							    NULL);
  
  fx_synth_audio->synth_1_sync_lfo_frequency->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->synth_1_sync_lfo_frequency,
	       "plugin-port", ags_fx_synth_audio_get_synth_1_sync_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->synth_1_sync_lfo_frequency);

  /* sequencer enabled */
  fx_synth_audio->sequencer_enabled = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_synth_audio_plugin_name,
						   "specifier", ags_fx_synth_audio_specifier[30],
						   "control-port", ags_fx_synth_audio_control_port[30],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 2,
						   NULL);
  
  fx_synth_audio->sequencer_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->sequencer_enabled,
	       "plugin-port", ags_fx_synth_audio_get_sequencer_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->sequencer_enabled);

  /* sequencer sign */
  fx_synth_audio->sequencer_sign = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_synth_audio_plugin_name,
						"specifier", ags_fx_synth_audio_specifier[31],
						"control-port", ags_fx_synth_audio_control_port[31],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 2,
						NULL);
  
  fx_synth_audio->sequencer_sign->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->sequencer_sign,
	       "plugin-port", ags_fx_synth_audio_get_sequencer_sign_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->sequencer_sign);

  /* pitch tuning */
  fx_synth_audio->pitch_tuning = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_fx_synth_audio_plugin_name,
					      "specifier", ags_fx_synth_audio_specifier[32],
					      "control-port", ags_fx_synth_audio_control_port[32],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_FLOAT,
					      "port-value-size", sizeof(gfloat),
					      "port-value-length", 2,
					      NULL);
  
  fx_synth_audio->pitch_tuning->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->pitch_tuning,
	       "plugin-port", ags_fx_synth_audio_get_pitch_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->pitch_tuning);

  /* noise gain */
  fx_synth_audio->noise_gain = g_object_new(AGS_TYPE_PORT,
					    "plugin-name", ags_fx_synth_audio_plugin_name,
					    "specifier", ags_fx_synth_audio_specifier[33],
					    "control-port", ags_fx_synth_audio_control_port[33],
					    "port-value-is-pointer", FALSE,
					    "port-value-type", G_TYPE_FLOAT,
					    "port-value-size", sizeof(gfloat),
					    "port-value-length", 2,
					    NULL);
  
  fx_synth_audio->noise_gain->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->noise_gain,
	       "plugin-port", ags_fx_synth_audio_get_noise_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->noise_gain);

  /* chorus enabled */
  fx_synth_audio->chorus_enabled = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_synth_audio_plugin_name,
						"specifier", ags_fx_synth_audio_specifier[34],
						"control-port", ags_fx_synth_audio_control_port[34],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 2,
						NULL);
  
  fx_synth_audio->chorus_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->chorus_enabled,
	       "plugin-port", ags_fx_synth_audio_get_chorus_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->chorus_enabled);

  /* chorus input volume */
  fx_synth_audio->chorus_input_volume = g_object_new(AGS_TYPE_PORT,
						     "plugin-name", ags_fx_synth_audio_plugin_name,
						     "specifier", ags_fx_synth_audio_specifier[35],
						     "control-port", ags_fx_synth_audio_control_port[35],
						     "port-value-is-pointer", FALSE,
						     "port-value-type", G_TYPE_FLOAT,
						     "port-value-size", sizeof(gfloat),
						     "port-value-length", 2,
						     NULL);
  
  fx_synth_audio->chorus_input_volume->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->chorus_input_volume,
	       "plugin-port", ags_fx_synth_audio_get_chorus_input_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->chorus_input_volume);

  /* chorus output volume */
  fx_synth_audio->chorus_output_volume = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_synth_audio_plugin_name,
						      "specifier", ags_fx_synth_audio_specifier[36],
						      "control-port", ags_fx_synth_audio_control_port[36],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 2,
						      NULL);
  
  fx_synth_audio->chorus_output_volume->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->chorus_output_volume,
	       "plugin-port", ags_fx_synth_audio_get_chorus_output_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->chorus_output_volume);

  /* chorus LFO oscillator */
  fx_synth_audio->chorus_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_synth_audio_plugin_name,
						       "specifier", ags_fx_synth_audio_specifier[37],
						       "control-port", ags_fx_synth_audio_control_port[37],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 2,
						       NULL);
  
  fx_synth_audio->chorus_lfo_oscillator->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->chorus_lfo_oscillator,
	       "plugin-port", ags_fx_synth_audio_get_chorus_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->chorus_lfo_oscillator);

  /* chorus LFO frequency */
  fx_synth_audio->chorus_lfo_frequency = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_synth_audio_plugin_name,
						      "specifier", ags_fx_synth_audio_specifier[38],
						      "control-port", ags_fx_synth_audio_control_port[38],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 2,
						      NULL);
  
  fx_synth_audio->chorus_lfo_frequency->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->chorus_lfo_frequency,
	       "plugin-port", ags_fx_synth_audio_get_chorus_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->chorus_lfo_frequency);

  /* chorus depth */
  fx_synth_audio->chorus_depth = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_fx_synth_audio_plugin_name,
					      "specifier", ags_fx_synth_audio_specifier[39],
					      "control-port", ags_fx_synth_audio_control_port[39],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_FLOAT,
					      "port-value-size", sizeof(gfloat),
					      "port-value-length", 2,
					      NULL);
  
  fx_synth_audio->chorus_depth->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->chorus_depth,
	       "plugin-port", ags_fx_synth_audio_get_chorus_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->chorus_depth);

  /* chorus mix */
  fx_synth_audio->chorus_mix = g_object_new(AGS_TYPE_PORT,
					    "plugin-name", ags_fx_synth_audio_plugin_name,
					    "specifier", ags_fx_synth_audio_specifier[40],
					    "control-port", ags_fx_synth_audio_control_port[40],
					    "port-value-is-pointer", FALSE,
					    "port-value-type", G_TYPE_FLOAT,
					    "port-value-size", sizeof(gfloat),
					    "port-value-length", 2,
					    NULL);
  
  fx_synth_audio->chorus_mix->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->chorus_mix,
	       "plugin-port", ags_fx_synth_audio_get_chorus_mix_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->chorus_mix);

  /* chorus delay */
  fx_synth_audio->chorus_delay = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_fx_synth_audio_plugin_name,
					      "specifier", ags_fx_synth_audio_specifier[41],
					      "control-port", ags_fx_synth_audio_control_port[41],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_FLOAT,
					      "port-value-size", sizeof(gfloat),
					      "port-value-length", 2,
					      NULL);
  
  fx_synth_audio->chorus_delay->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->chorus_delay,
	       "plugin-port", ags_fx_synth_audio_get_chorus_delay_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->chorus_delay);

  /* low-pass enabled */
  fx_synth_audio->low_pass_enabled = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_synth_audio_plugin_name,
						  "specifier", ags_fx_synth_audio_specifier[42],
						  "control-port", ags_fx_synth_audio_control_port[42],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 2,
						  NULL);
  
  fx_synth_audio->low_pass_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->low_pass_enabled,
	       "plugin-port", ags_fx_synth_audio_get_low_pass_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->low_pass_enabled);

  /* low-pass q-lin */
  fx_synth_audio->low_pass_q_lin = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_synth_audio_plugin_name,
						"specifier", ags_fx_synth_audio_specifier[43],
						"control-port", ags_fx_synth_audio_control_port[43],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 2,
						NULL);
  
  fx_synth_audio->low_pass_q_lin->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->low_pass_q_lin,
	       "plugin-port", ags_fx_synth_audio_get_low_pass_q_lin_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->low_pass_q_lin);

  /* low-pass filter-gain */
  fx_synth_audio->low_pass_filter_gain = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_synth_audio_plugin_name,
						      "specifier", ags_fx_synth_audio_specifier[44],
						      "control-port", ags_fx_synth_audio_control_port[44],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 2,
						      NULL);
  
  fx_synth_audio->low_pass_filter_gain->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->low_pass_filter_gain,
	       "plugin-port", ags_fx_synth_audio_get_low_pass_filter_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->low_pass_filter_gain);

  /* high-pass enabled */
  fx_synth_audio->high_pass_enabled = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_synth_audio_plugin_name,
						   "specifier", ags_fx_synth_audio_specifier[45],
						   "control-port", ags_fx_synth_audio_control_port[45],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 2,
						   NULL);
  
  fx_synth_audio->high_pass_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->high_pass_enabled,
	       "plugin-port", ags_fx_synth_audio_get_high_pass_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->high_pass_enabled);

  /* high-pass q-lin */
  fx_synth_audio->high_pass_q_lin = g_object_new(AGS_TYPE_PORT,
						 "plugin-name", ags_fx_synth_audio_plugin_name,
						 "specifier", ags_fx_synth_audio_specifier[46],
						 "control-port", ags_fx_synth_audio_control_port[46],
						 "port-value-is-pointer", FALSE,
						 "port-value-type", G_TYPE_FLOAT,
						 "port-value-size", sizeof(gfloat),
						 "port-value-length", 2,
						 NULL);
  
  fx_synth_audio->high_pass_q_lin->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->high_pass_q_lin,
	       "plugin-port", ags_fx_synth_audio_get_high_pass_q_lin_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->high_pass_q_lin);

  /* high-pass filter-gain */
  fx_synth_audio->high_pass_filter_gain = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_synth_audio_plugin_name,
						       "specifier", ags_fx_synth_audio_specifier[47],
						       "control-port", ags_fx_synth_audio_control_port[47],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 2,
						       NULL);
  
  fx_synth_audio->high_pass_filter_gain->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_synth_audio->high_pass_filter_gain,
	       "plugin-port", ags_fx_synth_audio_get_high_pass_filter_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_synth_audio,
		      fx_synth_audio->high_pass_filter_gain);
}

void
ags_fx_synth_audio_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsFxSynthAudio *fx_synth_audio;

  GRecMutex *recall_mutex;

  fx_synth_audio = AGS_FX_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_synth_audio);

  switch(prop_id){
  case PROP_SYNTH_0_OSCILLATOR:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_0_oscillator){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_0_oscillator != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_0_oscillator));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_0_oscillator = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_OCTAVE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_0_octave){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_0_octave != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_0_octave));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_0_octave = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_KEY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_0_key){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_0_key != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_0_key));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_0_key = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_PHASE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_0_phase){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_0_phase != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_0_phase));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_0_phase = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_VOLUME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_0_volume){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_0_volume != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_0_volume));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_0_volume = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_0_sync_enabled){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_0_sync_enabled != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_0_sync_enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_0_sync_enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_ATTACK_0:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_0_sync_attack_0){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_0_sync_attack_0 != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_0_sync_attack_0));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_0_sync_attack_0 = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_PHASE_0:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_0_sync_phase_0){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_0_sync_phase_0 != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_0_sync_phase_0));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_0_sync_phase_0 = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_ATTACK_1:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_0_sync_attack_1){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_0_sync_attack_1 != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_0_sync_attack_1));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_0_sync_attack_1 = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_PHASE_1:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_0_sync_phase_1){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_0_sync_phase_1 != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_0_sync_phase_1));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_0_sync_phase_1 = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_ATTACK_2:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_0_sync_attack_2){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_0_sync_attack_2 != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_0_sync_attack_2));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_0_sync_attack_2 = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_PHASE_2:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_0_sync_phase_2){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_0_sync_phase_2 != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_0_sync_phase_2));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_0_sync_phase_2 = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_LFO_OSCILLATOR:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_0_sync_lfo_oscillator){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_0_sync_lfo_oscillator != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_0_sync_lfo_oscillator));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_0_sync_lfo_oscillator = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_LFO_FREQUENCY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_0_sync_lfo_frequency){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_0_sync_lfo_frequency != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_0_sync_lfo_frequency));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_0_sync_lfo_frequency = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_OSCILLATOR:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_1_oscillator){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_1_oscillator != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_1_oscillator));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_1_oscillator = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_OCTAVE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_1_octave){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_1_octave != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_1_octave));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_1_octave = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_KEY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_1_key){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_1_key != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_1_key));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_1_key = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_PHASE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_1_phase){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_1_phase != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_1_phase));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_1_phase = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_VOLUME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_1_volume){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_1_volume != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_1_volume));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_1_volume = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_1_sync_enabled){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_1_sync_enabled != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_1_sync_enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_1_sync_enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_ATTACK_0:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_1_sync_attack_0){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_1_sync_attack_0 != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_1_sync_attack_0));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_1_sync_attack_0 = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_PHASE_0:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_1_sync_phase_0){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_1_sync_phase_0 != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_1_sync_phase_0));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_1_sync_phase_0 = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_ATTACK_1:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_1_sync_attack_1){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_1_sync_attack_1 != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_1_sync_attack_1));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_1_sync_attack_1 = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_PHASE_1:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_1_sync_phase_1){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_1_sync_phase_1 != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_1_sync_phase_1));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_1_sync_phase_1 = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_ATTACK_2:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_1_sync_attack_2){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_1_sync_attack_2 != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_1_sync_attack_2));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_1_sync_attack_2 = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_PHASE_2:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_1_sync_phase_2){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_1_sync_phase_2 != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_1_sync_phase_2));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_1_sync_phase_2 = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_LFO_OSCILLATOR:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_1_sync_lfo_oscillator){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_1_sync_lfo_oscillator != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_1_sync_lfo_oscillator));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_1_sync_lfo_oscillator = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_LFO_FREQUENCY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->synth_1_sync_lfo_frequency){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->synth_1_sync_lfo_frequency != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->synth_1_sync_lfo_frequency));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->synth_1_sync_lfo_frequency = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SEQUENCER_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->sequencer_enabled){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->sequencer_enabled != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->sequencer_enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->sequencer_enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SEQUENCER_SIGN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->sequencer_sign){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->sequencer_sign != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->sequencer_sign));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->sequencer_sign = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PITCH_TUNING:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->pitch_tuning){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->pitch_tuning != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->pitch_tuning));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->pitch_tuning = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_NOISE_GAIN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->noise_gain){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->noise_gain != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->noise_gain));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->noise_gain = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->chorus_enabled){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->chorus_enabled != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->chorus_enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->chorus_enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_INPUT_VOLUME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->chorus_input_volume){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->chorus_input_volume != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->chorus_input_volume));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->chorus_input_volume = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_OUTPUT_VOLUME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->chorus_output_volume){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->chorus_output_volume != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->chorus_output_volume));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->chorus_output_volume = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_LFO_OSCILLATOR:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->chorus_lfo_oscillator){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->chorus_lfo_oscillator != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->chorus_lfo_oscillator));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->chorus_lfo_oscillator = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_LFO_FREQUENCY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->chorus_lfo_frequency){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->chorus_lfo_frequency != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->chorus_lfo_frequency));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->chorus_lfo_frequency = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_DEPTH:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->chorus_depth){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->chorus_depth != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->chorus_depth));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->chorus_depth = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_MIX:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->chorus_mix){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->chorus_mix != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->chorus_mix));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->chorus_mix = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_DELAY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->chorus_delay){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->chorus_delay != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->chorus_delay));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->chorus_delay = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LOW_PASS_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->low_pass_enabled){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->low_pass_enabled != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->low_pass_enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->low_pass_enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LOW_PASS_Q_LIN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->low_pass_q_lin){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->low_pass_q_lin != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->low_pass_q_lin));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->low_pass_q_lin = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LOW_PASS_FILTER_GAIN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->low_pass_filter_gain){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->low_pass_filter_gain != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->low_pass_filter_gain));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->low_pass_filter_gain = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_HIGH_PASS_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->high_pass_enabled){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->high_pass_enabled != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->high_pass_enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->high_pass_enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_HIGH_PASS_Q_LIN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->high_pass_q_lin){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->high_pass_q_lin != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->high_pass_q_lin));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->high_pass_q_lin = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_HIGH_PASS_FILTER_GAIN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_synth_audio->high_pass_filter_gain){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_synth_audio->high_pass_filter_gain != NULL){
      g_object_unref(G_OBJECT(fx_synth_audio->high_pass_filter_gain));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_synth_audio->high_pass_filter_gain = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_synth_audio_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsFxSynthAudio *fx_synth_audio;

  GRecMutex *recall_mutex;

  fx_synth_audio = AGS_FX_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_synth_audio);

  switch(prop_id){
  case PROP_SYNTH_0_OSCILLATOR:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_0_oscillator);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  case PROP_SYNTH_0_OCTAVE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_0_octave);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  case PROP_SYNTH_0_KEY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_0_key);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_PHASE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_0_phase);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_VOLUME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_0_volume);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_0_sync_enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_ATTACK_0:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_0_sync_attack_0);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_PHASE_0:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_0_sync_phase_0);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_ATTACK_1:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_0_sync_attack_1);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_PHASE_1:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_0_sync_phase_1);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_ATTACK_2:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_0_sync_attack_2);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_PHASE_2:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_0_sync_phase_2);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_LFO_OSCILLATOR:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_0_sync_lfo_oscillator);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_0_SYNC_LFO_FREQUENCY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_0_sync_lfo_frequency);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_OSCILLATOR:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_1_oscillator);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  case PROP_SYNTH_1_OCTAVE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_1_octave);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  case PROP_SYNTH_1_KEY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_1_key);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_PHASE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_1_phase);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_VOLUME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_1_volume);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_1_sync_enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_ATTACK_0:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_1_sync_attack_0);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_PHASE_0:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_1_sync_phase_0);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_ATTACK_1:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_1_sync_attack_1);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_PHASE_1:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_1_sync_phase_1);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_ATTACK_2:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_1_sync_attack_2);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_PHASE_2:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_1_sync_phase_2);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_LFO_OSCILLATOR:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_1_sync_lfo_oscillator);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_1_SYNC_LFO_FREQUENCY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->synth_1_sync_lfo_frequency);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SEQUENCER_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->sequencer_enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SEQUENCER_SIGN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->sequencer_sign);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_PITCH_TUNING:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->pitch_tuning);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_NOISE_GAIN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->noise_gain);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->chorus_enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_INPUT_VOLUME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->chorus_input_volume);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_OUTPUT_VOLUME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->chorus_output_volume);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_LFO_OSCILLATOR:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->chorus_lfo_oscillator);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_LFO_FREQUENCY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->chorus_lfo_frequency);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_DEPTH:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->chorus_depth);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_MIX:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->chorus_mix);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_DELAY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->chorus_delay);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LOW_PASS_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->low_pass_enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LOW_PASS_Q_LIN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->low_pass_q_lin);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_LOW_PASS_FILTER_GAIN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->low_pass_filter_gain);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_HIGH_PASS_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->high_pass_enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_HIGH_PASS_Q_LIN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->high_pass_q_lin);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_HIGH_PASS_FILTER_GAIN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_synth_audio->high_pass_filter_gain);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_synth_audio_dispose(GObject *gobject)
{
  AgsFxSynthAudio *fx_synth_audio;
  
  fx_synth_audio = AGS_FX_SYNTH_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_synth_audio_parent_class)->dispose(gobject);
}

void
ags_fx_synth_audio_finalize(GObject *gobject)
{
  AgsFxSynthAudio *fx_synth_audio;
  
  fx_synth_audio = AGS_FX_SYNTH_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_synth_audio_parent_class)->finalize(gobject);
}

/**
 * ags_fx_synth_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxSynthAudio
 *
 * Returns: the new #AgsFxSynthAudio
 *
 * Since: 3.14.0
 */
AgsFxSynthAudio*
ags_fx_synth_audio_new(AgsAudio *audio)
{
  AgsFxSynthAudio *fx_synth_audio;

  fx_synth_audio = (AgsFxSynthAudio *) g_object_new(AGS_TYPE_FX_SYNTH_AUDIO,
						    "audio", audio,
						    NULL);

  return(fx_synth_audio);
}
