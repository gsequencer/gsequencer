/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_audio_unit_audio_signal.h>

#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_audio_unit_plugin.h>

#include <ags/audio/ags_soundcard_util.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/fx/ags_fx_audio_unit_audio.h>
#include <ags/audio/fx/ags_fx_audio_unit_audio_processor.h>
#include <ags/audio/fx/ags_fx_audio_unit_channel.h>
#include <ags/audio/fx/ags_fx_audio_unit_channel_processor.h>
#include <ags/audio/fx/ags_fx_audio_unit_recycling.h>

#import <CoreFoundation/CoreFoundation.h>
#import <AVFoundation/AVFoundation.h>

#include <ags/i18n.h>

void ags_fx_audio_unit_audio_signal_class_init(AgsFxAudioUnitAudioSignalClass *fx_audio_unit_audio_signal);
void ags_fx_audio_unit_audio_signal_init(AgsFxAudioUnitAudioSignal *fx_audio_unit_audio_signal);
void ags_fx_audio_unit_audio_signal_dispose(GObject *gobject);
void ags_fx_audio_unit_audio_signal_finalize(GObject *gobject);

void ags_fx_audio_unit_audio_signal_run_init_pre(AgsRecall *recall);
void ags_fx_audio_unit_audio_signal_run_inter(AgsRecall *recall);
void ags_fx_audio_unit_audio_signal_done(AgsRecall *recall);

void ags_fx_audio_unit_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
						AgsAudioSignal *source,
						AgsNote *note,
						gboolean pattern_mode,
						guint x0, guint x1,
						guint y,
						gdouble delay_counter, guint64 offset_counter,
						guint frame_count,
						gdouble delay, guint buffer_size);
void ags_fx_audio_unit_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
						  AgsAudioSignal *source,
						  AgsNote *note,
						  guint x0, guint x1,
						  guint y);

/**
 * SECTION:ags_fx_audio_unit_audio_signal
 * @short_description: fx audio_unit audio signal
 * @title: AgsFxAudioUnitAudioSignal
 * @section_id:
 * @include: ags/audio/fx/ags_fx_audio_unit_audio_signal.h
 *
 * The #AgsFxAudioUnitAudioSignal class provides ports to the effect processor.
 */

static gpointer ags_fx_audio_unit_audio_signal_recall_audio_signal_class = NULL;
static gpointer ags_fx_audio_unit_audio_signal_parent_class = NULL;

const gchar *ags_fx_audio_unit_audio_signal_plugin_name = "ags-fx-audio-unit";

GType
ags_fx_audio_unit_audio_signal_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_audio_unit_audio_signal = 0;

    static const GTypeInfo ags_fx_audio_unit_audio_signal_info = {
      sizeof (AgsFxAudioUnitAudioSignalClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_audio_unit_audio_signal_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_signal */
      sizeof (AgsFxAudioUnitAudioSignal),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_audio_unit_audio_signal_init,
    };

    ags_type_fx_audio_unit_audio_signal = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_SIGNAL,
								 "AgsFxAudioUnitAudioSignal",
								 &ags_fx_audio_unit_audio_signal_info,
								 0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_audio_unit_audio_signal);
  }

  return(g_define_type_id__static);
}

void
ags_fx_audio_unit_audio_signal_class_init(AgsFxAudioUnitAudioSignalClass *fx_audio_unit_audio_signal)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  AgsFxNotationAudioSignalClass *fx_notation_audio_signal;

  ags_fx_audio_unit_audio_signal_recall_audio_signal_class = g_type_class_peek(AGS_TYPE_RECALL_AUDIO_SIGNAL);
  ags_fx_audio_unit_audio_signal_parent_class = g_type_class_peek_parent(fx_audio_unit_audio_signal);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_audio_unit_audio_signal;

  gobject->dispose = ags_fx_audio_unit_audio_signal_dispose;
  gobject->finalize = ags_fx_audio_unit_audio_signal_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_audio_unit_audio_signal;
  
  //  recall->run_init_pre = ags_fx_audio_unit_audio_signal_run_init_pre;
  recall->run_inter = ags_fx_audio_unit_audio_signal_run_inter;
  //  recall->done = ags_fx_audio_unit_audio_signal_done;

  /* AgsFxNotationAudioSignalClass */
  fx_notation_audio_signal = (AgsFxNotationAudioSignalClass *) fx_audio_unit_audio_signal;
  
  fx_notation_audio_signal->stream_feed = ags_fx_audio_unit_audio_signal_stream_feed;
  fx_notation_audio_signal->notify_remove = ags_fx_audio_unit_audio_signal_notify_remove;
}

