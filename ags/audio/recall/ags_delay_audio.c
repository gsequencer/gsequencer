/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/recall/ags_delay_audio.h>

#include <ags/libags.h>

#include <ags/audio/ags_notation.h>
#include <ags/audio/ags_wave.h>
#include <ags/audio/ags_midi.h>

#include <ags/audio/recall/ags_delay_audio_run.h>

#include <math.h>

#include <ags/i18n.h>

void ags_delay_audio_class_init(AgsDelayAudioClass *delay_audio);
void ags_delay_audio_tactable_interface_init(AgsTactableInterface *tactable);
void ags_delay_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_delay_audio_init(AgsDelayAudio *delay_audio);
void ags_delay_audio_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_delay_audio_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_delay_audio_set_ports(AgsPlugin *plugin, GList *port);
void ags_delay_audio_dispose(GObject *gobject);
void ags_delay_audio_finalize(GObject *gobject);

void ags_delay_audio_notify_audio_callback(GObject *gobject,
					   GParamSpec *pspec,
					   gpointer user_data);
void ags_delay_audio_notify_output_soundcard_callback(GObject *gobject,
						      GParamSpec *pspec,
						      gpointer user_data);

void ags_delay_audio_notify_samplerate_callback(GObject *gobject,
						GParamSpec *pspec,
						gpointer user_data);
void ags_delay_audio_notify_buffer_size_callback(GObject *gobject,
						 GParamSpec *pspec,
						 gpointer user_data);

gdouble ags_delay_audio_get_bpm(AgsTactable *tactable);
gdouble ags_delay_audio_get_tact(AgsTactable *tactable);
gdouble ags_delay_audio_get_wave_duration(AgsTactable *tactable);
gdouble ags_delay_audio_get_sequencer_duration(AgsTactable *tactable);
gdouble ags_delay_audio_get_notation_duration(AgsTactable *tactable);
void ags_delay_audio_change_bpm(AgsTactable *tactable, gdouble new_bpm, gdouble old_bpm);
void ags_delay_audio_change_tact(AgsTactable *tactable, gdouble new_tact, gdouble old_bpm);
void ags_delay_audio_change_wave_duration(AgsTactable *tactable, gdouble duration);
void ags_delay_audio_change_sequencer_duration(AgsTactable *tactable, gdouble duration);
void ags_delay_audio_change_notation_duration(AgsTactable *tactable, gdouble duration);

void ags_delay_audio_refresh_delay(AgsDelayAudio *delay_audio);

/**
 * SECTION:ags_delay_audio
 * @short_description: delay audio 
 * @title: AgsDelayAudio
 * @section_id:
 * @include: ags/audio/recall/ags_delay_audio.h
 *
 * The #AgsDelayAudio class provides ports to the effect processor.
 */

enum{
  NOTATION_DURATION_CHANGED,
  SEQUENCER_DURATION_CHANGED,
  WAVE_DURATION_CHANGED,
  MIDI_DURATION_CHANGED,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_NOTATION_DELAY,
  PROP_SEQUENCER_DELAY,
  PROP_WAVE_DELAY,
  PROP_MIDI_DELAY,
  PROP_TACT,
  PROP_BPM,
  PROP_NOTATION_DURATION,
  PROP_SEQUENCER_DURATION,
  PROP_WAVE_DURATION,
  PROP_MIDI_DURATION,
};

static gpointer ags_delay_audio_parent_class = NULL;

static guint delay_audio_signals[LAST_SIGNAL];

static const gchar *ags_delay_audio_plugin_name = "ags-delay";
static const gchar *ags_delay_audio_specifier[] = {
  "./bpm[0]",
  "./tact[0]",
  "./sequencer_delay[0]",
  "./notation_delay[0]",
  "./wave_delay[0]",
  "./midi_delay[0]",
  "./sequencer_duration[0]",
  "./notation_duration[0]"
  "./wave_duration[0]",
  "./midi_duration[0]",
};

static const gchar *ags_delay_audio_control_port[] = {
  "1/10",
  "2/10",
  "3/10",
  "4/10",
  "5/10",
  "6/10",
  "7/10",
  "8/10",
  "9/10",
  "10/10",
};

