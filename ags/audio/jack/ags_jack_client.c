/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/ags_sound_provider.h>
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
void ags_jack_client_init(AgsJackClient *jack_client);
void ags_jack_client_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_jack_client_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_jack_client_dispose(GObject *gobject);
void ags_jack_client_finalize(GObject *gobject);

AgsUUID* ags_jack_client_get_uuid(AgsConnectable *connectable);
gboolean ags_jack_client_has_resource(AgsConnectable *connectable);
gboolean ags_jack_client_is_ready(AgsConnectable *connectable);
void ags_jack_client_add_to_registry(AgsConnectable *connectable);
void ags_jack_client_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_jack_client_list_resource(AgsConnectable *connectable);
xmlNode* ags_jack_client_xml_compose(AgsConnectable *connectable);
void ags_jack_client_xml_parse(AgsConnectable *connectable,
			       xmlNode *node);
gboolean ags_jack_client_is_connected(AgsConnectable *connectable);
void ags_jack_client_connect(AgsConnectable *connectable);
void ags_jack_client_disconnect(AgsConnectable *connectable);

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
  PROP_CLIENT_NAME,
  PROP_DEVICE,
  PROP_PORT,
};

static gpointer ags_jack_client_parent_class = NULL;

GType
ags_jack_client_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_jack_client = 0;

    static const GTypeInfo ags_jack_client_info = {
      sizeof(AgsJackClientClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_client_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsJackClient),
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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_jack_client);
  }

  return g_define_type_id__volatile;
}

