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

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_port.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_midiin.h>

#include <string.h>

#include <errno.h>

#include <ags/i18n.h>

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
void ags_jack_server_dispose(GObject *gobject);
void ags_jack_server_finalize(GObject *gobject);

void ags_jack_server_set_url(AgsDistributedManager *distributed_manager,
			     gchar *url);
gchar* ags_jack_server_get_url(AgsDistributedManager *distributed_manager);
void ags_jack_server_set_ports(AgsDistributedManager *distributed_manager,
			       guint *ports, guint port_count);
guint* ags_jack_server_get_ports(AgsDistributedManager *distributed_manager,
				 guint *port_count);
void ags_jack_server_set_soundcard(AgsDistributedManager *distributed_manager,
				   gchar *client_uuid,
				   GList *soundcard);
GList* ags_jack_server_get_soundcard(AgsDistributedManager *distributed_manager,
				     gchar *client_uuid);
void ags_jack_server_set_sequencer(AgsDistributedManager *distributed_manager,
				   gchar *client_uuid,
				   GList *sequencer);
GList* ags_jack_server_get_sequencer(AgsDistributedManager *distributed_manager,
				     gchar *client_uuid);
GObject* ags_jack_server_register_soundcard(AgsDistributedManager *distributed_manager,
					    gboolean is_output);
void ags_jack_server_unregister_soundcard(AgsDistributedManager *distributed_manager,
					  GObject *soundcard);
GObject* ags_jack_server_register_sequencer(AgsDistributedManager *distributed_manager,
					    gboolean is_output);
void ags_jack_server_unregister_sequencer(AgsDistributedManager *distributed_manager,
					  GObject *sequencer);

