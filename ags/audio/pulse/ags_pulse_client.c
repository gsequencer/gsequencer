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

#include <ags/audio/pulse/ags_pulse_client.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_main_loop.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/pulse/ags_pulse_server.h>
#include <ags/audio/pulse/ags_pulse_port.h>
#include <ags/audio/pulse/ags_pulse_devout.h>
#include <ags/audio/pulse/ags_pulse_devin.h>

#include <ags/i18n.h>

void ags_pulse_client_class_init(AgsPulseClientClass *pulse_client);
void ags_pulse_client_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pulse_client_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager);
void ags_pulse_client_init(AgsPulseClient *pulse_client);
void ags_pulse_client_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_pulse_client_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_pulse_client_connect(AgsConnectable *connectable);
void ags_pulse_client_disconnect(AgsConnectable *connectable);
void ags_pulse_client_dispose(GObject *gobject);
void ags_pulse_client_finalize(GObject *gobject);

#ifdef AGS_WITH_PULSE
void ags_pulse_client_state_callback(pa_context *c, AgsPulseClient *pulse_client);
#endif

/**
 * SECTION:ags_pulse_client
 * @short_description: pulseaudio connection
 * @title: AgsPulseClient
 * @section_id:
 * @include: ags/audio/pulse/ags_pulse_client.h
 *
 * The #AgsPulseClient communicates with a pulseaudio instance.
 */

enum{
  PROP_0,
  PROP_PULSE_SERVER,
  PROP_DEVICE,
  PROP_PORT,
};

static gpointer ags_pulse_client_parent_class = NULL;

GType
ags_pulse_client_get_type()
{
  static GType ags_type_pulse_client = 0;

  if(!ags_type_pulse_client){
    static const GTypeInfo ags_pulse_client_info = {
      sizeof (AgsPulseClientClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pulse_client_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPulseClient),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pulse_client_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pulse_client_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_pulse_client = g_type_register_static(G_TYPE_OBJECT,
						   "AgsPulseClient",
						   &ags_pulse_client_info,
						   0);

    g_type_add_interface_static(ags_type_pulse_client,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_pulse_client);
}

void
ags_pulse_client_class_init(AgsPulseClientClass *pulse_client)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_pulse_client_parent_class = g_type_class_peek_parent(pulse_client);

  /* GObjectClass */
  gobject = (GObjectClass *) pulse_client;

  gobject->set_property = ags_pulse_client_set_property;
  gobject->get_property = ags_pulse_client_get_property;

  gobject->dispose = ags_pulse_client_dispose;
  gobject->finalize = ags_pulse_client_finalize;

  /* properties */
  /**
   * AgsPulseClient:pulse-server:
   *
   * The assigned #AgsPulseServer.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("pulse-server",
				   i18n_pspec("assigned pulseaudio server"),
				   i18n_pspec("The assigned pulseaudio server"),
				   AGS_TYPE_PULSE_SERVER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PULSE_SERVER,
				  param_spec);

  /**
   * AgsPulseClient:device:
   *
   * The assigned devices.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("device",
				   i18n_pspec("assigned device"),
				   i18n_pspec("The assigned device"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);

  /**
   * AgsPulseClient:port:
   *
   * The assigned ports.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("port",
				   i18n_pspec("assigned port"),
				   i18n_pspec("The assigned port"),
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT,
				  param_spec);
}

void
ags_pulse_client_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_pulse_client_connect;
  connectable->disconnect = ags_pulse_client_disconnect;
}

void
ags_pulse_client_init(AgsPulseClient *pulse_client)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert client mutex */
  pulse_client->mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  pulse_client->mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) pulse_client,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  pulse_client->flags = 0;

  /* server */
  pulse_client->pulse_server = NULL;
  
  pulse_client->uuid = ags_id_generator_create_uuid();
  pulse_client->name = NULL;

  /* client */
  pulse_client->context = NULL;

  /* device */
  pulse_client->device = NULL;
  pulse_client->port = NULL;
}

