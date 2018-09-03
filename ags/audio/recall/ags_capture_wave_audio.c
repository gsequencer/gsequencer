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

#include <ags/audio/recall/ags_capture_wave_audio.h>

#include <ags/libags.h>

#include <stdlib.h>
#include <string.h>

#include <ags/i18n.h>

void ags_capture_wave_audio_class_init(AgsCaptureWaveAudioClass *capture_wave_audio);
void ags_capture_wave_audio_plugin_interface_init(AgsPluginInterface *plugin);
void ags_capture_wave_audio_init(AgsCaptureWaveAudio *capture_wave_audio);
void ags_capture_wave_audio_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_capture_wave_audio_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_capture_wave_audio_finalize(GObject *gobject);

void ags_capture_wave_audio_set_ports(AgsPlugin *plugin, GList *port);

/**
 * SECTION:ags_capture_wave_audio
 * @short_description: capture audio wave
 * @title: AgsCaptureWaveAudio
 * @section_id:
 * @include: ags/audio/recall/ags_capture_wave_audio.h
 *
 * The #AgsCaptureWaveAudio class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_PLAYBACK,
  PROP_REPLACE,
  PROP_RECORD,
  PROP_FILENAME,
  PROP_FILE_AUDIO_CHANNELS,
  PROP_FILE_SAMPLERATE,
  PROP_FILE_BUFFER_SIZE,
  PROP_FILE_FORMAT,
  PROP_WAVE_LOOP,
  PROP_WAVE_LOOP_START,
  PROP_WAVE_LOOP_END,
};

static gpointer ags_capture_wave_audio_parent_class = NULL;
static AgsPluginInterface *ags_capture_wave_audio_parent_plugin_interface;

static const gchar *ags_capture_wave_audio_plugin_name = "ags-capture-wave";
static const gchar *ags_capture_wave_audio_specifier[] = {
  "./playback[0]"
  "./replace[0]"
  "./record[0]",
  "./filename[0]",
  "./file-audio-channels[0]",
  "./file-samplerate[0]",
  "./file-buffer-size[0]",
  "./file-format[0]",
  "./wave_loop[0]",
  "./wave_loop_start[0]",
  "./wave_loop_end[0]",
};

static const gchar *ags_capture_wave_audio_control_port[] = {
  "1/11",
  "2/11",
  "3/11",
  "4/11",
  "5/11",
  "6/11",
  "7/11",
  "8/11",
  "9/11",
  "10/11",
  "11/11",
};

GType
ags_capture_wave_audio_get_type()
{
  static GType ags_type_capture_wave_audio = 0;

  if(!ags_type_capture_wave_audio){
    static const GTypeInfo ags_capture_wave_audio_info = {
      sizeof(AgsCaptureWaveAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_capture_wave_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof(AgsCaptureWaveAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_capture_wave_audio_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_capture_wave_audio_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };    

    ags_type_capture_wave_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
							 "AgsCaptureWaveAudio",
							 &ags_capture_wave_audio_info,
							 0);

    g_type_add_interface_static(ags_type_capture_wave_audio,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_capture_wave_audio);
}

void
ags_capture_wave_audio_class_init(AgsCaptureWaveAudioClass *capture_wave_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_capture_wave_audio_parent_class = g_type_class_peek_parent(capture_wave_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) capture_wave_audio;

  gobject->set_property = ags_capture_wave_audio_set_property;
  gobject->get_property = ags_capture_wave_audio_get_property;

  gobject->finalize = ags_capture_wave_audio_finalize;

  /* properties */
  /**
   * AgsCaptureWaveAudio:playback:
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
   * AgsCaptureWaveAudio:replace:
   * 
   * The replace port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("replace",
				   i18n_pspec("if do replace"),
				   i18n_pspec("If audio data should be replaced"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_REPLACE,
				  param_spec);

  /**
   * AgsCaptureWaveAudio:record:
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
   * AgsCaptureWaveAudio:filename:
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
   * AgsCaptureWaveAudio:file-audio-channels:
   * 
   * The file's audio channels port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("file-audio-channels",
				   i18n_pspec("file audio channels"),
				   i18n_pspec("Audio channels count of file"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE_AUDIO_CHANNELS,
				  param_spec);

  /**
   * AgsCaptureWaveAudio:file-samplerate:
   * 
   * The file's samplerate port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("file-samplerate",
				   i18n_pspec("file samplerate"),
				   i18n_pspec("Samplerate to use of file"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE_SAMPLERATE,
				  param_spec);

  /**
   * AgsCaptureWaveAudio:file-buffer-size:
   * 
   * The file's buffer size port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("file-buffer-size",
				   i18n_pspec("files buffer size"),
				   i18n_pspec("Buffer size to use of file"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsCaptureWaveAudio:format:
   * 
   * The file's format port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("file-format",
				   i18n_pspec("file format"),
				   i18n_pspec("Format to use of file"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILE_FORMAT,
				  param_spec);

  /**
   * AgsCaptureWaveAudio:wave-loop:
   *
   * Count until loop-end and start at loop-start.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("wave-loop",
				   i18n_pspec("wave loop capturing"),
				   i18n_pspec("Capture wave in a endless loop"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_WAVE_LOOP,
				  param_spec);

  /**
   * AgsCaptureWaveAudio:wave-loop-start:
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
   * AgsCaptureWaveAudio:wave-loop-end:
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
}

void
ags_capture_wave_audio_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_capture_wave_audio_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_capture_wave_audio_set_ports;
}

void
ags_capture_wave_audio_init(AgsCaptureWaveAudio *capture_wave_audio)
{
  GList *port;

  guint i;
  
  AGS_RECALL(capture_wave_audio)->name = "ags-capture-wave";
  AGS_RECALL(capture_wave_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(capture_wave_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(capture_wave_audio)->xml_type = "ags-capture-wave-audio";

  port = NULL;
  i = 0;
  
  /* playback */
  capture_wave_audio->playback = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_capture_wave_audio_plugin_name,
					      "specifier", ags_capture_wave_audio_specifier[i],
					      "control-port", ags_capture_wave_audio_control_port[i],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_BOOLEAN,
					      NULL);
  g_object_ref(capture_wave_audio->playback);
  
  capture_wave_audio->playback->port_value.ags_port_boolean = TRUE;

  /* add to port */
  port = g_list_prepend(port, capture_wave_audio->playback);
  g_object_ref(capture_wave_audio->playback);

  i++;
  
  /* replace */
  capture_wave_audio->replace = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_capture_wave_audio_plugin_name,
					      "specifier", ags_capture_wave_audio_specifier[i],
					      "control-port", ags_capture_wave_audio_control_port[i],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_BOOLEAN,
					      NULL);
  g_object_ref(capture_wave_audio->replace);
  
  capture_wave_audio->replace->port_value.ags_port_boolean = TRUE;

  /* add to port */
  port = g_list_prepend(port, capture_wave_audio->replace);
  g_object_ref(capture_wave_audio->replace);

  i++;

  /* record */
  capture_wave_audio->record = g_object_new(AGS_TYPE_PORT,
					    "plugin-name", ags_capture_wave_audio_plugin_name,
					    "specifier", ags_capture_wave_audio_specifier[i],
					    "control-port", ags_capture_wave_audio_control_port[i],
					    "port-value-is-pointer", FALSE,
					    "port-value-type", G_TYPE_BOOLEAN,
					    NULL);
  g_object_ref(capture_wave_audio->record);
  
  capture_wave_audio->record->port_value.ags_port_boolean = FALSE;

  /* add to port */
  port = g_list_prepend(port, capture_wave_audio->record);
  g_object_ref(capture_wave_audio->record);

  i++;

  /* filename */
  capture_wave_audio->filename = g_object_new(AGS_TYPE_PORT,
					      "plugin-name", ags_capture_wave_audio_plugin_name,
					      "specifier", ags_capture_wave_audio_specifier[i],
					      "control-port", ags_capture_wave_audio_control_port[i],
					      "port-value-is-pointer", FALSE,
					      "port-value-type", G_TYPE_POINTER,
					      NULL);
  g_object_ref(capture_wave_audio->filename);

  capture_wave_audio->filename->port_value.ags_port_pointer = NULL;

  /* add to port */
  port = g_list_prepend(port, capture_wave_audio->filename);
  g_object_ref(capture_wave_audio->filename);

  i++;

  /* audio channels */
  capture_wave_audio->file_audio_channels = g_object_new(AGS_TYPE_PORT,
							 "plugin-name", ags_capture_wave_audio_plugin_name,
							 "specifier", ags_capture_wave_audio_specifier[i],
							 "control-port", ags_capture_wave_audio_control_port[i],
							 "port-value-is-pointer", FALSE,
							 "port-value-type", G_TYPE_UINT64,
							 NULL);
  g_object_ref(capture_wave_audio->file_audio_channels);
  
  capture_wave_audio->file_audio_channels->port_value.ags_port_uint = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;

  /* add to port */
  port = g_list_prepend(port, capture_wave_audio->file_audio_channels);
  g_object_ref(capture_wave_audio->file_audio_channels);

  i++;

  /* samplerate */
  capture_wave_audio->file_samplerate = g_object_new(AGS_TYPE_PORT,
						     "plugin-name", ags_capture_wave_audio_plugin_name,
						     "specifier", ags_capture_wave_audio_specifier[i],
						     "control-port", ags_capture_wave_audio_control_port[i],
						     "port-value-is-pointer", FALSE,
						     "port-value-type", G_TYPE_UINT64,
						     NULL);
  g_object_ref(capture_wave_audio->file_samplerate);
  
  capture_wave_audio->file_samplerate->port_value.ags_port_uint = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  /* add to port */
  port = g_list_prepend(port, capture_wave_audio->file_samplerate);
  g_object_ref(capture_wave_audio->file_samplerate);

  i++;

  /* buffer size */
  capture_wave_audio->file_buffer_size = g_object_new(AGS_TYPE_PORT,
						      "plugin-name", ags_capture_wave_audio_plugin_name,
						      "specifier", ags_capture_wave_audio_specifier[i],
						      "control-port", ags_capture_wave_audio_control_port[i],
						      "port-value-is-pointer", FALSE,
						      "port-value-type", G_TYPE_UINT64,
						      NULL);
  g_object_ref(capture_wave_audio->file_buffer_size);
  
  capture_wave_audio->file_buffer_size->port_value.ags_port_uint = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  /* add to port */
  port = g_list_prepend(port, capture_wave_audio->file_buffer_size);
  g_object_ref(capture_wave_audio->file_buffer_size);

  i++;

  /* format */
  capture_wave_audio->file_format = g_object_new(AGS_TYPE_PORT,
						 "plugin-name", ags_capture_wave_audio_plugin_name,
						 "specifier", ags_capture_wave_audio_specifier[i],
						 "control-port", ags_capture_wave_audio_control_port[i],
						 "port-value-is-pointer", FALSE,
						 "port-value-type", G_TYPE_UINT64,
						 NULL);
  g_object_ref(capture_wave_audio->file_format);
  
  capture_wave_audio->file_format->port_value.ags_port_uint = AGS_SOUNDCARD_DEFAULT_FORMAT;

  /* add to port */
  port = g_list_prepend(port, capture_wave_audio->file_format);
  g_object_ref(capture_wave_audio->file_format);

  i++;

  /* wave loop */
  capture_wave_audio->wave_loop = g_object_new(AGS_TYPE_PORT,
					    "plugin-name", ags_capture_wave_audio_plugin_name,
					    "specifier", ags_capture_wave_audio_specifier[i],
					    "control-port", ags_capture_wave_audio_control_port[i],
					    "port-value-is-pointer", FALSE,
					    "port-value-type", G_TYPE_BOOLEAN,
					    "port-value-size", sizeof(gboolean),
					    "port-value-length", 1,
					    NULL);
  g_object_ref(capture_wave_audio->wave_loop);
  
  capture_wave_audio->wave_loop->port_value.ags_port_boolean = FALSE;

  /* add to port */
  port = g_list_prepend(port, capture_wave_audio->wave_loop);
  g_object_ref(capture_wave_audio->wave_loop);
  
  i++;

  /* wave-loop-start  */
  capture_wave_audio->wave_loop_start = g_object_new(AGS_TYPE_PORT,
						  "plugin-name", ags_capture_wave_audio_plugin_name,
						  "specifier", ags_capture_wave_audio_specifier[i],
						  "control-port", ags_capture_wave_audio_control_port[i],
						  "port-value-is-pointer", FALSE,
						  "port-value-type", G_TYPE_UINT64,
						  "port-value-size", sizeof(guint64),
						  "port-value-length", 1,
						  NULL);
  g_object_ref(capture_wave_audio->wave_loop_start);
  
  capture_wave_audio->wave_loop_start->port_value.ags_port_uint = 0.0;

  /* add to port */
  port = g_list_prepend(port, capture_wave_audio->wave_loop_start);
  g_object_ref(capture_wave_audio->wave_loop_start);
  
  i++;

  /* wave-loop-end */
  capture_wave_audio->wave_loop_end = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_capture_wave_audio_plugin_name,
						"specifier", ags_capture_wave_audio_specifier[i],
						"control-port", ags_capture_wave_audio_control_port[i],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_UINT64,
						"port-value-size", sizeof(guint64),
						"port-value-length", 1,
						NULL);
  g_object_ref(capture_wave_audio->wave_loop_end);
  
  capture_wave_audio->wave_loop_end->port_value.ags_port_uint = 64.0;

  /* add to port */
  port = g_list_prepend(port, capture_wave_audio->wave_loop_end);
  g_object_ref(capture_wave_audio->wave_loop_end);
  
  i++;

  /* set port */
  AGS_RECALL(capture_wave_audio)->port = port;

  /* the audio file */
  capture_wave_audio->audio_file_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(capture_wave_audio->audio_file_mutex,
		     NULL);

  capture_wave_audio->audio_file = NULL;
}

