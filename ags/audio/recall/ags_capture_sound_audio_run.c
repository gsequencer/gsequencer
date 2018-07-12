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

#include <ags/audio/recall/ags_capture_sound_audio_run.h>

#include <ags/libags.h>

#include <ags/audio/ags_wave.h>
#include <ags/audio/ags_buffer.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/recall/ags_capture_sound_audio.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/audio/file/ags_audio_file.h>

#include <math.h>

#include <ags/i18n.h>

void ags_capture_sound_audio_run_class_init(AgsCaptureSoundAudioRunClass *capture_sound_audio_run);
void ags_capture_sound_audio_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_capture_sound_audio_run_init(AgsCaptureSoundAudioRun *capture_sound_audio_run);
void ags_capture_sound_audio_run_set_property(GObject *gobject,
					      guint prop_id,
					      const GValue *value,
					      GParamSpec *param_spec);
void ags_capture_sound_audio_run_get_property(GObject *gobject,
					      guint prop_id,
					      GValue *value,
					      GParamSpec *param_spec);
void ags_capture_sound_audio_run_dispose(GObject *gobject);
void ags_capture_sound_audio_run_finalize(GObject *gobject);

void ags_capture_sound_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_capture_sound_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

void ags_capture_sound_audio_run_run_init_pre(AgsRecall *recall);
void ags_capture_sound_audio_run_run_pre(AgsRecall *recall);
void ags_capture_sound_audio_run_done(AgsRecall *recall);

/**
 * SECTION:ags_capture_sound_audio_run
 * @short_description: capture sound
 * @title: AgsCaptureSoundAudioRun
 * @section_id:
 * @include: ags/audio/recall/ags_capture_sound_audio_run.h
 *
 * The #AgsCaptureSoundAudioRun does capture sound.
 */

static gpointer ags_capture_sound_audio_run_parent_class = NULL;
static AgsPluginInterface *ags_capture_sound_audio_run_parent_plugin_interface;

