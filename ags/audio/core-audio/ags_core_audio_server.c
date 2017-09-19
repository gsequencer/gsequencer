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

#include <ags/audio/core-audio/ags_core_audio_server.h>
#include <ags/audio/core-audio/ags_core_audio_client.h>
#include <ags/audio/core-audio/ags_core_audio_port.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_thread-posix.h>

#include <ags/audio/core-audio/ags_core_audio_devout.h>
#include <ags/audio/core-audio/ags_core_audio_midiin.h>

#include <string.h>

#include <errno.h>

#include <ags/i18n.h>

void ags_core_audio_server_class_init(AgsCoreAudioServerClass *core_audio_server);
void ags_core_audio_server_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_core_audio_server_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager);
void ags_core_audio_server_init(AgsCoreAudioServer *core_audio_server);
void ags_core_audio_server_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_server_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_server_connect(AgsConnectable *connectable);
void ags_core_audio_server_disconnect(AgsConnectable *connectable);
void ags_core_audio_server_dispose(GObject *gobject);
void ags_core_audio_server_finalize(GObject *gobject);

void ags_core_audio_server_set_url(AgsDistributedManager *distributed_manager,
				   gchar *url);
gchar* ags_core_audio_server_get_url(AgsDistributedManager *distributed_manager);
void ags_core_audio_server_set_ports(AgsDistributedManager *distributed_manager,
				     guint *ports, guint port_count);
guint* ags_core_audio_server_get_ports(AgsDistributedManager *distributed_manager,
				       guint *port_count);
void ags_core_audio_server_set_soundcard(AgsDistributedManager *distributed_manager,
					 gchar *client_uuid,
					 GList *soundcard);
GList* ags_core_audio_server_get_soundcard(AgsDistributedManager *distributed_manager,
					   gchar *client_uuid);
void ags_core_audio_server_set_sequencer(AgsDistributedManager *distributed_manager,
					 gchar *client_uuid,
					 GList *sequencer);
GList* ags_core_audio_server_get_sequencer(AgsDistributedManager *distributed_manager,
					   gchar *client_uuid);
GObject* ags_core_audio_server_register_soundcard(AgsDistributedManager *distributed_manager,
						  gboolean is_output);
void ags_core_audio_server_unregister_soundcard(AgsDistributedManager *distributed_manager,
						GObject *soundcard);
GObject* ags_core_audio_server_register_sequencer(AgsDistributedManager *distributed_manager,
						  gboolean is_output);
void ags_core_audio_server_unregister_sequencer(AgsDistributedManager *distributed_manager,
						GObject *sequencer);

void* ags_core_audio_server_do_poll_loop(void *ptr);

