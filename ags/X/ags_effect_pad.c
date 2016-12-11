/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/ags_effect_pad.h>
#include <ags/X/ags_effect_pad_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_plugin.h>

#include <ags/X/ags_effect_bridge.h>
#include <ags/X/ags_effect_line.h>

void ags_effect_pad_class_init(AgsEffectPadClass *effect_pad);
void ags_effect_pad_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_effect_pad_plugin_interface_init(AgsPluginInterface *plugin);
void ags_effect_pad_init(AgsEffectPad *effect_pad);
void ags_effect_pad_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_effect_pad_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_effect_pad_connect(AgsConnectable *connectable);
void ags_effect_pad_disconnect(AgsConnectable *connectable);
gchar* ags_effect_pad_get_name(AgsPlugin *plugin);
void ags_effect_pad_set_name(AgsPlugin *plugin, gchar *name);
gchar* ags_effect_pad_get_version(AgsPlugin *plugin);
void ags_effect_pad_set_version(AgsPlugin *plugin, gchar *version);
gchar* ags_effect_pad_get_build_id(AgsPlugin *plugin);
void ags_effect_pad_set_build_id(AgsPlugin *plugin, gchar *build_id);

void ags_effect_pad_real_resize_lines(AgsEffectPad *effect_pad, GType line_type,
				      guint audio_channels, guint audio_channels_old);
void ags_effect_pad_real_map_recall(AgsEffectPad *effect_pad);
GList* ags_effect_pad_real_find_port(AgsEffectPad *effect_pad);

/**
 * SECTION:ags_effect_pad
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsEffectPad
 * @section_id:
 * @include: ags/X/ags_effect_pad.h
 *
 * #AgsEffectPad is a composite widget to visualize a bunch of #AgsChannel. It should be
 * packed by an #AgsEffectBridge.
 */

enum{
  RESIZE_LINES,
  MAP_RECALL,
  FIND_PORT,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_CHANNEL,
};

static gpointer ags_effect_pad_parent_class = NULL;
static guint effect_pad_signals[LAST_SIGNAL];

GType
ags_effect_pad_get_type(void)
{
  static GType ags_type_effect_pad = 0;

  if(!ags_type_effect_pad){
    static const GTypeInfo ags_effect_pad_info = {
      sizeof(AgsEffectPadClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_effect_pad_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsEffectPad),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_effect_pad_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_effect_pad_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_effect_pad_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_effect_pad = g_type_register_static(GTK_TYPE_VBOX,
						 "AgsEffectPad\0", &ags_effect_pad_info,
						 0);

    g_type_add_interface_static(ags_type_effect_pad,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_effect_pad,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_effect_pad);
}

void
ags_effect_pad_class_init(AgsEffectPadClass *effect_pad)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_effect_pad_parent_class = g_type_class_peek_parent(effect_pad);

  /* GObjectClass */
  gobject = G_OBJECT_CLASS(effect_pad);

  gobject->set_property = ags_effect_pad_set_property;
  gobject->get_property = ags_effect_pad_get_property;

  /* AgsEffectPadClass */
  effect_pad->resize_lines = ags_effect_pad_real_resize_lines;
  
  /* properties */
  /**
   * AgsEffectPad:channel:
   *
   * The start of a bunch of #AgsChannel to visualize.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("channel\0",
				   "assigned channel\0",
				   "The channel it is assigned with\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);


  /* AgsEffectPadClass */
  effect_pad->map_recall = ags_effect_pad_real_map_recall;
  effect_pad->find_port = ags_effect_pad_real_find_port;

  /* signals */
  /**
   * AgsPad::resize-lines:
   * @pad: the #AgsPad to resize
   * @line_type: the channel type
   * @audio_channels: count of lines
   * @audio_channels_old: old count of lines
   *
   * The ::resize-lines is emitted as count of lines pack is modified.
   */
  effect_pad_signals[RESIZE_LINES] =
    g_signal_new("resize-lines\0",
		 G_TYPE_FROM_CLASS(effect_pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectPadClass, resize_lines),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__ULONG_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_ULONG, G_TYPE_UINT, G_TYPE_UINT);

  /**
   * AgsEffectPad::map-recall:
   * @effect_pad: the #AgsEffectPad
   *
   * The ::map-recall should be used to add the effect_pad's default recall.
   */
  effect_pad_signals[MAP_RECALL] =
    g_signal_new("map-recall\0",
                 G_TYPE_FROM_CLASS (effect_pad),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsEffectPadClass, map_recall),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__UINT,
                 G_TYPE_NONE, 0);

  /**
   * AgsEffectPad::find-port:
   * @effect_pad: the #AgsEffectPad to resize
   * Returns: a #GList with associated ports
   *
   * The ::find-port as recall should be mapped
   */
  effect_pad_signals[FIND_PORT] =
    g_signal_new("find-port\0",
		 G_TYPE_FROM_CLASS(effect_pad),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsEffectPadClass, find_port),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);
}

void
ags_effect_pad_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_effect_pad_connect;
  connectable->disconnect = ags_effect_pad_disconnect;
}

