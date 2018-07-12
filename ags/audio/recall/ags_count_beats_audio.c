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

#include <ags/audio/recall/ags_count_beats_audio.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_count_beats_audio_run.h>

#include <ags/i18n.h>

void ags_count_beats_audio_class_init(AgsCountBeatsAudioClass *count_beats_audio);
void ags_count_beats_audio_tactable_interface_init(AgsTactableInterface *tactable);
void ags_count_beats_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_count_beats_audio_init(AgsCountBeatsAudio *count_beats_audio);
void ags_count_beats_audio_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_count_beats_audio_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_count_beats_audio_dispose(GObject *gobject);
void ags_count_beats_audio_finalize(GObject *gobject);

void ags_count_beats_audio_notify_output_soundcard_callback(GObject *gobject,
							    GParamSpec *pspec,
							    gpointer user_data);

void ags_count_beats_audio_change_sequencer_duration(AgsTactable *tactable, gdouble duration);
void ags_count_beats_audio_change_notation_duration(AgsTactable *tactable, gdouble duration);
void ags_count_beats_audio_change_wave_duration(AgsTactable *tactable, gdouble duration);
void ags_count_beats_audio_change_midi_duration(AgsTactable *tactable, gdouble duration);

void ags_count_beats_audio_set_ports(AgsPlugin *plugin, GList *port);

