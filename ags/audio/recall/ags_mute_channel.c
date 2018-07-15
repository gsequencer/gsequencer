/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
 *
 * This file is part of GSequencer.
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

#include <ags/audio/recall/ags_mute_channel.h>

#include <ags/libags.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_mute_channel_class_init(AgsMuteChannelClass *mute_channel);
void ags_mute_channel_mutable_interface_init(AgsMutableInterface *mutable);
void ags_mute_channel_plugin_interface_init(AgsPluginInterface *plugin);
void ags_mute_channel_init(AgsMuteChannel *mute_channel);
void ags_mute_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_mute_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_mute_channel_dispose(GObject *gobject);
void ags_mute_channel_finalize(GObject *gobject);

void ags_mute_channel_set_ports(AgsPlugin *plugin, GList *port);

void ags_mute_channel_set_muted(AgsMutable *mutable, gboolean muted);

static AgsPluginPort* ags_mute_channel_get_muted_plugin_port();

/**
 * SECTION:ags_mute_channel
 * @short_description: mutes channel
 * @title: AgsMuteChannel
 * @section_id:
 * @include: ags/audio/recall/ags_mute_channel.h
 *
 * The #AgsMuteChannel class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_MUTED,
};

static gpointer ags_mute_channel_parent_class = NULL;
static AgsPluginInterface *ags_mute_channel_parent_plugin_interface;

static const gchar *ags_mute_channel_plugin_name = "ags-mute";
static const gchar *ags_mute_channel_specifier[] = {
  "./muted[0]",
};
static const gchar *ags_mute_channel_control_port[] = {
  "1/1",
};

GType
ags_mute_channel_get_type()
{
  static GType ags_type_mute_channel = 0;

  if(!ags_type_mute_channel){
    static const GTypeInfo ags_mute_channel_info = {
      sizeof (AgsMuteChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_mute_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMuteChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_mute_channel_init,
    };

    static const GInterfaceInfo ags_mutable_interface_info = {
      (GInterfaceInitFunc) ags_mute_channel_mutable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_mute_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_mute_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsMuteChannel",
						   &ags_mute_channel_info,
						   0);

    g_type_add_interface_static(ags_type_mute_channel,
				AGS_TYPE_MUTABLE,
				&ags_mutable_interface_info);

    g_type_add_interface_static(ags_type_mute_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_mute_channel);
}

void
ags_mute_channel_mutable_interface_init(AgsMutableInterface *mutable)
{
  mutable->set_muted = ags_mute_channel_set_muted;
}

void
ags_mute_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  plugin->set_ports = ags_mute_channel_set_ports;
}

void
ags_mute_channel_class_init(AgsMuteChannelClass *mute_channel)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_mute_channel_parent_class = g_type_class_peek_parent(mute_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) mute_channel;

  gobject->set_property = ags_mute_channel_set_property;
  gobject->get_property = ags_mute_channel_get_property;

  gobject->dispose = ags_mute_channel_dispose;
  gobject->finalize = ags_mute_channel_finalize;

  /* properties */
  /**
   * AgsMuteChannel:muted:
   *
   * The mute port.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("muted",
				   i18n_pspec("mute channel"),
				   i18n_pspec("Mute the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MUTED,
				  param_spec);
}

void
ags_mute_channel_init(AgsMuteChannel *mute_channel)
{
  GList *port;

  AGS_RECALL(mute_channel)->name = "ags-mute";
  AGS_RECALL(mute_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(mute_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(mute_channel)->xml_type = "ags-mute-channel";

  port = NULL;

  /* muted */
  mute_channel->muted = g_object_new(AGS_TYPE_PORT,
				     "plugin-name", ags_mute_channel_plugin_name,
				     "specifier", ags_mute_channel_specifier[0],
				     "control-port", ags_mute_channel_control_port[0],
				     "port-value-is-pointer", FALSE,
				     "port-value-type", G_TYPE_FLOAT,
				     "port-value-size", sizeof(gfloat),
				     "port-value-length", 1,
				     NULL);
  g_object_ref(mute_channel->muted);

  mute_channel->muted->port_value.ags_port_float = 0.0;

  /* plugin port */
  g_object_set(mute_channel->muted,
	       "plugin-port", ags_mute_channel_get_muted_plugin_port(),
	       NULL);

  /* add to port */
  port = g_list_prepend(port, mute_channel->muted);
  g_object_ref(mute_channel->muted);

  /* set port */
  AGS_RECALL(mute_channel)->port = port;
}

