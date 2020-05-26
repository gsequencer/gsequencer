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

#include <ags/audio/recall/ags_capture_wave_channel.h>

#include <ags/i18n.h>

void ags_capture_wave_channel_class_init(AgsCaptureWaveChannelClass *capture_wave_channel);
void ags_capture_wave_channel_init(AgsCaptureWaveChannel *capture_wave_channel);
void ags_capture_wave_channel_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec);
void ags_capture_wave_channel_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec);
void ags_capture_wave_channel_dispose(GObject *gobject);
void ags_capture_wave_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_capture_wave_channel
 * @short_description: capture channel wave
 * @title: AgsCaptureWaveChannel
 * @section_id:
 * @include: ags/audio/recall/ags_capture_wave_channel.h
 *
 * The #AgsCaptureWaveChannel class provides ports to the effect processor.
 */

static gpointer ags_capture_wave_channel_parent_class = NULL;
static AgsPluginInterface *ags_capture_wave_parent_plugin_interface;

const gchar *ags_capture_wave_channel_plugin_name = "ags-capture-wave";
const gchar *ags_capture_wave_channel_specifier[] = {
  "./x-offset[0]", 
};
const gchar *ags_capture_wave_channel_control_port[] = {
  "1/1",
};

enum{
  PROP_0,
  PROP_X_OFFSET,
};

GType
ags_capture_wave_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_capture_wave_channel = 0;

    static const GTypeInfo ags_capture_wave_channel_info = {
      sizeof (AgsCaptureWaveChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_capture_wave_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsCaptureWaveChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_capture_wave_channel_init,
    };

    ags_type_capture_wave_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							   "AgsCaptureWaveChannel",
							   &ags_capture_wave_channel_info,
							   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_capture_wave_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_capture_wave_channel_class_init(AgsCaptureWaveChannelClass *capture_wave_channel)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_capture_wave_channel_parent_class = g_type_class_peek_parent(capture_wave_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) capture_wave_channel;

  gobject->set_property = ags_capture_wave_channel_set_property;
  gobject->get_property = ags_capture_wave_channel_get_property;

  gobject->dispose = ags_capture_wave_channel_dispose;
  gobject->finalize = ags_capture_wave_channel_finalize;

  /* properties */
  /**
   * AgsCaptureWaveChannel:x-offset:
   *
   * The x-offset port.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("x-offset",
				   i18n_pspec("x offset"),
				   i18n_pspec("The x offset control"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_X_OFFSET,
				  param_spec);
}

void
ags_capture_wave_channel_init(AgsCaptureWaveChannel *capture_wave_channel)
{
  GList *port;

  AGS_RECALL(capture_wave_channel)->name = "ags-capture-wave";
  AGS_RECALL(capture_wave_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(capture_wave_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(capture_wave_channel)->xml_type = "ags-capture-wave-channel";

  /* port */
  port = NULL;

  /* x offset */
  capture_wave_channel->x_offset = g_object_new(AGS_TYPE_PORT,
						"plugin-name", ags_capture_wave_channel_plugin_name,
						"specifier", ags_capture_wave_channel_specifier[0],
						"control-port", ags_capture_wave_channel_control_port[9],
						"port-value-is-pointer", FALSE,
						"port-value-type", G_TYPE_UINT64,
						NULL);
  g_object_ref(capture_wave_channel->x_offset);
  
  capture_wave_channel->x_offset->port_value.ags_port_boolean = FALSE;

  /* add to port */
  port = g_list_prepend(port, capture_wave_channel->x_offset);
  g_object_ref(capture_wave_channel->x_offset);
  
  /* set port */
  AGS_RECALL(capture_wave_channel)->port = port;
}

void
ags_capture_wave_channel_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec)
{
  AgsCaptureWaveChannel *capture_wave_channel;

  GRecMutex *recall_mutex;

  capture_wave_channel = AGS_CAPTURE_WAVE_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(capture_wave_channel);

  switch(prop_id){
  case PROP_X_OFFSET:
    {
      AgsPort *x_offset;

      x_offset = (AgsPort *) g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(capture_wave_channel->x_offset == x_offset){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      if(capture_wave_channel->x_offset != NULL){
	g_object_unref(G_OBJECT(capture_wave_channel->x_offset));
      }
      
      if(x_offset != NULL){
	g_object_ref(G_OBJECT(x_offset));
      }
      
      capture_wave_channel->x_offset = x_offset;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_capture_wave_channel_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec)
{
  AgsCaptureWaveChannel *capture_wave_channel;

  GRecMutex *recall_mutex;
  
  capture_wave_channel = AGS_CAPTURE_WAVE_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(capture_wave_channel);

  switch(prop_id){
  case PROP_X_OFFSET:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, capture_wave_channel->x_offset);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_capture_wave_channel_dispose(GObject *gobject)
{
  AgsCaptureWaveChannel *capture_wave_channel;

  capture_wave_channel = AGS_CAPTURE_WAVE_CHANNEL(gobject);

  /* x-offset */
  if(capture_wave_channel->x_offset != NULL){
    g_object_unref(capture_wave_channel->x_offset);

    capture_wave_channel->x_offset = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_capture_wave_channel_parent_class)->dispose(gobject);
}

void
ags_capture_wave_channel_finalize(GObject *gobject)
{
  AgsCaptureWaveChannel *capture_wave_channel;

  capture_wave_channel = AGS_CAPTURE_WAVE_CHANNEL(gobject);
  
  /* x-offset */
  if(capture_wave_channel->x_offset != NULL){
    g_object_unref(capture_wave_channel->x_offset);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_capture_wave_channel_parent_class)->finalize(gobject);
}

/**
 * ags_capture_wave_channel_new:
 * @source: the #AgsChannel
 *
 * Create a new instance of #AgsCaptureWaveChannel
 *
 * Returns: the new #AgsCaptureWaveChannel
 *
 * Since: 3.0.0
 */
AgsCaptureWaveChannel*
ags_capture_wave_channel_new(AgsChannel *source)
{
  AgsCaptureWaveChannel *capture_wave_channel;

  capture_wave_channel = (AgsCaptureWaveChannel *) g_object_new(AGS_TYPE_CAPTURE_WAVE_CHANNEL,
								"source", source,
								NULL);

  return(capture_wave_channel);
}