/**
 * SECTION:ags_count_beats_audio
 * @short_description: count audio beats
 * @title: AgsCountBeatsAudio
 * @section_id:
 * @include: ags/audio/recall/ags_count_beats_audio.h
 *
 * The #AgsCountBeatsAudio class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_SEQUENCER_LOOP,
  PROP_SEQUENCER_LOOP_START,
  PROP_SEQUENCER_LOOP_END,
  PROP_NOTATION_LOOP,
  PROP_NOTATION_LOOP_START,
  PROP_NOTATION_LOOP_END,
  PROP_WAVE_LOOP,
  PROP_WAVE_LOOP_START,
  PROP_WAVE_LOOP_END,
};

static gpointer ags_count_beats_audio_parent_class = NULL;
static AgsPluginInterface *ags_count_beats_audio_parent_plugin_interface;

static const gchar *ags_count_beats_audio_plugin_name = "ags-count-beats";
static const gchar *ags_count_beats_audio_specifier[] = {
  "./sequencer_loop[0]",
  "./sequencer_loop_start[0]",
  "./sequencer_loop_end[0]",
  "./notation-loop[0]",
  "./notation_loop_end[0]"
  "./notation_loop_start[0]",
  "./wave_loop[0]",
  "./wave_loop_start[0]",
  "./wave_loop_end[0]",
  "./midi_loop[0]",
  "./midi_loop_start[0]",
  "./midi_loop_end[0]",
};
static const gchar *ags_count_beats_audio_control_port[] = {
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
};

GType
ags_count_beats_audio_get_type()
{
  static GType ags_type_count_beats_audio = 0;

  if(!ags_type_count_beats_audio){
    static const GTypeInfo ags_count_beats_audio_info = {
      sizeof (AgsCountBeatsAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_count_beats_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCountBeatsAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_count_beats_audio_init,
    };
    
    static const GInterfaceInfo ags_tactable_interface_info = {
      (GInterfaceInitFunc) ags_count_beats_audio_tactable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_count_beats_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_count_beats_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							"AgsCountBeatsAudio",
							&ags_count_beats_audio_info,
							0);

    g_type_add_interface_static(ags_type_count_beats_audio,
				AGS_TYPE_TACTABLE,
				&ags_tactable_interface_info);

    g_type_add_interface_static(ags_type_count_beats_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_count_beats_audio);
}

void
ags_count_beats_audio_tactable_interface_init(AgsTactableInterface *tactable)
{
  tactable->get_sequencer_duration = NULL;
  tactable->get_notation_duration = NULL;
  tactable->get_wave_duration = NULL;
  tactable->get_midi_duration = NULL;

  tactable->get_bpm = NULL;
  tactable->get_tact = NULL;

  tactable->change_sequencer_duration = ags_count_beats_audio_change_sequencer_duration;
  tactable->change_notation_duration = ags_count_beats_audio_change_notation_duration;
  tactable->change_wave_duration = ags_count_beats_audio_change_wave_duration;
  tactable->change_midi_duration = ags_count_beats_audio_change_midi_duration;

  tactable->change_bpm = NULL;
  tactable->change_tact = NULL;
}

void
ags_count_beats_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_count_beats_audio_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_count_beats_audio_set_ports;
}

void
ags_count_beats_audio_class_init(AgsCountBeatsAudioClass *count_beats_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_count_beats_audio_parent_class = g_type_class_peek_parent(count_beats_audio);

  gobject = (GObjectClass *) count_beats_audio;

  gobject->set_property = ags_count_beats_audio_set_property;
  gobject->get_property = ags_count_beats_audio_get_property;

  gobject->dispose = ags_count_beats_audio_dispose;
  gobject->finalize = ags_count_beats_audio_finalize;

  /* properties */
  /**
   * AgsCountBeatsAudio:sequencer-loop:
   *
   * Count until loop-end and start at loop-start.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("sequencer-loop",
				   i18n_pspec("sequencer loop playing"),
				   i18n_pspec("Play sequencer in a endless loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_LOOP,
				  param_spec);

  /**
   * AgsCountBeatsAudio:sequencer-loop-start:
   *
   * The sequencer's loop-start.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("sequencer_loop_start",
				   i18n_pspec("start beat of loop"),
				   i18n_pspec("The start beat of the sequencer loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_LOOP_START,
				  param_spec);

  /**
   * AgsCountBeatsAudio:sequencer-loop-end:
   *
   * The sequencer's loop-end.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("sequencer-loop-end",
				   i18n_pspec("end beat of sequencer loop"),
				   i18n_pspec("The end beat of the sequencer loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SEQUENCER_LOOP_END,
				  param_spec);

  /**
   * AgsCountBeatsAudio:notation-loop:
   *
   * Count until notation-loop-end and start at notation-loop-start.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("notation-loop",
				   i18n_pspec("notation-loop playing"),
				   i18n_pspec("Play in a endless notation_loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION_LOOP,
				  param_spec);

  /**
   * AgsCountBeatsAudio:notation-loop-start:
   *
   * The notation's notation-loop-start.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("notation-loop-start",
				   i18n_pspec("start beat of notation loop"),
				   i18n_pspec("The start beat of the notation loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION_LOOP_START,
				  param_spec);

  /**
   * AgsCountBeatsAudio:notation-loop-end:
   *
   * The notation's loop-end.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("notation-loop-end",
				   i18n_pspec("end beat of notation loop"),
				   i18n_pspec("The end beat of the notation loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_NOTATION_LOOP_END,
				  param_spec);
  
  /**
   * AgsCountBeatsAudio:wave-loop:
   *
   * Count until loop-end and start at loop-start.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("wave-loop",
				   i18n_pspec("wave loop playing"),
				   i18n_pspec("Play wave in a endless loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAVE_LOOP,
				  param_spec);

  /**
   * AgsCountBeatsAudio:wave-loop-start:
   *
   * The wave's loop-start.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("wave_loop_start",
				   i18n_pspec("start beat of loop"),
				   i18n_pspec("The start beat of the wave loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAVE_LOOP_START,
				  param_spec);

  /**
   * AgsCountBeatsAudio:wave-loop-end:
   *
   * The wave's loop-end.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("wave-loop-end",
				   i18n_pspec("end beat of wave loop"),
				   i18n_pspec("The end beat of the wave loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAVE_LOOP_END,
				  param_spec);

  /**
   * AgsCountBeatsAudio:midi-loop:
   *
   * Count until loop-end and start at loop-start.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("midi-loop",
				   i18n_pspec("midi loop playing"),
				   i18n_pspec("Play midi in a endless loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIDI_LOOP,
				  param_spec);

  /**
   * AgsCountBeatsAudio:midi-loop-start:
   *
   * The midi's loop-start.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("midi_loop_start",
				   i18n_pspec("start beat of loop"),
				   i18n_pspec("The start beat of the midi loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIDI_LOOP_START,
				  param_spec);

  /**
   * AgsCountBeatsAudio:midi-loop-end:
   *
   * The midi's loop-end.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("midi-loop-end",
				   i18n_pspec("end beat of midi loop"),
				   i18n_pspec("The end beat of the midi loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MIDI_LOOP_END,
				  param_spec);
}

void
ags_count_beats_audio_init(AgsCountBeatsAudio *count_beats_audio)
{
  GList *port;

  g_signal_connect_after(count_beats_audio, "notify::output-soundcard",
			 G_CALLBACK(ags_count_beats_audio_notify_output_soundcard_callback), NULL);

  AGS_RECALL(count_beats_audio)->name = "ags-count-beats";
  AGS_RECALL(count_beats_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(count_beats_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(count_beats_audio)->xml_type = "ags-count-beats-audio";

  port = NULL;

  /* sequencer loop */
  count_beats_audio->sequencer_loop = g_object_new(AGS_TYPE_PORT,
						   "plugin-name", ags_count_beats_audio_plugin_name,
						   "specifier", ags_count_beats_audio_specifier[0],
						   "control-port", ags_count_beats_audio_control_port[0],
						   "port-value-is-pointer", FALSE,
						   "port-value-type", G_TYPE_BOOLEAN,
						   "port-value-size", sizeof(gboolean),
						   "port-value-length", 1,
						   NULL);
  g_object_ref(count_beats_audio->sequencer_loop);
  
  count_beats_audio->sequencer_loop->port_value.ags_port_boolean = FALSE;

  /* add to port */
  port = g_list_prepend(port, count_beats_audio->sequencer_loop);
  g_object_ref(count_beats_audio->sequencer_loop);
  
  /* sequencer-loop-start  */
  count_beats_audio->sequencer_loop_start = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_count_beats_audio_plugin_name,
							 "specifier", ags_count_beats_audio_specifier[1],
							 "control-port", ags_count_beats_audio_control_port[1],
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_DOUBLE,
							 "port-value-size", sizeof(gdouble),
							 "port-value-length", 1,
							 NULL);
  g_object_ref(count_beats_audio->sequencer_loop_start);
  
  count_beats_audio->sequencer_loop_start->port_value.ags_port_double = 0.0;

  /* add to port */
  port = g_list_prepend(port, count_beats_audio->sequencer_loop_start);
  g_object_ref(count_beats_audio->sequencer_loop_start);
  
  /* sequencer-loop-end */
  count_beats_audio->sequencer_loop_end = g_object_new(AGS_TYPE_PORT,
						       "plugin-name", ags_count_beats_audio_plugin_name,
						       "specifier", ags_count_beats_audio_specifier[3],
						       "control-port", ags_count_beats_audio_control_port[3],
						       "port-value-is-pointer", FALSE,
						       "port-value-type", G_TYPE_DOUBLE,
						       "port-value-size", sizeof(gdouble),
						       "port-value-length", 1,
						       NULL);
  g_object_ref(count_beats_audio->sequencer_loop_end);
  
  count_beats_audio->sequencer_loop_end->port_value.ags_port_double = 16.0;

  /* add to port */
  port = g_list_prepend(port, count_beats_audio->sequencer_loop_end);
  g_object_ref(count_beats_audio->sequencer_loop_end);

  /* notation loop */
  count_beats_audio->notation_loop = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_count_beats_audio_plugin_name,
						  "specifier", ags_count_beats_audio_specifier[0],
						  "control-port", ags_count_beats_audio_control_port[0],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_BOOLEAN,
						  "port-value-size", sizeof(gboolean),
						  "port-value-length", 1,
						  NULL);
  g_object_ref(count_beats_audio->notation_loop);
  
  count_beats_audio->notation_loop->port_value.ags_port_boolean = FALSE;

  /* add to port */
  port = g_list_prepend(port, count_beats_audio->notation_loop);
  g_object_ref(count_beats_audio->notation_loop);

  /* notation-loop-start */
  count_beats_audio->notation_loop_start = g_object_new(AGS_TYPE_PORT,
							"plugin-name", ags_count_beats_audio_plugin_name,
							"specifier", ags_count_beats_audio_specifier[2],
							"control-port", ags_count_beats_audio_control_port[2],
							"port-value-is-pointer", FALSE,
							"port-value-type", G_TYPE_DOUBLE,
							"port-value-size", sizeof(gdouble),
							"port-value-length", 1,
							NULL);
  g_object_ref(count_beats_audio->notation_loop_start);
  
  count_beats_audio->notation_loop_start->port_value.ags_port_double = 0.0;

  /* add to port */
  port = g_list_prepend(port, count_beats_audio->notation_loop_start);
  g_object_ref(count_beats_audio->notation_loop_start);
  
  /* notation-loop-end */
  count_beats_audio->notation_loop_end = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_count_beats_audio_plugin_name,
						      "specifier", ags_count_beats_audio_specifier[4],
						      "control-port", ags_count_beats_audio_control_port[4],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_DOUBLE,
						      "port-value-size", sizeof(gdouble),
						      "port-value-length", 1,
						      NULL);
  g_object_ref(count_beats_audio->notation_loop_end);
  
  count_beats_audio->notation_loop_end->port_value.ags_port_double = 64.0;
  
  /* add to port */
  port = g_list_prepend(port, count_beats_audio->notation_loop_end);
  g_object_ref(count_beats_audio->notation_loop_end);

  /* wave loop */
  count_beats_audio->wave_loop = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_count_beats_audio_plugin_name,
					      "specifier", ags_count_beats_audio_specifier[0],
					      "control-port", ags_count_beats_audio_control_port[0],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_BOOLEAN,
					      "port-value-size", sizeof(gboolean),
					      "port-value-length", 1,
					      NULL);
  g_object_ref(count_beats_audio->wave_loop);
  
  count_beats_audio->wave_loop->port_value.ags_port_boolean = FALSE;

  /* add to port */
  port = g_list_prepend(port, count_beats_audio->wave_loop);
  g_object_ref(count_beats_audio->wave_loop);
  
  /* wave-loop-start  */
  count_beats_audio->wave_loop_start = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_count_beats_audio_plugin_name,
						    "specifier", ags_count_beats_audio_specifier[1],
						    "control-port", ags_count_beats_audio_control_port[1],
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_DOUBLE,
						    "port-value-size", sizeof(gdouble),
						    "port-value-length", 1,
						    NULL);
  g_object_ref(count_beats_audio->wave_loop_start);
  
  count_beats_audio->wave_loop_start->port_value.ags_port_double = 0.0;

  /* add to port */
  port = g_list_prepend(port, count_beats_audio->wave_loop_start);
  g_object_ref(count_beats_audio->wave_loop_start);
  
  /* wave-loop-end */
  count_beats_audio->wave_loop_end = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_count_beats_audio_plugin_name,
						  "specifier", ags_count_beats_audio_specifier[3],
						  "control-port", ags_count_beats_audio_control_port[3],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_DOUBLE,
						  "port-value-size", sizeof(gdouble),
						  "port-value-length", 1,
						  NULL);
  g_object_ref(count_beats_audio->wave_loop_end);
  
  count_beats_audio->wave_loop_end->port_value.ags_port_double = 16.0;

  /* add to port */
  port = g_list_prepend(port, count_beats_audio->wave_loop_end);
  g_object_ref(count_beats_audio->wave_loop_end);

  /* midi loop */
  count_beats_audio->midi_loop = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_count_beats_audio_plugin_name,
					      "specifier", ags_count_beats_audio_specifier[0],
					      "control-port", ags_count_beats_audio_control_port[0],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_BOOLEAN,
					      "port-value-size", sizeof(gboolean),
					      "port-value-length", 1,
					      NULL);
  g_object_ref(count_beats_audio->midi_loop);
  
  count_beats_audio->midi_loop->port_value.ags_port_boolean = FALSE;

  /* add to port */
  port = g_list_prepend(port, count_beats_audio->midi_loop);
  g_object_ref(count_beats_audio->midi_loop);
  
  /* midi-loop-start  */
  count_beats_audio->midi_loop_start = g_object_new(AGS_TYPE_PORT,
						    "plugin-name", ags_count_beats_audio_plugin_name,
						    "specifier", ags_count_beats_audio_specifier[1],
						    "control-port", ags_count_beats_audio_control_port[1],
						    "port-value-is-pointer", FALSE,
						    "port-value-type", G_TYPE_DOUBLE,
						    "port-value-size", sizeof(gdouble),
						    "port-value-length", 1,
						    NULL);
  g_object_ref(count_beats_audio->midi_loop_start);
  
  count_beats_audio->midi_loop_start->port_value.ags_port_double = 0.0;

  /* add to port */
  port = g_list_prepend(port, count_beats_audio->midi_loop_start);
  g_object_ref(count_beats_audio->midi_loop_start);
  
  /* midi-loop-end */
  count_beats_audio->midi_loop_end = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_count_beats_audio_plugin_name,
						  "specifier", ags_count_beats_audio_specifier[3],
						  "control-port", ags_count_beats_audio_control_port[3],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_DOUBLE,
						  "port-value-size", sizeof(gdouble),
						  "port-value-length", 1,
						  NULL);
  g_object_ref(count_beats_audio->midi_loop_end);
  
  count_beats_audio->midi_loop_end->port_value.ags_port_double = 16.0;

  /* add to port */
  port = g_list_prepend(port, count_beats_audio->midi_loop_end);
  g_object_ref(count_beats_audio->midi_loop_end);

  /* port */
  AGS_RECALL(count_beats_audio)->port = port;
}

