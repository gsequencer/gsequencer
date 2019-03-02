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

#include <ags/libags.h>

#include <ags/audio/audio-unit/ags_audio_unit_devout.h>
#include <ags/audio/audio-unit/ags_audio_unit_devin.h>
#include <ags/audio/audio-unit/ags_audio_unit_midiin.h>

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
  PROP_APPLICATION_CONTEXT,
  PROP_URL,
  PROP_DEFAULT_SOUNDCARD,
  PROP_DEFAULT_AUDIO_UNIT_CLIENT,
  PROP_INPUT_AUDIO_UNIT_CLIENT,
  PROP_AUDIO_UNIT_CLIENT,
};

static gpointer ags_audio_unit_server_parent_class = NULL;

static pthread_mutex_t ags_audio_unit_server_class_mutex = PTHREAD_MUTEX_INITIALIZER;

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
   * AgsAudioUnitServer:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 2.2.0
   */
  param_spec = g_param_spec_object("application-context",
				   i18n_pspec("the application context object"),
				   i18n_pspec("The application context object"),
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /**
   * AgsAudioUnitServer:url:
   *
   * The assigned URL.
   * 
   * Since: 2.2.0
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
   * Since: 2.2.0
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
   * Since: 2.2.0
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
   * AgsAudio-UnitServer:input-audio-unit-client:
   *
   * The input audio unit client.
   * 
   * Since: 2.2.0
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
   * AgsAudioUnitServer:audio-unit-client:
   *
   * The audio unit client list.
   * 
   * Since: 2.2.0
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
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* flags */
  audio_unit_server->flags = 0;

  /* server mutex */
  audio_unit_server->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  audio_unit_server->obj_mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);
  
  g_atomic_int_set(&(audio_unit_server->running),
		   TRUE);
  audio_unit_server->thread = (pthread_t *) malloc(sizeof(pthread_t));

  /* parent */  
  audio_unit_server->application_context = NULL;

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

  pthread_mutex_t *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(gobject);

  /* get audio-unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      pthread_mutex_lock(audio_unit_server_mutex);

      if(audio_unit_server->application_context == application_context){
	pthread_mutex_unlock(audio_unit_server_mutex);

	return;
      }

      if(audio_unit_server->application_context != NULL){
	g_object_unref(G_OBJECT(audio_unit_server->application_context));
      }

      if(application_context != NULL){
	g_object_ref(G_OBJECT(application_context));
      }

      audio_unit_server->application_context = application_context;

      pthread_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_URL:
    {
      gchar *url;

      url = g_value_get_string(value);

      pthread_mutex_lock(audio_unit_server_mutex);

      if(audio_unit_server->url == url){
	pthread_mutex_unlock(audio_unit_server_mutex);

	return;
      }

      if(audio_unit_server->url != NULL){
	g_free(audio_unit_server->url);
      }

      audio_unit_server->url = g_strdup(url);

      pthread_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_DEFAULT_SOUNDCARD:
    {
      GObject *default_soundcard;

      default_soundcard = (GObject *) g_value_get_object(value);

      pthread_mutex_lock(audio_unit_server_mutex);

      if(audio_unit_server->default_soundcard == (GObject *) default_soundcard){
	pthread_mutex_unlock(audio_unit_server_mutex);

	return;
      }

      if(audio_unit_server->default_soundcard != NULL){
	g_object_unref(G_OBJECT(audio_unit_server->default_soundcard));
      }

      if(default_soundcard != NULL){
	g_object_ref(G_OBJECT(default_soundcard));
      }

      audio_unit_server->default_soundcard = (GObject *) default_soundcard;

      pthread_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_DEFAULT_AUDIO_UNIT_CLIENT:
    {
      AgsAudioUnitClient *default_client;

      default_client = (AgsAudioUnitClient *) g_value_get_object(value);

      pthread_mutex_lock(audio_unit_server_mutex);

      if(audio_unit_server->default_client == (GObject *) default_client){
	pthread_mutex_unlock(audio_unit_server_mutex);

	return;
      }

      if(audio_unit_server->default_client != NULL){
	g_object_unref(G_OBJECT(audio_unit_server->default_client));
      }

      if(default_client != NULL){
	g_object_ref(G_OBJECT(default_client));
      }

      audio_unit_server->default_client = (GObject *) default_client;

      pthread_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_INPUT_AUDIO_UNIT_CLIENT:
    {
      AgsAudioUnitClient *input_client;

      input_client = (AgsAudioUnitClient *) g_value_get_object(value);

      pthread_mutex_lock(audio_unit_server_mutex);

      if(audio_unit_server->input_client == (GObject *) input_client){
	pthread_mutex_unlock(audio_unit_server_mutex);

	return;
      }

      if(audio_unit_server->input_client != NULL){
	g_object_unref(G_OBJECT(audio_unit_server->input_client));
      }

      if(input_client != NULL){
	g_object_ref(G_OBJECT(input_client));
      }

      audio_unit_server->input_client = (GObject *) input_client;

      pthread_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_AUDIO_UNIT_CLIENT:
    {
      GObject *client;

      client = (GObject *) g_value_get_pointer(value);

      pthread_mutex_lock(audio_unit_server_mutex);

      if(!AGS_IS_AUDIO_UNIT_CLIENT(client) ||
	 g_list_find(audio_unit_server->client, client) != NULL){
	pthread_mutex_unlock(audio_unit_server_mutex);

	return;
      }

      g_object_ref(G_OBJECT(client));
      audio_unit_server->client = g_list_prepend(audio_unit_server->client,
						 client);

      pthread_mutex_unlock(audio_unit_server_mutex);
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

  pthread_mutex_t *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(gobject);

  /* get audio-unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      pthread_mutex_lock(audio_unit_server_mutex);

      g_value_set_object(value, audio_unit_server->application_context);

      pthread_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_URL:
    {
      pthread_mutex_lock(audio_unit_server_mutex);

      g_value_set_string(value, audio_unit_server->url);

      pthread_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_DEFAULT_SOUNDCARD:
    {
      pthread_mutex_lock(audio_unit_server_mutex);

      g_value_set_object(value, audio_unit_server->default_soundcard);

      pthread_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_DEFAULT_AUDIO_UNIT_CLIENT:
    {
      pthread_mutex_lock(audio_unit_server_mutex);

      g_value_set_object(value, audio_unit_server->default_client);

      pthread_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_INPUT_AUDIO_UNIT_CLIENT:
    {
      pthread_mutex_lock(audio_unit_server_mutex);

      g_value_set_object(value, audio_unit_server->input_client);

      pthread_mutex_unlock(audio_unit_server_mutex);
    }
    break;
  case PROP_AUDIO_UNIT_CLIENT:
    {
      pthread_mutex_lock(audio_unit_server_mutex);

      g_value_set_pointer(value,
			  g_list_copy(audio_unit_server->client));

      pthread_mutex_unlock(audio_unit_server_mutex);
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

  /* application context */
  if(audio_unit_server->application_context != NULL){
    g_object_unref(G_OBJECT(audio_unit_server->application_context));
    
    audio_unit_server->application_context = NULL;
  }

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

  pthread_mutex_destroy(audio_unit_server->obj_mutex);
  free(audio_unit_server->obj_mutex);

  pthread_mutexattr_destroy(audio_unit_server->obj_mutexattr);
  free(audio_unit_server->obj_mutexattr);
  
  /* application context */
  if(audio_unit_server->application_context != NULL){
    g_object_unref(G_OBJECT(audio_unit_server->application_context));
  }

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

  pthread_mutex_t *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(connectable);

  /* get audio unit server signal mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(audio_unit_server_mutex);

  ptr = audio_unit_server->uuid;

  pthread_mutex_unlock(audio_unit_server_mutex);
  
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

  pthread_mutex_t *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(connectable);

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* check is added */
  pthread_mutex_lock(audio_unit_server_mutex);

  is_ready = (((AGS_AUDIO_UNIT_SERVER_ADDED_TO_REGISTRY & (audio_unit_server->flags)) != 0) ? TRUE: FALSE);

  pthread_mutex_unlock(audio_unit_server_mutex);
  
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

  pthread_mutex_t *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(connectable);

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(audio_unit_server_mutex);

  is_connected = (((AGS_AUDIO_UNIT_SERVER_CONNECTED & (audio_unit_server->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(audio_unit_server_mutex);
  
  return(is_connected);
}

void
ags_audio_unit_server_connect(AgsConnectable *connectable)
{
  AgsAudioUnitServer *audio_unit_server;

  GList *list_start, *list;  

  pthread_mutex_t *audio_unit_server_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(connectable);

  ags_audio_unit_server_set_flags(audio_unit_server, AGS_AUDIO_UNIT_SERVER_CONNECTED);

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  list =
    list_start = g_list_copy(audio_unit_server->client);

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
}

void
ags_audio_unit_server_disconnect(AgsConnectable *connectable)
{
  AgsAudioUnitServer *audio_unit_server;

  GList *list_start, *list;

  pthread_mutex_t *audio_unit_server_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(connectable);
  
  ags_audio_unit_server_unset_flags(audio_unit_server, AGS_AUDIO_UNIT_SERVER_CONNECTED);

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* client */
  list =
    list_start = g_list_copy(audio_unit_server->client);

  while(list != NULL){
    ags_connectable_disconnect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }

  g_list_free(list_start);
}

/**
 * ags_audio_unit_server_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.2.0
 */
pthread_mutex_t*
ags_audio_unit_server_get_class_mutex()
{
  return(&ags_audio_unit_server_class_mutex);
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
 * Since: 2.2.0
 */
gboolean
ags_audio_unit_server_test_flags(AgsAudioUnitServer *audio_unit_server, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *audio_unit_server_mutex;

  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server)){
    return(FALSE);
  }

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* test */
  pthread_mutex_lock(audio_unit_server_mutex);

  retval = (flags & (audio_unit_server->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(audio_unit_server_mutex);

  return(retval);
}

/**
 * ags_audio_unit_server_set_flags:
 * @audio_unit_server: the #AgsAudioUnitServer
 * @flags: see #AgsAudioUnitServerFlags-enum
 *
 * Enable a feature of @audio_unit_server.
 *
 * Since: 2.2.0
 */
void
ags_audio_unit_server_set_flags(AgsAudioUnitServer *audio_unit_server, guint flags)
{
  pthread_mutex_t *audio_unit_server_mutex;

  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server)){
    return;
  }

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(audio_unit_server_mutex);

  audio_unit_server->flags |= flags;
  
  pthread_mutex_unlock(audio_unit_server_mutex);
}
    
/**
 * ags_audio_unit_server_unset_flags:
 * @audio_unit_server: the #AgsAudioUnitServer
 * @flags: see #AgsAudioUnitServerFlags-enum
 *
 * Disable a feature of @audio_unit_server.
 *
 * Since: 2.2.0
 */
void
ags_audio_unit_server_unset_flags(AgsAudioUnitServer *audio_unit_server, guint flags)
{  
  pthread_mutex_t *audio_unit_server_mutex;

  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server)){
    return;
  }

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(audio_unit_server_mutex);

  audio_unit_server->flags &= (~flags);
  
  pthread_mutex_unlock(audio_unit_server_mutex);
}

