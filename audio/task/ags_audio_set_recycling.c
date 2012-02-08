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

#include <ags/audio/task/ags_channel_set_recycling.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synths.h>

#include <math.h>

void ags_audio_set_recycling_class_init(AgsAudioSetRecyclingClass *audio_set_recycling);
void ags_audio_set_recycling_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_set_recycling_init(AgsAudioSetRecycling *audio_set_recycling);
void ags_audio_set_recycling_connect(AgsConnectable *connectable);
void ags_audio_set_recycling_disconnect(AgsConnectable *connectable);
void ags_audio_set_recycling_finalize(GObject *gobject);

void ags_audio_set_recycling_launch(AgsTask *task);

static gpointer ags_audio_set_recycling_parent_class = NULL;
static AgsConnectableInterface *ags_audio_set_recycling_parent_connectable_interface;

GType
ags_audio_set_recycling_get_type()
{
  static GType ags_type_audio_set_recycling = 0;

  if(!ags_type_audio_set_recycling){
    static const GTypeInfo ags_audio_set_recycling_info = {
      sizeof (AgsAudioSetRecyclingClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_set_recycling_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioSetRecycling),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_set_recycling_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_set_recycling_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_set_recycling = g_type_register_static(AGS_TYPE_TASK,
							    "AgsAudioSetRecycling\0",
							    &ags_audio_set_recycling_info,
							    0);

    g_type_add_interface_static(ags_type_audio_set_recycling,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_audio_set_recycling);
}

void
ags_audio_set_recycling_class_init(AgsAudioSetRecyclingClass *audio_set_recycling)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_audio_set_recycling_parent_class = g_type_class_peek_parent(audio_set_recycling);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_set_recycling;

  gobject->finalize = ags_audio_set_recycling_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) audio_set_recycling;

  task->launch = ags_audio_set_recycling_launch;
}

void
ags_audio_set_recycling_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_audio_set_recycling_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_audio_set_recycling_connect;
  connectable->disconnect = ags_audio_set_recycling_disconnect;
}

void
ags_audio_set_recycling_init(AgsAudioSetRecycling *audio_set_recycling)
{
  audio_set_recycling->audio = NULL;

  audio_set_recycling->start_channel = NULL;
  audio_set_recycling->end_channel = NULL;

  audio_set_recycling->first_recycling = NULL;
  audio_set_recycling->last_recycling = NULL;
}

void
ags_audio_set_recycling_connect(AgsConnectable *connectable)
{
  ags_audio_set_recycling_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_audio_set_recycling_disconnect(AgsConnectable *connectable)
{
  ags_audio_set_recycling_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_audio_set_recycling_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_audio_set_recycling_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_audio_set_recycling_launch(AgsTask *task)
{
  AgsAudioSetRecycling *audio_set_recycling;
  AgsChannel *channel;
  AgsRecycling *recycling;
  GList *link;
  GError *error;

  audio_set_recycling = AGS_AUDIO_SET_RECYCLING(task);

  /* unset link */
  channel = audio_set_recycling->start_channel;
  link = NULL;

  while(channel != audio_set_recycling->end_channel){
    link = g_list_prepend(link, channel->link);
    
    error = NULL;

    ags_channel_set_link(channel,
			 NULL,
			 &error);
    
    if(error != NULL){
      g_error(error->message);
    }
    
    channel = channel->next_pad;
  }

  link = g_list_reverse(link);

  /* set recycling */
  channel = audio_set_recycling->start_channel;
  recycling = audio_set_recycling->first_recycling;

  while(channel != audio_set_recycling->end_channel && recycling != last_recycling->next){
    link = g_list_prepend(link, channel->link);
    
    error = NULL;

    ags_channel_set_recycling(channel,
			      recycling,
			      recycling,
			      TRUE, TRUE);
    
    if(error != NULL){
      g_error(error->message);
    }
    
    channel = channel->next_pad;
  }

  /* reset link */
  channel = audio_set_recycling->start_channel;

  while(channel != audio_set_recycling->end_channel){
    error = NULL;

    ags_channel_set_link(channel,
			 AGS_CHANNEL(list->data),
			 &error);
    
    if(error != NULL){
      g_error(error->message);
    }
    
    channel = channel->next_pad;
    list = list->next;
  }
}

AgsAudioSetRecycling*
ags_audio_set_recycling_new(AgsAudio *audio,
			    AgsChannel *start_channel,
			    AgsChannel *end_channel,
			    AgsRecycling *first_recycling,
			    AgsRecycling *last_recycling)
{
  AgsAudioSetRecycling *audio_set_recycling;

  audio_set_recycling = (AgsAudioSetRecycling *) g_object_new(AGS_TYPE_AUDIO_SET_RECYCLING,
							      NULL);

  audio_set_recycling->audio = audio;

  audio_set_recycling->start_channel = start_channel;
  audio_set_recycling->end_channel = end_channel;

  audio_set_recycling->first_recycling = first_recycling;
  audio_set_recycling->last_recycling = last_recycling;

  return(audio_set_recycling);
}
