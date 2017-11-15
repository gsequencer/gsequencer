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

#include <ags/audio/pulse/ags_pulse_server.h>
#include <ags/audio/pulse/ags_pulse_client.h>
#include <ags/audio/pulse/ags_pulse_port.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_thread-posix.h>

#include <ags/audio/pulse/ags_pulse_devout.h>
#include <ags/audio/pulse/ags_pulse_devin.h>

#include <string.h>

#include <errno.h>

#include <ags/i18n.h>

void ags_pulse_server_class_init(AgsPulseServerClass *pulse_server);
void ags_pulse_server_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pulse_server_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager);
void ags_pulse_server_init(AgsPulseServer *pulse_server);
void ags_pulse_server_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_pulse_server_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_pulse_server_connect(AgsConnectable *connectable);
void ags_pulse_server_disconnect(AgsConnectable *connectable);
void ags_pulse_server_dispose(GObject *gobject);
void ags_pulse_server_finalize(GObject *gobject);

void ags_pulse_server_set_url(AgsDistributedManager *distributed_manager,
			      gchar *url);
gchar* ags_pulse_server_get_url(AgsDistributedManager *distributed_manager);
void ags_pulse_server_set_ports(AgsDistributedManager *distributed_manager,
				guint *ports, guint port_count);
guint* ags_pulse_server_get_ports(AgsDistributedManager *distributed_manager,
				  guint *port_count);
void ags_pulse_server_set_soundcard(AgsDistributedManager *distributed_manager,
				    gchar *client_uuid,
				    GList *soundcard);
GList* ags_pulse_server_get_soundcard(AgsDistributedManager *distributed_manager,
				      gchar *client_uuid);
void ags_pulse_server_set_sequencer(AgsDistributedManager *distributed_manager,
				    gchar *client_uuid,
				    GList *sequencer);
GList* ags_pulse_server_get_sequencer(AgsDistributedManager *distributed_manager,
				      gchar *client_uuid);
GObject* ags_pulse_server_register_soundcard(AgsDistributedManager *distributed_manager,
					     gboolean is_output);
void ags_pulse_server_unregister_soundcard(AgsDistributedManager *distributed_manager,
					   GObject *soundcard);
GObject* ags_pulse_server_register_sequencer(AgsDistributedManager *distributed_manager,
					     gboolean is_output);
void ags_pulse_server_unregister_sequencer(AgsDistributedManager *distributed_manager,
					   GObject *sequencer);

void* ags_pulse_server_do_poll_loop(void *ptr);

