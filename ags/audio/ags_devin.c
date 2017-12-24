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

#include <ags/audio/ags_devin.h>

#include <ags/lib/ags_time.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_concurrent_tree.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_poll_fd.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>
#include <ags/audio/task/ags_notify_soundcard.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#ifndef __APPLE__
#include <sys/soundcard.h>
#endif
#include <errno.h>

#define _GNU_SOURCE
#include <signal.h>
#include <poll.h>

#include <string.h>
#include <math.h>

#include <time.h>
#include <signal.h>
#include <strings.h>
#include <unistd.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_devin_class_init(AgsDevinClass *devin);
void ags_devin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_devin_concurrent_tree_interface_init(AgsConcurrentTreeInterface *concurrent_tree);
void ags_devin_init(AgsDevin *devin);
void ags_devin_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_devin_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_devin_disconnect(AgsConnectable *connectable);
void ags_devin_connect(AgsConnectable *connectable);
pthread_mutex_t* ags_devin_get_lock(AgsConcurrentTree *concurrent_tree);
pthread_mutex_t* ags_devin_get_parent_lock(AgsConcurrentTree *concurrent_tree);
void ags_devin_dispose(GObject *gobject);
void ags_devin_finalize(GObject *gobject);

void ags_devin_set_application_context(AgsSoundcard *soundcard,
				       AgsApplicationContext *application_context);
AgsApplicationContext* ags_devin_get_application_context(AgsSoundcard *soundcard);

void ags_devin_set_application_mutex(AgsSoundcard *soundcard,
				     pthread_mutex_t *application_mutex);
pthread_mutex_t* ags_devin_get_application_mutex(AgsSoundcard *soundcard);

void ags_devin_set_device(AgsSoundcard *soundcard,
			  gchar *device);
gchar* ags_devin_get_device(AgsSoundcard *soundcard);

void ags_devin_set_presets(AgsSoundcard *soundcard,
			   guint channels,
			   guint rate,
			   guint buffer_size,
			   guint format);
void ags_devin_get_presets(AgsSoundcard *soundcard,
			   guint *channels,
			   guint *rate,
			   guint *buffer_size,
			   guint *format);

void ags_devin_list_cards(AgsSoundcard *soundcard,
			  GList **card_id, GList **card_name);
void ags_devin_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			guint *channels_min, guint *channels_max,
			guint *rate_min, guint *rate_max,
			guint *buffer_size_min, guint *buffer_size_max,
			GError **error);

GList* ags_devin_get_poll_fd(AgsSoundcard *soundcard);
gboolean ags_devin_is_available(AgsSoundcard *soundcard);

gboolean ags_devin_is_starting(AgsSoundcard *soundcard);
gboolean ags_devin_is_recording(AgsSoundcard *soundcard);

gchar* ags_devin_get_uptime(AgsSoundcard *soundcard);

void ags_devin_delegate_record_init(AgsSoundcard *soundcard,
				    GError **error);
void ags_devin_delegate_record(AgsSoundcard *soundcard,
			       GError **error);
void ags_devin_delegate_stop(AgsSoundcard *soundcard);

void ags_devin_oss_init(AgsSoundcard *soundcard,
			GError **error);
void ags_devin_oss_record(AgsSoundcard *soundcard,
			  GError **error);
void ags_devin_oss_free(AgsSoundcard *soundcard);

void ags_devin_alsa_init(AgsSoundcard *soundcard,
			 GError **error);
void ags_devin_alsa_record(AgsSoundcard *soundcard,
			   GError **error);
void ags_devin_alsa_free(AgsSoundcard *soundcard);

void ags_devin_tic(AgsSoundcard *soundcard);
void ags_devin_offset_changed(AgsSoundcard *soundcard,
			      guint note_offset);

void ags_devin_set_bpm(AgsSoundcard *soundcard,
		       gdouble bpm);
gdouble ags_devin_get_bpm(AgsSoundcard *soundcard);

void ags_devin_set_delay_factor(AgsSoundcard *soundcard,
				gdouble delay_factor);
gdouble ags_devin_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_devin_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_devin_get_delay(AgsSoundcard *soundcard);
guint ags_devin_get_attack(AgsSoundcard *soundcard);

void* ags_devin_get_buffer(AgsSoundcard *soundcard);
void* ags_devin_get_next_buffer(AgsSoundcard *soundcard);
void* ags_devin_get_prev_buffer(AgsSoundcard *soundcard);

guint ags_devin_get_delay_counter(AgsSoundcard *soundcard);

void ags_devin_set_note_offset(AgsSoundcard *soundcard,
			       guint note_offset);
guint ags_devin_get_note_offset(AgsSoundcard *soundcard);

void ags_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
					guint note_offset);
guint ags_devin_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_devin_set_loop(AgsSoundcard *soundcard,
			guint loop_left, guint loop_right,
			gboolean do_loop);
void ags_devin_get_loop(AgsSoundcard *soundcard,
			guint *loop_left, guint *loop_right,
			gboolean *do_loop);

guint ags_devin_get_loop_offset(AgsSoundcard *soundcard);

void ags_devin_set_audio(AgsSoundcard *soundcard,
			 GList *audio);
GList* ags_devin_get_audio(AgsSoundcard *soundcard);

/**
 * SECTION:ags_devin
 * @short_description: Output to soundcard
 * @title: AgsDevin
 * @section_id:
 * @include: ags/audio/ags_devin.h
 *
 * #AgsDevin represents a soundcard and supports output.
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
};

enum{
  LAST_SIGNAL,
};

static gpointer ags_devin_parent_class = NULL;
static guint devin_signals[LAST_SIGNAL];

const int ags_devin_endian_i = 1;
#define is_bigendian() ( (*(char*)&ags_devin_endian_i) == 0 )

GType
ags_devin_get_type (void)
{
  static GType ags_type_devin = 0;

  if(!ags_type_devin){
    static const GTypeInfo ags_devin_info = {
      sizeof (AgsDevinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_devin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsDevin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_devin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_devin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_devin_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_concurrent_tree_interface_info = {
      (GInterfaceInitFunc) ags_devin_concurrent_tree_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_devin = g_type_register_static(G_TYPE_OBJECT,
					    "AgsDevin",
					    &ags_devin_info,
					    0);

    g_type_add_interface_static(ags_type_devin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_devin,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_type_add_interface_static(ags_type_devin,
				AGS_TYPE_CONCURRENT_TREE,
				&ags_concurrent_tree_interface_info);
  }

  return (ags_type_devin);
}

void
ags_devin_class_init(AgsDevinClass *devin)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_devin_parent_class = g_type_class_peek_parent(devin);

  /* GObjectClass */
  gobject = (GObjectClass *) devin;

  gobject->set_property = ags_devin_set_property;
  gobject->get_property = ags_devin_get_property;

  gobject->dispose = ags_devin_dispose;
  gobject->finalize = ags_devin_finalize;

  /* properties */
  /**
   * AgsDevin:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 1.2.0
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
   * AgsDevin:application-mutex:
   *
   * The assigned application mutex
   * 
   * Since: 1.2.0
   */
  param_spec = g_param_spec_pointer("application-mutex",
				    i18n_pspec("the application mutex object"),
				    i18n_pspec("The application mutex object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_MUTEX,
				  param_spec);

  /**
   * AgsDevin:device:
   *
   * The alsa soundcard indentifier
   * 
   * Since: 1.2.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "hw:0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsDevin:dsp-channels:
   *
   * The dsp channel count
   * 
   * Since: 1.2.0
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
   * AgsDevin:pcm-channels:
   *
   * The pcm channel count
   * 
   * Since: 1.2.0
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

  /*
   * TODO:JK: add support for other quality than 16 bit
   */
  /**
   * AgsDevin:format:
   *
   * The precision of the buffer
   * 
   * Since: 1.2.0
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
   * AgsDevin:buffer-size:
   *
   * The buffer size
   * 
   * Since: 1.2.0
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("frame count of a buffer"),
				 i18n_pspec("The count of frames a buffer contains"),
				 1,
				 44100,
				 AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsDevin:samplerate:
   *
   * The samplerate
   * 
   * Since: 1.2.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("frames per second"),
				 i18n_pspec("The frames count recorded during a second"),
				 8000,
				 96000,
				 44100,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsDevin:buffer:
   *
   * The buffer
   * 
   * Since: 1.2.0
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to record"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsDevin:bpm:
   *
   * Beats per minute
   * 
   * Since: 1.2.0
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
   * AgsDevin:delay-factor:
   *
   * tact
   * 
   * Since: 1.2.0
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
   * AgsDevin:attack:
   *
   * Attack of the buffer
   * 
   * Since: 1.2.0
   */
  param_spec = g_param_spec_pointer("attack",
				    i18n_pspec("attack of buffer"),
				    i18n_pspec("The attack to use for the buffer"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);


  /* AgsDevinClass */
}

GQuark
ags_devin_error_quark()
{
  return(g_quark_from_static_string("ags-devin-error-quark"));
}

void
ags_devin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_devin_connect;
  connectable->disconnect = ags_devin_disconnect;
}

void
ags_devin_concurrent_tree_interface_init(AgsConcurrentTreeInterface *concurrent_tree)
{
  concurrent_tree->get_lock = ags_devin_get_lock;
  concurrent_tree->get_parent_lock = ags_devin_get_parent_lock;
}