void
ags_fx_audio_unit_audio_signal_init(AgsFxAudioUnitAudioSignal *fx_audio_unit_audio_signal)
{
  AGS_RECALL(fx_audio_unit_audio_signal)->name = "ags-fx-audio-unit";
  AGS_RECALL(fx_audio_unit_audio_signal)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_audio_unit_audio_signal)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_audio_unit_audio_signal)->xml_type = "ags-fx-audio-unit-audio-signal";
}

void
ags_fx_audio_unit_audio_signal_dispose(GObject *gobject)
{
  AgsFxAudioUnitAudioSignal *fx_audio_unit_audio_signal;
  
  fx_audio_unit_audio_signal = AGS_FX_AUDIO_UNIT_AUDIO_SIGNAL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_audio_unit_audio_signal_parent_class)->dispose(gobject);
}

void
ags_fx_audio_unit_audio_signal_finalize(GObject *gobject)
{
  AgsFxAudioUnitAudioSignal *fx_audio_unit_audio_signal;
  
  fx_audio_unit_audio_signal = AGS_FX_AUDIO_UNIT_AUDIO_SIGNAL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_audio_unit_audio_signal_parent_class)->finalize(gobject);
}

void
ags_fx_audio_unit_audio_signal_run_init_pre(AgsRecall *recall)
{
  AgsFxAudioUnitAudioSignal *fx_audio_unit_audio_signal;

  if(ags_recall_test_flags(recall, AGS_RECALL_TEMPLATE) ||
     ags_recall_test_flags(recall, AGS_RECALL_DEFAULT_TEMPLATE)){
    g_warning("running on template");
    
    return;
  }
  
  fx_audio_unit_audio_signal = (AgsFxAudioUnitAudioSignal *) recall;
  
  //TODO:JK: implement me
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_audio_unit_audio_signal_recall_audio_signal_class)->run_init_pre(recall);
}

