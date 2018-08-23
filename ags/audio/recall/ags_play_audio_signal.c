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

#include <ags/audio/recall/ags_play_audio_signal.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_play_channel.h>
#include <ags/audio/recall/ags_play_channel_run.h>
#include <ags/audio/recall/ags_play_channel_run_master.h>
#include <ags/audio/recall/ags_play_recycling.h>

#include <stdlib.h>

void ags_play_audio_signal_class_init(AgsPlayAudioSignalClass *play_audio_signal);
void ags_play_audio_signal_init(AgsPlayAudioSignal *play_audio_signal);
void ags_play_audio_signal_finalize(GObject *gobject);

void ags_play_audio_signal_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_play_audio_signal
 * @short_description: plays audio signal
 * @title: AgsPlayAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_play_audio_signal.h
 *
 * The #AgsPlayAudioSignal class plays the audio signal.
 */

static gpointer ags_play_audio_signal_parent_class = NULL;

GType
ags_play_audio_signal_get_type()
{
  static GType ags_type_play_audio_signal = 0;

  if(!ags_type_play_audio_signal){
    static const GTypeInfo ags_play_audio_signal_info = {
      sizeof (AgsPlayAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_audio_signal_init,
    };

    ags_type_play_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							"AgsPlayAudioSignal",
							&ags_play_audio_signal_info,
							0);
  }

  return(ags_type_play_audio_signal);
}

void
ags_play_audio_signal_class_init(AgsPlayAudioSignalClass *play_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_play_audio_signal_parent_class = g_type_class_peek_parent(play_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) play_audio_signal;

  gobject->finalize = ags_play_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) play_audio_signal;

  recall->run_inter = ags_play_audio_signal_run_inter;
}

void
ags_play_audio_signal_init(AgsPlayAudioSignal *play_audio_signal)
{
  AGS_RECALL(play_audio_signal)->child_type = G_TYPE_NONE;
}

void
ags_play_audio_signal_finalize(GObject *gobject)
{  
  /* call parent */
  G_OBJECT_CLASS(ags_play_audio_signal_parent_class)->finalize(gobject);
}

