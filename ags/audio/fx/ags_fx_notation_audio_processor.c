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

#include <ags/audio/fx/ags_fx_notation_audio_processor.h>

void ags_fx_notation_audio_processor_class_init(AgsFxNotationAudioProcessorClass *fx_notation_audio_processor);
void ags_fx_notation_audio_processor_init(AgsFxNotationAudioProcessor *fx_notation_audio_processor);
void ags_fx_notation_audio_processor_dispose(GObject *gobject);
void ags_fx_notation_audio_processor_finalize(GObject *gobject);

void ags_fx_notation_audio_processor_real_key_on(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
						 AgsNote *note,
						 guint velocity);
void ags_fx_notation_audio_processor_real_key_off(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
						  AgsNote *note,
						  guint velocity);
void ags_fx_notation_audio_processor_real_key_pressure(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
						       AgsNote *note,
						       guint velocity);

void ags_fx_notation_audio_processor_real_play(AgsFxNotationAudioProcessor *fx_notation_audio_processor);
void ags_fx_notation_audio_processor_real_record(AgsFxNotationAudioProcessor *fx_notation_audio_processor);
void ags_fx_notation_audio_processor_real_feed(AgsFxNotationAudioProcessor *fx_notation_audio_processor);

void ags_fx_notation_audio_processor_notify_output_soundcard_callback(GObject *gobject,
								      GParamSpec *pspec,
								      gpointer user_data);

/**
 * SECTION:ags_fx_notation_audio_processor
 * @short_description: fx notation audio_processor
 * @title: AgsFxNotationAudioProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_notation_audio_processor.h
 *
 * The #AgsFxNotationAudioProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_notation_audio_processor_parent_class = NULL;

GType
ags_fx_notation_audio_processor_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_notation_audio_processor = 0;

    static const GTypeInfo ags_fx_notation_audio_processor_info = {
      sizeof (AgsFxNotationAudioProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_notation_audio_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_processor */
      sizeof (AgsFxNotationAudioProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_notation_audio_processor_init,
    };

    ags_type_fx_notation_audio_processor = g_type_register_static(AGS_TYPE_RECALL_AUDIO_PROCESSOR,
								  "AgsFxNotationAudioProcessor",
								  &ags_fx_notation_audio_processor_info,
								  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_notation_audio_processor);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_notation_audio_processor_class_init(AgsFxNotationAudioProcessorClass *fx_notation_audio_processor)
{
  GObjectClass *gobject;

  ags_fx_notation_audio_processor_parent_class = g_type_class_peek_parent(fx_notation_audio_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_notation_audio_processor;

  gobject->dispose = ags_fx_notation_audio_processor_dispose;
  gobject->finalize = ags_fx_notation_audio_processor_finalize;

  /* AgsFxNotationAudioProcessorClass */
  fx_notation_audio_processor->key_on = ags_fx_notation_audio_processor_real_key_on;
  fx_notation_audio_processor->key_off = ags_fx_notation_audio_processor_real_key_off;
  fx_notation_audio_processor->key_pressure = ags_fx_notation_audio_processor_real_key_pressure;

  fx_notation_audio_processor->play = ags_fx_notation_audio_processor_real_play;
  fx_notation_audio_processor->record = ags_fx_notation_audio_processor_real_record;
  fx_notation_audio_processor->feed = ags_fx_notation_audio_processor_real_feed;
}

void
ags_fx_notation_audio_processor_init(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  g_signal_connect(fx_notation_audio_processor, "notify::output-soundcard",
		   G_CALLBACK(ags_fx_notation_audio_processor_notify_output_soundcard_callback), NULL);
  
  fx_notation_audio_processor->delay_counter = 0.0;
  fx_notation_audio_processor->offset_counter = 0;

  fx_notation_audio_processor->timestamp = ags_timestamp_new();
  g_object_ref(fx_notation_audio_processor->timestamp);
  
  fx_notation_audio_processor->timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  fx_notation_audio_processor->timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  fx_notation_audio_processor->timestamp->timer.ags_offset.offset = 0;
}

void
ags_fx_notation_audio_processor_dispose(GObject *gobject)
{
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  
  fx_notation_audio_processor = AGS_FX_NOTATION_AUDIO_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_notation_audio_processor_parent_class)->dispose(gobject);
}