GType
ags_capture_sound_audio_run_get_type()
{
  static GType ags_type_capture_sound_audio_run = 0;

  if(!ags_type_capture_sound_audio_run){
    static const GTypeInfo ags_capture_sound_audio_run_info = {
      sizeof(AgsCaptureSoundAudioRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_capture_sound_audio_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCaptureSoundAudioRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_capture_sound_audio_run_init,
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_capture_sound_audio_run_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_capture_sound_audio_run = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
							      "AgsCaptureSoundAudioRun",
							      &ags_capture_sound_audio_run_info,
							      0);

    g_type_add_interface_static(ags_type_capture_sound_audio_run,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_capture_sound_audio_run);
}

void
ags_capture_sound_audio_run_class_init(AgsCaptureSoundAudioRunClass *capture_sound_audio_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_capture_sound_audio_run_parent_class = g_type_class_peek_parent(capture_sound_audio_run);

  /* GObjectClass */
  gobject = (GObjectClass *) capture_sound_audio_run;

  gobject->set_property = ags_capture_sound_audio_run_set_property;
  gobject->get_property = ags_capture_sound_audio_run_get_property;

  gobject->dispose = ags_capture_sound_audio_run_dispose;
  gobject->finalize = ags_capture_sound_audio_run_finalize;

  /* properties */
  /**
   * AgsCaptureSoundAudioRun:audio-file:
   * 
   * The audio file.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("audio-file",
				   i18n_pspec("audio file"),
				   i18n_pspec("The audio file"),
				   AGS_TYPE_AUDIO_FILE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_FILE,
				  param_spec);

  /**
   * AgsCaptureSoundAudioRun:timestamp:
   * 
   * The timestamp.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("timestamp",
				   i18n_pspec("timestamp"),
				   i18n_pspec("The timestamp"),
				   AGS_TYPE_TIMESTAMP,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_TIMESTAMP,
				  param_spec);

  /* AgsRecallClass */
  recall = (AgsRecallClass *) capture_sound_audio_run;

  recall->run_init_pre = ags_capture_sound_audio_run_run_init_pre;
  recall->run_pre = ags_capture_sound_audio_run_run_pre;
  recall->done = ags_capture_sound_audio_run_done;
}

void
ags_capture_sound_audio_run_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_capture_sound_audio_run_parent_plugin_interface = g_type_interface_peek_parent(plugin);
  
  plugin->read = ags_capture_sound_audio_run_read;
  plugin->write = ags_capture_sound_audio_run_write;
}

void
ags_capture_sound_audio_run_init(AgsCaptureSoundAudioRun *capture_sound_audio_run)
{
  AGS_RECALL(capture_sound_audio_run)->name = "ags-capture-sound";
  AGS_RECALL(capture_sound_audio_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(capture_sound_audio_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(capture_sound_audio_run)->xml_type = "ags-capture-sound-audio-run";
  AGS_RECALL(capture_sound_audio_run)->port = NULL;

  capture_sound_audio_run->file_buffer = NULL;

  capture_sound_audio_run->audio_file = NULL;

  capture_sound_audio_run->timestamp = ags_timestamp_new();

  capture_sound_audio_run->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  capture_sound_audio_run->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  capture_sound_audio_run->timestamp->timer.ags_offset.offset = 0;
}

void
ags_analyse_channel_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  pthread_mutex_t *recall_mutex;

  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO_FILE:
    {
      AgsAudioFile *audio_file;

      audio_file = (AgsAudioFile *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(audio_file == analyse_channel->audio_file){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(analyse_channel->audio_file != NULL){
	g_object_unref(G_OBJECT(analyse_channel->audio_file));
      }
      
      if(audio_file != NULL){
	g_object_ref(G_OBJECT(audio_file));
      }

      analyse_channel->audio_file = audio_file;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_TIMESTAMP:
    {
      AgsTimestamp *timestamp;

      timestamp = (AgsTimestamp *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(timestamp == analyse_channel->timestamp){
	pthread_mutex_unlock(recall_mutex);

	return;
      }

      if(analyse_channel->timestamp != NULL){
	g_object_unref(G_OBJECT(analyse_channel->timestamp));
      }
      
      if(timestamp != NULL){
	g_object_ref(G_OBJECT(timestamp));
      }

      analyse_channel->timestamp = timestamp;

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_analyse_channel_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  pthread_mutex_t *recall_mutex;

  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_AUDIO_FILE:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_sound_audio_run->audio_file);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_TIMESTAMP:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_sound_audio_run->timestamp);

      pthread_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_capture_sound_audio_run_dispose(GObject *gobject)
{
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(gobject);

  /* audio file */
  if(capture_sound_audio_run->audio_file != NULL){
    g_object_unref(capture_sound_audio_run->audio_file);

    capture_sound_audio_run->audio_file = NULL;
  }

  /* timestamp */
  if(capture_sound_audio_run->timestamp != NULL){
    g_object_unref(capture_sound_audio_run->timestamp);

    capture_sound_audio_run->timestamp = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_capture_sound_audio_run_parent_class)->dispose(gobject);
}

void
ags_capture_sound_audio_run_finalize(GObject *gobject)
{
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(gobject);

  /* audio file */
  if(capture_sound_audio_run->audio_file != NULL){
    g_object_unref(capture_sound_audio_run->audio_file);
  }

  /* timestamp */
  if(capture_sound_audio_run->timestamp != NULL){
    g_object_unref(capture_sound_audio_run->timestamp);
  }

  /* file buffer */
  g_free(capture_sound_audio_run->file_buffer);
  
  /* call parent */
  G_OBJECT_CLASS(ags_capture_sound_audio_run_parent_class)->finalize(gobject);
}

void
ags_capture_sound_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin)
{
  /* read parent */
  ags_capture_sound_audio_run_parent_plugin_interface->read(file, node, plugin);

  //TODO:JK: implement me
}

xmlNode*
ags_capture_sound_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin)
{
  xmlNode *node;
  
  /* write parent */
  node = ags_capture_sound_audio_run_parent_plugin_interface->write(file, parent, plugin);

  //TODO:JK: implement me
  
  return(node);
}

void
ags_capture_sound_audio_run_run_init_pre(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsPort *port;
  AgsCaptureSoundAudio *capture_sound_audio;
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  GObject *input_soundcard;
  
  gchar *filename;

  guint file_audio_channels;
  guint file_samplerate;
  guint file_format;
  guint file_buffer_size;
  gboolean do_record;
  
  GValue value = {0,};

  void (*parent_class_run_init_pre)(AgsRecall *recall);
  
  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *port_mutex;
  
  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(recall);

  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  parent_class_run_init_pre = AGS_RECALL_CLASS(ags_capture_sound_audio_run_parent_class)->run_init_pre;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  g_object_get(capture_sound_audio_run,
	       "recall-audio", &capture_sound_audio,
	       NULL);

  g_object_get(capture_sound_audio,
	       "audio", &audio,
	       NULL);

  /* record */
  g_object_get(capture_sound_audio,
	       "record", &port,
	       NULL);
  
  g_value_init(G_TYPE_BOOLEAN,
	       &value);
  ags_port_safe_read(port,
		     &value);

  do_record = g_value_get_boolean(&value);
  g_value_unset(&value);

  filename = NULL;

  if(do_record){
    /* read filename */
    g_object_get(capture_sound_audio,
		 "filename", &port,
		 NULL);

    pthread_mutex_lock(ags_port_class_get_mutex());

    port_mutex = port->obj_mutex;
    
    pthread_mutex_unlock(ags_port_class_get_mutex());

    /* get filename */
    pthread_mutex_lock(port_mutex);

    filename = g_strdup(port->port_value.ags_port_pointer);
    
    pthread_mutex_unlock(port_mutex);

    /* read audio channels */
    g_object_get(capture_sound_audio,
		 "file-audio-channels", &port,
		 NULL);

    g_value_init(G_TYPE_UINT64,
		 &value);
    ags_port_safe_read(capture_sound_audio->uint,
		       &value);

    file_audio_channels = g_value_get_uint(&value);
    g_value_unset(&value);
    
    /* read samplerate */
    g_object_get(capture_sound_audio,
		 "file-samplerate", &port,
		 NULL);

    g_value_init(G_TYPE_UINT64,
		 &value);
    ags_port_safe_read(port,
		       &value);

    file_samplerate = g_value_get_uint(&value);
    g_value_unset(&value);

    /* read buffer size */
    g_object_get(capture_sound_audio,
		 "file-buffer-size", &port,
		 NULL);

    g_value_init(G_TYPE_UINT64,
		 &value);
    ags_port_safe_read(port,
		       &value);

    file_buffer_size = g_value_get_uint(&value);
    g_value_unset(&value);

    /* read format */
    g_object_get(capture_sound_audio,
		 "file-format", &port,
		 NULL);

    g_value_init(G_TYPE_UINT64,
		 &value);
    ags_port_safe_read(port,
		       &value);

    file_format = g_value_get_uint(&value);
    g_value_unset(&value);

    /* file buffer */
    capture_sound_audio_run->file_buffer = ags_stream_alloc(file_audio_channels * file_buffer_size,
							    file_format);
    
    /* instantiate audio file */
    g_object_get(recall,
		 "input-soundcard", &input_soundcard,
		 NULL);
    
    capture_sound_audio_run->audio_file = ags_audio_file_new(filename,
							     input_soundcard,
							     -1);
    g_object_set(capture_sound_audio_run->audio_file,
		 "samplerate", file_samplerate,
		 "buffer-size", file_buffer_size,
		 "format", file_format,
		 NULL);
    
    ags_audio_file_rw_open(capture_sound_audio_run->audio_file,
			   TRUE);

    g_free(filename);
  }
  
  /* call parent */
  parent_class_run_init_pre(recall);
}

void
ags_capture_sound_audio_run_run_pre(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsPort *port;
  AgsWave *wave;
  AgsBuffer *buffer;
  AgsCaptureSoundAudio *capture_sound_audio;
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  AgsTimestamp *timestamp;

  GObject *output_soundcard, *input_soundcard;
  
  GList *list_start, *list;
  
  void *data, *file_data;

  guint note_offset;
  gdouble delay;
  guint delay_counter;
  guint64 x_offset;
  guint target_copy_mode, file_copy_mode;
  guint audio_channels, target_audio_channels, file_audio_channels;
  guint samplerate, target_samplerate, file_samplerate;
  guint format, target_format, file_format;
  guint buffer_size, target_buffer_size, file_buffer_size;
  guint i;
  gboolean do_playback;
  gboolean do_record;
  gboolean resample_target, resample_file;
  
  GValue value = {0,};

  void (*parent_class_run_pre)(AgsRecall *recall);
  
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *buffer_mutex;
  pthread_mutex_t *recall_mutex;
  
  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(recall);

  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  parent_class_run_pre = AGS_RECALL_CLASS(ags_capture_sound_audio_run_parent_class)->run_pre;

  pthread_mutex_unlock(ags_recall_get_class_mutex());
  
  /* get some fields */
  g_object_get(capture_sound_audio_run,
	       "recall-audio", &capture_sound_audio,
	       "timestamp", &timestamp,
	       NULL);

  /* get audio and mutex */
  g_object_get(capture_sound_audio,
	       "audio", &audio,
	       NULL);

  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  /* get soundcard */
  g_object_get(recall,
	       "output-soundcard", &output_soundcard,
	       "input-soundcard", &input_soundcard,
	       NULL);
  
  data = ags_soundcard_get_prev_buffer(AGS_SOUNDCARD(input_soundcard));
  ags_soundcard_get_presets(AGS_SOUNDCARD(input_soundcard),
			    &audio_channels,
			    &samplerate,
			    &buffer_size,
			    &format);

  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));

  delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(output_soundcard));

  x_offset = delay * note_offset + delay_counter;
  
  /* read playback */
  g_object_get(capture_sound_audio,
	       "playback", &port,
	       NULL);
  
  g_value_init(G_TYPE_BOOLEAN,
	       &value);
  ags_port_safe_read(port,
		     &value);

  do_playback = g_value_get_boolean(&value);
  g_value_unset(&value);
  
  if(do_playback){
    /* get target presets */
    pthread_mutex_lock(audio_mutex);

    target_audio_channels = audio->audio_channels;
    target_samplerate = audio->samplerate;
    target_buffer_size = audio->buffer_size;
    target_format = audio->format;

    list_start = g_list_copy(audio->wave);
    
    pthread_mutex_unlock(audio_mutex);

    /* check resample */
    if(target_samplerate != samplerate){
      data = ags_audio_buffer_util_resample(data, audio_channels,
					    format, samplerate,
					    buffer_size,
					    target_samplerate);

      resample_target = TRUE;
    }

    target_copy_mode = ags_audio_buffer_util_get_copy_mode(target_format,
							   format);

    ags_timestamp_set_ags_offset(timestamp,
				 AGS_NOTATION_DEFAULT_OFFSET * floor(x_offset / AGS_NOTATION_DEFAULT_OFFSET));

    for(i = 0; i < target_audio_channels && i < audio_channels; i++){      
      /* play */
      list = ags_wave_find_near_timestamp(list_start, i,
					  timestamp);

      if(list != NULL){
	wave = list->data;
      }else{
	wave = ags_wave_new(audio,
			    i);
	ags_audio_add_wave(audio,
			   wave);
      }

      buffer = ags_wave_find_point(wave,
				   x_offset,
				   FALSE);

      if(buffer == NULL){
	buffer = ags_buffer_new();
	buffer->x = x_offset;
	
	ags_wave_add_buffer(wave,
			    buffer);
      }

      /* get buffer mutex */
      pthread_mutex_lock(ags_buffer_get_class_mutex());

      buffer_mutex = buffer->obj_mutex;
      
      pthread_mutex_unlock(ags_buffer_get_class_mutex());
      
      /* copy to buffer */
      pthread_mutex_lock(buffer_mutex);
      
      ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, target_audio_channels, i,
						  data, audio_channels, i,
						  target_buffer_size);

      pthread_mutex_unlock(buffer_mutex);
    }

    g_list_free(list_start);
    
    if(resample_target){
      g_free(data);
    }

    g_object_unref(timestamp);
  }
  
  /* read record */
  g_object_get(capture_sound_audio,
	       "record", &port,
	       NULL);
  
  g_value_init(G_TYPE_BOOLEAN,
	       &value);
  ags_port_safe_read(port,
		     &value);

  do_record = g_value_get_boolean(&value);
  g_value_unset(&value);
  
  if(do_record){
    AgsAudioFile *audio_file;

    g_object_get(capture_sound_audio_run,
		 "audio-file", &audio_file,
		 NULL);

    /* get presets */
    g_object_get(audio_file,
		 "file-audio-channels", &file_audio_channels,
		 "samplerate", &file_samplerate,
		 "buffer-size", &file_buffer_size,
		 "format", &file_format,
		 NULL);

    ags_audio_buffer_util_clear_buffer(capture_sound_audio_run->file_buffer, 1,
				       file_audio_channels * file_buffer_size, file_format);

    if(file_samplerate != samplerate){
      data = ags_audio_buffer_util_resample(data, audio_channels,
					    format, samplerate,
					    buffer_size,
					    file_samplerate);

      resample_file = TRUE;
    }

    file_copy_mode = ags_audio_buffer_util_get_copy_mode(file_format,
							 format);
    
    for(i = 0; i < file_audio_channels && i < audio_channels; i++){      
      ags_audio_buffer_util_copy_buffer_to_buffer(file_data, file_audio_channels, i,
						  data, audio_channels, i,
						  file_buffer_size);

    }

    if(resample_file){
      g_free(data);
    }
    
    /* file */
    ags_audio_file_write(audio_file,
			 capture_sound_audio_run->file_buffer, file_buffer_size,
			 file_format);
  }
  
  /* call parent */
  parent_class_run_pre(recall);
}

