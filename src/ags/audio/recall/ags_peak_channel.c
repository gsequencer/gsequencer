/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/audio/recall/ags_peak_channel.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/main.h>

#include <ags/object/ags_mutable.h>
#include <ags/object/ags_plugin.h>

void ags_peak_channel_class_init(AgsPeakChannelClass *peak_channel);
void ags_peak_channel_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_peak_channel_plugin_interface_init(AgsPluginInterface *plugin);
void ags_peak_channel_init(AgsPeakChannel *peak_channel);
void ags_peak_channel_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_peak_channel_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_peak_channel_connect(AgsConnectable *connectable);
void ags_peak_channel_disconnect(AgsConnectable *connectable);
void ags_peak_channel_set_ports(AgsPlugin *plugin, GList *port);
void ags_peak_channel_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_PEAK,
};

static gpointer ags_peak_channel_parent_class = NULL;
static AgsConnectableInterface *ags_peak_channel_parent_connectable_interface;
static AgsPluginInterface *ags_peak_channel_parent_plugin_interface;

static const gchar *ags_peak_channel_plugin_name = "ags-peak\0";
static const gchar *ags_peak_channel_plugin_specifier[] = {
  "./peak[0]\0",
};
static const gchar *ags_peak_channel_plugin_control_port[] = {
  "1/1\0",
};

