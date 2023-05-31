/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_sf2_synth_audio.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_fx_sf2_synth_audio_class_init(AgsFxSF2SynthAudioClass *fx_sf2_synth_audio);
void ags_fx_sf2_synth_audio_init(AgsFxSF2SynthAudio *fx_sf2_synth_audio);
void ags_fx_sf2_synth_audio_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_fx_sf2_synth_audio_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_fx_sf2_synth_audio_dispose(GObject *gobject);
void ags_fx_sf2_synth_audio_finalize(GObject *gobject);

void ags_fx_sf2_synth_audio_notify_audio_callback(GObject *gobject,
						  GParamSpec *pspec,
						  gpointer user_data);
void ags_fx_sf2_synth_audio_notify_buffer_size_callback(GObject *gobject,
							GParamSpec *pspec,
							gpointer user_data);
void ags_fx_sf2_synth_audio_notify_format_callback(GObject *gobject,
						   GParamSpec *pspec,
						   gpointer user_data);
void ags_fx_sf2_synth_audio_notify_samplerate_callback(GObject *gobject,
						       GParamSpec *pspec,
						       gpointer user_data);

void ags_fx_sf2_synth_audio_set_audio_channels_callback(AgsAudio *audio,
							guint audio_channels, guint audio_channels_old,
							AgsFxSF2SynthAudio *fx_sf2_synth_audio);

static AgsPluginPort* ags_fx_sf2_synth_audio_get_synth_octave_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_synth_key_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_synth_pitch_type_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_synth_volume_plugin_port();

static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_enabled_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_pitch_type_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_input_volume_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_output_volume_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_depth_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_mix_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_delay_plugin_port();

static AgsPluginPort* ags_fx_sf2_synth_audio_get_vibrato_enabled_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_vibrato_gain_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_vibrato_lfo_depth_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_vibrato_lfo_freq_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_vibrato_tuning_plugin_port();

