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

#include <ags/audio/audio-unit/ags_audio_unit_server.h>
#include <ags/audio/audio-unit/ags_audio_unit_client.h>
#include <ags/audio/audio-unit/ags_audio_unit_port.h>

#include <ags/audio/audio-unit/ags_audio_unit_devout.h>
#include <ags/audio/audio-unit/ags_audio_unit_devin.h>

#include <string.h>

#include <errno.h>

#include <ags/i18n.h>

void ags_audio_unit_server_class_init(AgsAudioUnitServerClass *audio_unit_server);
void ags_audio_unit_server_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_unit_server_sound_server_interface_init(AgsSoundServerInterface *sound_server);
void ags_audio_unit_server_init(AgsAudioUnitServer *audio_unit_server);
void ags_audio_unit_server_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_audio_unit_server_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_audio_unit_server_dispose(GObject *gobject);
void ags_audio_unit_server_finalize(GObject *gobject);

AgsUUID* ags_audio_unit_server_get_uuid(AgsConnectable *connectable);
gboolean ags_audio_unit_server_has_resource(AgsConnectable *connectable);
gboolean ags_audio_unit_server_is_ready(AgsConnectable *connectable);
void ags_audio_unit_server_add_to_registry(AgsConnectable *connectable);
void ags_audio_unit_server_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_audio_unit_server_list_resource(AgsConnectable *connectable);
xmlNode* ags_audio_unit_server_xml_compose(AgsConnectable *connectable);
void ags_audio_unit_server_xml_parse(AgsConnectable *connectable,
				     xmlNode *node);
gboolean ags_audio_unit_server_is_connected(AgsConnectable *connectable);
void ags_audio_unit_server_connect(AgsConnectable *connectable);
void ags_audio_unit_server_disconnect(AgsConnectable *connectable);

void ags_audio_unit_server_set_url(AgsSoundServer *sound_server,
				   gchar *url);
gchar* ags_audio_unit_server_get_url(AgsSoundServer *sound_server);
void ags_audio_unit_server_set_ports(AgsSoundServer *sound_server,
				     guint *ports, guint port_count);
guint* ags_audio_unit_server_get_ports(AgsSoundServer *sound_server,
				       guint *port_count);
void ags_audio_unit_server_set_soundcard(AgsSoundServer *sound_server,
					 gchar *client_uuid,
					 GList *soundcard);
GList* ags_audio_unit_server_get_soundcard(AgsSoundServer *sound_server,
					   gchar *client_uuid);
void ags_audio_unit_server_set_sequencer(AgsSoundServer *sound_server,
					 gchar *client_uuid,
					 GList *sequencer);
GList* ags_audio_unit_server_get_sequencer(AgsSoundServer *sound_server,
					   gchar *client_uuid);
GObject* ags_audio_unit_server_register_soundcard(AgsSoundServer *sound_server,
						  gboolean is_output);
void ags_audio_unit_server_unregister_soundcard(AgsSoundServer *sound_server,
						GObject *soundcard);
GObject* ags_audio_unit_server_register_sequencer(AgsSoundServer *sound_server,
						  gboolean is_output);
void ags_audio_unit_server_unregister_sequencer(AgsSoundServer *sound_server,
						GObject *sequencer);

void* ags_audio_unit_server_do_poll_loop(void *ptr);

/**
 * SECTION:ags_audio_unit_server
 * @short_description: audio unit instance
 * @title: AgsAudioUnitServer
 * @section_id:
 * @include: ags/audio/audio-unit/ags_audio_unit_server.h
 *
 * The #AgsAudioUnitServer is an object to represent a running audio unit instance.
 */

enum{
  PROP_0,
  PROP_URL,
  PROP_DEFAULT_SOUNDCARD,
  PROP_DEFAULT_AUDIO_UNIT_CLIENT,
  PROP_INPUT_AUDIO_UNIT_CLIENT,
  PROP_AUDIO_UNIT_CLIENT,
};

static gpointer ags_audio_unit_server_parent_class = NULL;

GType
ags_audio_unit_server_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_unit_server = 0;

    static const GTypeInfo ags_audio_unit_server_info = {
      sizeof(AgsAudioUnitServerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_unit_server_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAudioUnitServer),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_unit_server_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_unit_server_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_sound_server_interface_info = {
      (GInterfaceInitFunc) ags_audio_unit_server_sound_server_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_unit_server = g_type_register_static(G_TYPE_OBJECT,
							"AgsAudioUnitServer",
							&ags_audio_unit_server_info,
							0);

    g_type_add_interface_static(ags_type_audio_unit_server,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_audio_unit_server,
				AGS_TYPE_SOUND_SERVER,
				&ags_sound_server_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_unit_server);
  }

  return g_define_type_id__volatile;
}

