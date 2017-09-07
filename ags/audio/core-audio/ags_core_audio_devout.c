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

#include <ags/audio/core-audio/ags_core_audio_devout.h>

#include <ags/lib/ags_time.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_main_loop.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_channel.h>
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

void ags_core_audio_devout_class_init(AgsCoreAudioDevoutClass *core_audio_devout);
void ags_core_audio_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_core_audio_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_core_audio_devout_init(AgsCoreAudioDevout *core_audio_devout);
void ags_core_audio_devout_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_devout_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_devout_disconnect(AgsConnectable *connectable);
void ags_core_audio_devout_connect(AgsConnectable *connectable);
void ags_core_audio_devout_dispose(GObject *gobject);
void ags_core_audio_devout_finalize(GObject *gobject);

void ags_core_audio_devout_set_application_context(AgsSoundcard *soundcard,
						   AgsApplicationContext *application_context);
AgsApplicationContext* ags_core_audio_devout_get_application_context(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_application_mutex(AgsSoundcard *soundcard,
						 pthread_mutex_t *application_mutex);
pthread_mutex_t* ags_core_audio_devout_get_application_mutex(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_device(AgsSoundcard *soundcard,
				      gchar *device);
gchar* ags_core_audio_devout_get_device(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_presets(AgsSoundcard *soundcard,
				       guint channels,
				       guint rate,
				       guint buffer_size,
				       guint format);
void ags_core_audio_devout_get_presets(AgsSoundcard *soundcard,
				       guint *channels,
				       guint *rate,
				       guint *buffer_size,
				       guint *format);

void ags_core_audio_devout_list_cards(AgsSoundcard *soundcard,
				      GList **card_id, GList **card_name);
void ags_core_audio_devout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
				    guint *channels_min, guint *channels_max,
				    guint *rate_min, guint *rate_max,
				    guint *buffer_size_min, guint *buffer_size_max,
				    GError **error);

gboolean ags_core_audio_devout_is_starting(AgsSoundcard *soundcard);
gboolean ags_core_audio_devout_is_playing(AgsSoundcard *soundcard);

gchar* ags_core_audio_devout_get_uptime(AgsSoundcard *soundcard);

void ags_core_audio_devout_port_init(AgsSoundcard *soundcard,
				     GError **error);
void ags_core_audio_devout_port_play(AgsSoundcard *soundcard,
				     GError **error);
void ags_core_audio_devout_port_free(AgsSoundcard *soundcard);

void ags_core_audio_devout_tic(AgsSoundcard *soundcard);
void ags_core_audio_devout_offset_changed(AgsSoundcard *soundcard,
					  guint note_offset);

void ags_core_audio_devout_set_bpm(AgsSoundcard *soundcard,
				   gdouble bpm);
gdouble ags_core_audio_devout_get_bpm(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_delay_factor(AgsSoundcard *soundcard,
					    gdouble delay_factor);
gdouble ags_core_audio_devout_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_core_audio_devout_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_core_audio_devout_get_delay(AgsSoundcard *soundcard);
guint ags_core_audio_devout_get_attack(AgsSoundcard *soundcard);

void* ags_core_audio_devout_get_buffer(AgsSoundcard *soundcard);
void* ags_core_audio_devout_get_next_buffer(AgsSoundcard *soundcard);
void* ags_core_audio_devout_get_prev_buffer(AgsSoundcard *soundcard);

guint ags_core_audio_devout_get_delay_counter(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_note_offset(AgsSoundcard *soundcard,
					   guint note_offset);
guint ags_core_audio_devout_get_note_offset(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
						    guint note_offset);
guint ags_core_audio_devout_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_loop(AgsSoundcard *soundcard,
				    guint loop_left, guint loop_right,
				    gboolean do_loop);
void ags_core_audio_devout_get_loop(AgsSoundcard *soundcard,
				    guint *loop_left, guint *loop_right,
				    gboolean *do_loop);

guint ags_core_audio_devout_get_loop_offset(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_audio(AgsSoundcard *soundcard,
				     GList *audio);
GList* ags_core_audio_devout_get_audio(AgsSoundcard *soundcard);

void ags_core_audio_devout_adjust_delay_and_attack(AgsCoreAudioDevout *core_audio_devout);
void ags_core_audio_devout_realloc_buffer(AgsCoreAudioDevout *core_audio_devout);

/**
 * SECTION:ags_core_audio_devout
 * @short_description: Output to soundcard
 * @title: AgsCoreAudioDevout
 * @section_id:
 * @include: ags/audio/core-audio/ags_core_audio_devout.h
 *
 * #AgsCoreAudioDevout represents a soundcard and supports output.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_APPLICATION_MUTEX,
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

enum{
  LAST_SIGNAL,
};

static gpointer ags_core_audio_devout_parent_class = NULL;
static guint core_audio_devout_signals[LAST_SIGNAL];

GType
ags_core_audio_devout_get_type (void)
{
  static GType ags_type_core_audio_devout = 0;

  if(!ags_type_core_audio_devout){
    static const GTypeInfo ags_core_audio_devout_info = {
      sizeof (AgsCoreAudioDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_core_audio_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCoreAudioDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_core_audio_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_devout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_core_audio_devout = g_type_register_static(G_TYPE_OBJECT,
							"AgsCoreAudioDevout",
							&ags_core_audio_devout_info,
							0);

    g_type_add_interface_static(ags_type_core_audio_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_core_audio_devout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);
  }

  return (ags_type_core_audio_devout);
}

void
ags_core_audio_devout_class_init(AgsCoreAudioDevoutClass *core_audio_devout)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_core_audio_devout_parent_class = g_type_class_peek_parent(core_audio_devout);

  /* GObjectClass */
  gobject = (GObjectClass *) core_audio_devout;

  gobject->set_property = ags_core_audio_devout_set_property;
  gobject->get_property = ags_core_audio_devout_get_property;

  gobject->dispose = ags_core_audio_devout_dispose;
  gobject->finalize = ags_core_audio_devout_finalize;

  /* properties */
  /**
   * AgsCoreAudioDevout:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 0.9.24
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
   * AgsCoreAudioDevout:application-mutex:
   *
   * The assigned application mutex
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_pointer("application-mutex",
				    i18n_pspec("the application mutex object"),
				    i18n_pspec("The application mutex object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_MUTEX,
				  param_spec);

  /**
   * AgsCoreAudioDevout:device:
   *
   * The core audio soundcard indentifier
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-core-audio-devout-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsCoreAudioDevout:dsp-channels:
   *
   * The dsp channel count
   * 
   * Since: 0.9.24
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
   * AgsCoreAudioDevout:pcm-channels:
   *
   * The pcm channel count
   * 
   * Since: 0.9.24
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
   * AgsCoreAudioDevout:format:
   *
   * The precision of the buffer
   * 
   * Since: 0.9.24
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
   * AgsCoreAudioDevout:buffer-size:
   *
   * The buffer size
   * 
   * Since: 0.9.24
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
   * AgsCoreAudioDevout:samplerate:
   *
   * The samplerate
   * 
   * Since: 0.9.24
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
   * AgsCoreAudioDevout:buffer:
   *
   * The buffer
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to play"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsCoreAudioDevout:bpm:
   *
   * Beats per minute
   * 
   * Since: 0.9.24
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
   * AgsCoreAudioDevout:delay-factor:
   *
   * tact
   * 
   * Since: 0.9.24
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
   * AgsCoreAudioDevout:attack:
   *
   * Attack of the buffer
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_pointer("attack",
				    i18n_pspec("attack of buffer"),
				    i18n_pspec("The attack to use for the buffer"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  
  /**
   * AgsCoreAudioDevout:core-audio-client:
   *
   * The assigned #AgsCoreAudioClient
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_object("core-audio-client",
				   i18n_pspec("core audio client object"),
				   i18n_pspec("The core audio client object"),
				   AGS_TYPE_PULSE_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CORE_AUDIO_CLIENT,
				  param_spec);

  /**
   * AgsCoreAudioDevout:core-audio-port:
   *
   * The assigned #AgsCoreAudioPort
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_pointer("core-audio-port",
				    i18n_pspec("core audio port object"),
				    i18n_pspec("The core audio port object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CORE_AUDIO_PORT,
				  param_spec);

  /**
   * AgsCoreAudioDevout:channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 0.9.24
   */
  param_spec = g_param_spec_object("channel",
				   i18n_pspec("channel"),
				   i18n_pspec("The mapped channel"),
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);
}

GQuark
ags_core_audio_devout_error_quark()
{
  return(g_quark_from_static_string("ags-core_audio_devout-error-quark"));
}

void
ags_core_audio_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_core_audio_devout_connect;
  connectable->disconnect = ags_core_audio_devout_disconnect;
}

void
ags_core_audio_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_application_context = ags_core_audio_devout_set_application_context;
  soundcard->get_application_context = ags_core_audio_devout_get_application_context;

  soundcard->set_application_mutex = ags_core_audio_devout_set_application_mutex;
  soundcard->get_application_mutex = ags_core_audio_devout_get_application_mutex;

  soundcard->set_device = ags_core_audio_devout_set_device;
  soundcard->get_device = ags_core_audio_devout_get_device;
  
  soundcard->set_presets = ags_core_audio_devout_set_presets;
  soundcard->get_presets = ags_core_audio_devout_get_presets;

  soundcard->list_cards = ags_core_audio_devout_list_cards;
  soundcard->pcm_info = ags_core_audio_devout_pcm_info;

  soundcard->get_poll_fd = NULL;
  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_core_audio_devout_is_starting;
  soundcard->is_playing = ags_core_audio_devout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_core_audio_devout_get_uptime;
  
  soundcard->play_init = ags_core_audio_devout_port_init;
  soundcard->play = ags_core_audio_devout_port_play;

  soundcard->record_init = NULL;
  soundcard->record = NULL;

  soundcard->stop = ags_core_audio_devout_port_free;

  soundcard->tic = ags_core_audio_devout_tic;
  soundcard->offset_changed = ags_core_audio_devout_offset_changed;
    
  soundcard->set_bpm = ags_core_audio_devout_set_bpm;
  soundcard->get_bpm = ags_core_audio_devout_get_bpm;

  soundcard->set_delay_factor = ags_core_audio_devout_set_delay_factor;
  soundcard->get_delay_factor = ags_core_audio_devout_get_delay_factor;
  
  soundcard->get_absolute_delay = ags_core_audio_devout_get_absolute_delay;

  soundcard->get_delay = ags_core_audio_devout_get_delay;
  soundcard->get_attack = ags_core_audio_devout_get_attack;

  soundcard->get_buffer = ags_core_audio_devout_get_buffer;
  soundcard->get_next_buffer = ags_core_audio_devout_get_next_buffer;
  soundcard->get_prev_buffer = ags_core_audio_devout_get_prev_buffer;

  soundcard->get_delay_counter = ags_core_audio_devout_get_delay_counter;

  soundcard->set_note_offset = ags_core_audio_devout_set_note_offset;
  soundcard->get_note_offset = ags_core_audio_devout_get_note_offset;

  soundcard->set_note_offset_absolute = ags_core_audio_devout_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_core_audio_devout_get_note_offset_absolute;

  soundcard->set_loop = ags_core_audio_devout_set_loop;
  soundcard->get_loop = ags_core_audio_devout_get_loop;

  soundcard->get_loop_offset = ags_core_audio_devout_get_loop_offset;

  soundcard->set_audio = ags_core_audio_devout_set_audio;
  soundcard->get_audio = ags_core_audio_devout_get_audio;
}

void
ags_core_audio_devout_init(AgsCoreAudioDevout *core_audio_devout)
{
  AgsMutexManager *mutex_manager;

  AgsConfig *config;

  gchar *str;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert devout mutex */
  core_audio_devout->mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  core_audio_devout->mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) core_audio_devout,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  core_audio_devout->flags = 0;
  g_atomic_int_set(&(core_audio_devout->sync_flags),
		   AGS_CORE_AUDIO_DEVOUT_PASS_THROUGH);

  /* quality */
  config = ags_config_get_instance();

  core_audio_devout->dsp_channels = AGS_SOUNDCARD_DEFAULT_DSP_CHANNELS;
  core_audio_devout->pcm_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
  core_audio_devout->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  core_audio_devout->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  core_audio_devout->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  /* read config */
  /* dsp channels */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "dsp-channels");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "dsp-channels");
  }
  
  if(str != NULL){
    core_audio_devout->dsp_channels = g_ascii_strtoull(str,
						       NULL,
						       10);
	  
    g_free(str);
  }

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
    core_audio_devout->pcm_channels = g_ascii_strtoull(str,
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
    core_audio_devout->samplerate = g_ascii_strtoull(str,
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
    core_audio_devout->buffer_size = g_ascii_strtoull(str,
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
    core_audio_devout->format = g_ascii_strtoull(str,
						 NULL,
						 10);
    free(str);
  }

  /*  */
  core_audio_devout->card_uri = NULL;
  core_audio_devout->core_audio_client = NULL;

  core_audio_devout->port_name = NULL;
  core_audio_devout->core_audio_port = NULL;

  /* buffer */
  core_audio_devout->buffer = (void **) malloc(8 * sizeof(void*));

  core_audio_devout->buffer[0] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * sizeof(signed short));
  core_audio_devout->buffer[1] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * sizeof(signed short));
  core_audio_devout->buffer[2] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * sizeof(signed short));
  core_audio_devout->buffer[3] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * sizeof(signed short));
  core_audio_devout->buffer[4] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * sizeof(signed short));
  core_audio_devout->buffer[5] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * sizeof(signed short));
  core_audio_devout->buffer[6] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * sizeof(signed short));
  core_audio_devout->buffer[7] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * sizeof(signed short));
  
  ags_core_audio_devout_realloc_buffer(core_audio_devout);
  
  /* bpm */
  core_audio_devout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  core_audio_devout->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* delay and attack */
  core_audio_devout->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
						sizeof(gdouble));
  
  core_audio_devout->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					       sizeof(guint));

  ags_core_audio_devout_adjust_delay_and_attack(core_audio_devout);
  
  /* counters */
  core_audio_devout->tact_counter = 0.0;
  core_audio_devout->delay_counter = 0;
  core_audio_devout->tic_counter = 0;

  core_audio_devout->note_offset = 0;
  core_audio_devout->note_offset_absolute = 0;

  core_audio_devout->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  core_audio_devout->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  core_audio_devout->do_loop = FALSE;

  core_audio_devout->loop_offset = 0;

  /* callback mutex */
  core_audio_devout->callback_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(core_audio_devout->callback_mutex,
		     NULL);

  core_audio_devout->callback_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(core_audio_devout->callback_cond, NULL);

  /* callback finish mutex */
  core_audio_devout->callback_finish_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(core_audio_devout->callback_finish_mutex,
		     NULL);

  core_audio_devout->callback_finish_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(core_audio_devout->callback_finish_cond, NULL);

  /* parent */
  core_audio_devout->application_context = NULL;
  core_audio_devout->application_mutex = NULL;

  /*  */
  core_audio_devout->notify_soundcard = NULL;
  
  /* all AgsAudio */
  core_audio_devout->audio = NULL;
}

