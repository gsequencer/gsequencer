/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/audio/recall/ags_prepare_channel.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_plugin.h>

void ags_prepare_channel_class_init(AgsPrepareChannelClass *prepare_channel);
void ags_prepare_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_prepare_channel_plugin_interface_init(AgsPluginInterface *plugin);
void ags_prepare_channel_init(AgsPrepareChannel *prepare_channel);
void ags_prepare_channel_connect(AgsConnectable *connectable);
void ags_prepare_channel_disconnect(AgsConnectable *connectable);
void ags_prepare_channel_dispose(GObject *gobject);
void ags_prepare_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_prepare_channel
 * @short_description: prepares channel
 * @title: AgsPrepareChannel
 * @section_id:
 * @include: ags/audio/recall/ags_prepare_channel.h
 *
 * The #AgsPrepareChannel class provides ports to the effect processor.
 */

static gpointer ags_prepare_channel_parent_class = NULL;
static AgsConnectableInterface *ags_prepare_channel_parent_connectable_interface;
static AgsPluginInterface *ags_prepare_channel_parent_plugin_interface;

static const gchar *ags_prepare_channel_plugin_name = "ags-prepare";

GType
ags_prepare_channel_get_type()
{
  static GType ags_type_prepare_channel = 0;

  if(!ags_type_prepare_channel){
    static const GTypeInfo ags_prepare_channel_info = {
      sizeof (AgsPrepareChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_prepare_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPrepareChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_prepare_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_prepare_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_prepare_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_prepare_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						      "AgsPrepareChannel",
						      &ags_prepare_channel_info,
						      0);

    g_type_add_interface_static(ags_type_prepare_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_prepare_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return (ags_type_prepare_channel);
}

void
ags_prepare_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_prepare_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_prepare_channel_connect;
  connectable->disconnect = ags_prepare_channel_disconnect;
}

void
ags_prepare_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_prepare_channel_parent_plugin_interface = g_type_interface_peek_parent(plugin);
}

void
ags_prepare_channel_class_init(AgsPrepareChannelClass *prepare_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  GParamSpec *param_spec;

  ags_prepare_channel_parent_class = g_type_class_peek_parent(prepare_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) prepare_channel;

  gobject->dispose = ags_prepare_channel_dispose;
  gobject->finalize = ags_prepare_channel_finalize;
}

void
ags_prepare_channel_init(AgsPrepareChannel *prepare_channel)
{
  AGS_RECALL(prepare_channel)->name = "ags-prepare";
  AGS_RECALL(prepare_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(prepare_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(prepare_channel)->xml_type = "ags-prepare-channel";
}

void
ags_prepare_channel_dispose(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_prepare_channel_parent_class)->dispose(gobject);
}

void
ags_prepare_channel_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_prepare_channel_parent_class)->finalize(gobject);
}

void
ags_prepare_channel_connect(AgsConnectable *connectable)
{
  /* call parent */
  ags_prepare_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_prepare_channel_disconnect(AgsConnectable *connectable)
{
  /* call parent */
  ags_prepare_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

/**
 * ags_prepare_channel_new:
 *
 * Creates an #AgsPrepareChannel
 *
 * Returns: a new #AgsPrepareChannel
 *
 * Since: 0.7.122.8
 */
AgsPrepareChannel*
ags_prepare_channel_new()
{
  AgsPrepareChannel *prepare_channel;

  prepare_channel = (AgsPrepareChannel *) g_object_new(AGS_TYPE_PREPARE_CHANNEL,
						       NULL);

  return(prepare_channel);
}