void
ags_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_application_context = ags_devin_set_application_context;
  soundcard->get_application_context = ags_devin_get_application_context;

  soundcard->set_application_mutex = ags_devin_set_application_mutex;
  soundcard->get_application_mutex = ags_devin_get_application_mutex;

  soundcard->set_device = ags_devin_set_device;
  soundcard->get_device = ags_devin_get_device;
  
  soundcard->set_presets = ags_devin_set_presets;
  soundcard->get_presets = ags_devin_get_presets;

  soundcard->list_cards = ags_devin_list_cards;
  soundcard->pcm_info = ags_devin_pcm_info;

  soundcard->get_poll_fd = ags_devin_get_poll_fd;
  soundcard->is_available = ags_devin_is_available;

  soundcard->is_starting =  ags_devin_is_starting;
  soundcard->is_recording = ags_devin_is_recording;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_devin_get_uptime;
  
  soundcard->record_init = ags_devin_delegate_record_init;
  soundcard->record = ags_devin_delegate_record;
  
  soundcard->record_init = NULL;
  soundcard->record = NULL;
  
  soundcard->stop = ags_devin_delegate_stop;

  soundcard->tic = ags_devin_tic;
  soundcard->offset_changed = ags_devin_offset_changed;
    
  soundcard->set_bpm = ags_devin_set_bpm;
  soundcard->get_bpm = ags_devin_get_bpm;

  soundcard->set_delay_factor = ags_devin_set_delay_factor;
  soundcard->get_delay_factor = ags_devin_get_delay_factor;

  soundcard->get_absolute_delay = ags_devin_get_absolute_delay;

  soundcard->get_delay = ags_devin_get_delay;
  soundcard->get_attack = ags_devin_get_attack;

  soundcard->get_buffer = ags_devin_get_buffer;
  soundcard->get_next_buffer = ags_devin_get_next_buffer;
  soundcard->get_prev_buffer = ags_devin_get_prev_buffer;

  soundcard->get_delay_counter = ags_devin_get_delay_counter;

  soundcard->set_note_offset = ags_devin_set_note_offset;
  soundcard->get_note_offset = ags_devin_get_note_offset;

  soundcard->set_note_offset_absolute = ags_devin_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_devin_get_note_offset_absolute;

  soundcard->set_loop = ags_devin_set_loop;
  soundcard->get_loop = ags_devin_get_loop;

  soundcard->get_loop_offset = ags_devin_get_loop_offset;

  soundcard->set_audio = ags_devin_set_audio;
  soundcard->get_audio = ags_devin_get_audio;
}

void
ags_devin_init(AgsDevin *devin)
{
  AgsMutexManager *mutex_manager;
  
  AgsConfig *config;
  
  gchar *str;

  gboolean use_alsa;  
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert devin mutex */
  devin->mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  devin->mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) devin,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  config = ags_config_get_instance();

#ifdef AGS_WITH_ALSA
  use_alsa = TRUE;
#else
  use_alsa = FALSE;
#endif

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "backend");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "backend");
  }
  
  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "oss",
			  4)){
    use_alsa = FALSE;
  }

  if(use_alsa){
    devin->flags = (AGS_DEVIN_ALSA);
  }else{
    devin->flags = (AGS_DEVIN_OSS);
  }

  /* quality */
  devin->dsp_channels = AGS_SOUNDCARD_DEFAULT_DSP_CHANNELS;
  devin->pcm_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
  devin->format = AGS_SOUNDCARD_SIGNED_16_BIT;
  devin->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  devin->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

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
    devin->dsp_channels = g_ascii_strtoull(str,
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
    devin->pcm_channels = g_ascii_strtoull(str,
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
    devin->samplerate = g_ascii_strtoull(str,
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
    devin->buffer_size = g_ascii_strtoull(str,
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
    devin->format = g_ascii_strtoull(str,
				     NULL,
				     10);
    free(str);
  }

  /* device */
  if(use_alsa){
    devin->out.alsa.handle = NULL;
    devin->out.alsa.device = AGS_DEVIN_DEFAULT_ALSA_DEVICE;
  }else{
    devin->out.oss.device_fd = -1;
    devin->out.oss.device = AGS_DEVIN_DEFAULT_OSS_DEVICE;
  }

  /* buffer */
  devin->buffer = (void **) malloc(4 * sizeof(void*));

  devin->buffer[0] = NULL;
  devin->buffer[1] = NULL;
  devin->buffer[2] = NULL;
  devin->buffer[3] = NULL;

  g_atomic_int_set(&(devin->available),
		   FALSE);
  
  devin->ring_buffer_size = AGS_DEVIN_DEFAULT_RING_BUFFER_SIZE;
  devin->nth_ring_buffer = 0;
  
  devin->ring_buffer = NULL;

  ags_devin_realloc_buffer(devin);
  
  /* bpm */
  devin->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  devin->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* delay and attack */
  devin->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
				    sizeof(gdouble));
  
  devin->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
				   sizeof(guint));

  ags_devin_adjust_delay_and_attack(devin);
  
  /* counters */
  devin->tact_counter = 0.0;
  devin->delay_counter = 0;
  devin->tic_counter = 0;

  devin->note_offset = 0;
  devin->note_offset_absolute = 0;

  devin->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  devin->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  devin->do_loop = FALSE;

  devin->loop_offset = 0;
  
  /* parent */
  devin->application_context = NULL;
  devin->application_mutex = NULL;

  devin->poll_fd = NULL;
  devin->notify_soundcard = NULL;
  
  /* all AgsAudio */
  devin->audio = NULL;
}

void
ags_devin_set_property(GObject *gobject,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec *param_spec)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if(devin->application_context == (GObject *) application_context){
	return;
      }

      if(devin->application_context != NULL){
	g_object_unref(G_OBJECT(devin->application_context));
      }

      if(application_context != NULL){
	AgsConfig *config;

	gchar *segmentation;
	guint denumerator, numerator;
	
	g_object_ref(G_OBJECT(application_context));

	devin->application_mutex = application_context->mutex;
	
	config = ags_config_get_instance();

	/* segmentation */
	segmentation = ags_config_get_value(config,
					    AGS_CONFIG_GENERIC,
					    "segmentation");

	if(segmentation != NULL){
	  sscanf(segmentation, "%d/%d",
		 &denumerator,
		 &numerator);
    
	  devin->delay_factor = 1.0 / numerator * (numerator / denumerator);

	  g_free(segmentation);
	}

	ags_devin_adjust_delay_and_attack(devin);
	ags_devin_realloc_buffer(devin);
      }else{
	devin->application_mutex = NULL;
      }

      devin->application_context = (GObject *) application_context;
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      pthread_mutex_t *application_mutex;

      application_mutex = (pthread_mutex_t *) g_value_get_pointer(value);

      if(devin->application_mutex == application_mutex){
	return;
      }
      
      devin->application_mutex = application_mutex;
    }
    break;
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      if((AGS_DEVIN_OSS & (devin->flags)) != 0){
	devin->out.oss.device = g_strdup(device);
      }else if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
	devin->out.alsa.device = g_strdup(device);
      }
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      if(dsp_channels == devin->dsp_channels){
	return;
      }

      devin->dsp_channels = dsp_channels;
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      if(pcm_channels == devin->pcm_channels){
	return;
      }

      devin->pcm_channels = pcm_channels;

      ags_devin_realloc_buffer(devin);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      if(format == devin->format){
	return;
      }

      devin->format = format;

      ags_devin_realloc_buffer(devin);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      if(buffer_size == devin->buffer_size){
	return;
      }

      devin->buffer_size = buffer_size;

      ags_devin_realloc_buffer(devin);
      ags_devin_adjust_delay_and_attack(devin);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      if(samplerate == devin->samplerate){
	return;
      }

      devin->samplerate = samplerate;
      ags_devin_adjust_delay_and_attack(devin);
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

      devin->bpm = bpm;

      ags_devin_adjust_delay_and_attack(devin);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      devin->delay_factor = delay_factor;

      ags_devin_adjust_delay_and_attack(devin);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_devin_get_property(GObject *gobject,
		       guint prop_id,
		       GValue *value,
		       GParamSpec *param_spec)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, devin->application_context);
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      g_value_set_pointer(value, devin->application_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      if((AGS_DEVIN_OSS & (devin->flags)) != 0){
	g_value_set_string(value, devin->out.oss.device);
      }else if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
	g_value_set_string(value, devin->out.alsa.device);
      }
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_value_set_uint(value, devin->dsp_channels);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_value_set_uint(value, devin->pcm_channels);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, devin->format);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, devin->buffer_size);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, devin->samplerate);
    }
    break;
  case PROP_BUFFER:
    {
      g_value_set_pointer(value, devin->buffer);
    }
    break;
  case PROP_BPM:
    {
      g_value_set_double(value, devin->bpm);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_value_set_double(value, devin->delay_factor);
    }
    break;
  case PROP_ATTACK:
    {
      g_value_set_pointer(value, devin->attack);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

pthread_mutex_t*
ags_devin_get_lock(AgsConcurrentTree *concurrent_tree)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *devin_mutex;
  
  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);
  
  devin_mutex = ags_mutex_manager_lookup(mutex_manager,
					 G_OBJECT(concurrent_tree));

  pthread_mutex_unlock(application_mutex);

  return(devin_mutex);
}

pthread_mutex_t*
ags_devin_get_parent_lock(AgsConcurrentTree *concurrent_tree)
{
  return(NULL);
}

void
ags_devin_dispose(GObject *gobject)
{
  AgsDevin *devin;

  GList *list;

  devin = AGS_DEVIN(gobject);

  /* application context */
  if(devin->application_context != NULL){
    g_object_unref(devin->application_context);

    devin->application_context = NULL;
  }

  /* unref audio */  
  if(devin->audio != NULL){
    list = devin->audio;

    while(list != NULL){
      g_object_set(G_OBJECT(list->data),
		   "soundcard", NULL,
		   NULL);
      
      list = list->next;
    }
    
    g_list_free_full(devin->audio,
		     g_object_unref);

    devin->audio = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_devin_parent_class)->dispose(gobject);
}

