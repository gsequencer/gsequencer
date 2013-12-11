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

#include <ags/audio/recall/ags_copy_channel.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_mutable.h>
#include <ags/object/ags_plugin.h>

void ags_copy_channel_class_init(AgsCopyChannelClass *copy_channel);
void ags_copy_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_copy_channel_mutable_interface_init(AgsMutableInterface *mutable);
void ags_copy_channel_plugin_interface_init(AgsPluginInterface *plugin);
void ags_copy_channel_init(AgsCopyChannel *copy_channel);
void ags_copy_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_copy_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_copy_channel_connect(AgsConnectable *connectable);
void ags_copy_channel_disconnect(AgsConnectable *connectable);
void ags_copy_channel_set_ports(AgsPlugin *plugin, GList *port);
void ags_copy_channel_finalize(GObject *gobject);

void ags_copy_channel_set_muted(AgsMutable *mutable, gboolean muted);

enum{
  PROP_0,
  PROP_MUTED,
};

static gpointer ags_copy_channel_parent_class = NULL;
static AgsConnectableInterface *ags_copy_channel_parent_connectable_interface;
static AgsMutableInterface *ags_copy_channel_parent_mutable_interface;
static AgsPluginInterface *ags_copy_channel_parent_plugin_interface;

static const gchar *ags_copy_channel_plugin_name = "ags-copy\0";
static const gchar *ags_copy_channel_plugin_specifier[] = {
  "./muted[0]\0",
};
static const gchar *ags_copy_channel_plugin_control_port[] = {
  "1/1\0",
};

GType
ags_copy_channel_get_type()
{
  static GType ags_type_copy_channel = 0;

  if(!ags_type_copy_channel){
    static const GTypeInfo ags_copy_channel_info = {
      sizeof (AgsCopyChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_copy_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_mutable_interface_info = {
      (GInterfaceInitFunc) ags_copy_channel_mutable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_copy_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_copy_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsCopyChannel\0",
						   &ags_copy_channel_info,
						   0);

    g_type_add_interface_static(ags_type_copy_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_copy_channel,
				AGS_TYPE_MUTABLE,
				&ags_mutable_interface_info);

    g_type_add_interface_static(ags_type_copy_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_copy_channel);
}

void
ags_copy_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_copy_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_copy_channel_connect;
  connectable->disconnect = ags_copy_channel_disconnect;
}

void
ags_copy_channel_mutable_interface_init(AgsMutableInterface *mutable)
{
  ags_copy_channel_parent_mutable_interface = g_type_interface_peek_parent(mutable);

  mutable->set_muted = ags_copy_channel_set_muted;
}

void
ags_copy_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_copy_channel_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_copy_channel_set_ports;
}

void
ags_copy_channel_class_init(AgsCopyChannelClass *copy_channel)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_copy_channel_parent_class = g_type_class_peek_parent(copy_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_channel;

  gobject->set_property = ags_copy_channel_set_property;
  gobject->get_property = ags_copy_channel_get_property;

  gobject->finalize = ags_copy_channel_finalize;

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
ags_copy_channel_init(AgsCopyChannel *copy_channel)
{
  GList *port;

  AGS_RECALL(copy_channel)->name = "ags-copy\0";
  AGS_RECALL(copy_channel)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(copy_channel)->build_id = AGS_BUILD_ID;
  AGS_RECALL(copy_channel)->xml_type = "ags-copy-channel\0";

  port = NULL;

  copy_channel->muted = g_object_new(AGS_TYPE_PORT,
				     "plugin-name\0", ags_copy_channel_plugin_name,
				     "specifier\0", ags_copy_channel_plugin_specifier[0],
				     "control-port\0", ags_copy_channel_plugin_control_port[0],
				     "port-value-is-pointer\0", FALSE,
				     "port-value-type\0", G_TYPE_BOOLEAN,
				     "port-value-size\0", sizeof(gboolean),
				     "port-value-length\0", 1,
				     NULL);
  copy_channel->muted->port_value.ags_port_boolean = FALSE;

  port = g_list_prepend(port, copy_channel->muted);

  /* set port */
  AGS_RECALL(copy_channel)->port = port;
}

void
ags_copy_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsCopyChannel *copy_channel;

  copy_channel = AGS_COPY_CHANNEL(gobject);

  switch(prop_id){
  case PROP_MUTED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == copy_channel->muted){
	return;
      }

      if(copy_channel->muted != NULL){
	g_object_unref(G_OBJECT(copy_channel->muted));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      copy_channel->muted = port;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_copy_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsCopyChannel *copy_channel;

  copy_channel = AGS_COPY_CHANNEL(gobject);

  switch(prop_id){
  case PROP_MUTED:
    {
      g_value_set_object(value, copy_channel->muted);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_copy_channel_finalize(GObject *gobject)
{
  AgsCopyChannel *copy_channel;

  copy_channel = AGS_COPY_CHANNEL(gobject);

  if(copy_channel->muted != NULL){
    g_object_unref(G_OBJECT(copy_channel->muted));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_copy_channel_parent_class)->finalize(gobject);
}

void
ags_copy_channel_connect(AgsConnectable *connectable)
{
  ags_copy_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_copy_channel_disconnect(AgsConnectable *connectable)
{
  ags_copy_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_copy_channel_set_ports(AgsPlugin *plugin, GList *port)
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
ags_copy_channel_set_muted(AgsMutable *mutable, gboolean muted)
{
  GValue value = {0,};

  g_value_init(&value, G_TYPE_BOOLEAN);
  g_value_set_boolean(&value, muted);

  ags_port_safe_write(AGS_COPY_CHANNEL(mutable)->muted, &value);
}

AgsCopyChannel*
ags_copy_channel_new(AgsChannel *destination,
		     AgsChannel *source)
{
  AgsCopyChannel *copy_channel;

  copy_channel = (AgsCopyChannel *) g_object_new(AGS_TYPE_COPY_CHANNEL,
						 "destination\0", destination,
						 "channel\0", source,
						 NULL);

  return(copy_channel);
}
