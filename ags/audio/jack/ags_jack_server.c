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

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_port.h>

#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_devin.h>
#include <ags/audio/jack/ags_jack_midiin.h>

#include <string.h>

#include <errno.h>

#include <ags/i18n.h>

void ags_jack_server_class_init(AgsJackServerClass *jack_server);
void ags_jack_server_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_jack_server_sound_server_interface_init(AgsSoundServerInterface *sound_server);
void ags_jack_server_init(AgsJackServer *jack_server);
void ags_jack_server_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_jack_server_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_jack_server_dispose(GObject *gobject);
void ags_jack_server_finalize(GObject *gobject);

AgsUUID* ags_jack_server_get_uuid(AgsConnectable *connectable);
gboolean ags_jack_server_has_resource(AgsConnectable *connectable);
gboolean ags_jack_server_is_ready(AgsConnectable *connectable);
void ags_jack_server_add_to_registry(AgsConnectable *connectable);
void ags_jack_server_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_jack_server_list_resource(AgsConnectable *connectable);
xmlNode* ags_jack_server_xml_compose(AgsConnectable *connectable);
void ags_jack_server_xml_parse(AgsConnectable *connectable,
			       xmlNode *node);
gboolean ags_jack_server_is_connected(AgsConnectable *connectable);
void ags_jack_server_connect(AgsConnectable *connectable);
void ags_jack_server_disconnect(AgsConnectable *connectable);

void ags_jack_server_set_url(AgsSoundServer *sound_server,
			     gchar *url);
gchar* ags_jack_server_get_url(AgsSoundServer *sound_server);
void ags_jack_server_set_ports(AgsSoundServer *sound_server,
			       guint *ports, guint port_count);
guint* ags_jack_server_get_ports(AgsSoundServer *sound_server,
				 guint *port_count);
void ags_jack_server_set_soundcard(AgsSoundServer *sound_server,
				   gchar *client_uuid,
				   GList *soundcard);
GList* ags_jack_server_get_soundcard(AgsSoundServer *sound_server,
				     gchar *client_uuid);
void ags_jack_server_set_sequencer(AgsSoundServer *sound_server,
				   gchar *client_uuid,
				   GList *sequencer);
GList* ags_jack_server_get_sequencer(AgsSoundServer *sound_server,
				     gchar *client_uuid);
GObject* ags_jack_server_register_soundcard(AgsSoundServer *sound_server,
					    gboolean is_output);
void ags_jack_server_unregister_soundcard(AgsSoundServer *sound_server,
					  GObject *soundcard);
GObject* ags_jack_server_register_sequencer(AgsSoundServer *sound_server,
					    gboolean is_output);
void ags_jack_server_unregister_sequencer(AgsSoundServer *sound_server,
					  GObject *sequencer);

/**
 * SECTION:ags_jack_server
 * @short_description: JACK instance
 * @title: AgsJackServer
 * @section_id:
 * @include: ags/audio/jack/ags_jack_server.h
 *
 * The #AgsJackServer is an object to represent a running JACK instance.
 */

enum{
  PROP_0,
  PROP_URL,
  PROP_DEFAULT_SOUNDCARD,
  PROP_DEFAULT_JACK_CLIENT,
  PROP_INPUT_JACK_CLIENT,
  PROP_JACK_CLIENT,
};

static gpointer ags_jack_server_parent_class = NULL;

GType
ags_jack_server_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_jack_server = 0;

    static const GTypeInfo ags_jack_server_info = {
      sizeof(AgsJackServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsJackServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_jack_server_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_jack_server_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_sound_server_interface_info = {
      (GInterfaceInitFunc) ags_jack_server_sound_server_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_jack_server = g_type_register_static(G_TYPE_OBJECT,
						  "AgsJackServer",
						  &ags_jack_server_info,
						  0);

    g_type_add_interface_static(ags_type_jack_server,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_jack_server,
				AGS_TYPE_SOUND_SERVER,
				&ags_sound_server_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_jack_server);
  }

  return g_define_type_id__volatile;
}

void
ags_jack_server_class_init(AgsJackServerClass *jack_server)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_jack_server_parent_class = g_type_class_peek_parent(jack_server);

  /* GObjectClass */
  gobject = (GObjectClass *) jack_server;

  gobject->set_property = ags_jack_server_set_property;
  gobject->get_property = ags_jack_server_get_property;

  gobject->dispose = ags_jack_server_dispose;
  gobject->finalize = ags_jack_server_finalize;

  /* properties */
  /**
   * AgsJackServer:url:
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
   * AgsJackServer:default-soundcard:
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
   * AgsJackServer:default-jack-client:
   *
   * The default jack client.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("default-jack-client",
				   i18n_pspec("default jack client"),
				   i18n_pspec("The default jack client"),
				   AGS_TYPE_JACK_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_JACK_CLIENT,
				  param_spec);

  /**
   * AgsJackServer:input-jack-client:
   *
   * The input jack client.
   * 
   * Since: 2.1.15
   */
  param_spec = g_param_spec_object("input-jack-client",
				   i18n_pspec("input jack client"),
				   i18n_pspec("The input jack client"),
				   AGS_TYPE_JACK_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_JACK_CLIENT,
				  param_spec);

  /**
   * AgsJackServer:jack-client:
   *
   * The jack client list.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("jack-client",
				    i18n_pspec("jack client list"),
				    i18n_pspec("The jack client list"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_JACK_CLIENT,
				  param_spec);
}

void
ags_jack_server_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_jack_server_get_uuid;
  connectable->has_resource = ags_jack_server_has_resource;

  connectable->is_ready = ags_jack_server_is_ready;
  connectable->add_to_registry = ags_jack_server_add_to_registry;
  connectable->remove_from_registry = ags_jack_server_remove_from_registry;

  connectable->list_resource = ags_jack_server_list_resource;
  connectable->xml_compose = ags_jack_server_xml_compose;
  connectable->xml_parse = ags_jack_server_xml_parse;

  connectable->is_connected = ags_jack_server_is_connected;  
  connectable->connect = ags_jack_server_connect;
  connectable->disconnect = ags_jack_server_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_jack_server_sound_server_interface_init(AgsSoundServerInterface *sound_server)
{
  sound_server->set_url = ags_jack_server_set_url;
  sound_server->get_url = ags_jack_server_get_url;
  sound_server->set_ports = ags_jack_server_set_ports;
  sound_server->get_ports = ags_jack_server_get_ports;
  sound_server->set_soundcard = ags_jack_server_set_soundcard;
  sound_server->get_soundcard = ags_jack_server_get_soundcard;
  sound_server->set_sequencer = ags_jack_server_set_sequencer;
  sound_server->get_sequencer = ags_jack_server_get_sequencer;
  sound_server->register_soundcard = ags_jack_server_register_soundcard;
  sound_server->unregister_soundcard = ags_jack_server_unregister_soundcard;
  sound_server->register_sequencer = ags_jack_server_register_sequencer;
  sound_server->unregister_sequencer = ags_jack_server_unregister_sequencer;
}

void
ags_jack_server_init(AgsJackServer *jack_server)
{
  /* flags */
  jack_server->flags = 0;

  /* server mutex */
  g_rec_mutex_init(&(jack_server->obj_mutex));

  /* uuid */
  jack_server->uuid = ags_uuid_alloc();
  ags_uuid_generate(jack_server->uuid);

  /* fields */
  jack_server->url = g_strdup_printf("%s://%s:%d",
				     AGS_JACK_SERVER_DEFAULT_PROTOCOL,
				     AGS_JACK_SERVER_DEFAULT_HOST,
				     AGS_JACK_SERVER_DEFAULT_PORT);
  //  jack_server->jackctl = jackctl_server_create(NULL,
  //					       NULL);

  jack_server->port = NULL;
  jack_server->port_count = 0;
  
  jack_server->n_soundcards = 0;
  jack_server->n_sequencers = 0;

  jack_server->default_soundcard = NULL;

  jack_server->default_client = NULL;
  jack_server->input_client = NULL;

  jack_server->client = NULL;
}

