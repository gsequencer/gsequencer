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

#include <ags/audio/jack/ags_jack_devout.h>

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

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_port.h>

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

void ags_jack_devout_class_init(AgsJackDevoutClass *jack_devout);
void ags_jack_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_jack_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_jack_devout_init(AgsJackDevout *jack_devout);
void ags_jack_devout_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_jack_devout_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_jack_devout_disconnect(AgsConnectable *connectable);
void ags_jack_devout_connect(AgsConnectable *connectable);
void ags_jack_devout_dispose(GObject *gobject);
void ags_jack_devout_finalize(GObject *gobject);

void ags_jack_devout_set_application_context(AgsSoundcard *soundcard,
					     AgsApplicationContext *application_context);
AgsApplicationContext* ags_jack_devout_get_application_context(AgsSoundcard *soundcard);

void ags_jack_devout_set_application_mutex(AgsSoundcard *soundcard,
					   pthread_mutex_t *application_mutex);
pthread_mutex_t* ags_jack_devout_get_application_mutex(AgsSoundcard *soundcard);

void ags_jack_devout_set_device(AgsSoundcard *soundcard,
				gchar *device);
gchar* ags_jack_devout_get_device(AgsSoundcard *soundcard);

void ags_jack_devout_set_presets(AgsSoundcard *soundcard,
				 guint channels,
				 guint rate,
				 guint buffer_size,
				 guint format);
void ags_jack_devout_get_presets(AgsSoundcard *soundcard,
				 guint *channels,
				 guint *rate,
				 guint *buffer_size,
				 guint *format);

void ags_jack_devout_list_cards(AgsSoundcard *soundcard,
				GList **card_id, GList **card_name);
void ags_jack_devout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			      guint *channels_min, guint *channels_max,
			      guint *rate_min, guint *rate_max,
			      guint *buffer_size_min, guint *buffer_size_max,
			      GError **error);

gboolean ags_jack_devout_is_starting(AgsSoundcard *soundcard);
gboolean ags_jack_devout_is_playing(AgsSoundcard *soundcard);

gchar* ags_jack_devout_get_uptime(AgsSoundcard *soundcard);

void ags_jack_devout_port_init(AgsSoundcard *soundcard,
			       GError **error);
void ags_jack_devout_port_play(AgsSoundcard *soundcard,
			       GError **error);
void ags_jack_devout_port_free(AgsSoundcard *soundcard);

void ags_jack_devout_tic(AgsSoundcard *soundcard);
void ags_jack_devout_offset_changed(AgsSoundcard *soundcard,
				    guint note_offset);

void ags_jack_devout_set_bpm(AgsSoundcard *soundcard,
			     gdouble bpm);
gdouble ags_jack_devout_get_bpm(AgsSoundcard *soundcard);

void ags_jack_devout_set_delay_factor(AgsSoundcard *soundcard,
				      gdouble delay_factor);
gdouble ags_jack_devout_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_jack_devout_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_jack_devout_get_delay(AgsSoundcard *soundcard);
guint ags_jack_devout_get_attack(AgsSoundcard *soundcard);

void* ags_jack_devout_get_buffer(AgsSoundcard *soundcard);
void* ags_jack_devout_get_next_buffer(AgsSoundcard *soundcard);
void* ags_jack_devout_get_prev_buffer(AgsSoundcard *soundcard);

guint ags_jack_devout_get_delay_counter(AgsSoundcard *soundcard);

void ags_jack_devout_set_note_offset(AgsSoundcard *soundcard,
				     guint note_offset);
guint ags_jack_devout_get_note_offset(AgsSoundcard *soundcard);

void ags_jack_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
					      guint note_offset);
guint ags_jack_devout_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_jack_devout_set_loop(AgsSoundcard *soundcard,
			      guint loop_left, guint loop_right,
			      gboolean do_loop);
void ags_jack_devout_get_loop(AgsSoundcard *soundcard,
			      guint *loop_left, guint *loop_right,
			      gboolean *do_loop);

guint ags_jack_devout_get_loop_offset(AgsSoundcard *soundcard);

void ags_jack_devout_set_audio(AgsSoundcard *soundcard,
			       GList *audio);
GList* ags_jack_devout_get_audio(AgsSoundcard *soundcard);

