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

#include <ags/audio/ags_devout.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_soundcard.h>

#include <ags/thread/ags_mutex_manager.h>

#include <ags/audio/ags_notation.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <errno.h>

#include <string.h>
#include <math.h>
#include <time.h>

/**
 * SECTION:ags_devout
 * @short_description: Output to soundcard
 * @title: AgsDevout
 * @section_id:
 * @include: ags/object/ags_soundcard.h
 *
 * #AgsDevout represents a soundcard and supports output.
 */

void ags_devout_class_init(AgsDevoutClass *devout);
void ags_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_devout_init(AgsDevout *devout);
void ags_devout_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_devout_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_devout_disconnect(AgsConnectable *connectable);
void ags_devout_connect(AgsConnectable *connectable);
void ags_devout_finalize(GObject *gobject);

void ags_devout_switch_buffer_flag(AgsDevout *devout);

void ags_devout_set_application_context(AgsSoundcard *soundcard,
					AgsApplicationContext *application_context);
AgsApplicationContext* ags_devout_get_application_context(AgsSoundcard *soundcard);

void ags_devout_set_application_mutex(AgsSoundcard *soundcard,
				      pthread_mutex_t *application_mutex);
pthread_mutex_t* ags_devout_get_application_mutex(AgsSoundcard *soundcard);

void ags_devout_set_device(AgsSoundcard *soundcard,
			   gchar *device);
gchar* ags_devout_get_device(AgsSoundcard *soundcard);

void ags_devout_set_presets(AgsSoundcard *soundcard,
			    guint channels,
			    guint rate,
			    guint buffer_size,
			    guint format);
void ags_devout_get_presets(AgsSoundcard *soundcard,
			    guint *channels,
			    guint *rate,
			    guint *buffer_size,
			    guint *format);

void ags_devout_list_cards(AgsSoundcard *soundcard,
			   GList **card_id, GList **card_name);
void ags_devout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			 guint *channels_min, guint *channels_max,
			 guint *rate_min, guint *rate_max,
			 guint *buffer_size_min, guint *buffer_size_max,
			 GError **error);

gboolean ags_devout_is_starting(AgsSoundcard *soundcard);
gboolean ags_devout_is_playing(AgsSoundcard *soundcard);

void ags_devout_alsa_init(AgsSoundcard *soundcard,
			  GError **error);
void ags_devout_alsa_play(AgsSoundcard *soundcard,
			  GError **error);
void ags_devout_alsa_free(AgsSoundcard *soundcard);

void ags_devout_tic(AgsSoundcard *soundcard);
void ags_devout_offset_changed(AgsSoundcard *soundcard,
			       guint note_offset);

void ags_devout_set_bpm(AgsSoundcard *soundcard,
			gdouble bpm);
gdouble ags_devout_get_bpm(AgsSoundcard *soundcard);

void ags_devout_set_delay_factor(AgsSoundcard *soundcard,
				 gdouble delay_factor);
gdouble ags_devout_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_devout_get_delay(AgsSoundcard *soundcard);
guint ags_devout_get_attack(AgsSoundcard *soundcard);

void* ags_devout_get_buffer(AgsSoundcard *soundcard);
void* ags_devout_get_next_buffer(AgsSoundcard *soundcard);

void ags_devout_set_note_offset(AgsSoundcard *soundcard,
				guint note_offset);
guint ags_devout_get_note_offset(AgsSoundcard *soundcard);

void ags_devout_set_audio(AgsSoundcard *soundcard,
			  GList *audio);
GList* ags_devout_get_audio(AgsSoundcard *soundcard);

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

static gpointer ags_devout_parent_class = NULL;
static guint devout_signals[LAST_SIGNAL];

GType
ags_devout_get_type (void)
{
  static GType ags_type_devout = 0;

  if(!ags_type_devout){
    static const GTypeInfo ags_devout_info = {
      sizeof (AgsDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_devout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_devout = g_type_register_static(G_TYPE_OBJECT,
					     "AgsDevout\0",
					     &ags_devout_info,
					     0);

    g_type_add_interface_static(ags_type_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_devout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);
  }

  return (ags_type_devout);
}

void
ags_devout_class_init(AgsDevoutClass *devout)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_devout_parent_class = g_type_class_peek_parent(devout);

  /* GObjectClass */
  gobject = (GObjectClass *) devout;

  gobject->set_property = ags_devout_set_property;
  gobject->get_property = ags_devout_get_property;

  gobject->finalize = ags_devout_finalize;

  /* properties */
  /**
   * AgsDevout:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_object("application-context\0",
				   "the application context object\0",
				   "The application context object\0",
				   AGS_TYPE_APPLICATION_CONTEXT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /**
   * AgsDevout:application-mutex:
   *
   * The assigned application mutex
   * 
   * Since: 0.7.0
   */
  param_spec = g_param_spec_pointer("application-mutex\0",
				    "the application mutex object\0",
				    "The application mutex object\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_MUTEX,
				  param_spec);

  /**
   * AgsDevout:device:
   *
   * The alsa soundcard indentifier
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_string("device\0",
				   "the device identifier\0",
				   "The device to perform output to\0",
				   "hw:0\0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsDevout:dsp-channels:
   *
   * The dsp channel count
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_uint("dsp-channels\0",
				 "count of DSP channels\0",
				 "The count of DSP channels to use\0",
				 1,
				 64,
				 2,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DSP_CHANNELS,
				  param_spec);

  /**
   * AgsDevout:pcm-channels:
   *
   * The pcm channel count
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_uint("pcm-channels\0",
				 "count of PCM channels\0",
				 "The count of PCM channels to use\0",
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
   * AgsDevout:format:
   *
   * The precision of the buffer
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_uint("format\0",
				 "precision of buffer\0",
				 "The precision to use for a frame\0",
				 1,
				 64,
				 AGS_SOUNDCARD_DEFAULT_FORMAT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsDevout:buffer-size:
   *
   * The buffer size
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_uint("buffer-size\0",
				 "frame count of a buffer\0",
				 "The count of frames a buffer contains\0",
				 1,
				 44100,
				 940,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsDevout:samplerate:
   *
   * The samplerate
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_uint("samplerate\0",
				 "frames per second\0",
				 "The frames count played during a second\0",
				 8000,
				 96000,
				 44100,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsDevout:buffer:
   *
   * The buffer
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_pointer("buffer\0",
				    "the buffer\0",
				    "The buffer to play\0",
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsDevout:bpm:
   *
   * Beats per minute
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_double("bpm\0",
				   "beats per minute\0",
				   "Beats per minute to use\0",
				   1.0,
				   240.0,
				   120.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  /**
   * AgsDevout:delay-factor:
   *
   * tact
   * 
   * Since: 0.4.2
   */
  param_spec = g_param_spec_double("delay-factor\0",
				   "delay factor\0",
				   "The delay factor\0",
				   0.0,
				   16.0,
				   1.0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DELAY_FACTOR,
				  param_spec);

  /**
   * AgsDevout:attack:
   *
   * Attack of the buffer
   * 
   * Since: 0.4.0
   */
  param_spec = g_param_spec_pointer("attack\0",
				    "attack of buffer\0",
				    "The attack to use for the buffer\0",
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);


  /* AgsDevoutClass */
}

GQuark
ags_devout_error_quark()
{
  return(g_quark_from_static_string("ags-devout-error-quark\0"));
}

void
ags_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_devout_connect;
  connectable->disconnect = ags_devout_disconnect;
}

