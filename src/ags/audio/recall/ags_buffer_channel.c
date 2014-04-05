/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/audio/recall/ags_buffer_channel.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_mutable.h>
#include <ags/object/ags_plugin.h>

void ags_buffer_channel_class_init(AgsBufferChannelClass *buffer_channel);
void ags_buffer_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_buffer_channel_mutable_interface_init(AgsMutableInterface *mutable);
void ags_buffer_channel_plugin_interface_init(AgsPluginInterface *plugin);
void ags_buffer_channel_init(AgsBufferChannel *buffer_channel);
void ags_buffer_channel_connect(AgsConnectable *connectable);
void ags_buffer_channel_disconnect(AgsConnectable *connectable);
void ags_buffer_channel_set_ports(AgsPlugin *plugin, GList *port);
void ags_buffer_channel_finalize(GObject *gobject);

void ags_buffer_channel_set_muted(AgsMutable *mutable, gboolean muted);

enum{
  PROP_0,
  PROP_MUTED,
};

static gpointer ags_buffer_channel_parent_class = NULL;
static AgsConnectableInterface *ags_buffer_channel_parent_connectable_interface;
static AgsMutableInterface *ags_buffer_channel_parent_mutable_interface;
static AgsPluginInterface *ags_buffer_channel_parent_plugin_interface;

static const gchar *ags_buffer_channel_plugin_name = "ags-buffer\0";
static const gchar *ags_buffer_channel_plugin_specifier[] = {
  "./muted[0]\0",
};
static const gchar *ags_buffer_channel_plugin_control_port[] = {
  "1/1\0",
};

GType
ags_buffer_channel_get_type()
{
  static GType ags_type_buffer_channel = 0;

  if(!ags_type_buffer_channel){
    static const GTypeInfo ags_buffer_channel_info = {
      sizeof (AgsBufferChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_buffer_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsBufferChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_buffer_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_buffer_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_mutable_interface_info = {
      (GInterfaceInitFunc) ags_buffer_channel_mutable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_buffer_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_buffer_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						     "AgsBufferChannel\0",
						     &ags_buffer_channel_info,
						     0);

    g_type_add_interface_static(ags_type_buffer_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_buffer_channel,
				AGS_TYPE_MUTABLE,
				&ags_mutable_interface_info);

    g_type_add_interface_static(ags_type_buffer_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_buffer_channel);
}

void
ags_buffer_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_buffer_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_buffer_channel_connect;
  connectable->disconnect = ags_buffer_channel_disconnect;
}

void
ags_buffer_channel_mutable_interface_init(AgsMutableInterface *mutable)
{
  ags_buffer_channel_parent_mutable_interface = g_type_interface_peek_parent(mutable);

  mutable->set_muted = ags_buffer_channel_set_muted;
}

void
ags_buffer_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_buffer_channel_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_buffer_channel_set_ports;
}

void
ags_buffer_channel_class_init(AgsBufferChannelClass *buffer_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_buffer_channel_parent_class = g_type_class_peek_parent(buffer_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) buffer_channel;

  gobject->finalize = ags_buffer_channel_finalize;
}

void
ags_buffer_channel_init(AgsBufferChannel *buffer_channel)
{
  AGS_RECALL(buffer_channel)->name = "ags-buffer\0";
  AGS_RECALL(buffer_channel)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(buffer_channel)->build_id = AGS_BUILD_ID;
  AGS_RECALL(buffer_channel)->xml_type = "ags-buffer-channel\0";
  AGS_RECALL(buffer_channel)->port = NULL;
}

void
ags_buffer_channel_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_buffer_channel_parent_class)->finalize(gobject);
}

void
ags_buffer_channel_connect(AgsConnectable *connectable)
{
  ags_buffer_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_buffer_channel_disconnect(AgsConnectable *connectable)
{
  ags_buffer_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_buffer_channel_set_ports(AgsPlugin *plugin, GList *port)
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
ags_buffer_channel_set_muted(AgsMutable *mutable, gboolean muted)
{
  GValue value = {0,};

  g_value_init(&value, G_TYPE_BOOLEAN);
  g_value_set_boolean(&value, muted);

  ags_port_safe_write(AGS_BUFFER_CHANNEL(mutable)->muted, &value);
}

AgsBufferChannel*
ags_buffer_channel_new()
{
  AgsBufferChannel *buffer_channel;

  buffer_channel = (AgsBufferChannel *) g_object_new(AGS_TYPE_BUFFER_CHANNEL,
						     NULL);

  return(buffer_channel);
}
