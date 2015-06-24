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

#include <ags/audio/recall/ags_clone_channel.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_plugin.h>

#include <ags/audio/ags_audio.h>

#include <stdlib.h>
#include <stdio.h>

void ags_clone_channel_class_init(AgsCloneChannelClass *clone_channel);
void ags_clone_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_clone_channel_plugin_interface_init(AgsPluginInterface *plugin);
void ags_clone_channel_init(AgsCloneChannel *clone_channel);
void ags_clone_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_clone_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_clone_channel_connect(AgsConnectable *connectable);
void ags_clone_channel_disconnect(AgsConnectable *connectable);
void ags_clone_channel_finalize(GObject *gobject);
void ags_clone_channel_set_ports(AgsPlugin *plugin, GList *port);

enum{
  PROP_0,
  PROP_AUDIO_CHANNEL,
};

static gpointer ags_clone_channel_parent_class = NULL;
static AgsConnectableInterface *ags_clone_channel_parent_connectable_interface;
static AgsPluginInterface *ags_clone_channel_parent_plugin_interface;

GType
ags_clone_channel_get_type()
{
  static GType ags_type_clone_channel = 0;

  if(!ags_type_clone_channel){
    static const GTypeInfo ags_clone_channel_info = {
      sizeof (AgsCloneChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_clone_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCloneChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_clone_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_clone_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_clone_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_clone_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						    "AgsCloneChannel\0",
						    &ags_clone_channel_info,
						    0);
    
    g_type_add_interface_static(ags_type_clone_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_clone_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_clone_channel);
}

void
ags_clone_channel_class_init(AgsCloneChannelClass *clone_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_clone_channel_parent_class = g_type_class_peek_parent(clone_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) clone_channel;

  gobject->set_property = ags_clone_channel_set_property;
  gobject->get_property = ags_clone_channel_get_property;

  gobject->finalize = ags_clone_channel_finalize;

  /* properties */
  param_spec = g_param_spec_uint("audio_channel\0",
				 "assigned audio Channel\0",
				 "The audio channel this recall does output to\0",
				 0,
				 65536,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);
}

void
ags_clone_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_clone_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_clone_channel_connect;
  connectable->disconnect = ags_clone_channel_disconnect;
}

void
ags_clone_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_clone_channel_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_clone_channel_set_ports;
}

void
ags_clone_channel_init(AgsCloneChannel *clone_channel)
{
  clone_channel->audio_channel = 0;
}

void
ags_clone_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsCloneChannel *clone_channel;

  clone_channel = AGS_CLONE_CHANNEL(gobject);

  switch(prop_id){
  case PROP_AUDIO_CHANNEL:
    {
      clone_channel->audio_channel = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_clone_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsCloneChannel *clone_channel;

  clone_channel = AGS_CLONE_CHANNEL(gobject);

  switch(prop_id){
  case PROP_AUDIO_CHANNEL:
    {
      g_value_set_uint(value, clone_channel->audio_channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_clone_channel_finalize(GObject *gobject)
{
  AgsCloneChannel *clone_channel;

  clone_channel = AGS_CLONE_CHANNEL(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_clone_channel_parent_class)->finalize(gobject);
}

void
ags_clone_channel_connect(AgsConnectable *connectable)
{
  AgsCloneChannel *clone_channel;

  ags_clone_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_clone_channel_disconnect(AgsConnectable *connectable)
{
  ags_clone_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_clone_channel_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"muted[0]\0",
		9)){
      g_object_set(G_OBJECT(plugin),
		   "muted\0", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

AgsCloneChannel*
ags_clone_channel_new(AgsDevout *devout,
		     guint audio_channel)
{
  AgsCloneChannel *clone_channel;

  clone_channel = (AgsCloneChannel *) g_object_new(AGS_TYPE_CLONE_CHANNEL,
						   "devout\0", devout,
						   "audio_channel\0", audio_channel,
						   NULL);
  
  return(clone_channel);
}