void
ags_devin_finalize(GObject *gobject)
{
  AgsDevin *devin;

  AgsMutexManager *mutex_manager;
  
  GList *list;

  devin = AGS_DEVIN(gobject);

  /* remove devin mutex */
  pthread_mutex_lock(devin->application_mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(devin->application_mutex);

  /* free output buffer */
  free(devin->buffer[0]);
  free(devin->buffer[1]);
  free(devin->buffer[2]);
  free(devin->buffer[3]);

  /* free buffer array */
  free(devin->buffer);

  /* free AgsAttack */
  free(devin->attack);

  /* notify soundcard */
  if(devin->notify_soundcard != NULL){
    if(devin->application_context != NULL){
      ags_task_thread_remove_cyclic_task(AGS_APPLICATION_CONTEXT(devin->application_context)->task_thread,
					 devin->notify_soundcard);
    }

    g_object_unref(devin->notify_soundcard);
  }

  /* application context */
  if(devin->application_context != NULL){
    g_object_unref(devin->application_context);
  }
  
  /* unref audio */  
  if(devin->audio != NULL){
    list = devin->audio;

    while(list != NULL){
      g_object_set(G_OBJECT(list->data),
		   "soundcard", NULL,
		   NULL);
      
      list = list->next;
    }

    g_list_free_full(devin->audio,
		     g_object_unref);
  }
  
  pthread_mutex_destroy(devin->mutex);
  free(devin->mutex);

  pthread_mutexattr_destroy(devin->mutexattr);
  free(devin->mutexattr);

  /* call parent */
  G_OBJECT_CLASS(ags_devin_parent_class)->finalize(gobject);
}

void
ags_devin_connect(AgsConnectable *connectable)
{
  AgsDevin *devin;

  GList *list;

  devin = AGS_DEVIN(connectable);

  /*  */  
  list = devin->audio;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_devin_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_devin_switch_buffer_flag:
 * @devin: an #AgsDevin
 *
 * The buffer flag indicates the currently recorded buffer.
 *
 * Since: 1.2.0
 */
void
ags_devin_switch_buffer_flag(AgsDevin *devin)
{
  AgsApplicationContext *application_context;
  
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *mutex;
  
  application_context = ags_soundcard_get_application_context(AGS_SOUNDCARD(devin));
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) devin);
  
  pthread_mutex_unlock(application_context->mutex);

  //  g_message("switch - 0x%0x", ((AGS_DEVIN_BUFFER0 |
  //				  AGS_DEVIN_BUFFER1 |
  //				  AGS_DEVIN_BUFFER2 |
  //				  AGS_DEVIN_BUFFER3) & (devin->flags)));
  
  /* switch buffer flag */
  pthread_mutex_lock(mutex);

  if((AGS_DEVIN_BUFFER0 & (devin->flags)) != 0){
    devin->flags &= (~AGS_DEVIN_BUFFER0);
    devin->flags |= AGS_DEVIN_BUFFER1;
  }else if((AGS_DEVIN_BUFFER1 & (devin->flags)) != 0){
    devin->flags &= (~AGS_DEVIN_BUFFER1);
    devin->flags |= AGS_DEVIN_BUFFER2;
  }else if((AGS_DEVIN_BUFFER2 & (devin->flags)) != 0){
    devin->flags &= (~AGS_DEVIN_BUFFER2);
    devin->flags |= AGS_DEVIN_BUFFER3;
  }else if((AGS_DEVIN_BUFFER3 & (devin->flags)) != 0){
    devin->flags &= (~AGS_DEVIN_BUFFER3);
    devin->flags |= AGS_DEVIN_BUFFER0;
  }

  pthread_mutex_unlock(mutex);
}

void
ags_devin_set_application_context(AgsSoundcard *soundcard,
				  AgsApplicationContext *application_context)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);
  devin->application_context = (GObject *) application_context;
}

AgsApplicationContext*
ags_devin_get_application_context(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);

  return((AgsApplicationContext *) devin->application_context);
}

void
ags_devin_set_application_mutex(AgsSoundcard *soundcard,
				pthread_mutex_t *application_mutex)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);
  devin->application_mutex = application_mutex;
}

pthread_mutex_t*
ags_devin_get_application_mutex(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);

  return(devin->application_mutex);
}

void
ags_devin_set_device(AgsSoundcard *soundcard,
		     gchar *device)
{
  AgsDevin *devin;

  GList *card_id, *card_id_start, *card_name, *card_name_start;
  
  devin = AGS_DEVIN(soundcard);

  card_id = NULL;
  card_name = NULL;
  
  ags_soundcard_list_cards(soundcard,
			   &card_id, &card_name);

  card_id_start = card_id;
  card_name_start = card_name;
  
  while(card_id != NULL){
    if(!g_ascii_strncasecmp(card_id->data,
			    device,
			    strlen(card_id->data))){
      if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
	devin->out.alsa.device = g_strdup(device);
      }else{
	devin->out.oss.device = g_strdup(device);
      }

      break;
    }
    
    card_id = card_id->next;
  }

  g_list_free_full(card_id_start,
		   g_free);
  g_list_free_full(card_name_start,
		   g_free);
}

gchar*
ags_devin_get_device(AgsSoundcard *soundcard)
{
  AgsDevin *devin;
  
  devin = AGS_DEVIN(soundcard);
  
  if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
    return(devin->out.alsa.device);
  }else{
    return(devin->out.oss.device);
  }
}

void
ags_devin_set_presets(AgsSoundcard *soundcard,
		      guint channels,
		      guint rate,
		      guint buffer_size,
		      guint format)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);
  g_object_set(devin,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_devin_get_presets(AgsSoundcard *soundcard,
		      guint *channels,
		      guint *rate,
		      guint *buffer_size,
		      guint *format)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);

  if(channels != NULL){
    *channels = devin->pcm_channels;
  }

  if(rate != NULL){
    *rate = devin->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = devin->buffer_size;
  }

  if(format != NULL){
    *format = devin->format;
  }
}

/**
 * ags_devin_list_cards:
 * @soundcard: the #AgsSoundcard
 * @card_id: alsa identifier
 * @card_name: card name
 *
 * List available soundcards.
 *
 * Since: 1.2.0
 */
void
ags_devin_list_cards(AgsSoundcard *soundcard,
		     GList **card_id, GList **card_name)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);

  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
#ifdef AGS_WITH_ALSA
    snd_ctl_t *card_handle;
    snd_ctl_card_info_t *card_info;

    char *name;
    gchar *str;
    gchar *str_err;
  
    int card_num;
    int device;
    int error;

    card_num = -1;

    while(TRUE){
      error = snd_card_next(&card_num);

      if(card_num < 0 || error < 0){
	str_err = snd_strerror(error);
	g_message("Can't get the next card number: %s", str_err);

	//free(str_err);
      
	break;
      }

      str = g_strdup_printf("hw:%d", card_num);

#ifdef AGS_DEBUG
      g_message("found soundcard - %s", str);
#endif
    
      error = snd_ctl_open(&card_handle, str, 0);

      if(error < 0){
	g_free(str);
      
	continue;
      }

      snd_ctl_card_info_alloca(&card_info);
      error = snd_ctl_card_info(card_handle, card_info);

      if(error < 0){
	g_free(str);
      
	continue;
      }

      device = -1;
      error = snd_ctl_pcm_next_device(card_handle, &device);

      if(error < 0){
	g_free(str);
      
	continue;
      }

      if(card_id != NULL){
	*card_id = g_list_prepend(*card_id, str);
      }

      if(card_name != NULL){
	*card_name = g_list_prepend(*card_name, g_strdup(snd_ctl_card_info_get_name(card_info)));
      }
    
      snd_ctl_close(card_handle);
    }

    snd_config_update_free_global();
#endif
  }else{
#ifdef AGS_WITH_OSS
    oss_sysinfo sysinfo;
    oss_audioinfo ai;

    char *mixer_device;
    
    int mixerfd = -1;

    int next, n;
    int i;

    if((mixer_device = getenv("OSS_MIXERDEV")) == NULL){
      mixer_device = "/dev/mixer";
    }

    if((mixerfd = open(mixer_device, O_RDONLY, 0)) == -1){
      int e = errno;
      
      switch(e){
      case ENXIO:
      case ENODEV:
	{
	  g_warning("Open Sound System is not running in your system.");
	}
	break;
      case ENOENT:
	{
	  g_warning("No %s device available in your system.\nPerhaps Open Sound System is not installed or running.", mixer_device);
	}
	break;  
      default:
	g_warning("%s", strerror(e));
      }
    }
      
    if(ioctl(mixerfd, SNDCTL_SYSINFO, &sysinfo) == -1){
      if(errno == ENXIO){
	g_warning("OSS has not detected any supported sound hardware in your system.");
      }else{
	g_warning("SNDCTL_SYSINFO");

	if(errno == EINVAL){
	  g_warning("Error: OSS version 4.0 or later is required");
	}
      }

      n = 0;
    }else{
      n = sysinfo.numaudios;
    }

    memset(&ai, 0, sizeof(oss_audioinfo));
    ioctl(mixerfd, SNDCTL_AUDIOINFO_EX, &ai);

    for(i = 0; i < n; i++){
      ai.dev = i;

      if(ioctl(mixerfd, SNDCTL_ENGINEINFO, &ai) == -1){
	int e = errno;
	
	g_warning("Can't get device info for /dev/dsp%d (SNDCTL_AUDIOINFO)\nerrno = %d: %s", i, e, strerror(e));
	
	continue;
      }
      
      if((DSP_CAP_OUTPUT & (ai.caps)) != 0){
	if(card_id != NULL){
	  *card_id = g_list_prepend(*card_id,
				    g_strdup_printf("/dev/dsp%i", i));
	}
	
	if(card_name != NULL){
	  *card_name = g_list_prepend(*card_name,
				      g_strdup(ai.name));
	}
      }

      next = ai.next_rec_engine;
      
      if(next <= 0){
	break;
      }
    }
#endif
  }

  if(card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

void
ags_devin_pcm_info(AgsSoundcard *soundcard,
		   char *card_id,
		   guint *channels_min, guint *channels_max,
		   guint *rate_min, guint *rate_max,
		   guint *buffer_size_min, guint *buffer_size_max,
		   GError **error)
{
  AgsDevin *devin;

  if(card_id == NULL){
    return;
  }
  
  devin = AGS_DEVIN(soundcard);
  
  if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
#ifdef AGS_WITH_ALSA
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    
    gchar *str;
    
    unsigned int val;
    int dir;
    snd_pcm_uframes_t frames;

    int rc;
    int err;

    /* Open PCM device for recordback. */
    handle = NULL;

    rc = snd_pcm_open(&handle, card_id, SND_PCM_STREAM_CAPTURE, 0);

    if(rc < 0) {
      str = snd_strerror(rc);
      g_message("unable to open pcm device: %s\n", str);

      if(error != NULL){
	g_set_error(error,
		    AGS_DEVIN_ERROR,
		    AGS_DEVIN_ERROR_LOCKED_SOUNDCARD,
		    "unable to open pcm device: %s\n",
		    str);
      }
    
      //    free(str);
    
      return;
    }

    /* Allocate a hardware parameters object. */
    snd_pcm_hw_params_alloca(&params);

    /* Fill it in with default values. */
    snd_pcm_hw_params_any(handle, params);

    /* channels */
    snd_pcm_hw_params_get_channels_min(params, &val);
    *channels_min = val;

    snd_pcm_hw_params_get_channels_max(params, &val);
    *channels_max = val;

    /* samplerate */
    dir = 0;
    snd_pcm_hw_params_get_rate_min(params, &val, &dir);
    *rate_min = val;

    dir = 0;
    snd_pcm_hw_params_get_rate_max(params, &val, &dir);
    *rate_max = val;

    /* buffer size */
    dir = 0;
    snd_pcm_hw_params_get_buffer_size_min(params, &frames);
    *buffer_size_min = frames;

    dir = 0;
    snd_pcm_hw_params_get_buffer_size_max(params, &frames);
    *buffer_size_max = frames;

    snd_pcm_close(handle);
#endif
  }else{
#ifdef AGS_WITH_OSS
    oss_audioinfo ainfo;

    gchar *str;
    
    int mixerfd;
    int acc;
    unsigned int cmd;
    
    mixerfd = open(card_id, O_RDWR, 0);

    if(mixerfd == -1){
      int e = errno;
      
      str = strerror(e);
      g_message("unable to open pcm device: %s\n", str);

      if(error != NULL){
	g_set_error(error,
		    AGS_DEVIN_ERROR,
		    AGS_DEVIN_ERROR_LOCKED_SOUNDCARD,
		    "unable to open pcm device: %s\n",
		    str);
      }
    
      return;      
    }
    
    memset(&ainfo, 0, sizeof (ainfo));
    
    cmd = SNDCTL_AUDIOINFO;

    if(card_id != NULL &&
       !g_ascii_strncasecmp(card_id,
			    "/dev/dsp",
			    8)){
      if(strlen(card_id) > 8){
	sscanf(card_id,
	       "/dev/dsp%d",
	       &(ainfo.dev));
      }else{
	ainfo.dev = 0;
      }
    }else{
      return;
    }
    
    if(ioctl(mixerfd, cmd, &ainfo) == -1){
      int e = errno;

      str = strerror(e);
      g_message("unable to retrieve audio info: %s\n", str);

      if(error != NULL){
	g_set_error(error,
		    AGS_DEVIN_ERROR,
		    AGS_DEVIN_ERROR_LOCKED_SOUNDCARD,
		    "unable to retrieve audio info: %s\n",
		    str);
      }
    
      return;
    }
  
    *channels_min = ainfo.min_channels;
    *channels_max = ainfo.max_channels;
    *rate_min = ainfo.min_rate;
    *rate_max = ainfo.max_rate;
    *buffer_size_min = 64;
    *buffer_size_max = 8192;
#endif
  }
}