void
ags_fx_notation_audio_processor_finalize(GObject *gobject)
{
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  
  fx_notation_audio_processor = AGS_FX_NOTATION_AUDIO_PROCESSOR(gobject);  

  /* timestamp */
  if(fx_notation_audio_processor->timestamp != NULL){
    g_object_unref((GObject *) fx_notation_audio_processor->timestamp);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_notation_audio_processor_parent_class)->finalize(gobject);
}

void
ags_fx_notation_audio_processor_real_key_on(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
					    AgsNote *note,
					    guint velocity)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *input, *selected_input;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context;
  AgsFxNotationAudio *fx_notation_audio;
  AgsPort *port;

  gdouble delay;
  guint64 offset_counter;
  guint input_pads;
  guint audio_channel;
  guint y;
  guint i;

  GValue value = {0,};

  GRecMutex *fx_notation_audio_processor_mutex;

  fx_notation_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio_processor);
  
  g_object_get(fx_notation_audio_processor,
	       "audio", &audio,
	       "input", &start_input,
	       "recall-id", &recall_id,
	       "recall-audio", &fx_notation_audio,
	       "audio-channel", &audio_channel,
	       NULL);
  
  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  g_rec_mutex_lock(fx_notation_audio_processor_mutex);
    
  offset_counter = fx_notation_audio_processor->offset_counter;

  g_rec_mutex_unlock(fx_notation_audio_processor_mutex);

  /* get delay */
  delay = AGS_SOUNDCARD_DEFAULT_DELAY;
  offset_counter = 0;
  
  fx_notation_audio_mutex = NULL;

  if(fx_notation_audio != NULL){        
    g_object_get(fx_notation_audio,
		 "delay", &port,
		 NULL);

    if(port != NUll){
      g_value_init(&value,
		   G_TYPE_DOUBLE);
    
      ags_port_safe_read(port,
			 &value);

      delay = g_value_get_double(&value);
      g_value_unset(&value);

      g_object_unref(port);
    }
  }

  g_object_get(audio,
	       "input-pads", &input_pads,
	       NULL);
  
  g_object_get(note,
	       "y", &y,
	       NULL);
  
  input = ags_channel_nth(start_input,
			  audio_channel);

  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
    selected_input = ags_channel_pad_nth(input,
					 input_pads - y - 1);
  }else{
    selected_input = ags_channel_pad_nth(input,
					 y);
  }
  
  if(selected_input != NULL){
    AgsRecycling *first_recycling, *last_recycling;
    AgsRecycling *recycling, *next_recycling;
    AgsRecycling *end_recycling;
    AgsRecallID *child_recall_id;
    
    GObject *output_soundcard;

    GList *start_list, *list;

    g_object_get(selected_input,
		 "output-soundcard", &output_soundcard,
		 "first-recycling", &first_recycling,
		 "last-recycling", &last_recycling,
		 NULL);

    end_recycling = ags_recycling_next(last_recycling);

    /* get child recall id */
    g_object_get(selected_channel,
		 "recall-id", &start_list,
		 NULL);

    list = start_list;
    child_recall_id = NULL;

    while(child_recall_id == NULL &&
	  list != NULL){
      AgsRecallID *current_recall_id;
      AgsRecyclingContext *current_recycling_context, *current_parent_recycling_context;

      g_object_get(list->data,
		   "recycling-context", &current_recycling_context,
		   NULL);

      g_object_get(current_recycling_context,
		   "parent", &current_parent_recycling_context,
		   NULL);
	  
      if(current_parent_recycling_context == recycling_context){
	child_recall_id = (AgsRecallID *) list->data;
	g_object_ref(child_recall_id);
      }

      if(current_recycling_context != NULL){
	g_object_unref(current_recycling_context);
      }

      if(current_parent_recycling_context != NULL){
	g_object_unref(current_parent_recycling_context);
      }
      
      /* iterate */
      list = list->next;
    }
    
    while(recycling != end_recycling){
      /* create audio signal */
      audio_signal = ags_audio_signal_new((GObject *) output_soundcard,
					  (GObject *) recycling,
					  (GObject *) child_recall_id);
      g_object_set(audio_signal,
		   "note", note,
		   NULL);
	  
      if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_PATTERN_MODE)){
	ags_recycling_create_audio_signal_with_defaults(recycling,
							audio_signal,
							0.0, 0);
      }else{
	guint note_x0, note_x1;
	guint buffer_size;

	note_x0 = (guint) offset_counter;

	g_object_get(note,
		     "x1", &note_x1,
		     NULL);

	buffer_size = audio_signal->buffer_size;
	
	/* create audio signal with frame count */
	ags_recycling_create_audio_signal_with_frame_count(recycling,
							   audio_signal,
							   (guint) (((gdouble) buffer_size * delay) * (gdouble) (note_x1 - note_x0)),
							   0.0, 0);
      }
	  
      audio_signal->stream_current = audio_signal->stream;
	    
      ags_connectable_connect(AGS_CONNECTABLE(audio_signal));
      ags_recycling_add_audio_signal(recycling,
				     audio_signal);

      /* iterate */
      next_recycling = ags_recycling_next(recycling);

      g_object_unref(recycling);

      recycling = next_recycling;
    }

    if(output_soundcard != NULL){
      g_object_unref(output_soundcard);
    }

    if(first_recycling != NULL){
      g_object_unref(first_recycling);
    }

    g_list_free_full(start_list,
		     g_object_unref);

    if(child_recall_id != NULL){
      g_object_unref(child_recall_id);
    }
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

  if(selected_input != NULL){
    g_object_unref(selected_input);
  }

  if(recall_id != NULL){
    g_object_unref(recall_id);
  }
  
  if(fx_notation_audio != NULL){
    g_object_unref(fx_notation_audio);
  }
}

