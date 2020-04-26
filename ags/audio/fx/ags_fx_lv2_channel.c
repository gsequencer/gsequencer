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

#include <ags/audio/fx/ags_fx_lv2_channel.h>

#include <ags/i18n.h>

void ags_fx_lv2_channel_class_init(AgsFxLv2ChannelClass *fx_lv2_channel);
void ags_fx_lv2_channel_init(AgsFxLv2Channel *fx_lv2_channel);
void ags_fx_lv2_channel_dispose(GObject *gobject);
void ags_fx_lv2_channel_finalize(GObject *gobject);

void ags_fx_lv2_channel_notify_buffer_size_callback(GObject *gobject,
						    GParamSpec *pspec,
						    gpointer user_data);
void ags_fx_lv2_channel_notify_samplerate_callback(GObject *gobject,
						   GParamSpec *pspec,
						   gpointer user_data);

/**
 * SECTION:ags_fx_lv2_channel
 * @short_description: fx lv2 channel
 * @title: AgsFxLv2Channel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_lv2_channel.h
 *
 * The #AgsFxLv2Channel class provides ports to the effect processor.
 */

static gpointer ags_fx_lv2_channel_parent_class = NULL;

static const gchar *ags_fx_lv2_channel_plugin_name = "ags-fx-lv2";

GType
ags_fx_lv2_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_lv2_channel = 0;

    static const GTypeInfo ags_fx_lv2_channel_info = {
      sizeof (AgsFxLv2ChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_lv2_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxLv2Channel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_lv2_channel_init,
    };

    ags_type_fx_lv2_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						     "AgsFxLv2Channel",
						     &ags_fx_lv2_channel_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_lv2_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_lv2_channel_class_init(AgsFxLv2ChannelClass *fx_lv2_channel)
{
  GObjectClass *gobject;

  ags_fx_lv2_channel_parent_class = g_type_class_peek_parent(fx_lv2_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_lv2_channel;

  gobject->dispose = ags_fx_lv2_channel_dispose;
  gobject->finalize = ags_fx_lv2_channel_finalize;
}

void
ags_fx_lv2_channel_init(AgsFxLv2Channel *fx_lv2_channel)
{
  g_signal_connect(fx_lv2_channel, "notify::buffer-size",
		   G_CALLBACK(ags_fx_lv2_channel_notify_buffer_size_callback), NULL);

  g_signal_connect(fx_lv2_channel, "notify::samplerate",
		   G_CALLBACK(ags_fx_lv2_channel_notify_samplerate_callback), NULL);

  AGS_RECALL(fx_lv2_channel)->name = "ags-fx-lv2";
  AGS_RECALL(fx_lv2_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_lv2_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_lv2_channel)->xml_type = "ags-fx-lv2-channel";
}

void
ags_fx_lv2_channel_dispose(GObject *gobject)
{
  AgsFxLv2Channel *fx_lv2_channel;
  
  fx_lv2_channel = AGS_FX_LV2_CHANNEL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_lv2_channel_parent_class)->dispose(gobject);
}

void
ags_fx_lv2_channel_finalize(GObject *gobject)
{
  AgsFxLv2Channel *fx_lv2_channel;
  
  fx_lv2_channel = AGS_FX_LV2_CHANNEL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_lv2_channel_parent_class)->finalize(gobject);
}

void
ags_fx_lv2_channel_notify_buffer_size_callback(GObject *gobject,
					       GParamSpec *pspec,
					       gpointer user_data)
{
  //TODO:JK: implement me
}

void
ags_fx_lv2_channel_notify_samplerate_callback(GObject *gobject,
					      GParamSpec *pspec,
					      gpointer user_data)
{
  //TODO:JK: implement me
}

/**
 * ags_fx_lv2_channel_input_data_alloc:
 * 
 * Allocate #AgsFxLv2ChannelInputData-struct
 * 
 * Returns: the new #AgsFxLv2ChannelInputData-struct
 * 
 * Since: 3.3.0
 */
AgsFxLv2ChannelInputData*
ags_fx_lv2_channel_input_data_alloc()
{
  AgsFxLv2ChannelInputData *input_data;

  input_data = (AgsFxLv2ChannelInputData *) g_malloc(sizeof(AgsFxLv2ChannelInputData));

  input_data->parent = NULL;

  input_data->output = NULL;
  input_data->input = NULL;

  input_data->lv2_handle = NULL;

  return(input_data);
}

/**
 * ags_fx_lv2_channel_input_data_free:
 * @input_data: the #AgsFxLv2ChannelInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 3.3.0
 */
void
ags_fx_lv2_channel_input_data_free(AgsFxLv2ChannelInputData *input_data)
{
  AgsLv2Plugin *lv2_plugin;
  
  if(input_data == NULL){
    return;
  }

  g_free(input_data->output);
  g_free(input_data->input);

  if(input_data->lv2_handle != NULL){
    gpointer plugin_descriptor;

    GRecMutex *base_plugin_mutex;
    
    void (*deactivate)(LV2_Handle Instance);
    void (*cleanup)(LV2_Handle Instance);
  
    lv2_plugin = NULL;
    
    deactivate = NULL;
    cleanup = NULL;

    base_plugin_mutex = NULL;
    
    if(input_data->parent != NULL){
      lv2_plugin = AGS_FX_LV2_CHANNEL(input_data->parent)->lv2_plugin;
    }
    
    if(lv2_plugin != NULL){
      base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2_plugin);
      
      g_rec_mutex_lock(base_plugin_mutex);
  
      plugin_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor;

      deactivate = AGS_LV2_PLUGIN_DESCRIPTOR(plugin_descriptor)->deactivate;
      cleanup = AGS_LV2_PLUGIN_DESCRIPTOR(plugin_descriptor)->cleanup;
      
      g_rec_mutex_unlock(base_plugin_mutex);
    }

    if(deactivate != NULL){
      deactivate(input_data->lv2_handle);
    }

    if(cleanup != NULL){
      cleanup(input_data->lv2_handle);
    }
  }
  
  g_free(input_data);
}

/**
 * ags_fx_lv2_channel_load_plugin:
 * @fx_lv2_channel: the #AgsFxLv2Channel
 * 
 * Load plugin of @fx_lv2_channel.
 * 
 * Since: 3.3.0
 */
void
ags_fx_lv2_channel_load_plugin(AgsFxLv2Channel *fx_lv2_channel)
{
  //TODO:JK: implement me
}

/**
 * ags_fx_lv2_channel_load_port:
 * @fx_lv2_channel: the #AgsFxLv2Channel
 * 
 * Load port of @fx_lv2_channel.
 * 
 * Since: 3.3.0
 */
void
ags_fx_lv2_channel_load_port(AgsFxLv2Channel *fx_lv2_channel)
{
  //TODO:JK: implement me
}

/**
 * ags_fx_lv2_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxLv2Channel
 *
 * Returns: the new #AgsFxLv2Channel
 *
 * Since: 3.3.0
 */
AgsFxLv2Channel*
ags_fx_lv2_channel_new(AgsChannel *channel)
{
  AgsFxLv2Channel *fx_lv2_channel;

  fx_lv2_channel = (AgsFxLv2Channel *) g_object_new(AGS_TYPE_FX_LV2_CHANNEL,
						    "source", channel,
						    NULL);

  return(fx_lv2_channel);
}