void
ags_audio_unit_server_class_init(AgsAudioUnitServerClass *audio_unit_server)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_audio_unit_server_parent_class = g_type_class_peek_parent(audio_unit_server);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_unit_server;

  gobject->set_property = ags_audio_unit_server_set_property;
  gobject->get_property = ags_audio_unit_server_get_property;

  gobject->dispose = ags_audio_unit_server_dispose;
  gobject->finalize = ags_audio_unit_server_finalize;

  /* properties */
  /**
   * AgsAudioUnitServer:url:
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
   * AgsAudioUnitServer:default-soundcard:
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
   * AgsAudioUnitServer:default-audio-unit-client:
   *
   * The default audio unit client.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("default-audio-unit-client",
				   i18n_pspec("default audio unit client"),
				   i18n_pspec("The default audio unit client"),
				   AGS_TYPE_AUDIO_UNIT_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEFAULT_AUDIO_UNIT_CLIENT,
				  param_spec);

  /**
   * AgsAudioUnitServer:input-audio-unit-client:
   *
   * The input audio unit client.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("input-audio-unit-client",
				   i18n_pspec("input audio unit client"),
				   i18n_pspec("The input audio unit client"),
				   AGS_TYPE_AUDIO_UNIT_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_AUDIO_UNIT_CLIENT,
				  param_spec);

  /**
   * AgsAudioUnitServer:audio-unit-client: (type GList(AgsAudioUnitClient)) (transfer full)
   *
   * The audio unit client list.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("audio-unit-client",
				    i18n_pspec("audio unit client list"),
				    i18n_pspec("The audio unit client list"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_UNIT_CLIENT,
				  param_spec);
}

void
ags_audio_unit_server_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_audio_unit_server_get_uuid;
  connectable->has_resource = ags_audio_unit_server_has_resource;

  connectable->is_ready = ags_audio_unit_server_is_ready;
  connectable->add_to_registry = ags_audio_unit_server_add_to_registry;
  connectable->remove_from_registry = ags_audio_unit_server_remove_from_registry;

  connectable->list_resource = ags_audio_unit_server_list_resource;
  connectable->xml_compose = ags_audio_unit_server_xml_compose;
  connectable->xml_parse = ags_audio_unit_server_xml_parse;

  connectable->is_connected = ags_audio_unit_server_is_connected;  
  connectable->connect = ags_audio_unit_server_connect;
  connectable->disconnect = ags_audio_unit_server_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_audio_unit_server_sound_server_interface_init(AgsSoundServerInterface *sound_server)
{
  sound_server->set_url = ags_audio_unit_server_set_url;
  sound_server->get_url = ags_audio_unit_server_get_url;
  sound_server->set_ports = ags_audio_unit_server_set_ports;
  sound_server->get_ports = ags_audio_unit_server_get_ports;
  sound_server->set_soundcard = ags_audio_unit_server_set_soundcard;
  sound_server->get_soundcard = ags_audio_unit_server_get_soundcard;
  sound_server->set_sequencer = ags_audio_unit_server_set_sequencer;
  sound_server->get_sequencer = ags_audio_unit_server_get_sequencer;
  sound_server->register_soundcard = ags_audio_unit_server_register_soundcard;
  sound_server->unregister_soundcard = ags_audio_unit_server_unregister_soundcard;
  sound_server->register_sequencer = ags_audio_unit_server_register_sequencer;
  sound_server->unregister_sequencer = ags_audio_unit_server_unregister_sequencer;
}

void
ags_audio_unit_server_init(AgsAudioUnitServer *audio_unit_server)
{
  /* flags */
  audio_unit_server->flags = 0;

  /* server mutex */
  g_rec_mutex_init(&(audio_unit_server->obj_mutex));
  
  g_atomic_int_set(&(audio_unit_server->running),
		   TRUE);
  audio_unit_server->thread = NULL;

  /* uuid */
  audio_unit_server->uuid = ags_uuid_alloc();
  ags_uuid_generate(audio_unit_server->uuid);

  audio_unit_server->url = NULL;
  
  audio_unit_server->port = NULL;
  audio_unit_server->port_count = 0;
  
  audio_unit_server->n_soundcards = 0;
  audio_unit_server->n_sequencers = 0;

  audio_unit_server->default_soundcard = NULL;

  audio_unit_server->default_client = NULL;
  audio_unit_server->input_client = NULL;

  audio_unit_server->client = NULL;
}

