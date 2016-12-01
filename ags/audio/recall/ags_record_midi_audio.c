/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/recall/ags_record_midi_audio.h>

#include <ags/object/ags_plugin.h>

void ags_record_midi_audio_class_init(AgsRecordMidiAudioClass *record_midi_audio);
void ags_record_midi_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_record_midi_audio_init(AgsRecordMidiAudio *record_midi_audio);
void ags_record_midi_audio_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_record_midi_audio_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_record_midi_audio_finalize(GObject *gobject);
void ags_record_midi_audio_set_ports(AgsPlugin *plugin, GList *port);

/**
 * SECTION:ags_record_midi_audio
 * @short_description: record audio midi
 * @title: AgsRecordMidiAudio
 * @section_id:
 * @include: ags/audio/recall/ags_record_midi_audio.h
 *
 * The #AgsRecordMidiAudio class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_PLAYBACK,
  PROP_RECORD,
  PROP_FILENAME,
  PROP_DIVISION,
  PROP_TEMPO,
  PROP_BPM,
};

static gpointer ags_record_midi_audio_parent_class = NULL;
static AgsPluginInterface *ags_record_midi_parent_plugin_interface;

static const gchar *ags_record_midi_audio_plugin_name = "ags-record-midi\0";
static const gchar *ags_record_midi_audio_specifier[] = {
  "playback[0]\0"
  "record[0]\0",
  "filename[0]\0",
  "division[0]\0",
  "tempo[0]\0",
  "bpm[0]\0",
};
static const gchar *ags_record_midi_audio_control_port[] = {
  "1/6\0",
  "2/6\0",
  "3/6\0",
  "4/6\0",
  "5/6\0",
  "6/6\0",
};

GType
ags_record_midi_audio_get_type()
{
  static GType ags_type_record_midi_audio = 0;

  if(!ags_type_record_midi_audio){
    static const GTypeInfo ags_record_midi_audio_info = {
      sizeof (AgsRecordMidiAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_record_midi_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsRecordMidiAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_record_midi_audio_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_record_midi_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_record_midi_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							"AgsRecordMidiAudio\0",
							&ags_record_midi_audio_info,
							0);

    g_type_add_interface_static(ags_type_record_midi_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_record_midi_audio);
}

void
ags_record_midi_audio_class_init(AgsRecordMidiAudioClass *record_midi_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_record_midi_audio_parent_class = g_type_class_peek_parent(record_midi_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) record_midi_audio;

  gobject->set_property = ags_record_midi_audio_set_property;
  gobject->get_property = ags_record_midi_audio_get_property;

  gobject->finalize = ags_record_midi_audio_finalize;

  /* properties */
  param_spec = g_param_spec_object("playback\0",
				   "if do playback\0",
				   "If playback should be performed\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAYBACK,
				  param_spec);

  param_spec = g_param_spec_object("record\0",
				   "if do record\0",
				   "If record data for later use should be done\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECORD,
				  param_spec);

  param_spec = g_param_spec_object("filename\0",
				   "filename of record\0",
				   "The filename of record\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  param_spec = g_param_spec_object("division\0",
				   "division of record\0",
				   "The division of record\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DIVISION,
				  param_spec);

  param_spec = g_param_spec_object("tempo\0",
				   "tempo of record\0",
				   "The tempo of record\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TEMPO,
				  param_spec);

  param_spec = g_param_spec_object("bpm\0",
				   "bpm of record\0",
				   "The bpm of record\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);
}

void
ags_record_midi_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_record_midi_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_record_midi_audio_set_ports;
}