/**
 * SECTION:ags_jack_devout
 * @short_description: Output to soundcard
 * @title: AgsJackDevout
 * @section_id:
 * @include: ags/audio/jack/ags_jack_devout.h
 *
 * #AgsJackDevout represents a soundcard and supports output.
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
  PROP_JACK_CLIENT,
  PROP_JACK_PORT,
  PROP_CHANNEL,
};

enum{
  LAST_SIGNAL,
};

static gpointer ags_jack_devout_parent_class = NULL;
static guint jack_devout_signals[LAST_SIGNAL];

GType
ags_jack_devout_get_type (void)
{
  static GType ags_type_jack_devout = 0;

  if(!ags_type_jack_devout){
    static const GTypeInfo ags_jack_devout_info = {
      sizeof (AgsJackDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsJackDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_jack_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_jack_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_jack_devout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_jack_devout = g_type_register_static(G_TYPE_OBJECT,
						  "AgsJackDevout",
						  &ags_jack_devout_info,
						  0);

    g_type_add_interface_static(ags_type_jack_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_jack_devout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);
  }

  return (ags_type_jack_devout);
}

void
ags_jack_devout_class_init(AgsJackDevoutClass *jack_devout)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_jack_devout_parent_class = g_type_class_peek_parent(jack_devout);

  /* GObjectClass */
  gobject = (GObjectClass *) jack_devout;

  gobject->set_property = ags_jack_devout_set_property;
  gobject->get_property = ags_jack_devout_get_property;

  gobject->dispose = ags_jack_devout_dispose;
  gobject->finalize = ags_jack_devout_finalize;

  /* properties */
  /**
   * AgsJackDevout:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 1.0.0
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
   * AgsJackDevout:application-mutex:
   *
   * The assigned application mutex
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_pointer("application-mutex",
				    i18n_pspec("the application mutex object"),
				    i18n_pspec("The application mutex object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_MUTEX,
				  param_spec);

  /**
   * AgsJackDevout:device:
   *
   * The jack soundcard indentifier
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-jack-devout-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsJackDevout:dsp-channels:
   *
   * The dsp channel count
   * 
   * Since: 1.0.0
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
   * AgsJackDevout:pcm-channels:
   *
   * The pcm channel count
   * 
   * Since: 1.0.0
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
   * AgsJackDevout:format:
   *
   * The precision of the buffer
   * 
   * Since: 1.0.0
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
   * AgsJackDevout:buffer-size:
   *
   * The buffer size
   * 
   * Since: 1.0.0
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
   * AgsJackDevout:samplerate:
   *
   * The samplerate
   * 
   * Since: 1.0.0
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
   * AgsJackDevout:buffer:
   *
   * The buffer
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to play"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsJackDevout:bpm:
   *
   * Beats per minute
   * 
   * Since: 1.0.0
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
   * AgsJackDevout:delay-factor:
   *
   * tact
   * 
   * Since: 1.0.0
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
   * AgsJackDevout:attack:
   *
   * Attack of the buffer
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_pointer("attack",
				    i18n_pspec("attack of buffer"),
				    i18n_pspec("The attack to use for the buffer"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  
  /**
   * AgsJackDevout:jack-client:
   *
   * The assigned #AgsJackClient
   * 
   * Since: 1.0.0
   */
  param_spec = g_param_spec_object("jack-client",
				   i18n_pspec("jack client object"),
				   i18n_pspec("The jack client object"),
				   AGS_TYPE_JACK_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_JACK_CLIENT,
				  param_spec);

  /**
   * AgsJackDevout:jack-port:
   *
   * The assigned #AgsJackPort
   * 
   * Since: 1.0.0.7
   */
  param_spec = g_param_spec_pointer("jack-port",
				    i18n_pspec("jack port object"),
				    i18n_pspec("The jack port object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_JACK_PORT,
				  param_spec);

  /**
   * AgsJackDevout:channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 1.0.0
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
ags_jack_devout_error_quark()
{
  return(g_quark_from_static_string("ags-jack_devout-error-quark"));
}

void
ags_jack_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_jack_devout_connect;
  connectable->disconnect = ags_jack_devout_disconnect;
}

void
ags_jack_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_application_context = ags_jack_devout_set_application_context;
  soundcard->get_application_context = ags_jack_devout_get_application_context;

  soundcard->set_application_mutex = ags_jack_devout_set_application_mutex;
  soundcard->get_application_mutex = ags_jack_devout_get_application_mutex;

  soundcard->set_device = ags_jack_devout_set_device;
  soundcard->get_device = ags_jack_devout_get_device;
  
  soundcard->set_presets = ags_jack_devout_set_presets;
  soundcard->get_presets = ags_jack_devout_get_presets;

  soundcard->list_cards = ags_jack_devout_list_cards;
  soundcard->pcm_info = ags_jack_devout_pcm_info;

  soundcard->get_poll_fd = NULL;
  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_jack_devout_is_starting;
  soundcard->is_playing = ags_jack_devout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_jack_devout_get_uptime;
  
  soundcard->play_init = ags_jack_devout_port_init;
  soundcard->play = ags_jack_devout_port_play;

  soundcard->record_init = NULL;
  soundcard->record = NULL;

  soundcard->stop = ags_jack_devout_port_free;

  soundcard->tic = ags_jack_devout_tic;
  soundcard->offset_changed = ags_jack_devout_offset_changed;
    
  soundcard->set_bpm = ags_jack_devout_set_bpm;
  soundcard->get_bpm = ags_jack_devout_get_bpm;

  soundcard->set_delay_factor = ags_jack_devout_set_delay_factor;
  soundcard->get_delay_factor = ags_jack_devout_get_delay_factor;
  
  soundcard->get_absolute_delay = ags_jack_devout_get_absolute_delay;

  soundcard->get_delay = ags_jack_devout_get_delay;
  soundcard->get_attack = ags_jack_devout_get_attack;

  soundcard->get_buffer = ags_jack_devout_get_buffer;
  soundcard->get_next_buffer = ags_jack_devout_get_next_buffer;
  soundcard->get_prev_buffer = ags_jack_devout_get_prev_buffer;

  soundcard->get_delay_counter = ags_jack_devout_get_delay_counter;

  soundcard->set_note_offset = ags_jack_devout_set_note_offset;
  soundcard->get_note_offset = ags_jack_devout_get_note_offset;

  soundcard->set_note_offset_absolute = ags_jack_devout_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_jack_devout_get_note_offset_absolute;

  soundcard->set_loop = ags_jack_devout_set_loop;
  soundcard->get_loop = ags_jack_devout_get_loop;

  soundcard->get_loop_offset = ags_jack_devout_get_loop_offset;

  soundcard->set_audio = ags_jack_devout_set_audio;
  soundcard->get_audio = ags_jack_devout_get_audio;
}

void
ags_jack_devout_init(AgsJackDevout *jack_devout)
{
  AgsMutexManager *mutex_manager;

  AgsConfig *config;

  gchar *str;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert devout mutex */
  jack_devout->mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  jack_devout->mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) jack_devout,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  jack_devout->flags = 0;
  g_atomic_int_set(&(jack_devout->sync_flags),
		   AGS_JACK_DEVOUT_PASS_THROUGH);

  /* quality */
  config = ags_config_get_instance();

  jack_devout->dsp_channels = AGS_SOUNDCARD_DEFAULT_DSP_CHANNELS;
  jack_devout->pcm_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
  jack_devout->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  jack_devout->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  jack_devout->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

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
    jack_devout->dsp_channels = g_ascii_strtoull(str,
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
    jack_devout->pcm_channels = g_ascii_strtoull(str,
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
    jack_devout->samplerate = g_ascii_strtoull(str,
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
    jack_devout->buffer_size = g_ascii_strtoull(str,
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
    jack_devout->format = g_ascii_strtoull(str,
					   NULL,
					   10);
    free(str);
  }

  /*  */
  jack_devout->card_uri = NULL;
  jack_devout->jack_client = NULL;

  jack_devout->port_name = NULL;
  jack_devout->jack_port = NULL;

  /* buffer */
  jack_devout->buffer = (void **) malloc(4 * sizeof(void*));

  jack_devout->buffer[0] = (void *) malloc(jack_devout->pcm_channels * jack_devout->buffer_size * sizeof(signed short));
  jack_devout->buffer[1] = (void *) malloc(jack_devout->pcm_channels * jack_devout->buffer_size * sizeof(signed short));
  jack_devout->buffer[2] = (void *) malloc(jack_devout->pcm_channels * jack_devout->buffer_size * sizeof(signed short));
  jack_devout->buffer[3] = (void *) malloc(jack_devout->pcm_channels * jack_devout->buffer_size * sizeof(signed short));
  
  ags_jack_devout_realloc_buffer(jack_devout);
  
  /* bpm */
  jack_devout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  jack_devout->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* delay and attack */
  jack_devout->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					  sizeof(gdouble));
  
  jack_devout->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					 sizeof(guint));

  ags_jack_devout_adjust_delay_and_attack(jack_devout);
  
  /* counters */
  jack_devout->tact_counter = 0.0;
  jack_devout->delay_counter = 0;
  jack_devout->tic_counter = 0;

  jack_devout->note_offset = 0;
  jack_devout->note_offset_absolute = 0;

  jack_devout->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  jack_devout->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  jack_devout->do_loop = FALSE;

  jack_devout->loop_offset = 0;

  /* callback mutex */
  jack_devout->callback_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(jack_devout->callback_mutex,
		     NULL);

  jack_devout->callback_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(jack_devout->callback_cond, NULL);

  /* callback finish mutex */
  jack_devout->callback_finish_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(jack_devout->callback_finish_mutex,
		     NULL);

  jack_devout->callback_finish_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(jack_devout->callback_finish_cond, NULL);

  /* parent */
  jack_devout->application_context = NULL;
  jack_devout->application_mutex = NULL;

  /*  */
  jack_devout->notify_soundcard = NULL;
  
  /* all AgsAudio */
  jack_devout->audio = NULL;
}

