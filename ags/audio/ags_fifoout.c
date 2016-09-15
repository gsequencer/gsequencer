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

#include <ags/audio/ags_fifoout.h>

#include <ags/lib/ags_time.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>

#include <ags/object/ags_config.h>
#include <ags/object/ags_soundcard.h>
#include <ags/object/ags_concurrent_tree.h>

#include <ags/thread/ags_mutex_manager.h>
#include <ags/thread/ags_task_thread.h>
#include <ags/thread/ags_poll_fd.h>

#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#include <errno.h>

#include <poll.h>

#include <string.h>
#include <math.h>

#include <time.h>
#include <signal.h>
#include <strings.h>
#include <unistd.h>

#include <ags/config.h>

/**
 * SECTION:ags_fifoout
 * @short_description: Output to soundcard
 * @title: AgsFifoout
 * @section_id:
 * @include: ags/audio/ags_fifoout.h
 *
 * #AgsFifoout represents a soundcard and supports output.
 */

void ags_fifoout_class_init(AgsFifooutClass *fifoout);
void ags_fifoout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_fifoout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_fifoout_concurrent_tree_interface_init(AgsConcurrentTreeInterface *concurrent_tree);
void ags_fifoout_init(AgsFifoout *fifoout);
void ags_fifoout_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec);
void ags_fifoout_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec);
void ags_fifoout_disconnect(AgsConnectable *connectable);
void ags_fifoout_connect(AgsConnectable *connectable);
pthread_mutex_t* ags_fifoout_get_lock(AgsConcurrentTree *concurrent_tree);
pthread_mutex_t* ags_fifoout_get_parent_lock(AgsConcurrentTree *concurrent_tree);
void ags_fifoout_finalize(GObject *gobject);

void ags_fifoout_switch_buffer_flag(AgsFifoout *fifoout);

void ags_fifoout_set_application_context(AgsSoundcard *soundcard,
					 AgsApplicationContext *application_context);
AgsApplicationContext* ags_fifoout_get_application_context(AgsSoundcard *soundcard);

void ags_fifoout_set_application_mutex(AgsSoundcard *soundcard,
				       pthread_mutex_t *application_mutex);
pthread_mutex_t* ags_fifoout_get_application_mutex(AgsSoundcard *soundcard);

void ags_fifoout_set_device(AgsSoundcard *soundcard,
			    gchar *device);
gchar* ags_fifoout_get_device(AgsSoundcard *soundcard);

void ags_fifoout_set_presets(AgsSoundcard *soundcard,
			     guint channels,
			     guint rate,
			     guint buffer_size,
			     guint format);
void ags_fifoout_get_presets(AgsSoundcard *soundcard,
			     guint *channels,
			     guint *rate,
			     guint *buffer_size,
			     guint *format);

void ags_fifoout_list_cards(AgsSoundcard *soundcard,
			    GList **card_id, GList **card_name);
void ags_fifoout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			  guint *channels_min, guint *channels_max,
			  guint *rate_min, guint *rate_max,
			  guint *buffer_size_min, guint *buffer_size_max,
			  GError **error);

GList* ags_fifoout_get_poll_fd(AgsSoundcard *soundcard);
gboolean ags_fifoout_is_available(AgsSoundcard *soundcard);

gboolean ags_fifoout_is_starting(AgsSoundcard *soundcard);
gboolean ags_fifoout_is_playing(AgsSoundcard *soundcard);

gchar* ags_fifoout_get_uptime(AgsSoundcard *soundcard);

void ags_fifoout_fifo_init(AgsSoundcard *soundcard,
			   GError **error);
void ags_fifoout_fifo_play(AgsSoundcard *soundcard,
			   GError **error);
void ags_fifoout_fifo_free(AgsSoundcard *soundcard);

void ags_fifoout_tic(AgsSoundcard *soundcard);
void ags_fifoout_offset_changed(AgsSoundcard *soundcard,
				guint note_offset);

void ags_fifoout_set_bpm(AgsSoundcard *soundcard,
			 gdouble bpm);
gdouble ags_fifoout_get_bpm(AgsSoundcard *soundcard);

void ags_fifoout_set_delay_factor(AgsSoundcard *soundcard,
				  gdouble delay_factor);
gdouble ags_fifoout_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_fifoout_get_delay(AgsSoundcard *soundcard);
guint ags_fifoout_get_attack(AgsSoundcard *soundcard);

void* ags_fifoout_get_buffer(AgsSoundcard *soundcard);
void* ags_fifoout_get_next_buffer(AgsSoundcard *soundcard);

guint ags_fifoout_get_delay_counter(AgsSoundcard *soundcard);

void ags_fifoout_set_note_offset(AgsSoundcard *soundcard,
				 guint note_offset);
guint ags_fifoout_get_note_offset(AgsSoundcard *soundcard);

void ags_fifoout_set_loop(AgsSoundcard *soundcard,
			  guint loop_left, guint loop_right,
			  gboolean do_loop);
