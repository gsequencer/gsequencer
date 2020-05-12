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
 * Returns: (type gpointer) (transfer full): the new #AgsFxBufferChannelInputData-struct
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
#if 0
						  g_object_unref,
						  g_object_unref);
#else
  NULL,
    NULL);
#endif

return(input_data);
}

/**
 * ags_fx_buffer_channel_input_data_free:
 * @input_data: (type gpointer) (transfer full): the #AgsFxBufferChannelInputData-struct
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
 * ags_fx_buffer_channel_input_data_get_strct_mutex:
 * @input_data: (type gpointer) (transfer none): the #AgsFxBufferChannelInputData
 * 
 * Get structure mutex.
 * 
 * Returns: (type gpointer) (transfer none): the #GRecMutex to lock @input_data
 * 
 * Since: 3.3.0
 */
GRecMutex*
ags_fx_buffer_channel_input_data_get_strct_mutex(AgsFxBufferChannelInputData *input_data)
{
  if(input_data == NULL){
    return(NULL);
  }

  return(AGS_FX_BUFFER_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(input_data));
}

/**
 * ags_fx_buffer_channel_input_get_parent:
 * @input_data: (type gpointer) (transfer none): the #AgsFxBufferChannelInputData-struct
 * 
 * Get parent of @input_data.
 * 
 * Returns: (type gpointer) (transfer none): the parent
 * 
 * Since: 3.3.0
 */
gpointer
ags_fx_buffer_channel_input_get_parent(AgsFxBufferChannelInputData *input_data)
{
  gpointer parent;
  
  GRecMutex *input_data_mutex;
  
  if(input_data == NULL){
    return(NULL);
  }

  input_data_mutex = AGS_FX_BUFFER_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(input_data);

  /* parent */
  g_rec_mutex_lock(input_data_mutex);

  parent = input_data->parent;
  
  g_rec_mutex_unlock(input_data_mutex);

  return(parent);
}

/**
 * ags_fx_buffer_channel_input_get_destination:
 * @input_data: (type gpointer) (transfer none): the #AgsFxBufferChannelInputData-struct
 * 
 * Get destination of @input_data.
 * 
 * Returns: (element-type AgsAudio.Recycling AgsAudio.AudioSignal) (transfer none): the destination
 * 
 * Since: 3.3.0
 */
gpointer
ags_fx_buffer_channel_input_get_destination(AgsFxBufferChannelInputData *input_data)
{
  gpointer destination;
  
  GRecMutex *input_data_mutex;
  
  if(input_data == NULL){
    return(NULL);
  }

  input_data_mutex = AGS_FX_BUFFER_CHANNEL_INPUT_DATA_GET_STRCT_MUTEX(input_data);

  /* destination */
  g_rec_mutex_lock(input_data_mutex);

  destination = input_data->destination;
  
  g_rec_mutex_unlock(input_data_mutex);

  return(destination);
}

/**
 * ags_fx_buffer_channel_get_input_data:
 * @fx_buffer_channel: the #AgsFxBufferChannel
 * @sound_scope: the sound scope
 * 
 * Get input data from @fx_buffer_channel by @sound_scope.
 * 
 * Returns: (type gpointer) (transfer none): the matching #AgsFxBufferChannelInputData-struct
 * 
 * Since: 3.3.0
 */
AgsFxBufferChannelInputData*
ags_fx_buffer_channel_get_input_data(AgsFxBufferChannel *fx_buffer_channel,
				     gint sound_scope)
{
  AgsFxBufferChannelInputData *input_data;
  
  GRecMutex *fx_buffer_channel_mutex;
  
  if(!AGS_IS_FX_BUFFER_CHANNEL(fx_buffer_channel) ||
     sound_scope < 0 ||
     sound_scope >= AGS_SOUND_SCOPE_LAST){
    return(NULL);
  }

  fx_buffer_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_buffer_channel);

  /* input data */
  g_rec_mutex_lock(fx_buffer_channel_mutex);

  input_data = fx_buffer_channel->input_data[sound_scope];
  
  g_rec_mutex_unlock(fx_buffer_channel_mutex);

  return(input_data);
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
