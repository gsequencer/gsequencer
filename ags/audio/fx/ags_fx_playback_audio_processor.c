/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_playback_audio_processor.h>

#include <ags/audio/ags_wave.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/fx/ags_fx_playback_audio.h>

#include <ags/i18n.h>

void ags_fx_playback_audio_processor_class_init(AgsFxPlaybackAudioProcessorClass *fx_playback_audio_processor);
void ags_fx_playback_audio_processor_init(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);
void ags_fx_playback_audio_processor_dispose(GObject *gobject);
void ags_fx_playback_audio_processor_finalize(GObject *gobject);

void ags_fx_playback_audio_processor_real_run_inter(AgsRecall *recall);

void ags_fx_playback_audio_processor_real_data_put(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
						   AgsBuffer *buffer,
						   guint data_mode);
void ags_fx_playback_audio_processor_real_data_get(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
						   AgsBuffer *buffer,
						   guint data_mode);

void ags_fx_playback_audio_processor_real_play(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);
void ags_fx_playback_audio_processor_real_record(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);
void ags_fx_playback_audio_processor_real_feed(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);
void ags_fx_playback_audio_processor_real_master(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);

void ags_fx_playback_audio_processor_real_counter_change(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);

/**
 * SECTION:ags_fx_playback_audio_processor
 * @short_description: fx playback audio processor
 * @title: AgsFxPlaybackAudioProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_playback_audio_processor.h
 *
 * The #AgsFxPlaybackAudioProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_playback_audio_processor_parent_class = NULL;

GType
ags_fx_playback_audio_processor_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_playback_audio_processor = 0;

    static const GTypeInfo ags_fx_playback_audio_processor_info = {
      sizeof (AgsFxPlaybackAudioProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_playback_audio_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_processor */
      sizeof (AgsFxPlaybackAudioProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_playback_audio_processor_init,
    };

    ags_type_fx_playback_audio_processor = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
								  "AgsFxPlaybackAudioProcessor",
								  &ags_fx_playback_audio_processor_info,
								  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_playback_audio_processor);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_playback_audio_processor_class_init(AgsFxPlaybackAudioProcessorClass *fx_playback_audio_processor)
{
  GObjectClass *gobject;  
  AgsRecallClass *recall;
  
  ags_fx_playback_audio_processor_parent_class = g_type_class_peek_parent(fx_playback_audio_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_playback_audio_processor;

  gobject->dispose = ags_fx_playback_audio_processor_dispose;
  gobject->finalize = ags_fx_playback_audio_processor_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_playback_audio_processor;
  
  recall->run_inter = ags_fx_playback_audio_processor_real_run_inter;

  /* AgsFxPlaybackAudioProcessorClass */
  fx_playback_audio_processor->data_put = ags_fx_playback_audio_processor_real_data_put;
  fx_playback_audio_processor->data_get = ags_fx_playback_audio_processor_real_data_get;

  fx_playback_audio_processor->play = ags_fx_playback_audio_processor_real_play;
  fx_playback_audio_processor->record = ags_fx_playback_audio_processor_real_record;
  fx_playback_audio_processor->feed = ags_fx_playback_audio_processor_real_feed;
  fx_playback_audio_processor->master = ags_fx_playback_audio_processor_real_master;

  fx_playback_audio_processor->counter_change = ags_fx_playback_audio_processor_real_counter_change;
}

void
ags_fx_playback_audio_processor_init(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  AGS_RECALL(fx_playback_audio_processor)->name = "ags-fx-playback";
  AGS_RECALL(fx_playback_audio_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_playback_audio_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_playback_audio_processor)->xml_type = "ags-fx-playback-audio-processor";

  /* counter */
  fx_playback_audio_processor->delay_counter = 0.0;
  fx_playback_audio_processor->offset_counter = 0;

  fx_playback_audio_processor->current_delay_counter = 0.0;
  fx_playback_audio_processor->current_offset_counter = 0;

  fx_playback_audio_processor->x_offset = 0;
  fx_playback_audio_processor->current_x_offset = 0;

  /* timestamp */
  fx_playback_audio_processor->timestamp = ags_timestamp_new();
  g_object_ref(fx_playback_audio_processor->timestamp);
  
  fx_playback_audio_processor->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  fx_playback_audio_processor->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  fx_playback_audio_processor->timestamp->timer.ags_offset.offset = 0;

  /* playing */
  fx_playback_audio_processor->playing_buffer = NULL;
  fx_playback_audio_processor->playing_audio_signal = NULL;

  /* recording */
  fx_playback_audio_processor->recording_buffer = NULL;
  fx_playback_audio_processor->recording_audio_signal = NULL;

  /* feeding */
  fx_playback_audio_processor->feeding_audio_signal = NULL;

  /* mastering */
  fx_playback_audio_processor->mastering_audio_signal = NULL;

  /* capture */
  fx_playback_audio_processor->capture_audio_signal = NULL;
}

void
ags_fx_playback_audio_processor_dispose(GObject *gobject)
{
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;
  
  fx_playback_audio_processor = AGS_FX_PLAYBACK_AUDIO_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_playback_audio_processor_parent_class)->dispose(gobject);
}