guint ags_fifoout_get_loop(AgsSoundcard *soundcard,
			   guint *loop_left, guint *loop_right,
			   gboolean *do_loop);

guint ags_fifoout_get_loop_offset(AgsSoundcard *soundcard);

void ags_fifoout_set_audio(AgsSoundcard *soundcard,
			   GList *audio);
GList* ags_fifoout_get_audio(AgsSoundcard *soundcard);

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

static gpointer ags_fifoout_parent_class = NULL;
static guint fifoout_signals[LAST_SIGNAL];

GType
ags_fifoout_get_type (void)
{
  static GType ags_type_fifoout = 0;

  if(!ags_type_fifoout){
    static const GTypeInfo ags_fifoout_info = {
      sizeof (AgsFifooutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fifoout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsFifoout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fifoout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_fifoout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_fifoout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_concurrent_tree_interface_info = {
      (GInterfaceInitFunc) ags_fifoout_concurrent_tree_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_fifoout = g_type_register_static(G_TYPE_OBJECT,
					      "AgsFifoout\0",
					      &ags_fifoout_info,
					      0);

    g_type_add_interface_static(ags_type_fifoout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_fifoout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_type_add_interface_static(ags_type_fifoout,
				AGS_TYPE_CONCURRENT_TREE,
				&ags_concurrent_tree_interface_info);
  }

  return (ags_type_fifoout);
}

void
ags_fifoout_class_init(AgsFifooutClass *fifoout)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_fifoout_parent_class = g_type_class_peek_parent(fifoout);

  /* GObjectClass */
  gobject = (GObjectClass *) fifoout;

  gobject->set_property = ags_fifoout_set_property;
  gobject->get_property = ags_fifoout_get_property;

  gobject->finalize = ags_fifoout_finalize;

  /* properties */
  /**
   * AgsFifoout:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 0.7.65
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
   * AgsFifoout:application-mutex:
   *
   * The assigned application mutex
   * 
   * Since: 0.7.65
   */
  param_spec = g_param_spec_pointer("application-mutex\0",
				    "the application mutex object\0",
				    "The application mutex object\0",
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_MUTEX,
				  param_spec);

  /**
   * AgsFifoout:device:
   *
   * The fifo soundcard indentifier
   * 
   * Since: 0.7.65
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
   * AgsFifoout:dsp-channels:
   *
   * The dsp channel count
   * 
   * Since: 0.7.65
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
   * AgsFifoout:pcm-channels:
   *
   * The pcm channel count
   * 
   * Since: 0.7.65
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
   * AgsFifoout:format:
   *
   * The precision of the buffer
   * 
   * Since: 0.7.65
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
   * AgsFifoout:buffer-size:
   *
   * The buffer size
   * 
   * Since: 0.7.65
   */
  param_spec = g_param_spec_uint("buffer-size\0",
				 "frame count of a buffer\0",
				 "The count of frames a buffer contains\0",
				 1,
				 44100,
				 AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsFifoout:samplerate:
   *
   * The samplerate
   * 
   * Since: 0.7.65
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
   * AgsFifoout:buffer:
   *
   * The buffer
   * 
   * Since: 0.7.65
   */
  param_spec = g_param_spec_pointer("buffer\0",
				    "the buffer\0",
				    "The buffer to play\0",
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsFifoout:bpm:
   *
   * Beats per minute
   * 
   * Since: 0.7.65
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
   * AgsFifoout:delay-factor:
   *
   * tact
   * 
   * Since: 0.7.65
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
   * AgsFifoout:attack:
   *
   * Attack of the buffer
   * 
   * Since: 0.7.65
   */
  param_spec = g_param_spec_pointer("attack\0",
				    "attack of buffer\0",
				    "The attack to use for the buffer\0",
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);


  /* AgsFifooutClass */
}

GQuark
ags_fifoout_error_quark()
{
  return(g_quark_from_static_string("ags-fifoout-error-quark\0"));
}

void
ags_fifoout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_fifoout_connect;
  connectable->disconnect = ags_fifoout_disconnect;
}

void
ags_fifoout_concurrent_tree_interface_init(AgsConcurrentTreeInterface *concurrent_tree)
{
  concurrent_tree->get_lock = ags_fifoout_get_lock;
  concurrent_tree->get_parent_lock = ags_fifoout_get_parent_lock;
}

void
ags_fifoout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_application_context = ags_fifoout_set_application_context;
  soundcard->get_application_context = ags_fifoout_get_application_context;

  soundcard->set_application_mutex = ags_fifoout_set_application_mutex;
  soundcard->get_application_mutex = ags_fifoout_get_application_mutex;

  soundcard->set_device = ags_fifoout_set_device;
  soundcard->get_device = ags_fifoout_get_device;
  
  soundcard->set_presets = ags_fifoout_set_presets;
  soundcard->get_presets = ags_fifoout_get_presets;

  soundcard->list_cards = ags_fifoout_list_cards;
  soundcard->pcm_info = ags_fifoout_pcm_info;

  soundcard->get_poll_fd = ags_fifoout_get_poll_fd;
  soundcard->is_available = ags_fifoout_is_available;

  soundcard->is_starting =  ags_fifoout_is_starting;
  soundcard->is_playing = ags_fifoout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_fifoout_get_uptime;
  
  soundcard->play_init = ags_fifoout_fifo_init;
  soundcard->play = ags_fifoout_fifo_play;

  soundcard->record_init = NULL;
  soundcard->record = NULL;

  soundcard->stop = ags_fifoout_fifo_free;

  soundcard->tic = ags_fifoout_tic;
  soundcard->offset_changed = ags_fifoout_offset_changed;
    
  soundcard->set_bpm = ags_fifoout_set_bpm;
  soundcard->get_bpm = ags_fifoout_get_bpm;

  soundcard->set_delay_factor = ags_fifoout_set_delay_factor;
  soundcard->get_delay_factor = ags_fifoout_get_delay_factor;
  
  soundcard->get_delay = ags_fifoout_get_delay;
  soundcard->get_attack = ags_fifoout_get_attack;

  soundcard->get_buffer = ags_fifoout_get_buffer;
  soundcard->get_next_buffer = ags_fifoout_get_next_buffer;

  soundcard->get_delay_counter = ags_fifoout_get_delay_counter;

  soundcard->set_note_offset = ags_fifoout_set_note_offset;
  soundcard->get_note_offset = ags_fifoout_get_note_offset;

  soundcard->set_loop = ags_fifoout_set_loop;
  soundcard->get_loop = ags_fifoout_get_loop;

  soundcard->get_loop_offset = ags_fifoout_get_loop_offset;

  soundcard->set_audio = ags_fifoout_set_audio;
  soundcard->get_audio = ags_fifoout_get_audio;
}

void
ags_fifoout_init(AgsFifoout *fifoout)
{
  AgsMutexManager *mutex_manager;
  
  AgsConfig *config;
  
  gchar *str;
  
  pthread_mutex_t *application_mutex;
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* insert fifoout mutex */
  //FIXME:JK: memory leak
  attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(attr,
				PTHREAD_PRIO_INHERIT);
#endif

  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) fifoout,
			   mutex);
  
  pthread_mutex_unlock(application_mutex);

  /* flags */
  config = ags_config_get_instance();

  /* quality */
  fifoout->dsp_channels = AGS_SOUNDCARD_DEFAULT_DSP_CHANNELS;
  fifoout->pcm_channels = AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS;
  fifoout->format = AGS_SOUNDCARD_SIGNED_16_BIT;
  fifoout->samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  fifoout->buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  /* read config */
  /* dsp channels */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "dsp-channels\0");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "dsp-channels\0");
  }
  
  if(str != NULL){
    fifoout->dsp_channels = g_ascii_strtoull(str,
					     NULL,
					     10);
	  
    g_free(str);
  }

  /* pcm channels */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "pcm-channels\0");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "pcm-channels\0");
  }
  
  if(str != NULL){
    fifoout->pcm_channels = g_ascii_strtoull(str,
					     NULL,
					     10);
	  
    g_free(str);
  }

  /* samplerate */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate\0");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "samplerate\0");
  }
  
  if(str != NULL){
    fifoout->samplerate = g_ascii_strtoull(str,
					   NULL,
					   10);
    free(str);
  }

  /* buffer size */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "buffer-size\0");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "buffer-size\0");
  }
  
  if(str != NULL){
    fifoout->buffer_size = g_ascii_strtoull(str,
					    NULL,
					    10);
    free(str);
  }

  /* format */
  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "format\0");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "format\0");
  }
  
  if(str != NULL){
    fifoout->format = g_ascii_strtoull(str,
				       NULL,
				       10);
    free(str);
  }

  /*  */
  fifoout->device = NULL;
  fifoout->fifo_fd = -1;

  /* buffer */
  fifoout->buffer = (void **) malloc(4 * sizeof(void*));

  fifoout->buffer[0] = NULL;
  fifoout->buffer[1] = NULL;
  fifoout->buffer[2] = NULL;
  fifoout->buffer[3] = NULL;
  
  fifoout->ring_buffer = NULL;

  ags_fifoout_realloc_buffer(fifoout);
  
  /* bpm */
  fifoout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  fifoout->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* delay and attack */
  fifoout->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
				      sizeof(gdouble));
  
  fifoout->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
				     sizeof(guint));

  ags_fifoout_adjust_delay_and_attack(fifoout);
  
  /* counters */
  fifoout->tact_counter = 0.0;
  fifoout->delay_counter = 0;
  fifoout->tic_counter = 0;

  fifoout->note_offset = 0;

  fifoout->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  fifoout->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  fifoout->do_loop = FALSE;

  fifoout->loop_offset = 0;
  
  /* parent */
  fifoout->application_context = NULL;
  fifoout->application_mutex = NULL;

  fifoout->poll_fd = NULL;
  
  /* all AgsAudio */
  fifoout->audio = NULL;
}