void
ags_audio_unit_server_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsAudioUnitServer *audio_unit_server;

  GRecMutex *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(gobject);

  /* get audio-unit server mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);

  switch(prop_id){
  case PROP_URL:
    {
      gchar *url;

      url = g_value_get_string(value);

      g_rec_mutex_lock(audio_unit_server_mutex);

      if(audio_unit_server->url == url){
	g_rec_mutex_unlock(audio_unit_server_mutex);

	return;
      }

      if(audio_unit_server->url != NULL){
	g_free(audio_unit_server->url);
      }

      audio_unit_server->url = g_strdup(url);

      g_rec_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_DEFAULT_SOUNDCARD:
    {
      GObject *default_soundcard;

      default_soundcard = (GObject *) g_value_get_object(value);

      g_rec_mutex_lock(audio_unit_server_mutex);

      if(audio_unit_server->default_soundcard == (GObject *) default_soundcard){
	g_rec_mutex_unlock(audio_unit_server_mutex);

	return;
      }

      if(audio_unit_server->default_soundcard != NULL){
	g_object_unref(G_OBJECT(audio_unit_server->default_soundcard));
      }

      if(default_soundcard != NULL){
	g_object_ref(G_OBJECT(default_soundcard));
      }

      audio_unit_server->default_soundcard = (GObject *) default_soundcard;

      g_rec_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_DEFAULT_AUDIO_UNIT_CLIENT:
    {
      AgsAudioUnitClient *default_client;

      default_client = (AgsAudioUnitClient *) g_value_get_object(value);

      g_rec_mutex_lock(audio_unit_server_mutex);

      if(audio_unit_server->default_client == (GObject *) default_client){
	g_rec_mutex_unlock(audio_unit_server_mutex);

	return;
      }

      if(audio_unit_server->default_client != NULL){
	g_object_unref(G_OBJECT(audio_unit_server->default_client));
      }

      if(default_client != NULL){
	g_object_ref(G_OBJECT(default_client));
      }

      audio_unit_server->default_client = (GObject *) default_client;

      g_rec_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_INPUT_AUDIO_UNIT_CLIENT:
    {
      AgsAudioUnitClient *input_client;

      input_client = (AgsAudioUnitClient *) g_value_get_object(value);

      g_rec_mutex_lock(audio_unit_server_mutex);

      if(audio_unit_server->input_client == (GObject *) input_client){
	g_rec_mutex_unlock(audio_unit_server_mutex);

	return;
      }

      if(audio_unit_server->input_client != NULL){
	g_object_unref(G_OBJECT(audio_unit_server->input_client));
      }

      if(input_client != NULL){
	g_object_ref(G_OBJECT(input_client));
      }

      audio_unit_server->input_client = (GObject *) input_client;

      g_rec_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_AUDIO_UNIT_CLIENT:
    {
      GObject *client;

      client = (GObject *) g_value_get_pointer(value);

      g_rec_mutex_lock(audio_unit_server_mutex);

      if(!AGS_IS_AUDIO_UNIT_CLIENT(client) ||
	 g_list_find(audio_unit_server->client, client) != NULL){
	g_rec_mutex_unlock(audio_unit_server_mutex);

	return;
      }

      g_object_ref(G_OBJECT(client));
      audio_unit_server->client = g_list_prepend(audio_unit_server->client,
						 client);

      g_rec_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_server_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsAudioUnitServer *audio_unit_server;

  GRecMutex *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(gobject);

  /* get audio-unit server mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);
  
  switch(prop_id){
  case PROP_URL:
    {
      g_rec_mutex_lock(audio_unit_server_mutex);

      g_value_set_string(value, audio_unit_server->url);

      g_rec_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_DEFAULT_SOUNDCARD:
    {
      g_rec_mutex_lock(audio_unit_server_mutex);

      g_value_set_object(value, audio_unit_server->default_soundcard);

      g_rec_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_DEFAULT_AUDIO_UNIT_CLIENT:
    {
      g_rec_mutex_lock(audio_unit_server_mutex);

      g_value_set_object(value, audio_unit_server->default_client);

      g_rec_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_INPUT_AUDIO_UNIT_CLIENT:
    {
      g_rec_mutex_lock(audio_unit_server_mutex);

      g_value_set_object(value, audio_unit_server->input_client);

      g_rec_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_AUDIO_UNIT_CLIENT:
    {
      g_rec_mutex_lock(audio_unit_server_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(audio_unit_server->client,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_server_dispose(GObject *gobject)
{
  AgsAudioUnitServer *audio_unit_server;

  GList *list;
  
  audio_unit_server = AGS_AUDIO_UNIT_SERVER(gobject);

  /* default soundcard */
  if(audio_unit_server->default_soundcard != NULL){
    g_object_unref(G_OBJECT(audio_unit_server->default_soundcard));

    audio_unit_server->default_soundcard = NULL;
  }
  
  /* default client */
  if(audio_unit_server->default_client != NULL){
    g_object_unref(G_OBJECT(audio_unit_server->default_client));

    audio_unit_server->default_client = NULL;
  }
  
  /* input client */
  if(audio_unit_server->input_client != NULL){
    g_object_unref(G_OBJECT(audio_unit_server->input_client));

    audio_unit_server->input_client = NULL;
  }
  
  /* client */
  if(audio_unit_server->client != NULL){
    list = audio_unit_server->client;

    while(list != NULL){
      g_object_run_dispose(G_OBJECT(list->data));

      list = list->next;
    }
    
    g_list_free_full(audio_unit_server->client,
		     g_object_unref);

    audio_unit_server->client = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_server_parent_class)->dispose(gobject);
}

