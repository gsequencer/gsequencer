/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/pulse/ags_pulse_server.h>
#include <ags/audio/pulse/ags_pulse_client.h>
#include <ags/audio/pulse/ags_pulse_port.h>

#include <ags/audio/pulse/ags_pulse_devout.h>
#include <ags/audio/pulse/ags_pulse_devin.h>

#include <string.h>

#include <errno.h>

#include <ags/i18n.h>

void ags_pulse_server_class_init(AgsPulseServerClass *pulse_server);
void ags_pulse_server_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pulse_server_sound_server_interface_init(AgsSoundServerInterface *sound_server);
void ags_pulse_server_init(AgsPulseServer *pulse_server);
void ags_pulse_server_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_pulse_server_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_pulse_server_dispose(GObject *gobject);
void ags_pulse_server_finalize(GObject *gobject);

AgsUUID* ags_pulse_server_get_uuid(AgsConnectable *connectable);
gboolean ags_pulse_server_has_resource(AgsConnectable *connectable);
gboolean ags_pulse_server_is_ready(AgsConnectable *connectable);
void ags_pulse_server_add_to_registry(AgsConnectable *connectable);
void ags_pulse_server_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_pulse_server_list_resource(AgsConnectable *connectable);
xmlNode* ags_pulse_server_xml_compose(AgsConnectable *connectable);
void ags_pulse_server_xml_parse(AgsConnectable *connectable,
				xmlNode *node);
gboolean ags_pulse_server_is_connected(AgsConnectable *connectable);
void ags_pulse_server_connect(AgsConnectable *connectable);
void ags_pulse_server_disconnect(AgsConnectable *connectable);

void ags_pulse_server_set_url(AgsSoundServer *sound_server,
			      gchar *url);
gchar* ags_pulse_server_get_url(AgsSoundServer *sound_server);
void ags_pulse_server_set_ports(AgsSoundServer *sound_server,
				guint *ports, guint port_count);
guint* ags_pulse_server_get_ports(AgsSoundServer *sound_server,
				  guint *port_count);
void ags_pulse_server_set_soundcard(AgsSoundServer *sound_server,
				    gchar *client_uuid,
				    GList *soundcard);
GList* ags_pulse_server_get_soundcard(AgsSoundServer *sound_server,
				      gchar *client_uuid);
void ags_pulse_server_set_sequencer(AgsSoundServer *sound_server,
				    gchar *client_uuid,
				    GList *sequencer);
GList* ags_pulse_server_get_sequencer(AgsSoundServer *sound_server,
				      gchar *client_uuid);
GObject* ags_pulse_server_register_soundcard(AgsSoundServer *sound_server,
					     gboolean is_output);
void ags_pulse_server_unregister_soundcard(AgsSoundServer *sound_server,
					   GObject *soundcard);
GObject* ags_pulse_server_register_sequencer(AgsSoundServer *sound_server,
					     gboolean is_output);
void ags_pulse_server_unregister_sequencer(AgsSoundServer *sound_server,
					   GObject *sequencer);

void* ags_pulse_server_do_poll_loop(void *ptr);

/**
 * SECTION:ags_pulse_server
 * @short_description: pulseaudio instance
 * @title: AgsPulseServer
 * @section_id:
 * @include: ags/audio/pulse/ags_pulse_server.h
 *
 * The #AgsPulseServer is an object to represent a running pulseaudio instance.
 */

enum{
  PROP_0,
  PROP_URL,
  PROP_DEFAULT_SOUNDCARD,
  PROP_DEFAULT_PULSE_CLIENT,
  PROP_PULSE_CLIENT,
};

static gpointer ags_pulse_server_parent_class = NULL;