GType
ags_delay_audio_get_type()
{
  static GType ags_type_delay_audio = 0;

  if(!ags_type_delay_audio){
    static const GTypeInfo ags_delay_audio_info = {
      sizeof (AgsDelayAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_delay_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsDelayAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_delay_audio_init,
    };

    static const GInterfaceInfo ags_tactable_interface_info = {
      (GInterfaceInitFunc) ags_delay_audio_tactable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_delay_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_delay_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						  "AgsDelayAudio",
						  &ags_delay_audio_info,
						  0);

    g_type_add_interface_static(ags_type_delay_audio,
				AGS_TYPE_TACTABLE,
				&ags_tactable_interface_info);

    g_type_add_interface_static(ags_type_delay_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_delay_audio);
}

void
ags_delay_audio_class_init(AgsDelayAudioClass *delay_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_delay_audio_parent_class = g_type_class_peek_parent(delay_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) delay_audio;

  gobject->set_property = ags_delay_audio_set_property;
  gobject->get_property = ags_delay_audio_get_property;

  gobject->dispose = ags_delay_audio_dispose;
  gobject->finalize = ags_delay_audio_finalize;

  /* properties */
  /**
   * AgsDelayAudio:bpm:
   *
   * The beats per minute.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("bpm",
				   i18n_pspec("bpm of recall"),
				   i18n_pspec("The recall's bpm"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  /**
   * AgsDelayAudio:tact:
   *
   * The tact segmentation.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("tact",
				   i18n_pspec("tact of recall"),
				   i18n_pspec("The recall's tact"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TACT,
				  param_spec);


  /**
   * AgsDelayAudio:sequencer-delay:
   *
   * The sequencer's delay.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("sequencer-delay",
				   i18n_pspec("sequencer-delay of recall"),
				   i18n_pspec("The delay of the sequencer"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_DELAY,
				  param_spec);

  /**
   * AgsDelayAudio:notation-delay:
   *
   * The notation's delay.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("notation-delay",
				   i18n_pspec("notation-delay of recall"),
				   i18n_pspec("The delay of the notation"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION_DELAY,
				  param_spec);
  /**
   * AgsDelayAudio:wave-delay:
   *
   * The wave's delay.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("wave-delay",
				   i18n_pspec("wave-delay of recall"),
				   i18n_pspec("The delay of the wave"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAVE_DELAY,
				  param_spec);
  
  /**
   * AgsDelayAudio:midi-delay:
   *
   * The midi's delay.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("midi-delay",
				   i18n_pspec("midi-delay of recall"),
				   i18n_pspec("The delay of the midi"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIDI_DELAY,
				  param_spec);

  /**
   * AgsDelayAudio:sequencer-duration:
   *
   * The sequencer's duration.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("sequencer-duration",
				   i18n_pspec("sequencer-duration of recall"),
				   i18n_pspec("The duration of the sequencer"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_DURATION,
				  param_spec);

  /**
   * AgsDelayAudio:notation-duration:
   *
   * The notation's duration.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("notation-duration",
				   i18n_pspec("notation-duration of recall"),
				   i18n_pspec("The duration of the notation"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION_DURATION,
				  param_spec);

  /**
   * AgsDelayAudio:wave-duration:
   *
   * The wave's duration.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("wave-duration",
				   i18n_pspec("wave-duration of recall"),
				   i18n_pspec("The duration of the wave"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAVE_DURATION,
				  param_spec);

  /**
   * AgsDelayAudio:midi-duration:
   *
   * The midi's duration.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("midi-duration",
				   i18n_pspec("midi-duration of recall"),
				   i18n_pspec("The duration of the midi"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIDI_DURATION,
				  param_spec);

  /* signals */
  /**
   * AgsDelayAudio::sequencer-duration-changed:
   * @delay_audio: the #AgsDelaAudio
   *
   * The ::sequencer-duration-changed signal notifies about changed duration
   * of sequencer.
   * 
   * Since: 2.0.0
   */
  delay_audio_signals[SEQUENCER_DURATION_CHANGED] = 
    g_signal_new("sequencer-duration-changed",
		 G_TYPE_FROM_CLASS(delay_audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioClass, sequencer_duration_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsDelayAudio::notation-duration-changed:
   * @delay_audio: the #AgsDelaAudio
   *
   * The ::notation-duration-changed signal notifies about changed duration
   * of notation.
   * 
   * Since: 2.0.0
   */
  delay_audio_signals[NOTATION_DURATION_CHANGED] = 
    g_signal_new("notation-duration-changed",
		 G_TYPE_FROM_CLASS(delay_audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioClass, notation_duration_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsDelayAudio::wave-duration-changed:
   * @delay_audio: the #AgsDelaAudio
   *
   * The ::wave-duration-changed signal notifies about changed duration
   * of wave.
   * 
   * Since: 2.0.0
   */
  delay_audio_signals[WAVE_DURATION_CHANGED] = 
    g_signal_new("wave-duration-changed",
		 G_TYPE_FROM_CLASS(delay_audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioClass, wave_duration_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /**
   * AgsDelayAudio::midi-duration-changed:
   * @delay_audio: the #AgsDelaAudio
   *
   * The ::midi-duration-changed signal notifies about changed duration
   * of midi.
   * 
   * Since: 2.0.0
   */
  delay_audio_signals[MIDI_DURATION_CHANGED] = 
    g_signal_new("midi-duration-changed",
		 G_TYPE_FROM_CLASS(delay_audio),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsDelayAudioClass, midi_duration_changed),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_delay_audio_tactable_interface_init(AgsTactableInterface *tactable)
{
  tactable->get_sequencer_duration = ags_delay_audio_get_sequencer_duration;
  tactable->get_notation_duration = ags_delay_audio_get_notation_duration;
  tactable->get_wave_duration = ags_delay_audio_get_wave_duration;
  tactable->get_midi_duration = ags_delay_audio_get_midi_duration;

  tactable->get_bpm = ags_delay_audio_get_bpm;
  tactable->get_tact = ags_delay_audio_get_tact;

  tactable->change_sequencer_duration = ags_delay_audio_change_sequencer_duration;
  tactable->change_notation_duration = ags_delay_audio_change_notation_duration;
  tactable->change_wave_duration = ags_delay_audio_change_wave_duration;
  tactable->change_midi_duration = ags_delay_audio_change_midi_duration;

  tactable->change_bpm = ags_delay_audio_change_bpm;
  tactable->change_tact = ags_delay_audio_change_tact;
}

void
ags_delay_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->set_ports = ags_delay_audio_set_ports;
}

void
ags_delay_audio_init(AgsDelayAudio *delay_audio)
{
  GList *port;

  gdouble bpm;
  gdouble delay;

  AGS_RECALL(delay_audio)->name = "ags-delay";
  AGS_RECALL(delay_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(delay_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(delay_audio)->xml_type = "ags-delay-audio";

  port = NULL;

  bpm = AGS_SOUNDCARD_DEFAULT_BPM;
  delay = AGS_SOUNDCARD_DEFAULT_DELAY;

  /* bpm */
  delay_audio->bpm = g_object_new(AGS_TYPE_PORT,
				  "plugin-name", ags_delay_audio_plugin_name,
				  "specifier", ags_delay_audio_specifier[0],
				  "control-port", ags_delay_audio_control_port[0],
				  "port-value-is-pointer", FALSE,
				  "port-value-type", G_TYPE_DOUBLE,
				  "port-value-size", sizeof(gdouble),
				  "port-value-length", 1,
				  NULL);
  g_object_ref(delay_audio->bpm);
  
  delay_audio->bpm->port_value.ags_port_double = bpm;

  /* add port */
  port = g_list_prepend(port, delay_audio->bpm);
  g_object_ref(delay_audio->bpm);
  
  /* tact */
  delay_audio->tact = g_object_new(AGS_TYPE_PORT,
				   "plugin-name", ags_delay_audio_plugin_name,
				   "specifier", ags_delay_audio_specifier[1],
				   "control-port", ags_delay_audio_control_port[1],
				   "port-value-is-pointer", FALSE,
				   "port-value-type", G_TYPE_DOUBLE,
				   "port-value-size", sizeof(gdouble),
				   "port-value-length", 1,
				   NULL);
  g_object_ref(delay_audio->tact);
  
  delay_audio->tact->port_value.ags_port_double = AGS_SOUNDCARD_DEFAULT_TACT;

  /* add port */
  port = g_list_prepend(port, delay_audio->tact);
  g_object_ref(delay_audio->tact);
  
  /* sequencer delay */
  delay_audio->sequencer_delay = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_delay_audio_plugin_name,
					      "specifier", ags_delay_audio_specifier[2],
					      "control-port", ags_delay_audio_control_port[2],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_DOUBLE,
					      "port-value-size", sizeof(gdouble),
					      "port-value-length", 1,
					      NULL);
  g_object_ref(delay_audio->sequencer_delay);
  
  delay_audio->sequencer_delay->port_value.ags_port_double = delay;

  /* add port */
  port = g_list_prepend(port, delay_audio->sequencer_delay);
  g_object_ref(delay_audio->sequencer_delay);
  
  /* notation delay */
  delay_audio->notation_delay = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_delay_audio_plugin_name,
					     "specifier", ags_delay_audio_specifier[3],
					     "control-port", ags_delay_audio_control_port[3],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_DOUBLE,
					     "port-value-size", sizeof(gdouble),
					     "port-value-length", 1,
					     NULL);
  g_object_ref(delay_audio->notation_delay);
  
  delay_audio->notation_delay->port_value.ags_port_double = delay;

  /* add port */
  port = g_list_prepend(port, delay_audio->notation_delay);
  g_object_ref(delay_audio->notation_delay);

  /* wave delay */
  delay_audio->wave_delay = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_delay_audio_plugin_name,
					      "specifier", ags_delay_audio_specifier[2],
					      "control-port", ags_delay_audio_control_port[2],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_DOUBLE,
					      "port-value-size", sizeof(gdouble),
					      "port-value-length", 1,
					      NULL);
  g_object_ref(delay_audio->wave_delay);
  
  delay_audio->wave_delay->port_value.ags_port_double = delay;

  /* add port */
  port = g_list_prepend(port, delay_audio->wave_delay);
  g_object_ref(delay_audio->wave_delay);

  /* midi delay */
  delay_audio->midi_delay = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_delay_audio_plugin_name,
					      "specifier", ags_delay_audio_specifier[2],
					      "control-port", ags_delay_audio_control_port[2],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_DOUBLE,
					      "port-value-size", sizeof(gdouble),
					      "port-value-length", 1,
					      NULL);
  g_object_ref(delay_audio->midi_delay);
  
  delay_audio->midi_delay->port_value.ags_port_double = delay;

  /* add port */
  port = g_list_prepend(port, delay_audio->midi_delay);
  g_object_ref(delay_audio->midi_delay);
  
  /* sequencer duration */
  delay_audio->sequencer_duration = g_object_new(AGS_TYPE_PORT,
						 "plugin-name", ags_delay_audio_plugin_name,
						 "specifier", ags_delay_audio_specifier[4],
						 "control-port", ags_delay_audio_control_port[4],
						 "port-value-is-pointer", FALSE,
						 "port-value-type", G_TYPE_UINT64,
						 "port-value-size", sizeof(guint64),
						 "port-value-length", 1,
						 NULL);
  g_object_ref(delay_audio->sequencer_duration);
  
  delay_audio->sequencer_duration->port_value.ags_port_double = ceil(16.0 * delay);

  /* add port */
  port = g_list_prepend(port, delay_audio->sequencer_duration);
  g_object_ref(delay_audio->sequencer_duration);
  
  /* notation duration */
  delay_audio->notation_duration = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_delay_audio_plugin_name,
						"specifier", ags_delay_audio_specifier[5],
						"control-port", ags_delay_audio_control_port[5],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_UINT64,
						"port-value-size", sizeof(guint64),
						"port-value-length", 1,
						NULL);
  g_object_ref(delay_audio->notation_duration);
  
  delay_audio->notation_duration->port_value.ags_port_double = ceil(AGS_NOTATION_DEFAULT_DURATION * delay);

  /* add port */
  port = g_list_prepend(port, delay_audio->notation_duration);
  g_object_ref(delay_audio->notation_duration);

  /* wave duration */
  delay_audio->wave_duration = g_object_new(AGS_TYPE_PORT,
						 "plugin-name", ags_delay_audio_plugin_name,
						 "specifier", ags_delay_audio_specifier[4],
						 "control-port", ags_delay_audio_control_port[4],
						 "port-value-is-pointer", FALSE,
						 "port-value-type", G_TYPE_UINT64,
						 "port-value-size", sizeof(guint64),
						 "port-value-length", 1,
						 NULL);
  g_object_ref(delay_audio->wave_duration);
  
  delay_audio->wave_duration->port_value.ags_port_double = ceil(16.0 * delay);

  /* add port */
  port = g_list_prepend(port, delay_audio->wave_duration);
  g_object_ref(delay_audio->wave_duration);

  /* midi duration */
  delay_audio->midi_duration = g_object_new(AGS_TYPE_PORT,
						 "plugin-name", ags_delay_audio_plugin_name,
						 "specifier", ags_delay_audio_specifier[4],
						 "control-port", ags_delay_audio_control_port[4],
						 "port-value-is-pointer", FALSE,
						 "port-value-type", G_TYPE_UINT64,
						 "port-value-size", sizeof(guint64),
						 "port-value-length", 1,
						 NULL);
  g_object_ref(delay_audio->midi_duration);
  
  delay_audio->midi_duration->port_value.ags_port_double = ceil(16.0 * delay);

  /* add port */
  port = g_list_prepend(port, delay_audio->midi_duration);
  g_object_ref(delay_audio->midi_duration);
  
  /* set port */
  AGS_RECALL(delay_audio)->port = port;
  
  /* notify some properties to do final configuration */
  g_signal_connect_after(delay_audio, "notify::audio",
			 G_CALLBACK(ags_delay_audio_notify_audio_callback), NULL);

  g_signal_connect_after(delay_audio, "notify::output-soundcard",
			 G_CALLBACK(ags_delay_audio_notify_output_soundcard_callback), NULL);
}

void
ags_delay_audio_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsDelayAudio *delay_audio;

  pthread_mutex_t *recall_mutex;

  delay_audio = AGS_DELAY_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_BPM:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == delay_audio->bpm){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(delay_audio->bpm != NULL){
	g_object_unref(G_OBJECT(delay_audio->bpm));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->bpm = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_TACT:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == delay_audio->tact){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(delay_audio->tact != NULL){
	g_object_unref(G_OBJECT(delay_audio->tact));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->tact = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SEQUENCER_DELAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == delay_audio->sequencer_delay){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(delay_audio->sequencer_delay != NULL){
	g_object_unref(G_OBJECT(delay_audio->sequencer_delay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->sequencer_delay = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION_DELAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == delay_audio->notation_delay){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(delay_audio->notation_delay != NULL){
	g_object_unref(G_OBJECT(delay_audio->notation_delay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->notation_delay = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_DELAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == delay_audio->wave_delay){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(delay_audio->wave_delay != NULL){
	g_object_unref(G_OBJECT(delay_audio->wave_delay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->wave_delay = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_MIDI_DELAY:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == delay_audio->midi_delay){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(delay_audio->midi_delay != NULL){
	g_object_unref(G_OBJECT(delay_audio->midi_delay));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->midi_delay = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SEQUENCER_DURATION:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == delay_audio->sequencer_duration){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(delay_audio->sequencer_duration != NULL){
	g_object_unref(G_OBJECT(delay_audio->sequencer_duration));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->sequencer_duration = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION_DURATION:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == delay_audio->notation_duration){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(delay_audio->notation_duration != NULL){
	g_object_unref(G_OBJECT(delay_audio->notation_duration));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->notation_duration = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_DURATION:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == delay_audio->wave_duration){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(delay_audio->wave_duration != NULL){
	g_object_unref(G_OBJECT(delay_audio->wave_duration));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->wave_duration = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_MIDI_DURATION:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == delay_audio->midi_duration){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(delay_audio->midi_duration != NULL){
	g_object_unref(G_OBJECT(delay_audio->midi_duration));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      delay_audio->midi_duration = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_delay_audio_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsDelayAudio *delay_audio;

  pthread_mutex_t *recall_mutex;

  delay_audio = AGS_DELAY_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_BPM:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, delay_audio->bpm);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_TACT:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, delay_audio->tact);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SEQUENCER_DELAY:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, delay_audio->sequencer_delay);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION_DELAY:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, delay_audio->notation_delay);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_DELAY:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, delay_audio->wave_delay);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_MIDI_DELAY:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, delay_audio->midi_delay);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION_DURATION:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, delay_audio->notation_duration);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SEQUENCER_DURATION:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, delay_audio->sequencer_duration);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_DURATION:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, delay_audio->wave_duration);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_MIDI_DURATION:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, delay_audio->midi_duration);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_delay_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./bpm[0]",
		8)){
      g_object_set(G_OBJECT(plugin),
		   "bpm", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./tact[0]",
		      9)){
      g_object_set(G_OBJECT(plugin),
		   "tact", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./sequencer-delay[0]",
		      19)){
      g_object_set(G_OBJECT(plugin),
		   "sequencer-delay", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./notation-delay[0]",
		      18)){
      g_object_set(G_OBJECT(plugin),
		   "notation-delay", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./wave-delay[0]",
		      15)){
      g_object_set(G_OBJECT(plugin),
		   "wave-delay", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./midi-delay[0]",
		      15)){
      g_object_set(G_OBJECT(plugin),
		   "midi-delay", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./sequencer-duration[0]",
		      22)){
      g_object_set(G_OBJECT(plugin),
		   "sequencer-duration", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./notation-duration[0]",
		      21)){
      g_object_set(G_OBJECT(plugin),
		   "notation-duration", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./wave-duration[0]",
		      18)){
      g_object_set(G_OBJECT(plugin),
		   "wave-duration", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./midi-duration[0]",
		      18)){
      g_object_set(G_OBJECT(plugin),
		   "midi-duration", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

void
ags_delay_audio_dispose(GObject *gobject)
{
  AgsDelayAudio *delay_audio;

  delay_audio = AGS_DELAY_AUDIO(gobject);

  /* bpm and tact */
  if(delay_audio->bpm != NULL){
    g_object_unref(G_OBJECT(delay_audio->bpm));

    delay_audio->bpm = NULL;
  }

  if(delay_audio->tact != NULL){
    g_object_unref(G_OBJECT(delay_audio->tact));

    delay_audio->tact = NULL;
  }

  /* delay */
  if(delay_audio->sequencer_delay != NULL){
    g_object_unref(G_OBJECT(delay_audio->sequencer_delay));

    delay_audio->sequencer_delay = NULL;
  }

  if(delay_audio->notation_delay != NULL){
    g_object_unref(G_OBJECT(delay_audio->notation_delay));

    delay_audio->notation_delay = NULL;
  }

  if(delay_audio->wave_delay != NULL){
    g_object_unref(G_OBJECT(delay_audio->wave_delay));

    delay_audio->wave_delay = NULL;
  }

  if(delay_audio->midi_delay != NULL){
    g_object_unref(G_OBJECT(delay_audio->midi_delay));

    delay_audio->midi_delay = NULL;
  }

  /* duration */
  if(delay_audio->sequencer_duration != NULL){
    g_object_unref(G_OBJECT(delay_audio->sequencer_duration));

    delay_audio->sequencer_duration = NULL;
  }

  if(delay_audio->notation_duration != NULL){
    g_object_unref(G_OBJECT(delay_audio->notation_duration));

    delay_audio->notation_duration = NULL;
  }

  if(delay_audio->wave_duration != NULL){
    g_object_unref(G_OBJECT(delay_audio->wave_duration));

    delay_audio->wave_duration = NULL;
  }

  if(delay_audio->midi_duration != NULL){
    g_object_unref(G_OBJECT(delay_audio->midi_duration));

    delay_audio->midi_duration = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_delay_audio_parent_class)->dispose(gobject);
}

void
ags_delay_audio_finalize(GObject *gobject)
{
  AgsDelayAudio *delay_audio;

  delay_audio = AGS_DELAY_AUDIO(gobject);

  if(delay_audio->bpm != NULL){
    g_object_unref(G_OBJECT(delay_audio->bpm));
  }

  if(delay_audio->tact != NULL){
    g_object_unref(G_OBJECT(delay_audio->tact));
  }

  if(delay_audio->sequencer_delay != NULL){
    g_object_unref(G_OBJECT(delay_audio->sequencer_delay));
  }

  if(delay_audio->notation_delay != NULL){
    g_object_unref(G_OBJECT(delay_audio->notation_delay));
  }

  if(delay_audio->wave_delay != NULL){
    g_object_unref(G_OBJECT(delay_audio->wave_delay));
  }
  
  if(delay_audio->midi_delay != NULL){
    g_object_unref(G_OBJECT(delay_audio->midi_delay));
  }

  if(delay_audio->sequencer_duration != NULL){
    g_object_unref(G_OBJECT(delay_audio->sequencer_duration));
  }
  
  if(delay_audio->notation_duration != NULL){
    g_object_unref(G_OBJECT(delay_audio->notation_duration));
  }

  if(delay_audio->wave_duration != NULL){
    g_object_unref(G_OBJECT(delay_audio->wave_duration));
  }

  if(delay_audio->midi_duration != NULL){
    g_object_unref(G_OBJECT(delay_audio->midi_duration));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_delay_audio_parent_class)->finalize(gobject);
}

void
ags_delay_audio_notify_audio_callback(GObject *gobject,
				      GParamSpec *pspec,
				      gpointer user_data)
{
  AgsAudio *audio;
  AgsDelayAudio *delay_audio;

  g_object_get(gobject,
	       "audio", &audio,
	       NULL);

  if(audio == NULL){
    return;
  }

  delay_audio = AGS_DELAY_AUDIO(gobject);

  g_signal_connect_after(audio, "notify::samplerate",
			 G_CALLBACK(ags_delay_audio_notify_samplerate_callback), delay_audio);

  g_signal_connect_after(audio, "notify::buffer-size",
			 G_CALLBACK(ags_delay_audio_notify_buffer_size_callback), delay_audio);
}

void
ags_delay_audio_notify_output_soundcard_callback(GObject *gobject,
						 GParamSpec *pspec,
						 gpointer user_data)
{
  AgsPort *bpm;
  AgsPort *tact;
  AgsPort *sequencer_delay;
  AgsPort *notation_delay;
  AgsPort *wave_delay;
  AgsPort *midi_delay;
  AgsPort *sequencer_duration;
  AgsPort *notation_duration;
  AgsPort *wave_duration;
  AgsPort *midi_duration;
  AgsDelayAudio *delay_audio;

  GObject *output_soundcard;

  gdouble current_bpm;
  gdouble current_delay;
  gchar *str;

  GValue value = {0,};
  
  g_object_get(gobject,
	       "output-soundcard", &output_soundcard,
	       NULL);

  if(output_soundcard == NULL){
    return;
  }

  delay_audio = AGS_DELAY_AUDIO(gobject);
  
  /* get some fields */
  current_bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(output_soundcard));
  current_delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));

  g_object_get(delay_audio,
	       "bpm", &bpm,
	       "tact", &tact,
	       "sequencer-delay", &sequencer_delay,
	       "notation-delay", &notation_delay,
	       "wave-delay", &wave_delay,
	       "midi-delay", &midi_delay,
	       "sequencer-duration", &sequencer_duration,
	       "notation-duration", &notation_duration,
	       "wave-duration", &wave_duration,
	       "midi-duration", &midi_duration,
	       NULL);
  
  /* bpm */
  g_value_init(&value,
	       G_TYPE_DOUBLE);

  g_value_set_double(&value,
		     current_bpm);

  ags_port_safe_write(bpm,
		      &value);

  /* tact */
  g_value_reset(&value);

  //TODO:JK: read config
  g_value_set_double(&value,
		     AGS_SOUNDCARD_DEFAULT_TACT);

  ags_port_safe_write(tact,
		      &value);

  /* sequencer delay */
  g_value_reset(&value);

  g_value_set_double(&value,
		     current_delay);

  ags_port_safe_write(sequencer_delay,
		      &value);

  /* notation delay */
  ags_port_safe_write(notation_delay,
		      &value);

  /* wave delay */
  ags_port_safe_write(wave_delay,
		      &value);

  /* midi delay */
  ags_port_safe_write(midi_delay,
		      &value);
  
  /* sequencer duration */
  g_value_unset(&value);
  
  g_value_init(&value,
	       G_TYPE_UINT64);
  
  g_value_set_uint64(&value,
		     (guint64) ceil(16.0 * current_delay));

  ags_port_safe_write(sequencer_duration,
		      &value);
  
  /* notation duration */  
  g_value_set_uint64(&value,
		     (guint64) ceil(AGS_NOTATION_DEFAULT_DURATION * current_delay));

  ags_port_safe_write(notation_duration,
		      &value);

  /* wave duration */
  ags_port_safe_write(wave_duration,
		      &value);

  /* midi duration */
  ags_port_safe_write(midi_duration,
		      &value);
}

void
ags_delay_audio_notify_samplerate_callback(GObject *gobject,
					   GParamSpec *pspec,
					   gpointer user_data)
{
  AgsDelayAudio *delay_audio;
  
  delay_audio = AGS_DELAY_AUDIO(user_data);

  ags_delay_audio_refresh_delay(delay_audio);
}

void
ags_delay_audio_notify_buffer_size_callback(GObject *gobject,
					    GParamSpec *pspec,
					    gpointer user_data)
{
  AgsDelayAudio *delay_audio;
  
  delay_audio = AGS_DELAY_AUDIO(user_data);

  ags_delay_audio_refresh_delay(delay_audio);
}

gdouble
ags_delay_audio_get_bpm(AgsTactable *tactable)
{
  AgsPort *bpm;
  AgsDelayAudio *delay_audio;
  
  gdouble current_bpm;

  GValue value = {0,};

  delay_audio = AGS_DELAY_AUDIO(tactable);

  g_object_get(delay_audio,
	       "bpm", &bpm,
	       NULL);
  
  /* retrieve bpm */
  g_value_init(&value,
	       G_TYPE_DOUBLE);

  ags_port_safe_read(bpm,
		     &value);
  
  current_bpm = g_value_get_double(&value);

  g_value_unset(&value);
  
  return(current_bpm);
}

gdouble
ags_delay_audio_get_tact(AgsTactable *tactable)
{
  AgsPort *tact;
  AgsDelayAudio *delay_audio;
  
  gdouble current_tact;

  GValue value = {0,};

  delay_audio = AGS_DELAY_AUDIO(tactable);

  g_object_get(delay_audio,
	       "tact", &tact,
	       NULL);
  
  /* retrieve tact */
  g_value_init(&value,
	       G_TYPE_DOUBLE);

  ags_port_safe_read(tact,
		     &value);
  
  current_tact = g_value_get_double(&value);

  g_value_unset(&value);
  
  return(current_tact);
}

guint64
ags_delay_audio_get_sequencer_duration(AgsTactable *tactable)
{
  AgsPort *sequencer_duration;
  AgsDelayAudio *delay_audio;
  
  gdouble current_sequencer_duration;

  GValue value = {0,};

  delay_audio = AGS_DELAY_AUDIO(tactable);
  
  g_object_get(delay_audio,
	       "sequencer-duration", &sequencer_durtation,
	       NULL);
  
  /* retrieve tact */
  g_value_init(&value,
	       G_TYPE_UINT64);

  ags_port_safe_read(sequencer_duration,
		     &value);
  
  current_sequencer_duration = g_value_get_uint64(&value);

  g_value_unset(&value);

  return(current_sequencer_duration);
}

guint64
ags_delay_audio_get_notation_duration(AgsTactable *tactable)
{
  AgsPort *notation_duration;
  AgsDelayAudio *delay_audio;
  
  gdouble current_notation_duration;

  GValue value = {0,};

  delay_audio = AGS_DELAY_AUDIO(tactable);
  
  g_object_get(delay_audio,
	       "notation-duration", &notation_durtation,
	       NULL);
  
  /* retrieve tact */
  g_value_init(&value,
	       G_TYPE_UINT64);

  ags_port_safe_read(notation_duration,
		     &value);
  
  current_notation_duration = g_value_get_uint64(&value);

  g_value_unset(&value);

  return(current_notation_duration);
}

guint64
ags_delay_audio_get_wave_duration(AgsTactable *tactable)
{
  AgsPort *wave_duration;
  AgsDelayAudio *delay_audio;
  
  gdouble current_wave_duration;

  GValue value = {0,};

  delay_audio = AGS_DELAY_AUDIO(tactable);
  
  g_object_get(delay_audio,
	       "wave-duration", &wave_durtation,
	       NULL);
  
  /* retrieve tact */
  g_value_init(&value,
	       G_TYPE_UINT64);

  ags_port_safe_read(wave_duration,
		     &value);
  
  current_wave_duration = g_value_get_uint64(&value);

  g_value_unset(&value);

  return(current_wave_duration);
}

guint64
ags_delay_audio_get_midi_duration(AgsTactable *tactable)
{
  AgsPort *midi_duration;
  AgsDelayAudio *delay_audio;
  
  gdouble current_midi_duration;

  GValue value = {0,};

  delay_audio = AGS_DELAY_AUDIO(tactable);
  
  g_object_get(delay_audio,
	       "midi-duration", &midi_durtation,
	       NULL);
  
  /* retrieve tact */
  g_value_init(&value,
	       G_TYPE_UINT64);

  ags_port_safe_read(midi_duration,
		     &value);
  
  current_midi_duration = g_value_get_uint64(&value);

  g_value_unset(&value);

  return(current_midi_duration);
}

void
ags_delay_audio_change_bpm(AgsTactable *tactable, gdouble new_bpm, gdouble old_bpm)
{
  AgsPort *bpm;
  AgsPort *sequencer_delay;
  AgsPort *notation_delay;
  AgsPort *wave_delay;
  AgsPort *midi_delay;
  AgsPort *sequencer_duration;
  AgsPort *notation_duration;
  AgsPort *wave_duration;
  AgsPort *midi_duration;
  AgsDelayAudio *delay_audio;

  GObject *output_soundcard;

  gdouble current_delay;

  GValue value = {0,};

  delay_audio = AGS_DELAY_AUDIO(tactable);

  g_object_get(delay_audio,
	       "output-soundcard", &output_soundcard,
	       "bpm", &bpm,
	       "sequencer-delay", &sequencer_delay,
	       "notation-delay", &notation_delay,
	       "wave-delay", &wave_delay,
	       "midi-delay", &midi_delay,
	       "sequencer-duration", &sequencer_duration,
	       "notation-duration", &notation_duration,
	       "wave-duration", &wave_duration,
	       "midi-duration", &midi_duration,
	       NULL);

  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));

  /* -- start adjust -- */
  g_value_init(&value,
	       G_TYPE_DOUBLE);

  g_value_set_double(&value,
		     current_delay);
  
  /* sequencer delay */
  ags_port_safe_write(sequencer_delay,
		      &value);
  
  /* notation delay */
  ags_port_safe_write(notation_delay,
		      &value);

  /* wave delay */
  ags_port_safe_write(wave_delay,
		      &value);
  
  /* midi delay */
  ags_port_safe_write(midi_delay,
		      &value);
  
  /* bpm */
  g_value_reset(&value);

  g_value_set_double(&value,
		     new_bpm);
  
  ags_port_safe_write(bpm,
		      &value);

  /* sequencer duration */
  g_value_unset(&value);

  g_value_init(&value,
	       G_TYPE_UINT64);

  g_value_set_uint64(&value,
		     (guint64) ceil(16.0 * delay));

  ags_port_safe_write(sequencer_duration, &value);


  /* notation duration */
  g_value_set_uint64(&value,
		     (guint64) ceil(AGS_NOTATION_DEFAULT_DURATION * current_delay));

  ags_port_safe_write(notation_duration, &value);

  /* wave duration */
  ags_port_safe_write(wave_duration, &value);
  
  /* midi duration */
  ags_port_safe_write(midi_duration, &value);

  /* -- finish adjust -- */

  /* emit changed */
  ags_delay_audio_sequencer_duration_changed(delay_audio);
  ags_delay_audio_notation_duration_changed(delay_audio);
  ags_delay_audio_wave_duration_changed(delay_audio);
  ags_delay_audio_midi_duration_changed(delay_audio);
}

void
ags_delay_audio_change_tact(AgsTactable *tactable, gdouble new_tact, gdouble old_tact)
{
  AgsPort *tact;
  AgsPort *sequencer_delay;
  AgsPort *notation_delay;
  AgsPort *wave_delay;
  AgsPort *midi_delay;
  AgsPort *sequencer_duration;
  AgsPort *notation_duration;
  AgsPort *wave_duration;
  AgsPort *midi_duration;
  AgsDelayAudio *delay_audio;

  GObject *output_soundcard;

  gdouble current_delay;

  GValue value = {0,};

  delay_audio = AGS_DELAY_AUDIO(tactable);

  g_object_get(delay_audio,
	       "output-soundcard", &output_soundcard,
	       "tact", &tact,
	       "sequencer-delay", &sequencer_delay,
	       "notation-delay", &notation_delay,
	       "wave-delay", &wave_delay,
	       "midi-delay", &midi_delay,
	       "sequencer-duration", &sequencer_duration,
	       "notation-duration", &notation_duration,
	       "wave-duration", &wave_duration,
	       "midi-duration", &midi_duration,
	       NULL);

  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));

  /* -- start adjust -- */
  g_value_init(&value,
	       G_TYPE_DOUBLE);

  g_value_set_double(&value,
		     current_delay);
  
  /* sequencer delay */
  ags_port_safe_write(sequencer_delay,
		      &value);
  
  /* notation delay */
  ags_port_safe_write(notation_delay,
		      &value);

  /* wave delay */
  ags_port_safe_write(wave_delay,
		      &value);
  
  /* midi delay */
  ags_port_safe_write(midi_delay,
		      &value);
  
  /* tact */
  g_value_reset(&value);

  g_value_set_double(&value,
		     new_tact);
  
  ags_port_safe_write(tact,
		      &value);

  /* sequencer duration */
  g_value_unset(&value);

  g_value_init(&value,
	       G_TYPE_UINT64);

  g_value_set_uint64(&value,
		     (guint64) ceil(16.0 * delay));

  ags_port_safe_write(sequencer_duration, &value);


  /* notation duration */
  g_value_set_uint64(&value,
		     (guint64) ceil(AGS_NOTATION_DEFAULT_DURATION * current_delay));

  ags_port_safe_write(notation_duration, &value);

  /* wave duration */
  ags_port_safe_write(wave_duration, &value);
  
  /* midi duration */
  ags_port_safe_write(midi_duration, &value);

  /* -- finish adjust -- */

  /* emit changed */
  ags_delay_audio_sequencer_duration_changed(delay_audio);
  ags_delay_audio_notation_duration_changed(delay_audio);
  ags_delay_audio_wave_duration_changed(delay_audio);
  ags_delay_audio_midi_duration_changed(delay_audio);
}

void
ags_delay_audio_refresh_delay(AgsDelayAudio *delay_audio)
{
  AgsPort *sequencer_delay;
  AgsPort *notation_delay;
  AgsPort *wave_delay;
  AgsPort *midi_delay;
  AgsPort *sequencer_duration;
  AgsPort *notation_duration;
  AgsPort *wave_duration;
  AgsPort *midi_duration;

  GObject *output_soundcard;

  gdouble current_delay;
  
  GValue value = {0,};

  g_object_get(delay_audio,
	       "output-soundcard", &output_soundcard,
	       "sequencer-delay", &sequencer_delay,
	       "notation-delay", &notation_delay,
	       "wave-delay", &wave_delay,
	       "midi-delay", &midi_delay,
	       "sequencer-duration", &sequencer_duration,
	       "notation-duration", &notation_duration,
	       "wave-duration", &wave_duration,
	       "midi-duration", &midi_duration,
	       NULL);

  if(output_soundcard == NULL){
    return;
  }
  
  current_delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));

  /* -- start adjust -- */
  g_value_init(&value,
	       G_TYPE_DOUBLE);

  g_value_set_double(&value,
		     current_delay);

  /* sequencer delay */
  ags_port_safe_write(sequencer_delay,
		      &value);

  /* notation delay */
  ags_port_safe_write(notation_delay,
		      &value);

  /* wave delay */
  ags_port_safe_write(wave_delay,
		      &value);

  /* midi delay */
  ags_port_safe_write(midi_delay,
		      &value);

  /* sequencer duration */
  g_value_unset(&value);

  g_value_init(&value,
	       G_TYPE_UINT64);

  g_value_set_uint64(&value,
		     (guint64) ceil(16.0 * delay));

  ags_port_safe_write(sequencer_duration, &value);


  /* notation duration */
  g_value_set_uint64(&value,
		     (guint64) ceil(AGS_NOTATION_DEFAULT_DURATION * current_delay));

  ags_port_safe_write(notation_duration, &value);

  /* wave duration */
  ags_port_safe_write(wave_duration, &value);
  
  /* midi duration */
  ags_port_safe_write(midi_duration, &value);
  
  /* -- finish adjust -- */

  /* emit changed */
  ags_delay_audio_sequencer_duration_changed(delay_audio);
  ags_delay_audio_notation_duration_changed(delay_audio);
  ags_delay_audio_wave_duration_changed(delay_audio);
  ags_delay_audio_midi_duration_changed(delay_audio);
}

void
ags_delay_audio_change_sequencer_duration(AgsTactable *tactable, guint64 duration)
{
  AgsPort *sequencer_duration;
  AgsDelayAudio *delay_audio;

  GObject *output_soundcard;
  
  gdouble current_delay;
  
  GValue value = {0,};

  delay_audio = AGS_DELAY_AUDIO(tactable);

  /* get some fields */
  g_object_get(delay_audio,
	       "output-soundcard", &output_soundcard,
	       "sequencer-duration", &sequencer_duration,
	       NULL);
  
  current_delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));

  /* apply duration */
  g_value_init(&value,
	       G_TYPE_UINT64);
  
  g_value_set_uint64(&value,
		     (guint64) ceil(duration * current_delay));

  ags_port_safe_write(sequencer_duration,
		      &value);

  /* emit changed */
  ags_delay_audio_sequencer_duration_changed(delay_audio);
}

