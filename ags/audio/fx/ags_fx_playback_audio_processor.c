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
void ags_fx_playback_audio_processor_seekable_interface_init(AgsSeekableInterface *seekable);
void ags_fx_playback_audio_processor_countable_interface_init(AgsCountableInterface *countable);
void ags_fx_playback_audio_processor_tactable_interface_init(AgsTactableInterface *tactable);
void ags_fx_playback_audio_processor_init(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor);
void ags_fx_playback_audio_processor_dispose(GObject *gobject);
void ags_fx_playback_audio_processor_finalize(GObject *gobject);

void ags_fx_playback_audio_processor_seek(AgsSeekable *seekable,
					  gint64 offset,
					  guint whence);

guint64 ags_fx_playback_audio_processor_get_wave_counter(AgsCountable *countable);

gdouble ags_fx_playback_audio_processor_get_bpm(AgsTactable *tactable);
gdouble ags_fx_playback_audio_processor_get_tact(AgsTactable *tactable);
void ags_fx_playback_audio_processor_change_bpm(AgsTactable *tactable, gdouble new_bpm, gdouble old_bpm);
void ags_fx_playback_audio_processor_change_tact(AgsTactable *tactable, gdouble new_tact, gdouble old_tact);

void ags_fx_playback_audio_processor_run_init_pre(AgsRecall *recall);
void ags_fx_playback_audio_processor_run_inter(AgsRecall *recall);

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

    static const GInterfaceInfo ags_seekable_interface_info = {
      (GInterfaceInitFunc) ags_fx_playback_audio_processor_seekable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_countable_interface_info = {
      (GInterfaceInitFunc) ags_fx_playback_audio_processor_countable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_tactable_interface_info = {
      (GInterfaceInitFunc) ags_fx_playback_audio_processor_tactable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_fx_playback_audio_processor = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
								  "AgsFxPlaybackAudioProcessor",
								  &ags_fx_playback_audio_processor_info,
								  0);

    g_type_add_interface_static(ags_type_fx_playback_audio_processor,
				AGS_TYPE_COUNTABLE,
				&ags_countable_interface_info);

    g_type_add_interface_static(ags_type_fx_playback_audio_processor,
				AGS_TYPE_SEEKABLE,
				&ags_seekable_interface_info);

    g_type_add_interface_static(ags_type_fx_playback_audio_processor,
				AGS_TYPE_TACTABLE,
				&ags_tactable_interface_info);

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
  
  recall->run_init_pre = ags_fx_playback_audio_processor_run_init_pre;
  recall->run_inter = ags_fx_playback_audio_processor_run_inter;

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
ags_fx_playback_audio_processor_seekable_interface_init(AgsSeekableInterface *seekable)
{
  seekable->seek = ags_fx_playback_audio_processor_seek;
}

void
ags_fx_playback_audio_processor_countable_interface_init(AgsCountableInterface *countable)
{
  countable->get_sequencer_counter = NULL;
  countable->get_notation_counter = NULL;
  countable->get_wave_counter = ags_fx_playback_audio_processor_get_wave_counter;
  countable->get_midi_counter = NULL;
}

void
ags_fx_playback_audio_processor_tactable_interface_init(AgsTactableInterface *tactable)
{
  tactable->get_bpm = ags_fx_playback_audio_processor_get_bpm;
  tactable->get_tact = ags_fx_playback_audio_processor_get_tact;

  tactable->get_sequencer_duration = NULL;
  tactable->get_notation_duration = NULL;
  tactable->get_wave_duration = NULL;
  tactable->get_midi_duration = NULL;

  tactable->change_sequencer_duration = NULL;
  tactable->change_notation_duration = NULL;
  tactable->change_wave_duration = NULL;
  tactable->change_midi_duration = NULL;

  tactable->change_bpm = ags_fx_playback_audio_processor_change_bpm;
  tactable->change_tact = ags_fx_playback_audio_processor_change_tact;
}

void
ags_fx_playback_audio_processor_init(AgsFxPlaybackAudioProcessor *fx_playback_audio_processor)
{
  AGS_RECALL(fx_playback_audio_processor)->name = "ags-fx-playback";
  AGS_RECALL(fx_playback_audio_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_playback_audio_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_playback_audio_processor)->xml_type = "ags-fx-playback-audio-processor";

  /* counter */
  fx_playback_audio_processor->delay_completion = 0.0;

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
ags_fx_playback_audio_processor_seek(AgsSeekable *seekable,
				     gint64 offset,
				     guint whence)
{
  AgsFxPlaybackAudio *fx_playback_audio;
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;
  AgsPort *port;

  gdouble playback_duration;
  
  GRecMutex *recall_mutex;

  gdouble delay;
  guint64 playback_counter;
  guint buffer_size;

  GValue value = {0,};

  fx_playback_audio = NULL;
  
  fx_playback_audio_processor = AGS_FX_PLAYBACK_AUDIO_PROCESSOR(seekable);
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio_processor);

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  g_object_get(fx_playback_audio_processor,
	       "recall-audio", &fx_playback_audio,
	       "buffer-size", &buffer_size,
	       NULL);

  /* delay */
  port = NULL;

  delay = AGS_SOUNDCARD_DEFAULT_DELAY;
  
  playback_duration = ceil(AGS_NOTATION_DEFAULT_DURATION * delay);
      
  g_object_get(fx_playback_audio,
	       "delay", &port,
	       NULL);

  if(port != NULL){
    g_value_init(&value, G_TYPE_DOUBLE);

    ags_port_safe_read(port, &value);
    
    delay = g_value_get_double(&value);

    g_value_unset(&value);
	
    g_object_unref(port);
  }

  /* duration */
  port = NULL;

  g_object_get(fx_playback_audio,
	       "duration", &port,
	       NULL);

  if(port != NULL){
    g_value_init(&value, G_TYPE_UINT64);

    ags_port_safe_read(port, &value);
    
    playback_duration = g_value_get_uint64(&value);

    g_value_unset(&value);
	
    g_object_unref(port);
  }
  
  switch(whence){
  case AGS_SEEK_CUR:
  {
    g_rec_mutex_lock(recall_mutex);

    playback_counter = fx_playback_audio_processor->offset_counter;

    if(playback_counter + offset < 0){
      playback_counter = (guint64) playback_duration - (guint64) ((offset - playback_counter) % (guint64) playback_duration);
    }else{
      playback_counter = (guint64) (playback_counter + offset) % (guint64) playback_duration;
    }

    fx_playback_audio_processor->delay_counter =
      fx_playback_audio_processor->current_delay_counter = 0.0;

    fx_playback_audio_processor->offset_counter = 
      fx_playback_audio_processor->current_offset_counter = playback_counter;

    fx_playback_audio_processor->x_offset = 
      fx_playback_audio_processor->current_x_offset = (guint64) floor(playback_counter * delay) * buffer_size;
  
    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case AGS_SEEK_END:
  {      
    g_rec_mutex_lock(recall_mutex);

    playback_counter = fx_playback_audio_processor->offset_counter;

    /* playback */
    if(playback_duration + offset < 0){
      playback_counter = (guint64) playback_duration - ((guint64) (offset - playback_duration) % (guint64) playback_duration);
    }else{
      playback_counter = (guint64) (playback_duration + offset) % (guint64) playback_duration;
    }

    fx_playback_audio_processor->delay_counter =
      fx_playback_audio_processor->current_delay_counter = 0.0;

    fx_playback_audio_processor->offset_counter =
      fx_playback_audio_processor->current_offset_counter = playback_counter;
  
    fx_playback_audio_processor->x_offset = 
      fx_playback_audio_processor->current_x_offset = (guint64) floor(playback_counter * delay) * buffer_size;

    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  case AGS_SEEK_SET:
  {
    g_rec_mutex_lock(recall_mutex);

    fx_playback_audio_processor->delay_counter =
      fx_playback_audio_processor->current_delay_counter = 0.0;
      
    fx_playback_audio_processor->offset_counter =
      fx_playback_audio_processor->current_offset_counter = offset;

    fx_playback_audio_processor->x_offset = 
      fx_playback_audio_processor->current_x_offset = (guint64) floor(offset * delay) * buffer_size;
    
    g_rec_mutex_unlock(recall_mutex);
  }
  break;
  }

  if(fx_playback_audio != NULL){
    g_object_unref(fx_playback_audio);
  }
}

guint64
ags_fx_playback_audio_processor_get_wave_counter(AgsCountable *countable)
{
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;
  
  guint64 playback_counter;

  GRecMutex *recall_mutex;

  fx_playback_audio_processor = AGS_FX_PLAYBACK_AUDIO_PROCESSOR(countable);
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio_processor);

  /* bpm */
  g_rec_mutex_lock(recall_mutex);

  playback_counter = fx_playback_audio_processor->offset_counter;
  
  g_rec_mutex_unlock(recall_mutex);
  
  return(playback_counter);
}

gdouble
ags_fx_playback_audio_processor_get_bpm(AgsTactable *tactable)
{
  AgsFxPlaybackAudio *fx_playback_audio;
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;
  AgsPort *port;
  
  gdouble bpm;
  
  GValue value = {0,};
  
  fx_playback_audio  = NULL;
  
  fx_playback_audio_processor = AGS_FX_PLAYBACK_AUDIO_PROCESSOR(tactable);

  port = NULL;
  
  bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  g_object_get(fx_playback_audio_processor,
	       "recall-audio", &fx_playback_audio,
	       NULL);
  
  if(fx_playback_audio != NULL){
    g_object_get(fx_playback_audio,
		 "bpm", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value, G_TYPE_DOUBLE);

      ags_port_safe_read(port, &value);

      bpm = g_value_get_double(&value);

      g_value_unset(&value);
      
      g_object_unref(port);
    }
  }
  
  if(fx_playback_audio != NULL){
    g_object_unref(fx_playback_audio);
  }
  
  return(bpm);
}

gdouble
ags_fx_playback_audio_processor_get_tact(AgsTactable *tactable)
{
  AgsFxPlaybackAudio *fx_playback_audio;
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;
  AgsPort *port;
  
  gdouble tact;
  
  GValue value = {0,};
  
  fx_playback_audio  = NULL;
  
  fx_playback_audio_processor = AGS_FX_PLAYBACK_AUDIO_PROCESSOR(tactable);

  port = NULL;
  
  tact = AGS_SOUNDCARD_DEFAULT_TACT;

  g_object_get(fx_playback_audio_processor,
	       "recall-audio", &fx_playback_audio,
	       NULL);
  
  if(fx_playback_audio != NULL){
    g_object_get(fx_playback_audio,
		 "tact", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value, G_TYPE_DOUBLE);

      ags_port_safe_read(port, &value);

      tact = g_value_get_double(&value);

      g_value_unset(&value);
      
      g_object_unref(port);
    }
  }
  
  if(fx_playback_audio != NULL){
    g_object_unref(fx_playback_audio);
  }
  
  return(tact);
}

void
ags_fx_playback_audio_processor_change_bpm(AgsTactable *tactable, gdouble new_bpm, gdouble old_bpm)
{
  AgsFxPlaybackAudio *fx_playback_audio;
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;
  AgsPort *port;

  GObject *output_soundcard;
    
  GValue value = {0,};

  output_soundcard = NULL;
  
  fx_playback_audio  = NULL;
  
  fx_playback_audio_processor = AGS_FX_PLAYBACK_AUDIO_PROCESSOR(tactable);

  port = NULL;
  
  g_object_get(fx_playback_audio_processor,
	       "output-soundcard", &output_soundcard,
	       "recall-audio", &fx_playback_audio,
	       NULL);

  /* delay */
  if(fx_playback_audio != NULL){
    port = NULL;
    
    g_object_get(fx_playback_audio,
		 "delay", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value, G_TYPE_DOUBLE);

      g_value_set_double(&value, ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard)));

      ags_port_safe_write(port, &value);

      g_value_unset(&value);

      g_object_unref(port);
    }
  }
  
  /* bpm */
  if(fx_playback_audio != NULL){
    port = NULL;
    
    g_object_get(fx_playback_audio,
		 "bpm", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value, G_TYPE_DOUBLE);

      g_value_set_double(&value, new_bpm);

      ags_port_safe_write(port, &value);

      g_value_unset(&value);
      
      g_object_unref(port);
    }
  }
  
  if(fx_playback_audio != NULL){
    g_object_unref(fx_playback_audio);
  }
}