GList*
ags_devin_get_poll_fd(AgsSoundcard *soundcard)
{
  AgsDevin *devin;
  AgsPollFd *poll_fd;

  GList *list;

  struct pollfd *fds;
  
  gint count;
  guint i;

  devin = AGS_DEVIN(soundcard);
  
  if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
    if(devin->out.alsa.handle == NULL){
      return(NULL);
    }
  }else{
    if(devin->out.oss.device_fd == -1){
      return(NULL);
    }
  }
  
  if(devin->poll_fd == NULL){
    count = 0;
    
    if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
#ifdef AGS_WITH_ALSA
      /* get poll fds of ALSA */
      count = snd_pcm_poll_descriptors_count(devin->out.alsa.handle);

      if(count > 0){
	fds = (struct pollfd *) malloc(count * sizeof(struct pollfd));
	snd_pcm_poll_descriptors(devin->out.alsa.handle, fds, count);
      }
#endif
    }else{
      if(devin->out.oss.device_fd != -1){
	count = 1;
	fds = (struct pollfd *) malloc(sizeof(struct pollfd));
	fds->fd = devin->out.oss.device_fd;
      }
    }
    
    /* map fds */
    list = NULL;

    for(i = 0; i < count; i++){
      poll_fd = ags_poll_fd_new();
      poll_fd->fd = fds[i].fd;
      poll_fd->poll_fd = &(fds[i]);
      
      list = g_list_prepend(list,
			    poll_fd);
    }

    devin->poll_fd = list;
  }else{
    list = devin->poll_fd;
  }
  
  return(list);
}

gboolean
ags_devin_is_available(AgsSoundcard *soundcard)
{
  AgsDevin *devin;
  GList *list;
  
  devin = AGS_DEVIN(soundcard);
  
  list = devin->poll_fd;

  while(list !=	NULL){
    signed short revents;

    if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
#ifdef AGS_WITH_ALSA
      snd_pcm_poll_descriptors_revents(devin->out.alsa.handle, AGS_POLL_FD(list->data)->poll_fd, 1, &revents);
#endif
      
      if((POLLOUT & revents) != 0){
	g_atomic_int_set(&(devin->available),
			 TRUE);
	AGS_POLL_FD(list->data)->poll_fd->revents = 0;

  	return(TRUE);
      }
    }else{
#ifdef AGS_WITH_OSS
      fd_set writefds;

      FD_ZERO(&writefds);
      FD_SET(AGS_POLL_FD(list->data)->poll_fd->fd, &writefds);
      
      if(FD_ISSET(AGS_POLL_FD(list->data)->poll_fd->fd, &writefds)){
	g_atomic_int_set(&(devin->available),
			 TRUE);
	AGS_POLL_FD(list->data)->poll_fd->revents = 0;

	return(TRUE);
      }
#endif
    }
    
    list = list->next;
  }
  
  return(FALSE);
}

gboolean
ags_devin_is_starting(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);
  
  return(((AGS_DEVIN_START_RECORD & (devin->flags)) != 0) ? TRUE: FALSE);
}

gboolean
ags_devin_is_recording(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);
  
  return(((AGS_DEVIN_RECORD & (devin->flags)) != 0) ? TRUE: FALSE);
}

gchar*
ags_devin_get_uptime(AgsSoundcard *soundcard)
{
  gchar *uptime;

  if(ags_soundcard_is_recording(soundcard)){
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
ags_devin_delegate_record_init(AgsSoundcard *soundcard,
			       GError **error)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);

  if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
    ags_devin_alsa_init(soundcard,
			error);
  }else if((AGS_DEVIN_OSS & (devin->flags)) != 0){
    ags_devin_oss_init(soundcard,
		       error);
  }
}

void
ags_devin_delegate_record(AgsSoundcard *soundcard,
			  GError **error)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);

  if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
    ags_devin_alsa_record(soundcard,
			  error);
  }else if((AGS_DEVIN_OSS & (devin->flags)) != 0){
    ags_devin_oss_record(soundcard,
			 error);
  }
}

void
ags_devin_delegate_stop(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);

  if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
    ags_devin_alsa_free(soundcard);
  }else if((AGS_DEVIN_OSS & (devin->flags)) != 0){
    ags_devin_oss_free(soundcard);
  }
}

void
ags_devin_oss_init(AgsSoundcard *soundcard,
		   GError **error)
{
  AgsDevin *devin;
  
  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  gchar *str;

  guint word_size;
  int format;
  int tmp;
  guint i;

  pthread_mutex_t *mutex;

  devin = AGS_DEVIN(soundcard);

  application_context = ags_soundcard_get_application_context(soundcard);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) devin);
  
  pthread_mutex_unlock(application_context->mutex);

  /* retrieve word size */
  pthread_mutex_lock(mutex);

  switch(devin->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
#ifdef AGS_WITH_OSS
      format = AFMT_U8;
#endif
      
      word_size = sizeof(signed char);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
#ifdef AGS_WITH_OSS
      format = AFMT_S16_NE;
#endif
      
      word_size = sizeof(signed short);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
#ifdef AGS_WITH_OSS
      format = AFMT_S24_NE;
#endif
      
      word_size = sizeof(signed long);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
#ifdef AGS_WITH_OSS
      format = AFMT_S32_NE;
#endif
      
      word_size = sizeof(signed long);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      word_size = sizeof(signed long long);
    }
  default:
    g_warning("ags_devin_oss_init(): unsupported word size");
    return;
  }

  /* prepare for recordback */
  devin->flags |= (AGS_DEVIN_BUFFER3 |
		   AGS_DEVIN_START_RECORD |
		   AGS_DEVIN_RECORD |
		   AGS_DEVIN_NONBLOCKING);

  memset(devin->buffer[0], 0, devin->pcm_channels * devin->buffer_size * word_size);
  memset(devin->buffer[1], 0, devin->pcm_channels * devin->buffer_size * word_size);
  memset(devin->buffer[2], 0, devin->pcm_channels * devin->buffer_size * word_size);
  memset(devin->buffer[3], 0, devin->pcm_channels * devin->buffer_size * word_size);

  /* allocate ring buffer */
  g_atomic_int_set(&(devin->available),
		   FALSE);
  
  devin->ring_buffer = (unsigned char **) malloc(devin->ring_buffer_size * sizeof(unsigned char *));

  for(i = 0; i < devin->ring_buffer_size; i++){
    devin->ring_buffer[i] = (unsigned char *) malloc(devin->pcm_channels *
						     devin->buffer_size * word_size *
						     sizeof(unsigned char));
  }

