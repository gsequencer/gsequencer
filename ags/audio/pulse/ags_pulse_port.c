/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/audio/pulse/ags_pulse_port.h>

#include <ags/util/ags_id_generator.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_distributed_manager.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_sequencer.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_polling_thread.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/task/ags_notify_soundcard.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <ags/audio/pulse/ags_pulse_server.h>
#include <ags/audio/pulse/ags_pulse_client.h>
#include <ags/audio/pulse/ags_pulse_devout.h>
#include <ags/audio/pulse/ags_pulse_devin.h>

#include <ags/config.h>
#include <ags/i18n.h>

#include <time.h>

void ags_pulse_port_class_init(AgsPulsePortClass *pulse_port);
void ags_pulse_port_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pulse_port_distributed_manager_interface_init(AgsDistributedManagerInterface *distributed_manager);
void ags_pulse_port_init(AgsPulsePort *pulse_port);
void ags_pulse_port_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_pulse_port_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_pulse_port_connect(AgsConnectable *connectable);
void ags_pulse_port_disconnect(AgsConnectable *connectable);
void ags_pulse_port_dispose(GObject *gobject);
void ags_pulse_port_finalize(GObject *gobject);

#ifdef AGS_WITH_PULSE
void ags_pulse_port_stream_request_callback(pa_stream *stream, size_t length, AgsPulsePort *pulse_port);
void ags_pulse_port_stream_underflow_callback(pa_stream *stream, AgsPulsePort *pulse_port);
#endif

/**
 * SECTION:ags_pulse_port
 * @short_description: pulseaudio resource.
 * @title: AgsPulsePort
 * @section_id:
 * @include: ags/audio/pulse/ags_pulse_port.h
 *
 * The #AgsPulsePort represents either a pulseaudio sequencer or soundcard to communicate
 * with.
 */

enum{
  PROP_0,
  PROP_PULSE_CLIENT,
  PROP_PULSE_DEVOUT,
  PROP_PULSE_DEVIN,
  PROP_PORT_NAME,
};

static gpointer ags_pulse_port_parent_class = NULL;

const int ags_pulse_port_endian_i = 1;
#define is_bigendian() ( (*(char*)&ags_pulse_port_endian_i) == 0 )

GType
ags_pulse_port_get_type()
{
  static GType ags_type_pulse_port = 0;

  if(!ags_type_pulse_port){
    static const GTypeInfo ags_pulse_port_info = {
      sizeof (AgsPulsePortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pulse_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPulsePort),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pulse_port_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pulse_port_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_pulse_port = g_type_register_static(G_TYPE_OBJECT,
						 "AgsPulsePort",
						 &ags_pulse_port_info,
						 0);

    g_type_add_interface_static(ags_type_pulse_port,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return (ags_type_pulse_port);
}

void
ags_pulse_port_class_init(AgsPulsePortClass *pulse_port)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_pulse_port_parent_class = g_type_class_peek_parent(pulse_port);

  /* GObjectClass */
  gobject = (GObjectClass *) pulse_port;

  gobject->set_property = ags_pulse_port_set_property;
  gobject->get_property = ags_pulse_port_get_property;

  gobject->dispose = ags_pulse_port_dispose;
  gobject->finalize = ags_pulse_port_finalize;

  /* properties */
  /**
   * AgsPulsePort:pulse-client:
   *
   * The assigned #AgsPulseClient.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("pulse-client",
				   i18n_pspec("assigned pulseaudio client"),
				   i18n_pspec("The assigned pulseaudio client"),
				   AGS_TYPE_PULSE_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PULSE_CLIENT,
				  param_spec);

  /**
   * AgsPulsePort:pulse-devout:
   *
   * The assigned #AgsPulseDevout.
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("pulse-devout",
				   i18n_pspec("assigned pulseaudio devout"),
				   i18n_pspec("The assigned pulseaudio devout"),
				   AGS_TYPE_PULSE_DEVOUT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PULSE_DEVOUT,
				  param_spec);

  /**
   * AgsPulsePort:pulse-devin:
   *
   * The assigned #AgsPulseDevout.
   * 
   * Since: 1.2.0
   */
  param_spec = g_param_spec_object("pulse-devin",
				   i18n_pspec("assigned pulseaudio devin"),
				   i18n_pspec("The assigned pulseaudio devin"),
				   AGS_TYPE_PULSE_DEVIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PULSE_DEVIN,
				  param_spec);

  /**
   * AgsPulsePort:port-name:
   *
   * The pulse soundcard indentifier
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_string("port-name",
				   i18n_pspec("port name"),
				   i18n_pspec("The port name"),
				   "hw:0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT_NAME,
				  param_spec);
}

void
ags_pulse_port_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_pulse_port_connect;
  connectable->disconnect = ags_pulse_port_disconnect;
}

void
ags_pulse_port_init(AgsPulsePort *pulse_port)
{
  AgsMutexManager *mutex_manager;

  AgsConfig *config;

  gchar *str;

  guint samplerate;
  guint pcm_channels;
  guint buffer_size;
  guint format;
  guint word_size;
  guint fixed_size;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert port mutex */
  pulse_port->mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  pulse_port->mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) pulse_port,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  pulse_port->flags = 0;

  /*  */
  pulse_port->pulse_client = NULL;
  pulse_port->pulse_devout = NULL;
  pulse_port->pulse_devin = NULL;
  
  pulse_port->uuid = ags_id_generator_create_uuid();
  pulse_port->name = NULL;

  pulse_port->stream = NULL;
  
  /* read config */
  config = ags_config_get_instance();
  
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  pcm_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  format = AGS_SOUNDCARD_DEFAULT_FORMAT;

  /* pcm channels */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "pcm-channels");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "pcm-channels");
  }
  
  if(str != NULL){
    pcm_channels = g_ascii_strtoull(str,
				    NULL,
				    10);
    g_free(str);
  }

  /* samplerate */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "samplerate");
  }
  
  if(str != NULL){
    samplerate = g_ascii_strtoull(str,
				  NULL,
				  10);
    free(str);
  }

  /* buffer size */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "buffer-size");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "buffer-size");
  }
  
  if(str != NULL){
    buffer_size = g_ascii_strtoull(str,
				   NULL,
				   10);
    free(str);
  }

  /* format */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "format");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "format");
  }
  
  if(str != NULL){
    format = g_ascii_strtoull(str,
			      NULL,
			      10);
    free(str);
  }
  
  pulse_port->buffer_size = buffer_size;
  pulse_port->format = format;
  pulse_port->pcm_channels = pcm_channels;

