/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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
static AgsPluginPort* ags_fx_sf2_synth_audio_get_synth_volume_plugin_port();

static AgsPluginPort* ags_fx_sf2_synth_audio_get_pitch_tuning_plugin_port();

static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_enabled_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_input_volume_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_output_volume_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_lfo_oscillator_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_lfo_frequency_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_depth_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_mix_plugin_port();
static AgsPluginPort* ags_fx_sf2_synth_audio_get_chorus_delay_plugin_port();

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
  "./synth-volume[0]",
  "./pitch-tuning[0]",
  "./chorus-enabled[0]",
  "./chorus-input-volume[0]",
  "./chorus-output-volume[0]",
  "./chorus-lfo-oscillator[0]",
  "./chorus-lfo-frequency[0]",
  "./chorus-depth[0]",
  "./chorus-mix[0]",
  "./chorus-delay[0]",
  NULL,
};

const gchar* ags_fx_sf2_synth_audio_control_port[] = {
  "1/12",
  "2/12",
  "3/12",
  "4/12",
  "5/12",
  "6/12",
  "7/12",
  "8/12",
  "9/12",
  "10/12",
  "11/12",
  "12/12",
  NULL,
};

enum{
  PROP_0,
  PROP_SYNTH_OCTAVE,
  PROP_SYNTH_KEY,
  PROP_SYNTH_VOLUME,
  PROP_PITCH_TUNING,
  PROP_CHORUS_ENABLED,
  PROP_CHORUS_INPUT_VOLUME,
  PROP_CHORUS_OUTPUT_VOLUME,
  PROP_CHORUS_LFO_OSCILLATOR,
  PROP_CHORUS_LFO_FREQUENCY,
  PROP_CHORUS_DEPTH,
  PROP_CHORUS_MIX,
  PROP_CHORUS_DELAY,
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
   * AgsFxSF2SynthAudio:pitch-tuning:
   *
   * The pitch tuning.
   * 
   * Since: 3.16.0
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

  /* synth octave */
  position = 0;
  
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

  /* synth key */
  position++;
  
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

  /* synth volume */
  position++;
  
  fx_sf2_synth_audio->synth_volume = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						   "specifier", ags_fx_sf2_synth_audio_specifier[position],
						   "control-port", ags_fx_sf2_synth_audio_control_port[position],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_sf2_synth_audio->synth_volume->port_value.ags_port_float = (gfloat) 0.333;

  g_object_set(fx_sf2_synth_audio->synth_volume,
	       "plugin-port", ags_fx_sf2_synth_audio_get_synth_volume_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->synth_volume);
  /* pitch tuning */
  position++;
  
  fx_sf2_synth_audio->pitch_tuning = g_object_new(AGS_TYPE_PORT,
						 "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						 "specifier", ags_fx_sf2_synth_audio_specifier[position],
						 "control-port", ags_fx_sf2_synth_audio_control_port[position],
						 "port-value-is-pointer", FALSE,
						 "port-value-type", G_TYPE_FLOAT,
						 "port-value-size", sizeof(gfloat),
						 "port-value-length", 1,
						 NULL);
  
  fx_sf2_synth_audio->pitch_tuning->port_value.ags_port_float = (gfloat) 0.0;

  g_object_set(fx_sf2_synth_audio->pitch_tuning,
	       "plugin-port", ags_fx_sf2_synth_audio_get_pitch_tuning_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->pitch_tuning);

  /* chorus enabled */
  position++;
  
