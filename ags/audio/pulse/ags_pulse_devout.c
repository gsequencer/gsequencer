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

#include <ags/audio/pulse/ags_pulse_devout.h>

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

#include <ags/audio/pulse/ags_pulse_server.h>
#include <ags/audio/pulse/ags_pulse_client.h>
#include <ags/audio/pulse/ags_pulse_port.h>

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

void ags_pulse_devout_class_init(AgsPulseDevoutClass *pulse_devout);
void ags_pulse_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pulse_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_pulse_devout_init(AgsPulseDevout *pulse_devout);
void ags_pulse_devout_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_pulse_devout_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_pulse_devout_disconnect(AgsConnectable *connectable);
void ags_pulse_devout_connect(AgsConnectable *connectable);
void ags_pulse_devout_dispose(GObject *gobject);
void ags_pulse_devout_finalize(GObject *gobject);

void ags_pulse_devout_set_application_context(AgsSoundcard *soundcard,
					      AgsApplicationContext *application_context);
AgsApplicationContext* ags_pulse_devout_get_application_context(AgsSoundcard *soundcard);

void ags_pulse_devout_set_application_mutex(AgsSoundcard *soundcard,
					    pthread_mutex_t *application_mutex);
pthread_mutex_t* ags_pulse_devout_get_application_mutex(AgsSoundcard *soundcard);

void ags_pulse_devout_set_device(AgsSoundcard *soundcard,
				 gchar *device);
gchar* ags_pulse_devout_get_device(AgsSoundcard *soundcard);

void ags_pulse_devout_set_presets(AgsSoundcard *soundcard,
				  guint channels,
				  guint rate,
				  guint buffer_size,
				  guint format);
void ags_pulse_devout_get_presets(AgsSoundcard *soundcard,
				  guint *channels,
				  guint *rate,
				  guint *buffer_size,
				  guint *format);

void ags_pulse_devout_list_cards(AgsSoundcard *soundcard,
				 GList **card_id, GList **card_name);
void ags_pulse_devout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			       guint *channels_min, guint *channels_max,
			       guint *rate_min, guint *rate_max,
			       guint *buffer_size_min, guint *buffer_size_max,
			       GError **error);

gboolean ags_pulse_devout_is_starting(AgsSoundcard *soundcard);
gboolean ags_pulse_devout_is_playing(AgsSoundcard *soundcard);

gchar* ags_pulse_devout_get_uptime(AgsSoundcard *soundcard);

void ags_pulse_devout_port_init(AgsSoundcard *soundcard,
				GError **error);
void ags_pulse_devout_port_play(AgsSoundcard *soundcard,
				GError **error);
void ags_pulse_devout_port_free(AgsSoundcard *soundcard);

void ags_pulse_devout_tic(AgsSoundcard *soundcard);
void ags_pulse_devout_offset_changed(AgsSoundcard *soundcard,
				     guint note_offset);

void ags_pulse_devout_set_bpm(AgsSoundcard *soundcard,
			      gdouble bpm);
gdouble ags_pulse_devout_get_bpm(AgsSoundcard *soundcard);

void ags_pulse_devout_set_delay_factor(AgsSoundcard *soundcard,
				       gdouble delay_factor);
gdouble ags_pulse_devout_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_pulse_devout_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_pulse_devout_get_delay(AgsSoundcard *soundcard);
guint ags_pulse_devout_get_attack(AgsSoundcard *soundcard);

void* ags_pulse_devout_get_buffer(AgsSoundcard *soundcard);
void* ags_pulse_devout_get_next_buffer(AgsSoundcard *soundcard);

guint ags_pulse_devout_get_delay_counter(AgsSoundcard *soundcard);

void ags_pulse_devout_set_note_offset(AgsSoundcard *soundcard,
				      guint note_offset);
guint ags_pulse_devout_get_note_offset(AgsSoundcard *soundcard);

void ags_pulse_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
					       guint note_offset);
guint ags_pulse_devout_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_pulse_devout_set_loop(AgsSoundcard *soundcard,
			       guint loop_left, guint loop_right,
			       gboolean do_loop);
void ags_pulse_devout_get_loop(AgsSoundcard *soundcard,
			       guint *loop_left, guint *loop_right,
			       gboolean *do_loop);

guint ags_pulse_devout_get_loop_offset(AgsSoundcard *soundcard);

void ags_pulse_devout_set_audio(AgsSoundcard *soundcard,
				GList *audio);
GList* ags_pulse_devout_get_audio(AgsSoundcard *soundcard);

void ags_pulse_devout_adjust_delay_and_attack(AgsPulseDevout *pulse_devout);
void ags_pulse_devout_realloc_buffer(AgsPulseDevout *pulse_devout);

