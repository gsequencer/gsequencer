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

#include <ags/audio/gstreamer/ags_gstreamer_server.h>
#include <ags/audio/gstreamer/ags_gstreamer_client.h>
#include <ags/audio/gstreamer/ags_gstreamer_port.h>

#include <ags/audio/gstreamer/ags_gstreamer_devout.h>
#include <ags/audio/gstreamer/ags_gstreamer_devin.h>

#include <string.h>

#include <errno.h>

#include <ags/i18n.h>

void ags_gstreamer_server_class_init(AgsGstreamerServerClass *gstreamer_server);
void ags_gstreamer_server_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_gstreamer_server_sound_server_interface_init(AgsSoundServerInterface *sound_server);
void ags_gstreamer_server_init(AgsGstreamerServer *gstreamer_server);
void ags_gstreamer_server_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_gstreamer_server_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_gstreamer_server_dispose(GObject *gobject);
void ags_gstreamer_server_finalize(GObject *gobject);

AgsUUID* ags_gstreamer_server_get_uuid(AgsConnectable *connectable);
gboolean ags_gstreamer_server_has_resource(AgsConnectable *connectable);
gboolean ags_gstreamer_server_is_ready(AgsConnectable *connectable);
void ags_gstreamer_server_add_to_registry(AgsConnectable *connectable);
void ags_gstreamer_server_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_gstreamer_server_list_resource(AgsConnectable *connectable);
xmlNode* ags_gstreamer_server_xml_compose(AgsConnectable *connectable);
void ags_gstreamer_server_xml_parse(AgsConnectable *connectable,
				    xmlNode *node);
gboolean ags_gstreamer_server_is_connected(AgsConnectable *connectable);
void ags_gstreamer_server_connect(AgsConnectable *connectable);
void ags_gstreamer_server_disconnect(AgsConnectable *connectable);

void ags_gstreamer_server_set_url(AgsSoundServer *sound_server,
				  gchar *url);
gchar* ags_gstreamer_server_get_url(AgsSoundServer *sound_server);
void ags_gstreamer_server_set_ports(AgsSoundServer *sound_server,
				    guint *ports, guint port_count);
guint* ags_gstreamer_server_get_ports(AgsSoundServer *sound_server,
				      guint *port_count);
void ags_gstreamer_server_set_soundcard(AgsSoundServer *sound_server,
					gchar *client_uuid,
					GList *soundcard);
GList* ags_gstreamer_server_get_soundcard(AgsSoundServer *sound_server,
					  gchar *client_uuid);
void ags_gstreamer_server_set_sequencer(AgsSoundServer *sound_server,
					gchar *client_uuid,
					GList *sequencer);
GList* ags_gstreamer_server_get_sequencer(AgsSoundServer *sound_server,
					  gchar *client_uuid);
GObject* ags_gstreamer_server_register_soundcard(AgsSoundServer *sound_server,
						 gboolean is_output);
void ags_gstreamer_server_unregister_soundcard(AgsSoundServer *sound_server,
					       GObject *soundcard);
GObject* ags_gstreamer_server_register_sequencer(AgsSoundServer *sound_server,
						 gboolean is_output);
void ags_gstreamer_server_unregister_sequencer(AgsSoundServer *sound_server,
					       GObject *sequencer);

/**
 * SECTION:ags_gstreamer_server
 * @short_description: gstreamer instance
 * @title: AgsGstreamerServer
 * @section_id:
 * @include: ags/audio/gstreamer/ags_gstreamer_server.h
 *
 * The #AgsGstreamerServer is an object to represent a running gstreamer instance.
 */

enum{
  PROP_0,
  PROP_URL,
  PROP_DEFAULT_SOUNDCARD,
  PROP_DEFAULT_GSTREAMER_CLIENT,
  PROP_GSTREAMER_CLIENT,
};

static gpointer ags_gstreamer_server_parent_class = NULL;

