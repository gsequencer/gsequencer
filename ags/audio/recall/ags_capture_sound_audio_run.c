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
#include <ags/audio/recall/ags_capture_sound_audio.h>

#include <ags/libags.h>

#include <ags/audio/ags_wave.h>
#include <ags/audio/ags_buffer.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <ags/audio/file/ags_audio_file.h>

#include <math.h>

#include <ags/i18n.h>

void ags_capture_sound_audio_run_class_init(AgsCaptureSoundAudioRunClass *capture_sound_audio_run);
void ags_capture_sound_audio_run_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_capture_sound_audio_run_plugin_interface_init(AgsPluginInterface *plugin);
void ags_capture_sound_audio_run_init(AgsCaptureSoundAudioRun *capture_sound_audio_run);
void ags_capture_sound_audio_run_dispose(GObject *gobject);
void ags_capture_sound_audio_run_finalize(GObject *gobject);

void ags_capture_sound_audio_run_read(AgsFile *file, xmlNode *node, AgsPlugin *plugin);
xmlNode* ags_capture_sound_audio_run_write(AgsFile *file, xmlNode *parent, AgsPlugin *plugin);

AgsRecall* ags_capture_sound_audio_run_duplicate(AgsRecall *recall,
						 AgsRecallID *recall_id,
						 guint *n_params, gchar **parameter_name, GValue *value);
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
static AgsConnectableInterface* ags_capture_sound_audio_run_parent_connectable_interface;
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

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_capture_sound_audio_run_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
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
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

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

  gobject->dispose = ags_capture_sound_audio_run_dispose;
  gobject->finalize = ags_capture_sound_audio_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) capture_sound_audio_run;

  recall->duplicate = ags_capture_sound_audio_run_duplicate;
  recall->run_init_pre = ags_capture_sound_audio_run_run_init_pre;
  recall->run_pre = ags_capture_sound_audio_run_run_pre;
  recall->done = ags_capture_sound_audio_run_done;
}

void
ags_capture_sound_audio_run_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_capture_sound_audio_run_parent_connectable_interface = g_type_interface_peek_parent(connectable);
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

  capture_sound_audio_run->audio_file = NULL;

  capture_sound_audio_run->file_buffer = NULL;
}

void
ags_capture_sound_audio_run_dispose(GObject *gobject)
{
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(gobject);

  if(capture_sound_audio_run->audio_file != NULL){
    g_object_unref(capture_sound_audio_run->audio_file);

    capture_sound_audio_run->audio_file = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_capture_sound_audio_run_parent_class)->dispose(gobject);
}