void
ags_effect_pad_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->get_name = NULL;
  plugin->set_name = NULL;
  plugin->get_version = ags_effect_pad_get_version;
  plugin->set_version = ags_effect_pad_set_version;
  plugin->get_build_id = ags_effect_pad_get_build_id;
  plugin->set_build_id = ags_effect_pad_set_build_id;
  plugin->get_xml_type = NULL;
  plugin->set_xml_type = NULL;
  plugin->get_ports = NULL;
  plugin->read = NULL;
  plugin->write = NULL;
  plugin->set_ports = NULL;
}

void
ags_effect_pad_init(AgsEffectPad *effect_pad)
{  
  effect_pad->flags = 0;

  effect_pad->name = NULL;
  
  effect_pad->version = AGS_EFFECT_PAD_DEFAULT_VERSION;
  effect_pad->build_id = AGS_EFFECT_PAD_DEFAULT_BUILD_ID;

  effect_pad->channel = NULL;

  effect_pad->cols = AGS_EFFECT_PAD_COLUMNS_COUNT;
  effect_pad->table = (GtkTable *) gtk_table_new(1, AGS_EFFECT_PAD_COLUMNS_COUNT,
						 TRUE);
  gtk_box_pack_start(GTK_BOX(effect_pad),
		     GTK_WIDGET(effect_pad->table),
		     FALSE, TRUE,
		     0);
}

