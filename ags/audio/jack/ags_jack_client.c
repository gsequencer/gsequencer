/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_port.h>
#include <ags/audio/jack/ags_jack_devout.h>
#include <ags/audio/jack/ags_jack_midiin.h>

#include <ags/audio/thread/ags_audio_loop.h>

void ags_jack_client_class_init(AgsJackClientClass *jack_client);
void ags_jack_client_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_jack_client_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager);
void ags_jack_client_init(AgsJackClient *jack_client);
void ags_jack_client_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_jack_client_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_jack_client_connect(AgsConnectable *connectable);
void ags_jack_client_disconnect(AgsConnectable *connectable);
void ags_jack_client_finalize(GObject *gobject);

int ags_jack_client_process_callback(jack_nframes_t nframes, void *ptr);

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
};

static gpointer ags_jack_client_parent_class = NULL;

GType
ags_jack_client_get_type()
{
  static GType ags_type_jack_client = 0;

  if(!ags_type_jack_client){
    static const GTypeInfo ags_jack_client_info = {
      sizeof (AgsJackClientClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_client_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsJackClient),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_jack_client_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_jack_client_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_jack_client = g_type_register_static(G_TYPE_OBJECT,
						  "AgsJackClient\0",
						  &ags_jack_client_info,
						  0);

    g_type_add_interface_static(ags_type_jack_client,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_jack_client);
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

  gobject->finalize = ags_jack_client_finalize;

  /* properties */
  /**
   * AgsJackClient:jack-server:
   *
   * The assigned #AgsJackServer.
   * 
   * Since: 0.7.1
   */
  param_spec = g_param_spec_object("jack-server\0",
				   "assigned JACK server\0",
				   "The assigned JACK server.\0",
				   AGS_TYPE_JACK_SERVER,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_JACK_SERVER,
				  param_spec);
}

void
ags_jack_client_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_jack_client_connect;
  connectable->disconnect = ags_jack_client_disconnect;
}

void
ags_jack_client_init(AgsJackClient *jack_client)
{
  jack_client->flags = 0;

  jack_client->jack_server = NULL;
  
  jack_client->uuid = NULL;

  jack_client->client = NULL;

  jack_client->device = NULL;
  jack_client->port = NULL;
}

void
ags_jack_client_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackClient *jack_client;

  jack_client = AGS_JACK_CLIENT(gobject);

  switch(prop_id){
  case PROP_JACK_SERVER:
    {
      AgsJackServer *jack_server;

      jack_server = (AgsJackServer *) g_value_get_object(value);

      if(jack_server == jack_client->jack_server){
	return;
      }

      if(jack_client->jack_server != NULL){
	g_object_unref(jack_client->jack_server);
      }

      if(jack_server != NULL){
	g_object_ref(jack_server);
      }
      
      jack_client->jack_server = jack_server;
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

  jack_client = AGS_JACK_CLIENT(gobject);
  
  switch(prop_id){
  case PROP_JACK_SERVER:
    {
      g_value_set_object(value, jack_client->jack_server);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_client_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_jack_client_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_jack_client_finalize(GObject *gobject)
{
  AgsJackClient *jack_client;

  jack_client = AGS_JACK_CLIENT(gobject);

  G_OBJECT_CLASS(ags_jack_client_parent_class)->finalize(gobject);
}

/**
 * ags_jack_client_find:
 * @jack_client: a #GList
 * @client_name: the client name to find
 *
 * Finds next match of @client_name in @jack_client.
 *
 * Returns: a #GList or %NULL
 * 
 * Since: 0.7.3
 */
GList*
ags_jack_client_find(GList *jack_client,
		     gchar *client_name)
{ 
  while(jack_client != NULL){
    if(!g_ascii_strcasecmp(jack_get_client_name(AGS_JACK_CLIENT(jack_client->data)->client),
			   client_name)){
      return(jack_client);
    }
  }

  return(NULL);
}

/**
 * ags_jack_client_open:
 * @jack_client: the #AgsJackClient
 * @client_name: the client's name
 *
 * Open the JACK client's connection and read uuid.
 *
 * Since: 0.7.2
 */
void
ags_jack_client_open(AgsJackClient *jack_client,
		     gchar *client_name)
{
  if(jack_client == NULL ||
     client_name == NULL){
    return;
  }
  
  jack_client->name = g_strdup(client_name);
  jack_client->client = jack_client_open(jack_client->name,
					 0,
					 NULL);
  
  if(jack_client->client != NULL){
    jack_client->uuid = jack_get_uuid_for_client_name(jack_client->client,
						      jack_client->name);
  }

  jack_set_process_callback(jack_client->client,
			    ags_jack_client_process_callback,
			    jack_client);

  g_message("Advanced Gtk+ Sequencer open JACK client\0");
}

/**
 * ags_jack_client_activate:
 * @jack_client: the #AgsJackClient
 *
 * Activate client.
 *
 * Since: 0.7.2
 */
void
ags_jack_client_activate(AgsJackClient *jack_client)
{
  GList *port;
  
  int ret;
  
  ret = jack_activate(jack_client->client);

  if(ret == 0){
    jack_client->flags |= AGS_JACK_CLIENT_ACTIVATED;
  }

  port = jack_client->port;

  while(port != NULL){
    ags_jack_port_register(port->data,
			   AGS_JACK_PORT(port->data)->name,
			   (((AGS_JACK_PORT_IS_AUDIO & (AGS_JACK_PORT(port->data)->flags)) != 0) ? TRUE: FALSE), (((AGS_JACK_PORT_IS_MIDI & (AGS_JACK_PORT(port->data)->flags)) != 0) ? TRUE: FALSE),
			   (((AGS_JACK_PORT_IS_OUTPUT & (AGS_JACK_PORT(port->data)->flags)) != 0) ? TRUE: FALSE));
    
    port = port->next;
  }
}

/**
 * ags_jack_client_deactivate:
 * @jack_client: the #AgsJackClient
 *
 * Deactivate client.
 *
 * Since: 0.7.2
 */
void
ags_jack_client_deactivate(AgsJackClient *jack_client)
{
  jack_deactivate(jack_client->client);

  jack_client->flags &= (~AGS_JACK_CLIENT_ACTIVATED);
}

/**
 * ags_jack_client_add_port:
 * @jack_client: the #AgsJackClient
 * @jack_port: an #AgsJackPort
 *
 * Add @jack_port to @jack_client.
 *
 * Since: 0.7.2
 */
void
ags_jack_client_add_port(AgsJackClient *jack_client,
			 GObject *jack_port)
{
  if(!AGS_IS_JACK_CLIENT(jack_client) ||
     !AGS_IS_JACK_PORT(jack_port)){
    return;
  }

  g_object_ref(jack_port);
  jack_client->port = g_list_prepend(jack_client->port,
				     jack_port);
}

/**
 * ags_jack_client_remove_port:
 * @jack_client: the #AgsJackClient
 * @jack_port: an #AgsJackPort
 *
 * Remove @jack_port from @jack_client.
 *
 * Since: 0.7.2
 */
void
ags_jack_client_remove_port(AgsJackClient *jack_client,
			    GObject *jack_port)
{
  if(!AGS_IS_JACK_CLIENT(jack_client)){
    return;
  }
  
  jack_client->port = g_list_remove(jack_client->port,
				    jack_port);
  g_object_unref(jack_port);
}

int
ags_jack_client_process_callback(jack_nframes_t nframes, void *ptr)
{
  AgsJackClient *jack_client;
  AgsJackPort *jack_port;
  AgsJackDevout *jack_devout;

  AgsAudioLoop *audio_loop;

  AgsMutexManager *mutex_manager;
  
  AgsApplicationContext *application_context;

  jack_client_t *client;
  jack_default_audio_sample_t *out;

  GList *device, *port;
  
  guint time_spent;
  guint copy_mode;
  guint word_size;
  guint i, j;
  gboolean no_event;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutex_t *device_mutex;
  
  if(ptr == NULL){
    return(0);
  }
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  /*  */  
  pthread_mutex_lock(application_mutex);
  
  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) ptr);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  pthread_mutex_lock(mutex);
  
  jack_client = AGS_JACK_CLIENT(ptr);

  if(jack_client->jack_server != NULL){
    application_context = AGS_JACK_SERVER(jack_client->jack_server)->application_context;
  }
  
  pthread_mutex_unlock(mutex);
  
  /*  */  
  pthread_mutex_lock(application_mutex);
    
  audio_loop = application_context->main_loop;
  
  pthread_mutex_unlock(application_mutex);
  
  /* retrieve word size */
  while(device != NULL){
    /*  */  
    pthread_mutex_lock(application_mutex);
  
    device_mutex = ags_mutex_manager_lookup(mutex_manager,
					    (GObject *) device->data);
  
    pthread_mutex_unlock(application_mutex);

    pthread_mutex_lock(device_mutex);
    
    if(AGS_IS_JACK_DEVOUT(device->data)){
      jack_devout = device->data;

      /* wait callback */
      if((AGS_JACK_DEVOUT_PASS_THROUGH & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0){
	no_event = FALSE;

	pthread_mutex_unlock(device_mutex);
    
	/* interrupt GUI */
	if(audio_loop != NULL){
	  pthread_mutex_lock(audio_loop->timing_mutex);
  
	  g_atomic_int_set(&(audio_loop->time_spent),
			   audio_loop->time_cycle);
  
	  pthread_mutex_unlock(audio_loop->timing_mutex);
  
	  ags_main_loop_interrupt(AGS_MAIN_LOOP(audio_loop),
				  AGS_THREAD_SUSPEND_SIG,
				  0, &time_spent);
	}
    
	/* give back computing time until ready */
	pthread_mutex_lock(jack_devout->callback_mutex);
    
	if((AGS_JACK_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0){
	  g_atomic_int_or(&(jack_devout->sync_flags),
			  AGS_JACK_DEVOUT_CALLBACK_WAIT);
    
	  while((AGS_JACK_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0 &&
		(AGS_JACK_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(jack_devout->sync_flags)))) != 0){
	    pthread_cond_wait(jack_devout->callback_cond,
			      jack_devout->callback_mutex);
	  }
	}
    
	g_atomic_int_and(&(jack_devout->sync_flags),
			 (~(AGS_JACK_DEVOUT_CALLBACK_WAIT |
			    AGS_JACK_DEVOUT_CALLBACK_DONE)));
    
	pthread_mutex_unlock(jack_devout->callback_mutex);

	pthread_mutex_lock(device_mutex);
      }else{
	no_event = TRUE;
      }

      /* get buffer */
      if((AGS_JACK_DEVOUT_BUFFER0 & (jack_devout->flags)) != 0){
	j = 0;
      }else if((AGS_JACK_DEVOUT_BUFFER1 & (jack_devout->flags)) != 0){
	j = 1;
      }else if((AGS_JACK_DEVOUT_BUFFER2 & (jack_devout->flags)) != 0){
	j = 2;
      }else if((AGS_JACK_DEVOUT_BUFFER3 & jack_devout->flags) != 0){
	j = 3;
      }

      /* get copy mode */
      copy_mode = ags_audio_buffer_util_get_copy_mode(AGS_AUDIO_BUFFER_UTIL_FLOAT,
						      ags_audio_buffer_util_format_from_soundcard(jack_devout->format));

      /* check buffer flag */
      switch(jack_devout->format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
	{
	  word_size = sizeof(signed char);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
	{
	  word_size = sizeof(signed short);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
	{
	  word_size = sizeof(signed long);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
	{
	  word_size = sizeof(signed long);
	}
	break;
      case AGS_SOUNDCARD_SIGNED_64_BIT:
	{
	  word_size = sizeof(signed long long);
	}
	break;
      default:
	pthread_mutex_unlock(mutex);

	g_warning("ags_jack_devout_process_callback(): unsupported word size\0");
	return(0);
      }
      
      port = jack_devout->jack_port;
      
      for(i = 0; port != NULL; i++){
	jack_port = port->data;
	
	out = jack_port_get_buffer(jack_port->port,
				   nframes);
  
	if(!no_event){
	  ags_audio_buffer_util_clear_float(out, jack_devout->pcm_channels,
					    nframes);

	  ags_audio_buffer_util_copy_buffer_to_buffer(out, 1, 0,
						      jack_devout->buffer[j], jack_devout->pcm_channels, i,
						      nframes, copy_mode);
	}

	port = port->next;
      }
    
      if(!no_event){
	if((AGS_JACK_DEVOUT_BUFFER0 & (jack_devout->flags)) != 0){
	  memset(jack_devout->buffer[2], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
	}else if((AGS_JACK_DEVOUT_BUFFER1 & (jack_devout->flags)) != 0){
	  memset(jack_devout->buffer[3], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
	}else if((AGS_JACK_DEVOUT_BUFFER2 & (jack_devout->flags)) != 0){
	  memset(jack_devout->buffer[0], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
	}else if((AGS_JACK_DEVOUT_BUFFER3 & jack_devout->flags) != 0){
	  memset(jack_devout->buffer[1], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);

	  /* tic */
	  ags_soundcard_tic(AGS_SOUNDCARD(jack_devout));
	  
	  /* reset - switch buffer flags */
	  ags_jack_devout_switch_buffer_flag(jack_devout);  
	}	
      }
    }

    pthread_mutex_unlock(device_mutex);
    
    device = device->next;
  }
  
  pthread_mutex_unlock(mutex);


  return(0);
}

/**
 * ags_jack_client_new:
 * @jack_server: the assigned #AgsJackServer
 *
 * Instantiate a new #AgsJackClient.
 *
 * Returns: the new #AgsJackClient
 *
 * Since: 0.7.1
 */
AgsJackClient*
ags_jack_client_new(GObject *jack_server)
{
  AgsJackClient *jack_client;

  jack_client = (AgsJackClient *) g_object_new(AGS_TYPE_JACK_CLIENT,
					       "jack-server\0", jack_server,
					       NULL);

  return(jack_client);
}
