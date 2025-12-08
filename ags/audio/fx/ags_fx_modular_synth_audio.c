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

#include <ags/audio/fx/ags_fx_modular_synth_audio.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_audio_signal.h>

#include <ags/i18n.h>

void ags_fx_modular_synth_audio_class_init(AgsFxModularSynthAudioClass *fx_modular_synth_audio);
void ags_fx_modular_synth_audio_init(AgsFxModularSynthAudio *fx_modular_synth_audio);
void ags_fx_modular_synth_audio_set_property(GObject *gobject,
					     guint prop_id,
					     const GValue *value,
					     GParamSpec *param_spec);
void ags_fx_modular_synth_audio_get_property(GObject *gobject,
					     guint prop_id,
					     GValue *value,
					     GParamSpec *param_spec);
void ags_fx_modular_synth_audio_dispose(GObject *gobject);
void ags_fx_modular_synth_audio_finalize(GObject *gobject);

void ags_fx_modular_synth_audio_notify_audio_callback(GObject *gobject,
						      GParamSpec *pspec,
						      gpointer user_data);
void ags_fx_modular_synth_audio_notify_buffer_size_callback(GObject *gobject,
							    GParamSpec *pspec,
							    gpointer user_data);
void ags_fx_modular_synth_audio_notify_format_callback(GObject *gobject,
						       GParamSpec *pspec,
						       gpointer user_data);
void ags_fx_modular_synth_audio_notify_samplerate_callback(GObject *gobject,
							   GParamSpec *pspec,
							   gpointer user_data);

void ags_fx_modular_synth_audio_set_audio_channels_callback(AgsAudio *audio,
							    guint audio_channels, guint audio_channels_old,
							    AgsFxModularSynthAudio *fx_modular_synth_audio);

static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_osc_0_oscillator_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_osc_0_octave_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_osc_0_key_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_osc_0_phase_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_osc_0_volume_plugin_port();

static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_osc_1_oscillator_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_osc_1_octave_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_osc_1_key_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_osc_1_phase_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_osc_1_volume_plugin_port();

static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_pitch_tuning_plugin_port();

static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_volume_plugin_port();

static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_env_0_attack_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_env_0_decay_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_env_0_sustain_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_env_0_release_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_env_0_gain_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_env_0_frequency_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_env_0_sends_plugin_port();

static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_env_1_attack_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_env_1_decay_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_env_1_sustain_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_env_1_release_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_env_1_gain_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_env_1_frequency_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_env_1_sends_plugin_port();

static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_lfo_0_oscillator_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_lfo_0_frequency_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_lfo_0_depth_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_lfo_0_tuning_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_lfo_0_sends_plugin_port();

static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_lfo_1_oscillator_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_lfo_1_frequency_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_lfo_1_depth_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_lfo_1_tuning_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_lfo_1_sends_plugin_port();

static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_noise_frequency_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_noise_gain_plugin_port();
static AgsPluginPort* ags_fx_modular_synth_audio_get_synth_0_noise_sends_plugin_port();