void
ags_audio_unit_server_set_url(AgsSoundServer *sound_server,
			      gchar *url)
{
  AgsAudioUnitServer *audio_unit_server;

  pthread_mutex_t *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* set URL */
  pthread_mutex_lock(audio_unit_server_mutex);

  audio_unit_server->url = g_strdup(url);

  pthread_mutex_unlock(audio_unit_server_mutex);
}

gchar*
ags_audio_unit_server_get_url(AgsSoundServer *sound_server)
{
  AgsAudioUnitServer *audio_unit_server;

  gchar *url;

  pthread_mutex_t *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* set URL */
  pthread_mutex_lock(audio_unit_server_mutex);

  url = audio_unit_server->url;

  pthread_mutex_unlock(audio_unit_server_mutex);
  
  return(url);
}


void
ags_audio_unit_server_set_ports(AgsSoundServer *sound_server,
				guint *port, guint port_count)
{
  AgsAudioUnitServer *audio_unit_server;

  pthread_mutex_t *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* set ports */
  pthread_mutex_lock(audio_unit_server_mutex);

  audio_unit_server->port = port;
  audio_unit_server->port_count = port_count;

  pthread_mutex_unlock(audio_unit_server_mutex);
}

guint*
ags_audio_unit_server_get_ports(AgsSoundServer *sound_server,
				guint *port_count)
{
  AgsAudioUnitServer *audio_unit_server;

  guint *port;
  
  pthread_mutex_t *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* get ports */
  pthread_mutex_lock(audio_unit_server_mutex);

  if(port_count != NULL){
    *port_count = AGS_AUDIO_UNIT_SERVER(sound_server)->port_count;
  }

  port = audio_unit_server->port;

  pthread_mutex_unlock(audio_unit_server_mutex);
  
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

#ifdef AGS_WITH_AUDIO_UNIT
  AUGraph *graph;
#else
  gpointer graph;
#endif

  gchar *str;  

  guint n_soundcards;
  gboolean initial_set;
  guint i;

  pthread_mutex_t *audio_unit_server_mutex;
  pthread_mutex_t *audio_unit_client_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* the default client */
  initial_set = FALSE;
  
  /* get some fields */
  pthread_mutex_lock(audio_unit_server_mutex);

  application_context= audio_unit_server->application_context;

  jack_client = 
    default_client = (AgsAudioUnitClient *) audio_unit_server->default_client;
  input_client = (AgsAudioUnitClient *) audio_unit_server->input_client;

  n_soundcards = audio_unit_server->n_soundcards;
  
  pthread_mutex_unlock(audio_unit_server_mutex);

  if(!is_output &&
     input_client != NULL){
    jack_client = input_client;
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
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_client_mutex = audio_unit_client->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* get graph */
  pthread_mutex_lock(audio_unit_client_mutex);

  graph = audio_unit_client->graph;

  pthread_mutex_unlock(audio_unit_client_mutex);

  if(graph == NULL){
    g_warning("ags_audio_unit_server.c - can't open audio unit client");
  }

  soundcard = NULL;

  /* the soundcard */
  if(is_output){
    audio_unit_devout = ags_audio_unit_devout_new(audio_unit_server->application_context);
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
    pthread_mutex_lock(audio_unit_server_mutex);

    audio_unit_server->n_soundcards += 1;

    pthread_mutex_unlock(audio_unit_server_mutex);
  }else{
    audio_unit_devin = ags_audio_unit_devin_new(audio_unit_server->application_context);
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
    pthread_mutex_lock(audio_unit_server_mutex);

    audio_unit_server->n_soundcards += 1;

    pthread_mutex_unlock(audio_unit_server_mutex);
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

#ifdef AGS_WITH_AUDIO_UNIT
  AUGraph *graph;
#else
  gpointer graph;
#endif

  GList *list_start, *list;
  GList *port;
  
  pthread_mutex_t *audio_unit_server_mutex;

  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());
  
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

    g_list_free(list_start);
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

    g_list_free(list_start);
  }
  
  ags_audio_unit_client_remove_device(audio_unit_client,
				      soundcard);
  
  g_object_get(audio_unit_client,
	       "port", &port,
	       NULL);

  if(port == NULL){
    /* reset n-soundcards */
    pthread_mutex_lock(audio_unit_server_mutex);

    audio_unit_server->n_soundcards = 0;

    pthread_mutex_unlock(audio_unit_server_mutex);
  }

  g_list_free(port);
}

