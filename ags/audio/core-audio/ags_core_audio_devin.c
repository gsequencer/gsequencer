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
guint ags_core_audio_devin_get_capability(AgsSoundcard *soundcard);

gboolean ags_core_audio_devin_is_starting(AgsSoundcard *soundcard);
gboolean ags_core_audio_devin_is_recording(AgsSoundcard *soundcard);

gchar* ags_core_audio_devin_get_uptime(AgsSoundcard *soundcard);

void ags_core_audio_devin_port_init(AgsSoundcard *soundcard,
				    GError **error);
void ags_core_audio_devin_port_record(AgsSoundcard *soundcard,
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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_core_audio_devin = 0;

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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_core_audio_devin);
  }

  return g_define_type_id__volatile;
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
  soundcard->set_application_context = ags_core_audio_devin_set_application_context;
  soundcard->get_application_context = ags_core_audio_devin_get_application_context;

  soundcard->set_device = ags_core_audio_devin_set_device;
  soundcard->get_device = ags_core_audio_devin_get_device;
  
  soundcard->set_presets = ags_core_audio_devin_set_presets;
  soundcard->get_presets = ags_core_audio_devin_get_presets;

  soundcard->list_cards = ags_core_audio_devin_list_cards;
  soundcard->pcm_info = ags_core_audio_devin_pcm_info;
  soundcard->get_capability = ags_core_audio_devin_get_capability;

  soundcard->get_poll_fd = NULL;
  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_core_audio_devin_is_starting;
  soundcard->is_playing = NULL;
  soundcard->is_recording = ags_core_audio_devin_is_recording;

  soundcard->get_uptime = ags_core_audio_devin_get_uptime;
  
  soundcard->play_init = NULL;
  soundcard->play = NULL;

  soundcard->record_init = ags_core_audio_devin_port_init;
  soundcard->record = ags_core_audio_devin_port_record;

  soundcard->stop = ags_core_audio_devin_port_free;

  soundcard->tic = ags_core_audio_devin_tic;
  soundcard->offset_changed = ags_core_audio_devin_offset_changed;
    
  soundcard->set_bpm = ags_core_audio_devin_set_bpm;
  soundcard->get_bpm = ags_core_audio_devin_get_bpm;

  soundcard->set_delay_factor = ags_core_audio_devin_set_delay_factor;
  soundcard->get_delay_factor = ags_core_audio_devin_get_delay_factor;
  
  soundcard->get_absolute_delay = ags_core_audio_devin_get_absolute_delay;

  soundcard->get_delay = ags_core_audio_devin_get_delay;
  soundcard->get_attack = ags_core_audio_devin_get_attack;

  soundcard->get_buffer = ags_core_audio_devin_get_buffer;
  soundcard->get_next_buffer = ags_core_audio_devin_get_next_buffer;
  soundcard->get_prev_buffer = ags_core_audio_devin_get_prev_buffer;

  soundcard->get_delay_counter = ags_core_audio_devin_get_delay_counter;

  soundcard->set_note_offset = ags_core_audio_devin_set_note_offset;
  soundcard->get_note_offset = ags_core_audio_devin_get_note_offset;

  soundcard->set_note_offset_absolute = ags_core_audio_devin_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_core_audio_devin_get_note_offset_absolute;

  soundcard->set_loop = ags_core_audio_devin_set_loop;
  soundcard->get_loop = ags_core_audio_devin_get_loop;

  soundcard->get_loop_offset = ags_core_audio_devin_get_loop_offset;
}

