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

#include <ags/audio/fx/ags_fx_notation_audio_signal.h>

#include <ags/audio/fx/ags_fx_notation_audio_processor.h>
#include <ags/audio/fx/ags_fx_notation_channel_processor.h>
#include <ags/audio/fx/ags_fx_notation_recycling.h>

#include <ags/i18n.h>

void ags_fx_notation_audio_signal_class_init(AgsFxNotationAudioSignalClass *fx_notation_audio_signal);
void ags_fx_notation_audio_signal_init(AgsFxNotationAudioSignal *fx_notation_audio_signal);
void ags_fx_notation_audio_signal_dispose(GObject *gobject);
void ags_fx_notation_audio_signal_finalize(GObject *gobject);

void ags_fx_notation_audio_signal_real_run_inter(AgsRecall *recall);

void ags_fx_notation_audio_signal_real_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
						   AgsAudioSignal *source,
						   AgsNote *note,
						   guint x0, guint x1,
						   guint y,
						   gdouble delay_counter, guint64 offset_counter,
						   guint frame_count,
						   gdouble delay, guint buffer_size);
void ags_fx_notation_audio_signal_real_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
						     AgsAudioSignal *source,
						     AgsNote *note,
						     guint x0, guint x1,
						     guint y);

/**
 * SECTION:ags_fx_notation_audio_signal
 * @short_description: fx notation audio signal
 * @title: AgsFxNotationAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_notation_audio_signal.h
 *
 * The #AgsFxNotationAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_notation_audio_signal_parent_class = NULL;

static const gchar *ags_fx_notation_audio_signal_plugin_name = "ags-fx-notation";

GType
ags_fx_notation_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_notation_audio_signal = 0;

    static const GTypeInfo ags_fx_notation_audio_signal_info = {
      sizeof (AgsFxNotationAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_notation_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxNotationAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_notation_audio_signal_init,
    };

    ags_type_fx_notation_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							       "AgsFxNotationAudioSignal",
							       &ags_fx_notation_audio_signal_info,
							       0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_notation_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_notation_audio_signal_class_init(AgsFxNotationAudioSignalClass *fx_notation_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_notation_audio_signal_parent_class = g_type_class_peek_parent(fx_notation_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_notation_audio_signal;

  gobject->dispose = ags_fx_notation_audio_signal_dispose;
  gobject->finalize = ags_fx_notation_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_notation_audio_signal;
  
  recall->run_inter = ags_fx_notation_audio_signal_real_run_inter;

  /* AgsFxNotationAudioSignalClass */
  fx_notation_audio_signal->stream_feed = ags_fx_notation_audio_signal_real_stream_feed;
  fx_notation_audio_signal->notify_remove = NULL;
}

void
ags_fx_notation_audio_signal_init(AgsFxNotationAudioSignal *fx_notation_audio_signal)
{
  AGS_RECALL(fx_notation_audio_signal)->name = "ags-fx-notation";
  AGS_RECALL(fx_notation_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_notation_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_notation_audio_signal)->xml_type = "ags-fx-notation-audio-signal";
}

void
ags_fx_notation_audio_signal_dispose(GObject *gobject)
{
  AgsFxNotationAudioSignal *fx_notation_audio_signal;
  
  fx_notation_audio_signal = AGS_FX_NOTATION_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_notation_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_notation_audio_signal_finalize(GObject *gobject)
{
  AgsFxNotationAudioSignal *fx_notation_audio_signal;
  
  fx_notation_audio_signal = AGS_FX_NOTATION_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_notation_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_notation_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsFxNotationAudioProcessor *fx_notation_audio_processor;
  AgsFxNotationChannelProcessor *fx_notation_channel_processor;
  AgsFxNotationRecycling *fx_notation_recycling;

  GList *start_note, *note;
  
  gdouble delay_counter;
  guint64 offset_counter;
  gdouble delay;
  guint length;
  guint frame_count;
  guint buffer_size;
  guint i;
  
  GRecMutex *fx_notation_audio_processor_mutex;

  if(!ags_recall_check_sound_scope(recall, AGS_SOUND_SCOPE_NOTATION)){
    ags_recall_done(recall);

    /* call parent */
    AGS_RECALL_CLASS(ags_fx_notation_audio_signal_parent_class)->run_inter(recall);
    
    return;
  }
  
  source = NULL;

  fx_notation_audio_processor = NULL;
  fx_notation_channel_processor = NULL;
  fx_notation_recycling = NULL;
  
  g_object_get(recall,
	       "source", &source,
	       "parent", &fx_notation_recycling,
	       NULL);

  g_object_get(fx_notation_recycling,
	       "parent", &fx_notation_channel_processor,
	       NULL);
  
  g_object_get(fx_notation_channel_processor,
	       "recall-audio-run", &fx_notation_audio_processor,
	       NULL);

  fx_notation_audio_processor_mutex = NULL;
  
  delay_counter = 0.0;
  offset_counter = 0;

  if(fx_notation_audio_processor != NULL){
    fx_notation_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio_processor);

    /* get delay counter */
    g_rec_mutex_lock(fx_notation_audio_processor_mutex);
    
    delay_counter = fx_notation_audio_processor->current_delay_counter;
    offset_counter = fx_notation_audio_processor->current_offset_counter;

    g_rec_mutex_unlock(fx_notation_audio_processor_mutex);
  }

  start_note = NULL;

  length = 0;
  frame_count = 0;
  
  g_object_get(source,
	       "note", &start_note,
	       "delay", &delay,
	       "length", &length,
	       "frame-count", &frame_count,
	       "buffer-size", &buffer_size,
	       NULL);

  note = start_note;

  for(i = 0; note != NULL; i++){
    guint x0, x1;
    guint y;
    
    g_object_get(note->data,
		 "x0", &x0,
		 "x1", &x1,
		 "y", &y,
		 NULL);

    if(offset_counter >= x0){
      g_message("ags-fx-notation 0x%x", source);
      
      if(offset_counter < x1){
	ags_audio_signal_add_stream(source);

	ags_fx_notation_audio_signal_stream_feed((AgsFxNotationAudioSignal *) recall,
						 source,
						 note->data,
						 x0, x1,
						 y,
						 delay_counter, offset_counter,
						 frame_count,
						 delay, buffer_size);
	if(i == 0){
	  g_object_set(source,
		       "frame-count", frame_count + buffer_size,
		       NULL);
	}
      }else{
	ags_audio_signal_remove_note(source,
				     note->data);

	ags_fx_notation_audio_signal_notify_remove((AgsFxNotationAudioSignal *) recall,
						   source,
						   note->data,
						   x0, x1,
						   y);
      }
    }

    note = note->next;
  }
  
  if(source != NULL){
    g_object_unref(source);
  }

  g_list_free_full(start_note,
		   (GDestroyNotify) g_object_unref);
  
  if(fx_notation_audio_processor != NULL){
    g_object_unref(fx_notation_audio_processor);
  }
  
  if(fx_notation_channel_processor != NULL){
    g_object_unref(fx_notation_channel_processor);
  }
  
  if(fx_notation_recycling != NULL){
    g_object_unref(fx_notation_recycling);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_notation_audio_signal_parent_class)->run_inter(recall);
}