void
ags_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_application_context = ags_devout_set_application_context;
  soundcard->get_application_context = ags_devout_get_application_context;

  soundcard->set_application_mutex = ags_devout_set_application_mutex;
  soundcard->get_application_mutex = ags_devout_get_application_mutex;

  soundcard->set_device = ags_devout_set_device;
  soundcard->get_device = ags_devout_get_device;
  
  soundcard->set_presets = ags_devout_set_presets;
  soundcard->get_presets = ags_devout_get_presets;

  soundcard->list_cards = ags_devout_list_cards;
  soundcard->pcm_info = ags_devout_pcm_info;

  soundcard->is_starting =  ags_devout_is_starting;
  soundcard->is_playing = ags_devout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->play_init = ags_devout_alsa_init;
  soundcard->play = ags_devout_alsa_play;

  soundcard->record_init = NULL;
  soundcard->record = NULL;

  soundcard->stop = ags_devout_alsa_free;

  soundcard->tic = ags_devout_tic;
  soundcard->offset_changed = ags_devout_offset_changed;
    
  soundcard->set_bpm = ags_devout_set_bpm;
  soundcard->get_bpm = ags_devout_get_bpm;

  soundcard->set_delay_factor = ags_devout_set_delay_factor;
  soundcard->get_delay_factor = ags_devout_get_delay_factor;
  
  soundcard->get_delay = ags_devout_get_delay;
  soundcard->get_attack = ags_devout_get_attack;

  soundcard->get_buffer = ags_devout_get_buffer;
  soundcard->get_next_buffer = ags_devout_get_next_buffer;

  soundcard->set_note_offset = ags_devout_set_note_offset;
  soundcard->get_note_offset = ags_devout_get_note_offset;

  soundcard->set_audio = ags_devout_set_audio;
  soundcard->get_audio = ags_devout_get_audio;
}

void
ags_devout_init(AgsDevout *devout)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t attr;

  /* insert devout mutex */
  //FIXME:JK: memory leak
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr,
			    PTHREAD_MUTEX_RECURSIVE);

  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     &attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) devout,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  devout->flags = (AGS_DEVOUT_ALSA);

  /* quality */
  devout->dsp_channels = AGS_SOUNDCARD_DEFAULT_DSP_CHANNELS;
  devout->pcm_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
  devout->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
  devout->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  devout->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  //  devout->out.oss.device = NULL;
  devout->out.alsa.handle = NULL;
  devout->out.alsa.device = AGS_SOUNDCARD_DEFAULT_DEVICE;

  /* buffer */
  devout->buffer = (void **) malloc(4 * sizeof(void*));

  devout->buffer[0] = NULL;
  devout->buffer[1] = NULL;
  devout->buffer[2] = NULL;
  devout->buffer[3] = NULL;
  
  ags_devout_realloc_buffer(devout);
  
  /* bpm */
  devout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  devout->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* delay and attack */
  devout->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
				     sizeof(gdouble));
  
  devout->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
				    sizeof(guint));

  ags_devout_adjust_delay_and_attack(devout);
  
  /* counters */
  devout->note_offset = 0;
  devout->tact_counter = 0.0;
  devout->delay_counter = 0;
  devout->tic_counter = 0;

  /* parent */
  devout->application_context = NULL;
  devout->application_mutex = NULL;

  /* all AgsAudio */
  devout->audio = NULL;
}