GObject*
ags_audio_unit_server_register_sequencer(AgsSoundServer *sound_server,
					 gboolean is_output)
{
  AgsAudioUnitServer *audio_unit_server;
  AgsAudioUnitClient *audio_unit_client;
  AgsAudioUnitClient *default_client;
  AgsAudioUnitClient *input_client;
  AgsAudioUnitPort *audio_unit_port;
  AgsAudioUnitMidiin *audio_unit_midiin;

  AgsApplicationContext *application_context;

#ifdef AGS_WITH_AUDIO_UNIT
  AUGraph *graph;
#else
  gpointer graph;
#endif

  gchar *str;

  guint n_sequencers;

  pthread_mutex_t *audio_unit_server_mutex;
  pthread_mutex_t *audio_unit_client_mutex;
  
  if(is_output){
    g_warning("GSequencer - MIDI output not implemented");
    return(NULL);
  }
  
  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  /* get audio-unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_unit_server_mutex);

  application_context= audio_unit_server->application_context;

  audio_unit_client = 
    default_client = (AgsAudioUnitClient *) audio_unit_server->default_client;
  input_client = (AgsAudioUnitClient *) audio_unit_server->input_client;
 
  n_sequencers = audio_unit_server->n_sequencers;
  
  pthread_mutex_unlock(audio_unit_server_mutex);

  if(!is_output &&
     input_client != NULL){
    audio_unit_client = input_client;
  }
  
#ifdef AGS_WITH_AUDIO_UNIT
  if(n_sequencers >= MIDIGetNumberOfDestinations()){
    return(NULL);
  }
#endif
  
  /* the audio unit client */
  if(audio_unit_client == NULL){
    audio_unit_client = ags_audio_unit_client_new((GObject *) audio_unit_server);
    g_object_set(audio_unit_server,
		 "default-audio-unit-client", audio_unit_client,
		 NULL);
    ags_audio_unit_server_add_client(audio_unit_server,
				     (GObject *) audio_unit_client);
    
    ags_audio_unit_client_open((AgsAudioUnitClient *) audio_unit_client,
			       "ags-default-client");    
  }

  /* get graph */
  pthread_mutex_lock(audio_unit_client_mutex);

  graph = audio_unit_client->graph;

  pthread_mutex_unlock(audio_unit_client_mutex);

  if(graph == NULL){
    g_warning("ags_audio_unit_server.c - can't open audio unit client");
  }

  audio_unit_midiin = ags_audio_unit_midiin_new(audio_unit_server->application_context);

  str = g_strdup_printf("ags-audio-unit-midiin-%d",
			n_sequencers);

  g_object_set(AGS_AUDIO_UNIT_MIDIIN(audio_unit_midiin),
	       "audio-unit-client", audio_unit_client,
	       "device", str,
	       NULL);

  g_free(str);
  
  /* register sequencer */  
  audio_unit_port = ags_audio_unit_port_new((GObject *) audio_unit_client);
  audio_unit_port->midi_port_number = n_sequencers;
  g_object_set(audio_unit_port,
	       "audio-unit-device", audio_unit_midiin,
	       NULL);
  ags_audio_unit_client_add_port(audio_unit_client,
				 (GObject *) audio_unit_port);

  g_object_set(audio_unit_midiin,
	       "audio-unit-port", audio_unit_port,
	       NULL);

  str = g_strdup_printf("ags-sequencer%d",
			n_sequencers);

#ifdef AGS_DEBUG
  g_message("%s", str);
#endif

  audio_unit_midiin->port_name = (gchar **) malloc(2 * sizeof(gchar *));
  audio_unit_midiin->port_name[0] = g_strdup(str);
  audio_unit_midiin->port_name[1] = NULL;
  
  ags_audio_unit_port_register(audio_unit_port,
			       str,
			       FALSE, TRUE,
			       FALSE);

  g_object_set(audio_unit_client,
	       "device", audio_unit_midiin,
	       NULL);

  /* increment n-sequencers */
  pthread_mutex_lock(audio_unit_server_mutex);

  audio_unit_server->n_sequencers += 1;

  pthread_mutex_unlock(audio_unit_server_mutex);
  
  return((GObject *) audio_unit_midiin);
}

