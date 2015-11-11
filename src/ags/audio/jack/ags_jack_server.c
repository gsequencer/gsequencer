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

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_port.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_midiin.h>

void ags_jack_server_class_init(AgsJackServerClass *jack_server);
void ags_jack_server_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_jack_server_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager);
void ags_jack_server_init(AgsJackServer *jack_server);
void ags_jack_server_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_jack_server_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_jack_server_connect(AgsConnectable *connectable);
void ags_jack_server_disconnect(AgsConnectable *connectable);
void ags_jack_server_finalize(GObject *gobject);

void ags_jack_server_set_url(AgsDistributedManager *distributed_manager,
			     gchar *url);
gchar* ags_jack_server_get_url(AgsDistributedManager *distributed_manager);
void ags_jack_server_set_ports(AgsDistributedManager *distributed_manager,
			       gchar **ports);
gchar** ags_jack_server_get_ports(AgsDistributedManager *distributed_manager);
void ags_jack_server_set_soundcard(AgsDistributedManager *distributed_manager,
				   gchar *uri,
				   GObject *soundcard);
GObject* ags_jack_server_get_soundcard(AgsDistributedManager *distributed_manager,
				       gchar *uri);
void ags_jack_server_set_sequencer(AgsDistributedManager *distributed_manager,
				   gchar *uri,
				   GObject *sequencer);
GObject* ags_jack_server_get_sequencer(AgsDistributedManager *distributed_manager,
				       gchar *uri);
GObject* ags_jack_server_register_soundcard(AgsDistributedManager *distributed_manager,
					    gboolean is_output);
void ags_jack_server_unregister_soundcard(AgsDistributedManager *distributed_manager,
					  GObject *soundcard);
GObject* ags_jack_server_register_sequencer(AgsDistributedManager *distributed_manager,
					    gboolean is_output);
void ags_jack_server_unregister_sequencer(AgsDistributedManager *distributed_manager,
					  GObject *sequencer);

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_URL,
};

static gpointer ags_jack_server_parent_class = NULL;

