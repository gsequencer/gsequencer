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

#include <ags/audio/recall/ags_capture_wave_channel_run.h>

#include <ags/libags.h>

#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_capture_wave_audio.h>
#include <ags/audio/recall/ags_capture_wave_audio_run.h>
#include <ags/audio/recall/ags_capture_wave_channel.h>

#include <ags/i18n.h>

void ags_capture_wave_channel_run_class_init(AgsCaptureWaveChannelRunClass *capture_wave_channel_run);
void ags_capture_wave_channel_run_init(AgsCaptureWaveChannelRun *capture_wave_channel_run);
void ags_capture_wave_channel_run_dispose(GObject *gobject);
void ags_capture_wave_channel_run_finalize(GObject *gobject);

void ags_capture_wave_channel_run_run_pre(AgsRecall *recall);

/**
 * SECTION:ags_capture_wave_channel_run
 * @short_description: capture wave
 * @title: AgsCaptureWaveChannelRun
 * @section_id:
 * @include: ags/channel/recall/ags_capture_wave_channel_run.h
 *
 * The #AgsCaptureWaveChannelRun class capture wave.
 */

static gpointer ags_capture_wave_channel_run_parent_class = NULL;

GType
ags_capture_wave_channel_run_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_capture_wave_channel_run = 0;

    static const GTypeInfo ags_capture_wave_channel_run_info = {
      sizeof (AgsCaptureWaveChannelRunClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_capture_wave_channel_run_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCaptureWaveChannelRun),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_capture_wave_channel_run_init,
    };

    ags_type_capture_wave_channel_run = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
							       "AgsCaptureWaveChannelRun",
							       &ags_capture_wave_channel_run_info,
							       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_capture_wave_channel_run);
  }

  return g_define_type_id__volatile;
}

void
ags_capture_wave_channel_run_class_init(AgsCaptureWaveChannelRunClass *capture_wave_channel_run)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_capture_wave_channel_run_parent_class = g_type_class_peek_parent(capture_wave_channel_run);

  /* GObjectClass */
  gobject = (GObjectClass *) capture_wave_channel_run;

  gobject->dispose = ags_capture_wave_channel_run_dispose;
  gobject->finalize = ags_capture_wave_channel_run_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) capture_wave_channel_run;

  recall->run_pre = ags_capture_wave_channel_run_run_pre;
}

void
ags_capture_wave_channel_run_init(AgsCaptureWaveChannelRun *capture_wave_channel_run)
{
  ags_recall_set_ability_flags(capture_wave_channel_run, (AGS_SOUND_ABILITY_WAVE));

  AGS_RECALL(capture_wave_channel_run)->name = "ags-capture-wave";
  AGS_RECALL(capture_wave_channel_run)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(capture_wave_channel_run)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(capture_wave_channel_run)->xml_type = "ags-capture-wave-channel-run";
  AGS_RECALL(capture_wave_channel_run)->port = NULL;

  capture_wave_channel_run->timestamp = ags_timestamp_new();
  g_object_ref(capture_wave_channel_run->timestamp);
  
  capture_wave_channel_run->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  capture_wave_channel_run->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  capture_wave_channel_run->timestamp->timer.ags_offset.offset = 0;

  capture_wave_channel_run->audio_signal = NULL;

  capture_wave_channel_run->x_offset = 0;
}

void
ags_capture_wave_channel_run_dispose(GObject *gobject)
{
  AgsCaptureWaveChannelRun *capture_wave_channel_run;

  capture_wave_channel_run = AGS_CAPTURE_WAVE_CHANNEL_RUN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_capture_wave_channel_run_parent_class)->dispose(gobject);
}

void
ags_capture_wave_channel_run_finalize(GObject *gobject)
{
  AgsCaptureWaveChannelRun *capture_wave_channel_run;

  capture_wave_channel_run = AGS_CAPTURE_WAVE_CHANNEL_RUN(gobject);

  /* timestamp */
  if(capture_wave_channel_run->timestamp != NULL){
    g_object_unref(G_OBJECT(capture_wave_channel_run->timestamp));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_capture_wave_channel_run_parent_class)->finalize(gobject);
}