void
ags_fx_playback_audio_processor_finalize(GObject *gobject)
{
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;
  
  fx_playback_audio_processor = AGS_FX_PLAYBACK_AUDIO_PROCESSOR(gobject);  

  /* timestamp */
  if(fx_playback_audio_processor->timestamp != NULL){
    g_object_unref((GObject *) fx_playback_audio_processor->timestamp);
  }

  /* playing - buffer and audio signal */
  if(fx_playback_audio_processor->playing_buffer != NULL){
    g_list_free_full(fx_playback_audio_processor->playing_buffer,
		     (GDestroyNotify) g_object_unref);
  }

  if(fx_playback_audio_processor->playing_audio_signal != NULL){
    g_list_free_full(fx_playback_audio_processor->playing_audio_signal,
		     (GDestroyNotify) g_object_unref);
  }

  /* recording - buffer and audio signal */
  if(fx_playback_audio_processor->recording_buffer != NULL){
    g_list_free_full(fx_playback_audio_processor->recording_buffer,
		     (GDestroyNotify) g_object_unref);
  }

  if(fx_playback_audio_processor->recording_audio_signal != NULL){
    g_list_free_full(fx_playback_audio_processor->recording_audio_signal,
		     (GDestroyNotify) g_object_unref);
  }

  /* feeding - audio signal */
  if(fx_playback_audio_processor->feeding_audio_signal != NULL){
    g_list_free_full(fx_playback_audio_processor->feeding_audio_signal,
		     (GDestroyNotify) g_object_unref);
  }

  /* mastering - audio signal */
  if(fx_playback_audio_processor->mastering_audio_signal != NULL){
    g_list_free_full(fx_playback_audio_processor->mastering_audio_signal,
		     (GDestroyNotify) g_object_unref);
  }

  /* capture - audio signal */
  if(fx_playback_audio_processor->capture_audio_signal != NULL){
    g_list_free_full(fx_playback_audio_processor->capture_audio_signal,
		     (GDestroyNotify) g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_playback_audio_processor_parent_class)->finalize(gobject);
}

void
ags_fx_playback_audio_processor_real_run_inter(AgsRecall *recall)
{
  AgsRecallID *recall_id;
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;

  gdouble delay_counter;
  
  GRecMutex *fx_playback_audio_processor_mutex;

  fx_playback_audio_processor = AGS_FX_PLAYBACK_AUDIO_PROCESSOR(recall);
  
  fx_playback_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio_processor);

  g_object_get(recall,
	       "recall-id", &recall_id,
	       NULL);
  
  /* get delay counter */
  g_rec_mutex_lock(fx_playback_audio_processor_mutex);
    
  fx_playback_audio_processor->current_delay_counter = fx_playback_audio_processor->delay_counter;
  fx_playback_audio_processor->current_offset_counter = fx_playback_audio_processor->offset_counter;
  
  fx_playback_audio_processor->current_x_offset = fx_playback_audio_processor->x_offset;

  delay_counter = fx_playback_audio_processor->delay_counter;

  g_rec_mutex_unlock(fx_playback_audio_processor_mutex);

  /* run */
  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_WAVE)){
    if(delay_counter == 0.0){
      ags_fx_playback_audio_processor_play(fx_playback_audio_processor);
    }
  }

  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_WAVE)){
    ags_fx_playback_audio_processor_record(fx_playback_audio_processor);
  }
  
  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_PLAYBACK) ||
     ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_MIDI)){
    ags_fx_playback_audio_processor_feed(fx_playback_audio_processor);
  }

  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_SEQUENCER) ||
     ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_NOTATION) ||
     ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_MIDI)){
    ags_fx_playback_audio_processor_master(fx_playback_audio_processor);
  }
  
  /* counter change */
  ags_fx_playback_audio_processor_counter_change(fx_playback_audio_processor);

  if(recall_id != NULL){
    g_object_unref(recall_id);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_playback_audio_processor_parent_class)->run_inter(recall);
}