void
ags_record_midi_audio_init(AgsRecordMidiAudio *record_midi_audio)
{
  GList *port;

  AGS_RECALL(record_midi_audio)->name = "ags-record-midi\0";
  AGS_RECALL(record_midi_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(record_midi_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(record_midi_audio)->xml_type = "ags-record-midi-audio\0";

  port = NULL;

  /* playback */
  record_midi_audio->playback = g_object_new(AGS_TYPE_PORT,
					     "plugin-name\0", ags_record_midi_audio_plugin_name,
					     "specifier\0", ags_record_midi_audio_specifier[0],
					     "control-port\0", ags_record_midi_audio_control_port[0],
					     "port-value-is-pointer\0", FALSE,
					     "port-value-type\0", G_TYPE_BOOLEAN,
					     NULL);

  record_midi_audio->playback->port_value.ags_port_boolean = TRUE;

  port = g_list_prepend(port, record_midi_audio->playback);

  /* record */
  record_midi_audio->record = g_object_new(AGS_TYPE_PORT,
					   "plugin-name\0", ags_record_midi_audio_plugin_name,
					   "specifier\0", ags_record_midi_audio_specifier[1],
					   "control-port\0", ags_record_midi_audio_control_port[1],
					   "port-value-is-pointer\0", FALSE,
					   "port-value-type\0", G_TYPE_BOOLEAN,
					   NULL);

  record_midi_audio->record->port_value.ags_port_boolean = FALSE;

  port = g_list_prepend(port, record_midi_audio->record);

  /* filename */
  record_midi_audio->filename = g_object_new(AGS_TYPE_PORT,
					     "plugin-name\0", ags_record_midi_audio_plugin_name,
					     "specifier\0", ags_record_midi_audio_specifier[2],
					     "control-port\0", ags_record_midi_audio_control_port[2],
					     "port-value-is-pointer\0", FALSE,
					     "port-value-type\0", G_TYPE_BOOLEAN,
					     NULL);

  record_midi_audio->filename->port_value.ags_port_boolean = TRUE;

  port = g_list_prepend(port, record_midi_audio->filename);

  /* division */
  record_midi_audio->division = g_object_new(AGS_TYPE_PORT,
					     "plugin-name\0", ags_record_midi_audio_plugin_name,
					     "specifier\0", ags_record_midi_audio_specifier[2],
					     "control-port\0", ags_record_midi_audio_control_port[2],
					     "port-value-is-pointer\0", FALSE,
					     "port-value-type\0", G_TYPE_INT64,
					     NULL);

  record_midi_audio->division->port_value.ags_port_int = 0;

  port = g_list_prepend(port, record_midi_audio->division);

  /* tempo */
  record_midi_audio->tempo = g_object_new(AGS_TYPE_PORT,
					  "plugin-name\0", ags_record_midi_audio_plugin_name,
					  "specifier\0", ags_record_midi_audio_specifier[2],
					  "control-port\0", ags_record_midi_audio_control_port[2],
					  "port-value-is-pointer\0", FALSE,
					  "port-value-type\0", G_TYPE_INT64,
					  NULL);

  record_midi_audio->tempo->port_value.ags_port_int = 0;

  port = g_list_prepend(port, record_midi_audio->tempo);

  /* bpm */
  record_midi_audio->bpm = g_object_new(AGS_TYPE_PORT,
					"plugin-name\0", ags_record_midi_audio_plugin_name,
					"specifier\0", ags_record_midi_audio_specifier[2],
					"control-port\0", ags_record_midi_audio_control_port[2],
					"port-value-is-pointer\0", FALSE,
					"port-value-type\0", G_TYPE_INT64,
					NULL);

  record_midi_audio->bpm->port_value.ags_port_int = 120;

  port = g_list_prepend(port, record_midi_audio->bpm);

  /* set port */
  AGS_RECALL(record_midi_audio)->port = port;
}

void
ags_record_midi_audio_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsRecordMidiAudio *record_midi_audio;

  record_midi_audio = AGS_RECORD_MIDI_AUDIO(gobject);

  switch(prop_id){
  case PROP_PLAYBACK:
    {
      AgsPort *playback;

      playback = (AgsPort *) g_value_get_object(value);

      if(record_midi_audio->playback == playback){
	return;
      }

      if(record_midi_audio->playback != NULL){
	g_object_unref(G_OBJECT(record_midi_audio->playback));
      }
      
      if(playback != NULL){
	g_object_ref(G_OBJECT(playback));
      }
      
      record_midi_audio->playback = playback;
    }
    break;
  case PROP_RECORD:
    {
      AgsPort *record;

      record = (AgsPort *) g_value_get_object(value);

      if(record_midi_audio->record == record){
	return;
      }

      if(record_midi_audio->record != NULL){
	g_object_unref(G_OBJECT(record_midi_audio->record));
      }
      
      if(record != NULL){
	g_object_ref(G_OBJECT(record));
      }
      
      record_midi_audio->record = record;
    }
    break;
  case PROP_FILENAME:
    {
      AgsPort *filename;

      filename = (AgsPort *) g_value_get_object(value);

      if(record_midi_audio->filename == filename){
	return;
      }

      if(record_midi_audio->filename != NULL){
	g_object_unref(G_OBJECT(record_midi_audio->filename));
      }
      
      if(filename != NULL){
	g_object_ref(G_OBJECT(filename));
      }
      
      record_midi_audio->filename = filename;
    }
    break;
  case PROP_DIVISION:
    {
      AgsPort *division;

      division = (AgsPort *) g_value_get_object(value);

      if(record_midi_audio->division == division){
	return;
      }

      if(record_midi_audio->division != NULL){
	g_object_unref(G_OBJECT(record_midi_audio->division));
      }
      
      if(division != NULL){
	g_object_ref(G_OBJECT(division));
      }
      
      record_midi_audio->division = division;
    }
    break;
  case PROP_TEMPO:
    {
      AgsPort *tempo;

      tempo = (AgsPort *) g_value_get_object(value);

      if(record_midi_audio->tempo == tempo){
	return;
      }

      if(record_midi_audio->tempo != NULL){
	g_object_unref(G_OBJECT(record_midi_audio->tempo));
      }
      
      if(tempo != NULL){
	g_object_ref(G_OBJECT(tempo));
      }
      
      record_midi_audio->tempo = tempo;
    }
    break;
  case PROP_BPM:
    {
      AgsPort *bpm;

      bpm = (AgsPort *) g_value_get_object(value);

      if(record_midi_audio->bpm == bpm){
	return;
      }

      if(record_midi_audio->bpm != NULL){
	g_object_unref(G_OBJECT(record_midi_audio->bpm));
      }
      
      if(bpm != NULL){
	g_object_ref(G_OBJECT(bpm));
      }
      
      record_midi_audio->bpm = bpm;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_record_midi_audio_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsRecordMidiAudio *record_midi_audio;

  record_midi_audio = AGS_RECORD_MIDI_AUDIO(gobject);

  switch(prop_id){
  case PROP_PLAYBACK:
    {
      g_value_set_object(value, record_midi_audio->playback);
    }
    break;
  case PROP_RECORD:
    {
      g_value_set_object(value, record_midi_audio->record);
    }
    break;
  case PROP_FILENAME:
    {
      g_value_set_object(value, record_midi_audio->filename);
    }
    break;
  case PROP_DIVISION:
    {
      g_value_set_object(value, record_midi_audio->division);
    }
    break;
  case PROP_TEMPO:
    {
      g_value_set_object(value, record_midi_audio->tempo);
    }
    break;
  case PROP_BPM:
    {
      g_value_set_object(value, record_midi_audio->bpm);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_record_midi_audio_finalize(GObject *gobject)
{
  AgsRecordMidiAudio *record_midi_audio;

  record_midi_audio = AGS_RECORD_MIDI_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_record_midi_audio_parent_class)->finalize(gobject);
}

void
ags_record_midi_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./playback[0]\0",
		11)){
      g_object_set(G_OBJECT(plugin),
		   "playback\0", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./record[0]\0",
		      11)){
      g_object_set(G_OBJECT(plugin),
		   "record\0", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./filename[0]\0",
		      11)){
      g_object_set(G_OBJECT(plugin),
		   "filename\0", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./division[0]\0",
		      11)){
      g_object_set(G_OBJECT(plugin),
		   "division\0", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./tempo[0]\0",
		      11)){
      g_object_set(G_OBJECT(plugin),
		   "tempo\0", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./bpm[0]\0",
		      11)){
      g_object_set(G_OBJECT(plugin),
		   "bpm\0", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

/**
 * ags_record_midi_audio_new:
 *
 * Creates an #AgsRecordMidiAudio
 *
 * Returns: a new #AgsRecordMidiAudio
 *
 * Since: 0.4
 */
AgsRecordMidiAudio*
ags_record_midi_audio_new()
{
  AgsRecordMidiAudio *record_midi_audio;

  record_midi_audio = (AgsRecordMidiAudio *) g_object_new(AGS_TYPE_RECORD_MIDI_AUDIO,
							  NULL);

  return(record_midi_audio);
}