  fx_sf2_synth_audio->chorus_enabled = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_fx_sf2_synth_audio_plugin_name,
						   "specifier", ags_fx_sf2_synth_audio_specifier[position],
						   "control-port", ags_fx_sf2_synth_audio_control_port[position],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_FLOAT,
						   "port-value-size", sizeof(gfloat),
						   "port-value-length", 1,
						   NULL);
  
  fx_sf2_synth_audio->chorus_enabled->port_value.ags_port_float = (gfloat) TRUE;

  g_object_set(fx_sf2_synth_audio->chorus_enabled,
	       "plugin-port", ags_fx_sf2_synth_audio_get_chorus_enabled_plugin_port(),
	       NULL);

  ags_recall_add_port((AgsRecall *) fx_sf2_synth_audio,
		      fx_sf2_synth_audio->chorus_enabled);

  /* chorus input volume */
  position++;
  
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

  /* chorus output volume */
  position++;
  
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

  /* chorus LFO oscillator */
  position++;
  
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

  /* chorus LFO frequency */
  position++;
  
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

  /* chorus depth */
  position++;
  
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

  /* chorus mix */
  position++;
  
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

  /* chorus delay */
  position++;
  
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
  case PROP_PITCH_TUNING:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_sf2_synth_audio->pitch_tuning){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_sf2_synth_audio->pitch_tuning != NULL){
      g_object_unref(G_OBJECT(fx_sf2_synth_audio->pitch_tuning));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_sf2_synth_audio->pitch_tuning = port;
      
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
  case PROP_PITCH_TUNING:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_sf2_synth_audio->pitch_tuning);
      
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

  //TODO:JK: implement me

  /* call parent */
  G_OBJECT_CLASS(ags_fx_sf2_synth_audio_parent_class)->dispose(gobject);
}