void
ags_core_audio_devin_init(AgsCoreAudioDevin *core_audio_devin)
{
  AgsConfig *config;

  gchar *str;
  gchar *segmentation;

  guint denumerator, numerator;
  guint i;
  
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

  /* uuid */
  core_audio_devin->uuid = ags_uuid_alloc();
  ags_uuid_generate(core_audio_devin->uuid);

  /* presets */
  config = ags_config_get_instance();

  core_audio_devin->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  core_audio_devin->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  core_audio_devin->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  core_audio_devin->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  core_audio_devin->format = ags_soundcard_helper_config_get_format(config);

  /*  */
  core_audio_devin->card_uri = NULL;
  core_audio_devin->core_audio_client = NULL;

  core_audio_devin->port_name = NULL;
  core_audio_devin->core_audio_port = NULL;

  /* buffer */
  core_audio_devin->buffer_mutex = (pthread_mutex_t **) malloc(8 * sizeof(pthread_mutex_t *));

  for(i = 0; i < 8; i++){
    core_audio_devin->buffer_mutex[i] = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));

    pthread_mutex_init(core_audio_devin->buffer_mutex[i],
		       NULL);
  }

  core_audio_devin->buffer = (void **) malloc(8 * sizeof(void*));

  core_audio_devin->buffer[0] = NULL;
  core_audio_devin->buffer[1] = NULL;
  core_audio_devin->buffer[2] = NULL;
  core_audio_devin->buffer[3] = NULL;
  core_audio_devin->buffer[4] = NULL;
  core_audio_devin->buffer[5] = NULL;
  core_audio_devin->buffer[6] = NULL;
  core_audio_devin->buffer[7] = NULL;
  
  ags_core_audio_devin_realloc_buffer(core_audio_devin);
  
  /* bpm */
  core_audio_devin->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  core_audio_devin->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denumerator,
	   &numerator);
    
    core_audio_devin->delay_factor = 1.0 / numerator * (numerator / denumerator);

    g_free(segmentation);
  }

  /* delay and attack */
  core_audio_devin->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
						sizeof(gdouble));
  
  core_audio_devin->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					       sizeof(guint));

  ags_core_audio_devin_adjust_delay_and_attack(core_audio_devin);
  
  /* counters */
  core_audio_devin->tact_counter = 0.0;
  core_audio_devin->delay_counter = 0;
  core_audio_devin->tic_counter = 0;

  core_audio_devin->note_offset = 0;
  core_audio_devin->note_offset_absolute = 0;

  core_audio_devin->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  core_audio_devin->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  core_audio_devin->do_loop = FALSE;

  core_audio_devin->loop_offset = 0;

  /* callback mutex */
  core_audio_devin->callback_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(core_audio_devin->callback_mutex,
		     NULL);

  core_audio_devin->callback_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(core_audio_devin->callback_cond, NULL);

  /* callback finish mutex */
  core_audio_devin->callback_finish_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(core_audio_devin->callback_finish_mutex,
		     NULL);

  core_audio_devin->callback_finish_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(core_audio_devin->callback_finish_cond, NULL);

  /*  */
  core_audio_devin->notify_soundcard = NULL;
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
 * ags_core_audio_devin_test_flags:
 * @core_audio_devin: the #AgsCoreAudioDevin
 * @flags: the flags
 *
 * Test @flags to be set on @core_audio_devin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.0.0
 */
