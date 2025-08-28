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

#include <ags/audio/fx/ags_fx_audio_unit_audio_processor.h>

#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_audio_unit_plugin.h>

#include <ags/audio/ags_soundcard_util.h>

#include <ags/audio/fx/ags_fx_audio_unit_audio.h>
#include <ags/audio/fx/ags_fx_audio_unit_audio_processor.h>

#include <ags/i18n.h>

void ags_fx_audio_unit_audio_processor_class_init(AgsFxAudioUnitAudioProcessorClass *fx_audio_unit_audio_processor);
void ags_fx_audio_unit_audio_processor_init(AgsFxAudioUnitAudioProcessor *fx_audio_unit_audio_processor);
void ags_fx_audio_unit_audio_processor_dispose(GObject *gobject);
void ags_fx_audio_unit_audio_processor_finalize(GObject *gobject);

void ags_fx_audio_unit_audio_processor_run_init_pre(AgsRecall *recall);
void ags_fx_audio_unit_audio_processor_run_inter(AgsRecall *recall);
void ags_fx_audio_unit_audio_processor_cancel(AgsRecall *recall);
void ags_fx_audio_unit_audio_processor_done(AgsRecall *recall);

void ags_fx_audio_unit_audio_processor_key_on(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
					      AgsNote *note,
					      guint velocity,
					      guint key_mode);

/**
 * SECTION:ags_fx_audio_unit_audio_processor
 * @short_description: fx Audio Unit audio processor
 * @title: AgsFxAudioUnitAudioProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_audio_unit_audio_processor.h
 *
 * The #AgsFxAudioUnitAudioProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_audio_unit_audio_processor_parent_class = NULL;

GType
ags_fx_audio_unit_audio_processor_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_audio_unit_audio_processor = 0;

    static const GTypeInfo ags_fx_audio_unit_audio_processor_info = {
      sizeof (AgsFxAudioUnitAudioProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_audio_unit_audio_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio_processor */
      sizeof (AgsFxAudioUnitAudioProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_audio_unit_audio_processor_init,
    };

    ags_type_fx_audio_unit_audio_processor = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO_PROCESSOR,
								    "AgsFxAudioUnitAudioProcessor",
								    &ags_fx_audio_unit_audio_processor_info,
								    0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_audio_unit_audio_processor);
  }

  return(g_define_type_id__static);
}

void
ags_fx_audio_unit_audio_processor_class_init(AgsFxAudioUnitAudioProcessorClass *fx_audio_unit_audio_processor)
{
  AgsFxNotationAudioProcessorClass *fx_notation_audio_processor;
  AgsRecallClass *recall;
  GObjectClass *gobject;
  
  ags_fx_audio_unit_audio_processor_parent_class = g_type_class_peek_parent(fx_audio_unit_audio_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_audio_unit_audio_processor;

  gobject->dispose = ags_fx_audio_unit_audio_processor_dispose;
  gobject->finalize = ags_fx_audio_unit_audio_processor_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_audio_unit_audio_processor;

  recall->run_init_pre = ags_fx_audio_unit_audio_processor_run_init_pre;
  recall->run_inter = ags_fx_audio_unit_audio_processor_run_inter;
  recall->cancel = ags_fx_audio_unit_audio_processor_cancel;
  recall->done = ags_fx_audio_unit_audio_processor_done;
  
  /* AgsFxNotationAudioProcessorClass */
  fx_notation_audio_processor = (AgsFxNotationAudioProcessorClass *) fx_audio_unit_audio_processor;
  
  fx_notation_audio_processor->key_on = ags_fx_audio_unit_audio_processor_key_on;
}

void
ags_fx_audio_unit_audio_processor_init(AgsFxAudioUnitAudioProcessor *fx_audio_unit_audio_processor)
{
  AGS_RECALL(fx_audio_unit_audio_processor)->name = "ags-fx-audio-unit";
  AGS_RECALL(fx_audio_unit_audio_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_audio_unit_audio_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_audio_unit_audio_processor)->xml_type = "ags-fx-audio-unit-audio-processor";
}

void
ags_fx_audio_unit_audio_processor_dispose(GObject *gobject)
{
  AgsFxAudioUnitAudioProcessor *fx_audio_unit_audio_processor;
  
  fx_audio_unit_audio_processor = AGS_FX_AUDIO_UNIT_AUDIO_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_audio_unit_audio_processor_parent_class)->dispose(gobject);
}