/**
 * SECTION:ags_pulse_devout
 * @short_description: Output to soundcard
 * @title: AgsPulseDevout
 * @section_id:
 * @include: ags/audio/pulse/ags_pulse_devout.h
 *
 * #AgsPulseDevout represents a soundcard and supports output.
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
  PROP_PULSE_CLIENT,
  PROP_PULSE_PORT,
  PROP_CHANNEL,
};

enum{
  LAST_SIGNAL,
};

static gpointer ags_pulse_devout_parent_class = NULL;
static guint pulse_devout_signals[LAST_SIGNAL];

GType
ags_pulse_devout_get_type (void)
{
  static GType ags_type_pulse_devout = 0;

  if(!ags_type_pulse_devout){
    static const GTypeInfo ags_pulse_devout_info = {
      sizeof (AgsPulseDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pulse_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPulseDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pulse_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pulse_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_pulse_devout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_pulse_devout = g_type_register_static(G_TYPE_OBJECT,
						   "AgsPulseDevout",
						   &ags_pulse_devout_info,
						   0);

    g_type_add_interface_static(ags_type_pulse_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_pulse_devout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);
  }

  return (ags_type_pulse_devout);
}

void
ags_pulse_devout_class_init(AgsPulseDevoutClass *pulse_devout)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_pulse_devout_parent_class = g_type_class_peek_parent(pulse_devout);

  /* GObjectClass */
  gobject = (GObjectClass *) pulse_devout;

  gobject->set_property = ags_pulse_devout_set_property;
  gobject->get_property = ags_pulse_devout_get_property;

  gobject->dispose = ags_pulse_devout_dispose;
  gobject->finalize = ags_pulse_devout_finalize;

  /* properties */
  /**
   * AgsPulseDevout:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 0.9.10
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
   * AgsPulseDevout:application-mutex:
   *
   * The assigned application mutex
   * 
   * Since: 0.9.10
   */
  param_spec = g_param_spec_pointer("application-mutex",
				    i18n_pspec("the application mutex object"),
				    i18n_pspec("The application mutex object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_MUTEX,
				  param_spec);

  /**
   * AgsPulseDevout:device:
   *
   * The pulse soundcard indentifier
   * 
   * Since: 0.9.10
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-pulse-devout-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsPulseDevout:dsp-channels:
   *
   * The dsp channel count
   * 
   * Since: 0.9.10
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
   * AgsPulseDevout:pcm-channels:
   *
   * The pcm channel count
   * 
   * Since: 0.9.10
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
   * AgsPulseDevout:format:
   *
   * The precision of the buffer
   * 
   * Since: 0.9.10
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
   * AgsPulseDevout:buffer-size:
   *
   * The buffer size
   * 
   * Since: 0.9.10
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
   * AgsPulseDevout:samplerate:
   *
   * The samplerate
   * 
   * Since: 0.9.10
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
   * AgsPulseDevout:buffer:
   *
   * The buffer
   * 
   * Since: 0.9.10
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to play"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsPulseDevout:bpm:
   *
   * Beats per minute
   * 
   * Since: 0.9.10
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
   * AgsPulseDevout:delay-factor:
   *
   * tact
   * 
   * Since: 0.9.10
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
   * AgsPulseDevout:attack:
   *
   * Attack of the buffer
   * 
   * Since: 0.9.10
   */
  param_spec = g_param_spec_pointer("attack",
				    i18n_pspec("attack of buffer"),
				    i18n_pspec("The attack to use for the buffer"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  
  /**
   * AgsPulseDevout:pulse-client:
   *
   * The assigned #AgsPulseClient
   * 
   * Since: 0.9.10
   */
  param_spec = g_param_spec_object("pulse-client",
				   i18n_pspec("pulse client object"),
				   i18n_pspec("The pulse client object"),
				   AGS_TYPE_PULSE_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PULSE_CLIENT,
				  param_spec);

  /**
   * AgsPulseDevout:pulse-port:
   *
   * The assigned #AgsPulsePort
   * 
   * Since: 0.9.10
   */
  param_spec = g_param_spec_pointer("pulse-port",
				    i18n_pspec("pulse port object"),
				    i18n_pspec("The pulse port object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PULSE_PORT,
				  param_spec);

  /**
   * AgsPulseDevout:channel:
   *
   * The assigned #AgsChannel
   * 
   * Since: 0.9.10
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
ags_pulse_devout_error_quark()
{
  return(g_quark_from_static_string("ags-pulse_devout-error-quark"));
}

void
ags_pulse_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_pulse_devout_connect;
  connectable->disconnect = ags_pulse_devout_disconnect;
}

void
ags_pulse_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_application_context = ags_pulse_devout_set_application_context;
  soundcard->get_application_context = ags_pulse_devout_get_application_context;

  soundcard->set_application_mutex = ags_pulse_devout_set_application_mutex;
  soundcard->get_application_mutex = ags_pulse_devout_get_application_mutex;

  soundcard->set_device = ags_pulse_devout_set_device;
  soundcard->get_device = ags_pulse_devout_get_device;
  
  soundcard->set_presets = ags_pulse_devout_set_presets;
  soundcard->get_presets = ags_pulse_devout_get_presets;

  soundcard->list_cards = ags_pulse_devout_list_cards;
  soundcard->pcm_info = ags_pulse_devout_pcm_info;

  soundcard->get_poll_fd = NULL;
  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_pulse_devout_is_starting;
  soundcard->is_playing = ags_pulse_devout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_pulse_devout_get_uptime;
  
  soundcard->play_init = ags_pulse_devout_port_init;
  soundcard->play = ags_pulse_devout_port_play;

  soundcard->record_init = NULL;
  soundcard->record = NULL;

  soundcard->stop = ags_pulse_devout_port_free;

  soundcard->tic = ags_pulse_devout_tic;
  soundcard->offset_changed = ags_pulse_devout_offset_changed;
    
  soundcard->set_bpm = ags_pulse_devout_set_bpm;
  soundcard->get_bpm = ags_pulse_devout_get_bpm;

  soundcard->set_delay_factor = ags_pulse_devout_set_delay_factor;
  soundcard->get_delay_factor = ags_pulse_devout_get_delay_factor;
  
  soundcard->get_absolute_delay = ags_pulse_devout_get_absolute_delay;

  soundcard->get_delay = ags_pulse_devout_get_delay;
  soundcard->get_attack = ags_pulse_devout_get_attack;

  soundcard->get_buffer = ags_pulse_devout_get_buffer;
  soundcard->get_next_buffer = ags_pulse_devout_get_next_buffer;

  soundcard->get_delay_counter = ags_pulse_devout_get_delay_counter;

  soundcard->set_note_offset = ags_pulse_devout_set_note_offset;
  soundcard->get_note_offset = ags_pulse_devout_get_note_offset;

  soundcard->set_note_offset_absolute = ags_pulse_devout_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_pulse_devout_get_note_offset_absolute;

  soundcard->set_loop = ags_pulse_devout_set_loop;
  soundcard->get_loop = ags_pulse_devout_get_loop;

  soundcard->get_loop_offset = ags_pulse_devout_get_loop_offset;

  soundcard->set_audio = ags_pulse_devout_set_audio;
  soundcard->get_audio = ags_pulse_devout_get_audio;
}

void
ags_pulse_devout_init(AgsPulseDevout *pulse_devout)
{
  AgsMutexManager *mutex_manager;

  AgsConfig *config;

  gchar *str;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert devout mutex */
  pulse_devout->mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  pulse_devout->mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) pulse_devout,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  pulse_devout->flags = 0;
  g_atomic_int_set(&(pulse_devout->sync_flags),
		   AGS_PULSE_DEVOUT_PASS_THROUGH);

  /* quality */
  config = ags_config_get_instance();

  pulse_devout->dsp_channels = AGS_SOUNDCARD_DEFAULT_DSP_CHANNELS;
  pulse_devout->pcm_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
  pulse_devout->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  pulse_devout->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  pulse_devout->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

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
    pulse_devout->dsp_channels = g_ascii_strtoull(str,
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
    pulse_devout->pcm_channels = g_ascii_strtoull(str,
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
    pulse_devout->samplerate = g_ascii_strtoull(str,
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
    pulse_devout->buffer_size = g_ascii_strtoull(str,
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
    pulse_devout->format = g_ascii_strtoull(str,
					    NULL,
					    10);
    free(str);
  }

  /*  */
  pulse_devout->card_uri = NULL;
  pulse_devout->pulse_client = NULL;

  pulse_devout->port_name = NULL;
  pulse_devout->pulse_port = NULL;

  /* buffer */
  pulse_devout->buffer = (void **) malloc(8 * sizeof(void*));

  pulse_devout->buffer[0] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * sizeof(signed short));
  pulse_devout->buffer[1] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * sizeof(signed short));
  pulse_devout->buffer[2] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * sizeof(signed short));
  pulse_devout->buffer[3] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * sizeof(signed short));
  pulse_devout->buffer[4] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * sizeof(signed short));
  pulse_devout->buffer[5] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * sizeof(signed short));
  pulse_devout->buffer[6] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * sizeof(signed short));
  pulse_devout->buffer[7] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * sizeof(signed short));
  
  ags_pulse_devout_realloc_buffer(pulse_devout);
  
  /* bpm */
  pulse_devout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  pulse_devout->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* delay and attack */
  pulse_devout->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					   sizeof(gdouble));
  
  pulse_devout->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					  sizeof(guint));

  ags_pulse_devout_adjust_delay_and_attack(pulse_devout);
  
  /* counters */
  pulse_devout->tact_counter = 0.0;
  pulse_devout->delay_counter = 0;
  pulse_devout->tic_counter = 0;

  pulse_devout->note_offset = 0;
  pulse_devout->note_offset_absolute = 0;

  pulse_devout->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  pulse_devout->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  pulse_devout->do_loop = FALSE;

  pulse_devout->loop_offset = 0;

  /* callback mutex */
  pulse_devout->callback_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(pulse_devout->callback_mutex,
		     NULL);

  pulse_devout->callback_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(pulse_devout->callback_cond, NULL);

  /* callback finish mutex */
  pulse_devout->callback_finish_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(pulse_devout->callback_finish_mutex,
		     NULL);

  pulse_devout->callback_finish_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(pulse_devout->callback_finish_cond, NULL);

  /* parent */
  pulse_devout->application_context = NULL;
  pulse_devout->application_mutex = NULL;

  /*  */
  pulse_devout->notify_soundcard = NULL;
  
  /* all AgsAudio */
  pulse_devout->audio = NULL;
}