void
ags_capture_wave_channel_run_run_pre(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsChannel *input;
  AgsPort *port;
  AgsWave *wave;
  AgsBuffer *buffer;
  AgsCaptureWaveAudio *capture_wave_audio;
  AgsCaptureWaveAudioRun *capture_wave_audio_run;
  AgsCaptureWaveChannel *capture_wave_channel;
  AgsCaptureWaveChannelRun *capture_wave_channel_run;

  AgsTimestamp *timestamp;

  GObject *output_soundcard, *input_soundcard;
  
  GList *list_start, *list;
  
  void *data, *file_data;

  gint input_soundcard_channel;
  guint line;
  guint64 relative_offset;
  guint note_offset;
  gdouble delay;
  guint delay_counter;
  gboolean do_loop;
  guint64 x_offset;
  guint target_copy_mode, file_copy_mode;
  guint audio_channels, target_audio_channels, file_audio_channels;
  guint samplerate, target_samplerate, file_samplerate;
  guint format, target_format, file_format;
  guint buffer_size, target_buffer_size, file_buffer_size;
  gboolean do_playback, do_replace, is_new_buffer;
  gboolean do_record;
  gboolean resample_target, resample_file;
  
  GValue value = {0,};
  GValue do_loop_value = {0,};
  GValue x_offset_value = {0,};

  void (*parent_class_run_pre)(AgsRecall *recall);
  
  pthread_mutex_t *audio_mutex;
  pthread_mutex_t *buffer_mutex;
  pthread_mutex_t *recall_mutex;
  
  capture_wave_channel_run = AGS_CAPTURE_WAVE_CHANNEL_RUN(recall);

  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  parent_class_run_pre = AGS_RECALL_CLASS(ags_capture_wave_channel_run_parent_class)->run_pre;

  pthread_mutex_unlock(ags_recall_get_class_mutex());
  
  /* get some fields */
  g_object_get(capture_wave_channel_run,
	       "recall-audio", &capture_wave_audio,
	       "recall-audio-run", &capture_wave_audio_run,
	       "recall-channel", &capture_wave_channel,
	       NULL);

  timestamp = capture_wave_channel_run->timestamp;
  
  g_object_get(capture_wave_channel_run,
	       "source", &channel,
	       NULL);

  g_object_get(channel,
	       "line", &line,
	       "input-soundcard-channel", &input_soundcard_channel,
	       NULL);

  if(input_soundcard_channel == -1){
    input_soundcard_channel = line;
  }
  
  /* get audio and mutex */
  g_object_get(capture_wave_audio,
	       "audio", &audio,
	       NULL);
  
  pthread_mutex_lock(ags_audio_get_class_mutex());

  audio_mutex = audio->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_get_class_mutex());

  g_object_get(audio,
	       "input", &input,
	       NULL);
  
  /* get soundcard */
  input = ags_channel_nth(input,
			  line);
  
  g_object_get(input,
	       "output-soundcard", &output_soundcard,
	       "input-soundcard", &input_soundcard,
	       NULL);
  
  if(input_soundcard == NULL){
    return;
  }

  data = ags_soundcard_get_prev_buffer(AGS_SOUNDCARD(input_soundcard));
  ags_soundcard_get_presets(AGS_SOUNDCARD(input_soundcard),
			    &audio_channels,
			    &samplerate,
			    &buffer_size,
			    &format);

  note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
  delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));

  delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(output_soundcard));

  x_offset = capture_wave_channel_run->x_offset;
  
  /* read playback */
  g_object_get(capture_wave_audio,
	       "playback", &port,
	       NULL);
  
  g_value_init(&value,
	       G_TYPE_BOOLEAN);
  ags_port_safe_read(port,
		     &value);

  do_playback = g_value_get_boolean(&value);
  g_value_unset(&value);
  
  if(do_playback){
    /* read replace */
    g_object_get(capture_wave_audio,
		 "replace", &port,
		 NULL);
  
    g_value_init(&value,
		 G_TYPE_BOOLEAN);
    ags_port_safe_read(port,
		       &value);

    do_replace = g_value_get_boolean(&value);
    g_value_unset(&value);
    
    /* get target presets */
    pthread_mutex_lock(audio_mutex);

    target_audio_channels = audio->audio_channels;
    target_samplerate = audio->samplerate;
    target_buffer_size = audio->buffer_size;
    target_format = audio->format;

    list_start = g_list_copy(audio->wave);
    
    pthread_mutex_unlock(audio_mutex);

    relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * target_samplerate;

    /* check resample */
    resample_target = FALSE;
    
    if(target_samplerate != samplerate){
      data = ags_audio_buffer_util_resample(data, audio_channels,
					    format, samplerate,
					    buffer_size,
					    target_samplerate);

      resample_target = TRUE;
    }

    target_copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(target_format),
							   ags_audio_buffer_util_format_from_soundcard(format));

    ags_timestamp_set_ags_offset(timestamp,
				 relative_offset * floor(x_offset / relative_offset));

    /* play */
    list = ags_wave_find_near_timestamp(list_start, line,
					timestamp);

    if(list != NULL){
      wave = list->data;
    }else{
      wave = ags_wave_new(audio,
			  line);
      ags_audio_add_wave(audio,
			 wave);
    }

    buffer = ags_wave_find_point(wave,
				 target_buffer_size * floor(x_offset / target_buffer_size),
				 FALSE);

    is_new_buffer = FALSE;
    
    if(buffer == NULL){
      buffer = ags_buffer_new();
      buffer->x = target_buffer_size * floor(x_offset / target_buffer_size);
	
      ags_wave_add_buffer(wave,
			  buffer,
			  FALSE);

      is_new_buffer = TRUE;
    }

    /* get buffer mutex */
    pthread_mutex_lock(ags_buffer_get_class_mutex());

    buffer_mutex = buffer->obj_mutex;
      
    pthread_mutex_unlock(ags_buffer_get_class_mutex());
    
    if(!is_new_buffer &&
       do_replace){
      ags_audio_buffer_util_clear_buffer(buffer->data, 1,
					 target_buffer_size, ags_audio_buffer_util_format_from_soundcard(target_format));
    }
      
    /* copy to buffer */
    pthread_mutex_lock(buffer_mutex);
    ags_soundcard_lock_buffer(AGS_SOUNDCARD(input_soundcard), data);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, 1, 0,
						data, audio_channels, input_soundcard_channel,
						target_buffer_size, target_copy_mode);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(input_soundcard), data);
    pthread_mutex_unlock(buffer_mutex);

    g_list_free(list_start);
    
    if(resample_target){
      g_free(data);
    }
  }
  
  /* read record */
  g_object_get(capture_wave_audio,
	       "record", &port,
	       NULL);
  
  g_value_init(&value,
	       G_TYPE_BOOLEAN);
  ags_port_safe_read(port,
		     &value);

  do_record = g_value_get_boolean(&value);
  g_value_unset(&value);
  
  if(do_record){
    AgsAudioFile *audio_file;

    pthread_mutex_lock(capture_wave_audio->audio_file_mutex);

    audio_file = capture_wave_audio->audio_file;
    
    pthread_mutex_unlock(capture_wave_audio->audio_file_mutex);
    
    /* get presets */
    g_object_get(audio_file,
		 "file-audio-channels", &file_audio_channels,
		 "samplerate", &file_samplerate,
		 "buffer-size", &file_buffer_size,
		 "format", &file_format,
		 NULL);

    ags_audio_buffer_util_clear_buffer(capture_wave_audio_run->file_buffer, 1,
				       file_audio_channels * file_buffer_size, file_format);

    if(file_samplerate != samplerate){
      data = ags_audio_buffer_util_resample(data, audio_channels,
					    format, samplerate,
					    buffer_size,
					    file_samplerate);

      resample_file = TRUE;
    }

    file_copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(file_format),
							 ags_audio_buffer_util_format_from_soundcard(format));
    
    ags_soundcard_lock_buffer(AGS_SOUNDCARD(input_soundcard), data);

    ags_audio_buffer_util_copy_buffer_to_buffer(file_data, file_audio_channels, line,
						data, audio_channels, input_soundcard_channel,
						file_buffer_size, file_copy_mode);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(input_soundcard), data);

    if(resample_file){
      g_free(data);
    }
    
    /* file */
    pthread_mutex_lock(capture_wave_audio->audio_file_mutex);

    ags_audio_file_write(audio_file,
			 capture_wave_audio_run->file_buffer, file_buffer_size,
			 file_format);

    pthread_mutex_unlock(capture_wave_audio->audio_file_mutex);
  }

  /* check loop */
  x_offset += buffer_size;
  
  g_object_get(capture_wave_audio,
	       "wave-loop", &port,
	       NULL);

  g_value_init(&do_loop_value,
	       G_TYPE_BOOLEAN);

  ags_port_safe_read(port,
		     &do_loop_value);

  do_loop = g_value_get_boolean(&do_loop_value);

  if(do_loop){
    guint64 loop_start, loop_end;
    
    GValue loop_start_value = {0,};
    GValue loop_end_value = {0,};
    
    g_object_get(capture_wave_audio,
		 "wave-loop-end", &port,
		 NULL);

    g_value_init(&loop_end_value,
		 G_TYPE_UINT64);

    ags_port_safe_read(port,
		       &loop_end_value);

    loop_end = g_value_get_uint64(&loop_end_value);
    
    if(x_offset / buffer_size / delay >= loop_end){
      g_object_get(capture_wave_audio,
		   "wave-loop-start", &port,
		   NULL);

      g_value_init(&loop_start_value,
		   G_TYPE_UINT64);

      ags_port_safe_read(port,
			 &loop_start_value);
      
      loop_start = g_value_get_uint64(&loop_start_value);

      x_offset = delay * buffer_size * loop_start;
    }
  }
  
  /* new x offset */
  capture_wave_channel_run->x_offset = x_offset;

  g_object_get(capture_wave_channel,
	       "x-offset", &port,
	       NULL);

  g_value_init(&x_offset_value,
	       G_TYPE_UINT64);

  g_value_set_uint64(&x_offset_value,
		     x_offset);

  ags_port_safe_write(port,
		      &x_offset_value);
  
  /* call parent */
  parent_class_run_pre(recall);
}

/**
 * ags_capture_wave_channel_run_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsCaptureWaveChannelRun
 *
 * Returns: the new #AgsCaptureWaveChannelRun
 *
 * Since: 2.0.0
 */
AgsCaptureWaveChannelRun*
ags_capture_wave_channel_run_new(AgsChannel *source)
{
  AgsCaptureWaveChannelRun *capture_wave_channel_run;

  capture_wave_channel_run = (AgsCaptureWaveChannelRun *) g_object_new(AGS_TYPE_CAPTURE_WAVE_CHANNEL_RUN,
								       "source", source,
								       NULL);

  return(capture_wave_channel_run);
}
