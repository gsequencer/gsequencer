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

#include <ags/audio/ags_jack_server.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

#include <ags/audio/ags_jack_devout.h>
#include <ags/audio/ags_jack_midiin.h>

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
    
    ags_type_jack_server = g_type_register_static(G_TYPE_OBJECT,
						  "AgsJackServer\0",
						  &ags_jack_server_info,
						  0);

    g_type_add_interface_static(ags_type_jack_server,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
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
   * AgsJackServer:url:
   *
   * The assigned URL.
   * 
   * Since: 0.7.0
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

  if(!is_output){
    g_warning("GSequencer - audio input not implemented");
    return(NULL);
  }

  /* the default client */
  default_client = ags_jack_server_find_default_client(AGS_JACK_SERVER(distributed_manager));

  if(default_client == NULL){
    default_client = ags_jack_client_alloc();

    default_client->default_client = TRUE;

    default_client->client = jack_client_open(g_strdup("ags-default-client\0"),
					      0,
					      NULL);
    
    AGS_JACK_SERVER(distributed_manager)->client = g_list_prepend(AGS_JACK_SERVER(distributed_manager)->client,
								  default_client);
  }

  /* register soundcard */
  soundcard = ags_jack_port_alloc();
  soundcard->port = jack_port_register(default_client,
				       g_strdup_printf("ags-soundcard-%04d\0",
						       AGS_JACK_SERVER(distributed_manager)->n_soundcards),
				       JACK_DEFAULT_AUDIO_TYPE,
				       JackPortIsOutput,
				       AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE);
  soundcard->gobject = ags_jack_devout_new(NULL);
  AGS_JACK_DEVOUT(soundcard->gobject)->out_port = soundcard->port;

  AGS_JACK_SERVER(distributed_manager)->n_soundcards += 1;
  
  return(soundcard->gobject);
}

void
ags_jack_server_unregister_soundcard(AgsDistributedManager *distributed_manager,
				     GObject *soundcard)
{
  //TODO:JK: implement me
}

GObject*
ags_jack_server_register_sequencer(AgsDistributedManager *distributed_manager,
				   gboolean is_output)
{
  AgsJackClient *default_client;
  AgsJackPort *sequencer;

  if(is_output){
    g_warning("GSequencer - MIDI output not implemented");
    return(NULL);
  }
  
  /* the default client */
  default_client = ags_jack_server_find_default_client(AGS_JACK_SERVER(distributed_manager));

  if(default_client == NULL){
    default_client = ags_jack_client_alloc();

    default_client->default_client = TRUE;

    default_client->client = jack_client_open(g_strdup("ags-default-client\0"),
					      0,
					      NULL);
    
    AGS_JACK_SERVER(distributed_manager)->client = g_list_prepend(AGS_JACK_SERVER(distributed_manager)->client,
								  default_client);
  }
  
  /* register sequencer */
  sequencer = ags_jack_port_alloc();
  sequencer->port = jack_port_register(default_client,
				       g_strdup_printf("ags-sequencer-%04d\0",
						       AGS_JACK_SERVER(distributed_manager)->n_sequencers),
				       JACK_DEFAULT_MIDI_TYPE,
				       JackPortIsOutput,
				       AGS_SEQUENCER_DEFAULT_BUFFER_SIZE);
  sequencer->gobject = ags_jack_midiin_new(NULL);
  AGS_JACK_MIDIIN(sequencer->gobject)->in_port = sequencer->port;

  AGS_JACK_SERVER(distributed_manager)->n_sequencers += 1;
  
  return(sequencer->gobject);
}

void
ags_jack_server_unregister_sequencer(AgsDistributedManager *distributed_manager,
				     GObject *sequencer)
{
  //TODO:JK: implement me
}

AgsJackClient*
ags_jack_client_alloc()
{
  AgsJackClient *jack_client;

  jack_client = (AgsJackClient *) malloc(sizeof(AgsJackClient));

  jack_client->default_client = FALSE;
  
  jack_client->uuid = NULL;

  jack_client->client = NULL;

  jack_client->port = NULL;

  return(jack_client);
}

void
ags_jack_client_free(AgsJackClient *jack_client)
{
  //TODO:JK: implement me
}

AgsJackPort*
ags_jack_port_alloc()
{
  AgsJackPort *jack_port;

  jack_port = (AgsJackPort *) malloc(sizeof(AgsJackPort));

  jack_port->uuid = NULL;
  jack_port->name = NULL;
  jack_port->uri = NULL;

  jack_port->port = NULL;

  jack_port->gobject = NULL;

  return(jack_port);
}

void
ags_jack_port_free(AgsJackPort *jack_port)
{
  //TODO:JK: implement me
}

AgsJackClient*
ags_jack_server_find_default_client(AgsJackServer *jack_server)
{
  GList *list;

  list = jack_server->client;

  while(list != NULL){
    if(AGS_JACK_CLIENT(list->data)->default_client){
      return(list->data);
    }
  }
  
  return(NULL);
}

GObject*
ags_jack_server_register_default_soundcard(AgsJackServer *jack_server)
{
  AgsJackClient *default_client;
  AgsJackPort *default_soundcard;

  default_client = ags_jack_server_find_default_client(jack_server);

  if(default_client == NULL){
    default_client = ags_jack_client_alloc();

    default_client->default_client = TRUE;

    default_client->client = jack_client_open("ags-default-client\0",
					      0,
					      NULL);
    
    jack_server->client = g_list_prepend(jack_server->client,
					 default_client);
  }


  default_soundcard = ags_jack_port_alloc();
  default_soundcard->port = jack_port_register(default_client,
					       "ags-default-soundcard\0",
					       JACK_DEFAULT_AUDIO_TYPE,
					       JackPortIsOutput,
					       AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE);
  default_soundcard->gobject = ags_jack_devout_new(NULL);
  AGS_JACK_DEVOUT(default_soundcard->gobject)->out_port = default_soundcard->port;

  return(default_soundcard->gobject);
}

/**
 * ags_jack_server_new:
 * @url: the URL as string
 *
 * Instantiate a new #AgsJackServer.
 *
 * Returns: the new #AgsJackServer
 *
 * Since: 0.7.0
 */
AgsJackServer*
ags_jack_server_new(gchar *url)
{
  AgsJackServer *jack_server;

  jack_server = (AgsJackServer *) g_object_new(AGS_TYPE_JACK_SERVER,
					       "url\0", url,
					       NULL);

  return(jack_server);
}