void
ags_count_beats_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsCountBeatsAudio *count_beats_audio;

  pthread_mutex_t *recall_mutex;
  
  count_beats_audio = AGS_COUNT_BEATS_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_SEQUENCER_LOOP:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == count_beats_audio->sequencer_loop){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(count_beats_audio->sequencer_loop != NULL){
	g_object_unref(G_OBJECT(count_beats_audio->sequencer_loop));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      count_beats_audio->sequencer_loop = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SEQUENCER_LOOP_START:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == count_beats_audio->sequencer_loop_start){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(count_beats_audio->sequencer_loop_start != NULL){
	g_object_unref(G_OBJECT(count_beats_audio->sequencer_loop_start));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      count_beats_audio->sequencer_loop_start = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SEQUENCER_LOOP_END:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == count_beats_audio->sequencer_loop_end){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(count_beats_audio->sequencer_loop_end != NULL){
	g_object_unref(G_OBJECT(count_beats_audio->sequencer_loop_end));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      count_beats_audio->sequencer_loop_end = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION_LOOP:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == count_beats_audio->notation_loop){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(count_beats_audio->notation_loop != NULL){
	g_object_unref(G_OBJECT(count_beats_audio->notation_loop));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      count_beats_audio->notation_loop = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION_LOOP_START:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == count_beats_audio->notation_loop_start){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(count_beats_audio->notation_loop_start != NULL){
	g_object_unref(G_OBJECT(count_beats_audio->notation_loop_start));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      count_beats_audio->notation_loop_start = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION_LOOP_END:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == count_beats_audio->notation_loop_end){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(count_beats_audio->notation_loop_end != NULL){
	g_object_unref(G_OBJECT(count_beats_audio->notation_loop_end));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      count_beats_audio->notation_loop_end = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == count_beats_audio->wave_loop){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(count_beats_audio->wave_loop != NULL){
	g_object_unref(G_OBJECT(count_beats_audio->wave_loop));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      count_beats_audio->wave_loop = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP_START:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == count_beats_audio->wave_loop_start){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(count_beats_audio->wave_loop_start != NULL){
	g_object_unref(G_OBJECT(count_beats_audio->wave_loop_start));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      count_beats_audio->wave_loop_start = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP_END:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == count_beats_audio->wave_loop_end){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(count_beats_audio->wave_loop_end != NULL){
	g_object_unref(G_OBJECT(count_beats_audio->wave_loop_end));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      count_beats_audio->wave_loop_end = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_MIDI_LOOP:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == count_beats_audio->midi_loop){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(count_beats_audio->midi_loop != NULL){
	g_object_unref(G_OBJECT(count_beats_audio->midi_loop));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      count_beats_audio->midi_loop = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_MIDI_LOOP_START:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == count_beats_audio->midi_loop_start){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(count_beats_audio->midi_loop_start != NULL){
	g_object_unref(G_OBJECT(count_beats_audio->midi_loop_start));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      count_beats_audio->midi_loop_start = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_MIDI_LOOP_END:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == count_beats_audio->midi_loop_end){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(count_beats_audio->midi_loop_end != NULL){
	g_object_unref(G_OBJECT(count_beats_audio->midi_loop_end));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      count_beats_audio->midi_loop_end = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_count_beats_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsCountBeatsAudio *count_beats;
  
  pthread_mutex_t *recall_mutex;

  count_beats = AGS_COUNT_BEATS_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_SEQUENCER_LOOP:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, count_beats->sequencer_loop);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SEQUENCER_LOOP_START:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, count_beats->sequencer_loop_start);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SEQUENCER_LOOP_END:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, count_beats->sequencer_loop_end);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION_LOOP:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, count_beats->notation_loop);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION_LOOP_START:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, count_beats->notation_loop_start);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_NOTATION_LOOP_END:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, count_beats->notation_loop_end);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, count_beats->wave_loop);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP_START:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, count_beats->wave_loop_start);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP_END:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, count_beats->wave_loop_end);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_MIDI_LOOP:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, count_beats->midi_loop);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_MIDI_LOOP_START:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, count_beats->midi_loop_start);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_MIDI_LOOP_END:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, count_beats->midi_loop_end);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_count_beats_audio_dispose(GObject *gobject)
{
  AgsCountBeatsAudio *count_beats_audio;

  count_beats_audio = AGS_COUNT_BEATS_AUDIO(gobject);

  /* sequencer */
  if(count_beats_audio->sequencer_loop != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->sequencer_loop));

    count_beats_audio->sequencer_loop = NULL;
  }

  if(count_beats_audio->sequencer_loop_start != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->sequencer_loop_start));

    count_beats_audio->sequencer_loop_start = NULL;
  }

  if(count_beats_audio->sequencer_loop_end != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->sequencer_loop_end));

    count_beats_audio->sequencer_loop_end = NULL;
  }

  /* notation */
  if(count_beats_audio->notation_loop != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->notation_loop));

    count_beats_audio->notation_loop = NULL;
  }

  if(count_beats_audio->notation_loop_start != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->notation_loop_start));

    count_beats_audio->notation_loop_start = NULL;
  }

  if(count_beats_audio->notation_loop_end != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->notation_loop_end));

    count_beats_audio->notation_loop_end = NULL;
  }

  /* wave */
  if(count_beats_audio->wave_loop != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->wave_loop));

    count_beats_audio->wave_loop = NULL;
  }

  if(count_beats_audio->wave_loop_start != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->wave_loop_start));

    count_beats_audio->wave_loop_start = NULL;
  }

  if(count_beats_audio->wave_loop_end != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->wave_loop_end));

    count_beats_audio->wave_loop_end = NULL;
  }

  /* midi */
  if(count_beats_audio->midi_loop != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->midi_loop));

    count_beats_audio->midi_loop = NULL;
  }

  if(count_beats_audio->midi_loop_start != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->midi_loop_start));

    count_beats_audio->midi_loop_start = NULL;
  }

  if(count_beats_audio->midi_loop_end != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->midi_loop_end));

    count_beats_audio->midi_loop_end = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_count_beats_audio_parent_class)->dispose(gobject);
}