/**
 * SECTION:ags_core_audio_server
 * @short_description: core audio instance
 * @title: AgsCoreAudioServer
 * @section_id:
 * @include: ags/audio/core-audio/ags_core_audio_server.h
 *
 * The #AgsCoreAudioServer is an object to represent a running core audio instance.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_URL,
  PROP_DEFAULT_SOUNDCARD,
  PROP_DEFAULT_CORE_AUDIO_CLIENT,
  PROP_CORE_AUDIO_CLIENT,
};

static gpointer ags_core_audio_server_parent_class = NULL;

GType
ags_core_audio_server_get_type()
{
  static GType ags_type_core_audio_server = 0;

  if(!ags_type_core_audio_server){
    static const GTypeInfo ags_core_audio_server_info = {
      sizeof (AgsCoreAudioServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_core_audio_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCoreAudioServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_core_audio_server_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_server_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_distributed_manager_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_server_distributed_manager_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_core_audio_server = g_type_register_static(G_TYPE_OBJECT,
							"AgsCoreAudioServer",
							&ags_core_audio_server_info,
							0);

    g_type_add_interface_static(ags_type_core_audio_server,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_core_audio_server,
				AGS_TYPE_DISTRIBUTED_MANAGER,
				&ags_distributed_manager_interface_info);
  }

  return (ags_type_core_audio_server);
}

void
ags_core_audio_server_class_init(AgsCoreAudioServerClass *core_audio_server)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_core_audio_server_parent_class = g_type_class_peek_parent(core_audio_server);

  /* GObjectClass */
  gobject = (GObjectClass *) core_audio_server;

  gobject->set_property = ags_core_audio_server_set_property;
  gobject->get_property = ags_core_audio_server_get_property;

  gobject->dispose = ags_core_audio_server_dispose;
  gobject->finalize = ags_core_audio_server_finalize;

  /* properties */
  /**
   * AgsCoreAudioServer:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 0.9.24
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
   * AgsCoreAudioServer:url:
   *
   * The assigned URL.
   * 
   * Since: 0.9.24
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
   * AgsCoreAudioServer:default-soundcard:
   *
   * The default soundcard.
   * 
   * Since: 0.9.24
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
   * AgsCoreAudioServer:default-core-audio-client:
   *
   * The default core audio client.
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_object("default-core-audio-client",
				   i18n_pspec("default core audio client"),
				   i18n_pspec("The default core audio client"),
				   AGS_TYPE_CORE_AUDIO_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_CORE_AUDIO_CLIENT,
				  param_spec);

  /**
   * AgsCoreAudioServer:core-audio-client:
   *
   * The core audio client list.
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_object("core-audio-client",
				   i18n_pspec("core audio client list"),
				   i18n_pspec("The core audio client list"),
				   AGS_TYPE_CORE_AUDIO_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CORE_AUDIO_CLIENT,
				  param_spec);
}

void
ags_core_audio_server_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_core_audio_server_connect;
  connectable->disconnect = ags_core_audio_server_disconnect;
}

void
ags_core_audio_server_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager)
{
  distributed_manager->set_url = ags_core_audio_server_set_url;
  distributed_manager->get_url = ags_core_audio_server_get_url;
  distributed_manager->set_ports = ags_core_audio_server_set_ports;
  distributed_manager->get_ports = ags_core_audio_server_get_ports;
  distributed_manager->set_soundcard = ags_core_audio_server_set_soundcard;
  distributed_manager->get_soundcard = ags_core_audio_server_get_soundcard;
  distributed_manager->set_sequencer = ags_core_audio_server_set_sequencer;
  distributed_manager->get_sequencer = ags_core_audio_server_get_sequencer;
  distributed_manager->register_soundcard = ags_core_audio_server_register_soundcard;
  distributed_manager->unregister_soundcard = ags_core_audio_server_unregister_soundcard;
  distributed_manager->register_sequencer = ags_core_audio_server_register_sequencer;
  distributed_manager->unregister_sequencer = ags_core_audio_server_unregister_sequencer;
}

void
ags_core_audio_server_init(AgsCoreAudioServer *core_audio_server)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert server mutex */
  core_audio_server->mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  core_audio_server->mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) core_audio_server,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  core_audio_server->flags = 0;
  
  g_atomic_int_set(&(core_audio_server->running),
		   TRUE);
  core_audio_server->thread = (pthread_t *) malloc(sizeof(pthread_t));
  
  core_audio_server->application_context = NULL;

  core_audio_server->url = NULL;
  
  core_audio_server->port = NULL;
  core_audio_server->port_count = 0;
  
  core_audio_server->n_soundcards = 0;
  core_audio_server->n_sequencers = 0;

  core_audio_server->default_soundcard = NULL;

  core_audio_server->default_client = NULL;
  core_audio_server->client = NULL;
}