#ifdef AGS_WITH_PULSE
  pulse_port->sample_spec = (pa_sample_spec *) malloc(sizeof(pa_sample_spec));
  pulse_port->sample_spec->rate = samplerate;
  pulse_port->sample_spec->channels = pcm_channels;
#else
  pulse_port->sample_spec = NULL;
#endif

  switch(format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
#ifdef AGS_WITH_PULSE
      if(is_bigendian()){
	pulse_port->sample_spec->format = PA_SAMPLE_S16BE;
      }else{
	pulse_port->sample_spec->format = PA_SAMPLE_S16LE;
      }
#endif

      word_size = sizeof(signed short);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
#ifdef AGS_WITH_PULSE
      if(is_bigendian()){
	pulse_port->sample_spec->format = PA_SAMPLE_S24_32BE;
      }else{
	pulse_port->sample_spec->format = PA_SAMPLE_S24_32LE;
      }
#endif

      word_size = sizeof(signed long);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
#ifdef AGS_WITH_PULSE
      if(is_bigendian()){
	pulse_port->sample_spec->format = PA_SAMPLE_S32BE;
      }else{
	pulse_port->sample_spec->format = PA_SAMPLE_S32LE;
      }
#endif

      word_size = sizeof(signed long);
    }
    break;
  default:
    g_warning("pulse devout/devin - unsupported format");
  }
  
  fixed_size = pcm_channels * buffer_size * word_size;

#ifdef AGS_WITH_PULSE
  pulse_port->buffer_attr = (pa_buffer_attr *) malloc(sizeof(pa_buffer_attr));
  pulse_port->buffer_attr->fragsize = (uint32_t) -1;
  pulse_port->buffer_attr->maxlength = (uint32_t) -1;
  pulse_port->buffer_attr->minreq = (uint32_t) fixed_size;
  pulse_port->buffer_attr->prebuf = (uint32_t) 0;
  pulse_port->buffer_attr->tlength = (uint32_t) fixed_size;
#else
  pulse_port->buffer_attr = NULL;
#endif

  pulse_port->empty_buffer = ags_stream_alloc(8 * pcm_channels * buffer_size,
					      AGS_SOUNDCARD_DEFAULT_FORMAT);

  g_atomic_int_set(&(pulse_port->is_empty),
		   TRUE);
  g_atomic_int_set(&(pulse_port->underflow),
		   0);
  g_atomic_int_set(&(pulse_port->restart),
		   FALSE);
  
  pulse_port->nth_empty_buffer = 0;
  
  g_atomic_int_set(&(pulse_port->queued),
		   0);
}

