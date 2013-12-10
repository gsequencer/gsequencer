/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/audio/recall/ags_volume_channel.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_plugin.h>

void ags_volume_channel_class_init(AgsVolumeChannelClass *volume_channel);
void ags_volume_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_volume_channel_plugin_interface_init(AgsPluginInterface *plugin);
void ags_volume_channel_init(AgsVolumeChannel *volume_channel);
void ags_volume_channel_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_volume_channel_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_volume_channel_connect(AgsConnectable *connectable);
void ags_volume_channel_disconnect(AgsConnectable *connectable);
void ags_volume_channel_set_ports(AgsPlugin *plugin, GList *port);
void ags_volume_channel_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_VOLUME,
};

static gpointer ags_volume_channel_parent_class = NULL;
static AgsConnectableInterface *ags_volume_channel_parent_connectable_interface;

GType
ags_volume_channel_get_type()
{
  static GType ags_type_volume_channel = 0;

  if(!ags_type_volume_channel){
    static const GTypeInfo ags_volume_channel_info = {
      sizeof (AgsVolumeChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_volume_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsVolumeChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_volume_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_volume_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_volume_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_volume_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						     "AgsVolumeChannel\0",
						     &ags_volume_channel_info,
						     0);
    
    g_type_add_interface_static(ags_type_volume_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_volume_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_volume_channel);
}

void
ags_volume_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_volume_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_volume_channel_connect;
  connectable->disconnect = ags_volume_channel_disconnect;
}

void
ags_volume_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->set_ports = ags_volume_channel_set_ports;
}

void
ags_volume_channel_class_init(AgsVolumeChannelClass *volume_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_volume_channel_parent_class = g_type_class_peek_parent(volume_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) volume_channel;

  gobject->set_property = ags_volume_channel_set_property;
  gobject->get_property = ags_volume_channel_get_property;

  gobject->finalize = ags_volume_channel_finalize;

  /* properties */
  param_spec = g_param_spec_object("volume\0",
				   "volume to apply\0",
				   "The volume to apply on the channel\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VOLUME,
				  param_spec);
}

void
ags_volume_channel_init(AgsVolumeChannel *volume_channel)
{
  GList *port;

  AGS_RECALL(volume_channel)->name = "ags-volume\0";
  AGS_RECALL(volume_channel)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(volume_channel)->build_id = AGS_BUILD_ID;
  AGS_RECALL(volume_channel)->xml_type = "ags-volume-channel\0";

  port = NULL;

  /* volume */
  volume_channel->volume = g_object_new(AGS_TYPE_PORT,
					"plugin-name\0", g_strdup("ags-volume\0"),
					"specifier\0", "./volume[0]\0",
					"control-port\0", "1/1\0",
					"port-value-is-pointer\0", FALSE,
					"port-value-type\0", G_TYPE_DOUBLE,
					"port-value-size\0", sizeof(gdouble),
					"port-value-length", 1,
					NULL);

  volume_channel->volume->port_value.ags_port_double = 1.0;

  port = g_list_prepend(port, volume_channel->volume);

  AGS_RECALL(volume_channel)->port = port;
}

void
ags_volume_channel_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec)
{
  AgsVolumeChannel *volume_channel;

  volume_channel = AGS_VOLUME_CHANNEL(gobject);

  switch(prop_id){
  case PROP_VOLUME:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == volume_channel->volume){
	return;
      }

      if(volume_channel->volume != NULL){
	g_object_unref(G_OBJECT(volume_channel->volume));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      volume_channel->volume = port;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_volume_channel_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec)
{
  AgsVolumeChannel *volume_channel;

  volume_channel = AGS_VOLUME_CHANNEL(gobject);

  switch(prop_id){
  case PROP_VOLUME:
    {
      g_value_set_object(value, volume_channel->volume);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_volume_channel_connect(AgsConnectable *connectable)
{
  ags_volume_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_volume_channel_disconnect(AgsConnectable *connectable)
{
  ags_volume_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_volume_channel_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"volume[0]\0",
		10)){
      g_object_set(G_OBJECT(plugin),
		   "volume\0", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

void
ags_volume_channel_finalize(GObject *gobject)
{
  AgsVolumeChannel *volume_channel;

  volume_channel = AGS_VOLUME_CHANNEL(gobject);

  if(volume_channel->volume != NULL){
    g_object_unref(G_OBJECT(volume_channel->volume));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_volume_channel_parent_class)->finalize(gobject);
}

AgsVolumeChannel*
ags_volume_channel_new()
{
  AgsVolumeChannel *volume_channel;

  volume_channel = (AgsVolumeChannel *) g_object_new(AGS_TYPE_VOLUME_CHANNEL,
						     NULL);

  return(volume_channel);
}
