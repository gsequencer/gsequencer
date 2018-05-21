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

#include <ags/audio/recall/ags_buffer_audio_signal.h>
#include <ags/audio/recall/ags_buffer_recycling.h>
#include <ags/audio/recall/ags_buffer_channel.h>

#include <ags/libags.h>

#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_recall_channel.h>
#include <ags/audio/ags_recall_channel_run.h>
#include <ags/audio/ags_recall_recycling.h>
#include <ags/audio/ags_audio_buffer_util.h>
 
void ags_buffer_audio_signal_class_init(AgsBufferAudioSignalClass *buffer_audio_signal);
void ags_buffer_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_buffer_audio_signal_init(AgsBufferAudioSignal *buffer_audio_signal);
void ags_buffer_audio_signal_finalize(GObject *gobject);

void ags_buffer_audio_signal_run_init_pre(AgsRecall *recall);
void ags_buffer_audio_signal_run_inter(AgsRecall *recall);
AgsRecall* ags_buffer_audio_signal_duplicate(AgsRecall *recall,
					     AgsRecallID *recall_id,
					     guint *n_params, gchar **parameter_name, GValue *value);

/**
 * SECTION:ags_buffer_audio_signal
 * @short_description: buffer audio signal
 * @title: AgsBufferAudioSignal
 * @section_id:
 * @include: ags/audio/recall/ags_buffer_audio_signal.h
 *
 * The #AgsBufferAudioSignal class buffers the audio signal.
 */

static gpointer ags_buffer_audio_signal_parent_class = NULL;
static AgsConnectableInterface *ags_buffer_audio_signal_parent_connectable_interface;

GType
ags_buffer_audio_signal_get_type()
{
  static GType ags_type_buffer_audio_signal = 0;

  if(!ags_type_buffer_audio_signal){
    static const GTypeInfo ags_buffer_audio_signal_info = {
      sizeof(AgsBufferAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_buffer_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsBufferAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_buffer_audio_signal_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_buffer_audio_signal_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_buffer_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							  "AgsBufferAudioSignal",
							  &ags_buffer_audio_signal_info,
							  0);

    g_type_add_interface_static(ags_type_buffer_audio_signal,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_buffer_audio_signal);
}

void
ags_buffer_audio_signal_class_init(AgsBufferAudioSignalClass *buffer_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_buffer_audio_signal_parent_class = g_type_class_peek_parent(buffer_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) buffer_audio_signal;

  gobject->finalize = ags_buffer_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) buffer_audio_signal;

  recall->run_init_pre = ags_buffer_audio_signal_run_init_pre;
  recall->run_inter = ags_buffer_audio_signal_run_inter;
  recall->duplicate = ags_buffer_audio_signal_duplicate;
}

void
ags_buffer_audio_signal_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_buffer_audio_signal_parent_connectable_interface = g_type_interface_peek_parent(connectable);
}

void
ags_buffer_audio_signal_init(AgsBufferAudioSignal *buffer_audio_signal)
{
  AGS_RECALL(buffer_audio_signal)->child_type = G_TYPE_NONE;
}

void
ags_buffer_audio_signal_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_buffer_audio_signal_parent_class)->finalize(gobject); 
}

void
ags_buffer_audio_signal_run_init_pre(AgsRecall *recall)
{
  AgsChannel *output;
  AgsRecycling *recycling;
  AgsAudioSignal *destination;
  AgsRecallID *recall_id;
  AgsRecallID *parent_recall_id;
  AgsRecyclingContext *recycling_context;
  AgsRecyclingContext *parent_recycling_context;
  
  AgsBufferChannelRun *buffer_channel_run;
  AgsBufferRecycling *buffer_recycling;
  AgsBufferAudioSignal *buffer_audio_signal;

  GObject *output_soundcard;

  GList *list_start, *list;
  GList *stream;

  gdouble delay;
  guint attack;
  guint length;

  buffer_audio_signal = AGS_BUFFER_AUDIO_SIGNAL(recall);

  g_object_get(buffer_audio_signal,
	       "output-soundcard", &output_soundcard,
	       NULL);

  g_object_get(buffer_audio_signal,
	       "parent", &buffer_recycling,
	       NULL);

  g_object_get(buffer_recycling,
	       "parent", &buffer_channel_run,
	       NULL);
  
  g_object_get(buffer_channel_run,
	       "destination", &output,
	       NULL);
  
  /* recycling */
  ags_recall_unset_behaviour_flags(recall, AGS_RECALL_PERSISTENT);

  g_object_get(buffer_recycling,
	       "destination", &recycling,
	       NULL);

  /* recall id */
  g_object_get(recall,
	       "recall-id", &recall_id,
	       NULL);
  
  /* recycling context */
  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  /* parent recycling context */
  g_object_get(recycling_context,
	       "parent", &parent_recycling_context,
	       NULL);

  /**/
  g_object_get(output,
	       "recall-id", &list_start,
	       NULL);

  list = list_start;
  parent_recall_id = ags_recall_id_find_recycling_context(list,
							  parent_recycling_context);

  g_list_free(list_start);
  
  //TODO:JK: unclear
  attack = 0;
  delay = 0.0;

  /* create new audio signal */
  destination = ags_audio_signal_new((GObject *) output_soundcard,
				     (GObject *) recycling,
				     (GObject *) parent_recall_id);
  
  g_object_set(buffer_audio_signal,
	       "destination", destination,
	       NULL);  
  ags_recycling_create_audio_signal_with_defaults(recycling,
						  destination,
						  delay, attack);
  length = 3; // (guint) (2.0 * soundcard->delay[soundcard->tic_counter]) + 1;
  ags_audio_signal_stream_resize(destination,
				 length);

  ags_connectable_connect(AGS_CONNECTABLE(destination));
  
  destination->stream = destination->stream_beginning;

  ags_recycling_add_audio_signal(recycling,
				 destination);

#ifdef AGS_DEBUG
  g_message("buffer %x to %x", destination, parent_recall_id);
  g_message("creating destination");
#endif
  
  /* call parent */
  AGS_RECALL_CLASS(ags_buffer_audio_signal_parent_class)->run_init_pre(recall);
}