void
ags_capture_wave_audio_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsCaptureWaveAudio *capture_wave_audio;

  pthread_mutex_t *recall_mutex;
  
  capture_wave_audio = AGS_CAPTURE_WAVE_AUDIO(gobject);

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

      if(capture_wave_audio->playback == playback){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_wave_audio->playback != NULL){
	g_object_unref(G_OBJECT(capture_wave_audio->playback));
      }
      
      if(playback != NULL){
	g_object_ref(G_OBJECT(playback));
      }
      
      capture_wave_audio->playback = playback;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_REPLACE:
    {
      AgsPort *replace;

      replace = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(capture_wave_audio->replace == replace){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_wave_audio->replace != NULL){
	g_object_unref(G_OBJECT(capture_wave_audio->replace));
      }
      
      if(replace != NULL){
	g_object_ref(G_OBJECT(replace));
      }
      
      capture_wave_audio->replace = replace;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECORD:
    {
      AgsPort *record;

      record = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(capture_wave_audio->record == record){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_wave_audio->record != NULL){
	g_object_unref(G_OBJECT(capture_wave_audio->record));
      }
      
      if(record != NULL){
	g_object_ref(G_OBJECT(record));
      }
      
      capture_wave_audio->record = record;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FILENAME:
    {
      AgsPort *filename;

      filename = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(capture_wave_audio->filename == filename){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_wave_audio->filename != NULL){
	g_object_unref(G_OBJECT(capture_wave_audio->filename));
      }
      
      if(filename != NULL){
	g_object_ref(G_OBJECT(filename));
      }
      
      capture_wave_audio->filename = filename;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FILE_AUDIO_CHANNELS:
    {
      AgsPort *file_audio_channels;

      file_audio_channels = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(capture_wave_audio->file_audio_channels == file_audio_channels){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_wave_audio->file_audio_channels != NULL){
	g_object_unref(G_OBJECT(capture_wave_audio->file_audio_channels));
      }
      
      if(file_audio_channels != NULL){
	g_object_ref(G_OBJECT(file_audio_channels));
      }
      
      capture_wave_audio->file_audio_channels = file_audio_channels;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FILE_SAMPLERATE:
    {
      AgsPort *file_samplerate;

      file_samplerate = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(capture_wave_audio->file_samplerate == file_samplerate){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_wave_audio->file_samplerate != NULL){
	g_object_unref(G_OBJECT(capture_wave_audio->file_samplerate));
      }
      
      if(file_samplerate != NULL){
	g_object_ref(G_OBJECT(file_samplerate));
      }
      
      capture_wave_audio->file_samplerate = file_samplerate;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FILE_BUFFER_SIZE:
    {
      AgsPort *file_buffer_size;

      file_buffer_size = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(capture_wave_audio->file_buffer_size == file_buffer_size){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_wave_audio->file_buffer_size != NULL){
	g_object_unref(G_OBJECT(capture_wave_audio->file_buffer_size));
      }
      
      if(file_buffer_size != NULL){
	g_object_ref(G_OBJECT(file_buffer_size));
      }
      
      capture_wave_audio->file_buffer_size = file_buffer_size;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FILE_FORMAT:
    {
      AgsPort *file_format;

      file_format = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(capture_wave_audio->file_format == file_format){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_wave_audio->file_format != NULL){
	g_object_unref(G_OBJECT(capture_wave_audio->file_format));
      }
      
      if(file_format != NULL){
	g_object_ref(G_OBJECT(file_format));
      }
      
      capture_wave_audio->file_format = file_format;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == capture_wave_audio->wave_loop){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_wave_audio->wave_loop != NULL){
	g_object_unref(G_OBJECT(capture_wave_audio->wave_loop));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      capture_wave_audio->wave_loop = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP_START:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == capture_wave_audio->wave_loop_start){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_wave_audio->wave_loop_start != NULL){
	g_object_unref(G_OBJECT(capture_wave_audio->wave_loop_start));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      capture_wave_audio->wave_loop_start = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP_END:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == capture_wave_audio->wave_loop_end){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_wave_audio->wave_loop_end != NULL){
	g_object_unref(G_OBJECT(capture_wave_audio->wave_loop_end));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      capture_wave_audio->wave_loop_end = port;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_capture_wave_audio_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsCaptureWaveAudio *capture_wave_audio;

  pthread_mutex_t *recall_mutex;
  
  capture_wave_audio = AGS_CAPTURE_WAVE_AUDIO(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_PLAYBACK:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_wave_audio->playback);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_REPLACE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_wave_audio->replace);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_RECORD:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_wave_audio->record);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FILENAME:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_wave_audio->filename);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FILE_AUDIO_CHANNELS:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_wave_audio->file_audio_channels);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FILE_SAMPLERATE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_wave_audio->file_samplerate);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FILE_BUFFER_SIZE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_wave_audio->file_buffer_size);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_FILE_FORMAT:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_wave_audio->file_format);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_wave_audio->wave_loop);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP_START:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_wave_audio->wave_loop_start);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_WAVE_LOOP_END:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_wave_audio->wave_loop_end);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_capture_wave_audio_dispose(GObject *gobject)
{
  AgsCaptureWaveAudio *capture_wave_audio;

  capture_wave_audio = AGS_CAPTURE_WAVE_AUDIO(gobject);

  /* playback */
  if(capture_wave_audio->playback != NULL){
    g_object_unref(capture_wave_audio->playback);

    capture_wave_audio->playback = NULL;
  }

  /* replace */
  if(capture_wave_audio->replace != NULL){
    g_object_unref(capture_wave_audio->replace);

    capture_wave_audio->replace = NULL;
  }

  /* record */
  if(capture_wave_audio->record != NULL){
    g_object_unref(capture_wave_audio->record);

    capture_wave_audio->record = NULL;
  }

  /* filename */
  if(capture_wave_audio->filename != NULL){
    g_object_unref(capture_wave_audio->filename);

    capture_wave_audio->filename = NULL;
  }

  /* audio channels */
  if(capture_wave_audio->file_audio_channels != NULL){
    g_object_unref(capture_wave_audio->file_audio_channels);

    capture_wave_audio->file_audio_channels = NULL;
  }

  /* samplerate */
  if(capture_wave_audio->file_samplerate != NULL){
    g_object_unref(capture_wave_audio->file_samplerate);

    capture_wave_audio->file_samplerate = NULL;
  }

  /* buffer size */
  if(capture_wave_audio->file_buffer_size != NULL){
    g_object_unref(capture_wave_audio->file_buffer_size);

    capture_wave_audio->file_buffer_size = NULL;
  }

  /* format */
  if(capture_wave_audio->file_format != NULL){
    g_object_unref(capture_wave_audio->file_format);

    capture_wave_audio->file_format = NULL;
  }

  /* wave */
  if(capture_wave_audio->wave_loop != NULL){
    g_object_unref(G_OBJECT(capture_wave_audio->wave_loop));

    capture_wave_audio->wave_loop = NULL;
  }

  if(capture_wave_audio->wave_loop_start != NULL){
    g_object_unref(G_OBJECT(capture_wave_audio->wave_loop_start));

    capture_wave_audio->wave_loop_start = NULL;
  }

  if(capture_wave_audio->wave_loop_end != NULL){
    g_object_unref(G_OBJECT(capture_wave_audio->wave_loop_end));

    capture_wave_audio->wave_loop_end = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_capture_wave_audio_parent_class)->dispose(gobject);
}

