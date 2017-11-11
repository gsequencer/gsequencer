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

#include <ags/audio/jack/ags_jack_client.h>

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

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_port.h>
#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_devin.h>
#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/audio/thread/ags_audio_loop.h>

#ifdef AGS_WITH_JACK
#include <jack/midiport.h>
#include <jack/weakmacros.h>
#include <jack/types.h>
#endif

#include <ags/i18n.h>

void ags_jack_client_class_init(AgsJackClientClass *jack_client);
void ags_jack_client_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_jack_client_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager);
void ags_jack_client_init(AgsJackClient *jack_client);
void ags_jack_client_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_jack_client_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_jack_client_connect(AgsConnectable *connectable);
void ags_jack_client_disconnect(AgsConnectable *connectable);
void ags_jack_client_dispose(GObject *gobject);
void ags_jack_client_finalize(GObject *gobject);

#ifdef AGS_WITH_JACK
void ags_jack_client_shutdown(void *arg);
int ags_jack_client_process_callback(jack_nframes_t nframes, void *ptr);
int ags_jack_client_xrun_callback(void *ptr);
#endif

/**
 * SECTION:ags_jack_client
 * @short_description: JACK connection
 * @title: AgsJackClient
 * @section_id:
 * @include: ags/audio/jack/ags_jack_client.h
 *
 * The #AgsJackClient communicates with a JACK instance.
 */

enum{
  PROP_0,
  PROP_JACK_SERVER,
  PROP_DEVICE,
  PROP_PORT,
};

static gpointer ags_jack_client_parent_class = NULL;

GType
ags_jack_client_get_type()
{
  static GType ags_type_jack_client = 0;

  if(!ags_type_jack_client){
    static const GTypeInfo ags_jack_client_info = {
      sizeof (AgsJackClientClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_client_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsJackClient),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_jack_client_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_jack_client_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_jack_client = g_type_register_static(G_TYPE_OBJECT,
						  "AgsJackClient",
						  &ags_jack_client_info,
						  0);

    g_type_add_interface_static(ags_type_jack_client,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_jack_client);
}

void
ags_jack_client_class_init(AgsJackClientClass *jack_client)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_jack_client_parent_class = g_type_class_peek_parent(jack_client);

  /* GObjectClass */
  gobject = (GObjectClass *) jack_client;

  gobject->set_property = ags_jack_client_set_property;
  gobject->get_property = ags_jack_client_get_property;

  gobject->dispose = ags_jack_client_dispose;
  gobject->finalize = ags_jack_client_finalize;

  /* properties */
  /**
   * AgsJackClient:jack-server:
   *
   * The assigned #AgsJackServer.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("jack-server",
				   i18n_pspec("assigned JACK server"),
				   i18n_pspec("The assigned JACK server"),
				   AGS_TYPE_JACK_SERVER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_JACK_SERVER,
				  param_spec);

  /**
   * AgsJackClient:device:
   *
   * The assigned devices.
   * 
   * Since: 1.0.0.7
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
   * AgsJackClient:port:
   *
   * The assigned ports.
   * 
   * Since: 1.0.0.7
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
ags_jack_client_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_jack_client_connect;
  connectable->disconnect = ags_jack_client_disconnect;
}

void
ags_jack_client_init(AgsJackClient *jack_client)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert client mutex */
  jack_client->mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  jack_client->mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) jack_client,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  jack_client->flags = 0;

  /* server */
  jack_client->jack_server = NULL;
  
  jack_client->uuid = NULL;
  jack_client->name = NULL;

  /* client */
  jack_client->client = NULL;

  /* device */
  jack_client->device = NULL;
  jack_client->port = NULL;

  g_atomic_int_set(&(jack_client->queued),
		   0);
}

