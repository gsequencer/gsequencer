/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/audio/audio-unit/ags_audio_unit_client.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/audio-unit/ags_audio_unit_server.h>
#include <ags/audio/audio-unit/ags_audio_unit_port.h>
#include <ags/audio/audio-unit/ags_audio_unit_devout.h>
#include <ags/audio/audio-unit/ags_audio_unit_devin.h>

#include <stdlib.h>

#include <ags/i18n.h>

void ags_audio_unit_client_class_init(AgsAudioUnitClientClass *audio_unit_client);
void ags_audio_unit_client_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_unit_client_init(AgsAudioUnitClient *audio_unit_client);
void ags_audio_unit_client_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_audio_unit_client_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_audio_unit_client_dispose(GObject *gobject);
void ags_audio_unit_client_finalize(GObject *gobject);

AgsUUID* ags_audio_unit_client_get_uuid(AgsConnectable *connectable);
gboolean ags_audio_unit_client_has_resource(AgsConnectable *connectable);
gboolean ags_audio_unit_client_is_ready(AgsConnectable *connectable);
void ags_audio_unit_client_add_to_registry(AgsConnectable *connectable);
void ags_audio_unit_client_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_audio_unit_client_list_resource(AgsConnectable *connectable);
xmlNode* ags_audio_unit_client_xml_compose(AgsConnectable *connectable);
void ags_audio_unit_client_xml_parse(AgsConnectable *connectable,
				     xmlNode *node);
gboolean ags_audio_unit_client_is_connected(AgsConnectable *connectable);
void ags_audio_unit_client_connect(AgsConnectable *connectable);
void ags_audio_unit_client_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_audio_unit_client
 * @short_description: core audio connection
 * @title: AgsAudioUnitClient
 * @section_id:
 * @include: ags/audio/audio-unit/ags_audio_unit_client.h
 *
 * The #AgsAudioUnitClient communicates with a core audio instance.
 */

enum{
  PROP_0,
  PROP_AUDIO_UNIT_SERVER,
  PROP_CLIENT_NAME,
  PROP_DEVICE,
  PROP_PORT,
};

static gpointer ags_audio_unit_client_parent_class = NULL;

GType
ags_audio_unit_client_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_audio_unit_client = 0;

    static const GTypeInfo ags_audio_unit_client_info = {
      sizeof(AgsAudioUnitClientClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_unit_client_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAudioUnitClient),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_unit_client_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_unit_client_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_audio_unit_client = g_type_register_static(G_TYPE_OBJECT,
							"AgsAudioUnitClient",
							&ags_audio_unit_client_info,
							0);

    g_type_add_interface_static(ags_type_audio_unit_client,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_audio_unit_client);
  }

  return(g_define_type_id__static);
}

GType
ags_audio_unit_client_flags_get_type()
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_AUDIO_UNIT_CLIENT_ACTIVATED, "AGS_AUDIO_UNIT_CLIENT_ACTIVATED", "audio_unit-client-activated" },
      { AGS_AUDIO_UNIT_CLIENT_READY, "AGS_AUDIO_UNIT_CLIENT_READY", "audio_unit-client-ready" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsAudioUnitClientFlags"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
}

