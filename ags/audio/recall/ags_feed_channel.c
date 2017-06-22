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

#include <ags/audio/recall/ags_feed_channel.h>

#include <ags/object/ags_connectable.h>

#include <ags/object/ags_plugin.h>

void ags_feed_channel_class_init(AgsFeedChannelClass *feed_channel);
void ags_feed_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_feed_channel_plugin_interface_init(AgsPluginInterface *plugin);
void ags_feed_channel_init(AgsFeedChannel *feed_channel);
void ags_feed_channel_connect(AgsConnectable *connectable);
void ags_feed_channel_disconnect(AgsConnectable *connectable);
void ags_feed_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_feed_channel
 * @short_description: feeds channel
 * @title: AgsFeedChannel
 * @section_id:
 * @include: ags/audio/recall/ags_feed_channel.h
 *
 * The #AgsFeedChannel is responsible to provide enough audio data
 * for real-time MIDI.
 */

static gpointer ags_feed_channel_parent_class = NULL;
static AgsConnectableInterface *ags_feed_channel_parent_connectable_interface;

static const gchar *ags_feed_channel_plugin_name = "ags-feed";

GType
ags_feed_channel_get_type()
{
  static GType ags_type_feed_channel = 0;

  if(!ags_type_feed_channel){
    static const GTypeInfo ags_feed_channel_info = {
      sizeof (AgsFeedChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_feed_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFeedChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_feed_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_feed_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_feed_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_feed_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsFeedChannel",
						   &ags_feed_channel_info,
						   0);
    
    g_type_add_interface_static(ags_type_feed_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_feed_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_feed_channel);
}

void
ags_feed_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_feed_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_feed_channel_connect;
  connectable->disconnect = ags_feed_channel_disconnect;
}

void
ags_feed_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->set_ports = NULL;
}

void
ags_feed_channel_class_init(AgsFeedChannelClass *feed_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_feed_channel_parent_class = g_type_class_peek_parent(feed_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) feed_channel;

  gobject->finalize = ags_feed_channel_finalize;
}

void
ags_feed_channel_init(AgsFeedChannel *feed_channel)
{
  GList *port;

  AGS_RECALL(feed_channel)->name = "ags-feed";
  AGS_RECALL(feed_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(feed_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(feed_channel)->xml_type = "ags-feed-channel";
}

void
ags_feed_channel_connect(AgsConnectable *connectable)
{
  ags_feed_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_feed_channel_disconnect(AgsConnectable *connectable)
{
  ags_feed_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_feed_channel_finalize(GObject *gobject)
{
  AgsFeedChannel *feed_channel;

  feed_channel = AGS_FEED_CHANNEL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_feed_channel_parent_class)->finalize(gobject);
}

/**
 * ags_feed_channel_new:
 *
 * Creates an #AgsFeedChannel
 *
 * Returns: a new #AgsFeedChannel
 *
 * Since: 0.7.111
 */
AgsFeedChannel*
ags_feed_channel_new()
{
  AgsFeedChannel *feed_channel;

  feed_channel = (AgsFeedChannel *) g_object_new(AGS_TYPE_FEED_CHANNEL,
						 NULL);

  return(feed_channel);
}
