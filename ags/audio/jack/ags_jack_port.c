/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/audio/jack/ags_jack_port.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

#include <ags/audio/ags_sound_provider.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/config.h>

void ags_jack_port_class_init(AgsJackPortClass *jack_port);
void ags_jack_port_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_jack_port_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager);
void ags_jack_port_init(AgsJackPort *jack_port);
void ags_jack_port_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_jack_port_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_jack_port_connect(AgsConnectable *connectable);
void ags_jack_port_disconnect(AgsConnectable *connectable);
void ags_jack_port_finalize(GObject *gobject);

/**
 * SECTION:ags_jack_port
 * @short_description: JACK resource.
 * @title: AgsJackPort
 * @section_id:
 * @include: ags/audio/jack/ags_jack_port.h
 *
 * The #AgsJackPort represents either a JACK sequencer or soundcard to communicate
 * with.
 */

enum{
  PROP_0,
  PROP_JACK_CLIENT,
  PROP_PORT_NAME,
};

static gpointer ags_jack_port_parent_class = NULL;

GType
ags_jack_port_get_type()
{
  static GType ags_type_jack_port = 0;

  if(!ags_type_jack_port){
    static const GTypeInfo ags_jack_port_info = {
      sizeof (AgsJackPortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsJackPort),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_jack_port_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_jack_port_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_jack_port = g_type_register_static(G_TYPE_OBJECT,
						"AgsJackPort\0",
						&ags_jack_port_info,
						0);

    g_type_add_interface_static(ags_type_jack_port,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_jack_port);
}

void
ags_jack_port_class_init(AgsJackPortClass *jack_port)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_jack_port_parent_class = g_type_class_peek_parent(jack_port);

  /* GObjectClass */
  gobject = (GObjectClass *) jack_port;

  gobject->set_property = ags_jack_port_set_property;
  gobject->get_property = ags_jack_port_get_property;

  gobject->finalize = ags_jack_port_finalize;

  /* properties */
  /**
   * AgsJackPort:jack-client:
   *
   * The assigned #AgsJackClient.
   * 
   * Since: 0.7.1
   */
  param_spec = g_param_spec_object("jack-client\0",
				   "assigned JACK client\0",
				   "The assigned JACK client.\0",
				   AGS_TYPE_JACK_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_JACK_CLIENT,
				  param_spec);

  /**
   * AgsJackPort:port-name:
   *
   * The jack soundcard indentifier
   * 
   * Since: 0.7.65
   */
  param_spec = g_param_spec_string("port-name\0",
				   "port name\0",
				   "The port name\0",
				   "hw:0\0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_NAME,
				  param_spec);
}

void
ags_jack_port_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_jack_port_connect;
  connectable->disconnect = ags_jack_port_disconnect;
}

void
ags_jack_port_init(AgsJackPort *jack_port)
{
  jack_port->flags = 0;

  jack_port->jack_client = NULL;
  
  jack_port->uuid = NULL;
  jack_port->name = NULL;

  jack_port->port = NULL;
}