void
ags_devout_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(gobject);

  //TODO:JK: implement set functionality
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = g_value_get_object(value);

      if(devout->application_context == application_context){
	return;
      }

      if(devout->application_context != NULL){
	g_object_unref(G_OBJECT(devout->application_context));
      }

      if(application_context != NULL){
	AgsConfig *config;

	gchar *str;
	gchar *segmentation;
	guint discriminante, nominante;
	
	g_object_ref(G_OBJECT(application_context));

	devout->application_mutex = application_context->mutex;
	
	config = ags_config_get_instance();

	str = ags_config_get_value(config,
				   AGS_CONFIG_SOUNDCARD,
				   "dsp-channels\0");

	if(str != NULL){
	  devout->dsp_channels = g_ascii_strtoull(str,
						  NULL,
						  10);
	  g_free(str);
	}

	str = ags_config_get_value(config,
				   AGS_CONFIG_SOUNDCARD,
				   "pcm-channels\0");

	if(str != NULL){
	  devout->pcm_channels = g_ascii_strtoull(str,
						  NULL,
						  10);
	  
	  g_free(str);
	}
	
	devout->format = AGS_SOUNDCARD_DEFAULT_FORMAT;
	str = ags_config_get_value(config,
				   AGS_CONFIG_SOUNDCARD,
				   "buffer-size\0");

	if(str != NULL){
	  devout->buffer_size = g_ascii_strtoull(str,
						 NULL,
						 10);
	  
	  g_free(str);
	}

	str = ags_config_get_value(config,
				   AGS_CONFIG_SOUNDCARD,
				   "samplerate\0");

	if(str != NULL){
	  devout->samplerate = g_ascii_strtoull(str,
						NULL,
						10);
	  
	  g_free(str);
	}
	
	segmentation = ags_config_get_value(config,
					    AGS_CONFIG_GENERIC,
					    "segmentation\0");

	if(segmentation != NULL){
	  sscanf(segmentation, "%d/%d\0",
		 &discriminante,
		 &nominante);
    
	  devout->delay_factor = 1.0 / nominante * (nominante / discriminante);
	}
	
	//  devout->out.oss.device = NULL;
	devout->out.alsa.handle = NULL;
	devout->out.alsa.device = g_strdup(ags_config_get_value(config,
								AGS_CONFIG_SOUNDCARD,
								"alsa-handle\0"));
	g_message("device %s\n", devout->out.alsa.device);

	ags_devout_adjust_delay_and_attack(devout);
	ags_devout_realloc_buffer(devout);
      }else{
	devout->application_mutex = NULL;
      }

      devout->application_context = application_context;
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      pthread_mutex_t *application_mutex;

      application_mutex = (pthread_mutex_t *) g_value_get_pointer(value);

      if(devout->application_mutex == application_mutex){
	return;
      }
      
      devout->application_mutex = application_mutex;
    }
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      if((AGS_DEVOUT_OSS & (devout->flags)) != 0){
	devout->out.oss.device = g_strdup(device);
      }else if((AGS_DEVOUT_ALSA & (devout->flags)) != 0){
	devout->out.alsa.device = g_strdup(device);
      }
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      if(dsp_channels == devout->dsp_channels){
	return;
      }

      devout->dsp_channels = dsp_channels;
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      if(pcm_channels == devout->pcm_channels){
	return;
      }

      devout->pcm_channels = pcm_channels;

      ags_devout_realloc_buffer(devout);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      if(format == devout->format){
	return;
      }

      devout->format = format;

      ags_devout_realloc_buffer(devout);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      if(buffer_size == devout->buffer_size){
	return;
      }

      devout->buffer_size = buffer_size;

      ags_devout_realloc_buffer(devout);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      if(samplerate == devout->samplerate){
	return;
      }

      devout->samplerate = samplerate;

      ags_devout_realloc_buffer(devout);
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

      devout->bpm = bpm;

      ags_devout_adjust_delay_and_attack(devout);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      devout->delay_factor = delay_factor;

      ags_devout_adjust_delay_and_attack(devout);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_devout_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, devout->application_context);
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      g_value_set_pointer(value, devout->application_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      if((AGS_DEVOUT_OSS & (devout->flags)) != 0){
	g_value_set_string(value, devout->out.oss.device);
      }else if((AGS_DEVOUT_ALSA & (devout->flags)) != 0){
	g_value_set_string(value, devout->out.alsa.device);
      }
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_value_set_uint(value, devout->dsp_channels);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_value_set_uint(value, devout->pcm_channels);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, devout->format);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, devout->buffer_size);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, devout->samplerate);
    }
    break;
  case PROP_BUFFER:
    {
      g_value_set_pointer(value, devout->buffer);
    }
    break;
  case PROP_BPM:
    {
      g_value_set_double(value, devout->bpm);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_value_set_double(value, devout->delay_factor);
    }
    break;
  case PROP_ATTACK:
    {
      g_value_set_pointer(value, devout->attack);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_devout_finalize(GObject *gobject)
{
  AgsDevout *devout;

  AgsMutexManager *mutex_manager;
  
  GList *list, *list_next;

  devout = AGS_DEVOUT(gobject);

  /* remove devout mutex */
  pthread_mutex_lock(devout->application_mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(devout->application_mutex);

  /* free output buffer */
  free(devout->buffer[0]);
  free(devout->buffer[1]);
  free(devout->buffer[2]);
  free(devout->buffer[3]);

  /* free buffer array */
  free(devout->buffer);

  /* free AgsAttack */
  free(devout->attack);

  if(devout->audio != NULL){
    g_list_free_full(devout->audio,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_devout_parent_class)->finalize(gobject);
}

void
ags_devout_connect(AgsConnectable *connectable)
{
  AgsDevout *devout;
  
  AgsMutexManager *mutex_manager;

  GList *list;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t attr;

  devout = AGS_DEVOUT(connectable);

  /* create devout mutex */
  //FIXME:JK: memory leak
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr,
			    PTHREAD_MUTEX_RECURSIVE);
  
  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     &attr);

  /* insert mutex */
  pthread_mutex_lock(devout->application_mutex);

  mutex_manager = ags_mutex_manager_get_instance();

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) devout,
			   mutex);
  
  pthread_mutex_unlock(devout->application_mutex);

  /*  */  
  list = devout->audio;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_devout_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_devout_switch_buffer_flag:
 * @devout: an #AgsDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 0.3
 */
