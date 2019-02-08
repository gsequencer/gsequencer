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

#include <ags/audio/jack/ags_jack_devin.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>
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

void ags_jack_devin_class_init(AgsJackDevinClass *jack_devin);
void ags_jack_devin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_jack_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_jack_devin_init(AgsJackDevin *jack_devin);
void ags_jack_devin_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_jack_devin_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_jack_devin_dispose(GObject *gobject);
void ags_jack_devin_finalize(GObject *gobject);

AgsUUID* ags_jack_devin_get_uuid(AgsConnectable *connectable);
gboolean ags_jack_devin_has_resource(AgsConnectable *connectable);
gboolean ags_jack_devin_is_ready(AgsConnectable *connectable);
void ags_jack_devin_add_to_registry(AgsConnectable *connectable);
void ags_jack_devin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_jack_devin_list_resource(AgsConnectable *connectable);
xmlNode* ags_jack_devin_xml_compose(AgsConnectable *connectable);
void ags_jack_devin_xml_parse(AgsConnectable *connectable,
			      xmlNode *node);
gboolean ags_jack_devin_is_connected(AgsConnectable *connectable);
void ags_jack_devin_connect(AgsConnectable *connectable);
void ags_jack_devin_disconnect(AgsConnectable *connectable);

void ags_jack_devin_set_application_context(AgsSoundcard *soundcard,
					    AgsApplicationContext *application_context);
AgsApplicationContext* ags_jack_devin_get_application_context(AgsSoundcard *soundcard);

void ags_jack_devin_set_device(AgsSoundcard *soundcard,
			       gchar *device);
gchar* ags_jack_devin_get_device(AgsSoundcard *soundcard);

void ags_jack_devin_set_presets(AgsSoundcard *soundcard,
				guint channels,
				guint rate,
				guint buffer_size,
				guint format);
void ags_jack_devin_get_presets(AgsSoundcard *soundcard,
				guint *channels,
				guint *rate,
				guint *buffer_size,
				guint *format);

void ags_jack_devin_list_cards(AgsSoundcard *soundcard,
			       GList **card_id, GList **card_name);
void ags_jack_devin_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			     guint *channels_min, guint *channels_max,
			     guint *rate_min, guint *rate_max,
			     guint *buffer_size_min, guint *buffer_size_max,
			     GError **error);
guint ags_jack_devin_get_capability(AgsSoundcard *soundcard);

gboolean ags_jack_devin_is_starting(AgsSoundcard *soundcard);
gboolean ags_jack_devin_is_recording(AgsSoundcard *soundcard);

gchar* ags_jack_devin_get_uptime(AgsSoundcard *soundcard);

void ags_jack_devin_port_init(AgsSoundcard *soundcard,
			      GError **error);
void ags_jack_devin_port_record(AgsSoundcard *soundcard,
				GError **error);
void ags_jack_devin_port_free(AgsSoundcard *soundcard);

void ags_jack_devin_tic(AgsSoundcard *soundcard);
void ags_jack_devin_offset_changed(AgsSoundcard *soundcard,
				   guint note_offset);

void ags_jack_devin_set_bpm(AgsSoundcard *soundcard,
			    gdouble bpm);
gdouble ags_jack_devin_get_bpm(AgsSoundcard *soundcard);

void ags_jack_devin_set_delay_factor(AgsSoundcard *soundcard,
				     gdouble delay_factor);
gdouble ags_jack_devin_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_jack_devin_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_jack_devin_get_delay(AgsSoundcard *soundcard);
guint ags_jack_devin_get_attack(AgsSoundcard *soundcard);

void* ags_jack_devin_get_buffer(AgsSoundcard *soundcard);
void* ags_jack_devin_get_next_buffer(AgsSoundcard *soundcard);
void* ags_jack_devin_get_prev_buffer(AgsSoundcard *soundcard);

void ags_jack_devin_lock_buffer(AgsSoundcard *soundcard,
				void *buffer);
void ags_jack_devin_unlock_buffer(AgsSoundcard *soundcard,
				  void *buffer);

guint ags_jack_devin_get_delay_counter(AgsSoundcard *soundcard);

void ags_jack_devin_set_start_note_offset(AgsSoundcard *soundcard,
					  guint start_note_offset);
guint ags_jack_devin_get_start_note_offset(AgsSoundcard *soundcard);

void ags_jack_devin_set_note_offset(AgsSoundcard *soundcard,
				    guint note_offset);
guint ags_jack_devin_get_note_offset(AgsSoundcard *soundcard);

void ags_jack_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
					     guint note_offset);
guint ags_jack_devin_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_jack_devin_set_loop(AgsSoundcard *soundcard,
			     guint loop_left, guint loop_right,
			     gboolean do_loop);
void ags_jack_devin_get_loop(AgsSoundcard *soundcard,
			     guint *loop_left, guint *loop_right,
			     gboolean *do_loop);

guint ags_jack_devin_get_loop_offset(AgsSoundcard *soundcard);