void
ags_core_audio_devout_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if(core_audio_devout->application_context == (GObject *) application_context){
	return;
      }

      if(core_audio_devout->application_context != NULL){
	g_object_unref(G_OBJECT(core_audio_devout->application_context));
      }

      if(application_context != NULL){
	AgsConfig *config;

	gchar *segmentation;
	guint discriminante, nominante;
	
	g_object_ref(G_OBJECT(application_context));

	core_audio_devout->application_mutex = application_context->mutex;

	config = ags_config_get_instance();

	/* segmentation */
	segmentation = ags_config_get_value(config,
					    AGS_CONFIG_GENERIC,
					    "segmentation");

	if(segmentation != NULL){
	  sscanf(segmentation, "%d/%d",
		 &discriminante,
		 &nominante);
    
	  core_audio_devout->delay_factor = 1.0 / nominante * (nominante / discriminante);
	}
	
	ags_core_audio_devout_adjust_delay_and_attack(core_audio_devout);
	ags_core_audio_devout_realloc_buffer(core_audio_devout);
      }else{
	core_audio_devout->application_mutex = NULL;
      }

      core_audio_devout->application_context = (GObject *) application_context;
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      pthread_mutex_t *application_mutex;

      application_mutex = (pthread_mutex_t *) g_value_get_pointer(value);

      if(core_audio_devout->application_mutex == application_mutex){
	return;
      }
      
      core_audio_devout->application_mutex = application_mutex;
    }
    break;
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      core_audio_devout->card_uri = g_strdup(device);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      if(dsp_channels == core_audio_devout->dsp_channels){
	return;
      }

      core_audio_devout->dsp_channels = dsp_channels;
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      if(pcm_channels == core_audio_devout->pcm_channels){
	return;
      }

      core_audio_devout->pcm_channels = pcm_channels;

      ags_core_audio_devout_realloc_buffer(core_audio_devout);

      if(core_audio_devout->core_audio_port != NULL){
	ags_core_audio_port_set_pcm_channels(core_audio_devout->core_audio_port->data,
					     pcm_channels);
      }
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      if(format == core_audio_devout->format){
	return;
      }

      core_audio_devout->format = format;

      ags_core_audio_devout_realloc_buffer(core_audio_devout);

      if(core_audio_devout->core_audio_port != NULL){
	ags_core_audio_port_set_format(core_audio_devout->core_audio_port->data,
				       format);
      }
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      if(buffer_size == core_audio_devout->buffer_size){
	return;
      }

      core_audio_devout->buffer_size = buffer_size;

      ags_core_audio_devout_realloc_buffer(core_audio_devout);
      ags_core_audio_devout_adjust_delay_and_attack(core_audio_devout);

      if(core_audio_devout->core_audio_port != NULL){
	ags_core_audio_port_set_buffer_size(core_audio_devout->core_audio_port->data,
					    buffer_size);
      }
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      if(samplerate == core_audio_devout->samplerate){
	return;
      }

      core_audio_devout->samplerate = samplerate;

      ags_core_audio_devout_realloc_buffer(core_audio_devout);
      ags_core_audio_devout_adjust_delay_and_attack(core_audio_devout);

      if(core_audio_devout->core_audio_port != NULL){
	ags_core_audio_port_set_samplerate(core_audio_devout->core_audio_port->data,
					   samplerate);
      }
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

      core_audio_devout->bpm = bpm;

      ags_core_audio_devout_adjust_delay_and_attack(core_audio_devout);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      core_audio_devout->delay_factor = delay_factor;

      ags_core_audio_devout_adjust_delay_and_attack(core_audio_devout);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      AgsCoreAudioClient *core_audio_client;

      core_audio_client = (AgsCoreAudioClient *) g_value_get_object(value);

      if(core_audio_devout->core_audio_client == (GObject *) core_audio_client){
	return;
      }

      if(core_audio_devout->core_audio_client != NULL){
	g_object_unref(G_OBJECT(core_audio_devout->core_audio_client));
      }

      if(core_audio_client != NULL){
	g_object_ref(core_audio_client);
      }
      
      core_audio_devout->core_audio_client = (GObject *) core_audio_client;
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      AgsCoreAudioPort *core_audio_port;

      core_audio_port = (AgsCoreAudioPort *) g_value_get_pointer(value);

      if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port) ||
	 g_list_find(core_audio_devout->core_audio_port, core_audio_port) != NULL){
	return;
      }

      if(core_audio_port != NULL){
	g_object_ref(core_audio_port);
	core_audio_devout->core_audio_port = g_list_append(core_audio_devout->core_audio_port,
							   core_audio_port);
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_devout_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, core_audio_devout->application_context);
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      g_value_set_pointer(value, core_audio_devout->application_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      g_value_set_string(value, core_audio_devout->card_uri);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_value_set_uint(value, core_audio_devout->dsp_channels);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_value_set_uint(value, core_audio_devout->pcm_channels);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, core_audio_devout->format);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, core_audio_devout->buffer_size);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, core_audio_devout->samplerate);
    }
    break;
  case PROP_BUFFER:
    {
      g_value_set_pointer(value, core_audio_devout->buffer);
    }
    break;
  case PROP_BPM:
    {
      g_value_set_double(value, core_audio_devout->bpm);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_value_set_double(value, core_audio_devout->delay_factor);
    }
    break;
  case PROP_ATTACK:
    {
      g_value_set_pointer(value, core_audio_devout->attack);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      g_value_set_object(value, core_audio_devout->core_audio_client);
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      g_value_set_pointer(value,
			  g_list_copy(core_audio_devout->core_audio_port));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_devout_dispose(GObject *gobject)
{
  AgsCoreAudioDevout *core_audio_devout;

  GList *list;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(gobject);

  /* application context */
  if(core_audio_devout->application_context != NULL){
    g_object_unref(core_audio_devout->application_context);

    core_audio_devout->application_context = NULL;
  }

  /* unref audio */
  if(core_audio_devout->audio != NULL){
    list = core_audio_devout->audio;

    while(list != NULL){
      g_object_set(G_OBJECT(list->data),
		   "soundcard", NULL,
		   NULL);
      
      list = list->next;
    }

    g_list_free_full(core_audio_devout->audio,
		     g_object_unref);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_devout_parent_class)->dispose(gobject);
}

void
ags_core_audio_devout_finalize(GObject *gobject)
{
  AgsCoreAudioDevout *core_audio_devout;

  AgsMutexManager *mutex_manager;
  
  GList *list, *list_next;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(gobject);

  /* remove core_audio_devout mutex */
  pthread_mutex_lock(core_audio_devout->application_mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(core_audio_devout->application_mutex);

  /* free output buffer */
  free(core_audio_devout->buffer[0]);
  free(core_audio_devout->buffer[1]);
  free(core_audio_devout->buffer[2]);
  free(core_audio_devout->buffer[3]);
  free(core_audio_devout->buffer[4]);
  free(core_audio_devout->buffer[5]);
  free(core_audio_devout->buffer[6]);
  free(core_audio_devout->buffer[7]);

  /* free buffer array */
  free(core_audio_devout->buffer);

  /* free AgsAttack */
  free(core_audio_devout->attack);

  /* unref notify soundcard */
  if(core_audio_devout->notify_soundcard != NULL){
    if(core_audio_devout->application_context != NULL){
      ags_task_thread_remove_cyclic_task(AGS_APPLICATION_CONTEXT(core_audio_devout->application_context)->task_thread,
					 core_audio_devout->notify_soundcard);
    }
    
    g_object_unref(core_audio_devout->notify_soundcard);
  }

  /* application context */
  if(core_audio_devout->application_context != NULL){
    g_object_unref(core_audio_devout->application_context);
  }
  
  /* unref audio */
  if(core_audio_devout->audio != NULL){
    list = core_audio_devout->audio;

    while(list != NULL){
      g_object_set(G_OBJECT(list->data),
		   "soundcard", NULL,
		   NULL);
      
      list = list->next;
    }

    g_list_free_full(core_audio_devout->audio,
		     g_object_unref);
  }

  pthread_mutex_destroy(core_audio_devout->mutex);
  free(core_audio_devout->mutex);

  pthread_mutexattr_destroy(core_audio_devout->mutexattr);
  free(core_audio_devout->mutexattr);

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_devout_parent_class)->finalize(gobject);
}

void
ags_core_audio_devout_connect(AgsConnectable *connectable)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  GList *list;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(connectable);

  /*  */  
  list = core_audio_devout->audio;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_core_audio_devout_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_core_audio_devout_switch_buffer_flag:
 * @core_audio_devout: an #AgsCoreAudioDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 0.9.24
 */
void
ags_core_audio_devout_switch_buffer_flag(AgsCoreAudioDevout *core_audio_devout)
{
  if((AGS_CORE_AUDIO_DEVOUT_BUFFER0 & (core_audio_devout->flags)) != 0){
    core_audio_devout->flags &= (~AGS_CORE_AUDIO_DEVOUT_BUFFER0);
    core_audio_devout->flags |= AGS_CORE_AUDIO_DEVOUT_BUFFER1;
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER1 & (core_audio_devout->flags)) != 0){
    core_audio_devout->flags &= (~AGS_CORE_AUDIO_DEVOUT_BUFFER1);
    core_audio_devout->flags |= AGS_CORE_AUDIO_DEVOUT_BUFFER2;
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER2 & (core_audio_devout->flags)) != 0){
    core_audio_devout->flags &= (~AGS_CORE_AUDIO_DEVOUT_BUFFER2);
    core_audio_devout->flags |= AGS_CORE_AUDIO_DEVOUT_BUFFER3;
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER3 & (core_audio_devout->flags)) != 0){
    core_audio_devout->flags &= (~AGS_CORE_AUDIO_DEVOUT_BUFFER3);
    core_audio_devout->flags |= AGS_CORE_AUDIO_DEVOUT_BUFFER4;
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER4 & (core_audio_devout->flags)) != 0){
    core_audio_devout->flags &= (~AGS_CORE_AUDIO_DEVOUT_BUFFER4);
    core_audio_devout->flags |= AGS_CORE_AUDIO_DEVOUT_BUFFER5;
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER5 & (core_audio_devout->flags)) != 0){
    core_audio_devout->flags &= (~AGS_CORE_AUDIO_DEVOUT_BUFFER5);
    core_audio_devout->flags |= AGS_CORE_AUDIO_DEVOUT_BUFFER6;
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER6 & (core_audio_devout->flags)) != 0){
    core_audio_devout->flags &= (~AGS_CORE_AUDIO_DEVOUT_BUFFER6);
    core_audio_devout->flags |= AGS_CORE_AUDIO_DEVOUT_BUFFER7;
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER7 & (core_audio_devout->flags)) != 0){
    core_audio_devout->flags &= (~AGS_CORE_AUDIO_DEVOUT_BUFFER7);
    core_audio_devout->flags |= AGS_CORE_AUDIO_DEVOUT_BUFFER0;
  }
}