#ifdef AGS_WITH_OSS
  /* open device fd */
  str = devin->out.oss.device;
  devin->out.oss.device_fd = open(str, O_RDONLY, 0);

  if(devin->out.oss.device_fd == -1){
    pthread_mutex_unlock(mutex);

    g_warning("couldn't open device %s", devin->out.oss.device);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_LOCKED_SOUNDCARD,
		  "unable to open dsp device: %s\n",
		  str);
    }

    return;
  }

  //NOTE:JK: unsupported on kfreebsd 9.0
  //  tmp = APF_NORMAL;
  //  ioctl(devin->out.oss.device_fd, SNDCTL_DSP_PROFILE, &tmp);

  tmp = format;

  if(ioctl(devin->out.oss.device_fd, SNDCTL_DSP_SETFMT, &tmp) == -1){
    pthread_mutex_unlock(mutex);

    str = strerror(errno);
    g_warning("failed to select bits/sample");

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
		  "unable to open dsp device: %s",
		  str);
    }

    devin->out.oss.device_fd = -1;

    return;
  }
  
  if(tmp != format){
    pthread_mutex_unlock(mutex);

    str = strerror(errno);
    g_warning("failed to select bits/sample");

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
		  "unable to open dsp device: %s",
		  str);
    }

    devin->out.oss.device_fd = -1;

    return;
  }

  tmp = devin->dsp_channels;

  if(ioctl(devin->out.oss.device_fd, SNDCTL_DSP_CHANNELS, &tmp) == -1){
    pthread_mutex_unlock(mutex);

    str = strerror(errno);
    g_warning("Channels count (%i) not available for recordbacks: %s", devin->dsp_channels, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_CHANNELS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.oss.device_fd = -1;
    
    return;
  }

  if(tmp != devin->dsp_channels){
    pthread_mutex_unlock(mutex);

    str = strerror(errno);
    g_warning("Channels count (%i) not available for capture: %s", devin->dsp_channels, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_CHANNELS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }
    
    devin->out.oss.device_fd = -1;
    
    return;
  }

  tmp = devin->samplerate;

  if(ioctl(devin->out.oss.device_fd, SNDCTL_DSP_SPEED, &tmp) == -1){
    pthread_mutex_unlock(mutex);

    str = strerror(errno);
    g_warning("Rate %iHz not available for capture: %s", devin->samplerate, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_SAMPLERATE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.oss.device_fd = -1;
    
    return;
  }

  if(tmp != devin->samplerate){
    g_warning("Warning: Capture using %d Hz (file %d Hz)",
	      tmp,
	      devin->samplerate);
  }
#endif
  
  devin->tact_counter = 0.0;
  devin->delay_counter = 0.0;
  devin->tic_counter = 0;

  devin->nth_ring_buffer = 0;
  
  ags_soundcard_get_poll_fd(soundcard);
  
#ifdef AGS_WITH_OSS
  devin->flags |= AGS_DEVIN_INITIALIZED;
#endif
  devin->flags |= AGS_DEVIN_BUFFER0;
  devin->flags &= (~(AGS_DEVIN_BUFFER1 |
		     AGS_DEVIN_BUFFER2 |
		     AGS_DEVIN_BUFFER3));
  
  pthread_mutex_unlock(mutex);
}

void
ags_devin_oss_record(AgsSoundcard *soundcard,
		     GError **error)
{
  AgsDevin *devin;

  AgsNotifySoundcard *notify_soundcard;
  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsThread *task_thread;
  AgsPollFd *poll_fd;
  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  GList *task;
  GList *list;

  gchar *str;
  
  guint word_size;
  guint nth_buffer;

  int n_write;
  
  pthread_mutex_t *mutex;

  static const struct timespec poll_interval = {
    0,
    250,
  };
  
  auto void ags_devin_oss_record_fill_ring_buffer(void *buffer, guint ags_format, unsigned char *ring_buffer, guint channels, guint buffer_size);

  void ags_devin_oss_record_fill_ring_buffer(void *buffer, guint ags_format, unsigned char *ring_buffer, guint channels, guint buffer_size){
    int format_bits;
    guint word_size;

    int bps;
    int res;
    guint chn;
    guint count, i;
    
    switch(ags_format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	word_size = sizeof(char);
	bps = 1;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	word_size = sizeof(short);
	bps = 2;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	word_size = sizeof(long);
	bps = 3;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	word_size = sizeof(long);
	bps = 4;
      }
      break;
    default:
      g_warning("ags_devin_oss_record(): unsupported word size");
      return;
    }

    /* fill the channel areas */
    for(count = 0; count < buffer_size; count++){
      for(chn = 0; chn < channels; chn++){
	switch(ags_format){
	case AGS_SOUNDCARD_SIGNED_8_BIT:
	  {
	    res = (int) ((signed char *) buffer)[count * channels + chn];
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_16_BIT:
	  {
	    res = (int) ((signed short *) buffer)[count * channels + chn];
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_24_BIT:
	  {
	    res = (int) ((signed long *) buffer)[count * channels + chn];
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_32_BIT:
	  {
	    res = (int) ((signed long *) buffer)[count * channels + chn];
	  }
	  break;
	}
	
	/* Generate data in native endian format */
	if(is_bigendian()){
	  for(i = 0; i < bps; i++){
	    *(ring_buffer + chn * bps + word_size - 1 - i) = (res >> i * 8) & 0xff;
	  }
	}else{
	  for(i = 0; i < bps; i++){
	    *(ring_buffer + chn * bps + i) = (res >>  i * 8) & 0xff;
	  }
	}	
      }

      ring_buffer += channels * bps;
    }
  }
  
  devin = AGS_DEVIN(soundcard);

  /* retrieve mutex */
  application_context = ags_soundcard_get_application_context(soundcard);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) devin);
  
  pthread_mutex_unlock(application_context->mutex);

  /* lock */
  pthread_mutex_lock(mutex);

  notify_soundcard = AGS_NOTIFY_SOUNDCARD(devin->notify_soundcard);
  
  /* notify cyclic task */
  pthread_mutex_lock(notify_soundcard->return_mutex);

  g_atomic_int_or(&(notify_soundcard->flags),
		  AGS_NOTIFY_SOUNDCARD_DONE_RETURN);
  
  if((AGS_NOTIFY_SOUNDCARD_WAIT_RETURN & (g_atomic_int_get(&(notify_soundcard->flags)))) != 0){
    pthread_cond_signal(notify_soundcard->return_cond);
  }
  
  pthread_mutex_unlock(notify_soundcard->return_mutex);

  /* retrieve word size */
  switch(devin->format){
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
    //NOTE:JK: not available    break;
  default:
    g_warning("ags_devin_oss_record(): unsupported word size");
    return;
  }

  /* do capture */
  devin->flags &= (~AGS_DEVIN_START_RECORD);

  if((AGS_DEVIN_INITIALIZED & (devin->flags)) == 0){
    pthread_mutex_unlock(mutex);
    
    return;
  }

  /* check buffer flag */
  if((AGS_DEVIN_BUFFER0 & (devin->flags)) != 0){
    nth_buffer = 0;
  }else if((AGS_DEVIN_BUFFER1 & (devin->flags)) != 0){
    nth_buffer = 1;
  }else if((AGS_DEVIN_BUFFER2 & (devin->flags)) != 0){
    nth_buffer = 2;
  }else if((AGS_DEVIN_BUFFER3 & (devin->flags)) != 0){
    nth_buffer = 3;
  }

#ifdef AGS_WITH_OSS    
  /* fill ring buffer */
  ags_devin_oss_record_fill_ring_buffer(devin->buffer[nth_buffer],
					devin->format,
					devin->ring_buffer[devin->nth_ring_buffer],
					devin->pcm_channels,
					devin->buffer_size);

  /* wait until available */
  list = ags_soundcard_get_poll_fd(soundcard);

  if(!ags_soundcard_is_available(soundcard) &&
     !g_atomic_int_get(&(devin->available)) &&
     list != NULL){
    poll_fd = list->data;
    poll_fd->poll_fd->events = POLLOUT;
    
    while(!ags_soundcard_is_available(soundcard) &&
	  !g_atomic_int_get(&(devin->available))){
      ppoll(poll_fd->poll_fd,
	    1,
	    &poll_interval,
	    NULL);
    }
  }
  
  /* write ring buffer */
  n_write = read(devin->out.oss.device_fd,
		 devin->ring_buffer[devin->nth_ring_buffer],
		 devin->pcm_channels * devin->buffer_size * word_size * sizeof (char));

  g_atomic_int_set(&(devin->available),
		   FALSE);
  
  if(n_write != devin->pcm_channels * devin->buffer_size * word_size * sizeof (char)){
    g_critical("write() return doesn't match written bytes");
  }
#endif

  /* increment nth ring-buffer */
  if(devin->nth_ring_buffer + 1 >= devin->ring_buffer_size){
    devin->nth_ring_buffer = 0;
  }else{
    devin->nth_ring_buffer += 1;
  }
  
  pthread_mutex_unlock(mutex);

  /* update soundcard */
  task_thread = ags_thread_find_type((AgsThread *) application_context->main_loop,
				     AGS_TYPE_TASK_THREAD);
  task = NULL;
  
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) devin);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) devin);
  task = g_list_append(task,
		       clear_buffer);
  
  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) devin);
  task = g_list_append(task,
		       clear_buffer);

  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) devin);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_thread_append_tasks((AgsTaskThread *) task_thread,
			       task);
}

void
ags_devin_oss_free(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  AgsNotifySoundcard *notify_soundcard;
  
  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  GList *poll_fd;

  guint i;
  
  pthread_mutex_t *mutex;
  
  devin = AGS_DEVIN(soundcard);
  
  application_context = ags_soundcard_get_application_context(soundcard);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) devin);
  
  pthread_mutex_unlock(application_context->mutex);

  /*  */
  pthread_mutex_lock(mutex);

#ifdef AGS_WITH_OSS
  /* remove poll fd */
  poll_fd = devin->poll_fd;
  
  while(poll_fd != NULL){
    ags_polling_thread_remove_poll_fd(AGS_POLL_FD(poll_fd->data)->polling_thread,
				      poll_fd->data);
    g_object_unref(poll_fd->data);
    
    poll_fd = poll_fd->next;
  }

  g_list_free(poll_fd);

  devin->poll_fd = NULL;
#endif

  notify_soundcard = AGS_NOTIFY_SOUNDCARD(devin->notify_soundcard);

  if((AGS_DEVIN_INITIALIZED & (devin->flags)) == 0){
    pthread_mutex_unlock(mutex);
    
    return;
  }
  
  close(devin->out.oss.device_fd);
  devin->out.oss.device_fd = -1;

  /* free ring-buffer */
  g_atomic_int_set(&(devin->available),
		   FALSE);
  
  for(i = 0; i < devin->ring_buffer_size; i++){
    free(devin->ring_buffer[i]);
  }
  
  free(devin->ring_buffer);

  devin->ring_buffer = NULL;

  /* reset flags */
  devin->flags &= (~(AGS_DEVIN_BUFFER0 |
		     AGS_DEVIN_BUFFER1 |
		     AGS_DEVIN_BUFFER2 |
		     AGS_DEVIN_BUFFER3 |
		     AGS_DEVIN_RECORD |
		     AGS_DEVIN_INITIALIZED));

  /* notify cyclic task */
  pthread_mutex_lock(notify_soundcard->return_mutex);

  g_atomic_int_or(&(notify_soundcard->flags),
		  AGS_NOTIFY_SOUNDCARD_DONE_RETURN);
  
  if((AGS_NOTIFY_SOUNDCARD_WAIT_RETURN & (g_atomic_int_get(&(notify_soundcard->flags)))) != 0){
    pthread_cond_signal(notify_soundcard->return_cond);
  }
  
  pthread_mutex_unlock(notify_soundcard->return_mutex);

  devin->note_offset = 0;
  devin->note_offset_absolute = 0;

  pthread_mutex_unlock(mutex);
}