void
ags_delay_audio_change_notation_duration(AgsTactable *tactable, guint64 duration)
{
  AgsPort *notation_duration;
  AgsDelayAudio *delay_audio;

  GObject *output_soundcard;
  
  gdouble current_delay;
  
  GValue value = {0,};

  delay_audio = AGS_DELAY_AUDIO(tactable);

  /* get some fields */
  g_object_get(delay_audio,
	       "output-soundcard", &output_soundcard,
	       "notation-duration", &notation_duration,
	       NULL);
  
  current_delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));

  /* apply duration */
  g_value_init(&value,
	       G_TYPE_UINT64);
  
  g_value_set_uint64(&value,
		     (guint64) ceil(duration * current_delay));

  ags_port_safe_write(notation_duration,
		      &value);

  /* emit changed */
  ags_delay_audio_notation_duration_changed(delay_audio);
}

void
ags_delay_audio_change_wave_duration(AgsTactable *tactable, guint64 duration)
{
  AgsPort *wave_duration;
  AgsDelayAudio *delay_audio;

  GObject *output_soundcard;
  
  gdouble current_delay;
  
  GValue value = {0,};

  delay_audio = AGS_DELAY_AUDIO(tactable);

  /* get some fields */
  g_object_get(delay_audio,
	       "output-soundcard", &output_soundcard,
	       "wave-duration", &wave_duration,
	       NULL);
  
  current_delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));

  /* apply duration */
  g_value_init(&value,
	       G_TYPE_UINT64);
  
  g_value_set_uint64(&value,
		     (guint64) ceil(duration * current_delay));

  ags_port_safe_write(wave_duration,
		      &value);

  /* emit changed */
  ags_delay_audio_wave_duration_changed(delay_audio);
}

