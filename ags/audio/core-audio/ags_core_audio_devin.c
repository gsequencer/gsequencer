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

#include <ags/audio/core-audio/ags_core_audio_devin.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/core-audio/ags_core_audio_server.h>
#include <ags/audio/core-audio/ags_core_audio_client.h>
#include <ags/audio/core-audio/ags_core_audio_port.h>

#include <ags/audio/task/ags_notify_soundcard.h>
#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <string.h>
#include <math.h>
#include <time.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_core_audio_devin_class_init(AgsCoreAudioDevinClass *core_audio_devin);
void ags_core_audio_devin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_core_audio_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_core_audio_devin_init(AgsCoreAudioDevin *core_audio_devin);
void ags_core_audio_devin_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_core_audio_devin_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_core_audio_devin_dispose(GObject *gobject);
void ags_core_audio_devin_finalize(GObject *gobject);

AgsUUID* ags_core_audio_devin_get_uuid(AgsConnectable *connectable);
gboolean ags_core_audio_devin_has_resource(AgsConnectable *connectable);
gboolean ags_core_audio_devin_is_ready(AgsConnectable *connectable);
void ags_core_audio_devin_add_to_registry(AgsConnectable *connectable);
void ags_core_audio_devin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_core_audio_devin_list_resource(AgsConnectable *connectable);
xmlNode* ags_core_audio_devin_xml_compose(AgsConnectable *connectable);
void ags_core_audio_devin_xml_parse(AgsConnectable *connectable,
				    xmlNode *node);
gboolean ags_core_audio_devin_is_connected(AgsConnectable *connectable);
void ags_core_audio_devin_connect(AgsConnectable *connectable);
void ags_core_audio_devin_disconnect(AgsConnectable *connectable);

void ags_core_audio_devin_set_application_context(AgsSoundcard *soundcard,
						  AgsApplicationContext *application_context);
AgsApplicationContext* ags_core_audio_devin_get_application_context(AgsSoundcard *soundcard);

void ags_core_audio_devin_set_device(AgsSoundcard *soundcard,
				     gchar *device);
gchar* ags_core_audio_devin_get_device(AgsSoundcard *soundcard);

void ags_core_audio_devin_set_presets(AgsSoundcard *soundcard,
				      guint channels,
				      guint rate,
				      guint buffer_size,
				      guint format);
void ags_core_audio_devin_get_presets(AgsSoundcard *soundcard,
				      guint *channels,
				      guint *rate,
				      guint *buffer_size,
				      guint *format);

void ags_core_audio_devin_list_cards(AgsSoundcard *soundcard,
				     GList **card_id, GList **card_name);
void ags_core_audio_devin_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
				   guint *channels_min, guint *channels_max,
				   guint *rate_min, guint *rate_max,
				   guint *buffer_size_min, guint *buffer_size_max,
				   GError **error);

gboolean ags_core_audio_devin_is_starting(AgsSoundcard *soundcard);
gboolean ags_core_audio_devin_is_playing(AgsSoundcard *soundcard);

gchar* ags_core_audio_devin_get_uptime(AgsSoundcard *soundcard);

void ags_core_audio_devin_port_init(AgsSoundcard *soundcard,
				    GError **error);
void ags_core_audio_devin_port_play(AgsSoundcard *soundcard,
				    GError **error);
void ags_core_audio_devin_port_free(AgsSoundcard *soundcard);

void ags_core_audio_devin_tic(AgsSoundcard *soundcard);
void ags_core_audio_devin_offset_changed(AgsSoundcard *soundcard,
					 guint note_offset);

void ags_core_audio_devin_set_bpm(AgsSoundcard *soundcard,
				  gdouble bpm);
gdouble ags_core_audio_devin_get_bpm(AgsSoundcard *soundcard);

void ags_core_audio_devin_set_delay_factor(AgsSoundcard *soundcard,
					   gdouble delay_factor);
gdouble ags_core_audio_devin_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_core_audio_devin_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_core_audio_devin_get_delay(AgsSoundcard *soundcard);
guint ags_core_audio_devin_get_attack(AgsSoundcard *soundcard);

void* ags_core_audio_devin_get_buffer(AgsSoundcard *soundcard);
void* ags_core_audio_devin_get_next_buffer(AgsSoundcard *soundcard);
void* ags_core_audio_devin_get_prev_buffer(AgsSoundcard *soundcard);