void
ags_fifoout_set_property(GObject *gobject,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec *param_spec)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = g_value_get_object(value);

      if(fifoout->application_context == application_context){
	return;
      }

      if(fifoout->application_context != NULL){
	g_object_unref(G_OBJECT(fifoout->application_context));
      }

      if(application_context != NULL){
	AgsConfig *config;

	gchar *device;
	gchar *str;
	gchar *segmentation;
	guint discriminante, nominante;
	
	g_object_ref(G_OBJECT(application_context));

	fifoout->application_mutex = application_context->mutex;
	
	config = ags_config_get_instance();

	/* dsp channels */
	str = ags_config_get_value(config,
				   AGS_CONFIG_SOUNDCARD,
				   "dsp-channels\0");

	if(str == NULL){
	  str = ags_config_get_value(config,
				     AGS_CONFIG_SOUNDCARD_0,
				     "dsp-channels\0");
	}
  
	if(str != NULL){
	  fifoout->dsp_channels = g_ascii_strtoull(str,
						   NULL,
						   10);
	  
	  g_free(str);
	}

	/* pcm channels */
	str = ags_config_get_value(config,
				   AGS_CONFIG_SOUNDCARD,
				   "pcm-channels\0");

	if(str == NULL){
	  str = ags_config_get_value(config,
				     AGS_CONFIG_SOUNDCARD_0,
				     "pcm-channels\0");
	}
  
	if(str != NULL){
	  fifoout->pcm_channels = g_ascii_strtoull(str,
						   NULL,
						   10);
	  
	  g_free(str);
	}

	/* samplerate */
	str = ags_config_get_value(config,
				   AGS_CONFIG_SOUNDCARD,
				   "samplerate\0");

	if(str == NULL){
	  str = ags_config_get_value(config,
				     AGS_CONFIG_SOUNDCARD_0,
				     "samplerate\0");
	}
  
	if(str != NULL){
	  fifoout->samplerate = g_ascii_strtoull(str,
						 NULL,
						 10);
	  free(str);
	}

	/* buffer size */
	str = ags_config_get_value(config,
				   AGS_CONFIG_SOUNDCARD,
				   "buffer-size\0");

	if(str == NULL){
	  str = ags_config_get_value(config,
				     AGS_CONFIG_SOUNDCARD_0,
				     "buffer-size\0");
	}
  
	if(str != NULL){
	  fifoout->buffer_size = g_ascii_strtoull(str,
						  NULL,
						  10);
	  free(str);
	}

	/* format */
	str = ags_config_get_value(config,
				   AGS_CONFIG_SOUNDCARD,
				   "format\0");

	if(str == NULL){
	  str = ags_config_get_value(config,
				     AGS_CONFIG_SOUNDCARD_0,
				     "format\0");
	}
  
	if(str != NULL){
	  fifoout->format = g_ascii_strtoull(str,
					     NULL,
					     10);
	  free(str);
	}

	/* segmentation */
	segmentation = ags_config_get_value(config,
					    AGS_CONFIG_GENERIC,
					    "segmentation\0");

	if(segmentation != NULL){
	  sscanf(segmentation, "%d/%d\0",
		 &discriminante,
		 &nominante);
    
	  fifoout->delay_factor = 1.0 / nominante * (nominante / discriminante);

	  g_free(segmentation);
	}

	fifoout->fifo_fd = -1;
	device = ags_config_get_value(config,
				      AGS_CONFIG_SOUNDCARD,
				      "device\0");

	if(device == NULL){
	  device = ags_config_get_value(config,
					AGS_CONFIG_SOUNDCARD_0,
					"device\0");
	}
	if(device != NULL){
	  fifoout->device = device;
	  g_message("FIFO device %s\n", fifoout->device);
	}	  

	ags_fifoout_adjust_delay_and_attack(fifoout);
	ags_fifoout_realloc_buffer(fifoout);
      }else{
	fifoout->application_mutex = NULL;
      }

      fifoout->application_context = application_context;
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      pthread_mutex_t *application_mutex;

      application_mutex = (pthread_mutex_t *) g_value_get_pointer(value);

      if(fifoout->application_mutex == application_mutex){
	return;
      }
      
      fifoout->application_mutex = application_mutex;
    }
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);
      
      fifoout->device = g_strdup(device);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      if(dsp_channels == fifoout->dsp_channels){
	return;
      }

      fifoout->dsp_channels = dsp_channels;
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      if(pcm_channels == fifoout->pcm_channels){
	return;
      }

      fifoout->pcm_channels = pcm_channels;

      ags_fifoout_realloc_buffer(fifoout);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      if(format == fifoout->format){
	return;
      }

      fifoout->format = format;

      ags_fifoout_realloc_buffer(fifoout);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      if(buffer_size == fifoout->buffer_size){
	return;
      }

      fifoout->buffer_size = buffer_size;

      ags_fifoout_realloc_buffer(fifoout);
      ags_fifoout_adjust_delay_and_attack(fifoout);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      if(samplerate == fifoout->samplerate){
	return;
      }

      fifoout->samplerate = samplerate;
      ags_fifoout_adjust_delay_and_attack(fifoout);
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

      fifoout->bpm = bpm;

      ags_fifoout_adjust_delay_and_attack(fifoout);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      fifoout->delay_factor = delay_factor;

      ags_fifoout_adjust_delay_and_attack(fifoout);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fifoout_get_property(GObject *gobject,
			 guint prop_id,
			 GValue *value,
			 GParamSpec *param_spec)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(gobject);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, fifoout->application_context);
    }
    break;
  case PROP_APPLICATION_MUTEX:
    {
      g_value_set_pointer(value, fifoout->application_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      g_value_set_string(value, fifoout->device);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_value_set_uint(value, fifoout->dsp_channels);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_value_set_uint(value, fifoout->pcm_channels);
    }
    break;
  case PROP_FORMAT:
    {
      g_value_set_uint(value, fifoout->format);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_value_set_uint(value, fifoout->buffer_size);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_value_set_uint(value, fifoout->samplerate);
    }
    break;
  case PROP_BUFFER:
    {
      g_value_set_pointer(value, fifoout->buffer);
    }
    break;
  case PROP_BPM:
    {
      g_value_set_double(value, fifoout->bpm);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_value_set_double(value, fifoout->delay_factor);
    }
    break;
  case PROP_ATTACK:
    {
      g_value_set_pointer(value, fifoout->attack);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

pthread_mutex_t*
ags_fifoout_get_lock(AgsConcurrentTree *concurrent_tree)
{
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *application_mutex;
  pthread_mutex_t *fifoout_mutex;
  
  /* lookup mutex */
  mutex_manager = ags_mutex_manager_get_instance();
  application_mutex = ags_mutex_manager_get_application_mutex(mutex_manager);
  
  pthread_mutex_lock(application_mutex);
  
  fifoout_mutex = ags_mutex_manager_lookup(mutex_manager,
					   AGS_FIFOOUT(concurrent_tree));

  pthread_mutex_unlock(application_mutex);

  return(fifoout_mutex);
}

pthread_mutex_t*
ags_fifoout_get_parent_lock(AgsConcurrentTree *concurrent_tree)
{
  return(NULL);
}

void
ags_fifoout_finalize(GObject *gobject)
{
  AgsFifoout *fifoout;

  AgsMutexManager *mutex_manager;
  
  GList *list, *list_next;

  fifoout = AGS_FIFOOUT(gobject);

  /* remove fifoout mutex */
  pthread_mutex_lock(fifoout->application_mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  ags_mutex_manager_remove(mutex_manager,
			   gobject);
  
  pthread_mutex_unlock(fifoout->application_mutex);

  /* free output buffer */
  free(fifoout->buffer[0]);
  free(fifoout->buffer[1]);
  free(fifoout->buffer[2]);
  free(fifoout->buffer[3]);

  /* free buffer array */
  free(fifoout->buffer);

  /* free AgsAttack */
  free(fifoout->attack);

  if(fifoout->audio != NULL){
    g_list_free_full(fifoout->audio,
		     g_object_unref);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_fifoout_parent_class)->finalize(gobject);
}

void
ags_fifoout_connect(AgsConnectable *connectable)
{
  AgsFifoout *fifoout;
  
  AgsMutexManager *mutex_manager;

  GList *list;

  pthread_mutex_t *mutex;
  pthread_mutexattr_t attr;

  fifoout = AGS_FIFOOUT(connectable);

  /* create fifoout mutex */
  //FIXME:JK: memory leak
  pthread_mutexattr_init(&attr);
  pthread_mutexattr_settype(&attr,
			    PTHREAD_MUTEX_RECURSIVE);
  
  mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     &attr);

  /* insert mutex */
  pthread_mutex_lock(fifoout->application_mutex);

  mutex_manager = ags_mutex_manager_get_instance();

  ags_mutex_manager_insert(mutex_manager,
			   (GObject *) fifoout,
			   mutex);
  
  pthread_mutex_unlock(fifoout->application_mutex);

  /*  */  
  list = fifoout->audio;

  while(list != NULL){
    ags_connectable_connect(AGS_CONNECTABLE(list->data));

    list = list->next;
  }
}

void
ags_fifoout_disconnect(AgsConnectable *connectable)
{
  //TODO:JK: implement me
}

/**
 * ags_fifoout_switch_buffer_flag:
 * @fifoout: an #AgsFifoout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 0.7.65
 */
void
ags_fifoout_switch_buffer_flag(AgsFifoout *fifoout)
{
  AgsApplicationContext *application_context;
  
  AgsMutexManager *mutex_manager;

  pthread_mutex_t *mutex;
  
  application_context = ags_soundcard_get_application_context(AGS_SOUNDCARD(fifoout));
  
  pthread_mutex_lock(application_context->mutex);
  
  mutex_manager = ags_mutex_manager_get_instance();

  mutex = ags_mutex_manager_lookup(mutex_manager,
				   (GObject *) fifoout);
  
  pthread_mutex_unlock(application_context->mutex);

  //  g_message("switch - 0x%0x\0", ((AGS_FIFOOUT_BUFFER0 |
  //				  AGS_FIFOOUT_BUFFER1 |
  //				  AGS_FIFOOUT_BUFFER2 |
  //				  AGS_FIFOOUT_BUFFER3) & (fifoout->flags)));
  
  /* switch buffer flag */
  pthread_mutex_lock(mutex);

  if((AGS_FIFOOUT_BUFFER0 & (fifoout->flags)) != 0){
    fifoout->flags &= (~AGS_FIFOOUT_BUFFER0);
    fifoout->flags |= AGS_FIFOOUT_BUFFER1;
  }else if((AGS_FIFOOUT_BUFFER1 & (fifoout->flags)) != 0){
    fifoout->flags &= (~AGS_FIFOOUT_BUFFER1);
    fifoout->flags |= AGS_FIFOOUT_BUFFER2;
  }else if((AGS_FIFOOUT_BUFFER2 & (fifoout->flags)) != 0){
    fifoout->flags &= (~AGS_FIFOOUT_BUFFER2);
    fifoout->flags |= AGS_FIFOOUT_BUFFER3;
  }else if((AGS_FIFOOUT_BUFFER3 & (fifoout->flags)) != 0){
    fifoout->flags &= (~AGS_FIFOOUT_BUFFER3);
    fifoout->flags |= AGS_FIFOOUT_BUFFER0;
  }

  pthread_mutex_unlock(mutex);
}

void
ags_fifoout_set_application_context(AgsSoundcard *soundcard,
				    AgsApplicationContext *application_context)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);
  fifoout->application_context = application_context;
}

