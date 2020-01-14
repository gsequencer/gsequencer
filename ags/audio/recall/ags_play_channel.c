/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/recall/ags_play_channel.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/i18n.h>

void ags_play_channel_class_init(AgsPlayChannelClass *play_channel);
void ags_play_channel_mutable_interface_init(AgsMutableInterface *mutable);
void ags_play_channel_init(AgsPlayChannel *play_channel);
void ags_play_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_play_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_play_channel_dispose(GObject *gobject);
void ags_play_channel_finalize(GObject *gobject);

void ags_play_channel_set_muted(AgsMutable *mutable, gboolean muted);

static AgsPluginPort* ags_play_channel_get_muted_plugin_port();

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

static const gchar *ags_play_channel_plugin_name = "ags-play";
static const gchar *ags_play_channel_specifier[] = {
  "./audio-channel[0]",
  "./muted[0]",
};
static const gchar *ags_play_channel_control_port[] = {
  "1/2",
  "2/2",
};

GType
ags_play_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_play_channel = 0;

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

    static const GInterfaceInfo ags_mutable_interface_info = {
      (GInterfaceInitFunc) ags_play_channel_mutable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_play_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsPlayChannel",
						   &ags_play_channel_info,
						   0);

    g_type_add_interface_static(ags_type_play_channel,
				AGS_TYPE_MUTABLE,
				&ags_mutable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_play_channel);
  }

  return g_define_type_id__volatile;
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

  gobject->dispose = ags_play_channel_dispose;
  gobject->finalize = ags_play_channel_finalize;

  /* properties */
  /**
   * AgsPlayChannel:audio-channel:
   * 
   * The audio channel port.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio-channel",
				   i18n_pspec("assigned audio channel"),
				   i18n_pspec("The audio channel this recall does output to"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);

  /**
   * AgsPlayChannel:muted:
   * 
   * The muted port.
   * 
   * Since: 3.0.0
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
ags_play_channel_mutable_interface_init(AgsMutableInterface *mutable)
{
  mutable->set_muted = ags_play_channel_set_muted;
}

void
ags_play_channel_init(AgsPlayChannel *play_channel)
{
  GList *port;

  AGS_RECALL(play_channel)->name = "ags-play";
  AGS_RECALL(play_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_channel)->xml_type = "ags-play-channel";

  port = NULL;

  /* audio channel */
  play_channel->audio_channel = g_object_new(AGS_TYPE_PORT,
					     "plugin-name", ags_play_channel_plugin_name,
					     "specifier", ags_play_channel_specifier[0],
					     "control-port", ags_play_channel_control_port[0],
					     "port-value-is-pointer", FALSE,
					     "port-value-type", G_TYPE_UINT64,
					     "port-value-size", sizeof(guint64),
					     "port-value-length", 1,
					     NULL);
  g_object_ref(play_channel->audio_channel);

  play_channel->audio_channel->port_value.ags_port_uint = 0;

  /* add to port */
  port = g_list_prepend(port,
			play_channel->audio_channel);
  g_object_ref(play_channel->audio_channel);

  /* muted */
  play_channel->muted = g_object_new(AGS_TYPE_PORT,
				     "plugin-name", ags_play_channel_plugin_name,
				     "specifier", ags_play_channel_specifier[1],
				     "control-port", ags_play_channel_control_port[1],
				     "port-value-is-pointer", FALSE,
				     "port-value-type", G_TYPE_FLOAT,
				     "port-value-size", sizeof(gfloat),
				     "port-value-length", 1,
				     NULL);
  g_object_ref(play_channel->muted);

  play_channel->muted->port_value.ags_port_float = (float) 0.0;

  /* plugin port */
  g_object_set(play_channel->muted,
	       "plugin-port", ags_play_channel_get_muted_plugin_port(),
	       NULL);
  
  /* add to port */
  port = g_list_prepend(port,
			play_channel->muted);
  g_object_ref(play_channel->muted);

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

  GRecMutex *recall_mutex;

  play_channel = AGS_PLAY_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_channel);

  switch(prop_id){
  case PROP_AUDIO_CHANNEL:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == play_channel->audio_channel){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(play_channel->audio_channel != NULL){
	g_object_unref(G_OBJECT(play_channel->audio_channel));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      play_channel->audio_channel = port;
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_MUTED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == play_channel->muted){
	g_rec_mutex_unlock(recall_mutex);	

	return;
      }

      if(play_channel->muted != NULL){
	g_object_unref(G_OBJECT(play_channel->muted));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      play_channel->muted = port;
      
      g_rec_mutex_unlock(recall_mutex);	
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

  GRecMutex *recall_mutex;

  play_channel = AGS_PLAY_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_channel);

  switch(prop_id){
  case PROP_AUDIO_CHANNEL:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, play_channel->audio_channel);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_MUTED:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, play_channel->muted);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_channel_dispose(GObject *gobject)
{
  AgsPlayChannel *play_channel;

  play_channel = AGS_PLAY_CHANNEL(gobject);

  /* audio channel */
  if(play_channel->audio_channel != NULL){
    g_object_unref(G_OBJECT(play_channel->audio_channel));

    play_channel->audio_channel = NULL;
  }

  /* muted */
  if(play_channel->muted != NULL){
    g_object_unref(G_OBJECT(play_channel->muted));

    play_channel->muted = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_channel_parent_class)->dispose(gobject);
}

void
ags_play_channel_finalize(GObject *gobject)
{
  AgsPlayChannel *play_channel;

  play_channel = AGS_PLAY_CHANNEL(gobject);

  /* audio channel */
  if(play_channel->audio_channel != NULL){
    g_object_unref(G_OBJECT(play_channel->audio_channel));
  }

  /* muted */
  if(play_channel->muted != NULL){
    g_object_unref(G_OBJECT(play_channel->muted));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_channel_parent_class)->finalize(gobject);
}

void
ags_play_channel_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./audio-channel[0]",
		17)){
      g_object_set(G_OBJECT(plugin),
		   "audio-channel", AGS_PORT(port->data),
		   NULL);
    }else if(!strncmp(AGS_PORT(port->data)->specifier,
		"./muted[0]",
		9)){
      g_object_set(G_OBJECT(plugin),
		   "muted", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

void
ags_play_channel_set_muted(AgsMutable *mutable, gboolean muted)
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

  g_value_unset(&value);

  g_object_unref(port);
}

static AgsPluginPort*
ags_play_channel_get_muted_plugin_port()
{
  static AgsPluginPort *plugin_port = NULL;

  static GMutex mutex;

  g_mutex_lock(&mutex);
  
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

  g_mutex_unlock(&mutex);
  
  return(plugin_port);
}

/**
 * ags_play_channel_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsPlayChannel
 *
 * Returns: the new #AgsPlayChannel
 *
 * Since: 3.0.0
 */
AgsPlayChannel*
ags_play_channel_new(AgsChannel *source)
{
  AgsPlayChannel *play_channel;

  play_channel = (AgsPlayChannel *) g_object_new(AGS_TYPE_PLAY_CHANNEL,
						 "source", source,
						 NULL);

  return(play_channel);
}