void
ags_delay_audio_change_midi_duration(AgsTactable *tactable, guint64 duration)
{
  AgsPort *midi_duration;
  AgsDelayAudio *delay_audio;

  GObject *output_soundcard;
  
  gdouble current_delay;
  
  GValue value = {0,};

  delay_audio = AGS_DELAY_AUDIO(tactable);

  /* get some fields */
  g_object_get(delay_audio,
	       "output-soundcard", &output_soundcard,
	       "midi-duration", &midi_duration,
	       NULL);
  
  current_delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));

  /* apply duration */
  g_value_init(&value,
	       G_TYPE_UINT64);
  
  g_value_set_uint64(&value,
		     (guint64) ceil(duration * current_delay));

  ags_port_safe_write(midi_duration,
		      &value);

  /* emit changed */
  ags_delay_audio_midi_duration_changed(delay_audio);
}

/**
 * ags_delay_audio_sequencer_duration_changed:
 * @delay_audio: the #AgsDelayAudio
 *
 * Sequencer duration changed of #AgsDelayAudio
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_sequencer_duration_changed(AgsDelayAudio *delay_audio)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO(delay_audio));

  g_object_ref((GObject *) delay_audio);
  g_signal_emit(G_OBJECT(delay_audio),
		delay_audio_signals[SEQUENCER_DURATION_CHANGED], 0);
  g_object_unref((GObject *) delay_audio);
}

/**
 * ags_delay_audio_notation_duration_changed:
 * @delay_audio: the #AgsDelayAudio
 *
 * Notation duration changed of #AgsDelayAudio
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_notation_duration_changed(AgsDelayAudio *delay_audio)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO(delay_audio));

  g_object_ref((GObject *) delay_audio);
  g_signal_emit(G_OBJECT(delay_audio),
		delay_audio_signals[NOTATION_DURATION_CHANGED], 0);
  g_object_unref((GObject *) delay_audio);
}

/**
 * ags_delay_audio_wave_duration_changed:
 * @delay_audio: the #AgsDelayAudio
 *
 * Wave duration changed of #AgsDelayAudio
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_wave_duration_changed(AgsDelayAudio *delay_audio)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO(delay_audio));

  g_object_ref((GObject *) delay_audio);
  g_signal_emit(G_OBJECT(delay_audio),
		delay_audio_signals[WAVE_DURATION_CHANGED], 0);
  g_object_unref((GObject *) delay_audio);
}

/**
 * ags_delay_audio_midi_duration_changed:
 * @delay_audio: the #AgsDelayAudio
 *
 * Midi duration changed of #AgsDelayAudio
 *
 * Since: 2.0.0
 */
void
ags_delay_audio_midi_duration_changed(AgsDelayAudio *delay_audio)
{
  g_return_if_fail(AGS_IS_DELAY_AUDIO(delay_audio));

  g_object_ref((GObject *) delay_audio);
  g_signal_emit(G_OBJECT(delay_audio),
		delay_audio_signals[MIDI_DURATION_CHANGED], 0);
  g_object_unref((GObject *) delay_audio);
}

/**
 * ags_delay_audio_new:
 *
 * Creates a new instance of #AgsDelayAudio
 *
 * Returns: the new #AgsDelayAudio
 *
 * Since: 2.0.0
 */
AgsDelayAudio*
ags_delay_audio_new(AgsAudio *audio)
{
  AgsDelayAudio *delay_audio;

  delay_audio = (AgsDelayAudio *) g_object_new(AGS_TYPE_DELAY_AUDIO,
					       "audio", audio,
					       NULL);

  return(delay_audio);
}
