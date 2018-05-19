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

#include <ags/audio/recall/ags_capture_sound_audio.h>

#include <ags/libags.h>

#include <ags/i18n.h>

void ags_capture_sound_audio_class_init(AgsCaptureSoundAudioClass *capture_sound_audio);
void ags_capture_sound_audio_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_capture_sound_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_capture_sound_audio_init(AgsCaptureSoundAudio *capture_sound_audio);
void ags_capture_sound_audio_set_property(GObject *gobject,
					  guint prop_id,
					  const GValue *value,
					  GParamSpec *param_spec);
void ags_capture_sound_audio_get_property(GObject *gobject,
					  guint prop_id,
					  GValue *value,
					  GParamSpec *param_spec);
void ags_capture_sound_audio_finalize(GObject *gobject);
void ags_capture_sound_audio_set_ports(AgsPlugin *plugin, GList *port);

/**
 * SECTION:ags_capture_sound_audio
 * @short_description: capture audio sound
 * @title: AgsCaptureSoundAudio
 * @section_id:
 * @include: ags/audio/recall/ags_capture_sound_audio.h
 *
 * The #AgsCaptureSoundAudio class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_PLAYBACK,
  PROP_RECORD,
  PROP_FILENAME,
  PROP_AUDIO_CHANNELS,
  PROP_FORMAT,
  PROP_SAMPLERATE,
  PROP_BUFFER_SIZE,
};

static gpointer ags_capture_sound_audio_parent_class = NULL;
static AgsConnectableInterface *ags_capture_sound_audio_parent_connectable_interface;
static AgsPluginInterface *ags_capture_sound_parent_plugin_interface;

static const gchar *ags_capture_sound_audio_plugin_name = "ags-capture-sound";
static const gchar *ags_capture_sound_audio_specifier[] = {
  "./playback[0]"
  "./record[0]",
  "./filename[0]",
  "./audio-channels[0]",
  "./format[0]",
  "./samplerate[0]",
  "./buffer-size[0]",
};

static const gchar *ags_capture_sound_audio_control_port[] = {
  "1/7",
  "2/7",
  "3/7",
  "4/7",
  "5/7",
  "6/7",
  "7/7",
};

GType
ags_capture_sound_audio_get_type()
{
  static GType ags_type_capture_sound_audio = 0;

  if(!ags_type_capture_sound_audio){
    static const GTypeInfo ags_capture_sound_audio_info = {
      sizeof(AgsCaptureSoundAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_capture_sound_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof(AgsCaptureSoundAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_capture_sound_audio_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_capture_sound_audio_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_capture_sound_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_capture_sound_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							  "AgsCaptureSoundAudio",
							  &ags_capture_sound_audio_info,
							  0);

    g_type_add_interface_static(ags_type_capture_sound_audio,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_capture_sound_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_capture_sound_audio);
}

void
ags_capture_sound_audio_class_init(AgsCaptureSoundAudioClass *capture_sound_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_capture_sound_audio_parent_class = g_type_class_peek_parent(capture_sound_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) capture_sound_audio;

  gobject->set_property = ags_capture_sound_audio_set_property;
  gobject->get_property = ags_capture_sound_audio_get_property;

  gobject->finalize = ags_capture_sound_audio_finalize;

  /* properties */
  /**
   * AgsCaptureSoundAudio:playback:
   * 
   * The playback port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("playback",
				   i18n_pspec("if do playback"),
				   i18n_pspec("If playback should be performed"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLAYBACK,
				  param_spec);

  /**
   * AgsCaptureSoundAudio:record:
   * 
   * The record port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("record",
				   i18n_pspec("if do record"),
				   i18n_pspec("If record data for later use should be done"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_RECORD,
				  param_spec);

  /**
   * AgsCaptureSoundAudio:filename:
   * 
   * The filename port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("filename",
				   i18n_pspec("filename of record"),
				   i18n_pspec("The filename of record"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsCaptureSoundAudio:audio-channels:
   * 
   * The audio channels port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("audio-channels",
				   i18n_pspec("audio channels"),
				   i18n_pspec("Audio channels count of file"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNELS,
				  param_spec);

  /**
   * AgsCaptureSoundAudio:format:
   * 
   * The format port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("format",
				   i18n_pspec("format"),
				   i18n_pspec("Format to use of file"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsCaptureSoundAudio:samplerate:
   * 
   * The samplerate port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("samplerate",
				   i18n_pspec("samplerate"),
				   i18n_pspec("Samplerate to use of file"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsCaptureSoundAudio:buffer-size:
   * 
   * The buffer size port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("buffer-size",
				   i18n_pspec("buffer size"),
				   i18n_pspec("Buffer size to use"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);
}

void
ags_capture_sound_audio_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_capture_sound_audio_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_capture_sound_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_capture_sound_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_capture_sound_audio_set_ports;
}

void
ags_capture_sound_audio_init(AgsCaptureSoundAudio *capture_sound_audio)
{
  GList *port;

  AGS_RECALL(capture_sound_audio)->name = "ags-capture-sound";
  AGS_RECALL(capture_sound_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(capture_sound_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(capture_sound_audio)->xml_type = "ags-capture-sound-audio";

  port = NULL;

  /* playback */
  capture_sound_audio->playback = g_object_new(AGS_TYPE_PORT,
					       "plugin-name", ags_capture_sound_audio_plugin_name,
					       "specifier", ags_capture_sound_audio_specifier[0],
					       "control-port", ags_capture_sound_audio_control_port[0],
					       "port-value-is-pointer", FALSE,
					       "port-value-type", G_TYPE_BOOLEAN,
					       NULL);
  g_object_ref(capture_sound_audio->playback);
  
  capture_sound_audio->playback->port_value.ags_port_boolean = TRUE;

  /* add to port */
  port = g_list_prepend(port, capture_sound_audio->playback);
  g_object_ref(capture_sound_audio->playback);
  
  /* record */
  capture_sound_audio->record = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_capture_sound_audio_plugin_name,
					     "specifier", ags_capture_sound_audio_specifier[1],
					     "control-port", ags_capture_sound_audio_control_port[1],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_BOOLEAN,
					     NULL);
  g_object_ref(capture_sound_audio->record);
  
  capture_sound_audio->record->port_value.ags_port_boolean = FALSE;

  /* add to port */
  port = g_list_prepend(port, capture_sound_audio->record);
  g_object_ref(capture_sound_audio->record);

  /* filename */
  capture_sound_audio->filename = g_object_new(AGS_TYPE_PORT,
					       "plugin-name", ags_capture_sound_audio_plugin_name,
					       "specifier", ags_capture_sound_audio_specifier[2],
					       "control-port", ags_capture_sound_audio_control_port[2],
					       "port-value-is-pointer", FALSE,
					       "port-value-type", G_TYPE_POINTER,
					       NULL);
  g_object_ref(capture_sound_audio->filename);

  capture_sound_audio->filename->port_value.ags_port_pointer = NULL;

  /* add to port */
  port = g_list_prepend(port, capture_sound_audio->filename);
  g_object_ref(capture_sound_audio->filename);

  /* audio channels */
  capture_sound_audio->audio_channels = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_capture_sound_audio_plugin_name,
					     "specifier", ags_capture_sound_audio_specifier[3],
					     "control-port", ags_capture_sound_audio_control_port[3],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_UINT,
					     NULL);
  g_object_ref(capture_sound_audio->audio_channels);
  
  capture_sound_audio->audio_channels->port_value.ags_port_uint = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;

  /* add to port */
  port = g_list_prepend(port, capture_sound_audio->audio_channels);
  g_object_ref(capture_sound_audio->audio_channels);

  /* format */
  capture_sound_audio->format = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_capture_sound_audio_plugin_name,
					     "specifier", ags_capture_sound_audio_specifier[4],
					     "control-port", ags_capture_sound_audio_control_port[4],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_UINT,
					     NULL);
  g_object_ref(capture_sound_audio->format);
  
  capture_sound_audio->format->port_value.ags_port_uint = AGS_SOUNDCARD_DEFAULT_FORMAT;

  /* add to port */
  port = g_list_prepend(port, capture_sound_audio->format);
  g_object_ref(capture_sound_audio->format);

  /* samplerate */
  capture_sound_audio->samplerate = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_capture_sound_audio_plugin_name,
					     "specifier", ags_capture_sound_audio_specifier[5],
					     "control-port", ags_capture_sound_audio_control_port[5],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_UINT,
					     NULL);
  g_object_ref(capture_sound_audio->samplerate);
  
  capture_sound_audio->samplerate->port_value.ags_port_uint = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  /* add to port */
  port = g_list_prepend(port, capture_sound_audio->samplerate);
  g_object_ref(capture_sound_audio->samplerate);

  /* buffer size */
  capture_sound_audio->buffer_size = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_capture_sound_audio_plugin_name,
					     "specifier", ags_capture_sound_audio_specifier[6],
					     "control-port", ags_capture_sound_audio_control_port[6],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_UINT,
					     NULL);
  g_object_ref(capture_sound_audio->buffer_size);
  
  capture_sound_audio->buffer_size->port_value.ags_port_uint = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  /* add to port */
  port = g_list_prepend(port, capture_sound_audio->buffer_size);
  g_object_ref(capture_sound_audio->buffer_size);

  /* set port */
  AGS_RECALL(capture_sound_audio)->port = port;
}