AgsApplicationContext*
ags_fifoout_get_application_context(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);

  return(fifoout->application_context);
}

void
ags_fifoout_set_application_mutex(AgsSoundcard *soundcard,
				  pthread_mutex_t *application_mutex)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);
  fifoout->application_mutex = application_mutex;
}

pthread_mutex_t*
ags_fifoout_get_application_mutex(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);

  return(fifoout->application_mutex);
}

void
ags_fifoout_set_device(AgsSoundcard *soundcard,
		       gchar *device)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);
  fifoout->device = g_strdup(device);
}

gchar*
ags_fifoout_get_device(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;
  
  fifoout = AGS_FIFOOUT(soundcard);

  return(fifoout->device);
}

void
ags_fifoout_set_presets(AgsSoundcard *soundcard,
			guint channels,
			guint rate,
			guint buffer_size,
			guint format)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);
  g_object_set(fifoout,
	       "pcm-channels\0", channels,
	       "samplerate\0", rate,
	       "buffer-size\0", buffer_size,
	       "format\0", format,
	       NULL);
}

void
ags_fifoout_get_presets(AgsSoundcard *soundcard,
			guint *channels,
			guint *rate,
			guint *buffer_size,
			guint *format)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);

  if(channels != NULL){
    *channels = fifoout->pcm_channels;
  }

  if(rate != NULL){
    *rate = fifoout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = fifoout->buffer_size;
  }

  if(format != NULL){
    *format = fifoout->format;
  }
}

