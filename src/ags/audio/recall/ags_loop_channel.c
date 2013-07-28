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

#include <ags/audio/recall/ags_loop_channel.h>

#include <ags-lib/object/ags_connectable.h>

#include <math.h>

void ags_loop_channel_class_init(AgsLoopChannelClass *loop_channel);
void ags_loop_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_loop_channel_init(AgsLoopChannel *loop_channel);
void ags_loop_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_loop_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_loop_channel_connect(AgsConnectable *connectable);
void ags_loop_channel_disconnect(AgsConnectable *connectable);
void ags_loop_channel_finalize(GObject *gobject);

void ags_loop_channel_sequencer_duration_changed_callback(AgsDelayAudio *delay_audio,
							  AgsLoopChannel *loop_channel);

static gpointer ags_loop_channel_parent_class = NULL;
static AgsConnectableInterface *ags_loop_channel_parent_connectable_interface;

enum{
  PROP_0,
  PROP_DELAY_AUDIO,
};

GType
ags_loop_channel_get_type()
{
  static GType ags_type_loop_channel = 0;

  if(!ags_type_loop_channel){
    static const GTypeInfo ags_loop_channel_info = {
      sizeof (AgsLoopChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_loop_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLoopChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_loop_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_loop_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_loop_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsLoopChannel\0",
						   &ags_loop_channel_info,
						   0);
    
    g_type_add_interface_static(ags_type_loop_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_loop_channel);
}

void
ags_loop_channel_class_init(AgsLoopChannelClass *loop_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_loop_channel_parent_class = g_type_class_peek_parent(loop_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) loop_channel;

  gobject->set_property = ags_loop_channel_set_property;
  gobject->get_property = ags_loop_channel_get_property;

  gobject->finalize = ags_loop_channel_finalize;

  /* properties */
  param_spec = g_param_spec_object("delay_audio\0",
				   "assigned delay-audio\0",
				   "The delay-audio it is assigned with\0",
				   AGS_TYPE_DELAY_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_AUDIO,
				  param_spec);
}

void
ags_loop_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_loop_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_loop_channel_connect;
  connectable->disconnect = ags_loop_channel_disconnect;
}

void
ags_loop_channel_init(AgsLoopChannel *loop_channel)
{
  loop_channel->delay_audio = NULL;
  loop_channel->sequencer_duration_changed_handler = 0;
}

void
ags_loop_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsLoopChannel *loop_channel;

  loop_channel = AGS_LOOP_CHANNEL(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO:
    {
      AgsDelayAudio *delay_audio;

      delay_audio = (AgsDelayAudio *) g_value_get_object(value);

      if(loop_channel->delay_audio == delay_audio)
	return;

      if(loop_channel->delay_audio != NULL)
	g_object_unref(G_OBJECT(loop_channel->delay_audio));

      if(delay_audio != NULL)
	g_object_ref(G_OBJECT(delay_audio));
	
      loop_channel->delay_audio = delay_audio;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_loop_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsLoopChannel *loop_channel;

  loop_channel = AGS_LOOP_CHANNEL(gobject);

  switch(prop_id){
  case PROP_DELAY_AUDIO:
    g_value_set_object(value, loop_channel->delay_audio);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_loop_channel_finalize(GObject *gobject)
{
  AgsLoopChannel *loop_channel;

  loop_channel = AGS_LOOP_CHANNEL(gobject);

  if(loop_channel->delay_audio != NULL){
    g_object_unref(G_OBJECT(loop_channel->delay_audio));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_loop_channel_parent_class)->finalize(gobject);
}

void
ags_loop_channel_connect(AgsConnectable *connectable)
{
  AgsLoopChannel *loop_channel;

  ags_loop_channel_parent_connectable_interface->connect(connectable);

  /*  */
  loop_channel = AGS_LOOP_CHANNEL(connectable);

  if(loop_channel->delay_audio != NULL){
    loop_channel->sequencer_duration_changed_handler = g_signal_connect(G_OBJECT(loop_channel->delay_audio), "sequencer_duration_changed\0",
									G_CALLBACK(ags_loop_channel_sequencer_duration_changed_callback), loop_channel);
  }
}

void
ags_loop_channel_disconnect(AgsConnectable *connectable)
{
  AgsLoopChannel *loop_channel;
  
  ags_loop_channel_parent_connectable_interface->disconnect(connectable);

  /*  */
  loop_channel = AGS_LOOP_CHANNEL(connectable);

  if(loop_channel->delay_audio != NULL){
    g_signal_handler_disconnect(G_OBJECT(loop_channel->delay_audio),
				loop_channel->sequencer_duration_changed_handler);
  }
}


void
ags_loop_channel_sequencer_duration_changed_callback(AgsDelayAudio *delay_audio,
						     AgsLoopChannel *loop_channel)
{
  /* resize audio signal */
  ags_channel_resize_audio_signal(AGS_RECALL_CHANNEL(loop_channel)->source,
				  (guint) ceil(delay_audio->sequencer_duration * delay_audio->sequencer_delay));
}

AgsLoopChannel*
ags_loop_channel_new()
{
  AgsLoopChannel *loop_channel;

  loop_channel = (AgsLoopChannel *) g_object_new(AGS_TYPE_LOOP_CHANNEL,
						 NULL);

  return(loop_channel);
}