void
ags_effect_pad_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsEffectPad *effect_pad;

  effect_pad = AGS_EFFECT_PAD(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      GList *effect_line, *effect_line_start;
      
      channel = (AgsChannel *) g_value_get_object(value);

      if(effect_pad->channel == channel){
	return;
      }

      if(effect_pad->channel != NULL){
	g_object_unref(effect_pad->channel);
      }

      if(channel != NULL){
	g_object_ref(channel);
      }

      effect_pad->channel = channel;

      effect_line =
	effect_line_start = gtk_container_get_children((GtkContainer *) effect_pad->table);

      while(effect_line != NULL){
	g_object_set(G_OBJECT(effect_line->data),
		     "channel\0", channel,
		     NULL);

	effect_line = effect_line->next;
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_pad_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsEffectPad *effect_pad;

  effect_pad = AGS_EFFECT_PAD(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      g_value_set_object(value,
			 effect_pad->channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_effect_pad_connect(AgsConnectable *connectable)
{
  AgsEffectPad *effect_pad;

  GList *effect_line_list, *effect_line_list_start;

  /* AgsEffect_Pad */
  effect_pad = AGS_EFFECT_PAD(connectable);

  if((AGS_EFFECT_PAD_CONNECTED & (effect_pad->flags)) != 0){
    return;
  }
  
  effect_pad->flags |= AGS_EFFECT_PAD_CONNECTED;

  if((AGS_EFFECT_PAD_PREMAPPED_RECALL & (effect_pad->flags)) == 0){
    if((AGS_EFFECT_PAD_MAPPED_RECALL & (effect_pad->flags)) == 0){
      //TODO:JK: implement me
    }
  }else{
    effect_pad->flags &= (~AGS_EFFECT_PAD_PREMAPPED_RECALL);
  }

  /* AgsEffectLine */
  effect_line_list_start =  
    effect_line_list = gtk_container_get_children(GTK_CONTAINER(effect_pad->table));

  while(effect_line_list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(effect_line_list->data));

    effect_line_list = effect_line_list->next;
  }

  g_list_free(effect_line_list_start);
}

void
ags_effect_pad_disconnect(AgsConnectable *connectable)
{
  AgsEffectPad *effect_pad;

  GList *effect_line_list, *effect_line_list_start;

  /* AgsEffect_Pad */
  effect_pad = AGS_EFFECT_PAD(connectable);

  if((AGS_EFFECT_PAD_CONNECTED & (effect_pad->flags)) == 0){
    return;
  }
  
  effect_pad->flags &= (~AGS_EFFECT_PAD_CONNECTED);

  /* AgsEffectLine */
  effect_line_list_start =  
    effect_line_list = gtk_container_get_children(GTK_CONTAINER(effect_pad->table));

  while(effect_line_list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(effect_line_list->data));

    effect_line_list = effect_line_list->next;
  }

  g_list_free(effect_line_list_start);
}

gchar*
ags_effect_pad_get_name(AgsPlugin *plugin)
{
  return(AGS_EFFECT_PAD(plugin)->name);
}

void
ags_effect_pad_set_name(AgsPlugin *plugin, gchar *name)
{
  AgsEffectPad *effect_pad;

  effect_pad = AGS_EFFECT_PAD(plugin);

  effect_pad->name = name;
}

gchar*
ags_effect_pad_get_version(AgsPlugin *plugin)
{
  return(AGS_EFFECT_PAD(plugin)->version);
}

void
ags_effect_pad_set_version(AgsPlugin *plugin, gchar *version)
{
  AgsEffectPad *effect_pad;

  effect_pad = AGS_EFFECT_PAD(plugin);

  effect_pad->version = version;
}

gchar*
ags_effect_pad_get_build_id(AgsPlugin *plugin)
{
  return(AGS_EFFECT_PAD(plugin)->build_id);
}

void
ags_effect_pad_set_build_id(AgsPlugin *plugin, gchar *build_id)
{
  AgsEffectPad *effect_pad;

  effect_pad = AGS_EFFECT_PAD(plugin);

  effect_pad->build_id = build_id;
}

void
ags_effect_pad_real_resize_lines(AgsEffectPad *effect_pad, GType effect_line_type,
				 guint audio_channels, guint audio_channels_old)
{
  AgsEffectLine *effect_line;

  AgsChannel *channel;

  GList *list, *list_next;

  guint i, j;  
  
  if(audio_channels > audio_channels_old){
    channel = ags_channel_nth(effect_pad->channel,
			      audio_channels_old);

    if(channel == NULL){
      return;
    }
    
    for(i = audio_channels_old; i < audio_channels;){
      for(j = audio_channels_old % effect_pad->cols; j < effect_pad->cols && i < audio_channels; j++, i++){
	effect_line = (AgsEffectLine *) g_object_new(effect_line_type,
						     "channel\0", channel,
						     NULL);
	gtk_table_attach(effect_pad->table,
			 (GtkWidget *) effect_line,
			 j, j + 1,
			 i / effect_pad->cols, i / effect_pad->cols + 1,
			 FALSE, FALSE,
			 0, 0);
	
	channel = channel->next;
      }
    }
  }else{
    list = gtk_container_get_children((GtkContainer *) effect_pad->table);
    list = g_list_nth(list,
		      audio_channels);

    while(list != NULL){
      list_next = list->next;
      
      gtk_widget_destroy(list->data);

      list = list_next;
    }
  }
}

void
ags_effect_pad_resize_lines(AgsEffectPad *effect_pad, GType line_type,
			    guint audio_channels, guint audio_channels_old)
{
  g_return_if_fail(AGS_IS_EFFECT_PAD(effect_pad));
  
  g_object_ref((GObject *) effect_pad);
  g_signal_emit(G_OBJECT(effect_pad),
		effect_pad_signals[RESIZE_LINES], 0,
		line_type,
		audio_channels, audio_channels_old);
  g_object_unref((GObject *) effect_pad);
}

void
ags_effect_pad_real_map_recall(AgsEffectPad *effect_pad)
{
  if((AGS_EFFECT_PAD_MAPPED_RECALL & (effect_pad->flags)) != 0){
    return;
  }

  effect_pad->flags |= AGS_EFFECT_PAD_MAPPED_RECALL;

  ags_effect_pad_find_port(effect_pad);
}

/**
 * ags_effect_pad_map_recall:
 * @effect_pad: the #AgsEffectPad to add its default recall.
 *
 * You may want the @effect_pad to add its default recall.
 */
void
ags_effect_pad_map_recall(AgsEffectPad *effect_pad)
{
  g_return_if_fail(AGS_IS_EFFECT_PAD(effect_pad));

  g_object_ref((GObject *) effect_pad);
  g_signal_emit((GObject *) effect_pad,
		effect_pad_signals[MAP_RECALL], 0);
  g_object_unref((GObject *) effect_pad);
}

GList*
ags_effect_pad_real_find_port(AgsEffectPad *effect_pad)
{
  GList *effect_line, *effect_line_start;
  
  GList *port, *tmp_port;

  port = NULL;

  /* find output ports */
  if(effect_pad->table != NULL){
    effect_line_start = 
      effect_line = gtk_container_get_children((GtkContainer *) effect_pad->table);

    while(effect_line != NULL){
      tmp_port = ags_effect_line_find_port(AGS_EFFECT_LINE(effect_line->data));
      
      if(port != NULL){
	port = g_list_concat(port,
			     tmp_port);
      }else{
	port = tmp_port;
      }

      effect_line = effect_line->next;
    }

    g_list_free(effect_line_start);
  }

  return(port);
}

/**
 * ags_effect_pad_find_port:
 * @effect_pad: the #AgsEffectPad
 * Returns: an #GList containing all related #AgsPort
 *
 * Lookup ports of associated recalls.
 *
 * Since: 0.7.8
 */
GList*
ags_effect_pad_find_port(AgsEffectPad *effect_pad)
{
  GList *list;

  list = NULL;
  g_return_val_if_fail(AGS_IS_EFFECT_PAD(effect_pad),
		       NULL);

  g_object_ref((GObject *) effect_pad);
  g_signal_emit((GObject *) effect_pad,
		effect_pad_signals[FIND_PORT], 0,
		&list);
  g_object_unref((GObject *) effect_pad);

  return(list);
}

/**
 * ags_effect_pad_new:
 * @channel: the #AgsChannel to visualize
 *
 * Creates an #AgsEffectPad
 *
 * Returns: a new #AgsEffectPad
 *
 * Since: 0.4
 */
AgsEffectPad*
ags_effect_pad_new(AgsChannel *channel)
{
  AgsEffectPad *effect_pad;

  effect_pad = (AgsEffectPad *) g_object_new(AGS_TYPE_EFFECT_PAD,
					     "channel\0", channel,
					     NULL);

  return(effect_pad);
}