void
ags_audio_unit_server_unregister_sequencer(AgsSoundServer *sound_server,
					   GObject *sequencer)
{
  AgsAudioUnitServer *audio_unit_server;
  AgsAudioUnitClient *audio_unit_client;
  AgsAudioUnitClient *default_client;
  AgsAudioUnitClient *input_client;

  GList *list_start, *list;
  GList *port;

  pthread_mutex_t *audio_unit_server_mutex;
  
  audio_unit_server = AGS_AUDIO_UNIT_SERVER(sound_server);

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());
  
  /* the default client */
  g_object_get(audio_unit_server,
	       "default-audio-unit-client", &default_client,
	       "input-audio-unit-client", &input_client,
	       NULL);

  audio_unit_client = default_client;
  
  if(AGS_IS_AUDIO_UNIT_MIDIIN(sequencer) &&
     input_client != NULL){
    audio_unit_client = input_client;
  }
  
  if(audio_unit_client == NULL){
    g_warning("GSequencer - no audio unit client");
    
    return;
  }

  g_object_get(sequencer,
	       "audio-unit-port", &list_start,
	       NULL);

  list = list_start;

  while(list != NULL){
    ags_audio_unit_port_unregister(list->data);
    ags_audio_unit_client_remove_port(audio_unit_client,
				      list->data);
    

    list = list->next;
  }

  g_list_free(list_start);

  ags_audio_unit_client_remove_device(audio_unit_client,
				      sequencer);
  
  g_object_get(audio_unit_client,
	       "port", &port,
	       NULL);

  if(port == NULL){
    /* reset n-sequencers */
    pthread_mutex_lock(audio_unit_server_mutex);

    audio_unit_server->n_sequencers = 0;

    pthread_mutex_unlock(audio_unit_server_mutex);
  }

  g_list_free(port);
}

