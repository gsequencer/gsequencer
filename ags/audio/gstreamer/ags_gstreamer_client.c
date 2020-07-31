/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/gstreamer/ags_gstreamer_client.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/gstreamer/ags_gstreamer_server.h>
#include <ags/audio/gstreamer/ags_gstreamer_port.h>
#include <ags/audio/gstreamer/ags_gstreamer_devout.h>
#include <ags/audio/gstreamer/ags_gstreamer_devin.h>

#include <ags/i18n.h>

void ags_gstreamer_client_class_init(AgsGstreamerClientClass *gstreamer_client);
void ags_gstreamer_client_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_gstreamer_client_init(AgsGstreamerClient *gstreamer_client);
void ags_gstreamer_client_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_gstreamer_client_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_gstreamer_client_dispose(GObject *gobject);
void ags_gstreamer_client_finalize(GObject *gobject);

AgsUUID* ags_gstreamer_client_get_uuid(AgsConnectable *connectable);
gboolean ags_gstreamer_client_has_resource(AgsConnectable *connectable);
gboolean ags_gstreamer_client_is_ready(AgsConnectable *connectable);
void ags_gstreamer_client_add_to_registry(AgsConnectable *connectable);
void ags_gstreamer_client_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_gstreamer_client_list_resource(AgsConnectable *connectable);
xmlNode* ags_gstreamer_client_xml_compose(AgsConnectable *connectable);
void ags_gstreamer_client_xml_parse(AgsConnectable *connectable,
				    xmlNode *node);
gboolean ags_gstreamer_client_is_connected(AgsConnectable *connectable);
void ags_gstreamer_client_connect(AgsConnectable *connectable);
void ags_gstreamer_client_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_gstreamer_client
 * @short_description: gstreamer connection
 * @title: AgsGstreamerClient
 * @section_id:
 * @include: ags/audio/gstreamer/ags_gstreamer_client.h
 *
 * The #AgsGstreamerClient communicates with a gstreamer instance.
 */

enum{
  PROP_0,
  PROP_GSTREAMER_SERVER,
  PROP_CLIENT_NAME,
  PROP_DEVICE,
  PROP_PORT,
};

static gpointer ags_gstreamer_client_parent_class = NULL;

GType
ags_gstreamer_client_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_gstreamer_client = 0;

    static const GTypeInfo ags_gstreamer_client_info = {
      sizeof(AgsGstreamerClientClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gstreamer_client_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsGstreamerClient),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gstreamer_client_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_gstreamer_client_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_gstreamer_client = g_type_register_static(G_TYPE_OBJECT,
						       "AgsGstreamerClient",
						       &ags_gstreamer_client_info,
						       0);

    g_type_add_interface_static(ags_type_gstreamer_client,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_gstreamer_client);
  }

  return g_define_type_id__volatile;
}