void
ags_fx_playback_audio_processor_change_tact(AgsTactable *tactable, gdouble new_tact, gdouble old_tact)
{
  AgsFxPlaybackAudio *fx_playback_audio;
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;
  AgsPort *port;
    
  GObject *output_soundcard;

  GValue value = {0,};

  output_soundcard = NULL;
  
  fx_playback_audio  = NULL;
  
  fx_playback_audio_processor = AGS_FX_PLAYBACK_AUDIO_PROCESSOR(tactable);

  port = NULL;
  
  g_object_get(fx_playback_audio_processor,
	       "output-soundcard", &output_soundcard,
	       "recall-audio", &fx_playback_audio,
	       NULL);
  /* delay */
  if(fx_playback_audio != NULL){
    port = NULL;
    
    g_object_get(fx_playback_audio,
		 "delay", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value, G_TYPE_DOUBLE);

      g_value_set_double(&value, ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard)));

      ags_port_safe_write(port, &value);

      g_value_unset(&value);

      g_object_unref(port);
    }
  }
  
  /* tact */
  if(fx_playback_audio != NULL){
    port = NULL;
    
    g_object_get(fx_playback_audio,
		 "tact", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value, G_TYPE_DOUBLE);

      g_value_set_double(&value, new_tact);

      ags_port_safe_write(port, &value);

      g_value_unset(&value);
      
      g_object_unref(port);
    }
  }
  
  if(fx_playback_audio != NULL){
    g_object_unref(fx_playback_audio);
  }
}