void
ags_jack_devout_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if(jack_devout->application_context == (GObject *) application_context){
	return;
      }

      if(jack_devout->application_context != NULL){
	g_object_unref(G_OBJECT(jack_devout->application_context));
      }

      if(application_context != NULL){
	AgsConfig *config;

	gchar *segmentation;
	guint denumerator, numerator;
	
	g_object_ref(G_OBJECT(application_context));

	jack_devout->application_mutex = application_context->mutex;

	config = ags_config_get_instance();

	/* segmentation */
	segmentation = ags_config_get_value(config,
					    AGS_CONFIG_GENERIC,
					    "segmentation");

	if(segmentation != NULL){
	  sscanf(segmentation, "%d/%d",
		 &denumerator,
		 &numerator);
    
	  jack_devout->delay_factor = 1.0 / numerator * (numerator / denumerator);
	}
	
	ags_jack_devout_adjust_delay_and_attack(jack_devout);
	ags_jack_devout_realloc_buffer(jack_devout);
      }else{
	jack_devout->application_mutex = NULL;
      }

      jack_devout->application_context = (GObject *) application_context;
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      pthread_mutex_t *application_mutex;

      application_mutex = (pthread_mutex_t *) g_value_get_pointer(value);

      if(jack_devout->application_mutex == application_mutex){
	return;
      }
      
      jack_devout->application_mutex = application_mutex;
    }
    break;
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      jack_devout->card_uri = g_strdup(device);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      if(dsp_channels == jack_devout->dsp_channels){
	return;
      }

      jack_devout->dsp_channels = dsp_channels;
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      if(pcm_channels == jack_devout->pcm_channels){
	return;
      }

      jack_devout->pcm_channels = pcm_channels;

      ags_jack_devout_realloc_buffer(jack_devout);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      if(format == jack_devout->format){
	return;
      }

      jack_devout->format = format;

      ags_jack_devout_realloc_buffer(jack_devout);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      if(buffer_size == jack_devout->buffer_size){
	return;
      }

      jack_devout->buffer_size = buffer_size;

      ags_jack_devout_realloc_buffer(jack_devout);
      ags_jack_devout_adjust_delay_and_attack(jack_devout);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      if(samplerate == jack_devout->samplerate){
	return;
      }

      jack_devout->samplerate = samplerate;

      ags_jack_devout_realloc_buffer(jack_devout);
      ags_jack_devout_adjust_delay_and_attack(jack_devout);
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

      jack_devout->bpm = bpm;

      ags_jack_devout_adjust_delay_and_attack(jack_devout);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      jack_devout->delay_factor = delay_factor;

      ags_jack_devout_adjust_delay_and_attack(jack_devout);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      AgsJackClient *jack_client;

      jack_client = (AgsJackClient *) g_value_get_object(value);

      if(jack_devout->jack_client == (GObject *) jack_client){
	return;
      }

      if(jack_devout->jack_client != NULL){
	g_object_unref(G_OBJECT(jack_devout->jack_client));
      }

      if(jack_client != NULL){
	g_object_ref(jack_client);
      }
      
      jack_devout->jack_client = (GObject *) jack_client;
    }
    break;
  case PROP_JACK_PORT:
    {
      AgsJackPort *jack_port;

      jack_port = (AgsJackPort *) g_value_get_pointer(value);

      if(!AGS_IS_JACK_PORT(jack_port) ||
	 g_list_find(jack_devout->jack_port, jack_port) != NULL){
	return;
      }

      if(jack_port != NULL){
	g_object_ref(jack_port);
	jack_devout->jack_port = g_list_append(jack_devout->jack_port,
					       jack_port);
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_devout_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, jack_devout->application_context);
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      g_value_set_pointer(value, jack_devout->application_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      g_value_set_string(value, jack_devout->card_uri);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_value_set_uint(value, jack_devout->dsp_channels);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_value_set_uint(value, jack_devout->pcm_channels);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, jack_devout->format);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, jack_devout->buffer_size);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, jack_devout->samplerate);
    }
    break;
  case PROP_BUFFER:
    {
      g_value_set_pointer(value, jack_devout->buffer);
    }
    break;
  case PROP_BPM:
    {
      g_value_set_double(value, jack_devout->bpm);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_value_set_double(value, jack_devout->delay_factor);
    }
    break;
  case PROP_ATTACK:
    {
      g_value_set_pointer(value, jack_devout->attack);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      g_value_set_object(value, jack_devout->jack_client);
    }
    break;
  case PROP_JACK_PORT:
    {
      g_value_set_pointer(value,
			  g_list_copy(jack_devout->jack_port));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_devout_dispose(GObject *gobject)
{
  AgsJackDevout *jack_devout;

  GList *list;

  jack_devout = AGS_JACK_DEVOUT(gobject);

  /* application context */
  if(jack_devout->application_context != NULL){
    g_object_unref(jack_devout->application_context);

    jack_devout->application_context = NULL;
  }

  /* unref audio */
  if(jack_devout->audio != NULL){
    list = jack_devout->audio;

    while(list != NULL){
      g_object_set(G_OBJECT(list->data),
		   "soundcard", NULL,
		   NULL);
      
      list = list->next;
    }

    g_list_free_full(jack_devout->audio,
		     g_object_unref);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_jack_devout_parent_class)->dispose(gobject);
}

void
ags_jack_devout_finalize(GObject *gobject)
{
  AgsJackDevout *jack_devout;

  AgsMutexManager *mutex_manager;
  
  GList *list, *list_next;

  jack_devout = AGS_JACK_DEVOUT(gobject);

  /* remove jack_devout mutex */
  pthread_mutex_lock(jack_devout->application_mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(jack_devout->application_mutex);

  /* free output buffer */
  free(jack_devout->buffer[0]);
  free(jack_devout->buffer[1]);
  free(jack_devout->buffer[2]);
  free(jack_devout->buffer[3]);

  /* free buffer array */
  free(jack_devout->buffer);

  /* free AgsAttack */
  free(jack_devout->attack);

  /* unref notify soundcard */
  if(jack_devout->notify_soundcard != NULL){
    if(jack_devout->application_context != NULL){
      ags_task_thread_remove_cyclic_task(AGS_APPLICATION_CONTEXT(jack_devout->application_context)->task_thread,
					 jack_devout->notify_soundcard);
    }
    
    g_object_unref(jack_devout->notify_soundcard);
  }

  /* application context */
  if(jack_devout->application_context != NULL){
    g_object_unref(jack_devout->application_context);
  }
  
  /* unref audio */
  if(jack_devout->audio != NULL){
    list = jack_devout->audio;

    while(list != NULL){
      g_object_set(G_OBJECT(list->data),
		   "soundcard", NULL,
		   NULL);
      
      list = list->next;
    }

    g_list_free_full(jack_devout->audio,
		     g_object_unref);
  }

  pthread_mutex_destroy(jack_devout->mutex);
  free(jack_devout->mutex);

  pthread_mutexattr_destroy(jack_devout->mutexattr);
  free(jack_devout->mutexattr);

  /* call parent */
  G_OBJECT_CLASS(ags_jack_devout_parent_class)->finalize(gobject);
}

void
ags_jack_devout_connect(AgsConnectable *connectable)
{
  AgsJackDevout *jack_devout;
  
  GList *list;

  jack_devout = AGS_JACK_DEVOUT(connectable);

  /*  */  
  list = jack_devout->audio;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_jack_devout_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_jack_devout_switch_buffer_flag:
 * @jack_devout: an #AgsJackDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 1.0.0
 */
void
ags_jack_devout_switch_buffer_flag(AgsJackDevout *jack_devout)
{
  if((AGS_JACK_DEVOUT_BUFFER0 & (jack_devout->flags)) != 0){
    jack_devout->flags &= (~AGS_JACK_DEVOUT_BUFFER0);
    jack_devout->flags |= AGS_JACK_DEVOUT_BUFFER1;
  }else if((AGS_JACK_DEVOUT_BUFFER1 & (jack_devout->flags)) != 0){
    jack_devout->flags &= (~AGS_JACK_DEVOUT_BUFFER1);
    jack_devout->flags |= AGS_JACK_DEVOUT_BUFFER2;
  }else if((AGS_JACK_DEVOUT_BUFFER2 & (jack_devout->flags)) != 0){
    jack_devout->flags &= (~AGS_JACK_DEVOUT_BUFFER2);
    jack_devout->flags |= AGS_JACK_DEVOUT_BUFFER3;
  }else if((AGS_JACK_DEVOUT_BUFFER3 & (jack_devout->flags)) != 0){
    jack_devout->flags &= (~AGS_JACK_DEVOUT_BUFFER3);
    jack_devout->flags |= AGS_JACK_DEVOUT_BUFFER0;
  }
}

void
ags_jack_devout_set_application_context(AgsSoundcard *soundcard,
					AgsApplicationContext *application_context)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  jack_devout->application_context = (GObject *) application_context;
}

AgsApplicationContext*
ags_jack_devout_get_application_context(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  return((AgsApplicationContext *) jack_devout->application_context);
}

void
ags_jack_devout_set_application_mutex(AgsSoundcard *soundcard,
				      pthread_mutex_t *application_mutex)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  jack_devout->application_mutex = application_mutex;
}

pthread_mutex_t*
ags_jack_devout_get_application_mutex(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  return(jack_devout->application_mutex);
}

void
ags_jack_devout_set_device(AgsSoundcard *soundcard,
			   gchar *device)
{
  AgsJackDevout *jack_devout;

  GList *jack_port, *jack_port_start;

  gchar *str;
  
  int ret;
  guint nth_card;
  guint i;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  if(jack_devout->card_uri == device ||
     !g_ascii_strcasecmp(jack_devout->card_uri,
			 device)){
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-jack-devout-")){
    g_warning("invalid JACK device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-jack-devout-%u",
	       &nth_card);

  if(ret != 1){
    g_warning("invalid JACK device specifier");

    return;
  }

  if(jack_devout->card_uri != NULL){
    g_free(jack_devout->card_uri);
  }
  
  jack_devout->card_uri = g_strdup(device);

  /* apply name to port */
  jack_port_start = 
    jack_port = g_list_copy(jack_devout->jack_port);
  
  for(i = 0; i < jack_devout->pcm_channels; i++){
    str = g_strdup_printf("ags-soundcard%d-%04d",
			  nth_card,
			  i);
    
    g_object_set(jack_port->data,
		 "port-name", str,
		 NULL);
    g_free(str);

    jack_port = jack_port->next;
  }

  g_list_free(jack_port_start);
}

gchar*
ags_jack_devout_get_device(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  return(jack_devout->card_uri);
}

void
ags_jack_devout_set_presets(AgsSoundcard *soundcard,
			    guint channels,
			    guint rate,
			    guint buffer_size,
			    guint format)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  g_object_set(jack_devout,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_jack_devout_get_presets(AgsSoundcard *soundcard,
			    guint *channels,
			    guint *rate,
			    guint *buffer_size,
			    guint *format)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  if(channels != NULL){
    *channels = jack_devout->pcm_channels;
  }

  if(rate != NULL){
    *rate = jack_devout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = jack_devout->buffer_size;
  }

  if(format != NULL){
    *format = jack_devout->format;
  }
}

/**
 * ags_jack_devout_list_cards:
 * @soundcard: the #AgsSoundcard
 * @card_id: JACK identifier
 * @card_name: card name
 *
 * List available soundcards.
 *
 * Since: 1.0.0
 */
void
ags_jack_devout_list_cards(AgsSoundcard *soundcard,
			   GList **card_id, GList **card_name)
{
  AgsJackDevout *jack_devout;

  AgsApplicationContext *application_context;
  
  GList *list, *list_start;

  pthread_mutex_t *application_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  application_context = (AgsApplicationContext *) jack_devout->application_context;

  if(application_context == NULL){
    return;
  }
  
  application_mutex = jack_devout->application_mutex;
  
  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  pthread_mutex_lock(application_mutex);

  list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  while(list != NULL){
    if(AGS_IS_JACK_DEVOUT(list->data)){
      if(card_id != NULL){
	if(AGS_JACK_DEVOUT(list->data)->card_uri != NULL){
	  *card_id = g_list_prepend(*card_id,
				    g_strdup(AGS_JACK_DEVOUT(list->data)->card_uri));
	}else{
	  *card_id = g_list_prepend(*card_id,
				    g_strdup("(null)"));

	  g_warning("ags_jack_devout_list_cards() - card id (null)");
	}
      }

      if(card_name != NULL){
	if(AGS_JACK_DEVOUT(list->data)->jack_client != NULL){
	  *card_name = g_list_prepend(*card_name,
				      g_strdup(AGS_JACK_CLIENT(AGS_JACK_DEVOUT(list->data)->jack_client)->name));
	}else{
	  *card_name = g_list_prepend(*card_name,
				      g_strdup("(null)"));

	  g_warning("ags_jack_devout_list_cards() - JACK client not connected (null)");
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
ags_jack_devout_pcm_info(AgsSoundcard *soundcard,
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
ags_jack_devout_is_starting(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  return((((AGS_JACK_DEVOUT_START_PLAY & (jack_devout->flags)) != 0) ? TRUE: FALSE));
}

gboolean
ags_jack_devout_is_playing(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  return((((AGS_JACK_DEVOUT_PLAY & (jack_devout->flags)) != 0) ? TRUE: FALSE));
}

gchar*
ags_jack_devout_get_uptime(AgsSoundcard *soundcard)
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
ags_jack_devout_port_init(AgsSoundcard *soundcard,
			  GError **error)
{
  AgsJackDevout *jack_devout;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  guint format, word_size;
  
  pthread_mutex_t *mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  application_context = ags_soundcard_get_application_context(soundcard);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) jack_devout);
  
  pthread_mutex_unlock(application_context->mutex);

  /* retrieve word size */
  pthread_mutex_lock(mutex);

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
    
    g_warning("ags_jack_devout_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for playback */
  jack_devout->flags |= (AGS_JACK_DEVOUT_BUFFER3 |
			 AGS_JACK_DEVOUT_START_PLAY |
			 AGS_JACK_DEVOUT_PLAY |
			 AGS_JACK_DEVOUT_NONBLOCKING);

  memset(jack_devout->buffer[0], 0, jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  memset(jack_devout->buffer[1], 0, jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  memset(jack_devout->buffer[2], 0, jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  memset(jack_devout->buffer[3], 0, jack_devout->pcm_channels * jack_devout->buffer_size * word_size);

  /*  */
  jack_devout->tact_counter = 0.0;
  jack_devout->delay_counter = 0.0;
  jack_devout->tic_counter = 0;

  jack_devout->flags |= (AGS_JACK_DEVOUT_INITIALIZED |
			 AGS_JACK_DEVOUT_START_PLAY |
			 AGS_JACK_DEVOUT_PLAY);
  
  g_atomic_int_and(&(jack_devout->sync_flags),
		   (~(AGS_JACK_DEVOUT_PASS_THROUGH)));
  g_atomic_int_or(&(jack_devout->sync_flags),
		  AGS_JACK_DEVOUT_INITIAL_CALLBACK);

  pthread_mutex_unlock(mutex);
}

void
ags_jack_devout_port_play(AgsSoundcard *soundcard,
			  GError **error)
{
  AgsJackClient *jack_client;
  AgsJackDevout *jack_devout;

  AgsNotifySoundcard *notify_soundcard;
  
  AgsMutexManager *mutex_manager;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  guint word_size;
  gboolean jack_client_activated;
  
  pthread_mutex_t *mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;
  pthread_mutex_t *client_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);
  application_context = ags_soundcard_get_application_context(soundcard);
  
  /* mutices */
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();
  task_thread = (AgsTaskThread *) application_context->task_thread;

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) jack_devout);
  
  pthread_mutex_unlock(application_context->mutex);

  /* client */
  pthread_mutex_lock(mutex);

  jack_client = (AgsJackClient *) jack_devout->jack_client;
  
  callback_mutex = jack_devout->callback_mutex;
  callback_finish_mutex = jack_devout->callback_finish_mutex;

  pthread_mutex_unlock(mutex);

  /* do playback */
  pthread_mutex_lock(mutex);
  
  jack_devout->flags &= (~AGS_JACK_DEVOUT_START_PLAY);
  notify_soundcard = AGS_NOTIFY_SOUNDCARD(jack_devout->notify_soundcard);
  
  if((AGS_JACK_DEVOUT_INITIALIZED & (jack_devout->flags)) == 0){
    pthread_mutex_unlock(mutex);
    
    return;
  }

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
    
    g_warning("ags_jack_devout_port_play(): unsupported word size");
    
    return;
  }

  pthread_mutex_unlock(mutex);

  /*  */
  pthread_mutex_lock(application_context->mutex);
  
  client_mutex = ags_mutex_manager_lookup(mutex_manager,
					  (GObject *) jack_client);
  
  pthread_mutex_unlock(application_context->mutex);

  pthread_mutex_lock(client_mutex);

  jack_client_activated = ((AGS_JACK_CLIENT_ACTIVATED & (jack_client->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(client_mutex);

  if(jack_client_activated){
    /* signal */
    if((AGS_JACK_DEVOUT_INITIAL_CALLBACK & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0){
      pthread_mutex_lock(callback_mutex);

      g_atomic_int_or(&(jack_devout->sync_flags),
		      AGS_JACK_DEVOUT_CALLBACK_DONE);
    
      if((AGS_JACK_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(jack_devout->sync_flags)))) != 0){
	pthread_cond_signal(jack_devout->callback_cond);
      }

      pthread_mutex_unlock(callback_mutex);
    }
    
    /* wait callback */	
    if((AGS_JACK_DEVOUT_INITIAL_CALLBACK & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0){
      pthread_mutex_lock(callback_finish_mutex);
    
      if((AGS_JACK_DEVOUT_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0){
	g_atomic_int_or(&(jack_devout->sync_flags),
			AGS_JACK_DEVOUT_CALLBACK_FINISH_WAIT);
    
	while((AGS_JACK_DEVOUT_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0 &&
	      (AGS_JACK_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_devout->sync_flags)))) != 0){
	  pthread_cond_wait(jack_devout->callback_finish_cond,
			    callback_finish_mutex);
	}
      }
    
      g_atomic_int_and(&(jack_devout->sync_flags),
		       (~(AGS_JACK_DEVOUT_CALLBACK_FINISH_WAIT |
			  AGS_JACK_DEVOUT_CALLBACK_FINISH_DONE)));
    
      pthread_mutex_unlock(callback_finish_mutex);
    }else{
      g_atomic_int_and(&(jack_devout->sync_flags),
		       (~AGS_JACK_DEVOUT_INITIAL_CALLBACK));
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
    tic_device = ags_tic_device_new((GObject *) jack_devout);
    task = g_list_append(task,
    			 tic_device);

    /* reset - clear buffer */
    clear_buffer = ags_clear_buffer_new((GObject *) jack_devout);
    task = g_list_append(task,
			 clear_buffer);
    
    /* reset - switch buffer flags */
    switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) jack_devout);
    task = g_list_append(task,
			 switch_buffer_flag);

    /* append tasks */
    ags_task_thread_append_tasks((AgsTaskThread *) task_thread,
				 task);
  }else{
    guint nth_buffer;
    guint word_size;
    
    /* tic */
    ags_soundcard_tic(AGS_SOUNDCARD(jack_devout));

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
      g_warning("ags_jack_devout_port_play(): unsupported word size");
      return;
    }
        
    /* reset - clear buffer */
    if((AGS_JACK_DEVOUT_BUFFER0 & (jack_devout->flags)) != 0){
      nth_buffer = 3;
    }else if((AGS_JACK_DEVOUT_BUFFER1 & (jack_devout->flags)) != 0){
      nth_buffer = 0;
    }else if((AGS_JACK_DEVOUT_BUFFER2 & (jack_devout->flags)) != 0){
      nth_buffer = 1;
    }else if((AGS_JACK_DEVOUT_BUFFER3 & (jack_devout->flags)) != 0){
      nth_buffer = 2;
    }

    memset(jack_devout->buffer[nth_buffer], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);

    /* reset - switch buffer flags */
    ags_jack_devout_switch_buffer_flag(jack_devout);
  }
}

void
ags_jack_devout_port_free(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  AgsNotifySoundcard *notify_soundcard;
  
  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  guint word_size;

  pthread_mutex_t *mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  application_context = ags_soundcard_get_application_context(soundcard);

  /*  */
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) jack_devout);
  
  pthread_mutex_unlock(application_context->mutex);

  /*  */
  pthread_mutex_lock(mutex);

  notify_soundcard = AGS_NOTIFY_SOUNDCARD(jack_devout->notify_soundcard);

  if((AGS_JACK_DEVOUT_INITIALIZED & (jack_devout->flags)) == 0){
    pthread_mutex_unlock(mutex);

    return;
  }

  g_object_ref(notify_soundcard);
  
  //  g_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);

  callback_mutex = jack_devout->callback_mutex;
  callback_finish_mutex = jack_devout->callback_finish_mutex;
  
  jack_devout->flags &= (~(AGS_JACK_DEVOUT_BUFFER0 |
			   AGS_JACK_DEVOUT_BUFFER1 |
			   AGS_JACK_DEVOUT_BUFFER2 |
			   AGS_JACK_DEVOUT_BUFFER3 |
			   AGS_JACK_DEVOUT_PLAY));

  g_atomic_int_or(&(jack_devout->sync_flags),
		  AGS_JACK_DEVOUT_PASS_THROUGH);
  g_atomic_int_and(&(jack_devout->sync_flags),
		   (~AGS_JACK_DEVOUT_INITIAL_CALLBACK));

  /* signal callback */
  pthread_mutex_lock(callback_mutex);

  g_atomic_int_or(&(jack_devout->sync_flags),
		  AGS_JACK_DEVOUT_CALLBACK_DONE);
    
  if((AGS_JACK_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(jack_devout->sync_flags)))) != 0){
    pthread_cond_signal(jack_devout->callback_cond);
  }

  pthread_mutex_unlock(callback_mutex);

  /* signal thread */
  pthread_mutex_lock(callback_finish_mutex);

  g_atomic_int_or(&(jack_devout->sync_flags),
		  AGS_JACK_DEVOUT_CALLBACK_FINISH_DONE);
    
  if((AGS_JACK_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_devout->sync_flags)))) != 0){
    pthread_cond_signal(jack_devout->callback_finish_cond);
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
  jack_devout->note_offset = 0;
  jack_devout->note_offset_absolute = 0;

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
    word_size = 0;
    
    g_critical("ags_jack_devout_free(): unsupported word size");
  }

  memset(jack_devout->buffer[1], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  memset(jack_devout->buffer[2], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  memset(jack_devout->buffer[3], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  memset(jack_devout->buffer[0], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);

  pthread_mutex_unlock(mutex);
}

void
ags_jack_devout_tic(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;
  gdouble delay;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* determine if attack should be switched */
  delay = jack_devout->delay[jack_devout->tic_counter];

  if((guint) jack_devout->delay_counter + 1 >= (guint) delay){
    if(jack_devout->do_loop &&
       jack_devout->note_offset + 1 == jack_devout->loop_right){
      ags_soundcard_set_note_offset(soundcard,
				    jack_devout->loop_left);
    }else{
      ags_soundcard_set_note_offset(soundcard,
				    jack_devout->note_offset + 1);
    }
    
    ags_soundcard_set_note_offset_absolute(soundcard,
					   jack_devout->note_offset_absolute + 1);
    
    /* delay */
    ags_soundcard_offset_changed(soundcard,
				 jack_devout->note_offset);
    
    /* reset - delay counter */
    jack_devout->delay_counter = 0.0;
    jack_devout->tact_counter += 1.0;
  }else{
    jack_devout->delay_counter += 1.0;
  }
}

void
ags_jack_devout_offset_changed(AgsSoundcard *soundcard,
			       guint note_offset)
{
  AgsJackDevout *jack_devout;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  jack_devout->tic_counter += 1;

  if(jack_devout->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    jack_devout->tic_counter = 0;
  }
}

void
ags_jack_devout_set_bpm(AgsSoundcard *soundcard,
			gdouble bpm)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  jack_devout->bpm = bpm;

  ags_jack_devout_adjust_delay_and_attack(jack_devout);
}

gdouble
ags_jack_devout_get_bpm(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  return(jack_devout->bpm);
}

void
ags_jack_devout_set_delay_factor(AgsSoundcard *soundcard,
				 gdouble delay_factor)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  jack_devout->delay_factor = delay_factor;

  ags_jack_devout_adjust_delay_and_attack(jack_devout);
}