void
ags_fx_notation_audio_processor_key_on(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
				       AgsNote *note,
				       guint velocity)
{
  g_return_if_fail(AGS_IS_FX_NOTATION_AUDIO_PROCESSOR(fx_notation_audio_processor));

  g_object_ref(fx_notation_audio_processor);
  AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->key_on(fx_notation_audio_processor,
										 note,
										 velocity);
  g_object_unref(fx_notation_audio_processor);
}

void
ags_fx_notation_audio_processor_real_key_off(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
					     AgsNote *note,
					     guint velocity)
{
  //TODO:JK: implement me
}

void
ags_fx_notation_audio_processor_key_off(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
					AgsNote *note,
					guint velocity)
{
  g_return_if_fail(AGS_IS_FX_NOTATION_AUDIO_PROCESSOR(fx_notation_audio_processor));

  g_object_ref(fx_notation_audio_processor);
  AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->key_off(fx_notation_audio_processor,
										  note,
										  velocity);
  g_object_unref(fx_notation_audio_processor);
}

void
ags_fx_notation_audio_processor_real_key_pressure(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
						  AgsNote *note,
						  guint velocity)
{
  //TODO:JK: implement me
}

void
ags_fx_notation_audio_processor_key_pressure(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
					     AgsNote *note,
					     guint velocity)
{
  g_return_if_fail(AGS_IS_FX_NOTATION_AUDIO_PROCESSOR(fx_notation_audio_processor));

  g_object_ref(fx_notation_audio_processor);
  AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->key_pressure(fx_notation_audio_processor,
										       note,
										       velocity);
  g_object_unref(fx_notation_audio_processor);
}