void
ags_pulse_port_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsPulsePort *pulse_port;

  pulse_port = AGS_PULSE_PORT(gobject);

  switch(prop_id){
  case PROP_PULSE_CLIENT:
    {
      AgsPulseClient *pulse_client;

      pulse_client = (AgsPulseClient *) g_value_get_object(value);

      if(pulse_port->pulse_client == (GObject *) pulse_client){
	return;
      }

      if(pulse_port->pulse_client != NULL){
	g_object_unref(pulse_port->pulse_client);
      }

      if(pulse_client != NULL){
	g_object_ref(pulse_client);
      }
      
      pulse_port->pulse_client = (GObject *) pulse_client;
    }
    break;
  case PROP_PULSE_DEVOUT:
    {
      AgsPulseDevout *pulse_devout;

      pulse_devout = (AgsPulseDevout *) g_value_get_object(value);

      if(pulse_port->pulse_devout == (GObject *) pulse_devout){
	return;
      }

      if(pulse_port->pulse_devout != NULL){
	g_object_unref(pulse_port->pulse_devout);
      }

      if(pulse_devout != NULL){
	g_object_ref(pulse_devout);
      }
      
      pulse_port->pulse_devout = (GObject *) pulse_devout;
    }
    break;
  case PROP_PULSE_DEVIN:
    {
      AgsPulseDevin *pulse_devin;

      pulse_devin = (AgsPulseDevin *) g_value_get_object(value);

      if(pulse_port->pulse_devin == (GObject *) pulse_devin){
	return;
      }

      if(pulse_port->pulse_devin != NULL){
	g_object_unref(pulse_port->pulse_devin);
      }

      if(pulse_devin != NULL){
	g_object_ref(pulse_devin);
      }
      
      pulse_port->pulse_devin = (GObject *) pulse_devin;
    }
    break;
  case PROP_PORT_NAME:
    {
      gchar *port_name;

      port_name = g_value_get_string(value);

      if(pulse_port->name == port_name ||
	 !g_ascii_strcasecmp(pulse_port->name,
			     port_name)){
	return;
      }

      if(pulse_port->name != NULL){
	g_free(pulse_port->name);
      }

      pulse_port->name = port_name;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_port_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsPulsePort *pulse_port;

  pulse_port = AGS_PULSE_PORT(gobject);
  
  switch(prop_id){
  case PROP_PULSE_CLIENT:
    {
      g_value_set_object(value, pulse_port->pulse_client);
    }
    break;
  case PROP_PULSE_DEVOUT:
    {
      g_value_set_object(value, pulse_port->pulse_devout);
    }
    break;
  case PROP_PULSE_DEVIN:
    {
      g_value_set_object(value, pulse_port->pulse_devin);
    }
    break;
  case PROP_PORT_NAME:
    {
      g_value_set_string(value, pulse_port->name);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_port_connect(AgsConnectable *connectable)
{
  AgsPulsePort *pulse_port;

  pulse_port = AGS_PULSE_PORT(connectable);

  if((AGS_PULSE_PORT_CONNECTED & (pulse_port->flags)) != 0){
    return;
  }

  pulse_port->flags |= AGS_PULSE_PORT_CONNECTED;
}

void
ags_pulse_port_disconnect(AgsConnectable *connectable)
{
  AgsPulsePort *pulse_port;

  pulse_port = AGS_PULSE_PORT(connectable);

  if((AGS_PULSE_PORT_CONNECTED & (pulse_port->flags)) == 0){
    return;
  }

  pulse_port->flags &= (~AGS_PULSE_PORT_CONNECTED);
}

void
ags_pulse_port_dispose(GObject *gobject)
{
  AgsPulsePort *pulse_port;

  pulse_port = AGS_PULSE_PORT(gobject);

  /* pulse client */
  if(pulse_port->pulse_client != NULL){
    g_object_unref(pulse_port->pulse_client);

    pulse_port->pulse_client = NULL;
  }

  /* pulse devout */
  if(pulse_port->pulse_devout != NULL){
    g_object_unref(pulse_port->pulse_devout);

    pulse_port->pulse_devout = NULL;
  }

  /* pulse devin */
  if(pulse_port->pulse_devin != NULL){
    g_object_unref(pulse_port->pulse_devin);

    pulse_port->pulse_devin = NULL;
  }

  /* name */
  g_free(pulse_port->name);

  /* call parent */
  G_OBJECT_CLASS(ags_pulse_port_parent_class)->dispose(gobject);
}

void
ags_pulse_port_finalize(GObject *gobject)
{
  AgsPulsePort *pulse_port;

  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;

  pulse_port = AGS_PULSE_PORT(gobject);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* pulse client */
  if(pulse_port->pulse_client != NULL){
    g_object_unref(pulse_port->pulse_client);
  }

  /* pulse devout */
  if(pulse_port->pulse_devout != NULL){
    g_object_unref(pulse_port->pulse_devout);
  }

  /* pulse devin */
  if(pulse_port->pulse_devin != NULL){
    g_object_unref(pulse_port->pulse_devin);
  }

  /* name */
  g_free(pulse_port->name);

  if(pulse_port->sample_spec != NULL){
    free(pulse_port->sample_spec);
  }
  
  if(pulse_port->buffer_attr != NULL){
    free(pulse_port->buffer_attr);
  }

  pthread_mutex_destroy(pulse_port->mutex);
  free(pulse_port->mutex);

  pthread_mutexattr_destroy(pulse_port->mutexattr);
  free(pulse_port->mutexattr);

  /* call parent */
  G_OBJECT_CLASS(ags_pulse_port_parent_class)->finalize(gobject);
}

/**
 * ags_pulse_port_find:
 * @pulse_port: a #GList
 * @port_name: the port name to find
 *
 * Finds next match of @port_name in @pulse_port.
 *
 * Returns: a #GList or %NULL
 * 
 * Since: 1.0.0
 */
GList*
ags_pulse_port_find(GList *pulse_port,
		    gchar *port_name)
{
  while(pulse_port != NULL){
    if(!g_ascii_strcasecmp(AGS_PULSE_PORT(pulse_port->data)->name,
			   port_name)){
      return(pulse_port);
    }
  }

  return(NULL);
}

/**
 * ags_pulse_port_register:
 * @pulse_port: the #AgsPulsePort
 * @port_name: the name as string
 * @is_audio: if %TRUE interpreted as audio port
 * @is_midi: if %TRUE interpreted as midi port
 * @is_output: if %TRUE port is acting as output, otherwise as input
 *
 * Register a new pulseaudio port and read uuid. Creates a new AgsSequencer or AgsSoundcard
 * object.
 *
 * Since: 1.0.0
 */
void
ags_pulse_port_register(AgsPulsePort *pulse_port,
			gchar *port_name,
			gboolean is_audio, gboolean is_midi,
			gboolean is_output)
{
  GList *list;

  gchar *name, *uuid;

  int r;
  
  if(!AGS_IS_PULSE_PORT(pulse_port) ||
     port_name == NULL){
    return;
  }

  if(pulse_port->pulse_client == NULL){
    g_warning("ags_pulse_port.c - no assigned AgsPulseClient");
    
    return;
  }

  if((AGS_PULSE_PORT_REGISTERED & (pulse_port->flags)) != 0){
    return;
  }

  /* get pulse server and application context */
  if(pulse_port->pulse_client == NULL ||
     AGS_PULSE_CLIENT(pulse_port->pulse_client)->pulse_server == NULL){
    return;
  }

  uuid = pulse_port->uuid;
  name = pulse_port->name;

  if(AGS_PULSE_CLIENT(pulse_port->pulse_client)->context == NULL){
    return;
  }
  
  pulse_port->name = g_strdup(port_name);

  /* create sequencer or soundcard */
  if(is_output){
    pulse_port->flags |= AGS_PULSE_PORT_IS_OUTPUT;
  }

#ifdef AGS_WITH_PULSE
  pulse_port->stream = pa_stream_new(AGS_PULSE_CLIENT(pulse_port->pulse_client)->context, "Playback", pulse_port->sample_spec, NULL);
#else
  pulse_port->stream = NULL;
#endif

  if(pulse_port->stream == NULL){
    return;
  }

#ifdef AGS_WITH_PULSE
  if(is_audio){  
    pulse_port->flags |= AGS_PULSE_PORT_IS_AUDIO;

    pa_stream_set_write_callback(pulse_port->stream,
				 ags_pulse_port_stream_request_callback,
				 pulse_port);
    pa_stream_set_underflow_callback(pulse_port->stream,
				     ags_pulse_port_stream_underflow_callback,
				     pulse_port);
    
    r = pa_stream_connect_playback(pulse_port->stream, NULL, pulse_port->buffer_attr,
				   (PA_STREAM_INTERPOLATE_TIMING |
				    PA_STREAM_ADJUST_LATENCY |
				    PA_STREAM_AUTO_TIMING_UPDATE),
				   NULL,
				   NULL);

    if(r < 0){
      // Old pulse audio servers don't like the ADJUST_LATENCY flag, so retry without that
      r = pa_stream_connect_playback(pulse_port->stream, NULL, pulse_port->buffer_attr,
				     (PA_STREAM_INTERPOLATE_TIMING |
				      PA_STREAM_AUTO_TIMING_UPDATE),
				     NULL,
				     NULL);
    }

    if(r < 0){
      return;
    }
  }else if(is_midi){
    pulse_port->flags |= AGS_PULSE_PORT_IS_MIDI;

    //NOTE:JK: not implemented
  }
  
  if(pulse_port->stream != NULL){
    pulse_port->flags |= AGS_PULSE_PORT_REGISTERED;
  }
#endif
}

void
ags_pulse_port_unregister(AgsPulsePort *pulse_port)
{
  if(!AGS_IS_PULSE_PORT(pulse_port)){
    return;
  }

#ifdef AGS_WITH_PULSE
  if(pulse_port->stream != NULL){
    pa_stream_disconnect(pulse_port->stream);
  }
#endif
}

#ifdef AGS_WITH_PULSE
void
ags_pulse_port_stream_request_callback(pa_stream *stream, size_t length, AgsPulsePort *pulse_port)
{
  AgsPulseDevout *pulse_devout;
  AgsPulseDevin *pulse_devin;
  
  AgsAudioLoop *audio_loop;

  AgsMutexManager *mutex_manager;
  AgsTaskThread *task_thread;
  
  AgsApplicationContext *application_context;

  GObject *soundcard;
  
  guint word_size;
  size_t count;
  guint nth_buffer;
  guint nth_empty_buffer, next_nth_empty_buffer;
  size_t n_bytes;
  gint remaining;
  guint i;
  
  gboolean no_event;
  gboolean empty_run;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutex_t *device_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;
  
  if(pulse_port == NULL){
    return;
  }
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  application_context = ags_application_context_get_instance();
  
  /*  */  
  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) pulse_port);
  
  pthread_mutex_unlock(application_mutex);

  if(g_atomic_int_get(&(pulse_port->queued)) > 0){
    g_warning("drop pulseaudio callback");
    
    return;
  }else{
    g_atomic_int_inc(&(pulse_port->queued));
  }

  /*
   * process audio
   */
  /*  */  
  pthread_mutex_lock(application_mutex);

  if(application_context != NULL){
    audio_loop = (AgsAudioLoop *) application_context->main_loop;
    task_thread = (AgsTaskThread *) application_context->task_thread;
  }else{
    audio_loop = NULL;
    task_thread = NULL;
  }
  
  pthread_mutex_unlock(application_mutex);

  /* interrupt GUI */
  if(task_thread != NULL){
    pthread_mutex_lock(task_thread->launch_mutex);
  }
  
  if(audio_loop != NULL){
    pthread_mutex_lock(audio_loop->timing_mutex);
  
    g_atomic_int_set(&(audio_loop->time_spent),
		     audio_loop->time_cycle);
  
    pthread_mutex_unlock(audio_loop->timing_mutex);
  
    //    ags_main_loop_interrupt(AGS_MAIN_LOOP(audio_loop),
    //			    AGS_THREAD_SUSPEND_SIG,
    //			    0, &time_spent);
  }

  if(task_thread != NULL){
    pthread_mutex_unlock(task_thread->launch_mutex);
  }

  g_atomic_int_and(&(AGS_THREAD(audio_loop)->flags),
		   (~(AGS_THREAD_TIMING)));

  /*  */
  pthread_mutex_lock(mutex);

  pulse_devout = pulse_port->pulse_devout;
  pulse_devin = pulse_port->pulse_devin;

  soundcard = NULL;
  
  if(pulse_devout != NULL){
    soundcard = pulse_devout;
  }else if(pulse_devin != NULL){
    soundcard = pulse_devin;
  }
  
  stream = pulse_port->stream;

  nth_empty_buffer = pulse_port->nth_empty_buffer;

  if(nth_empty_buffer >= 7){
    next_nth_empty_buffer = 0;
  }else{
    next_nth_empty_buffer = nth_empty_buffer + 1;
  }
  
  pthread_mutex_unlock(mutex);
  
  /*  */  
  pthread_mutex_lock(application_mutex);
  
  device_mutex = ags_mutex_manager_lookup(mutex_manager,
					  (GObject *) soundcard);
  
  pthread_mutex_unlock(application_mutex);

  /*  */
  pthread_mutex_lock(device_mutex);

  empty_run = FALSE;

  if((pulse_devout == NULL ||
      !ags_soundcard_is_playing(AGS_SOUNDCARD(pulse_devout))) &&
     (pulse_devin == NULL ||
      !ags_soundcard_is_recording(AGS_SOUNDCARD(pulse_devin)))){
    empty_run = TRUE;
  }
  
  pthread_mutex_unlock(device_mutex);

  /* check buffer flag */
  pthread_mutex_lock(mutex);
  
  switch(pulse_port->format){
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
  }
        
  count = pulse_port->sample_spec->channels * pulse_port->buffer_size * word_size;

  pthread_mutex_unlock(mutex);

  remaining = length;

  if(empty_run){
    void *empty_buffer;
    void *next_empty_buffer;
    
    n_bytes = 0;
    empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[nth_empty_buffer * count]);
    pa_stream_begin_write(stream, &empty_buffer, &n_bytes);
  
    // remaining = n_bytes;
    remaining = length;
    
    /* iterate */
    for(i = 0; remaining > 0; i++){
      pthread_mutex_lock(mutex);

      empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[nth_empty_buffer * count]);
      next_empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[next_nth_empty_buffer * count]);

      nth_empty_buffer = next_nth_empty_buffer;
	  
      if(nth_empty_buffer >= 7){
	next_nth_empty_buffer = 0;
      }else{
	next_nth_empty_buffer = nth_empty_buffer + 1;
      }

      pulse_port->nth_empty_buffer = nth_empty_buffer;

      pa_stream_write(stream, empty_buffer, count, NULL, 0, PA_SEEK_RELATIVE);
    
      pthread_mutex_unlock(mutex);

      memset(next_empty_buffer, 0, count * sizeof(unsigned char));
    
      remaining -= count;
    }

    g_atomic_int_set(&(pulse_port->is_empty),
		     TRUE);
  }

  pthread_mutex_lock(device_mutex);

  /* wait callback */
  no_event = TRUE;

  if(pulse_devout != NULL){
    if((AGS_PULSE_DEVOUT_PASS_THROUGH & (g_atomic_int_get(&(pulse_devout->sync_flags)))) == 0){
      callback_mutex = pulse_devout->callback_mutex;

      pthread_mutex_unlock(device_mutex);
	
      /* give back computing time until ready */
      pthread_mutex_lock(callback_mutex);
    
      if((AGS_PULSE_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(pulse_devout->sync_flags)))) == 0){
	struct timespec timeout = {
	  0,
	  0,
	};

	guint latency;
      
	g_atomic_int_or(&(pulse_devout->sync_flags),
			AGS_PULSE_DEVOUT_CALLBACK_WAIT);

	pthread_mutex_lock(mutex);
      
	latency = ags_pulse_port_get_latency(pulse_port);

	pthread_mutex_unlock(mutex);
      
	latency /= 8;
      
	if(timeout.tv_nsec + latency < NSEC_PER_SEC){
	  timeout.tv_nsec += latency;
	}else{
	  timeout.tv_sec += 1;
	  timeout.tv_nsec = timeout.tv_nsec + latency - NSEC_PER_SEC;
	}
      
	while((AGS_PULSE_DEVOUT_CALLBACK_DONE & (g_atomic_int_get(&(pulse_devout->sync_flags)))) == 0 &&
	      (AGS_PULSE_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(pulse_devout->sync_flags)))) != 0){
	  void *empty_buffer;
	  void *next_empty_buffer;
	
	  pthread_cond_timedwait(pulse_devout->callback_cond,
				 callback_mutex,
				 &timeout);

	  pthread_mutex_unlock(callback_mutex);

	  if(g_atomic_int_get(&(pulse_port->underflow)) > 0){
	    n_bytes = 0;
	    empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[nth_empty_buffer * count]);
	    pa_stream_begin_write(stream, &empty_buffer, &n_bytes);	

	    remaining = length;
	  }
	
	  /* feed */
	  for(i = 0; g_atomic_int_get(&(pulse_port->underflow)) > 0 || remaining > 0; i++){
	    pthread_mutex_lock(mutex);

	    empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[nth_empty_buffer * count]);
	    next_empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[next_nth_empty_buffer * count]);

	    nth_empty_buffer = next_nth_empty_buffer;
	  
	    if(nth_empty_buffer >= 7){
	      next_nth_empty_buffer = 0;
	    }else{
	      next_nth_empty_buffer = nth_empty_buffer + 1;
	    }

	    pulse_port->nth_empty_buffer = nth_empty_buffer;

	    pa_stream_write(stream, empty_buffer, count, NULL, 0, PA_SEEK_RELATIVE);
	  
	    pthread_mutex_unlock(mutex);

	    memset(next_empty_buffer, 0, count * sizeof(unsigned char));

	    if(g_atomic_int_get(&(pulse_port->underflow)) > 0){
	      g_atomic_int_dec_and_test(&(pulse_port->underflow));
	    }
	  
	    remaining -= count;
	  }

	  pthread_mutex_lock(mutex);
	  
	  latency = ags_pulse_port_get_latency(pulse_port);

	  pthread_mutex_unlock(mutex);

	  latency /= 8;

	  clock_gettime(CLOCK_MONOTONIC, &timeout);
	  
	  if(timeout.tv_nsec + latency < NSEC_PER_SEC){
	    timeout.tv_nsec += latency;
	  }else{
	    timeout.tv_sec += 1;
	    timeout.tv_nsec = timeout.tv_nsec + latency - NSEC_PER_SEC;
	  }

	  pthread_mutex_lock(callback_mutex);
	}
      }
    
      g_atomic_int_and(&(pulse_devout->sync_flags),
		       (~(AGS_PULSE_DEVOUT_CALLBACK_WAIT |
			  AGS_PULSE_DEVOUT_CALLBACK_DONE)));
    
      pthread_mutex_unlock(callback_mutex);

      no_event = FALSE;

      pthread_mutex_lock(device_mutex);
    }

    /* get buffer */  
    if((AGS_PULSE_DEVOUT_BUFFER0 & (pulse_devout->flags)) != 0){
      nth_buffer = 7;
    }else if((AGS_PULSE_DEVOUT_BUFFER1 & (pulse_devout->flags)) != 0){
      nth_buffer = 0;
    }else if((AGS_PULSE_DEVOUT_BUFFER2 & (pulse_devout->flags)) != 0){
      nth_buffer = 1;
    }else if((AGS_PULSE_DEVOUT_BUFFER3 & (pulse_devout->flags)) != 0){
      nth_buffer = 2;
    }else if((AGS_PULSE_DEVOUT_BUFFER4 & (pulse_devout->flags)) != 0){
      nth_buffer = 3;
    }else if((AGS_PULSE_DEVOUT_BUFFER5 & (pulse_devout->flags)) != 0){
      nth_buffer = 4;
    }else if((AGS_PULSE_DEVOUT_BUFFER6 & (pulse_devout->flags)) != 0){
      nth_buffer = 5;
    }else if((AGS_PULSE_DEVOUT_BUFFER7 & (pulse_devout->flags)) != 0){
      nth_buffer = 6;
    }else{
      empty_run = TRUE;
    }
  }else if(pulse_devin != NULL){
    if((AGS_PULSE_DEVIN_PASS_THROUGH & (g_atomic_int_get(&(pulse_devin->sync_flags)))) == 0){
      callback_mutex = pulse_devin->callback_mutex;

      pthread_mutex_unlock(device_mutex);
	
      /* give back computing time until ready */
      pthread_mutex_lock(callback_mutex);
    
      if((AGS_PULSE_DEVIN_CALLBACK_DONE & (g_atomic_int_get(&(pulse_devin->sync_flags)))) == 0){
	g_atomic_int_or(&(pulse_devin->sync_flags),
			AGS_PULSE_DEVIN_CALLBACK_WAIT);
      
	while((AGS_PULSE_DEVIN_CALLBACK_DONE & (g_atomic_int_get(&(pulse_devin->sync_flags)))) == 0 &&
	      (AGS_PULSE_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(pulse_devin->sync_flags)))) != 0){
	  pthread_cond_wait(pulse_devin->callback_cond,
			    callback_mutex);
	}
	
	g_atomic_int_and(&(pulse_devin->sync_flags),
			 (~(AGS_PULSE_DEVIN_CALLBACK_WAIT |
			    AGS_PULSE_DEVIN_CALLBACK_DONE)));
	
	pthread_mutex_unlock(callback_mutex);
	
	no_event = FALSE;
	
	pthread_mutex_lock(device_mutex);
      }
    }
    
    /* get buffer */  
    if((AGS_PULSE_DEVIN_BUFFER0 & (pulse_devin->flags)) != 0){
      nth_buffer = 7;
    }else if((AGS_PULSE_DEVIN_BUFFER1 & (pulse_devin->flags)) != 0){
      nth_buffer = 0;
    }else if((AGS_PULSE_DEVIN_BUFFER2 & (pulse_devin->flags)) != 0){
      nth_buffer = 1;
    }else if((AGS_PULSE_DEVIN_BUFFER3 & (pulse_devin->flags)) != 0){
      nth_buffer = 2;
    }else if((AGS_PULSE_DEVIN_BUFFER4 & (pulse_devin->flags)) != 0){
      nth_buffer = 3;
    }else if((AGS_PULSE_DEVIN_BUFFER5 & (pulse_devin->flags)) != 0){
      nth_buffer = 4;
    }else if((AGS_PULSE_DEVIN_BUFFER6 & (pulse_devin->flags)) != 0){
      nth_buffer = 5;
    }else if((AGS_PULSE_DEVIN_BUFFER7 & (pulse_devin->flags)) != 0){
      nth_buffer = 6;
    }else{
      empty_run = TRUE;
    }
  }  

  switch(pulse_port->format){
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
  default:
    empty_run = TRUE;
  }
  
  /* write */
  count = pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size;

  if(pulse_devout != NULL){
    if(!empty_run){
      n_bytes = 0;
      pa_stream_begin_write(stream, &(pulse_devout->buffer[nth_buffer]), &n_bytes);
    
      //    g_message("%d", ags_synth_util_get_xcross_count_s16(pulse_devout->buffer[nth_buffer],
      //							pulse_devout->pcm_channels * pulse_devout->buffer_size));

      pa_stream_write(stream,
		      pulse_devout->buffer[nth_buffer],
		      count,
		      NULL,
		      0,
		      PA_SEEK_RELATIVE);

      g_atomic_int_set(&(pulse_port->is_empty),
		       FALSE);
    }

    /* signal finish */
    if(!no_event){        
      callback_finish_mutex = pulse_devout->callback_finish_mutex;
	
      pthread_mutex_lock(callback_finish_mutex);

      g_atomic_int_or(&(pulse_devout->sync_flags),
		      AGS_PULSE_DEVOUT_CALLBACK_FINISH_DONE);
    
      if((AGS_PULSE_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(pulse_devout->sync_flags)))) != 0){
	pthread_cond_signal(pulse_devout->callback_finish_cond);
      }

      pthread_mutex_unlock(callback_finish_mutex);
    }

    pthread_mutex_unlock(device_mutex);

    if(empty_run){
      void *empty_buffer;
      void *next_empty_buffer;

      empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[nth_empty_buffer * count]);
    
      n_bytes = 0;
      pa_stream_begin_write(stream, &empty_buffer, &n_bytes);
    
      remaining = length;
    
      for(i = 0; remaining > 0; i++){
	/* feed */
	pthread_mutex_lock(mutex);

	empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[nth_empty_buffer * count]);
	next_empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[next_nth_empty_buffer * count]);

	nth_empty_buffer = next_nth_empty_buffer;
      
	if(nth_empty_buffer >= 7){
	  next_nth_empty_buffer = 0;
	}else{
	  next_nth_empty_buffer = nth_empty_buffer + 1;
	}

	pulse_port->nth_empty_buffer = nth_empty_buffer;

	pa_stream_write(stream, empty_buffer, count, NULL, 0, PA_SEEK_RELATIVE);
	  
	pthread_mutex_unlock(mutex);

	memset(next_empty_buffer, 0, count * sizeof(unsigned char));

	remaining -= count;
      }
    }
  }else if(pulse_devin != NULL){
    if(!empty_run){
      pa_stream_read(stream,
		     pulse_devout->buffer[nth_buffer],
		     count,
		     NULL,
		     0,
		     PA_SEEK_RELATIVE);
      
      g_atomic_int_set(&(pulse_port->is_empty),
		       FALSE);
    }

    /* signal finish */
    if(!no_event){        
      callback_finish_mutex = pulse_devin->callback_finish_mutex;
	
      pthread_mutex_lock(callback_finish_mutex);

      g_atomic_int_or(&(pulse_devin->sync_flags),
		      AGS_PULSE_DEVIN_CALLBACK_FINISH_DONE);
    
      if((AGS_PULSE_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(pulse_devin->sync_flags)))) != 0){
	pthread_cond_signal(pulse_devin->callback_finish_cond);
      }

      pthread_mutex_unlock(callback_finish_mutex);
    }

    pthread_mutex_unlock(device_mutex);

    if(empty_run){
      void *empty_buffer;
      void *next_empty_buffer;
      
      remaining = pa_stream_readable_size(stream);
    
      for(i = 0; remaining > 0; i++){
	/* feed */
	pthread_mutex_lock(mutex);

	empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[nth_empty_buffer * count]);
	next_empty_buffer = &(((unsigned char *) pulse_port->empty_buffer)[next_nth_empty_buffer * count]);

	nth_empty_buffer = next_nth_empty_buffer;
      
	if(nth_empty_buffer >= 7){
	  next_nth_empty_buffer = 0;
	}else{
	  next_nth_empty_buffer = nth_empty_buffer + 1;
	}

	pulse_port->nth_empty_buffer = nth_empty_buffer;

	pa_stream_read(stream, empty_buffer, count, NULL, 0, PA_SEEK_RELATIVE);
	
	pthread_mutex_unlock(mutex);
	
	memset(next_empty_buffer, 0, count * sizeof(unsigned char));
	
	remaining -= count;
      }
    }
  }  
  
  g_atomic_int_dec_and_test(&(pulse_port->queued));
}