void
ags_pulse_client_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsPulseClient *pulse_client;

  pulse_client = AGS_PULSE_CLIENT(gobject);

  switch(prop_id){
  case PROP_PULSE_SERVER:
    {
      AgsPulseServer *pulse_server;

      pulse_server = (AgsPulseServer *) g_value_get_object(value);

      if(pulse_client->pulse_server == (GObject *) pulse_server){
	return;
      }

      if(pulse_client->pulse_server != NULL){
	g_object_unref(pulse_client->pulse_server);
      }

      if(pulse_server != NULL){
	g_object_ref(pulse_server);
      }
      
      pulse_client->pulse_server = (GObject *) pulse_server;
    }
    break;
  case PROP_DEVICE:
    {
      GObject *device;

      device = (GObject *) g_value_get_object(value);

      if(g_list_find(pulse_client->device,
		     device) != NULL){
	return;
      }

      if(device != NULL){
	g_object_ref(device);
	
	pulse_client->device = g_list_prepend(pulse_client->device,
					      device);
      }
    }
    break;
  case PROP_PORT:
    {
      GObject *port;

      port = (GObject *) g_value_get_object(value);

      if(g_list_find(pulse_client->port,
		     port) != NULL){
	return;
      }

      if(port != NULL){
	g_object_ref(port);
	
	pulse_client->port = g_list_prepend(pulse_client->port,
					    port);
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_client_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsPulseClient *pulse_client;

  pulse_client = AGS_PULSE_CLIENT(gobject);
  
  switch(prop_id){
  case PROP_PULSE_SERVER:
    {
      g_value_set_object(value, pulse_client->pulse_server);
    }
    break;
  case PROP_DEVICE:
    {
      g_value_set_pointer(value,
			  g_list_copy(pulse_client->device));
    }
    break;
  case PROP_PORT:
    {
      g_value_set_pointer(value,
			  g_list_copy(pulse_client->port));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_client_connect(AgsConnectable *connectable)
{
  AgsPulseClient *pulse_client;

  GList *list;
  
  pulse_client = AGS_PULSE_CLIENT(connectable);

  if((AGS_PULSE_CLIENT_CONNECTED & (pulse_client->flags)) != 0){
    return;
  }

  pulse_client->flags |= AGS_PULSE_CLIENT_CONNECTED;

  /* port */
  list = pulse_client->port;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }
}

void
ags_pulse_client_disconnect(AgsConnectable *connectable)
{
  AgsPulseClient *pulse_client;

  GList *list;
  
  pulse_client = AGS_PULSE_CLIENT(connectable);

  if((AGS_PULSE_CLIENT_CONNECTED & (pulse_client->flags)) == 0){
    return;
  }

  pulse_client->flags &= (~AGS_PULSE_CLIENT_CONNECTED);

  /* port */
  list = pulse_client->port;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }
}

void
ags_pulse_client_dispose(GObject *gobject)
{
  AgsPulseClient *pulse_client;

  GList *list;
  
  pulse_client = AGS_PULSE_CLIENT(gobject);

  /* pulse server */
  if(pulse_client->pulse_server != NULL){
    g_object_unref(pulse_client->pulse_server);

    pulse_client->pulse_server = NULL;
  }

  /* device */
  if(pulse_client->device != NULL){
    list = pulse_client->device;

    while(list != NULL){
      g_object_set(G_OBJECT(list->data),
		   "pulse-client", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(pulse_client->device,
		     g_object_unref);

    pulse_client->device = NULL;
  }

  /* port */
  if(pulse_client->port != NULL){
    list = pulse_client->port;

    while(list != NULL){
      g_object_run_dispose(G_OBJECT(list->data));

      list = list->next;
    }
    
    g_list_free_full(pulse_client->port,
		     g_object_unref);

    pulse_client->port = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_pulse_client_parent_class)->dispose(gobject);
}

void
ags_pulse_client_finalize(GObject *gobject)
{
  AgsPulseClient *pulse_client;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;

  pulse_client = AGS_PULSE_CLIENT(gobject);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* remove pulse server mutex */
  pthread_mutex_lock(application_mutex);  

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(application_mutex);

  /* pulse server */
  if(pulse_client->pulse_server != NULL){
    g_object_unref(pulse_client->pulse_server);
  }

  /* device */
  if(pulse_client->device != NULL){
    g_list_free_full(pulse_client->device,
		     g_object_unref);
  }

  /* port */
  if(pulse_client->port != NULL){
    g_list_free_full(pulse_client->port,
		     g_object_unref);
  }

  pthread_mutex_destroy(pulse_client->mutex);
  free(pulse_client->mutex);

  pthread_mutexattr_destroy(pulse_client->mutexattr);
  free(pulse_client->mutexattr);
  
  /* call parent */
  G_OBJECT_CLASS(ags_pulse_client_parent_class)->finalize(gobject);
}

/**
 * ags_pulse_client_find_uuid:
 * @pulse_client: a #GList
 * @client_uuid: the client uuid to find
 *
 * Finds next match of @client_uuid in @pulse_client.
 *
 * Returns: a #GList or %NULL
 * 
 * Since: 1.0.0
 */
GList*
ags_pulse_client_find_uuid(GList *pulse_client,
			   gchar *client_uuid)
{
  while(pulse_client != NULL){
    if(AGS_PULSE_CLIENT(pulse_client->data)->context != NULL &&
       !g_ascii_strcasecmp(AGS_PULSE_CLIENT(pulse_client->data)->uuid,
			   client_uuid)){
      return(pulse_client);
    }
  }

  return(NULL);
}

/**
 * ags_pulse_client_find:
 * @pulse_client: a #GList
 * @client_name: the client name to find
 *
 * Finds next match of @client_name in @pulse_client.
 *
 * Returns: a #GList or %NULL
 * 
 * Since: 1.0.0
 */
GList*
ags_pulse_client_find(GList *pulse_client,
		      gchar *client_name)
{ 
  while(pulse_client != NULL){
    if(AGS_PULSE_CLIENT(pulse_client->data)->context != NULL &&
       !g_ascii_strcasecmp(AGS_PULSE_CLIENT(pulse_client->data)->name,
			   client_name)){
      return(pulse_client);
    }
  }

  return(NULL);
}

#ifdef AGS_WITH_PULSE
void
ags_pulse_client_state_callback(pa_context *c, AgsPulseClient *pulse_client)
{
  AgsMutexManager *mutex_manager;

  pa_context_state_t state;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  state = pa_context_get_state(c);

  /*  */  
  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) pulse_client);
  
  pthread_mutex_unlock(application_mutex);

  switch(state){
  case PA_CONTEXT_UNCONNECTED:
  case PA_CONTEXT_CONNECTING:
  case PA_CONTEXT_AUTHORIZING:
  case PA_CONTEXT_SETTING_NAME:
    break;
  case PA_CONTEXT_FAILED:
  case PA_CONTEXT_TERMINATED:
    g_warning("pulseaudio not running");
    break;
  case PA_CONTEXT_READY:
    pthread_mutex_lock(mutex);
    
    pulse_client->flags |= AGS_PULSE_CLIENT_READY;

    pthread_mutex_unlock(mutex);

    break;
  }
}
#endif

/**
 * ags_pulse_client_open:
 * @pulse_client: the #AgsPulseClient
 * @client_name: the client's name
 *
 * Open the pulseaudio client's connection and read uuid.
 *
 * Since: 1.0.0
 */
void
ags_pulse_client_open(AgsPulseClient *pulse_client,
		      gchar *client_name)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  gboolean ready;
  
  if(pulse_client == NULL ||
     client_name == NULL){
    return;
  }

  if(pulse_client->context != NULL){
    g_message("Advanced Gtk+ Sequencer pulseaudio client already open");
    
    return;
  } 

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /*  */  
  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) pulse_client);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  g_message("Advanced Gtk+ Sequencer open pulseaudio client");
  
  pulse_client->name = g_strdup(client_name);

#ifdef AGS_WITH_PULSE
  pulse_client->context = pa_context_new(AGS_PULSE_SERVER(pulse_client->pulse_server)->main_loop_api, client_name);
#else
  pulse_client->context = NULL;
#endif

  if(pulse_client->context != NULL){
#ifdef AGS_WITH_PULSE
    pa_context_connect(pulse_client->context,
		       NULL,
		       0,
		       NULL);
    pa_context_set_state_callback(pulse_client->context,
				  ags_pulse_client_state_callback,
				  pulse_client);
    ready = FALSE;
    
    while(!ready){
      pthread_mutex_lock(mutex);
      
      ready = (((AGS_PULSE_CLIENT_READY & (pulse_client->flags)) != 0) ? TRUE: FALSE);
      
      pthread_mutex_unlock(mutex);

      if(!ready){
	pa_mainloop_iterate(AGS_PULSE_SERVER(pulse_client->pulse_server)->main_loop,
			    TRUE,
			    NULL);
      }
    }
#endif
  }
}