/**
 * ags_fifoout_list_cards:
 * @soundcard: the #AgsSoundcard
 * @card_id: identifier
 * @card_name: card name
 *
 * List available soundcards.
 *
 * Since: 0.7.65
 */
void
ags_fifoout_list_cards(AgsSoundcard *soundcard,
		       GList **card_id, GList **card_name)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);

  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  //TODO:JK: implement me
  
  if(card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

void
ags_fifoout_pcm_info(AgsSoundcard *soundcard,
		     char *card_id,
		     guint *channels_min, guint *channels_max,
		     guint *rate_min, guint *rate_max,
		     guint *buffer_size_min, guint *buffer_size_max,
		     GError **error)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);
  
  //TODO:JK: implement me  
}

GList*
ags_fifoout_get_poll_fd(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  GList *list;

  fifoout = AGS_FIFOOUT(soundcard);
  list = NULL;
  
  //TODO:JK: implement me  
  
  return(list);
}

gboolean
ags_fifoout_is_available(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;
  
  //TODO:JK: implement me  

  return(TRUE);
}

gboolean
ags_fifoout_is_starting(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);
  
  return(((AGS_FIFOOUT_START_PLAY & (fifoout->flags)) != 0) ? TRUE: FALSE);
}

gboolean
ags_fifoout_is_playing(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);
  
  return(((AGS_FIFOOUT_PLAY & (fifoout->flags)) != 0) ? TRUE: FALSE);
}