void
ags_pulse_port_stream_underflow_callback(pa_stream *stream, AgsPulsePort *pulse_port)
{
  AgsAudioLoop *audio_loop;

  AgsPollingThread *polling_thread;
  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;
  
  guint time_spent;
  
  pthread_mutex_t *application_mutex;
    
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  application_context = ags_application_context_get_instance();
  
  pthread_mutex_lock(application_mutex);

  audio_loop = (AgsAudioLoop *) application_context->main_loop;

  pthread_mutex_unlock(application_mutex);

  polling_thread = ags_thread_find_type(audio_loop,
					AGS_TYPE_POLLING_THREAD);
  
  pthread_mutex_lock(audio_loop->timing_mutex);

  g_atomic_int_set(&(audio_loop->time_spent),
		   audio_loop->time_cycle);

  pthread_mutex_unlock(audio_loop->timing_mutex);

  g_atomic_int_add(&(pulse_port->underflow),
		   1);

  if(polling_thread != NULL){
    g_atomic_int_or(&(polling_thread->flags),
		    AGS_POLLING_THREAD_OMIT);

    /* just omit three times since we don't poll pulse */
    g_atomic_int_add(&(polling_thread->omit_count),
		     4);
  }
}
#endif

guint
ags_pulse_port_get_fixed_size(AgsPulsePort *pulse_port)
{
  AgsPulseDevout *pulse_devout;
  
  AgsMutexManager *mutex_manager;
  
  guint pcm_channels;
  guint buffer_size;
  guint format;
  guint word_size;
  guint fixed_size;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *devout_mutex;

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  /* lock pulse port */
  pthread_mutex_lock(application_mutex);

  pulse_devout = pulse_port->pulse_devout;

  devout_mutex = ags_mutex_manager_lookup(mutex_manager,
					  (GObject *) pulse_devout);

  pthread_mutex_unlock(application_mutex);

  /*  */
  pthread_mutex_lock(devout_mutex);

  pcm_channels = pulse_devout->pcm_channels;
  buffer_size = pulse_devout->buffer_size;
  format = pulse_devout->format;
  
  pthread_mutex_unlock(devout_mutex);
  
  switch(format){
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
  default:
    g_warning("pulse devout - unsupported format");

    return(0);
  }

  fixed_size = pcm_channels * buffer_size * word_size;
  
  return(fixed_size);
}

