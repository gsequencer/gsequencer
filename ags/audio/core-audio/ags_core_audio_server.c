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

#include <ags/audio/core-audio/ags_core_audio_server.h>
#include <ags/audio/core-audio/ags_core_audio_client.h>
#include <ags/audio/core-audio/ags_core_audio_port.h>

#include <ags/audio/core-audio/ags_core_audio_devout.h>
#include <ags/audio/core-audio/ags_core_audio_devin.h>
#include <ags/audio/core-audio/ags_core_audio_midiin.h>

#include <string.h>

#include <errno.h>

#include <ags/i18n.h>

#define AGS_CORE_AUDIO_PORT_USE_HW (1)

void ags_core_audio_server_class_init(AgsCoreAudioServerClass *core_audio_server);
void ags_core_audio_server_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_core_audio_server_sound_server_interface_init(AgsSoundServerInterface *sound_server);
void ags_core_audio_server_init(AgsCoreAudioServer *core_audio_server);
void ags_core_audio_server_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_server_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_server_dispose(GObject *gobject);
void ags_core_audio_server_finalize(GObject *gobject);

AgsUUID* ags_core_audio_server_get_uuid(AgsConnectable *connectable);
gboolean ags_core_audio_server_has_resource(AgsConnectable *connectable);
gboolean ags_core_audio_server_is_ready(AgsConnectable *connectable);
void ags_core_audio_server_add_to_registry(AgsConnectable *connectable);
void ags_core_audio_server_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_core_audio_server_list_resource(AgsConnectable *connectable);
xmlNode* ags_core_audio_server_xml_compose(AgsConnectable *connectable);
void ags_core_audio_server_xml_parse(AgsConnectable *connectable,
				     xmlNode *node);
gboolean ags_core_audio_server_is_connected(AgsConnectable *connectable);
void ags_core_audio_server_connect(AgsConnectable *connectable);
void ags_core_audio_server_disconnect(AgsConnectable *connectable);

void ags_core_audio_server_set_url(AgsSoundServer *sound_server,
				   gchar *url);
gchar* ags_core_audio_server_get_url(AgsSoundServer *sound_server);
void ags_core_audio_server_set_ports(AgsSoundServer *sound_server,
				     guint *ports, guint port_count);
guint* ags_core_audio_server_get_ports(AgsSoundServer *sound_server,
				       guint *port_count);
void ags_core_audio_server_set_soundcard(AgsSoundServer *sound_server,
					 gchar *client_uuid,
					 GList *soundcard);
GList* ags_core_audio_server_get_soundcard(AgsSoundServer *sound_server,
					   gchar *client_uuid);
void ags_core_audio_server_set_sequencer(AgsSoundServer *sound_server,
					 gchar *client_uuid,
					 GList *sequencer);
GList* ags_core_audio_server_get_sequencer(AgsSoundServer *sound_server,
					   gchar *client_uuid);
GObject* ags_core_audio_server_register_soundcard(AgsSoundServer *sound_server,
						  gboolean is_output);
void ags_core_audio_server_unregister_soundcard(AgsSoundServer *sound_server,
						GObject *soundcard);
GObject* ags_core_audio_server_register_sequencer(AgsSoundServer *sound_server,
						  gboolean is_output);
void ags_core_audio_server_unregister_sequencer(AgsSoundServer *sound_server,
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
  PROP_URL,
  PROP_DEFAULT_SOUNDCARD,
  PROP_DEFAULT_CORE_AUDIO_CLIENT,
  PROP_CORE_AUDIO_CLIENT,
};

static gpointer ags_core_audio_server_parent_class = NULL;

GType
ags_core_audio_server_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_core_audio_server = 0;

    static const GTypeInfo ags_core_audio_server_info = {
      sizeof(AgsCoreAudioServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_core_audio_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCoreAudioServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_core_audio_server_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_server_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_sound_server_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_server_sound_server_interface_init,
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
				AGS_TYPE_SOUND_SERVER,
				&ags_sound_server_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_core_audio_server);
  }

  return g_define_type_id__volatile;
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
   * AgsCoreAudioServer:url:
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
   * AgsCoreAudioServer:default-soundcard:
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
   * AgsCoreAudioServer:default-core-audio-client:
   *
   * The default core audio client.
   * 
   * Since: 3.0.0
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
   * AgsCoreAudioServer:core-audio-client: (type GList(AgsCoreAudioClient)) (transfer full)
   *
   * The core audio client list.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("core-audio-client",
				    i18n_pspec("core audio client list"),
				    i18n_pspec("The core audio client list"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CORE_AUDIO_CLIENT,
				  param_spec);
}

void
ags_core_audio_server_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_core_audio_server_get_uuid;
  connectable->has_resource = ags_core_audio_server_has_resource;

  connectable->is_ready = ags_core_audio_server_is_ready;
  connectable->add_to_registry = ags_core_audio_server_add_to_registry;
  connectable->remove_from_registry = ags_core_audio_server_remove_from_registry;

  connectable->list_resource = ags_core_audio_server_list_resource;
  connectable->xml_compose = ags_core_audio_server_xml_compose;
  connectable->xml_parse = ags_core_audio_server_xml_parse;

  connectable->is_connected = ags_core_audio_server_is_connected;  
  connectable->connect = ags_core_audio_server_connect;
  connectable->disconnect = ags_core_audio_server_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_core_audio_server_sound_server_interface_init(AgsSoundServerInterface *sound_server)
{
  sound_server->set_url = ags_core_audio_server_set_url;
  sound_server->get_url = ags_core_audio_server_get_url;
  sound_server->set_ports = ags_core_audio_server_set_ports;
  sound_server->get_ports = ags_core_audio_server_get_ports;
  sound_server->set_soundcard = ags_core_audio_server_set_soundcard;
  sound_server->get_soundcard = ags_core_audio_server_get_soundcard;
  sound_server->set_sequencer = ags_core_audio_server_set_sequencer;
  sound_server->get_sequencer = ags_core_audio_server_get_sequencer;
  sound_server->register_soundcard = ags_core_audio_server_register_soundcard;
  sound_server->unregister_soundcard = ags_core_audio_server_unregister_soundcard;
  sound_server->register_sequencer = ags_core_audio_server_register_sequencer;
  sound_server->unregister_sequencer = ags_core_audio_server_unregister_sequencer;
}