/**
 * SECTION:ags_pulse_server
 * @short_description: pulseaudio instance
 * @title: AgsPulseServer
 * @section_id:
 * @include: ags/audio/pulse/ags_pulse_server.h
 *
 * The #AgsPulseServer is an object to represent a running pulseaudio instance.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_URL,
  PROP_DEFAULT_SOUNDCARD,
  PROP_DEFAULT_PULSE_CLIENT,
  PROP_PULSE_CLIENT,
};

static gpointer ags_pulse_server_parent_class = NULL;

GType
ags_pulse_server_get_type()
{
  static GType ags_type_pulse_server = 0;

  if(!ags_type_pulse_server){
    static const GTypeInfo ags_pulse_server_info = {
      sizeof (AgsPulseServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pulse_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPulseServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pulse_server_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pulse_server_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_distributed_manager_interface_info = {
      (GInterfaceInitFunc) ags_pulse_server_distributed_manager_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_pulse_server = g_type_register_static(G_TYPE_OBJECT,
						   "AgsPulseServer",
						   &ags_pulse_server_info,
						   0);

    g_type_add_interface_static(ags_type_pulse_server,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_pulse_server,
				AGS_TYPE_DISTRIBUTED_MANAGER,
				&ags_distributed_manager_interface_info);
  }

  return (ags_type_pulse_server);
}

void
ags_pulse_server_class_init(AgsPulseServerClass *pulse_server)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_pulse_server_parent_class = g_type_class_peek_parent(pulse_server);

  /* GObjectClass */
  gobject = (GObjectClass *) pulse_server;

  gobject->set_property = ags_pulse_server_set_property;
  gobject->get_property = ags_pulse_server_get_property;

  gobject->dispose = ags_pulse_server_dispose;
  gobject->finalize = ags_pulse_server_finalize;

  /* properties */
  /**
   * AgsPulseServer:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 1.0.0
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
   * AgsPulseServer:url:
   *
   * The assigned URL.
   * 
   * Since: 1.0.0
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
   * AgsPulseServer:default-soundcard:
   *
   * The default soundcard.
   * 
   * Since: 1.0.0
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
   * AgsPulseServer:default-pulse-client:
   *
   * The default pulse client.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("default-pulse-client",
				   i18n_pspec("default pulse client"),
				   i18n_pspec("The default pulse client"),
				   AGS_TYPE_PULSE_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_PULSE_CLIENT,
				  param_spec);

  /**
   * AgsPulseServer:pulse-client:
   *
   * The pulse client list.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("pulse-client",
				   i18n_pspec("pulse client list"),
				   i18n_pspec("The pulse client list"),
				   AGS_TYPE_PULSE_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PULSE_CLIENT,
				  param_spec);
}

void
ags_pulse_server_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_pulse_server_connect;
  connectable->disconnect = ags_pulse_server_disconnect;
}

void
ags_pulse_server_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager)
{
  distributed_manager->set_url = ags_pulse_server_set_url;
  distributed_manager->get_url = ags_pulse_server_get_url;
  distributed_manager->set_ports = ags_pulse_server_set_ports;
  distributed_manager->get_ports = ags_pulse_server_get_ports;
  distributed_manager->set_soundcard = ags_pulse_server_set_soundcard;
  distributed_manager->get_soundcard = ags_pulse_server_get_soundcard;
  distributed_manager->set_sequencer = ags_pulse_server_set_sequencer;
  distributed_manager->get_sequencer = ags_pulse_server_get_sequencer;
  distributed_manager->register_soundcard = ags_pulse_server_register_soundcard;
  distributed_manager->unregister_soundcard = ags_pulse_server_unregister_soundcard;
  distributed_manager->register_sequencer = ags_pulse_server_register_sequencer;
  distributed_manager->unregister_sequencer = ags_pulse_server_unregister_sequencer;
}

void
ags_pulse_server_init(AgsPulseServer *pulse_server)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert server mutex */
  pulse_server->mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  pulse_server->mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) pulse_server,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  pulse_server->flags = 0;
  
  g_atomic_int_set(&(pulse_server->running),
		   TRUE);
  pulse_server->thread = (pthread_t *) malloc(sizeof(pthread_t));
  
  pulse_server->application_context = NULL;

#ifdef AGS_WITH_PULSE
  pulse_server->main_loop = pa_mainloop_new();
  pulse_server->main_loop_api = pa_mainloop_get_api(pulse_server->main_loop);
#else
  pulse_server->main_loop = NULL;
  pulse_server->main_loop_api = NULL;
#endif

  pulse_server->url = NULL;
  
  pulse_server->port = NULL;
  pulse_server->port_count = 0;
  
  pulse_server->n_soundcards = 0;
  pulse_server->n_sequencers = 0;

  pulse_server->default_soundcard = NULL;

  pulse_server->default_client = NULL;
  pulse_server->client = NULL;
}

