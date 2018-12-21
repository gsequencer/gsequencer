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

#include <ags/audio/pulse/ags_pulse_client.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/pulse/ags_pulse_server.h>
#include <ags/audio/pulse/ags_pulse_port.h>
#include <ags/audio/pulse/ags_pulse_devout.h>
#include <ags/audio/pulse/ags_pulse_devin.h>

#include <ags/i18n.h>

void ags_pulse_client_class_init(AgsPulseClientClass *pulse_client);
void ags_pulse_client_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pulse_client_init(AgsPulseClient *pulse_client);
void ags_pulse_client_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_pulse_client_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_pulse_client_dispose(GObject *gobject);
void ags_pulse_client_finalize(GObject *gobject);

AgsUUID* ags_pulse_client_get_uuid(AgsConnectable *connectable);
gboolean ags_pulse_client_has_resource(AgsConnectable *connectable);
gboolean ags_pulse_client_is_ready(AgsConnectable *connectable);
void ags_pulse_client_add_to_registry(AgsConnectable *connectable);
void ags_pulse_client_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_pulse_client_list_resource(AgsConnectable *connectable);
xmlNode* ags_pulse_client_xml_compose(AgsConnectable *connectable);
void ags_pulse_client_xml_parse(AgsConnectable *connectable,
				xmlNode *node);
gboolean ags_pulse_client_is_connected(AgsConnectable *connectable);
void ags_pulse_client_connect(AgsConnectable *connectable);
void ags_pulse_client_disconnect(AgsConnectable *connectable);

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
  PROP_CLIENT_NAME,
  PROP_DEVICE,
  PROP_PORT,
};

static gpointer ags_pulse_client_parent_class = NULL;