void
ags_core_audio_server_init(AgsCoreAudioServer *core_audio_server)
{
  /* flags */
  core_audio_server->flags = 0;
  core_audio_server->connectable_flags = 0;

  /* server mutex */
  g_rec_mutex_init(&(core_audio_server->obj_mutex));
  
  g_atomic_int_set(&(core_audio_server->running),
		   TRUE);
  core_audio_server->thread = NULL;

  /* uuid */
  core_audio_server->uuid = ags_uuid_alloc();
  ags_uuid_generate(core_audio_server->uuid);

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

  GRecMutex *core_audio_server_mutex;

  core_audio_server = AGS_CORE_AUDIO_SERVER(gobject);

  /* get core-audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  switch(prop_id){
  case PROP_URL:
    {
      gchar *url;

      url = g_value_get_string(value);

      g_rec_mutex_lock(core_audio_server_mutex);

      if(core_audio_server->url == url){
	g_rec_mutex_unlock(core_audio_server_mutex);

	return;
      }

      if(core_audio_server->url != NULL){
	g_free(core_audio_server->url);
      }

      core_audio_server->url = g_strdup(url);

      g_rec_mutex_unlock(core_audio_server_mutex);
    }
    break;
  case PROP_DEFAULT_SOUNDCARD:
    {
      GObject *default_soundcard;

      default_soundcard = (GObject *) g_value_get_object(value);

      g_rec_mutex_lock(core_audio_server_mutex);

      if(core_audio_server->default_soundcard == (GObject *) default_soundcard){
	g_rec_mutex_unlock(core_audio_server_mutex);

	return;
      }

      if(core_audio_server->default_soundcard != NULL){
	g_object_unref(G_OBJECT(core_audio_server->default_soundcard));
      }

      if(default_soundcard != NULL){
	g_object_ref(G_OBJECT(default_soundcard));
      }

      core_audio_server->default_soundcard = (GObject *) default_soundcard;

      g_rec_mutex_unlock(core_audio_server_mutex);
    }
    break;
  case PROP_DEFAULT_CORE_AUDIO_CLIENT:
    {
      AgsCoreAudioClient *default_client;

      default_client = (AgsCoreAudioClient *) g_value_get_object(value);

      g_rec_mutex_lock(core_audio_server_mutex);

      if(core_audio_server->default_client == (GObject *) default_client){
	g_rec_mutex_unlock(core_audio_server_mutex);

	return;
      }

      if(core_audio_server->default_client != NULL){
	g_object_unref(G_OBJECT(core_audio_server->default_client));
      }

      if(default_client != NULL){
	g_object_ref(G_OBJECT(default_client));
      }

      core_audio_server->default_client = (GObject *) default_client;

      g_rec_mutex_unlock(core_audio_server_mutex);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      GObject *client;

      client = (GObject *) g_value_get_pointer(value);

      g_rec_mutex_lock(core_audio_server_mutex);

      if(!AGS_IS_CORE_AUDIO_CLIENT(client) ||
	 g_list_find(core_audio_server->client, client) != NULL){
	g_rec_mutex_unlock(core_audio_server_mutex);

	return;
      }

      g_object_ref(G_OBJECT(client));
      core_audio_server->client = g_list_prepend(core_audio_server->client,
						 client);

      g_rec_mutex_unlock(core_audio_server_mutex);
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

  GRecMutex *core_audio_server_mutex;

  core_audio_server = AGS_CORE_AUDIO_SERVER(gobject);

  /* get core-audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);
  
  switch(prop_id){
  case PROP_URL:
    {
      g_rec_mutex_lock(core_audio_server_mutex);

      g_value_set_string(value, core_audio_server->url);

      g_rec_mutex_unlock(core_audio_server_mutex);
    }
    break;
  case PROP_DEFAULT_SOUNDCARD:
    {
      g_rec_mutex_lock(core_audio_server_mutex);

      g_value_set_object(value, core_audio_server->default_soundcard);

      g_rec_mutex_unlock(core_audio_server_mutex);
    }
    break;
  case PROP_DEFAULT_CORE_AUDIO_CLIENT:
    {
      g_rec_mutex_lock(core_audio_server_mutex);

      g_value_set_object(value, core_audio_server->default_client);

      g_rec_mutex_unlock(core_audio_server_mutex);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      g_rec_mutex_lock(core_audio_server_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(core_audio_server->client,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(core_audio_server_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_server_dispose(GObject *gobject)
{
  AgsCoreAudioServer *core_audio_server;

  GList *list;
  
  core_audio_server = AGS_CORE_AUDIO_SERVER(gobject);

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

  core_audio_server = AGS_CORE_AUDIO_SERVER(gobject);

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
  
  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_server_parent_class)->finalize(gobject);
}

AgsUUID*
ags_core_audio_server_get_uuid(AgsConnectable *connectable)
{
  AgsCoreAudioServer *core_audio_server;
  
  AgsUUID *ptr;

  GRecMutex *core_audio_server_mutex;

  core_audio_server = AGS_CORE_AUDIO_SERVER(connectable);

  /* get core audio server signal mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  /* get UUID */
  g_rec_mutex_lock(core_audio_server_mutex);

  ptr = core_audio_server->uuid;

  g_rec_mutex_unlock(core_audio_server_mutex);
  
  return(ptr);
}

