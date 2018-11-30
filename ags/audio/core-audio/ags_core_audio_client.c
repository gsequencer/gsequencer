/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/core-audio/ags_core_audio_server.h>
#include <ags/audio/core-audio/ags_core_audio_port.h>
#include <ags/audio/core-audio/ags_core_audio_devout.h>
#include <ags/audio/core-audio/ags_core_audio_devin.h>
#include <ags/audio/core-audio/ags_core_audio_midiin.h>

#include <ags/i18n.h>

void ags_core_audio_client_class_init(AgsCoreAudioClientClass *core_audio_client);
void ags_core_audio_client_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_core_audio_client_init(AgsCoreAudioClient *core_audio_client);
void ags_core_audio_client_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_client_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_client_dispose(GObject *gobject);
void ags_core_audio_client_finalize(GObject *gobject);

AgsUUID* ags_core_audio_client_get_uuid(AgsConnectable *connectable);
gboolean ags_core_audio_client_has_resource(AgsConnectable *connectable);
gboolean ags_core_audio_client_is_ready(AgsConnectable *connectable);
void ags_core_audio_client_add_to_registry(AgsConnectable *connectable);
void ags_core_audio_client_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_core_audio_client_list_resource(AgsConnectable *connectable);
xmlNode* ags_core_audio_client_xml_compose(AgsConnectable *connectable);
void ags_core_audio_client_xml_parse(AgsConnectable *connectable,
				     xmlNode *node);
gboolean ags_core_audio_client_is_connected(AgsConnectable *connectable);
void ags_core_audio_client_connect(AgsConnectable *connectable);
void ags_core_audio_client_disconnect(AgsConnectable *connectable);

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
  PROP_CLIENT_NAME,
  PROP_DEVICE,
  PROP_PORT,
};

static gpointer ags_core_audio_client_parent_class = NULL;

