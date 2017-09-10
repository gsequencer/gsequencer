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

#include <ags/audio/core-audio/ags_core_audio_client.h>

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

#include <ags/audio/core-audio/ags_core_audio_server.h>
#include <ags/audio/core-audio/ags_core_audio_port.h>
#include <ags/audio/core-audio/ags_core_audio_devout.h>

#include <ags/i18n.h>

void ags_core_audio_client_class_init(AgsCoreAudioClientClass *core_audio_client);
void ags_core_audio_client_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_core_audio_client_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager);
void ags_core_audio_client_init(AgsCoreAudioClient *core_audio_client);
void ags_core_audio_client_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_client_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_client_connect(AgsConnectable *connectable);
void ags_core_audio_client_disconnect(AgsConnectable *connectable);
void ags_core_audio_client_dispose(GObject *gobject);
void ags_core_audio_client_finalize(GObject *gobject);

/**
 * SECTION:ags_core_audio_client
 * @short_description: core audio connection
 * @title: AgsCoreAudioClient
 * @section_id:
 * @include: ags/audio/core-audio/ags_core_audio_client.h
 *
 * The #AgsCoreAudioClient communicates with a core audio instance.
 */

enum{
  PROP_0,
  PROP_CORE_AUDIO_SERVER,
  PROP_DEVICE,
  PROP_PORT,
};

static gpointer ags_core_audio_client_parent_class = NULL;

GType
ags_core_audio_client_get_type()
{
  static GType ags_type_core_audio_client = 0;

  if(!ags_type_core_audio_client){
    static const GTypeInfo ags_core_audio_client_info = {
      sizeof (AgsCoreAudioClientClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_core_audio_client_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCoreAudioClient),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_core_audio_client_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_client_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_core_audio_client = g_type_register_static(G_TYPE_OBJECT,
							"AgsCoreAudioClient",
							&ags_core_audio_client_info,
							0);

    g_type_add_interface_static(ags_type_core_audio_client,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_core_audio_client);
}

void
ags_core_audio_client_class_init(AgsCoreAudioClientClass *core_audio_client)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_core_audio_client_parent_class = g_type_class_peek_parent(core_audio_client);

  /* GObjectClass */
  gobject = (GObjectClass *) core_audio_client;

  gobject->set_property = ags_core_audio_client_set_property;
  gobject->get_property = ags_core_audio_client_get_property;

  gobject->dispose = ags_core_audio_client_dispose;
  gobject->finalize = ags_core_audio_client_finalize;

  /* properties */
  /**
   * AgsCoreAudioClient:core-audio-server:
   *
   * The assigned #AgsCoreAudioServer.
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_object("core-audio-server",
				   i18n_pspec("assigned core audio server"),
				   i18n_pspec("The assigned core audio server"),
				   AGS_TYPE_CORE_AUDIO_SERVER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CORE_AUDIO_SERVER,
				  param_spec);

  /**
   * AgsCoreAudioClient:device:
   *
   * The assigned devices.
   * 
   * Since: 0.9.24
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
   * AgsCoreAudioClient:port:
   *
   * The assigned ports.
   * 
   * Since: 0.9.24
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
ags_core_audio_client_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_core_audio_client_connect;
  connectable->disconnect = ags_core_audio_client_disconnect;
}

void
ags_core_audio_client_init(AgsCoreAudioClient *core_audio_client)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert client mutex */
  core_audio_client->mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  core_audio_client->mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) core_audio_client,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  core_audio_client->flags = 0;

  /* server */
  core_audio_client->core_audio_server = NULL;
  
  core_audio_client->uuid = ags_id_generator_create_uuid();
  core_audio_client->name = NULL;

  /* client */
  core_audio_client->graph = NULL;

  /* device */
  core_audio_client->device = NULL;
  core_audio_client->port = NULL;
}