void
ags_fx_notation_audio_signal_real_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					      AgsAudioSignal *source,
					      AgsNote *note,
					      guint x0, guint x1,
					      guint y,
					      gdouble delay_counter, guint64 offset_counter,
					      guint frame_count,
					      gdouble delay, guint buffer_size)
{
  AgsAudioSignal *template;

  template = NULL;

  g_object_get(source,
	       "template", &template,
	       NULL);
		 
  if(x0 == offset_counter &&
     delay_counter == 0.0){
    ags_audio_signal_open_feed(source,
			       template,
			       frame_count + buffer_size, frame_count);
  }else if(offset_counter + 1 == x1 &&
	   delay_counter + 1.0 >= delay){
    ags_audio_signal_close_feed(source,
				template,
				frame_count + buffer_size, frame_count);
  }else{
    ags_audio_signal_continue_feed(source,
				   template,
				   frame_count + buffer_size, frame_count);
  }

  g_object_set(source,
	       "frame-count", frame_count + buffer_size,
	       NULL);

  if(template != NULL){
    g_object_unref(template);
  }
}

void
ags_fx_notation_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					 AgsAudioSignal *source,
					 AgsNote *note,
					 guint x0, guint x1,
					 guint y,
					 gdouble delay_counter, guint64 offset_counter,
					 guint frame_count,
					 gdouble delay, guint buffer_size)
{
  g_return_if_fail(AGS_IS_FX_NOTATION_AUDIO_SIGNAL(fx_notation_audio_signal));

  g_object_ref(fx_notation_audio_signal);

  if(AGS_FX_NOTATION_AUDIO_SIGNAL_GET_CLASS(fx_notation_audio_signal)->stream_feed != NULL){
    AGS_FX_NOTATION_AUDIO_SIGNAL_GET_CLASS(fx_notation_audio_signal)->stream_feed(fx_notation_audio_signal,
										  source,
										  note,
										  x0, x1,
										  y,
										  delay_counter, offset_counter,
										  frame_count,
										  delay, buffer_size);
  }

  g_object_unref(fx_notation_audio_signal);
}

void
ags_fx_notation_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					   AgsAudioSignal *source,
					   AgsNote *note,
					   guint x0, guint x1,
					   guint y)
{
  g_return_if_fail(AGS_IS_FX_NOTATION_AUDIO_SIGNAL(fx_notation_audio_signal));

  g_object_ref(fx_notation_audio_signal);

  if(AGS_FX_NOTATION_AUDIO_SIGNAL_GET_CLASS(fx_notation_audio_signal)->notify_remove != NULL){
    AGS_FX_NOTATION_AUDIO_SIGNAL_GET_CLASS(fx_notation_audio_signal)->notify_remove(fx_notation_audio_signal,
										    source,
										    note,
										    x0, x1,
										    y);
  }
  
  g_object_unref(fx_notation_audio_signal);
}

/**
 * ags_fx_notation_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxNotationAudioSignal
 *
 * Returns: the new #AgsFxNotationAudioSignal
 *
 * Since: 3.3.0
 */
AgsFxNotationAudioSignal*
ags_fx_notation_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxNotationAudioSignal *fx_notation_audio_signal;

  fx_notation_audio_signal = (AgsFxNotationAudioSignal *) g_object_new(AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL,
								       "source", audio_signal,
								       NULL);

  return(fx_notation_audio_signal);
}