void
ags_fx_playback_audio_processor_run_init_pre(AgsRecall *recall)
{
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;

  gdouble delay_counter;
  
  GRecMutex *fx_playback_audio_processor_mutex;

  fx_playback_audio_processor = AGS_FX_PLAYBACK_AUDIO_PROCESSOR(recall);
  
  fx_playback_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio_processor);

  /* get delay counter */
  g_rec_mutex_lock(fx_playback_audio_processor_mutex);
    
  fx_playback_audio_processor->delay_counter = 0;
  fx_playback_audio_processor->offset_counter = 0;
  fx_playback_audio_processor->x_offset = 0;

  fx_playback_audio_processor->current_delay_counter = 0;
  fx_playback_audio_processor->current_offset_counter = 0;
  fx_playback_audio_processor->current_x_offset = 0;

  g_rec_mutex_unlock(fx_playback_audio_processor_mutex);
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_playback_audio_processor_parent_class)->run_init_pre(recall);
}

void
ags_fx_playback_audio_processor_run_inter(AgsRecall *recall)
{
  AgsFxPlaybackAudioProcessor *fx_playback_audio_processor;
  AgsRecallID *recall_id;
  AgsRecyclingContext *parent_recycling_context, *recycling_context;

  GList *start_playing_audio_signal, *playing_audio_signal;
  GList *start_recording_audio_signal, *recording_audio_signal;
  
  gint sound_scope;
  gdouble delay_counter;
  
  GRecMutex *fx_playback_audio_processor_mutex;

  fx_playback_audio_processor = AGS_FX_PLAYBACK_AUDIO_PROCESSOR(recall);
  
  fx_playback_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio_processor);

  recall_id = NULL;

  sound_scope = ags_recall_get_sound_scope(recall);

  g_object_get(recall,
	       "recall-id", &recall_id,
	       NULL);

  if(!ags_recall_id_check_sound_scope(recall_id, sound_scope)){
    if(recall_id != NULL){
      g_object_unref(recall_id);
    }

    AGS_RECALL_CLASS(ags_fx_playback_audio_processor_parent_class)->run_inter(recall);
    
    return;
  }

  recycling_context = NULL;
  parent_recycling_context = NULL;
  
  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);
  
  /* get delay counter */
  g_rec_mutex_lock(fx_playback_audio_processor_mutex);
    
  fx_playback_audio_processor->delay_counter = fx_playback_audio_processor->current_delay_counter;
  fx_playback_audio_processor->offset_counter = fx_playback_audio_processor->current_offset_counter;
  
  fx_playback_audio_processor->x_offset = fx_playback_audio_processor->current_x_offset;

  delay_counter = fx_playback_audio_processor->delay_counter;

  g_rec_mutex_unlock(fx_playback_audio_processor_mutex);

  /* run */
  g_rec_mutex_lock(fx_playback_audio_processor_mutex);

  start_playing_audio_signal = g_list_copy_deep(fx_playback_audio_processor->playing_audio_signal,
						(GCopyFunc) g_object_ref,
						NULL);

  start_recording_audio_signal = g_list_copy_deep(fx_playback_audio_processor->recording_audio_signal,
						  (GCopyFunc) g_object_ref,
						  NULL);
  
  g_rec_mutex_unlock(fx_playback_audio_processor_mutex);

  /* clear playing audio signal */
  playing_audio_signal = start_playing_audio_signal;

  while(playing_audio_signal != NULL){
    AgsAudioSignal *audio_signal;

    guint buffer_size;
    guint format;

    GRecMutex *stream_mutex;

    audio_signal = AGS_AUDIO_SIGNAL(playing_audio_signal->data);
    
    stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(audio_signal);
    
    g_object_get(audio_signal,
		 "buffer-size", &buffer_size,
		 "format", &format,
		 NULL);

    g_rec_mutex_lock(stream_mutex);
    
    ags_audio_buffer_util_clear_buffer(audio_signal->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));

    
    g_rec_mutex_unlock(stream_mutex);

    playing_audio_signal = playing_audio_signal->next;
  }

  /* clear recording audio signal */
  recording_audio_signal = start_recording_audio_signal;

  while(recording_audio_signal != NULL){
    AgsAudioSignal *audio_signal;
    
    guint buffer_size;
    guint format;

    GRecMutex *stream_mutex;

    audio_signal = AGS_AUDIO_SIGNAL(recording_audio_signal->data);

    stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(audio_signal);
    
    g_object_get(audio_signal,
		 "buffer-size", &buffer_size,
		 "format", &format,
		 NULL);

    g_rec_mutex_lock(stream_mutex);
    
    ags_audio_buffer_util_clear_buffer(audio_signal->stream_current->data, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));

    
    g_rec_mutex_unlock(stream_mutex);

    recording_audio_signal = recording_audio_signal->next;
  }
  
  /* play/record/feed/master */
  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_WAVE)){
    if(parent_recycling_context != NULL){
      ags_fx_playback_audio_processor_play(fx_playback_audio_processor);
    }
  }

  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_WAVE)){
    if(parent_recycling_context != NULL){
      ags_fx_playback_audio_processor_record(fx_playback_audio_processor);
    }
  }
  
  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_PLAYBACK) ||
     ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_MIDI)){
    if(parent_recycling_context == NULL){
      ags_fx_playback_audio_processor_feed(fx_playback_audio_processor);
    }
  }

  if(ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_SEQUENCER) ||
     ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_NOTATION) ||
     ags_recall_id_check_sound_scope(recall_id, AGS_SOUND_SCOPE_MIDI)){
    if(parent_recycling_context == NULL){
      ags_fx_playback_audio_processor_master(fx_playback_audio_processor);
    }
  }
  
  /* counter change */
  ags_fx_playback_audio_processor_counter_change(fx_playback_audio_processor);

  if(recall_id != NULL){
    g_object_unref(recall_id);
  }

  if(recycling_context != NULL){
    g_object_unref(recycling_context);
  }

  if(parent_recycling_context != NULL){
    g_object_unref(parent_recycling_context);
  }

  g_list_free_full(start_playing_audio_signal,
		   (GDestroyNotify) g_object_unref);

  g_list_free_full(start_recording_audio_signal,
		   (GDestroyNotify) g_object_unref);
  
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

  guint buffer_x_offset, x_offset;
  guint buffer_samplerate, samplerate;
  guint buffer_buffer_size, buffer_size;
  guint buffer_format, format;
  guint copy_mode;
  guint attack;
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
	       "x", &buffer_x_offset,
	       "samplerate", &buffer_samplerate,
	       "buffer-size", &buffer_buffer_size,
	       "format", &buffer_format,
	       NULL);

  /* get audio signal */
  current_audio_signal = NULL;
  
  start_audio_signal = NULL;
  
  g_rec_mutex_lock(fx_playback_audio_processor_mutex);

  x_offset = fx_playback_audio_processor->x_offset;
  
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
				   2);
    current_audio_signal->stream_current = current_audio_signal->stream;
    
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

    if(audio != NULL){
      g_object_unref(audio);
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

    if(x_offset < buffer_x_offset){
      attack = (samplerate / buffer_samplerate) * (buffer_x_offset - x_offset);
      
      ags_audio_buffer_util_copy_buffer_to_buffer(current_audio_signal->stream_current->data, 1, attack,
						  buffer_data, 1, 0,
						  buffer_size - attack, copy_mode);
    }else{
      attack = (samplerate / buffer_samplerate) * (x_offset - buffer_x_offset);
      
      ags_audio_buffer_util_copy_buffer_to_buffer(current_audio_signal->stream_current->data, 1, 0,
						  buffer_data, 1, attack,
						  buffer_size - attack, copy_mode);
    }
    
    g_rec_mutex_unlock(stream_mutex);

    ags_stream_free(buffer_data);
  }else{
    g_rec_mutex_lock(buffer_mutex);
    g_rec_mutex_lock(stream_mutex);

    if(x_offset < buffer_x_offset){
      attack = buffer_x_offset - x_offset;
      
      ags_audio_buffer_util_copy_buffer_to_buffer(current_audio_signal->stream_current->data, 1, attack,
						  buffer->data, 1, 0,
						  buffer_size - attack, copy_mode);
    }else{
      attack = x_offset - buffer_x_offset;
      
      ags_audio_buffer_util_copy_buffer_to_buffer(current_audio_signal->stream_current->data, 1, 0,
						  buffer->data, 1, attack,
						  buffer_size - attack, copy_mode);      
    }
    
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
  guint64 x_offset;
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
  
  x_offset = fx_playback_audio_processor->x_offset;
  
  g_rec_mutex_unlock(fx_playback_audio_processor_mutex);

  /* time stamp offset */
  relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * samplerate;

  ags_timestamp_set_ags_offset(timestamp,
			       (guint64) (relative_offset * floor((double) x_offset / (double) relative_offset)));

  attack = (x_offset % relative_offset) % buffer_size;

  frame_count = buffer_size - attack;

  if(x_offset + frame_count > relative_offset * floor(x_offset / relative_offset) + relative_offset){
    frame_count = relative_offset * floor((x_offset + frame_count) / relative_offset) - x_offset;
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
				 x_offset,
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
				 (guint64) (relative_offset * floor((double) (x_offset + frame_count) / (double) relative_offset)));

    wave = ags_wave_find_near_timestamp(start_wave, audio_channel,
					timestamp);

    if(wave != NULL){
      AgsBuffer *buffer;

      buffer = ags_wave_find_point(wave->data,
				   x_offset + frame_count,
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
  guint64 x_offset;
  guint64 x_buffer_offset_0, x_buffer_offset_1;
  guint attack;
  guint samplerate, target_samplerate, file_samplerate;
  guint buffer_size, target_buffer_size, file_buffer_size;
  guint format, target_format, file_format;
  guint frame_count;
  guint target_copy_mode, file_copy_mode;
  guint capture_mode;
  gboolean create_wave;
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
	       "recall-audio", &fx_playback_audio,
	       "audio-channel", &audio_channel,
	       "recall-id", &recall_id,
	       NULL);

  audio_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  if(input_soundcard != NULL){
    ags_soundcard_get_presets(AGS_SOUNDCARD(input_soundcard),
			      &audio_channels,
			      &samplerate,
			      &buffer_size,
			      &format);
  }
  
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
	       "input-soundcard", &input_soundcard,
	       "input-soundcard-channel", &input_soundcard_channel,
	       NULL);

  g_rec_mutex_lock(fx_playback_audio_processor_mutex);

  timestamp = fx_playback_audio_processor->timestamp;
  
  x_offset = fx_playback_audio_processor->x_offset;
  
  g_rec_mutex_unlock(fx_playback_audio_processor_mutex);

  /* time stamp offset */
  relative_offset = AGS_WAVE_DEFAULT_BUFFER_LENGTH * target_samplerate;

  ags_timestamp_set_ags_offset(timestamp,
			       (guint64) (relative_offset * floor((double) x_offset / (double) relative_offset)));

  attack = (x_offset % relative_offset) % target_buffer_size;

  frame_count = target_buffer_size - attack;

  create_wave = FALSE;
    
  if(x_offset + frame_count > relative_offset * floor(x_offset / relative_offset) + relative_offset){
    frame_count = relative_offset * floor((x_offset + frame_count) / relative_offset) - x_offset;

    create_wave = TRUE;
  }else if(x_offset + frame_count == relative_offset * floor(x_offset / relative_offset) + relative_offset){
    create_wave = TRUE;
  } 

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
				     2);
      current_audio_signal->stream_current = current_audio_signal->stream;
    
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

    target_copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(target_format),
							   ags_audio_buffer_util_format_from_soundcard(format));

    if(input_soundcard != NULL){
      stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(current_audio_signal);
    
      data = ags_soundcard_get_prev_buffer(AGS_SOUNDCARD(input_soundcard));
      
      g_rec_mutex_lock(stream_mutex);
      ags_soundcard_lock_buffer(AGS_SOUNDCARD(input_soundcard), data);

      ags_audio_buffer_util_clear_buffer(current_audio_signal->stream_current->data, 1,
					 target_buffer_size, ags_audio_buffer_util_format_from_soundcard(target_format));
    
      ags_audio_buffer_util_copy_buffer_to_buffer(current_audio_signal->stream_current->data, 1, 0,
						  data, audio_channels, input_soundcard_channel,
						  target_buffer_size, target_copy_mode);

      ags_soundcard_unlock_buffer(AGS_SOUNDCARD(input_soundcard), data);
      g_rec_mutex_unlock(stream_mutex);
    }
    
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

    x_buffer_offset_0 = x_offset - attack;

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
				   ags_timestamp_get_ags_offset(timestamp));
      
      ags_audio_add_wave(audio,
			 (GObject *) current_wave);

      g_object_unref(current_timestamp);
    }else{
      current_wave = wave->data;
    
      buffer = ags_wave_find_point(current_wave,
				   x_buffer_offset_0,
				   FALSE);
    }
  
    if(buffer == NULL){
      /* instantiate buffer */
      buffer = ags_buffer_new();
      buffer->x = x_buffer_offset_0;
      
      ags_wave_add_buffer(current_wave,
			  buffer,
			  FALSE);
    }else{
      found_buffer = TRUE;
    }

    /* record */
    data = NULL;

    target_copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(target_format),
							   ags_audio_buffer_util_format_from_soundcard(format));
    
    if(input_soundcard != NULL){
      buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);

      data = ags_soundcard_get_prev_buffer(AGS_SOUNDCARD(input_soundcard));
      
      g_rec_mutex_lock(buffer_mutex);
      ags_soundcard_lock_buffer(AGS_SOUNDCARD(input_soundcard), data);

      if(found_buffer &&
	 capture_mode == AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_REPLACE){
	void *data;

	data = buffer->data;

	switch(target_format){
	case AGS_SOUNDCARD_SIGNED_8_BIT:
	  data = ((gint8 *) data) + attack;
	  break;
	case AGS_SOUNDCARD_SIGNED_16_BIT:
	  data = ((gint16 *) data) + attack;
	  break;
	case AGS_SOUNDCARD_SIGNED_24_BIT:
	  data = ((gint32 *) data) + attack;
	  break;
	case AGS_SOUNDCARD_SIGNED_32_BIT:
	  data = ((gint32 *) data) + attack;
	  break;
	case AGS_SOUNDCARD_SIGNED_64_BIT:
	  data = ((gint64 *) data) + attack;
	  break;
	case AGS_SOUNDCARD_FLOAT:
	  data = ((gfloat *) data) + attack;
	  break;
	case AGS_SOUNDCARD_DOUBLE:
	  data = ((gdouble *) data) + attack;
	  break;
	case AGS_SOUNDCARD_COMPLEX:
	  data = ((AgsComplex *) data) + attack;
	  break;
	}
	
	ags_audio_buffer_util_clear_buffer(data, 1,
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
    }
    
    /* find wave - attempt #1 */
    if(attack != 0 ||
       frame_count != target_buffer_size){
      ags_timestamp_set_ags_offset(timestamp,
				   (guint64) (relative_offset * floor((double) (x_offset + frame_count) / (double) relative_offset)));

      wave = ags_wave_find_near_timestamp(start_wave, audio_channel,
					  timestamp);

      buffer = NULL;
  
      x_buffer_offset_1 = x_offset + frame_count;

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
				     ags_timestamp_get_ags_offset(timestamp));
      
	ags_audio_add_wave(audio,
			   (GObject *) current_wave);

	g_object_unref(current_timestamp);
      }else{
	current_wave = wave->data;
    
	buffer = ags_wave_find_point(current_wave,
				     x_buffer_offset_1,
				     FALSE);
      }
  
      if(buffer == NULL){
	/* instantiate buffer */
	buffer = ags_buffer_new();
	buffer->x = x_buffer_offset_1;
      
	ags_wave_add_buffer(current_wave,
			    buffer,
			    FALSE);
      }else{
	found_buffer = TRUE;
      }

      /* record */
      if(input_soundcard != NULL){
	buffer_mutex = AGS_BUFFER_GET_OBJ_MUTEX(buffer);
    
	g_rec_mutex_lock(buffer_mutex);
	ags_soundcard_lock_buffer(AGS_SOUNDCARD(input_soundcard), data);

	if(found_buffer &&
	   capture_mode == AGS_FX_PLAYBACK_AUDIO_CAPTURE_MODE_REPLACE){
	  ags_audio_buffer_util_clear_buffer(buffer->data, 1,
					     attack, ags_audio_buffer_util_format_from_soundcard(target_format));
	}
    
	ags_audio_buffer_util_copy_buffer_to_buffer(buffer->data, 1, 0,
						    data, audio_channels, (frame_count * audio_channels) + input_soundcard_channel,
						    attack, target_copy_mode);

	ags_soundcard_unlock_buffer(AGS_SOUNDCARD(input_soundcard), data);
	g_rec_mutex_unlock(buffer_mutex);
  
	/* data put */
	ags_fx_playback_audio_processor_data_put(fx_playback_audio_processor,
						 buffer,
						 AGS_FX_PLAYBACK_AUDIO_PROCESSOR_DATA_MODE_PLAY);
      }
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

  GObject *output_soundcard;

  gdouble delay;
  guint delay_counter;
  guint offset_counter;
  gboolean loop;
  guint64 loop_start, loop_end;
  guint buffer_size;
  
  GValue value = {0,};

  GRecMutex *fx_playback_audio_processor_mutex;
  
  fx_playback_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_playback_audio_processor);

  output_soundcard = NULL;

  fx_playback_audio = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  g_object_get(fx_playback_audio_processor,
	       "output-soundcard", &output_soundcard,
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

  g_rec_mutex_lock(fx_playback_audio_processor_mutex);

  delay_counter = floor(fx_playback_audio_processor->delay_counter);

  offset_counter = fx_playback_audio_processor->offset_counter;
  
  g_rec_mutex_unlock(fx_playback_audio_processor_mutex);  

  if(output_soundcard != NULL){
    delay = ags_soundcard_get_delay(AGS_SOUNDCARD(output_soundcard));

    delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(output_soundcard));
  }

  if(delay_counter + 1.0 >= floor(delay)){
    g_rec_mutex_lock(fx_playback_audio_processor_mutex);
    
    fx_playback_audio_processor->current_delay_counter = 0.0;

    if(loop &&
       offset_counter + 1 >= loop_end){
      fx_playback_audio_processor->current_offset_counter = loop_start;

      fx_playback_audio_processor->current_x_offset = (guint64) floor(loop_start * delay) * buffer_size;
    }else{
      fx_playback_audio_processor->current_offset_counter += 1;

      fx_playback_audio_processor->current_x_offset += buffer_size;
    }
    
    g_rec_mutex_unlock(fx_playback_audio_processor_mutex);
  }else{
    g_rec_mutex_lock(fx_playback_audio_processor_mutex);
    
    fx_playback_audio_processor->current_delay_counter += 1.0;

    fx_playback_audio_processor->current_x_offset += buffer_size;

    g_rec_mutex_unlock(fx_playback_audio_processor_mutex);
  }
  
  if(output_soundcard != NULL){
    g_object_unref(output_soundcard);
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