void
ags_devin_alsa_init(AgsSoundcard *soundcard,
		    GError **error)
{
  AgsDevin *devin;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

#ifdef AGS_WITH_ALSA

  snd_pcm_t *handle;
  snd_pcm_hw_params_t *hwparams;
  snd_pcm_sw_params_t *swparams;

  gchar *str;
  
  int rc;
  unsigned int val;
  snd_pcm_uframes_t frames;
  unsigned int rate;
  unsigned int rrate;
  unsigned int channels;
  snd_pcm_uframes_t size;
  snd_pcm_sframes_t buffer_size;
  snd_pcm_sframes_t period_size;
  snd_pcm_format_t format;

  int period_event;

  int err, dir;
#endif

  guint word_size;
  guint i;
  
  pthread_mutex_t *mutex; 
 
  static unsigned int period_time = 100000;
  static unsigned int buffer_time = 100000;

  devin = AGS_DEVIN(soundcard);

  application_context = ags_soundcard_get_application_context(soundcard);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) devin);
  
  pthread_mutex_unlock(application_context->mutex);

  /* retrieve word size */
  pthread_mutex_lock(mutex);

  switch(devin->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
#ifdef AGS_WITH_ALSA
      format = SND_PCM_FORMAT_S8;
#endif

      word_size = sizeof(signed char);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
#ifdef AGS_WITH_ALSA
      format = SND_PCM_FORMAT_S16;
#endif
      
      word_size = sizeof(signed short);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
#ifdef AGS_WITH_ALSA
      format = SND_PCM_FORMAT_S24;
#endif
      
      //NOTE:JK: The 24-bit linear samples use 32-bit physical space
      word_size = sizeof(signed long);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
#ifdef AGS_WITH_ALSA
      format = SND_PCM_FORMAT_S32;
#endif
      
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

    g_warning("ags_devin_alsa_init(): unsupported word size");

    return;
  }

  /* prepare for capture */
  devin->flags |= (AGS_DEVIN_BUFFER3 |
		   AGS_DEVIN_START_RECORD |
		   AGS_DEVIN_RECORD |
		   AGS_DEVIN_NONBLOCKING);

  memset(devin->buffer[0], 0, devin->pcm_channels * devin->buffer_size * word_size);
  memset(devin->buffer[1], 0, devin->pcm_channels * devin->buffer_size * word_size);
  memset(devin->buffer[2], 0, devin->pcm_channels * devin->buffer_size * word_size);
  memset(devin->buffer[3], 0, devin->pcm_channels * devin->buffer_size * word_size);

  /* allocate ring buffer */
