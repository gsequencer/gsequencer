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

#include <ags/audio/jack/ags_jack_devout.h>

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
void ags_jack_devout_dispose(GObject *gobject);
void ags_jack_devout_finalize(GObject *gobject);

AgsUUID* ags_jack_devout_get_uuid(AgsConnectable *connectable);
gboolean ags_jack_devout_has_resource(AgsConnectable *connectable);
gboolean ags_jack_devout_is_ready(AgsConnectable *connectable);
void ags_jack_devout_add_to_registry(AgsConnectable *connectable);
void ags_jack_devout_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_jack_devout_list_resource(AgsConnectable *connectable);
xmlNode* ags_jack_devout_xml_compose(AgsConnectable *connectable);
void ags_jack_devout_xml_parse(AgsConnectable *connectable,
			       xmlNode *node);
gboolean ags_jack_devout_is_connected(AgsConnectable *connectable);
void ags_jack_devout_connect(AgsConnectable *connectable);
void ags_jack_devout_disconnect(AgsConnectable *connectable);

void ags_jack_devout_set_application_context(AgsSoundcard *soundcard,
					     AgsApplicationContext *application_context);
AgsApplicationContext* ags_jack_devout_get_application_context(AgsSoundcard *soundcard);

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
};

static gpointer ags_jack_devout_parent_class = NULL;

static pthread_mutex_t ags_jack_devout_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_jack_devout_get_type (void)
{
  static GType ags_type_jack_devout = 0;

  if(!ags_type_jack_devout){
    static const GTypeInfo ags_jack_devout_info = {
      sizeof(AgsJackDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsJackDevout),
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
   * AgsJackDevout:device:
   *
   * The jack soundcard indentifier
   * 
   * Since: 2.0.0
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
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("dsp-channels",
				 i18n_pspec("count of DSP channels"),
				 i18n_pspec("The count of DSP channels to use"),
				 AGS_SOUNDCARD_MIN_DSP_CHANNELS,
				 AGS_SOUNDCARD_MAX_DSP_CHANNELS,
				 AGS_SOUNDCARD_DEFAULT_DSP_CHANNELS,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DSP_CHANNELS,
				  param_spec);

  /**
   * AgsJackDevout:pcm-channels:
   *
   * The pcm channel count
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("pcm-channels",
				 i18n_pspec("count of PCM channels"),
				 i18n_pspec("The count of PCM channels to use"),
				 AGS_SOUNDCARD_MIN_PCM_CHANNELS,
				 AGS_SOUNDCARD_MAX_PCM_CHANNELS,
				 AGS_SOUNDCARD_DEFAULT_PCM_CHANNELS,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PCM_CHANNELS,
				  param_spec);

  /**
   * AgsJackDevout:format:
   *
   * The precision of the buffer
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("precision of buffer"),
				 i18n_pspec("The precision to use for a frame"),
				 0,
				 G_MAXUINT32,
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
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("buffer-size",
				 i18n_pspec("frame count of a buffer"),
				 i18n_pspec("The count of frames a buffer contains"),
				 AGS_SOUNDCARD_MIN_BUFFER_SIZE,
				 AGS_SOUNDCARD_MAX_BUFFER_SIZE,
				 AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER_SIZE,
				  param_spec);

  /**
   * AgsJackDevout:samplerate:
   *
   * The samplerate
   * 
   * Since: 2.0.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("frames per second"),
				 i18n_pspec("The frames count played during a second"),
				 AGS_SOUNDCARD_MIN_SAMPLERATE,
				 AGS_SOUNDCARD_MAX_SAMPLERATE,
				 AGS_SOUNDCARD_DEFAULT_SAMPLERATE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsJackDevout:buffer:
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
   * AgsJackDevout:bpm:
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
   * AgsJackDevout:delay-factor:
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
   * AgsJackDevout:attack:
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
   * AgsJackDevout:jack-client:
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
   * AgsJackDevout:jack-port:
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
ags_jack_devout_error_quark()
{
  return(g_quark_from_static_string("ags-jack_devout-error-quark"));
}

void
ags_jack_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_jack_devout_get_uuid;
  connectable->has_resource = ags_jack_devout_has_resource;

  connectable->is_ready = ags_jack_devout_is_ready;
  connectable->add_to_registry = ags_jack_devout_add_to_registry;
  connectable->remove_from_registry = ags_jack_devout_remove_from_registry;

  connectable->list_resource = ags_jack_devout_list_resource;
  connectable->xml_compose = ags_jack_devout_xml_compose;
  connectable->xml_parse = ags_jack_devout_xml_parse;

  connectable->is_connected = ags_jack_devout_is_connected;  
  connectable->connect = ags_jack_devout_connect;
  connectable->disconnect = ags_jack_devout_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_jack_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_application_context = ags_jack_devout_set_application_context;
  soundcard->get_application_context = ags_jack_devout_get_application_context;

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
}

void
ags_jack_devout_init(AgsJackDevout *jack_devout)
{
  AgsConfig *config;

  gchar *str;
  gchar *segmentation;

  guint denumerator, numerator;
  
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* flags */
  jack_devout->flags = 0;
  g_atomic_int_set(&(jack_devout->sync_flags),
		   AGS_JACK_DEVOUT_PASS_THROUGH);

  /* devout mutex */
  jack_devout->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  jack_devout->obj_mutex = 
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* parent */
  jack_devout->application_context = NULL;

  /* uuid */
  jack_devout->uuid = ags_uuid_alloc();
  ags_uuid_generate(jack_devout->uuid);

  /* presets */
  config = ags_config_get_instance();
  
  jack_devout->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  jack_devout->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  jack_devout->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  jack_devout->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  jack_devout->format = ags_soundcard_helper_config_get_format(config);

  /*  */
  jack_devout->card_uri = NULL;
  jack_devout->jack_client = NULL;

  jack_devout->port_name = NULL;
  jack_devout->jack_port = NULL;

  /* buffer */
  jack_devout->buffer = (void **) malloc(4 * sizeof(void*));

  jack_devout->buffer[0] = NULL;
  jack_devout->buffer[1] = NULL;
  jack_devout->buffer[2] = NULL;
  jack_devout->buffer[3] = NULL;
  
  ags_jack_devout_realloc_buffer(jack_devout);
  
  /* bpm */
  jack_devout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  jack_devout->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denumerator,
	   &numerator);
    
    jack_devout->delay_factor = 1.0 / numerator * (numerator / denumerator);

    g_free(segmentation);
  }

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

  /*  */
  jack_devout->notify_soundcard = NULL;
}