void
ags_pulse_port_set_samplerate(AgsPulsePort *pulse_port,
			      guint samplerate)
{
  AgsMutexManager *mutex_manager;

  guint fixed_size;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  fixed_size = ags_pulse_port_get_fixed_size(pulse_port);
  
  /* lock pulse port */
  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) pulse_port);

  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

#ifdef AGS_WITH_PULSE
  pulse_port->sample_spec->rate = samplerate;

  pulse_port->buffer_attr->fragsize = -1;
  pulse_port->buffer_attr->maxlength = -1;
  pulse_port->buffer_attr->minreq = fixed_size;
  pulse_port->buffer_attr->tlength = fixed_size;
#endif

  pthread_mutex_unlock(mutex);
}

void
ags_pulse_port_set_buffer_size(AgsPulsePort *pulse_port,
			       guint buffer_size)
{
  AgsMutexManager *mutex_manager;

  guint fixed_size;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  fixed_size = ags_pulse_port_get_fixed_size(pulse_port);

  /* lock pulse port */
  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) pulse_port);

  pthread_mutex_unlock(application_mutex);
  
  pthread_mutex_lock(mutex);

#ifdef AGS_WITH_PULSE
  pulse_port->buffer_attr->fragsize = -1;
  pulse_port->buffer_attr->maxlength = -1;
  pulse_port->buffer_attr->minreq = fixed_size;
  pulse_port->buffer_attr->tlength = fixed_size;