gboolean
ags_core_audio_devin_test_flags(AgsCoreAudioDevin *core_audio_devin, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *core_audio_devin_mutex;

  if(!AGS_IS_CORE_AUDIO_DEVIN(core_audio_devin)){
    return(FALSE);
  }

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* test */
  pthread_mutex_lock(core_audio_devin_mutex);

  retval = (flags & (core_audio_devin->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(core_audio_devin_mutex);

  return(retval);
}

/**
 * ags_core_audio_devin_set_flags:
 * @core_audio_devin: the #AgsCoreAudioDevin
 * @flags: see #AgsCoreAudioDevinFlags-enum
 *
 * Enable a feature of @core_audio_devin.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_devin_set_flags(AgsCoreAudioDevin *core_audio_devin, guint flags)
{
  pthread_mutex_t *core_audio_devin_mutex;

  if(!AGS_IS_CORE_AUDIO_DEVIN(core_audio_devin)){
    return;
  }

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->flags |= flags;
  
  pthread_mutex_unlock(core_audio_devin_mutex);
}
    
/**
 * ags_core_audio_devin_unset_flags:
 * @core_audio_devin: the #AgsCoreAudioDevin
 * @flags: see #AgsCoreAudioDevinFlags-enum
 *
 * Disable a feature of @core_audio_devin.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_devin_unset_flags(AgsCoreAudioDevin *core_audio_devin, guint flags)
{  
  pthread_mutex_t *core_audio_devin_mutex;

  if(!AGS_IS_CORE_AUDIO_DEVIN(core_audio_devin)){
    return;
  }

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->flags &= (~flags);
  
  pthread_mutex_unlock(core_audio_devin_mutex);
}

void
ags_core_audio_devin_set_application_context(AgsSoundcard *soundcard,
					     AgsApplicationContext *application_context)
{
  AgsCoreAudioDevin *core_audio_devin;

  pthread_mutex_t *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* set application context */
  pthread_mutex_lock(core_audio_devin_mutex);
  
  core_audio_devin->application_context = (GObject *) application_context;
  
  pthread_mutex_unlock(core_audio_devin_mutex);
}

AgsApplicationContext*
ags_core_audio_devin_get_application_context(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  AgsApplicationContext *application_context;
  
  pthread_mutex_t *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* get application context */
  pthread_mutex_lock(core_audio_devin_mutex);

  application_context = (AgsApplicationContext *) core_audio_devin->application_context;

  pthread_mutex_unlock(core_audio_devin_mutex);
  
  return(application_context);
}

void
ags_core_audio_devin_set_device(AgsSoundcard *soundcard,
				gchar *device)
{
  AgsCoreAudioDevin *core_audio_devin;

  GList *core_audio_port, *core_audio_port_start;

  gchar *str;

  guint pcm_channels;
  int ret;
  guint nth_card;
  guint i;
  
  pthread_mutex_t *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* check device */
  pthread_mutex_lock(core_audio_devin_mutex);

  if(core_audio_devin->card_uri == device ||
     !g_ascii_strcasecmp(core_audio_devin->card_uri,
			 device)){
    pthread_mutex_unlock(core_audio_devin_mutex);
  
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-core-audio-devin-")){
    pthread_mutex_unlock(core_audio_devin_mutex);

    g_warning("invalid CoreAudio device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-core-audio-devin-%u",
	       &nth_card);

  if(ret != 1){
    pthread_mutex_unlock(core_audio_devin_mutex);

    g_warning("invalid CoreAudio device specifier");

    return;
  }

  g_free(core_audio_devin->card_uri);
  core_audio_devin->card_uri = g_strdup(device);

  /* apply name to port */
  pcm_channels = core_audio_devin->pcm_channels;
  
  core_audio_port_start = 
    core_audio_port = g_list_copy(core_audio_devin->core_audio_port);

  pthread_mutex_unlock(core_audio_devin_mutex);
  
  for(i = 0; i < pcm_channels && core_audio_port != NULL; i++){
    str = g_strdup_printf("ags-soundcard%d-%04d",
			  nth_card,
			  i);
    
    g_object_set(core_audio_port->data,
		 "port-name", str,
		 NULL);
    g_free(str);

    core_audio_port = core_audio_port->next;
  }

  g_list_free(core_audio_port_start);
}

gchar*
ags_core_audio_devin_get_device(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;
  
  gchar *device;

  pthread_mutex_t *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);
  
  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  device = NULL;

  /* get device */
  pthread_mutex_lock(core_audio_devin_mutex);

  device = g_strdup(core_audio_devin->card_uri);

  pthread_mutex_unlock(core_audio_devin_mutex);
  
  return(device);
}

void
ags_core_audio_devin_set_presets(AgsSoundcard *soundcard,
				 guint channels,
				 guint rate,
				 guint buffer_size,
				 guint format)
{
  AgsCoreAudioDevin *core_audio_devin;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  g_object_set(core_audio_devin,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_core_audio_devin_get_presets(AgsSoundcard *soundcard,
				 guint *channels,
				 guint *rate,
				 guint *buffer_size,
				 guint *format)
{
  AgsCoreAudioDevin *core_audio_devin;

  pthread_mutex_t *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);
  
  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* get presets */
  pthread_mutex_lock(core_audio_devin_mutex);

  if(channels != NULL){
    *channels = core_audio_devin->pcm_channels;
  }

  if(rate != NULL){
    *rate = core_audio_devin->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = core_audio_devin->buffer_size;
  }

  if(format != NULL){
    *format = core_audio_devin->format;
  }

  pthread_mutex_unlock(core_audio_devin_mutex);
}

/**
 * ags_core_audio_devin_list_cards:
 * @soundcard: the #AgsSoundcard
 * @card_id: CORE_AUDIO identifier
 * @card_name: card name
 *
 * List available soundcards.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_devin_list_cards(AgsSoundcard *soundcard,
				GList **card_id, GList **card_name)
{
  AgsCoreAudioClient *core_audio_client;
  AgsCoreAudioDevin *core_audio_devin;

  AgsApplicationContext *application_context;
  
  GList *list_start, *list;

  gchar *card_uri;
  gchar *client_name;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  g_object_get(core_audio_devin,
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
    if(AGS_IS_CORE_AUDIO_DEVIN(list->data)){
      if(card_id != NULL){
	card_uri = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));
	
	if(AGS_CORE_AUDIO_DEVIN(list->data)->card_uri != NULL){
	  *card_id = g_list_prepend(*card_id,
				    card_uri);
	}else{
	  *card_id = g_list_prepend(*card_id,
				    g_strdup("(null)"));

	  g_warning("ags_core_audio_devin_list_cards() - card id (null)");
	}
      }

      if(card_name != NULL){
	g_object_get(list->data,
		     "core_audio-client", &core_audio_client,
		     NULL);
	
	if(core_audio_client != NULL){
	  pthread_mutex_t *core_audio_client_mutex;
	  
	  /* get core_audio client mutex */
	  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
	  core_audio_client_mutex = core_audio_client->obj_mutex;
  
	  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

	  /* get client name */
	  pthread_mutex_lock(core_audio_client_mutex);

	  client_name = g_strdup(core_audio_client->client_name);

	  pthread_mutex_unlock(core_audio_client_mutex);
	  
	  *card_name = g_list_prepend(*card_name,
				      client_name);
	}else{
	  *card_name = g_list_prepend(*card_name,
				      g_strdup("(null)"));

	  g_warning("ags_core_audio_devin_list_cards() - CORE_AUDIO client not connected (null)");
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
ags_core_audio_devin_pcm_info(AgsSoundcard *soundcard,
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
ags_core_audio_devin_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_CAPTURE);
}

gboolean
ags_core_audio_devin_is_starting(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  gboolean is_starting;
  
  pthread_mutex_t *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* check is starting */
  pthread_mutex_lock(core_audio_devin_mutex);

  is_starting = ((AGS_CORE_AUDIO_DEVIN_START_RECORD & (core_audio_devin->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(core_audio_devin_mutex);
  
  return(is_starting);
}

gboolean
ags_core_audio_devin_is_recording(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  gboolean is_playing;
  
  pthread_mutex_t *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);
  
  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* check is starting */
  pthread_mutex_lock(core_audio_devin_mutex);

  is_playing = ((AGS_CORE_AUDIO_DEVIN_RECORD & (core_audio_devin->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(core_audio_devin_mutex);

  return(is_playing);
}

gchar*
ags_core_audio_devin_get_uptime(AgsSoundcard *soundcard)
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
ags_core_audio_devin_port_init(AgsSoundcard *soundcard,
			       GError **error)
{
  AgsCoreAudioDevin *core_audio_devin;

  guint format, word_size;
  
  pthread_mutex_t *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core-audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* retrieve word size */
  pthread_mutex_lock(core_audio_devin_mutex);

  switch(core_audio_devin->format){
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
    pthread_mutex_unlock(core_audio_devin_mutex);
    
    g_warning("ags_core_audio_devin_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for playback */
  core_audio_devin->flags |= (AGS_CORE_AUDIO_DEVIN_BUFFER7 |
			       AGS_CORE_AUDIO_DEVIN_START_RECORD |
			       AGS_CORE_AUDIO_DEVIN_RECORD |
			       AGS_CORE_AUDIO_DEVIN_NONBLOCKING);

  memset(core_audio_devin->buffer[0], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->buffer[1], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->buffer[2], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->buffer[3], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->buffer[4], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->buffer[5], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->buffer[6], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->buffer[7], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);

  /*  */
  core_audio_devin->tact_counter = 0.0;
  core_audio_devin->delay_counter = 0.0;
  core_audio_devin->tic_counter = 0;

  core_audio_devin->flags |= (AGS_CORE_AUDIO_DEVIN_INITIALIZED |
			       AGS_CORE_AUDIO_DEVIN_START_RECORD |
			       AGS_CORE_AUDIO_DEVIN_RECORD);
  
  g_atomic_int_or(&(core_audio_devin->sync_flags),
		  AGS_CORE_AUDIO_DEVIN_INITIAL_CALLBACK);

  pthread_mutex_unlock(core_audio_devin_mutex);
}

void
ags_core_audio_devin_port_record(AgsSoundcard *soundcard,
				 GError **error)
{
  AgsCoreAudioClient *core_audio_client;
  AgsCoreAudioDevin *core_audio_devin;

  AgsNotifySoundcard *notify_soundcard;
  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
      
  AgsTaskThread *task_thread;

  AgsApplicationContext *application_context;

  GList *task;
  guint word_size;
  gboolean core_audio_client_activated;

  pthread_mutex_t *core_audio_devin_mutex;
  pthread_mutex_t *core_audio_client_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);
  application_context = ags_soundcard_get_application_context(soundcard);
  
  /* get core-audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* client */
  pthread_mutex_lock(core_audio_devin_mutex);

  core_audio_client = (AgsCoreAudioClient *) core_audio_devin->core_audio_client;
  
  callback_mutex = core_audio_devin->callback_mutex;
  callback_finish_mutex = core_audio_devin->callback_finish_mutex;

  /* do playback */  
  core_audio_devin->flags &= (~AGS_CORE_AUDIO_DEVIN_START_RECORD);
  notify_soundcard = AGS_NOTIFY_SOUNDCARD(core_audio_devin->notify_soundcard);
  
  switch(core_audio_devin->format){
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
  default:
    pthread_mutex_unlock(core_audio_devin_mutex);
    
    g_warning("ags_core_audio_devin_port_record(): unsupported word size");
    
    return;
  }

  pthread_mutex_unlock(core_audio_devin_mutex);

  /* get client mutex */
  pthread_mutex_lock(ags_core_audio_client_get_class_mutex());
  
  core_audio_client_mutex = core_audio_client->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_client_get_class_mutex());

  /* get activated */
  pthread_mutex_lock(core_audio_client_mutex);

  core_audio_client_activated = ((AGS_CORE_AUDIO_CLIENT_ACTIVATED & (core_audio_client->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(core_audio_client_mutex);

  if(core_audio_client_activated){
    while((AGS_CORE_AUDIO_DEVIN_PASS_THROUGH & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
	usleep(4);
    }
    
    /* signal */
    if((AGS_CORE_AUDIO_DEVIN_INITIAL_CALLBACK & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) == 0){
      pthread_mutex_lock(callback_mutex);

      g_atomic_int_or(&(core_audio_devin->sync_flags),
		      AGS_CORE_AUDIO_DEVIN_CALLBACK_DONE);
    
      if((AGS_CORE_AUDIO_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
	pthread_cond_signal(core_audio_devin->callback_cond);
      }

      pthread_mutex_unlock(callback_mutex);
      //    }
    
    /* wait callback */	
      pthread_mutex_lock(callback_finish_mutex);
    
      if((AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) == 0){
	g_atomic_int_or(&(core_audio_devin->sync_flags),
			AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_WAIT);
    
	while((AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) == 0 &&
	      (AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
	  pthread_cond_wait(core_audio_devin->callback_finish_cond,
			    callback_finish_mutex);
	}
      }
      
      g_atomic_int_and(&(core_audio_devin->sync_flags),
		       (~(AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_WAIT |
			  AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_DONE)));
    
      pthread_mutex_unlock(callback_finish_mutex);
    }else{
      g_atomic_int_and(&(core_audio_devin->sync_flags),
		       (~AGS_CORE_AUDIO_DEVIN_INITIAL_CALLBACK));
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
  tic_device = ags_tic_device_new((GObject *) core_audio_devin);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) core_audio_devin);
  task = g_list_append(task,
		       clear_buffer);
    
  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) core_audio_devin);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_thread_append_tasks((AgsTaskThread *) task_thread,
			       task);
}

void
ags_core_audio_devin_port_free(AgsSoundcard *soundcard)
{
  AgsCoreAudioPort *core_audio_port;
  AgsCoreAudioDevin *core_audio_devin;

  AgsNotifySoundcard *notify_soundcard;

  guint word_size;

  pthread_mutex_t *core_audio_devin_mutex;
  pthread_mutex_t *callback_mutex;
  pthread_mutex_t *callback_finish_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);
  
  /* get core-audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /*  */
  pthread_mutex_lock(core_audio_devin_mutex);

  notify_soundcard = AGS_NOTIFY_SOUNDCARD(core_audio_devin->notify_soundcard);

  if((AGS_CORE_AUDIO_DEVIN_INITIALIZED & (core_audio_devin->flags)) == 0){
    pthread_mutex_unlock(core_audio_devin_mutex);

    return;
  }

  g_object_ref(notify_soundcard);

  callback_mutex = core_audio_devin->callback_mutex;
  callback_finish_mutex = core_audio_devin->callback_finish_mutex;
  
  //  g_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);
  
  core_audio_devin->flags &= (~(AGS_CORE_AUDIO_DEVIN_BUFFER0 |
				 AGS_CORE_AUDIO_DEVIN_BUFFER1 |
				 AGS_CORE_AUDIO_DEVIN_BUFFER2 |
				 AGS_CORE_AUDIO_DEVIN_BUFFER3 |
				 AGS_CORE_AUDIO_DEVIN_BUFFER4 |
				 AGS_CORE_AUDIO_DEVIN_BUFFER5 |
				 AGS_CORE_AUDIO_DEVIN_BUFFER6 |
				 AGS_CORE_AUDIO_DEVIN_BUFFER7 |
				 AGS_CORE_AUDIO_DEVIN_RECORD));

  g_atomic_int_or(&(core_audio_devin->sync_flags),
		  AGS_CORE_AUDIO_DEVIN_PASS_THROUGH);
  g_atomic_int_and(&(core_audio_devin->sync_flags),
		   (~AGS_CORE_AUDIO_DEVIN_INITIAL_CALLBACK));

  /* signal callback */
  pthread_mutex_lock(callback_mutex);

  g_atomic_int_or(&(core_audio_devin->sync_flags),
		  AGS_CORE_AUDIO_DEVIN_CALLBACK_DONE);
    
  if((AGS_CORE_AUDIO_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
    pthread_cond_signal(core_audio_devin->callback_cond);
  }

  pthread_mutex_unlock(callback_mutex);

  /* signal thread */
  pthread_mutex_lock(callback_finish_mutex);

  g_atomic_int_or(&(core_audio_devin->sync_flags),
		  AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_DONE);
    
  if((AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
    pthread_cond_signal(core_audio_devin->callback_finish_cond);
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
  core_audio_devin->note_offset = 0;
  core_audio_devin->note_offset_absolute = 0;

  switch(core_audio_devin->format){
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
    
    g_critical("ags_core_audio_devin_free(): unsupported word size");
  }

  pthread_mutex_unlock(core_audio_devin_mutex);

  if(core_audio_devin->core_audio_port != NULL){
    core_audio_port = core_audio_devin->core_audio_port->data;

    while(!g_atomic_int_get(&(core_audio_port->is_empty))) usleep(500000);
  }

  pthread_mutex_lock(core_audio_devin_mutex);
  
  memset(core_audio_devin->buffer[0], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->buffer[1], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->buffer[2], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->buffer[3], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->buffer[4], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->buffer[5], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->buffer[6], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->buffer[7], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);

  pthread_mutex_unlock(core_audio_devin_mutex);
}

void
ags_core_audio_devin_tic(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  gdouble delay;
  gdouble delay_counter;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  
  pthread_mutex_t *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());
  
  /* determine if attack should be switched */
  pthread_mutex_lock(core_audio_devin_mutex);

  delay = core_audio_devin->delay[core_audio_devin->tic_counter];
  delay_counter = core_audio_devin->delay_counter;

  note_offset = core_audio_devin->note_offset;
  note_offset_absolute = core_audio_devin->note_offset_absolute;
  
  loop_left = core_audio_devin->loop_left;
  loop_right = core_audio_devin->loop_right;
  
  do_loop = core_audio_devin->do_loop;

  pthread_mutex_unlock(core_audio_devin_mutex);

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
    pthread_mutex_lock(core_audio_devin_mutex);
    
    core_audio_devin->delay_counter = 0.0;
    core_audio_devin->tact_counter += 1.0;

    pthread_mutex_unlock(core_audio_devin_mutex);
  }else{
    pthread_mutex_lock(core_audio_devin_mutex);
    
    core_audio_devin->delay_counter += 1.0;

    pthread_mutex_unlock(core_audio_devin_mutex);
  }
}

void
ags_core_audio_devin_offset_changed(AgsSoundcard *soundcard,
				    guint note_offset)
{
  AgsCoreAudioDevin *core_audio_devin;
  
  pthread_mutex_t *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* offset changed */
  pthread_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->tic_counter += 1;

  if(core_audio_devin->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    core_audio_devin->tic_counter = 0;
  }

  pthread_mutex_unlock(core_audio_devin_mutex);
}

void
ags_core_audio_devin_set_bpm(AgsSoundcard *soundcard,
			     gdouble bpm)
{
  AgsCoreAudioDevin *core_audio_devin;

  pthread_mutex_t *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* set bpm */
  pthread_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->bpm = bpm;

  pthread_mutex_unlock(core_audio_devin_mutex);

  ags_core_audio_devin_adjust_delay_and_attack(core_audio_devin);
}

gdouble
ags_core_audio_devin_get_bpm(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  gdouble bpm;
  
  pthread_mutex_t *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* get bpm */
  pthread_mutex_lock(core_audio_devin_mutex);

  bpm = core_audio_devin->bpm;
  
  pthread_mutex_unlock(core_audio_devin_mutex);

  return(bpm);
}

void
ags_core_audio_devin_set_delay_factor(AgsSoundcard *soundcard,
				      gdouble delay_factor)
{
  AgsCoreAudioDevin *core_audio_devin;

  pthread_mutex_t *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* set delay factor */
  pthread_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->delay_factor = delay_factor;

  pthread_mutex_unlock(core_audio_devin_mutex);

  ags_core_audio_devin_adjust_delay_and_attack(core_audio_devin);
}

gdouble
ags_core_audio_devin_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  gdouble delay_factor;
  
  pthread_mutex_t *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* get delay factor */
  pthread_mutex_lock(core_audio_devin_mutex);

  delay_factor = core_audio_devin->delay_factor;
  
  pthread_mutex_unlock(core_audio_devin_mutex);

  return(delay_factor);
}

gdouble
ags_core_audio_devin_get_delay(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  guint delay_index;
  gdouble delay;
  
  pthread_mutex_t *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* get delay */
  pthread_mutex_lock(core_audio_devin_mutex);

  delay_index = core_audio_devin->tic_counter;

  delay = core_audio_devin->delay[delay_index];
  
  pthread_mutex_unlock(core_audio_devin_mutex);
  
  return(delay);
}

gdouble
ags_core_audio_devin_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  gdouble absolute_delay;
  
  pthread_mutex_t *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);
  
  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* get absolute delay */
  pthread_mutex_lock(core_audio_devin_mutex);

  absolute_delay = (60.0 * (((gdouble) core_audio_devin->samplerate / (gdouble) core_audio_devin->buffer_size) / (gdouble) core_audio_devin->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) core_audio_devin->delay_factor)));

  pthread_mutex_unlock(core_audio_devin_mutex);

  return(absolute_delay);
}

guint
ags_core_audio_devin_get_attack(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  guint attack_index;
  guint attack;
  
  pthread_mutex_t *core_audio_devin_mutex;  

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);
  
  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* get attack */
  pthread_mutex_lock(core_audio_devin_mutex);

  attack_index = core_audio_devin->tic_counter;

  attack = core_audio_devin->attack[attack_index];

  pthread_mutex_unlock(core_audio_devin_mutex);
  
  return(attack);
}

void*
ags_core_audio_devin_get_buffer(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  void *buffer;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER0)){
    buffer = core_audio_devin->buffer[0];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER1)){
    buffer = core_audio_devin->buffer[1];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER2)){
    buffer = core_audio_devin->buffer[2];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER3)){
    buffer = core_audio_devin->buffer[3];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER4)){
    buffer = core_audio_devin->buffer[4];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER5)){
    buffer = core_audio_devin->buffer[5];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER6)){
    buffer = core_audio_devin->buffer[6];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER7)){
    buffer = core_audio_devin->buffer[7];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_core_audio_devin_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  void *buffer;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  //  g_message("next - 0x%0x", ((AGS_CORE_AUDIO_DEVIN_BUFFER0 |
  //				AGS_CORE_AUDIO_DEVIN_BUFFER1 |
  //				AGS_CORE_AUDIO_DEVIN_BUFFER2 |
  //				AGS_CORE_AUDIO_DEVIN_BUFFER3) & (core_audio_devin->flags)));

  if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER0)){
    buffer = core_audio_devin->buffer[1];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER1)){
    buffer = core_audio_devin->buffer[2];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER2)){
    buffer = core_audio_devin->buffer[3];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER3)){
    buffer = core_audio_devin->buffer[4];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER4)){
    buffer = core_audio_devin->buffer[5];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER5)){
    buffer = core_audio_devin->buffer[6];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER6)){
    buffer = core_audio_devin->buffer[7];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER7)){
    buffer = core_audio_devin->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_core_audio_devin_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  void *buffer;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER0)){
    buffer = core_audio_devin->buffer[7];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER1)){
    buffer = core_audio_devin->buffer[0];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER2)){
    buffer = core_audio_devin->buffer[1];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER3)){
    buffer = core_audio_devin->buffer[2];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER4)){
    buffer = core_audio_devin->buffer[3];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER5)){
    buffer = core_audio_devin->buffer[4];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER6)){
    buffer = core_audio_devin->buffer[5];
  }else if(ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_BUFFER7)){
    buffer = core_audio_devin->buffer[6];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

guint
ags_core_audio_devin_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  guint delay_counter;
  
  pthread_mutex_t *core_audio_devin_mutex;  

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);
  
  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* delay counter */
  pthread_mutex_lock(core_audio_devin_mutex);

  delay_counter = core_audio_devin->delay_counter;
  
  pthread_mutex_unlock(core_audio_devin_mutex);

  return(delay_counter);
}