gchar*
ags_fifoout_get_uptime(AgsSoundcard *soundcard)
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
    
    note_offset = ags_soundcard_get_note_offset(soundcard);

    bpm = ags_soundcard_get_bpm(soundcard);
    delay_factor = ags_soundcard_get_delay_factor(soundcard);

    /* calculate delays */
    delay = ((gdouble) samplerate / (gdouble) buffer_size) * (gdouble)(60.0 / bpm) * delay_factor;
  
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
ags_fifoout_fifo_init(AgsSoundcard *soundcard,
		      GError **error)
{
  //TODO:JK: implement me  
}

void
ags_fifoout_fifo_play(AgsSoundcard *soundcard,
		      GError **error)
{
  //TODO:JK: implement me  
}

void
ags_fifoout_fifo_free(AgsSoundcard *soundcard)
{
  //TODO:JK: implement me  
}

void
ags_fifoout_tic(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;
  gdouble delay;
  
  fifoout = AGS_FIFOOUT(soundcard);
  
  /* determine if attack should be switched */
  delay = fifoout->delay[fifoout->tic_counter];
  fifoout->delay_counter += 1.0;

  if(fifoout->delay_counter >= delay){
    ags_soundcard_set_note_offset(soundcard,
				  fifoout->note_offset + 1);
    
    /* delay */
    ags_soundcard_offset_changed(soundcard,
				 fifoout->note_offset);
    
    /* reset - delay counter */
    fifoout->delay_counter = 0.0;
    fifoout->tact_counter += 1.0;
  } 
}

