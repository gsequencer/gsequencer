/* This file is part of GSequencer.
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

#include <ags/audio/recall/ags_mute_channel.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_mutable.h>
#include <ags/object/ags_plugin.h>

void ags_mute_channel_class_init(AgsMuteChannelClass *mute_channel);
void ags_mute_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_mute_channel_mutable_interface_init(AgsMutableInterface *mutable);
void ags_mute_channel_plugin_interface_init(AgsPluginInterface *plugin);
void ags_mute_channel_init(AgsMuteChannel *mute_channel);
void ags_mute_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_mute_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_mute_channel_connect(AgsConnectable *connectable);
void ags_mute_channel_disconnect(AgsConnectable *connectable);
void ags_mute_channel_set_ports(AgsPlugin *plugin, GList *port);
void ags_mute_channel_finalize(GObject *gobject);

void ags_mute_channel_set_muted(AgsMutable *mutable, gboolean muted);

/**
 * SECTION:ags_mute_channel
 * @short_description: mutes channel
 * @title: AgsMuteChannel
 * @section_id:
 * @include: ags/audio/recall/ags_mute_channel.h
 *
 * The #AgsMuteChannel class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_MUTED,
};

static gpointer ags_mute_channel_parent_class = NULL;
static AgsConnectableInterface *ags_mute_channel_parent_connectable_interface;
static AgsMutableInterface *ags_mute_channel_parent_mutable_interface;

GType
ags_mute_channel_get_type()
{
  static GType ags_type_mute_channel = 0;

  if(!ags_type_mute_channel){
    static const GTypeInfo ags_mute_channel_info = {
      sizeof (AgsMuteChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_mute_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMuteChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_mute_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_mute_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_mutable_interface_info = {
      (GInterfaceInitFunc) ags_mute_channel_mutable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_mute_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_mute_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsMuteChannel\0",
						   &ags_mute_channel_info,
						   0);

    g_type_add_interface_static(ags_type_mute_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_mute_channel,
				AGS_TYPE_MUTABLE,
				&ags_mutable_interface_info);

    g_type_add_interface_static(ags_type_mute_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_mute_channel);
}

void
ags_mute_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_mute_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_mute_channel_connect;
  connectable->disconnect = ags_mute_channel_disconnect;
}

void
ags_mute_channel_mutable_interface_init(AgsMutableInterface *mutable)
{
  ags_mute_channel_parent_mutable_interface = g_type_interface_peek_parent(mutable);

  mutable->set_muted = ags_mute_channel_set_muted;
}

void
ags_mute_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->set_ports = ags_mute_channel_set_ports;
}

void
ags_mute_channel_class_init(AgsMuteChannelClass *mute_channel)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_mute_channel_parent_class = g_type_class_peek_parent(mute_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) mute_channel;

  gobject->set_property = ags_mute_channel_set_property;
  gobject->get_property = ags_mute_channel_get_property;

  gobject->finalize = ags_mute_channel_finalize;

  /* properties */
  param_spec = g_param_spec_object("muted\0",
				   "mute channel\0",
				   "Mute the channel\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MUTED,
				  param_spec);
}

void
ags_mute_channel_init(AgsMuteChannel *mute_channel)
{
  GList *port;

  AGS_RECALL(mute_channel)->name = "ags-mute\0";
  AGS_RECALL(mute_channel)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(mute_channel)->build_id = AGS_BUILD_ID;
  AGS_RECALL(mute_channel)->xml_type = "ags-mute-channel\0";

  port = NULL;

  mute_channel->muted = g_object_new(AGS_TYPE_PORT,
				     "plugin-name\0", g_strdup("ags-mute\0"),
				     "specifier\0", "./muted[0]\0",
				     "control-port\0", "1/1\0",
				     "port-value-is-pointer\0", FALSE,
				     "port-value-type\0", G_TYPE_BOOLEAN,
				     "port-value-size\0", sizeof(gboolean),
				     "port-value-length\0", 1,
				     NULL);
  mute_channel->muted->port_value.ags_port_boolean = FALSE;

  port = g_list_prepend(port, mute_channel->muted);

  AGS_RECALL(mute_channel)->port = port;
}

void
ags_mute_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsMuteChannel *mute_channel;

  mute_channel = AGS_MUTE_CHANNEL(gobject);

  switch(prop_id){
  case PROP_MUTED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == mute_channel->muted){
	return;
      }

      if(mute_channel->muted != NULL){
	g_object_unref(G_OBJECT(mute_channel->muted));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      mute_channel->muted = port;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_mute_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsMuteChannel *mute_channel;

  mute_channel = AGS_MUTE_CHANNEL(gobject);

  switch(prop_id){
  case PROP_MUTED:
    {
      g_value_set_object(value, mute_channel->muted);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_mute_channel_connect(AgsConnectable *connectable)
{
  ags_mute_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_mute_channel_disconnect(AgsConnectable *connectable)
{
  ags_mute_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_mute_channel_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"muted[0]\0",
		9)){
      g_object_set(G_OBJECT(plugin),
		   "muted\0", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

void
ags_mute_channel_finalize(GObject *gobject)
{
  AgsMuteChannel *mute_channel;

  mute_channel = AGS_MUTE_CHANNEL(gobject);

  if(mute_channel->muted != NULL){
    g_object_unref(G_OBJECT(mute_channel->muted));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_mute_channel_parent_class)->finalize(gobject);
}

void
ags_mute_channel_set_muted(AgsMutable *mutable, gboolean muted)
{
  GValue value = {0,};

  g_value_init(&value, G_TYPE_BOOLEAN);
  g_value_set_boolean(&value, muted);

  ags_port_safe_write(AGS_MUTE_CHANNEL(mutable)->muted, &value);
}

/**
 * ags_mute_channel_new:
 *
 * Creates an #AgsMuteChannel
 *
 * Returns: a new #AgsMuteChannel
 *
 * Since: 0.4
 */
AgsMuteChannel*
ags_mute_channel_new()
{
  AgsMuteChannel *mute_channel;

  mute_channel = (AgsMuteChannel *) g_object_new(AGS_TYPE_MUTE_CHANNEL,
						 NULL);

  return(mute_channel);
}