void
ags_fx_playback_audio_processor_real_data_put(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
					      AgsBuffer *buffer,
					      guint data_mode)
{
  AgsAudioSignal *current_audio_signal;
  AgsRecallID *recall_id;
  AgsFxPlaybackAudio *fx_playback_audio;
  
  GList *start_audio_signal, *audio_signal;

  gpointer buffer_data;
  
  guint buffer_samplerate, samplerate;
  guint buffer_buffer_size, buffer_size;
  guint buffer_format, format;
  guint copy_mode;
  gboolean do_resample;
  
  GRecMutex *fx_playback_audio_processor_mutex;
  GRecMutex *buffer_mutex;
  GRecMutex *stream_mutex;

  fx_playback_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio_processor);

  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  g_object_get(fx_playback_audio_processor,
	       "recall-id", &recall_id,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);

  buffer_samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  buffer_format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  g_object_get(buffer,
	       "samplerate", &buffer_samplerate,
	       "buffer-size", &buffer_buffer_size,
	       "format", &buffer_format,
	       NULL);

  /* get audio signal */
  current_audio_signal = NULL;
  
  start_audio_signal = NULL;
  
  g_rec_mutex_lock(fx_playback_audio_processor_mutex);

  if(data_mode == AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_PLAY){
    start_audio_signal = fx_playback_audio_processor->playing_audio_signal;
  }else if(data_mode == AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_RECORD){
    start_audio_signal = fx_playback_audio_processor->recording_audio_signal;
  }
  
  audio_signal = ags_audio_signal_find_by_recall_id(start_audio_signal, (GObject *) recall_id);
  
  if(audio_signal != NULL){
    current_audio_signal = audio_signal->data;
    g_object_ref(current_audio_signal);
  }
  
  g_rec_mutex_unlock(fx_playback_audio_processor_mutex);

  /* create audio signal */
  if(current_audio_signal == NULL){  
    AgsAudio *audio;
    AgsChannel *start_input, *input;
    AgsRecycling *recycling;
    AgsPort *port;
      
    GObject *output_soundcard;
    
    guint audio_channel;
    gdouble delay;

    GValue value = {0,};
    
    output_soundcard = NULL;

    audio = NULL;
  
    recall_id = NULL;

    fx_playback_audio = NULL;

    audio_channel = 0;
  
    g_object_get(fx_playback_audio_processor,
		 "output-soundcard", &output_soundcard,
		 "audio", &audio,
		 "recall-id", &recall_id,
		 "recall-audio", &fx_playback_audio,
		 "audio-channel", &audio_channel,
		 NULL);

    start_input = NULL;
    
    g_object_get(audio,
		 "input", &start_input,
		 NULL);

    input = ags_channel_nth(start_input, audio_channel);

    recycling = NULL;

    if(input != NULL){
      g_object_get(input,
		   "first-recycling", &recycling,
		   NULL);
    }
    
    /* get delay */
    delay = AGS_SOUNDCARD_DEFAULT_DELAY;
  
    if(fx_playback_audio != NULL){
      port = NULL;
    
      g_object_get(fx_playback_audio,
		   "delay", &port,
		   NULL);

      if(port != NULL){
	g_value_init(&value,
		     G_TYPE_DOUBLE);
    
	ags_port_safe_read(port,
			   &value);

	delay = g_value_get_double(&value);
	g_value_unset(&value);

	g_object_unref(port);
      }
    }

    current_audio_signal = ags_audio_signal_new(output_soundcard,
						(GObject *) recycling,
						(GObject *) recall_id);
    g_object_ref(current_audio_signal);
    
    g_object_set(current_audio_signal,
		 "samplerate", samplerate,
		 "buffer-size", buffer_size,
		 "format", format,
		 NULL);

    ags_audio_signal_stream_resize(current_audio_signal,
				   (guint) floor(delay) + 1);      
	    
    g_rec_mutex_lock(fx_playback_audio_processor_mutex);

    if(data_mode == AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_PLAY){
      fx_playback_audio_processor->playing_audio_signal = g_list_prepend(fx_playback_audio_processor->playing_audio_signal,
									 current_audio_signal);
    }else if(data_mode == AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_RECORD){
      fx_playback_audio_processor->recording_audio_signal = g_list_prepend(fx_playback_audio_processor->recording_audio_signal,
									   current_audio_signal);
    }
    
    g_rec_mutex_unlock(fx_playback_audio_processor_mutex);
    
    ags_connectable_connect(AGS_CONNECTABLE(current_audio_signal));
    ags_recycling_add_audio_signal(recycling,
				   current_audio_signal);

    if(output_soundcard != NULL){
      g_object_unref(output_soundcard);
    }

    if(start_input != NULL){
      g_object_unref(start_input);
    }

    if(input != NULL){
      g_object_unref(input);
    }

    if(recycling != NULL){
      g_object_unref(recycling);
    }

    if(fx_playback_audio != NULL){
      g_object_unref(fx_playback_audio);
    }
  }

  buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);
  stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(current_audio_signal);
  
  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(format),
						  ags_audio_buffer_util_format_from_soundcard(buffer_format));

  do_resample = FALSE;
  
  if(samplerate != buffer_samplerate){
    do_resample = TRUE;

    buffer_data = ags_stream_alloc(buffer_size,
				   buffer_format);

    g_rec_mutex_lock(buffer_mutex);

    ags_audio_buffer_util_resample_with_buffer(buffer->data, 1,
					       buffer_format, buffer_samplerate,
					       buffer_buffer_size,
					       samplerate,
					       buffer_size,
					       buffer_data);
    
    g_rec_mutex_unlock(buffer_mutex);
    
    g_rec_mutex_lock(stream_mutex);

    ags_audio_buffer_util_copy_buffer_to_buffer(current_audio_signal->stream_current->data, 1, 0,
						buffer_data, 1, 0,
						buffer_size, copy_mode);
    
    g_rec_mutex_unlock(stream_mutex);

    ags_stream_free(buffer_data);
  }else{
    g_rec_mutex_lock(buffer_mutex);
    g_rec_mutex_lock(stream_mutex);
    
    ags_audio_buffer_util_copy_buffer_to_buffer(current_audio_signal->stream_current->data, 1, 0,
						buffer->data, 1, 0,
						buffer_size, copy_mode);

    g_rec_mutex_unlock(stream_mutex);
    g_rec_mutex_unlock(buffer_mutex);
  }
  
  /* unref */
  if(current_audio_signal != NULL){
    g_object_unref(current_audio_signal);
  }
  
  if(recall_id != NULL){
    g_object_unref(recall_id);
  }  
}

void
ags_fx_playback_audio_processor_data_put(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
					 AgsBuffer *buffer,
					 guint data_mode)
{
  g_return_if_fail(AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(fx_playback_audio_processor));

  g_object_ref(fx_playback_audio_processor);

  if(AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->data_put != NULL){
    AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->data_put(fx_playback_audio_processor,
										     buffer,
										     data_mode);
  }
  
  g_object_unref(fx_playback_audio_processor);
}

void
ags_fx_playback_audio_processor_real_data_get(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
					      AgsBuffer *buffer,
					      guint data_mode)
{
  //TODO:JK: implement me
}