void
ags_core_audio_server_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsCoreAudioServer *core_audio_server;

  core_audio_server = AGS_CORE_AUDIO_SERVER(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if(core_audio_server->application_context == (GObject *) application_context){
	return;
      }

      if(core_audio_server->application_context != NULL){
	g_object_unref(G_OBJECT(core_audio_server->application_context));
      }

      if(application_context != NULL){
	g_object_ref(G_OBJECT(application_context));
      }

      core_audio_server->application_context = (GObject *) application_context;
    }
    break;
  case PROP_URL:
    {
      gchar *url;

      url = g_value_get_string(value);

      if(core_audio_server->url == url){
	return;
      }

      if(core_audio_server->url != NULL){
	g_free(core_audio_server->url);
      }

      core_audio_server->url = g_strdup(url);
    }
    break;
  case PROP_DEFAULT_SOUNDCARD:
    {
      GObject *default_soundcard;

      default_soundcard = (GObject *) g_value_get_object(value);

      if(core_audio_server->default_soundcard == (GObject *) default_soundcard){
	return;
      }

      if(core_audio_server->default_soundcard != NULL){
	g_object_unref(G_OBJECT(core_audio_server->default_soundcard));
      }

      if(default_soundcard != NULL){
	g_object_ref(G_OBJECT(default_soundcard));
      }

      core_audio_server->default_soundcard = (GObject *) default_soundcard;
    }
    break;
  case PROP_DEFAULT_CORE_AUDIO_CLIENT:
    {
      AgsCoreAudioClient *default_client;

      default_client = (AgsCoreAudioClient *) g_value_get_object(value);

      if(core_audio_server->default_client == (GObject *) default_client){
	return;
      }

      if(core_audio_server->default_client != NULL){
	g_object_unref(G_OBJECT(core_audio_server->default_client));
      }

      if(default_client != NULL){
	g_object_ref(G_OBJECT(default_client));
      }

      core_audio_server->default_client = (GObject *) default_client;
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      GObject *client;

      client = (GObject *) g_value_get_object(value);

      if(g_list_find(core_audio_server->client, client) != NULL){
	return;
      }

      if(client != NULL){
	g_object_ref(G_OBJECT(client));

	core_audio_server->client = g_list_prepend(core_audio_server->client,
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
ags_core_audio_server_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsCoreAudioServer *core_audio_server;

  core_audio_server = AGS_CORE_AUDIO_SERVER(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, core_audio_server->application_context);
    }
    break;
  case PROP_URL:
    {
      g_value_set_string(value, core_audio_server->url);
    }
    break;
  case PROP_DEFAULT_SOUNDCARD:
    {
      g_value_set_object(value, core_audio_server->default_soundcard);
    }
    break;
  case PROP_DEFAULT_CORE_AUDIO_CLIENT:
    {
      g_value_set_object(value, core_audio_server->default_soundcard);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      g_value_set_pointer(value,
			  g_list_copy(core_audio_server->client));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_server_connect(AgsConnectable *connectable)
{
  AgsCoreAudioServer *core_audio_server;

  GList *list;
  
  core_audio_server = AGS_CORE_AUDIO_SERVER(connectable);

  if((AGS_CORE_AUDIO_SERVER_CONNECTED & (core_audio_server->flags)) != 0){
    return;
  }

  core_audio_server->flags |= AGS_CORE_AUDIO_SERVER_CONNECTED;

  list = core_audio_server->client;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_core_audio_server_disconnect(AgsConnectable *connectable)
{
  AgsCoreAudioServer *core_audio_server;

  GList *list;
  
  core_audio_server = AGS_CORE_AUDIO_SERVER(connectable);

  if((AGS_CORE_AUDIO_SERVER_CONNECTED & (core_audio_server->flags)) != 0){
    return;
  }

  core_audio_server->flags |= AGS_CORE_AUDIO_SERVER_CONNECTED;

  list = core_audio_server->client;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_core_audio_server_dispose(GObject *gobject)
{
  AgsCoreAudioServer *core_audio_server;

  GList *list;
  
  core_audio_server = AGS_CORE_AUDIO_SERVER(gobject);

  /* application context */
  if(core_audio_server->application_context != NULL){
    g_object_unref(G_OBJECT(core_audio_server->application_context));
    
    core_audio_server->application_context = NULL;
  }

  /* default soundcard */
  if(core_audio_server->default_soundcard != NULL){
    g_object_unref(G_OBJECT(core_audio_server->default_soundcard));

    core_audio_server->default_soundcard = NULL;
  }
  
  /* default client */
  if(core_audio_server->default_client != NULL){
    g_object_unref(G_OBJECT(core_audio_server->default_client));

    core_audio_server->default_client = NULL;
  }
  
  /* client */
  if(core_audio_server->client != NULL){
    list = core_audio_server->client;

    while(list != NULL){
      g_object_run_dispose(G_OBJECT(list->data));

      list = list->next;
    }
    
    g_list_free_full(core_audio_server->client,
		     g_object_unref);

    core_audio_server->client = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_server_parent_class)->dispose(gobject);
}

void
ags_core_audio_server_finalize(GObject *gobject)
{
  AgsCoreAudioServer *core_audio_server;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;

  core_audio_server = AGS_CORE_AUDIO_SERVER(gobject);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* remove core audio server mutex */
  pthread_mutex_lock(application_mutex);  

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(application_mutex);
  
  /* application context */
  if(core_audio_server->application_context != NULL){
    g_object_unref(G_OBJECT(core_audio_server->application_context));
  }

  /* url */
  g_free(core_audio_server->url);

  /* default soundcard */
  if(core_audio_server->default_soundcard != NULL){
    g_object_unref(G_OBJECT(core_audio_server->default_soundcard));
  }
  
  /* default client */
  if(core_audio_server->default_client != NULL){
    g_object_unref(G_OBJECT(core_audio_server->default_client));
  }
  
  /* client */
  if(core_audio_server->client != NULL){
    g_list_free_full(core_audio_server->client,
		     g_object_unref);
  }

  pthread_mutex_destroy(core_audio_server->mutex);
  free(core_audio_server->mutex);

  pthread_mutexattr_destroy(core_audio_server->mutexattr);
  free(core_audio_server->mutexattr);
  
  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_server_parent_class)->finalize(gobject);
}

void
ags_core_audio_server_set_url(AgsDistributedManager *distributed_manager,
			      gchar *url)
{
  AGS_CORE_AUDIO_SERVER(distributed_manager)->url = url;
}

gchar*
ags_core_audio_server_get_url(AgsDistributedManager *distributed_manager)
{
  return(AGS_CORE_AUDIO_SERVER(distributed_manager)->url);
}

void
ags_core_audio_server_set_ports(AgsDistributedManager *distributed_manager,
				guint *port, guint port_count)
{
  AGS_CORE_AUDIO_SERVER(distributed_manager)->port = port;
  AGS_CORE_AUDIO_SERVER(distributed_manager)->port_count = port_count;
}

guint*
ags_core_audio_server_get_ports(AgsDistributedManager *distributed_manager,
				guint *port_count)
{
  if(port_count != NULL){
    *port_count = AGS_CORE_AUDIO_SERVER(distributed_manager)->port_count;
  }
  
  return(AGS_CORE_AUDIO_SERVER(distributed_manager)->port);
}

void
ags_core_audio_server_set_soundcard(AgsDistributedManager *distributed_manager,
				    gchar *client_uuid,
				    GList *soundcard)
{
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioClient *core_audio_client;

  GList *list;

  core_audio_server = AGS_CORE_AUDIO_SERVER(distributed_manager);

  core_audio_client = (AgsCoreAudioClient *) ags_core_audio_server_find_client(core_audio_server,
									       client_uuid);

  //NOTE:JK: soundcard won't removed
  list = soundcard;

  while(list != NULL){
    ags_core_audio_client_add_device(core_audio_client,
				     (GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_core_audio_server_get_soundcard(AgsDistributedManager *distributed_manager,
				    gchar *client_uuid)
{
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioClient *core_audio_client;

  GList *list, *device;
  
  core_audio_server = AGS_CORE_AUDIO_SERVER(distributed_manager);

  core_audio_client = (AgsCoreAudioClient *) ags_core_audio_server_find_client(core_audio_server,
									       client_uuid);

  device = core_audio_client->device;
  list = NULL;

  while(device != NULL){
    if(AGS_IS_CORE_AUDIO_DEVOUT(device->data)){
      list = g_list_prepend(list,
			    device->data);
    }

    device = device->next;
  }

  return(g_list_reverse(list));
}

void
ags_core_audio_server_set_sequencer(AgsDistributedManager *distributed_manager,
				    gchar *client_uuid,
				    GList *sequencer)
{
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioClient *core_audio_client;

  GList *list;

  core_audio_server = AGS_CORE_AUDIO_SERVER(distributed_manager);

  core_audio_client = (AgsCoreAudioClient *) ags_core_audio_server_find_client(core_audio_server,
									       client_uuid);

  //NOTE:JK: sequencer won't removed
  list = sequencer;

  while(list != NULL){
    ags_core_audio_client_add_device(core_audio_client,
				     (GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_core_audio_server_get_sequencer(AgsDistributedManager *distributed_manager,
				    gchar *client_uuid)
{
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioClient *core_audio_client;

  GList *list, *device;
  
  core_audio_server = AGS_CORE_AUDIO_SERVER(distributed_manager);

  core_audio_client = (AgsCoreAudioClient *) ags_core_audio_server_find_client(core_audio_server,
									       client_uuid);

  device = core_audio_client->device;
  list = NULL;

  while(device != NULL){
    if(AGS_IS_CORE_AUDIO_MIDIIN(device->data)){
      list = g_list_prepend(list,
			    device->data);
    }

    device = device->next;
  }

  return(g_list_reverse(list));
}

GObject*
ags_core_audio_server_register_soundcard(AgsDistributedManager *distributed_manager,
					 gboolean is_output)
{
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioClient *default_client;
  AgsCoreAudioPort *core_audio_port;
  AgsCoreAudioDevout *core_audio_devout;

  gchar *str;  

  gboolean initial_set;
  guint i;
  
  if(!is_output){
    g_warning("GSequencer - audio input not implemented");
    return(NULL);
  }

  core_audio_server = AGS_CORE_AUDIO_SERVER(distributed_manager);
  initial_set = FALSE;
  
  /* the default client */
  if(core_audio_server->default_client == NULL){
    g_object_set(core_audio_server,
		 "default-core-audio-client", ags_core_audio_client_new((GObject *) core_audio_server),
		 NULL);
    ags_core_audio_server_add_client(core_audio_server,
				     core_audio_server->default_client);
    
    ags_core_audio_client_open((AgsCoreAudioClient *) core_audio_server->default_client,
			       "ags-default-client");
    initial_set = TRUE;
    
    if(AGS_CORE_AUDIO_CLIENT(core_audio_server->default_client)->graph == NULL){
      g_warning("ags_core_audio_server.c - can't open core audio client");
    }
  }

  default_client = (AgsCoreAudioClient *) core_audio_server->default_client;

  core_audio_devout = NULL;

  /* the soundcard */
  if(is_output){
    core_audio_devout = ags_core_audio_devout_new(core_audio_server->application_context);
    str = g_strdup_printf("ags-core-audio-devout-%d",
			  core_audio_server->n_soundcards);
    g_object_set(AGS_CORE_AUDIO_DEVOUT(core_audio_devout),
		 "core-audio-client", default_client,
		 "device", str,
		 NULL);
    g_free(str);
    g_object_set(default_client,
		 "device", core_audio_devout,
		 NULL);
        
    /* register ports */
    str = g_strdup_printf("ags-soundcard%d",
			  core_audio_server->n_soundcards);
    
#ifdef AGS_DEBUG
    g_message("%s", str);
#endif
      
    core_audio_port = ags_core_audio_port_new((GObject *) default_client);
    g_object_set(core_audio_port,
		 "core-audio-device", core_audio_devout,
		 NULL);
    ags_core_audio_client_add_port(default_client,
				   (GObject *) core_audio_port);

    g_object_set(core_audio_devout,
		 "core-audio-port", core_audio_port,
		 NULL);
    
    core_audio_devout->port_name = (gchar **) malloc(2 * sizeof(gchar *));
    core_audio_devout->port_name[0] = g_strdup(str);
    core_audio_devout->port_name[1] = NULL;
    
    ags_core_audio_port_register(core_audio_port,
				 str,
				 TRUE, FALSE,
				 TRUE);

    ags_core_audio_devout_realloc_buffer(core_audio_devout);
    core_audio_server->n_soundcards += 1;
  }
  
  return((GObject *) core_audio_devout);
}

void
ags_core_audio_server_unregister_soundcard(AgsDistributedManager *distributed_manager,
					   GObject *soundcard)
{
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioClient *default_client;

  GList *list;

  core_audio_server = AGS_CORE_AUDIO_SERVER(distributed_manager);
  
  /* the default client */
  default_client = (AgsCoreAudioClient *) core_audio_server->default_client;

  if(default_client == NULL){
    g_warning("GSequencer - no core audio client");
    
    return;
  }
  
  list = AGS_CORE_AUDIO_DEVOUT(soundcard)->core_audio_port;

  while(list != NULL){
    ags_core_audio_port_unregister(list->data);
    ags_core_audio_client_remove_port(default_client,
				      list->data);
    
    list = list->next;
  }

  ags_core_audio_client_remove_device(default_client,
				      soundcard);
  
  if(default_client->port == NULL){
    core_audio_server->n_soundcards = 0;
  }
}

GObject*
ags_core_audio_server_register_sequencer(AgsDistributedManager *distributed_manager,
					 gboolean is_output)
{
  g_message("GSequencer - can't register core audio sequencer");
  
  return(NULL);
}

void
ags_core_audio_server_unregister_sequencer(AgsDistributedManager *distributed_manager,
					   GObject *sequencer)
{
  g_message("GSequencer - can't unregister core audio sequencer");
}

GObject*
ags_core_audio_server_register_default_soundcard(AgsCoreAudioServer *core_audio_server)
{
  AgsCoreAudioClient *default_client;
  AgsCoreAudioDevout *core_audio_devout;
  AgsCoreAudioPort *core_audio_port;

  gchar *str;
  
  guint i;
  
  /* the default client */
  if(core_audio_server->default_client == NULL){
    g_object_set(core_audio_server,
		 "default-core-audio-client", (GObject *) ags_core_audio_client_new((GObject *) core_audio_server),
		 NULL);
    ags_core_audio_server_add_client(core_audio_server,
				     core_audio_server->default_client);
    
    ags_core_audio_client_open((AgsCoreAudioClient *) core_audio_server->default_client,
			       "ags-default-client");

    if(AGS_CORE_AUDIO_CLIENT(core_audio_server->default_client)->graph == NULL){
      g_warning("ags_core_audio_server.c - can't open core audio client");
      
      return(NULL);
    }
  }

  default_client = (AgsCoreAudioClient *) core_audio_server->default_client;

  /* the soundcard */
  core_audio_devout = ags_core_audio_devout_new(core_audio_server->application_context);
  g_object_set(AGS_CORE_AUDIO_DEVOUT(core_audio_devout),
	       "core-audio-client", default_client,
	       "device", "ags-default-devout",
	       NULL);
  g_object_set(default_client,
	       "device", core_audio_devout,
	       NULL);
  
  /* register ports */
  str = g_strdup_printf("ags-default-soundcard");

#ifdef AGS_DEBUG
  g_message("%s", str);
#endif
    
  core_audio_port = ags_core_audio_port_new((GObject *) default_client);
  g_object_set(core_audio_port,
	       "core-audio-device", core_audio_devout,
	       NULL);
  ags_core_audio_client_add_port(default_client,
				 (GObject *) core_audio_port);

  g_object_set(core_audio_devout,
	       "core-audio-port", core_audio_port,
	       NULL);
  
  core_audio_devout->port_name = (gchar **) malloc(2 * sizeof(gchar *));
  core_audio_devout->port_name[0] = g_strdup(str);
  core_audio_devout->port_name[1] = NULL;
  
  ags_core_audio_port_register(core_audio_port,
			       str,
			       TRUE, FALSE,
			       TRUE);

  g_free(str);
  
  return((GObject *) core_audio_devout);
}

/**
 * ags_core_audio_server_find_url:
 * @core_audio_server: the #AgsCoreAudioServer
 * @url: the url to find
 *
 * Find #AgsCoreAudioServer by url.
 *
 * Returns: the #GList containing a #AgsCoreAudioServer matching @url or %NULL
 *
 * Since: 0.9.24
 */
GList*
ags_core_audio_server_find_url(GList *core_audio_server,
			       gchar *url)
{
  while(core_audio_server != NULL){
    if(!g_ascii_strcasecmp(AGS_CORE_AUDIO_SERVER(core_audio_server->data)->url,
			   url)){
      return(core_audio_server);
    }

    core_audio_server = core_audio_server->next;
  }

  return(NULL);
}

/**
 * ags_core_audio_server_find_client:
 * @core_audio_server: the #AgsCoreAudioServer
 * @client_uuid: the uuid to find
 *
 * Find #AgsCoreAudioClient by uuid.
 *
 * Returns: the #AgsCoreAudioClient found or %NULL
 *
 * Since: 0.9.24
 */
GObject*
ags_core_audio_server_find_client(AgsCoreAudioServer *core_audio_server,
				  gchar *client_uuid)
{
  GList *list;

  list = core_audio_server->client;
  
  while(list != NULL){
    if(!g_ascii_strcasecmp(AGS_CORE_AUDIO_CLIENT(list->data)->uuid,
			   client_uuid)){
      return(list->data);
    }

    list = list->next;
  }
  
  return(NULL);
}

/**
 * ags_core_audio_server_find_port:
 * @core_audio_server: the #AgsCoreAudioServer
 * @port_uuid: the uuid to find
 *
 * Find #AgsCoreAudioPort by uuid.
 *
 * Returns: the #AgsCoreAudioPort found or %NULL
 *
 * Since: 0.9.24
 */
GObject*
ags_core_audio_server_find_port(AgsCoreAudioServer *core_audio_server,
				gchar *port_uuid)
{
  GList *client, *port;

  client = core_audio_server->client;
  
  while(client != NULL){
    port = AGS_CORE_AUDIO_CLIENT(client->data)->port;

    while(port != NULL){
      if(!g_ascii_strcasecmp(AGS_CORE_AUDIO_CLIENT(port->data)->uuid,
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
 * ags_core_audio_server_add_client:
 * @core_audio_server: the #AgsCoreAudioServer
 * @core_audio_client: the #AgsCoreAudioClient to add
 *
 * Add @core_audio_client to @core_audio_server
 *
 * Since: 0.9.24
 */
void
ags_core_audio_server_add_client(AgsCoreAudioServer *core_audio_server,
				 GObject *core_audio_client)
{
  if(!AGS_IS_CORE_AUDIO_SERVER(core_audio_server) ||
     !AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return;
  }

  g_object_ref(core_audio_client);
  core_audio_server->client = g_list_prepend(core_audio_server->client,
					     core_audio_client);
}

/**
 * ags_core_audio_server_remove_client:
 * @core_audio_server: the #AgsCoreAudioServer
 * @core_audio_client: the #AgsCoreAudioClient to remove
 *
 * Remove @core_audio_client to @core_audio_server
 *
 * Since: 0.9.24
 */
void
ags_core_audio_server_remove_client(AgsCoreAudioServer *core_audio_server,
				    GObject *core_audio_client)
{
  if(!AGS_IS_CORE_AUDIO_SERVER(core_audio_server) ||
     !AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return;
  }

  core_audio_server->client = g_list_remove(core_audio_server->client,
					    core_audio_client);
  g_object_unref(core_audio_client);
}

/**
 * ags_core_audio_server_connect_client:
 * @core_audio_server: the #AgsCoreAudioServer
 *
 * Connect all clients.
 *
 * Since: 0.9.24
 */
void
ags_core_audio_server_connect_client(AgsCoreAudioServer *core_audio_server)
{
  GList *client;

  client = core_audio_server->client;

  while(client != NULL){
    ags_core_audio_client_open((AgsCoreAudioClient *) client->data,
			       AGS_CORE_AUDIO_CLIENT(client->data)->name);
    ags_core_audio_client_activate(client->data);

    client = client->next;
  }
}

/**
 * ags_core_audio_server_new:
 * @application_context: the #AgsApplicationContext
 * @url: the URL as string
 *
 * Instantiate a new #AgsCoreAudioServer.
 *
 * Returns: the new #AgsCoreAudioServer
 *
 * Since: 0.9.24
 */
AgsCoreAudioServer*
ags_core_audio_server_new(GObject *application_context,
			  gchar *url)
{
  AgsCoreAudioServer *core_audio_server;

  core_audio_server = (AgsCoreAudioServer *) g_object_new(AGS_TYPE_CORE_AUDIO_SERVER,
							  "application-context", application_context,
							  "url", url,
							  NULL);

  return(core_audio_server);
}