#endif

  pulse_port->buffer_size = buffer_size;
  
  if(pulse_port->empty_buffer != NULL){
    free(pulse_port->empty_buffer);
  }
  
  pulse_port->empty_buffer = ags_stream_alloc(pulse_port->pcm_channels * buffer_size,
					      pulse_port->format);

  pthread_mutex_unlock(mutex);
}

void
ags_pulse_port_set_pcm_channels(AgsPulsePort *pulse_port,
				guint pcm_channels)
{
  AgsMutexManager *mutex_manager;

  guint fixed_size;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  fixed_size = ags_pulse_port_get_fixed_size(pulse_port);

  /* lock pulse port */
  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) pulse_port);

  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

  pulse_port->pcm_channels = pcm_channels;

#ifdef AGS_WITH_PULSE
  pulse_port->sample_spec->channels = pcm_channels;

  pulse_port->buffer_attr->fragsize = -1;
  pulse_port->buffer_attr->maxlength = -1;
  pulse_port->buffer_attr->minreq = fixed_size;
  pulse_port->buffer_attr->tlength = fixed_size;
#endif

  if(pulse_port->empty_buffer != NULL){
    free(pulse_port->empty_buffer);
  }
  
  pulse_port->empty_buffer = ags_stream_alloc(pcm_channels * pulse_port->buffer_size,
					      pulse_port->format);

  pthread_mutex_unlock(mutex);
}