guint ags_core_audio_devin_get_delay_counter(AgsSoundcard *soundcard);

void ags_core_audio_devin_set_note_offset(AgsSoundcard *soundcard,
					  guint note_offset);
guint ags_core_audio_devin_get_note_offset(AgsSoundcard *soundcard);

void ags_core_audio_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
						   guint note_offset);
guint ags_core_audio_devin_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_core_audio_devin_set_loop(AgsSoundcard *soundcard,
				   guint loop_left, guint loop_right,
				   gboolean do_loop);
void ags_core_audio_devin_get_loop(AgsSoundcard *soundcard,
				   guint *loop_left, guint *loop_right,
				   gboolean *do_loop);

guint ags_core_audio_devin_get_loop_offset(AgsSoundcard *soundcard);

/**
 * SECTION:ags_core_audio_devin
 * @short_description: Output to soundcard
 * @title: AgsCoreAudioDevin
 * @section_id:
 * @include: ags/audio/core-audio/ags_core_audio_devin.h
 *
 * #AgsCoreAudioDevin represents a soundcard and supports output.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_DEVICE,
  PROP_DSP_CHANNELS,
  PROP_PCM_CHANNELS,
  PROP_FORMAT,
  PROP_BUFFER_SIZE,
  PROP_SAMPLERATE,
  PROP_BUFFER,
  PROP_BPM,
  PROP_DELAY_FACTOR,
  PROP_ATTACK,
  PROP_CORE_AUDIO_CLIENT,
  PROP_CORE_AUDIO_PORT,
  PROP_CHANNEL,
};

static gpointer ags_core_audio_devin_parent_class = NULL;

static pthread_mutex_t ags_core_audio_devin_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_core_audio_devin_get_type (void)
{
  static GType ags_type_core_audio_devin = 0;

  if(!ags_type_core_audio_devin){
    static const GTypeInfo ags_core_audio_devin_info = {
      sizeof(AgsCoreAudioDevinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_core_audio_devin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCoreAudioDevin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_core_audio_devin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_devin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_devin_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_core_audio_devin = g_type_register_static(G_TYPE_OBJECT,
						       "AgsCoreAudioDevin",
						       &ags_core_audio_devin_info,
						       0);

    g_type_add_interface_static(ags_type_core_audio_devin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_core_audio_devin,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);
  }

  return (ags_type_core_audio_devin);
}

void
ags_core_audio_devin_class_init(AgsCoreAudioDevinClass *core_audio_devin)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_core_audio_devin_parent_class = g_type_class_peek_parent(core_audio_devin);

  /* GObjectClass */
  gobject = (GObjectClass *) core_audio_devin;

  gobject->set_property = ags_core_audio_devin_set_property;
  gobject->get_property = ags_core_audio_devin_get_property;

  gobject->dispose = ags_core_audio_devin_dispose;
  gobject->finalize = ags_core_audio_devin_finalize;

  /* properties */
  /**
   * AgsCoreAudioDevin:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 2.0.0
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
   * AgsCoreAudioDevin:device:
   *
   * The core audio soundcard indentifier
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-core-audio-devin-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsCoreAudioDevin:dsp-channels:
   *
   * The dsp channel count
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("dsp-channels",
				 i18n_pspec("count of DSP channels"),
				 i18n_pspec("The count of DSP channels to use"),
				 1,
				 64,
				 2,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DSP_CHANNELS,
				  param_spec);

  /**
   * AgsCoreAudioDevin:pcm-channels:
   *
   * The pcm channel count
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("pcm-channels",
				 i18n_pspec("count of PCM channels"),
				 i18n_pspec("The count of PCM channels to use"),
				 1,
				 64,
				 2,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PCM_CHANNELS,
				  param_spec);

  /**
   * AgsCoreAudioDevin:format:
   *
   * The precision of the buffer
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("precision of buffer"),
				 i18n_pspec("The precision to use for a frame"),
				 1,
				 64,
				 AGS_SOUNDCARD_DEFAULT_FORMAT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsCoreAudioDevin:buffer-size:
   *
   * The buffer size
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("frame count of a buffer"),
				 i18n_pspec("The count of frames a buffer contains"),
				 1,
				 44100,
				 940,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsCoreAudioDevin:samplerate:
   *
   * The samplerate
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("frames per second"),
				 i18n_pspec("The frames count played during a second"),
				 8000,
				 96000,
				 44100,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsCoreAudioDevin:buffer:
   *
   * The buffer
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to play"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsCoreAudioDevin:bpm:
   *
   * Beats per minute
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_double("bpm",
				   i18n_pspec("beats per minute"),
				   i18n_pspec("Beats per minute to use"),
				   1.0,
				   240.0,
				   120.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  /**
   * AgsCoreAudioDevin:delay-factor:
   *
   * tact
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_double("delay-factor",
				   i18n_pspec("delay factor"),
				   i18n_pspec("The delay factor"),
				   0.0,
				   16.0,
				   1.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_FACTOR,
				  param_spec);

  /**
   * AgsCoreAudioDevin:attack:
   *
   * Attack of the buffer
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("attack",
				    i18n_pspec("attack of buffer"),
				    i18n_pspec("The attack to use for the buffer"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  
  /**
   * AgsCoreAudioDevin:core-audio-client:
   *
   * The assigned #AgsCoreAudioClient
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_object("core-audio-client",
				   i18n_pspec("core audio client object"),
				   i18n_pspec("The core audio client object"),
				   AGS_TYPE_CORE_AUDIO_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CORE_AUDIO_CLIENT,
				  param_spec);

  /**
   * AgsCoreAudioDevin:core-audio-port:
   *
   * The assigned #AgsCoreAudioPort
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("core-audio-port",
				    i18n_pspec("core audio port object"),
				    i18n_pspec("The core audio port object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CORE_AUDIO_PORT,
				  param_spec);
}

GQuark
ags_core_audio_devin_error_quark()
{
  return(g_quark_from_static_string("ags-core_audio_devin-error-quark"));
}

void
ags_core_audio_devin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  //TODO:JK: implement me
}

void
ags_core_audio_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  //TODO:JK: implement me
}

void
ags_core_audio_devin_init(AgsCoreAudioDevin *core_audio_devin)
{
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* flags */
  core_audio_devin->flags = 0;
  g_atomic_int_set(&(core_audio_devin->sync_flags),
		   AGS_CORE_AUDIO_DEVIN_PASS_THROUGH);

  /* devin mutex */
  core_audio_devin->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  core_audio_devin->obj_mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* parent */
  core_audio_devin->application_context = NULL;

  //TODO:JK: implement me
}