void
ags_audio_unit_client_class_init(AgsAudioUnitClientClass *audio_unit_client)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_audio_unit_client_parent_class = g_type_class_peek_parent(audio_unit_client);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_unit_client;

  gobject->set_property = ags_audio_unit_client_set_property;
  gobject->get_property = ags_audio_unit_client_get_property;

  gobject->dispose = ags_audio_unit_client_dispose;
  gobject->finalize = ags_audio_unit_client_finalize;

  /* properties */
  /**
   * AgsAudioUnitClient:audio-unit-server:
   *
   * The assigned #AgsAudioUnitServer.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio-unit-server",
				   i18n_pspec("assigned core audio server"),
				   i18n_pspec("The assigned core audio server"),
				   AGS_TYPE_AUDIO_UNIT_SERVER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_UNIT_SERVER,
				  param_spec);

  /**
   * AgsAudioUnitClient:client-name:
   *
   * The audio-unit client name.
   * 
   * Since: 3.0.0
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
   * AgsAudioUnitClient:device: (type GList(GObject)) (transfer full)
   *
   * The assigned devices.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("device",
				    i18n_pspec("assigned device"),
				    i18n_pspec("The assigned device"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);

  /**
   * AgsAudioUnitClient:port: (type GList(AgsAudioUnitPort)) (transfer full)
   *
   * The assigned ports.
   * 
   * Since: 3.0.0
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
ags_audio_unit_client_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_audio_unit_client_get_uuid;
  connectable->has_resource = ags_audio_unit_client_has_resource;

  connectable->is_ready = ags_audio_unit_client_is_ready;
  connectable->add_to_registry = ags_audio_unit_client_add_to_registry;
  connectable->remove_from_registry = ags_audio_unit_client_remove_from_registry;

  connectable->list_resource = ags_audio_unit_client_list_resource;
  connectable->xml_compose = ags_audio_unit_client_xml_compose;
  connectable->xml_parse = ags_audio_unit_client_xml_parse;

  connectable->is_connected = ags_audio_unit_client_is_connected;  
  connectable->connect = ags_audio_unit_client_connect;
  connectable->disconnect = ags_audio_unit_client_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_audio_unit_client_init(AgsAudioUnitClient *audio_unit_client)
{
  /* flags */
  audio_unit_client->flags = 0;
  audio_unit_client->connectable_flags = 0;

  /* audio unit client mutex */
  g_rec_mutex_init(&(audio_unit_client->obj_mutex));

  /* server */
  audio_unit_client->audio_unit_server = NULL;
  
  /* uuid */
  audio_unit_client->uuid = ags_uuid_alloc();
  ags_uuid_generate(audio_unit_client->uuid);

  /* client name and uuid */
  audio_unit_client->client_uuid = ags_id_generator_create_uuid();
  audio_unit_client->client_name = NULL;
  
  /* device */
  audio_unit_client->device = NULL;
  audio_unit_client->port = NULL;
}

