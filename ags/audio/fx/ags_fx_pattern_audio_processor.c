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

#include <ags/audio/fx/ags_fx_pattern_audio_processor.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_recycling.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_port.h>
#include <ags/audio/ags_pattern.h>
#include <ags/audio/ags_note.h>
#include <ags/audio/ags_recall_id.h>
#include <ags/audio/ags_recycling_context.h>

#include <ags/audio/midi/ags_midi_util.h>

#include <ags/audio/fx/ags_fx_pattern_audio.h>

#include <ags/i18n.h>

void ags_fx_pattern_audio_processor_class_init(AgsFxPatternAudioProcessorClass *fx_pattern_audio_processor);
void ags_fx_pattern_audio_processor_init(AgsFxPatternAudioProcessor *fx_pattern_audio_processor);
void ags_fx_pattern_audio_processor_dispose(GObject *gobject);
void ags_fx_pattern_audio_processor_finalize(GObject *gobject);

void ags_fx_pattern_audio_processor_real_run_inter(AgsRecall *recall);

void ags_fx_pattern_audio_processor_real_key_on(AgsFxPatternAudioProcessor *fx_pattern_audio_processor,
						AgsNote *note,
						guint velocity,
						guint key_mode);

void ags_fx_pattern_audio_processor_real_play(AgsFxPatternAudioProcessor *fx_pattern_audio_processor);

void ags_fx_pattern_audio_processor_real_counter_change(AgsFxPatternAudioProcessor *fx_pattern_audio_processor);

/**
 * SECTION:ags_fx_pattern_audio_processor
 * @short_description: fx pattern audio_processor
 * @title: AgsFxPatternAudioProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_pattern_audio_processor.h
 *
 * The #AgsFxPatternAudioProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_pattern_audio_processor_parent_class = NULL;

GType
ags_fx_pattern_audio_processor_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_pattern_audio_processor = 0;

    static const GTypeInfo ags_fx_pattern_audio_processor_info = {
      sizeof (AgsFxPatternAudioProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_pattern_audio_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_processor */
      sizeof (AgsFxPatternAudioProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_pattern_audio_processor_init,
    };

    ags_type_fx_pattern_audio_processor = g_type_register_static(AGS_TYPE_RECALL_AUDIO_RUN,
								 "AgsFxPatternAudioProcessor",
								 &ags_fx_pattern_audio_processor_info,
								 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_pattern_audio_processor);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_pattern_audio_processor_class_init(AgsFxPatternAudioProcessorClass *fx_pattern_audio_processor)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  
  ags_fx_pattern_audio_processor_parent_class = g_type_class_peek_parent(fx_pattern_audio_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_pattern_audio_processor;

  gobject->dispose = ags_fx_pattern_audio_processor_dispose;
  gobject->finalize = ags_fx_pattern_audio_processor_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_pattern_audio_processor;
  
  recall->run_inter = ags_fx_pattern_audio_processor_real_run_inter;
  
  /* AgsFxPatternAudioProcessorClass */
  fx_pattern_audio_processor->key_on = ags_fx_pattern_audio_processor_real_key_on;

  fx_pattern_audio_processor->play = ags_fx_pattern_audio_processor_real_play;

  fx_pattern_audio_processor->counter_change = ags_fx_pattern_audio_processor_real_counter_change;
}

void
ags_fx_pattern_audio_processor_init(AgsFxPatternAudioProcessor *fx_pattern_audio_processor)
{
  AGS_RECALL(fx_pattern_audio_processor)->name = "ags-fx-pattern";
  AGS_RECALL(fx_pattern_audio_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_pattern_audio_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_pattern_audio_processor)->xml_type = "ags-fx-pattern-audio-processor";

  /* counter */
  fx_pattern_audio_processor->delay_counter = 0.0;
  fx_pattern_audio_processor->offset_counter = 0;

  fx_pattern_audio_processor->current_delay_counter = 0.0;
  fx_pattern_audio_processor->current_offset_counter = 0;
}

void
ags_fx_pattern_audio_processor_dispose(GObject *gobject)
{
  AgsFxPatternAudioProcessor *fx_pattern_audio_processor;
  
  fx_pattern_audio_processor = AGS_FX_PATTERN_AUDIO_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_pattern_audio_processor_parent_class)->dispose(gobject);
}

