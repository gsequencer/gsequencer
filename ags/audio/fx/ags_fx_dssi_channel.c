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

#include <ags/audio/fx/ags_fx_dssi_channel.h>

#include <ags/audio/fx/ags_fx_dssi_audio.h>

#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_plugin_port.h>

void ags_fx_dssi_channel_class_init(AgsFxDssiChannelClass *fx_dssi_channel);
void ags_fx_dssi_channel_init(AgsFxDssiChannel *fx_dssi_channel);
void ags_fx_dssi_channel_dispose(GObject *gobject);
void ags_fx_dssi_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_dssi_channel
 * @short_description: fx dssi channel
 * @title: AgsFxDssiChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_dssi_channel.h
 *
 * The #AgsFxDssiChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_dssi_channel_parent_class = NULL;

static const gchar *ags_fx_dssi_channel_plugin_name = "ags-fx-dssi";

GType
ags_fx_dssi_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_dssi_channel = 0;

    static const GTypeInfo ags_fx_dssi_channel_info = {
      sizeof (AgsFxDssiChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_dssi_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxDssiChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_dssi_channel_init,
    };

    ags_type_fx_dssi_channel = g_type_register_static(AGS_TYPE_FX_NOTATION_CHANNEL,
						      "AgsFxDssiChannel",
						      &ags_fx_dssi_channel_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_dssi_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_dssi_channel_class_init(AgsFxDssiChannelClass *fx_dssi_channel)
{
  GObjectClass *gobject;

  ags_fx_dssi_channel_parent_class = g_type_class_peek_parent(fx_dssi_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_dssi_channel;

  gobject->dispose = ags_fx_dssi_channel_dispose;
  gobject->finalize = ags_fx_dssi_channel_finalize;
}

void
ags_fx_dssi_channel_init(AgsFxDssiChannel *fx_dssi_channel)
{
  AGS_RECALL(fx_dssi_channel)->name = "ags-fx-dssi";
  AGS_RECALL(fx_dssi_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_dssi_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_dssi_channel)->xml_type = "ags-fx-dssi-channel";

  fx_dssi_channel->dssi_port = NULL;
}

void
ags_fx_dssi_channel_dispose(GObject *gobject)
{
  AgsFxDssiChannel *fx_dssi_channel;
  
  fx_dssi_channel = AGS_FX_DSSI_CHANNEL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_dssi_channel_parent_class)->dispose(gobject);
}

void
ags_fx_dssi_channel_finalize(GObject *gobject)
{
  AgsFxDssiChannel *fx_dssi_channel;
  
  fx_dssi_channel = AGS_FX_DSSI_CHANNEL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_dssi_channel_parent_class)->finalize(gobject);
}

/**
 * ags_fx_dssi_channel_load_port:
 * @fx_dssi_channel: the #AgsFxDssiChannel
 * 
 * Load port of @fx_dssi_channel.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_channel_load_port(AgsFxDssiChannel *fx_dssi_channel)
{
  AgsFxDssiAudio *fx_dssi_audio;
  
  AgsDssiPlugin *dssi_plugin;
  AgsPort **dssi_port, **iter;

  GList *start_plugin_port, *plugin_port;

  guint control_port_count;
  guint i;
  
  GRecMutex *fx_dssi_audio_mutex;
  GRecMutex *fx_dssi_channel_mutex;

  if(!AGS_IS_FX_DSSI_CHANNEL(fx_dssi_channel)){
    return;
  }

  g_object_get(fx_dssi_channel,
	       "recall-audio", &fx_dssi_audio,
	       NULL);

  if(fx_dssi_audio == NULL){
    return;
  }
  
  /* get recall mutex */
  fx_dssi_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);
  fx_dssi_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_channel);

  /* get DSSI plugin */
  g_rec_mutex_lock(fx_dssi_audio_mutex);

  dssi_plugin = fx_dssi_audio->dssi_plugin;
  
  g_rec_mutex_unlock(fx_dssi_audio_mutex);

  /* get DSSI port */
  g_rec_mutex_lock(fx_dssi_channel_mutex);

  dssi_port = fx_dssi_channel->dssi_port;

  g_rec_mutex_unlock(fx_dssi_channel_mutex);

  if(ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT)){
    g_rec_mutex_lock(fx_dssi_channel_mutex);
    
    fx_dssi_channel->dssi_port = NULL;
    
    g_rec_mutex_unlock(fx_dssi_channel_mutex);

    if(dssi_port != NULL){
      for(iter = dssi_port; iter[0] != NULL; iter++){
	g_object_unref(iter[0]);
      }

      g_free(dssi_port);
    }
    
    return;
  }

  if(dssi_port != NULL ||
     dssi_plugin == NULL){
    return;
  }

  g_object_get(dssi_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  /* get control port count */
  plugin_port = start_plugin_port;

  control_port_count = 0;
  
  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data,
				  AGS_PLUGIN_PORT_CONTROL)){
      control_port_count++;
    }

    plugin_port != NULL;
  }

  /*  */
  if(control_port_count > 0){
    dssi_port = (AgsPort **) g_malloc((control_port_count + 1) * sizeof(AgsPort *));

    plugin_port = start_plugin_port;
    
    for(i = 0; i < control_port_count; i++){
      AgsPluginPort *current_plugin_port;

      gchar *plugin_name;
      gchar *specifier;
      gchar *control_port;
      
      GValue default_value = {0,};

      GRecMutex *plugin_port_mutex;
      
      current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);

      /* get plugin port mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(current_plugin_port);

      /* plugin name, specifier and control port */
      plugin_name = g_strdup_printf("dssi-%u", dssi_plugin->unique_id);

      g_object_get(current_plugin_port,
		   "port-name", &specifier,
		   NULL);

      control_port = g_strdup_printf("%u/%u",
				     i,
				     control_port_count);

      /* default value */
      g_value_init(&default_value,
		   G_TYPE_FLOAT);
      
      g_rec_mutex_lock(plugin_port_mutex);
      
      g_value_copy(current_plugin_port->default_value,
		   &default_value);
      
      g_rec_mutex_unlock(plugin_port_mutex);

      /* dssi port */
      dssi_port[i] = g_object_new(AGS_TYPE_PORT,
				  "plugin-name", plugin_name,
				  "specifier", specifier,
				  "control-port", control_port,
				  "port-value-is-pointer", FALSE,
				  "port-value-type", G_TYPE_FLOAT,
				  NULL);
      ags_port_set_flags(dssi_port[i], AGS_PORT_USE_LADSPA_FLOAT);
      g_object_ref(dssi_port[i]);
      
      if(ags_plugin_port_test_flags(current_plugin_port,
				    AGS_PLUGIN_PORT_OUTPUT)){
	ags_port_set_flags(dssi_port[i], AGS_PORT_IS_OUTPUT);
	  
	ags_recall_set_flags((AgsRecall *) fx_dssi_channel,
			     AGS_RECALL_HAS_OUTPUT_PORT);
	
      }else{
	if(!ags_plugin_port_test_flags(current_plugin_port,
				       AGS_PLUGIN_PORT_INTEGER) &&
	   !ags_plugin_port_test_flags(current_plugin_port,
				       AGS_PLUGIN_PORT_TOGGLED)){
	  ags_port_set_flags(dssi_port[i], AGS_PORT_INFINITE_RANGE);
	}
      }
	
      g_object_set(dssi_port[i],
		   "plugin-port", current_plugin_port,
		   NULL);

      ags_port_util_load_ladspa_conversion(dssi_port[i],
					   current_plugin_port);
	
      ags_port_safe_write_raw(dssi_port[i],
			      &default_value);

      g_free(plugin_name);
      g_free(specifier);
      g_free(control_port);

      g_value_unset(&default_value);
    }

    dssi_port[i] = NULL;
  }

  /* set DSSI port */
  g_rec_mutex_lock(fx_dssi_channel_mutex);

  fx_dssi_channel->dssi_port = dssi_port;
  
  g_rec_mutex_unlock(fx_dssi_channel_mutex);

  /* unref */
  g_object_unref(fx_dssi_audio);

  g_list_free_full(start_plugin_port,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_fx_dssi_channel_unload_port:
 * @fx_dssi_channel: the #AgsFxDssiChannel
 * 
 * Unload port of @fx_dssi_channel.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_channel_unload_port(AgsFxDssiChannel *fx_dssi_channel)
{
  AgsPort **dssi_port, **iter;

  guint i;

  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_DSSI_CHANNEL(fx_dssi_channel)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_channel);

  g_rec_mutex_lock(recall_mutex);

  dssi_port = fx_dssi_channel->dssi_port;
  
  g_rec_mutex_unlock(recall_mutex);

  if(dssi_port == NULL){
    return;
  }
  
  g_rec_mutex_lock(recall_mutex);

  fx_dssi_channel->dssi_port = NULL;

  g_rec_mutex_unlock(recall_mutex);

  for(iter = dssi_port; iter[0] != NULL; iter++){
    g_object_unref(iter[0]);
  }

  g_free(dssi_port);
}

/**
 * ags_fx_dssi_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxDssiChannel
 *
 * Returns: the new #AgsFxDssiChannel
 *
 * Since: 3.3.0
 */
AgsFxDssiChannel*
ags_fx_dssi_channel_new(AgsChannel *channel)
{
  AgsFxDssiChannel *fx_dssi_channel;

  fx_dssi_channel = (AgsFxDssiChannel *) g_object_new(AGS_TYPE_FX_DSSI_CHANNEL,
						      "channel", channel,
						      NULL);

  return(fx_dssi_channel);
}