void
ags_devout_switch_buffer_flag(AgsDevout *devout)
{
  if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
    devout->flags &= (~AGS_DEVOUT_BUFFER0);
    devout->flags |= AGS_DEVOUT_BUFFER1;
  }else if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
    devout->flags &= (~AGS_DEVOUT_BUFFER1);
    devout->flags |= AGS_DEVOUT_BUFFER2;
  }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
    devout->flags &= (~AGS_DEVOUT_BUFFER2);
    devout->flags |= AGS_DEVOUT_BUFFER3;
  }else if((AGS_DEVOUT_BUFFER3 & (devout->flags)) != 0){
    devout->flags &= (~AGS_DEVOUT_BUFFER3);
    devout->flags |= AGS_DEVOUT_BUFFER0;
  }
}

void
ags_devout_set_application_context(AgsSoundcard *soundcard,
				   AgsApplicationContext *application_context)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);
  devout->application_context = application_context;
}

AgsApplicationContext*
ags_devout_get_application_context(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);

  return(devout->application_context);
}

void
ags_devout_set_application_mutex(AgsSoundcard *soundcard,
				 pthread_mutex_t *application_mutex)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);
  devout->application_mutex = application_mutex;
}

pthread_mutex_t*
ags_devout_get_application_mutex(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);

  return(devout->application_mutex);
}

void
ags_devout_set_device(AgsSoundcard *soundcard,
		      gchar *device)
{
  AgsDevout *devout;
  
  devout = AGS_DEVOUT(soundcard);
  devout->out.alsa.device = device;
}

gchar*
ags_devout_get_device(AgsSoundcard *soundcard)
{
  AgsDevout *devout;
  
  devout = AGS_DEVOUT(soundcard);
  
  return(devout->out.alsa.device);
}

void
ags_devout_set_presets(AgsSoundcard *soundcard,
		       guint channels,
		       guint rate,
		       guint buffer_size,
		       guint format)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);
  g_object_set(devout,
	       "dsp-channels\0", channels,
	       "samplerate\0", rate,
	       "buffer-size\0", buffer_size,
	       "format\0", format,
	       NULL);
}

void
ags_devout_get_presets(AgsSoundcard *soundcard,
		       guint *channels,
		       guint *rate,
		       guint *buffer_size,
		       guint *format)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);

  if(channels != NULL){
    *channels = devout->dsp_channels;
  }

  if(rate != NULL){
    *rate = devout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = devout->buffer_size;
  }

  if(format != NULL){
    *format = devout->format;
  }
}

/**
 * ags_devout_list_cards:
 * @soundcard: the #AgsSoundcard
 * @card_id: alsa identifier
 * @card_name: card name
 *
 * List available soundcards.
 *
 * Since: 0.4
 */
void
ags_devout_list_cards(AgsSoundcard *soundcard,
		      GList **card_id, GList **card_name)
{
  snd_ctl_t *card_handle;
  snd_ctl_card_info_t *card_info;
  char *name;
  gchar *str;
  int card_num;
  int device;
  int error;

  *card_id = NULL;
  *card_name = NULL;
  card_num = -1;

  while(TRUE){
    error = snd_card_next(&card_num);

    if(card_num < 0){
      break;
    }

    if(error < 0){
      continue;
    }

    str = g_strdup_printf("hw:%i\0", card_num);
    error = snd_ctl_open(&card_handle, str, 0);

    if(error < 0){
      continue;
    }

    snd_ctl_card_info_alloca(&card_info);
    error = snd_ctl_card_info(card_handle, card_info);

    if(error < 0){
      continue;
    }

    device = -1;
    error = snd_ctl_pcm_next_device(card_handle, &device);

    if(error < 0){
      continue;
    }
    
    *card_id = g_list_prepend(*card_id, str);
    *card_name = g_list_prepend(*card_name, g_strdup(snd_ctl_card_info_get_name(card_info)));

    snd_ctl_close(card_handle);
  }

  snd_config_update_free_global();

  *card_id = g_list_reverse(*card_id);
  *card_name = g_list_reverse(*card_name);
}

void
ags_devout_pcm_info(AgsSoundcard *soundcard,
		    char *card_id,
		    guint *channels_min, guint *channels_max,
		    guint *rate_min, guint *rate_max,
		    guint *buffer_size_min, guint *buffer_size_max,
		    GError **error)
{
  int rc;
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;
  int err;

  /* Open PCM device for playback. */
  handle = NULL;

  rc = snd_pcm_open(&handle, card_id, SND_PCM_STREAM_PLAYBACK, 0);

  if(rc < 0) {
    g_message("unable to open pcm device: %s\n\0", snd_strerror(rc));

    g_set_error(error,
		AGS_DEVOUT_ERROR,
		AGS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		"unable to open pcm device: %s\n\0",
		snd_strerror(rc));

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
}

gboolean
ags_devout_is_starting(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);
  
  return(((AGS_DEVOUT_START_PLAY & (devout->flags)) != 0) ? TRUE: FALSE);
}

gboolean
ags_devout_is_playing(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);
  
  return(((AGS_DEVOUT_PLAY & (devout->flags)) != 0) ? TRUE: FALSE);
}