void
ags_core_audio_devout_set_application_context(AgsSoundcard *soundcard,
					      AgsApplicationContext *application_context)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  core_audio_devout->application_context = (GObject *) application_context;
}

AgsApplicationContext*
ags_core_audio_devout_get_application_context(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  return((AgsApplicationContext *) core_audio_devout->application_context);
}

void
ags_core_audio_devout_set_application_mutex(AgsSoundcard *soundcard,
					    pthread_mutex_t *application_mutex)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  core_audio_devout->application_mutex = application_mutex;
}

pthread_mutex_t*
ags_core_audio_devout_get_application_mutex(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  return(core_audio_devout->application_mutex);
}

void
ags_core_audio_devout_set_device(AgsSoundcard *soundcard,
				 gchar *device)
{
  AgsCoreAudioDevout *core_audio_devout;

  GList *core_audio_port, *core_audio_port_start;

  gchar *str;
  
  int ret;
  guint nth_card;
  guint i;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  if(core_audio_devout->card_uri == device ||
     !g_ascii_strcasecmp(core_audio_devout->card_uri,
			 device)){
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-core-audio-devout-")){
    g_warning("invalid core audio device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-core-audio-devout-%u",
	       &nth_card);

  if(ret != 1){
    g_warning("invalid core audio device specifier");

    return;
  }

  if(core_audio_devout->card_uri != NULL){
    g_free(core_audio_devout->card_uri);
  }
  
  core_audio_devout->card_uri = g_strdup(device);

  /* apply name to port */
  core_audio_port_start = 
    core_audio_port = g_list_copy(core_audio_devout->core_audio_port);
  
  str = g_strdup_printf("ags-soundcard%d",
			nth_card);
    
  g_object_set(core_audio_port->data,
	       "port-name", str,
	       NULL);
  g_free(str);

  g_list_free(core_audio_port_start);
}

gchar*
ags_core_audio_devout_get_device(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  return(core_audio_devout->card_uri);
}

void
ags_core_audio_devout_set_presets(AgsSoundcard *soundcard,
				  guint channels,
				  guint rate,
				  guint buffer_size,
				  guint format)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  g_object_set(core_audio_devout,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_core_audio_devout_get_presets(AgsSoundcard *soundcard,
				  guint *channels,
				  guint *rate,
				  guint *buffer_size,
				  guint *format)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  if(channels != NULL){
    *channels = core_audio_devout->pcm_channels;
  }

  if(rate != NULL){
    *rate = core_audio_devout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = core_audio_devout->buffer_size;
  }

  if(format != NULL){
    *format = core_audio_devout->format;
  }
}

/**
 * ags_core_audio_devout_list_cards:
 * @soundcard: the #AgsSoundcard
 * @card_id: core audio identifier
 * @card_name: card name
 *
 * List available soundcards.
 *
 * Since: 0.9.24
 */
void
ags_core_audio_devout_list_cards(AgsSoundcard *soundcard,
				 GList **card_id, GList **card_name)
{
  AgsCoreAudioDevout *core_audio_devout;

  AgsApplicationContext *application_context;
  
  GList *list, *list_start;

  pthread_mutex_t *application_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  application_context = (AgsApplicationContext *) core_audio_devout->application_context;

  if(application_context == NULL){
    return;
  }
  
  application_mutex = core_audio_devout->application_mutex;
  
  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  pthread_mutex_lock(application_mutex);

  list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  while(list != NULL){
    if(AGS_IS_CORE_AUDIO_DEVOUT(list->data)){
      if(card_id != NULL){
	if(AGS_CORE_AUDIO_DEVOUT(list->data)->card_uri != NULL){
	  *card_id = g_list_prepend(*card_id,
				    g_strdup(AGS_CORE_AUDIO_DEVOUT(list->data)->card_uri));
	}else{
	  *card_id = g_list_prepend(*card_id,
				    g_strdup("(null)"));

	  g_warning("ags_core_audio_devout_list_cards() - card id (null)");
	}
      }

      if(card_name != NULL){
	if(AGS_CORE_AUDIO_DEVOUT(list->data)->core_audio_client != NULL){
	  *card_name = g_list_prepend(*card_name,
				      g_strdup(AGS_CORE_AUDIO_CLIENT(AGS_CORE_AUDIO_DEVOUT(list->data)->core_audio_client)->name));
	}else{
	  *card_name = g_list_prepend(*card_name,
				      g_strdup("(null)"));

	  g_warning("ags_core_audio_devout_list_cards() - core audio client not connected (null)");
	}
      }      
    }

    list = list->next;
  }

  pthread_mutex_unlock(application_mutex);
  
  if(card_id != NULL && *card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL && *card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

void
ags_core_audio_devout_pcm_info(AgsSoundcard *soundcard,
			       char *card_id,
			       guint *channels_min, guint *channels_max,
			       guint *rate_min, guint *rate_max,
			       guint *buffer_size_min, guint *buffer_size_max,
			       GError **error)
{
  if(channels_min != NULL){
    *channels_min = 1;
  }

  if(channels_max != NULL){
    *channels_max = 1024;
  }

  if(rate_min != NULL){
    *rate_min = 8000;
  }

  if(rate_max != NULL){
    *rate_max = 192000;
  }

  if(buffer_size_min != NULL){
    *buffer_size_min = 64;
  }

  if(buffer_size_max != NULL){
    *buffer_size_max = 8192;
  }
}

gboolean
ags_core_audio_devout_is_starting(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  return((((AGS_CORE_AUDIO_DEVOUT_START_PLAY & (core_audio_devout->flags)) != 0) ? TRUE: FALSE));
}

gboolean
ags_core_audio_devout_is_playing(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  return((((AGS_CORE_AUDIO_DEVOUT_PLAY & (core_audio_devout->flags)) != 0) ? TRUE: FALSE));
}

gchar*
ags_core_audio_devout_get_uptime(AgsSoundcard *soundcard)
{
  gchar *uptime;

  if(ags_soundcard_is_playing(soundcard)){
    guint samplerate;
    guint buffer_size;

    guint note_offset;
    gdouble bpm;
    gdouble delay_factor;
    
    gdouble delay;

    ags_soundcard_get_presets(soundcard,
			      NULL,
			      &samplerate,
			      &buffer_size,
			      NULL);
    
    note_offset = ags_soundcard_get_note_offset_absolute(soundcard);

    bpm = ags_soundcard_get_bpm(soundcard);
    delay_factor = ags_soundcard_get_delay_factor(soundcard);

    /* calculate delays */
    delay = ags_soundcard_get_absolute_delay(soundcard);
  
    uptime = ags_time_get_uptime_from_offset(note_offset,
					     bpm,
					     delay,
					     delay_factor);
  }else{
    uptime = g_strdup(AGS_TIME_ZERO);
  }
  
  return(uptime);
}
  
void
ags_core_audio_devout_port_init(AgsSoundcard *soundcard,
				GError **error)
{
  AgsCoreAudioDevout *core_audio_devout;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  guint format, word_size;
  
  pthread_mutex_t *mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  application_context = ags_soundcard_get_application_context(soundcard);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) core_audio_devout);
  
  pthread_mutex_unlock(application_context->mutex);

  /* retrieve word size */
  pthread_mutex_lock(mutex);

  switch(core_audio_devout->format){
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
      //NOTE:JK: The 24-bit linear samples use 32-bit physical space
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
    
    g_warning("ags_core_audio_devout_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for playback */
  core_audio_devout->flags |= (AGS_CORE_AUDIO_DEVOUT_BUFFER7 |
			       AGS_CORE_AUDIO_DEVOUT_START_PLAY |
			       AGS_CORE_AUDIO_DEVOUT_PLAY |
			       AGS_CORE_AUDIO_DEVOUT_NONBLOCKING);

  memset(core_audio_devout->buffer[0], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->buffer[1], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->buffer[2], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->buffer[3], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->buffer[4], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->buffer[5], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->buffer[6], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->buffer[7], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);

  /*  */
  core_audio_devout->tact_counter = 0.0;
  core_audio_devout->delay_counter = 0.0;
  core_audio_devout->tic_counter = 0;

  core_audio_devout->flags |= (AGS_CORE_AUDIO_DEVOUT_INITIALIZED |
			       AGS_CORE_AUDIO_DEVOUT_START_PLAY |
			       AGS_CORE_AUDIO_DEVOUT_PLAY);
  
  g_atomic_int_and(&(core_audio_devout->sync_flags),
		   (~(AGS_CORE_AUDIO_DEVOUT_PASS_THROUGH)));
  g_atomic_int_or(&(core_audio_devout->sync_flags),
		  AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK);

  pthread_mutex_unlock(mutex);
}

void
ags_core_audio_devout_port_play(AgsSoundcard *soundcard,
				GError **error)
{
  AgsCoreAudioClient *core_audio_client;
  AgsCoreAudioDevout *core_audio_devout;

  AgsNotifySoundcard *notify_soundcard;
  
  AgsMutexManager *mutex_manager;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  guint word_size;
  gboolean core_audio_client_activated;

  pthread_mutex_t *mutex;
  pthread_mutex_t *client_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  application_context = ags_soundcard_get_application_context(soundcard);
  
  /* mutices */
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();
  task_thread = (AgsTaskThread *) application_context->task_thread;

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) core_audio_devout);
  
  pthread_mutex_unlock(application_context->mutex);

  /* client */
  pthread_mutex_lock(mutex);

  core_audio_client = (AgsCoreAudioClient *) core_audio_devout->core_audio_client;
  
  callback_mutex = core_audio_devout->callback_mutex;
  callback_finish_mutex = core_audio_devout->callback_finish_mutex;

  /* do playback */  
  core_audio_devout->flags &= (~AGS_CORE_AUDIO_DEVOUT_START_PLAY);
  notify_soundcard = AGS_NOTIFY_SOUNDCARD(core_audio_devout->notify_soundcard);
  
  if((AGS_CORE_AUDIO_DEVOUT_INITIALIZED & (core_audio_devout->flags)) == 0){
    pthread_mutex_unlock(mutex);
    
    return;
  }

  switch(core_audio_devout->format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      word_size = sizeof(signed short);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {      
      //NOTE:JK: The 24-bit linear samples use 32-bit physical space
      word_size = sizeof(signed long);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      word_size = sizeof(signed long);
    }
    break;
  default:
    pthread_mutex_unlock(mutex);
    
    g_warning("ags_core_audio_devout_port_play(): unsupported word size");
    
    return;
  }

  pthread_mutex_unlock(mutex);

  /*  */
  pthread_mutex_lock(application_context->mutex);
  
  client_mutex = ags_mutex_manager_lookup(mutex_manager,
					  (GObject *) core_audio_client);
  
  pthread_mutex_unlock(application_context->mutex);

  pthread_mutex_lock(client_mutex);

  core_audio_client_activated = ((AGS_CORE_AUDIO_CLIENT_ACTIVATED & (core_audio_client->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(client_mutex);

  if(core_audio_client_activated){
    /* signal */
    if((AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0){
      pthread_mutex_lock(callback_mutex);

      g_atomic_int_or(&(core_audio_devout->sync_flags),
		      AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE);
    
      if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	pthread_cond_signal(core_audio_devout->callback_cond);
      }

      pthread_mutex_unlock(callback_mutex);
    }
    
    /* wait callback */	
    if((AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0){
      pthread_mutex_lock(callback_finish_mutex);
    
      if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0){
	g_atomic_int_or(&(core_audio_devout->sync_flags),
			AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT);
    
	while((AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0 &&
	      (AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	  pthread_cond_wait(core_audio_devout->callback_finish_cond,
			    callback_finish_mutex);
	}
      }
    
      g_atomic_int_and(&(core_audio_devout->sync_flags),
		       (~(AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT |
			  AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE)));
    
      pthread_mutex_unlock(callback_finish_mutex);
    }else{
      g_atomic_int_and(&(core_audio_devout->sync_flags),
		       (~AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK));
    }
  }

  /* notify cyclic task */
  pthread_mutex_lock(notify_soundcard->return_mutex);

  g_atomic_int_or(&(notify_soundcard->flags),
		  AGS_NOTIFY_SOUNDCARD_DONE_RETURN);
  
  if((AGS_NOTIFY_SOUNDCARD_WAIT_RETURN & (g_atomic_int_get(&(notify_soundcard->flags)))) != 0){
    pthread_cond_signal(notify_soundcard->return_cond);
  }
  
  pthread_mutex_unlock(notify_soundcard->return_mutex);

  if(task_thread != NULL){
    AgsTicDevice *tic_device;
    AgsClearBuffer *clear_buffer;
    AgsSwitchBufferFlag *switch_buffer_flag;
      
    GList *task;
      
    task = NULL;
  
    /* tic soundcard */
    tic_device = ags_tic_device_new((GObject *) core_audio_devout);
    task = g_list_append(task,
    			 tic_device);

    /* reset - clear buffer */
    clear_buffer = ags_clear_buffer_new((GObject *) core_audio_devout);
    task = g_list_append(task,
			 clear_buffer);
    
    /* reset - switch buffer flags */
    switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) core_audio_devout);
    task = g_list_append(task,
			 switch_buffer_flag);

    /* append tasks */
    ags_task_thread_append_tasks((AgsTaskThread *) task_thread,
				 task);
  }else{
    guint nth_buffer;

    /* get buffer */  
    if((AGS_CORE_AUDIO_DEVOUT_BUFFER0 & (core_audio_devout->flags)) != 0){
      nth_buffer = 7;
    }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER1 & (core_audio_devout->flags)) != 0){
      nth_buffer = 0;
    }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER2 & (core_audio_devout->flags)) != 0){
      nth_buffer = 1;
    }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER3 & (core_audio_devout->flags)) != 0){
      nth_buffer = 2;
    }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER4 & (core_audio_devout->flags)) != 0){
      nth_buffer = 3;
    }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER5 & (core_audio_devout->flags)) != 0){
      nth_buffer = 4;
    }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER6 & (core_audio_devout->flags)) != 0){
      nth_buffer = 5;
    }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER7 & (core_audio_devout->flags)) != 0){
      nth_buffer = 6;
    }
    
    /* tic */
    ags_soundcard_tic(AGS_SOUNDCARD(core_audio_devout));

    switch(core_audio_devout->format){
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	word_size = sizeof(signed short);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	//NOTE:JK: The 24-bit linear samples use 32-bit physical space
	word_size = sizeof(signed long);
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	word_size = sizeof(signed long);
      }
      break;
    default:
      g_warning("ags_core_audio_devout_port_play(): unsupported word size");
      return;
    }
    
    memset(core_audio_devout->buffer[nth_buffer], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);

    /* reset - switch buffer flags */
    ags_core_audio_devout_switch_buffer_flag(core_audio_devout);
  }
}