static pthread_mutex_t ags_core_audio_client_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_core_audio_client_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_core_audio_client = 0;

    static const GTypeInfo ags_core_audio_client_info = {
      sizeof(AgsCoreAudioClientClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_core_audio_client_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCoreAudioClient),
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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_core_audio_client);
  }

  return g_define_type_id__volatile;
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
   * Since: 2.0.0
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
   * AgsCoreAudioClient:client-name:
   *
   * The core-audio client name.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("client-name",
				   i18n_pspec("the client name"),
				   i18n_pspec("The client name"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CLIENT_NAME,
				  param_spec);

  /**
   * AgsCoreAudioClient:device:
   *
   * The assigned devices.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("device",
				    i18n_pspec("assigned device"),
				    i18n_pspec("The assigned device"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);

  /**
   * AgsCoreAudioClient:port:
   *
   * The assigned ports.
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("port",
				    i18n_pspec("assigned port"),
				    i18n_pspec("The assigned port"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT,
				  param_spec);
}

void
ags_core_audio_client_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_core_audio_client_get_uuid;
  connectable->has_resource = ags_core_audio_client_has_resource;

  connectable->is_ready = ags_core_audio_client_is_ready;
  connectable->add_to_registry = ags_core_audio_client_add_to_registry;
  connectable->remove_from_registry = ags_core_audio_client_remove_from_registry;

  connectable->list_resource = ags_core_audio_client_list_resource;
  connectable->xml_compose = ags_core_audio_client_xml_compose;
  connectable->xml_parse = ags_core_audio_client_xml_parse;

  connectable->is_connected = ags_core_audio_client_is_connected;  
  connectable->connect = ags_core_audio_client_connect;
  connectable->disconnect = ags_core_audio_client_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_core_audio_client_init(AgsCoreAudioClient *core_audio_client)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* flags */
  core_audio_client->flags = 0;

  /* client mutex */
  core_audio_client->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  core_audio_client->obj_mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* server */
  core_audio_client->core_audio_server = NULL;
  
  /* uuid */
  core_audio_client->uuid = ags_uuid_alloc();
  ags_uuid_generate(core_audio_client->uuid);

  /* client name and uuid */
  core_audio_client->client_uuid = ags_id_generator_create_uuid();
  core_audio_client->client_name = NULL;

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

  pthread_mutex_t *core_audio_client_mutex;

  core_audio_client = AGS_CORE_AUDIO_CLIENT(gobject);

  /* get core-audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  switch(prop_id){
  case PROP_CORE_AUDIO_SERVER:
    {
      AgsCoreAudioServer *core_audio_server;

      core_audio_server = (AgsCoreAudioServer *) g_value_get_object(value);

      pthread_mutex_lock(core_audio_client_mutex);
      
      if(core_audio_client->core_audio_server == (GObject *) core_audio_server){
	pthread_mutex_unlock(core_audio_client_mutex);
	
	return;
      }

      if(core_audio_client->core_audio_server != NULL){
	g_object_unref(core_audio_client->core_audio_server);
      }

      if(core_audio_server != NULL){
	g_object_ref(core_audio_server);
      }
      
      core_audio_client->core_audio_server = (GObject *) core_audio_server;

      pthread_mutex_unlock(core_audio_client_mutex);
    }
    break;
  case PROP_CLIENT_NAME:
    {
      char *client_name;

      client_name = (char *) g_value_get_string(value);

      pthread_mutex_lock(core_audio_client_mutex);

      g_free(core_audio_client->client_name);
	
      core_audio_client->client_name = g_strdup(client_name);

      pthread_mutex_unlock(core_audio_client_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      GObject *device;

      device = (GObject *) g_value_get_pointer(value);

      pthread_mutex_lock(core_audio_client_mutex);

      if(device == NULL ||
	 g_list_find(core_audio_client->device,
		     device) != NULL){
	pthread_mutex_unlock(core_audio_client_mutex);

	return;
      }

      g_object_ref(device);
	
      core_audio_client->device = g_list_prepend(core_audio_client->device,
						 device);

      pthread_mutex_unlock(core_audio_client_mutex);
    }
    break;
  case PROP_PORT:
    {
      GObject *port;

      port = (GObject *) g_value_get_pointer(value);

      pthread_mutex_lock(core_audio_client_mutex);

      if(!AGS_IS_CORE_AUDIO_PORT(port) ||
	 g_list_find(core_audio_client->port,
		     port) != NULL){
	pthread_mutex_unlock(core_audio_client_mutex);

	return;
      }

      g_object_ref(port);	
      core_audio_client->port = g_list_prepend(core_audio_client->port,
					       port);

      pthread_mutex_unlock(core_audio_client_mutex);
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

  pthread_mutex_t *core_audio_client_mutex;

  core_audio_client = AGS_CORE_AUDIO_CLIENT(gobject);

  /* get core-audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());
  
  switch(prop_id){
  case PROP_CORE_AUDIO_SERVER:
    {
      pthread_mutex_lock(core_audio_client_mutex);

      g_value_set_object(value, core_audio_client->core_audio_server);

      pthread_mutex_unlock(core_audio_client_mutex);
    }
    break;
  case PROP_CLIENT_NAME:
    {
      pthread_mutex_lock(core_audio_client_mutex);

      g_value_set_string(value, core_audio_client->client_name);

      pthread_mutex_unlock(core_audio_client_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      pthread_mutex_lock(core_audio_client_mutex);

      g_value_set_pointer(value,
			  g_list_copy(core_audio_client->device));

      pthread_mutex_unlock(core_audio_client_mutex);
    }
    break;
  case PROP_PORT:
    {
      pthread_mutex_lock(core_audio_client_mutex);

      g_value_set_pointer(value,
			  g_list_copy(core_audio_client->port));

      pthread_mutex_unlock(core_audio_client_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
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

  core_audio_client = AGS_CORE_AUDIO_CLIENT(gobject);

  pthread_mutex_destroy(core_audio_client->obj_mutex);
  free(core_audio_client->obj_mutex);

  pthread_mutexattr_destroy(core_audio_client->obj_mutexattr);
  free(core_audio_client->obj_mutexattr);
  
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

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_client_parent_class)->finalize(gobject);
}

AgsUUID*
ags_core_audio_client_get_uuid(AgsConnectable *connectable)
{
  AgsCoreAudioClient *core_audio_client;
  
  AgsUUID *ptr;

  pthread_mutex_t *core_audio_client_mutex;

  core_audio_client = AGS_CORE_AUDIO_CLIENT(connectable);

  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(core_audio_client_mutex);

  ptr = core_audio_client->uuid;

  pthread_mutex_unlock(core_audio_client_mutex);
  
  return(ptr);
}

gboolean
ags_core_audio_client_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_core_audio_client_is_ready(AgsConnectable *connectable)
{
  AgsCoreAudioClient *core_audio_client;
  
  gboolean is_ready;

  pthread_mutex_t *core_audio_client_mutex;

  core_audio_client = AGS_CORE_AUDIO_CLIENT(connectable);

  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  /* check is added */
  pthread_mutex_lock(core_audio_client_mutex);

  is_ready = (((AGS_CORE_AUDIO_CLIENT_ADDED_TO_REGISTRY & (core_audio_client->flags)) != 0) ? TRUE: FALSE);

  pthread_mutex_unlock(core_audio_client_mutex);
  
  return(is_ready);
}