void
ags_fx_audio_unit_audio_processor_finalize(GObject *gobject)
{
  AgsFxAudioUnitAudioProcessor *fx_audio_unit_audio_processor;
  
  fx_audio_unit_audio_processor = AGS_FX_AUDIO_UNIT_AUDIO_PROCESSOR(gobject);  
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_audio_unit_audio_processor_parent_class)->finalize(gobject);
}

void
ags_fx_audio_unit_audio_processor_run_init_pre(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  AgsFxAudioUnitAudioProcessor *fx_audio_unit_audio_processor;
  
  AgsFxAudioUnitAudioScopeData *scope_data;
  
  gint sound_scope;

  fx_audio_unit_audio_processor = AGS_FX_AUDIO_UNIT_AUDIO_PROCESSOR(recall);
  
  audio = NULL;
  
  fx_audio_unit_audio = NULL;
  
  g_object_get(fx_audio_unit_audio_processor,
	       "audio", &audio,
	       "recall-audio", &fx_audio_unit_audio,
	       NULL);

  sound_scope = ags_recall_get_sound_scope(recall);
  
  scope_data = fx_audio_unit_audio->scope_data[sound_scope];

  if(scope_data != NULL){
    scope_data->running = TRUE;
    
    ags_fx_audio_unit_audio_start_render_thread(fx_audio_unit_audio);
  }

  if(audio != NULL){
    g_object_unref(audio);
  }

  if(fx_audio_unit_audio != NULL){
    g_object_unref(fx_audio_unit_audio);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_audio_unit_audio_processor_parent_class)->run_init_pre(recall);
}
        
void
ags_fx_audio_unit_audio_processor_run_inter(AgsRecall *recall)
{
  //TODO:JK: implement me
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_audio_unit_audio_processor_parent_class)->run_inter(recall);
}

void
ags_fx_audio_unit_audio_processor_cancel(AgsRecall *recall)
{
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  AgsFxAudioUnitAudioProcessor *fx_audio_unit_audio_processor;

  AgsFxAudioUnitAudioScopeData *scope_data;
  
  gint sound_scope;

  fx_audio_unit_audio_processor = AGS_FX_AUDIO_UNIT_AUDIO_PROCESSOR(recall);
  
  fx_audio_unit_audio = NULL;
  
  g_object_get(fx_audio_unit_audio_processor,
	       "recall-audio", &fx_audio_unit_audio,
	       NULL);

  sound_scope = ags_recall_get_sound_scope(recall);
  
  scope_data = fx_audio_unit_audio->scope_data[sound_scope];

  if(scope_data != NULL){
    scope_data->running = FALSE;

    ags_fx_audio_unit_audio_stop_render_thread(fx_audio_unit_audio);
  }
  
  if(fx_audio_unit_audio != NULL){
    g_object_unref(fx_audio_unit_audio);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_audio_unit_audio_processor_parent_class)->cancel(recall);
}

void
ags_fx_audio_unit_audio_processor_done(AgsRecall *recall)
{
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  AgsFxAudioUnitAudioProcessor *fx_audio_unit_audio_processor;

  AgsFxAudioUnitAudioScopeData *scope_data;
  
  gint sound_scope;

  fx_audio_unit_audio_processor = AGS_FX_AUDIO_UNIT_AUDIO_PROCESSOR(recall);
  
  fx_audio_unit_audio = NULL;
  
  g_object_get(fx_audio_unit_audio_processor,
	       "recall-audio", &fx_audio_unit_audio,
	       NULL);

  sound_scope = ags_recall_get_sound_scope(recall);
  
  scope_data = fx_audio_unit_audio->scope_data[sound_scope];

  if(scope_data != NULL){
    scope_data->running = FALSE;
    
    //TODO:JK: implement me
  }
  
  if(fx_audio_unit_audio != NULL){
    g_object_unref(fx_audio_unit_audio);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_audio_unit_audio_processor_parent_class)->done(recall);
}