void
ags_devout_alsa_init(AgsSoundcard *soundcard,
		     GError **error)
{
  AgsDevout *devout;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  snd_pcm_t *handle;
  snd_pcm_hw_params_t *hwparams;
  snd_pcm_sw_params_t *swparams;

  int rc;
  unsigned int val;
  snd_pcm_uframes_t frames;
  unsigned int rate;
  unsigned int rrate;
  unsigned int channels;
  snd_pcm_uframes_t size;
  snd_pcm_sframes_t buffer_size;
  snd_pcm_sframes_t period_size;
  guint word_size;
  snd_pcm_format_t format;

  int period_event;

  int err, dir;
  
  pthread_mutex_t *mutex;
  
  static unsigned int period_time = 100000;

  devout = AGS_DEVOUT(soundcard);

  application_context = ags_soundcard_get_application_context(soundcard);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) devout);
  
  pthread_mutex_unlock(application_context->mutex);

  /* retrieve word size */
  pthread_mutex_lock(mutex);

  switch(devout->format){
  case AGS_SOUNDCARD_RESOLUTION_8_BIT:
    {
      format = SND_PCM_FORMAT_S8;

      word_size = sizeof(signed char);
    }
    break;
  case AGS_SOUNDCARD_RESOLUTION_16_BIT:
    {
      format = SND_PCM_FORMAT_S16;
      
      word_size = sizeof(signed short);
    }
    break;
  case AGS_SOUNDCARD_RESOLUTION_24_BIT:
    {
      format = SND_PCM_FORMAT_S24;
      
      //NOTE:JK: The 24-bit linear samples use 32-bit physical space
      word_size = sizeof(signed long);
    }
    break;
  case AGS_SOUNDCARD_RESOLUTION_32_BIT:
    {
      word_size = sizeof(signed long);
    }
    break;
  case AGS_SOUNDCARD_RESOLUTION_64_BIT:
    {
      word_size = sizeof(signed long long);
    }
    break;
  default:
    g_warning("ags_devout_alsa_init(): unsupported word size\0");
    return;
  }
  
  /* prepare for playback */
  devout->flags |= (AGS_DEVOUT_BUFFER3 |
		    AGS_DEVOUT_START_PLAY |
		    AGS_DEVOUT_PLAY |
		    AGS_DEVOUT_NONBLOCKING);

  devout->note_offset = 0;

  memset(devout->buffer[0], 0, devout->dsp_channels * devout->buffer_size * word_size);
  memset(devout->buffer[1], 0, devout->dsp_channels * devout->buffer_size * word_size);
  memset(devout->buffer[2], 0, devout->dsp_channels * devout->buffer_size * word_size);
  memset(devout->buffer[3], 0, devout->dsp_channels * devout->buffer_size * word_size);

  /*  */
  period_event = 0;
  
  /* Open PCM device for playback. */
  if ((err = snd_pcm_open(&handle, devout->out.alsa.device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
    pthread_mutex_unlock(mutex);
    printf("Playback open error: %s\n", snd_strerror(err));
    g_set_error(error,
		AGS_DEVOUT_ERROR,
		AGS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		"unable to open pcm device: %s\n\0",
		snd_strerror(err));
    return;
  }

  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_sw_params_alloca(&swparams);

  /* choose all parameters */
  err = snd_pcm_hw_params_any(handle, hwparams);
  if (err < 0) {
    pthread_mutex_unlock(mutex);
    printf("Broken configuration for playback: no configurations available: %s\n", snd_strerror(err));
    return;
  }

  /* set hardware resampling */
  err = snd_pcm_hw_params_set_rate_resample(handle, hwparams, 1);
  if (err < 0) {
    pthread_mutex_unlock(mutex);
    printf("Resampling setup failed for playback: %s\n", snd_strerror(err));
    return;
  }

  /* set the interleaved read/write format */
  err = snd_pcm_hw_params_set_access(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
  if (err < 0) {
    pthread_mutex_unlock(mutex);
    printf("Access type not available for playback: %s\n", snd_strerror(err));
    return;
  }

  /* set the sample format */
  err = snd_pcm_hw_params_set_format(handle, hwparams, format);
  if (err < 0) {
    pthread_mutex_unlock(mutex);
    printf("Sample format not available for playback: %s\n", snd_strerror(err));
    return;
  }

  /* set the count of channels */
  channels = devout->dsp_channels;
  err = snd_pcm_hw_params_set_channels(handle, hwparams, channels);
  if (err < 0) {
    pthread_mutex_unlock(mutex);
    printf("Channels count (%i) not available for playbacks: %s\n", channels, snd_strerror(err));
    return;
  }

  /* set the stream rate */
  rate = devout->samplerate;
  rrate = rate;
  err = snd_pcm_hw_params_set_rate_near(handle, hwparams, &rrate, 0);
  if (err < 0) {
    pthread_mutex_unlock(mutex);
    printf("Rate %iHz not available for playback: %s\n", rate, snd_strerror(err));
    return;
  }

  if (rrate != rate) {
    pthread_mutex_unlock(mutex);
    printf("Rate doesn't match (requested %iHz, get %iHz)\n", rate, err);
    //    exit(-EINVAL);
    return;
  }

  /* set the buffer size */
  size = devout->buffer_size;
  err = snd_pcm_hw_params_set_buffer_size(handle, hwparams, size);
  if (err < 0) {
    pthread_mutex_unlock(mutex);
    printf("Unable to set buffer size %i for playback: %s\n", size, snd_strerror(err));
    return;
  }

  buffer_size = size;

  /* set the period time */
  period_time = USEC_PER_SEC / devout->samplerate;
  dir = -1;
  err = snd_pcm_hw_params_set_period_time_near(handle, hwparams, &period_time, &dir);
  if (err < 0) {
    pthread_mutex_unlock(mutex);
    printf("Unable to set period time %i for playback: %s\n", period_time, snd_strerror(err));
    return;
  }

  err = snd_pcm_hw_params_get_period_size(hwparams, &size, &dir);
  if (err < 0) {
    pthread_mutex_unlock(mutex);
    printf("Unable to get period size for playback: %s\n", snd_strerror(err));
    return;
  }
  period_size = size;

  /* write the parameters to device */
  err = snd_pcm_hw_params(handle, hwparams);
  if (err < 0) {
    pthread_mutex_unlock(mutex);
    printf("Unable to set hw params for playback: %s\n", snd_strerror(err));
    return;
  }

  /* get the current swparams */
  err = snd_pcm_sw_params_current(handle, swparams);
  if (err < 0) {
    pthread_mutex_unlock(mutex);
    printf("Unable to determine current swparams for playback: %s\n", snd_strerror(err));
    return;
  }

  /* start the transfer when the buffer is almost full: */
  /* (buffer_size / avail_min) * avail_min */
  err = snd_pcm_sw_params_set_start_threshold(handle, swparams, (buffer_size / period_size) * period_size);
  if (err < 0) {
    pthread_mutex_unlock(mutex);
    printf("Unable to set start threshold mode for playback: %s\n", snd_strerror(err));
    return;
  }

  /* allow the transfer when at least period_size samples can be processed */
  /* or disable this mechanism when period event is enabled (aka interrupt like style processing) */
  err = snd_pcm_sw_params_set_avail_min(handle, swparams, period_event ? buffer_size : period_size);
  if (err < 0) {
    pthread_mutex_unlock(mutex);
    printf("Unable to set avail min for playback: %s\n", snd_strerror(err));
    return;
  }

  /* write the parameters to the playback device */
  err = snd_pcm_sw_params(handle, swparams);
  if (err < 0) {
    pthread_mutex_unlock(mutex);
    printf("Unable to set sw params for playback: %s\n", snd_strerror(err));
    return;
  }

  /*  */
  devout->out.alsa.handle = handle;
  devout->tact_counter = 0.0;
  devout->delay_counter = 0.0;
  devout->tic_counter = 0;

  devout->flags |= AGS_DEVOUT_INITIALIZED;

  pthread_mutex_unlock(mutex);
}

void
ags_devout_alsa_play(AgsSoundcard *soundcard,
		     GError **error)
{
  AgsDevout *devout;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  guint word_size;
  
  pthread_mutex_t *mutex;
  
  devout = AGS_DEVOUT(soundcard);

  /*  */
  application_context = ags_soundcard_get_application_context(soundcard);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) devout);
  
  pthread_mutex_unlock(application_context->mutex);

  /* retrieve word size */
  pthread_mutex_lock(mutex);

  switch(devout->format){
  case AGS_SOUNDCARD_RESOLUTION_8_BIT:
    {
      word_size = sizeof(signed char);
    }
    break;
  case AGS_SOUNDCARD_RESOLUTION_16_BIT:
    {
      word_size = sizeof(signed short);
    }
    break;
  case AGS_SOUNDCARD_RESOLUTION_24_BIT:
    {
      word_size = sizeof(signed long);
    }
    break;
  case AGS_SOUNDCARD_RESOLUTION_32_BIT:
    {
      word_size = sizeof(signed long);
    }
    break;
  case AGS_SOUNDCARD_RESOLUTION_64_BIT:
    {
      word_size = sizeof(signed long long);
    }
    break;
  default:
    g_warning("ags_devout_alsa_play(): unsupported word size\0");
    return;
  }

  /* do playback */
  devout->flags &= (~AGS_DEVOUT_START_PLAY);

  if((AGS_DEVOUT_INITIALIZED & (devout->flags)) == 0){
    pthread_mutex_unlock(mutex);
    
    return;
  }
  
  /* check buffer flag */
  if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
    memset(devout->buffer[3], 0, (size_t) devout->dsp_channels * devout->buffer_size * word_size);
      
    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle,
					 devout->buffer[0],
					 (snd_pcm_uframes_t) (devout->buffer_size));

    if((AGS_DEVOUT_NONBLOCKING & (devout->flags)) == 0){
      if(devout->out.alsa.rc == -EPIPE){
	/* EPIPE means underrun */
	snd_pcm_prepare(devout->out.alsa.handle);

#ifdef AGS_DEBUG
	g_message("underrun occurred\0");
#endif
      }else if(devout->out.alsa.rc == -ESTRPIPE){
	static const struct timespec idle = {
	  0,
	  4000,
	};

	int err;

	while((err = snd_pcm_resume(devout->out.alsa.handle)) == -EAGAIN)
	  nanosleep(&idle, NULL); /* wait until the suspend flag is released */
	if(err < 0){
	  err = snd_pcm_prepare(devout->out.alsa.handle);
	}
      }else if(devout->out.alsa.rc < 0){
	g_message("error from writei: %s\0", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	g_message("short write, write %d frames\0", devout->out.alsa.rc);
      }
    }      
    //      g_message("ags_devout_play 0\0");
  }else if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
    memset(devout->buffer[0], 0, (size_t) devout->dsp_channels * devout->buffer_size * word_size);

    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle,
					 devout->buffer[1],
					 (snd_pcm_uframes_t) (devout->buffer_size));
     
    if((AGS_DEVOUT_NONBLOCKING & (devout->flags)) == 0){
      if(devout->out.alsa.rc == -EPIPE){
	/* EPIPE means underrun */
	snd_pcm_prepare(devout->out.alsa.handle);

#ifdef AGS_DEBUG
	g_message("underrun occurred\0");
#endif
      }else if(devout->out.alsa.rc == -ESTRPIPE){
	static const struct timespec idle = {
	  0,
	  4000,
	};

	int err;	

	while((err = snd_pcm_resume(devout->out.alsa.handle)) == -EAGAIN)
	  nanosleep(&idle, NULL); /* wait until the suspend flag is released */
	if(err < 0){
	  err = snd_pcm_prepare(devout->out.alsa.handle);
	}
      }else if(devout->out.alsa.rc < 0){
	g_message("error from writei: %s\0", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	g_message("short write, write %d frames\0", devout->out.alsa.rc);
      }
    }      
    //      g_message("ags_devout_play 1\0");
  }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
    memset(devout->buffer[1], 0, (size_t) devout->dsp_channels * devout->buffer_size * word_size);
      
    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle,
					 devout->buffer[2],
					 (snd_pcm_uframes_t) (devout->buffer_size));

    if((AGS_DEVOUT_NONBLOCKING & (devout->flags)) == 0){
      if(devout->out.alsa.rc == -EPIPE){
	/* EPIPE means underrun */
	snd_pcm_prepare(devout->out.alsa.handle);

#ifdef AGS_DEBUG
	g_message("underrun occurred\0");
#endif
      }else if(devout->out.alsa.rc == -ESTRPIPE){
	static const struct timespec idle = {
	  0,
	  4000,
	};

	int err;

	while((err = snd_pcm_resume(devout->out.alsa.handle)) == -EAGAIN)
	  nanosleep(&idle, NULL); /* wait until the suspend flag is released */
	if(err < 0){
	  err = snd_pcm_prepare(devout->out.alsa.handle);
	}
      }else if(devout->out.alsa.rc < 0){
	g_message("error from writei: %s\0", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	g_message("short write, write %d frames\0", devout->out.alsa.rc);
      }
    }
    //      g_message("ags_devout_play 2\0");
  }else if((AGS_DEVOUT_BUFFER3 & devout->flags) != 0){
    memset(devout->buffer[2], 0, (size_t) devout->dsp_channels * devout->buffer_size * word_size);
      
    devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle,
					 devout->buffer[3],
					 (snd_pcm_uframes_t) (devout->buffer_size));

    if((AGS_DEVOUT_NONBLOCKING & (devout->flags)) == 0){
      if(devout->out.alsa.rc == -EPIPE){
	snd_pcm_prepare(devout->out.alsa.handle);

#ifdef AGS_DEBUG
	g_message("underrun occurred\0");
#endif
      }else if(devout->out.alsa.rc == -ESTRPIPE){
	static const struct timespec idle = {
	  0,
	  4000,
	};

	int err;

	while((err = snd_pcm_resume(devout->out.alsa.handle)) == -EAGAIN)
	  nanosleep(&idle, NULL); /* wait until the suspend flag is released */
	if(err < 0){
	  err = snd_pcm_prepare(devout->out.alsa.handle);
	}
      }else if(devout->out.alsa.rc < 0){
	g_message("error from writei: %s\0", snd_strerror(devout->out.alsa.rc));
      }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
	g_message("short write, write %d frames\0", devout->out.alsa.rc);
      }
    }
    //      g_message("ags_devout_play 3\0");
  }

  /* tic */
  ags_soundcard_tic(soundcard);

  /* reset - switch buffer flags */
  ags_devout_switch_buffer_flag(devout);

  snd_pcm_prepare(devout->out.alsa.handle);

  pthread_mutex_unlock(mutex);
}