void
ags_capture_sound_audio_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec)
{
  AgsCaptureSoundAudio *capture_sound_audio;

  pthread_mutex_t *recall_mutex;
  
  capture_sound_audio = AGS_CAPTURE_SOUND_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_PLAYBACK:
    {
      AgsPort *playback;

      playback = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(capture_sound_audio->playback == playback){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_sound_audio->playback != NULL){
	g_object_unref(G_OBJECT(capture_sound_audio->playback));
      }
      
      if(playback != NULL){
	g_object_ref(G_OBJECT(playback));
      }
      
      capture_sound_audio->playback = playback;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECORD:
    {
      AgsPort *record;

      record = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(capture_sound_audio->record == record){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_sound_audio->record != NULL){
	g_object_unref(G_OBJECT(capture_sound_audio->record));
      }
      
      if(record != NULL){
	g_object_ref(G_OBJECT(record));
      }
      
      capture_sound_audio->record = record;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FILENAME:
    {
      AgsPort *filename;

      filename = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(capture_sound_audio->filename == filename){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_sound_audio->filename != NULL){
	g_object_unref(G_OBJECT(capture_sound_audio->filename));
      }
      
      if(filename != NULL){
	g_object_ref(G_OBJECT(filename));
      }
      
      capture_sound_audio->filename = filename;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_AUDIO_CHANNELS:
    {
      AgsPort *audio_channels;

      audio_channels = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(capture_sound_audio->audio_channels == audio_channels){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_sound_audio->audio_channels != NULL){
	g_object_unref(G_OBJECT(capture_sound_audio->audio_channels));
      }
      
      if(audio_channels != NULL){
	g_object_ref(G_OBJECT(audio_channels));
      }
      
      capture_sound_audio->audio_channels = audio_channels;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      AgsPort *format;

      format = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(capture_sound_audio->format == format){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_sound_audio->format != NULL){
	g_object_unref(G_OBJECT(capture_sound_audio->format));
      }
      
      if(format != NULL){
	g_object_ref(G_OBJECT(format));
      }
      
      capture_sound_audio->format = format;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      AgsPort *samplerate;

      samplerate = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(capture_sound_audio->samplerate == samplerate){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_sound_audio->samplerate != NULL){
	g_object_unref(G_OBJECT(capture_sound_audio->samplerate));
      }
      
      if(samplerate != NULL){
	g_object_ref(G_OBJECT(samplerate));
      }
      
      capture_sound_audio->samplerate = samplerate;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      AgsPort *buffer_size;

      buffer_size = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(capture_sound_audio->buffer_size == buffer_size){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_sound_audio->buffer_size != NULL){
	g_object_unref(G_OBJECT(capture_sound_audio->buffer_size));
      }
      
      if(buffer_size != NULL){
	g_object_ref(G_OBJECT(buffer_size));
      }
      
      capture_sound_audio->buffer_size = buffer_size;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_capture_sound_audio_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec)
{
  AgsCaptureSoundAudio *capture_sound_audio;

  pthread_mutex_t *recall_mutex;
  
  capture_sound_audio = AGS_CAPTURE_SOUND_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_PLAYBACK:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_sound_audio->playback);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECORD:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_sound_audio->record);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FILENAME:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_sound_audio->filename);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_AUDIO_CHANNELS:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_sound_audio->audio_channels);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_sound_audio->format);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_sound_audio->samplerate);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_sound_audio->buffer_size);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_capture_sound_audio_dispose(GObject *gobject)
{
  AgsCaptureSoundAudio *capture_sound_audio;

  capture_sound_audio = AGS_CAPTURE_SOUND_AUDIO(gobject);

  /* playback */
  if(capture_sound_audio->playback != NULL){
    g_object_unref(capture_sound_audio->playback);

    capture_sound_audio->playback = NULL;
  }

  /* record */
  if(capture_sound_audio->record != NULL){
    g_object_unref(capture_sound_audio->record);

    capture_sound_audio->record = NULL;
  }

  /* filename */
  if(capture_sound_audio->filename != NULL){
    g_object_unref(capture_sound_audio->filename);

    capture_sound_audio->filename = NULL;
  }

  /* audio channels */
  if(capture_sound_audio->audio_channels != NULL){
    g_object_unref(capture_sound_audio->audio_channels);

    capture_sound_audio->audio_channels = NULL;
  }

  /* format */
  if(capture_sound_audio->format != NULL){
    g_object_unref(capture_sound_audio->format);

    capture_sound_audio->format = NULL;
  }

  /* samplerate */
  if(capture_sound_audio->samplerate != NULL){
    g_object_unref(capture_sound_audio->samplerate);

    capture_sound_audio->samplerate = NULL;
  }

  /* buffer size */
  if(capture_sound_audio->buffer_size != NULL){
    g_object_unref(capture_sound_audio->buffer_size);

    capture_sound_audio->buffer_size = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_capture_sound_audio_parent_class)->dispose(gobject);
}