void
ags_fx_playback_audio_processor_data_get(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor,
					 AgsBuffer *buffer,
					 guint data_mode)
{
  g_return_if_fail(AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(fx_playback_audio_processor));

  g_object_ref(fx_playback_audio_processor);

  if(AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->data_get != NULL){
    AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->data_get(fx_playback_audio_processor,
										     buffer,
										     data_mode);
  }
  
  g_object_unref(fx_playback_audio_processor);
}

void
ags_fx_playback_audio_processor_real_play(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  AgsAudio *audio;

  AgsTimestamp *timestamp;
  
  GList *start_wave, *wave;

  guint audio_channel;
  guint64 relative_offset;
  guint64 current_x_offset;
  guint attack;
  guint samplerate;
  guint buffer_size;
  guint frame_count;
  
  GRecMutex *fx_playback_audio_processor_mutex;

  fx_playback_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio_processor);

  audio = NULL;
  
  g_object_get(fx_playback_audio_processor,
	       "audio", &audio,
	       "audio-channel", &audio_channel,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       NULL);

  g_rec_mutex_lock(fx_playback_audio_processor_mutex);

  timestamp = fx_playback_audio_processor->timestamp;
  
  current_x_offset = fx_playback_audio_processor->current_x_offset;
  
  g_rec_mutex_unlock(fx_playback_audio_processor_mutex);

  /* time stamp offset */
  relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;

  ags_timestamp_set_ags_offset(timestamp,
			       (guint64) (relative_offset * floor((double) current_x_offset / (double) relative_offset)));

  attack = (current_x_offset % relative_offset) % buffer_size;

  frame_count = buffer_size - attack;

  if(current_x_offset + frame_count > relative_offset * floor(current_x_offset / relative_offset) + relative_offset){
    frame_count = relative_offset * floor((current_x_offset + frame_count) / relative_offset) - current_x_offset;
  }
  
  /* find wave - attempt #0 */
  start_wave = NULL;
  
  g_object_get(audio,
	       "wave", &start_wave,
	       NULL);
  
  wave = ags_wave_find_near_timestamp(start_wave, audio_channel,
				      timestamp);

  if(wave != NULL){
    AgsBuffer *buffer;

    buffer = ags_wave_find_point(wave->data,
				 current_x_offset,
				 FALSE);

    if(buffer != NULL){
      ags_fx_playback_audio_processor_data_put(fx_playback_audio_processor,
					       buffer,
					       AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_PLAY);
    }
  }

  /* find wave - attempt #1 */
  if(attack != 0 ||
     frame_count != buffer_size){
    ags_timestamp_set_ags_offset(timestamp,
				 (guint64) (relative_offset * floor((double) (current_x_offset + frame_count) / (double) relative_offset)));

    wave = ags_wave_find_near_timestamp(start_wave, audio_channel,
					timestamp);

    if(wave != NULL){
      AgsBuffer *buffer;

      buffer = ags_wave_find_point(wave->data,
				   current_x_offset + frame_count,
				   FALSE);

      if(buffer != NULL){
	ags_fx_playback_audio_processor_data_put(fx_playback_audio_processor,
						 buffer,
						 AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_PLAY);
      }
    }
  }

  /* unref */
  if(audio != NULL){
    g_object_unref(audio);
  }

  g_list_free_full(start_wave,
		   (GDestroyNotify) g_object_unref);
}

void
ags_fx_playback_audio_processor_play(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(fx_playback_audio_processor));

  g_object_ref(fx_playback_audio_processor);

  if(AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->play != NULL){
    AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->play(fx_playback_audio_processor);
  }
  
  g_object_unref(fx_playback_audio_processor);
}