void
ags_devout_alsa_free(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  AgsMutexManager *mutex_manager;

  AgsApplicationContext *application_context;

  pthread_mutex_t *mutex;
  
  devout = AGS_DEVOUT(soundcard);

  application_context = ags_soundcard_get_application_context(soundcard);
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) devout);
  
  pthread_mutex_unlock(application_context->mutex);

  if((AGS_DEVOUT_INITIALIZED & (devout->flags)) == 0){
    return;
  }
  
  snd_pcm_drain(devout->out.alsa.handle);
  snd_pcm_close(devout->out.alsa.handle);
  devout->out.alsa.handle = NULL;
  devout->flags &= (~(AGS_DEVOUT_BUFFER0 |
		      AGS_DEVOUT_BUFFER1 |
		      AGS_DEVOUT_BUFFER2 |
		      AGS_DEVOUT_BUFFER3 |
		      AGS_DEVOUT_PLAY));
}

void
ags_devout_tic(AgsSoundcard *soundcard)
{
  AgsDevout *devout;
  gdouble delay;
  
  devout = AGS_DEVOUT(soundcard);
  
  /* determine if attack should be switched */
  delay = devout->delay[devout->tic_counter];
  devout->delay_counter += 1.0;

  if(devout->delay_counter >= delay){
    ags_soundcard_set_note_offset(soundcard,
				  devout->note_offset + 1);
    
    /* delay */
    ags_soundcard_offset_changed(soundcard,
				 devout->note_offset);
    
    /* reset - delay counter */
    devout->delay_counter = 0.0;
    devout->tact_counter += 1.0;
  } 
}