void
ags_core_audio_devout_port_free(AgsSoundcard *soundcard)
{
  AgsCoreAudioPort *core_audio_port;
  AgsCoreAudioDevout *core_audio_devout;

  AgsNotifySoundcard *notify_soundcard;
  
  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  guint word_size;

  pthread_mutex_t *mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  application_context = ags_soundcard_get_application_context(soundcard);

  /*  */
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) core_audio_devout);
  
  pthread_mutex_unlock(application_context->mutex);

  /*  */
  pthread_mutex_lock(mutex);

  notify_soundcard = AGS_NOTIFY_SOUNDCARD(core_audio_devout->notify_soundcard);

  if((AGS_CORE_AUDIO_DEVOUT_INITIALIZED & (core_audio_devout->flags)) == 0){
    pthread_mutex_unlock(mutex);

    return;
  }

  g_object_ref(notify_soundcard);

  callback_mutex = core_audio_devout->callback_mutex;
  callback_finish_mutex = core_audio_devout->callback_finish_mutex;
  
  //  g_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);
  
  core_audio_devout->flags &= (~(AGS_CORE_AUDIO_DEVOUT_BUFFER0 |
				 AGS_CORE_AUDIO_DEVOUT_BUFFER1 |
				 AGS_CORE_AUDIO_DEVOUT_BUFFER2 |
				 AGS_CORE_AUDIO_DEVOUT_BUFFER3 |
				 AGS_CORE_AUDIO_DEVOUT_BUFFER4 |
				 AGS_CORE_AUDIO_DEVOUT_BUFFER5 |
				 AGS_CORE_AUDIO_DEVOUT_BUFFER6 |
				 AGS_CORE_AUDIO_DEVOUT_BUFFER7 |
				 AGS_CORE_AUDIO_DEVOUT_PLAY));

  g_atomic_int_or(&(core_audio_devout->sync_flags),
		  AGS_CORE_AUDIO_DEVOUT_PASS_THROUGH);
  g_atomic_int_and(&(core_audio_devout->sync_flags),
		   (~AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK));

  /* signal callback */
  pthread_mutex_lock(callback_mutex);

  g_atomic_int_or(&(core_audio_devout->sync_flags),
		  AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE);
    
  if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
    pthread_cond_signal(core_audio_devout->callback_cond);
  }

  pthread_mutex_unlock(callback_mutex);

  /* signal thread */
  pthread_mutex_lock(callback_finish_mutex);

  g_atomic_int_or(&(core_audio_devout->sync_flags),
		  AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE);
    
  if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
    pthread_cond_signal(core_audio_devout->callback_finish_cond);
  }

  pthread_mutex_unlock(callback_finish_mutex);

  /* notify cyclic task */
  pthread_mutex_lock(notify_soundcard->return_mutex);

  g_atomic_int_or(&(notify_soundcard->flags),
		  AGS_NOTIFY_SOUNDCARD_DONE_RETURN);
  
  if((AGS_NOTIFY_SOUNDCARD_WAIT_RETURN & (g_atomic_int_get(&(notify_soundcard->flags)))) != 0){
    pthread_cond_signal(notify_soundcard->return_cond);
  }
  
  pthread_mutex_unlock(notify_soundcard->return_mutex);

  g_object_unref(notify_soundcard);
  
  /*  */
  core_audio_devout->note_offset = 0;
  core_audio_devout->note_offset_absolute = 0;

  switch(core_audio_devout->format){
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
    word_size = 0;
    
    g_critical("ags_core_audio_devout_free(): unsupported word size");
  }

  pthread_mutex_unlock(mutex);

  if(core_audio_devout->core_audio_port != NULL){
    core_audio_port = core_audio_devout->core_audio_port->data;

    while(!g_atomic_int_get(&(core_audio_port->is_empty))) usleep(500000);
  }

  pthread_mutex_lock(mutex);
  
  memset(core_audio_devout->buffer[0], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->buffer[1], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->buffer[2], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->buffer[3], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->buffer[4], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->buffer[5], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->buffer[6], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->buffer[7], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);

  pthread_mutex_unlock(mutex);
}