void
ags_core_audio_devin_set_note_offset(AgsSoundcard *soundcard,
				     guint note_offset)
{
  AgsCoreAudioDevin *core_audio_devin;

  pthread_mutex_t *core_audio_devin_mutex;  

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->note_offset = note_offset;

  pthread_mutex_unlock(core_audio_devin_mutex);
}

guint
ags_core_audio_devin_get_note_offset(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  guint note_offset;
  
  pthread_mutex_t *core_audio_devin_mutex;  

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(core_audio_devin_mutex);

  note_offset = core_audio_devin->note_offset;

  pthread_mutex_unlock(core_audio_devin_mutex);

  return(note_offset);
}

void
ags_core_audio_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
					      guint note_offset_absolute)
{
  AgsCoreAudioDevin *core_audio_devin;
  
  pthread_mutex_t *core_audio_devin_mutex;  

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->note_offset_absolute = note_offset_absolute;

  pthread_mutex_unlock(core_audio_devin_mutex);
}

guint
ags_core_audio_devin_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  guint note_offset_absolute;
  
  pthread_mutex_t *core_audio_devin_mutex;  

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* set note offset */
  pthread_mutex_lock(core_audio_devin_mutex);

  note_offset_absolute = core_audio_devin->note_offset_absolute;

  pthread_mutex_unlock(core_audio_devin_mutex);

  return(note_offset_absolute);
}