void
ags_capture_wave_audio_finalize(GObject *gobject)
{
  AgsCaptureWaveAudio *capture_wave_audio;

  capture_wave_audio = AGS_CAPTURE_WAVE_AUDIO(gobject);

  /* playback */
  if(capture_wave_audio->playback != NULL){
    g_object_unref(capture_wave_audio->playback);
  }

  /* replace */
  if(capture_wave_audio->replace != NULL){
    g_object_unref(capture_wave_audio->replace);
  }

  /* record */
  if(capture_wave_audio->record != NULL){
    g_object_unref(capture_wave_audio->record);
  }

  /* filename */
  if(capture_wave_audio->filename != NULL){
    g_object_unref(capture_wave_audio->filename);
  }

  /* audio channels */
  if(capture_wave_audio->file_audio_channels != NULL){
    g_object_unref(capture_wave_audio->file_audio_channels);
  }

  /* samplerate */
  if(capture_wave_audio->file_samplerate != NULL){
    g_object_unref(capture_wave_audio->file_samplerate);
  }

  /* buffer size */
  if(capture_wave_audio->file_buffer_size != NULL){
    g_object_unref(capture_wave_audio->file_buffer_size);
  }

  /* format */
  if(capture_wave_audio->file_format != NULL){
    g_object_unref(capture_wave_audio->file_format);
  }

  /* wave */
  if(capture_wave_audio->wave_loop != NULL){
    g_object_unref(G_OBJECT(capture_wave_audio->wave_loop));
  }

  if(capture_wave_audio->wave_loop_start != NULL){
    g_object_unref(G_OBJECT(capture_wave_audio->wave_loop_start));
  }

  if(capture_wave_audio->wave_loop_end != NULL){
    g_object_unref(G_OBJECT(capture_wave_audio->wave_loop_end));
  }

  pthread_mutex_destroy(capture_wave_audio->audio_file_mutex);
  free(capture_wave_audio->audio_file_mutex);

  if(capture_wave_audio->audio_file != NULL){
    g_object_unref(capture_wave_audio->audio_file);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_capture_wave_audio_parent_class)->finalize(gobject);
}