GType
ags_jack_client_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_JACK_CLIENT_ACTIVATED, "AGS_JACK_CLIENT_ACTIVATED", "jack-client-activated" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsJackClientFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
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
   * Since: 3.0.0
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
   * AgsJackClient:client-name:
   *
   * The JACK client name.
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
   * AgsJackClient:device: (type GList(GObject)) (transfer full)
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
   * AgsJackClient:port: (type GList(AgsJackPort)) (transfer full)
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
ags_jack_client_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_jack_client_get_uuid;
  connectable->has_resource = ags_jack_client_has_resource;

  connectable->is_ready = ags_jack_client_is_ready;
  connectable->add_to_registry = ags_jack_client_add_to_registry;
  connectable->remove_from_registry = ags_jack_client_remove_from_registry;

  connectable->list_resource = ags_jack_client_list_resource;
  connectable->xml_compose = ags_jack_client_xml_compose;
  connectable->xml_parse = ags_jack_client_xml_parse;

  connectable->is_connected = ags_jack_client_is_connected;  
  connectable->connect = ags_jack_client_connect;
  connectable->disconnect = ags_jack_client_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_jack_client_init(AgsJackClient *jack_client)
{
  /* flags */
  jack_client->flags = 0;
  jack_client->connectable_flags = 0;

  /* jack client mutex */
  g_rec_mutex_init(&(jack_client->obj_mutex));

  /* server */
  jack_client->jack_server = NULL;
  
  /* uuid */
  jack_client->uuid = ags_uuid_alloc();
  ags_uuid_generate(jack_client->uuid);

  jack_client->client_name = NULL;
  jack_client->client_uuid = NULL;

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

  GRecMutex *jack_client_mutex;

  jack_client = AGS_JACK_CLIENT(gobject);

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  switch(prop_id){
  case PROP_JACK_SERVER:
    {
      AgsJackServer *jack_server;

      jack_server = (AgsJackServer *) g_value_get_object(value);

      g_rec_mutex_lock(jack_client_mutex);
      
      if(jack_client->jack_server == (GObject *) jack_server){
	g_rec_mutex_unlock(jack_client_mutex);
	
	return;
      }

      if(jack_client->jack_server != NULL){
	g_object_unref(jack_client->jack_server);
      }

      if(jack_server != NULL){
	g_object_ref(jack_server);
      }
      
      jack_client->jack_server = (GObject *) jack_server;

      g_rec_mutex_unlock(jack_client_mutex);
    }
    break;
  case PROP_CLIENT_NAME:
    {
      char *client_name;

      client_name = (char *) g_value_get_string(value);

      g_rec_mutex_lock(jack_client_mutex);

      g_free(jack_client->client_name);
	
      jack_client->client_name = g_strdup(client_name);

      g_rec_mutex_unlock(jack_client_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      GObject *device;

      device = (GObject *) g_value_get_pointer(value);

      g_rec_mutex_lock(jack_client_mutex);

      if(device == NULL ||
	 g_list_find(jack_client->device,
		     device) != NULL){
	g_rec_mutex_unlock(jack_client_mutex);

	return;
      }

      g_object_ref(device);
	
      jack_client->device = g_list_prepend(jack_client->device,
					   device);

      g_rec_mutex_unlock(jack_client_mutex);
    }
    break;
  case PROP_PORT:
    {
      GObject *port;

      port = (GObject *) g_value_get_pointer(value);

      g_rec_mutex_lock(jack_client_mutex);

      if(!AGS_IS_JACK_PORT(port) ||
	 g_list_find(jack_client->port,
		     port) != NULL){
	g_rec_mutex_unlock(jack_client_mutex);

	return;
      }

      g_object_ref(port);	
      jack_client->port = g_list_prepend(jack_client->port,
					 port);

      g_rec_mutex_unlock(jack_client_mutex);
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

  GRecMutex *jack_client_mutex;

  jack_client = AGS_JACK_CLIENT(gobject);

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);
  
  switch(prop_id){
  case PROP_JACK_SERVER:
    {
      g_rec_mutex_lock(jack_client_mutex);

      g_value_set_object(value, jack_client->jack_server);

      g_rec_mutex_unlock(jack_client_mutex);
    }
    break;
  case PROP_CLIENT_NAME:
    {
      g_rec_mutex_lock(jack_client_mutex);

      g_value_set_string(value, jack_client->client_name);

      g_rec_mutex_unlock(jack_client_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(jack_client_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(jack_client->device,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(jack_client_mutex);
    }
    break;
  case PROP_PORT:
    {
      g_rec_mutex_lock(jack_client_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(jack_client->port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(jack_client_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
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

  jack_client = AGS_JACK_CLIENT(gobject);

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
  
  /* call parent */
  G_OBJECT_CLASS(ags_jack_client_parent_class)->finalize(gobject);
}

AgsUUID*
ags_jack_client_get_uuid(AgsConnectable *connectable)
{
  AgsJackClient *jack_client;
  
  AgsUUID *ptr;

  GRecMutex *jack_client_mutex;

  jack_client = AGS_JACK_CLIENT(connectable);

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* get UUID */
  g_rec_mutex_lock(jack_client_mutex);

  ptr = jack_client->uuid;

  g_rec_mutex_unlock(jack_client_mutex);
  
  return(ptr);
}

gboolean
ags_jack_client_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_jack_client_is_ready(AgsConnectable *connectable)
{
  AgsJackClient *jack_client;
  
  gboolean is_ready;

  GRecMutex *jack_client_mutex;

  jack_client = AGS_JACK_CLIENT(connectable);

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* test */
  g_rec_mutex_lock(jack_client_mutex);

  is_ready = (AGS_CONNECTABLE_ADDED_TO_REGISTRY & (jack_client->connectable_flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(jack_client_mutex);
  
  return(is_ready);
}

void
ags_jack_client_add_to_registry(AgsConnectable *connectable)
{
  AgsJackClient *jack_client;

  GRecMutex *jack_client_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  jack_client = AGS_JACK_CLIENT(connectable);

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* set added to registry */
  g_rec_mutex_lock(jack_client_mutex);

  jack_client->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(jack_client_mutex);
}

void
ags_jack_client_remove_from_registry(AgsConnectable *connectable)
{
  AgsJackClient *jack_client;

  GRecMutex *jack_client_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  jack_client = AGS_JACK_CLIENT(connectable);

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* set added to registry */
  g_rec_mutex_lock(jack_client_mutex);

  jack_client->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(jack_client_mutex);
}

xmlNode*
ags_jack_client_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_jack_client_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_jack_client_xml_parse(AgsConnectable *connectable,
		      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_jack_client_is_connected(AgsConnectable *connectable)
{
  AgsJackClient *jack_client;
  
  gboolean is_connected;

  GRecMutex *jack_client_mutex;

  jack_client = AGS_JACK_CLIENT(connectable);

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* test */
  g_rec_mutex_lock(jack_client_mutex);

  is_connected = (AGS_CONNECTABLE_CONNECTED & (jack_client->connectable_flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(jack_client_mutex);
  
  return(is_connected);
}

void
ags_jack_client_connect(AgsConnectable *connectable)
{
  AgsJackClient *jack_client;

  GList *list_start, *list;

  GRecMutex *jack_client_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  jack_client = AGS_JACK_CLIENT(connectable);

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* set connected */
  g_rec_mutex_lock(jack_client_mutex);

  jack_client->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(jack_client_mutex);

  /* port */
  g_rec_mutex_lock(jack_client_mutex);

  list =
    list_start = g_list_copy_deep(jack_client->port,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(jack_client_mutex);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   (GDestroyNotify) g_object_unref);
}

void
ags_jack_client_disconnect(AgsConnectable *connectable)
{

  AgsJackClient *jack_client;

  GList *list_start, *list;

  GRecMutex *jack_client_mutex;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  jack_client = AGS_JACK_CLIENT(connectable);
  
  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* unset connected */
  g_rec_mutex_lock(jack_client_mutex);

  jack_client->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(jack_client_mutex);

  /* port */
  g_rec_mutex_lock(jack_client_mutex);

  list =
    list_start = g_list_copy_deep(jack_client->port,
				  (GCopyFunc) g_object_ref,
				  NULL);

  g_rec_mutex_unlock(jack_client_mutex);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free_full(list_start,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_jack_client_test_flags:
 * @jack_client: the #AgsJackClient
 * @flags: the flags
 *
 * Test @flags to be set on @jack_client.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_jack_client_test_flags(AgsJackClient *jack_client, guint flags)
{
  gboolean retval;  
  
  GRecMutex *jack_client_mutex;

  if(!AGS_IS_JACK_CLIENT(jack_client)){
    return(FALSE);
  }

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* test */
  g_rec_mutex_lock(jack_client_mutex);

  retval = (flags & (jack_client->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(jack_client_mutex);

  return(retval);
}

/**
 * ags_jack_client_set_flags:
 * @jack_client: the #AgsJackClient
 * @flags: see #AgsJackClientFlags-enum
 *
 * Enable a feature of @jack_client.
 *
 * Since: 3.0.0
 */
void
ags_jack_client_set_flags(AgsJackClient *jack_client, guint flags)
{
  GRecMutex *jack_client_mutex;

  if(!AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(jack_client_mutex);

  jack_client->flags |= flags;
  
  g_rec_mutex_unlock(jack_client_mutex);
}
    
/**
 * ags_jack_client_unset_flags:
 * @jack_client: the #AgsJackClient
 * @flags: see #AgsJackClientFlags-enum
 *
 * Disable a feature of @jack_client.
 *
 * Since: 3.0.0
 */
void
ags_jack_client_unset_flags(AgsJackClient *jack_client, guint flags)
{  
  GRecMutex *jack_client_mutex;

  if(!AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(jack_client_mutex);

  jack_client->flags &= (~flags);
  
  g_rec_mutex_unlock(jack_client_mutex);
}

/**
 * ags_jack_client_find_uuid:
 * @jack_client: (element-type AgsAudio.JackClient) (transfer none): the #GList-struct containing #AgsJackClient
 * @client_uuid: the client uuid to find
 *
 * Finds next match of @client_uuid in @jack_client.
 *
 * Returns: (element-type AgsAudio.JackClient) (transfer none): the matching #GList-struct or %NULL
 * 
 * Since: 3.0.0
 */
GList*
ags_jack_client_find_uuid(GList *jack_client,
			  gchar *client_uuid)
{
  AgsJackClient *current_jack_client;
  
  gboolean success;
  
  GRecMutex *jack_client_mutex;

#ifdef AGS_WITH_JACK
  while(jack_client != NULL){
    current_jack_client = AGS_JACK_CLIENT(jack_client->data);
    
    /* get jack client mutex */
    jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(current_jack_client);
    
    /* check uuid */
    g_rec_mutex_lock(jack_client_mutex);
    
    success = (current_jack_client->client != NULL &&
	       !g_ascii_strcasecmp(jack_get_uuid_for_client_name(current_jack_client->client,
								 jack_get_client_name(current_jack_client->client)),
				   client_uuid)) ? TRUE: FALSE;

    g_rec_mutex_unlock(jack_client_mutex);
    
    if(success){
      return(jack_client);
    }

    jack_client = jack_client->next;
  }
#endif

  return(NULL);
}

/**
 * ags_jack_client_find:
 * @jack_client: (element-type AgsAudio.JackClient) (transfer none): the #GList-struct containing #AgsJackClient
 * @client_name: the client name to find
 *
 * Finds next match of @client_name in @jack_client.
 *
 * Returns: (element-type AgsAudio.JackClient) (transfer none): the next matching #GList-struct or %NULL
 * 
 * Since: 3.0.0
 */
GList*
ags_jack_client_find(GList *jack_client,
		     gchar *client_name)
{ 
  AgsJackClient *current_jack_client;
  
  gboolean success;
  
  GRecMutex *jack_client_mutex;

#ifdef AGS_WITH_JACK
  while(jack_client != NULL){
    current_jack_client = AGS_JACK_CLIENT(jack_client->data);
    
    /* get jack client mutex */
    jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(current_jack_client);

    /* check client name */
    g_rec_mutex_lock(jack_client_mutex);
    
    success = (current_jack_client->client != NULL &&
	       !g_ascii_strcasecmp(jack_get_client_name(current_jack_client->client),
				   client_name));

    g_rec_mutex_unlock(jack_client_mutex);
    
    if(success){
      return(jack_client);
    }

    jack_client = jack_client->next;
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
 * Since: 3.0.0
 */
void
ags_jack_client_open(AgsJackClient *jack_client,
		     gchar *client_name)
{
#ifdef AGS_WITH_JACK
  jack_client_t *client;
#else
  gpointer client;
#endif
  
  gchar *client_uuid;
  
  GRecMutex *jack_client_mutex;

  if(!AGS_IS_JACK_CLIENT(jack_client) ||
     client_name == NULL){
    return;
  }

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* check already open */
  g_rec_mutex_lock(jack_client_mutex);

  if(jack_client->client != NULL){
    g_rec_mutex_unlock(jack_client_mutex);
    
    g_message("Advanced Gtk+ Sequencer JACK client already open");
    
    return;
  } 

  g_rec_mutex_unlock(jack_client_mutex);
  
  g_message("Advanced Gtk+ Sequencer open JACK client");

  g_object_set(jack_client,
	       "client-name", client_name,
	       NULL);

#ifdef AGS_WITH_JACK
  client = jack_client_open(client_name,
			    0,
			    NULL,
			    NULL);
  
  if(client != NULL){
    client_uuid = jack_get_uuid_for_client_name(client,
						client_name);

    /* apply client and uuid */
    g_rec_mutex_lock(jack_client_mutex);
    
    jack_client->client = client;
    jack_client->client_uuid = client_uuid;
    
    g_rec_mutex_unlock(jack_client_mutex);

    /* set callbacks */
    jack_on_shutdown(client,
		     ags_jack_client_shutdown,
		     jack_client);
  
    jack_set_process_callback(client,
			      ags_jack_client_process_callback,
			      jack_client);
    jack_set_xrun_callback(client,
			   ags_jack_client_xrun_callback,
			   jack_client);
  }

#endif
}

/**
 * ags_jack_client_close:
 * @jack_client: the #AgsJackClient
 *
 * Close the JACK client's connection.
 *
 * Since: 3.0.0
 */
void
ags_jack_client_close(AgsJackClient *jack_client)
{
#ifdef AGS_WITH_JACK
  jack_client_t *client;
#else
  gpointer client;
#endif
  
  GRecMutex *jack_client_mutex;

  if(!AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* check already closed */
  g_rec_mutex_lock(jack_client_mutex);

  if(jack_client->client == NULL){
    g_rec_mutex_unlock(jack_client_mutex);
    
    g_message("Advanced Gtk+ Sequencer JACK client already closed");
    
    return;
  } 

  g_rec_mutex_unlock(jack_client_mutex);
  
  g_message("Advanced Gtk+ Sequencer close JACK client");

#ifdef AGS_WITH_JACK
  g_rec_mutex_lock(jack_client_mutex);

  client = jack_client->client;

  g_rec_mutex_unlock(jack_client_mutex);

  jack_client_close(client);

  g_rec_mutex_lock(jack_client_mutex);

  jack_client->client = NULL;
  
  g_rec_mutex_unlock(jack_client_mutex);
#endif

  g_rec_mutex_lock(jack_client_mutex);

  g_list_free_full(jack_client->device,
		   g_object_unref);
  jack_client->device = NULL;

  g_rec_mutex_unlock(jack_client_mutex);
}

/**
 * ags_jack_client_activate:
 * @jack_client: the #AgsJackClient
 *
 * Activate client.
 *
 * Since: 3.0.0
 */
void
ags_jack_client_activate(AgsJackClient *jack_client)
{
#ifdef AGS_WITH_JACK
  jack_client_t *client;
#else
  gpointer client;
#endif
  
  GList *port_start, *port;
  
  int ret;

  GRecMutex *jack_client_mutex;

  if(!AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }
  
  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* get client */
  g_rec_mutex_lock(jack_client_mutex);

  client = jack_client->client;

  g_rec_mutex_unlock(jack_client_mutex);
  
  if(ags_jack_client_test_flags(jack_client, AGS_JACK_CLIENT_ACTIVATED) ||
     client == NULL){    
    return;
  }
  
#ifdef AGS_WITH_JACK
  ret = jack_activate(client);
#else
  ret = -1;
#endif

  if(ret != 0){
    return;
  }

  g_rec_mutex_lock(jack_client_mutex);

  port =
    port_start = g_list_copy(jack_client->port);

  g_rec_mutex_unlock(jack_client_mutex);

  while(port != NULL){
    gchar *port_name;

    //TODO:JK: make thread-safe
    g_object_get(port->data,
		 "port-name", &port_name,
		 NULL);
    
    ags_jack_port_register(port->data,
			   port_name,
			   (ags_jack_port_test_flags(port->data, AGS_JACK_PORT_IS_AUDIO) ? TRUE: FALSE), (ags_jack_port_test_flags(port->data, AGS_JACK_PORT_IS_MIDI) ? TRUE: FALSE),
			   (ags_jack_port_test_flags(port->data, AGS_JACK_PORT_IS_OUTPUT) ? TRUE: FALSE));

    g_free(port_name);
    
    port = port->next;
  }

  ags_jack_client_set_flags(jack_client, AGS_JACK_CLIENT_ACTIVATED);

  g_list_free(port_start);
}

/**
 * ags_jack_client_deactivate:
 * @jack_client: the #AgsJackClient
 *
 * Deactivate client.
 *
 * Since: 3.0.0
 */
void
ags_jack_client_deactivate(AgsJackClient *jack_client)
{
#ifdef AGS_WITH_JACK
  jack_client_t *client;
#else
  gpointer client;
#endif
  
  GRecMutex *jack_client_mutex;

  if(!AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }
  
  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* get client */
  g_rec_mutex_lock(jack_client_mutex);

  client = jack_client->client;

  g_rec_mutex_unlock(jack_client_mutex);

  if(client == NULL){
    return;
  }
  
#ifdef AGS_WITH_JACK
  jack_deactivate(client);
#endif

  /* set flags */
  ags_jack_client_unset_flags(jack_client, AGS_JACK_CLIENT_ACTIVATED);
}

/**
 * ags_jack_client_add_device:
 * @jack_client: the #AgsJackClient
 * @jack_device: an #AgsJackDevout, #AgsJackDevin or #AgsJackMidiin
 *
 * Add @jack_device to @jack_client.
 *
 * Since: 3.0.0
 */
void
ags_jack_client_add_device(AgsJackClient *jack_client,
			   GObject *jack_device)
{
  GRecMutex *jack_client_mutex;

  if(!AGS_IS_JACK_CLIENT(jack_client) ||
     (!AGS_IS_JACK_DEVOUT(jack_device) &&
      !AGS_IS_JACK_MIDIIN(jack_device) &&
      !AGS_IS_JACK_DEVIN(jack_device))){
    return;
  }

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* add device */
  g_rec_mutex_lock(jack_client_mutex);

  if(g_list_find(jack_client->device, jack_device) == NULL){
    g_object_ref(jack_device);
    jack_client->device = g_list_prepend(jack_client->device,
					 jack_device);
  }
  
  g_rec_mutex_unlock(jack_client_mutex);
}

/**
 * ags_jack_client_remove_device:
 * @jack_client: the #AgsJackClient
 * @jack_device: an #AgsJackDevout, #AgsJackDevin or #AgsJackMidiin
 *
 * Remove @jack_device from @jack_client.
 *
 * Since: 3.0.0
 */
void
ags_jack_client_remove_device(AgsJackClient *jack_client,
			      GObject *jack_device)
{
  GRecMutex *jack_client_mutex;

  if(!AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);
  
  /* remove */
  g_rec_mutex_lock(jack_client_mutex);

  if(g_list_find(jack_client->device, jack_device) != NULL){
    jack_client->device = g_list_remove(jack_client->device,
					jack_device);
    g_object_unref(jack_device);
  }

  g_rec_mutex_unlock(jack_client_mutex);
}

/**
 * ags_jack_client_add_port:
 * @jack_client: the #AgsJackClient
 * @jack_port: an #AgsJackPort
 *
 * Add @jack_port to @jack_client.
 *
 * Since: 3.0.0
 */
void
ags_jack_client_add_port(AgsJackClient *jack_client,
			 GObject *jack_port)
{
  GRecMutex *jack_client_mutex;

  if(!AGS_IS_JACK_CLIENT(jack_client) ||
     !AGS_IS_JACK_PORT(jack_port)){
    return;
  }
  
  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* add port */
  g_rec_mutex_lock(jack_client_mutex);

  if(g_list_find(jack_client->port, jack_port) == NULL){
    g_object_ref(jack_port);
    jack_client->port = g_list_prepend(jack_client->port,
				       jack_port);
  }
  
  g_rec_mutex_unlock(jack_client_mutex);
}

/**
 * ags_jack_client_remove_port:
 * @jack_client: the #AgsJackClient
 * @jack_port: an #AgsJackPort
 *
 * Remove @jack_port from @jack_client.
 *
 * Since: 3.0.0
 */
void
ags_jack_client_remove_port(AgsJackClient *jack_client,
			    GObject *jack_port)
{
  GRecMutex *jack_client_mutex;

  if(!AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }
  
  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* remove port */
  g_rec_mutex_lock(jack_client_mutex);

  if(g_list_find(jack_client->port, jack_port) != NULL){
    jack_client->port = g_list_remove(jack_client->port,
				      jack_port);
    g_object_unref(jack_port);
  }
  
  g_rec_mutex_unlock(jack_client_mutex);
}

#ifdef AGS_WITH_JACK
void
ags_jack_client_shutdown(void *ptr)
{
  AgsJackClient *jack_client;

  GList *port_start, *port;

  GRecMutex *jack_client_mutex;
  
  jack_client = ptr;

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* unset flags */
  g_rec_mutex_lock(jack_client_mutex);
  
  jack_client->flags &= (~AGS_JACK_CLIENT_ACTIVATED);

  port =
    port_start = g_list_copy(jack_client->port);

  g_rec_mutex_unlock(jack_client_mutex);

  while(port != NULL){
    ags_jack_port_unset_flags(port->data, AGS_JACK_PORT_REGISTERED);
    
    port = port->next;
  }

  g_list_free(port_start);
}

int
ags_jack_client_process_callback(jack_nframes_t nframes, void *ptr)
{
  AgsJackServer *jack_server;
  AgsJackClient *jack_client;
  AgsJackPort *jack_port;
  AgsJackDevout *jack_devout;
  AgsJackDevin *jack_devin;
  AgsJackMidiin *jack_midiin;
  
  AgsAudioLoop *audio_loop;

  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;
  
  jack_client_t *client;
  jack_default_audio_sample_t *out, *in;
  jack_midi_event_t in_event;

  GList *device_start, *device;
  GList *port_start, *port;

  void *port_buf;

  jack_nframes_t event_count;
  guint time_spent;
  guint copy_mode;
  guint word_size;
  guint event_size;
  guint i, j;
  guint nth_buffer;
  gboolean no_event;
  
  GRecMutex *jack_client_mutex;
  GRecMutex *device_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;
  
  if(ptr == NULL){
    return(0);
  }
  
  jack_client = ptr;
  
  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* get jack server */
  g_rec_mutex_lock(jack_client_mutex);

  jack_server = (AgsJackServer *) jack_client->jack_server;

  device_start = g_list_copy(jack_client->device);

  g_rec_mutex_unlock(jack_client_mutex);

  /* get application context */
  application_context = ags_application_context_get_instance();

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
  audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  device = device_start;
  
  if(device == NULL){
    g_atomic_int_dec_and_test(&(jack_client->queued));

    g_object_unref(audio_loop);
    g_object_unref(task_launcher);
    
    return(0);
  }

  ags_thread_set_flags(audio_loop, AGS_THREAD_TIME_ACCOUNTING);

  /*
   * process MIDI and audio input
   */
  /* get device */
  device = device_start;  

  while(device != NULL){
    /* get device mutex */
    if(AGS_IS_JACK_MIDIIN(device->data)){
      device_mutex = AGS_JACK_MIDIIN_GET_OBJ_MUTEX(device->data);
    }else{
      device = device->next;

      continue;
    }
    
    /*  */
    g_rec_mutex_lock(device_mutex);

    jack_midiin = NULL;

    if(AGS_IS_JACK_MIDIIN(device->data)){
      jack_midiin = (AgsJackMidiin *) device->data;

      /* wait callback */      
      no_event = TRUE;
      
      if((AGS_JACK_MIDIIN_PASS_THROUGH & (g_atomic_int_get(&(jack_midiin->sync_flags)))) == 0){
	callback_mutex = &(jack_midiin->callback_mutex);

	g_rec_mutex_unlock(device_mutex);
	
	/* give back computing time until ready */
	g_mutex_lock(callback_mutex);
    
	if((AGS_JACK_MIDIIN_CALLBACK_DONE & (g_atomic_int_get(&(jack_midiin->sync_flags)))) == 0){
	  g_atomic_int_or(&(jack_midiin->sync_flags),
			  AGS_JACK_MIDIIN_CALLBACK_WAIT);
    
	  while((AGS_JACK_MIDIIN_CALLBACK_DONE & (g_atomic_int_get(&(jack_midiin->sync_flags)))) == 0 &&
		(AGS_JACK_MIDIIN_CALLBACK_WAIT & (g_atomic_int_get(&(jack_midiin->sync_flags)))) != 0){
	    g_cond_wait(&(jack_midiin->callback_cond),
			callback_mutex);
	  }
	}
    
	g_atomic_int_and(&(jack_midiin->sync_flags),
			 (~(AGS_JACK_MIDIIN_CALLBACK_WAIT |
			    AGS_JACK_MIDIIN_CALLBACK_DONE)));
	  
	g_mutex_unlock(callback_mutex);

	no_event = FALSE;
	
	g_rec_mutex_lock(device_mutex);
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
		nth_buffer = 0;
		
		if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_0){
		  nth_buffer = 1;
		}else if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_1){
		  nth_buffer = 2;
		}else if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_2){
		  nth_buffer = 3;
		}else if(jack_midiin->app_buffer_mode == AGS_JACK_MIDIIN_APP_BUFFER_3){
		  nth_buffer = 0;
		}

		if(ceil((jack_midiin->app_buffer_size[nth_buffer] + in_event.size) / 4096.0) > ceil(jack_midiin->app_buffer_size[nth_buffer] / 4096.0)){
		  if(jack_midiin->app_buffer[nth_buffer] == NULL){
		    jack_midiin->app_buffer[nth_buffer] = g_malloc(4096 * sizeof(char));
		  }else{
		    jack_midiin->app_buffer[nth_buffer] = g_realloc(jack_midiin->app_buffer[nth_buffer],
								    (ceil(jack_midiin->app_buffer_size[nth_buffer] / 4096.0) * 4096 + 4096) * sizeof(char));
		  }
		}

		memcpy(&(jack_midiin->app_buffer[nth_buffer][jack_midiin->app_buffer_size[nth_buffer]]),
		       in_event.buffer,
		       in_event.size);
		jack_midiin->app_buffer_size[nth_buffer] += in_event.size;
	      }

	    }	  

	    jack_midi_clear_buffer(port_buf);
	
	    port = port->next;
	  }

	  /* signal finish */
	  callback_finish_mutex = &(jack_midiin->callback_finish_mutex);
	
	  g_mutex_lock(callback_finish_mutex);

	  g_atomic_int_or(&(jack_midiin->sync_flags),
			  AGS_JACK_MIDIIN_CALLBACK_FINISH_DONE);
    
	  if((AGS_JACK_MIDIIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_midiin->sync_flags)))) != 0){
	    g_cond_signal(&(jack_midiin->callback_finish_cond));
	  }

	  g_mutex_unlock(callback_finish_mutex);
	}
      }
    }
        
    g_rec_mutex_unlock(device_mutex);

    /* iterate */
    device = device->next;
  }

  /*
   * process audio input
   */
  /* get device */
  device = device_start;  

  while(device != NULL){
    if(AGS_IS_JACK_DEVIN(device->data)){
      device_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(device->data);
    }else{
      device = device->next;

      continue;
    }

    /*  */
    g_rec_mutex_lock(device_mutex);

    jack_devin = NULL;
    
    if(AGS_IS_JACK_DEVIN(device->data)){
      jack_devin = (AgsJackDevin *) device->data;

      /* wait callback */      
      no_event = TRUE;

      if((AGS_JACK_DEVIN_PASS_THROUGH & (g_atomic_int_get(&(jack_devin->sync_flags)))) == 0){
	callback_mutex = &(jack_devin->callback_mutex);

	g_rec_mutex_unlock(device_mutex);
	
	/* give back computing time until ready */
	g_mutex_lock(callback_mutex);
    
	if((AGS_JACK_DEVIN_CALLBACK_DONE & (g_atomic_int_get(&(jack_devin->sync_flags)))) == 0){
	  g_atomic_int_or(&(jack_devin->sync_flags),
			  AGS_JACK_DEVIN_CALLBACK_WAIT);
    
	  while((AGS_JACK_DEVIN_CALLBACK_DONE & (g_atomic_int_get(&(jack_devin->sync_flags)))) == 0 &&
		(AGS_JACK_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(jack_devin->sync_flags)))) != 0){
	    g_cond_wait(&(jack_devin->callback_cond),
			callback_mutex);
	  }
	}
    
	g_atomic_int_and(&(jack_devin->sync_flags),
			 (~(AGS_JACK_DEVIN_CALLBACK_WAIT |
			    AGS_JACK_DEVIN_CALLBACK_DONE)));
    
	g_mutex_unlock(callback_mutex);

	no_event = FALSE;

	g_rec_mutex_lock(device_mutex);
      }
      
      /* get buffer */
      if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_0){
	nth_buffer = 1;
      }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_1){
	nth_buffer = 2;
      }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_2){
	nth_buffer = 3;
      }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_3){
	nth_buffer = 0;
      }else{
	/* iterate */
	g_rec_mutex_unlock(device_mutex);
	
	device = device->next;
	
	continue;
      }

      /* get copy mode */
      copy_mode = ags_audio_buffer_util_get_copy_mode(ags_audio_buffer_util_format_from_soundcard(jack_devin->format),
						      AGS_AUDIO_BUFFER_UTIL_FLOAT);

      /* check buffer flag */
      switch(jack_devin->format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  word_size = sizeof(gint8);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  word_size = sizeof(gint16);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
	{
	  word_size = sizeof(gint32);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
	{
	  word_size = sizeof(gint32);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_64_BIT:
	{
	  word_size = sizeof(gint64);
	}
	break;
      default:
	g_rec_mutex_unlock(device_mutex);
	
	/* iterate */
	device = device->next;
	
	continue;
      }

      /* retrieve buffer */
      port = jack_devin->jack_port;
      
      for(i = 0; port != NULL; i++){
	jack_port = port->data;
	
	in = jack_port_get_buffer(jack_port->port,
				  jack_devin->buffer_size);
	
	if(!no_event && in != NULL){
	  ags_soundcard_lock_buffer(AGS_SOUNDCARD(jack_devin), jack_devin->app_buffer[nth_buffer]);
	    
	  ags_audio_buffer_util_copy_buffer_to_buffer(jack_devin->app_buffer[nth_buffer], jack_devin->pcm_channels, i,
						      in, 1, 0,
						      jack_devin->buffer_size, copy_mode);
	    
	  ags_soundcard_unlock_buffer(AGS_SOUNDCARD(jack_devin), jack_devin->app_buffer[nth_buffer]);	    
	}

	port = port->next;
      }

      /* clear buffer */
      port = jack_devin->jack_port;
      
      for(i = 0; port != NULL; i++){
	jack_port = port->data;
	
	out = jack_port_get_buffer(jack_port->port,
				   jack_devin->buffer_size);

	if(out != NULL){
	  ags_audio_buffer_util_clear_float(out, 1,
					    jack_devin->buffer_size);
	}

	port = port->next;
      }
    
      if(!no_event){
	/* signal finish */
	callback_finish_mutex = &(jack_devin->callback_finish_mutex);
	
	g_mutex_lock(callback_finish_mutex);

	g_atomic_int_or(&(jack_devin->sync_flags),
			AGS_JACK_DEVIN_CALLBACK_FINISH_DONE);
    
	if((AGS_JACK_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_devin->sync_flags)))) != 0){
	  g_cond_signal(&(jack_devin->callback_finish_cond));
	}

	g_mutex_unlock(callback_finish_mutex);
      }
    }
        
    g_rec_mutex_unlock(device_mutex);

    /* iterate */
    device = device->next;
  }
  
  /*
   * process audio output
   */
  /* get device */
  device = device_start;  

  while(device != NULL){
    if(AGS_IS_JACK_DEVOUT(device->data)){
      device_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(device->data);
    }else{
      device = device->next;

      continue;
    }

    /*  */
    g_rec_mutex_lock(device_mutex);

    jack_devout = NULL;
    
    if(AGS_IS_JACK_DEVOUT(device->data)){
      jack_devout = (AgsJackDevout *) device->data;

      /* wait callback */      
      no_event = TRUE;

      if((AGS_JACK_DEVOUT_PASS_THROUGH & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0){
	callback_mutex = &(jack_devout->callback_mutex);

	g_rec_mutex_unlock(device_mutex);
	
	/* give back computing time until ready */
	g_mutex_lock(callback_mutex);
    
	if((AGS_JACK_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0){
	  g_atomic_int_or(&(jack_devout->sync_flags),
			  AGS_JACK_DEVOUT_CALLBACK_WAIT);
    
	  while((AGS_JACK_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0 &&
		(AGS_JACK_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(jack_devout->sync_flags)))) != 0){
	    g_cond_wait(&(jack_devout->callback_cond),
			callback_mutex);
	  }
	}
    
	g_atomic_int_and(&(jack_devout->sync_flags),
			 (~(AGS_JACK_DEVOUT_CALLBACK_WAIT |
			    AGS_JACK_DEVOUT_CALLBACK_DONE)));
    
	g_mutex_unlock(callback_mutex);

	no_event = FALSE;

	g_rec_mutex_lock(device_mutex);
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
      if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_0){
	nth_buffer = 3;
      }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_1){
	nth_buffer = 0;
      }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_2){
	nth_buffer = 1;
      }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_3){
	nth_buffer = 2;
      }else{
	/* iterate */
	g_rec_mutex_unlock(device_mutex);
	
	device = device->next;
	
	continue;
      }

      /* get copy mode */
      copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						      ags_audio_buffer_util_format_from_soundcard(jack_devout->format));

      /* check buffer flag */
      switch(jack_devout->format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  word_size = sizeof(gint8);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  word_size = sizeof(gint16);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
	{
	  word_size = sizeof(gint32);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
	{
	  word_size = sizeof(gint32);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_64_BIT:
	{
	  word_size = sizeof(gint64);
	}
	break;
      default:
	g_rec_mutex_unlock(device_mutex);
	
	/* iterate */
	device = device->next;
	
	continue;
      }

      /* fill buffer */
      port = jack_devout->jack_port;
      
      for(i = 0; port != NULL; i++){
	jack_port = port->data;
	
	out = jack_port_get_buffer(jack_port->port,
				   jack_devout->buffer_size);
	
	if(!no_event && out != NULL){
	  ags_soundcard_lock_buffer(AGS_SOUNDCARD(jack_devout), jack_devout->app_buffer[nth_buffer]);
	    
	  ags_audio_buffer_util_copy_buffer_to_buffer(out, 1, 0,
						      jack_devout->app_buffer[nth_buffer], jack_devout->pcm_channels, i,
						      jack_devout->buffer_size, copy_mode);
	  
	  ags_soundcard_unlock_buffer(AGS_SOUNDCARD(jack_devout), jack_devout->app_buffer[nth_buffer]);	    
	}

	port = port->next;
      }
    
      if(!no_event){
	/* signal finish */
	callback_finish_mutex = &(jack_devout->callback_finish_mutex);
	
	g_mutex_lock(callback_finish_mutex);

	g_atomic_int_or(&(jack_devout->sync_flags),
			AGS_JACK_DEVOUT_CALLBACK_FINISH_DONE);
    
	if((AGS_JACK_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_devout->sync_flags)))) != 0){
	  g_cond_signal(&(jack_devout->callback_finish_cond));
	}

	g_mutex_unlock(callback_finish_mutex);
      }
    }
        
    g_rec_mutex_unlock(device_mutex);

    /* iterate */
    device = device->next;
  }

  g_list_free(device_start);
  g_atomic_int_dec_and_test(&(jack_client->queued));

  /* unref */
  g_object_unref(audio_loop);
  g_object_unref(task_launcher);

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

  //TODO:JK: implement me
  
  return(0);
}
#endif

/**
 * ags_jack_client_new:
 * @jack_server: the assigned #AgsJackServer
 *
 * Create a new instance of #AgsJackClient.
 *
 * Returns: the new #AgsJackClient
 *
 * Since: 3.0.0
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
