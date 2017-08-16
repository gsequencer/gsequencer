/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/audio/pulse/ags_pulse_port.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/audio/pulse/ags_pulse_server.h>
#include <ags/audio/pulse/ags_pulse_client.h>
#include <ags/audio/pulse/ags_pulse_devout.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_pulse_port_class_init(AgsPulsePortClass *pulse_port);
void ags_pulse_port_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pulse_port_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager);
void ags_pulse_port_init(AgsPulsePort *pulse_port);
void ags_pulse_port_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_pulse_port_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_pulse_port_connect(AgsConnectable *connectable);
void ags_pulse_port_disconnect(AgsConnectable *connectable);
void ags_pulse_port_dispose(GObject *gobject);
void ags_pulse_port_finalize(GObject *gobject);

void ags_pulse_port_stream_request_callback(pa_stream *stream, size_t length, AgsPulsePort *pulse_port);
void ags_pulse_port_stream_underflow_callback(pa_stream *stream, AgsPulsePort *pulse_port);

/**
 * SECTION:ags_pulse_port
 * @short_description: pulseaudio resource.
 * @title: AgsPulsePort
 * @section_id:
 * @include: ags/audio/pulse/ags_pulse_port.h
 *
 * The #AgsPulsePort represents either a pulseaudio sequencer or soundcard to communicate
 * with.
 */

enum{
  PROP_0,
  PROP_PULSE_CLIENT,
  PROP_PORT_NAME,
};

static gpointer ags_pulse_port_parent_class = NULL;

GType
ags_pulse_port_get_type()
{
  static GType ags_type_pulse_port = 0;

  if(!ags_type_pulse_port){
    static const GTypeInfo ags_pulse_port_info = {
      sizeof (AgsPulsePortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pulse_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPulsePort),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pulse_port_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pulse_port_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_pulse_port = g_type_register_static(G_TYPE_OBJECT,
						 "AgsPulsePort",
						 &ags_pulse_port_info,
						 0);

    g_type_add_interface_static(ags_type_pulse_port,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_pulse_port);
}

void
ags_pulse_port_class_init(AgsPulsePortClass *pulse_port)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_pulse_port_parent_class = g_type_class_peek_parent(pulse_port);

  /* GObjectClass */
  gobject = (GObjectClass *) pulse_port;

  gobject->set_property = ags_pulse_port_set_property;
  gobject->get_property = ags_pulse_port_get_property;

  gobject->dispose = ags_pulse_port_dispose;
  gobject->finalize = ags_pulse_port_finalize;

  /* properties */
  /**
   * AgsPulsePort:pulse-client:
   *
   * The assigned #AgsPulseClient.
   * 
   * Since: 0.9.10
   */
  param_spec = g_param_spec_object("pulse-client",
				   i18n_pspec("assigned pulseaudio client"),
				   i18n_pspec("The assigned pulseaudio client"),
				   AGS_TYPE_PULSE_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PULSE_CLIENT,
				  param_spec);

  /**
   * AgsPulsePort:port-name:
   *
   * The pulse soundcard indentifier
   * 
   * Since: 0.9.10
   */
  param_spec = g_param_spec_string("port-name",
				   i18n_pspec("port name"),
				   i18n_pspec("The port name"),
				   "hw:0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_NAME,
				  param_spec);
}

void
ags_pulse_port_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_pulse_port_connect;
  connectable->disconnect = ags_pulse_port_disconnect;
}

void
ags_pulse_port_init(AgsPulsePort *pulse_port)
{
  pulse_port->flags = 0;

  pulse_port->pulse_client = NULL;
  
  pulse_port->uuid = NULL;
  pulse_port->name = NULL;

  pulse_port->stream = NULL;

  pulse_port->sample_spec = (pa_sample_spec *) malloc(sizeof(pa_sample_spec));
  pulse_port->sample_spec->rate = 44100;
  pulse_port->sample_spec->channels = 2;
  pulse_port->sample_spec->format = PA_SAMPLE_S16LE;
  
  bufattr.fragsize = (uint32_t)-1;
  bufattr.maxlength = pa_usec_to_bytes(latency,&ss);
  bufattr.minreq = pa_usec_to_bytes(0,&ss);
  bufattr.prebuf = (uint32_t)-1;
  bufattr.tlength = pa_usec_to_bytes(latency,&ss);
}