/**
 * SECTION:ags_fx_sf2_synth_audio
 * @short_description: fx Soundfont2 synth audio
 * @title: AgsFxSF2SynthAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_sf2_synth_audio.h
 *
 * The #AgsFxSF2SynthAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_sf2_synth_audio_parent_class = NULL;

const gchar *ags_fx_sf2_synth_audio_plugin_name = "ags-fx-sf2-synth";

const gchar* ags_fx_sf2_synth_audio_specifier[] = {
  "./synth-octave[0]",
  "./synth-key[0]",
  "./synth-pitch-type[0]",
  "./synth-volume[0]",
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

const gchar* ags_fx_sf2_synth_audio_control_port[] = {
  "1/18",
  "2/18",
  "3/18",
  "4/18",
  "5/18",
  "6/18",
  "7/18",
  "8/18",
  "9/18",
  "10/18",
  "11/18",
  "12/18",
  "13/18",
  "14/18",
  "15/18",
  "16/18",
  "17/18",
  "18/18",
  NULL,
};

enum{
  PROP_0,
  PROP_SYNTH_OCTAVE,
  PROP_SYNTH_KEY,
  PROP_SYNTH_PITCH_TYPE,
  PROP_SYNTH_VOLUME,
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
ags_fx_sf2_synth_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_sf2_synth_audio = 0;

    static const GTypeInfo ags_fx_sf2_synth_audio_info = {
      sizeof (AgsFxSF2SynthAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_sf2_synth_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxSF2SynthAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_sf2_synth_audio_init,
    };

    ags_type_fx_sf2_synth_audio = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO,
							"AgsFxSF2SynthAudio",
							&ags_fx_sf2_synth_audio_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_sf2_synth_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_sf2_synth_audio_class_init(AgsFxSF2SynthAudioClass *fx_sf2_synth_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_sf2_synth_audio_parent_class = g_type_class_peek_parent(fx_sf2_synth_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_sf2_synth_audio;

  gobject->set_property = ags_fx_sf2_synth_audio_set_property;
  gobject->get_property = ags_fx_sf2_synth_audio_get_property;

  gobject->dispose = ags_fx_sf2_synth_audio_dispose;
  gobject->finalize = ags_fx_sf2_synth_audio_finalize;

  /* properties */

  /**
   * AgsFxSF2SynthAudio:synth-octave:
   *
   * The synth octave.
   * 
   * Since: 3.16.0
   */
  param_spec = g_param_spec_object("synth-octave",
				   i18n_pspec("synth octave of recall"),
				   i18n_pspec("The synth's octave"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_OCTAVE,
				  param_spec);

  /**
   * AgsFxSF2SynthAudio:synth-key:
   *
   * The synth key.
   * 
   * Since: 3.16.0
   */
  param_spec = g_param_spec_object("synth-key",
				   i18n_pspec("synth key of recall"),
				   i18n_pspec("The synth's key"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_KEY,
				  param_spec);

  /**
   * AgsFxSF2SynthAudio:synth-volume:
   *
   * The synth volume.
   * 
   * Since: 3.16.0
   */
  param_spec = g_param_spec_object("synth-volume",
				   i18n_pspec("synth volume of recall"),
				   i18n_pspec("The synth's volume"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_VOLUME,
				  param_spec);

  /**
   * AgsFxSF2SynthAudio:synth-pitch-type:
   *
   * The synth's pitch type.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_object("synth-pitch-type",
				   i18n_pspec("pitch type of synth"),
				   i18n_pspec("The pitch type"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SYNTH_PITCH_TYPE,
				  param_spec);

  /**
   * AgsFxSF2SynthAudio:chorus-enabled:
   *
   * The chorus enabled.
   * 
   * Since: 3.16.0
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
   * AgsFxSF2SynthAudio:chorus-pitch-type:
   *
   * The chorus's pitch type.
   * 
   * Since: 4.0.0
   */
  param_spec = g_param_spec_object("chorus-pitch-type",
				   i18n_pspec("pitch type of chorus"),
				   i18n_pspec("The pitch type"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHORUS_PITCH_TYPE,
				  param_spec);

  /**
   * AgsFxSF2SynthAudio:chorus-input-volume:
   *
   * The chorus input volume.
   * 
   * Since: 3.16.0
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
   * AgsFxSF2SynthAudio:chorus-output-volume:
   *
   * The chorus output volume.
   * 
   * Since: 3.16.0
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
   * AgsFxSF2SynthAudio:chorus-lfo-oscillator:
   *
   * The chorus lfo oscillator.
   * 
   * Since: 3.16.0
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
   * AgsFxSF2SynthAudio:chorus-lfo-frequency:
   *
   * The chorus lfo frequency.
   * 
   * Since: 3.16.0
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
   * AgsFxSF2SynthAudio:chorus-depth:
   *
   * The chorus depth.
   * 
   * Since: 3.16.0
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
   * AgsFxSF2SynthAudio:chorus-mix:
   *
   * The chorus mix.
   * 
   * Since: 3.16.0
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
   * AgsFxSF2SynthAudio:chorus-delay:
   *
   * The chorus delay.
   * 
   * Since: 3.16.0
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
   * AgsFxSF2SynthAudio:vibrato-enabled:
   *
   * The vibrato enabled.
   * 
   * Since: 5.2.0
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
   * AgsFxSF2SynthAudio:vibrato-gain:
   *
   * The vibrato gain.
   * 
   * Since: 5.2.0
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
   * AgsFxSF2SynthAudio:vibrato-lfo-depth:
   *
   * The vibrato LFO depth.
   * 
   * Since: 5.2.0
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
   * AgsFxSF2SynthAudio:vibrato-lfo-freq:
   *
   * The vibrato LFO freq.
   * 
   * Since: 5.2.0
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
   * AgsFxSF2SynthAudio:vibrato-tuning:
   *
   * The vibrato tuning.
   * 
   * Since: 5.2.0
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
ags_fx_sf2_synth_audio_init(AgsFxSF2SynthAudio *fx_sf2_synth_audio)
{
  guint i;
  guint position;

  g_signal_connect(fx_sf2_synth_audio, "notify::audio",
		   G_CALLBACK(ags_fx_sf2_synth_audio_notify_audio_callback), NULL);

  g_signal_connect(fx_sf2_synth_audio, "notify::buffer-size",
		   G_CALLBACK(ags_fx_sf2_synth_audio_notify_buffer_size_callback), NULL);

  g_signal_connect(fx_sf2_synth_audio, "notify::format",
		   G_CALLBACK(ags_fx_sf2_synth_audio_notify_format_callback), NULL);

  g_signal_connect(fx_sf2_synth_audio, "notify::samplerate",
		   G_CALLBACK(ags_fx_sf2_synth_audio_notify_samplerate_callback), NULL);

  AGS_RECALL(fx_sf2_synth_audio)->name = "ags-fx-sf2-synth";
  AGS_RECALL(fx_sf2_synth_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_sf2_synth_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_sf2_synth_audio)->xml_type = "ags-fx-sf2-synth-audio";

  position = 0;  

  /* synth octave */
  fx_sf2_synth_audio->synth_octave = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						   "specifier", ags_fx_sf2_synth_audio_specifier[position],
						   "control-port", ags_fx_sf2_synth_audio_control_port[position],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_sf2_synth_audio->synth_octave->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_sf2_synth_audio->synth_octave,
	       "plugin-port", ags_fx_sf2_synth_audio_get_synth_octave_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->synth_octave);

  position++;
  
  /* synth key */
  fx_sf2_synth_audio->synth_key = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						"specifier", ags_fx_sf2_synth_audio_specifier[position],
						"control-port", ags_fx_sf2_synth_audio_control_port[position],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_FLOAT,
						"port-value-size", sizeof(gfloat),
						"port-value-length", 1,
						NULL);
  
  fx_sf2_synth_audio->synth_key->port_value.ags_port_float = (gfloat) 2.0;

  g_object_set(fx_sf2_synth_audio->synth_key,
	       "plugin-port", ags_fx_sf2_synth_audio_get_synth_key_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->synth_key);

  position++;
  
  /* synth pitch type */
  fx_sf2_synth_audio->synth_pitch_type = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						      "specifier", ags_fx_sf2_synth_audio_specifier[position],
						      "control-port", ags_fx_sf2_synth_audio_control_port[position],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_sf2_synth_audio->synth_pitch_type->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_sf2_synth_audio->synth_pitch_type,
	       "plugin-port", ags_fx_sf2_synth_audio_get_synth_pitch_type_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->synth_pitch_type);

  position++;
  
  /* synth volume */
  fx_sf2_synth_audio->synth_volume = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						   "specifier", ags_fx_sf2_synth_audio_specifier[position],
						   "control-port", ags_fx_sf2_synth_audio_control_port[position],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_sf2_synth_audio->synth_volume->port_value.ags_port_float = (gfloat) 0.5;

  g_object_set(fx_sf2_synth_audio->synth_volume,
	       "plugin-port", ags_fx_sf2_synth_audio_get_synth_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->synth_volume);

  position++;  
  
  /* chorus enabled */
  fx_sf2_synth_audio->chorus_enabled = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						   "specifier", ags_fx_sf2_synth_audio_specifier[position],
						   "control-port", ags_fx_sf2_synth_audio_control_port[position],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_sf2_synth_audio->chorus_enabled->port_value.ags_port_float = (gfloat) FALSE;

  g_object_set(fx_sf2_synth_audio->chorus_enabled,
	       "plugin-port", ags_fx_sf2_synth_audio_get_chorus_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->chorus_enabled);

  position++;
  
  /* chorus pitch type */
  fx_sf2_synth_audio->chorus_pitch_type = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						      "specifier", ags_fx_sf2_synth_audio_specifier[position],
						      "control-port", ags_fx_sf2_synth_audio_control_port[position],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_sf2_synth_audio->chorus_pitch_type->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_sf2_synth_audio->chorus_pitch_type,
	       "plugin-port", ags_fx_sf2_synth_audio_get_chorus_pitch_type_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->chorus_pitch_type);

  position++;
  
  /* chorus input volume */
  fx_sf2_synth_audio->chorus_input_volume = g_object_new(AGS_TYPE_PORT,
							"plugin-name", ags_fx_sf2_synth_audio_plugin_name,
							"specifier", ags_fx_sf2_synth_audio_specifier[position],
							"control-port", ags_fx_sf2_synth_audio_control_port[position],
							"port-value-is-pointer", FALSE,
							"port-value-type", G_TYPE_FLOAT,
							"port-value-size", sizeof(gfloat),
							"port-value-length", 1,
							NULL);
  
  fx_sf2_synth_audio->chorus_input_volume->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_sf2_synth_audio->chorus_input_volume,
	       "plugin-port", ags_fx_sf2_synth_audio_get_chorus_input_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->chorus_input_volume);

  position++;
  
  /* chorus output volume */
  fx_sf2_synth_audio->chorus_output_volume = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
							 "specifier", ags_fx_sf2_synth_audio_specifier[position],
							 "control-port", ags_fx_sf2_synth_audio_control_port[position],
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_sf2_synth_audio->chorus_output_volume->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_sf2_synth_audio->chorus_output_volume,
	       "plugin-port", ags_fx_sf2_synth_audio_get_chorus_output_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->chorus_output_volume);

  position++;
  
  /* chorus LFO oscillator */
  fx_sf2_synth_audio->chorus_lfo_oscillator = g_object_new(AGS_TYPE_PORT,
							  "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
							  "specifier", ags_fx_sf2_synth_audio_specifier[position],
							  "control-port", ags_fx_sf2_synth_audio_control_port[position],
							  "port-value-is-pointer", FALSE,
							  "port-value-type", G_TYPE_FLOAT,
							  "port-value-size", sizeof(gfloat),
							  "port-value-length", 1,
							  NULL);
  
  fx_sf2_synth_audio->chorus_lfo_oscillator->port_value.ags_port_float = (gfloat) AGS_SYNTH_OSCILLATOR_SIN;

  g_object_set(fx_sf2_synth_audio->chorus_lfo_oscillator,
	       "plugin-port", ags_fx_sf2_synth_audio_get_chorus_lfo_oscillator_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->chorus_lfo_oscillator);

  position++;
  
  /* chorus LFO frequency */
  fx_sf2_synth_audio->chorus_lfo_frequency = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
							 "specifier", ags_fx_sf2_synth_audio_specifier[position],
							 "control-port", ags_fx_sf2_synth_audio_control_port[position],
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_FLOAT,
							 "port-value-size", sizeof(gfloat),
							 "port-value-length", 1,
							 NULL);
  
  fx_sf2_synth_audio->chorus_lfo_frequency->port_value.ags_port_float = (gfloat) 10.0;

  g_object_set(fx_sf2_synth_audio->chorus_lfo_frequency,
	       "plugin-port", ags_fx_sf2_synth_audio_get_chorus_lfo_frequency_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->chorus_lfo_frequency);

  position++;
  
  /* chorus depth */
  fx_sf2_synth_audio->chorus_depth = g_object_new(AGS_TYPE_PORT,
						 "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						 "specifier", ags_fx_sf2_synth_audio_specifier[position],
						 "control-port", ags_fx_sf2_synth_audio_control_port[position],
						 "port-value-is-pointer", FALSE,
						 "port-value-type", G_TYPE_FLOAT,
						 "port-value-size", sizeof(gfloat),
						 "port-value-length", 1,
						 NULL);
  
  fx_sf2_synth_audio->chorus_depth->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_sf2_synth_audio->chorus_depth,
	       "plugin-port", ags_fx_sf2_synth_audio_get_chorus_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->chorus_depth);

  position++;
  
  /* chorus mix */
  fx_sf2_synth_audio->chorus_mix = g_object_new(AGS_TYPE_PORT,
					       "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
					       "specifier", ags_fx_sf2_synth_audio_specifier[position],
					       "control-port", ags_fx_sf2_synth_audio_control_port[position],
					       "port-value-is-pointer", FALSE,
					       "port-value-type", G_TYPE_FLOAT,
					       "port-value-size", sizeof(gfloat),
					       "port-value-length", 1,
					       NULL);
  
  fx_sf2_synth_audio->chorus_mix->port_value.ags_port_float = (gfloat) 0.5;

  g_object_set(fx_sf2_synth_audio->chorus_mix,
	       "plugin-port", ags_fx_sf2_synth_audio_get_chorus_mix_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->chorus_mix);

  position++;
  
  /* chorus delay */
  fx_sf2_synth_audio->chorus_delay = g_object_new(AGS_TYPE_PORT,
						 "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						 "specifier", ags_fx_sf2_synth_audio_specifier[position],
						 "control-port", ags_fx_sf2_synth_audio_control_port[position],
						 "port-value-is-pointer", FALSE,
						 "port-value-type", G_TYPE_FLOAT,
						 "port-value-size", sizeof(gfloat),
						 "port-value-length", 1,
						 NULL);
  
  fx_sf2_synth_audio->chorus_delay->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_sf2_synth_audio->chorus_delay,
	       "plugin-port", ags_fx_sf2_synth_audio_get_chorus_delay_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->chorus_delay);

  /* vibrato enabled */
  fx_sf2_synth_audio->vibrato_enabled = g_object_new(AGS_TYPE_PORT,
						     "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						     "specifier", ags_fx_sf2_synth_audio_specifier[position],
						     "control-port", ags_fx_sf2_synth_audio_control_port[position],
						     "port-value-is-pointer", FALSE,
						     "port-value-type", G_TYPE_FLOAT,
						     "port-value-size", sizeof(gfloat),
						     "port-value-length", 1,
						     NULL);
  
  fx_sf2_synth_audio->vibrato_enabled->port_value.ags_port_float = (gfloat) TRUE;

  g_object_set(fx_sf2_synth_audio->vibrato_enabled,
	       "plugin-port", ags_fx_sf2_synth_audio_get_vibrato_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->vibrato_enabled);

  position++;

  /* vibrato gain */
  fx_sf2_synth_audio->vibrato_gain = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						  "specifier", ags_fx_sf2_synth_audio_specifier[position],
						  "control-port", ags_fx_sf2_synth_audio_control_port[position],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_FLOAT,
						  "port-value-size", sizeof(gfloat),
						  "port-value-length", 1,
						  NULL);
  
  fx_sf2_synth_audio->vibrato_gain->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_sf2_synth_audio->vibrato_gain,
	       "plugin-port", ags_fx_sf2_synth_audio_get_vibrato_gain_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->vibrato_gain);

  position++;
  
  /* vibrato LFO depth */
  fx_sf2_synth_audio->vibrato_lfo_depth = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						       "specifier", ags_fx_sf2_synth_audio_specifier[position],
						       "control-port", ags_fx_sf2_synth_audio_control_port[position],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_FLOAT,
						       "port-value-size", sizeof(gfloat),
						       "port-value-length", 1,
						       NULL);
  
  fx_sf2_synth_audio->vibrato_lfo_depth->port_value.ags_port_float = (gfloat) 1.0;

  g_object_set(fx_sf2_synth_audio->vibrato_lfo_depth,
	       "plugin-port", ags_fx_sf2_synth_audio_get_vibrato_lfo_depth_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->vibrato_lfo_depth);

  position++;
  
  /* vibrato LFO freq */
  fx_sf2_synth_audio->vibrato_lfo_freq = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						      "specifier", ags_fx_sf2_synth_audio_specifier[position],
						      "control-port", ags_fx_sf2_synth_audio_control_port[position],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_FLOAT,
						      "port-value-size", sizeof(gfloat),
						      "port-value-length", 1,
						      NULL);
  
  fx_sf2_synth_audio->vibrato_lfo_freq->port_value.ags_port_float = (gfloat) 8.172;

  g_object_set(fx_sf2_synth_audio->vibrato_lfo_freq,
	       "plugin-port", ags_fx_sf2_synth_audio_get_vibrato_lfo_freq_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->vibrato_lfo_freq);

  position++;

  /* vibrato tuning */
  fx_sf2_synth_audio->vibrato_tuning = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						    "specifier", ags_fx_sf2_synth_audio_specifier[position],
						    "control-port", ags_fx_sf2_synth_audio_control_port[position],
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_FLOAT,
						    "port-value-size", sizeof(gfloat),
						    "port-value-length", 1,
						    NULL);
  
  fx_sf2_synth_audio->vibrato_tuning->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_sf2_synth_audio->vibrato_tuning,
	       "plugin-port", ags_fx_sf2_synth_audio_get_vibrato_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->vibrato_tuning);

  position++;
  
  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      fx_sf2_synth_audio->scope_data[i] = ags_fx_sf2_synth_audio_scope_data_alloc();
      
      fx_sf2_synth_audio->scope_data[i]->parent = fx_sf2_synth_audio;
    }else{
      fx_sf2_synth_audio->scope_data[i] = NULL;
    }
  }
}

