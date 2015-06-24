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

#include <ags/audio/task/ags_channel_set_recycling.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synths.h>

#include <math.h>

void ags_channel_set_recycling_class_init(AgsChannelSetRecyclingClass *channel_set_recycling);
void ags_channel_set_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_channel_set_recycling_init(AgsChannelSetRecycling *channel_set_recycling);
void ags_channel_set_recycling_connect(AgsConnectable *connectable);
void ags_channel_set_recycling_disconnect(AgsConnectable *connectable);
void ags_channel_set_recycling_finalize(GObject *gobject);

void ags_channel_set_recycling_launch(AgsTask *task);

static gpointer ags_channel_set_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_channel_set_recycling_parent_connectable_interface;

GType
ags_channel_set_recycling_get_type()
{
  static GType ags_type_channel_set_recycling = 0;

  if(!ags_type_channel_set_recycling){
    static const GTypeInfo ags_channel_set_recycling_info = {
      sizeof (AgsChannelSetRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_channel_set_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsChannelSetRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_channel_set_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_channel_set_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_channel_set_recycling = g_type_register_static(AGS_TYPE_TASK,
							    "AgsChannelSetRecycling\0",
							    &ags_channel_set_recycling_info,
							    0);

    g_type_add_interface_static(ags_type_channel_set_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_channel_set_recycling);
}

void
ags_channel_set_recycling_class_init(AgsChannelSetRecyclingClass *channel_set_recycling)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_channel_set_recycling_parent_class = g_type_class_peek_parent(channel_set_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) channel_set_recycling;

  gobject->finalize = ags_channel_set_recycling_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) channel_set_recycling;

  task->launch = ags_channel_set_recycling_launch;
}

void
ags_channel_set_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_channel_set_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_channel_set_recycling_connect;
  connectable->disconnect = ags_channel_set_recycling_disconnect;
}

void
ags_channel_set_recycling_init(AgsChannelSetRecycling *channel_set_recycling)
{
  channel_set_recycling->channel = NULL;

  channel_set_recycling->first_recycling = NULL;
  channel_set_recycling->last_recycling = NULL;
}

void
ags_channel_set_recycling_connect(AgsConnectable *connectable)
{
  ags_channel_set_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_channel_set_recycling_disconnect(AgsConnectable *connectable)
{
  ags_channel_set_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_channel_set_recycling_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_channel_set_recycling_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_channel_set_recycling_launch(AgsTask *task)
{
  AgsChannelSetRecycling *channel_set_recycling;
  AgsChannel *link;
  GError *error;

  channel_set_recycling = AGS_CHANNEL_SET_RECYCLING(task);

  link = channel_set_recycling->channel;

  error = NULL;

  ags_channel_set_link(channel_set_recycling->channel,
		       NULL,
		       &error);

  if(error != NULL){
    g_error("%s\0", error->message);
  }

  ags_channel_set_recycling(channel_set_recycling->channel,
			    channel_set_recycling->first_recycling,
			    channel_set_recycling->last_recycling,
			    TRUE, TRUE);

  ags_channel_set_link(channel_set_recycling->channel,
		       link,
		       &error);

  if(error != NULL){
    g_error("%s\0", error->message);
  }
}

AgsChannelSetRecycling*
ags_channel_set_recycling_new(AgsChannel *channel,
			      AgsRecycling *first_recycling,
			      AgsRecycling *last_recycling)
{
  AgsChannelSetRecycling *channel_set_recycling;

  channel_set_recycling = (AgsChannelSetRecycling *) g_object_new(AGS_TYPE_CHANNEL_SET_RECYCLING,
								  NULL);

  channel_set_recycling->channel = channel;

  channel_set_recycling->first_recycling = first_recycling;
  channel_set_recycling->last_recycling = last_recycling;

  return(channel_set_recycling);
}