void
ags_count_beats_audio_finalize(GObject *gobject)
{
  AgsCountBeatsAudio *count_beats_audio;

  count_beats_audio = AGS_COUNT_BEATS_AUDIO(gobject);

  /* sequencer */
  if(count_beats_audio->sequencer_loop != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->sequencer_loop));
  }

  if(count_beats_audio->sequencer_loop_start != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->sequencer_loop_start));
  }

  if(count_beats_audio->sequencer_loop_end != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->sequencer_loop_end));
  }

  /* notation */
  if(count_beats_audio->notation_loop_start != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->notation_loop_start));
  }

  if(count_beats_audio->notation_loop_end != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->notation_loop_end));
  }

  if(count_beats_audio->notation_loop != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->notation_loop));
  }

  /* wave */
  if(count_beats_audio->wave_loop != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->wave_loop));
  }

  if(count_beats_audio->wave_loop_start != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->wave_loop_start));
  }

  if(count_beats_audio->wave_loop_end != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->wave_loop_end));
  }

  /* midi */
  if(count_beats_audio->midi_loop != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->midi_loop));
  }

  if(count_beats_audio->midi_loop_start != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->midi_loop_start));
  }

  if(count_beats_audio->midi_loop_end != NULL){
    g_object_unref(G_OBJECT(count_beats_audio->midi_loop_end));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_count_beats_audio_parent_class)->finalize(gobject);
}