void
ags_capture_sound_audio_run_finalize(GObject *gobject)
{
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(gobject);

  if(capture_sound_audio_run->audio_file != NULL){
    g_object_unref(capture_sound_audio_run->audio_file);
  }
  
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

AgsRecall*
ags_capture_sound_audio_run_duplicate(AgsRecall *recall,
				      AgsRecallID *recall_id,
				      guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsCaptureSoundAudioRun *copy_capture_sound_audio_run;

  copy_capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(AGS_RECALL_CLASS(ags_capture_sound_audio_run_parent_class)->duplicate(recall,
																   recall_id,
																   n_params, parameter_name, value));

  return((AgsRecall *) copy_capture_sound_audio_run);
}

void
ags_capture_sound_audio_run_run_init_pre(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsPort *port;
  
  AgsCaptureSoundAudio *capture_sound_audio;
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  GObject *output_soundcard;
  
  gchar *filename;

  guint file_audio_channels;
  guint file_samplerate;
  guint file_format;
  guint file_buffer_size;
  gboolean do_record;
  
  GValue value = {0,};

  pthread_mutex_t *port_mutex;
  
  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(recall);

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
		 "audio-channels", &port,
		 NULL);

    g_value_init(G_TYPE_UINT64,
		 &value);
    ags_port_safe_read(capture_sound_audio->uint,
		       &value);

    file_audio_channels = g_value_get_uint(&value);
    g_value_unset(&value);

    /* read format */
    g_object_get(capture_sound_audio,
		 "format", &port,
		 NULL);

    g_value_init(G_TYPE_UINT64,
		 &value);
    ags_port_safe_read(port,
		       &value);

    file_format = g_value_get_uint(&value);
    g_value_unset(&value);
    
    /* read samplerate */
    g_object_get(capture_sound_audio,
		 "samplerate", &port,
		 NULL);

    g_value_init(G_TYPE_UINT64,
		 &value);
    ags_port_safe_read(port,
		       &value);

    file_samplerate = g_value_get_uint(&value);
    g_value_unset(&value);

    /* read buffer size */
    g_object_get(capture_sound_audio,
		 "buffer-size", &port,
		 NULL);

    g_value_init(G_TYPE_UINT64,
		 &value);
    ags_port_safe_read(port,
		       &value);

    file_buffer_size = g_value_get_uint(&value);
    g_value_unset(&value);

    /* file buffer */
    capture_sound_audio_run->file_buffer = ags_stream_alloc(file_audio_channels * file_buffer_size,
							    file_format);
    
    /* instantiate audio file */
    g_object_get(recall,
		 "output-soundcard", &output_soundcard,
		 NULL);
    
    capture_sound_audio_run->audio_file = ags_audio_file_new(filename,
							     output_soundcard,
							     0, file_audio_channels);
    g_object_set(capture_sound_audio_run->audio_file,
		 "format", file_format,
		 "samplerate", file_samplerate,
		 NULL);
    
    ags_audio_file_rw_open(capture_sound_audio_run->audio_file,
			   TRUE);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_capture_sound_audio_run_parent_class)->run_init_pre(recall);
}