void
ags_core_audio_devout_tic(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;
  gdouble delay;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* determine if attack should be switched */
  delay = core_audio_devout->delay[core_audio_devout->tic_counter];

  if((guint) core_audio_devout->delay_counter + 1 >= (guint) delay){
    if(core_audio_devout->do_loop &&
       core_audio_devout->note_offset + 1 == core_audio_devout->loop_right){
      ags_soundcard_set_note_offset(soundcard,
				    core_audio_devout->loop_left);
    }else{
      ags_soundcard_set_note_offset(soundcard,
				    core_audio_devout->note_offset + 1);
    }
    
    ags_soundcard_set_note_offset_absolute(soundcard,
					   core_audio_devout->note_offset_absolute + 1);
    
    /* delay */
    ags_soundcard_offset_changed(soundcard,
				 core_audio_devout->note_offset);
    
    /* reset - delay counter */
    core_audio_devout->delay_counter = 0.0;
    core_audio_devout->tact_counter += 1.0;
  }else{
    core_audio_devout->delay_counter += 1.0;
  }
}

void
ags_core_audio_devout_offset_changed(AgsSoundcard *soundcard,
				     guint note_offset)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  core_audio_devout->tic_counter += 1;

  if(core_audio_devout->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    core_audio_devout->tic_counter = 0;
  }
}