void
ags_core_audio_devin_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsCoreAudioDevin *core_audio_devin;
  
  pthread_mutex_t *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(gobject);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      pthread_mutex_lock(core_audio_devin_mutex);

      if(core_audio_devin->application_context == application_context){
	pthread_mutex_unlock(core_audio_devin_mutex);

	return;
      }

      if(core_audio_devin->application_context != NULL){
	g_object_unref(G_OBJECT(core_audio_devin->application_context));
      }

      if(application_context != NULL){	
	g_object_ref(G_OBJECT(application_context));
      }

      core_audio_devin->application_context = application_context;

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      pthread_mutex_lock(core_audio_devin_mutex);

      core_audio_devin->card_uri = g_strdup(device);

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      pthread_mutex_lock(core_audio_devin_mutex);

      if(dsp_channels == core_audio_devin->dsp_channels){
	pthread_mutex_unlock(core_audio_devin_mutex);

	return;
      }

      core_audio_devin->dsp_channels = dsp_channels;

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      pthread_mutex_lock(core_audio_devin_mutex);

      if(pcm_channels == core_audio_devin->pcm_channels){
	pthread_mutex_unlock(core_audio_devin_mutex);

	return;
      }

      core_audio_devin->pcm_channels = pcm_channels;

      pthread_mutex_unlock(core_audio_devin_mutex);

      ags_core_audio_devin_realloc_buffer(core_audio_devin);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      pthread_mutex_lock(core_audio_devin_mutex);

      if(format == core_audio_devin->format){
	pthread_mutex_unlock(core_audio_devin_mutex);

	return;
      }

      core_audio_devin->format = format;

      pthread_mutex_unlock(core_audio_devin_mutex);

      ags_core_audio_devin_realloc_buffer(core_audio_devin);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      pthread_mutex_lock(core_audio_devin_mutex);

      if(buffer_size == core_audio_devin->buffer_size){
	pthread_mutex_unlock(core_audio_devin_mutex);

	return;
      }

      core_audio_devin->buffer_size = buffer_size;

      pthread_mutex_unlock(core_audio_devin_mutex);

      ags_core_audio_devin_realloc_buffer(core_audio_devin);
      ags_core_audio_devin_adjust_delay_and_attack(core_audio_devin);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      pthread_mutex_lock(core_audio_devin_mutex);
      
      if(samplerate == core_audio_devin->samplerate){
	pthread_mutex_unlock(core_audio_devin_mutex);

	return;
      }

      core_audio_devin->samplerate = samplerate;

      pthread_mutex_unlock(core_audio_devin_mutex);

      ags_core_audio_devin_realloc_buffer(core_audio_devin);
      ags_core_audio_devin_adjust_delay_and_attack(core_audio_devin);
    }
    break;
  case PROP_BUFFER:
    {
      //TODO:JK: implement me
    }
    break;
  case PROP_BPM:
    {
      gdouble bpm;
      
      bpm = g_value_get_double(value);

      pthread_mutex_lock(core_audio_devin_mutex);

      core_audio_devin->bpm = bpm;

      pthread_mutex_unlock(core_audio_devin_mutex);

      ags_core_audio_devin_adjust_delay_and_attack(core_audio_devin);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      pthread_mutex_lock(core_audio_devin_mutex);

      core_audio_devin->delay_factor = delay_factor;

      pthread_mutex_unlock(core_audio_devin_mutex);

      ags_core_audio_devin_adjust_delay_and_attack(core_audio_devin);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      AgsCoreAudioClient *core_audio_client;

      core_audio_client = (AgsCoreAudioClient *) g_value_get_object(value);

      pthread_mutex_lock(core_audio_devin_mutex);

      if(core_audio_devin->core_audio_client == (GObject *) core_audio_client){
	pthread_mutex_unlock(core_audio_devin_mutex);

	return;
      }

      if(core_audio_devin->core_audio_client != NULL){
	g_object_unref(G_OBJECT(core_audio_devin->core_audio_client));
      }

      if(core_audio_client != NULL){
	g_object_ref(core_audio_client);
      }
      
      core_audio_devin->core_audio_client = (GObject *) core_audio_client;

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      AgsCoreAudioPort *core_audio_port;

      core_audio_port = (AgsCoreAudioPort *) g_value_get_pointer(value);

      pthread_mutex_lock(core_audio_devin_mutex);

      if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port) ||
	 g_list_find(core_audio_devin->core_audio_port, core_audio_port) != NULL){
	pthread_mutex_unlock(core_audio_devin_mutex);

	return;
      }

      g_object_ref(core_audio_port);
      core_audio_devin->core_audio_port = g_list_append(core_audio_devin->core_audio_port,
							core_audio_port);

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_devin_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsCoreAudioDevin *core_audio_devin;

  pthread_mutex_t *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(gobject);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      pthread_mutex_lock(core_audio_devin_mutex);

      g_value_set_object(value, core_audio_devin->application_context);

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      pthread_mutex_lock(core_audio_devin_mutex);

      g_value_set_string(value, core_audio_devin->card_uri);

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      pthread_mutex_lock(core_audio_devin_mutex);

      g_value_set_uint(value, core_audio_devin->dsp_channels);

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      pthread_mutex_lock(core_audio_devin_mutex);

      g_value_set_uint(value, core_audio_devin->pcm_channels);

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      pthread_mutex_lock(core_audio_devin_mutex);

      g_value_set_uint(value, core_audio_devin->format);

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      pthread_mutex_lock(core_audio_devin_mutex);

      g_value_set_uint(value, core_audio_devin->buffer_size);

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(core_audio_devin_mutex);

      g_value_set_uint(value, core_audio_devin->samplerate);

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      pthread_mutex_lock(core_audio_devin_mutex);

      g_value_set_pointer(value, core_audio_devin->buffer);

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_BPM:
    {
      pthread_mutex_lock(core_audio_devin_mutex);

      g_value_set_double(value, core_audio_devin->bpm);

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      pthread_mutex_lock(core_audio_devin_mutex);

      g_value_set_double(value, core_audio_devin->delay_factor);

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_ATTACK:
    {
      pthread_mutex_lock(core_audio_devin_mutex);

      g_value_set_pointer(value, core_audio_devin->attack);

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      pthread_mutex_lock(core_audio_devin_mutex);

      g_value_set_object(value, core_audio_devin->core_audio_client);

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      pthread_mutex_lock(core_audio_devin_mutex);

      g_value_set_pointer(value,
			  g_list_copy(core_audio_devin->core_audio_port));

      pthread_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_devin_dispose(GObject *gobject)
{
  AgsCoreAudioDevin *core_audio_devin;

  GList *list;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(gobject);

  //TODO:JK: implement me
  
  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_devin_parent_class)->dispose(gobject);
}

void
ags_core_audio_devin_finalize(GObject *gobject)
{
  AgsCoreAudioDevin *core_audio_devin;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(gobject);

  pthread_mutex_destroy(core_audio_devin->obj_mutex);
  free(core_audio_devin->obj_mutex);

  pthread_mutexattr_destroy(core_audio_devin->obj_mutexattr);
  free(core_audio_devin->obj_mutexattr);

  //TODO:JK: implement me
  
  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_devin_parent_class)->finalize(gobject);
}