void
ags_fx_playback_audio_processor_real_record(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  AgsAudio *audio;
  AgsChannel *start_input, *input;
  AgsPort *port;
  AgsWave *current_wave;    
  AgsBuffer *buffer;
  AgsFxPlaybackAudio *fx_playback_audio;
  AgsRecallID *recall_id;
  
  AgsTimestamp *timestamp;
  
  GObject *output_soundcard, *input_soundcard;

  GList *start_wave, *wave;

  gpointer data, file_data;

  guint audio_channels;
  guint audio_channel;  
  gint input_soundcard_channel;
  guint64 relative_offset;
  guint64 current_x_offset;
  guint attack;
  guint samplerate, target_samplerate, file_samplerate;
  guint buffer_size, target_buffer_size, file_buffer_size;
  guint format, target_format, file_format;
  guint frame_count;
  guint target_copy_mode, file_copy_mode;
  guint capture_mode;
  gboolean found_buffer;
  
  GValue value = {0,};
  
  GRecMutex *fx_playback_audio_processor_mutex;
  GRecMutex *buffer_mutex;

  fx_playback_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio_processor);

  audio = NULL;

  output_soundcard = NULL;
  input_soundcard = NULL;

  fx_playback_audio = NULL;

  recall_id = NULL;
  
  audio_channel = 0;
  
  g_object_get(fx_playback_audio_processor,
	       "audio", &audio,
	       "output-soundcard", &output_soundcard,
	       "input-soundcard", &input_soundcard,
	       "recall-audio", &fx_playback_audio,
	       "audio-channel", &audio_channel,
	       "recall-id", &recall_id,
	       NULL);

  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  
  ags_soundcard_get_presets(AGS_SOUNDCARD(input_soundcard),
			    &audio_channels,
			    &samplerate,
			    &buffer_size,
			    &format);

  start_wave = NULL;

  start_input = NULL;
  
  g_object_get(audio,
	       "wave", &start_wave,
	       "input", &start_input,
	       "samplerate", &target_samplerate,
	       "buffer-size", &target_buffer_size,
	       "format", &target_format,
	       NULL);
  
  input = ags_channel_nth(start_input,
			  audio_channel);
  
  input_soundcard_channel = -1;
  
  g_object_get(input,
	       "input-soundcard-channel", &input_soundcard_channel,
	       NULL);

  g_rec_mutex_lock(fx_playback_audio_processor_mutex);

  timestamp = fx_playback_audio_processor->timestamp;
  
  current_x_offset = fx_playback_audio_processor->current_x_offset;
  
  g_rec_mutex_unlock(fx_playback_audio_processor_mutex);

  /* time stamp offset */
  relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * target_samplerate;

  ags_timestamp_set_ags_offset(timestamp,
			       (guint64) (relative_offset * floor((double) current_x_offset / (double) relative_offset)));

  attack = (current_x_offset % relative_offset) % target_buffer_size;

  frame_count = target_buffer_size - attack;

  /* capture mode */
  capture_mode = AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_NONE;

  if(fx_playback_audio != NULL){
    g_object_get(fx_playback_audio,
		 "capture-mode", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value, G_TYPE_UINT64);

      ags_port_safe_read(port, &value);

      capture_mode = g_value_get_uint64(&value);
      
      g_object_unref(port);

      g_value_unset(&value);
    }
  }

  if(capture_mode == AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_DISCARD){
    //empty
  }else if(capture_mode == AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_NONE){
    AgsAudioSignal *current_audio_signal;
    
    GList *start_audio_signal, *audio_signal;

    GRecMutex *stream_mutex;
    
    /* get audio signal */
    current_audio_signal = NULL;
  
    start_audio_signal = NULL;

    g_rec_mutex_lock(fx_playback_audio_processor_mutex);

    start_audio_signal = fx_playback_audio_processor->capture_audio_signal;
  
    audio_signal = ags_audio_signal_find_by_recall_id(start_audio_signal, (GObject *) recall_id);
  
    if(audio_signal != NULL){
      current_audio_signal = audio_signal->data;
      g_object_ref(current_audio_signal);
    }
    
    g_rec_mutex_unlock(fx_playback_audio_processor_mutex);

    /* create audio signal */
    if(current_audio_signal == NULL){  
      AgsChannel *start_input, *input;
      AgsRecycling *recycling;
      
      gdouble delay;

      GValue value = {0,};
    
      start_input = NULL;
    
      g_object_get(audio,
		   "input", &start_input,
		   NULL);

      input = ags_channel_nth(start_input, audio_channel);

      recycling = NULL;

      if(input != NULL){
	g_object_get(input,
		     "first-recycling", &recycling,
		     NULL);
      }
    
      /* get delay */
      delay = AGS_SOUNDCARD_DEFAULT_DELAY;
  
      if(fx_playback_audio != NULL){
	port = NULL;
    
	g_object_get(fx_playback_audio,
		     "delay", &port,
		     NULL);

	if(port != NULL){
	  g_value_init(&value,
		       G_TYPE_DOUBLE);
    
	  ags_port_safe_read(port,
			     &value);

	  delay = g_value_get_double(&value);
	  g_value_unset(&value);

	  g_object_unref(port);
	}
      }

      current_audio_signal = ags_audio_signal_new(output_soundcard,
						  (GObject *) recycling,
						  (GObject *) recall_id);
      g_object_ref(current_audio_signal);
    
      g_object_set(current_audio_signal,
		   "samplerate", target_samplerate,
		   "buffer-size", target_buffer_size,
		   "format", target_format,
		   NULL);

      ags_audio_signal_stream_resize(current_audio_signal,
				     (guint) floor(delay) + 1);      

      g_rec_mutex_lock(fx_playback_audio_processor_mutex);

      fx_playback_audio_processor->capture_audio_signal = g_list_prepend(fx_playback_audio_processor->capture_audio_signal,
									 current_audio_signal);
    
      g_rec_mutex_unlock(fx_playback_audio_processor_mutex);
      
      ags_connectable_connect(AGS_CONNECTABLE(current_audio_signal));
      ags_recycling_add_audio_signal(recycling,
				     current_audio_signal);

      if(start_input != NULL){
	g_object_unref(start_input);
      }

      if(input != NULL){
	g_object_unref(input);
      }

      if(recycling != NULL){
	g_object_unref(recycling);
      }
    }

    stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(current_audio_signal);
    
    data = ags_soundcard_get_prev_buffer(AGS_SOUNDCARD(input_soundcard));
  
    target_copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(target_format),
							   ags_audio_buffer_util_format_from_soundcard(format));

    g_rec_mutex_lock(stream_mutex);
    ags_soundcard_lock_buffer(AGS_SOUNDCARD(input_soundcard), data);

    ags_audio_buffer_util_clear_buffer(current_audio_signal->stream_current->data, 1,
				       target_buffer_size, ags_audio_buffer_util_format_from_soundcard(target_format));
    
    ags_audio_buffer_util_copy_buffer_to_buffer(current_audio_signal->stream_current, 1, 0,
						data, audio_channels, input_soundcard_channel,
						target_buffer_size, target_copy_mode);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(input_soundcard), data);
    g_rec_mutex_unlock(stream_mutex);

    /* unref */
    if(current_audio_signal != NULL){
      g_object_unref(current_audio_signal);
    }
  }else{
    /* find wave - attempt #0 */
    current_wave = NULL;
  
    wave = ags_wave_find_near_timestamp(start_wave, audio_channel,
					timestamp);

    buffer = NULL;

    found_buffer = FALSE;
    
    if(wave == NULL){
      AgsTimestamp *current_timestamp;

      /* instantiate */
      current_wave = ags_wave_new((GObject *) audio,
				  audio_channel);

      g_object_get(current_wave,
		   "timestamp", &current_timestamp,
		   NULL);
      ags_timestamp_set_ags_offset(current_timestamp,
				   relative_offset * floor(current_x_offset / relative_offset));
      
      ags_audio_add_wave(audio,
			 (GObject *) current_wave);

      g_object_unref(current_timestamp);
    }else{
      current_wave = wave->data;
    
      buffer = ags_wave_find_point(current_wave,
				   current_x_offset,
				   FALSE);
    }
  
    if(buffer == NULL){
      /* instantiate buffer */
      buffer = ags_buffer_new();
      buffer->x = current_x_offset;
      
      ags_wave_add_buffer(current_wave,
			  buffer,
			  FALSE);
    }else{
      found_buffer = TRUE;
    }

    /* record */
    buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);

    data = ags_soundcard_get_prev_buffer(AGS_SOUNDCARD(input_soundcard));
  
    target_copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(target_format),
							   ags_audio_buffer_util_format_from_soundcard(format));

    attack = (current_x_offset % relative_offset) % target_buffer_size;
  
    g_rec_mutex_lock(buffer_mutex);
    ags_soundcard_lock_buffer(AGS_SOUNDCARD(input_soundcard), data);

    if(found_buffer &&
       capture_mode == AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_REPLACE){
      ags_audio_buffer_util_clear_buffer(buffer->data, 1,
					 frame_count, ags_audio_buffer_util_format_from_soundcard(target_format));
    }
    
    ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, 1, attack,
						data, audio_channels, input_soundcard_channel,
						frame_count, target_copy_mode);

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(input_soundcard), data);
    g_rec_mutex_unlock(buffer_mutex);
  
    /* data put */
    ags_fx_playback_audio_processor_data_put(fx_playback_audio_processor,
					     buffer,
					     AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_PLAY);

    /* find wave - attempt #1 */
    if(attack != 0 ||
       frame_count != buffer_size){
      ags_timestamp_set_ags_offset(timestamp,
				   (guint64) (relative_offset * floor((double) (current_x_offset + frame_count) / (double) relative_offset)));

      wave = ags_wave_find_near_timestamp(start_wave, audio_channel,
					  timestamp);

      buffer = NULL;
  
      found_buffer = FALSE;
      
      if(wave == NULL){
	AgsTimestamp *current_timestamp;

	/* instantiate */
	current_wave = ags_wave_new((GObject *) audio,
				    audio_channel);

	g_object_get(current_wave,
		     "timestamp", &current_timestamp,
		     NULL);
	ags_timestamp_set_ags_offset(current_timestamp,
				     (guint64) (relative_offset * floor((double) (current_x_offset + frame_count) / (double) relative_offset)));
      
	ags_audio_add_wave(audio,
			   (GObject *) current_wave);

	g_object_unref(current_timestamp);
      }else{
	current_wave = wave->data;
    
	buffer = ags_wave_find_point(current_wave,
				     current_x_offset + frame_count,
				     FALSE);
      }
  
      if(buffer == NULL){
	/* instantiate buffer */
	buffer = ags_buffer_new();
	buffer->x = current_x_offset + frame_count;
      
	ags_wave_add_buffer(current_wave,
			    buffer,
			    FALSE);
      }else{
	found_buffer = TRUE;
      }

      /* record */
      buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);
    
      g_rec_mutex_lock(buffer_mutex);
      ags_soundcard_lock_buffer(AGS_SOUNDCARD(input_soundcard), data);

      if(found_buffer &&
	 capture_mode == AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_REPLACE){
	ags_audio_buffer_util_clear_buffer(buffer->data, 1,
					   frame_count, ags_audio_buffer_util_format_from_soundcard(target_format));
      }
    
      ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, 1, 0,
						  data, audio_channels, (frame_count * audio_channels) + input_soundcard_channel,
						  target_buffer_size - frame_count, target_copy_mode);

      ags_soundcard_unlock_buffer(AGS_SOUNDCARD(input_soundcard), data);
      g_rec_mutex_unlock(buffer_mutex);
  
      /* data put */
      ags_fx_playback_audio_processor_data_put(fx_playback_audio_processor,
					       buffer,
					       AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_PLAY);
    }
  }
  
  /* unref */
  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
  }

  if(input_soundcard != NULL){
    g_object_unref(input_soundcard);
  }
  
  if(audio != NULL){
    g_object_unref(audio);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(input != NULL){
    g_object_unref(input);
  }
  
  g_list_free_full(start_wave,
		   (GDestroyNotify) g_object_unref);

  if(fx_playback_audio != NULL){
    g_object_unref(fx_playback_audio);
  }

  if(recall_id != NULL){
    g_object_unref(recall_id);
  }
}