void
ags_jack_port_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsJackPort *jack_port;

  jack_port = AGS_JACK_PORT(gobject);

  switch(prop_id){
  case PROP_JACK_CLIENT:
    {
      AgsJackClient *jack_client;

      jack_client = (AgsJackClient *) g_value_get_object(value);

      if(jack_client == jack_port->jack_client){
	return;
      }

      if(jack_port->jack_client != NULL){
	g_object_unref(jack_port->jack_client);
      }

      if(jack_client != NULL){
	g_object_ref(jack_client);
      }
      
      jack_port->jack_client = jack_client;
    }
    break;
  case PROP_PORT_NAME:
    {
      gchar *port_name;

      port_name = g_value_get_string(value);

      if(port_name == jack_port->name){
	return;
      }

      if(jack_port->name != NULL){
	g_free(jack_port->name);
      }

      jack_port->name = port_name;
      jack_port_set_name(jack_port->port,
			 port_name);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_port_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsJackPort *jack_port;

  jack_port = AGS_JACK_PORT(gobject);
  
  switch(prop_id){
  case PROP_JACK_CLIENT:
    {
      g_value_set_object(value, jack_port->jack_client);
    }
    break;
  case PROP_PORT_NAME:
    {
      g_value_set_string(value, jack_port->name);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_port_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_jack_port_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_jack_port_finalize(GObject *gobject)
{
  AgsJackPort *jack_port;

  jack_port = AGS_JACK_PORT(gobject);

  G_OBJECT_CLASS(ags_jack_port_parent_class)->finalize(gobject);
}

/**
 * ags_jack_port_find:
 * @jack_port: a #GList
 * @port_name: the port name to find
 *
 * Finds next match of @port_name in @jack_port.
 *
 * Returns: a #GList or %NULL
 * 
 * Since: 0.7.3
 */
GList*
ags_jack_port_find(GList *jack_port,
		   gchar *port_name)
{
  while(jack_port != NULL){
    if(!g_ascii_strcasecmp(jack_port_name(AGS_JACK_PORT(jack_port->data)->port),
			   port_name)){
      return(jack_port);
    }
  }

  return(NULL);
}

/**
 * ags_jack_port_register:
 * @jack_port: the #AgsJackPort
 * @port_name: the name as string
 * @is_audio: if %TRUE interpreted as audio port
 * @is_midi: if %TRUE interpreted as midi port
 * @is_output: if %TRUE port is acting as output, otherwise as input
 *
 * Register a new JACK port and read uuid. Creates a new AgsSequencer or AgsSoundcard
 * object.
 *
 * Since: 0.7.1
 */
void
ags_jack_port_register(AgsJackPort *jack_port,
		       gchar *port_name,
		       gboolean is_audio, gboolean is_midi,
		       gboolean is_output)
{
  AgsJackServer *jack_server;

  GList *list;

  gchar *name, uuid;
  
  if(!AGS_IS_JACK_PORT(jack_port) ||
     port_name == NULL){
    return;
  }

  if(jack_port->jack_client == NULL){
    g_warning("ags_jack_port.c - no assigned AgsJackClient\0");
    
    return;
  }

  /* get jack server and application context */
  if(jack_port->jack_client != NULL &&
     AGS_JACK_CLIENT(jack_port->jack_client)->jack_server != NULL){
    jack_server = AGS_JACK_CLIENT(jack_port->jack_client)->jack_server;
  }else{
    jack_server = NULL;
  }

  /* create sequencer or soundcard */
  if(is_output){
    jack_port->flags |= AGS_JACK_PORT_IS_OUTPUT;
  }

  if(is_audio){
    jack_port->flags |= AGS_JACK_PORT_IS_AUDIO;
    
    jack_port->port = jack_port_register(AGS_JACK_CLIENT(jack_port->jack_client)->client,
					 port_name,
					 JACK_DEFAULT_AUDIO_TYPE,
					 (is_output ? JackPortIsOutput: JackPortIsInput),
					 AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE);
  }else if(is_midi){
    jack_port->flags |= AGS_JACK_PORT_IS_MIDI;
    
    jack_port->port = jack_port_register(AGS_JACK_CLIENT(jack_port->jack_client)->client,
					 port_name,
					 JACK_DEFAULT_MIDI_TYPE,
					 (is_output ? JackPortIsOutput: JackPortIsInput),
					 AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE);
  }

  uuid = jack_port->uuid;
  name = jack_port->name;
  
#ifdef HAVE_JACK_PORT_UUID
  if(jack_port->port != NULL){
    jack_port->uuid = jack_port_uuid(jack_port->port);
  }
#endif

  jack_port->name = g_strdup(port_name);

  g_free(uuid);
  g_free(name);
}

void
ags_jack_port_unregister(AgsJackPort *jack_port)
{
  if(!AGS_IS_JACK_PORT(jack_port)){
    return;
  }

  if(jack_port->port != NULL){
    jack_port_unregister(AGS_JACK_CLIENT(jack_port->jack_client)->client,
			 jack_port->port);
  }
}

/**
 * ags_jack_port_new:
 * @jack_client: the #AgsJackClient assigned to
 *
 * Instantiate a new #AgsJackPort.
 *
 * Returns: the new #AgsJackPort
 *
 * Since: 0.7.1
 */
AgsJackPort*
ags_jack_port_new(GObject *jack_client)
{
  AgsJackPort *jack_port;

  jack_port = (AgsJackPort *) g_object_new(AGS_TYPE_JACK_PORT,
					   "jack-client\0", jack_client,
					   NULL);

  return(jack_port);
}