void
ags_fx_audio_unit_audio_processor_key_on(AgsFxNotationAudioProcessor *fx_notation_audio_processor,
					 AgsNote *note,
					 guint velocity,
					 guint key_mode)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsChannel *input, *selected_input;
  AgsFxAudioUnitAudio *fx_audio_unit_audio;

  AgsFxAudioUnitAudioScopeData *scope_data;
  AgsFxAudioUnitAudioChannelData *channel_data;

  guint sound_scope;
  guint audio_channels;
  guint input_pads;
  guint audio_channel;
  guint audio_start_mapping;
  guint midi_start_mapping;
  gint midi_note;
  guint y;
  
  GRecMutex *fx_audio_unit_audio_mutex;

  audio = NULL;

  sound_scope = ags_recall_get_sound_scope((AgsRecall *) fx_notation_audio_processor);

  audio_channels = 0;
  
  audio_channel = 0;

  audio_start_mapping = 0;
  midi_start_mapping = 0;

  y = 0;

  g_object_get(fx_notation_audio_processor,
	       "audio", &audio,
	       "recall-audio", &fx_audio_unit_audio,
	       "audio-channel", &audio_channel,
	       NULL);

  fx_audio_unit_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);
  
  g_object_get(audio,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       "audio-start-mapping", &audio_start_mapping,
	       "midi-start-mapping", &midi_start_mapping,
	       NULL);

  input_pads = ags_audio_get_input_pads(audio);
  
  y = ags_note_get_y(note);
  
  input = ags_channel_nth(start_input,
			  audio_channel);

  if(ags_audio_test_behaviour_flags(audio, AGS_SOUND_BEHAVIOUR_REVERSE_MAPPING)){
    selected_input = ags_channel_pad_nth(input,
					 input_pads - y - 1);
  }else{
    selected_input = ags_channel_pad_nth(input,
					 y);
  }
  
  midi_note = (y - audio_start_mapping + midi_start_mapping);

  /* ags-fx-audio-unit scope and channel data */
  g_rec_mutex_lock(fx_audio_unit_audio_mutex);
  
  scope_data = fx_audio_unit_audio->scope_data[sound_scope];
  
  channel_data = scope_data->channel_data[audio_channel];

  g_rec_mutex_unlock(fx_audio_unit_audio_mutex);
  
  if(selected_input != NULL){
    AgsRecycling *first_recycling, *last_recycling;
    AgsRecycling *recycling, *next_recycling;
    AgsRecycling *end_recycling;

    first_recycling = NULL;
    last_recycling = NULL;
    
    g_object_get(selected_input,
		 "first-recycling", &first_recycling,
		 "last-recycling", &last_recycling,
		 NULL);

    end_recycling = ags_recycling_next(last_recycling);

    recycling = first_recycling;

    if(recycling != NULL){
      g_object_ref(recycling);
    }
    
    while(recycling != end_recycling){
      ags_atomic_int_increment(&(channel_data->queued_audio_signal));
      
      /* iterate */
      next_recycling = ags_recycling_next(recycling);

      g_object_unref(recycling);

      recycling = next_recycling;
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
  }

  if(midi_note >= 0 &&
     midi_note < 128){
    AgsFxAudioUnitAudioInputData *input_data;
    
    g_rec_mutex_lock(fx_audio_unit_audio_mutex);

    channel_data->event_count += 1;
    
    input_data = channel_data->input_data[midi_note];

    if(note != NULL){
      g_object_ref(note);

      input_data->note = g_list_prepend(input_data->note,
					note);
    }
    
    input_data->key_on += 1;
    
    g_rec_mutex_unlock(fx_audio_unit_audio_mutex);
  }

  if(audio != NULL){
    g_object_unref(audio);
  }
  
  if(fx_audio_unit_audio != NULL){
    g_object_unref(fx_audio_unit_audio);
  }
  
  /* call parent */
  AGS_FX_NOTATION_AUDIO_PROCESSOR_CLASS(ags_fx_audio_unit_audio_processor_parent_class)->key_on(fx_notation_audio_processor,
												note,
												velocity,
												key_mode);
}

/**
 * ags_fx_audio_unit_audio_processor_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxAudioUnitAudioProcessor
 *
 * Returns: the new #AgsFxAudioUnitAudioProcessor
 *
 * Since: 8.1.2
 */
AgsFxAudioUnitAudioProcessor*
ags_fx_audio_unit_audio_processor_new(AgsAudio *audio)
{
  AgsFxAudioUnitAudioProcessor *fx_audio_unit_audio_processor;

  fx_audio_unit_audio_processor = (AgsFxAudioUnitAudioProcessor *) g_object_new(AGS_TYPE_FX_AUDIO_UNIT_AUDIO_PROCESSOR,
										"audio", audio,
										NULL);

  return(fx_audio_unit_audio_processor);
}