/**
 * ags_audio_unit_server_register_default_soundcard:
 * @audio_unit_server: the #AgsAudioUnitServer
 * 
 * Register default soundcard.
 * 
 * Returns: the instantiated #AgsAudioUnitDevout
 * 
 * Since: 2.2.0
 */
GObject*
ags_audio_unit_server_register_default_soundcard(AgsAudioUnitServer *audio_unit_server)
{
  AgsAudioUnitClient *default_client;
  AgsAudioUnitDevout *audio_unit_devout;
  AgsAudioUnitPort *audio_unit_port;

  AgsApplicationContext *application_context;

#ifdef AGS_WITH_AUDIO_UNIT
  AUGraph *graph;
#else
  gpointer graph;
#endif

  gchar *str;
  
  guint n_soundcards;
  guint i;
  
  pthread_mutex_t *audio_unit_server_mutex;
  pthread_mutex_t *audio_unit_client_mutex;

  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server)){
    return(NULL);
  }

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_unit_server_mutex);

  application_context = audio_unit_server->application_context;

  default_client = (AgsAudioUnitClient *) audio_unit_server->default_client;

  n_soundcards = audio_unit_server->n_soundcards;
  
  pthread_mutex_unlock(audio_unit_server_mutex);
  
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
  pthread_mutex_lock(ags_audio_unit_client_get_class_mutex());
  
  audio_unit_client_mutex = default_client->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_client_get_class_mutex());

  /* get graph */
  pthread_mutex_lock(audio_unit_client_mutex);

  graph = default_client->graph;

  pthread_mutex_unlock(audio_unit_client_mutex);

  if(graph == NULL){
    g_warning("ags_audio_unit_server.c - can't open audio unit client");
  }

  /* the soundcard */
  audio_unit_devout = ags_audio_unit_devout_new(audio_unit_server->application_context);
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
 * @audio_unit_server: the #GList-struct containing #AgsAudioUnitServer
 * @url: the url to find
 *
 * Find #AgsAudioUnitServer by url.
 *
 * Returns: the next matching #GList-struct containing a #AgsAudioUnitServer matching @url or %NULL
 *
 * Since: 2.2.0
 */