void
ags_audio_unit_server_finalize(GObject *gobject)
{
  AgsAudioUnitServer *audio_unit_server;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(gobject);

  /* url */
  g_free(audio_unit_server->url);

  /* default soundcard */
  if(audio_unit_server->default_soundcard != NULL){
    g_object_unref(G_OBJECT(audio_unit_server->default_soundcard));
  }
  
  /* default client */
  if(audio_unit_server->default_client != NULL){
    g_object_unref(G_OBJECT(audio_unit_server->default_client));
  }
    
  /* input client */
  if(audio_unit_server->input_client != NULL){
    g_object_unref(G_OBJECT(audio_unit_server->input_client));
  }
  
  /* client */
  if(audio_unit_server->client != NULL){
    g_list_free_full(audio_unit_server->client,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_server_parent_class)->finalize(gobject);
}

AgsUUID*
ags_audio_unit_server_get_uuid(AgsConnectable *connectable)
{
  AgsAudioUnitServer *audio_unit_server;
  
  AgsUUID *ptr;

  GRecMutex *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(connectable);

  /* get audio unit server signal mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);

  /* get UUID */
  g_rec_mutex_lock(audio_unit_server_mutex);

  ptr = audio_unit_server->uuid;

  g_rec_mutex_unlock(audio_unit_server_mutex);
  
  return(ptr);
}

gboolean
ags_audio_unit_server_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_audio_unit_server_is_ready(AgsConnectable *connectable)
{
  AgsAudioUnitServer *audio_unit_server;
  
  gboolean is_ready;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(connectable);

  /* check is added */
  is_ready = ags_audio_unit_server_test_flags(audio_unit_server, AGS_AUDIO_UNIT_SERVER_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_audio_unit_server_add_to_registry(AgsConnectable *connectable)
{
  AgsAudioUnitServer *audio_unit_server;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  audio_unit_server = AGS_AUDIO_UNIT_SERVER(connectable);

  ags_audio_unit_server_set_flags(audio_unit_server, AGS_AUDIO_UNIT_SERVER_ADDED_TO_REGISTRY);
}

void
ags_audio_unit_server_remove_from_registry(AgsConnectable *connectable)
{
  AgsAudioUnitServer *audio_unit_server;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(connectable);

  ags_audio_unit_server_unset_flags(audio_unit_server, AGS_AUDIO_UNIT_SERVER_ADDED_TO_REGISTRY);
}

xmlNode*
ags_audio_unit_server_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_audio_unit_server_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_audio_unit_server_xml_parse(AgsConnectable *connectable,
				xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_audio_unit_server_is_connected(AgsConnectable *connectable)
{
  AgsAudioUnitServer *audio_unit_server;
  
  gboolean is_connected;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(connectable);

  /* check is connected */
  is_connected = ags_audio_unit_server_test_flags(audio_unit_server, AGS_AUDIO_UNIT_SERVER_CONNECTED);
  
  return(is_connected);
}

void
ags_audio_unit_server_connect(AgsConnectable *connectable)
{
  AgsAudioUnitServer *audio_unit_server;

  GList *start_list, *list;  

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(connectable);

  ags_audio_unit_server_set_flags(audio_unit_server, AGS_AUDIO_UNIT_SERVER_CONNECTED);

  /* connect client */
  g_object_get(audio_unit_server,
	       "audio-unit-client", &start_list,
	       NULL);

  list = start_list;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

void
ags_audio_unit_server_disconnect(AgsConnectable *connectable)
{
  AgsAudioUnitServer *audio_unit_server;

  GList *start_list, *list;  

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(connectable);
  
  ags_audio_unit_server_unset_flags(audio_unit_server, AGS_AUDIO_UNIT_SERVER_CONNECTED);

  /* connect client */
  g_object_get(audio_unit_server,
	       "audio-unit-client", &start_list,
	       NULL);

  list = start_list;

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
}

/**
 * ags_audio_unit_server_test_flags:
 * @audio_unit_server: the #AgsAudioUnitServer
 * @flags: the flags
 *
 * Test @flags to be set on @audio_unit_server.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_audio_unit_server_test_flags(AgsAudioUnitServer *audio_unit_server, guint flags)
{
  gboolean retval;  
  
  GRecMutex *audio_unit_server_mutex;

  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server)){
    return(FALSE);
  }

  /* get audio unit server mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);

  /* test */
  g_rec_mutex_lock(audio_unit_server_mutex);

  retval = (flags & (audio_unit_server->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(audio_unit_server_mutex);

  return(retval);
}

/**
 * ags_audio_unit_server_set_flags:
 * @audio_unit_server: the #AgsAudioUnitServer
 * @flags: see #AgsAudioUnitServerFlags-enum
 *
 * Enable a feature of @audio_unit_server.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_server_set_flags(AgsAudioUnitServer *audio_unit_server, guint flags)
{
  GRecMutex *audio_unit_server_mutex;

  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server)){
    return;
  }

  /* get audio unit server mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(audio_unit_server_mutex);

  audio_unit_server->flags |= flags;
  
  g_rec_mutex_unlock(audio_unit_server_mutex);
}
    
/**
 * ags_audio_unit_server_unset_flags:
 * @audio_unit_server: the #AgsAudioUnitServer
 * @flags: see #AgsAudioUnitServerFlags-enum
 *
 * Disable a feature of @audio_unit_server.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_server_unset_flags(AgsAudioUnitServer *audio_unit_server, guint flags)
{  
  GRecMutex *audio_unit_server_mutex;

  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server)){
    return;
  }

  /* get audio unit server mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(audio_unit_server_mutex);

  audio_unit_server->flags &= (~flags);
  
  g_rec_mutex_unlock(audio_unit_server_mutex);
}

void
ags_audio_unit_server_set_url(AgsSoundServer *sound_server,
			      gchar *url)
{
  AgsAudioUnitServer *audio_unit_server;

  GRecMutex *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  /* get audio unit server mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);

  /* set URL */
  g_rec_mutex_lock(audio_unit_server_mutex);

  audio_unit_server->url = g_strdup(url);

  g_rec_mutex_unlock(audio_unit_server_mutex);
}

gchar*
ags_audio_unit_server_get_url(AgsSoundServer *sound_server)
{
  AgsAudioUnitServer *audio_unit_server;

  gchar *url;

  GRecMutex *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  /* get audio unit server mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);

  /* set URL */
  g_rec_mutex_lock(audio_unit_server_mutex);

  url = g_strdup(audio_unit_server->url);

  g_rec_mutex_unlock(audio_unit_server_mutex);
  
  return(url);
}


void
ags_audio_unit_server_set_ports(AgsSoundServer *sound_server,
				guint *port, guint port_count)
{
  AgsAudioUnitServer *audio_unit_server;

  GRecMutex *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  /* get audio unit server mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);

  /* set ports */
  g_rec_mutex_lock(audio_unit_server_mutex);

  audio_unit_server->port = port;
  audio_unit_server->port_count = port_count;

  g_rec_mutex_unlock(audio_unit_server_mutex);
}

guint*
ags_audio_unit_server_get_ports(AgsSoundServer *sound_server,
				guint *port_count)
{
  AgsAudioUnitServer *audio_unit_server;

  guint *port;
  
  GRecMutex *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  /* get audio unit server mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);

  /* get ports */
  g_rec_mutex_lock(audio_unit_server_mutex);

  if(port_count != NULL){
    *port_count = AGS_AUDIO_UNIT_SERVER(sound_server)->port_count;
  }

  port = audio_unit_server->port;

  g_rec_mutex_unlock(audio_unit_server_mutex);
  
  return(port);
}

void
ags_audio_unit_server_set_soundcard(AgsSoundServer *sound_server,
				    gchar *client_uuid,
				    GList *soundcard)
{
  AgsAudioUnitServer *audio_unit_server;
  AgsAudioUnitClient *audio_unit_client;

  GList *list;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);
  
  audio_unit_client = (AgsAudioUnitClient *) ags_audio_unit_server_find_client(audio_unit_server,
									       client_uuid);

  if(!AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client)){
    return;
  }
  
  //NOTE:JK: soundcard won't removed
  list = soundcard;

  while(list != NULL){
    ags_audio_unit_client_add_device(audio_unit_client,
				     (GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_audio_unit_server_get_soundcard(AgsSoundServer *sound_server,
				    gchar *client_uuid)
{
  AgsAudioUnitServer *audio_unit_server;
  AgsAudioUnitClient *audio_unit_client;

  GList *device_start, *device;
  GList *list;
  
  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  audio_unit_client = (AgsAudioUnitClient *) ags_audio_unit_server_find_client(audio_unit_server,
									       client_uuid);

  if(!AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client)){
    return(NULL);
  }

  g_object_get(audio_unit_client,
	       "device", &device_start,
	       NULL);
  
  device = device_start;
  list = NULL;

  while(device != NULL){
    if(AGS_IS_AUDIO_UNIT_DEVOUT(device->data)){
      list = g_list_prepend(list,
			    device->data);
      g_object_ref(device->data);
    }

    device = device->next;
  }

  return(g_list_reverse(list));
}


void
ags_audio_unit_server_set_sequencer(AgsSoundServer *sound_server,
				    gchar *client_uuid,
				    GList *sequencer)
{
  AgsAudioUnitServer *audio_unit_server;
  AgsAudioUnitClient *audio_unit_client;

  GList *list;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  audio_unit_client = (AgsAudioUnitClient *) ags_audio_unit_server_find_client(audio_unit_server,
									       client_uuid);

  if(!AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client)){
    return;
  }

  //NOTE:JK: sequencer won't removed
  list = sequencer;

  while(list != NULL){
    ags_audio_unit_client_add_device(audio_unit_client,
				     (GObject *) list->data);
    
    list = list->next;
  }
}

GList*
ags_audio_unit_server_get_sequencer(AgsSoundServer *sound_server,
				    gchar *client_uuid)
{
  AgsAudioUnitServer *audio_unit_server;
  AgsAudioUnitClient *audio_unit_client;

  GList *device_start, *device;
  GList *list;
  
  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  audio_unit_client = (AgsAudioUnitClient *) ags_audio_unit_server_find_client(audio_unit_server,
									       client_uuid);

  if(!AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client)){
    return(NULL);
  }

  g_object_get(audio_unit_client,
	       "device", &device_start,
	       NULL);
  
  device = device_start;
  list = NULL;

#if 0  
  while(device != NULL){
    if(AGS_IS_AUDIO_UNIT_MIDIIN(device->data)){
      list = g_list_prepend(list,
			    device->data);
      g_object_ref(device->data);
    }

    device = device->next;
  }
#endif
  
  return(g_list_reverse(list));
}

GObject*
ags_audio_unit_server_register_soundcard(AgsSoundServer *sound_server,
					 gboolean is_output)
{
  AgsAudioUnitServer *audio_unit_server;
  AgsAudioUnitClient *audio_unit_client;
  AgsAudioUnitClient *default_client;
  AgsAudioUnitClient *input_client;
  AgsAudioUnitPort *audio_unit_port;
  AgsAudioUnitDevout *audio_unit_devout;
  AgsAudioUnitDevin *audio_unit_devin;

  AgsApplicationContext *application_context;

  GObject *soundcard;

  gchar *str;  

  guint n_soundcards;
  gboolean initial_set;
  guint i;

  GRecMutex *audio_unit_server_mutex;
  GRecMutex *audio_unit_client_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  application_context = ags_application_context_get_instance();

  /* get audio unit server mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);

  /* the default client */
  initial_set = FALSE;
  
  /* get some fields */
  g_rec_mutex_lock(audio_unit_server_mutex);

  audio_unit_client = 
    default_client = (AgsAudioUnitClient *) audio_unit_server->default_client;
  input_client = (AgsAudioUnitClient *) audio_unit_server->input_client;

  n_soundcards = audio_unit_server->n_soundcards;
  
  g_rec_mutex_unlock(audio_unit_server_mutex);

  if(!is_output &&
     input_client != NULL){
    audio_unit_client = input_client;
  }
  
  /* the default client */
  if(audio_unit_client == NULL){
    audio_unit_client = ags_audio_unit_client_new((GObject *) audio_unit_server);
    
    g_object_set(audio_unit_server,
		 "default-audio-unit-client", audio_unit_client,
		 NULL);
    ags_audio_unit_server_add_client(audio_unit_server,
				     (GObject *) audio_unit_client);
    
    ags_audio_unit_client_open((AgsAudioUnitClient *) audio_unit_client,
			       "ags-default-client");
    initial_set = TRUE;    
  }

  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  soundcard = NULL;

  /* the soundcard */
  if(is_output){
    audio_unit_devout = ags_audio_unit_devout_new(application_context);
    soundcard = (GObject *) audio_unit_devout;

    str = g_strdup_printf("ags-audio-unit-devout-%d",
			  n_soundcards);
    
    g_object_set(AGS_AUDIO_UNIT_DEVOUT(audio_unit_devout),
		 "audio-unit-client", audio_unit_client,
		 "device", str,
		 NULL);
    g_free(str);
        
    /* register ports */      
    audio_unit_port = ags_audio_unit_port_new((GObject *) audio_unit_client);

    str = g_strdup_printf("ags-soundcard%d",
			  n_soundcards);
    
    g_object_set(audio_unit_port,
		 "audio-unit-device", audio_unit_devout,
		 NULL);
    ags_audio_unit_client_add_port(audio_unit_client,
				   (GObject *) audio_unit_port);

    g_object_set(audio_unit_devout,
		 "audio-unit-port", audio_unit_port,
		 NULL);
    
    audio_unit_devout->port_name = (gchar **) malloc(2 * sizeof(gchar *));
    audio_unit_devout->port_name[0] = g_strdup(str);
    audio_unit_devout->port_name[1] = NULL;
    
    ags_audio_unit_port_register(audio_unit_port,
				 str,
				 TRUE, FALSE,
				 TRUE);

    ags_audio_unit_devout_realloc_buffer(audio_unit_devout);

    g_object_set(audio_unit_client,
		 "device", audio_unit_devout,
		 NULL);

    /* increment n-soundcards */
    g_rec_mutex_lock(audio_unit_server_mutex);

    audio_unit_server->n_soundcards += 1;

    g_rec_mutex_unlock(audio_unit_server_mutex);
  }else{
    audio_unit_devin = ags_audio_unit_devin_new(application_context);
    soundcard = (GObject *) audio_unit_devin;

    str = g_strdup_printf("ags-audio-unit-devin-%d",
			  n_soundcards);
    
    g_object_set(AGS_AUDIO_UNIT_DEVIN(audio_unit_devin),
		 "audio-unit-client", audio_unit_client,
		 "device", str,
		 NULL);
    g_free(str);
        
    /* register ports */      
    audio_unit_port = ags_audio_unit_port_new((GObject *) audio_unit_client);

    str = g_strdup_printf("ags-soundcard%d",
			  n_soundcards);
    
    g_object_set(audio_unit_port,
		 "audio-unit-device", audio_unit_devin,
		 NULL);
    ags_audio_unit_client_add_port(audio_unit_client,
				   (GObject *) audio_unit_port);

    g_object_set(audio_unit_devin,
		 "audio-unit-port", audio_unit_port,
		 NULL);
    
    audio_unit_devin->port_name = (gchar **) malloc(2 * sizeof(gchar *));
    audio_unit_devin->port_name[0] = g_strdup(str);
    audio_unit_devin->port_name[1] = NULL;
    
    ags_audio_unit_port_register(audio_unit_port,
				 str,
				 TRUE, FALSE,
				 FALSE);

    ags_audio_unit_devin_realloc_buffer(audio_unit_devin);

    g_object_set(audio_unit_client,
		 "device", audio_unit_devin,
		 NULL);

    /* increment n-soundcards */
    g_rec_mutex_lock(audio_unit_server_mutex);

    audio_unit_server->n_soundcards += 1;

    g_rec_mutex_unlock(audio_unit_server_mutex);
  }
  
  return(soundcard);
}

void
ags_audio_unit_server_unregister_soundcard(AgsSoundServer *sound_server,
					   GObject *soundcard)
{
  AgsAudioUnitServer *audio_unit_server;
  AgsAudioUnitClient *audio_unit_client;
  AgsAudioUnitClient *default_client;
  AgsAudioUnitClient *input_client;

  GList *list_start, *list;
  GList *port;
  
  GRecMutex *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  /* get audio unit server mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);
  
  /* the default client */
  g_object_get(audio_unit_server,
	       "default-audio-unit-client", &default_client,
	       "input-audio-unit-client", &input_client,
	       NULL);

  audio_unit_client = default_client;
  
  if(AGS_IS_AUDIO_UNIT_DEVIN(soundcard) &&
     input_client != NULL){
    audio_unit_client = input_client;
  }

  if(audio_unit_client == NULL){
    g_warning("GSequencer - no audio unit client");
    
    return;
  }
  
  if(AGS_IS_AUDIO_UNIT_DEVOUT(soundcard)){
    g_object_get(soundcard,
		 "audio-unit-port", &list_start,
		 NULL);

    list = list_start;

    while(list != NULL){
      ags_audio_unit_port_unregister(list->data);
      ags_audio_unit_client_remove_port(audio_unit_client,
					list->data);
    
      list = list->next;
    }

    g_list_free_full(list_start,
		     g_object_unref);
  }else if(AGS_IS_AUDIO_UNIT_DEVIN(soundcard)){
    g_object_get(soundcard,
		 "audio-unit-port", &list_start,
		 NULL);

    list = list_start;

    while(list != NULL){
      ags_audio_unit_port_unregister(list->data);
      ags_audio_unit_client_remove_port(audio_unit_client,
					list->data);
    
      list = list->next;
    }

    g_list_free_full(list_start,
		     g_object_unref);
  }
  
  ags_audio_unit_client_remove_device(audio_unit_client,
				      soundcard);
  
  g_object_get(audio_unit_client,
	       "port", &port,
	       NULL);

  if(port == NULL){
    /* reset n-soundcards */
    g_rec_mutex_lock(audio_unit_server_mutex);

    audio_unit_server->n_soundcards = 0;

    g_rec_mutex_unlock(audio_unit_server_mutex);
  }

  g_list_free_full(port,
		   g_object_unref);
}

GObject*
ags_audio_unit_server_register_sequencer(AgsSoundServer *sound_server,
					 gboolean is_output)
{
  g_message("GSequencer - can't register audio-unit sequencer");
  
  return(NULL);
}

void
ags_audio_unit_server_unregister_sequencer(AgsSoundServer *sound_server,
					   GObject *sequencer)
{
  g_message("GSequencer - can't unregister audio-unit sequencer");
}

/**
 * ags_audio_unit_server_register_default_soundcard:
 * @audio_unit_server: the #AgsAudioUnitServer
 * 
 * Register default soundcard.
 * 
 * Returns: the instantiated #AgsAudioUnitDevout
 * 
 * Since: 3.0.0
 */
GObject*
ags_audio_unit_server_register_default_soundcard(AgsAudioUnitServer *audio_unit_server)
{
  AgsAudioUnitClient *default_client;
  AgsAudioUnitDevout *audio_unit_devout;
  AgsAudioUnitPort *audio_unit_port;

  AgsApplicationContext *application_context;

  gchar *str;
  
  guint n_soundcards;
  guint i;
  
  GRecMutex *audio_unit_server_mutex;
  GRecMutex *audio_unit_client_mutex;

  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server)){
    return(NULL);
  }

  application_context = ags_application_context_get_instance();

  /* get audio unit server mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);

  /* get some fields */
  g_rec_mutex_lock(audio_unit_server_mutex);

  default_client = (AgsAudioUnitClient *) audio_unit_server->default_client;

  n_soundcards = audio_unit_server->n_soundcards;
  
  g_rec_mutex_unlock(audio_unit_server_mutex);
  
  /* the default client */
  g_object_get(audio_unit_server,
	       "default-audio-unit-client", &default_client,
	       NULL);

  /* the default client */
  if(default_client == NULL){
    default_client = ags_audio_unit_client_new((GObject *) audio_unit_server);
    
    g_object_set(audio_unit_server,
		 "default-audio-unit-client", default_client,
		 NULL);
    ags_audio_unit_server_add_client(audio_unit_server,
				     (GObject *) default_client);
    
    ags_audio_unit_client_open((AgsAudioUnitClient *) audio_unit_server->default_client,
			       "ags-default-client");
  }

  /* get audio unit client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(default_client);

  /* the soundcard */
  audio_unit_devout = ags_audio_unit_devout_new(application_context);
  g_object_set(AGS_AUDIO_UNIT_DEVOUT(audio_unit_devout),
	       "audio-unit-client", default_client,
	       "device", "ags-default-devout",
	       NULL);
  
  /* register ports */
  audio_unit_port = ags_audio_unit_port_new((GObject *) default_client);
  g_object_set(audio_unit_port,
	       "audio-unit-device", audio_unit_devout,
	       NULL);
  ags_audio_unit_client_add_port(default_client,
				 (GObject *) audio_unit_port);

  g_object_set(audio_unit_devout,
	       "audio-unit-port", audio_unit_port,
	       NULL);

  str = g_strdup_printf("ags-default-soundcard");

#ifdef AGS_DEBUG
  g_message("%s", str);
#endif    
  
  audio_unit_devout->port_name = (gchar **) malloc(2 * sizeof(gchar *));
  audio_unit_devout->port_name[0] = g_strdup(str);
  audio_unit_devout->port_name[1] = NULL;
  
  ags_audio_unit_port_register(audio_unit_port,
			       str,
			       TRUE, FALSE,
			       TRUE);

  g_free(str);

  g_object_set(default_client,
	       "device", audio_unit_devout,
	       NULL);
  
  return((GObject *) audio_unit_devout);
}

/**
 * ags_audio_unit_server_find_url:
 * @audio_unit_server: (element-type AgsAudio.AudioUnitServer) (transfer none): the #GList-struct containing #AgsAudioUnitServer
 * @url: the url to find
 *
 * Find #AgsAudioUnitServer by url.
 *
 * Returns: (element-type AgsAudio.AudioUnitServer) (transfer none): the next matching #GList-struct containing a #AgsAudioUnitServer matching @url or %NULL
 *
 * Since: 3.0.0
 */
GList*
ags_audio_unit_server_find_url(GList *audio_unit_server,
			       gchar *url)
{
  GList *retval;
  
  GRecMutex *audio_unit_server_mutex;

  retval = NULL;
  
  while(audio_unit_server != NULL){
    /* get audio unit server mutex */
    audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server->data);

    /* check URL */
    g_rec_mutex_lock(audio_unit_server_mutex);
    
    if(!g_ascii_strcasecmp(AGS_AUDIO_UNIT_SERVER(audio_unit_server->data)->url,
			   url)){
      retval = audio_unit_server;

      g_rec_mutex_unlock(audio_unit_server_mutex);
    
      break;
    }

    g_rec_mutex_unlock(audio_unit_server_mutex);
    
    audio_unit_server = audio_unit_server->next;
  }

  return(retval);
}

/**
 * ags_audio_unit_server_find_client:
 * @audio_unit_server: the #AgsAudioUnitServer
 * @client_uuid: the uuid to find
 *
 * Find #AgsAudioUnitClient by uuid.
 *
 * Returns: (transfer none): the #AgsAudioUnitClient found or %NULL
 *
 * Since: 3.0.0
 */
GObject*
ags_audio_unit_server_find_client(AgsAudioUnitServer *audio_unit_server,
				  gchar *client_uuid)
{
  AgsAudioUnitClient *retval;
  
  GList *start_list, *list;

  GRecMutex *audio_unit_client_mutex;

  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server)){
    return(NULL);
  }

  /* get some fields */
  g_object_get(audio_unit_server,
	       "audio-unit-client", &start_list,
	       NULL);

  list = start_list;
  
  retval = NULL;
  
  while(list != NULL){
    /* get audio unit client mutex */
    audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(list->data);

    /* check client UUID */
    g_rec_mutex_lock(audio_unit_client_mutex);
    
    if(!g_ascii_strcasecmp(AGS_AUDIO_UNIT_CLIENT(list->data)->client_uuid,
			   client_uuid)){
      retval = list->data;

      g_rec_mutex_unlock(audio_unit_client_mutex);

      break;
    }

    g_rec_mutex_unlock(audio_unit_client_mutex);
    
    list = list->next;
  }

  g_list_free_full(start_list,
		   g_object_unref);
  
  return((GObject *) retval);
}