void
ags_capture_sound_audio_run_run_pre(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsWave *wave;
  AgsBuffer *buffer;
  AgsCaptureSoundAudio *capture_sound_audio;
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  AgsTimestamp *timestamp;

  GList *list;
  
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

  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(recall);
  
  capture_sound_audio = AGS_CAPTURE_SOUND_AUDIO(AGS_RECALL_AUDIO_RUN(capture_sound_audio_run)->recall_audio);

  audio = AGS_RECALL_AUDIO(capture_sound_audio)->audio;
  
  data = ags_soundcard_get_prev_buffer(AGS_SOUNDCARD(recall->soundcard));
  ags_soundcard_get_presets(AGS_SOUNDCARD(recall->soundcard),
			    &audio_channels,
			    &samplerate,
			    &buffer_size,
			    &format);

  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(recall->soundcard));
  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(recall->soundcard));

  delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(recall->soundcard));

  x_offset = delay * note_offset + delay_counter;
  
  /* read playback */
  g_value_init(G_TYPE_BOOLEAN,
	       &value);
  ags_port_safe_read(capture_sound_audio->playback,
		     &value);

  do_playback = g_value_get_boolean(&value);
  g_value_unset(&value);
  
  if(do_playback){
    target_audio_channels = audio->audio_channels;
    target_samplerate = audio->samplerate;
    target_buffer_size = audio->buffer_size;
    target_format = audio->format;

    if(target_samplerate != samplerate){
      data = ags_audio_buffer_util_resample(data, audio_channels,
					    format, samplerate,
					    buffer_size,
					    target_samplerate);

      resample_target = TRUE;
    }

    target_copy_mode = ags_audio_buffer_util_get_copy_mode(target_format,
							 format);
    
    timestamp = ags_timestamp_new();

    timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
    timestamp->flags |= AGS_TIMESTAMP_OFFSET;

    timestamp->timer.ags_offset.offset = AGS_NOTATION_DEFAULT_OFFSET * floor(x_offset / AGS_NOTATION_DEFAULT_OFFSET);

    for(i = 0; i < target_audio_channels && i < audio_channels; i++){      
      /* play */
      list = ags_wave_find_near_timestamp(audio->wave, i,
					  timestamp);

      if(list != NULL){
	wave = list->data;
      }else{
	wave = ags_wave_new(audio,
			    i);
	audio->wave = ags_wave_add(audio->wave,
				   wave);
      }

      buffer = ags_wave_find_point(wave,
				   x_offset,
				   FALSE);

      if(buffer == NULL){
	buffer = ags_buffer_new();
	buffer->x = x_offset;
      }
      
      ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, target_audio_channels, i,
						  data, audio_channels, i,
						  target_buffer_size);

    }

    if(resample_target){
      g_free(data);
    }

    g_object_unref(timestamp);
  }
  
  /* read record */
  g_value_init(G_TYPE_BOOLEAN,
	       &value);
  ags_port_safe_read(capture_sound_audio->record,
		     &value);

  do_record = g_value_get_boolean(&value);
  g_value_unset(&value);
  
  if(do_record){
    /* read audio channels */
    g_value_init(G_TYPE_UINT,
		 &value);
    ags_port_safe_read(capture_sound_audio->audio_channels,
		       &value);

    file_audio_channels = g_value_get_uint(&value);
    g_value_unset(&value);

    /* read format */
    g_value_init(G_TYPE_UINT,
		 &value);
    ags_port_safe_read(capture_sound_audio->format,
		       &value);

    file_format = g_value_get_uint(&value);
    g_value_unset(&value);
    
    /* read samplerate */
    g_value_init(G_TYPE_UINT,
		 &value);
    ags_port_safe_read(capture_sound_audio->samplerate,
		       &value);

    file_samplerate = g_value_get_uint(&value);
    g_value_unset(&value);

    /* read buffer size */
    g_value_init(G_TYPE_UINT,
		 &value);
    ags_port_safe_read(capture_sound_audio->buffer_size,
		       &value);

    file_buffer_size = g_value_get_uint(&value);
    g_value_unset(&value);

    ags_audio_buffer_util_clear_buffer(file_data, 1,
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
    ags_audio_file_write(capture_sound_audio_run->audio_file,
			 file_data, file_buffer_size, file_format);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_capture_sound_audio_run_parent_class)->run_pre(recall);
}

void
ags_capture_sound_audio_run_done(AgsRecall *recall)
{
  AgsCaptureSoundAudio *capture_sound_audio;
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  gboolean do_record;

  GValue value = {0,};

  capture_sound_audio_run = AGS_CAPTURE_SOUND_AUDIO_RUN(recall);
  
  capture_sound_audio = AGS_CAPTURE_SOUND_AUDIO(AGS_RECALL_AUDIO_RUN(capture_sound_audio_run)->recall_audio);

  /* read record */
  g_value_init(G_TYPE_BOOLEAN,
	       &value);
  ags_port_safe_read(capture_sound_audio->record,
		     &value);

  do_record = g_value_get_boolean(&value);
  g_value_unset(&value);
  
  if(do_record){
    ags_audio_file_flush(capture_sound_audio_run->audio_file);
    ags_audio_file_close(capture_sound_audio_run->audio_file);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_capture_sound_audio_run_parent_class)->done(recall);
}

/**
 * ags_capture_sound_audio_run_new:
 *
 * Creates an #AgsCaptureSoundAudioRun
 *
 * Returns: a new #AgsCaptureSoundAudioRun
 *
 * Since: 1.4.0
 */
AgsCaptureSoundAudioRun*
ags_capture_sound_audio_run_new()
{
  AgsCaptureSoundAudioRun *capture_sound_audio_run;

  capture_sound_audio_run = (AgsCaptureSoundAudioRun *) g_object_new(AGS_TYPE_CAPTURE_SOUND_AUDIO_RUN,
								     NULL);

  return(capture_sound_audio_run);
}