/**
 * SECTION:ags_fx_modular_synth_audio
 * @short_description: fx modular synth audio
 * @title: AgsFxModularSynthAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_modular_synth_audio.h
 *
 * The #AgsFxModularSynthAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_modular_synth_audio_parent_class = NULL;

const gchar *ags_fx_modular_synth_audio_plugin_name = "ags-fx-modular-synth";

const gchar* ags_fx_modular_synth_audio_specifier[] = {
  "./synth-0-osc-0-oscillator[0]",
  "./synth-0-osc-0-octave[0]",
  "./synth-0-osc-0-key[0]",
  "./synth-0-osc-0-phase[0]",
  "./synth-0-osc-0-volume[0]",
  "./synth-0-osc-1-oscillator[0]",
  "./synth-0-osc-1-octave[0]",
  "./synth-0-osc-1-key[0]",
  "./synth-0-osc-1-phase[0]",
  "./synth-0-osc-1-volume[0]",
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
  "./synth-0-noise-frequency[0]",
  "./synth-0-noise-gain[0]",
  "./synth-0-noise-sends[0]",
  NULL,
};

enum{
  PROP_0,
  PROP_SYNTH_0_OSC_0_OSCILLATOR,
  PROP_SYNTH_0_OSC_0_OCTAVE,
  PROP_SYNTH_0_OSC_0_KEY,
  PROP_SYNTH_0_OSC_0_PHASE,
  PROP_SYNTH_0_OSC_0_VOLUME,
  PROP_SYNTH_0_OSC_1_OSCILLATOR,
  PROP_SYNTH_0_OSC_1_OCTAVE,
  PROP_SYNTH_0_OSC_1_KEY,
  PROP_SYNTH_0_OSC_1_PHASE,
  PROP_SYNTH_0_OSC_1_VOLUME,
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
  PROP_SYNTH_0_NOISE_FREQUENCY,
  PROP_SYNTH_0_NOISE_GAIN,
  PROP_SYNTH_0_NOISE_SENDS,
};

GType
ags_fx_modular_synth_audio_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_modular_synth_audio = 0;

    static const GTypeInfo ags_fx_modular_synth_audio_info = {
      sizeof (AgsFxModularSynthAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_modular_synth_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxModularSynthAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_modular_synth_audio_init,
    };

    ags_type_fx_modular_synth_audio = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO,
							     "AgsFxModularSynthAudio",
							     &ags_fx_modular_synth_audio_info,
							     0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_modular_synth_audio);
  }

  return(g_define_type_id__static);
}

void
ags_fx_modular_synth_audio_class_init(AgsFxModularSynthAudioClass *fx_modular_synth_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_modular_synth_audio_parent_class = g_type_class_peek_parent(fx_modular_synth_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_modular_synth_audio;

  gobject->set_property = ags_fx_modular_synth_audio_set_property;
  gobject->get_property = ags_fx_modular_synth_audio_get_property;

  gobject->dispose = ags_fx_modular_synth_audio_dispose;
  gobject->finalize = ags_fx_modular_synth_audio_finalize;

  /* properties */
  /**
   * AgsFxModularSynthAudio:synth-0-osc-0-oscillator:
   *
   * The synth-0 osc-0 oscillator.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-osc-0-octave:
   *
   * The synth-0 osc-0 octave.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-osc-0-key:
   *
   * The synth-0 osc-0 key.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-osc-0-phase:
   *
   * The synth-0 osc-0 phase.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-osc-0-volume:
   *
   * The synth-0 osc-0 volume.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-osc-1-oscillator:
   *
   * The synth-0 osc-1 oscillator.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-osc-1-octave:
   *
   * The synth-0 osc-1 octave.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-osc-1-key:
   *
   * The synth-0 osc-1 key.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-osc-1-phase:
   *
   * The synth-0 osc-1 phase.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-osc-1-volume:
   *
   * The synth-0 osc-1 volume.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-pitch-tuning:
   *
   * The synth 0 pitch tuning.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-volume:
   *
   * The synth 0 volume.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-env-0-attack:
   *
   * The synth 0 env-0 attack.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-env-0-decay:
   *
   * The synth 0 env-0 decay.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-env-0-sustain:
   *
   * The synth 0 env-0 sustain.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-env-0-release:
   *
   * The synth 0 env-0 release.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-env-0-gain:
   *
   * The synth 0 env-0 gain.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-env-0-frequency:
   *
   * The synth 0 env-0 frequency.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-env-0-sends:
   *
   * The synth 0 env-0 sends.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-env-1-attack:
   *
   * The synth 0 env-1 attack.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-env-1-decay:
   *
   * The synth 0 env-1 decay.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-env-1-sustain:
   *
   * The synth 0 env-1 sustain.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-env-1-release:
   *
   * The synth 0 env-1 release.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-env-1-gain:
   *
   * The synth 0 env-1 gain.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-env-1-frequency:
   *
   * The synth 0 env-1 frequency.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-env-1-sends:
   *
   * The synth 0 env-1 sends.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-lfo-0-oscillator:
   *
   * The synth 0 lfo-0 oscillator.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-lfo-0-frequency:
   *
   * The synth 0 lfo-0 frequency.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-lfo-0-tuning:
   *
   * The synth 0 lfo-0 tuning.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-lfo-0-sends:
   *
   * The synth 0 lfo-0 sends.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-lfo-1-oscillator:
   *
   * The synth 0 lfo-1 oscillator.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-lfo-1-frequency:
   *
   * The synth 0 lfo-1 frequency.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-lfo-1-tuning:
   *
   * The synth 0 lfo-1 tuning.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-lfo-1-sends:
   *
   * The synth 0 lfo-1 sends.
   * 
   * Since: 8.2.0
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
   * AgsFxModularSynthAudio:synth-0-noise-frequency:
   *
   * The synth 0 noise frequency.
   * 
   * Since: 8.2.0
   */
  param_spec = g_param_spec_object("synth-0-noise-frequency",
				   i18n_pspec("synth 0 noise frequency of recall"),
				   i18n_pspec("The synth 0 noise frequency"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_NOISE_FREQUENCY,
				  param_spec);

  /**
   * AgsFxModularSynthAudio:synth-0-noise-gain:
   *
   * The synth 0 noise gain.
   * 
   * Since: 8.2.0
   */
  param_spec = g_param_spec_object("synth-0-noise-gain",
				   i18n_pspec("synth 0 noise gain of recall"),
				   i18n_pspec("The synth 0 noise gain"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_NOISE_GAIN,
				  param_spec);

  /**
   * AgsFxModularSynthAudio:synth-0-noise-sends:
   *
   * The synth 0 noise sends.
   * 
   * Since: 8.2.0
   */
  param_spec = g_param_spec_object("synth-0-noise-sends",
				   i18n_pspec("synth 0 noise sends of recall"),
				   i18n_pspec("The synth 0 noise sends"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_0_NOISE_SENDS,
				  param_spec);
}

void
ags_fx_modular_synth_audio_init(AgsFxModularSynthAudio *fx_modular_synth_audio)
{
  gint position;
  guint i;
  
  g_signal_connect(fx_modular_synth_audio, "notify::audio",
		   G_CALLBACK(ags_fx_modular_synth_audio_notify_audio_callback), NULL);

  g_signal_connect(fx_modular_synth_audio, "notify::buffer-size",
		   G_CALLBACK(ags_fx_modular_synth_audio_notify_buffer_size_callback), NULL);

  g_signal_connect(fx_modular_synth_audio, "notify::format",
		   G_CALLBACK(ags_fx_modular_synth_audio_notify_format_callback), NULL);

  g_signal_connect(fx_modular_synth_audio, "notify::samplerate",
		   G_CALLBACK(ags_fx_modular_synth_audio_notify_samplerate_callback), NULL);

  AGS_RECALL(fx_modular_synth_audio)->name = "ags-fx-modular-synth";
  AGS_RECALL(fx_modular_synth_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_modular_synth_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_modular_synth_audio)->xml_type = "ags-fx-modular-synth-audio";

  position = 0;

  /* synth-0 osc-0 oscillator */
  fx_modular_synth_audio->synth_0_osc_0_oscillator = g_object_new(AGS_TYPE_PORT,
								  "plugin-name", ags_fx_modular_synth_audio_plugin_name,
								  "specifier", "./synth-0-osc-0-oscillator[0]",
								  "control-port", "1/40",
								  "port-value-is-pointer", FALSE,
								  "port-value-type", G_TYPE_FLOAT,
								  "port-value-size", sizeof(gfloat),
								  "port-value-length", 1,
								  NULL);
  
  fx_modular_synth_audio->synth_0_osc_0_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_modular_synth_audio->synth_0_osc_0_oscillator,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_osc_0_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_osc_0_oscillator);

  position++;

  /* synth-0 osc-0 octave */
  fx_modular_synth_audio->synth_0_osc_0_octave = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-osc-0-octave[0]",
							      "control-port", "2/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_osc_0_octave->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_modular_synth_audio->synth_0_osc_0_octave,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_osc_0_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_osc_0_octave);

  position++;

  /* synth-0 osc-0 key */
  fx_modular_synth_audio->synth_0_osc_0_key = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							   "specifier", "./synth-0-osc-0-key[0]",
							   "control-port", "3/40",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_modular_synth_audio->synth_0_osc_0_key->port_value.ags_port_float = (gfloat) 2.0;

  g_object_set(fx_modular_synth_audio->synth_0_osc_0_key,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_osc_0_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_osc_0_key);

  position++;

  /* synth-0 osc-0 phase */
  fx_modular_synth_audio->synth_0_osc_0_phase = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							     "specifier", "./synth-0-osc-0-phase[0]",
							     "control-port", "4/40",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_modular_synth_audio->synth_0_osc_0_phase->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_modular_synth_audio->synth_0_osc_0_phase,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_osc_0_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_osc_0_phase);

  position++;

  /* synth-0 osc-0 volume */
  fx_modular_synth_audio->synth_0_osc_0_volume = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-osc-0-volume[0]",
							      "control-port", "5/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_osc_0_volume->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_osc_0_volume,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_osc_0_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_osc_0_volume);

  position++;

  /* synth-0 osc-1 oscillator */
  fx_modular_synth_audio->synth_0_osc_1_oscillator = g_object_new(AGS_TYPE_PORT,
								  "plugin-name", ags_fx_modular_synth_audio_plugin_name,
								  "specifier", "./synth-0-osc-1-oscillator[0]",
								  "control-port", "6/40",
								  "port-value-is-pointer", FALSE,
								  "port-value-type", G_TYPE_FLOAT,
								  "port-value-size", sizeof(gfloat),
								  "port-value-length", 1,
								  NULL);
  
  fx_modular_synth_audio->synth_0_osc_1_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_modular_synth_audio->synth_0_osc_1_oscillator,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_osc_1_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_osc_1_oscillator);

  position++;

  /* synth-0 osc-1 octave */
  fx_modular_synth_audio->synth_0_osc_1_octave = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-osc-1-octave[0]",
							      "control-port", "7/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_osc_1_octave->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_modular_synth_audio->synth_0_osc_1_octave,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_osc_1_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_osc_1_octave);

  position++;

  /* synth-0 osc-1 key */
  fx_modular_synth_audio->synth_0_osc_1_key = g_object_new(AGS_TYPE_PORT,
							   "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							   "specifier", "./synth-0-osc-1-key[0]",
							   "control-port", "8/40",
							   "port-value-is-pointer", FALSE,
							   "port-value-type", G_TYPE_FLOAT,
							   "port-value-size", sizeof(gfloat),
							   "port-value-length", 1,
							   NULL);
  
  fx_modular_synth_audio->synth_0_osc_1_key->port_value.ags_port_float = (gfloat) 2.0;

  g_object_set(fx_modular_synth_audio->synth_0_osc_1_key,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_osc_1_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_osc_1_key);

  position++;

  /* synth-0 osc-1 phase */
  fx_modular_synth_audio->synth_0_osc_1_phase = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							     "specifier", "./synth-0-osc-1-phase[0]",
							     "control-port", "9/40",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_modular_synth_audio->synth_0_osc_1_phase->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_modular_synth_audio->synth_0_osc_1_phase,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_osc_1_phase_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_osc_1_phase);

  position++;

  /* synth-0 osc-1 volume */
  fx_modular_synth_audio->synth_0_osc_1_volume = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-osc-1-volume[0]",
							      "control-port", "10/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_osc_1_volume->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_osc_1_volume,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_osc_1_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_osc_1_volume);

  position++;

  /* synth-0 pitch tuning */
  fx_modular_synth_audio->synth_0_pitch_tuning = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./pitch-tuning[0]",
							      "control-port", "11/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_pitch_tuning->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_modular_synth_audio->synth_0_pitch_tuning,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_pitch_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_pitch_tuning);

  position++;

  /* synth-0 volume */
  fx_modular_synth_audio->synth_0_volume = g_object_new(AGS_TYPE_PORT,
							"plugin-name", ags_fx_modular_synth_audio_plugin_name,
							"specifier", "./volume[0]",
							"control-port", "12/40",
							"port-value-is-pointer", FALSE,
							"port-value-type", G_TYPE_FLOAT,
							"port-value-size", sizeof(gfloat),
							"port-value-length", 1,
							NULL);
  
  fx_modular_synth_audio->synth_0_volume->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_modular_synth_audio->synth_0_volume,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_volume);

  position++;

  /* synth-0 env-0 attack */
  fx_modular_synth_audio->synth_0_env_0_attack = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-env-0-attack[0]",
							      "control-port", "13/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_env_0_attack->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_env_0_attack,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_env_0_attack_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_env_0_attack);

  position++;

  /* synth-0 env-0 decay */
  fx_modular_synth_audio->synth_0_env_0_decay = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							     "specifier", "./synth-0-env-0-decay[0]",
							     "control-port", "14/40",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_modular_synth_audio->synth_0_env_0_decay->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_env_0_decay,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_env_0_decay_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_env_0_decay);

  position++;

  /* synth-0 env-0 sustain */
  fx_modular_synth_audio->synth_0_env_0_sustain = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							       "specifier", "./synth-0-env-0-sustain[0]",
							       "control-port", "15/40",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_modular_synth_audio->synth_0_env_0_sustain->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_env_0_sustain,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_env_0_sustain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_env_0_sustain);

  position++;

  /* synth-0 env-0 release */
  fx_modular_synth_audio->synth_0_env_0_release = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							       "specifier", "./synth-0-env-0-release[0]",
							       "control-port", "17/40",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_modular_synth_audio->synth_0_env_0_release->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_env_0_release,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_env_0_release_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_env_0_release);

  position++;

  /* synth-0 env-0 gain */
  fx_modular_synth_audio->synth_0_env_0_gain = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							    "specifier", "./synth-0-env-0-gain[0]",
							    "control-port", "18/40",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_modular_synth_audio->synth_0_env_0_gain->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_env_0_gain,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_env_0_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_env_0_gain);

  position++;

  /* synth-0 env-0 frequency */
  fx_modular_synth_audio->synth_0_env_0_frequency = g_object_new(AGS_TYPE_PORT,
								 "plugin-name", ags_fx_modular_synth_audio_plugin_name,
								 "specifier", "./synth-0-env-0-frequency[0]",
								 "control-port", "19/40",
								 "port-value-is-pointer", FALSE,
								 "port-value-type", G_TYPE_FLOAT,
								 "port-value-size", sizeof(gfloat),
								 "port-value-length", 1,
								 NULL);
  
  fx_modular_synth_audio->synth_0_env_0_frequency->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_env_0_frequency,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_env_0_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_env_0_frequency);

  position++;

  /* synth-0 env-0 sends */
  fx_modular_synth_audio->synth_0_env_0_sends = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							     "specifier", "./synth-0-env-0-sends[0]",
							     "control-port", "20/40",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_modular_synth_audio->synth_0_env_0_sends->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_env_0_sends,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_env_0_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_env_0_sends);

  position++;
  
  /* synth-0 env-1 attack */
  fx_modular_synth_audio->synth_0_env_1_attack = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-env-1-attack[0]",
							      "control-port", "21/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_env_1_attack->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_env_1_attack,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_env_1_attack_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_env_1_attack);

  position++;

  /* synth-0 env-1 decay */
  fx_modular_synth_audio->synth_0_env_1_decay = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							     "specifier", "./synth-0-env-1-decay[0]",
							     "control-port", "22/40",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_modular_synth_audio->synth_0_env_1_decay->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_env_1_decay,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_env_1_decay_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_env_1_decay);

  position++;

  /* synth-0 env-1 sustain */
  fx_modular_synth_audio->synth_0_env_1_sustain = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							       "specifier", "./synth-0-env-1-sustain[0]",
							       "control-port", "23/40",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_modular_synth_audio->synth_0_env_1_sustain->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_env_1_sustain,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_env_1_sustain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_env_1_sustain);

  position++;

  /* synth-0 env-1 release */
  fx_modular_synth_audio->synth_0_env_1_release = g_object_new(AGS_TYPE_PORT,
							       "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							       "specifier", "./synth-0-env-1-release[0]",
							       "control-port", "24/40",
							       "port-value-is-pointer", FALSE,
							       "port-value-type", G_TYPE_FLOAT,
							       "port-value-size", sizeof(gfloat),
							       "port-value-length", 1,
							       NULL);
  
  fx_modular_synth_audio->synth_0_env_1_release->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_env_1_release,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_env_1_release_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_env_1_release);

  position++;

  /* synth-0 env-1 gain */
  fx_modular_synth_audio->synth_0_env_1_gain = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							    "specifier", "./synth-0-env-1-gain[0]",
							    "control-port", "25/40",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_modular_synth_audio->synth_0_env_1_gain->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_env_1_gain,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_env_1_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_env_1_gain);

  position++;

  /* synth-0 env-1 frequency */
  fx_modular_synth_audio->synth_0_env_1_frequency = g_object_new(AGS_TYPE_PORT,
								 "plugin-name", ags_fx_modular_synth_audio_plugin_name,
								 "specifier", "./synth-0-env-1-frequency[0]",
								 "control-port", "26/40",
								 "port-value-is-pointer", FALSE,
								 "port-value-type", G_TYPE_FLOAT,
								 "port-value-size", sizeof(gfloat),
								 "port-value-length", 1,
								 NULL);
  
  fx_modular_synth_audio->synth_0_env_1_frequency->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_env_1_frequency,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_env_1_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_env_1_frequency);

  position++;

  /* synth-0 env-1 sends */
  fx_modular_synth_audio->synth_0_env_1_sends = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							     "specifier", "./synth-0-env-1-sends[0]",
							     "control-port", "27/40",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_modular_synth_audio->synth_0_env_1_sends->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_env_1_sends,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_env_1_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_env_1_sends);

  position++;
  
  /* synth-0 lfo-0 oscillator */
  fx_modular_synth_audio->synth_0_lfo_0_oscillator = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-lfo-0-oscillator[0]",
							      "control-port", "28/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_lfo_0_oscillator->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_lfo_0_oscillator,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_lfo_0_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_lfo_0_oscillator);

  position++;
  
  /* synth-0 lfo-0 frequency */
  fx_modular_synth_audio->synth_0_lfo_0_frequency = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-lfo-0-frequency[0]",
							      "control-port", "29/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_lfo_0_frequency->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_lfo_0_frequency,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_lfo_0_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_lfo_0_frequency);

  position++;
  
  /* synth-0 lfo-0 depth */
  fx_modular_synth_audio->synth_0_lfo_0_depth = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-lfo-0-depth[0]",
							      "control-port", "30/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_lfo_0_depth->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_lfo_0_depth,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_lfo_0_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_lfo_0_depth);

  position++;
  
  /* synth-0 lfo-0 tuning */
  fx_modular_synth_audio->synth_0_lfo_0_tuning = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-lfo-0-tuning[0]",
							      "control-port", "31/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_lfo_0_tuning->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_lfo_0_tuning,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_lfo_0_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_lfo_0_tuning);

  position++;
  
  /* synth-0 lfo-0 sends */
  fx_modular_synth_audio->synth_0_lfo_0_sends = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-lfo-0-sends[0]",
							      "control-port", "32/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_lfo_0_sends->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_lfo_0_sends,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_lfo_0_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_lfo_0_sends);

  position++;

  /* synth-0 lfo-1 oscillator */
  fx_modular_synth_audio->synth_0_lfo_1_oscillator = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-lfo-1-oscillator[0]",
							      "control-port", "33/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_lfo_1_oscillator->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_lfo_1_oscillator,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_lfo_1_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_lfo_1_oscillator);

  position++;
  
  /* synth-0 lfo-1 frequency */
  fx_modular_synth_audio->synth_0_lfo_1_frequency = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-lfo-1-frequency[0]",
							      "control-port", "34/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_lfo_1_frequency->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_lfo_1_frequency,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_lfo_1_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_lfo_1_frequency);

  position++;
  
  /* synth-0 lfo-1 depth */
  fx_modular_synth_audio->synth_0_lfo_1_depth = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-lfo-1-depth[0]",
							      "control-port", "35/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_lfo_1_depth->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_lfo_1_depth,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_lfo_1_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_lfo_1_depth);

  position++;
  
  /* synth-0 lfo-1 tuning */
  fx_modular_synth_audio->synth_0_lfo_1_tuning = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-lfo-1-tuning[0]",
							      "control-port", "36/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_lfo_1_tuning->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_lfo_1_tuning,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_lfo_1_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_lfo_1_tuning);

  position++;
  
  /* synth-0 lfo-1 sends */
  fx_modular_synth_audio->synth_0_lfo_1_sends = g_object_new(AGS_TYPE_PORT,
							      "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							      "specifier", "./synth-0-lfo-1-sends[0]",
							      "control-port", "37/40",
							      "port-value-is-pointer", FALSE,
							      "port-value-type", G_TYPE_FLOAT,
							      "port-value-size", sizeof(gfloat),
							      "port-value-length", 1,
							      NULL);
  
  fx_modular_synth_audio->synth_0_lfo_1_sends->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_modular_synth_audio->synth_0_lfo_1_sends,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_lfo_1_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_lfo_1_sends);

  position++;
  
  /* synth-0 noise frequency */
  fx_modular_synth_audio->synth_0_noise_frequency = g_object_new(AGS_TYPE_PORT,
								 "plugin-name", ags_fx_modular_synth_audio_plugin_name,
								 "specifier", "./noise-frequency[0]",
								 "control-port", "38/40",
								 "port-value-is-pointer", FALSE,
								 "port-value-type", G_TYPE_FLOAT,
								 "port-value-size", sizeof(gfloat),
								 "port-value-length", 1,
								 NULL);
  
  fx_modular_synth_audio->synth_0_noise_frequency->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_modular_synth_audio->synth_0_noise_frequency,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_noise_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_noise_frequency);

  position++;

  /* synth-0 noise gain */
  fx_modular_synth_audio->synth_0_noise_gain = g_object_new(AGS_TYPE_PORT,
							    "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							    "specifier", "./noise-gain[0]",
							    "control-port", "39/40",
							    "port-value-is-pointer", FALSE,
							    "port-value-type", G_TYPE_FLOAT,
							    "port-value-size", sizeof(gfloat),
							    "port-value-length", 1,
							    NULL);
  
  fx_modular_synth_audio->synth_0_noise_gain->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_modular_synth_audio->synth_0_noise_gain,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_noise_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_noise_gain);

  position++;

  /* synth-0 noise sends */
  fx_modular_synth_audio->synth_0_noise_sends = g_object_new(AGS_TYPE_PORT,
							     "plugin-name", ags_fx_modular_synth_audio_plugin_name,
							     "specifier", "./noise-sends[0]",
							     "control-port", "40/40",
							     "port-value-is-pointer", FALSE,
							     "port-value-type", G_TYPE_FLOAT,
							     "port-value-size", sizeof(gfloat),
							     "port-value-length", 1,
							     NULL);
  
  fx_modular_synth_audio->synth_0_noise_sends->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_modular_synth_audio->synth_0_noise_sends,
	       "plugin-port", ags_fx_modular_synth_audio_get_synth_0_noise_sends_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_modular_synth_audio,
		      fx_modular_synth_audio->synth_0_noise_sends);

  position++;
  
  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      fx_modular_synth_audio->scope_data[i] = ags_fx_modular_synth_audio_scope_data_alloc();
      
      fx_modular_synth_audio->scope_data[i]->parent = fx_modular_synth_audio;
    }else{
      fx_modular_synth_audio->scope_data[i] = NULL;
    }
  }
}

