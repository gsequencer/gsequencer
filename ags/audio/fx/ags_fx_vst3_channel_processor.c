/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_vst3_channel_processor.h>

#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_vst3_plugin.h>

#include <ags/audio/fx/ags_fx_vst3_audio.h>
#include <ags/audio/fx/ags_fx_vst3_channel.h>
#include <ags/audio/fx/ags_fx_vst3_audio_processor.h>
#include <ags/audio/fx/ags_fx_vst3_recycling.h>

#include <ags/i18n.h>

void ags_fx_vst3_channel_processor_class_init(AgsFxVst3ChannelProcessorClass *fx_vst3_channel_processor);
void ags_fx_vst3_channel_processor_init(AgsFxVst3ChannelProcessor *fx_vst3_channel_processor);
void ags_fx_vst3_channel_processor_dispose(GObject *gobject);
void ags_fx_vst3_channel_processor_finalize(GObject *gobject);

void ags_fx_vst3_channel_processor_run_init_pre(AgsRecall *recall);
void ags_fx_vst3_channel_processor_done(AgsRecall *recall);

/**
 * SECTION:ags_fx_vst3_channel_processor
 * @short_description: fx vst3 channel processor
 * @title: AgsFxVst3ChannelProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_vst3_channel_processor.h
 *
 * The #AgsFxVst3ChannelProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_vst3_channel_processor_parent_class = NULL;

const gchar *ags_fx_vst3_channel_processor_plugin_name = "ags-fx-vst3";

GType
ags_fx_vst3_channel_processor_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_vst3_channel_processor = 0;

    static const GTypeInfo ags_fx_vst3_channel_processor_info = {
      sizeof (AgsFxVst3ChannelProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_vst3_channel_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel_processor */
      sizeof (AgsFxVst3ChannelProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_vst3_channel_processor_init,
    };

    ags_type_fx_vst3_channel_processor = g_type_register_static(AGS_TYPE_FX_NOTATION_CHANNEL_PROCESSOR,
								"AgsFxVst3ChannelProcessor",
								&ags_fx_vst3_channel_processor_info,
								0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_vst3_channel_processor);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_vst3_channel_processor_class_init(AgsFxVst3ChannelProcessorClass *fx_vst3_channel_processor)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_vst3_channel_processor_parent_class = g_type_class_peek_parent(fx_vst3_channel_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_vst3_channel_processor;

  gobject->dispose = ags_fx_vst3_channel_processor_dispose;
  gobject->finalize = ags_fx_vst3_channel_processor_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_vst3_channel_processor;

  recall->run_init_pre = ags_fx_vst3_channel_processor_run_init_pre;
  recall->done = ags_fx_vst3_channel_processor_done;
}

void
ags_fx_vst3_channel_processor_init(AgsFxVst3ChannelProcessor *fx_vst3_channel_processor)
{
  AGS_RECALL(fx_vst3_channel_processor)->name = "ags-fx-vst3";
  AGS_RECALL(fx_vst3_channel_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_vst3_channel_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_vst3_channel_processor)->xml_type = "ags-fx-vst3-channel-processor";

  AGS_RECALL(fx_vst3_channel_processor)->child_type = AGS_TYPE_FX_VST3_RECYCLING;
}

void
ags_fx_vst3_channel_processor_dispose(GObject *gobject)
{
  AgsFxVst3ChannelProcessor *fx_vst3_channel_processor;
  
  fx_vst3_channel_processor = AGS_FX_VST3_CHANNEL_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_vst3_channel_processor_parent_class)->dispose(gobject);
}

void
ags_fx_vst3_channel_processor_finalize(GObject *gobject)
{
  AgsFxVst3ChannelProcessor *fx_vst3_channel_processor;
  
  fx_vst3_channel_processor = AGS_FX_VST3_CHANNEL_PROCESSOR(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_vst3_channel_processor_parent_class)->finalize(gobject);
}