void
ags_capture_sound_audio_run_done(AgsRecall *recall)
{
  AgsPort *port;
  AgsAudioFile *audio_file;
  AgsCaptureSoundAudio *capture_sound_audio;
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  gboolean do_record;

  GValue value = {0,};

  void (*parent_class_done)(AgsRecall *recall);
  
  pthread_mutex_t *recall_mutex;

  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(recall);

  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  parent_class_done = AGS_RECALL_CLASS(ags_capture_sound_audio_run_parent_class)->done;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* get some fields */
  g_object_get(capture_sound_audio_run,
	       "recall-audio", &capture_sound_audio,
	       "audio-file", &audio_file,
	       NULL);
  
  /* read record */
  g_object_get(capture_sound_audio,
	       "record", &port,
	       NULL);
  g_value_init(G_TYPE_BOOLEAN,
	       &value);
  ags_port_safe_read(port,
		     &value);

  do_record = g_value_get_boolean(&value);
  g_value_unset(&value);
  
  if(do_record){
    ags_audio_file_flush(audio_file);
    ags_audio_file_close(audio_file);
  }
  
  /* call parent */
  parent_class_done(recall);
}

/**
 * ags_capture_sound_audio_run_new:
 * @audio: the #AgsAudio
 * 
 * Create a new instance of #AgsCaptureSoundAudioRun
 *
 * Returns: the new #AgsCaptureSoundAudioRun
 *
 * Since: 2.0.0
 */
AgsCaptureSoundAudioRun*
ags_capture_sound_audio_run_new(AgsAudio *audio)
{
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  capture_sound_audio_run = (AgsCaptureSoundAudioRun *) g_object_new(AGS_TYPE_CAPTURE_SOUND_AUDIO_RUN,
								     "audio", audio,
								     NULL);

  return(capture_sound_audio_run);
}