void
ags_pulse_devout_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if(pulse_devout->application_context == (GObject *) application_context){
	return;
      }

      if(pulse_devout->application_context != NULL){
	g_object_unref(G_OBJECT(pulse_devout->application_context));
      }

      if(application_context != NULL){
	AgsConfig *config;

	gchar *segmentation;
	guint discriminante, nominante;
	
	g_object_ref(G_OBJECT(application_context));

	pulse_devout->application_mutex = application_context->mutex;

	config = ags_config_get_instance();

	/* segmentation */
	segmentation = ags_config_get_value(config,
					    AGS_CONFIG_GENERIC,
					    "segmentation");

	if(segmentation != NULL){
	  sscanf(segmentation, "%d/%d",
		 &discriminante,
		 &nominante);
    
	  pulse_devout->delay_factor = 1.0 / nominante * (nominante / discriminante);
	}
	
	ags_pulse_devout_adjust_delay_and_attack(pulse_devout);
	ags_pulse_devout_realloc_buffer(pulse_devout);
      }else{
	pulse_devout->application_mutex = NULL;
      }

      pulse_devout->application_context = (GObject *) application_context;
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      pthread_mutex_t *application_mutex;

      application_mutex = (pthread_mutex_t *) g_value_get_pointer(value);

      if(pulse_devout->application_mutex == application_mutex){
	return;
      }
      
      pulse_devout->application_mutex = application_mutex;
    }
    break;
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      pulse_devout->card_uri = g_strdup(device);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      if(dsp_channels == pulse_devout->dsp_channels){
	return;
      }

      pulse_devout->dsp_channels = dsp_channels;
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      if(pcm_channels == pulse_devout->pcm_channels){
	return;
      }

      pulse_devout->pcm_channels = pcm_channels;

      ags_pulse_devout_realloc_buffer(pulse_devout);

      if(pulse_devout->pulse_port != NULL){
	ags_pulse_port_set_pcm_channels(pulse_devout->pulse_port->data,
					pcm_channels);
      }
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      if(format == pulse_devout->format){
	return;
      }

      pulse_devout->format = format;

      ags_pulse_devout_realloc_buffer(pulse_devout);

      if(pulse_devout->pulse_port != NULL){
	ags_pulse_port_set_format(pulse_devout->pulse_port->data,
				  format);
      }
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      if(buffer_size == pulse_devout->buffer_size){
	return;
      }

      pulse_devout->buffer_size = buffer_size;

      ags_pulse_devout_realloc_buffer(pulse_devout);
      ags_pulse_devout_adjust_delay_and_attack(pulse_devout);

      if(pulse_devout->pulse_port != NULL){
	ags_pulse_port_set_buffer_size(pulse_devout->pulse_port->data,
				       buffer_size);
      }
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      if(samplerate == pulse_devout->samplerate){
	return;
      }

      pulse_devout->samplerate = samplerate;

      ags_pulse_devout_realloc_buffer(pulse_devout);
      ags_pulse_devout_adjust_delay_and_attack(pulse_devout);

      if(pulse_devout->pulse_port != NULL){
	ags_pulse_port_set_samplerate(pulse_devout->pulse_port->data,
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

      pulse_devout->bpm = bpm;

      ags_pulse_devout_adjust_delay_and_attack(pulse_devout);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      pulse_devout->delay_factor = delay_factor;

      ags_pulse_devout_adjust_delay_and_attack(pulse_devout);
    }
    break;
  case PROP_PULSE_CLIENT:
    {
      AgsPulseClient *pulse_client;

      pulse_client = (AgsPulseClient *) g_value_get_object(value);

      if(pulse_devout->pulse_client == (GObject *) pulse_client){
	return;
      }

      if(pulse_devout->pulse_client != NULL){
	g_object_unref(G_OBJECT(pulse_devout->pulse_client));
      }

      if(pulse_client != NULL){
	g_object_ref(pulse_client);
      }
      
      pulse_devout->pulse_client = (GObject *) pulse_client;
    }
    break;
  case PROP_PULSE_PORT:
    {
      AgsPulsePort *pulse_port;

      pulse_port = (AgsPulsePort *) g_value_get_pointer(value);

      if(!AGS_IS_PULSE_PORT(pulse_port) ||
	 g_list_find(pulse_devout->pulse_port, pulse_port) != NULL){
	return;
      }

      if(pulse_port != NULL){
	g_object_ref(pulse_port);
	pulse_devout->pulse_port = g_list_append(pulse_devout->pulse_port,
						 pulse_port);
      }
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_devout_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, pulse_devout->application_context);
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      g_value_set_pointer(value, pulse_devout->application_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      g_value_set_string(value, pulse_devout->card_uri);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_value_set_uint(value, pulse_devout->dsp_channels);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_value_set_uint(value, pulse_devout->pcm_channels);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, pulse_devout->format);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, pulse_devout->buffer_size);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, pulse_devout->samplerate);
    }
    break;
  case PROP_BUFFER:
    {
      g_value_set_pointer(value, pulse_devout->buffer);
    }
    break;
  case PROP_BPM:
    {
      g_value_set_double(value, pulse_devout->bpm);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_value_set_double(value, pulse_devout->delay_factor);
    }
    break;
  case PROP_ATTACK:
    {
      g_value_set_pointer(value, pulse_devout->attack);
    }
    break;
  case PROP_PULSE_CLIENT:
    {
      g_value_set_object(value, pulse_devout->pulse_client);
    }
    break;
  case PROP_PULSE_PORT:
    {
      g_value_set_pointer(value,
			  g_list_copy(pulse_devout->pulse_port));
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_devout_dispose(GObject *gobject)
{
  AgsPulseDevout *pulse_devout;

  GList *list;

  pulse_devout = AGS_PULSE_DEVOUT(gobject);

  /* application context */
  if(pulse_devout->application_context != NULL){
    g_object_unref(pulse_devout->application_context);

    pulse_devout->application_context = NULL;
  }

  /* unref audio */
  if(pulse_devout->audio != NULL){
    list = pulse_devout->audio;

    while(list != NULL){
      g_object_set(G_OBJECT(list->data),
		   "soundcard", NULL,
		   NULL);
      
      list = list->next;
    }

    g_list_free_full(pulse_devout->audio,
		     g_object_unref);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_pulse_devout_parent_class)->dispose(gobject);
}

void
ags_pulse_devout_finalize(GObject *gobject)
{
  AgsPulseDevout *pulse_devout;

  AgsMutexManager *mutex_manager;
  
  GList *list, *list_next;

  pulse_devout = AGS_PULSE_DEVOUT(gobject);

  /* remove pulse_devout mutex */
  pthread_mutex_lock(pulse_devout->application_mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(pulse_devout->application_mutex);

  /* free output buffer */
  free(pulse_devout->buffer[0]);
  free(pulse_devout->buffer[1]);
  free(pulse_devout->buffer[2]);
  free(pulse_devout->buffer[3]);
  free(pulse_devout->buffer[4]);
  free(pulse_devout->buffer[5]);
  free(pulse_devout->buffer[6]);
  free(pulse_devout->buffer[7]);

  /* free buffer array */
  free(pulse_devout->buffer);

  /* free AgsAttack */
  free(pulse_devout->attack);

  /* unref notify soundcard */
  if(pulse_devout->notify_soundcard != NULL){
    if(pulse_devout->application_context != NULL){
      ags_task_thread_remove_cyclic_task(AGS_APPLICATION_CONTEXT(pulse_devout->application_context)->task_thread,
					 pulse_devout->notify_soundcard);
    }
    
    g_object_unref(pulse_devout->notify_soundcard);
  }

  /* application context */
  if(pulse_devout->application_context != NULL){
    g_object_unref(pulse_devout->application_context);
  }
  
  /* unref audio */
  if(pulse_devout->audio != NULL){
    list = pulse_devout->audio;

    while(list != NULL){
      g_object_set(G_OBJECT(list->data),
		   "soundcard", NULL,
		   NULL);
      
      list = list->next;
    }

    g_list_free_full(pulse_devout->audio,
		     g_object_unref);
  }

  pthread_mutex_destroy(pulse_devout->mutex);
  free(pulse_devout->mutex);

  pthread_mutexattr_destroy(pulse_devout->mutexattr);
  free(pulse_devout->mutexattr);

  /* call parent */
  G_OBJECT_CLASS(ags_pulse_devout_parent_class)->finalize(gobject);
}

void
ags_pulse_devout_connect(AgsConnectable *connectable)
{
  AgsPulseDevout *pulse_devout;
  
  GList *list;

  pulse_devout = AGS_PULSE_DEVOUT(connectable);

  /*  */  
  list = pulse_devout->audio;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_pulse_devout_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_pulse_devout_switch_buffer_flag:
 * @pulse_devout: an #AgsPulseDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 0.9.10
 */
void
ags_pulse_devout_switch_buffer_flag(AgsPulseDevout *pulse_devout)
{
  if((AGS_PULSE_DEVOUT_BUFFER0 & (pulse_devout->flags)) != 0){
    pulse_devout->flags &= (~AGS_PULSE_DEVOUT_BUFFER0);
    pulse_devout->flags |= AGS_PULSE_DEVOUT_BUFFER1;
  }else if((AGS_PULSE_DEVOUT_BUFFER1 & (pulse_devout->flags)) != 0){
    pulse_devout->flags &= (~AGS_PULSE_DEVOUT_BUFFER1);
    pulse_devout->flags |= AGS_PULSE_DEVOUT_BUFFER2;
  }else if((AGS_PULSE_DEVOUT_BUFFER2 & (pulse_devout->flags)) != 0){
    pulse_devout->flags &= (~AGS_PULSE_DEVOUT_BUFFER2);
    pulse_devout->flags |= AGS_PULSE_DEVOUT_BUFFER3;
  }else if((AGS_PULSE_DEVOUT_BUFFER3 & (pulse_devout->flags)) != 0){
    pulse_devout->flags &= (~AGS_PULSE_DEVOUT_BUFFER3);
    pulse_devout->flags |= AGS_PULSE_DEVOUT_BUFFER4;
  }else if((AGS_PULSE_DEVOUT_BUFFER4 & (pulse_devout->flags)) != 0){
    pulse_devout->flags &= (~AGS_PULSE_DEVOUT_BUFFER4);
    pulse_devout->flags |= AGS_PULSE_DEVOUT_BUFFER5;
  }else if((AGS_PULSE_DEVOUT_BUFFER5 & (pulse_devout->flags)) != 0){
    pulse_devout->flags &= (~AGS_PULSE_DEVOUT_BUFFER5);
    pulse_devout->flags |= AGS_PULSE_DEVOUT_BUFFER6;
  }else if((AGS_PULSE_DEVOUT_BUFFER6 & (pulse_devout->flags)) != 0){
    pulse_devout->flags &= (~AGS_PULSE_DEVOUT_BUFFER6);
    pulse_devout->flags |= AGS_PULSE_DEVOUT_BUFFER7;
  }else if((AGS_PULSE_DEVOUT_BUFFER7 & (pulse_devout->flags)) != 0){
    pulse_devout->flags &= (~AGS_PULSE_DEVOUT_BUFFER7);
    pulse_devout->flags |= AGS_PULSE_DEVOUT_BUFFER0;
  }
}

void
ags_pulse_devout_set_application_context(AgsSoundcard *soundcard,
					 AgsApplicationContext *application_context)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  pulse_devout->application_context = (GObject *) application_context;
}

AgsApplicationContext*
ags_pulse_devout_get_application_context(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  return((AgsApplicationContext *) pulse_devout->application_context);
}

void
ags_pulse_devout_set_application_mutex(AgsSoundcard *soundcard,
				       pthread_mutex_t *application_mutex)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  pulse_devout->application_mutex = application_mutex;
}

pthread_mutex_t*
ags_pulse_devout_get_application_mutex(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  return(pulse_devout->application_mutex);
}

void
ags_pulse_devout_set_device(AgsSoundcard *soundcard,
			    gchar *device)
{
  AgsPulseDevout *pulse_devout;

  GList *pulse_port, *pulse_port_start;

  gchar *str;
  
  int ret;
  guint nth_card;
  guint i;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  if(pulse_devout->card_uri == device ||
     !g_ascii_strcasecmp(pulse_devout->card_uri,
			 device)){
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-pulse-devout-")){
    g_warning("invalid pulseaudio device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-pulse-devout-%u",
	       &nth_card);

  if(ret != 1){
    g_warning("invalid pulseaudio device specifier");

    return;
  }

  if(pulse_devout->card_uri != NULL){
    g_free(pulse_devout->card_uri);
  }
  
  pulse_devout->card_uri = g_strdup(device);

  /* apply name to port */
  pulse_port_start = 
    pulse_port = g_list_copy(pulse_devout->pulse_port);
  
  str = g_strdup_printf("ags-soundcard%d",
			nth_card);
    
  g_object_set(pulse_port->data,
	       "port-name", str,
	       NULL);
  g_free(str);

  g_list_free(pulse_port_start);
}

gchar*
ags_pulse_devout_get_device(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  
  return(pulse_devout->card_uri);
}

void
ags_pulse_devout_set_presets(AgsSoundcard *soundcard,
			     guint channels,
			     guint rate,
			     guint buffer_size,
			     guint format)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  g_object_set(pulse_devout,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_pulse_devout_get_presets(AgsSoundcard *soundcard,
			     guint *channels,
			     guint *rate,
			     guint *buffer_size,
			     guint *format)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  if(channels != NULL){
    *channels = pulse_devout->pcm_channels;
  }

  if(rate != NULL){
    *rate = pulse_devout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = pulse_devout->buffer_size;
  }

  if(format != NULL){
    *format = pulse_devout->format;
  }
}

/**
 * ags_pulse_devout_list_cards:
 * @soundcard: the #AgsSoundcard
 * @card_id: pulseaudio identifier
 * @card_name: card name
 *
 * List available soundcards.
 *
 * Since: 0.9.10
 */
void
ags_pulse_devout_list_cards(AgsSoundcard *soundcard,
			    GList **card_id, GList **card_name)
{
  AgsPulseDevout *pulse_devout;

  AgsApplicationContext *application_context;
  
  GList *list, *list_start;

  pthread_mutex_t *application_mutex;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  application_context = (AgsApplicationContext *) pulse_devout->application_context;

  if(application_context == NULL){
    return;
  }
  
  application_mutex = pulse_devout->application_mutex;
  
  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  pthread_mutex_lock(application_mutex);

  list = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  while(list != NULL){
    if(AGS_IS_PULSE_DEVOUT(list->data)){
      if(card_id != NULL){
	if(AGS_PULSE_DEVOUT(list->data)->card_uri != NULL){
	  *card_id = g_list_prepend(*card_id,
				    g_strdup(AGS_PULSE_DEVOUT(list->data)->card_uri));
	}else{
	  *card_id = g_list_prepend(*card_id,
				    g_strdup("(null)"));

	  g_warning("ags_pulse_devout_list_cards() - card id (null)");
	}
      }

      if(card_name != NULL){
	if(AGS_PULSE_DEVOUT(list->data)->pulse_client != NULL){
	  *card_name = g_list_prepend(*card_name,
				      g_strdup(AGS_PULSE_CLIENT(AGS_PULSE_DEVOUT(list->data)->pulse_client)->name));
	}else{
	  *card_name = g_list_prepend(*card_name,
				      g_strdup("(null)"));

	  g_warning("ags_pulse_devout_list_cards() - pulseaudio client not connected (null)");
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
ags_pulse_devout_pcm_info(AgsSoundcard *soundcard,
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
ags_pulse_devout_is_starting(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  
  return((((AGS_PULSE_DEVOUT_START_PLAY & (pulse_devout->flags)) != 0) ? TRUE: FALSE));
}

gboolean
ags_pulse_devout_is_playing(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  
  return((((AGS_PULSE_DEVOUT_PLAY & (pulse_devout->flags)) != 0) ? TRUE: FALSE));
}

gchar*
ags_pulse_devout_get_uptime(AgsSoundcard *soundcard)
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
ags_pulse_devout_port_init(AgsSoundcard *soundcard,
			   GError **error)
{
  AgsPulseDevout *pulse_devout;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  guint format, word_size;
  
  pthread_mutex_t *mutex;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  application_context = ags_soundcard_get_application_context(soundcard);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) pulse_devout);
  
  pthread_mutex_unlock(application_context->mutex);

  /* retrieve word size */
  pthread_mutex_lock(mutex);

  switch(pulse_devout->format){
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
    
    g_warning("ags_pulse_devout_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for playback */
  pulse_devout->flags |= (AGS_PULSE_DEVOUT_BUFFER7 |
			  AGS_PULSE_DEVOUT_START_PLAY |
			  AGS_PULSE_DEVOUT_PLAY |
			  AGS_PULSE_DEVOUT_NONBLOCKING);

  memset(pulse_devout->buffer[0], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->buffer[1], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->buffer[2], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->buffer[3], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->buffer[4], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->buffer[5], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->buffer[6], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->buffer[7], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);

  /*  */
  pulse_devout->tact_counter = 0.0;
  pulse_devout->delay_counter = 0.0;
  pulse_devout->tic_counter = 0;

  pulse_devout->flags |= (AGS_PULSE_DEVOUT_INITIALIZED |
			  AGS_PULSE_DEVOUT_START_PLAY |
			  AGS_PULSE_DEVOUT_PLAY);
  
  g_atomic_int_and(&(pulse_devout->sync_flags),
		   (~(AGS_PULSE_DEVOUT_PASS_THROUGH)));
  g_atomic_int_or(&(pulse_devout->sync_flags),
		  AGS_PULSE_DEVOUT_INITIAL_CALLBACK);

  pthread_mutex_unlock(mutex);
}

void
ags_pulse_devout_port_play(AgsSoundcard *soundcard,
			   GError **error)
{
  AgsPulseClient *pulse_client;
  AgsPulseDevout *pulse_devout;

  AgsNotifySoundcard *notify_soundcard;
  
  AgsMutexManager *mutex_manager;
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  guint word_size;
  gboolean pulse_client_activated;

  pthread_mutex_t *mutex;
  pthread_mutex_t *client_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  application_context = ags_soundcard_get_application_context(soundcard);
  
  /* mutices */
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();
  task_thread = (AgsTaskThread *) application_context->task_thread;

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) pulse_devout);
  
  pthread_mutex_unlock(application_context->mutex);

  /* client */
  pthread_mutex_lock(mutex);

  pulse_client = (AgsPulseClient *) pulse_devout->pulse_client;
  
  callback_mutex = pulse_devout->callback_mutex;
  callback_finish_mutex = pulse_devout->callback_finish_mutex;

  /* do playback */  
  pulse_devout->flags &= (~AGS_PULSE_DEVOUT_START_PLAY);
  notify_soundcard = AGS_NOTIFY_SOUNDCARD(pulse_devout->notify_soundcard);
  
  if((AGS_PULSE_DEVOUT_INITIALIZED & (pulse_devout->flags)) == 0){
    pthread_mutex_unlock(mutex);
    
    return;
  }

  switch(pulse_devout->format){
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
    
    g_warning("ags_pulse_devout_port_play(): unsupported word size");
    
    return;
  }

  pthread_mutex_unlock(mutex);

  /*  */
  pthread_mutex_lock(application_context->mutex);
  
  client_mutex = ags_mutex_manager_lookup(mutex_manager,
					  (GObject *) pulse_client);
  
  pthread_mutex_unlock(application_context->mutex);

  pthread_mutex_lock(client_mutex);

  pulse_client_activated = ((AGS_PULSE_CLIENT_ACTIVATED & (pulse_client->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(client_mutex);

  if(pulse_client_activated){
    /* signal */
    if((AGS_PULSE_DEVOUT_INITIAL_CALLBACK & (g_atomic_int_get(&(pulse_devout->sync_flags)))) == 0){
      pthread_mutex_lock(callback_mutex);

      g_atomic_int_or(&(pulse_devout->sync_flags),
		      AGS_PULSE_DEVOUT_CALLBACK_DONE);
    
      if((AGS_PULSE_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(pulse_devout->sync_flags)))) != 0){
	pthread_cond_signal(pulse_devout->callback_cond);
      }

      pthread_mutex_unlock(callback_mutex);
    }
    
    /* wait callback */	
    if((AGS_PULSE_DEVOUT_INITIAL_CALLBACK & (g_atomic_int_get(&(pulse_devout->sync_flags)))) == 0){
      pthread_mutex_lock(callback_finish_mutex);
    
      if((AGS_PULSE_DEVOUT_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(pulse_devout->sync_flags)))) == 0){
	g_atomic_int_or(&(pulse_devout->sync_flags),
			AGS_PULSE_DEVOUT_CALLBACK_FINISH_WAIT);
    
	while((AGS_PULSE_DEVOUT_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(pulse_devout->sync_flags)))) == 0 &&
	      (AGS_PULSE_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(pulse_devout->sync_flags)))) != 0){
	  pthread_cond_wait(pulse_devout->callback_finish_cond,
			    callback_finish_mutex);
	}
      }
    
      g_atomic_int_and(&(pulse_devout->sync_flags),
		       (~(AGS_PULSE_DEVOUT_CALLBACK_FINISH_WAIT |
			  AGS_PULSE_DEVOUT_CALLBACK_FINISH_DONE)));
    
      pthread_mutex_unlock(callback_finish_mutex);
    }else{
      g_atomic_int_and(&(pulse_devout->sync_flags),
		       (~AGS_PULSE_DEVOUT_INITIAL_CALLBACK));
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
    tic_device = ags_tic_device_new((GObject *) pulse_devout);
    task = g_list_append(task,
    			 tic_device);

    /* reset - clear buffer */
    clear_buffer = ags_clear_buffer_new((GObject *) pulse_devout);
    task = g_list_append(task,
			 clear_buffer);
    
    /* reset - switch buffer flags */
    switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) pulse_devout);
    task = g_list_append(task,
			 switch_buffer_flag);

    /* append tasks */
    ags_task_thread_append_tasks((AgsTaskThread *) task_thread,
				 task);
  }else{
    guint nth_buffer;

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
    }
    
    /* tic */
    ags_soundcard_tic(AGS_SOUNDCARD(pulse_devout));

    switch(pulse_devout->format){
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
      g_warning("ags_pulse_devout_port_play(): unsupported word size");
      return;
    }
    
    memset(pulse_devout->buffer[nth_buffer], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);

    /* reset - switch buffer flags */
    ags_pulse_devout_switch_buffer_flag(pulse_devout);
  }
}