gboolean
ags_core_audio_server_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_core_audio_server_is_ready(AgsConnectable *connectable)
{
  AgsCoreAudioServer *core_audio_server;
  
  gboolean is_ready;

  GRecMutex *core_audio_server_mutex;

  core_audio_server = AGS_CORE_AUDIO_SERVER(connectable);

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  /* check is ready */
  g_rec_mutex_lock(core_audio_server_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (core_audio_server->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_server_mutex);
  
  return(is_ready);
}

void
ags_core_audio_server_add_to_registry(AgsConnectable *connectable)
{
  AgsCoreAudioServer *core_audio_server;

  GRecMutex *core_audio_server_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  core_audio_server = AGS_CORE_AUDIO_SERVER(connectable);

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  g_rec_mutex_lock(core_audio_server_mutex);

  core_audio_server->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(core_audio_server_mutex);
}

void
ags_core_audio_server_remove_from_registry(AgsConnectable *connectable)
{
  AgsCoreAudioServer *core_audio_server;

  GRecMutex *core_audio_server_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  core_audio_server = AGS_CORE_AUDIO_SERVER(connectable);

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  g_rec_mutex_lock(core_audio_server_mutex);

  core_audio_server->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(core_audio_server_mutex);
}

xmlNode*
ags_core_audio_server_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_core_audio_server_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_core_audio_server_xml_parse(AgsConnectable *connectable,
				xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_core_audio_server_is_connected(AgsConnectable *connectable)
{
  AgsCoreAudioServer *core_audio_server;
  
  gboolean is_connected;

  GRecMutex *core_audio_server_mutex;

  core_audio_server = AGS_CORE_AUDIO_SERVER(connectable);

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  /* check is connected */
  g_rec_mutex_lock(core_audio_server_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (core_audio_server->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_server_mutex);
  
  return(is_connected);
}

void
ags_core_audio_server_connect(AgsConnectable *connectable)
{
  AgsCoreAudioServer *core_audio_server;

  GList *list_start, *list;  

  GRecMutex *core_audio_server_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_server = AGS_CORE_AUDIO_SERVER(connectable);

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  g_rec_mutex_lock(core_audio_server_mutex);

  core_audio_server->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(core_audio_server_mutex);

  list =
    list_start = g_list_copy_deep(core_audio_server->client,
				  (GCopyFunc) g_object_ref,
				  NULL);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(list_start,
		   (GDestroyNotify) g_object_unref);
}

void
ags_core_audio_server_disconnect(AgsConnectable *connectable)
{
  AgsCoreAudioServer *core_audio_server;

  GList *list_start, *list;

  GRecMutex *core_audio_server_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_server = AGS_CORE_AUDIO_SERVER(connectable);
  
  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  g_rec_mutex_lock(core_audio_server_mutex);

  core_audio_server->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(core_audio_server_mutex);

  /* client */
  list =
    list_start = g_list_copy_deep(core_audio_server->client,
				  (GCopyFunc) g_object_ref,
				  NULL);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(list_start,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_core_audio_server_test_flags:
 * @core_audio_server: the #AgsCoreAudioServer
 * @flags: the flags
 *
 * Test @flags to be set on @core_audio_server.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_core_audio_server_test_flags(AgsCoreAudioServer *core_audio_server, guint flags)
{
  gboolean retval;  
  
  GRecMutex *core_audio_server_mutex;

  if(!AGS_IS_CORE_AUDIO_SERVER(core_audio_server)){
    return(FALSE);
  }

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  /* test */
  g_rec_mutex_lock(core_audio_server_mutex);

  retval = (flags & (core_audio_server->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(core_audio_server_mutex);

  return(retval);
}

/**
 * ags_core_audio_server_set_flags:
 * @core_audio_server: the #AgsCoreAudioServer
 * @flags: see #AgsCoreAudioServerFlags-enum
 *
 * Enable a feature of @core_audio_server.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_server_set_flags(AgsCoreAudioServer *core_audio_server, guint flags)
{
  GRecMutex *core_audio_server_mutex;

  if(!AGS_IS_CORE_AUDIO_SERVER(core_audio_server)){
    return;
  }

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(core_audio_server_mutex);

  core_audio_server->flags |= flags;
  
  g_rec_mutex_unlock(core_audio_server_mutex);
}
    
/**
 * ags_core_audio_server_unset_flags:
 * @core_audio_server: the #AgsCoreAudioServer
 * @flags: see #AgsCoreAudioServerFlags-enum
 *
 * Disable a feature of @core_audio_server.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_server_unset_flags(AgsCoreAudioServer *core_audio_server, guint flags)
{  
  GRecMutex *core_audio_server_mutex;

  if(!AGS_IS_CORE_AUDIO_SERVER(core_audio_server)){
    return;
  }

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(core_audio_server_mutex);

  core_audio_server->flags &= (~flags);
  
  g_rec_mutex_unlock(core_audio_server_mutex);
}

void
ags_core_audio_server_set_url(AgsSoundServer *sound_server,
			      gchar *url)
{
  AgsCoreAudioServer *core_audio_server;

  GRecMutex *core_audio_server_mutex;

  core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server);

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  /* set URL */
  g_rec_mutex_lock(core_audio_server_mutex);

  core_audio_server->url = g_strdup(url);

  g_rec_mutex_unlock(core_audio_server_mutex);
}

gchar*
ags_core_audio_server_get_url(AgsSoundServer *sound_server)
{
  AgsCoreAudioServer *core_audio_server;

  gchar *url;

  GRecMutex *core_audio_server_mutex;

  core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server);

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  /* set URL */
  g_rec_mutex_lock(core_audio_server_mutex);

  url = core_audio_server->url;

  g_rec_mutex_unlock(core_audio_server_mutex);
  
  return(url);
}


void
ags_core_audio_server_set_ports(AgsSoundServer *sound_server,
				guint *port, guint port_count)
{
  AgsCoreAudioServer *core_audio_server;

  GRecMutex *core_audio_server_mutex;

  core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server);

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  /* set ports */
  g_rec_mutex_lock(core_audio_server_mutex);

  core_audio_server->port = port;
  core_audio_server->port_count = port_count;

  g_rec_mutex_unlock(core_audio_server_mutex);
}

guint*
ags_core_audio_server_get_ports(AgsSoundServer *sound_server,
				guint *port_count)
{
  AgsCoreAudioServer *core_audio_server;

  guint *port;
  
  GRecMutex *core_audio_server_mutex;

  core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server);

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  /* get ports */
  g_rec_mutex_lock(core_audio_server_mutex);

  if(port_count != NULL){
    *port_count = AGS_CORE_AUDIO_SERVER(sound_server)->port_count;
  }

  port = core_audio_server->port;

  g_rec_mutex_unlock(core_audio_server_mutex);
  
  return(port);
}

void
ags_core_audio_server_set_soundcard(AgsSoundServer *sound_server,
				    gchar *client_uuid,
				    GList *soundcard)
{
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioClient *core_audio_client;

  GList *list;

  core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server);
  
  core_audio_client = (AgsCoreAudioClient *) ags_core_audio_server_find_client(core_audio_server,
									       client_uuid);

  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return;
  }
  
  //NOTE:JK: soundcard won't removed
  list = soundcard;

  while(list != NULL){
    ags_core_audio_client_add_device(core_audio_client,
				     (GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_core_audio_server_get_soundcard(AgsSoundServer *sound_server,
				    gchar *client_uuid)
{
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioClient *core_audio_client;

  GList *device_start, *device;
  GList *list;
  
  core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server);

  core_audio_client = (AgsCoreAudioClient *) ags_core_audio_server_find_client(core_audio_server,
									       client_uuid);

  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return(NULL);
  }

  g_object_get(core_audio_client,
	       "device", &device_start,
	       NULL);
  
  device = device_start;
  list = NULL;

  while(device != NULL){
    if(AGS_IS_CORE_AUDIO_DEVOUT(device->data)){
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
ags_core_audio_server_set_sequencer(AgsSoundServer *sound_server,
				    gchar *client_uuid,
				    GList *sequencer)
{
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioClient *core_audio_client;

  GList *list;

  core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server);

  core_audio_client = (AgsCoreAudioClient *) ags_core_audio_server_find_client(core_audio_server,
									       client_uuid);

  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return;
  }

  //NOTE:JK: sequencer won't removed
  list = sequencer;

  while(list != NULL){
    ags_core_audio_client_add_device(core_audio_client,
				     (GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_core_audio_server_get_sequencer(AgsSoundServer *sound_server,
				    gchar *client_uuid)
{
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioClient *core_audio_client;

  GList *device_start, *device;
  GList *list;
  
  core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server);

  core_audio_client = (AgsCoreAudioClient *) ags_core_audio_server_find_client(core_audio_server,
									       client_uuid);

  if(!AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return(NULL);
  }

  g_object_get(core_audio_client,
	       "device", &device_start,
	       NULL);
  
  device = device_start;
  list = NULL;

#if 0  
  while(device != NULL){
    if(AGS_IS_CORE_AUDIO_MIDIIN(device->data)){
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
ags_core_audio_server_register_soundcard(AgsSoundServer *sound_server,
					 gboolean is_output)
{
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioClient *default_client;
  AgsCoreAudioPort *core_audio_port;
  AgsCoreAudioDevout *core_audio_devout;
  AgsCoreAudioDevin *core_audio_devin;

  AgsApplicationContext *application_context;

  GObject *soundcard;

#ifdef AGS_WITH_CORE_AUDIO
  AUGraph *graph;
#else
  gpointer graph;
#endif

  gchar *str;  

  guint n_soundcards;
  gboolean initial_set;
  guint i;

  GRecMutex *core_audio_server_mutex;
  GRecMutex *core_audio_client_mutex;

  core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server);

  application_context = ags_application_context_get_instance();

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  /* the default client */
  initial_set = FALSE;
  
  /* get some fields */
  g_rec_mutex_lock(core_audio_server_mutex);

  default_client = (AgsCoreAudioClient *) core_audio_server->default_client;

  n_soundcards = core_audio_server->n_soundcards;
  
  g_rec_mutex_unlock(core_audio_server_mutex);

  /* the default client */
  if(default_client == NULL){
    default_client = ags_core_audio_client_new((GObject *) core_audio_server);
    
    g_object_set(core_audio_server,
		 "default-core-audio-client", default_client,
		 NULL);
    ags_core_audio_server_add_client(core_audio_server,
				     (GObject *) default_client);
    
    ags_core_audio_client_open((AgsCoreAudioClient *) default_client,
			       "ags-default-client");
    initial_set = TRUE;    
  }

  /* get core audio client mutex */
  core_audio_client_mutex = AGS_CORE_AUDIO_CLIENT_GET_OBJ_MUTEX(default_client);

  /* get graph */
  g_rec_mutex_lock(core_audio_client_mutex);

  graph = default_client->graph;

  g_rec_mutex_unlock(core_audio_client_mutex);

#if defined(AGS_CORE_AUDIO_PORT_USE_HW)
#else
  if(graph == NULL){
    g_warning("ags_core_audio_server.c - can't open core audio client");
  }
#endif
  
  soundcard = NULL;

  /* the soundcard */
  if(is_output){
    core_audio_devout = ags_core_audio_devout_new();
    soundcard = (GObject *) core_audio_devout;

    str = g_strdup_printf("ags-core-audio-devout-%d",
			  n_soundcards);
    
    g_object_set(AGS_CORE_AUDIO_DEVOUT(core_audio_devout),
		 "core-audio-client", default_client,
		 "device", str,
		 NULL);
    g_free(str);
        
    /* register ports */      
    core_audio_port = ags_core_audio_port_new((GObject *) default_client);

    str = g_strdup_printf("ags-soundcard%d",
			  n_soundcards);
    
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

    g_object_set(default_client,
		 "device", core_audio_devout,
		 NULL);

    /* increment n-soundcards */
    g_rec_mutex_lock(core_audio_server_mutex);

    core_audio_server->n_soundcards += 1;

    g_rec_mutex_unlock(core_audio_server_mutex);
  }else{
    core_audio_devin = ags_core_audio_devin_new();
    soundcard = (GObject *) core_audio_devin;

    str = g_strdup_printf("ags-core-audio-devin-%d",
			  n_soundcards);
    
    g_object_set(AGS_CORE_AUDIO_DEVIN(core_audio_devin),
		 "core-audio-client", default_client,
		 "device", str,
		 NULL);
    g_free(str);
        
    /* register ports */      
    core_audio_port = ags_core_audio_port_new((GObject *) default_client);

    str = g_strdup_printf("ags-soundcard%d",
			  n_soundcards);
    
    g_object_set(core_audio_port,
		 "core-audio-device", core_audio_devin,
		 NULL);
    ags_core_audio_client_add_port(default_client,
				   (GObject *) core_audio_port);

    g_object_set(core_audio_devin,
		 "core-audio-port", core_audio_port,
		 NULL);
    
    core_audio_devin->port_name = (gchar **) malloc(2 * sizeof(gchar *));
    core_audio_devin->port_name[0] = g_strdup(str);
    core_audio_devin->port_name[1] = NULL;
    
    ags_core_audio_port_register(core_audio_port,
				 str,
				 TRUE, FALSE,
				 FALSE);

    ags_core_audio_devin_realloc_buffer(core_audio_devin);

    g_object_set(default_client,
		 "device", core_audio_devin,
		 NULL);

    /* increment n-soundcards */
    g_rec_mutex_lock(core_audio_server_mutex);

    core_audio_server->n_soundcards += 1;

    g_rec_mutex_unlock(core_audio_server_mutex);
  }
  
  return(soundcard);
}

void
ags_core_audio_server_unregister_soundcard(AgsSoundServer *sound_server,
					   GObject *soundcard)
{
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioClient *default_client;

#ifdef AGS_WITH_CORE_AUDIO
  AUGraph *graph;
#else
  gpointer graph;
#endif

  GList *list_start, *list;
  GList *port;
  
  GRecMutex *core_audio_server_mutex;

  core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server);

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);
  
  /* the default client */
  g_object_get(core_audio_server,
	       "default-core-audio-client", &default_client,
	       NULL);

  if(default_client == NULL){
    g_warning("GSequencer - no CoreAudio client");
    
    return;
  }
  
  if(AGS_IS_CORE_AUDIO_DEVOUT(soundcard)){
    g_object_get(soundcard,
		 "core-audio-port", &list_start,
		 NULL);

    list = list_start;

    while(list != NULL){
      ags_core_audio_port_unregister(list->data);
      ags_core_audio_client_remove_port(default_client,
				   list->data);
    
      list = list->next;
    }

    g_list_free_full(list_start,
		     g_object_unref);
  }else if(AGS_IS_CORE_AUDIO_DEVIN(soundcard)){
    g_object_get(soundcard,
		 "core-audio-port", &list_start,
		 NULL);

    list = list_start;

    while(list != NULL){
      ags_core_audio_port_unregister(list->data);
      ags_core_audio_client_remove_port(default_client,
				   list->data);
    
      list = list->next;
    }

    g_list_free_full(list_start,
		     g_object_unref);
  }
  
  ags_core_audio_client_remove_device(default_client,
				      soundcard);
  
  g_object_get(default_client,
	       "port", &port,
	       NULL);

  if(port == NULL){
    /* reset n-soundcards */
    g_rec_mutex_lock(core_audio_server_mutex);

    core_audio_server->n_soundcards = 0;

    g_rec_mutex_unlock(core_audio_server_mutex);
  }

  g_object_unref(default_client);
  
  g_list_free_full(port,
		   g_object_unref);
}

GObject*
ags_core_audio_server_register_sequencer(AgsSoundServer *sound_server,
					 gboolean is_output)
{
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioClient *default_client;
  AgsCoreAudioPort *core_audio_port;
  AgsCoreAudioMidiin *core_audio_midiin;

  AgsApplicationContext *application_context;

#ifdef AGS_WITH_CORE_AUDIO
  AUGraph *graph;
#else
  gpointer graph;
#endif

  gchar *str;

  guint n_sequencers;

  GRecMutex *core_audio_server_mutex;
  GRecMutex *core_audio_client_mutex;
  
  if(is_output){
    g_warning("GSequencer - MIDI output not implemented");
    return(NULL);
  }
  
  core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server);

  application_context = ags_application_context_get_instance();

  /* get core-audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  /* get some fields */
  g_rec_mutex_lock(core_audio_server_mutex);

  default_client = (AgsCoreAudioClient *) core_audio_server->default_client;

  n_sequencers = core_audio_server->n_sequencers;
  
  g_rec_mutex_unlock(core_audio_server_mutex);

  core_audio_client_mutex = AGS_CORE_AUDIO_CLIENT_GET_OBJ_MUTEX(default_client);

#ifdef AGS_WITH_CORE_AUDIO
  if(n_sequencers >= MIDIGetNumberOfDestinations()){
    return(NULL);
  }
#endif
  
  /* the default client */
  if(default_client == NULL){
    default_client = ags_core_audio_client_new((GObject *) core_audio_server);
    g_object_set(core_audio_server,
		 "default-core-audio-client", default_client,
		 NULL);
    ags_core_audio_server_add_client(core_audio_server,
				     (GObject *) default_client);
    
    ags_core_audio_client_open((AgsCoreAudioClient *) default_client,
			       "ags-default-client");    
  }

  /* get graph */
  g_rec_mutex_lock(core_audio_client_mutex);

  graph = default_client->graph;

  g_rec_mutex_unlock(core_audio_client_mutex);

#if defined(AGS_CORE_AUDIO_PORT_USE_HW)
#else
  if(graph == NULL){
    g_warning("ags_core_audio_server.c - can't open core audio client");
  }
#endif
  
  core_audio_midiin = ags_core_audio_midiin_new();

  str = g_strdup_printf("ags-core-audio-midiin-%d",
			n_sequencers);

  g_object_set(AGS_CORE_AUDIO_MIDIIN(core_audio_midiin),
	       "core-audio-client", default_client,
	       "device", str,
	       NULL);

  g_free(str);
  
  /* register sequencer */  
  core_audio_port = ags_core_audio_port_new((GObject *) default_client);
  core_audio_port->midi_port_number = n_sequencers;
  g_object_set(core_audio_port,
	       "core-audio-device", core_audio_midiin,
	       NULL);
  ags_core_audio_client_add_port(default_client,
				 (GObject *) core_audio_port);

  g_object_set(core_audio_midiin,
	       "core-audio-port", core_audio_port,
	       NULL);

  str = g_strdup_printf("ags-sequencer%d",
			n_sequencers);

#ifdef AGS_DEBUG
  g_message("%s", str);
#endif

  core_audio_midiin->port_name = (gchar **) malloc(2 * sizeof(gchar *));
  core_audio_midiin->port_name[0] = g_strdup(str);
  core_audio_midiin->port_name[1] = NULL;
  
  ags_core_audio_port_register(core_audio_port,
			       str,
			       FALSE, TRUE,
			       FALSE);

  g_object_set(default_client,
	       "device", core_audio_midiin,
	       NULL);

  /* increment n-sequencers */
  g_rec_mutex_lock(core_audio_server_mutex);

  core_audio_server->n_sequencers += 1;

  g_rec_mutex_unlock(core_audio_server_mutex);
  
  return((GObject *) core_audio_midiin);
}

void
ags_core_audio_server_unregister_sequencer(AgsSoundServer *sound_server,
					   GObject *sequencer)
{
  AgsCoreAudioServer *core_audio_server;
  AgsCoreAudioClient *default_client;

  GList *list_start, *list;
  GList *port;

  GRecMutex *core_audio_server_mutex;
  
  core_audio_server = AGS_CORE_AUDIO_SERVER(sound_server);

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);
  
  /* the default client */
  g_object_get(core_audio_server,
	       "default-core-audio-client", &default_client,
	       NULL);

  if(default_client == NULL){
    g_warning("GSequencer - no core audio client");
    
    return;
  }

  g_object_get(sequencer,
	       "core-audio-port", &list_start,
	       NULL);

  list = list_start;

  while(list != NULL){
    ags_core_audio_port_unregister(list->data);
    ags_core_audio_client_remove_port(default_client,
				list->data);
    

    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);

  ags_core_audio_client_remove_device(default_client,
				sequencer);
  
  g_object_get(default_client,
	       "port", &port,
	       NULL);

  if(port == NULL){
    /* reset n-sequencers */
    g_rec_mutex_lock(core_audio_server_mutex);

    core_audio_server->n_sequencers = 0;

    g_rec_mutex_unlock(core_audio_server_mutex);
  }

  g_object_unref(default_client);
  
  g_list_free_full(port,
		   g_object_unref);
}

/**
 * ags_core_audio_server_register_default_soundcard:
 * @core_audio_server: the #AgsCoreAudioServer
 * 
 * Register default soundcard.
 * 
 * Returns: the instantiated #AgsCoreAudioDevout
 * 
 * Since: 3.0.0
 */
GObject*
ags_core_audio_server_register_default_soundcard(AgsCoreAudioServer *core_audio_server)
{
  AgsCoreAudioClient *default_client;
  AgsCoreAudioDevout *core_audio_devout;
  AgsCoreAudioPort *core_audio_port;

  AgsApplicationContext *application_context;

#ifdef AGS_WITH_CORE_AUDIO
  AUGraph *graph;
#else
  gpointer graph;
#endif

  gchar *str;
  
  guint n_soundcards;
  guint i;
  
  GRecMutex *core_audio_server_mutex;
  GRecMutex *core_audio_client_mutex;

  if(!AGS_IS_CORE_AUDIO_SERVER(core_audio_server)){
    return(NULL);
  }

  application_context = ags_application_context_get_instance();

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  /* get some fields */
  g_rec_mutex_lock(core_audio_server_mutex);

  default_client = (AgsCoreAudioClient *) core_audio_server->default_client;

  n_soundcards = core_audio_server->n_soundcards;
  
  g_rec_mutex_unlock(core_audio_server_mutex);
  
  /* the default client */
  if(default_client == NULL){
    default_client = ags_core_audio_client_new((GObject *) core_audio_server);
    
    g_object_set(core_audio_server,
		 "default-core-audio-client", default_client,
		 NULL);
    ags_core_audio_server_add_client(core_audio_server,
				     (GObject *) default_client);
    
    ags_core_audio_client_open((AgsCoreAudioClient *) core_audio_server->default_client,
			       "ags-default-client");
  }

  /* get core audio client mutex */
  core_audio_client_mutex = AGS_CORE_AUDIO_CLIENT_GET_OBJ_MUTEX(default_client);

  /* get graph */
  g_rec_mutex_lock(core_audio_client_mutex);

  graph = default_client->graph;

  g_rec_mutex_unlock(core_audio_client_mutex);

#if defined(AGS_CORE_AUDIO_PORT_USE_HW)
#else
  if(graph == NULL){
    g_warning("ags_core_audio_server.c - can't open core audio client");
  }
#endif
  
  /* the soundcard */
  core_audio_devout = ags_core_audio_devout_new();
  g_object_set(AGS_CORE_AUDIO_DEVOUT(core_audio_devout),
	       "core-audio-client", default_client,
	       "device", "ags-default-devout",
	       NULL);
  
  /* register ports */
  core_audio_port = ags_core_audio_port_new((GObject *) default_client);
  g_object_set(core_audio_port,
	       "core-audio-device", core_audio_devout,
	       NULL);
  ags_core_audio_client_add_port(default_client,
				 (GObject *) core_audio_port);

  g_object_set(core_audio_devout,
	       "core-audio-port", core_audio_port,
	       NULL);

  str = g_strdup_printf("ags-default-soundcard");

#ifdef AGS_DEBUG
  g_message("%s", str);
#endif    
  
  core_audio_devout->port_name = (gchar **) malloc(2 * sizeof(gchar *));
  core_audio_devout->port_name[0] = g_strdup(str);
  core_audio_devout->port_name[1] = NULL;
  
  ags_core_audio_port_register(core_audio_port,
			       str,
			       TRUE, FALSE,
			       TRUE);

  g_free(str);

  g_object_set(default_client,
	       "device", core_audio_devout,
	       NULL);
  
  return((GObject *) core_audio_devout);
}

/**
 * ags_core_audio_server_find_url:
 * @core_audio_server: (element-type AgsAudio.CoreAudioServer) (transfer none): the #GList-struct containing #AgsCoreAudioServer
 * @url: the url to find
 *
 * Find #AgsCoreAudioServer by url.
 *
 * Returns: (element-type AgsAudio.CoreAudioServer) (transfer none): the next matching #GList-struct containing a #AgsCoreAudioServer matching @url or %NULL
 *
 * Since: 3.0.0
 */
GList*
ags_core_audio_server_find_url(GList *core_audio_server,
			       gchar *url)
{
  GList *retval;
  
  GRecMutex *core_audio_server_mutex;

  retval = NULL;
  
  while(core_audio_server != NULL){
    /* get core audio server mutex */
    core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server->data);

    /* check URL */
    g_rec_mutex_lock(core_audio_server_mutex);
    
    if(!g_ascii_strcasecmp(AGS_CORE_AUDIO_SERVER(core_audio_server->data)->url,
			   url)){
      retval = core_audio_server;

      g_rec_mutex_unlock(core_audio_server_mutex);
    
      break;
    }

    g_rec_mutex_unlock(core_audio_server_mutex);
    
    core_audio_server = core_audio_server->next;
  }

  return(retval);
}

/**
 * ags_core_audio_server_find_client:
 * @core_audio_server: the #AgsCoreAudioServer
 * @client_uuid: the uuid to find
 *
 * Find #AgsCoreAudioClient by uuid.
 *
 * Returns: (transfer none): the #AgsCoreAudioClient found or %NULL
 *
 * Since: 3.0.0
 */
GObject*
ags_core_audio_server_find_client(AgsCoreAudioServer *core_audio_server,
				  gchar *client_uuid)
{
  AgsCoreAudioClient *retval;
  
  GList *list_start, *list;

  GRecMutex *core_audio_server_mutex;
  GRecMutex *core_audio_client_mutex;

  if(!AGS_IS_CORE_AUDIO_SERVER(core_audio_server)){
    return(NULL);
  }

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  /* get some fields */
  g_rec_mutex_lock(core_audio_server_mutex);

  list =
    list_start = g_list_copy(core_audio_server->client);

  g_rec_mutex_unlock(core_audio_server_mutex);

  retval = NULL;
  
  while(list != NULL){
    /* get core audio client mutex */
    core_audio_client_mutex = AGS_CORE_AUDIO_CLIENT_GET_OBJ_MUTEX(list->data);

    /* check client UUID */
    g_rec_mutex_lock(core_audio_client_mutex);
    
    if(!g_ascii_strcasecmp(AGS_CORE_AUDIO_CLIENT(list->data)->client_uuid,
			   client_uuid)){
      retval = list->data;

      g_rec_mutex_unlock(core_audio_client_mutex);

      break;
    }

    g_rec_mutex_unlock(core_audio_client_mutex);
    
    list = list->next;
  }

  g_list_free(list_start);
  
  return((GObject *) retval);
}

/**
 * ags_core_audio_server_find_port:
 * @core_audio_server: the #AgsCoreAudioServer
 * @port_uuid: the uuid to find
 *
 * Find #AgsCoreAudioPort by uuid.
 *
 * Returns: (transfer none): the #AgsCoreAudioPort found or %NULL
 *
 * Since: 3.0.0
 */
GObject*
ags_core_audio_server_find_port(AgsCoreAudioServer *core_audio_server,
				gchar *port_uuid)
{
  GList *client_start, *client;
  GList *port_start, *port;

  gboolean success;
  
  GRecMutex *core_audio_port_mutex;

  g_object_get(core_audio_server,
	       "core-audio-client", &client_start,
	       NULL);

  client = client_start;
  
  while(client != NULL){
    g_object_get(core_audio_server,
		 "core-audio-port", &port_start,
		 NULL);

    port = port_start;
    
    while(port != NULL){
      /* get core audio port mutex */
      core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(port->data);
      
      /* check port UUID */
      g_rec_mutex_lock(core_audio_port_mutex);
      
      success = (!g_ascii_strcasecmp(AGS_CORE_AUDIO_PORT(port->data)->port_uuid,
				     port_uuid)) ? TRUE: FALSE;

      g_rec_mutex_unlock(core_audio_port_mutex);
      
      if(success){
	AgsCoreAudioPort *retval;
	
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
 * ags_core_audio_server_add_client:
 * @core_audio_server: the #AgsCoreAudioServer
 * @core_audio_client: the #AgsCoreAudioClient to add
 *
 * Add @core_audio_client to @core_audio_server
 *
 * Since: 3.0.0
 */
void
ags_core_audio_server_add_client(AgsCoreAudioServer *core_audio_server,
				 GObject *core_audio_client)
{
  GRecMutex *core_audio_server_mutex;

  if(!AGS_IS_CORE_AUDIO_SERVER(core_audio_server) ||
     !AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return;
  }

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  /* get some fields */
  g_rec_mutex_lock(core_audio_server_mutex);

  if(g_list_find(core_audio_server->client, core_audio_client) == NULL){
    g_object_ref(core_audio_client);
    core_audio_server->client = g_list_prepend(core_audio_server->client,
					 core_audio_client);
  }

  g_rec_mutex_unlock(core_audio_server_mutex);
}

/**
 * ags_core_audio_server_remove_client:
 * @core_audio_server: the #AgsCoreAudioServer
 * @core_audio_client: the #AgsCoreAudioClient to remove
 *
 * Remove @core_audio_client to @core_audio_server
 *
 * Since: 3.0.0
 */
void
ags_core_audio_server_remove_client(AgsCoreAudioServer *core_audio_server,
				    GObject *core_audio_client)
{
  GRecMutex *core_audio_server_mutex;

  if(!AGS_IS_CORE_AUDIO_SERVER(core_audio_server) ||
     !AGS_IS_CORE_AUDIO_CLIENT(core_audio_client)){
    return;
  }

  /* get core audio server mutex */
  core_audio_server_mutex = AGS_CORE_AUDIO_SERVER_GET_OBJ_MUTEX(core_audio_server);

  /* get some fields */
  g_rec_mutex_lock(core_audio_server_mutex);

  if(g_list_find(core_audio_server->client, core_audio_client) != NULL){
    core_audio_server->client = g_list_remove(core_audio_server->client,
					core_audio_client);
    g_object_unref(core_audio_client);
  }

  g_rec_mutex_unlock(core_audio_server_mutex);
}

/**
 * ags_core_audio_server_connect_client:
 * @core_audio_server: the #AgsCoreAudioServer
 *
 * Connect all clients.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_server_connect_client(AgsCoreAudioServer *core_audio_server)
{
  GList *client_start, *client;

  gchar *client_name;

  GRecMutex *core_audio_client_mutex;
  
  if(!AGS_IS_CORE_AUDIO_SERVER(core_audio_server)){
    return;
  }

  g_object_get(core_audio_server,
	       "core-audio-client", &client_start,
	       NULL);
  
  client = client_start;

  while(client != NULL){
    /* client name */
    g_object_get(client->data,
		 "client-name", &client_name,
		 NULL);

    /* open */
    ags_core_audio_client_open((AgsCoreAudioClient *) client->data,
			 client_name);
    ags_core_audio_client_activate(client->data);

    g_free(client_name);
    
    /* iterate */
    client = client->next;
  }

  g_list_free_full(client_start,
		   g_object_unref);
}

/**
 * ags_core_audio_server_new:
 * @url: the URL as string
 *
 * Create a new instance of #AgsCoreAudioServer.
 *
 * Returns: the new #AgsCoreAudioServer
 *
 * Since: 3.0.0
 */
AgsCoreAudioServer*
ags_core_audio_server_new(gchar *url)
{
  AgsCoreAudioServer *core_audio_server;

  core_audio_server = (AgsCoreAudioServer *) g_object_new(AGS_TYPE_CORE_AUDIO_SERVER,
							  "url", url,
							  NULL);

  return(core_audio_server);
}