void
ags_fx_pattern_audio_processor_finalize(GObject *gobject)
{
  AgsFxPatternAudioProcessor *fx_pattern_audio_processor;
  
  fx_pattern_audio_processor = AGS_FX_PATTERN_AUDIO_PROCESSOR(gobject);  
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_pattern_audio_processor_parent_class)->finalize(gobject);
}

void
ags_fx_pattern_audio_processor_real_run_inter(AgsRecall *recall)
{
  AgsFxPatternAudioProcessor *fx_pattern_audio_processor;

  gdouble delay_counter;
  
  GRecMutex *fx_pattern_audio_processor_mutex;

  fx_pattern_audio_processor = AGS_FX_PATTERN_AUDIO_PROCESSOR(recall);
  
  fx_pattern_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_pattern_audio_processor);

  /* get delay counter */
  g_rec_mutex_lock(fx_pattern_audio_processor_mutex);
    
  fx_pattern_audio_processor->current_delay_counter = fx_pattern_audio_processor->delay_counter;
  fx_pattern_audio_processor->current_offset_counter = fx_pattern_audio_processor->offset_counter;

  delay_counter = fx_pattern_audio_processor->delay_counter;

  g_rec_mutex_unlock(fx_pattern_audio_processor_mutex);

  /* run */
  if(delay_counter == 0.0){
    ags_fx_pattern_audio_processor_play(fx_pattern_audio_processor);
  }

  /* counter change */
  ags_fx_pattern_audio_processor_counter_change(fx_pattern_audio_processor);
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_pattern_audio_processor_parent_class)->run_inter(recall);
}