void
ags_fx_audio_unit_audio_signal_run_inter(AgsRecall *recall)
{ 
  AgsAudio *audio;
  AgsChannel *channel;
  AgsAudioSignal *audio_signal;
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  AgsFxAudioUnitChannel *fx_audio_unit_channel;
  AgsFxAudioUnitChannelProcessor *fx_audio_unit_channel_processor;
  AgsFxAudioUnitRecycling *fx_audio_unit_recycling;
  AgsFxAudioUnitAudioSignal *fx_audio_unit_audio_signal;
  
  AgsAudioUnitPlugin *audio_unit_plugin;
  
  GList *active_audio_signal;

  gint sound_scope;
  guint audio_channels;
  guint pad;
  guint audio_channel;
  guint buffer_size;
  AgsSoundcardFormat format;
  guint copy_mode;

  GRecMutex *fx_audio_unit_audio_mutex;
  GRecMutex *recall_mutex;
  
  fx_audio_unit_audio_signal = (AgsFxAudioUnitAudioSignal *) recall;

  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall);
  
  audio = NULL;
  
  channel = NULL;

  audio_signal = NULL;
  
  fx_audio_unit_audio = NULL;
  
  fx_audio_unit_channel = NULL;
  fx_audio_unit_channel_processor = NULL;

  fx_audio_unit_recycling = NULL;

  g_object_get(recall,
	       "parent", &fx_audio_unit_recycling,
	       "source", &audio_signal,
	       NULL);
  
  g_object_get(fx_audio_unit_recycling,
	       "parent", &fx_audio_unit_channel_processor,
	       NULL);

  g_object_get(fx_audio_unit_channel_processor,
	       "source", &channel,
	       "recall-audio", &fx_audio_unit_audio,
	       NULL);

  g_object_get(fx_audio_unit_audio,
	       "audio", &audio,
	       NULL);

  /* get mutex */
  fx_audio_unit_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  audio_unit_plugin = fx_audio_unit_audio->audio_unit_plugin;
  
  g_rec_mutex_unlock(recall_mutex);

  if(ags_base_plugin_test_flags((AgsBasePlugin *) audio_unit_plugin,
				AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    goto ags_fx_audio_unit_audio_signal_run_inter_END;
  }

  audio_channels = ags_audio_get_audio_channels(audio);
  
  pad = ags_channel_get_pad(channel);
  audio_channel = ags_channel_get_audio_channel(channel);
  
  sound_scope = ags_recall_get_sound_scope(fx_audio_unit_channel_processor);

  if(audio_signal != NULL){
    AgsFxAudioUnitAudioScopeData *scope_data;
    
    g_rec_mutex_lock(fx_audio_unit_audio_mutex);
    
    scope_data = fx_audio_unit_audio->scope_data[sound_scope];
    
    g_rec_mutex_unlock(fx_audio_unit_audio_mutex);    

    if(scope_data != NULL){
      AgsFxAudioUnitAudioChannelData *channel_data;
	  
      channel_data = scope_data->channel_data[audio_channel];

      if(channel_data != NULL){
	ags_fx_audio_unit_audio_render_thread_iteration(fx_audio_unit_audio,
							channel_data,
							audio_signal,
							pad,
							audio_channel,
							sound_scope);

      }
    }
  }

 ags_fx_audio_unit_audio_signal_run_inter_END:
  
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(channel != NULL){
    g_object_unref(channel);
  }
  
  if(audio_signal != NULL){
    g_object_unref(audio_signal);
  }

  if(fx_audio_unit_audio != NULL){
    g_object_unref(fx_audio_unit_audio);
  }

  if(fx_audio_unit_channel_processor != NULL){
    g_object_unref(fx_audio_unit_channel_processor);
  }

  if(fx_audio_unit_recycling != NULL){
    g_object_unref(fx_audio_unit_recycling);
  }
  
  /* call parent */
  if(ags_base_plugin_test_flags((AgsBasePlugin *) audio_unit_plugin,
				AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    AGS_RECALL_CLASS(ags_fx_audio_unit_audio_signal_parent_class)->run_inter(recall);
  }else{
    AGS_RECALL_CLASS(ags_fx_audio_unit_audio_signal_recall_audio_signal_class)->run_inter(recall);
  }
}

void
ags_fx_audio_unit_audio_signal_done(AgsRecall *recall)
{
  AgsFxAudioUnitAudioSignal *fx_audio_unit_audio_signal;

  fx_audio_unit_audio_signal = (AgsFxAudioUnitAudioSignal *) recall;

  //TODO:JK: implement me
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_audio_unit_audio_signal_parent_class)->done(recall);
}

void
ags_fx_audio_unit_audio_signal_stream_feed(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					   AgsAudioSignal *source,
					   AgsNote *note,
					   gboolean pattern_mode,
					   guint x0, guint x1,
					   guint y,
					   gdouble delay_counter, guint64 offset_counter,
					   guint frame_count,
					   gdouble delay, guint buffer_size)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsAudioSignal *audio_signal;
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  AgsFxAudioUnitChannel *fx_audio_unit_channel;
  AgsFxAudioUnitChannelProcessor *fx_audio_unit_channel_processor;
  AgsFxAudioUnitRecycling *fx_audio_unit_recycling;
  AgsFxAudioUnitAudioSignal *fx_audio_unit_audio_signal;
  
  AgsAudioUnitPlugin *audio_unit_plugin;
  
  GList *active_audio_signal;

  gint sound_scope;
  guint audio_channels;
  guint input_pads;
  guint pad;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;
  AgsSoundcardFormat format;
  guint copy_mode;

  GRecMutex *fx_audio_unit_audio_mutex;
  GRecMutex *recall_mutex;
    
  /* call parent */
#if 0
  AGS_FX_NOTATION_AUDIO_SIGNAL_CLASS(ags_fx_audio_unit_audio_signal_recall_audio_signal_class)->stream_feed(fx_notation_audio_signal,
													    source,
													    note,
													    pattern_mode,
													    x0, x1,
													    y,
													    delay_counter, offset_counter,
													    frame_count,
													    delay, buffer_size);