GType
ags_gstreamer_server_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_gstreamer_server = 0;

    static const GTypeInfo ags_gstreamer_server_info = {
      sizeof(AgsGstreamerServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gstreamer_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsGstreamerServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gstreamer_server_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_gstreamer_server_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_sound_server_interface_info = {
      (GInterfaceInitFunc) ags_gstreamer_server_sound_server_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_gstreamer_server = g_type_register_static(G_TYPE_OBJECT,
						       "AgsGstreamerServer",
						       &ags_gstreamer_server_info,
						       0);

    g_type_add_interface_static(ags_type_gstreamer_server,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_gstreamer_server,
				AGS_TYPE_SOUND_SERVER,
				&ags_sound_server_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_gstreamer_server);
  }

  return g_define_type_id__volatile;
}

void
ags_gstreamer_server_class_init(AgsGstreamerServerClass *gstreamer_server)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_gstreamer_server_parent_class = g_type_class_peek_parent(gstreamer_server);

  /* GObjectClass */
  gobject = (GObjectClass *) gstreamer_server;

  gobject->set_property = ags_gstreamer_server_set_property;
  gobject->get_property = ags_gstreamer_server_get_property;

  gobject->dispose = ags_gstreamer_server_dispose;
  gobject->finalize = ags_gstreamer_server_finalize;

  /* properties */
  /**
   * AgsGstreamerServer:url:
   *
   * The assigned URL.
   * 
   * Since: 3.6.0
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
   * AgsGstreamerServer:default-soundcard:
   *
   * The default soundcard.
   * 
   * Since: 3.6.0
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
   * AgsGstreamerServer:default-gstreamer-client:
   *
   * The default gstreamer client.
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_object("default-gstreamer-client",
				   i18n_pspec("default gstreamer client"),
				   i18n_pspec("The default gstreamer client"),
				   AGS_TYPE_GSTREAMER_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_GSTREAMER_CLIENT,
				  param_spec);

  /**
   * AgsGstreamerServer:gstreamer-client: (type GList(AgsGstreamerClient)) (transfer full)
   *
   * The gstreamer client list.
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_pointer("gstreamer-client",
				    i18n_pspec("gstreamer client list"),
				    i18n_pspec("The gstreamer client list"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GSTREAMER_CLIENT,
				  param_spec);
}

void
ags_gstreamer_server_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_gstreamer_server_get_uuid;
  connectable->has_resource = ags_gstreamer_server_has_resource;

  connectable->is_ready = ags_gstreamer_server_is_ready;
  connectable->add_to_registry = ags_gstreamer_server_add_to_registry;
  connectable->remove_from_registry = ags_gstreamer_server_remove_from_registry;

  connectable->list_resource = ags_gstreamer_server_list_resource;
  connectable->xml_compose = ags_gstreamer_server_xml_compose;
  connectable->xml_parse = ags_gstreamer_server_xml_parse;

  connectable->is_connected = ags_gstreamer_server_is_connected;  
  connectable->connect = ags_gstreamer_server_connect;
  connectable->disconnect = ags_gstreamer_server_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_gstreamer_server_sound_server_interface_init(AgsSoundServerInterface *sound_server)
{
  sound_server->set_url = ags_gstreamer_server_set_url;
  sound_server->get_url = ags_gstreamer_server_get_url;
  sound_server->set_ports = ags_gstreamer_server_set_ports;
  sound_server->get_ports = ags_gstreamer_server_get_ports;
  sound_server->set_soundcard = ags_gstreamer_server_set_soundcard;
  sound_server->get_soundcard = ags_gstreamer_server_get_soundcard;
  sound_server->set_sequencer = ags_gstreamer_server_set_sequencer;
  sound_server->get_sequencer = ags_gstreamer_server_get_sequencer;
  sound_server->register_soundcard = ags_gstreamer_server_register_soundcard;
  sound_server->unregister_soundcard = ags_gstreamer_server_unregister_soundcard;
  sound_server->register_sequencer = ags_gstreamer_server_register_sequencer;
  sound_server->unregister_sequencer = ags_gstreamer_server_unregister_sequencer;
}

void
ags_gstreamer_server_init(AgsGstreamerServer *gstreamer_server)
{
  /* flags */
  gstreamer_server->flags = 0;
  gstreamer_server->connectable_flags = 0;

  /* server mutex */
  g_rec_mutex_init(&(gstreamer_server->obj_mutex)); 

  g_atomic_int_set(&(gstreamer_server->running),
		   TRUE);
  
  gstreamer_server->thread = NULL;

  /* uuid */
  gstreamer_server->uuid = ags_uuid_alloc();
  ags_uuid_generate(gstreamer_server->uuid);

  gstreamer_server->url = NULL;
  
  gstreamer_server->port = NULL;
  gstreamer_server->port_count = 0;
  
  gstreamer_server->n_soundcards = 0;
  gstreamer_server->n_sequencers = 0;

  gstreamer_server->default_soundcard = NULL;

  gstreamer_server->default_client = NULL;
  gstreamer_server->client = NULL;
}