void
ags_fx_sf2_synth_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsFxSF2SynthAudio *fx_sf2_synth_audio;

  GRecMutex *recall_mutex;

  fx_sf2_synth_audio = AGS_FX_SF2_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_sf2_synth_audio);

  switch(prop_id){
  case PROP_SYNTH_OCTAVE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->synth_octave){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->synth_octave != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->synth_octave));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->synth_octave = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_KEY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->synth_key){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->synth_key != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->synth_key));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->synth_key = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_VOLUME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->synth_volume){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->synth_volume != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->synth_volume));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->synth_volume = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_PITCH_TYPE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->synth_pitch_type){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->synth_pitch_type != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->synth_pitch_type));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->synth_pitch_type = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->chorus_enabled){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->chorus_enabled != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->chorus_enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_PITCH_TYPE:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->chorus_pitch_type){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->chorus_pitch_type != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_pitch_type));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->chorus_pitch_type = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_INPUT_VOLUME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->chorus_input_volume){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->chorus_input_volume != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_input_volume));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->chorus_input_volume = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_OUTPUT_VOLUME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->chorus_output_volume){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->chorus_output_volume != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_output_volume));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->chorus_output_volume = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_LFO_OSCILLATOR:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->chorus_lfo_oscillator){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->chorus_lfo_oscillator != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_lfo_oscillator));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->chorus_lfo_oscillator = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_LFO_FREQUENCY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->chorus_lfo_frequency){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->chorus_lfo_frequency != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_lfo_frequency));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->chorus_lfo_frequency = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_DEPTH:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->chorus_depth){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->chorus_depth != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_depth));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->chorus_depth = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_MIX:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->chorus_mix){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->chorus_mix != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_mix));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->chorus_mix = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_DELAY:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->chorus_delay){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->chorus_delay != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_delay));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->chorus_delay = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_VIBRATO_ENABLED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->vibrato_enabled){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->vibrato_enabled != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->vibrato_enabled));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->vibrato_enabled = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_VIBRATO_GAIN:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->vibrato_gain){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->vibrato_gain != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->vibrato_gain));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->vibrato_gain = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_VIBRATO_LFO_DEPTH:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->vibrato_lfo_depth){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->vibrato_lfo_depth != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->vibrato_lfo_depth));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->vibrato_lfo_depth = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_VIBRATO_LFO_FREQ:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->vibrato_lfo_freq){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->vibrato_lfo_freq != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->vibrato_lfo_freq));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->vibrato_lfo_freq = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_VIBRATO_TUNING:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->vibrato_tuning){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->vibrato_tuning != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->vibrato_tuning));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->vibrato_tuning = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_sf2_synth_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsFxSF2SynthAudio *fx_sf2_synth_audio;

  GRecMutex *recall_mutex;

  fx_sf2_synth_audio = AGS_FX_SF2_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_sf2_synth_audio);

  switch(prop_id){
  case PROP_SYNTH_OCTAVE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->synth_octave);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_KEY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->synth_key);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_VOLUME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->synth_volume);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_SYNTH_PITCH_TYPE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->synth_pitch_type);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_VIBRATO_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->vibrato_enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_VIBRATO_GAIN:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->vibrato_gain);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_VIBRATO_LFO_DEPTH:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->vibrato_lfo_depth);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_VIBRATO_LFO_FREQ:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->vibrato_lfo_freq);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_VIBRATO_TUNING:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->vibrato_tuning);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_ENABLED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->chorus_enabled);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_PITCH_TYPE:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->chorus_pitch_type);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_INPUT_VOLUME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->chorus_input_volume);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_OUTPUT_VOLUME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->chorus_output_volume);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_LFO_OSCILLATOR:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->chorus_lfo_oscillator);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_LFO_FREQUENCY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->chorus_lfo_frequency);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_DEPTH:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->chorus_depth);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_MIX:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->chorus_mix);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_CHORUS_DELAY:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->chorus_delay);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_sf2_synth_audio_dispose(GObject *gobject)
{
  AgsFxSF2SynthAudio *fx_sf2_synth_audio;
  
  fx_sf2_synth_audio = AGS_FX_SF2_SYNTH_AUDIO(gobject);

  /* synth octave */
  if(fx_sf2_synth_audio->synth_octave != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->synth_octave));

    fx_sf2_synth_audio->synth_octave = NULL;
  }

  /* synth key */
  if(fx_sf2_synth_audio->synth_key != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->synth_key));

    fx_sf2_synth_audio->synth_key = NULL;
  }

  /* synth pitch type */
  if(fx_sf2_synth_audio->synth_pitch_type != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->synth_pitch_type));

    fx_sf2_synth_audio->synth_pitch_type = NULL;
  }

  /* synth volume */
  if(fx_sf2_synth_audio->synth_volume != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->synth_volume));

    fx_sf2_synth_audio->synth_volume = NULL;
  }

  /* chorus enabled */
  if(fx_sf2_synth_audio->chorus_enabled != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_enabled));

    fx_sf2_synth_audio->chorus_enabled = NULL;
  }

  /* chorus pitch type */
  if(fx_sf2_synth_audio->chorus_pitch_type != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_pitch_type));

    fx_sf2_synth_audio->chorus_pitch_type = NULL;
  }

  /* chorus input volume */
  if(fx_sf2_synth_audio->chorus_input_volume != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_input_volume));

    fx_sf2_synth_audio->chorus_input_volume = NULL;
  }

  /* chorus output volume */
  if(fx_sf2_synth_audio->chorus_output_volume != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_output_volume));

    fx_sf2_synth_audio->chorus_output_volume = NULL;
  }

  /* chorus lfo oscillator */
  if(fx_sf2_synth_audio->chorus_lfo_oscillator != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_lfo_oscillator));

    fx_sf2_synth_audio->chorus_lfo_oscillator = NULL;
  }

  /* chorus lfo frequency */
  if(fx_sf2_synth_audio->chorus_lfo_frequency != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_lfo_frequency));

    fx_sf2_synth_audio->chorus_lfo_frequency = NULL;
  }

  /* chorus depth */
  if(fx_sf2_synth_audio->chorus_depth != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_depth));

    fx_sf2_synth_audio->chorus_depth = NULL;
  }

  /* chorus mix */
  if(fx_sf2_synth_audio->chorus_mix != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_mix));

    fx_sf2_synth_audio->chorus_mix = NULL;
  }

  /* chorus delay */
  if(fx_sf2_synth_audio->chorus_delay != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_delay));

    fx_sf2_synth_audio->chorus_delay = NULL;
  }

  /* vibrato enabled */
  if(fx_sf2_synth_audio->vibrato_enabled != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->vibrato_enabled));

    fx_sf2_synth_audio->vibrato_enabled = NULL;
  }

  /* vibrato gain */
  if(fx_sf2_synth_audio->vibrato_gain != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->vibrato_gain));

    fx_sf2_synth_audio->vibrato_gain = NULL;
  }
  
  /* vibrato LFO depth */
  if(fx_sf2_synth_audio->vibrato_lfo_depth != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->vibrato_lfo_depth));

    fx_sf2_synth_audio->vibrato_lfo_depth = NULL;
  }

  /* vibrato LFO freq */
  if(fx_sf2_synth_audio->vibrato_lfo_freq != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->vibrato_lfo_freq));

    fx_sf2_synth_audio->vibrato_lfo_freq = NULL;
  }

  /* vibrato tuning */
  if(fx_sf2_synth_audio->vibrato_tuning != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->vibrato_tuning));

    fx_sf2_synth_audio->vibrato_tuning = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_sf2_synth_audio_parent_class)->dispose(gobject);
}

