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

#include <ags/audio/fx/ags_fx_pattern_audio_signal.h>

#include <ags/audio/fx/ags_fx_pattern_audio_processor.h>
#include <ags/audio/fx/ags_fx_pattern_channel_processor.h>
#include <ags/audio/fx/ags_fx_pattern_recycling.h>

#include <ags/i18n.h>

void ags_fx_pattern_audio_signal_class_init(AgsFxPatternAudioSignalClass *fx_pattern_audio_signal);
void ags_fx_pattern_audio_signal_init(AgsFxPatternAudioSignal *fx_pattern_audio_signal);
void ags_fx_pattern_audio_signal_dispose(GObject *gobject);
void ags_fx_pattern_audio_signal_finalize(GObject *gobject);

void ags_fx_pattern_audio_signal_real_run_inter(AgsRecall *recall);

void ags_fx_pattern_audio_signal_real_stream_feed(AgsFxPatternAudioSignal *fx_pattern_audio_signal,
						  AgsAudioSignal *source,
						  AgsNote *note,
						  guint x0, guint x1,
						  guint y,
						  gdouble delay_counter, guint64 offset_counter,
						  guint frame_count,
						  gdouble delay, guint buffer_size);

/**
 * SECTION:ags_fx_pattern_audio_signal
 * @short_description: fx pattern audio signal
 * @title: AgsFxPatternAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_pattern_audio_signal.h
 *
 * The #AgsFxPatternAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_pattern_audio_signal_parent_class = NULL;

static const gchar *ags_fx_pattern_audio_signal_plugin_name = "ags-fx-pattern";

GType
ags_fx_pattern_audio_signal_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_pattern_audio_signal = 0;

    static const GTypeInfo ags_fx_pattern_audio_signal_info = {
      sizeof (AgsFxPatternAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_pattern_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxPatternAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_pattern_audio_signal_init,
    };

    ags_type_fx_pattern_audio_signal = g_type_register_static(AGS_TYPE_RECALL_AUDIO_SIGNAL,
							      "AgsFxPatternAudioSignal",
							      &ags_fx_pattern_audio_signal_info,
							      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_pattern_audio_signal);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_pattern_audio_signal_class_init(AgsFxPatternAudioSignalClass *fx_pattern_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_pattern_audio_signal_parent_class = g_type_class_peek_parent(fx_pattern_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_pattern_audio_signal;

  gobject->dispose = ags_fx_pattern_audio_signal_dispose;
  gobject->finalize = ags_fx_pattern_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_pattern_audio_signal;
  
  recall->run_inter = ags_fx_pattern_audio_signal_real_run_inter;

  /* AgsFxPatternAudioSignalClass */
  fx_pattern_audio_signal->stream_feed = ags_fx_pattern_audio_signal_real_stream_feed;
  fx_pattern_audio_signal->notify_remove = NULL;
}

void
ags_fx_pattern_audio_signal_init(AgsFxPatternAudioSignal *fx_pattern_audio_signal)
{
  AGS_RECALL(fx_pattern_audio_signal)->name = "ags-fx-pattern";
  AGS_RECALL(fx_pattern_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_pattern_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_pattern_audio_signal)->xml_type = "ags-fx-pattern-audio-signal";
}