void
ags_capture_wave_audio_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./playback[0]",
		11)){
      g_object_set(G_OBJECT(plugin),
		   "playback", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		"./replace[0]",
		11)){
      g_object_set(G_OBJECT(plugin),
		   "replace", AGS_PORT(port->data),
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
		      "./file-audio-channels[0]",
		      24)){
      g_object_set(G_OBJECT(plugin),
		   "file-audio-channels", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./file-samplerate[0]",
		      20)){
      g_object_set(G_OBJECT(plugin),
		   "file-samplerate", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./file-buffer-size[0]",
		      21)){
      g_object_set(G_OBJECT(plugin),
		   "file-buffer-size", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		      "./file-format[0]",
		      16)){
      g_object_set(G_OBJECT(plugin),
		   "file-format", AGS_PORT(port->data),
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
    }

    port = port->next;
  }
}

/**
 * ags_capture_wave_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsCaptureWaveAudio
 *
 * Returns: the new #AgsCaptureWaveAudio
 *
 * Since: 2.0.0
 */
AgsCaptureWaveAudio*
ags_capture_wave_audio_new(AgsAudio *audio)
{
  AgsCaptureWaveAudio *capture_wave_audio;

  capture_wave_audio = (AgsCaptureWaveAudio *) g_object_new(AGS_TYPE_CAPTURE_WAVE_AUDIO,
							    "audio", audio,
							    NULL);

  return(capture_wave_audio);
}