void
ags_gstreamer_server_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsGstreamerServer *gstreamer_server;

  GRecMutex *gstreamer_server_mutex;

  gstreamer_server = AGS_GSTREAMER_SERVER(gobject);

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  switch(prop_id){
  case PROP_URL:
  {
    gchar *url;

    url = g_value_get_string(value);

    g_rec_mutex_lock(gstreamer_server_mutex);

    if(gstreamer_server->url == url){
      g_rec_mutex_unlock(gstreamer_server_mutex);

      return;
    }

    if(gstreamer_server->url != NULL){
      g_free(gstreamer_server->url);
    }

    gstreamer_server->url = g_strdup(url);

    g_rec_mutex_unlock(gstreamer_server_mutex);
  }
  break;
  case PROP_DEFAULT_SOUNDCARD:
  {
    GObject *default_soundcard;

    default_soundcard = (GObject *) g_value_get_object(value);

    g_rec_mutex_lock(gstreamer_server_mutex);

    if(gstreamer_server->default_soundcard == (GObject *) default_soundcard){
      g_rec_mutex_unlock(gstreamer_server_mutex);

      return;
    }

    if(gstreamer_server->default_soundcard != NULL){
      g_object_unref(G_OBJECT(gstreamer_server->default_soundcard));
    }

    if(default_soundcard != NULL){
      g_object_ref(G_OBJECT(default_soundcard));
    }

    gstreamer_server->default_soundcard = (GObject *) default_soundcard;

    g_rec_mutex_unlock(gstreamer_server_mutex);
  }
  break;
  case PROP_DEFAULT_GSTREAMER_CLIENT:
  {
    AgsGstreamerClient *default_client;

    default_client = (AgsGstreamerClient *) g_value_get_object(value);

    g_rec_mutex_lock(gstreamer_server_mutex);

    if(gstreamer_server->default_client == (GObject *) default_client){
      g_rec_mutex_unlock(gstreamer_server_mutex);

      return;
    }

    if(gstreamer_server->default_client != NULL){
      g_object_unref(G_OBJECT(gstreamer_server->default_client));
    }

    if(default_client != NULL){
      g_object_ref(G_OBJECT(default_client));
    }

    gstreamer_server->default_client = (GObject *) default_client;

    g_rec_mutex_unlock(gstreamer_server_mutex);
  }
  break;
  case PROP_GSTREAMER_CLIENT:
  {
    GObject *client;

    client = (GObject *) g_value_get_pointer(value);

    g_rec_mutex_lock(gstreamer_server_mutex);

    if(!AGS_IS_GSTREAMER_CLIENT(client) ||
       g_list_find(gstreamer_server->client, client) != NULL){
      g_rec_mutex_unlock(gstreamer_server_mutex);

      return;
    }

    g_object_ref(G_OBJECT(client));
    gstreamer_server->client = g_list_prepend(gstreamer_server->client,
					      client);

    g_rec_mutex_unlock(gstreamer_server_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_gstreamer_server_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsGstreamerServer *gstreamer_server;

  GRecMutex *gstreamer_server_mutex;

  gstreamer_server = AGS_GSTREAMER_SERVER(gobject);

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);
  
  switch(prop_id){
  case PROP_URL:
  {
    g_rec_mutex_lock(gstreamer_server_mutex);

    g_value_set_string(value, gstreamer_server->url);

    g_rec_mutex_unlock(gstreamer_server_mutex);
  }
  break;
  case PROP_DEFAULT_SOUNDCARD:
  {
    g_rec_mutex_lock(gstreamer_server_mutex);

    g_value_set_object(value, gstreamer_server->default_soundcard);

    g_rec_mutex_unlock(gstreamer_server_mutex);
  }
  break;
  case PROP_DEFAULT_GSTREAMER_CLIENT:
  {
    g_rec_mutex_lock(gstreamer_server_mutex);

    g_value_set_object(value, gstreamer_server->default_client);

    g_rec_mutex_unlock(gstreamer_server_mutex);
  }
  break;
  case PROP_GSTREAMER_CLIENT:
  {
    g_rec_mutex_lock(gstreamer_server_mutex);

    g_value_set_pointer(value,
			g_list_copy_deep(gstreamer_server->client,
					 (GCopyFunc) g_object_ref,
					 NULL));

    g_rec_mutex_unlock(gstreamer_server_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_gstreamer_server_dispose(GObject *gobject)
{
  AgsGstreamerServer *gstreamer_server;

  GList *list;
  
  gstreamer_server = AGS_GSTREAMER_SERVER(gobject);

  /* default soundcard */
  if(gstreamer_server->default_soundcard != NULL){
    g_object_unref(G_OBJECT(gstreamer_server->default_soundcard));

    gstreamer_server->default_soundcard = NULL;
  }
  
  /* default client */
  if(gstreamer_server->default_client != NULL){
    g_object_unref(G_OBJECT(gstreamer_server->default_client));

    gstreamer_server->default_client = NULL;
  }
  
  /* client */
  if(gstreamer_server->client != NULL){
    list = gstreamer_server->client;

    while(list != NULL){
      g_object_run_dispose(G_OBJECT(list->data));

      list = list->next;
    }
    
    g_list_free_full(gstreamer_server->client,
		     g_object_unref);

    gstreamer_server->client = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_server_parent_class)->dispose(gobject);
}

void
ags_gstreamer_server_finalize(GObject *gobject)
{
  AgsGstreamerServer *gstreamer_server;

  gstreamer_server = AGS_GSTREAMER_SERVER(gobject);

  /* url */
  g_free(gstreamer_server->url);

  /* default soundcard */
  if(gstreamer_server->default_soundcard != NULL){
    g_object_unref(G_OBJECT(gstreamer_server->default_soundcard));
  }
  
  /* default client */
  if(gstreamer_server->default_client != NULL){
    g_object_unref(G_OBJECT(gstreamer_server->default_client));
  }
  
  /* client */
  if(gstreamer_server->client != NULL){
    g_list_free_full(gstreamer_server->client,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_server_parent_class)->finalize(gobject);
}

AgsUUID*
ags_gstreamer_server_get_uuid(AgsConnectable *connectable)
{
  AgsGstreamerServer *gstreamer_server;
  
  AgsUUID *ptr;

  GRecMutex *gstreamer_server_mutex;

  gstreamer_server = AGS_GSTREAMER_SERVER(connectable);

  /* get gstreamer server signal mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  /* get UUID */
  g_rec_mutex_lock(gstreamer_server_mutex);

  ptr = gstreamer_server->uuid;

  g_rec_mutex_unlock(gstreamer_server_mutex);
  
  return(ptr);
}

gboolean
ags_gstreamer_server_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_gstreamer_server_is_ready(AgsConnectable *connectable)
{
  AgsGstreamerServer *gstreamer_server;
  
  gboolean is_ready;

  GRecMutex *gstreamer_server_mutex;

  gstreamer_server = AGS_GSTREAMER_SERVER(connectable);

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  /* check is ready */
  g_rec_mutex_lock(gstreamer_server_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (gstreamer_server->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(gstreamer_server_mutex);
  
  return(is_ready);
}

void
ags_gstreamer_server_add_to_registry(AgsConnectable *connectable)
{
  AgsGstreamerServer *gstreamer_server;

  GRecMutex *gstreamer_server_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  gstreamer_server = AGS_GSTREAMER_SERVER(connectable);

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  g_rec_mutex_lock(gstreamer_server_mutex);

  gstreamer_server->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(gstreamer_server_mutex);
}

void
ags_gstreamer_server_remove_from_registry(AgsConnectable *connectable)
{
  AgsGstreamerServer *gstreamer_server;

  GRecMutex *gstreamer_server_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  gstreamer_server = AGS_GSTREAMER_SERVER(connectable);

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  g_rec_mutex_lock(gstreamer_server_mutex);

  gstreamer_server->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(gstreamer_server_mutex);
}

xmlNode*
ags_gstreamer_server_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_gstreamer_server_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_gstreamer_server_xml_parse(AgsConnectable *connectable,
			       xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_gstreamer_server_is_connected(AgsConnectable *connectable)
{
  AgsGstreamerServer *gstreamer_server;
  
  gboolean is_connected;

  GRecMutex *gstreamer_server_mutex;

  gstreamer_server = AGS_GSTREAMER_SERVER(connectable);

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  /* check is connected */
  g_rec_mutex_lock(gstreamer_server_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (gstreamer_server->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(gstreamer_server_mutex);
  
  return(is_connected);
}

void
ags_gstreamer_server_connect(AgsConnectable *connectable)
{
  AgsGstreamerServer *gstreamer_server;

  GList *list_start, *list;  

  GRecMutex *gstreamer_server_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  gstreamer_server = AGS_GSTREAMER_SERVER(connectable);

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  g_rec_mutex_lock(gstreamer_server_mutex);

  gstreamer_server->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(gstreamer_server_mutex);

  list =
    list_start = g_list_copy(gstreamer_server->client);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_gstreamer_server_disconnect(AgsConnectable *connectable)
{
  AgsGstreamerServer *gstreamer_server;

  GList *list_start, *list;

  GRecMutex *gstreamer_server_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  gstreamer_server = AGS_GSTREAMER_SERVER(connectable);

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  g_rec_mutex_lock(gstreamer_server_mutex);

  gstreamer_server->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(gstreamer_server_mutex);

  /* client */
  list =
    list_start = g_list_copy(gstreamer_server->client);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_gstreamer_server_test_flags:
 * @gstreamer_server: the #AgsGstreamerServer
 * @flags: the flags
 *
 * Test @flags to be set on @gstreamer_server.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.6.0
 */
gboolean
ags_gstreamer_server_test_flags(AgsGstreamerServer *gstreamer_server, guint flags)
{
  gboolean retval;  
  
  GRecMutex *gstreamer_server_mutex;

  if(!AGS_IS_GSTREAMER_SERVER(gstreamer_server)){
    return(FALSE);
  }

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  /* test */
  g_rec_mutex_lock(gstreamer_server_mutex);

  retval = (flags & (gstreamer_server->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(gstreamer_server_mutex);

  return(retval);
}

/**
 * ags_gstreamer_server_set_flags:
 * @gstreamer_server: the #AgsGstreamerServer
 * @flags: see #AgsGstreamerServerFlags-enum
 *
 * Enable a feature of @gstreamer_server.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_server_set_flags(AgsGstreamerServer *gstreamer_server, guint flags)
{
  GRecMutex *gstreamer_server_mutex;

  if(!AGS_IS_GSTREAMER_SERVER(gstreamer_server)){
    return;
  }

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(gstreamer_server_mutex);

  gstreamer_server->flags |= flags;
  
  g_rec_mutex_unlock(gstreamer_server_mutex);
}
    
/**
 * ags_gstreamer_server_unset_flags:
 * @gstreamer_server: the #AgsGstreamerServer
 * @flags: see #AgsGstreamerServerFlags-enum
 *
 * Disable a feature of @gstreamer_server.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_server_unset_flags(AgsGstreamerServer *gstreamer_server, guint flags)
{  
  GRecMutex *gstreamer_server_mutex;

  if(!AGS_IS_GSTREAMER_SERVER(gstreamer_server)){
    return;
  }

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(gstreamer_server_mutex);

  gstreamer_server->flags &= (~flags);
  
  g_rec_mutex_unlock(gstreamer_server_mutex);
}

void
ags_gstreamer_server_set_url(AgsSoundServer *sound_server,
			     gchar *url)
{
  AgsGstreamerServer *gstreamer_server;

  GRecMutex *gstreamer_server_mutex;

  gstreamer_server = AGS_GSTREAMER_SERVER(sound_server);

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  /* set URL */
  g_rec_mutex_lock(gstreamer_server_mutex);

  gstreamer_server->url = g_strdup(url);

  g_rec_mutex_unlock(gstreamer_server_mutex);
}

gchar*
ags_gstreamer_server_get_url(AgsSoundServer *sound_server)
{
  AgsGstreamerServer *gstreamer_server;

  gchar *url;

  GRecMutex *gstreamer_server_mutex;

  gstreamer_server = AGS_GSTREAMER_SERVER(sound_server);

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  /* set URL */
  g_rec_mutex_lock(gstreamer_server_mutex);

  url = gstreamer_server->url;

  g_rec_mutex_unlock(gstreamer_server_mutex);
  
  return(url);
}


void
ags_gstreamer_server_set_ports(AgsSoundServer *sound_server,
			       guint *port, guint port_count)
{
  AgsGstreamerServer *gstreamer_server;

  GRecMutex *gstreamer_server_mutex;

  gstreamer_server = AGS_GSTREAMER_SERVER(sound_server);

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  /* set ports */
  g_rec_mutex_lock(gstreamer_server_mutex);

  gstreamer_server->port = port;
  gstreamer_server->port_count = port_count;

  g_rec_mutex_unlock(gstreamer_server_mutex);
}

guint*
ags_gstreamer_server_get_ports(AgsSoundServer *sound_server,
			       guint *port_count)
{
  AgsGstreamerServer *gstreamer_server;

  guint *port;
  
  GRecMutex *gstreamer_server_mutex;

  gstreamer_server = AGS_GSTREAMER_SERVER(sound_server);

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  /* get ports */
  g_rec_mutex_lock(gstreamer_server_mutex);

  if(port_count != NULL){
    *port_count = AGS_GSTREAMER_SERVER(sound_server)->port_count;
  }

  port = gstreamer_server->port;

  g_rec_mutex_unlock(gstreamer_server_mutex);
  
  return(port);
}

void
ags_gstreamer_server_set_soundcard(AgsSoundServer *sound_server,
				   gchar *client_uuid,
				   GList *soundcard)
{
  AgsGstreamerServer *gstreamer_server;
  AgsGstreamerClient *gstreamer_client;

  GList *list;

  gstreamer_server = AGS_GSTREAMER_SERVER(sound_server);
  
  gstreamer_client = (AgsGstreamerClient *) ags_gstreamer_server_find_client(gstreamer_server,
									     client_uuid);

  if(!AGS_IS_GSTREAMER_CLIENT(gstreamer_client)){
    return;
  }
  
  //NOTE:JK: soundcard won't removed
  list = soundcard;

  while(list != NULL){
    ags_gstreamer_client_add_device(gstreamer_client,
				    (GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_gstreamer_server_get_soundcard(AgsSoundServer *sound_server,
				   gchar *client_uuid)
{
  AgsGstreamerServer *gstreamer_server;
  AgsGstreamerClient *gstreamer_client;

  GList *device_start, *device;
  GList *list;
  
  gstreamer_server = AGS_GSTREAMER_SERVER(sound_server);

  gstreamer_client = (AgsGstreamerClient *) ags_gstreamer_server_find_client(gstreamer_server,
									     client_uuid);

  if(!AGS_IS_GSTREAMER_CLIENT(gstreamer_client)){
    return(NULL);
  }

  g_object_get(gstreamer_client,
	       "device", &device_start,
	       NULL);
  
  device = device_start;
  list = NULL;

  while(device != NULL){
    if(AGS_IS_GSTREAMER_DEVOUT(device->data) ||
       AGS_IS_GSTREAMER_DEVIN(device->data)){
      list = g_list_prepend(list,
			    device->data);
      g_object_ref(device->data);
    }

    device = device->next;
  }

  g_list_free_full(device_start,
		   g_object_unref);
  
  return(g_list_reverse(list));
}


void
ags_gstreamer_server_set_sequencer(AgsSoundServer *sound_server,
				   gchar *client_uuid,
				   GList *sequencer)
{
  AgsGstreamerServer *gstreamer_server;
  AgsGstreamerClient *gstreamer_client;

  GList *list;

  gstreamer_server = AGS_GSTREAMER_SERVER(sound_server);

  gstreamer_client = (AgsGstreamerClient *) ags_gstreamer_server_find_client(gstreamer_server,
									     client_uuid);

  if(!AGS_IS_GSTREAMER_CLIENT(gstreamer_client)){
    return;
  }

  //NOTE:JK: sequencer won't removed
  list = sequencer;

  while(list != NULL){
    ags_gstreamer_client_add_device(gstreamer_client,
				    (GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_gstreamer_server_get_sequencer(AgsSoundServer *sound_server,
				   gchar *client_uuid)
{
  AgsGstreamerServer *gstreamer_server;
  AgsGstreamerClient *gstreamer_client;

  GList *device_start, *device;
  GList *list;
  
  gstreamer_server = AGS_GSTREAMER_SERVER(sound_server);

  gstreamer_client = (AgsGstreamerClient *) ags_gstreamer_server_find_client(gstreamer_server,
									     client_uuid);

  if(!AGS_IS_GSTREAMER_CLIENT(gstreamer_client)){
    return(NULL);
  }

  g_object_get(gstreamer_client,
	       "device", &device_start,
	       NULL);
  
  device = device_start;
  list = NULL;

#if 0  
  while(device != NULL){
    if(AGS_IS_GSTREAMER_MIDIIN(device->data)){
      list = g_list_prepend(list,
			    device->data);
      g_object_ref(device->data);
    }

    device = device->next;
  }
#endif

  g_list_free_full(device_start,
		   g_object_unref);
    
  return(g_list_reverse(list));
}

GObject*
ags_gstreamer_server_register_soundcard(AgsSoundServer *sound_server,
					gboolean is_output)
{
  AgsGstreamerServer *gstreamer_server;
  AgsGstreamerClient *default_client;
  AgsGstreamerPort *gstreamer_port;
  AgsGstreamerDevout *gstreamer_devout;
  AgsGstreamerDevin *gstreamer_devin;

#if defined(AGS_WITH_GSTREAMER)
  GstElement *pipeline;
#else
  gpointer pipeline;
#endif
  
  AgsApplicationContext *application_context;

  GObject *soundcard;
  
  gchar *str;  

  guint n_soundcards;
  gboolean initial_set;
  guint i;  

  GRecMutex *gstreamer_server_mutex;
  GRecMutex *gstreamer_client_mutex;

  gstreamer_server = AGS_GSTREAMER_SERVER(sound_server);

  application_context= ags_application_context_get_instance();

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  /* the default client */
  initial_set = FALSE;
  
  /* get some fields */
  g_rec_mutex_lock(gstreamer_server_mutex);
  
  default_client = (AgsGstreamerClient *) gstreamer_server->default_client;

  n_soundcards = gstreamer_server->n_soundcards;
  
  g_rec_mutex_unlock(gstreamer_server_mutex);

  /* the default client */
  if(default_client == NULL){
    default_client = ags_gstreamer_client_new((GObject *) gstreamer_server);
    
    g_object_set(gstreamer_server,
		 "default-gstreamer-client", default_client,
		 NULL);
    ags_gstreamer_server_add_client(gstreamer_server,
				    (GObject *) default_client);
    
    ags_gstreamer_client_open((AgsGstreamerClient *) default_client,
			      "ags-default-client");
    initial_set = TRUE;    
  }

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(default_client);

  /* get pipeline */
  g_rec_mutex_lock(gstreamer_client_mutex);

  pipeline = default_client->pipeline;
  
  g_rec_mutex_unlock(gstreamer_client_mutex);
  
  if(pipeline == NULL){
    g_warning("ags_gstreamer_server.c - can't open gstreamer client");
  }

  /* the soundcard */
  soundcard = NULL;

  /* the soundcard */
  if(is_output){
    gstreamer_devout = ags_gstreamer_devout_new();
    soundcard = (GObject *) gstreamer_devout;
    
    str = g_strdup_printf("ags-gstreamer-devout-%d",
			  n_soundcards);
    
    g_object_set(AGS_GSTREAMER_DEVOUT(gstreamer_devout),
		 "gstreamer-client", default_client,
		 "device", str,
		 NULL);
    g_free(str);
            
    /* register ports */      
    gstreamer_port = ags_gstreamer_port_new((GObject *) default_client);

    str = g_strdup_printf("ags-soundcard%d",
			  n_soundcards);

    g_object_set(gstreamer_port,
		 "gstreamer-devout", gstreamer_devout,
		 NULL);
    ags_gstreamer_client_add_port(default_client,
				  (GObject *) gstreamer_port);

    g_object_set(gstreamer_devout,
		 "gstreamer-port", gstreamer_port,
		 NULL);

    gstreamer_devout->port_name = (gchar **) malloc(2 * sizeof(gchar *));
    gstreamer_devout->port_name[0] = g_strdup(str);
    gstreamer_devout->port_name[1] = NULL;
    
    ags_gstreamer_port_register(gstreamer_port,
				str,
				TRUE, FALSE,
				TRUE);

    ags_gstreamer_devout_realloc_buffer(gstreamer_devout);

    g_object_set(default_client,
		 "device", gstreamer_devout,
		 NULL);

    /* increment n-soundcards */
    g_rec_mutex_lock(gstreamer_server_mutex);

    gstreamer_server->n_soundcards += 1;

    g_rec_mutex_unlock(gstreamer_server_mutex);
  }else{
    gstreamer_devin = ags_gstreamer_devin_new();
    soundcard = (GObject *) gstreamer_devin;

    str = g_strdup_printf("ags-gstreamer-devin-%d",
			  gstreamer_server->n_soundcards);
    
    g_object_set(AGS_GSTREAMER_DEVIN(gstreamer_devin),
		 "gstreamer-client", default_client,
		 "device", str,
		 NULL);
    g_free(str);
        
    /* register ports */
    str = g_strdup_printf("ags-soundcard%d",
			  n_soundcards);
    
#ifdef AGS_DEBUG
    g_message("%s", str);
#endif
      
    gstreamer_port = ags_gstreamer_port_new((GObject *) default_client);
    g_object_set(gstreamer_port,
		 "gstreamer-devin", gstreamer_devin,
		 NULL);
    ags_gstreamer_client_add_port(default_client,
				  (GObject *) gstreamer_port);

    g_object_set(gstreamer_devin,
		 "gstreamer-port", gstreamer_port,
		 NULL);
      
    gstreamer_devin->port_name = (gchar **) malloc(2 * sizeof(gchar *));
    gstreamer_devin->port_name[0] = g_strdup(str);
    gstreamer_devin->port_name[1] = NULL;
    
    ags_gstreamer_port_register(gstreamer_port,
				str,
				TRUE, FALSE,
				TRUE);

    ags_gstreamer_devin_realloc_buffer(gstreamer_devin);

    g_object_set(default_client,
		 "device", gstreamer_devin,
		 NULL);
    
    /* increment n-soundcards */
    g_rec_mutex_lock(gstreamer_server_mutex);

    gstreamer_server->n_soundcards += 1;

    g_rec_mutex_unlock(gstreamer_server_mutex);
  }
  
  return((GObject *) soundcard);
}

void
ags_gstreamer_server_unregister_soundcard(AgsSoundServer *sound_server,
					  GObject *soundcard)
{
  AgsGstreamerServer *gstreamer_server;
  AgsGstreamerClient *default_client;

  GList *list_start, *list;
  GList *port;
  
  GRecMutex *gstreamer_server_mutex;

  gstreamer_server = AGS_GSTREAMER_SERVER(sound_server);

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);
  
  /* the default client */
  g_object_get(gstreamer_server,
	       "default-gstreamer-client", &default_client,
	       NULL);

  if(default_client == NULL){
    g_warning("GSequencer - no gstreamer client");
    
    return;
  }
  
  if(AGS_IS_GSTREAMER_DEVOUT(soundcard)){
    g_object_get(soundcard,
		 "gstreamer-port", &list_start,
		 NULL);

    list = list_start;

    while(list != NULL){
      ags_gstreamer_port_unregister(list->data);
      ags_gstreamer_client_remove_port(default_client,
				       list->data);
    
      list = list->next;
    }

    g_list_free_full(list_start,
		     g_object_unref);
  }else if(AGS_IS_GSTREAMER_DEVIN(soundcard)){
    g_object_get(soundcard,
		 "gstreamer-port", &list_start,
		 NULL);

    list = list_start;

    while(list != NULL){
      ags_gstreamer_port_unregister(list->data);
      ags_gstreamer_client_remove_port(default_client,
				       list->data);
    
      list = list->next;
    }

    g_list_free_full(list_start,
		     g_object_unref);
  }
  
  ags_gstreamer_client_remove_device(default_client,
				     soundcard);
  
  g_object_get(default_client,
	       "port", &port,
	       NULL);
  
  if(port == NULL){
    /* reset n-soundcards */
    g_rec_mutex_lock(gstreamer_server_mutex);

    gstreamer_server->n_soundcards = 0;

    g_rec_mutex_unlock(gstreamer_server_mutex);
  }

  g_object_unref(default_client);
  
  g_list_free_full(port,
		   g_object_unref);
}

GObject*
ags_gstreamer_server_register_sequencer(AgsSoundServer *sound_server,
					gboolean is_output)
{
  g_message("GSequencer - can't register gstreamer sequencer");
  
  return(NULL);
}

void
ags_gstreamer_server_unregister_sequencer(AgsSoundServer *sound_server,
					  GObject *sequencer)
{
  g_message("GSequencer - can't unregister gstreamer sequencer");
}

/**
 * ags_gstreamer_server_register_default_soundcard:
 * @gstreamer_server: the #AgsGstreamerServer
 * 
 * Register default soundcard.
 * 
 * Returns: the instantiated #AgsGstreamerDevout
 * 
 * Since: 3.6.0
 */
GObject*
ags_gstreamer_server_register_default_soundcard(AgsGstreamerServer *gstreamer_server)
{
  AgsGstreamerClient *default_client;
  AgsGstreamerDevout *gstreamer_devout;
  AgsGstreamerPort *gstreamer_port;

#if defined(AGS_WITH_GSTREAMER)
  GstElement *pipeline;
#else
  gpointer pipeline;
#endif

  AgsApplicationContext *application_context;

  gchar *str;
  
  guint i;

  GRecMutex *gstreamer_server_mutex;
  GRecMutex *gstreamer_client_mutex;

  if(!AGS_IS_GSTREAMER_SERVER(gstreamer_server)){
    return(NULL);
  }

  application_context = ags_application_context_get_instance();

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  /* get some fields */
  g_rec_mutex_lock(gstreamer_server_mutex);

  default_client = (AgsGstreamerClient *) gstreamer_server->default_client;

  g_rec_mutex_unlock(gstreamer_server_mutex);
    
  /* the default client */
  if(default_client == NULL){
    default_client = ags_gstreamer_client_new((GObject *) gstreamer_server);
    
    g_object_set(gstreamer_server,
		 "default-gstreamer-client", default_client,
		 NULL);
    ags_gstreamer_server_add_client(gstreamer_server,
				    (GObject *) default_client);
    
    ags_gstreamer_client_open((AgsGstreamerClient *) default_client,
			      "ags-default-client");
  }

  /* get gstreamer client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(default_client);

  /* get pipeline */
  g_rec_mutex_lock(gstreamer_client_mutex);

  pipeline = default_client->pipeline;

  g_rec_mutex_unlock(gstreamer_client_mutex);
  
  if(pipeline == NULL){
    g_warning("ags_gstreamer_server.c - can't open gstreamer client");
  }

  /* the soundcard */
  gstreamer_devout = ags_gstreamer_devout_new();
  
  g_object_set(AGS_GSTREAMER_DEVOUT(gstreamer_devout),
	       "gstreamer-client", default_client,
	       "device", "ags-default-devout",
	       NULL);
    
  /* register ports */
  str = g_strdup_printf("ags-default-soundcard");

#ifdef AGS_DEBUG
  g_message("%s", str);
#endif
    
  gstreamer_port = ags_gstreamer_port_new((GObject *) default_client);
  g_object_set(gstreamer_port,
	       "gstreamer-devout", gstreamer_devout,
	       NULL);
  ags_gstreamer_client_add_port(default_client,
				(GObject *) gstreamer_port);

  g_object_set(gstreamer_devout,
	       "gstreamer-port", gstreamer_port,
	       NULL);
  
  gstreamer_devout->port_name = (gchar **) malloc(2 * sizeof(gchar *));
  gstreamer_devout->port_name[0] = g_strdup(str);
  gstreamer_devout->port_name[1] = NULL;
  
  ags_gstreamer_port_register(gstreamer_port,
			      str,
			      TRUE, FALSE,
			      TRUE);

  g_free(str);

  g_object_set(default_client,
	       "device", gstreamer_devout,
	       NULL);
  
  return((GObject *) gstreamer_devout);
}

/**
 * ags_gstreamer_server_find_url:
 * @gstreamer_server: (element-type AgsAudio.GstreamerServer) (transfer none): the #GList-struct containing #AgsGstreamerServer
 * @url: the url to find
 *
 * Find #AgsGstreamerServer by url.
 *
 * Returns: (element-type AgsAudio.GstreamerServer) (transfer none): the #GList-struct containing a #AgsGstreamerServer matching @url or %NULL
 *
 * Since: 3.6.0
 */
GList*
ags_gstreamer_server_find_url(GList *gstreamer_server,
			      gchar *url)
{
  GList *retval;
  
  GRecMutex *gstreamer_server_mutex;

  retval = NULL;
  
  while(gstreamer_server != NULL){
    /* get gstreamer server mutex */
    gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server->data);

    /* check URL */
    g_rec_mutex_lock(gstreamer_server_mutex);
    
    if(!g_ascii_strcasecmp(AGS_GSTREAMER_SERVER(gstreamer_server->data)->url,
			   url)){
      retval = gstreamer_server;

      g_rec_mutex_unlock(gstreamer_server_mutex);
    
      break;
    }

    g_rec_mutex_unlock(gstreamer_server_mutex);
    
    gstreamer_server = gstreamer_server->next;
  }

  return(retval);
}

/**
 * ags_gstreamer_server_find_client:
 * @gstreamer_server: the #AgsGstreamerServer
 * @client_uuid: the uuid to find
 *
 * Find #AgsGstreamerClient by uuid.
 *
 * Returns: (transfer none): the #AgsGstreamerClient found or %NULL
 *
 * Since: 3.6.0
 */
GObject*
ags_gstreamer_server_find_client(AgsGstreamerServer *gstreamer_server,
				 gchar *client_uuid)
{
  AgsGstreamerClient *retval;
  
  GList *list_start, *list;

  GRecMutex *gstreamer_server_mutex;
  GRecMutex *gstreamer_client_mutex;

  if(!AGS_IS_GSTREAMER_SERVER(gstreamer_server)){
    return(NULL);
  }

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  /* get some fields */
  g_rec_mutex_lock(gstreamer_server_mutex);

  list =
    list_start = g_list_copy(gstreamer_server->client);

  g_rec_mutex_unlock(gstreamer_server_mutex);

  retval = NULL;
  
  while(list != NULL){
    /* get gstreamer client mutex */
    gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(list->data);

    /* check client UUID */
    g_rec_mutex_lock(gstreamer_client_mutex);
    
    if(!g_ascii_strcasecmp(AGS_GSTREAMER_CLIENT(list->data)->client_uuid,
			   client_uuid)){
      retval = list->data;

      g_rec_mutex_unlock(gstreamer_client_mutex);

      break;
    }

    g_rec_mutex_unlock(gstreamer_client_mutex);
    
    list = list->next;
  }

  g_list_free(list_start);
  
  return((GObject *) retval);
}

/**
 * ags_gstreamer_server_find_port:
 * @gstreamer_server: the #AgsGstreamerServer
 * @port_uuid: the uuid to find
 *
 * Find #AgsGstreamerPort by uuid.
 *
 * Returns: (transfer none): the #AgsGstreamerPort found or %NULL
 *
 * Since: 3.6.0
 */
GObject*
ags_gstreamer_server_find_port(AgsGstreamerServer *gstreamer_server,
			       gchar *port_uuid)
{
  GList *client_start, *client;
  GList *port_start, *port;

  gboolean success;
  
  GRecMutex *gstreamer_port_mutex;

  g_object_get(gstreamer_server,
	       "gstreamer-client", &client_start,
	       NULL);

  client = client_start;
  
  while(client != NULL){
    g_object_get(gstreamer_server,
		 "gstreamer-port", &port_start,
		 NULL);

    port = port_start;
    
    while(port != NULL){
      /* get gstreamer port mutex */
      gstreamer_port_mutex = AGS_GSTREAMER_PORT_GET_OBJ_MUTEX(port->data);
      
      /* check port UUID */
      g_rec_mutex_lock(gstreamer_port_mutex);
      
      success = (!g_ascii_strcasecmp(AGS_GSTREAMER_PORT(port->data)->port_uuid,
				     port_uuid)) ? TRUE: FALSE;

      g_rec_mutex_unlock(gstreamer_port_mutex);
      
      if(success){
	AgsGstreamerPort *retval;

	retval = port->data;
	
	g_list_free_full(client_start,
			 g_object_unref);
	g_list_free_full(port_start,
			 g_object_unref);
	
	return(retval);
      }

      /* iterate */
      port = port->next;
    }

    g_list_free_full(port_start,
		     g_object_unref);

    /* iterate */
    client = client->next;
  }

  g_list_free_full(client_start,
		   g_object_unref);
  
  return(NULL);
}

/**
 * ags_gstreamer_server_add_client:
 * @gstreamer_server: the #AgsGstreamerServer
 * @gstreamer_client: the #AgsGstreamerClient to add
 *
 * Add @gstreamer_client to @gstreamer_server
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_server_add_client(AgsGstreamerServer *gstreamer_server,
				GObject *gstreamer_client)
{
  GRecMutex *gstreamer_server_mutex;

  if(!AGS_IS_GSTREAMER_SERVER(gstreamer_server) ||
     !AGS_IS_GSTREAMER_CLIENT(gstreamer_client)){
    return;
  }

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  /* get some fields */
  g_rec_mutex_lock(gstreamer_server_mutex);

  if(g_list_find(gstreamer_server->client, gstreamer_client) == NULL){
    g_object_ref(gstreamer_client);
    gstreamer_server->client = g_list_prepend(gstreamer_server->client,
					      gstreamer_client);
  }

  g_rec_mutex_unlock(gstreamer_server_mutex);
}

/**
 * ags_gstreamer_server_remove_client:
 * @gstreamer_server: the #AgsGstreamerServer
 * @gstreamer_client: the #AgsGstreamerClient to remove
 *
 * Remove @gstreamer_client to @gstreamer_server
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_server_remove_client(AgsGstreamerServer *gstreamer_server,
				   GObject *gstreamer_client)
{
  GRecMutex *gstreamer_server_mutex;

  if(!AGS_IS_GSTREAMER_SERVER(gstreamer_server) ||
     !AGS_IS_GSTREAMER_CLIENT(gstreamer_client)){
    return;
  }

  /* get gstreamer server mutex */
  gstreamer_server_mutex = AGS_GSTREAMER_SERVER_GET_OBJ_MUTEX(gstreamer_server);

  /* get some fields */
  g_rec_mutex_lock(gstreamer_server_mutex);

  if(g_list_find(gstreamer_server->client, gstreamer_client) != NULL){
    gstreamer_server->client = g_list_remove(gstreamer_server->client,
					     gstreamer_client);
    g_object_unref(gstreamer_client);
  }

  g_rec_mutex_unlock(gstreamer_server_mutex);
}

/**
 * ags_gstreamer_server_connect_client:
 * @gstreamer_server: the #AgsGstreamerServer
 *
 * Connect all clients.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_server_connect_client(AgsGstreamerServer *gstreamer_server)
{
  GList *client_start, *client;

  gchar *client_name;
  
  GRecMutex *gstreamer_client_mutex;

  if(!AGS_IS_GSTREAMER_SERVER(gstreamer_server)){
    return;
  }

  g_object_get(gstreamer_server,
	       "gstreamer-client", &client_start,
	       NULL);
  
  client = client_start;

  while(client != NULL){
    /* client name */
    g_object_get(client->data,
		 "client-name", &client_name,
		 NULL);
    
    /* open */
    ags_gstreamer_client_open((AgsGstreamerClient *) client->data,
			      client_name);
    ags_gstreamer_client_activate(client->data);
    
    /* iterate */
    client = client->next;
  }

  g_list_free_full(client_start,
		   g_object_unref);
}

/**
 * ags_gstreamer_server_disconnect_client:
 * @gstreamer_server: the #AgsGstreamerServer
 *
 * Disconnect all clients.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_server_disconnect_client(AgsGstreamerServer *gstreamer_server)
{
  GList *client_start, *client;
  
  if(!AGS_IS_GSTREAMER_SERVER(gstreamer_server)){
    return;
  }

  g_object_get(gstreamer_server,
	       "gstreamer-client", &client_start,
	       NULL);
  
  client = client_start;

  while(client != NULL){
    /* close */
    ags_gstreamer_client_deactivate(client->data);
    
    ags_gstreamer_client_close((AgsGstreamerClient *) client->data);

    /* iterate */
    client = client->next;
  }

  g_list_free_full(client_start,
		   g_object_unref);
}

/**
 * ags_gstreamer_server_new:
 * @url: the URL as string
 *
 * Create a new instance of #AgsGstreamerServer.
 *
 * Returns: the new #AgsGstreamerServer
 *
 * Since: 3.6.0
 */
AgsGstreamerServer*
ags_gstreamer_server_new(gchar *url)
{
  AgsGstreamerServer *gstreamer_server;

  gstreamer_server = (AgsGstreamerServer *) g_object_new(AGS_TYPE_GSTREAMER_SERVER,
							 "url", url,
							 NULL);

  return(gstreamer_server);
}