/**
 * ags_core_audio_devin_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_core_audio_devin_get_class_mutex()
{
  return(&ags_core_audio_devin_class_mutex);
}


/**
 * ags_core_audio_devin_adjust_delay_and_attack:
 * @core_audio_devin: the #AgsCoreAudioDevin
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_devin_adjust_delay_and_attack(AgsCoreAudioDevin *core_audio_devin)
{
  gdouble delay;
  guint default_tact_frames;
  guint delay_tact_frames;
  guint default_period;
  gint next_attack;
  guint i;

  pthread_mutex_t *core_audio_devin_mutex;

  if(!AGS_IS_CORE_AUDIO_DEVIN(core_audio_devin)){
    return;
  }

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());
  
  /* get some initial values */
  delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(core_audio_devin));

#ifdef AGS_DEBUG
  g_message("delay : %f", delay);
#endif
  
  pthread_mutex_lock(core_audio_devin_mutex);

  default_tact_frames = (guint) (delay * core_audio_devin->buffer_size);
  delay_tact_frames = (guint) (floor(delay) * core_audio_devin->buffer_size);
  default_period = (1.0 / AGS_SOUNDCARD_DEFAULT_PERIOD) * (default_tact_frames);

  i = 0;
  
  core_audio_devin->attack[0] = (guint) floor(0.25 * core_audio_devin->buffer_size);
  next_attack = (((core_audio_devin->attack[i] + default_tact_frames) / core_audio_devin->buffer_size) - delay) * core_audio_devin->buffer_size;

  if(next_attack < 0){
    next_attack = 0;
  }

  if(next_attack >= core_audio_devin->buffer_size){
    next_attack = core_audio_devin->buffer_size - 1;
  }
  
  /* check if delay drops for next attack */
  if(next_attack < 0){
    core_audio_devin->attack[i] = core_audio_devin->attack[i] - ((gdouble) next_attack / 2.0);

    if(core_audio_devin->attack[i] < 0){
      core_audio_devin->attack[i] = 0;
    }
    
    if(core_audio_devin->attack[i] >= core_audio_devin->buffer_size){
      core_audio_devin->attack[i] = core_audio_devin->buffer_size - 1;
    }
    
    next_attack = next_attack + (next_attack / 2.0);

    if(next_attack < 0){
      next_attack = 0;
    }

    if(next_attack >= core_audio_devin->buffer_size){
      next_attack = core_audio_devin->buffer_size - 1;
    }
  }
  
  for(i = 1; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    core_audio_devin->attack[i] = next_attack;
    next_attack = (((core_audio_devin->attack[i] + default_tact_frames) / core_audio_devin->buffer_size) - delay) * core_audio_devin->buffer_size;

    if(next_attack >= core_audio_devin->buffer_size){
      next_attack = core_audio_devin->buffer_size - 1;
    }
    
    /* check if delay drops for next attack */
    if(next_attack < 0){
      core_audio_devin->attack[i] = core_audio_devin->attack[i] - ((gdouble) next_attack / 2.0);

      if(core_audio_devin->attack[i] < 0){
	core_audio_devin->attack[i] = 0;
      }

      if(core_audio_devin->attack[i] >= core_audio_devin->buffer_size){
	core_audio_devin->attack[i] = core_audio_devin->buffer_size - 1;
      }
    
      next_attack = next_attack + (next_attack / 2.0);

      if(next_attack < 0){
	next_attack = 0;
      }

      if(next_attack >= core_audio_devin->buffer_size){
	next_attack = core_audio_devin->buffer_size - 1;
      }
    }
    
#ifdef AGS_DEBUG
    g_message("%d", core_audio_devin->attack[i]);
#endif
  }

  core_audio_devin->attack[0] = core_audio_devin->attack[i - 2];
  
  for(i = 0; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD - 1; i++){
    core_audio_devin->delay[i] = ((gdouble) (default_tact_frames + core_audio_devin->attack[i] - core_audio_devin->attack[i + 1])) / (gdouble) core_audio_devin->buffer_size;
    
#ifdef AGS_DEBUG
    g_message("%f", core_audio_devin->delay[i]);
#endif
  }

  core_audio_devin->delay[i] = ((gdouble) (default_tact_frames + core_audio_devin->attack[i] - core_audio_devin->attack[0])) / (gdouble) core_audio_devin->buffer_size;

  pthread_mutex_unlock(core_audio_devin_mutex);
}

