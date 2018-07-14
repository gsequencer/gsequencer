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

#include <ags/audio/recall/ags_copy_audio_signal.h>

#include <ags/libags.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/recall/ags_copy_channel.h>
#include <ags/audio/recall/ags_copy_recycling.h>

#include <stdlib.h>

void ags_copy_audio_signal_class_init(AgsCopyAudioSignalClass *copy_audio_signal);
void ags_copy_audio_signal_init(AgsCopyAudioSignal *copy_audio_signal);
void ags_copy_audio_signal_finalize(GObject *gobject);

void ags_copy_audio_signal_run_init_pre(AgsRecall *recall);
void ags_copy_audio_signal_run_pre(AgsRecall *recall);
void ags_copy_audio_signal_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_copy_audio_signal
 * @short_description: copy audio signal
 * @title: AgsCopyAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_copy_audio_signal.h
 *
 * The #AgsCopyAudioSignal class copies the audio signal.
 */

static gpointer ags_copy_audio_signal_parent_class = NULL;

GType
ags_copy_audio_signal_get_type()
{
  static GType ags_type_copy_audio_signal = 0;

  if(!ags_type_copy_audio_signal){
    static const GTypeInfo ags_copy_audio_signal_info = {
      sizeof(AgsCopyAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCopyAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_audio_signal_init,
    };

    ags_type_copy_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							"AgsCopyAudioSignal",
							&ags_copy_audio_signal_info,
							0);
  }

  return(ags_type_copy_audio_signal);
}

void
ags_copy_audio_signal_class_init(AgsCopyAudioSignalClass *copy_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_copy_audio_signal_parent_class = g_type_class_peek_parent(copy_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_audio_signal;

  gobject->finalize = ags_copy_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) copy_audio_signal;

  recall->run_init_pre = ags_copy_audio_signal_run_init_pre;
  recall->run_pre = ags_copy_audio_signal_run_pre;
  recall->run_inter = ags_copy_audio_signal_run_inter;  
}