void
ags_devout_offset_changed(AgsSoundcard *soundcard,
			  guint note_offset)
{
  AgsDevout *devout;
  
  devout = AGS_DEVOUT(soundcard);

  devout->tic_counter += 1;

  if(devout->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    devout->tic_counter = 0;
  }
}

void
ags_devout_set_bpm(AgsSoundcard *soundcard,
		   gdouble bpm)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);

  devout->bpm = bpm;

  ags_devout_adjust_delay_and_attack(devout);
}

gdouble
ags_devout_get_bpm(AgsSoundcard *soundcard)
{
  AgsDevout *devout;
  
  devout = AGS_DEVOUT(soundcard);

  return(devout->bpm);
}

void
ags_devout_set_delay_factor(AgsSoundcard *soundcard,
			    gdouble delay_factor)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);

  devout->delay_factor = delay_factor;

  ags_devout_adjust_delay_and_attack(devout);
}

gdouble
ags_devout_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsDevout *devout;
  
  devout = AGS_DEVOUT(soundcard);

  return(devout->delay_factor);
}

gdouble
ags_devout_get_delay(AgsSoundcard *soundcard)
{
  AgsDevout *devout;
  guint index;
  
  devout = AGS_DEVOUT(soundcard);
  index = devout->tic_counter;
  
  return(devout->delay[index]);
}