void
ags_fifoout_offset_changed(AgsSoundcard *soundcard,
			   guint note_offset)
{
  AgsFifoout *fifoout;
  
  fifoout = AGS_FIFOOUT(soundcard);

  fifoout->tic_counter += 1;

  if(fifoout->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    fifoout->tic_counter = 0;
  }
}

void
ags_fifoout_set_bpm(AgsSoundcard *soundcard,
		    gdouble bpm)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);

  fifoout->bpm = bpm;

  ags_fifoout_adjust_delay_and_attack(fifoout);
}

gdouble
ags_fifoout_get_bpm(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;
  
  fifoout = AGS_FIFOOUT(soundcard);

  return(fifoout->bpm);
}

void
ags_fifoout_set_delay_factor(AgsSoundcard *soundcard,
			     gdouble delay_factor)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);

  fifoout->delay_factor = delay_factor;

  ags_fifoout_adjust_delay_and_attack(fifoout);
}

gdouble
ags_fifoout_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;
  
  fifoout = AGS_FIFOOUT(soundcard);

  return(fifoout->delay_factor);
}

gdouble
ags_fifoout_get_delay(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;
  guint index;
  
  fifoout = AGS_FIFOOUT(soundcard);
  index = fifoout->tic_counter;
  
  return(fifoout->delay[index]);
}

guint
ags_fifoout_get_attack(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;
  guint index;
  
  fifoout = AGS_FIFOOUT(soundcard);
  index = fifoout->tic_counter;
  
  return(fifoout->attack[index]);
}

void*
ags_fifoout_get_buffer(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;
  signed short *buffer;
  
  fifoout = AGS_FIFOOUT(soundcard);

  if((AGS_FIFOOUT_BUFFER0 & (fifoout->flags)) != 0){
    buffer = fifoout->buffer[0];
  }else if((AGS_FIFOOUT_BUFFER1 & (fifoout->flags)) != 0){
    buffer = fifoout->buffer[1];
  }else if((AGS_FIFOOUT_BUFFER2 & (fifoout->flags)) != 0){
    buffer = fifoout->buffer[2];
  }else if((AGS_FIFOOUT_BUFFER3 & (fifoout->flags)) != 0){
    buffer = fifoout->buffer[3];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_fifoout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;
  signed short *buffer;
  
  fifoout = AGS_FIFOOUT(soundcard);

  //  g_message("next - 0x%0x\0", ((AGS_FIFOOUT_BUFFER0 |
  //				AGS_FIFOOUT_BUFFER1 |
  //				AGS_FIFOOUT_BUFFER2 |
  //				AGS_FIFOOUT_BUFFER3) & (fifoout->flags)));
  
  if((AGS_FIFOOUT_BUFFER0 & (fifoout->flags)) != 0){
    buffer = fifoout->buffer[1];
  }else if((AGS_FIFOOUT_BUFFER1 & (fifoout->flags)) != 0){
    buffer = fifoout->buffer[2];
  }else if((AGS_FIFOOUT_BUFFER2 & (fifoout->flags)) != 0){
    buffer = fifoout->buffer[3];
  }else if((AGS_FIFOOUT_BUFFER3 & (fifoout->flags)) != 0){
    buffer = fifoout->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

guint
ags_fifoout_get_delay_counter(AgsSoundcard *soundcard)
{
  return(AGS_FIFOOUT(soundcard)->delay_counter);
}

void
ags_fifoout_set_note_offset(AgsSoundcard *soundcard,
			    guint note_offset)
{
  AGS_FIFOOUT(soundcard)->note_offset = note_offset;
}

guint
ags_fifoout_get_note_offset(AgsSoundcard *soundcard)
{
  return(AGS_FIFOOUT(soundcard)->note_offset);
}

void
ags_fifoout_set_loop(AgsSoundcard *soundcard,
		     guint loop_left, guint loop_right,
		     gboolean do_loop)
{
  AGS_FIFOOUT(soundcard)->loop_left = loop_left;
  AGS_FIFOOUT(soundcard)->loop_right = loop_right;
  AGS_FIFOOUT(soundcard)->do_loop = do_loop;

  if(do_loop){
    AGS_FIFOOUT(soundcard)->loop_offset = AGS_FIFOOUT(soundcard)->note_offset;
  }
}

guint
ags_fifoout_get_loop(AgsSoundcard *soundcard,
		     guint *loop_left, guint *loop_right,
		     gboolean *do_loop)
{
  if(loop_left != NULL){
    *loop_left = AGS_FIFOOUT(soundcard)->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = AGS_FIFOOUT(soundcard)->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = AGS_FIFOOUT(soundcard)->do_loop;
  }
}

guint
ags_fifoout_get_loop_offset(AgsSoundcard *soundcard)
{
  return(AGS_FIFOOUT(soundcard)->loop_offset);
}

void
ags_fifoout_set_audio(AgsSoundcard *soundcard,
		      GList *audio)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);
  fifoout->audio = audio;
}

GList*
ags_fifoout_get_audio(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);

  return(fifoout->audio);
}