void
ags_fx_pattern_audio_signal_dispose(GObject *gobject)
{
  AgsFxPatternAudioSignal *fx_pattern_audio_signal;
  
  fx_pattern_audio_signal = AGS_FX_PATTERN_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_pattern_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_pattern_audio_signal_finalize(GObject *gobject)
{
  AgsFxPatternAudioSignal *fx_pattern_audio_signal;
  
  fx_pattern_audio_signal = AGS_FX_PATTERN_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_pattern_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_pattern_audio_signal_real_run_inter(AgsRecall *recall)
{
  AgsAudioSignal *source;
  AgsAudioSignal *template;
  AgsFxPatternAudioProcessor *fx_pattern_audio_processor;
  AgsFxPatternChannelProcessor *fx_pattern_channel_processor;
  AgsFxPatternRecycling *fx_pattern_recycling;

  GList *start_note, *note;
  
  gdouble delay_counter;
  guint64 offset_counter;
  gdouble delay;
  guint length;
  guint frame_count;
  guint template_length;
  guint template_frame_count;
  guint buffer_size;
  guint i;
  
  GRecMutex *fx_pattern_audio_processor_mutex;

  source = NULL;

  fx_pattern_audio_processor = NULL;
  fx_pattern_channel_processor = NULL;
  fx_pattern_recycling = NULL;
  
  g_object_get(recall,
	       "source", &source,
	       "parent", &fx_pattern_recycling,
	       NULL);

  g_object_get(fx_pattern_recycling,
	       "parent", &fx_pattern_channel_processor,
	       NULL);
  
  g_object_get(fx_pattern_channel_processor,
	       "recall-audio-run", &fx_pattern_audio_processor,
	       NULL);

  fx_pattern_audio_processor_mutex = NULL;
  
  delay_counter = 0.0;
  offset_counter = 0;

  if(fx_pattern_audio_processor != NULL){
    fx_pattern_audio_processor_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_pattern_audio_processor);

    /* get delay counter */
    g_rec_mutex_lock(fx_pattern_audio_processor_mutex);
    
    delay_counter = fx_pattern_audio_processor->current_delay_counter;
    offset_counter = fx_pattern_audio_processor->current_offset_counter;

    g_rec_mutex_unlock(fx_pattern_audio_processor_mutex);
  }

  template = NULL;

  start_note = NULL;

  length = 0;
  frame_count = 0;

  template_length = 0;  
  template_frame_count = 0;  

  g_object_get(source,
	       "template", &template,
	       "note", &start_note,
	       "delay", &delay,
	       "length", &length,
	       "frame-count", &frame_count,
	       "buffer-size", &buffer_size,
	       NULL);

  if(template != NULL){
    g_object_get(template,
		 "length", &template_length,
		 "frame-count", &template_frame_count,
		 NULL);
  }else{
    template_length = (guint) floor(delay) + 1;
    template_frame_count = ((guint) floor(delay) + 1) * buffer_size;
  }
  
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
      if(frame_count <= template_frame_count){
	if(delay_counter == 0.0 &&
	   x0 != offset_counter){
	  if(length <= template_length){
	    ags_audio_signal_stream_safe_resize(source,
						length + ((guint) floor(delay) + 1));
	  }
	}

	if(i > 0){
	  g_object_set(source,
		       "frame-count", frame_count,
		       NULL);
	}

	ags_fx_pattern_audio_signal_stream_feed((AgsFxPatternAudioSignal *) recall,
						source,
						note->data,
						x0, x1,
						y,
						delay_counter, offset_counter,
						frame_count,
						delay, buffer_size);
      }else{
	ags_audio_signal_remove_note(source,
				     note->data);

	ags_fx_pattern_audio_signal_notify_remove((AgsFxPatternAudioSignal *) recall,
						  source,
						  note->data,
						  x0, x1,
						  y);
      }
    }

    note = note->next;
  }

  if(source == NULL ||
     source->stream_current == NULL){
    ags_recall_done(recall);
  }
  
  if(template != NULL){
    g_object_unref(template);
  }
  
  if(source != NULL){
    g_object_unref(source);
  }

  g_list_free_full(start_note,
		   (GDestroyNotify) g_object_unref);
  
  if(fx_pattern_audio_processor != NULL){
    g_object_unref(fx_pattern_audio_processor);
  }
  
  if(fx_pattern_channel_processor != NULL){
    g_object_unref(fx_pattern_channel_processor);
  }
  
  if(fx_pattern_recycling != NULL){
    g_object_unref(fx_pattern_recycling);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_pattern_audio_signal_parent_class)->run_inter(recall);
}

void
ags_fx_pattern_audio_signal_real_stream_feed(AgsFxPatternAudioSignal *fx_pattern_audio_signal,
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
			       frame_count + buffer_size);
  }else if(offset_counter + 1 == x1 &&
	   delay_counter + 1.0 >= delay){
    ags_audio_signal_close_feed(source,
				template,
				frame_count + buffer_size);
  }else{
    ags_audio_signal_continue_feed(source,
				   template,
				   frame_count + buffer_size);
  }

  g_object_set(source,
	       "frame-count", frame_count + buffer_size,
	       NULL);
  
  if(template != NULL){
    g_object_unref(template);
  }
}

void
ags_fx_pattern_audio_signal_stream_feed(AgsFxPatternAudioSignal *fx_pattern_audio_signal,
					AgsAudioSignal *source,
					AgsNote *note,
					guint x0, guint x1,
					guint y,
					gdouble delay_counter, guint64 offset_counter,
					guint frame_count,
					gdouble delay, guint buffer_size)
{
  g_return_if_fail(AGS_IS_FX_PATTERN_AUDIO_SIGNAL(fx_pattern_audio_signal));

  g_object_ref(fx_pattern_audio_signal);

  if(AGS_FX_PATTERN_AUDIO_SIGNAL_GET_CLASS(fx_pattern_audio_signal)->stream_feed != NULL){
    AGS_FX_PATTERN_AUDIO_SIGNAL_GET_CLASS(fx_pattern_audio_signal)->stream_feed(fx_pattern_audio_signal,
										source,
										note,
										x0, x1,
										y,
										delay_counter, offset_counter,
										frame_count,
										delay, buffer_size);
  }

  g_object_unref(fx_pattern_audio_signal);
}

void
ags_fx_pattern_audio_signal_notify_remove(AgsFxPatternAudioSignal *fx_pattern_audio_signal,
					  AgsAudioSignal *source,
					  AgsNote *note,
					  guint x0, guint x1,
					  guint y)
{
  g_return_if_fail(AGS_IS_FX_PATTERN_AUDIO_SIGNAL(fx_pattern_audio_signal));

  g_object_ref(fx_pattern_audio_signal);

  if(AGS_FX_PATTERN_AUDIO_SIGNAL_GET_CLASS(fx_pattern_audio_signal)->notify_remove != NULL){
    AGS_FX_PATTERN_AUDIO_SIGNAL_GET_CLASS(fx_pattern_audio_signal)->notify_remove(fx_pattern_audio_signal,
										  source,
										  note,
										  x0, x1,
										  y);
  }
  
  g_object_unref(fx_pattern_audio_signal);
}

/**
 * ags_fx_pattern_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxPatternAudioSignal
 *
 * Returns: the new #AgsFxPatternAudioSignal
 *
 * Since: 3.3.0
 */
AgsFxPatternAudioSignal*
ags_fx_pattern_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxPatternAudioSignal *fx_pattern_audio_signal;

  fx_pattern_audio_signal = (AgsFxPatternAudioSignal *) g_object_new(AGS_TYPE_FX_PATTERN_AUDIO_SIGNAL,
								     "source", audio_signal,
								     NULL);

  return(fx_pattern_audio_signal);
}