void
ags_pulse_devout_port_free(AgsSoundcard *soundcard)
{
  AgsPulsePort *pulse_port;
  AgsPulseDevout *pulse_devout;

  AgsNotifySoundcard *notify_soundcard;
  
  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  guint word_size;

  pthread_mutex_t *mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  
  application_context = ags_soundcard_get_application_context(soundcard);

  /*  */
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) pulse_devout);
  
  pthread_mutex_unlock(application_context->mutex);

  /*  */
  pthread_mutex_lock(mutex);

  notify_soundcard = AGS_NOTIFY_SOUNDCARD(pulse_devout->notify_soundcard);

  if((AGS_PULSE_DEVOUT_INITIALIZED & (pulse_devout->flags)) == 0){
    pthread_mutex_unlock(mutex);

    return;
  }

  g_object_ref(notify_soundcard);

  callback_mutex = pulse_devout->callback_mutex;
  callback_finish_mutex = pulse_devout->callback_finish_mutex;
  
  //  g_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);
  
  pulse_devout->flags &= (~(AGS_PULSE_DEVOUT_BUFFER0 |
			    AGS_PULSE_DEVOUT_BUFFER1 |
			    AGS_PULSE_DEVOUT_BUFFER2 |
			    AGS_PULSE_DEVOUT_BUFFER3 |
			    AGS_PULSE_DEVOUT_BUFFER4 |
			    AGS_PULSE_DEVOUT_BUFFER5 |
			    AGS_PULSE_DEVOUT_BUFFER6 |
			    AGS_PULSE_DEVOUT_BUFFER7 |
			    AGS_PULSE_DEVOUT_PLAY));

  g_atomic_int_or(&(pulse_devout->sync_flags),
		  AGS_PULSE_DEVOUT_PASS_THROUGH);
  g_atomic_int_and(&(pulse_devout->sync_flags),
		   (~AGS_PULSE_DEVOUT_INITIAL_CALLBACK));

  /* signal callback */
  pthread_mutex_lock(callback_mutex);

  g_atomic_int_or(&(pulse_devout->sync_flags),
		  AGS_PULSE_DEVOUT_CALLBACK_DONE);
    
  if((AGS_PULSE_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(pulse_devout->sync_flags)))) != 0){
    pthread_cond_signal(pulse_devout->callback_cond);
  }

  pthread_mutex_unlock(callback_mutex);

  /* signal thread */
  pthread_mutex_lock(callback_finish_mutex);

  g_atomic_int_or(&(pulse_devout->sync_flags),
		  AGS_PULSE_DEVOUT_CALLBACK_FINISH_DONE);
    
  if((AGS_PULSE_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(pulse_devout->sync_flags)))) != 0){
    pthread_cond_signal(pulse_devout->callback_finish_cond);
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
  pulse_devout->note_offset = 0;
  pulse_devout->note_offset_absolute = 0;

  switch(pulse_devout->format){
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
    
    g_critical("ags_pulse_devout_free(): unsupported word size");
  }

  pthread_mutex_unlock(mutex);

  if(pulse_devout->pulse_port != NULL){
    pulse_port = pulse_devout->pulse_port->data;
    
    while(!g_atomic_int_get(&(pulse_port->is_empty))) usleep(500000);
  }

  pthread_mutex_lock(mutex);
  
  memset(pulse_devout->buffer[0], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->buffer[1], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->buffer[2], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->buffer[3], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->buffer[4], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->buffer[5], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->buffer[6], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->buffer[7], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);

  pthread_mutex_unlock(mutex);
}

