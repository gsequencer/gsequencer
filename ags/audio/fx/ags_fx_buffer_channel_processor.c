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

#include <ags/audio/fx/ags_fx_buffer_channel_processor.h>

#include <ags/audio/fx/ags_fx_buffer_channel.h>
#include <ags/audio/fx/ags_fx_buffer_recycling.h>

#include <ags/i18n.h>

void ags_fx_buffer_channel_processor_class_init(AgsFxBufferChannelProcessorClass *fx_buffer_channel_processor);
void ags_fx_buffer_channel_processor_init(AgsFxBufferChannelProcessor *fx_buffer_channel_processor);
void ags_fx_buffer_channel_processor_dispose(GObject *gobject);
void ags_fx_buffer_channel_processor_finalize(GObject *gobject);

void ags_fx_buffer_channel_processor_run_init_pre(AgsRecall *recall);

/**
 * SECTION:ags_fx_buffer_channel_processor
 * @short_description: fx buffer channel processor
 * @title: AgsFxBufferChannelProcessor
 * @section_id:
 * @include: ags/audio/fx/ags_fx_buffer_channel_processor.h
 *
 * The #AgsFxBufferChannelProcessor class provides ports to the effect processor.
 */

static gpointer ags_fx_buffer_channel_processor_parent_class = NULL;

static const gchar *ags_fx_buffer_channel_processor_plugin_name = "ags-fx-buffer";

GType
ags_fx_buffer_channel_processor_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_buffer_channel_processor = 0;

    static const GTypeInfo ags_fx_buffer_channel_processor_info = {
      sizeof (AgsFxBufferChannelProcessorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_buffer_channel_processor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel_processor */
      sizeof (AgsFxBufferChannelProcessor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_buffer_channel_processor_init,
    };

    ags_type_fx_buffer_channel_processor = g_type_register_static(AGS_TYPE_RECALL_CHANNEL_RUN,
								  "AgsFxBufferChannelProcessor",
								  &ags_fx_buffer_channel_processor_info,
								  0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_buffer_channel_processor);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_buffer_channel_processor_class_init(AgsFxBufferChannelProcessorClass *fx_buffer_channel_processor)
{
  AgsRecallClass *recall;
  GObjectClass *gobject;

  ags_fx_buffer_channel_processor_parent_class = g_type_class_peek_parent(fx_buffer_channel_processor);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_buffer_channel_processor;

  gobject->dispose = ags_fx_buffer_channel_processor_dispose;
  gobject->finalize = ags_fx_buffer_channel_processor_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) fx_buffer_channel_processor;

  recall->run_init_pre = ags_fx_buffer_channel_processor_run_init_pre;
}

void
ags_fx_buffer_channel_processor_init(AgsFxBufferChannelProcessor *fx_buffer_channel_processor)
{
  AGS_RECALL(fx_buffer_channel_processor)->name = "ags-fx-buffer";
  AGS_RECALL(fx_buffer_channel_processor)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_buffer_channel_processor)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_buffer_channel_processor)->xml_type = "ags-fx-buffer-channel-processor";

  AGS_RECALL(fx_buffer_channel_processor)->child_type = AGS_TYPE_FX_BUFFER_RECYCLING;
}

void
ags_fx_buffer_channel_processor_dispose(GObject *gobject)
{
  AgsFxBufferChannelProcessor *fx_buffer_channel_processor;
  
  fx_buffer_channel_processor = AGS_FX_BUFFER_CHANNEL_PROCESSOR(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_buffer_channel_processor_parent_class)->dispose(gobject);
}

void
ags_fx_buffer_channel_processor_finalize(GObject *gobject)
{
  AgsFxBufferChannelProcessor *fx_buffer_channel_processor;
  
  fx_buffer_channel_processor = AGS_FX_BUFFER_CHANNEL_PROCESSOR(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_buffer_channel_processor_parent_class)->finalize(gobject);
}

void
ags_fx_buffer_channel_processor_run_init_pre(AgsRecall *recall)
{
  AgsFxBufferChannel *fx_buffer_channel;

  gint sound_scope;

  GRecMutex *fx_buffer_channel_mutex;

  fx_buffer_channel = NULL;
  fx_buffer_channel_mutex = NULL;

  sound_scope = ags_recall_get_sound_scope(recall);

  g_object_get(recall,
	       "recall-channel", &fx_buffer_channel,
	       NULL);
  
  if(sound_scope >= 0 &&
     sound_scope < AGS_SOUND_SCOPE_LAST &&
     fx_buffer_channel != NULL){
    fx_buffer_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_buffer_channel);

    g_rec_mutex_lock(fx_buffer_channel_mutex);
      
    g_hash_table_remove_all(fx_buffer_channel->input_data[sound_scope]->destination);
      
    g_rec_mutex_unlock(fx_buffer_channel_mutex);
  }

  if(fx_buffer_channel != NULL){
    g_object_unref(fx_buffer_channel);
  }
  
  /* call parent */
  AGS_RECALL_CLASS(ags_fx_buffer_channel_processor_parent_class)->run_init_pre(recall);
}

/**
 * ags_fx_buffer_channel_processor_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxBufferChannelProcessor
 *
 * Returns: the new #AgsFxBufferChannelProcessor
 *
 * Since: 3.3.0
 */
AgsFxBufferChannelProcessor*
ags_fx_buffer_channel_processor_new(AgsChannel *channel)
{
  AgsFxBufferChannelProcessor *fx_buffer_channel_processor;

  fx_buffer_channel_processor = (AgsFxBufferChannelProcessor *) g_object_new(AGS_TYPE_FX_BUFFER_CHANNEL_PROCESSOR,
									     "source", channel,
									     NULL);

  return(fx_buffer_channel_processor);
}