void
ags_pulse_server_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsPulseServer *pulse_server;

  pulse_server = AGS_PULSE_SERVER(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if(pulse_server->application_context == (GObject *) application_context){
	return;
      }

      if(pulse_server->application_context != NULL){
	g_object_unref(G_OBJECT(pulse_server->application_context));
      }

      if(application_context != NULL){
	g_object_ref(G_OBJECT(application_context));
      }

      pulse_server->application_context = (GObject *) application_context;
    }
    break;
  case PROP_URL:
    {
      gchar *url;

      url = g_value_get_string(value);

      if(pulse_server->url == url){
	return;
      }

      if(pulse_server->url != NULL){
	g_free(pulse_server->url);
      }

      pulse_server->url = g_strdup(url);
    }
    break;
  case PROP_DEFAULT_SOUNDCARD:
    {
      GObject *default_soundcard;

      default_soundcard = (GObject *) g_value_get_object(value);

      if(pulse_server->default_soundcard == (GObject *) default_soundcard){
	return;
      }

      if(pulse_server->default_soundcard != NULL){
	g_object_unref(G_OBJECT(pulse_server->default_soundcard));
      }

      if(default_soundcard != NULL){
	g_object_ref(G_OBJECT(default_soundcard));
      }

      pulse_server->default_soundcard = (GObject *) default_soundcard;
    }
    break;
  case PROP_DEFAULT_PULSE_CLIENT:
    {
      AgsPulseClient *default_client;

      default_client = (AgsPulseClient *) g_value_get_object(value);

      if(pulse_server->default_client == (GObject *) default_client){
	return;
      }

      if(pulse_server->default_client != NULL){
	g_object_unref(G_OBJECT(pulse_server->default_client));
      }

      if(default_client != NULL){
	g_object_ref(G_OBJECT(default_client));
      }

      pulse_server->default_client = (GObject *) default_client;
    }
    break;
  case PROP_PULSE_CLIENT:
    {
      GObject *client;

      client = (GObject *) g_value_get_object(value);

      if(g_list_find(pulse_server->client, client) != NULL){
	return;
      }

      if(client != NULL){
	g_object_ref(G_OBJECT(client));

	pulse_server->client = g_list_prepend(pulse_server->client,
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
ags_pulse_server_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsPulseServer *pulse_server;

  pulse_server = AGS_PULSE_SERVER(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, pulse_server->application_context);
    }
    break;
  case PROP_URL:
    {
      g_value_set_string(value, pulse_server->url);
    }
    break;
  case PROP_DEFAULT_SOUNDCARD:
    {
      g_value_set_object(value, pulse_server->default_soundcard);
    }
    break;
  case PROP_DEFAULT_PULSE_CLIENT:
    {
      g_value_set_object(value, pulse_server->default_soundcard);
    }
    break;
  case PROP_PULSE_CLIENT:
    {
      g_value_set_pointer(value,
			  g_list_copy(pulse_server->client));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_server_connect(AgsConnectable *connectable)
{
  AgsPulseServer *pulse_server;

  GList *list;
  
  pulse_server = AGS_PULSE_SERVER(connectable);

  if((AGS_PULSE_SERVER_CONNECTED & (pulse_server->flags)) != 0){
    return;
  }

  pulse_server->flags |= AGS_PULSE_SERVER_CONNECTED;

  list = pulse_server->client;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_pulse_server_disconnect(AgsConnectable *connectable)
{
  AgsPulseServer *pulse_server;

  GList *list;
  
  pulse_server = AGS_PULSE_SERVER(connectable);

  if((AGS_PULSE_SERVER_CONNECTED & (pulse_server->flags)) != 0){
    return;
  }

  pulse_server->flags |= AGS_PULSE_SERVER_CONNECTED;

  list = pulse_server->client;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_pulse_server_dispose(GObject *gobject)
{
  AgsPulseServer *pulse_server;

  GList *list;
  
  pulse_server = AGS_PULSE_SERVER(gobject);

  /* application context */
  if(pulse_server->application_context != NULL){
    g_object_unref(G_OBJECT(pulse_server->application_context));
    
    pulse_server->application_context = NULL;
  }

  /* default soundcard */
  if(pulse_server->default_soundcard != NULL){
    g_object_unref(G_OBJECT(pulse_server->default_soundcard));

    pulse_server->default_soundcard = NULL;
  }
  
  /* default client */
  if(pulse_server->default_client != NULL){
    g_object_unref(G_OBJECT(pulse_server->default_client));

    pulse_server->default_client = NULL;
  }
  
  /* client */
  if(pulse_server->client != NULL){
    list = pulse_server->client;

    while(list != NULL){
      g_object_run_dispose(G_OBJECT(list->data));

      list = list->next;
    }
    
    g_list_free_full(pulse_server->client,
		     g_object_unref);

    pulse_server->client = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_pulse_server_parent_class)->dispose(gobject);
}

void
ags_pulse_server_finalize(GObject *gobject)
{
  AgsPulseServer *pulse_server;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;

  pulse_server = AGS_PULSE_SERVER(gobject);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* remove pulse server mutex */
  pthread_mutex_lock(application_mutex);  

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(application_mutex);
  
  /* application context */
  if(pulse_server->application_context != NULL){
    g_object_unref(G_OBJECT(pulse_server->application_context));
  }

  /* url */
  g_free(pulse_server->url);

  /* default soundcard */
  if(pulse_server->default_soundcard != NULL){
    g_object_unref(G_OBJECT(pulse_server->default_soundcard));
  }
  
  /* default client */
  if(pulse_server->default_client != NULL){
    g_object_unref(G_OBJECT(pulse_server->default_client));
  }
  
  /* client */
  if(pulse_server->client != NULL){
    g_list_free_full(pulse_server->client,
		     g_object_unref);
  }

  pthread_mutex_destroy(pulse_server->mutex);
  free(pulse_server->mutex);

  pthread_mutexattr_destroy(pulse_server->mutexattr);
  free(pulse_server->mutexattr);
  
  /* call parent */
  G_OBJECT_CLASS(ags_pulse_server_parent_class)->finalize(gobject);
}

void
ags_pulse_server_set_url(AgsDistributedManager *distributed_manager,
			 gchar *url)
{
  AGS_PULSE_SERVER(distributed_manager)->url = url;
}

gchar*
ags_pulse_server_get_url(AgsDistributedManager *distributed_manager)
{
  return(AGS_PULSE_SERVER(distributed_manager)->url);
}

void
ags_pulse_server_set_ports(AgsDistributedManager *distributed_manager,
			   guint *port, guint port_count)
{
  AGS_PULSE_SERVER(distributed_manager)->port = port;
  AGS_PULSE_SERVER(distributed_manager)->port_count = port_count;
}

guint*
ags_pulse_server_get_ports(AgsDistributedManager *distributed_manager,
			   guint *port_count)
{
  if(port_count != NULL){
    *port_count = AGS_PULSE_SERVER(distributed_manager)->port_count;
  }
  
  return(AGS_PULSE_SERVER(distributed_manager)->port);
}

void
ags_pulse_server_set_soundcard(AgsDistributedManager *distributed_manager,
			       gchar *client_uuid,
			       GList *soundcard)
{
  AgsPulseServer *pulse_server;
  AgsPulseClient *pulse_client;

  GList *list;

  pulse_server = AGS_PULSE_SERVER(distributed_manager);

  pulse_client = (AgsPulseClient *) ags_pulse_server_find_client(pulse_server,
								 client_uuid);

  //NOTE:JK: soundcard won't removed
  list = soundcard;

  while(list != NULL){
    ags_pulse_client_add_device(pulse_client,
				(GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_pulse_server_get_soundcard(AgsDistributedManager *distributed_manager,
			       gchar *client_uuid)
{
  AgsPulseServer *pulse_server;
  AgsPulseClient *pulse_client;

  GList *list, *device;
  
  pulse_server = AGS_PULSE_SERVER(distributed_manager);

  pulse_client = (AgsPulseClient *) ags_pulse_server_find_client(pulse_server,
								 client_uuid);

  device = pulse_client->device;
  list = NULL;

  while(device != NULL){
    if(AGS_IS_PULSE_DEVOUT(device->data) ||
       AGS_IS_PULSE_DEVIN(device->data)){
      list = g_list_prepend(list,
			    device->data);
    }

    device = device->next;
  }

  return(g_list_reverse(list));
}

void
ags_pulse_server_set_sequencer(AgsDistributedManager *distributed_manager,
			       gchar *client_uuid,
			       GList *sequencer)
{
  AgsPulseServer *pulse_server;
  AgsPulseClient *pulse_client;

  GList *list;

  pulse_server = AGS_PULSE_SERVER(distributed_manager);

  pulse_client = (AgsPulseClient *) ags_pulse_server_find_client(pulse_server,
								 client_uuid);

  //NOTE:JK: sequencer won't removed
  list = sequencer;

  while(list != NULL){
    ags_pulse_client_add_device(pulse_client,
				(GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_pulse_server_get_sequencer(AgsDistributedManager *distributed_manager,
			       gchar *client_uuid)
{
  AgsPulseServer *pulse_server;
  AgsPulseClient *pulse_client;

  GList *list, *device;
  
  pulse_server = AGS_PULSE_SERVER(distributed_manager);

  pulse_client = (AgsPulseClient *) ags_pulse_server_find_client(pulse_server,
								 client_uuid);

  device = pulse_client->device;
  list = NULL;

  while(device != NULL){
    //    if(AGS_IS_PULSE_MIDIIN(device->data)){
    //      list = g_list_prepend(list,
    //			    device->data);
    //    }

    device = device->next;
  }

  return(g_list_reverse(list));
}

GObject*
ags_pulse_server_register_soundcard(AgsDistributedManager *distributed_manager,
				    gboolean is_output)
{
  AgsPulseServer *pulse_server;
  AgsPulseClient *default_client;
  AgsPulsePort *pulse_port;
  AgsPulseDevout *pulse_devout;
  AgsPulseDevin *pulse_devin;

  GObject *soundcard;
  
  gchar *str;  

  gboolean initial_set;
  guint i;  

  pulse_server = AGS_PULSE_SERVER(distributed_manager);
  initial_set = FALSE;
  
  /* the default client */
  if(pulse_server->default_client == NULL){
    g_object_set(pulse_server,
		 "default-pulse-client", ags_pulse_client_new((GObject *) pulse_server),
		 NULL);
    ags_pulse_server_add_client(pulse_server,
				pulse_server->default_client);
    
    ags_pulse_client_open((AgsPulseClient *) pulse_server->default_client,
			  "ags-default-client");
    initial_set = TRUE;
    
    if(AGS_PULSE_CLIENT(pulse_server->default_client)->context == NULL){
      g_warning("ags_pulse_server.c - can't open pulseaudio client");
    }
  }

  default_client = (AgsPulseClient *) pulse_server->default_client;

  soundcard = NULL;

  /* the soundcard */
  if(is_output){
    soundcard = 
      pulse_devout = ags_pulse_devout_new(pulse_server->application_context);
    str = g_strdup_printf("ags-pulse-devout-%d",
			  pulse_server->n_soundcards);
    g_object_set(AGS_PULSE_DEVOUT(pulse_devout),
		 "pulse-client", default_client,
		 "device", str,
		 NULL);
    g_free(str);
    g_object_set(default_client,
		 "device", pulse_devout,
		 NULL);
        
    /* register ports */
    str = g_strdup_printf("ags-soundcard%d",
			  pulse_server->n_soundcards);
    
#ifdef AGS_DEBUG
    g_message("%s", str);
#endif
      
    pulse_port = ags_pulse_port_new((GObject *) default_client);
    g_object_set(pulse_port,
		 "pulse-devout", pulse_devout,
		 NULL);
    ags_pulse_client_add_port(default_client,
			      (GObject *) pulse_port);

    g_object_set(pulse_devout,
		 "pulse-port", pulse_port,
		 NULL);
      
    pulse_devout->port_name = (gchar **) malloc(2 * sizeof(gchar *));
    pulse_devout->port_name[0] = g_strdup(str);
    pulse_devout->port_name[1] = NULL;
    
    ags_pulse_port_register(pulse_port,
			    str,
			    TRUE, FALSE,
			    TRUE);

    ags_pulse_devout_realloc_buffer(pulse_devout);
    pulse_server->n_soundcards += 1;
  }else{
    soundcard = 
      pulse_devin = ags_pulse_devin_new(pulse_server->application_context);
    str = g_strdup_printf("ags-pulse-devin-%d",
			  pulse_server->n_soundcards);
    g_object_set(AGS_PULSE_DEVIN(pulse_devin),
		 "pulse-client", default_client,
		 "device", str,
		 NULL);
    g_free(str);
    g_object_set(default_client,
		 "device", pulse_devin,
		 NULL);
        
    /* register ports */
    str = g_strdup_printf("ags-soundcard%d",
			  pulse_server->n_soundcards);
    
#ifdef AGS_DEBUG
    g_message("%s", str);
#endif
      
    pulse_port = ags_pulse_port_new((GObject *) default_client);
    g_object_set(pulse_port,
		 "pulse-devin", pulse_devin,
		 NULL);
    ags_pulse_client_add_port(default_client,
			      (GObject *) pulse_port);

    g_object_set(pulse_devin,
		 "pulse-port", pulse_port,
		 NULL);
      
    pulse_devin->port_name = (gchar **) malloc(2 * sizeof(gchar *));
    pulse_devin->port_name[0] = g_strdup(str);
    pulse_devin->port_name[1] = NULL;
    
    ags_pulse_port_register(pulse_port,
			    str,
			    TRUE, FALSE,
			    TRUE);

    ags_pulse_devin_realloc_buffer(pulse_devin);
    pulse_server->n_soundcards += 1;
  }
  
  return((GObject *) pulse_devin);
}

void
ags_pulse_server_unregister_soundcard(AgsDistributedManager *distributed_manager,
				      GObject *soundcard)
{
  AgsPulseServer *pulse_server;
  AgsPulseClient *default_client;

  GList *list;

  pulse_server = AGS_PULSE_SERVER(distributed_manager);
  
  /* the default client */
  default_client = (AgsPulseClient *) pulse_server->default_client;

  if(default_client == NULL){
    g_warning("GSequencer - no pulse client");
    
    return;
  }

  if(AGS_IS_PULSE_DEVOUT(soundcard)){
    list = AGS_PULSE_DEVOUT(soundcard)->pulse_port;

    while(list != NULL){
      ags_pulse_port_unregister(list->data);
      ags_pulse_client_remove_port(default_client,
				   list->data);
    
      list = list->next;
    }
  }else if(AGS_IS_PULSE_DEVIN(soundcard)){
    list = AGS_PULSE_DEVIN(soundcard)->pulse_port;

    while(list != NULL){
      ags_pulse_port_unregister(list->data);
      ags_pulse_client_remove_port(default_client,
				   list->data);
    
      list = list->next;
    }
  }
  
  ags_pulse_client_remove_device(default_client,
				 soundcard);
  
  if(default_client->port == NULL){
    pulse_server->n_soundcards = 0;
  }
}

GObject*
ags_pulse_server_register_sequencer(AgsDistributedManager *distributed_manager,
				    gboolean is_output)
{
  g_message("GSequencer - can't register pulseaudio sequencer");
  
  return(NULL);
}

void
ags_pulse_server_unregister_sequencer(AgsDistributedManager *distributed_manager,
				      GObject *sequencer)
{
  g_message("GSequencer - can't unregister pulseaudio sequencer");
}

GObject*
ags_pulse_server_register_default_soundcard(AgsPulseServer *pulse_server)
{
  AgsPulseClient *default_client;
  AgsPulseDevout *pulse_devout;
  AgsPulsePort *pulse_port;

  gchar *str;
  
  guint i;
  
  /* the default client */
  if(pulse_server->default_client == NULL){
    g_object_set(pulse_server,
		 "default-pulse-client", (GObject *) ags_pulse_client_new((GObject *) pulse_server),
		 NULL);
    ags_pulse_server_add_client(pulse_server,
				pulse_server->default_client);
    
    ags_pulse_client_open((AgsPulseClient *) pulse_server->default_client,
			  "ags-default-client");

    if(AGS_PULSE_CLIENT(pulse_server->default_client)->context == NULL){
      g_warning("ags_pulse_server.c - can't open pulseaudio client");
      
      return(NULL);
    }
  }

  default_client = (AgsPulseClient *) pulse_server->default_client;

  /* the soundcard */
  pulse_devout = ags_pulse_devout_new(pulse_server->application_context);
  g_object_set(AGS_PULSE_DEVOUT(pulse_devout),
	       "pulse-client", default_client,
	       "device", "ags-default-devout",
	       NULL);
  g_object_set(default_client,
	       "device", pulse_devout,
	       NULL);
  
  /* register ports */
  str = g_strdup_printf("ags-default-soundcard");

#ifdef AGS_DEBUG
  g_message("%s", str);
#endif
    
  pulse_port = ags_pulse_port_new((GObject *) default_client);
  g_object_set(pulse_port,
	       "pulse-devout", pulse_devout,
	       NULL);
  ags_pulse_client_add_port(default_client,
			    (GObject *) pulse_port);

  g_object_set(pulse_devout,
	       "pulse-port", pulse_port,
	       NULL);
  
  pulse_devout->port_name = (gchar **) malloc(2 * sizeof(gchar *));
  pulse_devout->port_name[0] = g_strdup(str);
  pulse_devout->port_name[1] = NULL;
  
  ags_pulse_port_register(pulse_port,
			  str,
			  TRUE, FALSE,
			  TRUE);

  g_free(str);
  
  return((GObject *) pulse_devout);
}

/**
 * ags_pulse_server_find_url:
 * @pulse_server: the #AgsPulseServer
 * @url: the url to find
 *
 * Find #AgsPulseServer by url.
 *
 * Returns: the #GList containing a #AgsPulseServer matching @url or %NULL
 *
 * Since: 1.0.0
 */
GList*
ags_pulse_server_find_url(GList *pulse_server,
			  gchar *url)
{
  while(pulse_server != NULL){
    if(!g_ascii_strcasecmp(AGS_PULSE_SERVER(pulse_server->data)->url,
			   url)){
      return(pulse_server);
    }

    pulse_server = pulse_server->next;
  }

  return(NULL);
}

/**
 * ags_pulse_server_find_client:
 * @pulse_server: the #AgsPulseServer
 * @client_uuid: the uuid to find
 *
 * Find #AgsPulseClient by uuid.
 *
 * Returns: the #AgsPulseClient found or %NULL
 *
 * Since: 1.0.0
 */
GObject*
ags_pulse_server_find_client(AgsPulseServer *pulse_server,
			     gchar *client_uuid)
{
  GList *list;

  list = pulse_server->client;
  
  while(list != NULL){
    if(!g_ascii_strcasecmp(AGS_PULSE_CLIENT(list->data)->uuid,
			   client_uuid)){
      return(list->data);
    }

    list = list->next;
  }
  
  return(NULL);
}

/**
 * ags_pulse_server_find_port:
 * @pulse_server: the #AgsPulseServer
 * @port_uuid: the uuid to find
 *
 * Find #AgsPulsePort by uuid.
 *
 * Returns: the #AgsPulsePort found or %NULL
 *
 * Since: 1.0.0
 */
GObject*
ags_pulse_server_find_port(AgsPulseServer *pulse_server,
			   gchar *port_uuid)
{
  GList *client, *port;

  client = pulse_server->client;
  
  while(client != NULL){
    port = AGS_PULSE_CLIENT(client->data)->port;

    while(port != NULL){
      if(!g_ascii_strcasecmp(AGS_PULSE_CLIENT(port->data)->uuid,
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
 * ags_pulse_server_add_client:
 * @pulse_server: the #AgsPulseServer
 * @pulse_client: the #AgsPulseClient to add
 *
 * Add @pulse_client to @pulse_server
 *
 * Since: 1.0.0
 */
void
ags_pulse_server_add_client(AgsPulseServer *pulse_server,
			    GObject *pulse_client)
{
  if(!AGS_IS_PULSE_SERVER(pulse_server) ||
     !AGS_IS_PULSE_CLIENT(pulse_client)){
    return;
  }

  g_object_ref(pulse_client);
  pulse_server->client = g_list_prepend(pulse_server->client,
					pulse_client);
}

/**
 * ags_pulse_server_remove_client:
 * @pulse_server: the #AgsPulseServer
 * @pulse_client: the #AgsPulseClient to remove
 *
 * Remove @pulse_client to @pulse_server
 *
 * Since: 1.0.0
 */
void
ags_pulse_server_remove_client(AgsPulseServer *pulse_server,
			       GObject *pulse_client)
{
  if(!AGS_IS_PULSE_SERVER(pulse_server) ||
     !AGS_IS_PULSE_CLIENT(pulse_client)){
    return;
  }

  pulse_server->client = g_list_remove(pulse_server->client,
				       pulse_client);
  g_object_unref(pulse_client);
}

/**
 * ags_pulse_server_connect_client:
 * @pulse_server: the #AgsPulseServer
 *
 * Connect all clients.
 *
 * Since: 1.0.0
 */
void
ags_pulse_server_connect_client(AgsPulseServer *pulse_server)
{
  GList *client;

  client = pulse_server->client;

  while(client != NULL){
    ags_pulse_client_open((AgsPulseClient *) client->data,
			  AGS_PULSE_CLIENT(client->data)->name);
    ags_pulse_client_activate(client->data);

    client = client->next;
  }
}

void*
ags_pulse_server_do_poll_loop(void *ptr)
{
  AgsPulseServer *pulse_server;

#ifndef __APPLE__
  struct sched_param param;
#endif

  pulse_server = (AgsPulseServer *) ptr;
    
  /* Declare ourself as a real time task */
#ifndef __APPLE__
  param.sched_priority = AGS_RT_PRIORITY;
  
  if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
    perror("sched_setscheduler failed");
  }
#endif

#ifdef AGS_WITH_PULSE
  pa_mainloop_run(pulse_server->main_loop,
		  NULL);
#endif

  pthread_exit(NULL);
}

void
ags_pulse_server_start_poll(AgsPulseServer *pulse_server)
{
  pthread_create(pulse_server->thread, NULL,
		 ags_pulse_server_do_poll_loop, pulse_server);
}

/**
 * ags_pulse_server_new:
 * @application_context: the #AgsApplicationContext
 * @url: the URL as string
 *
 * Instantiate a new #AgsPulseServer.
 *
 * Returns: the new #AgsPulseServer
 *
 * Since: 1.0.0
 */
AgsPulseServer*
ags_pulse_server_new(GObject *application_context,
		     gchar *url)
{
  AgsPulseServer *pulse_server;

  pulse_server = (AgsPulseServer *) g_object_new(AGS_TYPE_PULSE_SERVER,
						 "application-context", application_context,
						 "url", url,
						 NULL);

  return(pulse_server);
}
