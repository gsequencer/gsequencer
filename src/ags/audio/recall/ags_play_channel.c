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

#include <ags/audio/recall/ags_play_channel.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_mutable.h>
#include <ags/object/ags_plugin.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_audio.h>

#include <stdlib.h>
#include <stdio.h>

void ags_play_channel_class_init(AgsPlayChannelClass *play_channel);
void ags_play_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_play_channel_mutable_interface_init(AgsMutableInterface *mutable);
void ags_play_channel_plugin_interface_init(AgsPluginInterface *plugin);
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
void ags_play_channel_set_ports(AgsPlugin *plugin, GList *port);
void ags_play_channel_finalize(GObject *gobject);

void ags_play_channel_set_muted(AgsMutable *mutable, gboolean muted);

enum{
  PROP_0,
  PROP_AUDIO_CHANNEL,
  PROP_MUTED,
};

/**
 * SECTION:ags_play_channel
 * @short_description: plays channel
 * @title: AgsPlayChannel
 * @section_id:
 * @include: ags/audio/recall/ags_play_channel.h
 *
 * The #AgsPlayChannel class provides ports to the effect processor.
 */

static gpointer ags_play_channel_parent_class = NULL;
static AgsConnectableInterface *ags_play_channel_parent_connectable_interface;
static AgsMutableInterface *ags_play_channel_parent_mutable_interface;
static AgsPluginInterface *ags_play_channel_parent_plugin_interface;

static const gchar *ags_play_channel_plugin_name = "ags-play\0";
static const gchar *ags_play_channel_specifier[] = {
  "./audio-channel[0]\0",
  "./muted[0]\0",
};
static const gchar *ags_play_channel_control_port[] = {
  "1/2\0",
  "2/2\0",
};

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

    static const GInterfaceInfo ags_mutable_interface_info = {
      (GInterfaceInitFunc) ags_play_channel_mutable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_play_channel_plugin_interface_init,
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

    g_type_add_interface_static(ags_type_play_channel,
				AGS_TYPE_MUTABLE,
				&ags_mutable_interface_info);

    g_type_add_interface_static(ags_type_play_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
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
				   "assigned audio channel\0",
				   "The audio channel this recall does output to\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);

  param_spec = g_param_spec_object("muted\0",
				   "mute channel\0",
				   "Mute the channel\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MUTED,
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
ags_play_channel_mutable_interface_init(AgsMutableInterface *mutable)
{
  ags_play_channel_parent_mutable_interface = g_type_interface_peek_parent(mutable);

  mutable->set_muted = ags_play_channel_set_muted;
}

void
ags_play_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_play_channel_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_play_channel_set_ports;
}

void
ags_play_channel_init(AgsPlayChannel *play_channel)
{
  GList *port;

  AGS_RECALL(play_channel)->name = "ags-play\0";
  AGS_RECALL(play_channel)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(play_channel)->build_id = AGS_BUILD_ID;
  AGS_RECALL(play_channel)->xml_type = "ags-play-channel\0";

  port = NULL;

  play_channel->audio_channel = g_object_new(AGS_TYPE_PORT,
					     "plugin-name\0", ags_play_channel_plugin_name,
					     "specifier\0", ags_play_channel_specifier[0],
					     "control-port\0", ags_play_channel_control_port[0],
					     "port-value-is-pointer\0", FALSE,
					     "port-value-type\0", G_TYPE_UINT64,
					     "port-value-size\0", sizeof(guint64),
					     "port-value-length\0", 1,
					     NULL);
  play_channel->audio_channel->port_value.ags_port_uint = 0;

  port = g_list_prepend(port, play_channel->audio_channel);

  play_channel->muted = g_object_new(AGS_TYPE_PORT,
				     "plugin-name\0", ags_play_channel_plugin_name,
				     "specifier\0", ags_play_channel_specifier[1],
				     "control-port\0", ags_play_channel_control_port[1],
				     "port-value-is-pointer\0", FALSE,
				     "port-value-type\0", G_TYPE_BOOLEAN,
				     "port-value-size\0", sizeof(gboolean),
				     "port-value-length\0", 1,
				     NULL);
  play_channel->muted->port_value.ags_port_boolean = FALSE;

  port = g_list_prepend(port, play_channel->muted);

  /* set port */
  AGS_RECALL(play_channel)->port = port;
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
  case PROP_MUTED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == play_channel->muted){
	return;
      }

      if(play_channel->muted != NULL){
	g_object_unref(G_OBJECT(play_channel->muted));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      play_channel->muted = port;
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
  case PROP_MUTED:
    {
      g_value_set_object(value, play_channel->muted);
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

  if(play_channel->muted != NULL){
    g_object_unref(G_OBJECT(play_channel->muted));
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

void
ags_play_channel_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./audio-channel[0]\0",
		17)){
      g_object_set(G_OBJECT(plugin),
		   "audio-channel\0", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		"./muted[0]\0",
		9)){
      g_object_set(G_OBJECT(plugin),
		   "muted\0", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

void
ags_play_channel_set_muted(AgsMutable *mutable, gboolean muted)
{
  GValue value = {0,};

  g_value_init(&value, G_TYPE_BOOLEAN);
  g_value_set_boolean(&value, muted);

  ags_port_safe_write(AGS_PLAY_CHANNEL(mutable)->muted, &value);
}

/**
 * ags_play_channel_new:
 * @devout: the #AgsDevout outputting to
 * @audio_channel: the audio channel to use
 *
 * Creates an #AgsPlayChannel
 *
 * Returns: a new #AgsPlayChannel
 *
 * Since: 0.4
 */
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