gdouble
ags_jack_devout_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  return(jack_devout->delay_factor);
}

gdouble
ags_jack_devout_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  return((60.0 * (((gdouble) jack_devout->samplerate / (gdouble) jack_devout->buffer_size) / (gdouble) jack_devout->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) jack_devout->delay_factor))));
}

gdouble
ags_jack_devout_get_delay(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;
  guint index;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);
  index = jack_devout->tic_counter;
  
  return(jack_devout->delay[index]);
}

guint
ags_jack_devout_get_attack(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;
  guint index;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);
  index = jack_devout->tic_counter;
  
  return(jack_devout->attack[index]);
}

void*
ags_jack_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;
  void *buffer;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  if((AGS_JACK_DEVOUT_BUFFER0 & (jack_devout->flags)) != 0){
    buffer = jack_devout->buffer[0];
  }else if((AGS_JACK_DEVOUT_BUFFER1 & (jack_devout->flags)) != 0){
    buffer = jack_devout->buffer[1];
  }else if((AGS_JACK_DEVOUT_BUFFER2 & (jack_devout->flags)) != 0){
    buffer = jack_devout->buffer[2];
  }else if((AGS_JACK_DEVOUT_BUFFER3 & (jack_devout->flags)) != 0){
    buffer = jack_devout->buffer[3];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_jack_devout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;
  void *buffer;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  if((AGS_JACK_DEVOUT_BUFFER0 & (jack_devout->flags)) != 0){
    buffer = jack_devout->buffer[1];
  }else if((AGS_JACK_DEVOUT_BUFFER1 & (jack_devout->flags)) != 0){
    buffer = jack_devout->buffer[2];
  }else if((AGS_JACK_DEVOUT_BUFFER2 & (jack_devout->flags)) != 0){
    buffer = jack_devout->buffer[3];
  }else if((AGS_JACK_DEVOUT_BUFFER3 & (jack_devout->flags)) != 0){
    buffer = jack_devout->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_jack_devout_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;
  void *buffer;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  if((AGS_JACK_DEVOUT_BUFFER0 & (jack_devout->flags)) != 0){
    buffer = jack_devout->buffer[3];
  }else if((AGS_JACK_DEVOUT_BUFFER1 & (jack_devout->flags)) != 0){
    buffer = jack_devout->buffer[0];
  }else if((AGS_JACK_DEVOUT_BUFFER2 & (jack_devout->flags)) != 0){
    buffer = jack_devout->buffer[1];
  }else if((AGS_JACK_DEVOUT_BUFFER3 & (jack_devout->flags)) != 0){
    buffer = jack_devout->buffer[2];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

guint
ags_jack_devout_get_delay_counter(AgsSoundcard *soundcard)
{
  return(AGS_JACK_DEVOUT(soundcard)->delay_counter);
}

void
ags_jack_devout_set_note_offset(AgsSoundcard *soundcard,
				guint note_offset)
{
  AGS_JACK_DEVOUT(soundcard)->note_offset = note_offset;
}

guint
ags_jack_devout_get_note_offset(AgsSoundcard *soundcard)
{
  return(AGS_JACK_DEVOUT(soundcard)->note_offset);
}

void
ags_jack_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
					 guint note_offset)
{
  AGS_JACK_DEVOUT(soundcard)->note_offset_absolute = note_offset;
}

guint
ags_jack_devout_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  return(AGS_JACK_DEVOUT(soundcard)->note_offset_absolute);
}

void
ags_jack_devout_set_loop(AgsSoundcard *soundcard,
			 guint loop_left, guint loop_right,
			 gboolean do_loop)
{
  AGS_JACK_DEVOUT(soundcard)->loop_left = loop_left;
  AGS_JACK_DEVOUT(soundcard)->loop_right = loop_right;
  AGS_JACK_DEVOUT(soundcard)->do_loop = do_loop;

  if(do_loop){
    AGS_JACK_DEVOUT(soundcard)->loop_offset = AGS_JACK_DEVOUT(soundcard)->note_offset;
  }
}

void
ags_jack_devout_get_loop(AgsSoundcard *soundcard,
			 guint *loop_left, guint *loop_right,
			 gboolean *do_loop)  
{
  if(loop_left != NULL){
    *loop_left = AGS_JACK_DEVOUT(soundcard)->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = AGS_JACK_DEVOUT(soundcard)->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = AGS_JACK_DEVOUT(soundcard)->do_loop;
  }
}

guint
ags_jack_devout_get_loop_offset(AgsSoundcard *soundcard)  
{
  return(AGS_JACK_DEVOUT(soundcard)->loop_offset);
}

void
ags_jack_devout_set_audio(AgsSoundcard *soundcard,
			  GList *audio)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  jack_devout->audio = audio;
}