void
ags_count_beats_audio_notify_output_soundcard_callback(GObject *gobject,
						       GParamSpec *pspec,
						       gpointer user_data)
{
  AgsCountBeatsAudio *count_beats_audio;
  AgsPort *notation_loop;
  AgsPort *notation_loop_start;
  AgsPort *notation_loop_end;
  AgsPort *wave_loop;
  AgsPort *wave_loop_start;
  AgsPort *wave_loop_end;
  AgsPort *midi_loop;
  AgsPort *midi_loop_start;
  AgsPort *midi_loop_end;
  
  GObject *output_soundcard;
  
  guint loop_start, loop_end;
  gboolean do_loop;

  GValue loop_start_value = {0,};
  GValue loop_end_value = {0,};
  GValue do_loop_value = {0,};

  count_beats_audio = AGS_COUNT_BEATS_AUDIO(gobject);

  g_object_get(gobject,
	       "output-soundcard", &output_soundcard,
	       NULL);
  
  if(output_soundcard == NULL){
    return;
  }
  
  /* retrieve loop information */
  ags_soundcard_get_loop(AGS_SOUNDCARD(output_soundcard),
			 &loop_start, &loop_end,
			 &do_loop);

  /* set loop information on ports */
  g_object_get(gobject,
	       "notation-loop", &notation_loop,
	       "notation-loop-start", &notation_loop_start,
	       "notation-loop-end", &notation_loop_end,
	       "wave-loop", &wave_loop,
	       "wave-loop-start", &wave_loop_start,
	       "wave-loop-end", &wave_loop_end,
	       "midi-loop", &midi_loop,
	       "midi-loop-start", &midi_loop_start,
	       "midi-loop-end", &midi_loop_end,
	       NULL);
  
  g_value_init(&do_loop_value, G_TYPE_BOOLEAN);
  g_value_init(&loop_start_value, G_TYPE_DOUBLE);
  g_value_init(&loop_end_value, G_TYPE_DOUBLE);

  g_value_set_double(&loop_start_value,
		     (gdouble) loop_start);
  g_value_set_double(&loop_end_value,
		     (gdouble) loop_end);
  g_value_set_boolean(&do_loop_value,
		      do_loop);

  /* notation */
  ags_port_safe_write(count_beats_audio->notation_loop,
		      &do_loop_value);
  ags_port_safe_write(count_beats_audio->notation_loop_start,
		      &loop_start_value);
  ags_port_safe_write(count_beats_audio->notation_loop_end,
		      &loop_end_value);

  /* wave */
  ags_port_safe_write(count_beats_audio->wave_loop,
		      &do_loop_value);
  ags_port_safe_write(count_beats_audio->wave_loop_start,
		      &loop_start_value);
  ags_port_safe_write(count_beats_audio->wave_loop_end,
		      &loop_end_value);

  /* midi */
  ags_port_safe_write(count_beats_audio->midi_loop,
		      &do_loop_value);
  ags_port_safe_write(count_beats_audio->midi_loop_start,
		      &loop_start_value);
  ags_port_safe_write(count_beats_audio->midi_loop_end,
		      &loop_end_value);
}