void
ags_core_audio_devout_set_bpm(AgsSoundcard *soundcard,
			      gdouble bpm)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  core_audio_devout->bpm = bpm;

  ags_core_audio_devout_adjust_delay_and_attack(core_audio_devout);
}

gdouble
ags_core_audio_devout_get_bpm(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  return(core_audio_devout->bpm);
}

void
ags_core_audio_devout_set_delay_factor(AgsSoundcard *soundcard,
				       gdouble delay_factor)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  core_audio_devout->delay_factor = delay_factor;

  ags_core_audio_devout_adjust_delay_and_attack(core_audio_devout);
}

gdouble
ags_core_audio_devout_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  return(core_audio_devout->delay_factor);
}

gdouble
ags_core_audio_devout_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  return((60.0 * (((gdouble) core_audio_devout->samplerate / (gdouble) core_audio_devout->buffer_size) / (gdouble) core_audio_devout->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) core_audio_devout->delay_factor))));
}

gdouble
ags_core_audio_devout_get_delay(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;
  guint index;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  index = core_audio_devout->tic_counter;
  
  return(core_audio_devout->delay[index]);
}

guint
ags_core_audio_devout_get_attack(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;
  guint index;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  index = core_audio_devout->tic_counter;
  
  return(core_audio_devout->attack[index]);
}

