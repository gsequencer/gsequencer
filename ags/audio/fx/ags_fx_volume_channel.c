/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_volume_channel.h>

#include <ags/i18n.h>

void ags_fx_volume_channel_class_init(AgsFxVolumeChannelClass *fx_volume_channel);
void ags_fx_volume_channel_init(AgsFxVolumeChannel *fx_volume_channel);
void ags_fx_volume_channel_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_fx_volume_channel_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_fx_volume_channel_dispose(GObject *gobject);
void ags_fx_volume_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_volume_channel
 * @short_description: fx volume channel
 * @title: AgsFxVolumeChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_volume_channel.h
 *
 * The #AgsFxVolumeChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_volume_channel_parent_class = NULL;

static const gchar *ags_fx_volume_channel_plugin_name = "ags-fx-volume";

static const gchar *ags_fx_volume_channel_specifier[] = {
  "./muted[0]",
  "./volume[0]",
  NULL,
};

static const gchar *ags_fx_volume_channel_control_port[] = {
  "1/2",
  "2/2",
  NULL,
};

enum{
  PROP_0,
  PROP_MUTED,
  PROP_VOLUME,
};

GType
ags_fx_volume_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_volume_channel = 0;

    static const GTypeInfo ags_fx_volume_channel_info = {
      sizeof (AgsFxVolumeChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_volume_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxVolumeChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_volume_channel_init,
    };

    ags_type_fx_volume_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							"AgsFxVolumeChannel",
							&ags_fx_volume_channel_info,
							0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_volume_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_volume_channel_class_init(AgsFxVolumeChannelClass *fx_volume_channel)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_volume_channel_parent_class = g_type_class_peek_parent(fx_volume_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_volume_channel;

  gobject->set_property = ags_fx_volume_channel_set_property;
  gobject->get_property = ags_fx_volume_channel_get_property;

  gobject->dispose = ags_fx_volume_channel_dispose;
  gobject->finalize = ags_fx_volume_channel_finalize;

  /* properties */
  /**
   * AgsFxVolumeChannel:muted:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("muted",
				   i18n_pspec("muted of recall"),
				   i18n_pspec("The recall's muted"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MUTED,
				  param_spec);

  /**
   * AgsFxVolumeChannel:volume:
   *
   * The beats per minute.
   * 
   * Since: 3.3.0
   */
  param_spec = g_param_spec_object("volume",
				   i18n_pspec("volume of recall"),
				   i18n_pspec("The recall's volume"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VOLUME,
				  param_spec);
}

void
ags_fx_volume_channel_init(AgsFxVolumeChannel *fx_volume_channel)
{
  AGS_RECALL(fx_volume_channel)->name = "ags-fx-volume";
  AGS_RECALL(fx_volume_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_volume_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_volume_channel)->xml_type = "ags-fx-volume-channel";

  /* muted */
  fx_volume_channel->muted = g_object_new(AGS_TYPE_PORT,
					"plugin-name", ags_fx_volume_channel_plugin_name,
					"specifier", ags_fx_volume_channel_specifier[0],
					"control-port", ags_fx_volume_channel_control_port[0],
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_FLOAT,
					"port-value-size", sizeof(gfloat),
					"port-value-length", 1,
					NULL);
  
  fx_volume_channel->muted->port_value.ags_port_float = (gfloat) FALSE;

  ags_recall_add_port((AgsRecall *) fx_volume_channel,
		      fx_volume_channel->muted);

  /* volume */
  fx_volume_channel->volume = g_object_new(AGS_TYPE_PORT,
					"plugin-name", ags_fx_volume_channel_plugin_name,
					"specifier", ags_fx_volume_channel_specifier[1],
					"control-port", ags_fx_volume_channel_control_port[1],
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_FLOAT,
					"port-value-size", sizeof(gfloat),
					"port-value-length", 1,
					NULL);
  
  fx_volume_channel->volume->port_value.ags_port_float = (gfloat) 1.0;

  ags_recall_add_port((AgsRecall *) fx_volume_channel,
		      fx_volume_channel->volume);
}

void
ags_fx_volume_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsFxVolumeChannel *fx_volume_channel;

  GRecMutex *recall_mutex;

  fx_volume_channel = AGS_FX_VOLUME_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_volume_channel);

  switch(prop_id){
  case PROP_MUTED:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_volume_channel->muted){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_volume_channel->muted != NULL){
      g_object_unref(G_OBJECT(fx_volume_channel->muted));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_volume_channel->muted = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_VOLUME:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    g_rec_mutex_lock(recall_mutex);

    if(port == fx_volume_channel->volume){
      g_rec_mutex_unlock(recall_mutex);	

      return;
    }

    if(fx_volume_channel->volume != NULL){
      g_object_unref(G_OBJECT(fx_volume_channel->volume));
    }
      
    if(port != NULL){
      g_object_ref(G_OBJECT(port));
    }

    fx_volume_channel->volume = port;
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_fx_volume_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsFxVolumeChannel *fx_volume_channel;

  GRecMutex *recall_mutex;

  fx_volume_channel = AGS_FX_VOLUME_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_volume_channel);

  switch(prop_id){
  case PROP_MUTED:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_volume_channel->muted);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  case PROP_VOLUME:
  {
    g_rec_mutex_lock(recall_mutex);

    g_value_set_object(value, fx_volume_channel->volume);
      
    g_rec_mutex_unlock(recall_mutex);	
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fx_volume_channel_dispose(GObject *gobject)
{
  AgsFxVolumeChannel *fx_volume_channel;
  
  fx_volume_channel = AGS_FX_VOLUME_CHANNEL(gobject);

  /* muted */
  if(fx_volume_channel->muted != NULL){
    g_object_unref(G_OBJECT(fx_volume_channel->muted));

    fx_volume_channel->muted = NULL;
  }  

  /* volume */
  if(fx_volume_channel->volume != NULL){
    g_object_unref(G_OBJECT(fx_volume_channel->volume));

    fx_volume_channel->volume = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_volume_channel_parent_class)->dispose(gobject);
}

void
ags_fx_volume_channel_finalize(GObject *gobject)
{
  AgsFxVolumeChannel *fx_volume_channel;
  
  fx_volume_channel = AGS_FX_VOLUME_CHANNEL(gobject);

  /* muted */
  if(fx_volume_channel->muted != NULL){
    g_object_unref(G_OBJECT(fx_volume_channel->muted));
  }

  /* volume */
  if(fx_volume_channel->volume != NULL){
    g_object_unref(G_OBJECT(fx_volume_channel->volume));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_volume_channel_parent_class)->finalize(gobject);
}

/**
 * ags_fx_volume_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxVolumeChannel
 *
 * Returns: the new #AgsFxVolumeChannel
 *
 * Since: 3.3.0
 */
AgsFxVolumeChannel*
ags_fx_volume_channel_new(AgsChannel *channel)
{
  AgsFxVolumeChannel *fx_volume_channel;

  fx_volume_channel = (AgsFxVolumeChannel *) g_object_new(AGS_TYPE_FX_VOLUME_CHANNEL,
							  "source", channel,
							  NULL);

  return(fx_volume_channel);
}