GType
ags_pulse_server_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_pulse_server = 0;

    static const GTypeInfo ags_pulse_server_info = {
      sizeof(AgsPulseServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pulse_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPulseServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pulse_server_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pulse_server_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_sound_server_interface_info = {
      (GInterfaceInitFunc) ags_pulse_server_sound_server_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_pulse_server = g_type_register_static(G_TYPE_OBJECT,
						   "AgsPulseServer",
						   &ags_pulse_server_info,
						   0);

    g_type_add_interface_static(ags_type_pulse_server,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_pulse_server,
				AGS_TYPE_SOUND_SERVER,
				&ags_sound_server_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_pulse_server);
  }

  return g_define_type_id__volatile;
}

void
ags_pulse_server_class_init(AgsPulseServerClass *pulse_server)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_pulse_server_parent_class = g_type_class_peek_parent(pulse_server);

  /* GObjectClass */
  gobject = (GObjectClass *) pulse_server;

  gobject->set_property = ags_pulse_server_set_property;
  gobject->get_property = ags_pulse_server_get_property;

  gobject->dispose = ags_pulse_server_dispose;
  gobject->finalize = ags_pulse_server_finalize;

  /* properties */
  /**
   * AgsPulseServer:url:
   *
   * The assigned URL.
   * 
   * Since: 3.0.0
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
   * AgsPulseServer:default-soundcard:
   *
   * The default soundcard.
   * 
   * Since: 3.0.0
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
   * AgsPulseServer:default-pulse-client:
   *
   * The default pulse client.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("default-pulse-client",
				   i18n_pspec("default pulse client"),
				   i18n_pspec("The default pulse client"),
				   AGS_TYPE_PULSE_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_PULSE_CLIENT,
				  param_spec);

  /**
   * AgsPulseServer:pulse-client:
   *
   * The pulse client list.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("pulse-client",
				    i18n_pspec("pulse client list"),
				    i18n_pspec("The pulse client list"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PULSE_CLIENT,
				  param_spec);
}

void
ags_pulse_server_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_pulse_server_get_uuid;
  connectable->has_resource = ags_pulse_server_has_resource;

  connectable->is_ready = ags_pulse_server_is_ready;
  connectable->add_to_registry = ags_pulse_server_add_to_registry;
  connectable->remove_from_registry = ags_pulse_server_remove_from_registry;

  connectable->list_resource = ags_pulse_server_list_resource;
  connectable->xml_compose = ags_pulse_server_xml_compose;
  connectable->xml_parse = ags_pulse_server_xml_parse;

  connectable->is_connected = ags_pulse_server_is_connected;  
  connectable->connect = ags_pulse_server_connect;
  connectable->disconnect = ags_pulse_server_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_pulse_server_sound_server_interface_init(AgsSoundServerInterface *sound_server)
{
  sound_server->set_url = ags_pulse_server_set_url;
  sound_server->get_url = ags_pulse_server_get_url;
  sound_server->set_ports = ags_pulse_server_set_ports;
  sound_server->get_ports = ags_pulse_server_get_ports;
  sound_server->set_soundcard = ags_pulse_server_set_soundcard;
  sound_server->get_soundcard = ags_pulse_server_get_soundcard;
  sound_server->set_sequencer = ags_pulse_server_set_sequencer;
  sound_server->get_sequencer = ags_pulse_server_get_sequencer;
  sound_server->register_soundcard = ags_pulse_server_register_soundcard;
  sound_server->unregister_soundcard = ags_pulse_server_unregister_soundcard;
  sound_server->register_sequencer = ags_pulse_server_register_sequencer;
  sound_server->unregister_sequencer = ags_pulse_server_unregister_sequencer;
}

void
ags_pulse_server_init(AgsPulseServer *pulse_server)
{
  /* flags */
  pulse_server->flags = 0;

  /* server mutex */
  g_rec_mutex_init(&(pulse_server->obj_mutex)); 

  g_atomic_int_set(&(pulse_server->running),
		   TRUE);
  
  pulse_server->thread = NULL;

  /* uuid */
  pulse_server->uuid = ags_uuid_alloc();
  ags_uuid_generate(pulse_server->uuid);
  
#ifdef AGS_WITH_PULSE
  pulse_server->main_loop = NULL;
  pulse_server->main_loop_api = NULL;
#else
  pulse_server->main_loop = NULL;
  pulse_server->main_loop_api = NULL;
#endif

  pulse_server->url = NULL;
  
  pulse_server->port = NULL;
  pulse_server->port_count = 0;
  
  pulse_server->n_soundcards = 0;
  pulse_server->n_sequencers = 0;

  pulse_server->default_soundcard = NULL;

  pulse_server->default_client = NULL;
  pulse_server->client = NULL;
}