void
ags_jack_client_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackClient *jack_client;

  jack_client = AGS_JACK_CLIENT(gobject);

  switch(prop_id){
  case PROP_JACK_SERVER:
    {
      AgsJackServer *jack_server;

      jack_server = (AgsJackServer *) g_value_get_object(value);

      if(jack_client->jack_server == (GObject *) jack_server){
	return;
      }

      if(jack_client->jack_server != NULL){
	g_object_unref(jack_client->jack_server);
      }

      if(jack_server != NULL){
	g_object_ref(jack_server);
      }
      
      jack_client->jack_server = (GObject *) jack_server;
    }
    break;
  case PROP_DEVICE:
    {
      GObject *device;

      device = (GObject *) g_value_get_object(value);

      if(g_list_find(jack_client->device,
		     device) != NULL){
	return;
      }

      if(device != NULL){
	g_object_ref(device);
	
	jack_client->device = g_list_prepend(jack_client->device,
					     device);
      }
    }
    break;
  case PROP_PORT:
    {
      GObject *port;

      port = (GObject *) g_value_get_object(value);

      if(g_list_find(jack_client->port,
		     port) != NULL){
	return;
      }

      if(port != NULL){
	g_object_ref(port);
	
	jack_client->port = g_list_prepend(jack_client->port,
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
ags_jack_client_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackClient *jack_client;

  jack_client = AGS_JACK_CLIENT(gobject);
  
  switch(prop_id){
  case PROP_JACK_SERVER:
    {
      g_value_set_object(value, jack_client->jack_server);
    }
    break;
  case PROP_DEVICE:
    {
      g_value_set_pointer(value,
			  g_list_copy(jack_client->device));
    }
    break;
  case PROP_PORT:
    {
      g_value_set_pointer(value,
			  g_list_copy(jack_client->port));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_client_connect(AgsConnectable *connectable)
{
  AgsJackClient *jack_client;

  GList *list;
  
  jack_client = AGS_JACK_CLIENT(connectable);

  if((AGS_JACK_CLIENT_CONNECTED & (jack_client->flags)) != 0){
    return;
  }

  jack_client->flags |= AGS_JACK_CLIENT_CONNECTED;

  /* port */
  list = jack_client->port;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }
}

void
ags_jack_client_disconnect(AgsConnectable *connectable)
{
  AgsJackClient *jack_client;

  GList *list;
  
  jack_client = AGS_JACK_CLIENT(connectable);

  if((AGS_JACK_CLIENT_CONNECTED & (jack_client->flags)) == 0){
    return;
  }

  jack_client->flags &= (~AGS_JACK_CLIENT_CONNECTED);

  /* port */
  list = jack_client->port;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }
}

void
ags_jack_client_dispose(GObject *gobject)
{
  AgsJackClient *jack_client;

  GList *list;
  
  jack_client = AGS_JACK_CLIENT(gobject);

  /* jack server */
  if(jack_client->jack_server != NULL){
    g_object_unref(jack_client->jack_server);

    jack_client->jack_server = NULL;
  }

  /* device */
  if(jack_client->device != NULL){
    list = jack_client->device;

    while(list != NULL){
      g_object_set(G_OBJECT(list->data),
		   "jack-client", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(jack_client->device,
		     g_object_unref);

    jack_client->device = NULL;
  }

  /* port */
  if(jack_client->port != NULL){
    list = jack_client->port;

    while(list != NULL){
      g_object_run_dispose(G_OBJECT(list->data));

      list = list->next;
    }
    
    g_list_free_full(jack_client->port,
		     g_object_unref);

    jack_client->port = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_jack_client_parent_class)->dispose(gobject);
}

void
ags_jack_client_finalize(GObject *gobject)
{
  AgsJackClient *jack_client;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;

  jack_client = AGS_JACK_CLIENT(gobject);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* remove jack client mutex */
  pthread_mutex_lock(application_mutex);  

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(application_mutex);

  /* jack server */
  if(jack_client->jack_server != NULL){
    g_object_unref(jack_client->jack_server);
  }

  /* device */
  if(jack_client->device != NULL){
    g_list_free_full(jack_client->device,
		     g_object_unref);
  }

  /* port */
  if(jack_client->port != NULL){
    g_list_free_full(jack_client->port,
		     g_object_unref);
  }
  
  pthread_mutex_destroy(jack_client->mutex);
  free(jack_client->mutex);

  pthread_mutexattr_destroy(jack_client->mutexattr);
  free(jack_client->mutexattr);

  /* call parent */
  G_OBJECT_CLASS(ags_jack_client_parent_class)->finalize(gobject);
}

/**
 * ags_jack_client_find_uuid:
 * @jack_client: a #GList
 * @client_uuid: the client uuid to find
 *
 * Finds next match of @client_uuid in @jack_client.
 *
 * Returns: a #GList or %NULL
 * 
 * Since: 1.0.0
 */
GList*
ags_jack_client_find_uuid(GList *jack_client,
			  gchar *client_uuid)
{
#ifdef AGS_WITH_JACK
  while(jack_client != NULL){
    if(AGS_JACK_CLIENT(jack_client->data)->client != NULL &&
       !g_ascii_strcasecmp(jack_get_uuid_for_client_name(AGS_JACK_CLIENT(jack_client->data)->client,
							 jack_get_client_name(AGS_JACK_CLIENT(jack_client->data)->client)),
			   client_uuid)){
      return(jack_client);
    }
  }
#endif

  return(NULL);
}

/**
 * ags_jack_client_find:
 * @jack_client: a #GList
 * @client_name: the client name to find
 *
 * Finds next match of @client_name in @jack_client.
 *
 * Returns: a #GList or %NULL
 * 
 * Since: 1.0.0
 */
GList*
ags_jack_client_find(GList *jack_client,
		     gchar *client_name)
{ 
#ifdef AGS_WITH_JACK
  while(jack_client != NULL){
    if(AGS_JACK_CLIENT(jack_client->data)->client != NULL &&
       !g_ascii_strcasecmp(jack_get_client_name(AGS_JACK_CLIENT(jack_client->data)->client),
			   client_name)){
      return(jack_client);
    }
  }
#endif

  return(NULL);
}

/**
 * ags_jack_client_open:
 * @jack_client: the #AgsJackClient
 * @client_name: the client's name
 *
 * Open the JACK client's connection and read uuid.
 *
 * Since: 1.0.0
 */
void
ags_jack_client_open(AgsJackClient *jack_client,
		     gchar *client_name)
{
  if(jack_client == NULL ||
     client_name == NULL){
    return;
  }

  if(jack_client->client != NULL){
    g_message("Advanced Gtk+ Sequencer JACK client already open");
    
    return;
  } 
  
  g_message("Advanced Gtk+ Sequencer open JACK client");
  
  jack_client->name = g_strdup(client_name);

#ifdef AGS_WITH_JACK
  jack_client->client = jack_client_open(jack_client->name,
					 0,
					 NULL,
					 NULL);
  
  if(jack_client->client != NULL){
    jack_client->uuid = jack_get_uuid_for_client_name(jack_client->client,
						      jack_client->name);

    jack_on_shutdown(jack_client->client,
		     ags_jack_client_shutdown,
		     jack_client);
  
    jack_set_process_callback(jack_client->client,
			      ags_jack_client_process_callback,
			      jack_client);
    jack_set_xrun_callback(jack_client->client,
			   ags_jack_client_xrun_callback,
			   jack_client);
  }
#endif
}

/**
 * ags_jack_client_activate:
 * @jack_client: the #AgsJackClient
 *
 * Activate client.
 *
 * Since: 1.0.0
 */
void
ags_jack_client_activate(AgsJackClient *jack_client)
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
				   (GObject *) jack_client);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  //TODO:JK: make thread-safe
  pthread_mutex_lock(mutex);
  
  if((AGS_JACK_CLIENT_ACTIVATED & (jack_client->flags)) != 0 ||
     jack_client->client == NULL){
    pthread_mutex_unlock(mutex);
    
    return;
  }
  
#ifdef AGS_WITH_JACK
  ret = jack_activate(jack_client->client);
#else
  ret = -1;
#endif

  if(ret == 0){
    jack_client->flags |= AGS_JACK_CLIENT_ACTIVATED;
  }else{
    pthread_mutex_unlock(mutex);
    
    return;
  }

  port = jack_client->port;

  while(port != NULL){
    ags_jack_port_register(port->data,
			   AGS_JACK_PORT(port->data)->name,
			   (((AGS_JACK_PORT_IS_AUDIO & (AGS_JACK_PORT(port->data)->flags)) != 0) ? TRUE: FALSE), (((AGS_JACK_PORT_IS_MIDI & (AGS_JACK_PORT(port->data)->flags)) != 0) ? TRUE: FALSE),
			   (((AGS_JACK_PORT_IS_OUTPUT & (AGS_JACK_PORT(port->data)->flags)) != 0) ? TRUE: FALSE));
    
    port = port->next;
  }

  pthread_mutex_unlock(mutex);
}

/**
 * ags_jack_client_deactivate:
 * @jack_client: the #AgsJackClient
 *
 * Deactivate client.
 *
 * Since: 1.0.0
 */
void
ags_jack_client_deactivate(AgsJackClient *jack_client)
{
  if(jack_client->client == NULL){
    return;
  }
  
#ifdef AGS_WITH_JACK
  jack_deactivate(jack_client->client);

  jack_client->flags &= (~AGS_JACK_CLIENT_ACTIVATED);
#endif
}

/**
 * ags_jack_client_add_device:
 * @jack_client: the #AgsJackClient
 * @jack_device: an #AgsJackDevout, #AgsJackDevin or #AgsJackMidiin
 *
 * Add @jack_device to @jack_client.
 *
 * Since: 1.0.0
 */
void
ags_jack_client_add_device(AgsJackClient *jack_client,
			   GObject *jack_device)
{
  if(!AGS_IS_JACK_CLIENT(jack_client) ||
     (!AGS_IS_JACK_DEVOUT(jack_device) &&
      !AGS_IS_JACK_MIDIIN(jack_device) &&
      !AGS_IS_JACK_DEVIN(jack_device))){
    return;
  }

  g_object_ref(jack_device);
  jack_client->device = g_list_prepend(jack_client->device,
				     jack_device);
}

/**
 * ags_jack_client_remove_device:
 * @jack_client: the #AgsJackClient
 * @jack_device: an #AgsJackDevout, #AgsJackDevin or #AgsJackMidiin
 *
 * Remove @jack_device from @jack_client.
 *
 * Since: 1.0.0
 */
void
ags_jack_client_remove_device(AgsJackClient *jack_client,
			      GObject *jack_device)
{
  if(!AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }
  
  jack_client->device = g_list_remove(jack_client->device,
				      jack_device);
  g_object_unref(jack_device);
}

/**
 * ags_jack_client_add_port:
 * @jack_client: the #AgsJackClient
 * @jack_port: an #AgsJackPort
 *
 * Add @jack_port to @jack_client.
 *
 * Since: 1.0.0
 */
void
ags_jack_client_add_port(AgsJackClient *jack_client,
			 GObject *jack_port)
{
  if(!AGS_IS_JACK_CLIENT(jack_client) ||
     !AGS_IS_JACK_PORT(jack_port)){
    return;
  }

  g_object_ref(jack_port);
  jack_client->port = g_list_prepend(jack_client->port,
				     jack_port);
}

/**
 * ags_jack_client_remove_port:
 * @jack_client: the #AgsJackClient
 * @jack_port: an #AgsJackPort
 *
 * Remove @jack_port from @jack_client.
 *
 * Since: 1.0.0
 */
void
ags_jack_client_remove_port(AgsJackClient *jack_client,
			    GObject *jack_port)
{
  if(!AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }
  
  jack_client->port = g_list_remove(jack_client->port,
				    jack_port);
  g_object_unref(jack_port);
}

#ifdef AGS_WITH_JACK
void
ags_jack_client_shutdown(void *ptr)
{
  AgsJackClient *jack_client;

  AgsMutexManager *mutex_manager;

  GList *port;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /*  */  
  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) ptr);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  //TODO:JK: make thread-safe
  pthread_mutex_lock(mutex);
  
  jack_client = ptr;
  
  jack_client->flags &= (~AGS_JACK_CLIENT_ACTIVATED);

  port = jack_client->port;

  while(port != NULL){
    AGS_JACK_PORT(port->data)->flags &= (~AGS_JACK_PORT_REGISTERED);
    
    port = port->next;
  }

  pthread_mutex_unlock(mutex);
}

int
ags_jack_client_process_callback(jack_nframes_t nframes, void *ptr)
{
  AgsJackClient *jack_client;
  AgsJackPort *jack_port;
  AgsJackDevout *jack_devout;
  AgsJackDevout *jack_devin;
  AgsJackMidiin *jack_midiin;
  
  AgsAudioLoop *audio_loop;

  AgsMutexManager *mutex_manager;
  AgsTaskThread *task_thread;
  
  AgsApplicationContext *application_context;

  jack_client_t *client;
  jack_default_audio_sample_t *out, *in;
  jack_midi_event_t in_event;

  GList *device, *device_start, *port;

  void *port_buf;

  jack_nframes_t event_count;
  guint time_spent;
  guint copy_mode;
  guint word_size;
  guint event_size;
  guint i, j;
  guint nth_buffer;
  gboolean no_event;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutex_t *device_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;
  
  if(ptr == NULL){
    return(0);
  }
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /*  */  
  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) ptr);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  pthread_mutex_lock(mutex);
  
  jack_client = AGS_JACK_CLIENT(ptr);
  
  if(jack_client->jack_server != NULL){
    application_context = (AgsApplicationContext *) AGS_JACK_SERVER(jack_client->jack_server)->application_context;
  }else{
    application_context = NULL;
  }

  device_start = jack_client->device;
  
  pthread_mutex_unlock(mutex);

  if(g_atomic_int_get(&(jack_client->queued)) > 0){
    g_warning("drop JACK callback");
    
    return(0);
  }else{
    g_atomic_int_inc(&(jack_client->queued));
  }

  /*
   * process audio
   */
  /*  */  
  pthread_mutex_lock(application_mutex);

  if(application_context != NULL){
    audio_loop = (AgsAudioLoop *) application_context->main_loop;
    task_thread = (AgsTaskThread *) application_context->task_thread;
  }else{
    audio_loop = NULL;
    task_thread = NULL;
  }
  
  pthread_mutex_unlock(application_mutex);

  /* interrupt GUI */
  if(task_thread != NULL){
    pthread_mutex_lock(task_thread->launch_mutex);
  }
  
  if(audio_loop != NULL){
    pthread_mutex_lock(audio_loop->timing_mutex);
  
    g_atomic_int_set(&(audio_loop->time_spent),
		     audio_loop->time_cycle);
  
    pthread_mutex_unlock(audio_loop->timing_mutex);
  
    //    ags_main_loop_interrupt(AGS_MAIN_LOOP(audio_loop),
    //			    AGS_THREAD_SUSPEND_SIG,
    //			    0, &time_spent);
  }

  if(task_thread != NULL){
    pthread_mutex_unlock(task_thread->launch_mutex);
  }

  device = device_start;
  
  if(device == NULL){
    g_atomic_int_dec_and_test(&(jack_client->queued));
    
    return(0);
  }

  g_atomic_int_and(&(AGS_THREAD(audio_loop)->flags),
		   (~(AGS_THREAD_TIMING)));

  /*
   * process MIDI input
   */
  /* get device */
  device = device_start;  

  while(device != NULL){
    /*  */  
    pthread_mutex_lock(application_mutex);
  
    device_mutex = ags_mutex_manager_lookup(mutex_manager,
					    (GObject *) device->data);
  
    pthread_mutex_unlock(application_mutex);

    pthread_mutex_lock(device_mutex);

    jack_devin = NULL;
    jack_midiin = NULL;

    if(AGS_IS_JACK_DEVIN(device->data)){
      jack_devin = (AgsJackDevin *) device->data;

      /* wait callback */      
      no_event = TRUE;
      
      if((AGS_JACK_DEVIN_PASS_THROUGH & (g_atomic_int_get(&(jack_devin->sync_flags)))) == 0){
	callback_mutex = jack_devin->callback_mutex;

	pthread_mutex_unlock(device_mutex);
	
	/* give back computing time until ready */
	pthread_mutex_lock(callback_mutex);
    
	if((AGS_JACK_DEVIN_CALLBACK_DONE & (g_atomic_int_get(&(jack_devin->sync_flags)))) == 0){
	  g_atomic_int_or(&(jack_devin->sync_flags),
			  AGS_JACK_DEVIN_CALLBACK_WAIT);
    
	  while((AGS_JACK_DEVIN_CALLBACK_DONE & (g_atomic_int_get(&(jack_devin->sync_flags)))) == 0 &&
		(AGS_JACK_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(jack_devin->sync_flags)))) != 0){
	    pthread_cond_wait(jack_devin->callback_cond,
			      callback_mutex);
	  }
	}
    
	g_atomic_int_and(&(jack_devin->sync_flags),
			 (~(AGS_JACK_DEVIN_CALLBACK_WAIT |
			    AGS_JACK_DEVIN_CALLBACK_DONE)));
	  
	pthread_mutex_unlock(callback_mutex);

	no_event = FALSE;
	
	pthread_mutex_lock(device_mutex);
      }      
    }else if(AGS_IS_JACK_MIDIIN(device->data)){
      jack_midiin = (AgsJackMidiin *) device->data;

      /* wait callback */      
      no_event = TRUE;
      
      if((AGS_JACK_MIDIIN_PASS_THROUGH & (g_atomic_int_get(&(jack_midiin->sync_flags)))) == 0){
	callback_mutex = jack_midiin->callback_mutex;

	pthread_mutex_unlock(device_mutex);
	
	/* give back computing time until ready */
	pthread_mutex_lock(callback_mutex);
    
	if((AGS_JACK_MIDIIN_CALLBACK_DONE & (g_atomic_int_get(&(jack_midiin->sync_flags)))) == 0){
	  g_atomic_int_or(&(jack_midiin->sync_flags),
			  AGS_JACK_MIDIIN_CALLBACK_WAIT);
    
	  while((AGS_JACK_MIDIIN_CALLBACK_DONE & (g_atomic_int_get(&(jack_midiin->sync_flags)))) == 0 &&
		(AGS_JACK_MIDIIN_CALLBACK_WAIT & (g_atomic_int_get(&(jack_midiin->sync_flags)))) != 0){
	    pthread_cond_wait(jack_midiin->callback_cond,
			      callback_mutex);
	  }
	}
    
	g_atomic_int_and(&(jack_midiin->sync_flags),
			 (~(AGS_JACK_MIDIIN_CALLBACK_WAIT |
			    AGS_JACK_MIDIIN_CALLBACK_DONE)));
	  
	pthread_mutex_unlock(callback_mutex);

	no_event = FALSE;
	
	pthread_mutex_lock(device_mutex);
      }

      /* audio input */
      if(jack_devin != NULL){      
	/* get buffer */
	if((AGS_JACK_DEVIN_BUFFER0 & (jack_devin->flags)) != 0){
	  nth_buffer = 3;
	}else if((AGS_JACK_DEVIN_BUFFER1 & (jack_devin->flags)) != 0){
	  nth_buffer = 0;
	}else if((AGS_JACK_DEVIN_BUFFER2 & (jack_devin->flags)) != 0){
	  nth_buffer = 1;
	}else if((AGS_JACK_DEVIN_BUFFER3 & jack_devin->flags) != 0){
	  nth_buffer = 2;
	}else{
	  /* iterate */
	  device = device->next;

	  pthread_mutex_unlock(device_mutex);
	
	  continue;
	}

	/* get copy mode */
	copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(jack_devin->format),
							AGS_AUDIO_BUFFER_UTIL_FLOAT);

	/* check buffer flag */
	switch(jack_devin->format){
	case AGS_SOUNDCARD_SIGNED_8_BIT:
	  {
	    word_size = sizeof(signed char);
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_16_BIT:
	  {
	    word_size = sizeof(signed short);
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_24_BIT:
	  {
	    word_size = sizeof(signed long);
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_32_BIT:
	  {
	    word_size = sizeof(signed long);
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_64_BIT:
	  {
	    word_size = sizeof(signed long long);
	  }
	  break;
	default:
	  /* iterate */
	  device = device->next;

	  pthread_mutex_unlock(device_mutex);
	
	  continue;
	}

	/* retrieve buffer */
	port = jack_devin->jack_port;
      
	for(i = 0; port != NULL; i++){
	  jack_port = port->data;
	
	  in = jack_port_get_buffer(jack_port->port,
				    jack_devin->buffer_size);
	
	  if(!no_event && in != NULL){
	    ags_audio_buffer_util_copy_buffer_to_buffer(jack_devin->buffer[nth_buffer], jack_devin->pcm_channels, i,
							in, 1, 0,
							jack_devin->buffer_size, copy_mode);
	  }

	  port = port->next;
	}

	/* clear buffer */
	port = jack_devin->jack_port;
      
	for(i = 0; port != NULL; i++){
	  jack_port = port->data;
	
	  in = jack_port_get_buffer(jack_port->port,
				    jack_devin->buffer_size);

	  if(in != NULL){
	    ags_audio_buffer_util_clear_float(in, 1,
					      jack_devin->buffer_size);
	  }

	  port = port->next;
	}
	  
	if(!no_event){
	  /* signal finish */
	  callback_finish_mutex = jack_devin->callback_finish_mutex;
	
	  pthread_mutex_lock(callback_finish_mutex);

	  g_atomic_int_or(&(jack_devin->sync_flags),
			  AGS_JACK_DEVIN_CALLBACK_FINISH_DONE);
    
	  if((AGS_JACK_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_devin->sync_flags)))) != 0){
	    pthread_cond_signal(jack_devin->callback_finish_cond);
	  }

	  pthread_mutex_unlock(callback_finish_mutex);
	}
      }
      
      /* MIDI input */
      if(jack_midiin != NULL){
	if(!no_event){
	  port = jack_midiin->jack_port;

	  for(i = 0; port != NULL; i++){
	    jack_port = port->data;

	    port_buf = jack_port_get_buffer(jack_port->port,
					    4096);
	    event_count = jack_midi_get_event_count(port_buf);
		
	    for(j = 0; j < event_count; j++){
	      jack_midi_event_get(&in_event, port_buf, j);

	      if(in_event.size > 0){
		if((AGS_JACK_MIDIIN_BUFFER0 & (jack_midiin->flags)) != 0){
		  nth_buffer = 1;
		}else if((AGS_JACK_MIDIIN_BUFFER1 & (jack_midiin->flags)) != 0){
		  nth_buffer = 2;
		}else if((AGS_JACK_MIDIIN_BUFFER2 & (jack_midiin->flags)) != 0){
		  nth_buffer = 3;
		}else if((AGS_JACK_MIDIIN_BUFFER3 & jack_midiin->flags) != 0){
		  nth_buffer = 0;
		}

		if(ceil((jack_midiin->buffer_size[nth_buffer] + in_event.size) / 4096.0) > ceil(jack_midiin->buffer_size[nth_buffer] / 4096.0)){
		  if(jack_midiin->buffer[nth_buffer] == NULL){
		    jack_midiin->buffer[nth_buffer] = malloc(4096 * sizeof(char));
		  }else{
		    jack_midiin->buffer[nth_buffer] = realloc(jack_midiin->buffer[nth_buffer],
							      (ceil(jack_midiin->buffer_size[nth_buffer] / 4096.0) * 4096 + 4096) * sizeof(char));
		  }
		}

		memcpy(&(jack_midiin->buffer[nth_buffer][jack_midiin->buffer_size[nth_buffer]]),
		       in_event.buffer,
		       in_event.size);
		jack_midiin->buffer_size[nth_buffer] += in_event.size;
	      }

	    }	  

	    jack_midi_clear_buffer(port_buf);
	
	    port = port->next;
	  }

	  /* signal finish */
	  callback_finish_mutex = jack_midiin->callback_finish_mutex;
	
	  pthread_mutex_lock(callback_finish_mutex);

	  g_atomic_int_or(&(jack_midiin->sync_flags),
			  AGS_JACK_MIDIIN_CALLBACK_FINISH_DONE);
    
	  if((AGS_JACK_MIDIIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_midiin->sync_flags)))) != 0){
	    pthread_cond_signal(jack_midiin->callback_finish_cond);
	  }

	  pthread_mutex_unlock(callback_finish_mutex);
	}
      }
    }
        
    pthread_mutex_unlock(device_mutex);

    /* iterate */
    pthread_mutex_lock(mutex);
    
    device = device->next;

    pthread_mutex_unlock(mutex);
  }
  
  /*
   * process audio output
   */
  /* get device */
  device = device_start;  

  while(device != NULL){
    /*  */  
    pthread_mutex_lock(application_mutex);
  
    device_mutex = ags_mutex_manager_lookup(mutex_manager,
					    (GObject *) device->data);
  
    pthread_mutex_unlock(application_mutex);

    pthread_mutex_lock(device_mutex);

    jack_devout = NULL;
    
    if(AGS_IS_JACK_DEVOUT(device->data)){
      jack_devout = (AgsJackDevout *) device->data;

      /* wait callback */      
      no_event = TRUE;

      if((AGS_JACK_DEVOUT_PASS_THROUGH & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0){
	callback_mutex = jack_devout->callback_mutex;

	pthread_mutex_unlock(device_mutex);
	
	/* give back computing time until ready */
	pthread_mutex_lock(callback_mutex);
    
	if((AGS_JACK_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0){
	  g_atomic_int_or(&(jack_devout->sync_flags),
			  AGS_JACK_DEVOUT_CALLBACK_WAIT);
    
	  while((AGS_JACK_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0 &&
		(AGS_JACK_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(jack_devout->sync_flags)))) != 0){
	    pthread_cond_wait(jack_devout->callback_cond,
			      callback_mutex);
	  }
	}
    
	g_atomic_int_and(&(jack_devout->sync_flags),
			 (~(AGS_JACK_DEVOUT_CALLBACK_WAIT |
			    AGS_JACK_DEVOUT_CALLBACK_DONE)));
    
	pthread_mutex_unlock(callback_mutex);

	no_event = FALSE;

	pthread_mutex_lock(device_mutex);
      }

      /* clear buffer */
      port = jack_devout->jack_port;
      
      for(i = 0; port != NULL; i++){
	jack_port = port->data;
	
	out = jack_port_get_buffer(jack_port->port,
				   jack_devout->buffer_size);

	if(out != NULL){
	  ags_audio_buffer_util_clear_float(out, 1,
					    jack_devout->buffer_size);
	}

	port = port->next;
      }
      
      /* get buffer */
      if((AGS_JACK_DEVOUT_BUFFER0 & (jack_devout->flags)) != 0){
	nth_buffer = 3;
      }else if((AGS_JACK_DEVOUT_BUFFER1 & (jack_devout->flags)) != 0){
	nth_buffer = 0;
      }else if((AGS_JACK_DEVOUT_BUFFER2 & (jack_devout->flags)) != 0){
	nth_buffer = 1;
      }else if((AGS_JACK_DEVOUT_BUFFER3 & jack_devout->flags) != 0){
	nth_buffer = 2;
      }else{
	/* iterate */
	device = device->next;

	pthread_mutex_unlock(device_mutex);
	
	continue;
      }

      /* get copy mode */
      copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						      ags_audio_buffer_util_format_from_soundcard(jack_devout->format));

      /* check buffer flag */
      switch(jack_devout->format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  word_size = sizeof(signed char);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  word_size = sizeof(signed short);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
	{
	  word_size = sizeof(signed long);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
	{
	  word_size = sizeof(signed long);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_64_BIT:
	{
	  word_size = sizeof(signed long long);
	}
	break;
      default:
	/* iterate */
	device = device->next;

	pthread_mutex_unlock(device_mutex);
	
	continue;
      }

      /* fill buffer */
      port = jack_devout->jack_port;
      
      for(i = 0; port != NULL; i++){
	jack_port = port->data;
	
	out = jack_port_get_buffer(jack_port->port,
				   jack_devout->buffer_size);
	
	if(!no_event && out != NULL){
	  ags_audio_buffer_util_copy_buffer_to_buffer(out, 1, 0,
						      jack_devout->buffer[nth_buffer], jack_devout->pcm_channels, i,
						      jack_devout->buffer_size, copy_mode);
	}

	port = port->next;
      }
    
      if(!no_event){
	/* signal finish */
	callback_finish_mutex = jack_devout->callback_finish_mutex;
	
	pthread_mutex_lock(callback_finish_mutex);

	g_atomic_int_or(&(jack_devout->sync_flags),
			AGS_JACK_DEVOUT_CALLBACK_FINISH_DONE);
    
	if((AGS_JACK_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_devout->sync_flags)))) != 0){
	  pthread_cond_signal(jack_devout->callback_finish_cond);
	}

	pthread_mutex_unlock(callback_finish_mutex);
      }
    }
        
    pthread_mutex_unlock(device_mutex);

    /* iterate */
    pthread_mutex_lock(mutex);
    
    device = device->next;

    pthread_mutex_unlock(mutex);
  }
  
  g_atomic_int_dec_and_test(&(jack_client->queued));

  return(0);
}

int
ags_jack_client_xrun_callback(void *ptr)
{
  AgsJackClient *jack_client;

  if(ptr == NULL){
    return(0);
  }

  jack_client = (AgsJackClient *) ptr;
  
  return(0);
}
#endif

/**
 * ags_jack_client_new:
 * @jack_server: the assigned #AgsJackServer
 *
 * Instantiate a new #AgsJackClient.
 *
 * Returns: the new #AgsJackClient
 *
 * Since: 1.0.0
 */
AgsJackClient*
ags_jack_client_new(GObject *jack_server)
{
  AgsJackClient *jack_client;

  jack_client = (AgsJackClient *) g_object_new(AGS_TYPE_JACK_CLIENT,
					       "jack-server", jack_server,
					       NULL);

  return(jack_client);
}