void
ags_fx_pattern_audio_processor_real_key_on(AgsFxPatternAudioProcessor *fx_pattern_audio_processor,
					   AgsNote *note,
					   guint velocity,
					   guint key_mode)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *input, *selected_input;
  AgsRecallID *recall_id;
  AgsRecyclingContext *recycling_context;
  AgsFxPatternAudio *fx_pattern_audio;
  AgsPort *port;

  gdouble delay;
  guint input_pads;
  guint audio_channel;
  guint y;

  GValue value = {0,};

  GRecMutex *fx_pattern_audio_processor_mutex;

  fx_pattern_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_pattern_audio_processor);
  
  g_object_get(fx_pattern_audio_processor,
	       "audio", &audio,
	       "input", &start_input,
	       "recall-id", &recall_id,
	       "recall-audio", &fx_pattern_audio,
	       "audio-channel", &audio_channel,
	       NULL);
  
  g_object_get(recall_id,
	       "recycling-context", &recycling_context,
	       NULL);

  /* get delay */
  delay = AGS_SOUNDCARD_DEFAULT_DELAY;
  
  if(fx_pattern_audio != NULL){        
    g_object_get(fx_pattern_audio,
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
    g_object_get(selected_input,
		 "recall-id", &start_list,
		 NULL);

    list = start_list;
    child_recall_id = NULL;

    while(child_recall_id == NULL &&
	  list != NULL){
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

    recycling = first_recycling;
    g_object_ref(recycling);
    
    while(recycling != end_recycling){
      AgsAudioSignal *audio_signal;
      
      /* create audio signal */
      audio_signal = ags_audio_signal_new((GObject *) output_soundcard,
					  (GObject *) recycling,
					  (GObject *) child_recall_id);
      g_object_set(audio_signal,
		   "note", note,
		   NULL);

      ags_audio_signal_stream_resize(audio_signal,
				     (guint) floor(delay) + 1);      
	    
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

    if(last_recycling != NULL){
      g_object_unref(last_recycling);
    }

    if(recycling != NULL){
      g_object_unref(recycling);
    }
    
    if(end_recycling != NULL){
      g_object_unref(end_recycling);
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
  
  if(fx_pattern_audio != NULL){
    g_object_unref(fx_pattern_audio);
  }
}

void
ags_fx_pattern_audio_processor_key_on(AgsFxPatternAudioProcessor *fx_pattern_audio_processor,
				      AgsNote *note,
				      guint velocity,
				      guint key_mode)
{
  g_return_if_fail(AGS_IS_FX_PATTERN_AUDIO_PROCESSOR(fx_pattern_audio_processor));

  g_object_ref(fx_pattern_audio_processor);

  if(AGS_FX_PATTERN_AUDIO_PROCESSOR_GET_CLASS(fx_pattern_audio_processor)->key_on != NULL){
    AGS_FX_PATTERN_AUDIO_PROCESSOR_GET_CLASS(fx_pattern_audio_processor)->key_on(fx_pattern_audio_processor,
										 note,
										 velocity,
										 key_mode);
  }
  
  g_object_unref(fx_pattern_audio_processor);
}

void
ags_fx_pattern_audio_processor_real_play(AgsFxPatternAudioProcessor *fx_pattern_audio_processor)
{
  AgsAudio *audio;
  AgsChannel *start_input, *input;
  AgsChannel *channel, *next_pad;
  AgsNote *note;
  
  AgsFxPatternAudio *fx_pattern_audio;
  
  guint64 offset_counter;
  guint audio_channel;
  gfloat bank_index_0;
  gfloat bank_index_1;

  GValue value = {0,};
  
  GRecMutex *fx_pattern_audio_mutex;
  GRecMutex *fx_pattern_audio_processor_mutex;

  fx_pattern_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_pattern_audio_processor);
  
  g_object_get(fx_pattern_audio_processor,
	       "audio", &audio,
	       "input", &start_input,
	       "recall-audio", &fx_pattern_audio,
	       "audio-channel", &audio_channel,
	       NULL);

  g_rec_mutex_lock(fx_pattern_audio_processor_mutex);
    
  offset_counter = fx_pattern_audio_processor->offset_counter;

  g_rec_mutex_unlock(fx_pattern_audio_processor_mutex);

  note = NULL;
    
  bank_index_0 = 0.0;
  bank_index_1 = 0.0;

  if(fx_pattern_audio != NULL){
    AgsPort *port;

    /* bank index 0 */
    g_object_get(fx_pattern_audio,
		 "bank-index-0", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value,
		   G_TYPE_FLOAT);
    
      ags_port_safe_read(port,
			 &value);

      bank_index_0 = g_value_get_float(&value);
      g_value_unset(&value);

      g_object_unref(port);
    }

    /* bank index 1 */
    g_object_get(fx_pattern_audio,
		 "bank-index-1", &port,
		 NULL);

    if(port != NULL){
      g_value_init(&value,
		   G_TYPE_FLOAT);
    
      ags_port_safe_read(port,
			 &value);

      bank_index_1 = g_value_get_float(&value);
      g_value_unset(&value);

      g_object_unref(port);
    }
  }
  
  /* get input */
  input = ags_channel_nth(start_input,
			  audio_channel);

  channel = input;
  g_object_ref(channel);
  
  while(channel != NULL){
    AgsPattern *current_pattern;
    
    GList *start_pattern;

    guint pad;
    
    current_pattern = NULL;

    pad = 0;
    
    g_object_get(channel,
		 "pattern", &start_pattern,
		 "pad", &pad,
		 NULL);

    if(start_pattern != NULL){
      current_pattern = start_pattern->data;
    }
    
    if(ags_pattern_get_bit(current_pattern,
			   (guint) bank_index_0,
			   (guint) bank_index_1,
			   offset_counter)){
      note = ags_pattern_get_note(current_pattern,
				  offset_counter);
      
      ags_fx_pattern_audio_processor_key_on(fx_pattern_audio_processor,
					    note,
					    AGS_FX_PATTERN_AUDIO_PROCESSOR_DEFAULT_KEY_ON_VELOCITY,
					    0);

      if(note != NULL){
	g_object_unref(note);
      }
    }
    
    g_list_free_full(start_pattern,
		     (GDestroyNotify) g_object_unref);

    /* iterate */
    next_pad = ags_channel_next_pad(channel);

    g_object_unref(channel);

    channel = next_pad;
  }
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(input != NULL){
    g_object_unref(input);
  }

  if(fx_pattern_audio != NULL){
    g_object_unref(fx_pattern_audio);
  }

  if(audio != NULL){
    g_object_unref(audio);
  }
}

void
ags_fx_pattern_audio_processor_play(AgsFxPatternAudioProcessor *fx_pattern_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_PATTERN_AUDIO_PROCESSOR(fx_pattern_audio_processor));

  g_object_ref(fx_pattern_audio_processor);

  if(AGS_FX_PATTERN_AUDIO_PROCESSOR_GET_CLASS(fx_pattern_audio_processor)->play != NULL){
    AGS_FX_PATTERN_AUDIO_PROCESSOR_GET_CLASS(fx_pattern_audio_processor)->play(fx_pattern_audio_processor);
  }
  
  g_object_unref(fx_pattern_audio_processor);
}

void
ags_fx_pattern_audio_processor_real_counter_change(AgsFxPatternAudioProcessor *fx_pattern_audio_processor)
{
  AgsFxPatternAudio *fx_pattern_audio;

  gdouble delay;
  gboolean loop;
  guint64 loop_start, loop_end;
  
  GValue value = {0,};

  GRecMutex *fx_pattern_audio_processor_mutex;
  
  fx_pattern_audio_processor = NULL;
  fx_pattern_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_pattern_audio_processor);

  g_object_get(fx_pattern_audio_processor,
	       "recall-audio", &fx_pattern_audio,
	       NULL);

  delay = AGS_SOUNDCARD_DEFAULT_DELAY;

  loop = FALSE;

  loop_start = AGS_FX_PATTERN_AUDIO_DEFAULT_LOOP_START;
  loop_end = AGS_FX_PATTERN_AUDIO_DEFAULT_LOOP_END;
  
  if(fx_pattern_audio != NULL){
    AgsPort *port;

    /* delay */
    g_object_get(fx_pattern_audio,
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
    g_object_get(fx_pattern_audio,
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
    g_object_get(fx_pattern_audio,
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
    g_object_get(fx_pattern_audio,
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
    g_rec_mutex_lock(fx_pattern_audio_processor_mutex);
    
    fx_pattern_audio_processor->delay_counter = 0.0;

    if(loop &&
       fx_pattern_audio_processor->offset_counter + 1 >= loop_end){
      fx_pattern_audio_processor->offset_counter = loop_start;
    }else{
      fx_pattern_audio_processor->offset_counter += 1;

      if(fx_pattern_audio_processor->offset_counter >= loop_end){
	ags_recall_done(fx_pattern_audio_processor);
      }
    }
    
    g_rec_mutex_unlock(fx_pattern_audio_processor_mutex);
  }else{
    g_rec_mutex_lock(fx_pattern_audio_processor_mutex);
    
    fx_pattern_audio_processor->delay_counter += 1.0;

    g_rec_mutex_unlock(fx_pattern_audio_processor_mutex);
  }
  
  if(fx_pattern_audio != NULL){
    g_object_unref(fx_pattern_audio);
  }
}

void
ags_fx_pattern_audio_processor_counter_change(AgsFxPatternAudioProcessor *fx_pattern_audio_processor)
{
  g_return_if_fail(AGS_IS_FX_PATTERN_AUDIO_PROCESSOR(fx_pattern_audio_processor));

  g_object_ref(fx_pattern_audio_processor);
  
  if(AGS_FX_PATTERN_AUDIO_PROCESSOR_GET_CLASS(fx_pattern_audio_processor)->counter_change != NULL){
    AGS_FX_PATTERN_AUDIO_PROCESSOR_GET_CLASS(fx_pattern_audio_processor)->counter_change(fx_pattern_audio_processor);
  }
  
  g_object_unref(fx_pattern_audio_processor);
}

/**
 * ags_fx_pattern_audio_processor_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxPatternAudioProcessor
 *
 * Returns: the new #AgsFxPatternAudioProcessor
 *
 * Since: 3.3.0
 */
AgsFxPatternAudioProcessor*
ags_fx_pattern_audio_processor_new(AgsAudio *audio)
{
  AgsFxPatternAudioProcessor *fx_pattern_audio_processor;

  fx_pattern_audio_processor = (AgsFxPatternAudioProcessor *) g_object_new(AGS_TYPE_FX_PATTERN_AUDIO_PROCESSOR,
									   "audio", audio,
									   NULL);

  return(fx_pattern_audio_processor);
}
