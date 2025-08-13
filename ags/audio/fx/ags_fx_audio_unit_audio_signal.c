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
  
  recall->run_init_pre = ags_fx_audio_unit_audio_signal_run_init_pre;
  recall->run_inter = ags_fx_audio_unit_audio_signal_run_inter;
  recall->done = ags_fx_audio_unit_audio_signal_done;

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
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  AgsFxAudioUnitChannel *fx_audio_unit_channel;
  AgsFxAudioUnitChannelProcessor *fx_audio_unit_channel_processor;
  AgsFxAudioUnitRecycling *fx_audio_unit_recycling;
  AgsFxAudioUnitAudioSignal *fx_audio_unit_audio_signal;
  AgsChannel *channel;
  AgsAudioSignal *audio_signal;
  
  GList *active_audio_signal;

  guint pad;
  gint sound_scope;

  GRecMutex *fx_audio_unit_audio_mutex;

  fx_audio_unit_audio_signal = (AgsFxAudioUnitAudioSignal *) recall;

  channel = NULL;

  audio_signal = NULL;
  
  fx_audio_unit_audio = NULL;
  
  fx_audio_unit_channel = NULL;
  fx_audio_unit_channel_processor = NULL;

  fx_audio_unit_recycling = NULL;
  
  sound_scope = ags_recall_get_sound_scope(recall);

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

  /* set active audio signal */
  fx_audio_unit_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  if(audio_signal != NULL){
    g_rec_mutex_lock(fx_audio_unit_audio_mutex);

    ags_atomic_int_increment(&(fx_audio_unit_audio->scope_data[sound_scope]->active_audio_signal_count));

    active_audio_signal = g_hash_table_lookup(fx_audio_unit_audio->scope_data[sound_scope]->active_audio_signal,
					      GUINT_TO_POINTER(pad));

    active_audio_signal = g_list_prepend(active_audio_signal,
					 audio_signal);

    g_hash_table_insert(fx_audio_unit_audio->scope_data[sound_scope]->active_audio_signal,
			GUINT_TO_POINTER(pad),
			active_audio_signal);
  
    g_rec_mutex_unlock(fx_audio_unit_audio_mutex);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_audio_unit_audio_signal_recall_audio_signal_class)->run_init_pre(recall);
}

void
ags_fx_audio_unit_audio_signal_run_inter(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  AgsFxAudioUnitChannel *fx_audio_unit_channel;
  AgsFxAudioUnitChannelProcessor *fx_audio_unit_channel_processor;
  AgsFxAudioUnitRecycling *fx_audio_unit_recycling;
  AgsFxAudioUnitAudioSignal *fx_audio_unit_audio_signal;
  AgsChannel *channel;
  AgsAudioSignal *audio_signal;
  
  GList *active_audio_signal;

  gint sound_scope;
  guint audio_channels;
  guint buffer_size;
  AgsSoundcardFormat format;
  guint copy_mode;

  GRecMutex *fx_audio_unit_audio_mutex;
  GRecMutex *stream_mutex;

  fx_audio_unit_audio_signal = (AgsFxAudioUnitAudioSignal *) recall;
  
  audio = NULL;
  
  channel = NULL;

  audio_signal = NULL;
  
  fx_audio_unit_audio = NULL;
  
  fx_audio_unit_channel = NULL;
  fx_audio_unit_channel_processor = NULL;

  fx_audio_unit_recycling = NULL;
  
  sound_scope = ags_recall_get_sound_scope(recall);

  g_object_get(recall,
	       "parent", &fx_audio_unit_recycling,
	       "source", &audio_signal,
	       NULL);

  stream_mutex = AGS_AUDIO_SIGNAL_GET_STREAM_MUTEX(audio_signal);
  
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
  
  audio_channels = ags_audio_get_audio_channels(audio);

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;
    
  g_object_get(fx_audio_unit_audio,
	       "buffer-size", &buffer_size,
	       "format", &format,
	       NULL);
  
  copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(NULL,
							      AGS_AUDIO_BUFFER_UTIL_FLOAT,
							      ags_audio_buffer_util_format_from_soundcard(NULL, format));

  /* mix active audio signal */
  fx_audio_unit_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  if(audio_signal != NULL){
    AgsFxAudioUnitAudioScopeData *scope_data;
    
    AVAudioPCMBuffer *av_input_buffer;
    
    guint audio_channel;

    audio_channel = ags_channel_get_audio_channel(channel);

    //NOTE:JK: nested mutex lock
    g_rec_mutex_lock(fx_audio_unit_audio_mutex);
    
    scope_data = fx_audio_unit_audio->scope_data[sound_scope];
    
    av_input_buffer = (AVAudioPCMBuffer *) scope_data->av_input_buffer;
    
    g_rec_mutex_lock(stream_mutex);

    /* fill input buffer */
    ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						[av_input_buffer floatChannelData], audio_channels, audio_channel,
						audio_signal->stream->data, 1, 0,
						buffer_size, copy_mode);

    ags_audio_buffer_util_clear_buffer(NULL,
				       audio_signal->stream->data, 1,
				       buffer_size, AGS_AUDIO_BUFFER_UTIL_FLOAT);
     
    g_rec_mutex_unlock(stream_mutex);
    g_rec_mutex_unlock(fx_audio_unit_audio_mutex);

    /* signal render thread */
    g_mutex_lock(&(scope_data->completed_mutex));
    
    ags_atomic_int_increment(&(scope_data->completed_audio_signal_count));

    if(ags_atomic_boolean_get(&(scope_data->completed_wait))){
      g_cond_signal(&(scope_data->completed_cond));
    }
    
    g_mutex_unlock(&(scope_data->completed_mutex));
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_audio_unit_audio_signal_recall_audio_signal_class)->run_inter(recall);
}

