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

#include <ags/audio/recall/ags_copy_channel.h>

#include <ags/i18n.h>

void ags_copy_channel_class_init(AgsCopyChannelClass *copy_channel);
void ags_copy_channel_mutable_interface_init(AgsMutableInterface *mutable);
void ags_copy_channel_init(AgsCopyChannel *copy_channel);
void ags_copy_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_copy_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_copy_channel_dispose(GObject *gobject);
void ags_copy_channel_finalize(GObject *gobject);

void ags_copy_channel_set_muted(AgsMutable *mutable, gboolean muted);

/**
 * SECTION:ags_copy_channel
 * @short_description: copys channel
 * @title: AgsCopyChannel
 * @section_id:
 * @include: ags/audio/recall/ags_copy_channel.h
 *
 * The #AgsCopyChannel class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_MUTED,
};

static gpointer ags_copy_channel_parent_class = NULL;
static AgsMutableInterface *ags_copy_channel_parent_mutable_interface;
static AgsPluginInterface *ags_copy_channel_parent_plugin_interface;

static const gchar *ags_copy_channel_plugin_name = "ags-copy";
static const gchar *ags_copy_channel_plugin_specifier[] = {
  "./muted[0]",
};
static const gchar *ags_copy_channel_plugin_control_port[] = {
  "1/1",
};

GType
ags_copy_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_copy_channel = 0;

    static const GTypeInfo ags_copy_channel_info = {
      sizeof(AgsCopyChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCopyChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_channel_init,
    };

    static const GInterfaceInfo ags_mutable_interface_info = {
      (GInterfaceInitFunc) ags_copy_channel_mutable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    ags_type_copy_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsCopyChannel",
						   &ags_copy_channel_info,
						   0);

    g_type_add_interface_static(ags_type_copy_channel,
				AGS_TYPE_MUTABLE,
				&ags_mutable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_copy_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_copy_channel_mutable_interface_init(AgsMutableInterface *mutable)
{
  ags_copy_channel_parent_mutable_interface = g_type_interface_peek_parent(mutable);

  mutable->set_muted = ags_copy_channel_set_muted;
}

void
ags_copy_channel_class_init(AgsCopyChannelClass *copy_channel)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_copy_channel_parent_class = g_type_class_peek_parent(copy_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_channel;

  gobject->set_property = ags_copy_channel_set_property;
  gobject->get_property = ags_copy_channel_get_property;

  gobject->dispose = ags_copy_channel_dispose;
  gobject->finalize = ags_copy_channel_finalize;

  /* properties */
  /**
   * AgsCopyChannel:muted:
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
ags_copy_channel_init(AgsCopyChannel *copy_channel)
{
  GList *port;

  AGS_RECALL(copy_channel)->name = "ags-copy";
  AGS_RECALL(copy_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(copy_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(copy_channel)->xml_type = "ags-copy-channel";

  port = NULL;

  copy_channel->muted = g_object_new(AGS_TYPE_PORT,
				     "plugin-name", ags_copy_channel_plugin_name,
				     "specifier", ags_copy_channel_plugin_specifier[0],
				     "control-port", ags_copy_channel_plugin_control_port[0],
				     "port-value-is-pointer", FALSE,
				     "port-value-type", G_TYPE_FLOAT,
				     "port-value-size", sizeof(gfloat),
				     "port-value-length", 1,
				     NULL);
  g_object_ref(copy_channel->muted);
  copy_channel->muted->port_value.ags_port_float = (float) FALSE;

  /* add to port */
  port = g_list_prepend(port, copy_channel->muted);
  g_object_ref(copy_channel->muted);

  /* set port */
  AGS_RECALL(copy_channel)->port = port;
}

void
ags_copy_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsCopyChannel *copy_channel;

  GRecMutex *recall_mutex;
  
  copy_channel = AGS_COPY_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(copy_channel);

  switch(prop_id){
  case PROP_MUTED:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(port == copy_channel->muted){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(copy_channel->muted != NULL){
	g_object_unref(G_OBJECT(copy_channel->muted));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      copy_channel->muted = port;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_copy_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsCopyChannel *copy_channel;

  GRecMutex *recall_mutex;
  
  copy_channel = AGS_COPY_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(copy_channel);

  switch(prop_id){
  case PROP_MUTED:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, copy_channel->muted);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_copy_channel_dispose(GObject *gobject)
{
  AgsCopyChannel *copy_channel;

  copy_channel = AGS_COPY_CHANNEL(gobject);

  if(copy_channel->muted != NULL){
    g_object_unref(G_OBJECT(copy_channel->muted));

    copy_channel->muted = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_copy_channel_parent_class)->dispose(gobject);
}

void
ags_copy_channel_finalize(GObject *gobject)
{
  AgsCopyChannel *copy_channel;

  copy_channel = AGS_COPY_CHANNEL(gobject);

  if(copy_channel->muted != NULL){
    g_object_unref(G_OBJECT(copy_channel->muted));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_copy_channel_parent_class)->finalize(gobject);
}

void
ags_copy_channel_set_muted(AgsMutable *mutable, gboolean muted)
{
  AgsPort *port;
  
  GValue value = {0,};

  g_object_get(G_OBJECT(mutable),
	       "muted", &port,
	       NULL);
  
  g_value_init(&value,
	       G_TYPE_FLOAT);
  
  g_value_set_float(&value,
		    (float) (muted ? 1.0: 0.0));

  ags_port_safe_write(port,
		      &value);

  g_value_unset(&value);
  g_object_unref(port);
}

/**
 * ags_copy_channel_new:
 * @destination: the destination #AgsChannel
 * @source: the source #AgsChannel
 *
 * Create a new instance of #AgsCopyChannel
 *
 * Returns: the new #AgsCopyChannel
 *
 * Since: 2.0.0
 */
AgsCopyChannel*
ags_copy_channel_new(AgsChannel *destination,
		     AgsChannel *source)
{
  AgsCopyChannel *copy_channel;

  copy_channel = (AgsCopyChannel *) g_object_new(AGS_TYPE_COPY_CHANNEL,
						 "destination", destination,
						 "source", source,
						 NULL);

  return(copy_channel);
}