#ifdef AGS_WITH_ALSA
  g_atomic_int_set(&(devin->available),
		   FALSE);
  
  devin->ring_buffer = (unsigned char **) malloc(devin->ring_buffer_size * sizeof(unsigned char *));

  for(i = 0; i < devin->ring_buffer_size; i++){
    devin->ring_buffer[i] = (unsigned char *) malloc(devin->pcm_channels *
						     devin->buffer_size * (snd_pcm_format_physical_width(format) / 8) *
						     sizeof(unsigned char));
  }
 
  /*  */
  period_event = 0;
  
  /* Open PCM device for capture. */
  if ((err = snd_pcm_open(&handle, devin->out.alsa.device, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
    pthread_mutex_unlock(mutex);

    str = snd_strerror(err);
    g_warning("Capture open error: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_LOCKED_SOUNDCARD,
		  "unable to open pcm device: %s",
		  str);
    }
    
    //    free(str);

    return;
  }

  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_sw_params_alloca(&swparams);

  /* choose all parameters */
  err = snd_pcm_hw_params_any(handle, hwparams);

  if (err < 0) {
    pthread_mutex_unlock(mutex);

    str = snd_strerror(err);
    g_warning("Broken configuration for capture: no configurations available: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_BROKEN_CONFIGURATION,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set hardware resampling * /
     err = snd_pcm_hw_params_set_rate_resample(handle, hwparams, 0);
     if (err < 0) {
     pthread_mutex_unlock(mutex);

     str = snd_strerror(err);
     g_warning("Resampling setup failed for capture: %s\n", str);

     //    free(str);
    
     return;
     }
  */
  
  /* set the interleaved read/write format */
  err = snd_pcm_hw_params_set_access(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
  if (err < 0) {
    pthread_mutex_unlock(mutex);

    str = snd_strerror(err);
    g_warning("Access type not available for capture: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_ACCESS_TYPE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the sample format */
  err = snd_pcm_hw_params_set_format(handle, hwparams, format);
  if (err < 0) {
    pthread_mutex_unlock(mutex);

    str = snd_strerror(err);
    g_warning("Sample format not available for capture: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the count of channels */
  channels = devin->pcm_channels;
  err = snd_pcm_hw_params_set_channels(handle, hwparams, channels);
  if (err < 0) {
    pthread_mutex_unlock(mutex);

    str = snd_strerror(err);
    g_warning("Channels count (%i) not available for capture: %s", channels, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_CHANNELS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the stream rate */
  rate = devin->samplerate;
  rrate = rate;
  err = snd_pcm_hw_params_set_rate_near(handle, hwparams, &rrate, 0);
  if (err < 0) {
    pthread_mutex_unlock(mutex);

    str = snd_strerror(err);
    g_warning("Rate %iHz not available for capture: %s", rate, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_SAMPLERATE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  if (rrate != rate) {
    pthread_mutex_unlock(mutex);
    g_warning("Rate doesn't match (requested %iHz, get %iHz)", rate, err);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_SAMPLERATE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.alsa.handle = NULL;
    
    return;
  }

  /* set the buffer size */
  size = 2 * devin->buffer_size;
  err = snd_pcm_hw_params_set_buffer_size(handle, hwparams, size);
  if (err < 0) {
    pthread_mutex_unlock(mutex);

    str = snd_strerror(err);
    g_warning("Unable to set buffer size %lu for capture: %s", size, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_BUFFER_SIZE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the period size * /
     period_size = devin->buffer_size;
     err = snd_pcm_hw_params_set_period_size_near(handle, hwparams, period_size, dir);
     if (err < 0) {
     pthread_mutex_unlock(mutex);

     str = snd_strerror(err);
     g_warning("Unable to get period size for capture: %s\n", str);

     //    free(str);
    
     return;
     }
  */
  
  /* write the parameters to device */
  err = snd_pcm_hw_params(handle, hwparams);
  if (err < 0) {
    pthread_mutex_unlock(mutex);

    str = snd_strerror(err);
    g_warning("Unable to set hw params for capture: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_HW_PARAMETERS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* get the current swparams * /
     err = snd_pcm_sw_params_current(handle, swparams);
     if (err < 0) {
     pthread_mutex_unlock(mutex);

     str = snd_strerror(err);
     g_warning("Unable to determine current swparams for capture: %s\n", str);

     //    free(str);
    
     return;
     }
  */
  /* start the transfer when the buffer is almost full: */
  /* (buffer_size / avail_min) * avail_min * /
     err = snd_pcm_sw_params_set_start_threshold(handle, swparams, (buffer_size / period_size) * period_size);
     if (err < 0) {
     pthread_mutex_unlock(mutex);

     str = snd_strerror(err);
     g_warning("Unable to set start threshold mode for capture: %s\n", str);
    
     //    free(str);
    
     return;
     }
  */
  /* allow the transfer when at least period_size samples can be processed */
  /* or disable this mechanism when period event is enabled (aka interrupt like style processing) * /
     err = snd_pcm_sw_params_set_avail_min(handle, swparams, period_event ? buffer_size : period_size);
     if (err < 0) {
     pthread_mutex_unlock(mutex);

     str = snd_strerror(err);
     g_warning("Unable to set avail min for capture: %s\n", str);

     //    free(str);
    
     return;
     }

     /* write the parameters to the capture device * /
     err = snd_pcm_sw_params(handle, swparams);
     if (err < 0) {
     pthread_mutex_unlock(mutex);

     str = snd_strerror(err);
     g_warning("Unable to set sw params for capture: %s\n", str);

     //    free(str);
    
     return;
     }
  */

  /*  */
  devin->out.alsa.handle = handle;
#endif

  devin->tact_counter = 0.0;
  devin->delay_counter = 0.0;
  devin->tic_counter = 0;

  devin->nth_ring_buffer = 0;
  
  ags_soundcard_get_poll_fd(soundcard);
  
#ifdef AGS_WITH_ALSA
  devin->flags |= AGS_DEVIN_INITIALIZED;
#endif
  devin->flags |= AGS_DEVIN_BUFFER0;
  devin->flags &= (~(AGS_DEVIN_BUFFER1 |
		     AGS_DEVIN_BUFFER2 |
		     AGS_DEVIN_BUFFER3));
  
  pthread_mutex_unlock(mutex);
}

void
ags_devin_alsa_record(AgsSoundcard *soundcard,
		      GError **error)
{
  AgsDevin *devin;

  AgsNotifySoundcard *notify_soundcard;
  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsThread *task_thread;
  AgsPollFd *poll_fd;
  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  GList *task;
  GList *list;
  
  gchar *str;
  
  guint word_size;
  guint nth_buffer;
  
  pthread_mutex_t *mutex;

  static const struct timespec poll_interval = {
    0,
    250,
  };
  
#ifdef AGS_WITH_ALSA
  auto void ags_devin_alsa_record_fill_ring_buffer(void *buffer, guint ags_format, unsigned char *ring_buffer, guint channels, guint buffer_size);

  void ags_devin_alsa_record_fill_ring_buffer(void *buffer, guint ags_format, unsigned char *ring_buffer, guint channels, guint buffer_size){
    snd_pcm_format_t format;

    int format_bits;

    unsigned int max_val;
    
    int bps; /* bytes per sample */
    int phys_bps;

    int big_endian;
    int to_unsigned;

    int res;

    gint count;
    guint i, chn;
    
    switch(ags_format){
    case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	format = SND_PCM_FORMAT_S8;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	format = SND_PCM_FORMAT_S16;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	format = SND_PCM_FORMAT_S24;
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	format = SND_PCM_FORMAT_S32;
      }
      break;
    default:
      g_warning("ags_devin_alsa_record(): unsupported word size");
      return;
    }

    count = buffer_size;
    format_bits = snd_pcm_format_width(format);

    max_val = (1 << (format_bits - 1)) - 1;

    bps = format_bits / 8;
    phys_bps = snd_pcm_format_physical_width(format) / 8;
    
    big_endian = snd_pcm_format_big_endian(format) == 1;
    to_unsigned = snd_pcm_format_unsigned(format) == 1;

    /* fill the channel areas */
    for(count = 0; count < buffer_size; count++){
      for(chn = 0; chn < channels; chn++){
	switch(ags_format){
	case AGS_SOUNDCARD_SIGNED_8_BIT:
	  {
	    res = (int) ((signed char *) buffer)[count * channels + chn];
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_16_BIT:
	  {
	    res = (int) ((signed short *) buffer)[count * channels + chn];
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_24_BIT:
	  {
	    res = (int) ((signed long *) buffer)[count * channels + chn];
	  }
	  break;
	case AGS_SOUNDCARD_SIGNED_32_BIT:
	  {
	    res = (int) ((signed long *) buffer)[count * channels + chn];
	  }
	  break;
	}

	if(to_unsigned){
	  res ^= 1U << (format_bits - 1);
	}
	
	/* Generate data in native endian format */
	if (big_endian) {
	  for (i = 0; i < bps; i++)
	    *(ring_buffer + chn * bps + phys_bps - 1 - i) = (res >> i * 8) & 0xff;
	} else {
	  for (i = 0; i < bps; i++)
	    *(ring_buffer + chn * bps + i) = (res >>  i * 8) & 0xff;
	}	
      }

      ring_buffer += channels * bps;
    }
  }
#endif
  
  devin = AGS_DEVIN(soundcard);

  /*  */
  application_context = ags_soundcard_get_application_context(soundcard);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) devin);
  
  pthread_mutex_unlock(application_context->mutex);

  /* lock */
  pthread_mutex_lock(mutex);

  notify_soundcard = AGS_NOTIFY_SOUNDCARD(devin->notify_soundcard);

  /* notify cyclic task */
  pthread_mutex_lock(notify_soundcard->return_mutex);

  g_atomic_int_or(&(notify_soundcard->flags),
		  AGS_NOTIFY_SOUNDCARD_DONE_RETURN);
  
  if((AGS_NOTIFY_SOUNDCARD_WAIT_RETURN & (g_atomic_int_get(&(notify_soundcard->flags)))) != 0){
    pthread_cond_signal(notify_soundcard->return_cond);
  }
  
  pthread_mutex_unlock(notify_soundcard->return_mutex);

  /* retrieve word size */
  switch(devin->format){
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
    
    g_warning("ags_devin_alsa_record(): unsupported word size");

    return;
  }

  /* do capture */
  devin->flags &= (~AGS_DEVIN_START_RECORD);

  if((AGS_DEVIN_INITIALIZED & (devin->flags)) == 0){
    pthread_mutex_unlock(mutex);
    
    return;
  }

  //  g_message("record - 0x%0x", ((AGS_DEVIN_BUFFER0 |
  //				AGS_DEVIN_BUFFER1 |
  //				AGS_DEVIN_BUFFER2 |
  //				AGS_DEVIN_BUFFER3) & (devin->flags)));

  /* check buffer flag */
  if((AGS_DEVIN_BUFFER0 & (devin->flags)) != 0){
    nth_buffer = 0;
  }else if((AGS_DEVIN_BUFFER1 & (devin->flags)) != 0){
    nth_buffer = 1;
  }else if((AGS_DEVIN_BUFFER2 & (devin->flags)) != 0){
    nth_buffer = 2;
  }else if((AGS_DEVIN_BUFFER3 & (devin->flags)) != 0){
    nth_buffer = 3;
  }

#ifdef AGS_WITH_ALSA

  /* fill ring buffer */
  ags_devin_alsa_record_fill_ring_buffer(devin->buffer[nth_buffer], devin->format,
					 devin->ring_buffer[devin->nth_ring_buffer],
					 devin->pcm_channels, devin->buffer_size);

  /* wait until available */
  list = ags_soundcard_get_poll_fd(soundcard);

  if(!ags_soundcard_is_available(soundcard) &&
     !g_atomic_int_get(&(devin->available)) &&
     list != NULL){
    poll_fd = list->data;
    poll_fd->poll_fd->events = POLLOUT;
    
    while(!ags_soundcard_is_available(soundcard) &&
	  !g_atomic_int_get(&(devin->available))){
      ppoll(poll_fd->poll_fd,
	    1,
	    &poll_interval,
	    NULL);
    }
  }
  
  /* write ring buffer */
  devin->out.alsa.rc = snd_pcm_readi(devin->out.alsa.handle,
				     devin->ring_buffer[devin->nth_ring_buffer],
				     (snd_pcm_uframes_t) (devin->buffer_size));

  g_atomic_int_set(&(devin->available),
		   FALSE);
  
  /* check error flag */
  if((AGS_DEVIN_NONBLOCKING & (devin->flags)) == 0){
    if(devin->out.alsa.rc == -EPIPE){
      /* EPIPE means underrun */
      snd_pcm_prepare(devin->out.alsa.handle);

#ifdef AGS_DEBUG
      g_message("underrun occurred");
#endif
    }else if(devin->out.alsa.rc == -ESTRPIPE){
      static const struct timespec idle = {
	0,
	4000,
      };

      int err;

      while((err = snd_pcm_resume(devin->out.alsa.handle)) < 0){ // == -EAGAIN
	nanosleep(&idle, NULL); /* wait until the suspend flag is released */
      }
	
      if(err < 0){
	err = snd_pcm_prepare(devin->out.alsa.handle);
      }
    }else if(devin->out.alsa.rc < 0){
      str = snd_strerror(devin->out.alsa.rc);
      
      g_message("error from writei: %s", str);
    }else if(devin->out.alsa.rc != (int) devin->buffer_size) {
      g_message("short write, write %d frames", devin->out.alsa.rc);
    }
  }      
  
#endif

  /* increment nth ring-buffer */
  g_atomic_int_set(&(devin->available),
		   FALSE);
  
  if(devin->nth_ring_buffer + 1 >= devin->ring_buffer_size){
    devin->nth_ring_buffer = 0;
  }else{
    devin->nth_ring_buffer += 1;
  }
  
  pthread_mutex_unlock(mutex);

  /* update soundcard */
  task_thread = ags_thread_find_type((AgsThread *) application_context->main_loop,
				     AGS_TYPE_TASK_THREAD);
  task = NULL;
  
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) devin);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) devin);
  task = g_list_append(task,
		       clear_buffer);

  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) devin);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_thread_append_tasks((AgsTaskThread *) task_thread,
			       task);
  
#ifdef AGS_WITH_ALSA
  snd_pcm_prepare(devin->out.alsa.handle);
#endif
}

void
ags_devin_alsa_free(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  AgsNotifySoundcard *notify_soundcard;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  GList *poll_fd;

  guint i;
  
  pthread_mutex_t *mutex;
  
  devin = AGS_DEVIN(soundcard);
  
  application_context = ags_soundcard_get_application_context(soundcard);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) devin);
  
  pthread_mutex_unlock(application_context->mutex);

  /* lock */
  pthread_mutex_lock(mutex);

#ifdef AGS_WITH_ALSA
  /* remove poll fd */
  poll_fd = devin->poll_fd;
  
  while(poll_fd != NULL){
    ags_polling_thread_remove_poll_fd(AGS_POLL_FD(poll_fd->data)->polling_thread,
				      poll_fd->data);
    g_object_unref(poll_fd->data);
    
    poll_fd = poll_fd->next;
  }

  g_list_free(poll_fd);

  devin->poll_fd = NULL;
#endif

  notify_soundcard = AGS_NOTIFY_SOUNDCARD(devin->notify_soundcard);

  if((AGS_DEVIN_INITIALIZED & (devin->flags)) == 0){
    pthread_mutex_unlock(mutex);
    
    return;
  }
  
#ifdef AGS_WITH_ALSA
  snd_pcm_drain(devin->out.alsa.handle);
  snd_pcm_close(devin->out.alsa.handle);
  devin->out.alsa.handle = NULL;
#endif

  /* free ring-buffer */
  for(i = 0; i < devin->ring_buffer_size; i++){
    free(devin->ring_buffer[i]);
  }

  free(devin->ring_buffer);

  devin->ring_buffer = NULL;

  /* reset flags */
  devin->flags &= (~(AGS_DEVIN_BUFFER0 |
		     AGS_DEVIN_BUFFER1 |
		     AGS_DEVIN_BUFFER2 |
		     AGS_DEVIN_BUFFER3 |
		     AGS_DEVIN_RECORD |
		     AGS_DEVIN_INITIALIZED));

  /* notify cyclic task */
  pthread_mutex_lock(notify_soundcard->return_mutex);

  g_atomic_int_or(&(notify_soundcard->flags),
		  AGS_NOTIFY_SOUNDCARD_DONE_RETURN);
  
  if((AGS_NOTIFY_SOUNDCARD_WAIT_RETURN & (g_atomic_int_get(&(notify_soundcard->flags)))) != 0){
    pthread_cond_signal(notify_soundcard->return_cond);
  }
  
  pthread_mutex_unlock(notify_soundcard->return_mutex);

  devin->note_offset = 0;
  devin->note_offset_absolute = 0;

  pthread_mutex_unlock(mutex);
}

void
ags_devin_tic(AgsSoundcard *soundcard)
{
  AgsDevin *devin;
  gdouble delay;
  
  devin = AGS_DEVIN(soundcard);
  
  /* determine if attack should be switched */
  delay = devin->delay[devin->tic_counter];

  if((guint) devin->delay_counter + 1 >= (guint) delay){
    if(devin->do_loop &&
       devin->note_offset + 1 == devin->loop_right){
      ags_soundcard_set_note_offset(soundcard,
				    devin->loop_left);
    }else{
      ags_soundcard_set_note_offset(soundcard,
				    devin->note_offset + 1);
    }
    
    ags_soundcard_set_note_offset_absolute(soundcard,
					   devin->note_offset_absolute + 1);

    /* delay */
    ags_soundcard_offset_changed(soundcard,
				 devin->note_offset);
    
    /* reset - delay counter */
    devin->delay_counter = 0.0;
    devin->tact_counter += 1.0;
  }else{
    devin->delay_counter += 1.0;
  }
}

void
ags_devin_offset_changed(AgsSoundcard *soundcard,
			 guint note_offset)
{
  AgsDevin *devin;
  
  devin = AGS_DEVIN(soundcard);

  devin->tic_counter += 1;

  if(devin->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    devin->tic_counter = 0;
  }
}

void
ags_devin_set_bpm(AgsSoundcard *soundcard,
		  gdouble bpm)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);

  devin->bpm = bpm;

  ags_devin_adjust_delay_and_attack(devin);
}

gdouble
ags_devin_get_bpm(AgsSoundcard *soundcard)
{
  AgsDevin *devin;
  
  devin = AGS_DEVIN(soundcard);

  return(devin->bpm);
}

void
ags_devin_set_delay_factor(AgsSoundcard *soundcard,
			   gdouble delay_factor)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);

  devin->delay_factor = delay_factor;

  ags_devin_adjust_delay_and_attack(devin);
}

gdouble
ags_devin_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsDevin *devin;
  
  devin = AGS_DEVIN(soundcard);

  return(devin->delay_factor);
}

gdouble
ags_devin_get_delay(AgsSoundcard *soundcard)
{
  AgsDevin *devin;
  guint index;
  
  devin = AGS_DEVIN(soundcard);
  index = devin->tic_counter;
  
  return(devin->delay[index]);
}

gdouble
ags_devin_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);
  
  return((60.0 * (((gdouble) devin->samplerate / (gdouble) devin->buffer_size) / (gdouble) devin->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) devin->delay_factor))));
}

