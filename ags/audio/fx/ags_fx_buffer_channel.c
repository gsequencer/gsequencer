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

#include <ags/audio/fx/ags_fx_buffer_channel.h>

#include <ags/i18n.h>

void ags_fx_buffer_channel_class_init(AgsFxBufferChannelClass *fx_buffer_channel);
void ags_fx_buffer_channel_init(AgsFxBufferChannel *fx_buffer_channel);
void ags_fx_buffer_channel_dispose(GObject *gobject);
void ags_fx_buffer_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_buffer_channel
 * @short_description: fx buffer channel
 * @title: AgsFxBufferChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_buffer_channel.h
 *
 * The #AgsFxBufferChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_buffer_channel_parent_class = NULL;

static const gchar *ags_fx_buffer_channel_plugin_name = "ags-fx-buffer";

GType
ags_fx_buffer_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_buffer_channel = 0;

    static const GTypeInfo ags_fx_buffer_channel_info = {
      sizeof (AgsFxBufferChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_buffer_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxBufferChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_buffer_channel_init,
    };

    ags_type_fx_buffer_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							"AgsFxBufferChannel",
							&ags_fx_buffer_channel_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_buffer_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_buffer_channel_class_init(AgsFxBufferChannelClass *fx_buffer_channel)
{
  GObjectClass *gobject;

  ags_fx_buffer_channel_parent_class = g_type_class_peek_parent(fx_buffer_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_buffer_channel;

  gobject->dispose = ags_fx_buffer_channel_dispose;
  gobject->finalize = ags_fx_buffer_channel_finalize;
}

void
ags_fx_buffer_channel_init(AgsFxBufferChannel *fx_buffer_channel)
{
  guint i;
  
  AGS_RECALL(fx_buffer_channel)->name = "ags-fx-buffer";
  AGS_RECALL(fx_buffer_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_buffer_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_buffer_channel)->xml_type = "ags-fx-buffer-channel";

  /* input data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    fx_buffer_channel->input_data[i] = ags_fx_buffer_channel_input_data_alloc();
      
    fx_buffer_channel->input_data[i]->parent = fx_buffer_channel;
  }
}

void
ags_fx_buffer_channel_dispose(GObject *gobject)
{
  AgsFxBufferChannel *fx_buffer_channel;
  
  fx_buffer_channel = AGS_FX_BUFFER_CHANNEL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_buffer_channel_parent_class)->dispose(gobject);
}

void
ags_fx_buffer_channel_finalize(GObject *gobject)
{
  AgsFxBufferChannel *fx_buffer_channel;
  
  guint i;
  
  fx_buffer_channel = AGS_FX_BUFFER_CHANNEL(gobject);

 /* input data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    ags_fx_buffer_channel_input_data_free(fx_buffer_channel->input_data[i]);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_buffer_channel_parent_class)->finalize(gobject);
}

/**
 * ags_fx_buffer_channel_input_data_alloc:
 * 
 * Allocate #AgsFxBufferChannelInputData-struct
 * 
 * Returns: the new #AgsFxBufferChannelInputData-struct
 * 
 * Since: 3.3.0
 */
AgsFxBufferChannelInputData*
ags_fx_buffer_channel_input_data_alloc()
{
  AgsFxBufferChannelInputData *input_data;

  input_data = (AgsFxBufferChannelInputData *) g_malloc(sizeof(AgsFxBufferChannelInputData));

  input_data->parent = NULL;

  input_data->destination = g_hash_table_new_full(g_direct_hash, g_direct_equal,
						  g_object_unref,
						  g_object_unref);
  
  return(input_data);
}

/**
 * ags_fx_buffer_channel_input_data_free:
 * @input_data: the #AgsFxBufferChannelInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 3.3.0
 */
void
ags_fx_buffer_channel_input_data_free(AgsFxBufferChannelInputData *input_data)
{
  if(input_data == NULL){
    return;
  }

  g_hash_table_destroy(input_data->destination);

  g_free(input_data);
}

/**
 * ags_fx_buffer_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxBufferChannel
 *
 * Returns: the new #AgsFxBufferChannel
 *
 * Since: 3.3.0
 */
AgsFxBufferChannel*
ags_fx_buffer_channel_new(AgsChannel *channel)
{
  AgsFxBufferChannel *fx_buffer_channel;

  fx_buffer_channel = (AgsFxBufferChannel *) g_object_new(AGS_TYPE_FX_BUFFER_CHANNEL,
							  "source", channel,
							  NULL);

  return(fx_buffer_channel);
}
