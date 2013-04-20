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

#include <ags/object/ags_connectable.h>

void ags_volume_channel_class_init(AgsVolumeChannelClass *volume_channel);
void ags_volume_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_volume_channel_init(AgsVolumeChannel *volume_channel);
void ags_volume_channel_connect(AgsConnectable *connectable);
void ags_volume_channel_disconnect(AgsConnectable *connectable);
void ags_volume_channel_finalize(GObject *gobject);

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

    ags_type_volume_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						     "AgsVolumeChannel\0",
						     &ags_volume_channel_info,
						     0);
    
    g_type_add_interface_static(ags_type_volume_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_volume_channel);
}

void
ags_volume_channel_class_init(AgsVolumeChannelClass *volume_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  ags_volume_channel_parent_class = g_type_class_peek_parent(volume_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) volume_channel;

  gobject->finalize = ags_volume_channel_finalize;
}

void
ags_volume_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_volume_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_volume_channel_connect;
  connectable->disconnect = ags_volume_channel_disconnect;
}

void
ags_volume_channel_init(AgsVolumeChannel *volume_channel)
{
  /* empty */
}

void
ags_volume_channel_finalize(GObject *gobject)
{
  /* empty */

  /* call parent */
  G_OBJECT_CLASS(ags_volume_channel_parent_class)->finalize(gobject);
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

AgsVolumeChannel*
ags_volume_channel_new()
{
  AgsVolumeChannel *volume_channel;

  volume_channel = (AgsVolumeChannel *) g_object_new(AGS_TYPE_VOLUME_CHANNEL,
						     NULL);

  return(volume_channel);
}