GList*
ags_audio_unit_server_find_url(GList *audio_unit_server,
			       gchar *url)
{
  GList *retval;
  
  pthread_mutex_t *audio_unit_server_mutex;

  retval = NULL;
  
  while(audio_unit_server != NULL){
    /* get audio unit server mutex */
    pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
    audio_unit_server_mutex = AGS_AUDIO_UNIT_SERVER(audio_unit_server->data)->obj_mutex;
  
    pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

    /* check URL */
    pthread_mutex_lock(audio_unit_server_mutex);
    
    if(!g_ascii_strcasecmp(AGS_AUDIO_UNIT_SERVER(audio_unit_server->data)->url,
			   url)){
      retval = audio_unit_server;

      pthread_mutex_unlock(audio_unit_server_mutex);
    
      break;
    }

    pthread_mutex_unlock(audio_unit_server_mutex);
    
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
 * Returns: the #AgsAudioUnitClient found or %NULL
 *
 * Since: 2.2.0
 */
GObject*
ags_audio_unit_server_find_client(AgsAudioUnitServer *audio_unit_server,
				  gchar *client_uuid)
{
  AgsAudioUnitClient *retval;
  
  GList *list_start, *list;

  pthread_mutex_t *audio_unit_server_mutex;
  pthread_mutex_t *audio_unit_client_mutex;

  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server)){
    return(NULL);
  }

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_unit_server_mutex);

  list =
    list_start = g_list_copy(audio_unit_server->client);

  pthread_mutex_unlock(audio_unit_server_mutex);

  retval = NULL;
  
  while(list != NULL){
    /* get audio unit client mutex */
    pthread_mutex_lock(ags_audio_unit_client_get_class_mutex());
  
    audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT(list->data)->obj_mutex;
  
    pthread_mutex_unlock(ags_audio_unit_client_get_class_mutex());

    /* check client UUID */
    pthread_mutex_lock(audio_unit_client_mutex);
    
    if(!g_ascii_strcasecmp(AGS_AUDIO_UNIT_CLIENT(list->data)->client_uuid,
			   client_uuid)){
      retval = list->data;

      pthread_mutex_unlock(audio_unit_client_mutex);

      break;
    }

    pthread_mutex_unlock(audio_unit_client_mutex);
    
    list = list->next;
  }

  g_list_free(list_start);
  
  return((GObject *) retval);
}

