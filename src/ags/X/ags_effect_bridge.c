/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_bridge_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_output.h>

#include <ags/X/ags_machine.h>
#include <ags/X/ags_effect_pad.h>

void ags_effect_bridge_class_init(AgsEffectBridgeClass *effect_bridge);
void ags_effect_bridge_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_effect_bridge_plugin_interface_init(AgsPluginInterface *plugin);
void ags_effect_bridge_init(AgsEffectBridge *effect_bridge);
void ags_effect_bridge_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_effect_bridge_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_effect_bridge_connect(AgsConnectable *connectable);
void ags_effect_bridge_disconnect(AgsConnectable *connectable);
gchar* ags_effect_bridge_get_name(AgsPlugin *plugin);
void ags_effect_bridge_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_effect_bridge_get_version(AgsPlugin *plugin);
void ags_effect_bridge_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_effect_bridge_get_build_id(AgsPlugin *plugin);
void ags_effect_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id);

void ags_effect_bridge_real_resize_audio_channels(AgsEffectBridge *effect_bridge,
						  guint new_size, guint old_size);
void ags_effect_bridge_real_resize_pads(AgsEffectBridge *effect_bridge,
					GType channel_type,
					guint new_size, guint old_size);

/**
 * SECTION:ags_effect_bridge
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsEffectBridge
 * @section_id:
 * @include: ags/X/ags_effect_bridge.h
 *
 * #AgsEffectBridge is a composite widget to visualize all #AgsChannel. It should be
 * packed by an #AgsMachine.
 */

enum{
  RESIZE_AUDIO_CHANNELS,
  RESIZE_PADS,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_AUDIO,
};

static gpointer ags_effect_bridge_parent_class = NULL;
static guint effect_bridge_signals[LAST_SIGNAL];

GType
ags_effect_bridge_get_type(void)
{
  static GType ags_type_effect_bridge = 0;

  if(!ags_type_effect_bridge){
    static const GTypeInfo ags_effect_bridge_info = {
      sizeof(AgsEffectBridgeClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_effect_bridge_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEffectBridge),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_effect_bridge_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_effect_bridge_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_effect_bridge_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_effect_bridge = g_type_register_static(GTK_TYPE_VBOX,
						    "AgsEffectBridge\0", &ags_effect_bridge_info,
						    0);

    g_type_add_interface_static(ags_type_effect_bridge,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_effect_bridge,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_effect_bridge);
}

void
ags_effect_bridge_class_init(AgsEffectBridgeClass *effect_bridge)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_effect_bridge_parent_class = g_type_class_peek_parent(effect_bridge);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(effect_bridge);

  gobject->set_property = ags_effect_bridge_set_property;
  gobject->get_property = ags_effect_bridge_get_property;

  /* properties */
  /**
   * AgsEffectBridge:audio:
   *
   * The start of a bunch of #AgsAudio to visualize.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("audio\0",
				   "assigned audio\0",
				   "The audio it is assigned with\0",
				   AGS_TYPE_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO,
				  param_spec);

  /* AgsEffectBridgeClass */
  effect_bridge->resize_pads = ags_effect_bridge_real_resize_pads;
  effect_bridge->resize_audio_channels = ags_effect_bridge_real_resize_audio_channels;

  /* signals */
  /**
   * AgsEffectBridge::resize-audio-channels:
   * @effect_bridge: the #AgsEffectBridge to modify
   * @channel: the #AgsChannel to set
   * @new_size: the new size
   * @old_size: the old size
   *
   * The ::resize-audio-channels signal notifies about changed channel allocation within
   * audio.
   */
  effect_bridge_signals[RESIZE_AUDIO_CHANNELS] =
    g_signal_new("resize-audio-channels\0",
		 G_TYPE_FROM_CLASS(effect_bridge),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBridgeClass, resize_pads),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__UINT_UINT,
		 G_TYPE_NONE, 2,
		 G_TYPE_UINT,
		 G_TYPE_UINT);

  /**
   * AgsEffectBridge::resize-pads:
   * @effect_bridge: the #AgsEffectBridge to modify
   * @channel: the #AgsChannel to set
   * @channel_type: either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
   * @new_size: the new size
   * @old_size: the old size
   *
   * The ::resize-pads signal notifies about changed channel allocation within
   * audio.
   */
  effect_bridge_signals[RESIZE_PADS] =
    g_signal_new("resize-pads\0",
		 G_TYPE_FROM_CLASS(effect_bridge),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectBridgeClass, resize_pads),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__ULONG_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_ULONG,
		 G_TYPE_UINT,
		 G_TYPE_UINT);
}