void
ags_play_audio_signal_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsAudioSignal *rt_template;
  AgsPort *muted;
  AgsPlayChannel *play_channel;
  AgsPlayChannelRun *play_channel_run;
  AgsPlayRecycling *play_recycling;
  AgsPlayAudioSignal *play_audio_signal;

  GObject *output_soundcard;

  GList *note_start, *note;
  GList *stream;

  void *buffer_source;
  void *buffer0, *buffer1;
  
  gboolean current_muted;
  guint audio_channel, pcm_channels;
  guint samplerate, soundcard_samplerate;
  guint buffer_size, soundcard_buffer_size;
  guint format, soundcard_format;
  guint attack;
  guint copy_mode;
  gboolean resample;
  
  GValue muted_value = {0,};
  GValue audio_channel_value = {0,};

  void (*parent_class_run_inter)(AgsRecall *recall);  

  pthread_mutex_t *recall_mutex;

  play_audio_signal = AGS_PLAY_AUDIO_SIGNAL(recall);
  
  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  parent_class_run_inter = AGS_RECALL_CLASS(ags_play_audio_signal_parent_class)->run_inter;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  g_object_get(play_audio_signal,
	       "output-soundcard", &output_soundcard,
	       "source", &source,
	       NULL);

  if(output_soundcard == NULL){
    //    g_warning("no soundcard");
    
    return;
  }

  /* get presets */
  ags_soundcard_get_presets(AGS_SOUNDCARD(output_soundcard),
			    &pcm_channels,
			    &soundcard_samplerate,
			    &soundcard_buffer_size,
			    &soundcard_format);
  
  buffer0 = ags_soundcard_get_buffer(AGS_SOUNDCARD(output_soundcard));
  
  if(buffer0 == NULL){
    g_warning("no output buffer");
    
    return;
  }

  play_channel = NULL;
  play_channel_run = NULL;
  play_recycling = NULL;

  g_object_get(play_audio_signal,
	       "parent", &play_recycling,
	       NULL);

  if(play_recycling != NULL){
    g_object_get(play_recycling,
		 "parent", &play_channel_run,
		 NULL);
  }
  
  if(play_recycling == NULL ||
     play_channel_run == NULL){
    g_object_get(play_audio_signal,
		 "output-soundcard-channel", &audio_channel,
		 NULL);
  }else{
    g_object_get(play_channel_run,
		 "recall-channel", &play_channel,
		 NULL);

    g_object_get(play_channel,
		 "muted", &muted,
		 NULL);
    
    g_value_init(&muted_value,
		 G_TYPE_FLOAT);
    
    ags_port_safe_read(muted,
		       &muted_value);

    current_muted = (g_value_get_float(&muted_value) == 1.0) ? TRUE: FALSE;
    g_value_unset(&muted_value);

    if(current_muted){
      return;
    }

    g_value_init(&audio_channel_value,
		 G_TYPE_UINT64);
    
    ags_port_safe_read(play_channel->audio_channel,
		       &audio_channel_value);

    audio_channel = g_value_get_uint64(&audio_channel_value);
    g_value_unset(&audio_channel_value);
  }

  buffer_size = source->buffer_size;
  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(soundcard_format),
						  ags_audio_buffer_util_format_from_soundcard(source->format));

  /* get template */    
  g_object_get(source,
	       "rt-template", &rt_template,
	       "note", &note_start,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       "attack", &attack,
	       NULL);
  
  if(samplerate != soundcard_samplerate){
    attack = soundcard_samplerate * (attack / samplerate);
  }
  
  if(rt_template != NULL){
    note = note_start;
    
    while(note != NULL){
      AgsNote *current;
      
      guint rt_attack;
      guint64 rt_offset;
      
      current = note->data;

      g_object_get(current,
		   "rt-attack", &rt_attack,
		   "rt-offset", &rt_offset,
		   NULL);

      stream = source->stream;
      
      if(stream == NULL){
	note = note->next;

	continue;
      }

      /* check if resample */
      buffer_source = stream->data;
      resample = FALSE;

      if(samplerate != soundcard_samplerate){
	buffer_source = ags_audio_buffer_util_resample(buffer_source, 1,
						       ags_audio_buffer_util_format_from_soundcard(format), samplerate,
						       buffer_size,
						       soundcard_samplerate);
      
	resample = TRUE;
      }

      /* copy */
      ags_soundcard_lock_buffer(AGS_SOUNDCARD(output_soundcard),
				buffer0);
      
      ags_audio_buffer_util_copy_buffer_to_buffer(buffer0, pcm_channels, audio_channel,
						  buffer_source, 1, 0,
						  buffer_size, copy_mode);	  

      ags_soundcard_unlock_buffer(AGS_SOUNDCARD(output_soundcard),
				  buffer0);
  
      if(resample){
	free(buffer_source);
      }
      
      note = note->next;
    }

    g_list_free(note_start);
  }else{
    stream = source->stream_current;

    g_list_free(note_start);
        
    if(stream == NULL){
      AgsRecycling *recycling;
      
      ags_recall_done(recall);

      if(play_recycling != NULL){
	g_object_get(play_recycling,
		     "source", &recycling,
		     NULL);
	ags_recycling_remove_audio_signal(recycling,
					  source);
	g_object_unref(source);
      }
      
      return;
    }
    
    /* check if resample */
    buffer_source = stream->data;
    resample = FALSE;
    
    if(samplerate != soundcard_samplerate){
      buffer_source = ags_audio_buffer_util_resample(buffer_source, 1,
						     ags_audio_buffer_util_format_from_soundcard(format), samplerate,
						     buffer_size,
						     soundcard_samplerate);
      
      resample = TRUE;
    }
    
    if(ags_recall_test_flags(recall, AGS_RECALL_INITIAL_RUN)){
      /* copy */
      ags_soundcard_lock_buffer(AGS_SOUNDCARD(output_soundcard),
				buffer0);
      
      ags_audio_buffer_util_copy_buffer_to_buffer(buffer0, pcm_channels, audio_channel + attack * pcm_channels,
						  buffer_source, 1, 0,
						  soundcard_buffer_size - attack, copy_mode);

      ags_soundcard_unlock_buffer(AGS_SOUNDCARD(output_soundcard),
				  buffer0);
    }else{
      if(source->attack != 0 && stream->prev != NULL){
	void *buffer_source_prev;
	
	buffer_source_prev = stream->prev->data;

	if(resample){
	  buffer_source_prev = ags_audio_buffer_util_resample(buffer_source_prev, 1,
							      ags_audio_buffer_util_format_from_soundcard(format), samplerate,
							      buffer_size,
							      soundcard_samplerate);

	}
	
	/* copy */
	ags_soundcard_lock_buffer(AGS_SOUNDCARD(output_soundcard),
				  buffer0);
	
	ags_audio_buffer_util_copy_buffer_to_buffer(buffer0, pcm_channels, audio_channel,
						    buffer_source_prev, 1, soundcard_buffer_size - source->attack,
						    attack, copy_mode);

	ags_soundcard_unlock_buffer(AGS_SOUNDCARD(output_soundcard),
				    buffer0);
	
	if(resample){
	  free(buffer_source_prev);
	}
      }
      
      /* copy */
      ags_soundcard_lock_buffer(AGS_SOUNDCARD(output_soundcard),
				buffer0);
      
      ags_audio_buffer_util_copy_buffer_to_buffer(buffer0, pcm_channels, audio_channel + attack * pcm_channels,
						  buffer_source, 1, 0,
						  soundcard_buffer_size - attack, copy_mode);

      ags_soundcard_unlock_buffer(AGS_SOUNDCARD(output_soundcard),
				  buffer0);
    }
  
    if(resample){
      free(buffer_source);
    }
  }
  
  /* call parent */
  parent_class_run_inter(recall);
}

/**
 * ags_play_audio_signal_new:
 * @source: the #AgsAudioSignal
 *
 * Create a new instance of #AgsPlayAudioSignal
 *
 * Returns: the new #AgsPlayAudioSignal
 *
 * Since: 2.0.0
 */
AgsPlayAudioSignal*
ags_play_audio_signal_new(AgsAudioSignal *source)
{
  AgsPlayAudioSignal *play_audio_signal;

  play_audio_signal = (AgsPlayAudioSignal *) g_object_new(AGS_TYPE_PLAY_AUDIO_SIGNAL,
							  "source", source,
							  NULL);

  return(play_audio_signal);
}