guint
ags_devout_get_attack(AgsSoundcard *soundcard)
{
  AgsDevout *devout;
  guint index;
  
  devout = AGS_DEVOUT(soundcard);
  index = devout->tic_counter;
  
  return(devout->attack[index]);
}

void*
ags_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsDevout *devout;
  signed short *buffer;
  
  devout = AGS_DEVOUT(soundcard);

  if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
    buffer = devout->buffer[0];
  }else if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
    buffer = devout->buffer[1];
  }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
    buffer = devout->buffer[2];
  }else if((AGS_DEVOUT_BUFFER3 & (devout->flags)) != 0){
    buffer = devout->buffer[3];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_devout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsDevout *devout;
  signed short *buffer;
  
  devout = AGS_DEVOUT(soundcard);

  if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
    buffer = devout->buffer[1];
  }else if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
    buffer = devout->buffer[2];
  }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
    buffer = devout->buffer[3];
  }else if((AGS_DEVOUT_BUFFER3 & (devout->flags)) != 0){
    buffer = devout->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void
ags_devout_set_note_offset(AgsSoundcard *soundcard,
			   guint note_offset)
{
  AGS_DEVOUT(soundcard)->note_offset = note_offset;
}

guint
ags_devout_get_note_offset(AgsSoundcard *soundcard)
{
  return(AGS_DEVOUT(soundcard)->note_offset);
}

void
ags_devout_set_audio(AgsSoundcard *soundcard,
		     GList *audio)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);
  devout->audio = audio;
}

GList*
ags_devout_get_audio(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);

  return(devout->audio);
}

/**
 * ags_devout_adjust_delay_and_attack:
 * @devout: the #AgsDevout
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 0.7.0
 */
void
ags_devout_adjust_delay_and_attack(AgsDevout *devout)
{
  gdouble delay;
  guint default_tact_frames;
  guint default_period;
  guint i;

  if(devout == NULL){
    return;
  }
  
  delay = ((gdouble) devout->samplerate / (gdouble) devout->buffer_size) * (gdouble)(60.0 / devout->bpm) * devout->delay_factor;

#ifdef AGS_DEBUG
  g_message("delay : %f\0", delay);
#endif
  
  default_tact_frames = (guint) (delay * devout->buffer_size);
  default_period = (1.0 / AGS_SOUNDCARD_DEFAULT_PERIOD) * (default_tact_frames);

  devout->attack[0] = 0;
  devout->delay[0] = delay;
  
  for(i = 1; i < (int)  2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    devout->attack[i] = (guint) ((i * default_tact_frames + devout->attack[i - 1]) / (AGS_SOUNDCARD_DEFAULT_PERIOD / (delay * i))) % (guint) (devout->buffer_size);
    
#ifdef AGS_DEBUG
    g_message("%d\0", devout->attack[i]);
#endif
  }
  
  for(i = 1; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    devout->delay[i] = ((gdouble) (default_tact_frames + devout->attack[i])) / (gdouble) devout->buffer_size;
    
#ifdef AGS_DEBUG
    g_message("%f\0", devout->delay[i]);
#endif
  }
}

/**
 * ags_devout_realloc_buffer:
 * @devout: the #AgsDevout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 0.7.0
 */
void
ags_devout_realloc_buffer(AgsDevout *devout)
{
  guint word_size;

  if(devout == NULL){
    return;
  }

  switch(devout->format){
  case AGS_SOUNDCARD_RESOLUTION_8_BIT:
    {
      word_size = sizeof(signed char);
    }
    break;
  case AGS_SOUNDCARD_RESOLUTION_16_BIT:
    {
      word_size = sizeof(signed short);
    }
    break;
  case AGS_SOUNDCARD_RESOLUTION_24_BIT:
    {
      word_size = sizeof(signed long);
    }
    break;
  case AGS_SOUNDCARD_RESOLUTION_32_BIT:
    {
      word_size = sizeof(signed long);
    }
    break;
  case AGS_SOUNDCARD_RESOLUTION_64_BIT:
    {
      word_size = sizeof(signed long long);
    }
    break;
  default:
    g_warning("ags_devout_realloc_buffer(): unsupported word size\0");
    return;
  }
  
  /* AGS_DEVOUT_BUFFER_0 */
  if(devout->buffer[0] != NULL){
    free(devout->buffer[0]);
  }
  
  devout->buffer[0] = (void *) malloc(devout->dsp_channels * devout->buffer_size * word_size);
  
  /* AGS_DEVOUT_BUFFER_1 */
  if(devout->buffer[1] != NULL){
    free(devout->buffer[1]);
  }

  devout->buffer[1] = (void *) malloc(devout->dsp_channels * devout->buffer_size * word_size);
  
  /* AGS_DEVOUT_BUFFER_2 */
  if(devout->buffer[2] != NULL){
    free(devout->buffer[2]);
  }

  devout->buffer[2] = (void *) malloc(devout->dsp_channels * devout->buffer_size * word_size);
  
  /* AGS_DEVOUT_BUFFER_3 */
  if(devout->buffer[3] != NULL){
    free(devout->buffer[3]);
  }
  
  devout->buffer[3] = (void *) malloc(devout->dsp_channels * devout->buffer_size * word_size);
}

/**
 * ags_devout_new:
 * @application_context: the #AgsApplicationContext
 *
 * Creates an #AgsDevout, refering to @application_context.
 *
 * Returns: a new #AgsDevout
 *
 * Since: 0.3
 */
AgsDevout*
ags_devout_new(GObject *application_context)
{
  AgsDevout *devout;

  devout = (AgsDevout *) g_object_new(AGS_TYPE_DEVOUT,
				      "application-context\0", application_context,
				      NULL);
  
  return(devout);
}