void
ags_gstreamer_client_class_init(AgsGstreamerClientClass *gstreamer_client)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_gstreamer_client_parent_class = g_type_class_peek_parent(gstreamer_client);

  /* GObjectClass */
  gobject = (GObjectClass *) gstreamer_client;

  gobject->set_property = ags_gstreamer_client_set_property;
  gobject->get_property = ags_gstreamer_client_get_property;

  gobject->dispose = ags_gstreamer_client_dispose;
  gobject->finalize = ags_gstreamer_client_finalize;

  /* properties */
  /**
   * AgsGstreamerClient:gstreamer-server:
   *
   * The assigned #AgsGstreamerServer.
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_object("gstreamer-server",
				   i18n_pspec("assigned gstreamer server"),
				   i18n_pspec("The assigned gstreamer server"),
				   AGS_TYPE_GSTREAMER_SERVER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GSTREAMER_SERVER,
				  param_spec);

  /**
   * AgsGstreamerClient:client-name:
   *
   * The gstreamer client name.
   * 
   * Since: 3.6.0
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
   * AgsGstreamerClient:device: (type GList(GObject)) (transfer full)
   *
   * The assigned devices.
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_pointer("device",
				    i18n_pspec("assigned device"),
				    i18n_pspec("The assigned device"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);

  /**
   * AgsGstreamerClient:port: (type GList(AgsGstreamerPort)) (transfer full)
   *
   * The assigned ports.
   * 
   * Since: 3.6.0
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
ags_gstreamer_client_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_gstreamer_client_get_uuid;
  connectable->has_resource = ags_gstreamer_client_has_resource;

  connectable->is_ready = ags_gstreamer_client_is_ready;
  connectable->add_to_registry = ags_gstreamer_client_add_to_registry;
  connectable->remove_from_registry = ags_gstreamer_client_remove_from_registry;

  connectable->list_resource = ags_gstreamer_client_list_resource;
  connectable->xml_compose = ags_gstreamer_client_xml_compose;
  connectable->xml_parse = ags_gstreamer_client_xml_parse;

  connectable->is_connected = ags_gstreamer_client_is_connected;  
  connectable->connect = ags_gstreamer_client_connect;
  connectable->disconnect = ags_gstreamer_client_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_gstreamer_client_init(AgsGstreamerClient *gstreamer_client)
{

  /* flags */
  gstreamer_client->flags = 0;

  /* client mutex */
  g_rec_mutex_init(&(gstreamer_client->obj_mutex));

  /* server */
  gstreamer_client->gstreamer_server = NULL;
  
  /* uuid */
  gstreamer_client->uuid = ags_uuid_alloc();
  ags_uuid_generate(gstreamer_client->uuid);

  /* client name and uuid */
  gstreamer_client->client_uuid = ags_id_generator_create_uuid();
  gstreamer_client->client_name = NULL;

  /* client */
  gstreamer_client->pipeline = NULL;
  
  /* device */
  gstreamer_client->device = NULL;
  gstreamer_client->port = NULL;
}