GType
ags_peak_channel_get_type()
{
  static GType ags_type_peak_channel = 0;

  if(!ags_type_peak_channel){
    static const GTypeInfo ags_peak_channel_info = {
      sizeof (AgsPeakChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_peak_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPeakChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_peak_channel_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_peak_channel_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_plugin_interface_info = {
      (GInterfaceInitFunc) ags_peak_channel_plugin_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_peak_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
						   "AgsPeakChannel\0",
						   &ags_peak_channel_info,
						   0);

    g_type_add_interface_static(ags_type_peak_channel,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_peak_channel,
				AGS_TYPE_PLUGIN,
				&ags_plugin_interface_info);
  }

  return(ags_type_peak_channel);
}

void
ags_peak_channel_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_peak_channel_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_peak_channel_connect;
  connectable->disconnect = ags_peak_channel_disconnect;
}

void
ags_peak_channel_plugin_interface_init(AgsPluginInterface *plugin)
{
  ags_peak_channel_parent_plugin_interface = g_type_interface_peek_parent(plugin);

  plugin->set_ports = ags_peak_channel_set_ports;
}

void
ags_peak_channel_class_init(AgsPeakChannelClass *peak_channel)
{
  GObjectClass *gobject;
  AgsRecallClass *recall;
  GParamSpec *param_spec;

  ags_peak_channel_parent_class = g_type_class_peek_parent(peak_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) peak_channel;

  gobject->set_property = ags_peak_channel_set_property;
  gobject->get_property = ags_peak_channel_get_property;

  gobject->finalize = ags_peak_channel_finalize;

  /* AgsRecallClass */
  recall = (AgsRecallClass *) peak_channel;

  /* properties */
  param_spec = g_param_spec_object("peak\0",
				   "peak of channel\0",
				   "The peak of channel\0",
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PEAK,
				  param_spec);
}

void
ags_peak_channel_init(AgsPeakChannel *peak_channel)
{
  GList *port;

  AGS_RECALL(peak_channel)->name = "ags-peak\0";
  AGS_RECALL(peak_channel)->version = AGS_EFFECTS_DEFAULT_VERSION;
  AGS_RECALL(peak_channel)->build_id = AGS_BUILD_ID;
  AGS_RECALL(peak_channel)->xml_type = "ags-peak-channel\0";

  port = NULL;

  peak_channel->peak = g_object_new(AGS_TYPE_PORT,
				     "plugin-name\0", ags_peak_channel_plugin_name,
				     "specifier\0", ags_peak_channel_plugin_specifier[0],
				     "control-port\0", ags_peak_channel_plugin_control_port[0],
				     "port-value-is-pointer\0", FALSE,
				     "port-value-type\0", G_TYPE_DOUBLE,
				     "port-value-size\0", sizeof(gdouble),
				     "port-value-length\0", 1,
				     NULL);
  peak_channel->peak->port_value.ags_port_double = FALSE;

  port = g_list_prepend(port, peak_channel->peak);

  /* set port */
  AGS_RECALL(peak_channel)->port = port;
}

void
ags_peak_channel_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsPeakChannel *peak_channel;

  peak_channel = AGS_PEAK_CHANNEL(gobject);

  switch(prop_id){
  case PROP_PEAK:
    {
      AgsPort *port;

      port = (AgsPort *) g_value_get_object(value);

      if(port == peak_channel->peak){
	return;
      }

      if(peak_channel->peak != NULL){
	g_object_unref(G_OBJECT(peak_channel->peak));
      }
      
      if(port != NULL){
	g_object_ref(G_OBJECT(port));
      }

      peak_channel->peak = port;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }  
}

void
ags_peak_channel_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsPeakChannel *peak_channel;

  peak_channel = AGS_PEAK_CHANNEL(gobject);

  switch(prop_id){
  case PROP_PEAK:
    {
      g_value_set_object(value, peak_channel->peak);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_peak_channel_finalize(GObject *gobject)
{
  AgsPeakChannel *peak_channel;

  peak_channel = AGS_PEAK_CHANNEL(gobject);

  if(peak_channel->peak != NULL){
    g_object_unref(G_OBJECT(peak_channel->peak));
  }

  /* call parent */
  G_OBJECT_CLASS(ags_peak_channel_parent_class)->finalize(gobject);
}

void
ags_peak_channel_connect(AgsConnectable *connectable)
{
  ags_peak_channel_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_peak_channel_disconnect(AgsConnectable *connectable)
{
  ags_peak_channel_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_peak_channel_set_ports(AgsPlugin *plugin, GList *port)
{
  while(port != NULL){
    if(!strncmp(AGS_PORT(port->data)->specifier,
		"./peak[0]\0",
		9)){
      g_object_set(G_OBJECT(plugin),
		   "peak\0", AGS_PORT(port->data),
		   NULL);
    }

    port = port->next;
  }
}

void
ags_peak_channel_retrieve_peak(AgsPeakChannel *peak_channel,
			       gboolean is_play)
{
  AgsRecall *recall;
  AgsChannel *source;
  AgsRecycling *recycling;
  GList *audio_signal;
  double *buffer;
  double current_value, prev_value;
  static const double scale_precision = 10.0;
  guint i;
  GValue value = {0,};

  if(peak_channel == NULL){
    return;
  }

  recall = (AgsRecall *) peak_channel;

  source = AGS_RECALL_CHANNEL(peak_channel)->source;
  recycling = source->first_recycling;

  buffer = (double *) malloc(AGS_DEVOUT_DEFAULT_BUFFER_SIZE * sizeof(double));
  for(i = 0; i < AGS_DEVOUT_DEFAULT_BUFFER_SIZE; i++) buffer[i] = 0.0;

  while(recycling != source->last_recycling->next){
    audio_signal = recycling->audio_signal;

    while(audio_signal != NULL){
      if(AGS_AUDIO_SIGNAL(audio_signal)->stream_current != NULL){
	ags_audio_signal_copy_buffer_to_double_buffer(buffer, 1,
						      (signed short *) AGS_AUDIO_SIGNAL(audio_signal)->stream_current->data, 1,
						      AGS_DEVOUT_DEFAULT_BUFFER_SIZE);
      }

      audio_signal = audio_signal->next;
    }

    recycling = recycling->next;
  }

  current_value = 0.0;

  for(i = 0; i < AGS_DEVOUT_DEFAULT_BUFFER_SIZE; i++){
    current_value +=  (1.0 / (1.0 / G_MAXUINT16 * buffer[i]));
  }

  //TODO:JK: verify me
  current_value = scale_precision * (atan(1.0 / 440.0) / sin(current_value / 440.0));

  g_value_init(&value, G_TYPE_DOUBLE);

  /*  */
  if(is_play){
    ags_port_safe_read(peak_channel->peak,
		       &value);

    prev_value = g_value_get_double(&value);
  }else{
    prev_value = 0.0;
  }

  g_value_set_double(&value,
		     current_value + prev_value);

  ags_port_safe_write(peak_channel->peak,
		      &value);
}

AgsPeakChannel*
ags_peak_channel_new(AgsChannel *source)
{
  AgsPeakChannel *peak_channel;

  peak_channel = (AgsPeakChannel *) g_object_new(AGS_TYPE_PEAK_CHANNEL,
						 "channel\0", source,
						 NULL);

  return(peak_channel);
}