void
ags_core_audio_client_add_to_registry(AgsConnectable *connectable)
{
  AgsCoreAudioClient *core_audio_client;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  core_audio_client = AGS_CORE_AUDIO_CLIENT(connectable);

  ags_core_audio_client_set_flags(core_audio_client, AGS_CORE_AUDIO_CLIENT_ADDED_TO_REGISTRY);
}

void
ags_core_audio_client_remove_from_registry(AgsConnectable *connectable)
{
  AgsCoreAudioClient *core_audio_client;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  core_audio_client = AGS_CORE_AUDIO_CLIENT(connectable);

  ags_core_audio_client_unset_flags(core_audio_client, AGS_CORE_AUDIO_CLIENT_ADDED_TO_REGISTRY);
}

xmlNode*
ags_core_audio_client_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_core_audio_client_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_core_audio_client_xml_parse(AgsConnectable *connectable,
				xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_core_audio_client_is_connected(AgsConnectable *connectable)
{
  AgsCoreAudioClient *core_audio_client;
  
  gboolean is_connected;

  pthread_mutex_t *core_audio_client_mutex;

  core_audio_client = AGS_CORE_AUDIO_CLIENT(connectable);

  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(core_audio_client_mutex);

  is_connected = (((AGS_CORE_AUDIO_CLIENT_CONNECTED & (core_audio_client->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(core_audio_client_mutex);
  
  return(is_connected);
}

void
ags_core_audio_client_connect(AgsConnectable *connectable)
{
  AgsCoreAudioClient *core_audio_client;

  GList *list_start, *list;

  pthread_mutex_t *core_audio_client_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_client = AGS_CORE_AUDIO_CLIENT(connectable);

  ags_core_audio_client_set_flags(core_audio_client, AGS_CORE_AUDIO_CLIENT_CONNECTED);

  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  /* port */
  pthread_mutex_lock(core_audio_client_mutex);

  list =
    list_start = g_list_copy(core_audio_client->port);

  pthread_mutex_unlock(core_audio_client_mutex);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_core_audio_client_disconnect(AgsConnectable *connectable)
{

  AgsCoreAudioClient *core_audio_client;

  GList *list_start, *list;

  pthread_mutex_t *core_audio_client_mutex;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_client = AGS_CORE_AUDIO_CLIENT(connectable);
  
  ags_core_audio_client_unset_flags(core_audio_client, AGS_CORE_AUDIO_CLIENT_CONNECTED);

  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  /* port */
  pthread_mutex_lock(core_audio_client_mutex);

  list =
    list_start = g_list_copy(core_audio_client->port);

  pthread_mutex_unlock(core_audio_client_mutex);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_core_audio_client_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_core_audio_client_get_class_mutex()
{
  return(&ags_core_audio_client_class_mutex);
}

/**
 * ags_core_audio_client_test_flags:
 * @core_audio_client: the #AgsCoreAudioClient
 * @flags: the flags
 *
 * Test @flags to be set on @core_audio_client.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_core_audio_client_test_flags(AgsCoreAudioClient *core_audio_client, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *core_audio_client_mutex;

  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return(FALSE);
  }

  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  /* test */
  pthread_mutex_lock(core_audio_client_mutex);

  retval = (flags & (core_audio_client->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(core_audio_client_mutex);

  return(retval);
}

/**
 * ags_core_audio_client_set_flags:
 * @core_audio_client: the #AgsCoreAudioClient
 * @flags: see #AgsCoreAudioClientFlags-enum
 *
 * Enable a feature of @core_audio_client.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_client_set_flags(AgsCoreAudioClient *core_audio_client, guint flags)
{
  pthread_mutex_t *core_audio_client_mutex;

  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return;
  }

  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(core_audio_client_mutex);

  core_audio_client->flags |= flags;
  
  pthread_mutex_unlock(core_audio_client_mutex);
}
    
/**
 * ags_core_audio_client_unset_flags:
 * @core_audio_client: the #AgsCoreAudioClient
 * @flags: see #AgsCoreAudioClientFlags-enum
 *
 * Disable a feature of @core_audio_client.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_client_unset_flags(AgsCoreAudioClient *core_audio_client, guint flags)
{  
  pthread_mutex_t *core_audio_client_mutex;

  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return;
  }

  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(core_audio_client_mutex);

  core_audio_client->flags &= (~flags);
  
  pthread_mutex_unlock(core_audio_client_mutex);
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
 * Since: 2.0.0
 */
GList*
ags_core_audio_client_find_uuid(GList *core_audio_client,
				gchar *client_uuid)
{
  while(core_audio_client != NULL){
    if(AGS_CORE_AUDIO_CLIENT(core_audio_client->data)->graph != NULL &&
       !g_ascii_strcasecmp(AGS_CORE_AUDIO_CLIENT(core_audio_client->data)->client_uuid,
			   client_uuid)){
      return(core_audio_client);
    }
  }

  return(NULL);
}

/**
 * ags_core_audio_client_find:
 * @core_audio_client: the #GList-struct containing #AgsCoreAudioClient
 * @client_name: the client name to find
 *
 * Finds next match of @client_name in @core_audio_client.
 *
 * Returns: the next matching #GList-struct or %NULL
 * 
 * Since: 2.0.0
 */
GList*
ags_core_audio_client_find(GList *core_audio_client,
			   gchar *client_name)
{ 
  AgsCoreAudioClient *current_core_audio_client;
  
  gboolean success;
  
  pthread_mutex_t *core_audio_client_mutex;

  while(core_audio_client != NULL){
    current_core_audio_client = AGS_CORE_AUDIO_CLIENT(core_audio_client->data);
    
    /* get core_audio client mutex */
    pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
    core_audio_client_mutex = current_core_audio_client->obj_mutex;
  
    pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

    /* check uuid */
    pthread_mutex_lock(core_audio_client_mutex);
    
    success = (current_core_audio_client->graph != NULL &&
	       !g_ascii_strcasecmp(current_core_audio_client->client_name,
				   client_name)) ? TRUE: FALSE;

    pthread_mutex_unlock(core_audio_client_mutex);

    if(success){
      return(core_audio_client);
    }

    core_audio_client = core_audio_client->next;
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
 * Since: 2.0.0
 */
void
ags_core_audio_client_open(AgsCoreAudioClient *core_audio_client,
			   gchar *client_name)
{
  AgsCoreAudioServer *core_audio_server;

#ifdef AGS_WITH_CORE_AUDIO
  OSStatus retval;
#endif
  
  gboolean ready;

  pthread_mutex_t *core_audio_server_mutex;
  pthread_mutex_t *core_audio_client_mutex;
  
  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client) ||
     client_name == NULL){
    return;
  }

  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  /* check already open */
  pthread_mutex_lock(core_audio_client_mutex);

  if(core_audio_client->graph != NULL){
    pthread_mutex_unlock(core_audio_client_mutex);

    g_message("Advanced Gtk+ Sequencer core-audio client already open");
    
    return;
  } 

  core_audio_server = core_audio_client->core_audio_server;
  
  pthread_mutex_unlock(core_audio_client_mutex);
  
  g_message("Advanced Gtk+ Sequencer open core-audio client");
  
  g_object_set(core_audio_client,
	       "client-name", client_name,
	       NULL);

  /* get core_audio server mutex */
  pthread_mutex_lock(ags_core_audio_server_get_class_mutex());
  
  core_audio_server_mutex = core_audio_server->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_server_get_class_mutex());

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
 * Since: 2.0.0
 */
void
ags_core_audio_client_activate(AgsCoreAudioClient *core_audio_client)
{
#ifdef AGS_WITH_CORE_AUDIO
  AUGraph *graph;
#else
  gpointer graph;
#endif

  GList *port_start, *port;
  
  int ret;

  pthread_mutex_t *core_audio_client_mutex;

  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return;
  }
  
  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  /* get graph */
  pthread_mutex_lock(core_audio_client_mutex);

  graph = core_audio_client->graph;
  
  pthread_mutex_unlock(core_audio_client_mutex);

  if(ags_core_audio_client_test_flags(core_audio_client, AGS_CORE_AUDIO_CLIENT_ACTIVATED) ||
     graph == NULL){
    return;
  }  

  pthread_mutex_lock(core_audio_client_mutex);

  port =
    port_start = g_list_copy(core_audio_client->port);

  pthread_mutex_unlock(core_audio_client_mutex);

  while(port != NULL){
    gchar *port_name;

    //TODO:JK: make thread-safe
    g_object_get(port->data,
		 "port-name", &port_name,
		 NULL);

    ags_core_audio_port_register(port->data,
				 port_name,
				 (ags_core_audio_port_test_flags(port->data, AGS_CORE_AUDIO_PORT_IS_AUDIO) ? TRUE: FALSE), (ags_core_audio_port_test_flags(port->data, AGS_CORE_AUDIO_PORT_IS_MIDI) ? TRUE: FALSE),
				 (ags_core_audio_port_test_flags(port->data, AGS_CORE_AUDIO_PORT_IS_OUTPUT) ? TRUE: FALSE));
    
    port = port->next;
  }

  ags_core_audio_client_set_flags(core_audio_client, AGS_CORE_AUDIO_CLIENT_ACTIVATED);

  g_list_free(port_start);
}

/**
 * ags_core_audio_client_deactivate:
 * @core_audio_client: the #AgsCoreAudioClient
 *
 * Deactivate client.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_client_deactivate(AgsCoreAudioClient *core_audio_client)
{
#ifdef AGS_WITH_CORE_AUDIO
  AUGraph *graph;
#else
  gpointer graph;
#endif

  pthread_mutex_t *core_audio_client_mutex;

  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return;
  }
  
  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  /* get graph */
  pthread_mutex_lock(core_audio_client_mutex);

  graph = core_audio_client->graph;
  
  pthread_mutex_unlock(core_audio_client_mutex);

  if(graph == NULL){
    return;
  }

  AUGraphStop(graph);
    
  /* set graph */
  pthread_mutex_lock(core_audio_client_mutex);

  core_audio_client->graph = NULL;
  
  pthread_mutex_unlock(core_audio_client_mutex);

  ags_core_audio_client_unset_flags(core_audio_client, AGS_CORE_AUDIO_CLIENT_ACTIVATED);
}

/**
 * ags_core_audio_client_add_device:
 * @core_audio_client: the #AgsCoreAudioClient
 * @core_audio_device: an #AgsCoreAudioDevout or #AgsCoreAudioDevin
 *
 * Add @core_audio_device to @core_audio_client.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_client_add_device(AgsCoreAudioClient *core_audio_client,
				 GObject *core_audio_device)
{
  pthread_mutex_t *core_audio_client_mutex;

  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client) ||
     (!AGS_IS_CORE_AUDIO_DEVOUT(core_audio_device) &&
      !AGS_IS_CORE_AUDIO_DEVIN(core_audio_device))){
    return;
  }

  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  /* add device */
  pthread_mutex_lock(core_audio_client_mutex);

  if(g_list_find(core_audio_client->device, core_audio_device) == NULL){
    g_object_ref(core_audio_device);
    core_audio_client->device = g_list_prepend(core_audio_client->device,
					       core_audio_device);
  }
  
  pthread_mutex_unlock(core_audio_client_mutex);
}

/**
 * ags_core_audio_client_remove_device:
 * @core_audio_client: the #AgsCoreAudioClient
 * @core_audio_device: an #AgsCoreAudioDevout or #AgsCoreAudioDevin
 *
 * Remove @core_audio_device from @core_audio_client.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_client_remove_device(AgsCoreAudioClient *core_audio_client,
				    GObject *core_audio_device)
{
  pthread_mutex_t *core_audio_client_mutex;

  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return;
  }

  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());
  
  /* remove */
  pthread_mutex_lock(core_audio_client_mutex);

  if(g_list_find(core_audio_client->device, core_audio_device) != NULL){
    core_audio_client->device = g_list_remove(core_audio_client->device,
					      core_audio_device);
    g_object_unref(core_audio_device);
  }

  pthread_mutex_unlock(core_audio_client_mutex);
}

/**
 * ags_core_audio_client_add_port:
 * @core_audio_client: the #AgsCoreAudioClient
 * @core_audio_port: an #AgsCoreAudioPort
 *
 * Add @core_audio_port to @core_audio_client.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_client_add_port(AgsCoreAudioClient *core_audio_client,
			       GObject *core_audio_port)
{
  pthread_mutex_t *core_audio_client_mutex;

  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client) ||
     !AGS_IS_CORE_AUDIO_PORT(core_audio_port)){
    return;
  }
  
  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  /* add port */
  pthread_mutex_lock(core_audio_client_mutex);

  if(g_list_find(core_audio_client->port, core_audio_port) == NULL){
    g_object_ref(core_audio_port);
    core_audio_client->port = g_list_prepend(core_audio_client->port,
					     core_audio_port);
  }
  
  pthread_mutex_unlock(core_audio_client_mutex);
}

/**
 * ags_core_audio_client_remove_port:
 * @core_audio_client: the #AgsCoreAudioClient
 * @core_audio_port: an #AgsCoreAudioPort
 *
 * Remove @core_audio_port from @core_audio_client.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_client_remove_port(AgsCoreAudioClient *core_audio_client,
				  GObject *core_audio_port)
{
  pthread_mutex_t *core_audio_client_mutex;

  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return;
  }
  
  /* get core_audio client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  /* remove port */
  pthread_mutex_lock(core_audio_client_mutex);

  if(g_list_find(core_audio_client->port, core_audio_port) != NULL){
    core_audio_client->port = g_list_remove(core_audio_client->port,
					    core_audio_port);
    g_object_unref(core_audio_port);
  }
  
  pthread_mutex_unlock(core_audio_client_mutex);
}

/**
 * ags_core_audio_client_new:
 * @core_audio_server: the assigned #AgsCoreAudioServer
 *
 * Create a new instance of #AgsCoreAudioClient.
 *
 * Returns: the new #AgsCoreAudioClient
 *
 * Since: 2.0.0
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