void
ags_fx_sf2_synth_audio_finalize(GObject *gobject)
{
  AgsFxSF2SynthAudio *fx_sf2_synth_audio;

  guint i;
  
  fx_sf2_synth_audio = AGS_FX_SF2_SYNTH_AUDIO(gobject);

  /* synth octave */
  if(fx_sf2_synth_audio->synth_octave != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->synth_octave));
  }

  /* synth key */
  if(fx_sf2_synth_audio->synth_key != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->synth_key));
  }

  /* synth pitch type */
  if(fx_sf2_synth_audio->synth_pitch_type != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->synth_pitch_type));
  }

  /* synth volume */
  if(fx_sf2_synth_audio->synth_volume != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->synth_volume));
  }

  /* chorus enabled */
  if(fx_sf2_synth_audio->chorus_enabled != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_enabled));
  }

  /* chorus pitch type */
  if(fx_sf2_synth_audio->chorus_pitch_type != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_pitch_type));
  }

  /* chorus input volume */
  if(fx_sf2_synth_audio->chorus_input_volume != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_input_volume));
  }

  /* chorus output volume */
  if(fx_sf2_synth_audio->chorus_output_volume != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_output_volume));
  }

  /* chorus lfo oscillator */
  if(fx_sf2_synth_audio->chorus_lfo_oscillator != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_lfo_oscillator));
  }

  /* chorus lfo frequency */
  if(fx_sf2_synth_audio->chorus_lfo_frequency != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_lfo_frequency));
  }

  /* chorus depth */
  if(fx_sf2_synth_audio->chorus_depth != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_depth));
  }

  /* chorus mix */
  if(fx_sf2_synth_audio->chorus_mix != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_mix));
  }

  /* chorus delay */
  if(fx_sf2_synth_audio->chorus_delay != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->chorus_delay));
  }

  /* vibrato enabled */
  if(fx_sf2_synth_audio->vibrato_enabled != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->vibrato_enabled));
  }

  /* vibrato gain */
  if(fx_sf2_synth_audio->vibrato_gain != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->vibrato_gain));
  }

  /* vibrato LFO depth */
  if(fx_sf2_synth_audio->vibrato_lfo_depth != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->vibrato_lfo_depth));
  }

  /* vibrato LFO freq */
  if(fx_sf2_synth_audio->vibrato_lfo_freq != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->vibrato_lfo_freq));
  }

  /* vibrato tuning */
  if(fx_sf2_synth_audio->vibrato_tuning != NULL){
    g_object_unref(G_OBJECT(fx_sf2_synth_audio->vibrato_tuning));
  }
  
  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      ags_fx_sf2_synth_audio_scope_data_free(fx_sf2_synth_audio->scope_data[i]);
    }
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_sf2_synth_audio_parent_class)->finalize(gobject);
}

