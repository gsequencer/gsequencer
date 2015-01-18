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

#include <ags/audio/task/ags_cancel_channel.h>

#include <ags-lib/object/ags_connectable.h>

void ags_cancel_channel_class_init(AgsCancelChannelClass *cancel_channel);
void ags_cancel_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_cancel_channel_init(AgsCancelChannel *cancel_channel);
void ags_cancel_channel_connect(AgsConnectable *connectable);
void ags_cancel_channel_disconnect(AgsConnectable *connectable);
void ags_cancel_channel_finalize(GObject *gobject);

void ags_cancel_channel_launch(AgsTask *task);

/**
 * SECTION:ags_cancel_channel
 * @short_description: cancel channel object
 * @title: AgsCancelChannel
 * @section_id:
 * @include: ags/audio/task/ags_cancel_channel.h
 *
 * The #AgsCancelChannel task cancels #AgsChannel playback.
 */

static gpointer ags_cancel_channel_parent_class = NULL;
static AgsConnectableInterface *ags_cancel_channel_parent_connectable_interface;

GType
ags_cancel_channel_get_type()
{
  static GType ags_type_cancel_channel = 0;

  if(!ags_type_cancel_channel){
    static const GTypeInfo ags_cancel_channel_info = {
      sizeof (AgsCancelChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_cancel_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCancelChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_cancel_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_cancel_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_cancel_channel = g_type_register_static(AGS_TYPE_TASK,
						     "AgsCancelChannel\0",
						     &ags_cancel_channel_info,
						     0);
    
    g_type_add_interface_static(ags_type_cancel_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_cancel_channel);
}

void
ags_cancel_channel_class_init(AgsCancelChannelClass *cancel_channel)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_cancel_channel_parent_class = g_type_class_peek_parent(cancel_channel);

  /* gobject */
  gobject = (GObjectClass *) cancel_channel;

  gobject->finalize = ags_cancel_channel_finalize;

  /* task */
  task = (AgsTaskClass *) cancel_channel;

  task->launch = ags_cancel_channel_launch;
}

void
ags_cancel_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_cancel_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_cancel_channel_connect;
  connectable->disconnect = ags_cancel_channel_disconnect;
}

void
ags_cancel_channel_init(AgsCancelChannel *cancel_channel)
{
  cancel_channel->channel = NULL;
  cancel_channel->recall_id = NULL;

  cancel_channel->play = NULL;
}

void
ags_cancel_channel_connect(AgsConnectable *connectable)
{
  ags_cancel_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_cancel_channel_disconnect(AgsConnectable *connectable)
{
  ags_cancel_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_cancel_channel_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_cancel_channel_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_cancel_channel_launch(AgsTask *task)
{
  AgsCancelChannel *cancel_channel;
  AgsChannel *channel;

  cancel_channel = AGS_CANCEL_CHANNEL(task);

  channel = cancel_channel->channel;

  /* cancel playback */
  if(AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[0] == NULL){
    return;
  }

  g_object_ref(AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[0]);
  ags_channel_tillrecycling_cancel(channel,
				   AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[0]);

  /* set remove flag */
  AGS_DEVOUT_PLAY(channel->devout_play)->flags |= (AGS_DEVOUT_PLAY_DONE | AGS_DEVOUT_PLAY_REMOVE);
  AGS_DEVOUT_PLAY(channel->devout_play)->recall_id[0] = NULL;
}

/**
 * ags_cancel_channel_new:
 * @channel: the #AgsChannel to cancel
 * @recall_id: the #AgsRecallID to cancel
 * @play: the #AgsDevoutPlay-struct
 *
 * Creates an #AgsCancelChannel.
 *
 * Returns: an new #AgsCancelChannel.
 *
 * Since: 0.4
 */
AgsCancelChannel*
ags_cancel_channel_new(AgsChannel *channel, AgsRecallID *recall_id,
		       AgsDevoutPlay *play)
{
  AgsCancelChannel *cancel_channel;

  cancel_channel = (AgsCancelChannel *) g_object_new(AGS_TYPE_CANCEL_CHANNEL,
						     NULL);

  cancel_channel->channel = channel;
  cancel_channel->recall_id = recall_id;

  cancel_channel->play = play;

  return(cancel_channel);
}
