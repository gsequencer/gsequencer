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

#include <ags/audio/fx/ags_fx_audio_unit_channel_processor.h>

#include <ags/audio/fx/ags_fx_audio_unit_audio.h>
#include <ags/audio/fx/ags_fx_audio_unit_channel.h>
#include <ags/audio/fx/ags_fx_audio_unit_audio_processor.h>
#include <ags/audio/fx/ags_fx_audio_unit_recycling.h>

#include <ags/i18n.h>

void ags_fx_audio_unit_channel_processor_class_init(AgsFxAudioUnitChannelProcessorClass *fx_audio_unit_channel_processor);
void ags_fx_audio_unit_channel_processor_init(AgsFxAudioUnitChannelProcessor *fx_audio_unit_channel_processor);
void ags_fx_audio_unit_channel_processor_dispose(GObject *gobject);
void ags_fx_audio_unit_channel_processor_finalize(GObject *gobject);

void ags_fx_audio_unit_channel_processor_run_init_pre(AgsRecall *recall);
void ags_fx_audio_unit_channel_processor_run_inter(AgsRecall *recall);
void ags_fx_audio_unit_channel_processor_done(AgsRecall *recall);

/**
 * SECTION:ags_fx_audio_unit_channel_processor
 * @short_description: fx Audio Unit channel processor
 * @title: AgsFxAudioUnitChannelProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_audio_unit_channel_processor.h
 *
 * The #AgsFxAudioUnitChannelProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_audio_unit_channel_processor_parent_class = NULL;

const gchar *ags_fx_audio_unit_channel_processor_plugin_name = "ags-fx-audio-unit";

GType
ags_fx_audio_unit_channel_processor_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_audio_unit_channel_processor = 0;

    static const GTypeInfo ags_fx_audio_unit_channel_processor_info = {
      sizeof (AgsFxAudioUnitChannelProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_audio_unit_channel_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel_processor */
      sizeof (AgsFxAudioUnitChannelProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_audio_unit_channel_processor_init,
    };

    ags_type_fx_audio_unit_channel_processor = g_type_register_static(AGS_TYPE_FX_NOTATION_CHANNEL_PROCESSOR,
								      "AgsFxAudioUnitChannelProcessor",
								      &ags_fx_audio_unit_channel_processor_info,
								      0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_audio_unit_channel_processor);
  }

  return(g_define_type_id__static);
}

void
ags_fx_audio_unit_channel_processor_class_init(AgsFxAudioUnitChannelProcessorClass *fx_audio_unit_channel_processor)
{
  AgsRecallClass *recall;
  GObjectClass *gobject;

  ags_fx_audio_unit_channel_processor_parent_class = g_type_class_peek_parent(fx_audio_unit_channel_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_audio_unit_channel_processor;

  gobject->dispose = ags_fx_audio_unit_channel_processor_dispose;
  gobject->finalize = ags_fx_audio_unit_channel_processor_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_audio_unit_channel_processor;

  recall->run_init_pre = ags_fx_audio_unit_channel_processor_run_init_pre;
  recall->run_inter = ags_fx_audio_unit_channel_processor_run_inter;
  recall->done = ags_fx_audio_unit_channel_processor_done;
}

void
ags_fx_audio_unit_channel_processor_init(AgsFxAudioUnitChannelProcessor *fx_audio_unit_channel_processor)
{
  AGS_RECALL(fx_audio_unit_channel_processor)->name = "ags-fx-audio-unit";
  AGS_RECALL(fx_audio_unit_channel_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_audio_unit_channel_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_audio_unit_channel_processor)->xml_type = "ags-fx-audio-unit-channel-processor";

  AGS_RECALL(fx_audio_unit_channel_processor)->child_type = AGS_TYPE_FX_AUDIO_UNIT_RECYCLING;
}

void
ags_fx_audio_unit_channel_processor_dispose(GObject *gobject)
{
  AgsFxAudioUnitChannelProcessor *fx_audio_unit_channel_processor;
  
  fx_audio_unit_channel_processor = AGS_FX_AUDIO_UNIT_CHANNEL_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_audio_unit_channel_processor_parent_class)->dispose(gobject);
}