void
ags_fx_sf2_synth_audio_notify_audio_callback(GObject *gobject,
					    GParamSpec *pspec,
					    gpointer user_data)
{
  AgsAudio *audio;
  AgsFxSF2SynthAudio *fx_sf2_synth_audio;

  fx_sf2_synth_audio = AGS_FX_SF2_SYNTH_AUDIO(gobject);

  /* get audio */
  audio = NULL;

  g_object_get(fx_sf2_synth_audio,
	       "audio", &audio,
	       NULL);

  g_signal_connect_after((GObject *) audio, "set-audio-channels",
			 G_CALLBACK(ags_fx_sf2_synth_audio_set_audio_channels_callback), fx_sf2_synth_audio);

  if(audio != NULL){
    g_object_unref(audio);
  }
}

void
ags_fx_sf2_synth_audio_notify_buffer_size_callback(GObject *gobject,
						  GParamSpec *pspec,
						  gpointer user_data)
{
  AgsFxSF2SynthAudio *fx_sf2_synth_audio;

  guint buffer_size;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_sf2_synth_audio = AGS_FX_SF2_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_sf2_synth_audio);

  /* get buffer size */
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  g_object_get(fx_sf2_synth_audio,
	       "buffer-size", &buffer_size,
	       NULL);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxSF2SynthAudioScopeData *scope_data;

    scope_data = fx_sf2_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxSF2SynthAudioChannelData *channel_data;

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
	ags_sf2_synth_util_set_buffer_length(channel_data->synth,
					     buffer_size);

	ags_chorus_util_set_buffer_length(channel_data->chorus_util,
					  buffer_size);
      }
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_sf2_synth_audio_notify_format_callback(GObject *gobject,
					     GParamSpec *pspec,
					     gpointer user_data)
{
  AgsFxSF2SynthAudio *fx_sf2_synth_audio;

  guint format;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_sf2_synth_audio = AGS_FX_SF2_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_sf2_synth_audio);

  format =  AGS_SOUNDCARD_DEFAULT_FORMAT;

  g_object_get(fx_sf2_synth_audio,
	       "format", &format,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxSF2SynthAudioScopeData *scope_data;

    scope_data = fx_sf2_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxSF2SynthAudioChannelData *channel_data;

	gpointer destination;

	channel_data = scope_data->channel_data[j];

	ags_sf2_synth_util_set_format(channel_data->synth,
				      format);

	/* free chorus destination */
	destination = ags_chorus_util_get_destination(channel_data->chorus_util);
	
	ags_stream_free(destination);

	/* alloc chorus destination */
	destination = ags_stream_alloc(ags_chorus_util_get_buffer_length(channel_data->chorus_util),
				       format);
	
	ags_chorus_util_set_destination(channel_data->chorus_util,
					destination);

	ags_chorus_util_set_format(channel_data->chorus_util,
				   format);
      }
    }
  }

  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_sf2_synth_audio_notify_samplerate_callback(GObject *gobject,
						 GParamSpec *pspec,
						 gpointer user_data)
{
  AgsFxSF2SynthAudio *fx_sf2_synth_audio;

  guint samplerate;
  guint i, j;
  
  GRecMutex *recall_mutex;
  
  fx_sf2_synth_audio = AGS_FX_SF2_SYNTH_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_sf2_synth_audio);

  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  g_object_get(fx_sf2_synth_audio,
	       "samplerate", &samplerate,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxSF2SynthAudioScopeData *scope_data;

    scope_data = fx_sf2_synth_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxSF2SynthAudioChannelData *channel_data;

	channel_data = scope_data->channel_data[j];

	ags_sf2_synth_util_set_samplerate(channel_data->synth,
					  samplerate);

	ags_chorus_util_set_samplerate(channel_data->chorus_util,
				       samplerate);
      }
    }
  }

  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_sf2_synth_audio_set_audio_channels_callback(AgsAudio *audio,
						  guint audio_channels, guint audio_channels_old,
						  AgsFxSF2SynthAudio *fx_sf2_synth_audio)
{
  guint input_pads;
  guint output_port_count, input_port_count;
  guint buffer_size;
  guint format;
  guint samplerate;
  guint i, j, k;

  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_sf2_synth_audio);

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
    AgsFxSF2SynthAudioScopeData *scope_data;

    scope_data = fx_sf2_synth_audio->scope_data[i];

    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      if(scope_data->audio_channels > audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxSF2SynthAudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];
	
	  ags_fx_sf2_synth_audio_channel_data_free(channel_data);
	}
      }
      
      if(scope_data->channel_data == NULL){
	scope_data->channel_data = (AgsFxSF2SynthAudioChannelData **) g_malloc(audio_channels * sizeof(AgsFxSF2SynthAudioChannelData *)); 
      }else{
	scope_data->channel_data = (AgsFxSF2SynthAudioChannelData **) g_realloc(scope_data->channel_data,
									     audio_channels * sizeof(AgsFxSF2SynthAudioChannelData *)); 
      }

      if(scope_data->audio_channels < audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxSF2SynthAudioChannelData *channel_data;

	  gpointer destination;  

	  channel_data =
	    scope_data->channel_data[j] = ags_fx_sf2_synth_audio_channel_data_alloc();

	  ags_sf2_synth_util_set_buffer_length(channel_data->synth,
					       buffer_size);
	  ags_sf2_synth_util_set_format(channel_data->synth,
					format);		
	  ags_sf2_synth_util_set_samplerate(channel_data->synth,
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
	    AgsFxSF2SynthAudioInputData *input_data;

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
 * ags_fx_sf2_synth_audio_scope_data_alloc:
 * 
 * Allocate #AgsFxSF2SynthAudioScopeData-struct
 * 
 * Returns: the new #AgsFxSF2SynthAudioScopeData-struct
 * 
 * Since: 3.16.0
 */
AgsFxSF2SynthAudioScopeData*
ags_fx_sf2_synth_audio_scope_data_alloc()
{
  AgsFxSF2SynthAudioScopeData *scope_data;

  scope_data = (AgsFxSF2SynthAudioScopeData *) g_malloc(sizeof(AgsFxSF2SynthAudioScopeData));

  g_rec_mutex_init(&(scope_data->strct_mutex));
  
  scope_data->parent = NULL;
  
  scope_data->audio_channels = 0;

  scope_data->channel_data = NULL;
  
  return(scope_data);
}

/**
 * ags_fx_sf2_synth_audio_scope_data_free:
 * @scope_data: the #AgsFxSF2SynthAudioScopeData-struct
 * 
 * Free @scope_data.
 * 
 * Since: 3.16.0
 */
void
ags_fx_sf2_synth_audio_scope_data_free(AgsFxSF2SynthAudioScopeData *scope_data)
{
  guint i;

  if(scope_data == NULL){
    return;
  }

  for(i = 0; i < scope_data->audio_channels; i++){
    ags_fx_sf2_synth_audio_channel_data_free(scope_data->channel_data[i]);
  }

  g_free(scope_data);
}

/**
 * ags_fx_sf2_synth_audio_channel_data_alloc:
 * 
 * Allocate #AgsFxSF2SynthAudioChannelData-struct
 * 
 * Returns: the new #AgsFxSF2SynthAudioChannelData-struct
 * 
 * Since: 3.16.0
 */
AgsFxSF2SynthAudioChannelData*
ags_fx_sf2_synth_audio_channel_data_alloc()
{
  AgsFxSF2SynthAudioChannelData *channel_data;

  guint i;
  
  channel_data = (AgsFxSF2SynthAudioChannelData *) g_malloc(sizeof(AgsFxSF2SynthAudioChannelData));

  g_rec_mutex_init(&(channel_data->strct_mutex));

  channel_data->parent = NULL;

  /* sf2 synth util */
  channel_data->synth = ags_sf2_synth_util_alloc();

  /* chorus util */
  channel_data->chorus_util = ags_chorus_util_alloc();
  
  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    channel_data->input_data[i] = ags_fx_sf2_synth_audio_input_data_alloc();

    channel_data->input_data[i]->parent = channel_data;
  }

  return(channel_data);
}

/**
 * ags_fx_sf2_synth_audio_channel_data_free:
 * @channel_data: the #AgsFxSF2SynthAudioChannelData-struct
 * 
 * Free @channel_data.
 * 
 * Since: 3.16.0
 */
void
ags_fx_sf2_synth_audio_channel_data_free(AgsFxSF2SynthAudioChannelData *channel_data)
{
  guint i;

  if(channel_data == NULL){
    return;
  }

  /* SF2 synth util */
  ags_sf2_synth_util_free(channel_data->synth);

  /* chorus util */
  ags_chorus_util_free(channel_data->chorus_util);
  
  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    ags_fx_sf2_synth_audio_input_data_free(channel_data->input_data[i]);
  }

  g_free(channel_data);
}

