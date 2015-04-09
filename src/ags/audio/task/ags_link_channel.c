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

#include <ags/audio/task/ags_link_channel.h>

#include <ags-lib/object/ags_connectable.h>

void ags_link_channel_class_init(AgsLinkChannelClass *link_channel);
void ags_link_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_link_channel_init(AgsLinkChannel *link_channel);
void ags_link_channel_connect(AgsConnectable *connectable);
void ags_link_channel_disconnect(AgsConnectable *connectable);
void ags_link_channel_finalize(GObject *gobject);

void ags_link_channel_launch(AgsTask *task);

static gpointer ags_link_channel_parent_class = NULL;
static AgsConnectableInterface *ags_link_channel_parent_connectable_interface;

GType
ags_link_channel_get_type()
{
  static GType ags_type_link_channel = 0;

  if(!ags_type_link_channel){
    static const GTypeInfo ags_link_channel_info = {
      sizeof (AgsLinkChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_link_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLinkChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_link_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_link_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_link_channel = g_type_register_static(AGS_TYPE_TASK,
						   "AgsLinkChannel\0",
						   &ags_link_channel_info,
						   0);

    g_type_add_interface_static(ags_type_link_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_link_channel);
}

void
ags_link_channel_class_init(AgsLinkChannelClass *link_channel)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_link_channel_parent_class = g_type_class_peek_parent(link_channel);

  /* GObject */
  gobject = (GObjectClass *) link_channel;

  gobject->finalize = ags_link_channel_finalize;

  /* AgsTask */
  task = (AgsTaskClass *) link_channel;

  task->launch = ags_link_channel_launch;
}

void
ags_link_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_link_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_link_channel_connect;
  connectable->disconnect = ags_link_channel_disconnect;
}

void
ags_link_channel_init(AgsLinkChannel *link_channel)
{
  link_channel->channel = NULL;
  link_channel->link = NULL;

  link_channel->error = NULL;
}

void
ags_link_channel_connect(AgsConnectable *connectable)
{
  ags_link_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_link_channel_disconnect(AgsConnectable *connectable)
{
  ags_link_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_link_channel_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_link_channel_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_link_channel_launch(AgsTask *task)
{
  AgsLinkChannel *link_channel;

  link_channel = AGS_LINK_CHANNEL(task);

  /* link channel */
  ags_channel_set_link(link_channel->channel, link_channel->link,
		       &(link_channel->error));

  if(link_channel->error != NULL){
    g_message(link_channel->error->message);
  }
}

AgsLinkChannel*
ags_link_channel_new(AgsChannel *channel, AgsChannel *link)
{
  AgsLinkChannel *link_channel;

  link_channel = (AgsLinkChannel *) g_object_new(AGS_TYPE_LINK_CHANNEL,
						 NULL);

  link_channel->channel = channel;
  link_channel->link = link;

  return(link_channel);
}