/**
 * SECTION:ags_jack_server
 * @short_description: JACK instance
 * @title: AgsJackServer
 * @section_id:
 * @include: ags/audio/jack/ags_jack_server.h
 *
 * The #AgsJackServer is an object to represent a running JACK instance.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_URL,
  PROP_DEFAULT_SOUNDCARD,
  PROP_DEFAULT_JACK_CLIENT,
  PROP_JACK_CLIENT,
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
						  "AgsJackServer",
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

  gobject->dispose = ags_jack_server_dispose;
  gobject->finalize = ags_jack_server_finalize;

  /* properties */
  /**
   * AgsJackServer:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 0.7.1
   */
  param_spec = g_param_spec_object("application-context",
				   i18n_pspec("the application context object"),
				   i18n_pspec("The application context object"),
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
  param_spec = g_param_spec_string("url",
				   i18n_pspec("the URL"),
				   i18n_pspec("The URL"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_URL,
				  param_spec);

  /**
   * AgsJackServer:default-soundcard:
   *
   * The default soundcard.
   * 
   * Since: 0.7.122.7
   */
  param_spec = g_param_spec_object("default-soundcard",
				   i18n_pspec("default soundcard"),
				   i18n_pspec("The default soundcard"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_SOUNDCARD,
				  param_spec);

  /**
   * AgsJackServer:default-jack-client:
   *
   * The default jack client.
   * 
   * Since: 0.7.122.7
   */
  param_spec = g_param_spec_object("default-jack-client",
				   i18n_pspec("default jack client"),
				   i18n_pspec("The default jack client"),
				   AGS_TYPE_JACK_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_JACK_CLIENT,
				  param_spec);

  /**
   * AgsJackServer:jack-client:
   *
   * The jack client list.
   * 
   * Since: 0.7.122.7
   */
  param_spec = g_param_spec_object("jack-client",
				   i18n_pspec("jack client list"),
				   i18n_pspec("The jack client list"),
				   AGS_TYPE_JACK_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_JACK_CLIENT,
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
  AgsMutexManager *mutex_manager;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert server mutex */
  jack_server->mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  jack_server->mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) jack_server,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  jack_server->flags = 0;

  jack_server->application_context = NULL;

  jack_server->url = g_strdup_printf("%s://%s:%d",
				     AGS_JACK_SERVER_DEFAULT_PROTOCOL,
				     AGS_JACK_SERVER_DEFAULT_HOST,
				     AGS_JACK_SERVER_DEFAULT_PORT);
  //  jack_server->jackctl = jackctl_server_create(NULL,
  //					       NULL);

  jack_server->port = NULL;
  jack_server->port_count = 0;
  
  jack_server->n_soundcards = 0;
  jack_server->n_sequencers = 0;

  jack_server->default_soundcard = NULL;

  jack_server->default_client = NULL;
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

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if(jack_server->application_context == (GObject *) application_context){
	return;
      }

      if(jack_server->application_context != NULL){
	g_object_unref(G_OBJECT(jack_server->application_context));
      }

      if(application_context != NULL){
	g_object_ref(G_OBJECT(application_context));
      }

      jack_server->application_context = (GObject *) application_context;
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
  case PROP_DEFAULT_SOUNDCARD:
    {
      GObject *default_soundcard;

      default_soundcard = (GObject *) g_value_get_object(value);

      if(jack_server->default_soundcard == (GObject *) default_soundcard){
	return;
      }

      if(jack_server->default_soundcard != NULL){
	g_object_unref(G_OBJECT(jack_server->default_soundcard));
      }

      if(default_soundcard != NULL){
	g_object_ref(G_OBJECT(default_soundcard));
      }

      jack_server->default_soundcard = (GObject *) default_soundcard;
    }
    break;
  case PROP_DEFAULT_JACK_CLIENT:
    {
      AgsJackClient *default_client;

      default_client = (AgsJackClient *) g_value_get_object(value);

      if(jack_server->default_client == (GObject *) default_client){
	return;
      }

      if(jack_server->default_client != NULL){
	g_object_unref(G_OBJECT(jack_server->default_client));
      }

      if(default_client != NULL){
	g_object_ref(G_OBJECT(default_client));
      }

      jack_server->default_client = (GObject *) default_client;
    }
    break;
  case PROP_JACK_CLIENT:
    {
      GObject *client;

      client = (GObject *) g_value_get_object(value);

      if(g_list_find(jack_server->client, client) != NULL){
	return;
      }

      if(client != NULL){
	g_object_ref(G_OBJECT(client));

	jack_server->client = g_list_prepend(jack_server->client,
					     client);
      }
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
  case PROP_DEFAULT_SOUNDCARD:
    {
      g_value_set_object(value, jack_server->default_soundcard);
    }
    break;
  case PROP_DEFAULT_JACK_CLIENT:
    {
      g_value_set_object(value, jack_server->default_soundcard);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      g_value_set_pointer(value,
			  g_list_copy(jack_server->client));
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
  AgsJackServer *jack_server;

  GList *list;
  
  jack_server = AGS_JACK_SERVER(connectable);

  if((AGS_JACK_SERVER_CONNECTED & (jack_server->flags)) != 0){
    return;
  }

  jack_server->flags |= AGS_JACK_SERVER_CONNECTED;

  list = jack_server->client;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_jack_server_disconnect(AgsConnectable *connectable)
{
  AgsJackServer *jack_server;

  GList *list;
  
  jack_server = AGS_JACK_SERVER(connectable);

  if((AGS_JACK_SERVER_CONNECTED & (jack_server->flags)) != 0){
    return;
  }

  jack_server->flags |= AGS_JACK_SERVER_CONNECTED;

  list = jack_server->client;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_jack_server_dispose(GObject *gobject)
{
  AgsJackServer *jack_server;

  GList *list;
  
  jack_server = AGS_JACK_SERVER(gobject);

  /* application context */
  if(jack_server->application_context != NULL){
    g_object_unref(G_OBJECT(jack_server->application_context));
    
    jack_server->application_context = NULL;
  }

  /* default soundcard */
  if(jack_server->default_soundcard != NULL){
    g_object_unref(G_OBJECT(jack_server->default_soundcard));

    jack_server->default_soundcard = NULL;
  }
  
  /* default client */
  if(jack_server->default_client != NULL){
    g_object_unref(G_OBJECT(jack_server->default_client));

    jack_server->default_client = NULL;
  }
  
  /* client */
  if(jack_server->client != NULL){
    list = jack_server->client;

    while(list != NULL){
      g_object_run_dispose(G_OBJECT(list->data));

      list = list->next;
    }
    
    g_list_free_full(jack_server->client,
		     g_object_unref);

    jack_server->client = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_jack_server_parent_class)->dispose(gobject);
}

void
ags_jack_server_finalize(GObject *gobject)
{
  AgsJackServer *jack_server;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;

  jack_server = AGS_JACK_SERVER(gobject);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* remove jack server mutex */
  pthread_mutex_lock(application_mutex);  

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(application_mutex);

  /* application context */
  if(jack_server->application_context != NULL){
    g_object_unref(G_OBJECT(jack_server->application_context));
  }

  /* url */
  g_free(jack_server->url);

  /* default soundcard */
  if(jack_server->default_soundcard != NULL){
    g_object_unref(G_OBJECT(jack_server->default_soundcard));
  }
  
  /* default client */
  if(jack_server->default_client != NULL){
    g_object_unref(G_OBJECT(jack_server->default_client));
  }
  
  /* client */
  if(jack_server->client != NULL){
    g_list_free_full(jack_server->client,
		     g_object_unref);
  }
  
  pthread_mutex_destroy(jack_server->mutex);
  free(jack_server->mutex);

  pthread_mutexattr_destroy(jack_server->mutexattr);
  free(jack_server->mutexattr);

  /* call parent */
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
			  guint *port, guint port_count)
{
  AGS_JACK_SERVER(distributed_manager)->port = port;
  AGS_JACK_SERVER(distributed_manager)->port_count = port_count;
}

guint*
ags_jack_server_get_ports(AgsDistributedManager *distributed_manager,
			  guint *port_count)
{
  if(port_count != NULL){
    *port_count = AGS_JACK_SERVER(distributed_manager)->port_count;
  }
  
  return(AGS_JACK_SERVER(distributed_manager)->port);
}

void
ags_jack_server_set_soundcard(AgsDistributedManager *distributed_manager,
			      gchar *client_uuid,
			      GList *soundcard)
{
  AgsJackServer *jack_server;
  AgsJackClient *jack_client;

  GList *list;

  jack_server = AGS_JACK_SERVER(distributed_manager);

  jack_client = (AgsJackClient *) ags_jack_server_find_client(jack_server,
							      client_uuid);

  //NOTE:JK: soundcard won't removed
  list = soundcard;

  while(list != NULL){
    ags_jack_client_add_device(jack_client,
			       (GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_jack_server_get_soundcard(AgsDistributedManager *distributed_manager,
			      gchar *client_uuid)
{
  AgsJackServer *jack_server;
  AgsJackClient *jack_client;

  GList *list, *device;
  
  jack_server = AGS_JACK_SERVER(distributed_manager);

  jack_client = (AgsJackClient *) ags_jack_server_find_client(jack_server,
							      client_uuid);

  device = jack_client->device;
  list = NULL;

  while(device != NULL){
    if(AGS_IS_JACK_DEVOUT(device->data)){
      list = g_list_prepend(list,
			    device->data);
    }

    device = device->next;
  }

  return(g_list_reverse(list));
}

void
ags_jack_server_set_sequencer(AgsDistributedManager *distributed_manager,
			      gchar *client_uuid,
			      GList *sequencer)
{
  AgsJackServer *jack_server;
  AgsJackClient *jack_client;

  GList *list;

  jack_server = AGS_JACK_SERVER(distributed_manager);

  jack_client = (AgsJackClient *) ags_jack_server_find_client(jack_server,
							      client_uuid);

  //NOTE:JK: sequencer won't removed
  list = sequencer;

  while(list != NULL){
    ags_jack_client_add_device(jack_client,
			       (GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_jack_server_get_sequencer(AgsDistributedManager *distributed_manager,
			      gchar *client_uuid)
{
  AgsJackServer *jack_server;
  AgsJackClient *jack_client;

  GList *list, *device;
  
  jack_server = AGS_JACK_SERVER(distributed_manager);

  jack_client = (AgsJackClient *) ags_jack_server_find_client(jack_server,
							      client_uuid);

  device = jack_client->device;
  list = NULL;

  while(device != NULL){
    if(AGS_IS_JACK_MIDIIN(device->data)){
      list = g_list_prepend(list,
			    device->data);
    }

    device = device->next;
  }

  return(g_list_reverse(list));
}

GObject*
ags_jack_server_register_soundcard(AgsDistributedManager *distributed_manager,
				   gboolean is_output)
{
  AgsJackServer *jack_server;
  AgsJackClient *default_client;
  AgsJackPort *jack_port;
  AgsJackDevout *jack_devout;

  gchar *str;  

  gboolean initial_set;
  int rc;
  guint i;
  
  if(!is_output){
    g_warning("GSequencer - audio input not implemented");
    return(NULL);
  }

  jack_server = AGS_JACK_SERVER(distributed_manager);
  initial_set = FALSE;
  
  /* the default client */
  if(jack_server->default_client == NULL){
    g_object_set(jack_server,
		 "default-jack-client", ags_jack_client_new((GObject *) jack_server),
		 NULL);
    ags_jack_server_add_client(jack_server,
			       jack_server->default_client);
    
    ags_jack_client_open((AgsJackClient *) jack_server->default_client,
			 "ags-default-client");
    initial_set = TRUE;
    
    if(AGS_JACK_CLIENT(jack_server->default_client)->client == NULL){
      g_warning("ags_jack_server.c - can't open JACK client");
    }
  }

  default_client = (AgsJackClient *) jack_server->default_client;

  /* the soundcard */
  if(is_output){
    jack_devout = ags_jack_devout_new(jack_server->application_context);
    str = g_strdup_printf("ags-jack-devout-%d",
			  jack_server->n_soundcards);
    g_object_set(AGS_JACK_DEVOUT(jack_devout),
		 "jack-client", default_client,
		 "device", str,
		 NULL);
    g_free(str);
    g_object_set(default_client,
		 "device", jack_devout,
		 NULL);
    
#ifdef AGS_WITH_JACK
    if(initial_set &&
       default_client->client != NULL){
      rc = jack_set_buffer_size(default_client->client,
				jack_devout->buffer_size);

      if(rc != 0){
	g_message("%s", strerror(rc));
      }
    }
#endif
    
    /* register ports */
    for(i = 0; i < jack_devout->pcm_channels; i++){
      str = g_strdup_printf("ags-soundcard%d-%04d",
			    jack_server->n_soundcards,
			    i);
      
#ifdef AGS_DEBUG
      g_message("%s", str);
#endif
      
      jack_port = ags_jack_port_new((GObject *) default_client);
      ags_jack_client_add_port(default_client,
			       (GObject *) jack_port);

      g_object_set(jack_devout,
		   "jack-port", jack_port,
		   NULL);
      
      if(jack_devout->port_name == NULL){
	jack_devout->port_name = (gchar **) malloc(2 * sizeof(gchar *));
	jack_devout->port_name[0] = g_strdup(str);
      }else{
	jack_devout->port_name = (gchar **) realloc(jack_devout->port_name,
						    (i + 2) * sizeof(gchar *));
	jack_devout->port_name[i] = g_strdup(str);
      }
    
      ags_jack_port_register(jack_port,
			     str,
			     TRUE, FALSE,
			     TRUE);

      g_free(str);
    }

    if(jack_devout->port_name != NULL){
      jack_devout->port_name[jack_devout->pcm_channels] = NULL;
    }

    ags_jack_devout_realloc_buffer(jack_devout);
    jack_server->n_soundcards += 1;
  }
  
  return((GObject *) jack_devout);
}

void
ags_jack_server_unregister_soundcard(AgsDistributedManager *distributed_manager,
				     GObject *soundcard)
{
  AgsJackServer *jack_server;
  AgsJackClient *default_client;

  GList *list;

  jack_server = AGS_JACK_SERVER(distributed_manager);
  
  /* the default client */
  default_client = (AgsJackClient *) jack_server->default_client;

  if(default_client == NULL){
    g_warning("GSequencer - no jack client");
    
    return;
  }
  
  list = AGS_JACK_DEVOUT(soundcard)->jack_port;

  while(list != NULL){
    ags_jack_port_unregister(list->data);
    ags_jack_client_remove_port(default_client,
				list->data);
    
    list = list->next;
  }

  ags_jack_client_remove_device(default_client,
				soundcard);
  
  if(default_client->port == NULL){
    jack_server->n_soundcards = 0;
  }
}

GObject*
ags_jack_server_register_sequencer(AgsDistributedManager *distributed_manager,
				   gboolean is_output)
{
  AgsJackServer *jack_server;
  AgsJackClient *default_client;
  AgsJackPort *jack_port;
  AgsJackMidiin *jack_midiin;
  gchar *str;
  
  if(is_output){
    g_warning("GSequencer - MIDI output not implemented");
    return(NULL);
  }
  
  jack_server = AGS_JACK_SERVER(distributed_manager);
  
  /* the default client */
  if(jack_server->default_client == NULL){
    g_object_set(jack_server,
		 "default-jack-client", (GObject *) ags_jack_client_new((GObject *) jack_server),
		 NULL);
    ags_jack_server_add_client(jack_server,
			       jack_server->default_client);
    
    ags_jack_client_open((AgsJackClient *) jack_server->default_client,
			 "ags-default-client");

    if(AGS_JACK_CLIENT(jack_server->default_client)->client == NULL){
      g_warning("ags_jack_server.c - can't open JACK client");
    }
  }

  default_client = (AgsJackClient *) jack_server->default_client;

  str = g_strdup_printf("ags-jack-midiin-%d",
			jack_server->n_sequencers);
  jack_midiin = ags_jack_midiin_new(jack_server->application_context);
  g_object_set(AGS_JACK_MIDIIN(jack_midiin),
	       "jack-client", default_client,
	       "device", str,
	       NULL);
  g_object_set(default_client,
	       "device", jack_midiin,
	       NULL);

  /* register sequencer */
  str = g_strdup_printf("ags-sequencer%d",
			jack_server->n_sequencers);

#ifdef AGS_DEBUG
  g_message("%s", str);
#endif
  
  jack_port = ags_jack_port_new((GObject *) default_client);
  ags_jack_client_add_port(default_client,
			   (GObject *) jack_port);

  g_object_set(jack_midiin,
	       "jack-port", jack_port,
	       NULL);
  
  ags_jack_port_register(jack_port,
			 str,
			 FALSE, TRUE,
			 FALSE);

  jack_server->n_sequencers += 1;
  
  return((GObject *) jack_midiin);
}

void
ags_jack_server_unregister_sequencer(AgsDistributedManager *distributed_manager,
				     GObject *sequencer)
{
  AgsJackServer *jack_server;
  AgsJackClient *default_client;

  GList *list;
  
  jack_server = AGS_JACK_SERVER(distributed_manager);

  /* the default client */
  default_client = (AgsJackClient *) jack_server->default_client;

  if(default_client == NULL){
    g_warning("GSequencer - no jack client");
    
    return;
  }

  list = AGS_JACK_MIDIIN(sequencer)->jack_port;

  while(list != NULL){
    ags_jack_port_unregister(list->data);
    ags_jack_client_remove_port(default_client,
				list->data);
    

    list = list->next;
  }

  ags_jack_client_remove_device(default_client,
				sequencer);
  
  if(default_client->port == NULL){
    jack_server->n_sequencers = 0;
  }
}

GObject*
ags_jack_server_register_default_soundcard(AgsJackServer *jack_server)
{
  AgsJackClient *default_client;
  AgsJackDevout *jack_devout;
  AgsJackPort *jack_port;

  gchar *str;
  
  guint i;
  int rc;
  
  /* the default client */
  if(jack_server->default_client == NULL){
    g_object_set(jack_server,
		 "default-jack-client", (GObject *) ags_jack_client_new((GObject *) jack_server),
		 NULL);
    ags_jack_server_add_client(jack_server,
			       jack_server->default_client);
    
    ags_jack_client_open((AgsJackClient *) jack_server->default_client,
			 "ags-default-client");

    if(AGS_JACK_CLIENT(jack_server->default_client)->client == NULL){
      g_warning("ags_jack_server.c - can't open JACK client");
      
      return(NULL);
    }
  }

  default_client = (AgsJackClient *) jack_server->default_client;

  /* the soundcard */
  jack_devout = ags_jack_devout_new(jack_server->application_context);
  g_object_set(AGS_JACK_DEVOUT(jack_devout),
	       "jack-client", default_client,
	       "device", "ags-default-devout",
	       NULL);
  g_object_set(default_client,
	       "device", jack_devout,
	       NULL);

#ifdef AGS_WITH_JACK
  if(default_client->client != NULL){
    rc = jack_set_buffer_size(default_client->client,
			      jack_devout->buffer_size);
    
    if(rc != 0){
      g_message("%s", strerror(rc));
    }
  }
#endif

  /* register ports */
  for(i = 0; i < jack_devout->pcm_channels; i++){
    str = g_strdup_printf("ags-default-soundcard-%04d",
			  i);

#ifdef AGS_DEBUG
    g_message("%s", str);
#endif
    
    jack_port = ags_jack_port_new((GObject *) default_client);
    ags_jack_client_add_port(default_client,
			     (GObject *) jack_port);

    g_object_set(jack_devout,
		 "jack-port", jack_port,
		 NULL);

    if(jack_devout->port_name == NULL){
      jack_devout->port_name = (gchar **) malloc(2 * sizeof(gchar *));
      jack_devout->port_name[0] = g_strdup(str);
    }else{
      jack_devout->port_name = (gchar **) realloc(jack_devout->port_name,
						  (i + 2) * sizeof(gchar *));
      jack_devout->port_name[i] = g_strdup(str);
    }
    
    ags_jack_port_register(jack_port,
			   str,
			   TRUE, FALSE,
			   TRUE);
  }

  if(jack_devout->port_name != NULL){
    jack_devout->port_name[jack_devout->pcm_channels] = NULL;
  }
  
  return((GObject *) jack_devout);
}

/**
 * ags_jack_server_find_url:
 * @jack_server: the #AgsJackServer
 * @url: the url to find
 *
 * Find #AgsJackServer by url.
 *
 * Returns: the #GList containing a #AgsJackServer matching @url or %NULL
 *
 * Since: 0.7.3
 */
GList*
ags_jack_server_find_url(GList *jack_server,
			 gchar *url)
{
  while(jack_server != NULL){
    if(!g_ascii_strcasecmp(AGS_JACK_SERVER(jack_server->data)->url,
			    url)){
      return(jack_server);
    }

    jack_server = jack_server->next;
  }

  return(NULL);
}

/**
 * ags_jack_server_find_client:
 * @jack_server: the #AgsJackServer
 * @client_uuid: the uuid to find
 *
 * Find #AgsJackClient by uuid.
 *
 * Returns: the #AgsJackClient found or %NULL
 *
 * Since: 0.7.3
 */
GObject*
ags_jack_server_find_client(AgsJackServer *jack_server,
			    gchar *client_uuid)
{
  GList *list;

  list = jack_server->client;
  
  while(list != NULL){
    if(!g_ascii_strcasecmp(AGS_JACK_CLIENT(list->data)->uuid,
			   client_uuid)){
      return(list->data);
    }

    list = list->next;
  }
  
  return(NULL);
}

/**
 * ags_jack_server_find_port:
 * @jack_server: the #AgsJackServer
 * @port_uuid: the uuid to find
 *
 * Find #AgsJackPort by uuid.
 *
 * Returns: the #AgsJackPort found or %NULL
 *
 * Since: 0.7.3
 */
GObject*
ags_jack_server_find_port(AgsJackServer *jack_server,
			  gchar *port_uuid)
{
  GList *client, *port;

  client = jack_server->client;
  
  while(client != NULL){
    port = AGS_JACK_CLIENT(client->data)->port;

    while(port != NULL){
      if(!g_ascii_strcasecmp(AGS_JACK_CLIENT(port->data)->uuid,
			     port_uuid)){
	return(port->data);
      }

      port = port->next;
    }
    
    client = client->next;
  }
  
  return(NULL);
}

/**
 * ags_jack_server_add_client:
 * @jack_server: the #AgsJackServer
 * @jack_client: the #AgsJackClient to add
 *
 * Add @jack_client to @jack_server
 *
 * Since: 0.7.3
 */
void
ags_jack_server_add_client(AgsJackServer *jack_server,
			   GObject *jack_client)
{
  if(!AGS_IS_JACK_SERVER(jack_server) ||
     !AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }

  g_object_ref(jack_client);
  jack_server->client = g_list_prepend(jack_server->client,
				       jack_client);
}

/**
 * ags_jack_server_remove_client:
 * @jack_server: the #AgsJackServer
 * @jack_client: the #AgsJackClient to remove
 *
 * Remove @jack_client to @jack_server
 *
 * Since: 0.7.3
 */
void
ags_jack_server_remove_client(AgsJackServer *jack_server,
			      GObject *jack_client)
{
  if(!AGS_IS_JACK_SERVER(jack_server) ||
     !AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }

  jack_server->client = g_list_remove(jack_server->client,
				      jack_client);
  g_object_unref(jack_client);
}

/**
 * ags_jack_server_connect_client:
 * @jack_server: the #AgsJackServer
 *
 * Connect all clients.
 *
 * Since: 0.7.65
 */
void
ags_jack_server_connect_client(AgsJackServer *jack_server)
{
  GList *client;

  client = jack_server->client;

  while(client != NULL){
    ags_jack_client_open((AgsJackClient *) client->data,
			 AGS_JACK_CLIENT(client->data)->name);
    ags_jack_client_activate(client->data);

    client = client->next;
  }
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
					       "application-context", application_context,
					       "url", url,
					       NULL);

  return(jack_server);
}