/**
 * ags_fx_sf2_synth_audio_input_data_alloc:
 * 
 * Allocate #AgsFxSF2SynthAudioInputData-struct
 * 
 * Returns: the new #AgsFxSF2SynthAudioInputData-struct
 * 
 * Since: 3.16.0
 */
AgsFxSF2SynthAudioInputData*
ags_fx_sf2_synth_audio_input_data_alloc()
{
  AgsFxSF2SynthAudioInputData *input_data;

  input_data = (AgsFxSF2SynthAudioInputData *) g_malloc(sizeof(AgsFxSF2SynthAudioInputData));

  g_rec_mutex_init(&(input_data->strct_mutex));

  input_data->parent = NULL;

  input_data->key_on = 0;
  
  return(input_data);
}

/**
 * ags_fx_sf2_synth_audio_input_data_free:
 * @input_data: the #AgsFxSF2SynthAudioInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 3.16.0
 */
void
ags_fx_sf2_synth_audio_input_data_free(AgsFxSF2SynthAudioInputData *input_data)
{
  if(input_data == NULL){
    return;
  }

  g_free(input_data);
}

static AgsPluginPort*
ags_fx_sf2_synth_audio_get_synth_octave_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

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
		      -12.0);
    g_value_set_float(plugin_port->upper_value,
		      12.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_sf2_synth_audio_get_synth_key_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

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
ags_fx_sf2_synth_audio_get_synth_volume_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

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
ags_fx_sf2_synth_audio_get_synth_pitch_type_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

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
		      4.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_sf2_synth_audio_get_chorus_enabled_plugin_port()
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
ags_fx_sf2_synth_audio_get_chorus_pitch_type_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

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
		      4.0);
  }

  g_mutex_unlock(&mutex);
    
  return(plugin_port);
}