void
ags_mute_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsMuteChannel *mute_channel;

  pthread_mutex_t *recall_mutex;

  mute_channel = AGS_MUTE_CHANNEL(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_MUTED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      pthread_mutex_lock(recall_mutex);

      if(port == mute_channel->muted){      
	pthread_mutex_unlock(recall_mutex);	

	return;
      }

      if(mute_channel->muted != NULL){
	g_object_unref(G_OBJECT(mute_channel->muted));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      mute_channel->muted = port;
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_mute_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsMuteChannel *mute_channel;

  pthread_mutex_t *recall_mutex;

  mute_channel = AGS_MUTE_CHANNEL(gobject);

  /* get recall mutex */
  pthread_mutex_lock(ags_recall_get_class_mutex());
  
  recall_mutex = AGS_RECALL(gobject)->obj_mutex;
  
  pthread_mutex_unlock(ags_recall_get_class_mutex());

  switch(prop_id){
  case PROP_MUTED:
    {
      pthread_mutex_lock(recall_mutex);

      g_value_set_object(value, mute_channel->muted);
      
      pthread_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_mute_channel_dispose(GObject *gobject)
{
  AgsMuteChannel *mute_channel;

  mute_channel = AGS_MUTE_CHANNEL(gobject);

  /* muted */
  if(mute_channel->muted != NULL){
    g_object_unref(G_OBJECT(mute_channel->muted));

    mute_channel->muted = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_mute_channel_parent_class)->dispose(gobject);
}

void
ags_mute_channel_finalize(GObject *gobject)
{
  AgsMuteChannel *mute_channel;

  mute_channel = AGS_MUTE_CHANNEL(gobject);

  /* muted */
  if(mute_channel->muted != NULL){
    g_object_unref(G_OBJECT(mute_channel->muted));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_mute_channel_parent_class)->finalize(gobject);
}

void
ags_mute_channel_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"muted[0]",
		9)){
      g_object_set(G_OBJECT(plugin),
		   "muted", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

void
ags_mute_channel_set_muted(AgsMutable *mutable, gboolean muted)
{
  AgsPort *port;
  
  GValue value = {0,};

  g_object_get(G_OBJECT(mutable),
	       "muted", &port,
	       NULL);
  
  g_value_init(&value,
	       G_TYPE_FLOAT);

  g_value_set_float(&value,
		    (muted ? 1.0: 0.0));

  ags_port_safe_write(port,
		      &value);
}

static AgsPluginPort*
ags_mute_channel_get_muted_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_mutex_lock(&mutex);

  if(plugin_port == NULL){
    plugin_port = ags_plugin_port_new();
    g_object_ref(plugin_port);

    plugin_port->flags |= (AGS_PLUGIN_PORT_INPUT |
			       AGS_PLUGIN_PORT_CONTROL |
			       AGS_PLUGIN_PORT_TOGGLED);

    plugin_port->port_index = 0;

    /* range */
    g_value_init(plugin_port->default_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->lower_value,
		 G_TYPE_FLOAT);
    g_value_init(plugin_port->upper_value,
		 G_TYPE_FLOAT);

    g_value_set_float(plugin_port->default_value,
		      0.0);
    g_value_set_float(plugin_port->lower_value,
		      0.0);
    g_value_set_float(plugin_port->upper_value,
		      1.0);
  }

  pthread_mutex_unlock(&mutex);
  
  return(plugin_port);
}

/**
 * ags_mute_channel_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsMuteChannel
 *
 * Returns: the new #AgsMuteChannel
 *
 * Since: 2.0.0
 */
AgsMuteChannel*
ags_mute_channel_new(AgsChannel *source)
{
  AgsMuteChannel *mute_channel;

  mute_channel = (AgsMuteChannel *) g_object_new(AGS_TYPE_MUTE_CHANNEL,
						 "source", source,
						 NULL);

  return(mute_channel);
}