void
ags_fx_modular_synth_audio_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec)
{
  AgsFxModularSynthAudio *fx_modular_synth_audio;

  GRecMutex *recall_mutex;

  fx_modular_synth_audio = AGS_FX_MODULAR_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_modular_synth_audio);

  switch(prop_id){
  case PROP_SYNTH_0_OSC_0_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_osc_0_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_osc_0_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_osc_0_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_osc_0_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_OCTAVE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_osc_0_octave){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_osc_0_octave != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_osc_0_octave));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_osc_0_octave = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_KEY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_osc_0_key){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_osc_0_key != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_osc_0_key));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_osc_0_key = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_PHASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_osc_0_phase){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_osc_0_phase != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_osc_0_phase));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_osc_0_phase = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_osc_0_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_osc_0_volume != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_osc_0_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_osc_0_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_osc_1_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_osc_1_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_osc_1_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_osc_1_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_OCTAVE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_osc_1_octave){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_osc_1_octave != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_osc_1_octave));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_osc_1_octave = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_KEY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_osc_1_key){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_osc_1_key != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_osc_1_key));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_osc_1_key = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_PHASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_osc_1_phase){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_osc_1_phase != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_osc_1_phase));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_osc_1_phase = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_osc_1_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_osc_1_volume != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_osc_1_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_osc_1_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PITCH_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_pitch_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_pitch_tuning != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_pitch_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_pitch_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_volume){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_volume != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_volume = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_ATTACK:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_env_0_attack){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_env_0_attack != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_env_0_attack));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_env_0_attack = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_DECAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_env_0_decay){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_env_0_decay != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_env_0_decay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_env_0_decay = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_SUSTAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_env_0_sustain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_env_0_sustain != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_env_0_sustain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_env_0_sustain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_RELEASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_env_0_release){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_env_0_release != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_env_0_release));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_env_0_release = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_env_0_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_env_0_gain != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_env_0_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_env_0_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_env_0_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_env_0_frequency != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_env_0_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_env_0_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_env_0_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_env_0_sends != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_env_0_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_env_0_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_ATTACK:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_env_1_attack){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_env_1_attack != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_env_1_attack));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_env_1_attack = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_DECAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_env_1_decay){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_env_1_decay != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_env_1_decay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_env_1_decay = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_SUSTAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_env_1_sustain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_env_1_sustain != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_env_1_sustain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_env_1_sustain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_RELEASE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_env_1_release){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_env_1_release != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_env_1_release));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_env_1_release = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_env_1_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_env_1_gain != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_env_1_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_env_1_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_env_1_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_env_1_frequency != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_env_1_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_env_1_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_env_1_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_env_1_sends != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_env_1_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_env_1_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_lfo_0_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_lfo_0_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_lfo_0_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_lfo_0_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_lfo_0_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_lfo_0_frequency != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_lfo_0_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_lfo_0_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_lfo_0_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_lfo_0_depth != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_lfo_0_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_lfo_0_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_lfo_0_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_lfo_0_tuning != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_lfo_0_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_lfo_0_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_OSCILLATOR:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_lfo_1_oscillator){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_lfo_1_oscillator != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_lfo_1_oscillator));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_lfo_1_oscillator = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_lfo_1_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_lfo_1_frequency != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_lfo_1_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_lfo_1_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_DEPTH:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_lfo_1_depth){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_lfo_1_depth != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_lfo_1_depth));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_lfo_1_depth = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_TUNING:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_lfo_1_tuning){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_lfo_1_tuning != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_lfo_1_tuning));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_lfo_1_tuning = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_NOISE_FREQUENCY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_noise_frequency){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_noise_frequency != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_noise_frequency));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_noise_frequency = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_NOISE_GAIN:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_noise_gain){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_noise_gain != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_noise_gain));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_noise_gain = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_NOISE_SENDS:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == fx_modular_synth_audio->synth_0_noise_sends){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(fx_modular_synth_audio->synth_0_noise_sends != NULL){
	g_object_unref(G_OBJECT(fx_modular_synth_audio->synth_0_noise_sends));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      fx_modular_synth_audio->synth_0_noise_sends = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_modular_synth_audio_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec)
{
  AgsFxModularSynthAudio *fx_modular_synth_audio;

  GRecMutex *recall_mutex;

  fx_modular_synth_audio = AGS_FX_MODULAR_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_modular_synth_audio);

  switch(prop_id){
  case PROP_SYNTH_0_OSC_0_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_osc_0_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_OCTAVE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_osc_0_octave);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_KEY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_osc_0_key);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_PHASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_osc_0_phase);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_0_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_osc_0_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_osc_1_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_OCTAVE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_osc_1_octave);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_KEY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_osc_1_key);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_PHASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_osc_1_phase);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_OSC_1_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_osc_1_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_PITCH_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_pitch_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_VOLUME:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_volume);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_lfo_0_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_lfo_0_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_lfo_0_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_0_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_lfo_0_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_OSCILLATOR:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_lfo_1_oscillator);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_lfo_1_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_DEPTH:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_lfo_1_depth);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_LFO_1_TUNING:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_lfo_1_tuning);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_ATTACK:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_env_0_attack);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_DECAY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_env_0_decay);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_SUSTAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_env_0_sustain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_RELEASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_env_0_release);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_env_0_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_env_0_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_0_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_env_0_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_ATTACK:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_env_1_attack);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_DECAY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_env_1_decay);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_SUSTAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_env_1_sustain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_RELEASE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_env_1_release);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_env_1_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_env_1_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_ENV_1_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_env_1_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_NOISE_FREQUENCY:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_noise_frequency);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_NOISE_GAIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_noise_gain);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_SYNTH_0_NOISE_SENDS:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, fx_modular_synth_audio->synth_0_noise_sends);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_modular_synth_audio_dispose(GObject *gobject)
{
  AgsFxModularSynthAudio *fx_modular_synth_audio;
  
  fx_modular_synth_audio = AGS_FX_MODULAR_SYNTH_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_modular_synth_audio_parent_class)->dispose(gobject);
}

