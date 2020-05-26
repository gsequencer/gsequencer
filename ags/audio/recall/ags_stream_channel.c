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

#include <ags/audio/recall/ags_stream_channel.h>

#include <ags/i18n.h>

void ags_stream_channel_class_init(AgsStreamChannelClass *stream_channel);
void ags_stream_channel_init(AgsStreamChannel *stream_channel);
void ags_stream_channel_set_property(GObject *gobject,
				     guint prop_id,
				     const GValue *value,
				     GParamSpec *param_spec);
void ags_stream_channel_get_property(GObject *gobject,
				     guint prop_id,
				     GValue *value,
				     GParamSpec *param_spec);
void ags_stream_channel_dispose(GObject *gobject);
void ags_stream_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_stream_channel
 * @short_description: streams channel
 * @title: AgsStreamChannel
 * @section_id:
 * @include: ags/audio/recall/ags_stream_channel.h
 *
 * The #AgsStreamChannel class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_AUTO_SENSE,
};

static gpointer ags_stream_channel_parent_class = NULL;
static AgsConnectableInterface *ags_stream_channel_parent_connectable_interface;

const gchar *ags_stream_channel_plugin_name = "ags-stream";
const gchar *ags_stream_channel_plugin_specifier[] = {
  "./auto-sense[0]",
};
const gchar *ags_stream_channel_plugin_control_port[] = {
  "1/1",
};

GType
ags_stream_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_stream_channel = 0;

    static const GTypeInfo ags_stream_channel_info = {
      sizeof (AgsStreamChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_stream_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsStreamChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_stream_channel_init,
    };

    ags_type_stream_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						     "AgsStreamChannel",
						     &ags_stream_channel_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_stream_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_stream_channel_class_init(AgsStreamChannelClass *stream_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;

  GParamSpec *param_spec;

  ags_stream_channel_parent_class = g_type_class_peek_parent(stream_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) stream_channel;

  gobject->set_property = ags_stream_channel_set_property;
  gobject->get_property = ags_stream_channel_get_property;

  gobject->dispose = ags_stream_channel_dispose;
  gobject->finalize = ags_stream_channel_finalize;

  /* properties */
  /**
   * AgsStreamChannel:auto-sense:
   * 
   * The auto-sense port.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("auto-sense",
				   i18n_pspec("mute channel"),
				   i18n_pspec("Mute the channel"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUTO_SENSE,
				  param_spec);
}

void
ags_stream_channel_init(AgsStreamChannel *stream_channel)
{
  AgsConfig *config;
  
  GList *port;
  
  gchar *str;
  
  AGS_RECALL(stream_channel)->name = "ags-stream";
  AGS_RECALL(stream_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(stream_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(stream_channel)->xml_type = "ags-stream-channel";

  /* initialize port */
  port = NULL;

  /* auto-sense */
  stream_channel->auto_sense = g_object_new(AGS_TYPE_PORT,
				     "plugin-name", ags_stream_channel_plugin_name,
				     "specifier", ags_stream_channel_plugin_specifier[0],
				     "control-port", ags_stream_channel_plugin_control_port[0],
				     "port-value-is-pointer", FALSE,
				     "port-value-type", G_TYPE_BOOLEAN,
				     "port-value-size", sizeof(gboolean),
				     "port-value-length", 1,
				     NULL);
  g_object_ref(stream_channel->auto_sense);
  
  config = ags_config_get_instance();
  
  str = ags_config_get_value(config,
			     AGS_CONFIG_RECALL,
			     "auto-sense");
  stream_channel->auto_sense->port_value.ags_port_boolean = ((!g_strcmp0(str, "true")
							      ) ? TRUE: FALSE);
  g_free(str);

  /* add to port */
  port = g_list_prepend(port, stream_channel->auto_sense);
  g_object_ref(stream_channel->auto_sense);
  
  /* set port */
  AGS_RECALL(stream_channel)->port = port;

}


void
ags_stream_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsStreamChannel *stream_channel;

  GRecMutex *recall_mutex;

  stream_channel = AGS_STREAM_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(stream_channel);

  switch(prop_id){
  case PROP_AUTO_SENSE:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == stream_channel->auto_sense){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(stream_channel->auto_sense != NULL){
	g_object_unref(G_OBJECT(stream_channel->auto_sense));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      stream_channel->auto_sense = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_stream_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsStreamChannel *stream_channel;

  GRecMutex *recall_mutex;

  stream_channel = AGS_STREAM_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(stream_channel);

  switch(prop_id){
  case PROP_AUTO_SENSE:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, stream_channel->auto_sense);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_stream_channel_dispose(GObject *gobject)
{
  AgsStreamChannel *stream_channel;

  stream_channel = AGS_STREAM_CHANNEL(gobject);

  /* auto-sense */
  if(stream_channel->auto_sense != NULL){
    g_object_unref(G_OBJECT(stream_channel->auto_sense));

    stream_channel->auto_sense = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_stream_channel_parent_class)->dispose(gobject);
}

void
ags_stream_channel_finalize(GObject *gobject)
{
  AgsStreamChannel *stream_channel;

  stream_channel = AGS_STREAM_CHANNEL(gobject);

  /* auto-sense */
  if(stream_channel->auto_sense != NULL){
    g_object_unref(G_OBJECT(stream_channel->auto_sense));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_stream_channel_parent_class)->finalize(gobject);
}

/**
 * ags_stream_channel_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsStreamChannel
 *
 * Returns: the new #AgsStreamChannel
 *
 * Since: 3.0.0
 */
AgsStreamChannel*
ags_stream_channel_new(AgsChannel *source)
{
  AgsStreamChannel *stream_channel;

  stream_channel = (AgsStreamChannel *) g_object_new(AGS_TYPE_STREAM_CHANNEL,
						     "source", source,
						     NULL);

  return(stream_channel);
}