/**
 * ags_pulse_client_activate:
 * @pulse_client: the #AgsPulseClient
 *
 * Activate client.
 *
 * Since: 1.0.0
 */
void
ags_pulse_client_activate(AgsPulseClient *pulse_client)
{
  AgsMutexManager *mutex_manager;

  GList *port;
  
  int ret;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /*  */  
  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) pulse_client);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  //TODO:JK: make thread-safe
  pthread_mutex_lock(mutex);
  
  if((AGS_PULSE_CLIENT_ACTIVATED & (pulse_client->flags)) != 0 ||
     pulse_client->context == NULL){
    pthread_mutex_unlock(mutex);
    
    return;
  }
  
  port = pulse_client->port;

  while(port != NULL){
    ags_pulse_port_register(port->data,
			    AGS_PULSE_PORT(port->data)->name,
			    (((AGS_PULSE_PORT_IS_AUDIO & (AGS_PULSE_PORT(port->data)->flags)) != 0) ? TRUE: FALSE), (((AGS_PULSE_PORT_IS_MIDI & (AGS_PULSE_PORT(port->data)->flags)) != 0) ? TRUE: FALSE),
			    (((AGS_PULSE_PORT_IS_OUTPUT & (AGS_PULSE_PORT(port->data)->flags)) != 0) ? TRUE: FALSE));
    
    port = port->next;
  }

  pulse_client->flags |= AGS_PULSE_CLIENT_ACTIVATED;

  pthread_mutex_unlock(mutex);
}