void
ags_fx_modular_synth_audio_finalize(GObject *gobject)
{
  AgsFxModularSynthAudio *fx_modular_synth_audio;

  guint i;
  
  fx_modular_synth_audio = AGS_FX_MODULAR_SYNTH_AUDIO(gobject);

  
  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      ags_fx_modular_synth_audio_scope_data_free(fx_modular_synth_audio->scope_data[i]);
    }
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_modular_synth_audio_parent_class)->finalize(gobject);
}

void
ags_fx_modular_synth_audio_notify_audio_callback(GObject *gobject,
						 GParamSpec *pspec,
						 gpointer user_data)
{
  AgsAudio *audio;
  AgsFxModularSynthAudio *fx_modular_synth_audio;

  fx_modular_synth_audio = AGS_FX_MODULAR_SYNTH_AUDIO(gobject);

  /* get audio */
  audio = NULL;

  g_object_get(fx_modular_synth_audio,
	       "audio", &audio,
	       NULL);

  g_signal_connect_after((GObject *) audio, "set-audio-channels",
			 G_CALLBACK(ags_fx_modular_synth_audio_set_audio_channels_callback), fx_modular_synth_audio);

  if(audio != NULL){
    g_object_unref(audio);
  }
}

void
ags_fx_modular_synth_audio_notify_buffer_size_callback(GObject *gobject,
						       GParamSpec *pspec,
						       gpointer user_data)
{
  AgsFxModularSynthAudio *fx_modular_synth_audio;

  guint buffer_size;
  guint format;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_modular_synth_audio = AGS_FX_MODULAR_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_modular_synth_audio);

  /* get buffer size */
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  format =  AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  g_object_get(fx_modular_synth_audio,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxModularSynthAudioScopeData *scope_data;

    scope_data = fx_modular_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxModularSynthAudioChannelData *channel_data;

	AgsModularSynthUtil *modular_synth_0_util;
	
	gpointer destination;

	channel_data = scope_data->channel_data[j];

	modular_synth_0_util = channel_data->modular_synth_0_util;

	/* synth buffer */
	ags_stream_free(channel_data->synth_0_buffer);

	channel_data->synth_0_buffer = NULL;

	if(buffer_size > 0){
	  channel_data->synth_0_buffer = ags_stream_alloc(buffer_size,
							  format);
	}
	
	/* set buffer length */
	ags_modular_synth_util_set_buffer_length(modular_synth_0_util,
						 buffer_size);
      }
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_modular_synth_audio_notify_format_callback(GObject *gobject,
						  GParamSpec *pspec,
						  gpointer user_data)
{
  AgsFxModularSynthAudio *fx_modular_synth_audio;

  guint buffer_size;
  guint format;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_modular_synth_audio = AGS_FX_MODULAR_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_modular_synth_audio);

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  format =  AGS_SOUNDCARD_DEFAULT_FORMAT;

  g_object_get(fx_modular_synth_audio,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxModularSynthAudioScopeData *scope_data;

    scope_data = fx_modular_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxModularSynthAudioChannelData *channel_data;

	AgsModularSynthUtil *modular_synth_0_util;
	
	gpointer destination;
	
	channel_data = scope_data->channel_data[j];

	modular_synth_0_util = channel_data->modular_synth_0_util;

	ags_modular_synth_util_set_format(modular_synth_0_util,
					  format);

	/* synth buffer */
	ags_stream_free(channel_data->synth_0_buffer);

	channel_data->synth_0_buffer = NULL;

	if(buffer_size > 0){
	  channel_data->synth_0_buffer = ags_stream_alloc(buffer_size,
							  format);
	}
      }
    }
  }

  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_modular_synth_audio_notify_samplerate_callback(GObject *gobject,
						      GParamSpec *pspec,
						      gpointer user_data)
{
  AgsFxModularSynthAudio *fx_modular_synth_audio;

  guint samplerate;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_modular_synth_audio = AGS_FX_MODULAR_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_modular_synth_audio);

  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  g_object_get(fx_modular_synth_audio,
	       "samplerate", &samplerate,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxModularSynthAudioScopeData *scope_data;

    scope_data = fx_modular_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxModularSynthAudioChannelData *channel_data;

	AgsModularSynthUtil *modular_synth_0_util;

	channel_data = scope_data->channel_data[j];

	modular_synth_0_util = channel_data->modular_synth_0_util;

	ags_modular_synth_util_set_samplerate(modular_synth_0_util,
					      samplerate);
      }
    }
  }

  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_modular_synth_audio_set_audio_channels_callback(AgsAudio *audio,
						       guint audio_channels, guint audio_channels_old,
						       AgsFxModularSynthAudio *fx_modular_synth_audio)
{
  guint input_pads;
  guint output_port_count, input_port_count;
  guint buffer_size;
  guint format;
  guint samplerate;
  guint i, j, k;

  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_modular_synth_audio);

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
    AgsFxModularSynthAudioScopeData *scope_data;

    scope_data = fx_modular_synth_audio->scope_data[i];

    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      if(scope_data->audio_channels > audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxModularSynthAudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];
	
	  ags_fx_modular_synth_audio_channel_data_free(channel_data);
	}
      }
      
      if(scope_data->channel_data == NULL){
	scope_data->channel_data = (AgsFxModularSynthAudioChannelData **) g_malloc(audio_channels * sizeof(AgsFxModularSynthAudioChannelData *)); 
      }else{
	scope_data->channel_data = (AgsFxModularSynthAudioChannelData **) g_realloc(scope_data->channel_data,
										    audio_channels * sizeof(AgsFxModularSynthAudioChannelData *)); 
      }

      if(scope_data->audio_channels < audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxModularSynthAudioChannelData *channel_data;

	  AgsModularSynthUtil *modular_synth_0_util;

	  gpointer destination;  

	  channel_data =
	    scope_data->channel_data[j] = ags_fx_modular_synth_audio_channel_data_alloc();

	  modular_synth_0_util = channel_data->modular_synth_0_util;

	  channel_data->synth_0_buffer = ags_stream_alloc(buffer_size,
							  format);

	  ags_modular_synth_util_set_buffer_length(modular_synth_0_util,
						   buffer_size);
	  ags_modular_synth_util_set_format(channel_data->modular_synth_0_util,
					    format);		
	  ags_modular_synth_util_set_samplerate(channel_data->modular_synth_0_util,
						samplerate);

	  for(k = 0; k < AGS_SEQUENCER_MAX_MIDI_KEYS; k++){
	    AgsFxModularSynthAudioInputData *input_data;

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
 * ags_fx_modular_synth_audio_scope_data_alloc:
 * 
 * Allocate #AgsFxModularSynthAudioScopeData-struct
 * 
 * Returns: the new #AgsFxModularSynthAudioScopeData-struct
 * 
 * Since: 8.2.0
 */
AgsFxModularSynthAudioScopeData*
ags_fx_modular_synth_audio_scope_data_alloc()
{
  AgsFxModularSynthAudioScopeData *scope_data;

  scope_data = (AgsFxModularSynthAudioScopeData *) g_malloc(sizeof(AgsFxModularSynthAudioScopeData));

  g_rec_mutex_init(&(scope_data->strct_mutex));
  
  scope_data->parent = NULL;
  
  scope_data->audio_channels = 0;

  scope_data->channel_data = NULL;
  
  return(scope_data);
}

/**
 * ags_fx_modular_synth_audio_scope_data_free:
 * @scope_data: the #AgsFxModularSynthAudioScopeData-struct
 * 
 * Free @scope_data.
 * 
 * Since: 8.2.0
 */
void
ags_fx_modular_synth_audio_scope_data_free(AgsFxModularSynthAudioScopeData *scope_data)
{
  guint i;

  if(scope_data == NULL){
    return;
  }

  for(i = 0; i < scope_data->audio_channels; i++){
    ags_fx_modular_synth_audio_channel_data_free(scope_data->channel_data[i]);
  }

  g_free(scope_data);
}

/**
 * ags_fx_modular_synth_audio_channel_data_alloc:
 * 
 * Allocate #AgsFxModularSynthAudioChannelData-struct
 * 
 * Returns: the new #AgsFxModularSynthAudioChannelData-struct
 * 
 * Since: 8.2.0
 */
AgsFxModularSynthAudioChannelData*
ags_fx_modular_synth_audio_channel_data_alloc()
{
  AgsFxModularSynthAudioChannelData *channel_data;

  guint i;
  
  channel_data = (AgsFxModularSynthAudioChannelData *) g_malloc(sizeof(AgsFxModularSynthAudioChannelData));

  g_rec_mutex_init(&(channel_data->strct_mutex));

  channel_data->parent = NULL;

  channel_data->synth_0_buffer = NULL;

  /* modular synth util */
  channel_data->modular_synth_0_util = ags_modular_synth_util_alloc();

  return(channel_data);
}

/**
 * ags_fx_modular_synth_audio_channel_data_free:
 * @channel_data: the #AgsFxModularSynthAudioChannelData-struct
 * 
 * Free @channel_data.
 * 
 * Since: 8.2.0
 */
void
ags_fx_modular_synth_audio_channel_data_free(AgsFxModularSynthAudioChannelData *channel_data)
{
  guint i;

  if(channel_data == NULL){
    return;
  }

  /* modular synth util */
  ags_modular_synth_util_free(channel_data->modular_synth_0_util);
  
  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    ags_fx_modular_synth_audio_input_data_free(channel_data->input_data[i]);
  }

  g_free(channel_data);
}

/**
 * ags_fx_modular_synth_audio_input_data_alloc:
 * 
 * Allocate #AgsFxModularSynthAudioInputData-struct
 * 
 * Returns: the new #AgsFxModularSynthAudioInputData-struct
 * 
 * Since: 8.2.0
 */
AgsFxModularSynthAudioInputData*
ags_fx_modular_synth_audio_input_data_alloc()
{
  AgsFxModularSynthAudioInputData *input_data;

  input_data = (AgsFxModularSynthAudioInputData *) g_malloc(sizeof(AgsFxModularSynthAudioInputData));

  g_rec_mutex_init(&(input_data->strct_mutex));

  input_data->parent = NULL;

  input_data->key_on = 0;
  
  return(input_data);
}

/**
 * ags_fx_modular_synth_audio_input_data_free:
 * @input_data: the #AgsFxModularSynthAudioInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 8.2.0
 */
void
ags_fx_modular_synth_audio_input_data_free(AgsFxModularSynthAudioInputData *input_data)
{
  if(input_data == NULL){
    return;
  }

  g_free(input_data);
}

static AgsPluginPort*
ags_fx_modular_synth_audio_get_synth_0_osc_0_oscillator_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_osc_0_octave_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_osc_0_key_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_osc_0_phase_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_osc_0_volume_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_osc_1_oscillator_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_osc_1_octave_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_osc_1_key_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_osc_1_phase_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_osc_1_volume_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_pitch_tuning_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_volume_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_env_0_attack_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_env_0_decay_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_env_0_sustain_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_env_0_release_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_env_0_gain_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_env_0_frequency_plugin_port()
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
		      AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_modular_synth_audio_get_synth_0_env_0_sends_plugin_port()
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
		      (gfloat) 0xff);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_modular_synth_audio_get_synth_0_env_1_attack_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_env_1_decay_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_env_1_sustain_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_env_1_release_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_env_1_gain_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_env_1_frequency_plugin_port()
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
		      AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_modular_synth_audio_get_synth_0_env_1_sends_plugin_port()
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
		      (gfloat) 0xff);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_modular_synth_audio_get_synth_0_lfo_0_oscillator_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_lfo_0_frequency_plugin_port()
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
		      AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_modular_synth_audio_get_synth_0_lfo_0_depth_plugin_port()
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
		      AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_DEPTH);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_modular_synth_audio_get_synth_0_lfo_0_tuning_plugin_port()
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
		      AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_TUNING);
    g_value_set_float(plugin_port->lower_value,
		      -1200.0);
    g_value_set_float(plugin_port->upper_value,
		      1200.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_modular_synth_audio_get_synth_0_lfo_1_oscillator_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_lfo_1_frequency_plugin_port()
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
		      AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_FREQUENCY);
    g_value_set_float(plugin_port->lower_value,
		      0.01);
    g_value_set_float(plugin_port->upper_value,
		      16.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_modular_synth_audio_get_synth_0_lfo_1_depth_plugin_port()
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
		      AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_DEPTH);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_modular_synth_audio_get_synth_0_lfo_1_tuning_plugin_port()
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
		      AGS_MODULAR_SYNTH_UTIL_DEFAULT_LFO_TUNING);
    g_value_set_float(plugin_port->lower_value,
		      -1200.0);
    g_value_set_float(plugin_port->upper_value,
		      1200.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_modular_synth_audio_get_synth_0_noise_frequency_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_noise_gain_plugin_port()
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
ags_fx_modular_synth_audio_get_synth_0_noise_sends_plugin_port()
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
		      (gfloat) 0xff);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

/**
 * ags_fx_modular_synth_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxModularSynthAudio
 *
 * Returns: the new #AgsFxModularSynthAudio
 *
 * Since: 8.2.0
 */
AgsFxModularSynthAudio*
ags_fx_modular_synth_audio_new(AgsAudio *audio)
{
  AgsFxModularSynthAudio *fx_modular_synth_audio;

  fx_modular_synth_audio = (AgsFxModularSynthAudio *) g_object_new(AGS_TYPE_FX_MODULAR_SYNTH_AUDIO,
								   "audio", audio,
								   NULL);
  
  return(fx_modular_synth_audio);
}