/**
 * ags_audio_unit_server_find_port:
 * @audio_unit_server: the #AgsAudioUnitServer
 * @port_uuid: the uuid to find
 *
 * Find #AgsAudioUnitPort by uuid.
 *
 * Returns: (transfer none): the #AgsAudioUnitPort found or %NULL
 *
 * Since: 3.0.0
 */
GObject*
ags_audio_unit_server_find_port(AgsAudioUnitServer *audio_unit_server,
				gchar *port_uuid)
{
  GList *client_start, *client;
  GList *port_start, *port;

  gboolean success;
  
  GRecMutex *audio_unit_port_mutex;

  g_object_get(audio_unit_server,
	       "audio-unit-client", &client_start,
	       NULL);

  client = client_start;
  
  while(client != NULL){
    g_object_get(audio_unit_server,
		 "audio-unit-port", &port_start,
		 NULL);

    port = port_start;
    
    while(port != NULL){
      /* get audio unit port mutex */
      audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(port->data);
      
      /* check port UUID */
      g_rec_mutex_lock(audio_unit_port_mutex);
      
      success = (!g_ascii_strcasecmp(AGS_AUDIO_UNIT_PORT(port->data)->port_uuid,
				     port_uuid)) ? TRUE: FALSE;

      g_rec_mutex_unlock(audio_unit_port_mutex);
      
      if(success){
	AgsAudioUnitPort *retval;

	retval = port->data;
	
	g_list_free(client_start);
	g_list_free(port_start);
	
	return(retval);
      }

      /* iterate */
      port = port->next;
    }

    g_list_free(port_start);

    /* iterate */
    client = client->next;
  }

  g_list_free(client_start);
  
  return(NULL);
}