void
ags_count_beats_audio_change_sequencer_duration(AgsTactable *tactable, gdouble duration)
{
  AgsCountBeatsAudio *count_beats_audio;

  GValue value = {0,};
  
  count_beats_audio = AGS_COUNT_BEATS_AUDIO(tactable);

  g_value_init(&value, G_TYPE_DOUBLE);
  g_value_set_double(&value, duration);
  ags_port_safe_write(count_beats_audio->sequencer_loop_end,
		      &value);
}

void
ags_count_beats_audio_change_notation_duration(AgsTactable *tactable, gdouble duration)
{
  AgsCountBeatsAudio *count_beats_audio;

  GValue value = {0,};
  
  count_beats_audio = AGS_COUNT_BEATS_AUDIO(tactable);

  g_value_init(&value, G_TYPE_DOUBLE);
  g_value_set_double(&value, duration);
  ags_port_safe_write(count_beats_audio->notation_loop_end,
		      &value);
}

void
ags_count_beats_audio_change_wave_duration(AgsTactable *tactable, gdouble duration)
{
  AgsCountBeatsAudio *count_beats_audio;

  GValue value = {0,};
  
  count_beats_audio = AGS_COUNT_BEATS_AUDIO(tactable);

  g_value_init(&value, G_TYPE_DOUBLE);
  g_value_set_double(&value, duration);
  ags_port_safe_write(count_beats_audio->wave_loop_end,
		      &value);
}