void
ags_fx_playback_audio_processor_record(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(fx_playback_audio_processor));

  g_object_ref(fx_playback_audio_processor);
  
  if(AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->record != NULL){
    AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->record(fx_playback_audio_processor);
  }
  
  g_object_unref(fx_playback_audio_processor);
}

void
ags_fx_playback_audio_processor_real_feed(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  AgsFxPlaybackAudio *fx_playback_audio;

  GList *start_feed_audio_signal, *feed_audio_signal;
  GList *start_feeding_audio_signal, *feeding_audio_signal;

  gboolean done;
  
  GRecMutex *fx_playback_audio_processor_mutex;

  fx_playback_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio_processor);

  fx_playback_audio = NULL;
  
  g_object_get(fx_playback_audio_processor,
	       "recall-audio", &fx_playback_audio,
	       NULL);

  done = FALSE;

  /* get feed audio signal */
  feed_audio_signal = 
    start_feed_audio_signal = ags_fx_playback_audio_get_feed_audio_signal(fx_playback_audio);
  
  /* check new */
  while(feed_audio_signal != NULL){
    gboolean is_new;
    
    g_rec_mutex_lock(fx_playback_audio_processor_mutex);

    is_new = (g_list_find(fx_playback_audio_processor->feeding_audio_signal, feed_audio_signal->data) == NULL) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(fx_playback_audio_processor_mutex);

    if(is_new){
      g_rec_mutex_lock(fx_playback_audio_processor_mutex);

      fx_playback_audio_processor->feeding_audio_signal = g_list_prepend(fx_playback_audio_processor->feeding_audio_signal,
									 feed_audio_signal->data);
      
      g_rec_mutex_unlock(fx_playback_audio_processor_mutex);

      g_object_ref(feed_audio_signal->data);
    }
    
    feed_audio_signal = feed_audio_signal->next;
  }

  /* check removed and update */
  g_rec_mutex_lock(fx_playback_audio_processor_mutex);

  feeding_audio_signal = 
    start_feeding_audio_signal = g_list_copy_deep(fx_playback_audio_processor->feeding_audio_signal,
						  (GCopyFunc) g_object_ref,
						  NULL);
  
  g_rec_mutex_unlock(fx_playback_audio_processor_mutex);
  
  while(feeding_audio_signal != NULL){
    gboolean is_removed;

    is_removed = (g_list_find(start_feed_audio_signal, feeding_audio_signal->data) == NULL) ? TRUE: FALSE;

    if(is_removed){
      g_rec_mutex_lock(fx_playback_audio_processor_mutex);
      
      fx_playback_audio_processor->feeding_audio_signal = g_list_remove(fx_playback_audio_processor->feeding_audio_signal,
									feeding_audio_signal->data); 

      done = (fx_playback_audio_processor->feeding_audio_signal == NULL) ? TRUE: FALSE;

      g_object_unref(feeding_audio_signal->data);
      
      g_rec_mutex_unlock(fx_playback_audio_processor_mutex);
    }

    feeding_audio_signal = feeding_audio_signal->next;
  }

  if(done){
    ags_recall_done((AgsRecall *) fx_playback_audio_processor);
  }

  if(fx_playback_audio != NULL){
    g_object_unref(fx_playback_audio);
  }

  g_list_free_full(start_feed_audio_signal,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(start_feeding_audio_signal,
		   (GDestroyNotify) g_object_unref);
}