/**
 * ags_fifoout_adjust_delay_and_attack:
 * @fifoout: the #AgsFifoout
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 0.7.65
 */
void
ags_fifoout_adjust_delay_and_attack(AgsFifoout *fifoout)
{
  gdouble delay;
  guint default_tact_frames;
  guint default_period;
  guint i;

  if(fifoout == NULL){
    return;
  }
  
  delay = (60.0 * (((gdouble) fifoout->samplerate / (gdouble) fifoout->buffer_size) / (gdouble) fifoout->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) fifoout->delay_factor)));

#ifdef AGS_DEBUG
  g_message("delay : %f\0", delay);
#endif
  
  default_tact_frames = (guint) (delay * fifoout->buffer_size);
  default_period = (1.0 / AGS_SOUNDCARD_DEFAULT_PERIOD) * (default_tact_frames);

  fifoout->attack[0] = 0;
  fifoout->delay[0] = delay;
  
  for(i = 1; i < (int)  2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    fifoout->attack[i] = (guint) ((i * default_tact_frames + fifoout->attack[i - 1]) / (AGS_SOUNDCARD_DEFAULT_PERIOD / (delay * i))) % (guint) (fifoout->buffer_size);
    
#ifdef AGS_DEBUG
    g_message("%d\0", fifoout->attack[i]);
#endif
  }
  
  for(i = 1; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    fifoout->delay[i] = ((gdouble) (default_tact_frames + fifoout->attack[i])) / (gdouble) fifoout->buffer_size;
    
#ifdef AGS_DEBUG
    g_message("%f\0", fifoout->delay[i]);
#endif
  }
}

/**
 * ags_fifoout_realloc_buffer:
 * @fifoout: the #AgsFifoout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 0.7.65
 */
void
ags_fifoout_realloc_buffer(AgsFifoout *fifoout)
{
  guint word_size;

  if(fifoout == NULL){
    return;
  }

  switch(fifoout->format){
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
    g_warning("ags_fifoout_realloc_buffer(): unsupported word size\0");
    return;
  }
  
  /* AGS_FIFOOUT_BUFFER_0 */
  if(fifoout->buffer[0] != NULL){
    free(fifoout->buffer[0]);
  }
  
  fifoout->buffer[0] = (void *) malloc(fifoout->pcm_channels * fifoout->buffer_size * word_size);
  
  /* AGS_FIFOOUT_BUFFER_1 */
  if(fifoout->buffer[1] != NULL){
    free(fifoout->buffer[1]);
  }

  fifoout->buffer[1] = (void *) malloc(fifoout->pcm_channels * fifoout->buffer_size * word_size);
  
  /* AGS_FIFOOUT_BUFFER_2 */
  if(fifoout->buffer[2] != NULL){
    free(fifoout->buffer[2]);
  }

  fifoout->buffer[2] = (void *) malloc(fifoout->pcm_channels * fifoout->buffer_size * word_size);
  
  /* AGS_FIFOOUT_BUFFER_3 */
  if(fifoout->buffer[3] != NULL){
    free(fifoout->buffer[3]);
  }
  
  fifoout->buffer[3] = (void *) malloc(fifoout->pcm_channels * fifoout->buffer_size * word_size);
}

/**
 * ags_fifoout_new:
 * @application_context: the #AgsApplicationContext
 *
 * Creates an #AgsFifoout, refering to @application_context.
 *
 * Returns: a new #AgsFifoout
 *
 * Since: 0.7.65
 */
AgsFifoout*
ags_fifoout_new(GObject *application_context)
{
  AgsFifoout *fifoout;

  fifoout = (AgsFifoout *) g_object_new(AGS_TYPE_FIFOOUT,
					"application-context\0", application_context,
					NULL);
  
  return(fifoout);
}