void
ags_fx_notation_audio_processor_real_play(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  AgsAudio *audio;

  AgsTimestamp *timestamp;
  
  GList *start_notation, *notation;

  guint64 offset_counter;
  guint audio_channel;
  
  GRecMutex *fx_notation_audio_processor_mutex;

  fx_notation_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio_processor);
  
  g_object_get(fx_notation_audio_processor,
	       "audio", &audio,
	       "audio-channel", &audio_channel,
	       NULL);

  start_notation = NULL;

  if(audio != NULL){
    g_object_get(audio,
		 "notation", &start_notation,
		 NULL);
  }
  
  g_rec_mutex_lock(fx_notation_audio_processor_mutex);
    
  timestamp = fx_notation_audio_processor->timestamp;
  
  offset_counter = fx_notation_audio_processor->offset_counter;

  g_rec_mutex_unlock(fx_notation_audio_processor_mutex);

  ags_timestamp_set_ags_offset(timestamp,
			       AGS_NOTATION_DEFAULT_OFFSET * floor(offset_counter / AGS_NOTATION_DEFAULT_OFFSET));
  
  notation = ags_notation_find_near_timestamp(start_notation, audio_channel,
					      timestamp);

  if(notation != NULL){
    GList *start_note, *note;
    
    start_note = ags_notation_find_offset(notation->data,
					  offset_counter,
					  FALSE);

    note = start_note;

    while(note != NULL){
      ags_fx_notation_audio_processor_key_on(fx_notation_audio_processor,
					     note->data,
					     AGS_FX_NOTATION_AUDIO_PROCESSOR_DEFAULT_KEY_ON_VELOCITY);

      /* iterate */
      note = note->next;
    }

    g_list_free_full(start_note,
		     (GDestroyNotify) g_object_unref);
  }

  if(audio != NULL){
    g_object_unref(audio);
  }
  
  g_list_free_full(start_notation,
		   (GDestroyNotify) g_object_unref);
  
}

void
ags_fx_notation_audio_processor_play(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_NOTATION_AUDIO_PROCESSOR(fx_notation_audio_processor));

  g_object_ref(fx_notation_audio_processor);
  AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->play(fx_notation_audio_processor);
  g_object_unref(fx_notation_audio_processor);
}

void
ags_fx_notation_audio_processor_real_record(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  //TODO:JK: implement me
}

void
ags_fx_notation_audio_processor_record(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_NOTATION_AUDIO_PROCESSOR(fx_notation_audio_processor));

  g_object_ref(fx_notation_audio_processor);
  AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->record(fx_notation_audio_processor);
  g_object_unref(fx_notation_audio_processor);
}

void
ags_fx_notation_audio_processor_real_feed(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  //TODO:JK: implement me
}

void
ags_fx_notation_audio_processor_feed(AgsFxNotationAudioProcessor *fx_notation_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_NOTATION_AUDIO_PROCESSOR(fx_notation_audio_processor));

  g_object_ref(fx_notation_audio_processor);
  AGS_FX_NOTATION_AUDIO_PROCESSOR_GET_CLASS(fx_notation_audio_processor)->feed(fx_notation_audio_processor);
  g_object_unref(fx_notation_audio_processor);
}

void
ags_fx_notation_audio_processor_notify_output_soundcard_callback(GObject *gobject,
								 GParamSpec *pspec,
								 gpointer user_data)
{
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;

  fx_notation_audio_processor = AGS_FX_NOTATION_AUDIO_PROCESSOR(gobject);
  
  
}

/**
 * ags_fx_notation_audio_processor_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxNotationAudioProcessor
 *
 * Returns: the new #AgsFxNotationAudioProcessor
 *
 * Since: 3.3.0
 */
AgsFxNotationAudioProcessor*
ags_fx_notation_audio_processor_new(AgsAudio *audio)
{
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;

  fx_notation_audio_processor = (AgsFxNotationAudioProcessor *) g_object_new(AGS_TYPE_FX_NOTATION_AUDIO_PROCESSOR,
									     "audio", audio,
									     NULL);

  return(fx_notation_audio_processor);
}