static AgsPluginPort*
ags_fx_sf2_synth_audio_get_chorus_input_volume_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

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
ags_fx_sf2_synth_audio_get_chorus_output_volume_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

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
ags_fx_sf2_synth_audio_get_chorus_lfo_oscillator_plugin_port()
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
ags_fx_sf2_synth_audio_get_chorus_lfo_frequency_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

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
ags_fx_sf2_synth_audio_get_chorus_depth_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

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
ags_fx_sf2_synth_audio_get_chorus_mix_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

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
ags_fx_sf2_synth_audio_get_chorus_delay_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

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
ags_fx_sf2_synth_audio_get_vibrato_enabled_plugin_port()
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
ags_fx_sf2_synth_audio_get_vibrato_gain_plugin_port()
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
ags_fx_sf2_synth_audio_get_vibrato_lfo_depth_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

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
ags_fx_sf2_synth_audio_get_vibrato_lfo_freq_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

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
ags_fx_sf2_synth_audio_get_vibrato_tuning_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

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
 * ags_fx_sf2_synth_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxSF2SynthAudio
 *
 * Returns: the new #AgsFxSF2SynthAudio
 *
 * Since: 3.16.0
 */
AgsFxSF2SynthAudio*
ags_fx_sf2_synth_audio_new(AgsAudio *audio)
{
  AgsFxSF2SynthAudio *fx_sf2_synth_audio;

  fx_sf2_synth_audio = (AgsFxSF2SynthAudio *) g_object_new(AGS_TYPE_FX_SF2_SYNTH_AUDIO,
							   "audio", audio,
							   NULL);
  
  return(fx_sf2_synth_audio);
}