/**
 * ags_audio_unit_server_add_client:
 * @audio_unit_server: the #AgsAudioUnitServer
 * @audio_unit_client: the #AgsAudioUnitClient to add
 *
 * Add @audio_unit_client to @audio_unit_server
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_server_add_client(AgsAudioUnitServer *audio_unit_server,
				 GObject *audio_unit_client)
{
  GRecMutex *audio_unit_server_mutex;

  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server) ||
     !AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client)){
    return;
  }

  /* get audio unit server mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);

  /* get some fields */
  g_rec_mutex_lock(audio_unit_server_mutex);

  if(g_list_find(audio_unit_server->client, audio_unit_client) == NULL){
    g_object_ref(audio_unit_client);
    audio_unit_server->client = g_list_prepend(audio_unit_server->client,
					 audio_unit_client);
  }

  g_rec_mutex_unlock(audio_unit_server_mutex);
}

/**
 * ags_audio_unit_server_remove_client:
 * @audio_unit_server: the #AgsAudioUnitServer
 * @audio_unit_client: the #AgsAudioUnitClient to remove
 *
 * Remove @audio_unit_client to @audio_unit_server
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_server_remove_client(AgsAudioUnitServer *audio_unit_server,
				    GObject *audio_unit_client)
{
  GRecMutex *audio_unit_server_mutex;

  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server) ||
     !AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client)){
    return;
  }

  /* get audio unit server mutex */
  audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER_GET_OBJ_MUTEX(audio_unit_server);

  /* get some fields */
  g_rec_mutex_lock(audio_unit_server_mutex);

  if(g_list_find(audio_unit_server->client, audio_unit_client) != NULL){
    audio_unit_server->client = g_list_remove(audio_unit_server->client,
					audio_unit_client);
    g_object_unref(audio_unit_client);
  }

  g_rec_mutex_unlock(audio_unit_server_mutex);
}