void
ags_count_beats_audio_change_midi_duration(AgsTactable *tactable, gdouble duration)
{
  AgsCountBeatsAudio *count_beats_audio;

  GValue value = {0,};
  
  count_beats_audio = AGS_COUNT_BEATS_AUDIO(tactable);

  g_value_init(&value, G_TYPE_DOUBLE);
  g_value_set_double(&value, duration);
  ags_port_safe_write(count_beats_audio->midi_loop_end,
		      &value);
}

void
ags_count_beats_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./sequencer-loop[0]",
		      19)){
      g_object_set(G_OBJECT(plugin),
		   "sequencer-loop", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./sequencer-loop-start[0]",
		      24)){
      g_object_set(G_OBJECT(plugin),
		   "sequencer-loop-start", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./sequencer-loop-end[0]",
		      22)){
      g_object_set(G_OBJECT(plugin),
		   "sequencer-loop-end", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		"./notation-loop[0]",
		18)){
      g_object_set(G_OBJECT(plugin),
		   "notation-loop", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./notation-loop-start[0]",
		      23)){
      g_object_set(G_OBJECT(plugin),
		   "notation-loop-start", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./notation-loop-end[0]",
		      21)){
      g_object_set(G_OBJECT(plugin),
		   "notation-loop-end", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./wave-loop[0]",
		      19)){
      g_object_set(G_OBJECT(plugin),
		   "wave-loop", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./wave-loop-start[0]",
		      24)){
      g_object_set(G_OBJECT(plugin),
		   "wave-loop-start", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./wave-loop-end[0]",
		      22)){
      g_object_set(G_OBJECT(plugin),
		   "wave-loop-end", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./midi-loop[0]",
		      19)){
      g_object_set(G_OBJECT(plugin),
		   "midi-loop", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./midi-loop-start[0]",
		      24)){
      g_object_set(G_OBJECT(plugin),
		   "midi-loop-start", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./midi-loop-end[0]",
		      22)){
      g_object_set(G_OBJECT(plugin),
		   "midi-loop-end", AGS_PORT(port->data),
		   NULL);
    }
    
    port = port->next;
  }
}

/**
 * ags_count_beats_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsCountBeatsAudio
 *
 * Returns: the new #AgsCountBeatsAudio
 *
 * Since: 2.0.0
 */
AgsCountBeatsAudio*
ags_count_beats_audio_new(AgsAudio *audio)
{
  AgsCountBeatsAudio *count_beats_audio;

  count_beats_audio = (AgsCountBeatsAudio *) g_object_new(AGS_TYPE_COUNT_BEATS_AUDIO,
							  "audio", audio,
							  NULL);
  
  return(count_beats_audio);
}