void
ags_effect_bridge_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_effect_bridge_connect;
  connectable->disconnect = ags_effect_bridge_disconnect;
}

void
ags_effect_bridge_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = ags_effect_bridge_get_name;
  plugin->set_name = ags_effect_bridge_set_name;
  plugin->get_version = ags_effect_bridge_get_version;
  plugin->set_version = ags_effect_bridge_set_version;
  plugin->get_build_id = ags_effect_bridge_get_build_id;
  plugin->set_build_id = ags_effect_bridge_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_effect_bridge_init(AgsEffectBridge *effect_bridge)
{
  effect_bridge->flags = 0;

  effect_bridge->name = "ags-default-effect-bridge\0";
  effect_bridge->version = AGS_EFFECT_BRIDGE_DEFAULT_VERSION;
  effect_bridge->build_id = AGS_EFFECT_BRIDGE_DEFAULT_BUILD_ID;

  effect_bridge->audio = NULL;

  /* output */
  effect_bridge->output_pad_type = G_TYPE_NONE;
  effect_bridge->output_line_type = G_TYPE_NONE;
  
  effect_bridge->bulk_output_type = G_TYPE_NONE;

  effect_bridge->bulk_output = NULL;

  effect_bridge->output = NULL;
  
  /* input */
  effect_bridge->input_pad_type = G_TYPE_NONE;
  effect_bridge->input_line_type = G_TYPE_NONE;  

  effect_bridge->bulk_input_type = G_TYPE_NONE;

  effect_bridge->bulk_input = NULL;

  effect_bridge->input = NULL;
}

void
ags_effect_bridge_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = AGS_EFFECT_BRIDGE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
      AgsAudio *audio;

      audio = (AgsAudio *) g_value_get_object(value);

      if(effect_bridge->audio == audio){
	return;
      }

      if(effect_bridge->audio != NULL){
	g_object_unref(effect_bridge->audio);
      }

      if(audio != NULL){
	g_object_ref(audio);

	if((AGS_EFFECT_BRIDGE_CONNECTED & (effect_bridge->flags)) != 0){
	  g_signal_connect_after(G_OBJECT(audio), "set-audio-channels\0",
				 G_CALLBACK(ags_effect_bridge_set_audio_channels_callback), effect_bridge);
	  
	  g_signal_connect_after(G_OBJECT(audio), "set-pads\0",
				 G_CALLBACK(ags_effect_bridge_set_pads_callback), effect_bridge);
	}
      }

      effect_bridge->audio = audio;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_bridge_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = AGS_EFFECT_BRIDGE(gobject);

  switch(prop_id){
  case PROP_AUDIO:
    {
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_bridge_connect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

void
ags_effect_bridge_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

gchar*
ags_effect_bridge_get_name(AgsPlugin *plugin)
{
  return(AGS_EFFECT_BRIDGE(plugin)->name);
}

void
ags_effect_bridge_set_name(AgsPlugin *plugin, gchar *name)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = AGS_EFFECT_BRIDGE(plugin);

  effect_bridge->name = name;
}

gchar*
ags_effect_bridge_get_version(AgsPlugin *plugin)
{
  return(AGS_EFFECT_BRIDGE(plugin)->version);
}

void
ags_effect_bridge_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = AGS_EFFECT_BRIDGE(plugin);

  effect_bridge->version = version;
}

gchar*
ags_effect_bridge_get_build_id(AgsPlugin *plugin)
{
  return(AGS_EFFECT_BRIDGE(plugin)->build_id);
}

void
ags_effect_bridge_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = AGS_EFFECT_BRIDGE(plugin);

  effect_bridge->build_id = build_id;
}

void
ags_effect_bridge_real_resize_audio_channels(AgsEffectBridge *effect_bridge,
					     guint new_size,
					     guint old_size)
{
  GtkTable *table;
  AgsAudio *audio;
  AgsChannel *start, *current;
  GList *list;
  
  audio = effect_bridge->audio;

  if(audio == NULL){
    return;
  }

  /* output */
  list = gtk_container_get_children(effect_bridge->output);

  while(list != NULL){
    ags_effect_pad_resize_lines(AGS_EFFECT_PAD(list->data), effect_bridge->output_line_type,
				new_size, old_size);

    list = list->next;
  }
  
  /* input */
  list = gtk_container_get_children(effect_bridge->input);

  while(list != NULL){
    ags_effect_pad_resize_lines(AGS_EFFECT_PAD(list->data), effect_bridge->input_line_type,
				new_size, old_size);

    list = list->next;
  }
}