void
ags_fx_audio_unit_channel_processor_finalize(GObject *gobject)
{
  AgsFxAudioUnitChannelProcessor *fx_audio_unit_channel_processor;
  
  fx_audio_unit_channel_processor = AGS_FX_AUDIO_UNIT_CHANNEL_PROCESSOR(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_audio_unit_channel_processor_parent_class)->finalize(gobject);
}

void
ags_fx_audio_unit_channel_processor_run_init_pre(AgsRecall *recall)
{
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  AgsFxAudioUnitChannelProcessor *fx_audio_unit_channel_processor;

  AgsFxAudioUnitAudioScopeData *scope_data;
  
  gint sound_scope;

  fx_audio_unit_channel_processor = AGS_FX_AUDIO_UNIT_CHANNEL_PROCESSOR(recall);

  fx_audio_unit_audio = NULL;
  
  g_object_get(fx_audio_unit_channel_processor,
	       "recall-audio", &fx_audio_unit_audio,
	       NULL);

  sound_scope = ags_recall_get_sound_scope(recall);
  
  scope_data = fx_audio_unit_audio->scope_data[sound_scope];
  
  if(scope_data != NULL){
    ags_atomic_uint_increment(&(scope_data->active_audio_channels));
  }
  
  if(fx_audio_unit_audio != NULL){
    g_object_unref(fx_audio_unit_audio);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_audio_unit_channel_processor_parent_class)->run_init_pre(recall);
}

void
ags_fx_audio_unit_channel_processor_run_inter(AgsRecall *recall)
{
  AgsAudio *audio;
  AgsChannel *channel;
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  AgsFxAudioUnitChannelProcessor *fx_audio_unit_channel_processor;

  AgsConfig *config;
  
  AgsFxAudioUnitAudioScopeData *scope_data;

  gchar *thread_model, *super_threaded_scope;

  gboolean super_threaded_channel;
  guint audio_channels;
  guint audio_channel;
  gint sound_scope;

  fx_audio_unit_channel_processor = AGS_FX_AUDIO_UNIT_CHANNEL_PROCESSOR(recall);

  /* config */
  config = ags_config_get_instance();
  
  /* thread model */
  super_threaded_channel = FALSE;
  
  thread_model = ags_config_get_value(config,
				      AGS_CONFIG_THREAD,
				      "model");

  if(thread_model != NULL &&
     !g_ascii_strncasecmp(thread_model,
			  "super-threaded",
			  15)){
    super_threaded_scope = ags_config_get_value(config,
						AGS_CONFIG_THREAD,
						"super-threaded-scope");
    if(super_threaded_scope != NULL &&
       (!g_ascii_strncasecmp(super_threaded_scope,
			     "channel",
			     8))){
      super_threaded_channel = TRUE;
    }
    
    g_free(super_threaded_scope);
  }

  g_free(thread_model);

  /* fields */
  audio = NULL;

  channel = NULL;
  
  fx_audio_unit_audio = NULL;
  
  g_object_get(fx_audio_unit_channel_processor,
	       "source", &channel,
	       "recall-audio", &fx_audio_unit_audio,
	       NULL);
 
  g_object_get(fx_audio_unit_audio,
	       "audio", &audio,
	       NULL);

  audio_channels = ags_audio_get_audio_channels(audio);

  audio_channel = ags_channel_get_audio_channel(channel);
  
  sound_scope = ags_recall_get_sound_scope(recall);
  
  scope_data = fx_audio_unit_audio->scope_data[sound_scope];
  
  if(scope_data != NULL){
    /* reset pre sync completed */
    g_mutex_lock(&(scope_data->completed_pre_sync_mutex));
    
    ags_atomic_int_increment(&(scope_data->completed_pre_sync_count));
    
    if(ags_atomic_boolean_get(&(scope_data->completed_pre_sync))){
      g_cond_signal(&(scope_data->completed_pre_sync_cond));
    }
    
    g_mutex_unlock(&(scope_data->completed_pre_sync_mutex));
  }

  if(audio != NULL){
    g_object_unref(audio);
  }

  if(channel != NULL){
    g_object_unref(channel);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_audio_unit_channel_processor_parent_class)->run_inter(recall);

  /* post sync */
  if(scope_data != NULL){
    ags_atomic_uint_increment(&(scope_data->completed_audio_channels));
    
    if(super_threaded_channel ||
       ags_atomic_uint_get(&(scope_data->completed_audio_channels)) >= ags_atomic_uint_get(&(scope_data->active_audio_channels))){
      /* wait until render thread completed */
      g_mutex_lock(&(scope_data->render_mutex));

      ags_atomic_boolean_set(&(scope_data->render_wait),
			     TRUE);

      while(ags_atomic_boolean_get(&(scope_data->render_wait)) &&
	    !ags_atomic_boolean_get(&(scope_data->render_done))){
	g_cond_wait(&(scope_data->render_cond),
		    &(scope_data->render_mutex));
      }
    
      g_mutex_unlock(&(scope_data->render_mutex));
      
      ags_atomic_uint_set(&(scope_data->completed_audio_channels),
			  0);      
    }
  }

  if(fx_audio_unit_audio != NULL){
    g_object_unref(fx_audio_unit_audio);
  }
}