void
ags_core_audio_client_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsCoreAudioClient *core_audio_client;

  core_audio_client = AGS_CORE_AUDIO_CLIENT(gobject);

  switch(prop_id){
  case PROP_CORE_AUDIO_SERVER:
    {
      AgsCoreAudioServer *core_audio_server;

      core_audio_server = (AgsCoreAudioServer *) g_value_get_object(value);

      if(core_audio_client->core_audio_server == (GObject *) core_audio_server){
	return;
      }

      if(core_audio_client->core_audio_server != NULL){
	g_object_unref(core_audio_client->core_audio_server);
      }

      if(core_audio_server != NULL){
	g_object_ref(core_audio_server);
      }
      
      core_audio_client->core_audio_server = (GObject *) core_audio_server;
    }
    break;
  case PROP_DEVICE:
    {
      GObject *device;

      device = (GObject *) g_value_get_object(value);

      if(g_list_find(core_audio_client->device,
		     device) != NULL){
	return;
      }

      if(device != NULL){
	g_object_ref(device);
	
	core_audio_client->device = g_list_prepend(core_audio_client->device,
						   device);
      }
    }
    break;
  case PROP_PORT:
    {
      GObject *port;

      port = (GObject *) g_value_get_object(value);

      if(g_list_find(core_audio_client->port,
		     port) != NULL){
	return;
      }

      if(port != NULL){
	g_object_ref(port);
	
	core_audio_client->port = g_list_prepend(core_audio_client->port,
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
ags_core_audio_client_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsCoreAudioClient *core_audio_client;

  core_audio_client = AGS_CORE_AUDIO_CLIENT(gobject);
  
  switch(prop_id){
  case PROP_CORE_AUDIO_SERVER:
    {
      g_value_set_object(value, core_audio_client->core_audio_server);
    }
    break;
  case PROP_DEVICE:
    {
      g_value_set_pointer(value,
			  g_list_copy(core_audio_client->device));
    }
    break;
  case PROP_PORT:
    {
      g_value_set_pointer(value,
			  g_list_copy(core_audio_client->port));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_client_connect(AgsConnectable *connectable)
{
  AgsCoreAudioClient *core_audio_client;

  GList *list;
  
  core_audio_client = AGS_CORE_AUDIO_CLIENT(connectable);

  if((AGS_CORE_AUDIO_CLIENT_CONNECTED & (core_audio_client->flags)) != 0){
    return;
  }

  core_audio_client->flags |= AGS_CORE_AUDIO_CLIENT_CONNECTED;

  /* port */
  list = core_audio_client->port;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }
}

void
ags_core_audio_client_disconnect(AgsConnectable *connectable)
{
  AgsCoreAudioClient *core_audio_client;

  GList *list;
  
  core_audio_client = AGS_CORE_AUDIO_CLIENT(connectable);

  if((AGS_CORE_AUDIO_CLIENT_CONNECTED & (core_audio_client->flags)) == 0){
    return;
  }

  core_audio_client->flags &= (~AGS_CORE_AUDIO_CLIENT_CONNECTED);

  /* port */
  list = core_audio_client->port;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }
}

void
ags_core_audio_client_dispose(GObject *gobject)
{
  AgsCoreAudioClient *core_audio_client;

  GList *list;
  
  core_audio_client = AGS_CORE_AUDIO_CLIENT(gobject);

  /* core audio server */
  if(core_audio_client->core_audio_server != NULL){
    g_object_unref(core_audio_client->core_audio_server);

    core_audio_client->core_audio_server = NULL;
  }

  /* device */
  if(core_audio_client->device != NULL){
    list = core_audio_client->device;

    while(list != NULL){
      g_object_set(G_OBJECT(list->data),
		   "core-audio-client", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(core_audio_client->device,
		     g_object_unref);

    core_audio_client->device = NULL;
  }

  /* port */
  if(core_audio_client->port != NULL){
    list = core_audio_client->port;

    while(list != NULL){
      g_object_run_dispose(G_OBJECT(list->data));

      list = list->next;
    }
    
    g_list_free_full(core_audio_client->port,
		     g_object_unref);

    core_audio_client->port = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_client_parent_class)->dispose(gobject);
}

void
ags_core_audio_client_finalize(GObject *gobject)
{
  AgsCoreAudioClient *core_audio_client;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;

  core_audio_client = AGS_CORE_AUDIO_CLIENT(gobject);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* remove core audio server mutex */
  pthread_mutex_lock(application_mutex);  

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(application_mutex);

  /* core audio server */
  if(core_audio_client->core_audio_server != NULL){
    g_object_unref(core_audio_client->core_audio_server);
  }

  /* core audio graph */
  if(core_audio_client->graph != NULL){
    free(core_audio_client->graph);
  }

  /* device */
  if(core_audio_client->device != NULL){
    g_list_free_full(core_audio_client->device,
		     g_object_unref);
  }

  /* port */
  if(core_audio_client->port != NULL){
    g_list_free_full(core_audio_client->port,
		     g_object_unref);
  }

  pthread_mutex_destroy(core_audio_client->mutex);
  free(core_audio_client->mutex);

  pthread_mutexattr_destroy(core_audio_client->mutexattr);
  free(core_audio_client->mutexattr);
  
  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_client_parent_class)->finalize(gobject);
}

/**
 * ags_core_audio_client_find_uuid:
 * @core_audio_client: a #GList
 * @client_uuid: the client uuid to find
 *
 * Finds next match of @client_uuid in @core_audio_client.
 *
 * Returns: a #GList or %NULL
 * 
 * Since: 0.9.24
 */
GList*
ags_core_audio_client_find_uuid(GList *core_audio_client,
				gchar *client_uuid)
{
  while(core_audio_client != NULL){
    if(AGS_CORE_AUDIO_CLIENT(core_audio_client->data)->graph != NULL &&
       !g_ascii_strcasecmp(AGS_CORE_AUDIO_CLIENT(core_audio_client->data)->uuid,
			   client_uuid)){
      return(core_audio_client);
    }
  }

  return(NULL);
}

/**
 * ags_core_audio_client_find:
 * @core_audio_client: a #GList
 * @client_name: the client name to find
 *
 * Finds next match of @client_name in @core_audio_client.
 *
 * Returns: a #GList or %NULL
 * 
 * Since: 0.9.24
 */
GList*
ags_core_audio_client_find(GList *core_audio_client,
			   gchar *client_name)
{ 
  while(core_audio_client != NULL){
    if(AGS_CORE_AUDIO_CLIENT(core_audio_client->data)->graph != NULL &&
       !g_ascii_strcasecmp(AGS_CORE_AUDIO_CLIENT(core_audio_client->data)->name,
			   client_name)){
      return(core_audio_client);
    }
  }

  return(NULL);
}

/**
 * ags_core_audio_client_open:
 * @core_audio_client: the #AgsCoreAudioClient
 * @client_name: the client's name
 *
 * Open the core audio client's connection and read uuid.
 *
 * Since: 0.9.24
 */
void
ags_core_audio_client_open(AgsCoreAudioClient *core_audio_client,
			   gchar *client_name)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;

#ifdef AGS_WITH_CORE_AUDIO
  OSStatus retval;
#endif
  
  gboolean ready;
  
  if(core_audio_client == NULL ||
     client_name == NULL){
    return;
  }

  if(core_audio_client->graph != NULL){
    g_message("Advanced Gtk+ Sequencer core audio client already open");
    
    return;
  } 

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /*  */  
  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) core_audio_client);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  g_message("Advanced Gtk+ Sequencer open core audio client");
  
  core_audio_client->name = g_strdup(client_name);

#ifdef AGS_WITH_CORE_AUDIO
  core_audio_client->graph = (AUGraph *) malloc(sizeof(AUGraph));
  memset(core_audio_client->graph, 0, sizeof(AUGraph));
  
  retval = NewAUGraph(core_audio_client->graph);

  if(retval != noErr){
    free(core_audio_client->graph);
    
    core_audio_client->graph = NULL;
  }
#endif
}

/**
 * ags_core_audio_client_activate:
 * @core_audio_client: the #AgsCoreAudioClient
 *
 * Activate client.
 *
 * Since: 0.9.24
 */
void
ags_core_audio_client_activate(AgsCoreAudioClient *core_audio_client)
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
				   (GObject *) core_audio_client);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  //TODO:JK: make thread-safe
  pthread_mutex_lock(mutex);
  
  if((AGS_CORE_AUDIO_CLIENT_ACTIVATED & (core_audio_client->flags)) != 0 ||
     core_audio_client->graph == NULL){
    pthread_mutex_unlock(mutex);
    
    return;
  }
  
  port = core_audio_client->port;

  while(port != NULL){
    ags_core_audio_port_register(port->data,
				 AGS_CORE_AUDIO_PORT(port->data)->name,
				 (((AGS_CORE_AUDIO_PORT_IS_AUDIO & (AGS_CORE_AUDIO_PORT(port->data)->flags)) != 0) ? TRUE: FALSE), (((AGS_CORE_AUDIO_PORT_IS_MIDI & (AGS_CORE_AUDIO_PORT(port->data)->flags)) != 0) ? TRUE: FALSE),
				 (((AGS_CORE_AUDIO_PORT_IS_OUTPUT & (AGS_CORE_AUDIO_PORT(port->data)->flags)) != 0) ? TRUE: FALSE));
    
    port = port->next;
  }

  //AUGraphOpen(core_audio_client->graph);
  //AUGraphStart(core_audio_client->graph);
  
  core_audio_client->flags |= AGS_CORE_AUDIO_CLIENT_ACTIVATED;

  pthread_mutex_unlock(mutex);
}