#endif
  
  fx_audio_unit_audio_signal = (AgsFxAudioUnitAudioSignal *) fx_notation_audio_signal;

  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio_signal);
  
  audio = NULL;
  
  channel = NULL;

  audio_signal = NULL;
  
  fx_audio_unit_audio = NULL;
  
  fx_audio_unit_channel = NULL;
  fx_audio_unit_channel_processor = NULL;

  fx_audio_unit_recycling = NULL;

  audio_start_mapping = 0;
  midi_start_mapping = 0;
  
  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_audio_unit_recycling,
	       "source", &audio_signal,
	       NULL);
  
  g_object_get(fx_audio_unit_recycling,
	       "parent", &fx_audio_unit_channel_processor,
	       NULL);

  g_object_get(fx_audio_unit_channel_processor,
	       "source", &channel,
	       "recall-audio", &fx_audio_unit_audio,
	       NULL);

  g_object_get(fx_audio_unit_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  /* get mutex */
  fx_audio_unit_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  audio_unit_plugin = fx_audio_unit_audio->audio_unit_plugin;
  
  g_rec_mutex_unlock(recall_mutex);

  audio_channels = ags_audio_get_audio_channels(audio);
  
  input_pads = ags_audio_get_input_pads(audio);
  
  pad = ags_channel_get_pad(channel);

  audio_channel = ags_channel_get_audio_channel(channel);
  
  midi_note = (y - audio_start_mapping + midi_start_mapping);
  
  sound_scope = ags_recall_get_sound_scope(fx_audio_unit_channel_processor);

  if(audio_signal != NULL){
    AgsFxAudioUnitAudioScopeData *scope_data;
    
    g_rec_mutex_lock(fx_audio_unit_audio_mutex);
    
    scope_data = fx_audio_unit_audio->scope_data[sound_scope];
    
    g_rec_mutex_unlock(fx_audio_unit_audio_mutex);    

    if(scope_data != NULL){
      AgsFxAudioUnitAudioChannelData *channel_data;
	  
      channel_data = scope_data->channel_data[audio_channel];

      if(channel_data != NULL){
	if(g_list_find(channel_data->input_data[midi_note]->note, note) == NULL){
	  //	  g_message("feed midi note %d", midi_note);

	  g_object_ref(note);
	  
	  channel_data->input_data[midi_note]->note = g_list_prepend(channel_data->input_data[midi_note]->note,
								     note);
	}
	
	ags_fx_audio_unit_audio_render_thread_iteration(fx_audio_unit_audio,
							channel_data,
							audio_signal,
							pad,
							audio_channel,
							sound_scope);

      }
    }
  }

 ags_fx_audio_unit_audio_signal_stream_feed_END:
  
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(channel != NULL){
    g_object_unref(channel);
  }
  
  if(audio_signal != NULL){
    g_object_unref(audio_signal);
  }

  if(fx_audio_unit_audio != NULL){
    g_object_unref(fx_audio_unit_audio);
  }

  if(fx_audio_unit_channel_processor != NULL){
    g_object_unref(fx_audio_unit_channel_processor);
  }

  if(fx_audio_unit_recycling != NULL){
    g_object_unref(fx_audio_unit_recycling);
  }
}