void
ags_fx_playback_audio_processor_feed(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(fx_playback_audio_processor));

  g_object_ref(fx_playback_audio_processor);

  if(AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->feed != NULL){
    AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->feed(fx_playback_audio_processor);
  }
  
  g_object_unref(fx_playback_audio_processor);
}

void
ags_fx_playback_audio_processor_real_master(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{ 
  AgsFxPlaybackAudio *fx_playback_audio;

  GList *start_master_audio_signal, *master_audio_signal;
  GList *start_mastering_audio_signal, *mastering_audio_signal;

  gboolean done;
  
  GRecMutex *fx_playback_audio_processor_mutex;

  fx_playback_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio_processor);

  fx_playback_audio = NULL;
  
  g_object_get(fx_playback_audio_processor,
	       "recall-audio", &fx_playback_audio,
	       NULL);

  done = FALSE;

  /* get master audio signal */
  master_audio_signal = 
    start_master_audio_signal = ags_fx_playback_audio_get_master_audio_signal(fx_playback_audio);
  
  /* check new */
  while(master_audio_signal != NULL){
    gboolean is_new;
    
    g_rec_mutex_lock(fx_playback_audio_processor_mutex);

    is_new = (g_list_find(fx_playback_audio_processor->mastering_audio_signal, master_audio_signal->data) == NULL) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(fx_playback_audio_processor_mutex);

    if(is_new){
      g_rec_mutex_lock(fx_playback_audio_processor_mutex);

      fx_playback_audio_processor->mastering_audio_signal = g_list_prepend(fx_playback_audio_processor->mastering_audio_signal,
									   master_audio_signal->data);
      
      g_rec_mutex_unlock(fx_playback_audio_processor_mutex);

      g_object_ref(master_audio_signal->data);
    }
    
    master_audio_signal = master_audio_signal->next;
  }

  /* check removed and update */
  g_rec_mutex_lock(fx_playback_audio_processor_mutex);

  mastering_audio_signal = 
    start_mastering_audio_signal = g_list_copy_deep(fx_playback_audio_processor->mastering_audio_signal,
						    (GCopyFunc) g_object_ref,
						    NULL);
  
  g_rec_mutex_unlock(fx_playback_audio_processor_mutex);
  
  while(mastering_audio_signal != NULL){
    gboolean is_removed;

    is_removed = (g_list_find(start_master_audio_signal, mastering_audio_signal->data) == NULL) ? TRUE: FALSE;

    if(is_removed){
      g_rec_mutex_lock(fx_playback_audio_processor_mutex);
      
      fx_playback_audio_processor->mastering_audio_signal = g_list_remove(fx_playback_audio_processor->mastering_audio_signal,
									  mastering_audio_signal->data); 

      done = (fx_playback_audio_processor->mastering_audio_signal == NULL) ? TRUE: FALSE;
      
      g_rec_mutex_unlock(fx_playback_audio_processor_mutex);

      g_object_unref(mastering_audio_signal->data);
    }

    mastering_audio_signal = mastering_audio_signal->next;
  }

  if(done){
    ags_recall_done((AgsRecall *) fx_playback_audio_processor);
  }

  if(fx_playback_audio != NULL){
    g_object_unref(fx_playback_audio);
  }

  g_list_free_full(start_master_audio_signal,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(start_mastering_audio_signal,
		   (GDestroyNotify) g_object_unref);
}

void
ags_fx_playback_audio_processor_master(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(fx_playback_audio_processor));

  g_object_ref(fx_playback_audio_processor);

  if(AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->master != NULL){
    AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->master(fx_playback_audio_processor);
  }
  
  g_object_unref(fx_playback_audio_processor);
}