void
ags_buffer_audio_signal_run_inter(AgsRecall *recall)
{
  AgsRecycling *recycling;
  AgsAudioSignal *source, *destination;
  AgsPort *muted;

  AgsBufferChannel *buffer_channel;
  AgsBufferChannelRun *buffer_channel_run;
  AgsBufferRecycling *buffer_recycling;
  AgsBufferAudioSignal *buffer_audio_signal;
  
  GList *stream_source, *stream_destination;

  guint source_buffer_size, destination_buffer_size;
  guint source_samplerate, destination_samplerate;
  guint source_format, destination_format;
  guint copy_mode;
  guint attack;
  gboolean is_muted;
  
  GValue value = {0,};

  /* call parent */
  AGS_RECALL_CLASS(ags_buffer_audio_signal_parent_class)->run_inter(recall);

  /* initialize some variables */
  buffer_audio_signal = AGS_BUFFER_AUDIO_SIGNAL(recall);

  g_object_get(recall,
	       "parent", &buffer_recycling,
	       NULL);

  g_object_get(buffer_recycling,
	       "parent", &buffer_channel_run,
	       NULL);

  g_object_get(buffer_channel,
	       "parent", &buffer_channel,
	       NULL);

  g_object_get(buffer_audio_signal,
	       "source", &source,
	       "destination", &destination,
	       NULL);

  stream_source = source->stream_current;

  if(stream_source == NULL){
    //    if((AGS_RECALL_PERSISTENT & (recall->flags)) == 0){
      ags_recall_done(recall);
      //    }

    return;
  }

  g_object_get(buffer_channel,
	       "muted", &muted,
	       NULL);
  
  //FIXME:JK: attack probably needs to be removed
  g_value_init(&value, G_TYPE_FLOAT);
  ags_port_safe_read(muted,
		     &value);

  is_muted = (g_value_get_float(&value) == 0.0) ? TRUE: FALSE;  
  g_value_unset(&value);

  if(is_muted){
    return;
  }

  stream_destination = destination->stream_current;

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
  
  if(stream_destination != NULL){
    void *buffer_source;

    gboolean resample;
    
    copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(destination_format),
						    ags_audio_buffer_util_format_from_soundcard(source_format));
    resample = FALSE;
    
    if(stream_destination->next == NULL){
      ags_audio_signal_add_stream(destination);
    }

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
    
    /* copy */
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

      ags_audio_buffer_util_copy_buffer_to_buffer(stream_destination->data, 1, attack,
						  buffer_source, 1, 0,
						  destination_buffer_size - attack, copy_mode);
    }

    if(resample){
      free(buffer_source);
    }
  }
}

AgsRecall*
ags_buffer_audio_signal_duplicate(AgsRecall *recall,
				  AgsRecallID *recall_id,
				  guint *n_params, gchar **parameter_name, GValue *value)
{
  AgsBufferAudioSignal *buffer;

  buffer = (AgsBufferAudioSignal *) AGS_RECALL_CLASS(ags_buffer_audio_signal_parent_class)->duplicate(recall,
												      recall_id,
												      n_params, parameter_name, value);
  
  return((AgsRecall *) buffer);
}

/**
 * ags_buffer_audio_signal_new:
 * @destination: the destination #AgsAudioSignal
 * @source: the source #AgsAudioSignal
 *
 * Create a new instance of #AgsBufferAudioSignal
 *
 * Returns: the new #AgsBufferAudioSignal
 *
 * Since: 2.0.0
 */
AgsBufferAudioSignal*
ags_buffer_audio_signal_new(AgsAudioSignal *destination,
			    AgsAudioSignal *source)
{
  AgsBufferAudioSignal *buffer_audio_signal;

  buffer_audio_signal = (AgsBufferAudioSignal *) g_object_new(AGS_TYPE_BUFFER_AUDIO_SIGNAL,
							      "destination", destination,
							      "source", source,
							      NULL);

  return(buffer_audio_signal);
}