/**
 * SECTION:ags_jack_devin
 * @short_description: Input from soundcard
 * @title: AgsJackDevin
 * @section_id:
 * @include: ags/audio/jack/ags_jack_devin.h
 *
 * #AgsJackDevin represents a soundcard and supports input.
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
  PROP_JACK_CLIENT,
  PROP_JACK_PORT,
  PROP_CHANNEL,
};

static gpointer ags_jack_devin_parent_class = NULL;

static pthread_mutex_t ags_jack_devin_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_jack_devin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_jack_devin = 0;

    static const GTypeInfo ags_jack_devin_info = {
      sizeof (AgsJackDevinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_devin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsJackDevin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_jack_devin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_jack_devin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_jack_devin_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_jack_devin = g_type_register_static(G_TYPE_OBJECT,
						 "AgsJackDevin",
						 &ags_jack_devin_info,
						 0);

    g_type_add_interface_static(ags_type_jack_devin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_jack_devin,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_jack_devin);
  }

  return g_define_type_id__volatile;
}

void
ags_jack_devin_class_init(AgsJackDevinClass *jack_devin)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_jack_devin_parent_class = g_type_class_peek_parent(jack_devin);

  /* GObjectClass */
  gobject = (GObjectClass *) jack_devin;

  gobject->set_property = ags_jack_devin_set_property;
  gobject->get_property = ags_jack_devin_get_property;

  gobject->dispose = ags_jack_devin_dispose;
  gobject->finalize = ags_jack_devin_finalize;

  /* properties */
  /**
   * AgsJackDevin:application-context:
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
   * AgsJackDevin:device:
   *
   * The jack soundcard indentifier
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-jack-devin-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsJackDevin:dsp-channels:
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
   * AgsJackDevin:pcm-channels:
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
   * AgsJackDevin:format:
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
   * AgsJackDevin:buffer-size:
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
   * AgsJackDevin:samplerate:
   *
   * The samplerate
   * 
   * Since: 2.0.0
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
   * AgsJackDevin:buffer:
   *
   * The buffer
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to record"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsJackDevin:bpm:
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
   * AgsJackDevin:delay-factor:
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
   * AgsJackDevin:attack:
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
   * AgsJackDevin:jack-client:
   *
   * The assigned #AgsJackClient
   * 
   * Since: 2.0.0
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
   * AgsJackDevin:jack-port:
   *
   * The assigned #AgsJackPort
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_pointer("jack-port",
				    i18n_pspec("jack port object"),
				    i18n_pspec("The jack port object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_JACK_PORT,
				  param_spec);
}

GQuark
ags_jack_devin_error_quark()
{
  return(g_quark_from_static_string("ags-jack_devin-error-quark"));
}

void
ags_jack_devin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_jack_devin_get_uuid;
  connectable->has_resource = ags_jack_devin_has_resource;

  connectable->is_ready = ags_jack_devin_is_ready;
  connectable->add_to_registry = ags_jack_devin_add_to_registry;
  connectable->remove_from_registry = ags_jack_devin_remove_from_registry;

  connectable->list_resource = ags_jack_devin_list_resource;
  connectable->xml_compose = ags_jack_devin_xml_compose;
  connectable->xml_parse = ags_jack_devin_xml_parse;

  connectable->is_connected = ags_jack_devin_is_connected;  
  connectable->connect = ags_jack_devin_connect;
  connectable->disconnect = ags_jack_devin_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_jack_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_application_context = ags_jack_devin_set_application_context;
  soundcard->get_application_context = ags_jack_devin_get_application_context;

  soundcard->set_device = ags_jack_devin_set_device;
  soundcard->get_device = ags_jack_devin_get_device;
  
  soundcard->set_presets = ags_jack_devin_set_presets;
  soundcard->get_presets = ags_jack_devin_get_presets;

  soundcard->list_cards = ags_jack_devin_list_cards;
  soundcard->pcm_info = ags_jack_devin_pcm_info;
  soundcard->get_capability = ags_jack_devin_get_capability;

  soundcard->get_poll_fd = NULL;
  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_jack_devin_is_starting;
  soundcard->is_playing = NULL;
  soundcard->is_recording = ags_jack_devin_is_recording;

  soundcard->get_uptime = ags_jack_devin_get_uptime;
  
  soundcard->play_init = NULL;
  soundcard->play = NULL;

  soundcard->record_init = ags_jack_devin_port_init;
  soundcard->record = ags_jack_devin_port_record;

  soundcard->stop = ags_jack_devin_port_free;

  soundcard->tic = ags_jack_devin_tic;
  soundcard->offset_changed = ags_jack_devin_offset_changed;
    
  soundcard->set_bpm = ags_jack_devin_set_bpm;
  soundcard->get_bpm = ags_jack_devin_get_bpm;

  soundcard->set_delay_factor = ags_jack_devin_set_delay_factor;
  soundcard->get_delay_factor = ags_jack_devin_get_delay_factor;
  
  soundcard->get_absolute_delay = ags_jack_devin_get_absolute_delay;

  soundcard->get_delay = ags_jack_devin_get_delay;
  soundcard->get_attack = ags_jack_devin_get_attack;

  soundcard->get_buffer = ags_jack_devin_get_buffer;
  soundcard->get_next_buffer = ags_jack_devin_get_next_buffer;
  soundcard->get_prev_buffer = ags_jack_devin_get_prev_buffer;

  soundcard->lock_buffer = ags_jack_devin_lock_buffer;
  soundcard->unlock_buffer = ags_jack_devin_unlock_buffer;

  soundcard->get_delay_counter = ags_jack_devin_get_delay_counter;

  soundcard->set_start_note_offset = ags_jack_devin_set_start_note_offset;
  soundcard->get_start_note_offset = ags_jack_devin_get_start_note_offset;

  soundcard->set_note_offset = ags_jack_devin_set_note_offset;
  soundcard->get_note_offset = ags_jack_devin_get_note_offset;

  soundcard->set_note_offset_absolute = ags_jack_devin_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_jack_devin_get_note_offset_absolute;

  soundcard->set_loop = ags_jack_devin_set_loop;
  soundcard->get_loop = ags_jack_devin_get_loop;

  soundcard->get_loop_offset = ags_jack_devin_get_loop_offset;
}

void
ags_jack_devin_init(AgsJackDevin *jack_devin)
{
  AgsConfig *config;

  gchar *str;
  gchar *segmentation;

  guint denumerator, numerator;
  guint i;
  
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* flags */
  jack_devin->flags = 0;
  g_atomic_int_set(&(jack_devin->sync_flags),
		   AGS_JACK_DEVIN_PASS_THROUGH);

  /* insert devin mutex */
  jack_devin->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  jack_devin->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* parent */
  jack_devin->application_context = NULL;

  /* uuid */
  jack_devin->uuid = ags_uuid_alloc();
  ags_uuid_generate(jack_devin->uuid);

  /* presets */
  config = ags_config_get_instance();

  jack_devin->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  jack_devin->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  jack_devin->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  jack_devin->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  jack_devin->format = ags_soundcard_helper_config_get_format(config);

  /*  */
  jack_devin->card_uri = NULL;
  jack_devin->jack_client = NULL;

  jack_devin->port_name = NULL;
  jack_devin->jack_port = NULL;

  /* buffer */
  jack_devin->buffer_mutex = (pthread_mutex_t **) malloc(4 * sizeof(pthread_mutex_t *));

  for(i = 0; i < 4; i++){
    jack_devin->buffer_mutex[i] = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));

    pthread_mutex_init(jack_devin->buffer_mutex[i],
		       NULL);
  }

  jack_devin->buffer = (void **) malloc(4 * sizeof(void*));

  jack_devin->buffer[0] = (void *) malloc(jack_devin->pcm_channels * jack_devin->buffer_size * sizeof(gint16));
  jack_devin->buffer[1] = (void *) malloc(jack_devin->pcm_channels * jack_devin->buffer_size * sizeof(gint16));
  jack_devin->buffer[2] = (void *) malloc(jack_devin->pcm_channels * jack_devin->buffer_size * sizeof(gint16));
  jack_devin->buffer[3] = (void *) malloc(jack_devin->pcm_channels * jack_devin->buffer_size * sizeof(gint16));
  
  ags_jack_devin_realloc_buffer(jack_devin);
  
  /* bpm */
  jack_devin->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  jack_devin->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denumerator,
	   &numerator);
    
    jack_devin->delay_factor = 1.0 / numerator * (numerator / denumerator);

    g_free(segmentation);
  }

  /* delay and attack */
  jack_devin->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					 sizeof(gdouble));
  
  jack_devin->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					sizeof(guint));

  ags_jack_devin_adjust_delay_and_attack(jack_devin);
  
  /* counters */
  jack_devin->tact_counter = 0.0;
  jack_devin->delay_counter = 0;
  jack_devin->tic_counter = 0;

  jack_devin->start_note_offset = 0;
  jack_devin->note_offset = 0;
  jack_devin->note_offset_absolute = 0;

  jack_devin->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  jack_devin->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  jack_devin->do_loop = FALSE;

  jack_devin->loop_offset = 0;

  /* callback mutex */
  jack_devin->callback_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(jack_devin->callback_mutex,
		     NULL);

  jack_devin->callback_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(jack_devin->callback_cond, NULL);

  /* callback finish mutex */
  jack_devin->callback_finish_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(jack_devin->callback_finish_mutex,
		     NULL);

  jack_devin->callback_finish_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(jack_devin->callback_finish_cond, NULL);

  /*  */
  jack_devin->notify_soundcard = NULL;
}