void
ags_pulse_devout_tic(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;
  gdouble delay;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  
  /* determine if attack should be switched */
  delay = pulse_devout->delay[pulse_devout->tic_counter];

  if((guint) pulse_devout->delay_counter + 1 >= (guint) delay){
    if(pulse_devout->do_loop &&
       pulse_devout->note_offset + 1 == pulse_devout->loop_right){
      ags_soundcard_set_note_offset(soundcard,
				    pulse_devout->loop_left);
    }else{
      ags_soundcard_set_note_offset(soundcard,
				    pulse_devout->note_offset + 1);
    }
    
    ags_soundcard_set_note_offset_absolute(soundcard,
					   pulse_devout->note_offset_absolute + 1);
    
    /* delay */
    ags_soundcard_offset_changed(soundcard,
				 pulse_devout->note_offset);
    
    /* reset - delay counter */
    pulse_devout->delay_counter = 0.0;
    pulse_devout->tact_counter += 1.0;
  }else{
    pulse_devout->delay_counter += 1.0;
  }
}

void
ags_pulse_devout_offset_changed(AgsSoundcard *soundcard,
				guint note_offset)
{
  AgsPulseDevout *pulse_devout;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  pulse_devout->tic_counter += 1;

  if(pulse_devout->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    pulse_devout->tic_counter = 0;
  }
}