GList*
ags_jack_devout_get_audio(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  return(jack_devout->audio);
}

/**
 * ags_jack_devout_adjust_delay_and_attack:
 * @jack_devout: the #AgsJackDevout
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 1.0.0
 */
void
ags_jack_devout_adjust_delay_and_attack(AgsJackDevout *jack_devout)
{
  gdouble delay;
  guint default_tact_frames;
  guint delay_tact_frames;
  guint default_period;
  gint next_attack;
  guint i;

  if(jack_devout == NULL){
    return;
  }
  
  delay = ags_jack_devout_get_absolute_delay(AGS_SOUNDCARD(jack_devout));

#ifdef AGS_DEBUG
  g_message("delay : %f", delay);
#endif
  
  default_tact_frames = (guint) (delay * jack_devout->buffer_size);
  delay_tact_frames = (guint) (floor(delay) * jack_devout->buffer_size);
  default_period = (1.0 / AGS_SOUNDCARD_DEFAULT_PERIOD) * (default_tact_frames);

  i = 0;
  
  jack_devout->attack[0] = (guint) floor(0.25 * jack_devout->buffer_size);
  next_attack = (((jack_devout->attack[i] + default_tact_frames) / jack_devout->buffer_size) - delay) * jack_devout->buffer_size;

  if(next_attack >= jack_devout->buffer_size){
    next_attack = jack_devout->buffer_size - 1;
  }
  
  /* check if delay drops for next attack */
  if(next_attack < 0){
    jack_devout->attack[i] = jack_devout->attack[i] - ((gdouble) next_attack / 2.0);

    if(jack_devout->attack[i] < 0){
      jack_devout->attack[i] = 0;
    }
    
    if(jack_devout->attack[i] >= jack_devout->buffer_size){
      jack_devout->attack[i] = jack_devout->buffer_size - 1;
    }
    
    next_attack = next_attack + (next_attack / 2.0);
    
    if(next_attack < 0){
      next_attack = 0;
    }

    if(next_attack >= jack_devout->buffer_size){
      next_attack = jack_devout->buffer_size - 1;
    }
  }
  
  for(i = 1; i < (int)  2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    jack_devout->attack[i] = next_attack;
    next_attack = (((jack_devout->attack[i] + default_tact_frames) / jack_devout->buffer_size) - delay) * jack_devout->buffer_size;

    if(next_attack >= jack_devout->buffer_size){
      next_attack = jack_devout->buffer_size - 1;
    }
    
    /* check if delay drops for next attack */
    if(next_attack < 0){
      jack_devout->attack[i] = jack_devout->attack[i] - ((gdouble) next_attack / 2.0);

      if(jack_devout->attack[i] < 0){
	jack_devout->attack[i] = 0;
      }

      if(jack_devout->attack[i] >= jack_devout->buffer_size){
	jack_devout->attack[i] = jack_devout->buffer_size - 1;
      }
    
      next_attack = next_attack + (next_attack / 2.0);
    
      if(next_attack < 0){
	next_attack = 0;
      }

      if(next_attack >= jack_devout->buffer_size){
	next_attack = jack_devout->buffer_size - 1;
      }
    }
    
#ifdef AGS_DEBUG
    g_message("%d", jack_devout->attack[i]);
#endif
  }

  jack_devout->attack[0] = jack_devout->attack[i - 2];
  
  for(i = 0; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD - 1; i++){
    jack_devout->delay[i] = ((gdouble) (default_tact_frames + jack_devout->attack[i] - jack_devout->attack[i + 1])) / (gdouble) jack_devout->buffer_size;
    
#ifdef AGS_DEBUG
    g_message("%f", jack_devout->delay[i]);
#endif
  }

  jack_devout->delay[i] = ((gdouble) (default_tact_frames + jack_devout->attack[i] - jack_devout->attack[0])) / (gdouble) jack_devout->buffer_size;
}