void
ags_fx_audio_unit_channel_processor_done(AgsRecall *recall)
{
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  AgsFxAudioUnitChannelProcessor *fx_audio_unit_channel_processor;

  AgsFxAudioUnitAudioScopeData *scope_data;
  
  gint sound_scope;

  fx_audio_unit_channel_processor = AGS_FX_AUDIO_UNIT_CHANNEL_PROCESSOR(recall);

  fx_audio_unit_audio = NULL;
  
  g_object_get(fx_audio_unit_channel_processor,
	       "recall-audio", &fx_audio_unit_audio,
	       NULL);

  sound_scope = ags_recall_get_sound_scope(recall);
  
  scope_data = fx_audio_unit_audio->scope_data[sound_scope];
  
  if(scope_data != NULL){
    ags_atomic_uint_decrement(&(scope_data->active_audio_channels));

    /* reset pre sync completed */
    g_mutex_lock(&(scope_data->completed_pre_sync_mutex));
    
    ags_atomic_int_decrement(&(scope_data->completed_pre_sync_count));

    if(ags_atomic_boolean_get(&(scope_data->completed_pre_sync))){
      g_cond_signal(&(scope_data->completed_pre_sync_cond));
    }    

    g_mutex_unlock(&(scope_data->completed_pre_sync_mutex));
  }

  if(fx_audio_unit_audio != NULL){
    g_object_unref(fx_audio_unit_audio);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_audio_unit_channel_processor_parent_class)->done(recall);
}

/**
 * ags_fx_audio_unit_channel_processor_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxAudioUnitChannelProcessor
 *
 * Returns: the new #AgsFxAudioUnitChannelProcessor
 *
 * Since: 8.1.2
 */
AgsFxAudioUnitChannelProcessor*
ags_fx_audio_unit_channel_processor_new(AgsChannel *channel)
{
  AgsFxAudioUnitChannelProcessor *fx_audio_unit_channel_processor;

  fx_audio_unit_channel_processor = (AgsFxAudioUnitChannelProcessor *) g_object_new(AGS_TYPE_FX_AUDIO_UNIT_CHANNEL_PROCESSOR,
										    "source", channel,
										    NULL);

  return(fx_audio_unit_channel_processor);
}