void
ags_pulse_devout_set_bpm(AgsSoundcard *soundcard,
			 gdouble bpm)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  pulse_devout->bpm = bpm;

  ags_pulse_devout_adjust_delay_and_attack(pulse_devout);
}

gdouble
ags_pulse_devout_get_bpm(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  return(pulse_devout->bpm);
}

void
ags_pulse_devout_set_delay_factor(AgsSoundcard *soundcard,
				  gdouble delay_factor)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  pulse_devout->delay_factor = delay_factor;

  ags_pulse_devout_adjust_delay_and_attack(pulse_devout);
}

gdouble
ags_pulse_devout_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  return(pulse_devout->delay_factor);
}

gdouble
ags_pulse_devout_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  
  return((60.0 * (((gdouble) pulse_devout->samplerate / (gdouble) pulse_devout->buffer_size) / (gdouble) pulse_devout->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) pulse_devout->delay_factor))));
}

gdouble
ags_pulse_devout_get_delay(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;
  guint index;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  index = pulse_devout->tic_counter;
  
  return(pulse_devout->delay[index]);
}

guint
ags_pulse_devout_get_attack(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;
  guint index;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  index = pulse_devout->tic_counter;
  
  return(pulse_devout->attack[index]);
}

void*
ags_pulse_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;
  void *buffer;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  if((AGS_PULSE_DEVOUT_BUFFER0 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[0];
  }else if((AGS_PULSE_DEVOUT_BUFFER1 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[1];
  }else if((AGS_PULSE_DEVOUT_BUFFER2 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[2];
  }else if((AGS_PULSE_DEVOUT_BUFFER3 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[3];
  }else if((AGS_PULSE_DEVOUT_BUFFER4 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[4];
  }else if((AGS_PULSE_DEVOUT_BUFFER5 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[5];
  }else if((AGS_PULSE_DEVOUT_BUFFER6 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[6];
  }else if((AGS_PULSE_DEVOUT_BUFFER7 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[7];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_pulse_devout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;
  void *buffer;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  if((AGS_PULSE_DEVOUT_BUFFER0 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[1];
  }else if((AGS_PULSE_DEVOUT_BUFFER1 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[2];
  }else if((AGS_PULSE_DEVOUT_BUFFER2 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[3];
  }else if((AGS_PULSE_DEVOUT_BUFFER3 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[4];
  }else if((AGS_PULSE_DEVOUT_BUFFER4 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[5];
  }else if((AGS_PULSE_DEVOUT_BUFFER5 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[6];
  }else if((AGS_PULSE_DEVOUT_BUFFER6 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[7];
  }else if((AGS_PULSE_DEVOUT_BUFFER7 & (pulse_devout->flags)) != 0){
    buffer = pulse_devout->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

guint
ags_pulse_devout_get_delay_counter(AgsSoundcard *soundcard)
{
  return(AGS_PULSE_DEVOUT(soundcard)->delay_counter);
}

void
ags_pulse_devout_set_note_offset(AgsSoundcard *soundcard,
				 guint note_offset)
{
  AGS_PULSE_DEVOUT(soundcard)->note_offset = note_offset;
}

guint
ags_pulse_devout_get_note_offset(AgsSoundcard *soundcard)
{
  return(AGS_PULSE_DEVOUT(soundcard)->note_offset);
}

void
ags_pulse_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
					  guint note_offset)
{
  AGS_PULSE_DEVOUT(soundcard)->note_offset_absolute = note_offset;
}

guint
ags_pulse_devout_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  return(AGS_PULSE_DEVOUT(soundcard)->note_offset_absolute);
}

void
ags_pulse_devout_set_loop(AgsSoundcard *soundcard,
			  guint loop_left, guint loop_right,
			  gboolean do_loop)
{
  AGS_PULSE_DEVOUT(soundcard)->loop_left = loop_left;
  AGS_PULSE_DEVOUT(soundcard)->loop_right = loop_right;
  AGS_PULSE_DEVOUT(soundcard)->do_loop = do_loop;

  if(do_loop){
    AGS_PULSE_DEVOUT(soundcard)->loop_offset = AGS_PULSE_DEVOUT(soundcard)->note_offset;
  }
}

void
ags_pulse_devout_get_loop(AgsSoundcard *soundcard,
			  guint *loop_left, guint *loop_right,
			  gboolean *do_loop)  
{
  if(loop_left != NULL){
    *loop_left = AGS_PULSE_DEVOUT(soundcard)->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = AGS_PULSE_DEVOUT(soundcard)->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = AGS_PULSE_DEVOUT(soundcard)->do_loop;
  }
}

guint
ags_pulse_devout_get_loop_offset(AgsSoundcard *soundcard)  
{
  return(AGS_PULSE_DEVOUT(soundcard)->loop_offset);
}

void
ags_pulse_devout_set_audio(AgsSoundcard *soundcard,
			   GList *audio)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  pulse_devout->audio = audio;
}

GList*
ags_pulse_devout_get_audio(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  return(pulse_devout->audio);
}

/**
 * ags_pulse_devout_adjust_delay_and_attack:
 * @pulse_devout: the #AgsPulseDevout
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 0.9.10
 */
void
ags_pulse_devout_adjust_delay_and_attack(AgsPulseDevout *pulse_devout)
{
  gdouble delay;
  guint default_tact_frames;
  guint delay_tact_frames;
  guint default_period;
  gint next_attack;
  guint i;

  if(pulse_devout == NULL){
    return;
  }
  
  delay = ags_pulse_devout_get_absolute_delay(AGS_SOUNDCARD(pulse_devout));

#ifdef AGS_DEBUG
  g_message("delay : %f", delay);
#endif
  
  default_tact_frames = (guint) (delay * pulse_devout->buffer_size);
  delay_tact_frames = (guint) (floor(delay) * pulse_devout->buffer_size);
  default_period = (1.0 / AGS_SOUNDCARD_DEFAULT_PERIOD) * (default_tact_frames);

  i = 0;
  
  pulse_devout->attack[0] = (guint) floor(0.5 * pulse_devout->buffer_size);
  next_attack = (((pulse_devout->attack[i] + default_tact_frames) / pulse_devout->buffer_size) - delay) * pulse_devout->buffer_size;

  if(next_attack < 0){
    next_attack = 0;
  }

  if(next_attack >= pulse_devout->buffer_size){
    next_attack = pulse_devout->buffer_size - 1;
  }
  
  /* check if delay drops for next attack */
  if(next_attack < 0){
    pulse_devout->attack[i] = pulse_devout->attack[i] - ((gdouble) next_attack / 2.0);

    if(pulse_devout->attack[i] < 0){
      pulse_devout->attack[i] = 0;
    }
    
    if(pulse_devout->attack[i] >= pulse_devout->buffer_size){
      pulse_devout->attack[i] = pulse_devout->buffer_size - 1;
    }
    
    next_attack = next_attack + (next_attack / 2.0);

    if(next_attack < 0){
      next_attack = 0;
    }

    if(next_attack >= pulse_devout->buffer_size){
      next_attack = pulse_devout->buffer_size - 1;
    }
  }
  
  for(i = 1; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    pulse_devout->attack[i] = next_attack;
    next_attack = (((pulse_devout->attack[i] + default_tact_frames) / pulse_devout->buffer_size) - delay) * pulse_devout->buffer_size;

    if(next_attack >= pulse_devout->buffer_size){
      next_attack = pulse_devout->buffer_size - 1;
    }
    
    /* check if delay drops for next attack */
    if(next_attack < 0){
      pulse_devout->attack[i] = pulse_devout->attack[i] - ((gdouble) next_attack / 2.0);

      if(pulse_devout->attack[i] < 0){
	pulse_devout->attack[i] = 0;
      }

      if(pulse_devout->attack[i] >= pulse_devout->buffer_size){
	pulse_devout->attack[i] = pulse_devout->buffer_size - 1;
      }
    
      next_attack = next_attack + (next_attack / 2.0);

      if(next_attack < 0){
	next_attack = 0;
      }

      if(next_attack >= pulse_devout->buffer_size){
	next_attack = pulse_devout->buffer_size - 1;
      }
    }
    
    //#ifdef AGS_DEBUG
    g_message("%d", pulse_devout->attack[i]);
    //#endif
  }
  
  for(i = 0; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD - 1; i++){
    pulse_devout->delay[i] = ((gdouble) (default_tact_frames + pulse_devout->attack[i] - pulse_devout->attack[i + 1])) / (gdouble) pulse_devout->buffer_size;
    
#ifdef AGS_DEBUG
    g_message("%f", pulse_devout->delay[i]);
#endif
  }

  pulse_devout->delay[i] = ((gdouble) (default_tact_frames + pulse_devout->attack[i] - pulse_devout->attack[0])) / (gdouble) pulse_devout->buffer_size;
}

/**
 * ags_pulse_devout_realloc_buffer:
 * @pulse_devout: the #AgsPulseDevout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 0.9.10
 */
void
ags_pulse_devout_realloc_buffer(AgsPulseDevout *pulse_devout)
{
  guint word_size;

  if(pulse_devout == NULL){
    return;
  }

  switch(pulse_devout->format){
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
    g_warning("ags_pulse_devout_realloc_buffer(): unsupported word size");
    return;
  }
  
  /* AGS_PULSE_DEVOUT_BUFFER_0 */
  if(pulse_devout->buffer[0] != NULL){
    free(pulse_devout->buffer[0]);
  }
  
  pulse_devout->buffer[0] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  
  /* AGS_PULSE_DEVOUT_BUFFER_1 */
  if(pulse_devout->buffer[1] != NULL){
    free(pulse_devout->buffer[1]);
  }

  pulse_devout->buffer[1] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  
  /* AGS_PULSE_DEVOUT_BUFFER_2 */
  if(pulse_devout->buffer[2] != NULL){
    free(pulse_devout->buffer[2]);
  }

  pulse_devout->buffer[2] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  
  /* AGS_PULSE_DEVOUT_BUFFER_3 */
  if(pulse_devout->buffer[3] != NULL){
    free(pulse_devout->buffer[3]);
  }
  
  pulse_devout->buffer[3] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);

  /* AGS_PULSE_DEVOUT_BUFFER_4 */
  if(pulse_devout->buffer[4] != NULL){
    free(pulse_devout->buffer[4]);
  }
  
  pulse_devout->buffer[4] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);

  /* AGS_PULSE_DEVOUT_BUFFER_5 */
  if(pulse_devout->buffer[5] != NULL){
    free(pulse_devout->buffer[5]);
  }
  
  pulse_devout->buffer[5] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);

  /* AGS_PULSE_DEVOUT_BUFFER_6 */
  if(pulse_devout->buffer[6] != NULL){
    free(pulse_devout->buffer[6]);
  }
  
  pulse_devout->buffer[6] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);

  /* AGS_PULSE_DEVOUT_BUFFER_7 */
  if(pulse_devout->buffer[7] != NULL){
    free(pulse_devout->buffer[7]);
  }
  
  pulse_devout->buffer[7] = (void *) malloc(pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
}

/**
 * ags_pulse_devout_new:
 * @application_context: the #AgsApplicationContext
 *
 * Creates an #AgsPulseDevout, refering to @application_context.
 *
 * Returns: a new #AgsPulseDevout
 *
 * Since: 0.9.10
 */
AgsPulseDevout*
ags_pulse_devout_new(GObject *application_context)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = (AgsPulseDevout *) g_object_new(AGS_TYPE_PULSE_DEVOUT,
						 "application-context", application_context,
						 NULL);
  
  return(pulse_devout);
}