void
ags_pulse_port_set_format(AgsPulsePort *pulse_port,
			  guint format)
{
  AgsMutexManager *mutex_manager;

  guint fixed_size;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);

  fixed_size = ags_pulse_port_get_fixed_size(pulse_port);
  
  /* lock pulse port */
  pthread_mutex_lock(application_mutex);

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) pulse_port);

  pthread_mutex_unlock(application_mutex);

  pthread_mutex_lock(mutex);

#ifdef AGS_WITH_PULSE
  switch(format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      if(is_bigendian()){
	pulse_port->sample_spec->format = PA_SAMPLE_S16BE;
      }else{
	pulse_port->sample_spec->format = PA_SAMPLE_S16LE;
      }
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      if(is_bigendian()){
	pulse_port->sample_spec->format = PA_SAMPLE_S24_32BE;
      }else{
	pulse_port->sample_spec->format = PA_SAMPLE_S24_32LE;
      }
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      if(is_bigendian()){
	pulse_port->sample_spec->format = PA_SAMPLE_S32BE;
      }else{
	pulse_port->sample_spec->format = PA_SAMPLE_S32LE;
      }
    }
    break;
  default:
    g_warning("pulse devout - unsupported format");
  }
  
  pulse_port->buffer_attr->fragsize = -1;
  pulse_port->buffer_attr->maxlength = -1;
  pulse_port->buffer_attr->minreq = fixed_size;
  pulse_port->buffer_attr->tlength = fixed_size;