void
ags_core_audio_devin_set_loop(AgsSoundcard *soundcard,
			      guint loop_left, guint loop_right,
			      gboolean do_loop)
{
  AgsCoreAudioDevin *core_audio_devin;

  pthread_mutex_t *core_audio_devin_mutex;  

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* set loop */
  pthread_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->loop_left = loop_left;
  core_audio_devin->loop_right = loop_right;
  core_audio_devin->do_loop = do_loop;

  if(do_loop){
    core_audio_devin->loop_offset = core_audio_devin->note_offset;
  }

  pthread_mutex_unlock(core_audio_devin_mutex);
}

void
ags_core_audio_devin_get_loop(AgsSoundcard *soundcard,
			      guint *loop_left, guint *loop_right,
			      gboolean *do_loop)
{
  AgsCoreAudioDevin *core_audio_devin;

  pthread_mutex_t *core_audio_devin_mutex;  

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* get loop */
  pthread_mutex_lock(core_audio_devin_mutex);

  if(loop_left != NULL){
    *loop_left = core_audio_devin->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = core_audio_devin->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = core_audio_devin->do_loop;
  }

  pthread_mutex_unlock(core_audio_devin_mutex);
}

guint
ags_core_audio_devin_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  guint loop_offset;
  
  pthread_mutex_t *core_audio_devin_mutex;  

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* get loop offset */
  pthread_mutex_lock(core_audio_devin_mutex);

  loop_offset = core_audio_devin->loop_offset;
  
  pthread_mutex_unlock(core_audio_devin_mutex);

  return(loop_offset);
}