void
ags_capture_sound_audio_finalize(GObject *gobject)
{
  AgsCaptureSoundAudio *capture_sound_audio;

  capture_sound_audio = AGS_CAPTURE_SOUND_AUDIO(gobject);

  /* playback */
  if(capture_sound_audio->playback != NULL){
    g_object_unref(capture_sound_audio->playback);
  }

  /* record */
  if(capture_sound_audio->record != NULL){
    g_object_unref(capture_sound_audio->record);
  }

  /* filename */
  if(capture_sound_audio->filename != NULL){
    g_object_unref(capture_sound_audio->filename);
  }

  /* audio channels */
  if(capture_sound_audio->audio_channels != NULL){
    g_object_unref(capture_sound_audio->audio_channels);
  }

  /* format */
  if(capture_sound_audio->format != NULL){
    g_object_unref(capture_sound_audio->format);
  }

  /* samplerate */
  if(capture_sound_audio->samplerate != NULL){
    g_object_unref(capture_sound_audio->samplerate);
  }

  /* buffer size */
  if(capture_sound_audio->buffer_size != NULL){
    g_object_unref(capture_sound_audio->buffer_size);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_capture_sound_audio_parent_class)->finalize(gobject);
}

void
ags_capture_sound_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./playback[0]",
		11)){
      g_object_set(G_OBJECT(plugin),
		   "playback", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./record[0]",
		      11)){
      g_object_set(G_OBJECT(plugin),
		   "record", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./filename[0]",
		      11)){
      g_object_set(G_OBJECT(plugin),
		   "filename", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./audio-channels[0]",
		      11)){
      g_object_set(G_OBJECT(plugin),
		   "audio-channels", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./format[0]",
		      11)){
      g_object_set(G_OBJECT(plugin),
		   "format", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./samplerate[0]",
		      11)){
      g_object_set(G_OBJECT(plugin),
		   "samplerate", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./buffer-size[0]",
		      11)){
      g_object_set(G_OBJECT(plugin),
		   "buffer-size", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

/**
 * ags_capture_sound_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsCaptureSoundAudio
 *
 * Returns: the new #AgsCaptureSoundAudio
 *
 * Since: 2.0.0
 */
AgsCaptureSoundAudio*
ags_capture_sound_audio_new(AgsAudio *audio)
{
  AgsCaptureSoundAudio *capture_sound_audio;

  capture_sound_audio = (AgsCaptureSoundAudio *) g_object_new(AGS_TYPE_CAPTURE_SOUND_AUDIO,
							      "audio", audio,
							      NULL);

  return(capture_sound_audio);
}