/**
 * ags_audio_unit_server_find_port:
 * @audio_unit_server: the #AgsAudioUnitServer
 * @port_uuid: the uuid to find
 *
 * Find #AgsAudioUnitPort by uuid.
 *
 * Returns: the #AgsAudioUnitPort found or %NULL
 *
 * Since: 2.2.0
 */
GObject*
ags_audio_unit_server_find_port(AgsAudioUnitServer *audio_unit_server,
				gchar *port_uuid)
{
  GList *client_start, *client;
  GList *port_start, *port;

  gboolean success;
  
  pthread_mutex_t *audio_unit_port_mutex;

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
      pthread_mutex_lock(ags_audio_unit_port_get_class_mutex());
  
      audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT(port->data)->obj_mutex;
  
      pthread_mutex_unlock(ags_audio_unit_port_get_class_mutex());
      
      /* check port UUID */
      pthread_mutex_lock(audio_unit_port_mutex);
      
      success = (!g_ascii_strcasecmp(AGS_AUDIO_UNIT_PORT(port->data)->port_uuid,
				     port_uuid)) ? TRUE: FALSE;

      pthread_mutex_unlock(audio_unit_port_mutex);
      
      if(success){
	g_list_free(client_start);
	g_list_free(port_start);
	
	return(port->data);
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
 * Since: 2.2.0
 */
void
ags_audio_unit_server_add_client(AgsAudioUnitServer *audio_unit_server,
				 GObject *audio_unit_client)
{
  pthread_mutex_t *audio_unit_server_mutex;

  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server) ||
     !AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client)){
    return;
  }

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_unit_server_mutex);

  if(g_list_find(audio_unit_server->client, audio_unit_client) == NULL){
    g_object_ref(audio_unit_client);
    audio_unit_server->client = g_list_prepend(audio_unit_server->client,
					 audio_unit_client);
  }

  pthread_mutex_unlock(audio_unit_server_mutex);
}