void
ags_jack_devout_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackDevout *jack_devout;

  pthread_mutex_t *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(gobject);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      pthread_mutex_lock(jack_devout_mutex);

      if(jack_devout->application_context == application_context){
	pthread_mutex_unlock(jack_devout_mutex);

	return;
      }

      if(jack_devout->application_context != NULL){
	g_object_unref(G_OBJECT(jack_devout->application_context));
      }

      if(application_context != NULL){	
	g_object_ref(G_OBJECT(application_context));
      }

      jack_devout->application_context = application_context;

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      pthread_mutex_lock(jack_devout_mutex);

      jack_devout->card_uri = g_strdup(device);

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      pthread_mutex_lock(jack_devout_mutex);

      if(dsp_channels == jack_devout->dsp_channels){
	pthread_mutex_unlock(jack_devout_mutex);

	return;
      }

      jack_devout->dsp_channels = dsp_channels;

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      pthread_mutex_lock(jack_devout_mutex);

      if(pcm_channels == jack_devout->pcm_channels){
	pthread_mutex_unlock(jack_devout_mutex);

	return;
      }

      jack_devout->pcm_channels = pcm_channels;

      pthread_mutex_unlock(jack_devout_mutex);

      ags_jack_devout_realloc_buffer(jack_devout);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      pthread_mutex_lock(jack_devout_mutex);

      if(format == jack_devout->format){
	pthread_mutex_unlock(jack_devout_mutex);

	return;
      }

      jack_devout->format = format;

      pthread_mutex_unlock(jack_devout_mutex);

      ags_jack_devout_realloc_buffer(jack_devout);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      pthread_mutex_lock(jack_devout_mutex);

      if(buffer_size == jack_devout->buffer_size){
	pthread_mutex_unlock(jack_devout_mutex);

	return;
      }

      jack_devout->buffer_size = buffer_size;

      pthread_mutex_unlock(jack_devout_mutex);

      ags_jack_devout_realloc_buffer(jack_devout);
      ags_jack_devout_adjust_delay_and_attack(jack_devout);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      pthread_mutex_lock(jack_devout_mutex);
      
      if(samplerate == jack_devout->samplerate){
	pthread_mutex_unlock(jack_devout_mutex);

	return;
      }

      jack_devout->samplerate = samplerate;

      pthread_mutex_unlock(jack_devout_mutex);

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

      pthread_mutex_lock(jack_devout_mutex);

      jack_devout->bpm = bpm;

      pthread_mutex_unlock(jack_devout_mutex);

      ags_jack_devout_adjust_delay_and_attack(jack_devout);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      pthread_mutex_lock(jack_devout_mutex);

      jack_devout->delay_factor = delay_factor;

      pthread_mutex_unlock(jack_devout_mutex);

      ags_jack_devout_adjust_delay_and_attack(jack_devout);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      AgsJackClient *jack_client;

      jack_client = (AgsJackClient *) g_value_get_object(value);

      pthread_mutex_lock(jack_devout_mutex);

      if(jack_devout->jack_client == (GObject *) jack_client){
	pthread_mutex_unlock(jack_devout_mutex);

	return;
      }

      if(jack_devout->jack_client != NULL){
	g_object_unref(G_OBJECT(jack_devout->jack_client));
      }

      if(jack_client != NULL){
	g_object_ref(jack_client);
      }
      
      jack_devout->jack_client = (GObject *) jack_client;

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_JACK_PORT:
    {
      AgsJackPort *jack_port;

      jack_port = (AgsJackPort *) g_value_get_pointer(value);

      pthread_mutex_lock(jack_devout_mutex);

      if(!AGS_IS_JACK_PORT(jack_port) ||
	 g_list_find(jack_devout->jack_port, jack_port) != NULL){
	pthread_mutex_unlock(jack_devout_mutex);

	return;
      }

      g_object_ref(jack_port);
      jack_devout->jack_port = g_list_append(jack_devout->jack_port,
					     jack_port);

      pthread_mutex_unlock(jack_devout_mutex);
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

  pthread_mutex_t *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(gobject);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      pthread_mutex_lock(jack_devout_mutex);

      g_value_set_object(value, jack_devout->application_context);

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      pthread_mutex_lock(jack_devout_mutex);

      g_value_set_string(value, jack_devout->card_uri);

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      pthread_mutex_lock(jack_devout_mutex);

      g_value_set_uint(value, jack_devout->dsp_channels);

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      pthread_mutex_lock(jack_devout_mutex);

      g_value_set_uint(value, jack_devout->pcm_channels);

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      pthread_mutex_lock(jack_devout_mutex);

      g_value_set_uint(value, jack_devout->format);

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      pthread_mutex_lock(jack_devout_mutex);

      g_value_set_uint(value, jack_devout->buffer_size);

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(jack_devout_mutex);

      g_value_set_uint(value, jack_devout->samplerate);

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      pthread_mutex_lock(jack_devout_mutex);

      g_value_set_pointer(value, jack_devout->buffer);

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_BPM:
    {
      pthread_mutex_lock(jack_devout_mutex);

      g_value_set_double(value, jack_devout->bpm);

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      pthread_mutex_lock(jack_devout_mutex);

      g_value_set_double(value, jack_devout->delay_factor);

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_ATTACK:
    {
      pthread_mutex_lock(jack_devout_mutex);

      g_value_set_pointer(value, jack_devout->attack);

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      pthread_mutex_lock(jack_devout_mutex);

      g_value_set_object(value, jack_devout->jack_client);

      pthread_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_JACK_PORT:
    {
      pthread_mutex_lock(jack_devout_mutex);

      g_value_set_pointer(value,
			  g_list_copy(jack_devout->jack_port));

      pthread_mutex_unlock(jack_devout_mutex);
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

  jack_devout = AGS_JACK_DEVOUT(gobject);

  /* jack client */
  if(jack_devout->jack_client != NULL){
    g_object_unref(jack_devout->jack_client);

    jack_devout->jack_client = NULL;
  }

  /* jack port */
  g_list_free_full(jack_devout->jack_port,
		   g_object_unref);

  jack_devout->jack_port = NULL;

  /* notify soundcard */
  if(jack_devout->notify_soundcard != NULL){
    if(jack_devout->application_context != NULL){
      AgsTaskThread *task_thread;
    
      g_object_get(jack_devout->application_context,
		   "task-thread", &task_thread,
		   NULL);
      
      ags_task_thread_remove_cyclic_task(task_thread,
					 jack_devout->notify_soundcard);
    }
    
    g_object_unref(jack_devout->notify_soundcard);

    jack_devout->notify_soundcard = NULL;
  }

  /* application context */
  if(jack_devout->application_context != NULL){
    g_object_unref(jack_devout->application_context);

    jack_devout->application_context = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_jack_devout_parent_class)->dispose(gobject);
}

void
ags_jack_devout_finalize(GObject *gobject)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(gobject);

  pthread_mutex_destroy(jack_devout->obj_mutex);
  free(jack_devout->obj_mutex);

  pthread_mutexattr_destroy(jack_devout->obj_mutexattr);
  free(jack_devout->obj_mutexattr);

  /* free output buffer */
  free(jack_devout->buffer[0]);
  free(jack_devout->buffer[1]);
  free(jack_devout->buffer[2]);
  free(jack_devout->buffer[3]);

  /* free buffer array */
  free(jack_devout->buffer);

  /* free AgsAttack */
  free(jack_devout->attack);

  /* jack client */
  if(jack_devout->jack_client != NULL){
    g_object_unref(jack_devout->jack_client);
  }

  /* jack port */
  g_list_free_full(jack_devout->jack_port,
		   g_object_unref);

  /* notify soundcard */
  if(jack_devout->notify_soundcard != NULL){
    if(jack_devout->application_context != NULL){
      AgsTaskThread *task_thread;
      
      g_object_get(jack_devout->application_context,
		   "task-thread", &task_thread,
		   NULL);

      ags_task_thread_remove_cyclic_task(task_thread,
					 jack_devout->notify_soundcard);
    }
    
    g_object_unref(jack_devout->notify_soundcard);
  }

  /* application context */
  if(jack_devout->application_context != NULL){
    g_object_unref(jack_devout->application_context);
  }  
  
  /* call parent */
  G_OBJECT_CLASS(ags_jack_devout_parent_class)->finalize(gobject);
}

AgsUUID*
ags_jack_devout_get_uuid(AgsConnectable *connectable)
{
  AgsJackDevout *jack_devout;
  
  AgsUUID *ptr;

  pthread_mutex_t *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(connectable);

  /* get jack devout signal mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* get UUID */
  pthread_mutex_lock(jack_devout_mutex);

  ptr = jack_devout->uuid;

  pthread_mutex_unlock(jack_devout_mutex);
  
  return(ptr);
}

gboolean
ags_jack_devout_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_jack_devout_is_ready(AgsConnectable *connectable)
{
  AgsJackDevout *jack_devout;
  
  gboolean is_ready;

  pthread_mutex_t *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(connectable);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* check is added */
  pthread_mutex_lock(jack_devout_mutex);

  is_ready = (((AGS_JACK_DEVOUT_ADDED_TO_REGISTRY & (jack_devout->flags)) != 0) ? TRUE: FALSE);

  pthread_mutex_unlock(jack_devout_mutex);
  
  return(is_ready);
}

void
ags_jack_devout_add_to_registry(AgsConnectable *connectable)
{
  AgsJackDevout *jack_devout;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  jack_devout = AGS_JACK_DEVOUT(connectable);

  ags_jack_devout_set_flags(jack_devout, AGS_JACK_DEVOUT_ADDED_TO_REGISTRY);
}

void
ags_jack_devout_remove_from_registry(AgsConnectable *connectable)
{
  AgsJackDevout *jack_devout;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  jack_devout = AGS_JACK_DEVOUT(connectable);

  ags_jack_devout_unset_flags(jack_devout, AGS_JACK_DEVOUT_ADDED_TO_REGISTRY);
}

xmlNode*
ags_jack_devout_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_jack_devout_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_jack_devout_xml_parse(AgsConnectable *connectable,
		      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_jack_devout_is_connected(AgsConnectable *connectable)
{
  AgsJackDevout *jack_devout;
  
  gboolean is_connected;

  pthread_mutex_t *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(connectable);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* check is connected */
  pthread_mutex_lock(jack_devout_mutex);

  is_connected = (((AGS_JACK_DEVOUT_CONNECTED & (jack_devout->flags)) != 0) ? TRUE: FALSE);
  
  pthread_mutex_unlock(jack_devout_mutex);
  
  return(is_connected);
}

void
ags_jack_devout_connect(AgsConnectable *connectable)
{
  AgsJackDevout *jack_devout;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  jack_devout = AGS_JACK_DEVOUT(connectable);

  ags_jack_devout_set_flags(jack_devout, AGS_JACK_DEVOUT_CONNECTED);
}

void
ags_jack_devout_disconnect(AgsConnectable *connectable)
{

  AgsJackDevout *jack_devout;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  jack_devout = AGS_JACK_DEVOUT(connectable);
  
  ags_jack_devout_unset_flags(jack_devout, AGS_JACK_DEVOUT_CONNECTED);
}

/**
 * ags_jack_devout_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.0.0
 */
pthread_mutex_t*
ags_jack_devout_get_class_mutex()
{
  return(&ags_jack_devout_class_mutex);
}

/**
 * ags_jack_devout_test_flags:
 * @jack_devout: the #AgsJackDevout
 * @flags: the flags
 *
 * Test @flags to be set on @jack_devout.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_jack_devout_test_flags(AgsJackDevout *jack_devout, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *jack_devout_mutex;

  if(!AGS_IS_JACK_DEVOUT(jack_devout)){
    return(FALSE);
  }

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* test */
  pthread_mutex_lock(jack_devout_mutex);

  retval = (flags & (jack_devout->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(jack_devout_mutex);

  return(retval);
}

/**
 * ags_jack_devout_set_flags:
 * @jack_devout: the #AgsJackDevout
 * @flags: see #AgsJackDevoutFlags-enum
 *
 * Enable a feature of @jack_devout.
 *
 * Since: 2.0.0
 */
void
ags_jack_devout_set_flags(AgsJackDevout *jack_devout, guint flags)
{
  pthread_mutex_t *jack_devout_mutex;

  if(!AGS_IS_JACK_DEVOUT(jack_devout)){
    return;
  }

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(jack_devout_mutex);

  jack_devout->flags |= flags;
  
  pthread_mutex_unlock(jack_devout_mutex);
}
    
/**
 * ags_jack_devout_unset_flags:
 * @jack_devout: the #AgsJackDevout
 * @flags: see #AgsJackDevoutFlags-enum
 *
 * Disable a feature of @jack_devout.
 *
 * Since: 2.0.0
 */
void
ags_jack_devout_unset_flags(AgsJackDevout *jack_devout, guint flags)
{  
  pthread_mutex_t *jack_devout_mutex;

  if(!AGS_IS_JACK_DEVOUT(jack_devout)){
    return;
  }

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(jack_devout_mutex);

  jack_devout->flags &= (~flags);
  
  pthread_mutex_unlock(jack_devout_mutex);
}

void
ags_jack_devout_set_application_context(AgsSoundcard *soundcard,
					AgsApplicationContext *application_context)
{
  AgsJackDevout *jack_devout;

  pthread_mutex_t *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* set application context */
  pthread_mutex_lock(jack_devout_mutex);
  
  jack_devout->application_context = (GObject *) application_context;
  
  pthread_mutex_unlock(jack_devout_mutex);
}

AgsApplicationContext*
ags_jack_devout_get_application_context(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  AgsApplicationContext *application_context;
  
  pthread_mutex_t *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* get application context */
  pthread_mutex_lock(jack_devout_mutex);

  application_context = (AgsApplicationContext *) jack_devout->application_context;

  pthread_mutex_unlock(jack_devout_mutex);
  
  return(application_context);
}

void
ags_jack_devout_set_device(AgsSoundcard *soundcard,
			   gchar *device)
{
  AgsJackDevout *jack_devout;

  GList *jack_port, *jack_port_start;

  gchar *str;

  guint pcm_channels;
  int ret;
  guint nth_card;
  guint i;
  
  pthread_mutex_t *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* check device */
  pthread_mutex_lock(jack_devout_mutex);

  if(jack_devout->card_uri == device ||
     !g_ascii_strcasecmp(jack_devout->card_uri,
			 device)){
    pthread_mutex_unlock(jack_devout_mutex);
  
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-jack-devout-")){
    pthread_mutex_unlock(jack_devout_mutex);

    g_warning("invalid JACK device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-jack-devout-%u",
	       &nth_card);

  if(ret != 1){
    pthread_mutex_unlock(jack_devout_mutex);

    g_warning("invalid JACK device specifier");

    return;
  }

  g_free(jack_devout->card_uri);
  jack_devout->card_uri = g_strdup(device);

  /* apply name to port */
  pcm_channels = jack_devout->pcm_channels;
  
  jack_port_start = 
    jack_port = g_list_copy(jack_devout->jack_port);

  pthread_mutex_unlock(jack_devout_mutex);
  
  for(i = 0; i < pcm_channels; i++){
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
  
  gchar *device;

  pthread_mutex_t *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  device = NULL;

  /* get device */
  pthread_mutex_lock(jack_devout_mutex);

  device = g_strdup(jack_devout->card_uri);

  pthread_mutex_unlock(jack_devout_mutex);
  
  return(device);
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

  pthread_mutex_t *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* get presets */
  pthread_mutex_lock(jack_devout_mutex);

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

  pthread_mutex_unlock(jack_devout_mutex);
}

void
ags_jack_devout_list_cards(AgsSoundcard *soundcard,
			   GList **card_id, GList **card_name)
{
  AgsJackClient *jack_client;
  AgsJackDevout *jack_devout;

  AgsApplicationContext *application_context;
  
  GList *list_start, *list;

  gchar *card_uri;
  gchar *client_name;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  g_object_get(jack_devout,
	       "application-context", &application_context,
	       NULL);

  if(application_context == NULL){
    return;
  }
  
  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  list =
    list_start = ags_sound_provider_get_soundcard(AGS_SOUND_PROVIDER(application_context));
  
  while(list != NULL){
    if(AGS_IS_JACK_DEVOUT(list->data)){
      if(card_id != NULL){
	card_uri = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));
	
	if(AGS_JACK_DEVOUT(list->data)->card_uri != NULL){
	  *card_id = g_list_prepend(*card_id,
				    card_uri);
	}else{
	  *card_id = g_list_prepend(*card_id,
				    g_strdup("(null)"));

	  g_warning("ags_jack_devout_list_cards() - card id (null)");
	}
      }

      if(card_name != NULL){
	g_object_get(list->data,
		     "jack-client", &jack_client,
		     NULL);
	
	if(jack_client != NULL){
	  pthread_mutex_t *jack_client_mutex;
	  
	  /* get jack client mutex */
	  pthread_mutex_lock(ags_jack_client_get_class_mutex());
  
	  jack_client_mutex = jack_client->obj_mutex;
  
	  pthread_mutex_unlock(ags_jack_client_get_class_mutex());

	  /* get client name */
	  pthread_mutex_lock(jack_client_mutex);

	  client_name = g_strdup(jack_client->client_name);

	  pthread_mutex_unlock(jack_client_mutex);
	  
	  *card_name = g_list_prepend(*card_name,
				      client_name);
	}else{
	  *card_name = g_list_prepend(*card_name,
				      g_strdup("(null)"));

	  g_warning("ags_jack_devout_list_cards() - JACK client not connected (null)");
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

  gboolean is_starting;
  
  pthread_mutex_t *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* check is starting */
  pthread_mutex_lock(jack_devout_mutex);

  is_starting = ((AGS_JACK_DEVOUT_START_PLAY & (jack_devout->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(jack_devout_mutex);
  
  return(is_starting);
}

gboolean
ags_jack_devout_is_playing(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  gboolean is_playing;
  
  pthread_mutex_t *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* check is starting */
  pthread_mutex_lock(jack_devout_mutex);

  is_playing = ((AGS_JACK_DEVOUT_PLAY & (jack_devout->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(jack_devout_mutex);

  return(is_playing);
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

  guint format, word_size;
  
  pthread_mutex_t *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* retrieve word size */
  pthread_mutex_lock(jack_devout_mutex);

  switch(jack_devout->format){
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
    pthread_mutex_unlock(jack_devout_mutex);
    
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

  pthread_mutex_unlock(jack_devout_mutex);
}

void
ags_jack_devout_port_play(AgsSoundcard *soundcard,
			  GError **error)
{
  AgsJackClient *jack_client;
  AgsJackDevout *jack_devout;

  AgsNotifySoundcard *notify_soundcard;
  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsTaskThread *task_thread;
  
  AgsApplicationContext *application_context;

  GList *task;

  guint word_size;
  gboolean jack_client_activated;
  
  pthread_mutex_t *jack_devout_mutex;
  pthread_mutex_t *jack_client_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);
  application_context = ags_soundcard_get_application_context(soundcard);
  
  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* client */
  pthread_mutex_lock(jack_devout_mutex);

  jack_client = (AgsJackClient *) jack_devout->jack_client;
  
  callback_mutex = jack_devout->callback_mutex;
  callback_finish_mutex = jack_devout->callback_finish_mutex;

  pthread_mutex_unlock(jack_devout_mutex);

  /* do playback */
  pthread_mutex_lock(jack_devout_mutex);
  
  jack_devout->flags &= (~AGS_JACK_DEVOUT_START_PLAY);
  notify_soundcard = AGS_NOTIFY_SOUNDCARD(jack_devout->notify_soundcard);
  
  if((AGS_JACK_DEVOUT_INITIALIZED & (jack_devout->flags)) == 0){
    pthread_mutex_unlock(jack_devout_mutex);
    
    return;
  }

  switch(jack_devout->format){
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
    pthread_mutex_unlock(jack_devout_mutex);
    
    g_warning("ags_jack_devout_port_play(): unsupported word size");
    
    return;
  }

  pthread_mutex_unlock(jack_devout_mutex);

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

  /* update soundcard */
  g_object_get(application_context,
	       "task-thread", &task_thread,
	       NULL);  
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
}

void
ags_jack_devout_port_free(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  AgsNotifySoundcard *notify_soundcard;

  guint word_size;

  pthread_mutex_t *jack_devout_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /*  */
  pthread_mutex_lock(jack_devout_mutex);

  notify_soundcard = AGS_NOTIFY_SOUNDCARD(jack_devout->notify_soundcard);

  if((AGS_JACK_DEVOUT_INITIALIZED & (jack_devout->flags)) == 0){
    pthread_mutex_unlock(jack_devout_mutex);

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
    
    g_critical("ags_jack_devout_free(): unsupported word size");
  }

  memset(jack_devout->buffer[1], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  memset(jack_devout->buffer[2], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  memset(jack_devout->buffer[3], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  memset(jack_devout->buffer[0], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);

  pthread_mutex_unlock(jack_devout_mutex);
}

void
ags_jack_devout_tic(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  gdouble delay;
  gdouble delay_counter;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  
  pthread_mutex_t *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());
  
  /* determine if attack should be switched */
  pthread_mutex_lock(jack_devout_mutex);

  delay = jack_devout->delay[jack_devout->tic_counter];
  delay_counter = jack_devout->delay_counter;

  note_offset = jack_devout->note_offset;
  note_offset_absolute = jack_devout->note_offset_absolute;
  
  loop_left = jack_devout->loop_left;
  loop_right = jack_devout->loop_right;
  
  do_loop = jack_devout->do_loop;

  pthread_mutex_unlock(jack_devout_mutex);

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
    pthread_mutex_lock(jack_devout_mutex);
    
    jack_devout->delay_counter = 0.0;
    jack_devout->tact_counter += 1.0;

    pthread_mutex_unlock(jack_devout_mutex);
  }else{
    pthread_mutex_lock(jack_devout_mutex);
    
    jack_devout->delay_counter += 1.0;

    pthread_mutex_unlock(jack_devout_mutex);
  }
}

void
ags_jack_devout_offset_changed(AgsSoundcard *soundcard,
			       guint note_offset)
{
  AgsJackDevout *jack_devout;
  
  pthread_mutex_t *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* offset changed */
  pthread_mutex_lock(jack_devout_mutex);

  jack_devout->tic_counter += 1;

  if(jack_devout->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    jack_devout->tic_counter = 0;
  }

  pthread_mutex_unlock(jack_devout_mutex);
}

void
ags_jack_devout_set_bpm(AgsSoundcard *soundcard,
			gdouble bpm)
{
  AgsJackDevout *jack_devout;

  pthread_mutex_t *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* set bpm */
  pthread_mutex_lock(jack_devout_mutex);

  jack_devout->bpm = bpm;

  pthread_mutex_unlock(jack_devout_mutex);

  ags_jack_devout_adjust_delay_and_attack(jack_devout);
}

gdouble
ags_jack_devout_get_bpm(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  gdouble bpm;
  
  pthread_mutex_t *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* get bpm */
  pthread_mutex_lock(jack_devout_mutex);

  bpm = jack_devout->bpm;
  
  pthread_mutex_unlock(jack_devout_mutex);

  return(bpm);
}

void
ags_jack_devout_set_delay_factor(AgsSoundcard *soundcard,
				 gdouble delay_factor)
{
  AgsJackDevout *jack_devout;

  pthread_mutex_t *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* set delay factor */
  pthread_mutex_lock(jack_devout_mutex);

  jack_devout->delay_factor = delay_factor;

  pthread_mutex_unlock(jack_devout_mutex);

  ags_jack_devout_adjust_delay_and_attack(jack_devout);
}

gdouble
ags_jack_devout_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  gdouble delay_factor;
  
  pthread_mutex_t *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* get delay factor */
  pthread_mutex_lock(jack_devout_mutex);

  delay_factor = jack_devout->delay_factor;
  
  pthread_mutex_unlock(jack_devout_mutex);

  return(delay_factor);
}

gdouble
ags_jack_devout_get_delay(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  guint delay_index;
  gdouble delay;
  
  pthread_mutex_t *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* get delay */
  pthread_mutex_lock(jack_devout_mutex);

  delay_index = jack_devout->tic_counter;

  delay = jack_devout->delay[delay_index];
  
  pthread_mutex_unlock(jack_devout_mutex);
  
  return(delay);
}

gdouble
ags_jack_devout_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  gdouble absolute_delay;
  
  pthread_mutex_t *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* get absolute delay */
  pthread_mutex_lock(jack_devout_mutex);

  absolute_delay = (60.0 * (((gdouble) jack_devout->samplerate / (gdouble) jack_devout->buffer_size) / (gdouble) jack_devout->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) jack_devout->delay_factor)));

  pthread_mutex_unlock(jack_devout_mutex);

  return(absolute_delay);
}

guint
ags_jack_devout_get_attack(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  guint attack_index;
  guint attack;
  
  pthread_mutex_t *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* get attack */
  pthread_mutex_lock(jack_devout_mutex);

  attack_index = jack_devout->tic_counter;

  attack = jack_devout->attack[attack_index];

  pthread_mutex_unlock(jack_devout_mutex);
  
  return(attack);
}

void*
ags_jack_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  void *buffer;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  if(ags_jack_devout_test_flags(jack_devout, AGS_JACK_DEVOUT_BUFFER0)){
    buffer = jack_devout->buffer[0];
  }else if(ags_jack_devout_test_flags(jack_devout, AGS_JACK_DEVOUT_BUFFER1)){
    buffer = jack_devout->buffer[1];
  }else if(ags_jack_devout_test_flags(jack_devout, AGS_JACK_DEVOUT_BUFFER2)){
    buffer = jack_devout->buffer[2];
  }else if(ags_jack_devout_test_flags(jack_devout, AGS_JACK_DEVOUT_BUFFER3)){
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

  //  g_message("next - 0x%0x", ((AGS_JACK_DEVOUT_BUFFER0 |
  //				AGS_JACK_DEVOUT_BUFFER1 |
  //				AGS_JACK_DEVOUT_BUFFER2 |
  //				AGS_JACK_DEVOUT_BUFFER3) & (jack_devout->flags)));

  if(ags_jack_devout_test_flags(jack_devout, AGS_JACK_DEVOUT_BUFFER0)){
    buffer = jack_devout->buffer[1];
  }else if(ags_jack_devout_test_flags(jack_devout, AGS_JACK_DEVOUT_BUFFER1)){
    buffer = jack_devout->buffer[2];
  }else if(ags_jack_devout_test_flags(jack_devout, AGS_JACK_DEVOUT_BUFFER2)){
    buffer = jack_devout->buffer[3];
  }else if(ags_jack_devout_test_flags(jack_devout, AGS_JACK_DEVOUT_BUFFER3)){
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

  if(ags_jack_devout_test_flags(jack_devout, AGS_JACK_DEVOUT_BUFFER0)){
    buffer = jack_devout->buffer[3];
  }else if(ags_jack_devout_test_flags(jack_devout, AGS_JACK_DEVOUT_BUFFER1)){
    buffer = jack_devout->buffer[0];
  }else if(ags_jack_devout_test_flags(jack_devout, AGS_JACK_DEVOUT_BUFFER2)){
    buffer = jack_devout->buffer[1];
  }else if(ags_jack_devout_test_flags(jack_devout, AGS_JACK_DEVOUT_BUFFER3)){
    buffer = jack_devout->buffer[2];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

guint
ags_jack_devout_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  guint delay_counter;
  
  pthread_mutex_t *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* delay counter */
  pthread_mutex_lock(jack_devout_mutex);

  delay_counter = jack_devout->delay_counter;
  
  pthread_mutex_unlock(jack_devout_mutex);

  return(delay_counter);
}

void
ags_jack_devout_set_note_offset(AgsSoundcard *soundcard,
				guint note_offset)
{
  AgsJackDevout *jack_devout;

  pthread_mutex_t *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(jack_devout_mutex);

  jack_devout->note_offset = note_offset;

  pthread_mutex_unlock(jack_devout_mutex);
}

guint
ags_jack_devout_get_note_offset(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  guint note_offset;
  
  pthread_mutex_t *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(jack_devout_mutex);

  note_offset = jack_devout->note_offset;

  pthread_mutex_unlock(jack_devout_mutex);

  return(note_offset);
}

void
ags_jack_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
					 guint note_offset_absolute)
{
  AgsJackDevout *jack_devout;
  
  pthread_mutex_t *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(jack_devout_mutex);

  jack_devout->note_offset_absolute = note_offset_absolute;

  pthread_mutex_unlock(jack_devout_mutex);
}

guint
ags_jack_devout_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  guint note_offset_absolute;
  
  pthread_mutex_t *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(jack_devout_mutex);

  note_offset_absolute = jack_devout->note_offset_absolute;

  pthread_mutex_unlock(jack_devout_mutex);

  return(note_offset_absolute);
}

void
ags_jack_devout_set_loop(AgsSoundcard *soundcard,
			 guint loop_left, guint loop_right,
			 gboolean do_loop)
{
  AgsJackDevout *jack_devout;

  pthread_mutex_t *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* set loop */
  pthread_mutex_lock(jack_devout_mutex);

  jack_devout->loop_left = loop_left;
  jack_devout->loop_right = loop_right;
  jack_devout->do_loop = do_loop;

  if(do_loop){
    jack_devout->loop_offset = jack_devout->note_offset;
  }

  pthread_mutex_unlock(jack_devout_mutex);
}

void
ags_jack_devout_get_loop(AgsSoundcard *soundcard,
			 guint *loop_left, guint *loop_right,
			 gboolean *do_loop)
{
  AgsJackDevout *jack_devout;

  pthread_mutex_t *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* get loop */
  pthread_mutex_lock(jack_devout_mutex);

  if(loop_left != NULL){
    *loop_left = jack_devout->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = jack_devout->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = jack_devout->do_loop;
  }

  pthread_mutex_unlock(jack_devout_mutex);
}

guint
ags_jack_devout_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  guint loop_offset;
  
  pthread_mutex_t *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* get loop offset */
  pthread_mutex_lock(jack_devout_mutex);

  loop_offset = jack_devout->loop_offset;
  
  pthread_mutex_unlock(jack_devout_mutex);

  return(loop_offset);
}

/**
 * ags_jack_devout_switch_buffer_flag:
 * @jack_devout: an #AgsJackDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 2.0.0
 */
void
ags_jack_devout_switch_buffer_flag(AgsJackDevout *jack_devout)
{
  pthread_mutex_t *jack_devout_mutex;
  
  if(!AGS_IS_JACK_DEVOUT(jack_devout)){
    return;
  }

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* switch buffer flag */
  pthread_mutex_lock(jack_devout_mutex);

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

  pthread_mutex_unlock(jack_devout_mutex);
}

/**
 * ags_jack_devout_adjust_delay_and_attack:
 * @jack_devout: the #AgsJackDevout
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 2.0.0
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

  pthread_mutex_t *jack_devout_mutex;

  if(!AGS_IS_JACK_DEVOUT(jack_devout)){
    return;
  }

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());
  
  /* get some initial values */
  delay = ags_jack_devout_get_absolute_delay(AGS_SOUNDCARD(jack_devout));

#ifdef AGS_DEBUG
  g_message("delay : %f", delay);
#endif
  
  pthread_mutex_lock(jack_devout_mutex);

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

  pthread_mutex_unlock(jack_devout_mutex);
}

/**
 * ags_jack_devout_realloc_buffer:
 * @jack_devout: the #AgsJackDevout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 2.0.0
 */
void
ags_jack_devout_realloc_buffer(AgsJackDevout *jack_devout)
{
  AgsJackClient *jack_client;
  
  guint port_count;
  guint pcm_channels;
  guint buffer_size;
  guint format;
  guint word_size;

  pthread_mutex_t *jack_devout_mutex;  

  if(!AGS_IS_JACK_DEVOUT(jack_devout)){
    return;
  }

  /* get jack devout mutex */
  pthread_mutex_lock(ags_jack_devout_get_class_mutex());
  
  jack_devout_mutex = jack_devout->obj_mutex;
  
  pthread_mutex_unlock(ags_jack_devout_get_class_mutex());

  /* get word size */  
  pthread_mutex_lock(jack_devout_mutex);

  jack_client = jack_devout->jack_client;
  
  port_count = g_list_length(jack_devout->jack_port);
  
  pcm_channels = jack_devout->pcm_channels;
  buffer_size = jack_devout->buffer_size;

  format = jack_devout->format;
  
  pthread_mutex_unlock(jack_devout_mutex);

  switch(format){
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
    g_warning("ags_jack_devout_realloc_buffer(): unsupported word size");
    return;
  }

  if(port_count < pcm_channels){
    AgsJackPort *jack_port;

    gchar *str;
    
    guint nth_soundcard;
    guint i;

    pthread_mutex_lock(jack_devout_mutex);

    if(jack_devout->card_uri != NULL){
      sscanf(jack_devout->card_uri,
	     "ags-jack-devout-%u",
	     &nth_soundcard);
    }else{
      pthread_mutex_unlock(jack_devout_mutex);

      g_warning("ags_jack_devout_realloc_buffer() - card uri not set");
      
      return;
    }

    pthread_mutex_unlock(jack_devout_mutex);
    
    for(i = port_count; i < pcm_channels; i++){
      str = g_strdup_printf("ags-soundcard%d-%04d",
			    nth_soundcard,
			    i);
      
      jack_port = ags_jack_port_new(jack_client);
      ags_jack_client_add_port((AgsJackClient *) jack_client,
			       (GObject *) jack_port);

      pthread_mutex_lock(jack_devout_mutex);
    
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
      
      pthread_mutex_unlock(jack_devout_mutex);
      
      ags_jack_port_register(jack_port,
			     str,
			     TRUE, FALSE,
			     TRUE);
    }

    jack_devout->port_name[jack_devout->pcm_channels] = NULL;    
  }else if(port_count > pcm_channels){
    GList *jack_port_start, *jack_port;

    guint i;

    pthread_mutex_lock(jack_devout_mutex);

    jack_port =
      jack_port_start = g_list_copy(jack_devout->jack_port);

    pthread_mutex_unlock(jack_devout_mutex);

    for(i = 0; i < port_count - pcm_channels; i++){
      jack_devout->jack_port = g_list_remove(jack_devout->jack_port,
					     jack_port->data);
      ags_jack_port_unregister(jack_port->data);
      
      g_object_unref(jack_port->data);
      
      jack_port = jack_port->next;
    }

    g_list_free(jack_port_start);

    pthread_mutex_lock(jack_devout_mutex);
    
    jack_devout->port_name = (gchar **) realloc(jack_devout->port_name,
					        (jack_devout->pcm_channels + 1) * sizeof(gchar *));
    jack_devout->port_name[jack_devout->pcm_channels] = NULL;

    pthread_mutex_unlock(jack_devout_mutex);
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
 * Creates a new instance of #AgsJackDevout.
 *
 * Returns: the new #AgsJackDevout
 *
 * Since: 2.0.0
 */
AgsJackDevout*
ags_jack_devout_new(AgsApplicationContext *application_context)
{
  AgsJackDevout *jack_devout;

  jack_devout = (AgsJackDevout *) g_object_new(AGS_TYPE_JACK_DEVOUT,
					       "application-context", application_context,
					       NULL);
  
  return(jack_devout);
}