void
ags_pulse_port_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsPulsePort *pulse_port;

  pulse_port = AGS_PULSE_PORT(gobject);

  switch(prop_id){
  case PROP_PULSE_CLIENT:
    {
      AgsPulseClient *pulse_client;

      pulse_client = (AgsPulseClient *) g_value_get_object(value);

      if(pulse_port->pulse_client == (GObject *) pulse_client){
	return;
      }

      if(pulse_port->pulse_client != NULL){
	g_object_unref(pulse_port->pulse_client);
      }

      if(pulse_client != NULL){
	g_object_ref(pulse_client);
      }
      
      pulse_port->pulse_client = (GObject *) pulse_client;
    }
    break;
  case PROP_PORT_NAME:
    {
      gchar *port_name;

      port_name = g_value_get_string(value);

      if(pulse_port->name == port_name ||
	 !g_ascii_strcasecmp(pulse_port->name,
			     port_name)){
	return;
      }

      if(pulse_port->name != NULL){
	g_free(pulse_port->name);
      }

      pulse_port->name = port_name;

      if(pulse_port->port != NULL){
	pulse_port_set_name(pulse_port->port,
			    port_name);
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_port_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsPulsePort *pulse_port;

  pulse_port = AGS_PULSE_PORT(gobject);
  
  switch(prop_id){
  case PROP_PULSE_CLIENT:
    {
      g_value_set_object(value, pulse_port->pulse_client);
    }
    break;
  case PROP_PORT_NAME:
    {
      g_value_set_string(value, pulse_port->name);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_port_connect(AgsConnectable *connectable)
{
  AgsPulsePort *pulse_port;

  pulse_port = AGS_PULSE_PORT(connectable);

  if((AGS_PULSE_PORT_CONNECTED & (pulse_port->flags)) != 0){
    return;
  }

  pulse_port->flags |= AGS_PULSE_PORT_CONNECTED;
}

void
ags_pulse_port_disconnect(AgsConnectable *connectable)
{
  AgsPulsePort *pulse_port;

  pulse_port = AGS_PULSE_PORT(connectable);

  if((AGS_PULSE_PORT_CONNECTED & (pulse_port->flags)) == 0){
    return;
  }

  pulse_port->flags &= (~AGS_PULSE_PORT_CONNECTED);
}

void
ags_pulse_port_dispose(GObject *gobject)
{
  AgsPulsePort *pulse_port;

  pulse_port = AGS_PULSE_PORT(gobject);

  /* pulse client */
  if(pulse_port->pulse_client != NULL){
    g_object_unref(pulse_port->pulse_client);

    pulse_port->pulse_client = NULL;
  }

  /* name */
  g_free(pulse_port->name);

  /* call parent */
  G_OBJECT_CLASS(ags_pulse_port_parent_class)->dispose(gobject);
}

void
ags_pulse_port_finalize(GObject *gobject)
{
  AgsPulsePort *pulse_port;

  pulse_port = AGS_PULSE_PORT(gobject);

  /* pulse client */
  if(pulse_port->pulse_client != NULL){
    g_object_unref(pulse_port->pulse_client);
  }

  /* name */
  g_free(pulse_port->name);

  /* call parent */
  G_OBJECT_CLASS(ags_pulse_port_parent_class)->finalize(gobject);
}

/**
 * ags_pulse_port_find:
 * @pulse_port: a #GList
 * @port_name: the port name to find
 *
 * Finds next match of @port_name in @pulse_port.
 *
 * Returns: a #GList or %NULL
 * 
 * Since: 0.9.10
 */
GList*
ags_pulse_port_find(GList *pulse_port,
		    gchar *port_name)
{
  while(pulse_port != NULL){
    if(!g_ascii_strcasecmp(pulse_port_name(AGS_PULSE_PORT(pulse_port->data)->port),
			   port_name)){
      return(pulse_port);
    }
  }

  return(NULL);
}

/**
 * ags_pulse_port_register:
 * @pulse_port: the #AgsPulsePort
 * @port_name: the name as string
 * @is_audio: if %TRUE interpreted as audio port
 * @is_midi: if %TRUE interpreted as midi port
 * @is_output: if %TRUE port is acting as output, otherwise as input
 *
 * Register a new pulseaudio port and read uuid. Creates a new AgsSequencer or AgsSoundcard
 * object.
 *
 * Since: 0.9.10
 */
void
ags_pulse_port_register(AgsPulsePort *pulse_port,
			gchar *port_name,
			gboolean is_audio, gboolean is_midi,
			gboolean is_output)
{
  GList *list;

  gchar *name, *uuid;

  int r;
  
  if(!AGS_IS_PULSE_PORT(pulse_port) ||
     port_name == NULL){
    return;
  }

  if(pulse_port->pulse_client == NULL){
    g_warning("ags_pulse_port.c - no assigned AgsPulseClient");
    
    return;
  }

  if((AGS_PULSE_PORT_REGISTERED & (pulse_port->flags)) != 0){
    return;
  }

  /* get pulse server and application context */
  if(pulse_port->pulse_client == NULL ||
     AGS_PULSE_CLIENT(pulse_port->pulse_client)->pulse_server == NULL){
    return;
  }

  uuid = pulse_port->uuid;
  name = pulse_port->name;

  if(AGS_PULSE_CLIENT(pulse_port->pulse_client)->client == NULL){
    return;
  }
  
  pulse_port->name = g_strdup(port_name);

  /* create sequencer or soundcard */
  if(is_output){
    pulse_port->flags |= AGS_PULSE_PORT_IS_OUTPUT;
  }

  pulse_port->stream = pa_stream_new(AGS_PULSE_CLIENT(pulse_port->pulse_client)->context, "GSequencer Playback", pulse_port->sample_spec, NULL);

  if(pulse_port->stream){
    return;
  }

  if(is_audio){
    pulse_port->flags |= AGS_PULSE_PORT_IS_AUDIO;

    pa_stream_set_write_callback(pulse_port->stream,
				 ags_pulse_port_stream_request_callback,
				 pulse_port);
    pa_stream_set_underflow_callback(pulse_port->stream,
				     ags_pulse_port_stream_underflow_callback,
				     pulse_port);
    
    r = pa_stream_connect_playback(pulse_port->stream, NULL, pulse_port->buffer_attr,
				   (PA_STREAM_INTERPOLATE_TIMING |
				    PA_STREAM_ADJUST_LATENCY |
				    PA_STREAM_AUTO_TIMING_UPDATE),
				   NULL,
				   NULL);

    if(r < 0){
      // Old pulse audio servers don't like the ADJUST_LATENCY flag, so retry without that
      r = pa_stream_connect_playback(pulse_port->stream, NULL, pulse_port->buffer_attr,
				     (PA_STREAM_INTERPOLATE_TIMING |
				      PA_STREAM_AUTO_TIMING_UPDATE),
				     NULL,
				     NULL);
    }

    if(r < 0){
      return;
    }
  }else if(is_midi){
    pulse_port->flags |= AGS_PULSE_PORT_IS_MIDI;

    //NOTE:JK: not implemented
  }
  
  if(pulse_port->stream != NULL){
    pulse_port->flags |= AGS_PULSE_PORT_REGISTERED;
  }
}

void
ags_pulse_port_unregister(AgsPulsePort *pulse_port)
{
  if(!AGS_IS_PULSE_PORT(pulse_port)){
    return;
  }

  if(pulse_port->stream != NULL){
    pa_stream_disconnect(pulse_port->stream);
  }
}

void
ags_pulse_port_stream_request_callback(pa_stream *stream, size_t length, AgsPulsePort *pulse_port)
{
  //TODO:JK: implement me
}

void
ags_pulse_port_stream_underflow_callback(pa_stream *stream, AgsPulsePort *pulse_port)
{
  //TODO:JK: implement me
}

/**
 * ags_pulse_port_new:
 * @pulse_client: the #AgsPulseClient assigned to
 *
 * Instantiate a new #AgsPulsePort.
 *
 * Returns: the new #AgsPulsePort
 *
 * Since: 0.9.10
 */
AgsPulsePort*
ags_pulse_port_new(GObject *pulse_client)
{
  AgsPulsePort *pulse_port;

  pulse_port = (AgsPulsePort *) g_object_new(AGS_TYPE_PULSE_PORT,
					     "pulse-client", pulse_client,
					     NULL);

  return(pulse_port);
}