void
ags_pulse_server_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsPulseServer *pulse_server;

  GRecMutex *pulse_server_mutex;

  pulse_server = AGS_PULSE_SERVER(gobject);

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  switch(prop_id){
  case PROP_URL:
    {
      gchar *url;

      url = g_value_get_string(value);

      g_rec_mutex_lock(pulse_server_mutex);

      if(pulse_server->url == url){
	g_rec_mutex_unlock(pulse_server_mutex);

	return;
      }

      if(pulse_server->url != NULL){
	g_free(pulse_server->url);
      }

      pulse_server->url = g_strdup(url);

      g_rec_mutex_unlock(pulse_server_mutex);
    }
    break;
  case PROP_DEFAULT_SOUNDCARD:
    {
      GObject *default_soundcard;

      default_soundcard = (GObject *) g_value_get_object(value);

      g_rec_mutex_lock(pulse_server_mutex);

      if(pulse_server->default_soundcard == (GObject *) default_soundcard){
	g_rec_mutex_unlock(pulse_server_mutex);

	return;
      }

      if(pulse_server->default_soundcard != NULL){
	g_object_unref(G_OBJECT(pulse_server->default_soundcard));
      }

      if(default_soundcard != NULL){
	g_object_ref(G_OBJECT(default_soundcard));
      }

      pulse_server->default_soundcard = (GObject *) default_soundcard;

      g_rec_mutex_unlock(pulse_server_mutex);
    }
    break;
  case PROP_DEFAULT_PULSE_CLIENT:
    {
      AgsPulseClient *default_client;

      default_client = (AgsPulseClient *) g_value_get_object(value);

      g_rec_mutex_lock(pulse_server_mutex);

      if(pulse_server->default_client == (GObject *) default_client){
	g_rec_mutex_unlock(pulse_server_mutex);

	return;
      }

      if(pulse_server->default_client != NULL){
	g_object_unref(G_OBJECT(pulse_server->default_client));
      }

      if(default_client != NULL){
	g_object_ref(G_OBJECT(default_client));
      }

      pulse_server->default_client = (GObject *) default_client;

      g_rec_mutex_unlock(pulse_server_mutex);
    }
    break;
  case PROP_PULSE_CLIENT:
    {
      GObject *client;

      client = (GObject *) g_value_get_pointer(value);

      g_rec_mutex_lock(pulse_server_mutex);

      if(!AGS_IS_PULSE_CLIENT(client) ||
	 g_list_find(pulse_server->client, client) != NULL){
	g_rec_mutex_unlock(pulse_server_mutex);

	return;
      }

      g_object_ref(G_OBJECT(client));
      pulse_server->client = g_list_prepend(pulse_server->client,
					    client);

      g_rec_mutex_unlock(pulse_server_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_server_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsPulseServer *pulse_server;

  GRecMutex *pulse_server_mutex;

  pulse_server = AGS_PULSE_SERVER(gobject);

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);
  
  switch(prop_id){
  case PROP_URL:
    {
      g_rec_mutex_lock(pulse_server_mutex);

      g_value_set_string(value, pulse_server->url);

      g_rec_mutex_unlock(pulse_server_mutex);
    }
    break;
  case PROP_DEFAULT_SOUNDCARD:
    {
      g_rec_mutex_lock(pulse_server_mutex);

      g_value_set_object(value, pulse_server->default_soundcard);

      g_rec_mutex_unlock(pulse_server_mutex);
    }
    break;
  case PROP_DEFAULT_PULSE_CLIENT:
    {
      g_rec_mutex_lock(pulse_server_mutex);

      g_value_set_object(value, pulse_server->default_client);

      g_rec_mutex_unlock(pulse_server_mutex);
    }
    break;
  case PROP_PULSE_CLIENT:
    {
      g_rec_mutex_lock(pulse_server_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(pulse_server->client,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(pulse_server_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_server_dispose(GObject *gobject)
{
  AgsPulseServer *pulse_server;

  GList *list;
  
  pulse_server = AGS_PULSE_SERVER(gobject);

  /* default soundcard */
  if(pulse_server->default_soundcard != NULL){
    g_object_unref(G_OBJECT(pulse_server->default_soundcard));

    pulse_server->default_soundcard = NULL;
  }
  
  /* default client */
  if(pulse_server->default_client != NULL){
    g_object_unref(G_OBJECT(pulse_server->default_client));

    pulse_server->default_client = NULL;
  }
  
  /* client */
  if(pulse_server->client != NULL){
    list = pulse_server->client;

    while(list != NULL){
      g_object_run_dispose(G_OBJECT(list->data));

      list = list->next;
    }
    
    g_list_free_full(pulse_server->client,
		     g_object_unref);

    pulse_server->client = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_pulse_server_parent_class)->dispose(gobject);
}

void
ags_pulse_server_finalize(GObject *gobject)
{
  AgsPulseServer *pulse_server;

  pulse_server = AGS_PULSE_SERVER(gobject);

  /* url */
  g_free(pulse_server->url);

  /* default soundcard */
  if(pulse_server->default_soundcard != NULL){
    g_object_unref(G_OBJECT(pulse_server->default_soundcard));
  }
  
  /* default client */
  if(pulse_server->default_client != NULL){
    g_object_unref(G_OBJECT(pulse_server->default_client));
  }
  
  /* client */
  if(pulse_server->client != NULL){
    g_list_free_full(pulse_server->client,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_pulse_server_parent_class)->finalize(gobject);
}

AgsUUID*
ags_pulse_server_get_uuid(AgsConnectable *connectable)
{
  AgsPulseServer *pulse_server;
  
  AgsUUID *ptr;

  GRecMutex *pulse_server_mutex;

  pulse_server = AGS_PULSE_SERVER(connectable);

  /* get pulse server signal mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  /* get UUID */
  g_rec_mutex_lock(pulse_server_mutex);

  ptr = pulse_server->uuid;

  g_rec_mutex_unlock(pulse_server_mutex);
  
  return(ptr);
}

gboolean
ags_pulse_server_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_pulse_server_is_ready(AgsConnectable *connectable)
{
  AgsPulseServer *pulse_server;
  
  gboolean is_ready;

  pulse_server = AGS_PULSE_SERVER(connectable);

  /* check is added */
  is_ready = ags_pulse_server_test_flags(pulse_server, AGS_PULSE_SERVER_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_pulse_server_add_to_registry(AgsConnectable *connectable)
{
  AgsPulseServer *pulse_server;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  pulse_server = AGS_PULSE_SERVER(connectable);

  ags_pulse_server_set_flags(pulse_server, AGS_PULSE_SERVER_ADDED_TO_REGISTRY);
}

void
ags_pulse_server_remove_from_registry(AgsConnectable *connectable)
{
  AgsPulseServer *pulse_server;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  pulse_server = AGS_PULSE_SERVER(connectable);

  ags_pulse_server_unset_flags(pulse_server, AGS_PULSE_SERVER_ADDED_TO_REGISTRY);
}

xmlNode*
ags_pulse_server_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_pulse_server_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_pulse_server_xml_parse(AgsConnectable *connectable,
			   xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_pulse_server_is_connected(AgsConnectable *connectable)
{
  AgsPulseServer *pulse_server;
  
  gboolean is_connected;

  pulse_server = AGS_PULSE_SERVER(connectable);

  /* check is connected */
  is_connected = ags_pulse_server_test_flags(pulse_server, AGS_PULSE_SERVER_CONNECTED);
  
  return(is_connected);
}

void
ags_pulse_server_connect(AgsConnectable *connectable)
{
  AgsPulseServer *pulse_server;

  GList *list_start, *list;  

  GRecMutex *pulse_server_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  pulse_server = AGS_PULSE_SERVER(connectable);

  ags_pulse_server_set_flags(pulse_server, AGS_PULSE_SERVER_CONNECTED);

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  list =
    list_start = g_list_copy(pulse_server->client);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_pulse_server_disconnect(AgsConnectable *connectable)
{
  AgsPulseServer *pulse_server;

  GList *list_start, *list;

  GRecMutex *pulse_server_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  pulse_server = AGS_PULSE_SERVER(connectable);
  
  ags_pulse_server_unset_flags(pulse_server, AGS_PULSE_SERVER_CONNECTED);

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  /* client */
  list =
    list_start = g_list_copy(pulse_server->client);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_pulse_server_test_flags:
 * @pulse_server: the #AgsPulseServer
 * @flags: the flags
 *
 * Test @flags to be set on @pulse_server.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_pulse_server_test_flags(AgsPulseServer *pulse_server, guint flags)
{
  gboolean retval;  
  
  GRecMutex *pulse_server_mutex;

  if(!AGS_IS_PULSE_SERVER(pulse_server)){
    return(FALSE);
  }

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  /* test */
  g_rec_mutex_lock(pulse_server_mutex);

  retval = (flags & (pulse_server->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(pulse_server_mutex);

  return(retval);
}

/**
 * ags_pulse_server_set_flags:
 * @pulse_server: the #AgsPulseServer
 * @flags: see #AgsPulseServerFlags-enum
 *
 * Enable a feature of @pulse_server.
 *
 * Since: 3.0.0
 */
void
ags_pulse_server_set_flags(AgsPulseServer *pulse_server, guint flags)
{
  GRecMutex *pulse_server_mutex;

  if(!AGS_IS_PULSE_SERVER(pulse_server)){
    return;
  }

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(pulse_server_mutex);

  pulse_server->flags |= flags;
  
  g_rec_mutex_unlock(pulse_server_mutex);
}
    
/**
 * ags_pulse_server_unset_flags:
 * @pulse_server: the #AgsPulseServer
 * @flags: see #AgsPulseServerFlags-enum
 *
 * Disable a feature of @pulse_server.
 *
 * Since: 3.0.0
 */
void
ags_pulse_server_unset_flags(AgsPulseServer *pulse_server, guint flags)
{  
  GRecMutex *pulse_server_mutex;

  if(!AGS_IS_PULSE_SERVER(pulse_server)){
    return;
  }

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(pulse_server_mutex);

  pulse_server->flags &= (~flags);
  
  g_rec_mutex_unlock(pulse_server_mutex);
}

void
ags_pulse_server_set_url(AgsSoundServer *sound_server,
			 gchar *url)
{
  AgsPulseServer *pulse_server;

  GRecMutex *pulse_server_mutex;

  pulse_server = AGS_PULSE_SERVER(sound_server);

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  /* set URL */
  g_rec_mutex_lock(pulse_server_mutex);

  pulse_server->url = g_strdup(url);

  g_rec_mutex_unlock(pulse_server_mutex);
}

gchar*
ags_pulse_server_get_url(AgsSoundServer *sound_server)
{
  AgsPulseServer *pulse_server;

  gchar *url;

  GRecMutex *pulse_server_mutex;

  pulse_server = AGS_PULSE_SERVER(sound_server);

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  /* set URL */
  g_rec_mutex_lock(pulse_server_mutex);

  url = pulse_server->url;

  g_rec_mutex_unlock(pulse_server_mutex);
  
  return(url);
}


void
ags_pulse_server_set_ports(AgsSoundServer *sound_server,
			   guint *port, guint port_count)
{
  AgsPulseServer *pulse_server;

  GRecMutex *pulse_server_mutex;

  pulse_server = AGS_PULSE_SERVER(sound_server);

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  /* set ports */
  g_rec_mutex_lock(pulse_server_mutex);

  pulse_server->port = port;
  pulse_server->port_count = port_count;

  g_rec_mutex_unlock(pulse_server_mutex);
}

guint*
ags_pulse_server_get_ports(AgsSoundServer *sound_server,
			   guint *port_count)
{
  AgsPulseServer *pulse_server;

  guint *port;
  
  GRecMutex *pulse_server_mutex;

  pulse_server = AGS_PULSE_SERVER(sound_server);

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  /* get ports */
  g_rec_mutex_lock(pulse_server_mutex);

  if(port_count != NULL){
    *port_count = AGS_PULSE_SERVER(sound_server)->port_count;
  }

  port = pulse_server->port;

  g_rec_mutex_unlock(pulse_server_mutex);
  
  return(port);
}

void
ags_pulse_server_set_soundcard(AgsSoundServer *sound_server,
			       gchar *client_uuid,
			       GList *soundcard)
{
  AgsPulseServer *pulse_server;
  AgsPulseClient *pulse_client;

  GList *list;

  pulse_server = AGS_PULSE_SERVER(sound_server);
  
  pulse_client = (AgsPulseClient *) ags_pulse_server_find_client(pulse_server,
								 client_uuid);

  if(!AGS_IS_PULSE_CLIENT(pulse_client)){
    return;
  }
  
  //NOTE:JK: soundcard won't removed
  list = soundcard;

  while(list != NULL){
    ags_pulse_client_add_device(pulse_client,
				(GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_pulse_server_get_soundcard(AgsSoundServer *sound_server,
			       gchar *client_uuid)
{
  AgsPulseServer *pulse_server;
  AgsPulseClient *pulse_client;

  GList *device_start, *device;
  GList *list;
  
  pulse_server = AGS_PULSE_SERVER(sound_server);

  pulse_client = (AgsPulseClient *) ags_pulse_server_find_client(pulse_server,
								 client_uuid);

  if(!AGS_IS_PULSE_CLIENT(pulse_client)){
    return(NULL);
  }

  g_object_get(pulse_client,
	       "device", &device_start,
	       NULL);
  
  device = device_start;
  list = NULL;

  while(device != NULL){
    if(AGS_IS_PULSE_DEVOUT(device->data) ||
       AGS_IS_PULSE_DEVIN(device->data)){
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
ags_pulse_server_set_sequencer(AgsSoundServer *sound_server,
			       gchar *client_uuid,
			       GList *sequencer)
{
  AgsPulseServer *pulse_server;
  AgsPulseClient *pulse_client;

  GList *list;

  pulse_server = AGS_PULSE_SERVER(sound_server);

  pulse_client = (AgsPulseClient *) ags_pulse_server_find_client(pulse_server,
								 client_uuid);

  if(!AGS_IS_PULSE_CLIENT(pulse_client)){
    return;
  }

  //NOTE:JK: sequencer won't removed
  list = sequencer;

  while(list != NULL){
    ags_pulse_client_add_device(pulse_client,
				(GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_pulse_server_get_sequencer(AgsSoundServer *sound_server,
			       gchar *client_uuid)
{
  AgsPulseServer *pulse_server;
  AgsPulseClient *pulse_client;

  GList *device_start, *device;
  GList *list;
  
  pulse_server = AGS_PULSE_SERVER(sound_server);

  pulse_client = (AgsPulseClient *) ags_pulse_server_find_client(pulse_server,
								 client_uuid);

  if(!AGS_IS_PULSE_CLIENT(pulse_client)){
    return(NULL);
  }

  g_object_get(pulse_client,
	       "device", &device_start,
	       NULL);
  
  device = device_start;
  list = NULL;

#if 0  
  while(device != NULL){
    if(AGS_IS_PULSE_MIDIIN(device->data)){
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
ags_pulse_server_register_soundcard(AgsSoundServer *sound_server,
				    gboolean is_output)
{
  AgsPulseServer *pulse_server;
  AgsPulseClient *default_client;
  AgsPulsePort *pulse_port;
  AgsPulseDevout *pulse_devout;
  AgsPulseDevin *pulse_devin;

  AgsApplicationContext *application_context;

  GObject *soundcard;
  
#ifdef AGS_WITH_PULSE
  pa_context *context;
#else
  gpointer context;
#endif

  gchar *str;  

  guint n_soundcards;
  gboolean initial_set;
  guint i;  

  GRecMutex *pulse_server_mutex;
  GRecMutex *pulse_client_mutex;

  pulse_server = AGS_PULSE_SERVER(sound_server);

  application_context= ags_application_context_get_instance();

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  /* the default client */
  initial_set = FALSE;
  
  /* get some fields */
  g_rec_mutex_lock(pulse_server_mutex);

  if(pulse_server->main_loop == NULL){
#ifdef AGS_WITH_PULSE
    pulse_server->main_loop = pa_mainloop_new();
    pulse_server->main_loop_api = pa_mainloop_get_api(pulse_server->main_loop);
#else
    pulse_server->main_loop = NULL;
    pulse_server->main_loop_api = NULL;
#endif
  }
  
  default_client = (AgsPulseClient *) pulse_server->default_client;

  n_soundcards = pulse_server->n_soundcards;
  
  g_rec_mutex_unlock(pulse_server_mutex);

  /* the default client */
  if(default_client == NULL){
    default_client = ags_pulse_client_new((GObject *) pulse_server);
    
    g_object_set(pulse_server,
		 "default-pulse-client", default_client,
		 NULL);
    ags_pulse_server_add_client(pulse_server,
				(GObject *) default_client);
    
    ags_pulse_client_open((AgsPulseClient *) default_client,
			  "ags-default-client");
    initial_set = TRUE;    
  }

  /* get pulse client mutex */
  pulse_client_mutex = AGS_PULSE_CLIENT_GET_OBJ_MUTEX(default_client);

  /* get context */
  g_rec_mutex_lock(pulse_client_mutex);

  context = default_client->context;

  g_rec_mutex_unlock(pulse_client_mutex);
  
  if(context == NULL){
    g_warning("ags_pulse_server.c - can't open pulseaudio client");
  }

  /* the soundcard */
  soundcard = NULL;

  /* the soundcard */
  if(is_output){
    pulse_devout = ags_pulse_devout_new();
    soundcard = (GObject *) pulse_devout;
    
    str = g_strdup_printf("ags-pulse-devout-%d",
			  n_soundcards);
    
    g_object_set(AGS_PULSE_DEVOUT(pulse_devout),
		 "pulse-client", default_client,
		 "device", str,
		 NULL);
    g_free(str);
            
    /* register ports */      
    pulse_port = ags_pulse_port_new((GObject *) default_client);

    str = g_strdup_printf("ags-soundcard%d",
			  n_soundcards);

    g_object_set(pulse_port,
		 "pulse-devout", pulse_devout,
		 NULL);
    ags_pulse_client_add_port(default_client,
			      (GObject *) pulse_port);

    g_object_set(pulse_devout,
		 "pulse-port", pulse_port,
		 NULL);

    pulse_devout->port_name = (gchar **) malloc(2 * sizeof(gchar *));
    pulse_devout->port_name[0] = g_strdup(str);
    pulse_devout->port_name[1] = NULL;
    
    ags_pulse_port_register(pulse_port,
			    str,
			    TRUE, FALSE,
			    TRUE);

    ags_pulse_devout_realloc_buffer(pulse_devout);

    g_object_set(default_client,
		 "device", pulse_devout,
		 NULL);

    /* increment n-soundcards */
    g_rec_mutex_lock(pulse_server_mutex);

    pulse_server->n_soundcards += 1;

    g_rec_mutex_unlock(pulse_server_mutex);
  }else{
    pulse_devin = ags_pulse_devin_new();
    soundcard = (GObject *) pulse_devin;

    str = g_strdup_printf("ags-pulse-devin-%d",
			  pulse_server->n_soundcards);
    
    g_object_set(AGS_PULSE_DEVIN(pulse_devin),
		 "pulse-client", default_client,
		 "device", str,
		 NULL);
    g_free(str);
        
    /* register ports */
    str = g_strdup_printf("ags-soundcard%d",
			  n_soundcards);
    
#ifdef AGS_DEBUG
    g_message("%s", str);
#endif
      
    pulse_port = ags_pulse_port_new((GObject *) default_client);
    g_object_set(pulse_port,
		 "pulse-devin", pulse_devin,
		 NULL);
    ags_pulse_client_add_port(default_client,
			      (GObject *) pulse_port);

    g_object_set(pulse_devin,
		 "pulse-port", pulse_port,
		 NULL);
      
    pulse_devin->port_name = (gchar **) malloc(2 * sizeof(gchar *));
    pulse_devin->port_name[0] = g_strdup(str);
    pulse_devin->port_name[1] = NULL;
    
    ags_pulse_port_register(pulse_port,
			    str,
			    TRUE, FALSE,
			    TRUE);

    ags_pulse_devin_realloc_buffer(pulse_devin);

    g_object_set(default_client,
		 "device", pulse_devin,
		 NULL);
    
    /* increment n-soundcards */
    g_rec_mutex_lock(pulse_server_mutex);

    pulse_server->n_soundcards += 1;

    g_rec_mutex_unlock(pulse_server_mutex);
  }
  
  return((GObject *) soundcard);
}

void
ags_pulse_server_unregister_soundcard(AgsSoundServer *sound_server,
				      GObject *soundcard)
{
  AgsPulseServer *pulse_server;
  AgsPulseClient *default_client;

  GList *list_start, *list;
  GList *port;
  
  GRecMutex *pulse_server_mutex;

  pulse_server = AGS_PULSE_SERVER(sound_server);

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);
  
  /* the default client */
  g_object_get(pulse_server,
	       "default-pulse-client", &default_client,
	       NULL);

  if(default_client == NULL){
    g_warning("GSequencer - no pulse client");
    
    return;
  }
  
  if(AGS_IS_PULSE_DEVOUT(soundcard)){
    g_object_get(soundcard,
		 "pulse-port", &list_start,
		 NULL);

    list = list_start;

    while(list != NULL){
      ags_pulse_port_unregister(list->data);
      ags_pulse_client_remove_port(default_client,
				   list->data);
    
      list = list->next;
    }

    g_list_free_full(list_start,
		     g_object_unref);
  }else if(AGS_IS_PULSE_DEVIN(soundcard)){
    g_object_get(soundcard,
		 "pulse-port", &list_start,
		 NULL);

    list = list_start;

    while(list != NULL){
      ags_pulse_port_unregister(list->data);
      ags_pulse_client_remove_port(default_client,
				   list->data);
    
      list = list->next;
    }

    g_list_free_full(list_start,
		     g_object_unref);
  }
  
  ags_pulse_client_remove_device(default_client,
				 soundcard);
  
  g_object_get(default_client,
	       "port", &port,
	       NULL);
  
  if(port == NULL){
    /* reset n-soundcards */
    g_rec_mutex_lock(pulse_server_mutex);

    pulse_server->n_soundcards = 0;

    g_rec_mutex_unlock(pulse_server_mutex);
  }

  g_object_unref(default_client);
  
  g_list_free_full(port,
		   g_object_unref);
}

GObject*
ags_pulse_server_register_sequencer(AgsSoundServer *sound_server,
				    gboolean is_output)
{
  g_message("GSequencer - can't register pulseaudio sequencer");
  
  return(NULL);
}

void
ags_pulse_server_unregister_sequencer(AgsSoundServer *sound_server,
				      GObject *sequencer)
{
  g_message("GSequencer - can't unregister pulseaudio sequencer");
}

/**
 * ags_pulse_server_register_default_soundcard:
 * @pulse_server: the #AgsPulseServer
 * 
 * Register default soundcard.
 * 
 * Returns: the instantiated #AgsPulseDevout
 * 
 * Since: 3.0.0
 */
GObject*
ags_pulse_server_register_default_soundcard(AgsPulseServer *pulse_server)
{
  AgsPulseClient *default_client;
  AgsPulseDevout *pulse_devout;
  AgsPulsePort *pulse_port;

  AgsApplicationContext *application_context;
  
#ifdef AGS_WITH_PULSE
  pa_context *context;
#else
  gpointer context;
#endif

  gchar *str;
  
  guint i;

  GRecMutex *pulse_server_mutex;
  GRecMutex *pulse_client_mutex;

  if(!AGS_IS_PULSE_SERVER(pulse_server)){
    return(NULL);
  }

  application_context = ags_application_context_get_instance();

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  /* get some fields */
  g_rec_mutex_lock(pulse_server_mutex);

  default_client = (AgsPulseClient *) pulse_server->default_client;

  g_rec_mutex_unlock(pulse_server_mutex);
    
  /* the default client */
  if(default_client == NULL){
    default_client = ags_pulse_client_new((GObject *) pulse_server);
    
    g_object_set(pulse_server,
		 "default-pulse-client", default_client,
		 NULL);
    ags_pulse_server_add_client(pulse_server,
				(GObject *) default_client);
    
    ags_pulse_client_open((AgsPulseClient *) default_client,
			  "ags-default-client");
  }

  /* get pulse client mutex */
  pulse_client_mutex = AGS_PULSE_CLIENT_GET_OBJ_MUTEX(default_client);

  /* get context */
  g_rec_mutex_lock(pulse_client_mutex);

  context = default_client->context;

  g_rec_mutex_unlock(pulse_client_mutex);
  
  if(context == NULL){
    g_warning("ags_pulse_server.c - can't open pulseaudio client");
  }

  /* the soundcard */
  pulse_devout = ags_pulse_devout_new();
  
  g_object_set(AGS_PULSE_DEVOUT(pulse_devout),
	       "pulse-client", default_client,
	       "device", "ags-default-devout",
	       NULL);
    
  /* register ports */
  str = g_strdup_printf("ags-default-soundcard");

#ifdef AGS_DEBUG
  g_message("%s", str);
#endif
    
  pulse_port = ags_pulse_port_new((GObject *) default_client);
  g_object_set(pulse_port,
	       "pulse-devout", pulse_devout,
	       NULL);
  ags_pulse_client_add_port(default_client,
			    (GObject *) pulse_port);

  g_object_set(pulse_devout,
	       "pulse-port", pulse_port,
	       NULL);
  
  pulse_devout->port_name = (gchar **) malloc(2 * sizeof(gchar *));
  pulse_devout->port_name[0] = g_strdup(str);
  pulse_devout->port_name[1] = NULL;
  
  ags_pulse_port_register(pulse_port,
			  str,
			  TRUE, FALSE,
			  TRUE);

  g_free(str);

  g_object_set(default_client,
	       "device", pulse_devout,
	       NULL);
  
  return((GObject *) pulse_devout);
}

/**
 * ags_pulse_server_find_url:
 * @pulse_server: (element-type AgsAudio.PulseServer) (transfer none): the #GList-struct containing #AgsPulseServer
 * @url: the url to find
 *
 * Find #AgsPulseServer by url.
 *
 * Returns: (element-type AgsAudio.PulseServer) (transfer none): the #GList-struct containing a #AgsPulseServer matching @url or %NULL
 *
 * Since: 3.0.0
 */
GList*
ags_pulse_server_find_url(GList *pulse_server,
			  gchar *url)
{
  GList *retval;
  
  GRecMutex *pulse_server_mutex;

  retval = NULL;
  
  while(pulse_server != NULL){
    /* get pulse server mutex */
    pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server->data);

    /* check URL */
    g_rec_mutex_lock(pulse_server_mutex);
    
    if(!g_ascii_strcasecmp(AGS_PULSE_SERVER(pulse_server->data)->url,
			   url)){
      retval = pulse_server;

      g_rec_mutex_unlock(pulse_server_mutex);
    
      break;
    }

    g_rec_mutex_unlock(pulse_server_mutex);
    
    pulse_server = pulse_server->next;
  }

  return(retval);
}

/**
 * ags_pulse_server_find_client:
 * @pulse_server: the #AgsPulseServer
 * @client_uuid: the uuid to find
 *
 * Find #AgsPulseClient by uuid.
 *
 * Returns: (transfer none): the #AgsPulseClient found or %NULL
 *
 * Since: 3.0.0
 */
GObject*
ags_pulse_server_find_client(AgsPulseServer *pulse_server,
			     gchar *client_uuid)
{
  AgsPulseClient *retval;
  
  GList *list_start, *list;

  GRecMutex *pulse_server_mutex;
  GRecMutex *pulse_client_mutex;

  if(!AGS_IS_PULSE_SERVER(pulse_server)){
    return(NULL);
  }

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  /* get some fields */
  g_rec_mutex_lock(pulse_server_mutex);

  list =
    list_start = g_list_copy(pulse_server->client);

  g_rec_mutex_unlock(pulse_server_mutex);

  retval = NULL;
  
  while(list != NULL){
    /* get pulse client mutex */
    pulse_client_mutex = AGS_PULSE_CLIENT_GET_OBJ_MUTEX(list->data);

    /* check client UUID */
    g_rec_mutex_lock(pulse_client_mutex);
    
    if(!g_ascii_strcasecmp(AGS_PULSE_CLIENT(list->data)->client_uuid,
			   client_uuid)){
      retval = list->data;

      g_rec_mutex_unlock(pulse_client_mutex);

      break;
    }

    g_rec_mutex_unlock(pulse_client_mutex);
    
    list = list->next;
  }

  g_list_free(list_start);
  
  return((GObject *) retval);
}

/**
 * ags_pulse_server_find_port:
 * @pulse_server: the #AgsPulseServer
 * @port_uuid: the uuid to find
 *
 * Find #AgsPulsePort by uuid.
 *
 * Returns: (transfer none): the #AgsPulsePort found or %NULL
 *
 * Since: 3.0.0
 */
GObject*
ags_pulse_server_find_port(AgsPulseServer *pulse_server,
			   gchar *port_uuid)
{
  GList *client_start, *client;
  GList *port_start, *port;

  gboolean success;
  
  GRecMutex *pulse_port_mutex;

  g_object_get(pulse_server,
	       "pulse-client", &client_start,
	       NULL);

  client = client_start;
  
  while(client != NULL){
    g_object_get(pulse_server,
		 "pulse-port", &port_start,
		 NULL);

    port = port_start;
    
    while(port != NULL){
      /* get pulse port mutex */
      pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(port->data);
      
      /* check port UUID */
      g_rec_mutex_lock(pulse_port_mutex);
      
      success = (!g_ascii_strcasecmp(AGS_PULSE_PORT(port->data)->port_uuid,
				     port_uuid)) ? TRUE: FALSE;

      g_rec_mutex_unlock(pulse_port_mutex);
      
      if(success){
	AgsPulsePort *retval;

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
 * ags_pulse_server_add_client:
 * @pulse_server: the #AgsPulseServer
 * @pulse_client: the #AgsPulseClient to add
 *
 * Add @pulse_client to @pulse_server
 *
 * Since: 3.0.0
 */
void
ags_pulse_server_add_client(AgsPulseServer *pulse_server,
			    GObject *pulse_client)
{
  GRecMutex *pulse_server_mutex;

  if(!AGS_IS_PULSE_SERVER(pulse_server) ||
     !AGS_IS_PULSE_CLIENT(pulse_client)){
    return;
  }

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  /* get some fields */
  g_rec_mutex_lock(pulse_server_mutex);

  if(g_list_find(pulse_server->client, pulse_client) == NULL){
    g_object_ref(pulse_client);
    pulse_server->client = g_list_prepend(pulse_server->client,
					  pulse_client);
  }

  g_rec_mutex_unlock(pulse_server_mutex);
}

/**
 * ags_pulse_server_remove_client:
 * @pulse_server: the #AgsPulseServer
 * @pulse_client: the #AgsPulseClient to remove
 *
 * Remove @pulse_client to @pulse_server
 *
 * Since: 3.0.0
 */
void
ags_pulse_server_remove_client(AgsPulseServer *pulse_server,
			       GObject *pulse_client)
{
  GRecMutex *pulse_server_mutex;

  if(!AGS_IS_PULSE_SERVER(pulse_server) ||
     !AGS_IS_PULSE_CLIENT(pulse_client)){
    return;
  }

  /* get pulse server mutex */
  pulse_server_mutex = AGS_PULSE_SERVER_GET_OBJ_MUTEX(pulse_server);

  /* get some fields */
  g_rec_mutex_lock(pulse_server_mutex);

  if(g_list_find(pulse_server->client, pulse_client) != NULL){
    pulse_server->client = g_list_remove(pulse_server->client,
					 pulse_client);
    g_object_unref(pulse_client);
  }

  g_rec_mutex_unlock(pulse_server_mutex);
}

/**
 * ags_pulse_server_connect_client:
 * @pulse_server: the #AgsPulseServer
 *
 * Connect all clients.
 *
 * Since: 3.0.0
 */
void
ags_pulse_server_connect_client(AgsPulseServer *pulse_server)
{
  GList *client_start, *client;

  gchar *client_name;
  
  GRecMutex *pulse_client_mutex;

  if(!AGS_IS_PULSE_SERVER(pulse_server)){
    return;
  }

  g_object_get(pulse_server,
	       "pulse-client", &client_start,
	       NULL);
  
  client = client_start;

  while(client != NULL){
    /* client name */
    g_object_get(client->data,
		 "client-name", &client_name,
		 NULL);
    
    /* open */
    ags_pulse_client_open((AgsPulseClient *) client->data,
			  client_name);
    ags_pulse_client_activate(client->data);
    
    /* iterate */
    client = client->next;
  }

  g_list_free_full(client_start,
		   g_object_unref);
}

/**
 * ags_pulse_server_disconnect_client:
 * @pulse_server: the #AgsPulseServer
 *
 * Disconnect all clients.
 *
 * Since: 3.0.0
 */
void
ags_pulse_server_disconnect_client(AgsPulseServer *pulse_server)
{
  GList *client_start, *client;
  
  if(!AGS_IS_PULSE_SERVER(pulse_server)){
    return;
  }

  g_object_get(pulse_server,
	       "pulse-client", &client_start,
	       NULL);
  
  client = client_start;

  while(client != NULL){
    /* close */
    ags_pulse_client_deactivate(client->data);
    
    ags_pulse_client_close((AgsPulseClient *) client->data);

    /* iterate */
    client = client->next;
  }

  g_list_free_full(client_start,
		   g_object_unref);
}


void*
ags_pulse_server_do_poll_loop(void *ptr)
{
  AgsPulseServer *pulse_server;

#ifdef AGS_WITH_RT
  AgsPriority *priority;

  struct sched_param param;
    
  gchar *str;
#endif

  pulse_server = (AgsPulseServer *) ptr;
    
  /* Declare ourself as a real time task */
#ifdef AGS_WITH_RT
  priority = ags_priority_get_instance();
    
  /* Declare ourself as a real time task */
  param.sched_priority = 1;

  str = ags_priority_get_value(priority,
			       AGS_PRIORITY_RT_THREAD,
			       AGS_PRIORITY_KEY_AUDIO);

  if(str != NULL){
    param.sched_priority = (int) g_ascii_strtoull(str,
						  NULL,
						  10);

    g_free(str);
  }
      
  if(sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
    perror("sched_setscheduler failed");
  }
#endif

#ifdef AGS_WITH_PULSE
  pa_mainloop_run(pulse_server->main_loop,
		  NULL);
#endif

  g_thread_exit(NULL);

  return(NULL);
}

void
ags_pulse_server_start_poll(AgsPulseServer *pulse_server)
{
  pulse_server->thread = g_thread_new("Advanced Gtk+ Sequencer - pulseaudio server",
				      ags_pulse_server_do_poll_loop,
				      pulse_server);
}

/**
 * ags_pulse_server_new:
 * @url: the URL as string
 *
 * Create a new instance of #AgsPulseServer.
 *
 * Returns: the new #AgsPulseServer
 *
 * Since: 3.0.0
 */
AgsPulseServer*
ags_pulse_server_new(gchar *url)
{
  AgsPulseServer *pulse_server;

  pulse_server = (AgsPulseServer *) g_object_new(AGS_TYPE_PULSE_SERVER,
						 "url", url,
						 NULL);

  return(pulse_server);
}