/**
 * ags_core_audio_client_deactivate:
 * @core_audio_client: the #AgsCoreAudioClient
 *
 * Deactivate client.
 *
 * Since: 0.9.24
 */
void
ags_core_audio_client_deactivate(AgsCoreAudioClient *core_audio_client)
{
  if(core_audio_client->graph == NULL){
    return;
  }

  //AUGraphStop(core_audio_client->graph);
  
  core_audio_client->flags &= (~AGS_CORE_AUDIO_CLIENT_ACTIVATED);
}

/**
 * ags_core_audio_client_add_device:
 * @core_audio_client: the #AgsCoreAudioClient
 * @core_audio_device: an #AgsCoreAudioDevout or #AgsCoreAudioMidiin
 *
 * Add @core_audio_device to @core_audio_client.
 *
 * Since: 0.9.24
 */
void
ags_core_audio_client_add_device(AgsCoreAudioClient *core_audio_client,
				 GObject *core_audio_device)
{
  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client) ||
     (!AGS_IS_CORE_AUDIO_DEVOUT(core_audio_device) &&
      !AGS_IS_CORE_AUDIO_MIDIIN(core_audio_device))){
    return;
  }

  g_object_ref(core_audio_device);
  core_audio_client->device = g_list_prepend(core_audio_client->device,
					     core_audio_device);
}