void
ags_fx_vst3_channel_processor_run_init_pre(AgsRecall *recall)
{
  AgsFxVst3Audio *fx_vst3_audio;
  
  AgsVst3Plugin *vst3_plugin;

  AgsFxVst3ChannelInputData *input_data;

  guint sound_scope;
  gboolean is_live_instrument;

  guint j, k;
  GRecMutex *recall_mutex;
  
  GRecMutex *fx_vst3_audio_mutex;
  
  fx_vst3_audio = NULL;

  g_object_get(recall,
	       "recall-audio", &fx_vst3_audio,
	       NULL);

  sound_scope = ags_recall_get_sound_scope(recall);

  /* get VST3 plugin */
  fx_vst3_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  is_live_instrument = ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);

  g_rec_mutex_lock(fx_vst3_audio_mutex);

  vst3_plugin = fx_vst3_audio->vst3_plugin;
  
  g_rec_mutex_unlock(fx_vst3_audio_mutex);

  if(vst3_plugin == NULL ||
     ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    /* unref */
    if(fx_vst3_audio != NULL){
      g_object_unref(fx_vst3_audio);
    }

    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  g_rec_mutex_lock(recall_mutex);

  input_data = fx_vst3_audio->scope_data[sound_scope];

  ags_base_plugin_activate(vst3_plugin,
			   input_data->icomponent);

  ags_vst_icomponent_activate_bus(input_data->icomponent,
				  AGS_VST_KAUDIO, AGS_VST_KINPUT,
				  0,
				  TRUE);

  ags_vst_icomponent_activate_bus(input_data->icomponent,
				  AGS_VST_KAUDIO, AGS_VST_KOUTPUT,
				  0,
				  TRUE);

  ags_vst_icomponent_activate_bus(input_data->icomponent,
				  AGS_VST_KEVENT, AGS_VST_KINPUT,
				  0,
				  TRUE);

  g_rec_mutex_unlock(recall_mutex);
      
  /* unref */
  if(fx_vst3_audio != NULL){
    g_object_unref(fx_vst3_audio);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_vst3_channel_processor_parent_class)->run_init_pre(recall);
}

void
ags_fx_vst3_channel_processor_done(AgsRecall *recall)
{
  AgsFxVst3Audio *fx_vst3_audio;
  
  AgsVst3Plugin *vst3_plugin;

  AgsFxVst3ChannelInputData *input_data;

  guint sound_scope;
  gboolean is_live_instrument;

  guint j, k;
  GRecMutex *recall_mutex;
  
  GRecMutex *fx_vst3_audio_mutex;
  
  fx_vst3_audio = NULL;

  g_object_get(recall,
	       "recall-audio", &fx_vst3_audio,
	       NULL);

  sound_scope = ags_recall_get_sound_scope(recall);

  /* get VST3 plugin */
  fx_vst3_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  is_live_instrument = ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);

  g_rec_mutex_lock(fx_vst3_audio_mutex);

  vst3_plugin = fx_vst3_audio->vst3_plugin;
  
  g_rec_mutex_unlock(fx_vst3_audio_mutex);

  if(vst3_plugin == NULL ||
     ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    /* unref */
    if(fx_vst3_audio != NULL){
      g_object_unref(fx_vst3_audio);
    }

    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  g_rec_mutex_lock(recall_mutex);

  input_data = fx_vst3_audio->scope_data[sound_scope];

  ags_base_plugin_deactivate(vst3_plugin,
			     input_data->icomponent);

  ags_vst_icomponent_activate_bus(input_data->icomponent,
				  AGS_VST_KAUDIO, AGS_VST_KINPUT,
				  0,
				  FALSE);

  ags_vst_icomponent_activate_bus(input_data->icomponent,
				  AGS_VST_KAUDIO, AGS_VST_KOUTPUT,
				  0,
				  FALSE);

  ags_vst_icomponent_activate_bus(input_data->icomponent,
				  AGS_VST_KEVENT, AGS_VST_KINPUT,
				  0,
				  FALSE);

  g_rec_mutex_unlock(recall_mutex);
      
  /* unref */
  if(fx_vst3_audio != NULL){
    g_object_unref(fx_vst3_audio);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_vst3_channel_processor_parent_class)->done(recall);
}

/**
 * ags_fx_vst3_channel_processor_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxVst3ChannelProcessor
 *
 * Returns: the new #AgsFxVst3ChannelProcessor
 *
 * Since: 3.10.5
 */
AgsFxVst3ChannelProcessor*
ags_fx_vst3_channel_processor_new(AgsChannel *channel)
{
  AgsFxVst3ChannelProcessor *fx_vst3_channel_processor;

  fx_vst3_channel_processor = (AgsFxVst3ChannelProcessor *) g_object_new(AGS_TYPE_FX_VST3_CHANNEL_PROCESSOR,
									 "source", channel,
									 NULL);

  return(fx_vst3_channel_processor);
}