void
ags_copy_audio_signal_init(AgsCopyAudioSignal *copy_audio_signal)
{
  AGS_RECALL(copy_audio_signal)->name = "ags-copy";
  AGS_RECALL(copy_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(copy_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(copy_audio_signal)->xml_type = "ags-copy-audio-signal";
  AGS_RECALL(copy_audio_signal)->port = NULL;

  AGS_RECALL(copy_audio_signal)->child_type = G_TYPE_NONE;
}

void
ags_copy_audio_signal_finalize(GObject *gobject)
{  
  /* call parent */
  G_OBJECT_CLASS(ags_copy_audio_signal_parent_class)->finalize(gobject);
}

void
ags_copy_audio_signal_run_init_pre(AgsRecall *recall)
{
  AgsChannel *destination_channel;
  AgsRecycling *destination_recycling;
  AgsAudioSignal *destination;
  AgsRecallID *parent_recall_id;
  AgsRecallID *recall_id;
  AgsRecyclingContext *parent_recycling_context;
  AgsRecyclingContext *recycling_context;
  AgsCopyChannelRun *copy_channel_run;
  AgsCopyRecycling *copy_recycling;
  AgsCopyAudioSignal *copy_audio_signal;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *stream;

  gdouble delay;
  guint attack;
  guint length;
  
  void (*parent_class_run_init_pre)(AgsRecall *recall);
  
  pthread_mutex_t *recall_mutex;
  pthread_mutex_t *recycling_mutex;
  
  copy_audio_signal = AGS_COPY_AUDIO_SIGNAL(recall);

  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  parent_class_run_init_pre = AGS_RECALL_CLASS(ags_copy_audio_signal_parent_class)->run_init_pre;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* set flags */
  ags_recall_unset_behaviour_flags(recall, AGS_RECALL_PERSISTENT);

  /* get some fields */
  g_object_get(copy_audio_signal,
	       "parent", &copy_recycling,
	       "output-soundcard", &output_soundcard,
	       "recall-id", &recall_id,
	       NULL);

  g_object_get(copy_recycling,
	       "parent", &copy_channel_run,
	       NULL);

  /* channel */
  g_object_get(copy_channel_run,
	       "destination", &destination_channel,
	       NULL);
  
  /* recycling */
  g_object_get(copy_recycling,
	       "destination", &destination_recycling,
	       NULL);

  /* get recycling context */
  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  g_object_get(destination_channel,
	       "recall-id", &list_start,
	       NULL);
  
  parent_recall_id = ags_recall_id_find_recycling_context(list_start,
							  parent_recycling_context);

  g_list_free(list_start);
  
  //TODO:JK: unclear
  attack = 0;
  delay = 0.0;

  /* create new audio signal */
  destination = ags_audio_signal_new((GObject *) output_soundcard,
				     (GObject *) recycling,
				     (GObject *) parent_recall_id);
  
  g_object_set(copy_audio_signal,
	       "destination", destination,
	       NULL);  
  ags_recycling_create_audio_signal_with_defaults(recycling,
						  destination,
						  delay, attack);
  length = 1; // (guint) (2.0 * soundcard->delay[soundcard->tic_counter]) + 1;
  ags_audio_signal_stream_resize(destination,
				 length);

  ags_connectable_connect(AGS_CONNECTABLE(destination));
  
  destination->stream = destination->stream_beginning;

  ags_recycling_add_audio_signal(destination_recycling,
				 destination);

#ifdef AGS_DEBUG
  g_message("copy %x to %x", destination, parent_recall_id);
  g_message("creating destination");
#endif
  
  /* call parent */
  parent_class_run_init_pre(recall);
}

void
ags_copy_audio_signal_run_pre(AgsRecall *recall)
{
  AgsAudioSignal *destination, *source;
  AgsCopyAudioSignal *copy_audio_signal;
  
  void *buffer;

  guint buffer_size;
  guint format;

  void (*parent_class_run_pre)(AgsRecall *recall);
  
  pthread_mutex_t *recall_mutex;

  copy_audio_signal = AGS_COPY_AUDIO_SIGNAL(recall);

  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  parent_class_run_pre = AGS_RECALL_CLASS(ags_copy_audio_signal_parent_class)->run_pre;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* call parent */
  parent_class_run_pre(recall);

  g_object_get(recall,
	       "source", &source,
	       NULL);
  
  if(source->stream != NULL){
    g_object_get(recall,
		 "destination", &destination,
		 NULL);
    
    buffer = destination->stream->data;

    g_object_get(destination,
		 "buffer-size", &buffer_size,
		 "format", &format,
		 NULL);

    ags_audio_buffer_util_clear_buffer(buffer, 1,
				       buffer_size, ags_audio_buffer_util_format_from_soundcard(format));
  }
}

void
ags_copy_audio_signal_run_inter(AgsRecall *recall)
{
  AgsRecycling *destination_recycling, *source_recycling;
  AgsAudioSignal *destination, *source;
  AgsPort *port;
  AgsRecallID *recall_id;
  AgsRecyclingContext *parent_recycling_context;
  AgsRecyclingContext *recycling_context;

  AgsCopyChannel *copy_channel;
  AgsCopyChannelRun *copy_channel_run;
  AgsCopyRecycling *copy_recycling;
  AgsCopyAudioSignal *copy_audio_signal;

  GList *note_start;
  GList *stream_source, *stream_destination;

  void *buffer_source;
  gchar *str;
  
  guint destination_buffer_size, source_buffer_size;
  guint destination_samplerate, source_samplerate;
  guint destination_format, source_format;
  guint attack; 
  guint copy_mode;
  gboolean is_muted;
  gboolean resample;
  
  GValue value = {0,};

  void (*parent_class_run_inter)(AgsRecall *recall);
  
  pthread_mutex_t *recall_mutex;

  copy_audio_signal = AGS_COPY_AUDIO_SIGNAL(recall);

  /* get mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());

  recall_mutex = recall->obj_mutex;
  
  parent_class_run_inter = AGS_RECALL_CLASS(ags_copy_audio_signal_parent_class)->run_inter;

  pthread_mutex_unlock(ags_recall_get_class_mutex());

  /* call parent */
  parent_class_run_inter(recall);

  /* get source and recall id */
  g_object_get(recall,
	       "source", &source,
	       "recall-id", &recall_id,
	       NULL);

  /* get recycling context and its parent */
  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  /* get notes */
  g_object_get(source,
	       "note", &note_start,
	       NULL);
  
  if(ags_recall_global_get_rt_safe() &&
     parent_recycling_context != NULL &&
     note_start == NULL){
    return;
  }

  g_list_free(note_start);

  /* get destination */
  g_object_get(recall,
	       "destination", &destination,
	       NULL);
  
  /* get stream */
  stream_source = source->stream;
  
  if(stream_source == NULL){
    g_object_get(source,
		 "recycling", &source_recycling,
		 NULL);
    
    if(destination != NULL){
      g_object_get(destination,
		   "recycling", &destination_recycling,
		   NULL);
      
      ags_recycling_remove_audio_signal(destination_recycling,
					destination);
      
      g_object_run_dispose(destination);
      g_object_unref(destination);
    }

    ags_recall_done(recall);

    ags_recycling_remove_audio_signal(source_recycling,
				      source);
    g_object_unref(source);
    
    return;
  }

  if(destination == NULL){
    g_warning("no destination");
    
    return;
  }

  /* get related recalls */
  g_object_get(recall,
	       "parent", &copy_recycling,
	       NULL);

  g_object_get(copy_recycling,
	       "parent", &copy_channel_run,
	       NULL);

  g_object_get(copy_channel_run,
	       "recall-channel", &copy_channel_run,
	       NULL);
  
  /* check muted */
  g_object_get(copy_channel,
	       "muted", &port,
	       NULL);
  
  g_value_init(&value, G_TYPE_FLOAT);
  ags_port_safe_read(port,
		     &value);

  is_muted = (g_value_get_float(&value) == 0.0) ? TRUE: FALSE;  
  g_value_unset(&value);

  if(is_muted){
    return;
  }

  stream_destination = destination->stream;

  if(!ags_recall_global_get_rt_safe() &&
     stream_destination->next == NULL){
    ags_audio_signal_add_stream(destination);
  }


  g_object_get(destination,
	       "buffer-size", &destination_buffer_size,
	       "samplerate", &destination_samplerate,
	       "format", &destination_format,
	       NULL);

  g_object_get(source,
	       "buffer-size", &source_buffer_size,
	       "attack", &attack,
	       "samplerate", &source_samplerate,
	       "format", &source_format,
	       NULL);
  
  copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(destination_format),
						  ags_audio_buffer_util_format_from_soundcard(source_format));
  
  resample = FALSE;

  /* check if resample */
  buffer_source = stream_source->data;
  attack = (destination_samplerate / source_samplerate) * attack;

  if(source_samplerate != destination_samplerate){
    buffer_source = ags_audio_buffer_util_resample(buffer_source, 1,
						   ags_audio_buffer_util_format_from_soundcard(source_format), source_samplerate,
						   source_buffer_size,
						   destination_samplerate);
      
    resample = TRUE;
  }

  if(ags_recall_test_flags(recall, AGS_RECALL_INITIAL_RUN)){
    ags_audio_buffer_util_copy_buffer_to_buffer(stream_destination->data, 1, attack,
						buffer_source, 1, 0,
						destination_buffer_size - attack, copy_mode);

    ags_recall_unset_flags(recall, AGS_RECALL_INITIAL_RUN);
  }else{
    if(attack != 0 && stream_source->prev != NULL){
      void *buffer_source_prev;
	
      buffer_source_prev = stream_source->prev->data;

      if(resample){
	buffer_source_prev = ags_audio_buffer_util_resample(buffer_source_prev, 1,
							    ags_audio_buffer_util_format_from_soundcard(source_format), source_samplerate,
							    source_buffer_size,
							    destination_samplerate);

      }

      ags_audio_buffer_util_copy_buffer_to_buffer(stream_destination->data, 1, 0,
						  buffer_source_prev, 1, destination_buffer_size - attack,
						  attack, copy_mode);

      if(resample){
	free(buffer_source_prev);
      }
    }

    ags_audio_buffer_util_copy_buffer_to_buffer(stream_destination->data, 1, source->attack,
						buffer_source, 1, 0,
						destination_buffer_size - attack, copy_mode);
  }

  if(resample){
    free(buffer_source);
  }
}

/**
 * ags_copy_audio_signal_new:
 * @destination: the destination #AgsAudioSignal
 * @source: the source #AgsAudioSignal
 *
 * Create a new instance of #AgsCopyAudioSignal
 *
 * Returns: the new #AgsCopyAudioSignal
 *
 * Since: 2.0.0
 */
AgsCopyAudioSignal*
ags_copy_audio_signal_new(AgsAudioSignal *destination,
			  AgsAudioSignal *source)
{
  AgsCopyAudioSignal *copy_audio_signal;

  copy_audio_signal = (AgsCopyAudioSignal *) g_object_new(AGS_TYPE_COPY_AUDIO_SIGNAL,
							  "destination", destination,
							  "source", source,
							  NULL);

  return(copy_audio_signal);
}