void
ags_audio_unit_client_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsAudioUnitClient *audio_unit_client;

  GRecMutex *audio_unit_client_mutex;

  audio_unit_client = AGS_AUDIO_UNIT_CLIENT(gobject);

  /* get audio-unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  switch(prop_id){
  case PROP_AUDIO_UNIT_SERVER:
    {
      AgsAudioUnitServer *audio_unit_server;

      audio_unit_server = (AgsAudioUnitServer *) g_value_get_object(value);

      g_rec_mutex_lock(audio_unit_client_mutex);
      
      if(audio_unit_client->audio_unit_server == (GObject *) audio_unit_server){
	g_rec_mutex_unlock(audio_unit_client_mutex);
	
	return;
      }

      if(audio_unit_client->audio_unit_server != NULL){
	g_object_unref(audio_unit_client->audio_unit_server);
      }

      if(audio_unit_server != NULL){
	g_object_ref(audio_unit_server);
      }
      
      audio_unit_client->audio_unit_server = (GObject *) audio_unit_server;

      g_rec_mutex_unlock(audio_unit_client_mutex);
    }
    break;
  case PROP_CLIENT_NAME:
    {
      char *client_name;

      client_name = (char *) g_value_get_string(value);

      g_rec_mutex_lock(audio_unit_client_mutex);

      g_free(audio_unit_client->client_name);
	
      audio_unit_client->client_name = g_strdup(client_name);

      g_rec_mutex_unlock(audio_unit_client_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      GObject *device;

      device = (GObject *) g_value_get_pointer(value);

      g_rec_mutex_lock(audio_unit_client_mutex);

      if(device == NULL ||
	 g_list_find(audio_unit_client->device,
		     device) != NULL){
	g_rec_mutex_unlock(audio_unit_client_mutex);

	return;
      }

      g_object_ref(device);
	
      audio_unit_client->device = g_list_prepend(audio_unit_client->device,
						 device);

      g_rec_mutex_unlock(audio_unit_client_mutex);
    }
    break;
  case PROP_PORT:
    {
      GObject *port;

      port = (GObject *) g_value_get_pointer(value);

      g_rec_mutex_lock(audio_unit_client_mutex);

      if(!AGS_IS_AUDIO_UNIT_PORT(port) ||
	 g_list_find(audio_unit_client->port,
		     port) != NULL){
	g_rec_mutex_unlock(audio_unit_client_mutex);

	return;
      }

      g_object_ref(port);	
      audio_unit_client->port = g_list_prepend(audio_unit_client->port,
					       port);

      g_rec_mutex_unlock(audio_unit_client_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_client_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsAudioUnitClient *audio_unit_client;

  GRecMutex *audio_unit_client_mutex;

  audio_unit_client = AGS_AUDIO_UNIT_CLIENT(gobject);

  /* get audio-unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);
  
  switch(prop_id){
  case PROP_AUDIO_UNIT_SERVER:
    {
      g_rec_mutex_lock(audio_unit_client_mutex);

      g_value_set_object(value, audio_unit_client->audio_unit_server);

      g_rec_mutex_unlock(audio_unit_client_mutex);
    }
    break;
  case PROP_CLIENT_NAME:
    {
      g_rec_mutex_lock(audio_unit_client_mutex);

      g_value_set_string(value, audio_unit_client->client_name);

      g_rec_mutex_unlock(audio_unit_client_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(audio_unit_client_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(audio_unit_client->device,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(audio_unit_client_mutex);
    }
    break;
  case PROP_PORT:
    {
      g_rec_mutex_lock(audio_unit_client_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(audio_unit_client->port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(audio_unit_client_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_client_dispose(GObject *gobject)
{
  AgsAudioUnitClient *audio_unit_client;

  GList *list;
  
  audio_unit_client = AGS_AUDIO_UNIT_CLIENT(gobject);

  /* core audio server */
  if(audio_unit_client->audio_unit_server != NULL){
    g_object_unref(audio_unit_client->audio_unit_server);

    audio_unit_client->audio_unit_server = NULL;
  }

  /* device */
  if(audio_unit_client->device != NULL){
    list = audio_unit_client->device;

    while(list != NULL){
      g_object_set(G_OBJECT(list->data),
		   "audio-unit-client", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(audio_unit_client->device,
		     g_object_unref);

    audio_unit_client->device = NULL;
  }

  /* port */
  if(audio_unit_client->port != NULL){
    list = audio_unit_client->port;

    while(list != NULL){
      g_object_run_dispose(G_OBJECT(list->data));

      list = list->next;
    }
    
    g_list_free_full(audio_unit_client->port,
		     g_object_unref);

    audio_unit_client->port = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_client_parent_class)->dispose(gobject);
}

void
ags_audio_unit_client_finalize(GObject *gobject)
{
  AgsAudioUnitClient *audio_unit_client;

  audio_unit_client = AGS_AUDIO_UNIT_CLIENT(gobject);
  
  /* core audio server */
  if(audio_unit_client->audio_unit_server != NULL){
    g_object_unref(audio_unit_client->audio_unit_server);
  }

  /* device */
  if(audio_unit_client->device != NULL){
    g_list_free_full(audio_unit_client->device,
		     g_object_unref);
  }

  /* port */
  if(audio_unit_client->port != NULL){
    g_list_free_full(audio_unit_client->port,
		     g_object_unref);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_client_parent_class)->finalize(gobject);
}

AgsUUID*
ags_audio_unit_client_get_uuid(AgsConnectable *connectable)
{
  AgsAudioUnitClient *audio_unit_client;
  
  AgsUUID *ptr;

  GRecMutex *audio_unit_client_mutex;

  audio_unit_client = AGS_AUDIO_UNIT_CLIENT(connectable);

  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  /* get UUID */
  g_rec_mutex_lock(audio_unit_client_mutex);

  ptr = audio_unit_client->uuid;

  g_rec_mutex_unlock(audio_unit_client_mutex);
  
  return(ptr);
}

gboolean
ags_audio_unit_client_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_audio_unit_client_is_ready(AgsConnectable *connectable)
{
  AgsAudioUnitClient *audio_unit_client;
  
  gboolean is_ready;

  GRecMutex *audio_unit_client_mutex;

  audio_unit_client = AGS_AUDIO_UNIT_CLIENT(connectable);

  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  /* test */
  g_rec_mutex_lock(audio_unit_client_mutex);

  is_ready = (AGS_CONNECTABLE_ADDED_TO_REGISTRY & (audio_unit_client->connectable_flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(audio_unit_client_mutex);
  
  return(is_ready);
}

void
ags_audio_unit_client_add_to_registry(AgsConnectable *connectable)
{
  AgsAudioUnitClient *audio_unit_client;

  GRecMutex *audio_unit_client_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  audio_unit_client = AGS_AUDIO_UNIT_CLIENT(connectable);

  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  /* set added to registry */
  g_rec_mutex_lock(audio_unit_client_mutex);

  audio_unit_client->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(audio_unit_client_mutex);
}

void
ags_audio_unit_client_remove_from_registry(AgsConnectable *connectable)
{
  AgsAudioUnitClient *audio_unit_client;

  GRecMutex *audio_unit_client_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  audio_unit_client = AGS_AUDIO_UNIT_CLIENT(connectable);

  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  /* set added to registry */
  g_rec_mutex_lock(audio_unit_client_mutex);

  audio_unit_client->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(audio_unit_client_mutex);
}

xmlNode*
ags_audio_unit_client_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_audio_unit_client_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_audio_unit_client_xml_parse(AgsConnectable *connectable,
				xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_audio_unit_client_is_connected(AgsConnectable *connectable)
{
  AgsAudioUnitClient *audio_unit_client;
  
  gboolean is_connected;

  GRecMutex *audio_unit_client_mutex;

  audio_unit_client = AGS_AUDIO_UNIT_CLIENT(connectable);

  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  /* test */
  g_rec_mutex_lock(audio_unit_client_mutex);

  is_connected = (AGS_CONNECTABLE_CONNECTED & (audio_unit_client->connectable_flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(audio_unit_client_mutex);
  
  return(is_connected);
}

void
ags_audio_unit_client_connect(AgsConnectable *connectable)
{
  AgsAudioUnitClient *audio_unit_client;

  GList *list_start, *list;

  GRecMutex *audio_unit_client_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  audio_unit_client = AGS_AUDIO_UNIT_CLIENT(connectable);

  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  /* set connected */
  g_rec_mutex_lock(audio_unit_client_mutex);

  audio_unit_client->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(audio_unit_client_mutex);

  /* port */
  g_object_get(audio_unit_client,
	       "port", &list_start,
	       NULL);

  list = list_start;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
}

void
ags_audio_unit_client_disconnect(AgsConnectable *connectable)
{

  AgsAudioUnitClient *audio_unit_client;

  GList *list_start, *list;

  GRecMutex *audio_unit_client_mutex;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  audio_unit_client = AGS_AUDIO_UNIT_CLIENT(connectable);
  
  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  /* unset connected */
  g_rec_mutex_lock(audio_unit_client_mutex);

  audio_unit_client->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(audio_unit_client_mutex);

  /* port */
  g_object_get(audio_unit_client,
	       "port", &list_start,
	       NULL);

  list = list_start;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
}

/**
 * ags_audio_unit_client_test_flags:
 * @audio_unit_client: the #AgsAudioUnitClient
 * @flags: the flags
 *
 * Test @flags to be set on @audio_unit_client.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_audio_unit_client_test_flags(AgsAudioUnitClient *audio_unit_client, AgsAudioUnitClientFlags flags)
{
  gboolean retval;  
  
  GRecMutex *audio_unit_client_mutex;

  if(!AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client)){
    return(FALSE);
  }

  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  /* test */
  g_rec_mutex_lock(audio_unit_client_mutex);

  retval = (flags & (audio_unit_client->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(audio_unit_client_mutex);

  return(retval);
}

/**
 * ags_audio_unit_client_set_flags:
 * @audio_unit_client: the #AgsAudioUnitClient
 * @flags: see #AgsAudioUnitClientFlags-enum
 *
 * Enable a feature of @audio_unit_client.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_client_set_flags(AgsAudioUnitClient *audio_unit_client, AgsAudioUnitClientFlags flags)
{
  GRecMutex *audio_unit_client_mutex;

  if(!AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client)){
    return;
  }

  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(audio_unit_client_mutex);

  audio_unit_client->flags |= flags;
  
  g_rec_mutex_unlock(audio_unit_client_mutex);
}
    
/**
 * ags_audio_unit_client_unset_flags:
 * @audio_unit_client: the #AgsAudioUnitClient
 * @flags: see #AgsAudioUnitClientFlags-enum
 *
 * Disable a feature of @audio_unit_client.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_client_unset_flags(AgsAudioUnitClient *audio_unit_client, AgsAudioUnitClientFlags flags)
{  
  GRecMutex *audio_unit_client_mutex;

  if(!AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client)){
    return;
  }

  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(audio_unit_client_mutex);

  audio_unit_client->flags &= (~flags);
  
  g_rec_mutex_unlock(audio_unit_client_mutex);
}

/**
 * ags_audio_unit_client_find_uuid:
 * @audio_unit_client: (element-type AgsAudio.AudioUnitClient) (transfer none): the #GList-struct containing #AgsAudioUnitClient
 * @client_uuid: the client uuid to find
 *
 * Finds next match of @client_uuid in @audio_unit_client.
 *
 * Returns: (element-type AgsAudio.AudioUnitClient) (transfer none): the next matching #GList-struct or %NULL
 * 
 * Since: 3.0.0
 */
GList*
ags_audio_unit_client_find_uuid(GList *audio_unit_client,
				gchar *client_uuid)
{
  while(audio_unit_client != NULL){
    if(!g_ascii_strcasecmp(AGS_AUDIO_UNIT_CLIENT(audio_unit_client->data)->client_uuid,
			   client_uuid)){
      return(audio_unit_client);
    }
  }

  return(NULL);
}

/**
 * ags_audio_unit_client_find:
 * @audio_unit_client: (element-type AgsAudio.AudioUnitClient) (transfer none): the #GList-struct containing #AgsAudioUnitClient
 * @client_name: the client name to find
 *
 * Finds next match of @client_name in @audio_unit_client.
 *
 * Returns: (element-type AgsAudio.AudioUnitClient) (transfer none): the next matching #GList-struct or %NULL
 * 
 * Since: 3.0.0
 */
GList*
ags_audio_unit_client_find(GList *audio_unit_client,
			   gchar *client_name)
{ 
  AgsAudioUnitClient *current_audio_unit_client;
  
  gboolean success;
  
  GRecMutex *audio_unit_client_mutex;

  while(audio_unit_client != NULL){
    current_audio_unit_client = AGS_AUDIO_UNIT_CLIENT(audio_unit_client->data);
    
    /* get audio unit client mutex */
    audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(current_audio_unit_client);

    /* check uuid */
    g_rec_mutex_lock(audio_unit_client_mutex);
    
    success = (!g_ascii_strcasecmp(current_audio_unit_client->client_name,
				   client_name)) ? TRUE: FALSE;

    g_rec_mutex_unlock(audio_unit_client_mutex);

    if(success){
      return(audio_unit_client);
    }

    audio_unit_client = audio_unit_client->next;
  }

  return(NULL);
}

/**
 * ags_audio_unit_client_open:
 * @audio_unit_client: the #AgsAudioUnitClient
 * @client_name: the client's name
 *
 * Open the core audio client's connection and read uuid.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_client_open(AgsAudioUnitClient *audio_unit_client,
			   gchar *client_name)
{
  AgsAudioUnitServer *audio_unit_server;

#ifdef AGS_WITH_AUDIO_UNIT
  OSStatus retval;
#endif
  
  gboolean ready;

  GRecMutex *audio_unit_server_mutex;
  GRecMutex *audio_unit_client_mutex;
  
  if(!AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client) ||
     client_name == NULL){
    return;
  }

  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  /* check already open */
  g_rec_mutex_lock(audio_unit_client_mutex);

  if(ags_audio_unit_client_test_flags(audio_unit_client, AGS_AUDIO_UNIT_CLIENT_ACTIVATED)){
    g_rec_mutex_unlock(audio_unit_client_mutex);

    g_message("Advanced Gtk+ Sequencer audio-unit client already open");
    
    return;
  } 

  audio_unit_server = (AgsAudioUnitServer *) audio_unit_client->audio_unit_server;
  
  g_rec_mutex_unlock(audio_unit_client_mutex);
  
  g_message("Advanced Gtk+ Sequencer open audio-unit client");
  
  g_object_set(audio_unit_client,
	       "client-name", client_name,
	       NULL);

  /* get audio unit server mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);
}

/**
 * ags_audio_unit_client_activate:
 * @audio_unit_client: the #AgsAudioUnitClient
 *
 * Activate client.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_client_activate(AgsAudioUnitClient *audio_unit_client)
{
  GList *port_start, *port;
  
  int ret;

  GRecMutex *audio_unit_client_mutex;

  if(!AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client)){
    return;
  }
  
  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  if(ags_audio_unit_client_test_flags(audio_unit_client, AGS_AUDIO_UNIT_CLIENT_ACTIVATED)){
    return;
  }  

  g_rec_mutex_lock(audio_unit_client_mutex);

  port =
    port_start = g_list_copy(audio_unit_client->port);

  g_rec_mutex_unlock(audio_unit_client_mutex);

  while(port != NULL){
    gchar *port_name;

    g_object_get(port->data,
		 "port-name", &port_name,
		 NULL);

    ags_audio_unit_port_register(port->data,
				 port_name,
				 (ags_audio_unit_port_test_flags(port->data, AGS_AUDIO_UNIT_PORT_IS_AUDIO) ? TRUE: FALSE), (ags_audio_unit_port_test_flags(port->data, AGS_AUDIO_UNIT_PORT_IS_MIDI) ? TRUE: FALSE),
				 (ags_audio_unit_port_test_flags(port->data, AGS_AUDIO_UNIT_PORT_IS_OUTPUT) ? TRUE: FALSE));

    g_free(port_name);
    
    port = port->next;
  }

  ags_audio_unit_client_set_flags(audio_unit_client, AGS_AUDIO_UNIT_CLIENT_ACTIVATED);

  g_list_free(port_start);
}

/**
 * ags_audio_unit_client_deactivate:
 * @audio_unit_client: the #AgsAudioUnitClient
 *
 * Deactivate client.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_client_deactivate(AgsAudioUnitClient *audio_unit_client)
{
  GRecMutex *audio_unit_client_mutex;

  if(!AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client)){
    return;
  }
  
  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  ags_audio_unit_client_unset_flags(audio_unit_client, AGS_AUDIO_UNIT_CLIENT_ACTIVATED);
}

/**
 * ags_audio_unit_client_add_device:
 * @audio_unit_client: the #AgsAudioUnitClient
 * @audio_unit_device: an #AgsAudioUnitDevout or #AgsAudioUnitDevin
 *
 * Add @audio_unit_device to @audio_unit_client.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_client_add_device(AgsAudioUnitClient *audio_unit_client,
				 GObject *audio_unit_device)
{
  GRecMutex *audio_unit_client_mutex;

  if(!AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client) ||
     (!AGS_IS_AUDIO_UNIT_DEVOUT(audio_unit_device) &&
      !AGS_IS_AUDIO_UNIT_DEVIN(audio_unit_device))){
    return;
  }

  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  /* add device */
  g_rec_mutex_lock(audio_unit_client_mutex);

  if(g_list_find(audio_unit_client->device, audio_unit_device) == NULL){
    g_object_ref(audio_unit_device);
    audio_unit_client->device = g_list_prepend(audio_unit_client->device,
					       audio_unit_device);
  }
  
  g_rec_mutex_unlock(audio_unit_client_mutex);
}

/**
 * ags_audio_unit_client_remove_device:
 * @audio_unit_client: the #AgsAudioUnitClient
 * @audio_unit_device: an #AgsAudioUnitDevout or #AgsAudioUnitDevin
 *
 * Remove @audio_unit_device from @audio_unit_client.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_client_remove_device(AgsAudioUnitClient *audio_unit_client,
				    GObject *audio_unit_device)
{
  GRecMutex *audio_unit_client_mutex;

  if(!AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client)){
    return;
  }

  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);
  
  /* remove */
  g_rec_mutex_lock(audio_unit_client_mutex);

  if(g_list_find(audio_unit_client->device, audio_unit_device) != NULL){
    audio_unit_client->device = g_list_remove(audio_unit_client->device,
					      audio_unit_device);
    g_object_unref(audio_unit_device);
  }

  g_rec_mutex_unlock(audio_unit_client_mutex);
}

/**
 * ags_audio_unit_client_add_port:
 * @audio_unit_client: the #AgsAudioUnitClient
 * @audio_unit_port: an #AgsAudioUnitPort
 *
 * Add @audio_unit_port to @audio_unit_client.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_client_add_port(AgsAudioUnitClient *audio_unit_client,
			       GObject *audio_unit_port)
{
  GRecMutex *audio_unit_client_mutex;

  if(!AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client) ||
     !AGS_IS_AUDIO_UNIT_PORT(audio_unit_port)){
    return;
  }
  
  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  /* add port */
  g_rec_mutex_lock(audio_unit_client_mutex);

  if(g_list_find(audio_unit_client->port, audio_unit_port) == NULL){
    g_object_ref(audio_unit_port);
    audio_unit_client->port = g_list_prepend(audio_unit_client->port,
					     audio_unit_port);
  }
  
  g_rec_mutex_unlock(audio_unit_client_mutex);
}

/**
 * ags_audio_unit_client_remove_port:
 * @audio_unit_client: the #AgsAudioUnitClient
 * @audio_unit_port: an #AgsAudioUnitPort
 *
 * Remove @audio_unit_port from @audio_unit_client.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_client_remove_port(AgsAudioUnitClient *audio_unit_client,
				  GObject *audio_unit_port)
{
  GRecMutex *audio_unit_client_mutex;

  if(!AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client)){
    return;
  }
  
  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  /* remove port */
  g_rec_mutex_lock(audio_unit_client_mutex);

  if(g_list_find(audio_unit_client->port, audio_unit_port) != NULL){
    audio_unit_client->port = g_list_remove(audio_unit_client->port,
					    audio_unit_port);
    g_object_unref(audio_unit_port);
  }
  
  g_rec_mutex_unlock(audio_unit_client_mutex);
}

/**
 * ags_audio_unit_client_new:
 * @audio_unit_server: the assigned #AgsAudioUnitServer
 *
 * Create a new instance of #AgsAudioUnitClient.
 *
 * Returns: the new #AgsAudioUnitClient
 *
 * Since: 3.0.0
 */
AgsAudioUnitClient*
ags_audio_unit_client_new(GObject *audio_unit_server)
{
  AgsAudioUnitClient *audio_unit_client;

  audio_unit_client = (AgsAudioUnitClient *) g_object_new(AGS_TYPE_AUDIO_UNIT_CLIENT,
							  "audio-unit-server", audio_unit_server,
							  NULL);

  return(audio_unit_client);
}