void
ags_fx_audio_unit_audio_signal_done(AgsRecall *recall)
{
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  AgsFxAudioUnitChannel *fx_audio_unit_channel;
  AgsFxAudioUnitChannelProcessor *fx_audio_unit_channel_processor;
  AgsFxAudioUnitRecycling *fx_audio_unit_recycling;
  AgsFxAudioUnitAudioSignal *fx_audio_unit_audio_signal;
  AgsChannel *channel;
  AgsAudioSignal *audio_signal;
  
  GList *active_audio_signal;

  guint pad;
  gint sound_scope;

  GRecMutex *fx_audio_unit_audio_mutex;

  fx_audio_unit_audio_signal = (AgsFxAudioUnitAudioSignal *) recall;

  channel = NULL;

  audio_signal = NULL;
  
  fx_audio_unit_audio = NULL;
  
  fx_audio_unit_channel = NULL;
  fx_audio_unit_channel_processor = NULL;

  fx_audio_unit_recycling = NULL;
  
  sound_scope = ags_recall_get_sound_scope(recall);

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

  pad = ags_channel_get_pad(channel);
  
  /* unset active audio signal */
  fx_audio_unit_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  if(audio_signal != NULL){
    AgsFxAudioUnitAudioScopeData *scope_data;

    scope_data = fx_audio_unit_audio->scope_data[sound_scope];
    
    g_rec_mutex_lock(fx_audio_unit_audio_mutex);

    ags_atomic_int_decrement(&(scope_data->active_audio_signal_count));

    active_audio_signal = g_hash_table_lookup(scope_data->active_audio_signal,
					      GUINT_TO_POINTER(pad));

    active_audio_signal = g_list_remove(active_audio_signal,
					audio_signal);

    g_hash_table_insert(scope_data->active_audio_signal,
			GUINT_TO_POINTER(pad),
			active_audio_signal);
  
    g_rec_mutex_unlock(fx_audio_unit_audio_mutex);

    /* signal render thread */
    g_mutex_lock(&(scope_data->completed_mutex));
    
    ags_atomic_int_increment(&(scope_data->completed_audio_signal_count));

    if(ags_atomic_boolean_get(&(scope_data->completed_wait))){
      g_cond_signal(&(scope_data->completed_cond));
    }
    
    g_mutex_unlock(&(scope_data->completed_mutex));
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_audio_unit_audio_signal_recall_audio_signal_class)->done(recall);
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
  //TODO:JK: implement me
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
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  AgsFxAudioUnitChannel *fx_audio_unit_channel;
  AgsFxAudioUnitChannelProcessor *fx_audio_unit_channel_processor;
  AgsFxAudioUnitRecycling *fx_audio_unit_recycling;
  AgsFxAudioUnitAudioSignal *fx_audio_unit_audio_signal;
  
  GList *active_audio_signal;

  gint sound_scope;
  guint pad;
  guint audio_start_mapping;
  guint midi_start_mapping;
  guint midi_note;

  GRecMutex *fx_audio_unit_audio_mutex;

  fx_audio_unit_audio_signal = (AgsFxAudioUnitAudioSignal *) fx_notation_audio_signal;

  audio = NULL;
  channel = NULL;
  
  fx_audio_unit_audio = NULL;
  
  fx_audio_unit_channel = NULL;
  fx_audio_unit_channel_processor = NULL;

  fx_audio_unit_recycling = NULL;
  
  sound_scope = ags_recall_get_sound_scope((AgsRecall *) fx_notation_audio_signal);

  g_object_get(fx_notation_audio_signal,
	       "parent", &fx_audio_unit_recycling,
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

  pad = ags_channel_get_pad(channel);
  
  midi_note = (y - audio_start_mapping + midi_start_mapping);

  /* unset active audio signal */
  fx_audio_unit_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  if(note != NULL){
    AgsFxAudioUnitAudioScopeData *scope_data;
    AgsFxAudioUnitAudioChannelData *channel_data;
    AgsFxAudioUnitAudioInputData *input_data;
    
    g_rec_mutex_lock(fx_audio_unit_audio_mutex);

    scope_data = fx_audio_unit_audio->scope_data[sound_scope];

    channel_data = scope_data->channel_data[0];
    
    channel_data->event_count -= 1;

    input_data = channel_data->input_data[midi_note];

    input_data->note = g_list_remove(input_data->note,
				     note);
    g_object_unref(note);
    
    g_rec_mutex_unlock(fx_audio_unit_audio_mutex);
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
