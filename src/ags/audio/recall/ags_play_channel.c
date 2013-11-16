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

#include <ags/audio/recall/ags_play_channel.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>

#include <stdlib.h>
#include <stdio.h>

void ags_play_channel_class_init(AgsPlayChannelClass *play_channel);
void ags_play_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_channel_init(AgsPlayChannel *play_channel);
void ags_play_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_play_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_play_channel_connect(AgsConnectable *connectable);
void ags_play_channel_disconnect(AgsConnectable *connectable);
void ags_play_channel_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_AUDIO_CHANNEL,
};

static gpointer ags_play_channel_parent_class = NULL;
static AgsConnectableInterface *ags_play_channel_parent_connectable_interface;

GType
ags_play_channel_get_type()
{
  static GType ags_type_play_channel = 0;

  if(!ags_type_play_channel){
    static const GTypeInfo ags_play_channel_info = {
      sizeof (AgsPlayChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPlayChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_play_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsPlayChannel\0",
						   &ags_play_channel_info,
						   0);

    g_type_add_interface_static(ags_type_play_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_play_channel);
}

void
ags_play_channel_class_init(AgsPlayChannelClass *play_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_play_channel_parent_class = g_type_class_peek_parent(play_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) play_channel;

  gobject->set_property = ags_play_channel_set_property;
  gobject->get_property = ags_play_channel_get_property;

  gobject->finalize = ags_play_channel_finalize;

  /* properties */
  param_spec = g_param_spec_object("audio-channel\0",
				   "assigned audio Channel\0",
				   "The audio channel this recall does output to\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);
}

void
ags_play_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_play_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_play_channel_connect;
  connectable->disconnect = ags_play_channel_disconnect;
}

void
ags_play_channel_init(AgsPlayChannel *play_channel)
{
  play_channel->audio_channel = g_object_new(AGS_TYPE_PORT,
					     "plugin-name\0", g_strdup("ags-play\0"),
					     "specifier\0", "./audio-channel[0]\0",
					     "control-port\0", "1/1\0",
					     "port-value-is-pointer\0", FALSE,
					     "value-type\0", G_TYPE_UINT,
					     "value-size\0", sizeof(guint),
					     "value-length\0", 1,
					     NULL);
}


void
ags_play_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsPlayChannel *play_channel;

  play_channel = AGS_PLAY_CHANNEL(gobject);

  switch(prop_id){
  case PROP_AUDIO_CHANNEL:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == play_channel->audio_channel){
	return;
      }

      if(play_channel->audio_channel != NULL){
	g_object_unref(G_OBJECT(play_channel->audio_channel));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      play_channel->audio_channel = port;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_play_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsPlayChannel *play_channel;

  play_channel = AGS_PLAY_CHANNEL(gobject);

  switch(prop_id){
  case PROP_AUDIO_CHANNEL:
    {
      g_value_set_object(value, play_channel->audio_channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_channel_finalize(GObject *gobject)
{
  AgsPlayChannel *play_channel;

  play_channel = AGS_PLAY_CHANNEL(gobject);

  if(play_channel->audio_channel != NULL){
    g_object_unref(G_OBJECT(play_channel->audio_channel));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_channel_parent_class)->finalize(gobject);
}

void
ags_play_channel_connect(AgsConnectable *connectable)
{
  AgsPlayChannel *play_channel;

  ags_play_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_play_channel_disconnect(AgsConnectable *connectable)
{
  ags_play_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

AgsPlayChannel*
ags_play_channel_new(AgsDevout *devout,
		     guint audio_channel)
{
  AgsPlayChannel *play_channel;

  play_channel = (AgsPlayChannel *) g_object_new(AGS_TYPE_PLAY_CHANNEL,
						 "devout\0", devout,
						 NULL);
  
  play_channel->audio_channel->port_value.ags_port_uint = audio_channel;

  return(play_channel);
}
