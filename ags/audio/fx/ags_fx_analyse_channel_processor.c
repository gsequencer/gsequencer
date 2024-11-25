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

#include <ags/audio/fx/ags_fx_analyse_channel_processor.h>

#include <ags/audio/fx/ags_fx_analyse_channel.h>
#include <ags/audio/fx/ags_fx_analyse_recycling.h>

#include <ags/i18n.h>

void ags_fx_analyse_channel_processor_class_init(AgsFxAnalyseChannelProcessorClass *fx_analyse_channel_processor);
void ags_fx_analyse_channel_processor_init(AgsFxAnalyseChannelProcessor *fx_analyse_channel_processor);
void ags_fx_analyse_channel_processor_dispose(GObject *gobject);
void ags_fx_analyse_channel_processor_finalize(GObject *gobject);

void ags_fx_analyse_channel_processor_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_analyse_channel_processor
 * @short_description: fx analyse channel processor
 * @title: AgsFxAnalyseChannelProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_analyse_channel_processor.h
 *
 * The #AgsFxAnalyseChannelProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_analyse_channel_processor_parent_class = NULL;

const gchar *ags_fx_analyse_channel_processor_plugin_name = "ags-fx-analyse";

GType
ags_fx_analyse_channel_processor_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_analyse_channel_processor = 0;

    static const GTypeInfo ags_fx_analyse_channel_processor_info = {
      sizeof (AgsFxAnalyseChannelProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_analyse_channel_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel_processor */
      sizeof (AgsFxAnalyseChannelProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_analyse_channel_processor_init,
    };

    ags_type_fx_analyse_channel_processor = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
								   "AgsFxAnalyseChannelProcessor",
								   &ags_fx_analyse_channel_processor_info,
								   0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_analyse_channel_processor);
  }

  return g_define_type_id__static;
}

void
ags_fx_analyse_channel_processor_class_init(AgsFxAnalyseChannelProcessorClass *fx_analyse_channel_processor)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_fx_analyse_channel_processor_parent_class = g_type_class_peek_parent(fx_analyse_channel_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_analyse_channel_processor;

  gobject->dispose = ags_fx_analyse_channel_processor_dispose;
  gobject->finalize = ags_fx_analyse_channel_processor_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_analyse_channel_processor;

  recall->run_inter = ags_fx_analyse_channel_processor_real_run_inter;
}

void
ags_fx_analyse_channel_processor_init(AgsFxAnalyseChannelProcessor *fx_analyse_channel_processor)
{
  AGS_RECALL(fx_analyse_channel_processor)->name = "ags-fx-analyse";
  AGS_RECALL(fx_analyse_channel_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_analyse_channel_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_analyse_channel_processor)->xml_type = "ags-fx-analyse-channel-processor";

  AGS_RECALL(fx_analyse_channel_processor)->child_type = AGS_TYPE_FX_ANALYSE_RECYCLING;
}

void
ags_fx_analyse_channel_processor_dispose(GObject *gobject)
{
  AgsFxAnalyseChannelProcessor *fx_analyse_channel_processor;
  
  fx_analyse_channel_processor = AGS_FX_ANALYSE_CHANNEL_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_analyse_channel_processor_parent_class)->dispose(gobject);
}

void
ags_fx_analyse_channel_processor_finalize(GObject *gobject)
{
  AgsFxAnalyseChannelProcessor *fx_analyse_channel_processor;
  
  fx_analyse_channel_processor = AGS_FX_ANALYSE_CHANNEL_PROCESSOR(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_analyse_channel_processor_parent_class)->finalize(gobject);
}

void
ags_fx_analyse_channel_processor_real_run_inter(AgsRecall *recall)
{
  AgsFxAnalyseChannel *fx_analyse_channel;
  AgsFxAnalyseChannelProcessor *fx_analyse_channel_processor;
  
  guint buffer_size;
  gint sound_scope;
  guint i;

  GRecMutex *fx_analyse_channel_mutex;

  fx_analyse_channel_processor = (AgsFxAnalyseChannelProcessor *) recall;
  
  sound_scope = ags_recall_get_sound_scope(recall);
  
  fx_analyse_channel = NULL;

  fx_analyse_channel_mutex = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  g_object_get(recall,
	       "recall-channel", &fx_analyse_channel,
	       "buffer-size", &buffer_size,
	       NULL);

  if(fx_analyse_channel != NULL &&
     sound_scope >= 0 &&
     sound_scope < AGS_SOUND_SCOPE_LAST &&
     buffer_size > 0){
    AgsPort *magnitude;

    gboolean magnitude_cleared;
    
    GRecMutex *port_mutex;
    
    magnitude = NULL;
    
    g_object_get(fx_analyse_channel,
		 "magnitude", &magnitude,
		 NULL);

    fx_analyse_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_analyse_channel);

    g_rec_mutex_lock(fx_analyse_channel_mutex);

    magnitude_cleared = fx_analyse_channel->magnitude_cleared;

    fx_analyse_channel->magnitude_cleared = TRUE;
    
    if(!magnitude_cleared &&
       magnitude != NULL){
      port_mutex = AGS_PORT_GET_OBJ_MUTEX(magnitude);

      g_rec_mutex_lock(port_mutex);

      ags_audio_buffer_util_clear_buffer(&(fx_analyse_channel_processor->audio_buffer_util),
					 magnitude->port_value.ags_port_double_ptr, 1,
					 buffer_size, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
      
      g_rec_mutex_unlock(port_mutex);
    }

    memset((void *) fx_analyse_channel->input_data[sound_scope]->out, 0, buffer_size * sizeof(double));
    
    fftw_execute(fx_analyse_channel->input_data[sound_scope]->plan);

    memset((void *) fx_analyse_channel->input_data[sound_scope]->in, 0, buffer_size * sizeof(double));

    g_rec_mutex_unlock(fx_analyse_channel_mutex);

    if(magnitude != NULL){
      port_mutex = AGS_PORT_GET_OBJ_MUTEX(magnitude);

      g_rec_mutex_lock(port_mutex);
      
      for(i = 0; i < buffer_size; i++){
	magnitude->port_value.ags_port_double_ptr[i] += fx_analyse_channel->input_data[sound_scope]->out[i];
      }

      g_rec_mutex_unlock(port_mutex);
    }

    if(magnitude != NULL){    
      g_object_unref(magnitude);
    }
  }

  /* unref */
  if(fx_analyse_channel != NULL){
    g_object_unref(fx_analyse_channel);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_analyse_channel_processor_parent_class)->run_inter(recall);
}

/**
 * ags_fx_analyse_channel_processor_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxAnalyseChannelProcessor
 *
 * Returns: the new #AgsFxAnalyseChannelProcessor
 *
 * Since: 3.3.0
 */
AgsFxAnalyseChannelProcessor*
ags_fx_analyse_channel_processor_new(AgsChannel *channel)
{
  AgsFxAnalyseChannelProcessor *fx_analyse_channel_processor;

  fx_analyse_channel_processor = (AgsFxAnalyseChannelProcessor *) g_object_new(AGS_TYPE_FX_ANALYSE_CHANNEL_PROCESSOR,
									       "source", channel,
									       NULL);

  return(fx_analyse_channel_processor);
}