void
ags_effect_bridge_resize_audio_channels(AgsEffectBridge *effect_bridge,
					guint new_size,
					guint old_size)
{
  g_return_if_fail(AGS_IS_EFFECT_BRIDGE(effect_bridge));

  g_object_ref((GObject *) effect_bridge);
  g_signal_emit(G_OBJECT(effect_bridge),
		effect_bridge_signals[RESIZE_AUDIO_CHANNELS], 0,
		new_size,
		old_size);
  g_object_unref((GObject *) effect_bridge);
}


void
ags_effect_bridge_real_resize_pads(AgsEffectBridge *effect_bridge,
				   GType channel_type,
				   guint new_size,
				   guint old_size)
{
  AgsEffectPad *effect_pad;
  GtkTable *table;
  AgsAudio *audio;
  AgsChannel *start, *current;
  GList *list, *list_next;
  guint i;
  
  audio = effect_bridge->audio;

  if(audio == NULL){
    return;
  }

  if(new_size > old_size){
    if(channel_type == AGS_TYPE_OUTPUT){
      start =
	current = ags_channel_nth(audio->output,
				  old_size * audio->audio_channels);
    }else{
      start =
	current = ags_channel_nth(audio->input,
				  old_size * audio->audio_channels);
    }
    
    for(i = 0; i < new_size - old_size; i++){
      if(channel_type == AGS_TYPE_OUTPUT){
	effect_pad = g_object_new(effect_bridge->output_pad_type,
				  "channel\0", current,
				  NULL);
	ags_effect_pad_resize_lines(effect_pad, effect_bridge->output_line_type,
				    audio->audio_channels, 0);
	gtk_container_add(GTK_CONTAINER(effect_bridge->output),
			  GTK_WIDGET(effect_pad));
      }else{
	effect_pad = g_object_new(effect_bridge->input_pad_type,
				  "channel\0", current,
				  NULL);
	gtk_container_add(GTK_CONTAINER(effect_bridge->input),
			  GTK_WIDGET(effect_pad));
      }

      current = current->next;
    }
  }else{
    if(channel_type == AGS_TYPE_OUTPUT){
      list = gtk_container_get_children(effect_bridge->output);
      list = g_list_nth(list,
			new_size);
    }else{
      list = gtk_container_get_children(effect_bridge->input);
      list = g_list_nth(list,
			new_size);
    }
    
    for(i = 0; i < new_size - old_size; i++){
      list_next = list->next;
      
      gtk_widget_destroy(list->data);

      list = list_next;
    }
  }
}

void
ags_effect_bridge_resize_pads(AgsEffectBridge *effect_bridge,
			      GType channel_type,
			      guint new_size,
			      guint old_size)
{
  g_return_if_fail(AGS_IS_EFFECT_BRIDGE(effect_bridge));

  g_object_ref((GObject *) effect_bridge);
  g_signal_emit(G_OBJECT(effect_bridge),
		effect_bridge_signals[RESIZE_PADS], 0,
		channel_type,
		new_size,
		old_size);
  g_object_unref((GObject *) effect_bridge);
}

void
ags_effect_bridge_add_effect(AgsEffectBridge *effect_bridge,
			     GType channel_type,
			     gchar *effect_name,
			     guint nth_line)
{
  //TODO:JK: implement me
}

void
ags_effect_bridge_add_bulk_effect(AgsEffectBridge *effect_bridge,
				  GType channel_type,
				  gchar *effect_name)
{
  //TODO:JK: implement me
}

/**
 * ags_effect_bridge_new:
 * @effect_bridge: the parent effect_bridge
 * @audio: the #AgsAudio to visualize
 *
 * Creates an #AgsEffectBridge
 *
 * Returns: a new #AgsEffectBridge
 *
 * Since: 0.4
 */
AgsEffectBridge*
ags_effect_bridge_new(AgsAudio *audio)
{
  AgsEffectBridge *effect_bridge;

  effect_bridge = (AgsEffectBridge *) g_object_new(AGS_TYPE_EFFECT_BRIDGE,
						   "audio\0", audio,
						   NULL);

  return(effect_bridge);
}