void*
ags_core_audio_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;
  void *buffer;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  if((AGS_CORE_AUDIO_DEVOUT_BUFFER0 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[0];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER1 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[1];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER2 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[2];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER3 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[3];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER4 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[4];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER5 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[5];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER6 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[6];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER7 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[7];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_core_audio_devout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;
  void *buffer;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  if((AGS_CORE_AUDIO_DEVOUT_BUFFER0 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[1];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER1 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[2];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER2 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[3];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER3 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[4];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER4 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[5];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER5 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[6];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER6 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[7];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER7 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_core_audio_devout_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;
  void *buffer;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  if((AGS_CORE_AUDIO_DEVOUT_BUFFER0 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[7];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER1 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[0];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER2 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[1];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER3 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[2];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER4 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[3];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER5 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[4];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER6 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[5];
  }else if((AGS_CORE_AUDIO_DEVOUT_BUFFER7 & (core_audio_devout->flags)) != 0){
    buffer = core_audio_devout->buffer[6];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

guint
ags_core_audio_devout_get_delay_counter(AgsSoundcard *soundcard)
{
  return(AGS_CORE_AUDIO_DEVOUT(soundcard)->delay_counter);
}

void
ags_core_audio_devout_set_note_offset(AgsSoundcard *soundcard,
				      guint note_offset)
{
  AGS_CORE_AUDIO_DEVOUT(soundcard)->note_offset = note_offset;
}

guint
ags_core_audio_devout_get_note_offset(AgsSoundcard *soundcard)
{
  return(AGS_CORE_AUDIO_DEVOUT(soundcard)->note_offset);
}

void
ags_core_audio_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
					       guint note_offset)
{
  AGS_CORE_AUDIO_DEVOUT(soundcard)->note_offset_absolute = note_offset;
}

guint
ags_core_audio_devout_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  return(AGS_CORE_AUDIO_DEVOUT(soundcard)->note_offset_absolute);
}

void
ags_core_audio_devout_set_loop(AgsSoundcard *soundcard,
			       guint loop_left, guint loop_right,
			       gboolean do_loop)
{
  AGS_CORE_AUDIO_DEVOUT(soundcard)->loop_left = loop_left;
  AGS_CORE_AUDIO_DEVOUT(soundcard)->loop_right = loop_right;
  AGS_CORE_AUDIO_DEVOUT(soundcard)->do_loop = do_loop;

  if(do_loop){
    AGS_CORE_AUDIO_DEVOUT(soundcard)->loop_offset = AGS_CORE_AUDIO_DEVOUT(soundcard)->note_offset;
  }
}

void
ags_core_audio_devout_get_loop(AgsSoundcard *soundcard,
			       guint *loop_left, guint *loop_right,
			       gboolean *do_loop)  
{
  if(loop_left != NULL){
    *loop_left = AGS_CORE_AUDIO_DEVOUT(soundcard)->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = AGS_CORE_AUDIO_DEVOUT(soundcard)->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = AGS_CORE_AUDIO_DEVOUT(soundcard)->do_loop;
  }
}

guint
ags_core_audio_devout_get_loop_offset(AgsSoundcard *soundcard)  
{
  return(AGS_CORE_AUDIO_DEVOUT(soundcard)->loop_offset);
}

void
ags_core_audio_devout_set_audio(AgsSoundcard *soundcard,
				GList *audio)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  core_audio_devout->audio = audio;
}

GList*
ags_core_audio_devout_get_audio(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  return(core_audio_devout->audio);
}

/**
 * ags_core_audio_devout_adjust_delay_and_attack:
 * @core_audio_devout: the #AgsCoreAudioDevout
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 0.9.24
 */
void
ags_core_audio_devout_adjust_delay_and_attack(AgsCoreAudioDevout *core_audio_devout)
{
  gdouble delay;
  guint default_tact_frames;
  guint delay_tact_frames;
  guint default_period;
  gint next_attack;
  guint i;

  if(core_audio_devout == NULL){
    return;
  }
  
  delay = ags_core_audio_devout_get_absolute_delay(AGS_SOUNDCARD(core_audio_devout));

#ifdef AGS_DEBUG
  g_message("delay : %f", delay);
#endif
  
  default_tact_frames = (guint) (delay * core_audio_devout->buffer_size);
  delay_tact_frames = (guint) (floor(delay) * core_audio_devout->buffer_size);
  default_period = (1.0 / AGS_SOUNDCARD_DEFAULT_PERIOD) * (default_tact_frames);

  i = 0;
  
  core_audio_devout->attack[0] = (guint) floor(0.25 * core_audio_devout->buffer_size);
  next_attack = (((core_audio_devout->attack[i] + default_tact_frames) / core_audio_devout->buffer_size) - delay) * core_audio_devout->buffer_size;

  if(next_attack < 0){
    next_attack = 0;
  }

  if(next_attack >= core_audio_devout->buffer_size){
    next_attack = core_audio_devout->buffer_size - 1;
  }
  
  /* check if delay drops for next attack */
  if(next_attack < 0){
    core_audio_devout->attack[i] = core_audio_devout->attack[i] - ((gdouble) next_attack / 2.0);

    if(core_audio_devout->attack[i] < 0){
      core_audio_devout->attack[i] = 0;
    }
    
    if(core_audio_devout->attack[i] >= core_audio_devout->buffer_size){
      core_audio_devout->attack[i] = core_audio_devout->buffer_size - 1;
    }
    
    next_attack = next_attack + (next_attack / 2.0);

    if(next_attack < 0){
      next_attack = 0;
    }

    if(next_attack >= core_audio_devout->buffer_size){
      next_attack = core_audio_devout->buffer_size - 1;
    }
  }
  
  for(i = 1; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    core_audio_devout->attack[i] = next_attack;
    next_attack = (((core_audio_devout->attack[i] + default_tact_frames) / core_audio_devout->buffer_size) - delay) * core_audio_devout->buffer_size;

    if(next_attack >= core_audio_devout->buffer_size){
      next_attack = core_audio_devout->buffer_size - 1;
    }
    
    /* check if delay drops for next attack */
    if(next_attack < 0){
      core_audio_devout->attack[i] = core_audio_devout->attack[i] - ((gdouble) next_attack / 2.0);

      if(core_audio_devout->attack[i] < 0){
	core_audio_devout->attack[i] = 0;
      }

      if(core_audio_devout->attack[i] >= core_audio_devout->buffer_size){
	core_audio_devout->attack[i] = core_audio_devout->buffer_size - 1;
      }
    
      next_attack = next_attack + (next_attack / 2.0);

      if(next_attack < 0){
	next_attack = 0;
      }

      if(next_attack >= core_audio_devout->buffer_size){
	next_attack = core_audio_devout->buffer_size - 1;
      }
    }
    
#ifdef AGS_DEBUG
    g_message("%d", core_audio_devout->attack[i]);
#endif
  }

  core_audio_devout->attack[0] = core_audio_devout->attack[i - 2];
  
  for(i = 0; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD - 1; i++){
    core_audio_devout->delay[i] = ((gdouble) (default_tact_frames + core_audio_devout->attack[i] - core_audio_devout->attack[i + 1])) / (gdouble) core_audio_devout->buffer_size;
    
#ifdef AGS_DEBUG
    g_message("%f", core_audio_devout->delay[i]);
#endif
  }

  core_audio_devout->delay[i] = ((gdouble) (default_tact_frames + core_audio_devout->attack[i] - core_audio_devout->attack[0])) / (gdouble) core_audio_devout->buffer_size;
}

/**
 * ags_core_audio_devout_realloc_buffer:
 * @core_audio_devout: the #AgsCoreAudioDevout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 0.9.24
 */
void
ags_core_audio_devout_realloc_buffer(AgsCoreAudioDevout *core_audio_devout)
{
  guint word_size;

  if(core_audio_devout == NULL){
    return;
  }

  switch(core_audio_devout->format){
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
    g_warning("ags_core_audio_devout_realloc_buffer(): unsupported word size");
    return;
  }
  
  /* AGS_CORE_AUDIO_DEVOUT_BUFFER_0 */
  if(core_audio_devout->buffer[0] != NULL){
    free(core_audio_devout->buffer[0]);
  }
  
  core_audio_devout->buffer[0] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_BUFFER_1 */
  if(core_audio_devout->buffer[1] != NULL){
    free(core_audio_devout->buffer[1]);
  }

  core_audio_devout->buffer[1] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_BUFFER_2 */
  if(core_audio_devout->buffer[2] != NULL){
    free(core_audio_devout->buffer[2]);
  }

  core_audio_devout->buffer[2] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_BUFFER_3 */
  if(core_audio_devout->buffer[3] != NULL){
    free(core_audio_devout->buffer[3]);
  }
  
  core_audio_devout->buffer[3] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);

  /* AGS_CORE_AUDIO_DEVOUT_BUFFER_4 */
  if(core_audio_devout->buffer[4] != NULL){
    free(core_audio_devout->buffer[4]);
  }
  
  core_audio_devout->buffer[4] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);

  /* AGS_CORE_AUDIO_DEVOUT_BUFFER_5 */
  if(core_audio_devout->buffer[5] != NULL){
    free(core_audio_devout->buffer[5]);
  }
  
  core_audio_devout->buffer[5] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);

  /* AGS_CORE_AUDIO_DEVOUT_BUFFER_6 */
  if(core_audio_devout->buffer[6] != NULL){
    free(core_audio_devout->buffer[6]);
  }
  
  core_audio_devout->buffer[6] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);

  /* AGS_CORE_AUDIO_DEVOUT_BUFFER_7 */
  if(core_audio_devout->buffer[7] != NULL){
    free(core_audio_devout->buffer[7]);
  }
  
  core_audio_devout->buffer[7] = (void *) malloc(core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
}

/**
 * ags_core_audio_devout_new:
 * @application_context: the #AgsApplicationContext
 *
 * Creates an #AgsCoreAudioDevout, refering to @application_context.
 *
 * Returns: a new #AgsCoreAudioDevout
 *
 * Since: 0.9.24
 */
AgsCoreAudioDevout*
ags_core_audio_devout_new(GObject *application_context)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = (AgsCoreAudioDevout *) g_object_new(AGS_TYPE_CORE_AUDIO_DEVOUT,
							  "application-context", application_context,
							  NULL);
  
  return(core_audio_devout);
}