GType
ags_jack_server_get_type()
{
  static GType ags_type_jack_server = 0;

  if(!ags_type_jack_server){
    static const GTypeInfo ags_jack_server_info = {
      sizeof (AgsJackServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsJackServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_jack_server_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_jack_server_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_distributed_manager_interface_info = {
      (GInterfaceInitFunc) ags_jack_server_distributed_manager_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_jack_server = g_type_register_static(G_TYPE_OBJECT,
						  "AgsJackServer\0",
						  &ags_jack_server_info,
						  0);

    g_type_add_interface_static(ags_type_jack_server,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_jack_server,
				AGS_TYPE_DISTRIBUTED_MANAGER,
				&ags_distributed_manager_interface_info);
  }

  return (ags_type_jack_server);
}

void
ags_jack_server_class_init(AgsJackServerClass *jack_server)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_jack_server_parent_class = g_type_class_peek_parent(jack_server);

  /* GObjectClass */
  gobject = (GObjectClass *) jack_server;

  gobject->set_property = ags_jack_server_set_property;
  gobject->get_property = ags_jack_server_get_property;

  gobject->finalize = ags_jack_server_finalize;

  /* properties */
  /**
   * AgsJackServer:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 0.7.1
   */
  param_spec = g_param_spec_object("application-context\0",
				   "the application context object\0",
				   "The application context object\0",
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /**
   * AgsJackServer:url:
   *
   * The assigned URL.
   * 
   * Since: 0.7.1
   */
  param_spec = g_param_spec_string("url\0",
				   "the URL\0",
				   "The URL\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_URL,
				  param_spec);
}

void
ags_jack_server_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_jack_server_connect;
  connectable->disconnect = ags_jack_server_disconnect;
}

void
ags_jack_server_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager)
{
  distributed_manager->set_url = ags_jack_server_set_url;
  distributed_manager->get_url = ags_jack_server_get_url;
  distributed_manager->set_ports = ags_jack_server_set_ports;
  distributed_manager->get_ports = ags_jack_server_get_ports;
  distributed_manager->set_soundcard = ags_jack_server_set_soundcard;
  distributed_manager->get_soundcard = ags_jack_server_get_soundcard;
  distributed_manager->set_sequencer = ags_jack_server_set_sequencer;
  distributed_manager->get_sequencer = ags_jack_server_get_sequencer;
  distributed_manager->register_soundcard = ags_jack_server_register_soundcard;
  distributed_manager->unregister_soundcard = ags_jack_server_unregister_soundcard;
  distributed_manager->register_sequencer = ags_jack_server_register_sequencer;
  distributed_manager->unregister_sequencer = ags_jack_server_unregister_sequencer;
}

void
ags_jack_server_init(AgsJackServer *jack_server)
{
  jack_server->flags = 0;

  jack_server->application_context = NULL;
  jack_server->url = NULL;

  jack_server->n_soundcards = 0;
  jack_server->n_sequencers = 0;
  
  jack_server->client = NULL;
}

void
ags_jack_server_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackServer *jack_server;

  jack_server = AGS_JACK_SERVER(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = g_value_get_object(value);

      if(jack_server->application_context == application_context){
	return;
      }

      if(jack_server->application_context != NULL){
	g_object_unref(G_OBJECT(jack_server->application_context));
      }

      if(application_context != NULL){
	g_object_ref(G_OBJECT(application_context));
      }

      jack_server->application_context = application_context;
    }
    break;
  case PROP_URL:
    {
      gchar *url;

      url = g_value_get_string(value);

      if(jack_server->url == url){
	return;
      }

      if(jack_server->url != NULL){
	g_free(jack_server->url);
      }

      jack_server->url = g_strdup(url);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_server_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackServer *jack_server;

  jack_server = AGS_JACK_SERVER(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, jack_server->application_context);
    }
    break;
  case PROP_URL:
    {
      g_value_set_string(value, jack_server->url);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_server_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_jack_server_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_jack_server_finalize(GObject *gobject)
{
  AgsJackServer *jack_server;

  jack_server = AGS_JACK_SERVER(gobject);

  G_OBJECT_CLASS(ags_jack_server_parent_class)->finalize(gobject);
}

void
ags_jack_server_set_url(AgsDistributedManager *distributed_manager,
			gchar *url)
{
  AGS_JACK_SERVER(distributed_manager)->url = url;
}

gchar*
ags_jack_server_get_url(AgsDistributedManager *distributed_manager)
{
  return(AGS_JACK_SERVER(distributed_manager)->url);
}

void
ags_jack_server_set_ports(AgsDistributedManager *distributed_manager,
			  gchar **ports)
{
  //TODO:JK: implement me
}

gchar**
ags_jack_server_get_ports(AgsDistributedManager *distributed_manager)
{
  //TODO:JK: implement me
}

void
ags_jack_server_set_soundcard(AgsDistributedManager *distributed_manager,
			      gchar *uri,
			      GObject *soundcard)
{
  //TODO:JK: implement me
}

GObject*
ags_jack_server_get_soundcard(AgsDistributedManager *distributed_manager,
			      gchar *uri)
{
  //TODO:JK: implement me
}

void
ags_jack_server_set_sequencer(AgsDistributedManager *distributed_manager,
			      gchar *uri,
			      GObject *sequencer)
{
  //TODO:JK: implement me
}

GObject*
ags_jack_server_get_sequencer(AgsDistributedManager *distributed_manager,
			      gchar *uri)
{
  //TODO:JK: implement me
}

GObject*
ags_jack_server_register_soundcard(AgsDistributedManager *distributed_manager,
				   gboolean is_output)
{
  AgsJackClient *default_client;
  AgsJackPort *soundcard;
  gchar *str;
  
  if(!is_output){
    g_warning("GSequencer - audio input not implemented");
    return(NULL);
  }

  /* the default client */
  default_client = AGS_JACK_SERVER(distributed_manager)->default_client;

  if(default_client == NULL){
    AGS_JACK_SERVER(distributed_manager)->default_client = 
      default_client = ags_jack_client_new();

    default_client->client = jack_client_open(g_strdup("ags-default-client\0"),
					      0,
					      NULL);

    if(default_client->client == NULL){
      g_warning("ags_jack_server.c - can't open JACK client");
      
      return;
    }

    AGS_JACK_SERVER(distributed_manager)->client = g_list_prepend(AGS_JACK_SERVER(distributed_manager)->client,
								  default_client);
  }

  /* register soundcard */
  str = g_strdup_printf("ags-soundcard-%04d\0",
			AGS_JACK_SERVER(distributed_manager)->n_soundcards);
  g_message("%s\0", str);
  
  soundcard = ags_jack_port_new();
  soundcard->port = jack_port_register(default_client->client,
				       str,
				       JACK_DEFAULT_AUDIO_TYPE,
				       JackPortIsOutput,
				       AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE);
  soundcard->device = ags_jack_devout_new(AGS_JACK_SERVER(distributed_manager)->application_context);
  AGS_JACK_DEVOUT(soundcard->device)->out_port = soundcard->port;

  AGS_JACK_SERVER(distributed_manager)->n_soundcards += 1;
  
  return(soundcard->device);
}

void
ags_jack_server_unregister_soundcard(AgsDistributedManager *distributed_manager,
				     GObject *soundcard)
{
  AgsJackClient *default_client;

  /* the default client */
  default_client = AGS_JACK_SERVER(distributed_manager)->default_client;

  if(default_client == NULL){
    g_warning("GSequencer - no jack client\0");
    
    return;
  }
  
  jack_port_unregister(default_client->client,
		       AGS_JACK_DEVOUT(soundcard)->out_port);
}

GObject*
ags_jack_server_register_sequencer(AgsDistributedManager *distributed_manager,
				   gboolean is_output)
{
  AgsJackClient *default_client;
  AgsJackPort *sequencer;
  gchar *str;
  
  if(is_output){
    g_warning("GSequencer - MIDI output not implemented\0");
    return(NULL);
  }
  
  /* the default client */
  default_client = AGS_JACK_SERVER(distributed_manager)->default_client;

  if(default_client == NULL){
    AGS_JACK_SERVER(distributed_manager)->default_client = 
      default_client = ags_jack_client_new();

    default_client->client = jack_client_open(g_strdup("ags-default-client\0"),
					      0,
					      NULL);

    if(default_client->client == NULL){
      g_warning("ags_jack_server.c - can't open JACK client");
      
      return;
    }
    
    AGS_JACK_SERVER(distributed_manager)->client = g_list_prepend(AGS_JACK_SERVER(distributed_manager)->client,
								  default_client);
  }
  
  /* register sequencer */
  str = g_strdup_printf("ags-sequencer-%04d\0",
			AGS_JACK_SERVER(distributed_manager)->n_soundcards);
  g_message("%s\0", str);

  sequencer = ags_jack_port_new();
  sequencer->port = jack_port_register(default_client->client,
				       str,
				       JACK_DEFAULT_MIDI_TYPE,
				       JackPortIsOutput,
				       AGS_SEQUENCER_DEFAULT_BUFFER_SIZE);
  sequencer->device = ags_jack_midiin_new(AGS_JACK_SERVER(distributed_manager)->application_context);
  AGS_JACK_MIDIIN(sequencer->device)->in_port = sequencer->port;

  AGS_JACK_SERVER(distributed_manager)->n_sequencers += 1;
  
  return(sequencer->device);
}

void
ags_jack_server_unregister_sequencer(AgsDistributedManager *distributed_manager,
				     GObject *sequencer)
{
  AgsJackClient *default_client;

  /* the default client */
  default_client = AGS_JACK_SERVER(distributed_manager)->default_client;

  if(default_client == NULL){
    g_warning("GSequencer - no jack client\0");
    
    return;
  }
  
  jack_port_unregister(default_client->client,
		       AGS_JACK_MIDIIN(sequencer)->in_port);
}

GObject*
ags_jack_server_register_default_soundcard(AgsJackServer *jack_server)
{
  AgsJackClient *default_client;
  AgsJackPort *default_soundcard;

  default_client = jack_server->default_client;

  if(default_client == NULL){
    jack_server->default_client = 
      default_client = ags_jack_client_new();

    default_client->client = jack_client_open("ags-default-client\0",
					      0,
					      NULL);
    
    jack_server->client = g_list_prepend(jack_server->client,
					 default_client);
  }

  jack_server->default_soundcard = 
    default_soundcard = ags_jack_port_new();
  default_soundcard->port = jack_port_register(default_client,
					       "ags-default-soundcard\0",
					       JACK_DEFAULT_AUDIO_TYPE,
					       JackPortIsOutput,
					       AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE);
  default_soundcard->device = ags_jack_devout_new(NULL);
  AGS_JACK_DEVOUT(default_soundcard->device)->out_port = default_soundcard->port;

  return(default_soundcard->device);
}

/**
 * ags_jack_server_new:
 * @application_context: the #AgsApplicationContext
 * @url: the URL as string
 *
 * Instantiate a new #AgsJackServer.
 *
 * Returns: the new #AgsJackServer
 *
 * Since: 0.7.1
 */
AgsJackServer*
ags_jack_server_new(GObject *application_context,
		    gchar *url)
{
  AgsJackServer *jack_server;

  jack_server = (AgsJackServer *) g_object_new(AGS_TYPE_JACK_SERVER,
					       "application-context\0", application_context,
					       "url\0", url,
					       NULL);

  return(jack_server);
}