/**
 * ags_core_audio_client_remove_device:
 * @core_audio_client: the #AgsCoreAudioClient
 * @core_audio_device: an #AgsCoreAudioDevout or #AgsCoreAudioMidiin
 *
 * Remove @core_audio_device from @core_audio_client.
 *
 * Since: 0.9.24
 */
void
ags_core_audio_client_remove_device(AgsCoreAudioClient *core_audio_client,
				    GObject *core_audio_device)
{
  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return;
  }
  
  core_audio_client->device = g_list_remove(core_audio_client->device,
					    core_audio_device);
  g_object_unref(core_audio_device);
}

/**
 * ags_core_audio_client_add_port:
 * @core_audio_client: the #AgsCoreAudioClient
 * @core_audio_port: an #AgsCoreAudioPort
 *
 * Add @core_audio_port to @core_audio_client.
 *
 * Since: 0.9.24
 */
void
ags_core_audio_client_add_port(AgsCoreAudioClient *core_audio_client,
			       GObject *core_audio_port)
{
  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client) ||
     !AGS_IS_CORE_AUDIO_PORT(core_audio_port)){
    return;
  }

  g_object_ref(core_audio_port);
  core_audio_client->port = g_list_prepend(core_audio_client->port,
					   core_audio_port);
}

/**
 * ags_core_audio_client_remove_port:
 * @core_audio_client: the #AgsCoreAudioClient
 * @core_audio_port: an #AgsCoreAudioPort
 *
 * Remove @core_audio_port from @core_audio_client.
 *
 * Since: 0.9.24
 */
void
ags_core_audio_client_remove_port(AgsCoreAudioClient *core_audio_client,
				  GObject *core_audio_port)
{
  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return;
  }
  
  core_audio_client->port = g_list_remove(core_audio_client->port,
					  core_audio_port);
  g_object_unref(core_audio_port);
}

/**
 * ags_core_audio_client_new:
 * @core_audio_server: the assigned #AgsCoreAudioServer
 *
 * Instantiate a new #AgsCoreAudioClient.
 *
 * Returns: the new #AgsCoreAudioClient
 *
 * Since: 0.9.24
 */
AgsCoreAudioClient*
ags_core_audio_client_new(GObject *core_audio_server)
{
  AgsCoreAudioClient *core_audio_client;

  core_audio_client = (AgsCoreAudioClient *) g_object_new(AGS_TYPE_CORE_AUDIO_CLIENT,
							  "core-audio-server", core_audio_server,
							  NULL);

  return(core_audio_client);
}