void
ags_fx_playback_audio_processor_real_counter_change(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  AgsFxPlaybackAudio *fx_playback_audio;

  gdouble delay;
  gboolean loop;
  guint64 loop_start, loop_end;
  guint buffer_size;
  
  GValue value = {0,};

  GRecMutex *fx_playback_audio_processor_mutex;
  
  fx_playback_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio_processor);

  fx_playback_audio = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  g_object_get(fx_playback_audio_processor,
	       "recall-audio", &fx_playback_audio,
	       "buffer-size", &buffer_size,
	       NULL);

  delay = AGS_SOUNDCARD_DEFAULT_DELAY;

  loop = FALSE;

  loop_start = AGS_FX_PLAYBACK_AUDIO_DEFAULT_LOOP_START;
  loop_end = AGS_FX_PLAYBACK_AUDIO_DEFAULT_LOOP_END;
  
  if(fx_playback_audio != NULL){
    AgsPort *port;

    /* delay */
    g_object_get(fx_playback_audio,
		 "delay", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value,
		   G_TYPE_DOUBLE);
    
      ags_port_safe_read(port,
			 &value);

      delay = g_value_get_double(&value);
      g_value_unset(&value);

      g_object_unref(port);
    }

    /* loop */
    g_object_get(fx_playback_audio,
		 "loop", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value,
		   G_TYPE_BOOLEAN);
    
      ags_port_safe_read(port,
			 &value);

      loop = g_value_get_boolean(&value);
      g_value_unset(&value);

      g_object_unref(port);
    }

    /* loop-start */
    g_object_get(fx_playback_audio,
		 "loop-start", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value,
		   G_TYPE_UINT64);
    
      ags_port_safe_read(port,
			 &value);

      loop_start = g_value_get_uint64(&value);
      g_value_unset(&value);

      g_object_unref(port);
    }

    /* loop-end */
    g_object_get(fx_playback_audio,
		 "loop-end", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value,
		   G_TYPE_UINT64);
    
      ags_port_safe_read(port,
			 &value);

      loop_end = g_value_get_uint64(&value);
      g_value_unset(&value);

      g_object_unref(port);
    }
  }

  if(delay + 1.0 >= delay){
    g_rec_mutex_lock(fx_playback_audio_processor_mutex);
    
    fx_playback_audio_processor->delay_counter = 0.0;

    if(loop &&
       fx_playback_audio_processor->offset_counter + 1 >= loop_end){
      fx_playback_audio_processor->offset_counter = loop_start;

      fx_playback_audio_processor->x_offset = (guint64) floor(loop_start * delay) * buffer_size;
    }else{
      fx_playback_audio_processor->offset_counter += 1;

      fx_playback_audio_processor->x_offset += buffer_size;
    }
    
    g_rec_mutex_unlock(fx_playback_audio_processor_mutex);
  }else{
    g_rec_mutex_lock(fx_playback_audio_processor_mutex);
    
    fx_playback_audio_processor->delay_counter += 1.0;

    fx_playback_audio_processor->x_offset += buffer_size;

    g_rec_mutex_unlock(fx_playback_audio_processor_mutex);
  }
  
  if(fx_playback_audio != NULL){
    g_object_unref(fx_playback_audio);
  }
}

void
ags_fx_playback_audio_processor_counter_change(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_PLAYBACK_AUDIO_PROCESSOR(fx_playback_audio_processor));

  g_object_ref(fx_playback_audio_processor);
  
  if(AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->counter_change != NULL){
    AGS_FX_PLAYBACK_AUDIO_PROCESSOR_GET_CLASS(fx_playback_audio_processor)->counter_change(fx_playback_audio_processor);
  }
  
  g_object_unref(fx_playback_audio_processor);
}

/**
 * ags_fx_playback_audio_processor_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxPlaybackAudioProcessor
 *
 * Returns: the new #AgsFxPlaybackAudioProcessor
 *
 * Since: 3.3.0
 */
AgsFxPlaybackAudioProcessor*
ags_fx_playback_audio_processor_new(AgsAudio *audio)
{
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;

  fx_playback_audio_processor = (AgsFxPlaybackAudioProcessor *) g_object_new(AGS_TYPE_FX_PLAYBACK_AUDIO_PROCESSOR,
									     "audio", audio,
									     NULL);

  return(fx_playback_audio_processor);
}
