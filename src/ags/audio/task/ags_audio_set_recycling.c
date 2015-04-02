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

#include <ags/audio/task/ags_audio_set_recycling.h>

#include <ags-lib/object/ags_connectable.h>

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

  audio_set_recycling->parameter = NULL;
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
  AgsChannel *channel, *start_channel, *end_channel;
  AgsRecycling *recycling, *start_recycling, *end_recycling;
  GParameter *parameter;
  GList *link;
  GError *error;
  int i;

  auto void ags_audio_set_recycling_launch_read_parameter();

  void ags_audio_set_recycling_launch_read_parameter(){
    int i;

    for(i = 0; i < 4; i++){
      if(!strncmp("start_channel\0", parameter[i].name, 13)){
	start_channel = (AgsChannel *) g_value_get_object(&(parameter[i].value));
      }else if(!strncmp("end_channel\0", parameter[i].name, 11)){
	end_channel = (AgsChannel *) g_value_get_object(&(parameter[i].value));
      }else if(!strncmp("start_recycling\0", parameter[i].name, 15)){
	start_recycling = (AgsRecycling *) g_value_get_object(&(parameter[i].value));
      }else if(!strncmp("end_recycling\0", parameter[i].name, 13)){
	end_recycling = (AgsRecycling *) g_value_get_object(&(parameter[i].value));
      }else{
	g_error("ags_audio_set_recycling_launch_read_parameter: unknow Parameter\0");
      }
    }
  }

  parameter = audio_set_recycling->parameter;
  audio_set_recycling = AGS_AUDIO_SET_RECYCLING(task);

  for(i = 0; i < audio_set_recycling->audio->audio_channels; i++){
    ags_audio_set_recycling_launch_read_parameter();

    /* unset link */
    channel = start_channel;
    link = NULL;

    while(channel != end_channel){
      link = g_list_prepend(link, channel->link);
    
      error = NULL;

      ags_channel_set_link(channel,
			   NULL,
			   &error);
    
      if(error != NULL){
	g_error("%s\0", error->message);
      }
    
      channel = channel->next_pad;
    }

    link = g_list_reverse(link);

    /* set recycling */
    channel = start_channel;
    recycling = start_recycling;

    while(channel != end_channel && recycling != end_recycling->next){
      link = g_list_prepend(link, channel->link);
    
      error = NULL;

      ags_channel_set_recycling(channel,
				recycling,
				recycling,
				TRUE, TRUE);
    
      if(error != NULL){
	g_error("%s\0", error->message);
      }
    
      channel = channel->next_pad;
    }

    /* reset link */
    channel = start_channel;

    while(channel != end_channel){
      error = NULL;

      ags_channel_set_link(channel,
			   AGS_CHANNEL(link->data),
			   &error);
    
      if(error != NULL){
	g_error("%s\0", error->message);
      }
    
      channel = channel->next_pad;
      link = link->next;
    }
  }
}

AgsAudioSetRecycling*
ags_audio_set_recycling_new(AgsAudio *audio,
			    GParameter *parameter)
{
  AgsAudioSetRecycling *audio_set_recycling;

  audio_set_recycling = (AgsAudioSetRecycling *) g_object_new(AGS_TYPE_AUDIO_SET_RECYCLING,
							      NULL);

  audio_set_recycling->audio = audio;

  audio_set_recycling->parameter = parameter;

  return(audio_set_recycling);
}
