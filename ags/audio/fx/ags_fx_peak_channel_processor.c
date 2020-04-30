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

#include <ags/audio/fx/ags_fx_peak_channel_processor.h>

#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/fx/ags_fx_peak_channel.h>
#include <ags/audio/fx/ags_fx_peak_recycling.h>

#include <ags/i18n.h>

void ags_fx_peak_channel_processor_class_init(AgsFxPeakChannelProcessorClass *fx_peak_channel_processor);
void ags_fx_peak_channel_processor_init(AgsFxPeakChannelProcessor *fx_peak_channel_processor);
void ags_fx_peak_channel_processor_dispose(GObject *gobject);
void ags_fx_peak_channel_processor_finalize(GObject *gobject);

void ags_fx_peak_channel_processor_real_run_inter(AgsRecall *recall);

/**
 * SECTION:ags_fx_peak_channel_processor
 * @short_description: fx peak channel processor
 * @title: AgsFxPeakChannelProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_peak_channel_processor.h
 *
 * The #AgsFxPeakChannelProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_peak_channel_processor_parent_class = NULL;

static const gchar *ags_fx_peak_channel_processor_plugin_name = "ags-fx-peak";

GType
ags_fx_peak_channel_processor_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_peak_channel_processor = 0;

    static const GTypeInfo ags_fx_peak_channel_processor_info = {
      sizeof (AgsFxPeakChannelProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_peak_channel_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel_processor */
      sizeof (AgsFxPeakChannelProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_peak_channel_processor_init,
    };

    ags_type_fx_peak_channel_processor = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
								"AgsFxPeakChannelProcessor",
								&ags_fx_peak_channel_processor_info,
								0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_peak_channel_processor);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_peak_channel_processor_class_init(AgsFxPeakChannelProcessorClass *fx_peak_channel_processor)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  
  ags_fx_peak_channel_processor_parent_class = g_type_class_peek_parent(fx_peak_channel_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_peak_channel_processor;

  gobject->dispose = ags_fx_peak_channel_processor_dispose;
  gobject->finalize = ags_fx_peak_channel_processor_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_peak_channel_processor;

  recall->run_inter = ags_fx_peak_channel_processor_real_run_inter;
}

void
ags_fx_peak_channel_processor_init(AgsFxPeakChannelProcessor *fx_peak_channel_processor)
{
  AGS_RECALL(fx_peak_channel_processor)->name = "ags-fx-peak";
  AGS_RECALL(fx_peak_channel_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_peak_channel_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_peak_channel_processor)->xml_type = "ags-fx-peak-channel-processor";

  AGS_RECALL(fx_peak_channel_processor)->child_type = AGS_TYPE_FX_PEAK_RECYCLING;
}

void
ags_fx_peak_channel_processor_dispose(GObject *gobject)
{
  AgsFxPeakChannelProcessor *fx_peak_channel_processor;
  
  fx_peak_channel_processor = AGS_FX_PEAK_CHANNEL_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_peak_channel_processor_parent_class)->dispose(gobject);
}

void
ags_fx_peak_channel_processor_finalize(GObject *gobject)
{
  AgsFxPeakChannelProcessor *fx_peak_channel_processor;
  
  fx_peak_channel_processor = AGS_FX_PEAK_CHANNEL_PROCESSOR(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_peak_channel_processor_parent_class)->finalize(gobject);
}

void
ags_fx_peak_channel_processor_real_run_inter(AgsRecall *recall)
{
  AgsFxPeakChannel *fx_peak_channel;
  
  gdouble peak;
  guint buffer_size;
  gint sound_scope;

  GRecMutex *fx_peak_channel_mutex;

  sound_scope = ags_recall_get_sound_scope(recall);
  
  fx_peak_channel = NULL;

  fx_peak_channel_mutex = NULL;

  peak = 0.0;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  g_object_get(recall,
	       "recall-channel", &fx_peak_channel,
	       "buffer-size", &buffer_size,
	       NULL);
    
  if(fx_peak_channel != NULL){
    AgsPort *port;

    gboolean peak_reseted;
    
    GValue value = {0,};

    port = NULL;

    g_object_get(fx_peak_channel,
		 "peak", &port,
		 NULL);
    
    fx_peak_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_peak_channel);
  
    g_rec_mutex_lock(fx_peak_channel_mutex);

    peak_reseted = fx_peak_channel->peak_reseted;

    fx_peak_channel->peak_reseted = TRUE;
    
    if(!peak_reseted &&
       port != NULL){
      g_value_init(&value, G_TYPE_FLOAT);

      g_value_set_float(&value, 0.0);
      
      ags_port_safe_write(port, &value);

      g_value_unset(&value);
    }
  
    peak = ags_audio_buffer_util_peak(fx_peak_channel->input_data[sound_scope]->buffer, 1,
				      AGS_AUDIO_BUFFER_UTIL_DOUBLE,
				      buffer_size,
				      440.0,
				      22000.0,
				      1.0);

    ags_audio_buffer_util_clear_buffer(fx_peak_channel->input_data[sound_scope]->buffer, 1,
				       buffer_size, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    
    g_rec_mutex_unlock(fx_peak_channel_mutex);

    if(port != NULL){
      g_value_init(&value, G_TYPE_FLOAT);

      ags_port_safe_read(port, &value);
      
      g_value_set_float(&value, g_value_get_float(&value) + (gfloat) peak);
      ags_port_safe_write(port, &value);
      
      g_value_unset(&value);
      
      g_object_unref(port);
    }
  }

  /* unref */
  if(fx_peak_channel != NULL){
    g_object_unref(fx_peak_channel);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_peak_channel_processor_parent_class)->run_inter(recall);
}

/**
 * ags_fx_peak_channel_processor_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxPeakChannelProcessor
 *
 * Returns: the new #AgsFxPeakChannelProcessor
 *
 * Since: 3.3.0
 */
AgsFxPeakChannelProcessor*
ags_fx_peak_channel_processor_new(AgsChannel *channel)
{
  AgsFxPeakChannelProcessor *fx_peak_channel_processor;

  fx_peak_channel_processor = (AgsFxPeakChannelProcessor *) g_object_new(AGS_TYPE_FX_PEAK_CHANNEL_PROCESSOR,
									 "source", channel,
									 NULL);

  return(fx_peak_channel_processor);
}