void
ags_fx_sf2_synth_audio_finalize(GObject *gobject)
{
  AgsFxSF2SynthAudio *fx_sf2_synth_audio;
  
  fx_sf2_synth_audio = AGS_FX_SF2_SYNTH_AUDIO(gobject);

  //TODO:JK: implement me

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

	channel_data = scope_data->channel_data[j];

	channel_data->synth.buffer_length = buffer_size;

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

	channel_data = scope_data->channel_data[j];

	channel_data->synth.format = format;

	channel_data->hq_pitch_linear_interpolate_util.format = format;

	ags_stream_free(channel_data->hq_pitch_util.destination);
	
	channel_data->hq_pitch_util.destination = ags_stream_alloc(AGS_FX_SF2_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
								   format);
	
	ags_stream_free(channel_data->hq_pitch_util.low_mix_buffer);	
	ags_stream_free(channel_data->hq_pitch_util.new_mix_buffer);
	
	channel_data->hq_pitch_util.low_mix_buffer = ags_stream_alloc(AGS_FX_SF2_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
								      format);
	channel_data->hq_pitch_util.new_mix_buffer = ags_stream_alloc(AGS_FX_SF2_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
								      format);

	channel_data->hq_pitch_util.format = format;

	channel_data->chorus_hq_pitch_util.format = format;

	ags_stream_free(channel_data->chorus_hq_pitch_util.low_mix_buffer);
	ags_stream_free(channel_data->chorus_hq_pitch_util.new_mix_buffer);

	channel_data->chorus_hq_pitch_util.low_mix_buffer = ags_stream_alloc(AGS_FX_SF2_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									     format);
	channel_data->chorus_hq_pitch_util.new_mix_buffer = ags_stream_alloc(AGS_FX_SF2_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									     format);
	
	channel_data->chorus_linear_interpolate_util.format = format;

	ags_stream_free(channel_data->chorus_util.destination);
	ags_stream_free(channel_data->chorus_hq_pitch_util.destination);
	ags_stream_free(channel_data->chorus_util.pitch_mix_buffer_history);

	channel_data->chorus_util.destination = ags_stream_alloc(AGS_FX_SF2_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
								 format);

	channel_data->chorus_hq_pitch_util.destination =
	  channel_data->chorus_util.pitch_mix_buffer = ags_stream_alloc(AGS_FX_SF2_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									format);

	
	channel_data->chorus_util.pitch_mix_buffer_history = ags_stream_alloc(AGS_CHORUS_UTIL_DEFAULT_HISTORY_BUFFER_LENGTH,
									      format);
	  
	channel_data->chorus_util.history_buffer_length = AGS_CHORUS_UTIL_DEFAULT_HISTORY_BUFFER_LENGTH;
	  
	channel_data->chorus_util.format = format;
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

	channel_data->synth.samplerate = samplerate;

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

	  channel_data =
	    scope_data->channel_data[j] = ags_fx_sf2_synth_audio_channel_data_alloc();

	  channel_data->synth.buffer_length = buffer_size;
	  channel_data->synth.format = format;
	  channel_data->synth.samplerate = samplerate;
	  
	  channel_data->hq_pitch_linear_interpolate_util.buffer_length = buffer_size;
	  channel_data->hq_pitch_linear_interpolate_util.format = format;
	  channel_data->hq_pitch_linear_interpolate_util.samplerate = samplerate;

	  channel_data->hq_pitch_util.destination = ags_stream_alloc(AGS_FX_SF2_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
								     format);

	  channel_data->hq_pitch_util.low_mix_buffer = ags_stream_alloc(AGS_FX_SF2_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									format);
	  channel_data->hq_pitch_util.new_mix_buffer = ags_stream_alloc(AGS_FX_SF2_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									format);

	  channel_data->hq_pitch_util.buffer_length = buffer_size;
	  channel_data->hq_pitch_util.format = format;
	  channel_data->hq_pitch_util.samplerate = samplerate;
	  
	  channel_data->chorus_hq_pitch_util.buffer_length = buffer_size;
	  channel_data->chorus_hq_pitch_util.format = format;
	  channel_data->chorus_hq_pitch_util.samplerate = samplerate;

	  channel_data->chorus_hq_pitch_util.low_mix_buffer = ags_stream_alloc(AGS_FX_SF2_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									       format);
	  channel_data->chorus_hq_pitch_util.new_mix_buffer = ags_stream_alloc(AGS_FX_SF2_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									       format);

	  channel_data->chorus_linear_interpolate_util.buffer_length = buffer_size;
	  channel_data->chorus_linear_interpolate_util.format = format;
	  channel_data->chorus_linear_interpolate_util.samplerate = samplerate;

	  channel_data->chorus_linear_interpolate_util.factor = 1.0;
	  
	  channel_data->chorus_util.destination = ags_stream_alloc(AGS_FX_SF2_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
								   format);

	  channel_data->chorus_hq_pitch_util.destination =
	    channel_data->chorus_util.pitch_mix_buffer = ags_stream_alloc(AGS_FX_SF2_SYNTH_AUDIO_DEFAULT_BUFFER_SIZE,
									  format);

	  channel_data->chorus_util.pitch_mix_buffer_history = ags_stream_alloc(AGS_CHORUS_UTIL_DEFAULT_HISTORY_BUFFER_LENGTH,
										format);
	  
	  channel_data->chorus_util.history_buffer_length = AGS_CHORUS_UTIL_DEFAULT_HISTORY_BUFFER_LENGTH;	  
	  
	  channel_data->chorus_util.buffer_length = buffer_size;
	  channel_data->chorus_util.format = format;
	  channel_data->chorus_util.samplerate = samplerate;

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

  /* synth 0 */
  channel_data->synth.source = NULL;
  channel_data->synth.source_stride = 1;
 
  channel_data->synth.buffer_length = 0;
  channel_data->synth.format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  channel_data->synth.samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  channel_data->synth.note = 0.0;
  channel_data->synth.volume = 0.333;

  channel_data->synth.resample_util = &(channel_data->synth_resample_util);
  
  channel_data->synth_resample_util.destination = NULL;
  channel_data->synth_resample_util.destination_stride = 1;

  channel_data->synth_resample_util.source = NULL;
  channel_data->synth_resample_util.source_stride = 1;

  channel_data->synth_resample_util.buffer_length = 0;
  channel_data->synth_resample_util.format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  channel_data->synth_resample_util.samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  channel_data->synth_resample_util.audio_buffer_util_format = AGS_AUDIO_BUFFER_UTIL_S16;

  channel_data->synth_resample_util.target_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  channel_data->synth.hq_pitch_util = &(channel_data->synth_hq_pitch_util);
  
  channel_data->synth_hq_pitch_util.destination = NULL;
  channel_data->synth_hq_pitch_util.destination_stride = 1;

  channel_data->synth_hq_pitch_util.source = NULL;
  channel_data->synth_hq_pitch_util.source_stride = 1;

  channel_data->synth_hq_pitch_util.low_mix_buffer = NULL;
  channel_data->synth_hq_pitch_util.new_mix_buffer = NULL;

  channel_data->synth_hq_pitch_util.buffer_length = 0;
  channel_data->synth_hq_pitch_util.format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  channel_data->synth_hq_pitch_util.samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  channel_data->synth_hq_pitch_util.base_key = 0.0;
  channel_data->synth_hq_pitch_util.tuning = 0.0;

  channel_data->synth.hq_pitch_util->linear_interpolate_util = &(channel_data->synth_hq_pitch_linear_interpolate_util);
  
  channel_data->synth_hq_pitch_linear_interpolate_util.source = NULL;
  channel_data->synth_hq_pitch_linear_interpolate_util.source_stride = 1;
	  
  channel_data->synth_hq_pitch_linear_interpolate_util.destination = NULL;
  channel_data->synth_hq_pitch_linear_interpolate_util.destination_stride = 1;
	  
  channel_data->synth_hq_pitch_linear_interpolate_util.buffer_length = 0;
  channel_data->synth_hq_pitch_linear_interpolate_util.format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  channel_data->synth_hq_pitch_linear_interpolate_util.samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  channel_data->synth_hq_pitch_linear_interpolate_util.factor = 1.0;

  channel_data->synth.volume_util = &(channel_data->synth_volume_util);

  channel_data->synth_volume_util.destination = NULL;
  channel_data->synth_volume_util.destination_stride = 1;

  channel_data->synth_volume_util.source = NULL;
  channel_data->synth_volume_util.source_stride = 1;
  
  channel_data->synth_volume_util.buffer_length = 0;
  channel_data->synth_volume_util.format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  channel_data->synth_volume_util.volume = 0.333;
  
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

  channel_data->chorus_util.pitch_mix_buffer = NULL;

  channel_data->chorus_util.pitch_mix_buffer_history = NULL;
  channel_data->chorus_util.history_buffer_length = 0;
  
  channel_data->chorus_util.buffer_length = 0;
  channel_data->chorus_util.format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  channel_data->chorus_util.samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  channel_data->chorus_util.offset = 0;
  
  channel_data->chorus_util.base_key = 0.0;

  channel_data->chorus_util.input_volume = 1.0;
  channel_data->chorus_util.output_volume = 1.0;

  channel_data->chorus_util.lfo_oscillator = AGS_CHORUS_UTIL_DEFAULT_LFO_OSCILLATOR;
  channel_data->chorus_util.lfo_frequency = AGS_CHORUS_UTIL_DEFAULT_LFO_FREQUENCY;

  channel_data->chorus_util.depth = AGS_CHORUS_UTIL_DEFAULT_DEPTH;
  channel_data->chorus_util.mix = AGS_CHORUS_UTIL_DEFAULT_MIX;
  channel_data->chorus_util.delay = AGS_CHORUS_UTIL_DEFAULT_DELAY;

  channel_data->chorus_util.hq_pitch_util = &(channel_data->chorus_hq_pitch_util);
  
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
		      -6.0);
    g_value_set_float(plugin_port->upper_value,
		      6.0);
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
ags_fx_sf2_synth_audio_get_pitch_tuning_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);
    
    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			   AGS_PLUGIN_PORT_CONTROL);

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