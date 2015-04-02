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

#include <ags/X/ags_effect_pad.h>
#include <ags/X/ags_effect_pad_callbacks.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

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

/**
 * SECTION:ags_effect_pad
 * @short_description: A composite widget to visualize a bunch of #AgsChannel
 * @title: AgsEffectPad
 * @section_id:
 * @include: ags/X/ags_effect_pad.h
 *
 * #AgsEffectPad is a composite widget to visualize one #AgsChannel. It should be
 * packed by an #AgsEffectPad.
 */

enum{
  RESIZE_LINES,
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
  effect_pad->table = gtk_table_new(1, AGS_EFFECT_PAD_COLUMNS_COUNT,
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
  //TODO:JK: implement me
}

void
ags_effect_pad_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
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
  AgsEffectBridge *effect_bridge;
  AgsEffectLine *effect_line;
  AgsChannel *channel;
  GList *list, *list_next;
  guint i, j;
  
  effect_bridge = (AgsEffectBridge *) gtk_widget_get_ancestor((GtkWidget *) effect_pad,
							      AGS_TYPE_EFFECT_BRIDGE);
  
  if(audio_channels > audio_channels_old){
    channel = ags_channel_nth(effect_pad->channel,
			      audio_channels_old);

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
    list = gtk_container_get_children(effect_pad->table);
    list = g_list_nth(list,
		      audio_channels);

    while(list = list->next){
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

/**
 * ags_effect_pad_new:
 * @effect_pad: the parent effect_pad
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