void
ags_fx_audio_unit_audio_signal_notify_remove(AgsFxNotationAudioSignal *fx_notation_audio_signal,
					     AgsAudioSignal *source,
					     AgsNote *note,
					     guint x0, guint x1,
					     guint y)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsAudioSignal *audio_signal;
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  AgsFxAudioUnitChannel *fx_audio_unit_channel;
  AgsFxAudioUnitChannelProcessor *fx_audio_unit_channel_processor;
  AgsFxAudioUnitRecycling *fx_audio_unit_recycling;
  AgsFxAudioUnitAudioSignal *fx_audio_unit_audio_signal;
  
  AgsAudioUnitPlugin *audio_unit_plugin;
  
  GList *active_audio_signal;

  gint sound_scope;
  guint audio_channels;
  guint pad;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;
  guint buffer_size;
  AgsSoundcardFormat format;
  guint copy_mode;

  GRecMutex *fx_audio_unit_audio_mutex;
  GRecMutex *recall_mutex;

  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_notation_audio_signal);
  
  audio = NULL;
  
  channel = NULL;

  audio_signal = NULL;
  
  fx_audio_unit_audio = NULL;
  
  fx_audio_unit_channel = NULL;
  fx_audio_unit_channel_processor = NULL;

  fx_audio_unit_recycling = NULL;

  audio_start_mapping = 0;
  midi_start_mapping = 0;
  
  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_audio_unit_recycling,
	       "source", &audio_signal,
	       NULL);
  
  g_object_get(fx_audio_unit_recycling,
	       "parent", &fx_audio_unit_channel_processor,
	       NULL);

  g_object_get(fx_audio_unit_channel_processor,
	       "source", &channel,
	       "recall-audio", &fx_audio_unit_audio,
	       NULL);

  g_object_get(fx_audio_unit_audio,
	       "audio", &audio,
	       NULL);

  g_object_get(audio,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);
  
  /* get mutex */
  fx_audio_unit_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  audio_unit_plugin = fx_audio_unit_audio->audio_unit_plugin;
  
  g_rec_mutex_unlock(recall_mutex);

  if(!ags_base_plugin_test_flags((AgsBasePlugin *) audio_unit_plugin,
				 AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    goto ags_fx_audio_unit_audio_signal_notify_remove_END;
  }

  audio_channels = ags_audio_get_audio_channels(audio);
  
  pad = ags_channel_get_pad(channel);
  audio_channel = ags_channel_get_audio_channel(channel);
  
  sound_scope = ags_recall_get_sound_scope(fx_audio_unit_channel_processor);
  
  y = ags_note_get_y(note);
  
  midi_note = (y - audio_start_mapping + midi_start_mapping);
  
  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxAudioUnitAudioScopeData *scope_data;
    
    g_rec_mutex_lock(fx_audio_unit_audio_mutex);
    
    scope_data = fx_audio_unit_audio->scope_data[sound_scope];
    
    g_rec_mutex_unlock(fx_audio_unit_audio_mutex);    

    if(scope_data != NULL){
      AgsFxAudioUnitAudioChannelData *channel_data;
	  
      channel_data = scope_data->channel_data[audio_channel];

      if(channel_data != NULL){
	AgsFxAudioUnitAudioInputData *input_data;
	
	ags_atomic_int_decrement(&(channel_data->queued_audio_signal));
	
	channel_data->event_count -= 1;
	
	input_data = channel_data->input_data[midi_note];

	if(note != NULL){
	  input_data->note = g_list_remove(input_data->note,
					   note);

	  g_object_unref(note);
	}
	
	input_data->key_on -= 1;
      }
    }
  }

 ags_fx_audio_unit_audio_signal_notify_remove_END:
  
  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(channel != NULL){
    g_object_unref(channel);
  }
  
  if(audio_signal != NULL){
    g_object_unref(audio_signal);
  }

  if(fx_audio_unit_audio != NULL){
    g_object_unref(fx_audio_unit_audio);
  }

  if(fx_audio_unit_channel_processor != NULL){
    g_object_unref(fx_audio_unit_channel_processor);
  }

  if(fx_audio_unit_recycling != NULL){
    g_object_unref(fx_audio_unit_recycling);
  }
}

/**
 * ags_fx_audio_unit_audio_signal_new:
 * @audio_signal: the #AgsAudioSignal
 *
 * Create a new instance of #AgsFxAudioUnitAudioSignal
 *
 * Returns: the new #AgsFxAudioUnitAudioSignal
 *
 * Since: 8.1.2
 */
AgsFxAudioUnitAudioSignal*
ags_fx_audio_unit_audio_signal_new(AgsAudioSignal *audio_signal)
{
  AgsFxAudioUnitAudioSignal *fx_audio_unit_audio_signal;

  fx_audio_unit_audio_signal = (AgsFxAudioUnitAudioSignal *) g_object_new(AGS_TYPE_FX_AUDIO_UNIT_AUDIO_SIGNAL,
									  "source", audio_signal,
									  NULL);

  return(fx_audio_unit_audio_signal);
}