static pthread_mutex_t ags_pulse_client_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_pulse_client_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_pulse_client = 0;

    static const GTypeInfo ags_pulse_client_info = {
      sizeof(AgsPulseClientClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pulse_client_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPulseClient),
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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_pulse_client);
  }

  return g_define_type_id__volatile;
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
   * Since: 2.0.0
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
   * AgsPulseClient:client-name:
   *
   * The pulseaudio client name.
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
   * AgsPulseClient:device:
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
   * AgsPulseClient:port:
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
ags_pulse_client_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_pulse_client_get_uuid;
  connectable->has_resource = ags_pulse_client_has_resource;

  connectable->is_ready = ags_pulse_client_is_ready;
  connectable->add_to_registry = ags_pulse_client_add_to_registry;
  connectable->remove_from_registry = ags_pulse_client_remove_from_registry;

  connectable->list_resource = ags_pulse_client_list_resource;
  connectable->xml_compose = ags_pulse_client_xml_compose;
  connectable->xml_parse = ags_pulse_client_xml_parse;

  connectable->is_connected = ags_pulse_client_is_connected;  
  connectable->connect = ags_pulse_client_connect;
  connectable->disconnect = ags_pulse_client_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_pulse_client_init(AgsPulseClient *pulse_client)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* flags */
  pulse_client->flags = 0;

  /* client mutex */
  pulse_client->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  pulse_client->obj_mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* server */
  pulse_client->pulse_server = NULL;
  
  /* uuid */
  pulse_client->uuid = ags_uuid_alloc();
  ags_uuid_generate(pulse_client->uuid);

  /* client name and uuid */
  pulse_client->client_uuid = ags_id_generator_create_uuid();
  pulse_client->client_name = NULL;

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

  pthread_mutex_t *pulse_client_mutex;

  pulse_client = AGS_PULSE_CLIENT(gobject);

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  switch(prop_id){
  case PROP_PULSE_SERVER:
    {
      AgsPulseServer *pulse_server;

      pulse_server = (AgsPulseServer *) g_value_get_object(value);

      pthread_mutex_lock(pulse_client_mutex);
      
      if(pulse_client->pulse_server == (GObject *) pulse_server){
	pthread_mutex_unlock(pulse_client_mutex);
	
	return;
      }

      if(pulse_client->pulse_server != NULL){
	g_object_unref(pulse_client->pulse_server);
      }

      if(pulse_server != NULL){
	g_object_ref(pulse_server);
      }
      
      pulse_client->pulse_server = (GObject *) pulse_server;

      pthread_mutex_unlock(pulse_client_mutex);
    }
    break;
  case PROP_CLIENT_NAME:
    {
      char *client_name;

      client_name = (char *) g_value_get_string(value);

      pthread_mutex_lock(pulse_client_mutex);

      g_free(pulse_client->client_name);
	
      pulse_client->client_name = g_strdup(client_name);

      pthread_mutex_unlock(pulse_client_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      GObject *device;

      device = (GObject *) g_value_get_pointer(value);

      pthread_mutex_lock(pulse_client_mutex);

      if(device == NULL ||
	 g_list_find(pulse_client->device,
		     device) != NULL){
	pthread_mutex_unlock(pulse_client_mutex);

	return;
      }

      g_object_ref(device);
	
      pulse_client->device = g_list_prepend(pulse_client->device,
					    device);

      pthread_mutex_unlock(pulse_client_mutex);
    }
    break;
  case PROP_PORT:
    {
      GObject *port;

      port = (GObject *) g_value_get_pointer(value);

      pthread_mutex_lock(pulse_client_mutex);

      if(!AGS_IS_PULSE_PORT(port) ||
	 g_list_find(pulse_client->port,
		     port) != NULL){
	pthread_mutex_unlock(pulse_client_mutex);

	return;
      }

      g_object_ref(port);	
      pulse_client->port = g_list_prepend(pulse_client->port,
					  port);

      pthread_mutex_unlock(pulse_client_mutex);
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

  pthread_mutex_t *pulse_client_mutex;

  pulse_client = AGS_PULSE_CLIENT(gobject);

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());
  
  switch(prop_id){
  case PROP_PULSE_SERVER:
    {
      pthread_mutex_lock(pulse_client_mutex);

      g_value_set_object(value, pulse_client->pulse_server);

      pthread_mutex_unlock(pulse_client_mutex);
    }
    break;
  case PROP_CLIENT_NAME:
    {
      pthread_mutex_lock(pulse_client_mutex);

      g_value_set_string(value, pulse_client->client_name);

      pthread_mutex_unlock(pulse_client_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      pthread_mutex_lock(pulse_client_mutex);

      g_value_set_pointer(value,
			  g_list_copy(pulse_client->device));

      pthread_mutex_unlock(pulse_client_mutex);
    }
    break;
  case PROP_PORT:
    {
      pthread_mutex_lock(pulse_client_mutex);

      g_value_set_pointer(value,
			  g_list_copy(pulse_client->port));

      pthread_mutex_unlock(pulse_client_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
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

  pulse_client = AGS_PULSE_CLIENT(gobject);

  pthread_mutex_destroy(pulse_client->obj_mutex);
  free(pulse_client->obj_mutex);

  pthread_mutexattr_destroy(pulse_client->obj_mutexattr);
  free(pulse_client->obj_mutexattr);

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
  
  /* call parent */
  G_OBJECT_CLASS(ags_pulse_client_parent_class)->finalize(gobject);
}

AgsUUID*
ags_pulse_client_get_uuid(AgsConnectable *connectable)
{
  AgsPulseClient *pulse_client;
  
  AgsUUID *ptr;

  pthread_mutex_t *pulse_client_mutex;

  pulse_client = AGS_PULSE_CLIENT(connectable);

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(pulse_client_mutex);

  ptr = pulse_client->uuid;

  pthread_mutex_unlock(pulse_client_mutex);
  
  return(ptr);
}

gboolean
ags_pulse_client_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_pulse_client_is_ready(AgsConnectable *connectable)
{
  AgsPulseClient *pulse_client;
  
  gboolean is_ready;

  pthread_mutex_t *pulse_client_mutex;

  pulse_client = AGS_PULSE_CLIENT(connectable);

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  /* check is added */
  pthread_mutex_lock(pulse_client_mutex);

  is_ready = (((AGS_PULSE_CLIENT_ADDED_TO_REGISTRY & (pulse_client->flags)) != 0) ? TRUE: FALSE);

  pthread_mutex_unlock(pulse_client_mutex);
  
  return(is_ready);
}

void
ags_pulse_client_add_to_registry(AgsConnectable *connectable)
{
  AgsPulseClient *pulse_client;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  pulse_client = AGS_PULSE_CLIENT(connectable);

  ags_pulse_client_set_flags(pulse_client, AGS_PULSE_CLIENT_ADDED_TO_REGISTRY);
}

void
ags_pulse_client_remove_from_registry(AgsConnectable *connectable)
{
  AgsPulseClient *pulse_client;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  pulse_client = AGS_PULSE_CLIENT(connectable);

  ags_pulse_client_unset_flags(pulse_client, AGS_PULSE_CLIENT_ADDED_TO_REGISTRY);
}

xmlNode*
ags_pulse_client_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_pulse_client_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_pulse_client_xml_parse(AgsConnectable *connectable,
			   xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_pulse_client_is_connected(AgsConnectable *connectable)
{
  AgsPulseClient *pulse_client;
  
  gboolean is_connected;

  pthread_mutex_t *pulse_client_mutex;

  pulse_client = AGS_PULSE_CLIENT(connectable);

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(pulse_client_mutex);

  is_connected = (((AGS_PULSE_CLIENT_CONNECTED & (pulse_client->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(pulse_client_mutex);
  
  return(is_connected);
}

void
ags_pulse_client_connect(AgsConnectable *connectable)
{
  AgsPulseClient *pulse_client;

  GList *list_start, *list;

  pthread_mutex_t *pulse_client_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  pulse_client = AGS_PULSE_CLIENT(connectable);

  ags_pulse_client_set_flags(pulse_client, AGS_PULSE_CLIENT_CONNECTED);

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  /* port */
  pthread_mutex_lock(pulse_client_mutex);

  list =
    list_start = g_list_copy(pulse_client->port);

  pthread_mutex_unlock(pulse_client_mutex);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_pulse_client_disconnect(AgsConnectable *connectable)
{

  AgsPulseClient *pulse_client;

  GList *list_start, *list;

  pthread_mutex_t *pulse_client_mutex;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  pulse_client = AGS_PULSE_CLIENT(connectable);
  
  ags_pulse_client_unset_flags(pulse_client, AGS_PULSE_CLIENT_CONNECTED);

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  /* port */
  pthread_mutex_lock(pulse_client_mutex);

  list =
    list_start = g_list_copy(pulse_client->port);

  pthread_mutex_unlock(pulse_client_mutex);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_pulse_client_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_pulse_client_get_class_mutex()
{
  return(&ags_pulse_client_class_mutex);
}

/**
 * ags_pulse_client_test_flags:
 * @pulse_client: the #AgsPulseClient
 * @flags: the flags
 *
 * Test @flags to be set on @pulse_client.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_pulse_client_test_flags(AgsPulseClient *pulse_client, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *pulse_client_mutex;

  if(!AGS_IS_PULSE_CLIENT(pulse_client)){
    return(FALSE);
  }

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  /* test */
  pthread_mutex_lock(pulse_client_mutex);

  retval = (flags & (pulse_client->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(pulse_client_mutex);

  return(retval);
}

/**
 * ags_pulse_client_set_flags:
 * @pulse_client: the #AgsPulseClient
 * @flags: see #AgsPulseClientFlags-enum
 *
 * Enable a feature of @pulse_client.
 *
 * Since: 2.0.0
 */
void
ags_pulse_client_set_flags(AgsPulseClient *pulse_client, guint flags)
{
  pthread_mutex_t *pulse_client_mutex;

  if(!AGS_IS_PULSE_CLIENT(pulse_client)){
    return;
  }

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(pulse_client_mutex);

  pulse_client->flags |= flags;
  
  pthread_mutex_unlock(pulse_client_mutex);
}
    
/**
 * ags_pulse_client_unset_flags:
 * @pulse_client: the #AgsPulseClient
 * @flags: see #AgsPulseClientFlags-enum
 *
 * Disable a feature of @pulse_client.
 *
 * Since: 2.0.0
 */
void
ags_pulse_client_unset_flags(AgsPulseClient *pulse_client, guint flags)
{  
  pthread_mutex_t *pulse_client_mutex;

  if(!AGS_IS_PULSE_CLIENT(pulse_client)){
    return;
  }

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(pulse_client_mutex);

  pulse_client->flags &= (~flags);
  
  pthread_mutex_unlock(pulse_client_mutex);
}

/**
 * ags_pulse_client_find_uuid:
 * @pulse_client: the #GList-struct containing #AgsPulseClient
 * @client_uuid: the client uuid to find
 *
 * Finds next match of @client_uuid in @pulse_client.
 *
 * Returns: the next matching #GList-struct or %NULL
 * 
 * Since: 2.0.0
 */
GList*
ags_pulse_client_find_uuid(GList *pulse_client,
			   gchar *client_uuid)
{
  AgsPulseClient *current_pulse_client;
  
  gboolean success;
  
  pthread_mutex_t *pulse_client_mutex;

  while(pulse_client != NULL){
    current_pulse_client = AGS_PULSE_CLIENT(pulse_client->data);
    
    /* get pulse client mutex */
    pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
    pulse_client_mutex = current_pulse_client->obj_mutex;
  
    pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

    /* check uuid */
    pthread_mutex_lock(pulse_client_mutex);
    
    success = (current_pulse_client->context != NULL &&
	       !g_ascii_strcasecmp(current_pulse_client->client_uuid,
				   client_uuid)) ? TRUE: FALSE;

    pthread_mutex_unlock(pulse_client_mutex);

    if(success){
      return(pulse_client);
    }

    pulse_client = pulse_client->next;
  }

  return(NULL);
}

/**
 * ags_pulse_client_find:
 * @pulse_client: the #GList-struct containing #AgsPulseClient
 * @client_name: the client name to find
 *
 * Finds next match of @client_name in @pulse_client.
 *
 * Returns: the next matching #GList-struct or %NULL
 * 
 * Since: 2.0.0
 */
GList*
ags_pulse_client_find(GList *pulse_client,
		      gchar *client_name)
{ 
  AgsPulseClient *current_pulse_client;
  
  gboolean success;
  
  pthread_mutex_t *pulse_client_mutex;

  while(pulse_client != NULL){
    current_pulse_client = AGS_PULSE_CLIENT(pulse_client->data);
    
    /* get pulse client mutex */
    pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
    pulse_client_mutex = current_pulse_client->obj_mutex;
  
    pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

    /* check client name */
    pthread_mutex_lock(pulse_client_mutex);

    success = (current_pulse_client->context != NULL &&
	       !g_ascii_strcasecmp(current_pulse_client->client_name,
				   client_name)) ? TRUE: FALSE;
    
    pthread_mutex_unlock(pulse_client_mutex);
    
    if(success){
      return(pulse_client);
    }

    pulse_client = pulse_client->next;
  }

  return(NULL);
}

#ifdef AGS_WITH_PULSE
void
ags_pulse_client_state_callback(pa_context *c, AgsPulseClient *pulse_client)
{
  pa_context_state_t state;
  
  pthread_mutex_t *pulse_client_mutex;

  state = pa_context_get_state(c);

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

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
    pthread_mutex_lock(pulse_client_mutex);
    
    pulse_client->flags |= AGS_PULSE_CLIENT_READY;

    pthread_mutex_unlock(pulse_client_mutex);

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
 * Since: 2.0.0
 */
void
ags_pulse_client_open(AgsPulseClient *pulse_client,
		      gchar *client_name)
{
  AgsPulseServer *pulse_server;
  
#ifdef AGS_WITH_PULSE
  pa_context *context;
  pa_mainloop *main_loop;
  pa_mainloop_api *main_loop_api;
#else
  gpointer context;
  gpointer main_loop;
  gpointer main_loop_api;
#endif

  gchar *client_uuid;

  pthread_mutex_t *pulse_server_mutex;
  pthread_mutex_t *pulse_client_mutex;

  if(!AGS_IS_PULSE_CLIENT(pulse_client) ||
     client_name == NULL){
    return;
  }

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  /* check already open */
  pthread_mutex_lock(pulse_client_mutex);

  if(pulse_client->context != NULL){
    pthread_mutex_unlock(pulse_client_mutex);

    g_message("Advanced Gtk+ Sequencer pulseaudio client already open");
    
    return;
  } 

  pulse_server = pulse_client->pulse_server;
  
  pthread_mutex_unlock(pulse_client_mutex);
  
  g_message("Advanced Gtk+ Sequencer open pulseaudio client");
  
  g_object_set(pulse_client,
	       "client-name", client_name,
	       NULL);

  /* get pulse server mutex */
  pthread_mutex_lock(ags_pulse_server_get_class_mutex());
  
  pulse_server_mutex = pulse_server->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_server_get_class_mutex());

  /* get main loop API */
  pthread_mutex_lock(pulse_server_mutex);

  main_loop = pulse_server->main_loop;
  main_loop_api = pulse_server->main_loop_api;
  
  pthread_mutex_unlock(pulse_server_mutex);

  /* new context */
  pthread_mutex_lock(pulse_client_mutex);

#ifdef AGS_WITH_PULSE
  context = 
    pulse_client->context = pa_context_new(main_loop_api, client_name);
#else
  context = 
    pulse_client->context = NULL;
#endif

  pthread_mutex_unlock(pulse_client_mutex);  

  if(context != NULL){
#ifdef AGS_WITH_PULSE
    pa_context_connect(context,
		       NULL,
		       0,
		       NULL);
    pa_context_set_state_callback(context,
				  ags_pulse_client_state_callback,
				  pulse_client);
    
    while(!ags_pulse_client_test_flags(pulse_client, AGS_PULSE_CLIENT_READY)){
      if(!ags_pulse_client_test_flags(pulse_client, AGS_PULSE_CLIENT_READY)){
	pa_mainloop_iterate(main_loop,
			    TRUE,
			    NULL);
      }
    }
#endif
  }
}

/**
 * ags_pulse_client_close:
 * @pulse_client: the #AgsPulseClient
 *
 * Close the pulseaudio client's connection.
 *
 * Since: 2.1.14
 */
void
ags_pulse_client_close(AgsPulseClient *pulse_client)
{
  AgsPulseServer *pulse_server;

#ifdef AGS_WITH_PULSE
  pa_context *context;
#else
  gpointer context;
#endif

  GList *start_device;
  
  pthread_mutex_t *pulse_client_mutex;

  if(!AGS_IS_PULSE_CLIENT(pulse_client)){
    return;
  }

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  /* check already closed */
  pthread_mutex_lock(pulse_client_mutex);

  if(pulse_client->context == NULL){
    pthread_mutex_unlock(pulse_client_mutex);

    g_message("Advanced Gtk+ Sequencer pulseaudio client already closed");
    
    return;
  } 

  pulse_server = pulse_client->pulse_server;
  
  pthread_mutex_unlock(pulse_client_mutex);
  
  g_message("Advanced Gtk+ Sequencer close pulseaudio client");

  /* unset context */
#ifdef AGS_WITH_PULSE
  pthread_mutex_lock(pulse_client_mutex);

  context = pulse_client->context;

  pthread_mutex_unlock(pulse_client_mutex);  

  pa_context_disconnect(context);

  pthread_mutex_lock(pulse_client_mutex);

  pulse_client->context = NULL;
  
  pthread_mutex_unlock(pulse_client_mutex);
#endif

  g_object_get(pulse_client,
	       "device", &start_device,
	       NULL);

  g_list_free_full(start_device,
		   g_object_unref);

  pthread_mutex_lock(pulse_client_mutex);

  g_list_free(pulse_client->device);
  pulse_client->device = NULL;

  pthread_mutex_unlock(pulse_client_mutex);
}

/**
 * ags_pulse_client_activate:
 * @pulse_client: the #AgsPulseClient
 *
 * Activate client.
 *
 * Since: 2.0.0
 */
void
ags_pulse_client_activate(AgsPulseClient *pulse_client)
{
#ifdef AGS_WITH_PULSE
  pa_context *context;
#else
  gpointer context;
#endif

  GList *port_start, *port;
  
  int ret;

  pthread_mutex_t *pulse_client_mutex;

  if(!AGS_IS_PULSE_CLIENT(pulse_client)){
    return;
  }

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  /* get context */
  pthread_mutex_lock(pulse_client_mutex);

  context = pulse_client->context;
  
  pthread_mutex_unlock(pulse_client_mutex);

  if(ags_pulse_client_test_flags(pulse_client, AGS_PULSE_CLIENT_ACTIVATED) ||
     context == NULL){
    return;
  }  

  pthread_mutex_lock(pulse_client_mutex);

  port =
    port_start = g_list_copy(pulse_client->port);

  pthread_mutex_unlock(pulse_client_mutex);

  while(port != NULL){
    gchar *port_name;

    //TODO:JK: make thread-safe
    g_object_get(port->data,
		 "port-name", &port_name,
		 NULL);

    ags_pulse_port_register(port->data,
			    port_name,
			    (ags_pulse_port_test_flags(port->data, AGS_PULSE_PORT_IS_AUDIO) ? TRUE: FALSE), (ags_pulse_port_test_flags(port->data, AGS_PULSE_PORT_IS_MIDI) ? TRUE: FALSE),
			    (ags_pulse_port_test_flags(port->data, AGS_PULSE_PORT_IS_OUTPUT) ? TRUE: FALSE));
    
    port = port->next;
  }

  ags_pulse_client_set_flags(pulse_client, AGS_PULSE_CLIENT_ACTIVATED);

  g_list_free(port_start);
}

/**
 * ags_pulse_client_deactivate:
 * @pulse_client: the #AgsPulseClient
 *
 * Deactivate client.
 *
 * Since: 2.0.0
 */
void
ags_pulse_client_deactivate(AgsPulseClient *pulse_client)
{
#ifdef AGS_WITH_PULSE
  pa_context *context;
#else
  gpointer context;
#endif

  pthread_mutex_t *pulse_client_mutex;

  if(!AGS_IS_PULSE_CLIENT(pulse_client)){
    return;
  }
  
  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  /* get context */
  pthread_mutex_lock(pulse_client_mutex);

  context = pulse_client->context;
  
  pthread_mutex_unlock(pulse_client_mutex);

  if(context == NULL){
    return;
  }
  
  ags_pulse_client_unset_flags(pulse_client, (AGS_PULSE_CLIENT_ACTIVATED |
					      AGS_PULSE_CLIENT_READY));
}

/**
 * ags_pulse_client_add_device:
 * @pulse_client: the #AgsPulseClient
 * @pulse_device: an #AgsPulseDevout or #AgsPulseDevin
 *
 * Add @pulse_device to @pulse_client.
 *
 * Since: 2.0.0
 */
void
ags_pulse_client_add_device(AgsPulseClient *pulse_client,
			    GObject *pulse_device)
{
  pthread_mutex_t *pulse_client_mutex;

  if(!AGS_IS_PULSE_CLIENT(pulse_client) ||
     (!AGS_IS_PULSE_DEVOUT(pulse_device) &&
      !AGS_IS_PULSE_DEVIN(pulse_device))){
    return;
  }

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  /* add device */
  pthread_mutex_lock(pulse_client_mutex);

  if(g_list_find(pulse_client->device, pulse_device) == NULL){
    g_object_ref(pulse_device);
    pulse_client->device = g_list_prepend(pulse_client->device,
					  pulse_device);
  }
  
  pthread_mutex_unlock(pulse_client_mutex);
}

/**
 * ags_pulse_client_remove_device:
 * @pulse_client: the #AgsPulseClient
 * @pulse_device: an #AgsPulseDevout or #AgsPulseDevin
 *
 * Remove @pulse_device from @pulse_client.
 *
 * Since: 2.0.0
 */
void
ags_pulse_client_remove_device(AgsPulseClient *pulse_client,
			       GObject *pulse_device)
{
  pthread_mutex_t *pulse_client_mutex;

  if(!AGS_IS_PULSE_CLIENT(pulse_client)){
    return;
  }

  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());
  
  /* remove */
  pthread_mutex_lock(pulse_client_mutex);

  if(g_list_find(pulse_client->device, pulse_device) != NULL){
    pulse_client->device = g_list_remove(pulse_client->device,
					 pulse_device);
    g_object_unref(pulse_device);
  }

  pthread_mutex_unlock(pulse_client_mutex);
}

/**
 * ags_pulse_client_add_port:
 * @pulse_client: the #AgsPulseClient
 * @pulse_port: an #AgsPulsePort
 *
 * Add @pulse_port to @pulse_client.
 *
 * Since: 2.0.0
 */
void
ags_pulse_client_add_port(AgsPulseClient *pulse_client,
			  GObject *pulse_port)
{
  pthread_mutex_t *pulse_client_mutex;

  if(!AGS_IS_PULSE_CLIENT(pulse_client) ||
     !AGS_IS_PULSE_PORT(pulse_port)){
    return;
  }
  
  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  /* add port */
  pthread_mutex_lock(pulse_client_mutex);

  if(g_list_find(pulse_client->port, pulse_port) == NULL){
    g_object_ref(pulse_port);
    pulse_client->port = g_list_prepend(pulse_client->port,
					pulse_port);
  }
  
  pthread_mutex_unlock(pulse_client_mutex);
}

/**
 * ags_pulse_client_remove_port:
 * @pulse_client: the #AgsPulseClient
 * @pulse_port: an #AgsPulsePort
 *
 * Remove @pulse_port from @pulse_client.
 *
 * Since: 2.0.0
 */
void
ags_pulse_client_remove_port(AgsPulseClient *pulse_client,
			     GObject *pulse_port)
{
  pthread_mutex_t *pulse_client_mutex;

  if(!AGS_IS_PULSE_CLIENT(pulse_client)){
    return;
  }
  
  /* get pulse client mutex */
  pthread_mutex_lock(ags_pulse_client_get_class_mutex());
  
  pulse_client_mutex = pulse_client->obj_mutex;
  
  pthread_mutex_unlock(ags_pulse_client_get_class_mutex());

  /* remove port */
  pthread_mutex_lock(pulse_client_mutex);

  if(g_list_find(pulse_client->port, pulse_port) != NULL){
    pulse_client->port = g_list_remove(pulse_client->port,
				       pulse_port);
    g_object_unref(pulse_port);
  }
  
  pthread_mutex_unlock(pulse_client_mutex);
}

/**
 * ags_pulse_client_new:
 * @pulse_server: the assigned #AgsPulseServer
 *
 * Create a new instance of #AgsPulseClient.
 *
 * Returns: the new #AgsPulseClient
 *
 * Since: 2.0.0
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