void
ags_gstreamer_client_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsGstreamerClient *gstreamer_client;

  GRecMutex *gstreamer_client_mutex;

  gstreamer_client = AGS_GSTREAMER_CLIENT(gobject);

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  switch(prop_id){
  case PROP_GSTREAMER_SERVER:
  {
    AgsGstreamerServer *gstreamer_server;

    gstreamer_server = (AgsGstreamerServer *) g_value_get_object(value);

    g_rec_mutex_lock(gstreamer_client_mutex);
      
    if(gstreamer_client->gstreamer_server == (GObject *) gstreamer_server){
      g_rec_mutex_unlock(gstreamer_client_mutex);
	
      return;
    }

    if(gstreamer_client->gstreamer_server != NULL){
      g_object_unref(gstreamer_client->gstreamer_server);
    }

    if(gstreamer_server != NULL){
      g_object_ref(gstreamer_server);
    }
      
    gstreamer_client->gstreamer_server = (GObject *) gstreamer_server;

    g_rec_mutex_unlock(gstreamer_client_mutex);
  }
  break;
  case PROP_CLIENT_NAME:
  {
    char *client_name;

    client_name = (char *) g_value_get_string(value);

    g_rec_mutex_lock(gstreamer_client_mutex);

    g_free(gstreamer_client->client_name);
	
    gstreamer_client->client_name = g_strdup(client_name);

    g_rec_mutex_unlock(gstreamer_client_mutex);
  }
  break;
  case PROP_DEVICE:
  {
    GObject *device;

    device = (GObject *) g_value_get_pointer(value);

    g_rec_mutex_lock(gstreamer_client_mutex);

    if(device == NULL ||
       g_list_find(gstreamer_client->device,
		   device) != NULL){
      g_rec_mutex_unlock(gstreamer_client_mutex);

      return;
    }

    g_object_ref(device);
	
    gstreamer_client->device = g_list_prepend(gstreamer_client->device,
					      device);

    g_rec_mutex_unlock(gstreamer_client_mutex);
  }
  break;
  case PROP_PORT:
  {
    GObject *port;

    port = (GObject *) g_value_get_pointer(value);

    g_rec_mutex_lock(gstreamer_client_mutex);

    if(!AGS_IS_GSTREAMER_PORT(port) ||
       g_list_find(gstreamer_client->port,
		   port) != NULL){
      g_rec_mutex_unlock(gstreamer_client_mutex);

      return;
    }

    g_object_ref(port);	
    gstreamer_client->port = g_list_prepend(gstreamer_client->port,
					    port);

    g_rec_mutex_unlock(gstreamer_client_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_gstreamer_client_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsGstreamerClient *gstreamer_client;

  GRecMutex *gstreamer_client_mutex;

  gstreamer_client = AGS_GSTREAMER_CLIENT(gobject);

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);
  
  switch(prop_id){
  case PROP_GSTREAMER_SERVER:
  {
    g_rec_mutex_lock(gstreamer_client_mutex);

    g_value_set_object(value, gstreamer_client->gstreamer_server);

    g_rec_mutex_unlock(gstreamer_client_mutex);
  }
  break;
  case PROP_CLIENT_NAME:
  {
    g_rec_mutex_lock(gstreamer_client_mutex);

    g_value_set_string(value, gstreamer_client->client_name);

    g_rec_mutex_unlock(gstreamer_client_mutex);
  }
  break;
  case PROP_DEVICE:
  {
    g_rec_mutex_lock(gstreamer_client_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(gstreamer_client->device,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(gstreamer_client_mutex);
  }
  break;
  case PROP_PORT:
  {
    g_rec_mutex_lock(gstreamer_client_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(gstreamer_client->port,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(gstreamer_client_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_gstreamer_client_dispose(GObject *gobject)
{
  AgsGstreamerClient *gstreamer_client;

  GList *list;
  
  gstreamer_client = AGS_GSTREAMER_CLIENT(gobject);

  /* gstreamer server */
  if(gstreamer_client->gstreamer_server != NULL){
    g_object_unref(gstreamer_client->gstreamer_server);

    gstreamer_client->gstreamer_server = NULL;
  }

  /* device */
  if(gstreamer_client->device != NULL){
    list = gstreamer_client->device;

    while(list != NULL){
      g_object_set(G_OBJECT(list->data),
		   "gstreamer-client", NULL,
		   NULL);

      list = list->next;
    }

    g_list_free_full(gstreamer_client->device,
		     g_object_unref);

    gstreamer_client->device = NULL;
  }

  /* port */
  if(gstreamer_client->port != NULL){
    list = gstreamer_client->port;

    while(list != NULL){
      g_object_run_dispose(G_OBJECT(list->data));

      list = list->next;
    }
    
    g_list_free_full(gstreamer_client->port,
		     g_object_unref);

    gstreamer_client->port = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_client_parent_class)->dispose(gobject);
}

void
ags_gstreamer_client_finalize(GObject *gobject)
{
  AgsGstreamerClient *gstreamer_client;

  gstreamer_client = AGS_GSTREAMER_CLIENT(gobject);

  /* gstreamer server */
  if(gstreamer_client->gstreamer_server != NULL){
    g_object_unref(gstreamer_client->gstreamer_server);
  }

  /* device */
  if(gstreamer_client->device != NULL){
    g_list_free_full(gstreamer_client->device,
		     g_object_unref);
  }

  /* port */
  if(gstreamer_client->port != NULL){
    g_list_free_full(gstreamer_client->port,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_client_parent_class)->finalize(gobject);
}

AgsUUID*
ags_gstreamer_client_get_uuid(AgsConnectable *connectable)
{
  AgsGstreamerClient *gstreamer_client;
  
  AgsUUID *ptr;

  GRecMutex *gstreamer_client_mutex;

  gstreamer_client = AGS_GSTREAMER_CLIENT(connectable);

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  /* get UUID */
  g_rec_mutex_lock(gstreamer_client_mutex);

  ptr = gstreamer_client->uuid;

  g_rec_mutex_unlock(gstreamer_client_mutex);
  
  return(ptr);
}

gboolean
ags_gstreamer_client_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_gstreamer_client_is_ready(AgsConnectable *connectable)
{
  AgsGstreamerClient *gstreamer_client;
  
  gboolean is_ready;

  gstreamer_client = AGS_GSTREAMER_CLIENT(connectable);

  /* check is added */
  is_ready = ags_gstreamer_client_test_flags(gstreamer_client, AGS_GSTREAMER_CLIENT_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_gstreamer_client_add_to_registry(AgsConnectable *connectable)
{
  AgsGstreamerClient *gstreamer_client;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  gstreamer_client = AGS_GSTREAMER_CLIENT(connectable);

  ags_gstreamer_client_set_flags(gstreamer_client, AGS_GSTREAMER_CLIENT_ADDED_TO_REGISTRY);
}

void
ags_gstreamer_client_remove_from_registry(AgsConnectable *connectable)
{
  AgsGstreamerClient *gstreamer_client;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  gstreamer_client = AGS_GSTREAMER_CLIENT(connectable);

  ags_gstreamer_client_unset_flags(gstreamer_client, AGS_GSTREAMER_CLIENT_ADDED_TO_REGISTRY);
}

xmlNode*
ags_gstreamer_client_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_gstreamer_client_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_gstreamer_client_xml_parse(AgsConnectable *connectable,
			       xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_gstreamer_client_is_connected(AgsConnectable *connectable)
{
  AgsGstreamerClient *gstreamer_client;
  
  gboolean is_connected;

  gstreamer_client = AGS_GSTREAMER_CLIENT(connectable);

  /* check is connected */
  is_connected = ags_gstreamer_client_test_flags(gstreamer_client, AGS_GSTREAMER_CLIENT_CONNECTED);
  
  return(is_connected);
}

void
ags_gstreamer_client_connect(AgsConnectable *connectable)
{
  AgsGstreamerClient *gstreamer_client;

  GList *list_start, *list;

  GRecMutex *gstreamer_client_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  gstreamer_client = AGS_GSTREAMER_CLIENT(connectable);

  ags_gstreamer_client_set_flags(gstreamer_client, AGS_GSTREAMER_CLIENT_CONNECTED);

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  /* port */
  g_rec_mutex_lock(gstreamer_client_mutex);

  list =
    list_start = g_list_copy(gstreamer_client->port);

  g_rec_mutex_unlock(gstreamer_client_mutex);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_gstreamer_client_disconnect(AgsConnectable *connectable)
{

  AgsGstreamerClient *gstreamer_client;

  GList *list_start, *list;

  GRecMutex *gstreamer_client_mutex;
  
  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  gstreamer_client = AGS_GSTREAMER_CLIENT(connectable);
  
  ags_gstreamer_client_unset_flags(gstreamer_client, AGS_GSTREAMER_CLIENT_CONNECTED);

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  /* port */
  g_rec_mutex_lock(gstreamer_client_mutex);

  list =
    list_start = g_list_copy(gstreamer_client->port);

  g_rec_mutex_unlock(gstreamer_client_mutex);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));
    
    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_gstreamer_client_test_flags:
 * @gstreamer_client: the #AgsGstreamerClient
 * @flags: the flags
 *
 * Test @flags to be set on @gstreamer_client.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.6.0
 */
gboolean
ags_gstreamer_client_test_flags(AgsGstreamerClient *gstreamer_client, guint flags)
{
  gboolean retval;  
  
  GRecMutex *gstreamer_client_mutex;

  if(!AGS_IS_GSTREAMER_CLIENT(gstreamer_client)){
    return(FALSE);
  }

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  /* test */
  g_rec_mutex_lock(gstreamer_client_mutex);

  retval = (flags & (gstreamer_client->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(gstreamer_client_mutex);

  return(retval);
}

/**
 * ags_gstreamer_client_set_flags:
 * @gstreamer_client: the #AgsGstreamerClient
 * @flags: see #AgsGstreamerClientFlags-enum
 *
 * Enable a feature of @gstreamer_client.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_client_set_flags(AgsGstreamerClient *gstreamer_client, guint flags)
{
  GRecMutex *gstreamer_client_mutex;

  if(!AGS_IS_GSTREAMER_CLIENT(gstreamer_client)){
    return;
  }

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(gstreamer_client_mutex);

  gstreamer_client->flags |= flags;
  
  g_rec_mutex_unlock(gstreamer_client_mutex);
}
    
/**
 * ags_gstreamer_client_unset_flags:
 * @gstreamer_client: the #AgsGstreamerClient
 * @flags: see #AgsGstreamerClientFlags-enum
 *
 * Disable a feature of @gstreamer_client.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_client_unset_flags(AgsGstreamerClient *gstreamer_client, guint flags)
{  
  GRecMutex *gstreamer_client_mutex;

  if(!AGS_IS_GSTREAMER_CLIENT(gstreamer_client)){
    return;
  }

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(gstreamer_client_mutex);

  gstreamer_client->flags &= (~flags);
  
  g_rec_mutex_unlock(gstreamer_client_mutex);
}

/**
 * ags_gstreamer_client_find_uuid:
 * @gstreamer_client: (element-type AgsAudio.GstreamerClient) (transfer none): the #GList-struct containing #AgsGstreamerClient
 * @client_uuid: the client uuid to find
 *
 * Finds next match of @client_uuid in @gstreamer_client.
 *
 * Returns: (element-type AgsAudio.GstreamerClient) (transfer none): the next matching #GList-struct or %NULL
 * 
 * Since: 3.6.0
 */
GList*
ags_gstreamer_client_find_uuid(GList *gstreamer_client,
			       gchar *client_uuid)
{
  AgsGstreamerClient *current_gstreamer_client;
  
  gboolean success;
  
  GRecMutex *gstreamer_client_mutex;

  while(gstreamer_client != NULL){
    current_gstreamer_client = AGS_GSTREAMER_CLIENT(gstreamer_client->data);
    
    /* get gstreamer client mutex */
    gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(current_gstreamer_client);

    /* check uuid */
    g_rec_mutex_lock(gstreamer_client_mutex);
    
    success = (!g_ascii_strcasecmp(current_gstreamer_client->client_uuid,
				   client_uuid)) ? TRUE: FALSE;

    g_rec_mutex_unlock(gstreamer_client_mutex);

    if(success){
      return(gstreamer_client);
    }

    gstreamer_client = gstreamer_client->next;
  }

  return(NULL);
}

/**
 * ags_gstreamer_client_find:
 * @gstreamer_client: (element-type AgsAudio.GstreamerClient) (transfer none): the #GList-struct containing #AgsGstreamerClient
 * @client_name: the client name to find
 *
 * Finds next match of @client_name in @gstreamer_client.
 *
 * Returns: (element-type AgsAudio.GstreamerClient) (transfer none): the next matching #GList-struct or %NULL
 * 
 * Since: 3.6.0
 */
GList*
ags_gstreamer_client_find(GList *gstreamer_client,
			  gchar *client_name)
{ 
  AgsGstreamerClient *current_gstreamer_client;
  
  gboolean success;
  
  GRecMutex *gstreamer_client_mutex;

  while(gstreamer_client != NULL){
    current_gstreamer_client = AGS_GSTREAMER_CLIENT(gstreamer_client->data);
    
    /* get gstreamer client mutex */
    gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(current_gstreamer_client);

    /* check client name */
    g_rec_mutex_lock(gstreamer_client_mutex);

    success = (!g_ascii_strcasecmp(current_gstreamer_client->client_name,
				   client_name)) ? TRUE: FALSE;
    
    g_rec_mutex_unlock(gstreamer_client_mutex);
    
    if(success){
      return(gstreamer_client);
    }

    gstreamer_client = gstreamer_client->next;
  }

  return(NULL);
}

/**
 * ags_gstreamer_client_open:
 * @gstreamer_client: the #AgsGstreamerClient
 * @client_name: the client's name
 *
 * Open the gstreamer client's connection and read uuid.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_client_open(AgsGstreamerClient *gstreamer_client,
			  gchar *client_name)
{
  AgsGstreamerServer *gstreamer_server;

  gchar *client_uuid;

  GRecMutex *gstreamer_server_mutex;
  GRecMutex *gstreamer_client_mutex;

  if(!AGS_IS_GSTREAMER_CLIENT(gstreamer_client) ||
     client_name == NULL){
    return;
  }

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  /* check already open */
  g_rec_mutex_lock(gstreamer_client_mutex);

  if(gstreamer_client->pipeline != NULL){
    g_rec_mutex_unlock(gstreamer_client_mutex);

    g_message("Advanced Gtk+ Sequencer gstreamer client already open");
    
    return;
  } 

  gstreamer_server = (AgsGstreamerServer *) gstreamer_client->gstreamer_server;
  
  g_rec_mutex_unlock(gstreamer_client_mutex);
  
  g_message("Advanced Gtk+ Sequencer open gstreamer client");
  
  g_object_set(gstreamer_client,
	       "client-name", client_name,
	       NULL);

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  /* new pipeline */
  g_rec_mutex_lock(gstreamer_client_mutex);
  
  //TODO:JK: implement me
  
  g_rec_mutex_unlock(gstreamer_client_mutex);  
}

/**
 * ags_gstreamer_client_close:
 * @gstreamer_client: the #AgsGstreamerClient
 *
 * Close the gstreamer client's connection.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_client_close(AgsGstreamerClient *gstreamer_client)
{
  AgsGstreamerServer *gstreamer_server;

  GList *start_device;
  
  GRecMutex *gstreamer_client_mutex;

  if(!AGS_IS_GSTREAMER_CLIENT(gstreamer_client)){
    return;
  }

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  /* check already closed */
  g_rec_mutex_lock(gstreamer_client_mutex);

  if(gstreamer_client->pipeline == NULL){
    g_rec_mutex_unlock(gstreamer_client_mutex);

    g_message("Advanced Gtk+ Sequencer gstreamer client already closed");
    
    return;
  } 

  gstreamer_server = (AgsGstreamerServer *) gstreamer_client->gstreamer_server;
  
  g_rec_mutex_unlock(gstreamer_client_mutex);
  
  g_message("Advanced Gtk+ Sequencer close gstreamer client");

  /* unset context */
#if defined(AGS_WITH_GSTREAMER)
  //TODO:JK: implement me
  
  g_rec_mutex_lock(gstreamer_client_mutex);

  gstreamer_client->pipeline = NULL;
  
  g_rec_mutex_unlock(gstreamer_client_mutex);
#endif

  g_rec_mutex_lock(gstreamer_client_mutex);

  g_list_free_full(gstreamer_client->device,
		   g_object_unref);
  gstreamer_client->device = NULL;

  g_rec_mutex_unlock(gstreamer_client_mutex);
}

/**
 * ags_gstreamer_client_activate:
 * @gstreamer_client: the #AgsGstreamerClient
 *
 * Activate client.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_client_activate(AgsGstreamerClient *gstreamer_client)
{
  GList *port_start, *port;

  GRecMutex *gstreamer_client_mutex;

  if(!AGS_IS_GSTREAMER_CLIENT(gstreamer_client)){
    return;
  }

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  if(ags_gstreamer_client_test_flags(gstreamer_client, AGS_GSTREAMER_CLIENT_ACTIVATED)){
    return;
  }  

  g_rec_mutex_lock(gstreamer_client_mutex);

  port =
    port_start = g_list_copy(gstreamer_client->port);

  g_rec_mutex_unlock(gstreamer_client_mutex);

  while(port != NULL){
    gchar *port_name;

    //TODO:JK: make thread-safe
    g_object_get(port->data,
		 "port-name", &port_name,
		 NULL);

    ags_gstreamer_port_register(port->data,
				port_name,
				(ags_gstreamer_port_test_flags(port->data, AGS_GSTREAMER_PORT_IS_AUDIO) ? TRUE: FALSE), (ags_gstreamer_port_test_flags(port->data, AGS_GSTREAMER_PORT_IS_MIDI) ? TRUE: FALSE),
				(ags_gstreamer_port_test_flags(port->data, AGS_GSTREAMER_PORT_IS_OUTPUT) ? TRUE: FALSE));

    g_free(port_name);
    
    port = port->next;
  }

  //TODO:JK: implement me

  ags_gstreamer_client_set_flags(gstreamer_client, AGS_GSTREAMER_CLIENT_ACTIVATED);

  g_list_free(port_start);
}

/**
 * ags_gstreamer_client_deactivate:
 * @gstreamer_client: the #AgsGstreamerClient
 *
 * Deactivate client.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_client_deactivate(AgsGstreamerClient *gstreamer_client)
{
  GRecMutex *gstreamer_client_mutex;

  if(!AGS_IS_GSTREAMER_CLIENT(gstreamer_client)){
    return;
  }
  
  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  /* get context */
  //TODO:JK: implement me
  
  ags_gstreamer_client_unset_flags(gstreamer_client, (AGS_GSTREAMER_CLIENT_ACTIVATED |
						      AGS_GSTREAMER_CLIENT_READY));
}

/**
 * ags_gstreamer_client_add_device:
 * @gstreamer_client: the #AgsGstreamerClient
 * @gstreamer_device: an #AgsGstreamerDevout or #AgsGstreamerDevin
 *
 * Add @gstreamer_device to @gstreamer_client.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_client_add_device(AgsGstreamerClient *gstreamer_client,
				GObject *gstreamer_device)
{
  GRecMutex *gstreamer_client_mutex;

  if(!AGS_IS_GSTREAMER_CLIENT(gstreamer_client) ||
     (!AGS_IS_GSTREAMER_DEVOUT(gstreamer_device) &&
      !AGS_IS_GSTREAMER_DEVIN(gstreamer_device))){
    return;
  }

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  /* add device */
  g_rec_mutex_lock(gstreamer_client_mutex);

  if(g_list_find(gstreamer_client->device, gstreamer_device) == NULL){
    g_object_ref(gstreamer_device);
    gstreamer_client->device = g_list_prepend(gstreamer_client->device,
					      gstreamer_device);
  }
  
  g_rec_mutex_unlock(gstreamer_client_mutex);
}

/**
 * ags_gstreamer_client_remove_device:
 * @gstreamer_client: the #AgsGstreamerClient
 * @gstreamer_device: an #AgsGstreamerDevout or #AgsGstreamerDevin
 *
 * Remove @gstreamer_device from @gstreamer_client.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_client_remove_device(AgsGstreamerClient *gstreamer_client,
				   GObject *gstreamer_device)
{
  GRecMutex *gstreamer_client_mutex;

  if(!AGS_IS_GSTREAMER_CLIENT(gstreamer_client)){
    return;
  }

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);
  
  /* remove */
  g_rec_mutex_lock(gstreamer_client_mutex);

  if(g_list_find(gstreamer_client->device, gstreamer_device) != NULL){
    gstreamer_client->device = g_list_remove(gstreamer_client->device,
					     gstreamer_device);
    g_object_unref(gstreamer_device);
  }

  g_rec_mutex_unlock(gstreamer_client_mutex);
}

/**
 * ags_gstreamer_client_add_port:
 * @gstreamer_client: the #AgsGstreamerClient
 * @gstreamer_port: an #AgsGstreamerPort
 *
 * Add @gstreamer_port to @gstreamer_client.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_client_add_port(AgsGstreamerClient *gstreamer_client,
			      GObject *gstreamer_port)
{
  GRecMutex *gstreamer_client_mutex;

  if(!AGS_IS_GSTREAMER_CLIENT(gstreamer_client) ||
     !AGS_IS_GSTREAMER_PORT(gstreamer_port)){
    return;
  }
  
  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  /* add port */
  g_rec_mutex_lock(gstreamer_client_mutex);

  if(g_list_find(gstreamer_client->port, gstreamer_port) == NULL){
    g_object_ref(gstreamer_port);
    gstreamer_client->port = g_list_prepend(gstreamer_client->port,
					    gstreamer_port);
  }
  
  g_rec_mutex_unlock(gstreamer_client_mutex);
}

/**
 * ags_gstreamer_client_remove_port:
 * @gstreamer_client: the #AgsGstreamerClient
 * @gstreamer_port: an #AgsGstreamerPort
 *
 * Remove @gstreamer_port from @gstreamer_client.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_client_remove_port(AgsGstreamerClient *gstreamer_client,
				 GObject *gstreamer_port)
{
  GRecMutex *gstreamer_client_mutex;

  if(!AGS_IS_GSTREAMER_CLIENT(gstreamer_client)){
    return;
  }
  
  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  /* remove port */
  g_rec_mutex_lock(gstreamer_client_mutex);

  if(g_list_find(gstreamer_client->port, gstreamer_port) != NULL){
    gstreamer_client->port = g_list_remove(gstreamer_client->port,
					   gstreamer_port);
    g_object_unref(gstreamer_port);
  }
  
  g_rec_mutex_unlock(gstreamer_client_mutex);
}

/**
 * ags_gstreamer_client_new:
 * @gstreamer_server: the assigned #AgsGstreamerServer
 *
 * Create a new instance of #AgsGstreamerClient.
 *
 * Returns: the new #AgsGstreamerClient
 *
 * Since: 3.6.0
 */
AgsGstreamerClient*
ags_gstreamer_client_new(GObject *gstreamer_server)
{
  AgsGstreamerClient *gstreamer_client;

  gstreamer_client = (AgsGstreamerClient *) g_object_new(AGS_TYPE_GSTREAMER_CLIENT,
							 "gstreamer-server", gstreamer_server,
							 NULL);

  return(gstreamer_client);
}