/**
 * ags_audio_unit_server_connect_client:
 * @audio_unit_server: the #AgsAudioUnitServer
 *
 * Connect all clients.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_server_connect_client(AgsAudioUnitServer *audio_unit_server)
{
  GList *client_start, *client;

  gchar *client_name;

  GRecMutex *audio_unit_client_mutex;
  
  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server)){
    return;
  }

  g_object_get(audio_unit_server,
	       "audio-unit-client", &client_start,
	       NULL);
  
  client = client_start;

  while(client != NULL){
    /* get audio unit client mutex */
    audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(client->data);

    /* client name */
    g_rec_mutex_lock(audio_unit_client_mutex);

    client_name = g_strdup(AGS_AUDIO_UNIT_CLIENT(client->data)->client_name);
    
    g_rec_mutex_unlock(audio_unit_client_mutex);

    /* open */
    ags_audio_unit_client_open((AgsAudioUnitClient *) client->data,
			       client_name);
    ags_audio_unit_client_activate(client->data);

    g_free(client_name);
    
    /* iterate */
    client = client->next;
  }

  g_list_free(client_start);
}

/**
 * ags_audio_unit_server_new:
 * @url: the URL as string
 *
 * Create a new instance of #AgsAudioUnitServer.
 *
 * Returns: the new #AgsAudioUnitServer
 *
 * Since: 3.0.0
 */
AgsAudioUnitServer*
ags_audio_unit_server_new(gchar *url)
{
  AgsAudioUnitServer *audio_unit_server;

  audio_unit_server = (AgsAudioUnitServer *) g_object_new(AGS_TYPE_AUDIO_UNIT_SERVER,
							  "url", url,
							  NULL);

  return(audio_unit_server);
}