/**
 * ags_audio_unit_server_remove_client:
 * @audio_unit_server: the #AgsAudioUnitServer
 * @audio_unit_client: the #AgsAudioUnitClient to remove
 *
 * Remove @audio_unit_client to @audio_unit_server
 *
 * Since: 2.2.0
 */
void
ags_audio_unit_server_remove_client(AgsAudioUnitServer *audio_unit_server,
				    GObject *audio_unit_client)
{
  pthread_mutex_t *audio_unit_server_mutex;

  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server) ||
     !AGS_IS_AUDIO_UNIT_CLIENT(audio_unit_client)){
    return;
  }

  /* get audio unit server mutex */
  pthread_mutex_lock(ags_audio_unit_server_get_class_mutex());
  
  audio_unit_server_mutex = audio_unit_server->obj_mutex;
  
  pthread_mutex_unlock(ags_audio_unit_server_get_class_mutex());

  /* get some fields */
  pthread_mutex_lock(audio_unit_server_mutex);

  if(g_list_find(audio_unit_server->client, audio_unit_client) != NULL){
    audio_unit_server->client = g_list_remove(audio_unit_server->client,
					audio_unit_client);
    g_object_unref(audio_unit_client);
  }

  pthread_mutex_unlock(audio_unit_server_mutex);
}

/**
 * ags_audio_unit_server_connect_client:
 * @audio_unit_server: the #AgsAudioUnitServer
 *
 * Connect all clients.
 *
 * Since: 2.2.0
 */
void
ags_audio_unit_server_connect_client(AgsAudioUnitServer *audio_unit_server)
{
  GList *client_start, *client;

  gchar *client_name;

  pthread_mutex_t *audio_unit_client_mutex;
  
  if(!AGS_IS_AUDIO_UNIT_SERVER(audio_unit_server)){
    return;
  }

  g_object_get(audio_unit_server,
	       "audio-unit-client", &client_start,
	       NULL);
  
  client = client_start;

  while(client != NULL){
    /* get audio unit client mutex */
    pthread_mutex_lock(ags_audio_unit_client_get_class_mutex());
  
    audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT(client->data)->obj_mutex;
  
    pthread_mutex_unlock(ags_audio_unit_client_get_class_mutex());

    /* client name */
    pthread_mutex_lock(audio_unit_client_mutex);

    client_name = g_strdup(client_name);
    
    pthread_mutex_unlock(audio_unit_client_mutex);

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
 * @application_context: the #AgsApplicationContext
 * @url: the URL as string
 *
 * Create a new instance of #AgsAudioUnitServer.
 *
 * Returns: the new #AgsAudioUnitServer
 *
 * Since: 2.2.0
 */
AgsAudioUnitServer*
ags_audio_unit_server_new(AgsApplicationContext *application_context,
			  gchar *url)
{
  AgsAudioUnitServer *audio_unit_server;

  audio_unit_server = (AgsAudioUnitServer *) g_object_new(AGS_TYPE_AUDIO_UNIT_SERVER,
							  "application-context", application_context,
							  "url", url,
							  NULL);

  return(audio_unit_server);
}