#endif

  pulse_port->format = format;
  
  if(pulse_port->empty_buffer != NULL){
    free(pulse_port->empty_buffer);
  }
  
  pulse_port->empty_buffer = ags_stream_alloc(pulse_port->pcm_channels * pulse_port->buffer_size,
					      format);

  pthread_mutex_unlock(mutex);
}

/**
 * ags_pulse_port_get_latency:
 * @pulse_port: the #AgsPulsePort
 * 
 * Gets latency.
 * 
 * Since: 1.0.0
 */
guint
ags_pulse_port_get_latency(AgsPulsePort *pulse_port)
{
  guint latency;

#ifdef AGS_WITH_PULSE
  latency = (guint) floor((gdouble) NSEC_PER_SEC / (gdouble) pulse_port->sample_spec->rate * (gdouble) pulse_port->buffer_size);
#endif

  return(latency);
}

/**
 * ags_pulse_port_new:
 * @pulse_client: the #AgsPulseClient assigned to
 *
 * Instantiate a new #AgsPulsePort.
 *
 * Returns: the new #AgsPulsePort
 *
 * Since: 1.0.0
 */
AgsPulsePort*
ags_pulse_port_new(GObject *pulse_client)
{
  AgsPulsePort *pulse_port;

  pulse_port = (AgsPulsePort *) g_object_new(AGS_TYPE_PULSE_PORT,
					     "pulse-client", pulse_client,
					     NULL);

  return(pulse_port);
}