/**
 * ags_jack_devout_realloc_buffer:
 * @jack_devout: the #AgsJackDevout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 1.0.0
 */
void
ags_jack_devout_realloc_buffer(AgsJackDevout *jack_devout)
{
  guint word_size;

  if(jack_devout == NULL){
    return;
  }

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
    g_warning("ags_jack_devout_realloc_buffer(): unsupported word size");
    return;
  }

  if(g_list_length(jack_devout->jack_port) < jack_devout->pcm_channels){
    AgsJackPort *jack_port;

    gchar *str;
    
    guint nth_soundcard;
    guint i;

    if(jack_devout->card_uri != NULL){
      sscanf(jack_devout->card_uri,
	     "ags-jack-devout-%u",
	     &nth_soundcard);
    }else{
      g_warning("ags_jack_devout_realloc_buffer() - card uri not set");
      
      return;
    }
    
    for(i = g_list_length(jack_devout->jack_port); i < jack_devout->pcm_channels; i++){
      str = g_strdup_printf("ags-soundcard%d-%04d",
			    nth_soundcard,
			    i);
      
      jack_port = ags_jack_port_new(jack_devout->jack_client);
      ags_jack_client_add_port((AgsJackClient *) jack_devout->jack_client,
			       (GObject *) jack_port);

      jack_devout->jack_port = g_list_prepend(jack_devout->jack_port,
					      jack_port);

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

    jack_devout->port_name[jack_devout->pcm_channels] = NULL;    
  }else if(g_list_length(jack_devout->jack_port) > jack_devout->pcm_channels){
    GList *jack_port, *jack_port_next;

    guint i;
    
    jack_port = jack_devout->jack_port;

    for(i = 0; i < g_list_length(jack_devout->jack_port) - jack_devout->pcm_channels; i++){
      jack_port_next = jack_port->next;

      jack_devout->jack_port = g_list_remove(jack_devout->jack_port,
					     jack_port->data);
      ags_jack_port_unregister(jack_port->data);
      
      g_object_unref(jack_port->data);
      
      jack_port = jack_port_next;
    }

    jack_devout->port_name = (gchar **) realloc(jack_devout->port_name,
					        (jack_devout->pcm_channels + 1) * sizeof(gchar *));
    jack_devout->port_name[jack_devout->pcm_channels] = NULL;
  }
  
  /* AGS_JACK_DEVOUT_BUFFER_0 */
  if(jack_devout->buffer[0] != NULL){
    free(jack_devout->buffer[0]);
  }
  
  jack_devout->buffer[0] = (void *) malloc(jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  
  /* AGS_JACK_DEVOUT_BUFFER_1 */
  if(jack_devout->buffer[1] != NULL){
    free(jack_devout->buffer[1]);
  }

  jack_devout->buffer[1] = (void *) malloc(jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  
  /* AGS_JACK_DEVOUT_BUFFER_2 */
  if(jack_devout->buffer[2] != NULL){
    free(jack_devout->buffer[2]);
  }

  jack_devout->buffer[2] = (void *) malloc(jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  
  /* AGS_JACK_DEVOUT_BUFFER_3 */
  if(jack_devout->buffer[3] != NULL){
    free(jack_devout->buffer[3]);
  }
  
  jack_devout->buffer[3] = (void *) malloc(jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
}

/**
 * ags_jack_devout_new:
 * @application_context: the #AgsApplicationContext
 *
 * Creates an #AgsJackDevout, refering to @application_context.
 *
 * Returns: a new #AgsJackDevout
 *
 * Since: 1.0.0
 */
AgsJackDevout*
ags_jack_devout_new(GObject *application_context)
{
  AgsJackDevout *jack_devout;

  jack_devout = (AgsJackDevout *) g_object_new(AGS_TYPE_JACK_DEVOUT,
					       "application-context", application_context,
					       NULL);
  
  return(jack_devout);
}