guint
ags_devin_get_attack(AgsSoundcard *soundcard)
{
  AgsDevin *devin;
  guint index;
  
  devin = AGS_DEVIN(soundcard);
  index = devin->tic_counter;
  
  return(devin->attack[index]);
}

void*
ags_devin_get_buffer(AgsSoundcard *soundcard)
{
  AgsDevin *devin;
  void *buffer;
  
  devin = AGS_DEVIN(soundcard);

  if((AGS_DEVIN_BUFFER0 & (devin->flags)) != 0){
    buffer = devin->buffer[0];
  }else if((AGS_DEVIN_BUFFER1 & (devin->flags)) != 0){
    buffer = devin->buffer[1];
  }else if((AGS_DEVIN_BUFFER2 & (devin->flags)) != 0){
    buffer = devin->buffer[2];
  }else if((AGS_DEVIN_BUFFER3 & (devin->flags)) != 0){
    buffer = devin->buffer[3];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_devin_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsDevin *devin;
  void *buffer;
  
  devin = AGS_DEVIN(soundcard);

  //  g_message("next - 0x%0x", ((AGS_DEVIN_BUFFER0 |
  //				AGS_DEVIN_BUFFER1 |
  //				AGS_DEVIN_BUFFER2 |
  //				AGS_DEVIN_BUFFER3) & (devin->flags)));
  
  if((AGS_DEVIN_BUFFER0 & (devin->flags)) != 0){
    buffer = devin->buffer[1];
  }else if((AGS_DEVIN_BUFFER1 & (devin->flags)) != 0){
    buffer = devin->buffer[2];
  }else if((AGS_DEVIN_BUFFER2 & (devin->flags)) != 0){
    buffer = devin->buffer[3];
  }else if((AGS_DEVIN_BUFFER3 & (devin->flags)) != 0){
    buffer = devin->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_devin_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsDevin *devin;
  void *buffer;
  
  devin = AGS_DEVIN(soundcard);

  if((AGS_DEVIN_BUFFER0 & (devin->flags)) != 0){
    buffer = devin->buffer[3];
  }else if((AGS_DEVIN_BUFFER1 & (devin->flags)) != 0){
    buffer = devin->buffer[0];
  }else if((AGS_DEVIN_BUFFER2 & (devin->flags)) != 0){
    buffer = devin->buffer[1];
  }else if((AGS_DEVIN_BUFFER3 & (devin->flags)) != 0){
    buffer = devin->buffer[2];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

guint
ags_devin_get_delay_counter(AgsSoundcard *soundcard)
{
  return(AGS_DEVIN(soundcard)->delay_counter);
}

void
ags_devin_set_note_offset(AgsSoundcard *soundcard,
			  guint note_offset)
{
  AGS_DEVIN(soundcard)->note_offset = note_offset;
}

guint
ags_devin_get_note_offset(AgsSoundcard *soundcard)
{
  return(AGS_DEVIN(soundcard)->note_offset);
}

void
ags_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
				   guint note_offset)
{
  AGS_DEVIN(soundcard)->note_offset_absolute = note_offset;
}

guint
ags_devin_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  return(AGS_DEVIN(soundcard)->note_offset_absolute);
}

void
ags_devin_set_loop(AgsSoundcard *soundcard,
		   guint loop_left, guint loop_right,
		   gboolean do_loop)
{
  AGS_DEVIN(soundcard)->loop_left = loop_left;
  AGS_DEVIN(soundcard)->loop_right = loop_right;
  AGS_DEVIN(soundcard)->do_loop = do_loop;

  if(do_loop){
    AGS_DEVIN(soundcard)->loop_offset = AGS_DEVIN(soundcard)->note_offset;
  }
}

void
ags_devin_get_loop(AgsSoundcard *soundcard,
		   guint *loop_left, guint *loop_right,
		   gboolean *do_loop)
{
  if(loop_left != NULL){
    *loop_left = AGS_DEVIN(soundcard)->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = AGS_DEVIN(soundcard)->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = AGS_DEVIN(soundcard)->do_loop;
  }
}

guint
ags_devin_get_loop_offset(AgsSoundcard *soundcard)
{
  return(AGS_DEVIN(soundcard)->loop_offset);
}

void
ags_devin_set_audio(AgsSoundcard *soundcard,
		    GList *audio)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);
  devin->audio = audio;
}

GList*
ags_devin_get_audio(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);

  return(devin->audio);
}

/**
 * ags_devin_adjust_delay_and_attack:
 * @devin: the #AgsDevin
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 1.2.0
 */
void
ags_devin_adjust_delay_and_attack(AgsDevin *devin)
{
  gdouble delay;
  guint default_tact_frames;
  guint delay_tact_frames;
  guint default_period;
  gint next_attack;
  guint i;

  if(devin == NULL){
    return;
  }
  
  delay = ags_devin_get_absolute_delay(AGS_SOUNDCARD(devin));

#ifdef AGS_DEBUG
  g_message("delay : %f", delay);
#endif
  
  default_tact_frames = (guint) (delay * devin->buffer_size);
  delay_tact_frames = (guint) (floor(delay) * devin->buffer_size);
  default_period = (1.0 / AGS_SOUNDCARD_DEFAULT_PERIOD) * (default_tact_frames);

  i = 0;
  
  devin->attack[0] = (guint) floor(0.25 * devin->buffer_size);
  next_attack = (((devin->attack[i] + default_tact_frames) / devin->buffer_size) - delay) * devin->buffer_size;

  if(next_attack >= devin->buffer_size){
    next_attack = devin->buffer_size - 1;
  }
  
  /* check if delay drops for next attack */
  if(next_attack < 0){
    devin->attack[i] = devin->attack[i] - ((gdouble) next_attack / 2.0);

    if(devin->attack[i] < 0){
      devin->attack[i] = 0;
    }
    
    if(devin->attack[i] >= devin->buffer_size){
      devin->attack[i] = devin->buffer_size - 1;
    }
    
    next_attack = next_attack + (next_attack / 2.0);

    if(next_attack < 0){
      next_attack = 0;
    }

    if(next_attack >= devin->buffer_size){
      next_attack = devin->buffer_size - 1;
    }
  }
  
  for(i = 1; i < (int)  2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    devin->attack[i] = next_attack;
    next_attack = (((devin->attack[i] + default_tact_frames) / devin->buffer_size) - delay) * devin->buffer_size;

    if(next_attack >= devin->buffer_size){
      next_attack = devin->buffer_size - 1;
    }
    
    /* check if delay drops for next attack */
    if(next_attack < 0){
      devin->attack[i] = devin->attack[i] - ((gdouble) next_attack / 2.0);

      if(devin->attack[i] < 0){
	devin->attack[i] = 0;
      }

      if(devin->attack[i] >= devin->buffer_size){
	devin->attack[i] = devin->buffer_size - 1;
      }
    
      next_attack = next_attack + (next_attack / 2.0);
      
      if(next_attack < 0){
	next_attack = 0;
      }

      if(next_attack >= devin->buffer_size){
	next_attack = devin->buffer_size - 1;
      }
    }
    
#ifdef AGS_DEBUG
    g_message("%d", devin->attack[i]);
#endif
  }

  devin->attack[0] = devin->attack[i - 2];
  
  for(i = 0; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD - 1; i++){
    devin->delay[i] = ((gdouble) (default_tact_frames + devin->attack[i] - devin->attack[i + 1])) / (gdouble) devin->buffer_size;
    
#ifdef AGS_DEBUG
    g_message("%f", devin->delay[i]);
#endif
  }

  devin->delay[i] = ((gdouble) (default_tact_frames + devin->attack[i] - devin->attack[0])) / (gdouble) devin->buffer_size;
}

/**
 * ags_devin_realloc_buffer:
 * @devin: the #AgsDevin
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 1.2.0
 */
void
ags_devin_realloc_buffer(AgsDevin *devin)
{
  guint word_size;

  if(devin == NULL){
    return;
  }

  switch(devin->format){
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
    g_warning("ags_devin_realloc_buffer(): unsupported word size");
    return;
  }
  
  /* AGS_DEVIN_BUFFER_0 */
  if(devin->buffer[0] != NULL){
    free(devin->buffer[0]);
  }
  
  devin->buffer[0] = (void *) malloc(devin->pcm_channels * devin->buffer_size * word_size);
  
  /* AGS_DEVIN_BUFFER_1 */
  if(devin->buffer[1] != NULL){
    free(devin->buffer[1]);
  }

  devin->buffer[1] = (void *) malloc(devin->pcm_channels * devin->buffer_size * word_size);
  
  /* AGS_DEVIN_BUFFER_2 */
  if(devin->buffer[2] != NULL){
    free(devin->buffer[2]);
  }

  devin->buffer[2] = (void *) malloc(devin->pcm_channels * devin->buffer_size * word_size);
  
  /* AGS_DEVIN_BUFFER_3 */
  if(devin->buffer[3] != NULL){
    free(devin->buffer[3]);
  }
  
  devin->buffer[3] = (void *) malloc(devin->pcm_channels * devin->buffer_size * word_size);
}

/**
 * ags_devin_new:
 * @application_context: the #AgsApplicationContext
 *
 * Creates an #AgsDevin, refering to @application_context.
 *
 * Returns: a new #AgsDevin
 *
 * Since: 1.2.0
 */
AgsDevin*
ags_devin_new(GObject *application_context)
{
  AgsDevin *devin;

  devin = (AgsDevin *) g_object_new(AGS_TYPE_DEVIN,
				    "application-context", application_context,
				    NULL);
  
  return(devin);
}