void
ags_jack_server_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackServer *jack_server;

  GRecMutex *jack_server_mutex;

  jack_server = AGS_JACK_SERVER(gobject);

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  switch(prop_id){
  case PROP_URL:
    {
      gchar *url;

      url = g_value_get_string(value);

      g_rec_mutex_lock(jack_server_mutex);

      if(jack_server->url == url){
	g_rec_mutex_unlock(jack_server_mutex);

	return;
      }

      if(jack_server->url != NULL){
	g_free(jack_server->url);
      }

      jack_server->url = g_strdup(url);

      g_rec_mutex_unlock(jack_server_mutex);
    }
    break;
  case PROP_DEFAULT_SOUNDCARD:
    {
      GObject *default_soundcard;

      default_soundcard = (GObject *) g_value_get_object(value);

      g_rec_mutex_lock(jack_server_mutex);

      if(jack_server->default_soundcard == (GObject *) default_soundcard){
	g_rec_mutex_unlock(jack_server_mutex);

	return;
      }

      if(jack_server->default_soundcard != NULL){
	g_object_unref(G_OBJECT(jack_server->default_soundcard));
      }

      if(default_soundcard != NULL){
	g_object_ref(G_OBJECT(default_soundcard));
      }

      jack_server->default_soundcard = (GObject *) default_soundcard;

      g_rec_mutex_unlock(jack_server_mutex);
    }
    break;
  case PROP_DEFAULT_JACK_CLIENT:
    {
      AgsJackClient *default_client;

      default_client = (AgsJackClient *) g_value_get_object(value);

      g_rec_mutex_lock(jack_server_mutex);

      if(jack_server->default_client == (GObject *) default_client){
	g_rec_mutex_unlock(jack_server_mutex);

	return;
      }

      if(jack_server->default_client != NULL){
	g_object_unref(G_OBJECT(jack_server->default_client));
      }

      if(default_client != NULL){
	g_object_ref(G_OBJECT(default_client));
      }

      jack_server->default_client = (GObject *) default_client;

      g_rec_mutex_unlock(jack_server_mutex);
    }
    break;
  case PROP_INPUT_JACK_CLIENT:
    {
      AgsJackClient *input_client;

      input_client = (AgsJackClient *) g_value_get_object(value);

      g_rec_mutex_lock(jack_server_mutex);

      if(jack_server->input_client == (GObject *) input_client){
	g_rec_mutex_unlock(jack_server_mutex);

	return;
      }

      if(jack_server->input_client != NULL){
	g_object_unref(G_OBJECT(jack_server->input_client));
      }

      if(input_client != NULL){
	g_object_ref(G_OBJECT(input_client));
      }

      jack_server->input_client = (GObject *) input_client;

      g_rec_mutex_unlock(jack_server_mutex);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      GObject *client;

      client = (GObject *) g_value_get_pointer(value);

      g_rec_mutex_lock(jack_server_mutex);

      if(!AGS_IS_JACK_CLIENT(client) ||
	 g_list_find(jack_server->client, client) != NULL){
	g_rec_mutex_unlock(jack_server_mutex);

	return;
      }

      g_object_ref(G_OBJECT(client));
      jack_server->client = g_list_prepend(jack_server->client,
					   client);

      g_rec_mutex_unlock(jack_server_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_server_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackServer *jack_server;

  GRecMutex *jack_server_mutex;

  jack_server = AGS_JACK_SERVER(gobject);

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);
  
  switch(prop_id){
  case PROP_URL:
    {
      g_rec_mutex_lock(jack_server_mutex);

      g_value_set_string(value, jack_server->url);

      g_rec_mutex_unlock(jack_server_mutex);
    }
    break;
  case PROP_DEFAULT_SOUNDCARD:
    {
      g_rec_mutex_lock(jack_server_mutex);

      g_value_set_object(value, jack_server->default_soundcard);

      g_rec_mutex_unlock(jack_server_mutex);
    }
    break;
  case PROP_DEFAULT_JACK_CLIENT:
    {
      g_rec_mutex_lock(jack_server_mutex);

      g_value_set_object(value, jack_server->default_client);

      g_rec_mutex_unlock(jack_server_mutex);
    }
    break;
  case PROP_INPUT_JACK_CLIENT:
    {
      g_rec_mutex_lock(jack_server_mutex);

      g_value_set_object(value, jack_server->input_client);

      g_rec_mutex_unlock(jack_server_mutex);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      g_rec_mutex_lock(jack_server_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(jack_server->client,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(jack_server_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_server_dispose(GObject *gobject)
{
  AgsJackServer *jack_server;

  GList *list;
  
  jack_server = AGS_JACK_SERVER(gobject);

  /* default soundcard */
  if(jack_server->default_soundcard != NULL){
    g_object_unref(G_OBJECT(jack_server->default_soundcard));

    jack_server->default_soundcard = NULL;
  }
  
  /* default client */
  if(jack_server->default_client != NULL){
    g_object_unref(G_OBJECT(jack_server->default_client));

    jack_server->default_client = NULL;
  }

  /* input client */
  if(jack_server->input_client != NULL){
    g_object_unref(G_OBJECT(jack_server->input_client));

    jack_server->input_client = NULL;
  }
  
  /* client */
  if(jack_server->client != NULL){
    list = jack_server->client;

    while(list != NULL){
      g_object_run_dispose(G_OBJECT(list->data));

      list = list->next;
    }
    
    g_list_free_full(jack_server->client,
		     g_object_unref);

    jack_server->client = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_jack_server_parent_class)->dispose(gobject);
}

void
ags_jack_server_finalize(GObject *gobject)
{
  AgsJackServer *jack_server;

  jack_server = AGS_JACK_SERVER(gobject);

  /* url */
  g_free(jack_server->url);

  /* default soundcard */
  if(jack_server->default_soundcard != NULL){
    g_object_unref(G_OBJECT(jack_server->default_soundcard));
  }
  
  /* default client */
  if(jack_server->default_client != NULL){
    g_object_unref(G_OBJECT(jack_server->default_client));
  }
  
  /* input client */
  if(jack_server->input_client != NULL){
    g_object_unref(G_OBJECT(jack_server->input_client));
  }
  
  /* client */
  if(jack_server->client != NULL){
    g_list_free_full(jack_server->client,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_jack_server_parent_class)->finalize(gobject);
}

AgsUUID*
ags_jack_server_get_uuid(AgsConnectable *connectable)
{
  AgsJackServer *jack_server;
  
  AgsUUID *ptr;

  GRecMutex *jack_server_mutex;

  jack_server = AGS_JACK_SERVER(connectable);

  /* get jack server signal mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  /* get UUID */
  g_rec_mutex_lock(jack_server_mutex);

  ptr = jack_server->uuid;

  g_rec_mutex_unlock(jack_server_mutex);
  
  return(ptr);
}

gboolean
ags_jack_server_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_jack_server_is_ready(AgsConnectable *connectable)
{
  AgsJackServer *jack_server;
  
  gboolean is_ready;

  jack_server = AGS_JACK_SERVER(connectable);

  /* check is added */
  is_ready = ags_jack_server_test_flags(jack_server, AGS_JACK_SERVER_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_jack_server_add_to_registry(AgsConnectable *connectable)
{
  AgsJackServer *jack_server;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  jack_server = AGS_JACK_SERVER(connectable);

  ags_jack_server_set_flags(jack_server, AGS_JACK_SERVER_ADDED_TO_REGISTRY);
}

void
ags_jack_server_remove_from_registry(AgsConnectable *connectable)
{
  AgsJackServer *jack_server;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  jack_server = AGS_JACK_SERVER(connectable);

  ags_jack_server_unset_flags(jack_server, AGS_JACK_SERVER_ADDED_TO_REGISTRY);
}

xmlNode*
ags_jack_server_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_jack_server_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_jack_server_xml_parse(AgsConnectable *connectable,
			  xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_jack_server_is_connected(AgsConnectable *connectable)
{
  AgsJackServer *jack_server;
  
  gboolean is_connected;

  jack_server = AGS_JACK_SERVER(connectable);

  /* check is connected */
  is_connected = ags_jack_server_test_flags(jack_server, AGS_JACK_SERVER_CONNECTED);
  
  return(is_connected);
}

void
ags_jack_server_connect(AgsConnectable *connectable)
{
  AgsJackServer *jack_server;

  GList *list_start, *list;  

  GRecMutex *jack_server_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  jack_server = AGS_JACK_SERVER(connectable);

  ags_jack_server_set_flags(jack_server, AGS_JACK_SERVER_CONNECTED);

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  list =
    list_start = g_list_copy(jack_server->client);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_jack_server_disconnect(AgsConnectable *connectable)
{
  AgsJackServer *jack_server;

  GList *list_start, *list;

  GRecMutex *jack_server_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  jack_server = AGS_JACK_SERVER(connectable);
  
  ags_jack_server_unset_flags(jack_server, AGS_JACK_SERVER_CONNECTED);

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  /* client */
  list =
    list_start = g_list_copy(jack_server->client);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_jack_server_test_flags:
 * @jack_server: the #AgsJackServer
 * @flags: the flags
 *
 * Test @flags to be set on @jack_server.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_jack_server_test_flags(AgsJackServer *jack_server, guint flags)
{
  gboolean retval;  
  
  GRecMutex *jack_server_mutex;

  if(!AGS_IS_JACK_SERVER(jack_server)){
    return(FALSE);
  }

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  /* test */
  g_rec_mutex_lock(jack_server_mutex);

  retval = (flags & (jack_server->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(jack_server_mutex);

  return(retval);
}

/**
 * ags_jack_server_set_flags:
 * @jack_server: the #AgsJackServer
 * @flags: see #AgsJackServerFlags-enum
 *
 * Enable a feature of @jack_server.
 *
 * Since: 3.0.0
 */
void
ags_jack_server_set_flags(AgsJackServer *jack_server, guint flags)
{
  GRecMutex *jack_server_mutex;

  if(!AGS_IS_JACK_SERVER(jack_server)){
    return;
  }

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(jack_server_mutex);

  jack_server->flags |= flags;
  
  g_rec_mutex_unlock(jack_server_mutex);
}
    
/**
 * ags_jack_server_unset_flags:
 * @jack_server: the #AgsJackServer
 * @flags: see #AgsJackServerFlags-enum
 *
 * Disable a feature of @jack_server.
 *
 * Since: 3.0.0
 */
void
ags_jack_server_unset_flags(AgsJackServer *jack_server, guint flags)
{  
  GRecMutex *jack_server_mutex;

  if(!AGS_IS_JACK_SERVER(jack_server)){
    return;
  }

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(jack_server_mutex);

  jack_server->flags &= (~flags);
  
  g_rec_mutex_unlock(jack_server_mutex);
}

void
ags_jack_server_set_url(AgsSoundServer *sound_server,
			gchar *url)
{
  AgsJackServer *jack_server;

  GRecMutex *jack_server_mutex;

  jack_server = AGS_JACK_SERVER(sound_server);

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  /* set URL */
  g_rec_mutex_lock(jack_server_mutex);

  jack_server->url = g_strdup(url);

  g_rec_mutex_unlock(jack_server_mutex);
}

gchar*
ags_jack_server_get_url(AgsSoundServer *sound_server)
{
  AgsJackServer *jack_server;

  gchar *url;

  GRecMutex *jack_server_mutex;

  jack_server = AGS_JACK_SERVER(sound_server);

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  /* set URL */
  g_rec_mutex_lock(jack_server_mutex);

  url = jack_server->url;

  g_rec_mutex_unlock(jack_server_mutex);
  
  return(url);
}

void
ags_jack_server_set_ports(AgsSoundServer *sound_server,
			  guint *port, guint port_count)
{
  AgsJackServer *jack_server;

  GRecMutex *jack_server_mutex;

  jack_server = AGS_JACK_SERVER(sound_server);

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  /* set ports */
  g_rec_mutex_lock(jack_server_mutex);

  jack_server->port = port;
  jack_server->port_count = port_count;

  g_rec_mutex_unlock(jack_server_mutex);
}

guint*
ags_jack_server_get_ports(AgsSoundServer *sound_server,
			  guint *port_count)
{
  AgsJackServer *jack_server;

  guint *port;
  
  GRecMutex *jack_server_mutex;

  jack_server = AGS_JACK_SERVER(sound_server);

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  /* get ports */
  g_rec_mutex_lock(jack_server_mutex);

  if(port_count != NULL){
    *port_count = AGS_JACK_SERVER(sound_server)->port_count;
  }

  port = jack_server->port;

  g_rec_mutex_unlock(jack_server_mutex);
  
  return(port);
}

void
ags_jack_server_set_soundcard(AgsSoundServer *sound_server,
			      gchar *client_uuid,
			      GList *soundcard)
{
  AgsJackServer *jack_server;
  AgsJackClient *jack_client;

  GList *list;

  jack_server = AGS_JACK_SERVER(sound_server);
  
  jack_client = (AgsJackClient *) ags_jack_server_find_client(jack_server,
							      client_uuid);

  if(!AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }
  
  //NOTE:JK: soundcard won't removed
  list = soundcard;

  while(list != NULL){
    ags_jack_client_add_device(jack_client,
			       (GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_jack_server_get_soundcard(AgsSoundServer *sound_server,
			      gchar *client_uuid)
{
  AgsJackServer *jack_server;
  AgsJackClient *jack_client;

  GList *device_start, *device;
  GList *list;
  
  jack_server = AGS_JACK_SERVER(sound_server);

  jack_client = (AgsJackClient *) ags_jack_server_find_client(jack_server,
							      client_uuid);

  if(!AGS_IS_JACK_CLIENT(jack_client)){
    return(NULL);
  }

  g_object_get(jack_client,
	       "device", &device_start,
	       NULL);
  
  device = device_start;
  list = NULL;

  while(device != NULL){
    if(AGS_IS_JACK_DEVOUT(device->data) ||
       AGS_IS_JACK_DEVIN(device->data)){
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
ags_jack_server_set_sequencer(AgsSoundServer *sound_server,
			      gchar *client_uuid,
			      GList *sequencer)
{
  AgsJackServer *jack_server;
  AgsJackClient *jack_client;

  GList *list;

  jack_server = AGS_JACK_SERVER(sound_server);

  jack_client = (AgsJackClient *) ags_jack_server_find_client(jack_server,
							      client_uuid);

  if(!AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }

  //NOTE:JK: sequencer won't removed
  list = sequencer;

  while(list != NULL){
    ags_jack_client_add_device(jack_client,
			       (GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_jack_server_get_sequencer(AgsSoundServer *sound_server,
			      gchar *client_uuid)
{
  AgsJackServer *jack_server;
  AgsJackClient *jack_client;

  GList *device_start, *device;
  GList *list;
  
  jack_server = AGS_JACK_SERVER(sound_server);

  jack_client = (AgsJackClient *) ags_jack_server_find_client(jack_server,
							      client_uuid);

  if(!AGS_IS_JACK_CLIENT(jack_client)){
    return(NULL);
  }

  g_object_get(jack_client,
	       "device", &device_start,
	       NULL);
  
  device = device_start;
  list = NULL;

  while(device != NULL){
    if(AGS_IS_JACK_MIDIIN(device->data)){
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

GObject*
ags_jack_server_register_soundcard(AgsSoundServer *sound_server,
				   gboolean is_output)
{
  AgsJackServer *jack_server;
  AgsJackClient *jack_client;
  AgsJackClient *default_client;
  AgsJackClient *input_client;
  AgsJackPort *jack_port;
  AgsJackDevout *jack_devout;
  AgsJackDevin *jack_devin;

  AgsApplicationContext *application_context;
  
  GObject *soundcard;

#ifdef AGS_WITH_JACK
  jack_client_t *client;
#else
  gpointer client;
#endif
  
  gchar *str;  

  guint n_soundcards;
  gboolean initial_set;
  int rc;
  guint i;
  
  GRecMutex *jack_server_mutex;
  GRecMutex *jack_client_mutex;

  jack_server = AGS_JACK_SERVER(sound_server);

  application_context = ags_application_context_get_instance();

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  /* the default client */
  initial_set = FALSE;
  
  /* get some fields */
  g_rec_mutex_lock(jack_server_mutex);

  jack_client = 
    default_client = (AgsJackClient *) jack_server->default_client;
  input_client = (AgsJackClient *) jack_server->input_client;

  n_soundcards = jack_server->n_soundcards;
  
  g_rec_mutex_unlock(jack_server_mutex);

  if(!is_output &&
     input_client != NULL){
    jack_client = input_client;
  }
  
  if(jack_client == NULL){
    jack_client = ags_jack_client_new((GObject *) jack_server);
    g_object_set(jack_server,
		 "default-jack-client", jack_client,
		 NULL);
    ags_jack_server_add_client(jack_server,
			       (GObject *) jack_client);
    
    ags_jack_client_open((AgsJackClient *) jack_client,
			 "ags-default-client");
    initial_set = TRUE;
  }

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* get client */
  g_rec_mutex_lock(jack_client_mutex);

  client = jack_client->client;

  g_rec_mutex_unlock(jack_client_mutex);
  
  if(client == NULL){
    g_warning("ags_jack_server.c - can't open JACK client");
  }

  /* the soundcard */
  soundcard = NULL;
  
  if(is_output){
    jack_devout = ags_jack_devout_new();
    soundcard = (GObject *) jack_devout;

    str = g_strdup_printf("ags-jack-devout-%d",
			  n_soundcards);
    
    g_object_set(AGS_JACK_DEVOUT(jack_devout),
		 "jack-client", jack_client,
		 "device", str,
		 NULL);
    g_free(str);
    
#ifdef AGS_WITH_JACK
    if(initial_set &&
       client != NULL){
      rc = jack_set_buffer_size(client,
				jack_devout->buffer_size);

      if(rc != 0){
	g_message("%s", strerror(rc));
      }
    }
#endif
    
    /* register ports */
    for(i = 0; i < jack_devout->pcm_channels; i++){
      str = g_strdup_printf("ags%d-%04d",
			    n_soundcards,
			    i);
      
#ifdef AGS_DEBUG
      g_message("%s %x", str, jack_client);
#endif
      
      jack_port = ags_jack_port_new((GObject *) jack_client);
      ags_jack_client_add_port(jack_client,
			       (GObject *) jack_port);

      g_object_set(jack_devout,
		   "jack-port", jack_port,
		   NULL);
      
      if(jack_devout->port_name == NULL){
	jack_devout->port_name = (gchar **) malloc(2 * sizeof(gchar *));
	jack_devout->port_name[0] = g_strdup(str);
      }else{
	jack_devout->port_name = (gchar **) realloc(jack_devout->port_name,
						    (i + 2) * sizeof(gchar *));
	jack_devout->port_name[i] = g_strdup(str);
      }
    
      ags_jack_port_register(jack_port,
			     str,
			     TRUE, FALSE,
			     TRUE);

      g_free(str);
    }

    if(jack_devout->port_name != NULL){
      jack_devout->port_name[jack_devout->pcm_channels] = NULL;
    }

    ags_jack_devout_realloc_buffer(jack_devout);

    g_object_set(jack_client,
		 "device", jack_devout,
		 NULL);
    
    /* increment n-soundcards */
    g_rec_mutex_lock(jack_server_mutex);
    
    jack_server->n_soundcards += 1;

    g_rec_mutex_unlock(jack_server_mutex);
  }else{
    jack_devin = ags_jack_devin_new();
    soundcard = (GObject *) jack_devin;

    str = g_strdup_printf("ags-jack-devin-%d",
			  n_soundcards);

    g_object_set(AGS_JACK_DEVIN(jack_devin),
		 "jack-client", jack_client,
		 "device", str,
		 NULL);
    g_free(str);

#ifdef AGS_WITH_JACK
    if(initial_set &&
       client != NULL){
      rc = jack_set_buffer_size(client,
				jack_devin->buffer_size);

      if(rc != 0){
	g_message("%s", strerror(rc));
      }
    }
#endif
    
    /* register ports */
    for(i = 0; i < jack_devin->pcm_channels; i++){
      str = g_strdup_printf("ags%d-%04d",
			    n_soundcards,
			    i);
      
#ifdef AGS_DEBUG
      g_message("%s %x", str, jack_client);
#endif
      
      jack_port = ags_jack_port_new((GObject *) jack_client);
      ags_jack_client_add_port(jack_client,
			       (GObject *) jack_port);

      g_object_set(jack_devin,
		   "jack-port", jack_port,
		   NULL);
      
      if(jack_devin->port_name == NULL){
	jack_devin->port_name = (gchar **) malloc(2 * sizeof(gchar *));
	jack_devin->port_name[0] = g_strdup(str);
      }else{
	jack_devin->port_name = (gchar **) realloc(jack_devin->port_name,
						   (i + 2) * sizeof(gchar *));
	jack_devin->port_name[i] = g_strdup(str);
      }
    
      ags_jack_port_register(jack_port,
			     str,
			     TRUE, FALSE,
			     FALSE);

      g_free(str);
    }

    if(jack_devin->port_name != NULL){
      jack_devin->port_name[jack_devin->pcm_channels] = NULL;
    }

    ags_jack_devin_realloc_buffer(jack_devin);

    g_object_set(jack_client,
		 "device", jack_devin,
		 NULL);
    
    /* increment n-soundcards */
    g_rec_mutex_lock(jack_server_mutex);

    jack_server->n_soundcards += 1;

    g_rec_mutex_unlock(jack_server_mutex);
  }
  
  return(soundcard);
}

void
ags_jack_server_unregister_soundcard(AgsSoundServer *sound_server,
				     GObject *soundcard)
{
  AgsJackServer *jack_server;
  AgsJackClient *jack_client;
  AgsJackClient *default_client;
  AgsJackClient *input_client;

  GList *list_start, *list;
  GList *port;
  
  GRecMutex *jack_server_mutex;

  jack_server = AGS_JACK_SERVER(sound_server);

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);
  
  /* get some fields */
  g_rec_mutex_lock(jack_server_mutex);

  jack_client = 
    default_client = (AgsJackClient *) jack_server->default_client;
  input_client = (AgsJackClient *) jack_server->input_client;
  
  g_rec_mutex_unlock(jack_server_mutex);
  
  if(AGS_IS_JACK_DEVIN(soundcard) &&
     input_client != NULL){
    jack_client = input_client;
  }

  if(jack_client == NULL){
    g_warning("GSequencer - no jack client");
    
    return;
  }

  if(AGS_IS_JACK_DEVOUT(soundcard)){
    g_object_get(soundcard,
		 "jack-port", &list_start,
		 NULL);

    list = list_start;

    while(list != NULL){
      ags_jack_port_unregister(list->data);
      ags_jack_client_remove_port(jack_client,
				  list->data);
    
      list = list->next;
    }

    g_list_free_full(list_start,
		     g_object_unref);
  }else if(AGS_IS_JACK_DEVIN(soundcard)){
    g_object_get(soundcard,
		 "jack-port", &list_start,
		 NULL);

    list = list_start;

    while(list != NULL){
      ags_jack_port_unregister(list->data);
      ags_jack_client_remove_port(jack_client,
				  list->data);
    
      list = list->next;
    }

    g_list_free_full(list_start,
		     g_object_unref);
  }
  
  ags_jack_client_remove_device(jack_client,
				soundcard);

  g_object_get(default_client,
	       "port", &port,
	       NULL);
  
  if(port == NULL){
    g_object_get(input_client,
		 "port", &port,
		 NULL);
    
    if(port == NULL){    
      /* reset n-soundcards */
      g_rec_mutex_lock(jack_server_mutex);

      jack_server->n_soundcards = 0;

      g_rec_mutex_unlock(jack_server_mutex);
    }else{
      g_list_free_full(port,
		       g_object_unref);
    }
  }else{
    g_list_free_full(port,
		     g_object_unref);
  }
}

GObject*
ags_jack_server_register_sequencer(AgsSoundServer *sound_server,
				   gboolean is_output)
{
  AgsJackServer *jack_server;
  AgsJackClient *jack_client;
  AgsJackClient *default_client;
  AgsJackClient *input_client;
  AgsJackPort *jack_port;
  AgsJackMidiin *jack_midiin;

  AgsApplicationContext *application_context;

#ifdef AGS_WITH_JACK
  jack_client_t *client;
#else
  gpointer client;
#endif

  gchar *str;
  
  guint n_sequencers;

  GRecMutex *jack_server_mutex;
  GRecMutex *jack_client_mutex;

  if(is_output){
    g_warning("GSequencer - MIDI output not implemented");
    return(NULL);
  }
  
  jack_server = AGS_JACK_SERVER(sound_server);

  application_context = ags_application_context_get_instance();

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  /* get some fields */
  g_rec_mutex_lock(jack_server_mutex);

  jack_client = 
    default_client = (AgsJackClient *) jack_server->default_client;
  input_client = (AgsJackClient *) jack_server->input_client;

  n_sequencers = jack_server->n_sequencers;
  
  g_rec_mutex_unlock(jack_server_mutex);
  
  if(!is_output &&
     input_client != NULL){
    jack_client = input_client;
  }
  
  /* the jack client */
  if(jack_client == NULL){
    jack_client = ags_jack_client_new((GObject *) jack_server);
    g_object_set(jack_server,
		 "default-jack-client", jack_client,
		 NULL);
    ags_jack_server_add_client(jack_server,
			       (GObject *) jack_client);
    
    ags_jack_client_open((AgsJackClient *) jack_client,
			 "ags-default-client");
  }

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* get client */
  g_rec_mutex_lock(jack_client_mutex);

  client = jack_client->client;

  g_rec_mutex_unlock(jack_client_mutex);

  if(client == NULL){
    g_warning("ags_jack_server.c - can't open JACK client");
  }

  
  jack_midiin = ags_jack_midiin_new(application_context);
  
  str = g_strdup_printf("ags-jack-midiin-%d",
			n_sequencers);

  g_object_set(AGS_JACK_MIDIIN(jack_midiin),
	       "jack-client", jack_client,
	       "device", str,
	       NULL);

  g_free(str);
  
  /* register sequencer */
  jack_port = ags_jack_port_new((GObject *) jack_client);
  ags_jack_client_add_port(jack_client,
			   (GObject *) jack_port);

  g_object_set(jack_midiin,
	       "jack-port", jack_port,
	       NULL);
  
  str = g_strdup_printf("ags-sequencer%d",
			jack_server->n_sequencers);

#ifdef AGS_DEBUG
  g_message("%s", str);
#endif
  
  ags_jack_port_register(jack_port,
			 str,
			 FALSE, TRUE,
			 FALSE);
    
  g_object_set(jack_client,
	       "device", jack_midiin,
	       NULL);

  /* increment n-sequencers */
  g_rec_mutex_lock(jack_server_mutex);
  
  jack_server->n_sequencers += 1;

  g_rec_mutex_unlock(jack_server_mutex);
  
  return((GObject *) jack_midiin);
}

void
ags_jack_server_unregister_sequencer(AgsSoundServer *sound_server,
				     GObject *sequencer)
{
  AgsJackServer *jack_server;
  AgsJackClient *jack_client;
  AgsJackClient *input_client;
  AgsJackClient *default_client;

  GList *list_start, *list;
  GList *port;

  GRecMutex *jack_server_mutex;
  
  jack_server = AGS_JACK_SERVER(sound_server);

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);
  
  /* get some fields */
  g_rec_mutex_lock(jack_server_mutex);

  jack_client = 
    default_client = (AgsJackClient *) jack_server->default_client;
  input_client = (AgsJackClient *) jack_server->input_client;
  
  g_rec_mutex_unlock(jack_server_mutex);
  
  if(AGS_IS_JACK_MIDIIN(sequencer) &&
     input_client != NULL){
    jack_client = input_client;
  }

  if(jack_client == NULL){
    g_warning("GSequencer - no jack client");
    
    return;
  }

  g_object_get(sequencer,
	       "jack-port", &list_start,
	       NULL);

  list = list_start;

  while(list != NULL){
    ags_jack_port_unregister(list->data);
    ags_jack_client_remove_port(jack_client,
				list->data);
    

    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);

  ags_jack_client_remove_device(jack_client,
				sequencer);

  g_object_get(default_client,
	       "port", &port,
	       NULL);
  
  if(port == NULL){
    g_object_get(input_client,
		 "port", &port,
		 NULL);
    
    if(port == NULL){    
      /* reset n-sequencers */
      g_rec_mutex_lock(jack_server_mutex);

      jack_server->n_sequencers = 0;

      g_rec_mutex_unlock(jack_server_mutex);
    }else{
      g_list_free_full(port,
		       g_object_unref);
    }
  }else{
    g_list_free_full(port,
		     g_object_unref);
  }
}

/**
 * ags_jack_server_register_default_soundcard:
 * @jack_server: the #AgsJackServer
 * 
 * Register default soundcard.
 * 
 * Returns: the instantiated #AgsJackDevout
 * 
 * Since: 3.0.0
 */
GObject*
ags_jack_server_register_default_soundcard(AgsJackServer *jack_server)
{
  AgsJackClient *default_client;
  AgsJackDevout *jack_devout;
  AgsJackPort *jack_port;

  AgsApplicationContext *application_context;

#ifdef AGS_WITH_JACK
  jack_client_t *client;
#else
  gpointer client;
#endif
  
  gchar *str;

  guint n_soundcards;
  guint i;
  int rc;

  GRecMutex *jack_server_mutex;
  GRecMutex *jack_client_mutex;

  if(!AGS_IS_JACK_SERVER(jack_server)){
    return(NULL);
  }

  application_context = ags_application_context_get_instance();

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  /* get some fields */
  g_rec_mutex_lock(jack_server_mutex);

  default_client = (AgsJackClient *) jack_server->default_client;

  n_soundcards = jack_server->n_soundcards;
  
  g_rec_mutex_unlock(jack_server_mutex);
  
  /* the default client */
  if(default_client == NULL){
    default_client = ags_jack_client_new((GObject *) jack_server);
    g_object_set(jack_server,
		 "default-jack-client", default_client,
		 NULL);
    ags_jack_server_add_client(jack_server,
			       (GObject *) default_client);
    
    ags_jack_client_open((AgsJackClient *) default_client,
			 "ags-default-client");
  }

  /* get jack client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(default_client);

  /* get client */
  g_rec_mutex_lock(jack_client_mutex);

  client = default_client->client;

  g_rec_mutex_unlock(jack_client_mutex);
  
  if(client == NULL){
    g_warning("ags_jack_server.c - can't open JACK client");
  }

  /* the soundcard */
  jack_devout = ags_jack_devout_new();
  g_object_set(AGS_JACK_DEVOUT(jack_devout),
	       "jack-client", default_client,
	       "device", "ags-default-devout",
	       NULL);

#ifdef AGS_WITH_JACK
  if(default_client->client != NULL){
    rc = jack_set_buffer_size(client,
			      jack_devout->buffer_size);
    
    if(rc != 0){
      g_message("%s", strerror(rc));
    }
  }
#endif

  /* register ports */
  for(i = 0; i < jack_devout->pcm_channels; i++){
    jack_port = ags_jack_port_new((GObject *) default_client);
    ags_jack_client_add_port(default_client,
			     (GObject *) jack_port);

    g_object_set(jack_devout,
		 "jack-port", jack_port,
		 NULL);

    str = g_strdup_printf("ags-default-soundcard-%04d",
			  i);

#ifdef AGS_DEBUG
    g_message("%s", str);
#endif
    
    if(jack_devout->port_name == NULL){
      jack_devout->port_name = (gchar **) malloc(2 * sizeof(gchar *));
      jack_devout->port_name[0] = g_strdup(str);
    }else{
      jack_devout->port_name = (gchar **) realloc(jack_devout->port_name,
						  (i + 2) * sizeof(gchar *));
      jack_devout->port_name[i] = g_strdup(str);
    }
    
    ags_jack_port_register(jack_port,
			   str,
			   TRUE, FALSE,
			   TRUE);
  }

  if(jack_devout->port_name != NULL){
    jack_devout->port_name[jack_devout->pcm_channels] = NULL;
  }

  g_object_set(default_client,
	       "device", jack_devout,
	       NULL);
  
  return((GObject *) jack_devout);
}

/**
 * ags_jack_server_find_url:
 * @jack_server: (element-type AgsAudio.JackServer) (transfer none): the #GList-struct containing #AgsJackServer
 * @url: the url to find
 *
 * Find #AgsJackServer by url.
 *
 * Returns: (element-type AgsAudio.JackServer) (transfer none): the #GList-struct containing a #AgsJackServer matching @url or %NULL
 *
 * Since: 3.0.0
 */
GList*
ags_jack_server_find_url(GList *jack_server,
			 gchar *url)
{
  GList *retval;
  
  GRecMutex *jack_server_mutex;

  retval = NULL;
  
  while(jack_server != NULL){
    /* get jack server mutex */
    jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server->data);

    /* check URL */
    g_rec_mutex_lock(jack_server_mutex);
    
    if(!g_ascii_strcasecmp(AGS_JACK_SERVER(jack_server->data)->url,
			   url)){
      retval = jack_server;

      g_rec_mutex_unlock(jack_server_mutex);
    
      break;
    }

    g_rec_mutex_unlock(jack_server_mutex);
    
    jack_server = jack_server->next;
  }

  return(retval);
}

/**
 * ags_jack_server_find_client:
 * @jack_server: the #AgsJackServer
 * @client_uuid: the uuid to find
 *
 * Find #AgsJackClient by uuid.
 *
 * Returns: (transfer none): the #AgsJackClient found or %NULL
 *
 * Since: 3.0.0
 */
GObject*
ags_jack_server_find_client(AgsJackServer *jack_server,
			    gchar *client_uuid)
{
  AgsJackClient *retval;
  
  GList *list_start, *list;

  GRecMutex *jack_server_mutex;
  GRecMutex *jack_client_mutex;

  if(!AGS_IS_JACK_SERVER(jack_server)){
    return(NULL);
  }

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  /* get some fields */
  g_rec_mutex_lock(jack_server_mutex);

  list =
    list_start = g_list_copy(jack_server->client);

  g_rec_mutex_unlock(jack_server_mutex);

  retval = NULL;
  
  while(list != NULL){
    /* get jack client mutex */
    jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(list->data);

    /* check client UUID */
    g_rec_mutex_lock(jack_client_mutex);
    
    if(!g_ascii_strcasecmp(AGS_JACK_CLIENT(list->data)->client_uuid,
			   client_uuid)){
      retval = list->data;

      g_rec_mutex_unlock(jack_client_mutex);

      break;
    }

    g_rec_mutex_unlock(jack_client_mutex);
    
    list = list->next;
  }

  g_list_free(list_start);
  
  return((GObject *) retval);
}

/**
 * ags_jack_server_find_port:
 * @jack_server: the #AgsJackServer
 * @port_uuid: the uuid to find
 *
 * Find #AgsJackPort by uuid.
 *
 * Returns: (transfer none): the #AgsJackPort found or %NULL
 *
 * Since: 3.0.0
 */
GObject*
ags_jack_server_find_port(AgsJackServer *jack_server,
			  gchar *port_uuid)
{
  GList *client_start, *client;
  GList *port_start, *port;

  gboolean success;
  
  GRecMutex *jack_port_mutex;

  g_object_get(jack_server,
	       "jack-client", &client_start,
	       NULL);

  client = client_start;
  
  while(client != NULL){
    g_object_get(jack_server,
		 "jack-port", &port_start,
		 NULL);

    port = port_start;
    
    while(port != NULL){
      /* get jack port mutex */
      jack_port_mutex = AGS_JACK_PORT_GET_OBJ_MUTEX(port->data);
      
      /* check port UUID */
      g_rec_mutex_lock(jack_port_mutex);
      
      success = (!g_ascii_strcasecmp(AGS_JACK_PORT(port->data)->port_uuid,
				     port_uuid)) ? TRUE: FALSE;

      g_rec_mutex_unlock(jack_port_mutex);
      
      if(success){
	AgsJackPort *retval;

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
 * ags_jack_server_add_client:
 * @jack_server: the #AgsJackServer
 * @jack_client: the #AgsJackClient to add
 *
 * Add @jack_client to @jack_server
 *
 * Since: 3.0.0
 */
void
ags_jack_server_add_client(AgsJackServer *jack_server,
			   GObject *jack_client)
{
  GRecMutex *jack_server_mutex;

  if(!AGS_IS_JACK_SERVER(jack_server) ||
     !AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  /* get some fields */
  g_rec_mutex_lock(jack_server_mutex);

  if(g_list_find(jack_server->client, jack_client) == NULL){
    g_object_ref(jack_client);
    jack_server->client = g_list_prepend(jack_server->client,
					 jack_client);
  }

  g_rec_mutex_unlock(jack_server_mutex);
}

/**
 * ags_jack_server_remove_client:
 * @jack_server: the #AgsJackServer
 * @jack_client: the #AgsJackClient to remove
 *
 * Remove @jack_client to @jack_server
 *
 * Since: 3.0.0
 */
void
ags_jack_server_remove_client(AgsJackServer *jack_server,
			      GObject *jack_client)
{
  GRecMutex *jack_server_mutex;

  if(!AGS_IS_JACK_SERVER(jack_server) ||
     !AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }

  /* get jack server mutex */
  jack_server_mutex = AGS_JACK_SERVER_GET_OBJ_MUTEX(jack_server);

  /* get some fields */
  g_rec_mutex_lock(jack_server_mutex);

  if(g_list_find(jack_server->client, jack_client) != NULL){
    jack_server->client = g_list_remove(jack_server->client,
					jack_client);
    g_object_unref(jack_client);
  }

  g_rec_mutex_unlock(jack_server_mutex);
}

/**
 * ags_jack_server_connect_client:
 * @jack_server: the #AgsJackServer
 *
 * Connect all clients.
 *
 * Since: 3.0.0
 */
void
ags_jack_server_connect_client(AgsJackServer *jack_server)
{
  GList *client_start, *client;

  gchar *client_name;

  GRecMutex *jack_client_mutex;
  
  if(!AGS_IS_JACK_SERVER(jack_server)){
    return;
  }

  g_object_get(jack_server,
	       "jack-client", &client_start,
	       NULL);
  
  client = client_start;

  while(client != NULL){
    /* client name */
    g_object_get(client->data,
		 "client-name", &client_name,
		 NULL);
    
    /* open */
    ags_jack_client_open((AgsJackClient *) client->data,
    			 client_name);
    ags_jack_client_activate(client->data);

    g_free(client_name);
    
    /* iterate */
    client = client->next;
  }

  g_list_free_full(client_start,
		   g_object_unref);
}

/**
 * ags_jack_server_disconnect_client:
 * @jack_server: the #AgsJackServer
 *
 * Connect all clients.
 *
 * Since: 2.1.14
 */
void
ags_jack_server_disconnect_client(AgsJackServer *jack_server)
{
  GList *client_start, *client;

  if(!AGS_IS_JACK_SERVER(jack_server)){
    return;
  }

  g_object_get(jack_server,
	       "jack-client", &client_start,
	       NULL);
  
  client = client_start;

  while(client != NULL){
    /* close */
    ags_jack_client_deactivate(client->data);
    
    ags_jack_client_close((AgsJackClient *) client->data);
    
    /* iterate */
    client = client->next;
  }

  g_list_free_full(client_start,
		   g_object_unref);
}

/**
 * ags_jack_server_new:
 * @url: the URL as string
 *
 * Create a new instance of #AgsJackServer.
 *
 * Returns: the new #AgsJackServer
 *
 * Since: 3.0.0
 */
AgsJackServer*
ags_jack_server_new(gchar *url)
{
  AgsJackServer *jack_server;

  jack_server = (AgsJackServer *) g_object_new(AGS_TYPE_JACK_SERVER,
					       "url", url,
					       NULL);

  return(jack_server);
}