void
ags_jack_devin_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsJackDevin *jack_devin;

  pthread_mutex_t *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(gobject);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      pthread_mutex_lock(jack_devin_mutex);

      if(jack_devin->application_context == application_context){
	pthread_mutex_unlock(jack_devin_mutex);

	return;
      }

      if(jack_devin->application_context != NULL){
	g_object_unref(G_OBJECT(jack_devin->application_context));
      }

      if(application_context != NULL){	
	g_object_ref(G_OBJECT(application_context));
      }

      jack_devin->application_context = application_context;

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      pthread_mutex_lock(jack_devin_mutex);

      jack_devin->card_uri = g_strdup(device);

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      pthread_mutex_lock(jack_devin_mutex);

      if(dsp_channels == jack_devin->dsp_channels){
	pthread_mutex_unlock(jack_devin_mutex);

	return;
      }

      jack_devin->dsp_channels = dsp_channels;

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      pthread_mutex_lock(jack_devin_mutex);

      if(pcm_channels == jack_devin->pcm_channels){
	pthread_mutex_unlock(jack_devin_mutex);

	return;
      }

      jack_devin->pcm_channels = pcm_channels;

      pthread_mutex_unlock(jack_devin_mutex);

      ags_jack_devin_realloc_buffer(jack_devin);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      pthread_mutex_lock(jack_devin_mutex);

      if(format == jack_devin->format){
	pthread_mutex_unlock(jack_devin_mutex);

	return;
      }

      jack_devin->format = format;

      pthread_mutex_unlock(jack_devin_mutex);

      ags_jack_devin_realloc_buffer(jack_devin);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      pthread_mutex_lock(jack_devin_mutex);

      if(buffer_size == jack_devin->buffer_size){
	pthread_mutex_unlock(jack_devin_mutex);

	return;
      }

      jack_devin->buffer_size = buffer_size;

      pthread_mutex_unlock(jack_devin_mutex);

      ags_jack_devin_realloc_buffer(jack_devin);
      ags_jack_devin_adjust_delay_and_attack(jack_devin);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      pthread_mutex_lock(jack_devin_mutex);
      
      if(samplerate == jack_devin->samplerate){
	pthread_mutex_unlock(jack_devin_mutex);

	return;
      }

      jack_devin->samplerate = samplerate;

      pthread_mutex_unlock(jack_devin_mutex);

      ags_jack_devin_realloc_buffer(jack_devin);
      ags_jack_devin_adjust_delay_and_attack(jack_devin);
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

      pthread_mutex_lock(jack_devin_mutex);

      jack_devin->bpm = bpm;

      pthread_mutex_unlock(jack_devin_mutex);

      ags_jack_devin_adjust_delay_and_attack(jack_devin);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      pthread_mutex_lock(jack_devin_mutex);

      jack_devin->delay_factor = delay_factor;

      pthread_mutex_unlock(jack_devin_mutex);

      ags_jack_devin_adjust_delay_and_attack(jack_devin);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      AgsJackClient *jack_client;

      jack_client = (AgsJackClient *) g_value_get_object(value);

      pthread_mutex_lock(jack_devin_mutex);

      if(jack_devin->jack_client == (GObject *) jack_client){
	pthread_mutex_unlock(jack_devin_mutex);

	return;
      }

      if(jack_devin->jack_client != NULL){
	g_object_unref(G_OBJECT(jack_devin->jack_client));
      }

      if(jack_client != NULL){
	g_object_ref(jack_client);
      }
      
      jack_devin->jack_client = (GObject *) jack_client;

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_JACK_PORT:
    {
      AgsJackPort *jack_port;

      jack_port = (AgsJackPort *) g_value_get_pointer(value);

      pthread_mutex_lock(jack_devin_mutex);

      if(!AGS_IS_JACK_PORT(jack_port) ||
	 g_list_find(jack_devin->jack_port, jack_port) != NULL){
	pthread_mutex_unlock(jack_devin_mutex);

	return;
      }

      g_object_ref(jack_port);
      jack_devin->jack_port = g_list_append(jack_devin->jack_port,
					    jack_port);

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_devin_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsJackDevin *jack_devin;

  pthread_mutex_t *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(gobject);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      pthread_mutex_lock(jack_devin_mutex);

      g_value_set_object(value, jack_devin->application_context);

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      pthread_mutex_lock(jack_devin_mutex);

      g_value_set_string(value, jack_devin->card_uri);

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      pthread_mutex_lock(jack_devin_mutex);

      g_value_set_uint(value, jack_devin->dsp_channels);

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      pthread_mutex_lock(jack_devin_mutex);

      g_value_set_uint(value, jack_devin->pcm_channels);

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      pthread_mutex_lock(jack_devin_mutex);

      g_value_set_uint(value, jack_devin->format);

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      pthread_mutex_lock(jack_devin_mutex);

      g_value_set_uint(value, jack_devin->buffer_size);

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(jack_devin_mutex);

      g_value_set_uint(value, jack_devin->samplerate);

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      pthread_mutex_lock(jack_devin_mutex);

      g_value_set_pointer(value, jack_devin->buffer);

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_BPM:
    {
      pthread_mutex_lock(jack_devin_mutex);

      g_value_set_double(value, jack_devin->bpm);

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      pthread_mutex_lock(jack_devin_mutex);

      g_value_set_double(value, jack_devin->delay_factor);

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_ATTACK:
    {
      pthread_mutex_lock(jack_devin_mutex);

      g_value_set_pointer(value, jack_devin->attack);

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      pthread_mutex_lock(jack_devin_mutex);

      g_value_set_object(value, jack_devin->jack_client);

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_JACK_PORT:
    {
      pthread_mutex_lock(jack_devin_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(jack_devin->jack_port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      pthread_mutex_unlock(jack_devin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_devin_dispose(GObject *gobject)
{
  AgsJackDevin *jack_devin;

  jack_devin = AGS_JACK_DEVIN(gobject);

  /* application context */
  if(jack_devin->application_context != NULL){
    g_object_unref(jack_devin->application_context);

    jack_devin->application_context = NULL;
  }

  /* jack client */
  if(jack_devin->jack_client != NULL){
    g_object_unref(jack_devin->jack_client);

    jack_devin->jack_client = NULL;
  }

  /* jack port */
  g_list_free_full(jack_devin->jack_port,
		   g_object_unref);

  jack_devin->jack_port = NULL;

  /* notify soundcard */
  if(jack_devin->notify_soundcard != NULL){
    AgsTaskThread *task_thread;

    task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(ags_application_context_get_instance()));
      
    ags_task_thread_remove_cyclic_task(task_thread,
				       (AgsTask *) jack_devin->notify_soundcard);
    
    g_object_unref(jack_devin->notify_soundcard);

    jack_devin->notify_soundcard = NULL;
  }

  /* call parent */
  G_OBJECT_CLASS(ags_jack_devin_parent_class)->dispose(gobject);
}

void
ags_jack_devin_finalize(GObject *gobject)
{
  AgsJackDevin *jack_devin;

  jack_devin = AGS_JACK_DEVIN(gobject);

  pthread_mutex_destroy(jack_devin->obj_mutex);
  free(jack_devin->obj_mutex);

  pthread_mutexattr_destroy(jack_devin->obj_mutexattr);
  free(jack_devin->obj_mutexattr);

  /* free output buffer */
  free(jack_devin->buffer[0]);
  free(jack_devin->buffer[1]);
  free(jack_devin->buffer[2]);
  free(jack_devin->buffer[3]);

  /* free buffer array */
  free(jack_devin->buffer);

  /* free AgsAttack */
  free(jack_devin->attack);

  /* unref notify soundcard */
  if(jack_devin->notify_soundcard != NULL){
    AgsTaskThread *task_thread;

    task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(ags_application_context_get_instance()));

    ags_task_thread_remove_cyclic_task(task_thread,
				       (AgsTask *) jack_devin->notify_soundcard);
    
    g_object_unref(jack_devin->notify_soundcard);
  }

  /* jack client */
  if(jack_devin->jack_client != NULL){
    g_object_unref(jack_devin->jack_client);
  }
  
  /* jack port */
  g_list_free_full(jack_devin->jack_port,
		   g_object_unref);

  /* application context */
  if(jack_devin->application_context != NULL){
    g_object_unref(jack_devin->application_context);
  }  

  /* call parent */
  G_OBJECT_CLASS(ags_jack_devin_parent_class)->finalize(gobject);
}

AgsUUID*
ags_jack_devin_get_uuid(AgsConnectable *connectable)
{
  AgsJackDevin *jack_devin;
  
  AgsUUID *ptr;

  pthread_mutex_t *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(connectable);

  /* get jack devin signal mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(jack_devin_mutex);

  ptr = jack_devin->uuid;

  pthread_mutex_unlock(jack_devin_mutex);
  
  return(ptr);
}

gboolean
ags_jack_devin_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_jack_devin_is_ready(AgsConnectable *connectable)
{
  AgsJackDevin *jack_devin;
  
  gboolean is_ready;

  pthread_mutex_t *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(connectable);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* check is added */
  pthread_mutex_lock(jack_devin_mutex);

  is_ready = (((AGS_JACK_DEVIN_ADDED_TO_REGISTRY & (jack_devin->flags)) != 0) ? TRUE: FALSE);

  pthread_mutex_unlock(jack_devin_mutex);
  
  return(is_ready);
}

void
ags_jack_devin_add_to_registry(AgsConnectable *connectable)
{
  AgsJackDevin *jack_devin;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  jack_devin = AGS_JACK_DEVIN(connectable);

  ags_jack_devin_set_flags(jack_devin, AGS_JACK_DEVIN_ADDED_TO_REGISTRY);
}

void
ags_jack_devin_remove_from_registry(AgsConnectable *connectable)
{
  AgsJackDevin *jack_devin;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  jack_devin = AGS_JACK_DEVIN(connectable);

  ags_jack_devin_unset_flags(jack_devin, AGS_JACK_DEVIN_ADDED_TO_REGISTRY);
}

xmlNode*
ags_jack_devin_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_jack_devin_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_jack_devin_xml_parse(AgsConnectable *connectable,
			 xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_jack_devin_is_connected(AgsConnectable *connectable)
{
  AgsJackDevin *jack_devin;
  
  gboolean is_connected;

  pthread_mutex_t *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(connectable);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(jack_devin_mutex);

  is_connected = (((AGS_JACK_DEVIN_CONNECTED & (jack_devin->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(jack_devin_mutex);
  
  return(is_connected);
}

void
ags_jack_devin_connect(AgsConnectable *connectable)
{
  AgsJackDevin *jack_devin;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  jack_devin = AGS_JACK_DEVIN(connectable);

  ags_jack_devin_set_flags(jack_devin, AGS_JACK_DEVIN_CONNECTED);
}

void
ags_jack_devin_disconnect(AgsConnectable *connectable)
{

  AgsJackDevin *jack_devin;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  jack_devin = AGS_JACK_DEVIN(connectable);
  
  ags_jack_devin_unset_flags(jack_devin, AGS_JACK_DEVIN_CONNECTED);
}

/**
 * ags_jack_devin_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_jack_devin_get_class_mutex()
{
  return(&ags_jack_devin_class_mutex);
}

/**
 * ags_jack_devin_test_flags:
 * @jack_devin: the #AgsJackDevin
 * @flags: the flags
 *
 * Test @flags to be set on @jack_devin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_jack_devin_test_flags(AgsJackDevin *jack_devin, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *jack_devin_mutex;

  if(!AGS_IS_JACK_DEVIN(jack_devin)){
    return(FALSE);
  }

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* test */
  pthread_mutex_lock(jack_devin_mutex);

  retval = (flags & (jack_devin->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(jack_devin_mutex);

  return(retval);
}

/**
 * ags_jack_devin_set_flags:
 * @jack_devin: the #AgsJackDevin
 * @flags: see #AgsJackDevinFlags-enum
 *
 * Enable a feature of @jack_devin.
 *
 * Since: 2.0.0
 */
void
ags_jack_devin_set_flags(AgsJackDevin *jack_devin, guint flags)
{
  pthread_mutex_t *jack_devin_mutex;

  if(!AGS_IS_JACK_DEVIN(jack_devin)){
    return;
  }

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(jack_devin_mutex);

  jack_devin->flags |= flags;
  
  pthread_mutex_unlock(jack_devin_mutex);
}
    
/**
 * ags_jack_devin_unset_flags:
 * @jack_devin: the #AgsJackDevin
 * @flags: see #AgsJackDevinFlags-enum
 *
 * Disable a feature of @jack_devin.
 *
 * Since: 2.0.0
 */
void
ags_jack_devin_unset_flags(AgsJackDevin *jack_devin, guint flags)
{  
  pthread_mutex_t *jack_devin_mutex;

  if(!AGS_IS_JACK_DEVIN(jack_devin)){
    return;
  }

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(jack_devin_mutex);

  jack_devin->flags &= (~flags);
  
  pthread_mutex_unlock(jack_devin_mutex);
}

void
ags_jack_devin_set_application_context(AgsSoundcard *soundcard,
				       AgsApplicationContext *application_context)
{
  AgsJackDevin *jack_devin;

  pthread_mutex_t *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* set application context */
  pthread_mutex_lock(jack_devin_mutex);
  
  jack_devin->application_context = (GObject *) application_context;
  
  pthread_mutex_unlock(jack_devin_mutex);
}

AgsApplicationContext*
ags_jack_devin_get_application_context(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  AgsApplicationContext *application_context;
  
  pthread_mutex_t *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* get application context */
  pthread_mutex_lock(jack_devin_mutex);

  application_context = (AgsApplicationContext *) jack_devin->application_context;

  pthread_mutex_unlock(jack_devin_mutex);
  
  return(application_context);
}

void
ags_jack_devin_set_device(AgsSoundcard *soundcard,
			  gchar *device)
{
  AgsJackDevin *jack_devin;

  GList *jack_port, *jack_port_start;

  gchar *str;

  guint pcm_channels;
  int ret;
  guint nth_card;
  guint i;
  
  pthread_mutex_t *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* check device */
  pthread_mutex_lock(jack_devin_mutex);

  if(jack_devin->card_uri == device ||
     !g_ascii_strcasecmp(jack_devin->card_uri,
			 device)){
    pthread_mutex_unlock(jack_devin_mutex);
  
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-jack-devin-")){
    pthread_mutex_unlock(jack_devin_mutex);

    g_warning("invalid JACK device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-jack-devin-%u",
	       &nth_card);

  if(ret != 1){
    pthread_mutex_unlock(jack_devin_mutex);

    g_warning("invalid JACK device specifier");

    return;
  }

  g_free(jack_devin->card_uri);
  jack_devin->card_uri = g_strdup(device);

  /* apply name to port */
  pthread_mutex_unlock(jack_devin_mutex);
  
#if 0
  pcm_channels = jack_devin->pcm_channels;
  
  jack_port_start = 
    jack_port = g_list_copy(jack_devin->jack_port);
  
  for(i = 0; i < pcm_channels && jack_port != NULL; i++){
    str = g_strdup_printf("ags%d-%04d",
			  nth_card,
			  i);
    
    g_object_set(jack_port->data,
		 "port-name", str,
		 NULL);
    g_free(str);

    jack_port = jack_port->next;
  }

  g_list_free(jack_port_start);
#endif
}

gchar*
ags_jack_devin_get_device(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;
  
  gchar *device;

  pthread_mutex_t *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  device = NULL;

  /* get device */
  pthread_mutex_lock(jack_devin_mutex);

  device = g_strdup(jack_devin->card_uri);

  pthread_mutex_unlock(jack_devin_mutex);
  
  return(device);
}

void
ags_jack_devin_set_presets(AgsSoundcard *soundcard,
			   guint channels,
			   guint rate,
			   guint buffer_size,
			   guint format)
{
  AgsJackDevin *jack_devin;

  jack_devin = AGS_JACK_DEVIN(soundcard);

  g_object_set(jack_devin,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_jack_devin_get_presets(AgsSoundcard *soundcard,
			   guint *channels,
			   guint *rate,
			   guint *buffer_size,
			   guint *format)
{
  AgsJackDevin *jack_devin;

  pthread_mutex_t *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* get presets */
  pthread_mutex_lock(jack_devin_mutex);

  if(channels != NULL){
    *channels = jack_devin->pcm_channels;
  }

  if(rate != NULL){
    *rate = jack_devin->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = jack_devin->buffer_size;
  }

  if(format != NULL){
    *format = jack_devin->format;
  }

  pthread_mutex_unlock(jack_devin_mutex);
}

/**
 * ags_jack_devin_list_cards:
 * @soundcard: the #AgsSoundcard
 * @card_id: JACK identifier
 * @card_name: card name
 *
 * List available soundcards.
 *
 * Since: 2.0.0
 */
void
ags_jack_devin_list_cards(AgsSoundcard *soundcard,
			  GList **card_id, GList **card_name)
{
  AgsJackClient *jack_client;
  AgsJackDevin *jack_devin;

  AgsApplicationContext *application_context;
  
  GList *list_start, *list;

  gchar *card_uri;
  gchar *client_name;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  application_context = ags_application_context_get_instance();
  
  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  list =
    list_start = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  while(list != NULL){
    if(AGS_IS_JACK_DEVIN(list->data)){
      if(card_id != NULL){
	card_uri = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));
	
	if(AGS_JACK_DEVIN(list->data)->card_uri != NULL){
	  *card_id = g_list_prepend(*card_id,
				    card_uri);
	}else{
	  *card_id = g_list_prepend(*card_id,
				    g_strdup("(null)"));

	  g_warning("ags_jack_devin_list_cards() - card id (null)");
	}
      }

      if(card_name != NULL){
	g_object_get(list->data,
		     "jack-client", &jack_client,
		     NULL);
	
	if(jack_client != NULL){
	  /* get client name */
	  g_object_get(jack_client,
		       "client-name", &client_name,
		       NULL);
	  
	  *card_name = g_list_prepend(*card_name,
				      client_name);

	  g_object_unref(jack_client);
	}else{
	  *card_name = g_list_prepend(*card_name,
				      g_strdup("(null)"));

	  g_warning("ags_jack_devin_list_cards() - JACK client not connected (null)");
	}
      }      
    }

    list = list->next;
  }
  
  if(card_id != NULL && *card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL && *card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

void
ags_jack_devin_pcm_info(AgsSoundcard *soundcard,
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

guint
ags_jack_devin_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_CAPTURE);
}

gboolean
ags_jack_devin_is_starting(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  gboolean is_starting;
  
  pthread_mutex_t *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* check is starting */
  pthread_mutex_lock(jack_devin_mutex);

  is_starting = ((AGS_JACK_DEVIN_START_RECORD & (jack_devin->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(jack_devin_mutex);
  
  return(is_starting);
}

gboolean
ags_jack_devin_is_recording(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  gboolean is_playing;
  
  pthread_mutex_t *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* check is starting */
  pthread_mutex_lock(jack_devin_mutex);

  is_playing = ((AGS_JACK_DEVIN_RECORD & (jack_devin->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(jack_devin_mutex);

  return(is_playing);
}

gchar*
ags_jack_devin_get_uptime(AgsSoundcard *soundcard)
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
ags_jack_devin_port_init(AgsSoundcard *soundcard,
			 GError **error)
{
  AgsJackDevin *jack_devin;

  guint format, word_size;
  
  pthread_mutex_t *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* retrieve word size */
  pthread_mutex_lock(jack_devin_mutex);

  switch(jack_devin->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      word_size = sizeof(gint8);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      word_size = sizeof(gint16);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {      
      //NOTE:JK: The 24-bit linear samples use 32-bit physical space
      word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      word_size = sizeof(gint64);
    }
    break;
  default:
    pthread_mutex_unlock(jack_devin_mutex);
    
    g_warning("ags_jack_devin_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for capture */
  jack_devin->flags |= (AGS_JACK_DEVIN_BUFFER3 |
			AGS_JACK_DEVIN_START_RECORD |
			AGS_JACK_DEVIN_RECORD |
			AGS_JACK_DEVIN_NONBLOCKING);

  memset(jack_devin->buffer[0], 0, jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  memset(jack_devin->buffer[1], 0, jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  memset(jack_devin->buffer[2], 0, jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  memset(jack_devin->buffer[3], 0, jack_devin->pcm_channels * jack_devin->buffer_size * word_size);

  /*  */
  jack_devin->tact_counter = 0.0;
  jack_devin->delay_counter = 0.0;
  jack_devin->tic_counter = 0;

  jack_devin->flags |= (AGS_JACK_DEVIN_INITIALIZED |
			AGS_JACK_DEVIN_START_RECORD |
			AGS_JACK_DEVIN_RECORD);
  
  g_atomic_int_and(&(jack_devin->sync_flags),
		   (~(AGS_JACK_DEVIN_PASS_THROUGH)));
  g_atomic_int_or(&(jack_devin->sync_flags),
		  AGS_JACK_DEVIN_INITIAL_CALLBACK);

  pthread_mutex_unlock(jack_devin_mutex);
}

void
ags_jack_devin_port_record(AgsSoundcard *soundcard,
			   GError **error)
{
  AgsJackClient *jack_client;
  AgsJackDevin *jack_devin;

  AgsNotifySoundcard *notify_soundcard;
  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
        
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  GList *task;

  guint word_size;
  gboolean jack_client_activated;
  
  pthread_mutex_t *jack_devin_mutex;
  pthread_mutex_t *jack_client_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  application_context = ags_application_context_get_instance();

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* client */
  pthread_mutex_lock(jack_devin_mutex);

  jack_client = (AgsJackClient *) jack_devin->jack_client;
  
  callback_mutex = jack_devin->callback_mutex;
  callback_finish_mutex = jack_devin->callback_finish_mutex;

  pthread_mutex_unlock(jack_devin_mutex);

  /* do capture */
  pthread_mutex_lock(jack_devin_mutex);
  
  jack_devin->flags &= (~AGS_JACK_DEVIN_START_RECORD);
  notify_soundcard = AGS_NOTIFY_SOUNDCARD(jack_devin->notify_soundcard);
  
  if((AGS_JACK_DEVIN_INITIALIZED & (jack_devin->flags)) == 0){
    pthread_mutex_unlock(jack_devin_mutex);
    
    return;
  }

  switch(jack_devin->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      word_size = sizeof(gint8);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      word_size = sizeof(gint16);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {      
      //NOTE:JK: The 24-bit linear samples use 32-bit physical space
      word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      word_size = sizeof(gint32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      word_size = sizeof(gint64);
    }
    break;
  default:
    pthread_mutex_unlock(jack_devin_mutex);
    
    g_warning("ags_jack_devin_port_record(): unsupported word size");
    
    return;
  }

  pthread_mutex_unlock(jack_devin_mutex);

  /* get client mutex */
  pthread_mutex_lock(ags_jack_client_get_class_mutex());
  
  jack_client_mutex = jack_client->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_client_get_class_mutex());

  /* get activated */
  pthread_mutex_lock(jack_client_mutex);

  jack_client_activated = ((AGS_JACK_CLIENT_ACTIVATED & (jack_client->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(jack_client_mutex);

  if(jack_client_activated){
    /* signal */
    if((AGS_JACK_DEVIN_INITIAL_CALLBACK & (g_atomic_int_get(&(jack_devin->sync_flags)))) == 0){
      pthread_mutex_lock(callback_mutex);

      g_atomic_int_or(&(jack_devin->sync_flags),
		      AGS_JACK_DEVIN_CALLBACK_DONE);
    
      if((AGS_JACK_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(jack_devin->sync_flags)))) != 0){
	pthread_cond_signal(jack_devin->callback_cond);
      }

      pthread_mutex_unlock(callback_mutex);
    }
    
    /* wait callback */	
    if((AGS_JACK_DEVIN_INITIAL_CALLBACK & (g_atomic_int_get(&(jack_devin->sync_flags)))) == 0){
      pthread_mutex_lock(callback_finish_mutex);
    
      if((AGS_JACK_DEVIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(jack_devin->sync_flags)))) == 0){
	g_atomic_int_or(&(jack_devin->sync_flags),
			AGS_JACK_DEVIN_CALLBACK_FINISH_WAIT);
    
	while((AGS_JACK_DEVIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(jack_devin->sync_flags)))) == 0 &&
	      (AGS_JACK_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_devin->sync_flags)))) != 0){
	  pthread_cond_wait(jack_devin->callback_finish_cond,
			    callback_finish_mutex);
	}
      }
    
      g_atomic_int_and(&(jack_devin->sync_flags),
		       (~(AGS_JACK_DEVIN_CALLBACK_FINISH_WAIT |
			  AGS_JACK_DEVIN_CALLBACK_FINISH_DONE)));
    
      pthread_mutex_unlock(callback_finish_mutex);
    }else{
      g_atomic_int_and(&(jack_devin->sync_flags),
		       (~AGS_JACK_DEVIN_INITIAL_CALLBACK));
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

  /* update soundcard */
  task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;
    
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) jack_devin);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) jack_devin);
  task = g_list_append(task,
		       clear_buffer);
    
  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) jack_devin);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_thread_append_tasks((AgsTaskThread *) task_thread,
			       task);
}

void
ags_jack_devin_port_free(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  AgsNotifySoundcard *notify_soundcard;
  
  AgsApplicationContext *application_context;

  guint word_size;

  pthread_mutex_t *jack_devin_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /*  */
  pthread_mutex_lock(jack_devin_mutex);

  notify_soundcard = AGS_NOTIFY_SOUNDCARD(jack_devin->notify_soundcard);

  if((AGS_JACK_DEVIN_INITIALIZED & (jack_devin->flags)) == 0){
    pthread_mutex_unlock(jack_devin_mutex);

    return;
  }

  g_object_ref(notify_soundcard);
  
  //  g_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);

  callback_mutex = jack_devin->callback_mutex;
  callback_finish_mutex = jack_devin->callback_finish_mutex;
  
  jack_devin->flags &= (~(AGS_JACK_DEVIN_BUFFER0 |
			  AGS_JACK_DEVIN_BUFFER1 |
			  AGS_JACK_DEVIN_BUFFER2 |
			  AGS_JACK_DEVIN_BUFFER3 |
			  AGS_JACK_DEVIN_RECORD));

  g_atomic_int_or(&(jack_devin->sync_flags),
		  AGS_JACK_DEVIN_PASS_THROUGH);
  g_atomic_int_and(&(jack_devin->sync_flags),
		   (~AGS_JACK_DEVIN_INITIAL_CALLBACK));

  /* signal callback */
  pthread_mutex_lock(callback_mutex);

  g_atomic_int_or(&(jack_devin->sync_flags),
		  AGS_JACK_DEVIN_CALLBACK_DONE);
    
  if((AGS_JACK_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(jack_devin->sync_flags)))) != 0){
    pthread_cond_signal(jack_devin->callback_cond);
  }

  pthread_mutex_unlock(callback_mutex);

  /* signal thread */
  pthread_mutex_lock(callback_finish_mutex);

  g_atomic_int_or(&(jack_devin->sync_flags),
		  AGS_JACK_DEVIN_CALLBACK_FINISH_DONE);
    
  if((AGS_JACK_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_devin->sync_flags)))) != 0){
    pthread_cond_signal(jack_devin->callback_finish_cond);
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
  jack_devin->note_offset = jack_devin->start_note_offset;
  jack_devin->note_offset_absolute = jack_devin->start_note_offset;

  switch(jack_devin->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      word_size = sizeof(gint8);
    }
    break;
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
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      word_size = sizeof(gint64);
    }
    break;
  default:
    word_size = 0;
    
    g_critical("ags_jack_devin_free(): unsupported word size");
  }

  memset(jack_devin->buffer[1], 0, (size_t) jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  memset(jack_devin->buffer[2], 0, (size_t) jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  memset(jack_devin->buffer[3], 0, (size_t) jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  memset(jack_devin->buffer[0], 0, (size_t) jack_devin->pcm_channels * jack_devin->buffer_size * word_size);

  pthread_mutex_unlock(jack_devin_mutex);
}

void
ags_jack_devin_tic(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  gdouble delay;
  gdouble delay_counter;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  
  pthread_mutex_t *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());
  
  /* determine if attack should be switched */
  pthread_mutex_lock(jack_devin_mutex);

  delay = jack_devin->delay[jack_devin->tic_counter];
  delay_counter = jack_devin->delay_counter;

  note_offset = jack_devin->note_offset;
  note_offset_absolute = jack_devin->note_offset_absolute;
  
  loop_left = jack_devin->loop_left;
  loop_right = jack_devin->loop_right;
  
  do_loop = jack_devin->do_loop;

  pthread_mutex_unlock(jack_devin_mutex);

  if((guint) delay_counter + 1 >= (guint) delay){
    if(do_loop &&
       note_offset + 1 == loop_right){
      ags_soundcard_set_note_offset(soundcard,
				    loop_left);
    }else{
      ags_soundcard_set_note_offset(soundcard,
				    note_offset + 1);
    }
    
    ags_soundcard_set_note_offset_absolute(soundcard,
					   note_offset_absolute + 1);

    /* delay */
    ags_soundcard_offset_changed(soundcard,
				 note_offset);
    
    /* reset - delay counter */
    pthread_mutex_lock(jack_devin_mutex);
    
    jack_devin->delay_counter = 0.0;
    jack_devin->tact_counter += 1.0;

    pthread_mutex_unlock(jack_devin_mutex);
  }else{
    pthread_mutex_lock(jack_devin_mutex);
    
    jack_devin->delay_counter += 1.0;

    pthread_mutex_unlock(jack_devin_mutex);
  }
}

void
ags_jack_devin_offset_changed(AgsSoundcard *soundcard,
			      guint note_offset)
{
  AgsJackDevin *jack_devin;
  
  pthread_mutex_t *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* offset changed */
  pthread_mutex_lock(jack_devin_mutex);

  jack_devin->tic_counter += 1;

  if(jack_devin->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    jack_devin->tic_counter = 0;
  }

  pthread_mutex_unlock(jack_devin_mutex);
}

void
ags_jack_devin_set_bpm(AgsSoundcard *soundcard,
		       gdouble bpm)
{
  AgsJackDevin *jack_devin;

  pthread_mutex_t *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* set bpm */
  pthread_mutex_lock(jack_devin_mutex);

  jack_devin->bpm = bpm;

  pthread_mutex_unlock(jack_devin_mutex);

  ags_jack_devin_adjust_delay_and_attack(jack_devin);
}

gdouble
ags_jack_devin_get_bpm(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  gdouble bpm;
  
  pthread_mutex_t *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* get bpm */
  pthread_mutex_lock(jack_devin_mutex);

  bpm = jack_devin->bpm;
  
  pthread_mutex_unlock(jack_devin_mutex);

  return(bpm);
}

void
ags_jack_devin_set_delay_factor(AgsSoundcard *soundcard,
				gdouble delay_factor)
{
  AgsJackDevin *jack_devin;

  pthread_mutex_t *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* set delay factor */
  pthread_mutex_lock(jack_devin_mutex);

  jack_devin->delay_factor = delay_factor;

  pthread_mutex_unlock(jack_devin_mutex);

  ags_jack_devin_adjust_delay_and_attack(jack_devin);
}

gdouble
ags_jack_devin_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  gdouble delay_factor;
  
  pthread_mutex_t *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* get delay factor */
  pthread_mutex_lock(jack_devin_mutex);

  delay_factor = jack_devin->delay_factor;
  
  pthread_mutex_unlock(jack_devin_mutex);

  return(delay_factor);
}

gdouble
ags_jack_devin_get_delay(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint delay_index;
  gdouble delay;
  
  pthread_mutex_t *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* get delay */
  pthread_mutex_lock(jack_devin_mutex);

  delay_index = jack_devin->tic_counter;

  delay = jack_devin->delay[delay_index];
  
  pthread_mutex_unlock(jack_devin_mutex);
  
  return(delay);
}

gdouble
ags_jack_devin_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  gdouble absolute_delay;
  
  pthread_mutex_t *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* get absolute delay */
  pthread_mutex_lock(jack_devin_mutex);

  absolute_delay = (60.0 * (((gdouble) jack_devin->samplerate / (gdouble) jack_devin->buffer_size) / (gdouble) jack_devin->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) jack_devin->delay_factor)));

  pthread_mutex_unlock(jack_devin_mutex);

  return(absolute_delay);
}

guint
ags_jack_devin_get_attack(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint attack_index;
  guint attack;
  
  pthread_mutex_t *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* get attack */
  pthread_mutex_lock(jack_devin_mutex);

  attack_index = jack_devin->tic_counter;

  attack = jack_devin->attack[attack_index];

  pthread_mutex_unlock(jack_devin_mutex);
  
  return(attack);
}

void*
ags_jack_devin_get_buffer(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  void *buffer;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  if(ags_jack_devin_test_flags(jack_devin, AGS_JACK_DEVIN_BUFFER0)){
    buffer = jack_devin->buffer[0];
  }else if(ags_jack_devin_test_flags(jack_devin, AGS_JACK_DEVIN_BUFFER1)){
    buffer = jack_devin->buffer[1];
  }else if(ags_jack_devin_test_flags(jack_devin, AGS_JACK_DEVIN_BUFFER2)){
    buffer = jack_devin->buffer[2];
  }else if(ags_jack_devin_test_flags(jack_devin, AGS_JACK_DEVIN_BUFFER3)){
    buffer = jack_devin->buffer[3];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_jack_devin_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  void *buffer;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  //  g_message("next - 0x%0x", ((AGS_JACK_DEVIN_BUFFER0 |
  //				AGS_JACK_DEVIN_BUFFER1 |
  //				AGS_JACK_DEVIN_BUFFER2 |
  //				AGS_JACK_DEVIN_BUFFER3) & (jack_devin->flags)));

  if(ags_jack_devin_test_flags(jack_devin, AGS_JACK_DEVIN_BUFFER0)){
    buffer = jack_devin->buffer[1];
  }else if(ags_jack_devin_test_flags(jack_devin, AGS_JACK_DEVIN_BUFFER1)){
    buffer = jack_devin->buffer[2];
  }else if(ags_jack_devin_test_flags(jack_devin, AGS_JACK_DEVIN_BUFFER2)){
    buffer = jack_devin->buffer[3];
  }else if(ags_jack_devin_test_flags(jack_devin, AGS_JACK_DEVIN_BUFFER3)){
    buffer = jack_devin->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_jack_devin_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  void *buffer;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  if(ags_jack_devin_test_flags(jack_devin, AGS_JACK_DEVIN_BUFFER0)){
    buffer = jack_devin->buffer[3];
  }else if(ags_jack_devin_test_flags(jack_devin, AGS_JACK_DEVIN_BUFFER1)){
    buffer = jack_devin->buffer[0];
  }else if(ags_jack_devin_test_flags(jack_devin, AGS_JACK_DEVIN_BUFFER2)){
    buffer = jack_devin->buffer[1];
  }else if(ags_jack_devin_test_flags(jack_devin, AGS_JACK_DEVIN_BUFFER3)){
    buffer = jack_devin->buffer[2];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void
ags_jack_devin_lock_buffer(AgsSoundcard *soundcard,
			    void *buffer)
{
  AgsJackDevin *jack_devin;

  pthread_mutex_t *buffer_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(jack_devin->buffer != NULL){
    if(buffer == jack_devin->buffer[0]){
      buffer_mutex = jack_devin->buffer_mutex[0];
    }else if(buffer == jack_devin->buffer[1]){
      buffer_mutex = jack_devin->buffer_mutex[1];
    }else if(buffer == jack_devin->buffer[2]){
      buffer_mutex = jack_devin->buffer_mutex[2];
    }else if(buffer == jack_devin->buffer[3]){
      buffer_mutex = jack_devin->buffer_mutex[3];
    }
  }
  
  if(buffer_mutex != NULL){
    pthread_mutex_lock(buffer_mutex);
  }
}


void
ags_jack_devin_unlock_buffer(AgsSoundcard *soundcard,
			      void *buffer)
{
  AgsJackDevin *jack_devin;

  pthread_mutex_t *buffer_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(jack_devin->buffer != NULL){
    if(buffer == jack_devin->buffer[0]){
      buffer_mutex = jack_devin->buffer_mutex[0];
    }else if(buffer == jack_devin->buffer[1]){
      buffer_mutex = jack_devin->buffer_mutex[1];
    }else if(buffer == jack_devin->buffer[2]){
      buffer_mutex = jack_devin->buffer_mutex[2];
    }else if(buffer == jack_devin->buffer[3]){
      buffer_mutex = jack_devin->buffer_mutex[3];
    }
  }

  if(buffer_mutex != NULL){
    pthread_mutex_unlock(buffer_mutex);
  }
}

guint
ags_jack_devin_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint delay_counter;
  
  pthread_mutex_t *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* delay counter */
  pthread_mutex_lock(jack_devin_mutex);

  delay_counter = jack_devin->delay_counter;
  
  pthread_mutex_unlock(jack_devin_mutex);

  return(delay_counter);
}

void
ags_jack_devin_set_start_note_offset(AgsSoundcard *soundcard,
				     guint start_note_offset)
{
  AgsJackDevin *jack_devin;

  pthread_mutex_t *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(jack_devin_mutex);

  jack_devin->start_note_offset = start_note_offset;

  pthread_mutex_unlock(jack_devin_mutex);
}

guint
ags_jack_devin_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint start_note_offset;
  
  pthread_mutex_t *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(jack_devin_mutex);

  start_note_offset = jack_devin->start_note_offset;

  pthread_mutex_unlock(jack_devin_mutex);

  return(start_note_offset);
}

void
ags_jack_devin_set_note_offset(AgsSoundcard *soundcard,
			       guint note_offset)
{
  AgsJackDevin *jack_devin;

  pthread_mutex_t *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(jack_devin_mutex);

  jack_devin->note_offset = note_offset;

  pthread_mutex_unlock(jack_devin_mutex);
}

guint
ags_jack_devin_get_note_offset(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint note_offset;
  
  pthread_mutex_t *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(jack_devin_mutex);

  note_offset = jack_devin->note_offset;

  pthread_mutex_unlock(jack_devin_mutex);

  return(note_offset);
}

void
ags_jack_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
					guint note_offset_absolute)
{
  AgsJackDevin *jack_devin;
  
  pthread_mutex_t *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(jack_devin_mutex);

  jack_devin->note_offset_absolute = note_offset_absolute;

  pthread_mutex_unlock(jack_devin_mutex);
}

guint
ags_jack_devin_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint note_offset_absolute;
  
  pthread_mutex_t *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(jack_devin_mutex);

  note_offset_absolute = jack_devin->note_offset_absolute;

  pthread_mutex_unlock(jack_devin_mutex);

  return(note_offset_absolute);
}

void
ags_jack_devin_set_loop(AgsSoundcard *soundcard,
			guint loop_left, guint loop_right,
			gboolean do_loop)
{
  AgsJackDevin *jack_devin;

  pthread_mutex_t *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* set loop */
  pthread_mutex_lock(jack_devin_mutex);

  jack_devin->loop_left = loop_left;
  jack_devin->loop_right = loop_right;
  jack_devin->do_loop = do_loop;

  if(do_loop){
    jack_devin->loop_offset = jack_devin->note_offset;
  }

  pthread_mutex_unlock(jack_devin_mutex);
}

void
ags_jack_devin_get_loop(AgsSoundcard *soundcard,
			guint *loop_left, guint *loop_right,
			gboolean *do_loop)
{
  AgsJackDevin *jack_devin;

  pthread_mutex_t *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* get loop */
  pthread_mutex_lock(jack_devin_mutex);

  if(loop_left != NULL){
    *loop_left = jack_devin->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = jack_devin->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = jack_devin->do_loop;
  }

  pthread_mutex_unlock(jack_devin_mutex);
}

guint
ags_jack_devin_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint loop_offset;
  
  pthread_mutex_t *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* get loop offset */
  pthread_mutex_lock(jack_devin_mutex);

  loop_offset = jack_devin->loop_offset;
  
  pthread_mutex_unlock(jack_devin_mutex);

  return(loop_offset);
}

/**
 * ags_jack_devin_switch_buffer_flag:
 * @jack_devin: an #AgsJackDevin
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 2.0.0
 */
void
ags_jack_devin_switch_buffer_flag(AgsJackDevin *jack_devin)
{
  pthread_mutex_t *jack_devin_mutex;
  
  if(!AGS_IS_JACK_DEVIN(jack_devin)){
    return;
  }

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* switch buffer flag */
  pthread_mutex_lock(jack_devin_mutex);

  if((AGS_JACK_DEVIN_BUFFER0 & (jack_devin->flags)) != 0){
    jack_devin->flags &= (~AGS_JACK_DEVIN_BUFFER0);
    jack_devin->flags |= AGS_JACK_DEVIN_BUFFER1;
  }else if((AGS_JACK_DEVIN_BUFFER1 & (jack_devin->flags)) != 0){
    jack_devin->flags &= (~AGS_JACK_DEVIN_BUFFER1);
    jack_devin->flags |= AGS_JACK_DEVIN_BUFFER2;
  }else if((AGS_JACK_DEVIN_BUFFER2 & (jack_devin->flags)) != 0){
    jack_devin->flags &= (~AGS_JACK_DEVIN_BUFFER2);
    jack_devin->flags |= AGS_JACK_DEVIN_BUFFER3;
  }else if((AGS_JACK_DEVIN_BUFFER3 & (jack_devin->flags)) != 0){
    jack_devin->flags &= (~AGS_JACK_DEVIN_BUFFER3);
    jack_devin->flags |= AGS_JACK_DEVIN_BUFFER0;
  }

  pthread_mutex_unlock(jack_devin_mutex);
}

/**
 * ags_jack_devin_adjust_delay_and_attack:
 * @jack_devin: the #AgsJackDevin
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 2.0.0
 */
void
ags_jack_devin_adjust_delay_and_attack(AgsJackDevin *jack_devin)
{
  gdouble delay;
  guint default_tact_frames;
  guint delay_tact_frames;
  guint default_period;
  gint next_attack;
  guint i;

  pthread_mutex_t *jack_devin_mutex;

  if(!AGS_IS_JACK_DEVIN(jack_devin)){
    return;
  }

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());
  
  /* get some initial values */
  delay = ags_jack_devin_get_absolute_delay(AGS_SOUNDCARD(jack_devin));

#ifdef AGS_DEBUG
  g_message("delay : %f", delay);
#endif
  
  pthread_mutex_lock(jack_devin_mutex);

  default_tact_frames = (guint) (delay * jack_devin->buffer_size);
  delay_tact_frames = (guint) (floor(delay) * jack_devin->buffer_size);
  default_period = (1.0 / AGS_SOUNDCARD_DEFAULT_PERIOD) * (default_tact_frames);

  i = 0;
  
  jack_devin->attack[0] = (guint) floor(0.25 * jack_devin->buffer_size);
  next_attack = (((jack_devin->attack[i] + default_tact_frames) / jack_devin->buffer_size) - delay) * jack_devin->buffer_size;

  if(next_attack >= jack_devin->buffer_size){
    next_attack = jack_devin->buffer_size - 1;
  }
  
  /* check if delay drops for next attack */
  if(next_attack < 0){
    jack_devin->attack[i] = jack_devin->attack[i] - ((gdouble) next_attack / 2.0);

    if(jack_devin->attack[i] < 0){
      jack_devin->attack[i] = 0;
    }
    
    if(jack_devin->attack[i] >= jack_devin->buffer_size){
      jack_devin->attack[i] = jack_devin->buffer_size - 1;
    }
    
    next_attack = next_attack + (next_attack / 2.0);
    
    if(next_attack < 0){
      next_attack = 0;
    }

    if(next_attack >= jack_devin->buffer_size){
      next_attack = jack_devin->buffer_size - 1;
    }
  }
  
  for(i = 1; i < (int)  2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    jack_devin->attack[i] = next_attack;
    next_attack = (((jack_devin->attack[i] + default_tact_frames) / jack_devin->buffer_size) - delay) * jack_devin->buffer_size;

    if(next_attack >= jack_devin->buffer_size){
      next_attack = jack_devin->buffer_size - 1;
    }
    
    /* check if delay drops for next attack */
    if(next_attack < 0){
      jack_devin->attack[i] = jack_devin->attack[i] - ((gdouble) next_attack / 2.0);

      if(jack_devin->attack[i] < 0){
	jack_devin->attack[i] = 0;
      }

      if(jack_devin->attack[i] >= jack_devin->buffer_size){
	jack_devin->attack[i] = jack_devin->buffer_size - 1;
      }
    
      next_attack = next_attack + (next_attack / 2.0);
    
      if(next_attack < 0){
	next_attack = 0;
      }

      if(next_attack >= jack_devin->buffer_size){
	next_attack = jack_devin->buffer_size - 1;
      }
    }
    
#ifdef AGS_DEBUG
    g_message("%d", jack_devin->attack[i]);
#endif
  }

  jack_devin->attack[0] = jack_devin->attack[i - 2];
  
  for(i = 0; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD - 1; i++){
    jack_devin->delay[i] = ((gdouble) (default_tact_frames + jack_devin->attack[i] - jack_devin->attack[i + 1])) / (gdouble) jack_devin->buffer_size;
    
#ifdef AGS_DEBUG
    g_message("%f", jack_devin->delay[i]);
#endif
  }

  jack_devin->delay[i] = ((gdouble) (default_tact_frames + jack_devin->attack[i] - jack_devin->attack[0])) / (gdouble) jack_devin->buffer_size;

  pthread_mutex_unlock(jack_devin_mutex);
}

/**
 * ags_jack_devin_realloc_buffer:
 * @jack_devin: the #AgsJackDevin
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 2.0.0
 */
void
ags_jack_devin_realloc_buffer(AgsJackDevin *jack_devin)
{
  AgsJackClient *jack_client;
  
  guint port_count;
  guint pcm_channels;
  guint buffer_size;
  guint word_size;

  pthread_mutex_t *jack_devin_mutex;  

  if(!AGS_IS_JACK_DEVIN(jack_devin)){
    return;
  }

  /* get jack devin mutex */
  pthread_mutex_lock(ags_jack_devin_get_class_mutex());
  
  jack_devin_mutex = jack_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devin_get_class_mutex());

  /* get word size */  
  pthread_mutex_lock(jack_devin_mutex);

  jack_client = jack_devin->jack_client;
  
  port_count = g_list_length(jack_devin->jack_port);
  
  pcm_channels = jack_devin->pcm_channels;
  buffer_size = jack_devin->buffer_size;

  switch(jack_devin->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
    {
      word_size = sizeof(gint8);
    }
    break;
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
  case AGS_SOUNDCARD_SIGNED_64_BIT:
    {
      word_size = sizeof(gint64);
    }
    break;
  default:
    g_warning("ags_jack_devin_realloc_buffer(): unsupported word size");
    return;
  }

  pthread_mutex_unlock(jack_devin_mutex);

  if(port_count < pcm_channels){
    AgsJackPort *jack_port;

    gchar *str;
    
    guint nth_soundcard;
    guint i;

    pthread_mutex_lock(jack_devin_mutex);

    if(jack_devin->card_uri != NULL){
      sscanf(jack_devin->card_uri,
	     "ags-jack-devin-%u",
	     &nth_soundcard);
    }else{
      pthread_mutex_unlock(jack_devin_mutex);

      g_warning("ags_jack_devin_realloc_buffer() - card uri not set");
      
      return;
    }

    pthread_mutex_unlock(jack_devin_mutex);
    
    for(i = port_count; i < pcm_channels; i++){
      str = g_strdup_printf("ags%d-%04d",
			    nth_soundcard,
			    i);
      
      jack_port = ags_jack_port_new(jack_client);
      ags_jack_client_add_port((AgsJackClient *) jack_client,
			       (GObject *) jack_port);

      pthread_mutex_lock(jack_devin_mutex);
    
      jack_devin->jack_port = g_list_prepend(jack_devin->jack_port,
					     jack_port);
    
      if(jack_devin->port_name == NULL){
	jack_devin->port_name = (gchar **) malloc(2 * sizeof(gchar *));
	jack_devin->port_name[0] = g_strdup(str);
      }else{
	jack_devin->port_name = (gchar **) realloc(jack_devin->port_name,
						   (i + 2) * sizeof(gchar *));
	jack_devin->port_name[i] = g_strdup(str);
      }
      
      pthread_mutex_unlock(jack_devin_mutex);
      
      ags_jack_port_register(jack_port,
			     str,
			     TRUE, FALSE,
			     FALSE);
    }

    jack_devin->port_name[jack_devin->pcm_channels] = NULL;    
  }else if(port_count > pcm_channels){
    GList *jack_port_start, *jack_port;

    guint i;

    pthread_mutex_lock(jack_devin_mutex);

    jack_port =
      jack_port_start = g_list_copy(jack_devin->jack_port);

    pthread_mutex_unlock(jack_devin_mutex);

    for(i = 0; i < port_count - pcm_channels; i++){
      jack_devin->jack_port = g_list_remove(jack_devin->jack_port,
					    jack_port->data);
      ags_jack_port_unregister(jack_port->data);
      
      g_object_unref(jack_port->data);
      
      jack_port = jack_port->next;
    }

    g_list_free(jack_port_start);

    pthread_mutex_lock(jack_devin_mutex);
    
    jack_devin->port_name = (gchar **) realloc(jack_devin->port_name,
					       (jack_devin->pcm_channels + 1) * sizeof(gchar *));
    jack_devin->port_name[jack_devin->pcm_channels] = NULL;

    pthread_mutex_unlock(jack_devin_mutex);
  }
  
  /* AGS_JACK_DEVIN_BUFFER_0 */
  if(jack_devin->buffer[0] != NULL){
    free(jack_devin->buffer[0]);
  }
  
  jack_devin->buffer[0] = (void *) malloc(jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  
  /* AGS_JACK_DEVIN_BUFFER_1 */
  if(jack_devin->buffer[1] != NULL){
    free(jack_devin->buffer[1]);
  }

  jack_devin->buffer[1] = (void *) malloc(jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  
  /* AGS_JACK_DEVIN_BUFFER_2 */
  if(jack_devin->buffer[2] != NULL){
    free(jack_devin->buffer[2]);
  }

  jack_devin->buffer[2] = (void *) malloc(jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  
  /* AGS_JACK_DEVIN_BUFFER_3 */
  if(jack_devin->buffer[3] != NULL){
    free(jack_devin->buffer[3]);
  }
  
  jack_devin->buffer[3] = (void *) malloc(jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
}

/**
 * ags_jack_devin_new:
 * @application_context: the #AgsApplicationContext
 *
 * Creates a new instance of #AgsJackDevin.
 *
 * Returns: the new #AgsJackDevin
 *
 * Since: 2.0.0
 */
AgsJackDevin*
ags_jack_devin_new(AgsApplicationContext *application_context)
{
  AgsJackDevin *jack_devin;

  jack_devin = (AgsJackDevin *) g_object_new(AGS_TYPE_JACK_DEVIN,
					     "application-context", application_context,
					     NULL);
  
  return(jack_devin);
}