/**
 * ags_pulse_client_deactivate:
 * @pulse_client: the #AgsPulseClient
 *
 * Deactivate client.
 *
 * Since: 1.0.0
 */
void
ags_pulse_client_deactivate(AgsPulseClient *pulse_client)
{
  if(pulse_client->context == NULL){
    return;
  }

  pa_context_disconnect(pulse_client->context);
  
  pulse_client->flags &= (~AGS_PULSE_CLIENT_ACTIVATED);
}

/**
 * ags_pulse_client_add_device:
 * @pulse_client: the #AgsPulseClient
 * @pulse_device: an #AgsPulseDevout or #AgsPulseDevin
 *
 * Add @pulse_device to @pulse_client.
 *
 * Since: 1.0.0
 */
void
ags_pulse_client_add_device(AgsPulseClient *pulse_client,
			    GObject *pulse_device)
{
  if(!AGS_IS_PULSE_CLIENT(pulse_client) ||
     (!AGS_IS_PULSE_DEVOUT(pulse_device) &&
      !AGS_IS_PULSE_DEVIN(pulse_device))){
    return;
  }

  g_object_ref(pulse_device);
  pulse_client->device = g_list_prepend(pulse_client->device,
					pulse_device);
}

/**
 * ags_pulse_client_remove_device:
 * @pulse_client: the #AgsPulseClient
 * @pulse_device: an #AgsPulseDevout or #AgsPulseDevin
 *
 * Remove @pulse_device from @pulse_client.
 *
 * Since: 1.0.0
 */
void
ags_pulse_client_remove_device(AgsPulseClient *pulse_client,
			       GObject *pulse_device)
{
  if(!AGS_IS_PULSE_CLIENT(pulse_client)){
    return;
  }
  
  pulse_client->device = g_list_remove(pulse_client->device,
				       pulse_device);
  g_object_unref(pulse_device);
}

/**
 * ags_pulse_client_add_port:
 * @pulse_client: the #AgsPulseClient
 * @pulse_port: an #AgsPulsePort
 *
 * Add @pulse_port to @pulse_client.
 *
 * Since: 1.0.0
 */
void
ags_pulse_client_add_port(AgsPulseClient *pulse_client,
			  GObject *pulse_port)
{
  if(!AGS_IS_PULSE_CLIENT(pulse_client) ||
     !AGS_IS_PULSE_PORT(pulse_port)){
    return;
  }

  g_object_ref(pulse_port);
  pulse_client->port = g_list_prepend(pulse_client->port,
				      pulse_port);
}

/**
 * ags_pulse_client_remove_port:
 * @pulse_client: the #AgsPulseClient
 * @pulse_port: an #AgsPulsePort
 *
 * Remove @pulse_port from @pulse_client.
 *
 * Since: 1.0.0
 */
void
ags_pulse_client_remove_port(AgsPulseClient *pulse_client,
			     GObject *pulse_port)
{
  if(!AGS_IS_PULSE_CLIENT(pulse_client)){
    return;
  }
  
  pulse_client->port = g_list_remove(pulse_client->port,
				     pulse_port);
  g_object_unref(pulse_port);
}

/**
 * ags_pulse_client_new:
 * @pulse_server: the assigned #AgsPulseServer
 *
 * Instantiate a new #AgsPulseClient.
 *
 * Returns: the new #AgsPulseClient
 *
 * Since: 1.0.0
 */
AgsPulseClient*
ags_pulse_client_new(GObject *pulse_server)
{
  AgsPulseClient *pulse_client;

  pulse_client = (AgsPulseClient *) g_object_new(AGS_TYPE_PULSE_CLIENT,
						 "pulse-server", pulse_server,
						 NULL);

  return(pulse_client);
}