/**
 * ags_core_audio_devin_realloc_buffer:
 * @core_audio_devin: the #AgsCoreAudioDevin
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_devin_realloc_buffer(AgsCoreAudioDevin *core_audio_devin)
{
  guint pcm_channels;
  guint buffer_size;
  guint format;
  guint word_size;

  pthread_mutex_t *core_audio_devin_mutex;  

  if(!AGS_IS_CORE_AUDIO_DEVIN(core_audio_devin)){
    return;
  }

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* get word size */  
  pthread_mutex_lock(core_audio_devin_mutex);

  pcm_channels = core_audio_devin->pcm_channels;
  buffer_size = core_audio_devin->buffer_size;

  format = core_audio_devin->format;
  
  pthread_mutex_unlock(core_audio_devin_mutex);

  switch(format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      word_size = sizeof(gint16);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      word_size = sizeof(gint32);
    }
    break;
  default:
    g_warning("ags_core_audio_devin_realloc_buffer(): unsupported word size");
    return;
  }
  
  /* AGS_CORE_AUDIO_DEVIN_BUFFER_0 */
  if(core_audio_devin->buffer[0] != NULL){
    free(core_audio_devin->buffer[0]);
  }
  
  core_audio_devin->buffer[0] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVIN_BUFFER_1 */
  if(core_audio_devin->buffer[1] != NULL){
    free(core_audio_devin->buffer[1]);
  }

  core_audio_devin->buffer[1] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVIN_BUFFER_2 */
  if(core_audio_devin->buffer[2] != NULL){
    free(core_audio_devin->buffer[2]);
  }

  core_audio_devin->buffer[2] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVIN_BUFFER_3 */
  if(core_audio_devin->buffer[3] != NULL){
    free(core_audio_devin->buffer[3]);
  }
  
  core_audio_devin->buffer[3] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_CORE_AUDIO_DEVIN_BUFFER_4 */
  if(core_audio_devin->buffer[4] != NULL){
    free(core_audio_devin->buffer[4]);
  }
  
  core_audio_devin->buffer[4] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_CORE_AUDIO_DEVIN_BUFFER_5 */
  if(core_audio_devin->buffer[5] != NULL){
    free(core_audio_devin->buffer[5]);
  }
  
  core_audio_devin->buffer[5] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_CORE_AUDIO_DEVIN_BUFFER_6 */
  if(core_audio_devin->buffer[6] != NULL){
    free(core_audio_devin->buffer[6]);
  }
  
  core_audio_devin->buffer[6] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_CORE_AUDIO_DEVIN_BUFFER_7 */
  if(core_audio_devin->buffer[7] != NULL){
    free(core_audio_devin->buffer[7]);
  }
  
  core_audio_devin->buffer[7] = (void *) malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_core_audio_devin_new:
 * @application_context: the #AgsApplicationContext
 *
 * Creates a new instance of #AgsCoreAudioDevin.
 *
 * Returns: a new #AgsCoreAudioDevin
 *
 * Since: 2.0.0
 */
AgsCoreAudioDevin*
ags_core_audio_devin_new(AgsApplicationContext *application_context)
{
  AgsCoreAudioDevin *core_audio_devin;

  core_audio_devin = (AgsCoreAudioDevin *) g_object_new(AGS_TYPE_CORE_AUDIO_DEVIN,
							"application-context", application_context,
							NULL);
  
  return(core_audio_devin);
}