/**
 * ags_core_audio_devin_switch_buffer_flag:
 * @core_audio_devin: an #AgsCoreAudioDevin
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 2.0.0
 */
void
ags_core_audio_devin_switch_buffer_flag(AgsCoreAudioDevin *core_audio_devin)
{
  pthread_mutex_t *core_audio_devin_mutex;
  
  if(!AGS_IS_CORE_AUDIO_DEVIN(core_audio_devin)){
    return;
  }

  /* get core_audio devin mutex */
  pthread_mutex_lock(ags_core_audio_devin_get_class_mutex());
  
  core_audio_devin_mutex = core_audio_devin->obj_mutex;
  
  pthread_mutex_unlock(ags_core_audio_devin_get_class_mutex());

  /* switch buffer flag */
  pthread_mutex_lock(core_audio_devin_mutex);

  if((AGS_CORE_AUDIO_DEVIN_BUFFER0 & (core_audio_devin->flags)) != 0){
    core_audio_devin->flags &= (~AGS_CORE_AUDIO_DEVIN_BUFFER0);
    core_audio_devin->flags |= AGS_CORE_AUDIO_DEVIN_BUFFER1;
  }else if((AGS_CORE_AUDIO_DEVIN_BUFFER1 & (core_audio_devin->flags)) != 0){
    core_audio_devin->flags &= (~AGS_CORE_AUDIO_DEVIN_BUFFER1);
    core_audio_devin->flags |= AGS_CORE_AUDIO_DEVIN_BUFFER2;
  }else if((AGS_CORE_AUDIO_DEVIN_BUFFER2 & (core_audio_devin->flags)) != 0){
    core_audio_devin->flags &= (~AGS_CORE_AUDIO_DEVIN_BUFFER2);
    core_audio_devin->flags |= AGS_CORE_AUDIO_DEVIN_BUFFER3;
  }else if((AGS_CORE_AUDIO_DEVIN_BUFFER3 & (core_audio_devin->flags)) != 0){
    core_audio_devin->flags &= (~AGS_CORE_AUDIO_DEVIN_BUFFER3);
    core_audio_devin->flags |= AGS_CORE_AUDIO_DEVIN_BUFFER4;
  }else if((AGS_CORE_AUDIO_DEVIN_BUFFER4 & (core_audio_devin->flags)) != 0){
    core_audio_devin->flags &= (~AGS_CORE_AUDIO_DEVIN_BUFFER4);
    core_audio_devin->flags |= AGS_CORE_AUDIO_DEVIN_BUFFER5;
  }else if((AGS_CORE_AUDIO_DEVIN_BUFFER5 & (core_audio_devin->flags)) != 0){
    core_audio_devin->flags &= (~AGS_CORE_AUDIO_DEVIN_BUFFER5);
    core_audio_devin->flags |= AGS_CORE_AUDIO_DEVIN_BUFFER6;
  }else if((AGS_CORE_AUDIO_DEVIN_BUFFER6 & (core_audio_devin->flags)) != 0){
    core_audio_devin->flags &= (~AGS_CORE_AUDIO_DEVIN_BUFFER6);
    core_audio_devin->flags |= AGS_CORE_AUDIO_DEVIN_BUFFER7;
  }else if((AGS_CORE_AUDIO_DEVIN_BUFFER7 & (core_audio_devin->flags)) != 0){
    core_audio_devin->flags &= (~AGS_CORE_AUDIO_DEVIN_BUFFER7);
    core_audio_devin->flags |= AGS_CORE_AUDIO_DEVIN_BUFFER0;
  }

  pthread_mutex_unlock(core_audio_devin_mutex);
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
