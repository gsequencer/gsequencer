/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#include <ags/audio/wasapi/ags_wasapi_devin.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/task/ags_notify_soundcard.h>
#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <string.h>
#include <math.h>
#include <time.h>

#ifdef AGS_WITH_WASAPI
#include <windows.h>
#include <ole2.h>
#include <ksmedia.h>
#endif

#include <ags/config.h>
#include <ags/i18n.h>

void ags_wasapi_devin_class_init(AgsWasapiDevinClass *wasapi_devin);
void ags_wasapi_devin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_wasapi_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_wasapi_devin_init(AgsWasapiDevin *wasapi_devin);
void ags_wasapi_devin_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_wasapi_devin_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_wasapi_devin_dispose(GObject *gobject);
void ags_wasapi_devin_finalize(GObject *gobject);

AgsUUID* ags_wasapi_devin_get_uuid(AgsConnectable *connectable);
gboolean ags_wasapi_devin_has_resource(AgsConnectable *connectable);
gboolean ags_wasapi_devin_is_ready(AgsConnectable *connectable);
void ags_wasapi_devin_add_to_registry(AgsConnectable *connectable);
void ags_wasapi_devin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_wasapi_devin_list_resource(AgsConnectable *connectable);
xmlNode* ags_wasapi_devin_xml_compose(AgsConnectable *connectable);
void ags_wasapi_devin_xml_parse(AgsConnectable *connectable,
				xmlNode *node);
gboolean ags_wasapi_devin_is_connected(AgsConnectable *connectable);
void ags_wasapi_devin_connect(AgsConnectable *connectable);
void ags_wasapi_devin_disconnect(AgsConnectable *connectable);

void ags_wasapi_devin_set_application_context(AgsSoundcard *soundcard,
					      AgsApplicationContext *application_context);
AgsApplicationContext* ags_wasapi_devin_get_application_context(AgsSoundcard *soundcard);

void ags_wasapi_devin_set_device(AgsSoundcard *soundcard,
				 gchar *device);
gchar* ags_wasapi_devin_get_device(AgsSoundcard *soundcard);

void ags_wasapi_devin_set_presets(AgsSoundcard *soundcard,
				  guint channels,
				  guint rate,
				  guint buffer_size,
				  guint format);
void ags_wasapi_devin_get_presets(AgsSoundcard *soundcard,
				  guint *channels,
				  guint *rate,
				  guint *buffer_size,
				  guint *format);

void ags_wasapi_devin_list_cards(AgsSoundcard *soundcard,
				 GList **card_id, GList **card_name);
void ags_wasapi_devin_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			       guint *channels_min, guint *channels_max,
			       guint *rate_min, guint *rate_max,
			       guint *buffer_size_min, guint *buffer_size_max,
			       GError **error);
guint ags_wasapi_devin_get_capability(AgsSoundcard *soundcard);

gboolean ags_wasapi_devin_is_starting(AgsSoundcard *soundcard);
gboolean ags_wasapi_devin_is_recording(AgsSoundcard *soundcard);

gchar* ags_wasapi_devin_get_uptime(AgsSoundcard *soundcard);

void ags_wasapi_devin_client_init(AgsSoundcard *soundcard,
				GError **error);
void ags_wasapi_devin_client_record(AgsSoundcard *soundcard,
				  GError **error);
void ags_wasapi_devin_client_free(AgsSoundcard *soundcard);

void ags_wasapi_devin_tic(AgsSoundcard *soundcard);
void ags_wasapi_devin_offset_changed(AgsSoundcard *soundcard,
				     guint note_offset);

void ags_wasapi_devin_set_bpm(AgsSoundcard *soundcard,
			      gdouble bpm);
gdouble ags_wasapi_devin_get_bpm(AgsSoundcard *soundcard);

void ags_wasapi_devin_set_delay_factor(AgsSoundcard *soundcard,
				       gdouble delay_factor);
gdouble ags_wasapi_devin_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_wasapi_devin_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_wasapi_devin_get_delay(AgsSoundcard *soundcard);
guint ags_wasapi_devin_get_attack(AgsSoundcard *soundcard);

void* ags_wasapi_devin_get_buffer(AgsSoundcard *soundcard);
void* ags_wasapi_devin_get_next_buffer(AgsSoundcard *soundcard);
void* ags_wasapi_devin_get_prev_buffer(AgsSoundcard *soundcard);

void ags_wasapi_devin_lock_buffer(AgsSoundcard *soundcard,
				  void *buffer);
void ags_wasapi_devin_unlock_buffer(AgsSoundcard *soundcard,
				    void *buffer);

guint ags_wasapi_devin_get_delay_counter(AgsSoundcard *soundcard);

void ags_wasapi_devin_set_start_note_offset(AgsSoundcard *soundcard,
					    guint start_note_offset);
guint ags_wasapi_devin_get_start_note_offset(AgsSoundcard *soundcard);

void ags_wasapi_devin_set_note_offset(AgsSoundcard *soundcard,
				      guint note_offset);
guint ags_wasapi_devin_get_note_offset(AgsSoundcard *soundcard);

void ags_wasapi_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
					       guint note_offset);
guint ags_wasapi_devin_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_wasapi_devin_set_loop(AgsSoundcard *soundcard,
			       guint loop_left, guint loop_right,
			       gboolean do_loop);
void ags_wasapi_devin_get_loop(AgsSoundcard *soundcard,
			       guint *loop_left, guint *loop_right,
			       gboolean *do_loop);

guint ags_wasapi_devin_get_loop_offset(AgsSoundcard *soundcard);

/**
 * SECTION:ags_wasapi_devin
 * @short_description: Output to soundcard
 * @title: AgsWasapiDevin
 * @section_id:
 * @include: ags/audio/wasapi/ags_wasapi_devin.h
 *
 * #AgsWasapiDevin represents a soundcard and supports output.
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
     PROP_CHANNEL,
};

static gpointer ags_wasapi_devin_parent_class = NULL;

static pthread_mutex_t ags_wasapi_devin_class_mutex = PTHREAD_MUTEX_INITIALIZER;

#ifdef AGS_W32API
static const GUID ags_wasapi_clsid_mm_device_enumerator_guid = {0xBCDE0395, 0xE52F, 0x467C, 0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E};
static const GUID ags_wasapi_iid_mm_device_enumerator_guid = {0xA95664D2, 0x9614, 0x4F35, 0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6};
static const GUID ags_wasapi_iid_audio_client_guid = {0x1CB9AD4C, 0xDBFA, 0x4c32, 0xB1, 0x78, 0xC2, 0xF5, 0x68, 0xA7, 0x03, 0xB2};
static const GUID ags_wasapi_iid_audio_capture_client_guid = {0xc8adbd64, 0xe71e, 0x48a0, 0xa4,0xde, 0x18,0x5c,0x39,0x5c,0xd3,0x17};
static const GUID ags_wasapi_pcm_subformat_guid = {0x00000001, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71};
static const GUID ags_wasapi_pkey_device_friendly_name_guid = {0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0, 14};
#endif

GType
ags_wasapi_devin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_wasapi_devin = 0;

    static const GTypeInfo ags_wasapi_devin_info = {
						    sizeof(AgsWasapiDevinClass),
						    NULL, /* base_init */
						    NULL, /* base_finalize */
						    (GClassInitFunc) ags_wasapi_devin_class_init,
						    NULL, /* class_finalize */
						    NULL, /* class_data */
						    sizeof(AgsWasapiDevin),
						    0,    /* n_preallocs */
						    (GInstanceInitFunc) ags_wasapi_devin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
								  (GInterfaceInitFunc) ags_wasapi_devin_connectable_interface_init,
								  NULL, /* interface_finalize */
								  NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
								(GInterfaceInitFunc) ags_wasapi_devin_soundcard_interface_init,
								NULL, /* interface_finalize */
								NULL, /* interface_data */
    };

    ags_type_wasapi_devin = g_type_register_static(G_TYPE_OBJECT,
						   "AgsWasapiDevin",
						   &ags_wasapi_devin_info,
						   0);

    g_type_add_interface_static(ags_type_wasapi_devin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_wasapi_devin,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_wasapi_devin);
  }

  return g_define_type_id__volatile;
}

void
ags_wasapi_devin_class_init(AgsWasapiDevinClass *wasapi_devin)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_wasapi_devin_parent_class = g_type_class_peek_parent(wasapi_devin);

  /* GObjectClass */
  gobject = (GObjectClass *) wasapi_devin;

  gobject->set_property = ags_wasapi_devin_set_property;
  gobject->get_property = ags_wasapi_devin_get_property;

  gobject->dispose = ags_wasapi_devin_dispose;
  gobject->finalize = ags_wasapi_devin_finalize;

  /* properties */
  /**
   * AgsWasapiDevin:application-context:
   *
   * The assigned #AgsApplicationContext
   * 
   * Since: 2.3.4
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
   * AgsWasapiDevin:device:
   *
   * The core audio soundcard indentifier
   * 
   * Since: 2.3.4
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-wasapi-devin-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsWasapiDevin:dsp-channels:
   *
   * The dsp channel count
   * 
   * Since: 2.3.4
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
   * AgsWasapiDevin:pcm-channels:
   *
   * The pcm channel count
   * 
   * Since: 2.3.4
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
   * AgsWasapiDevin:format:
   *
   * The precision of the buffer
   * 
   * Since: 2.3.4
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
   * AgsWasapiDevin:buffer-size:
   *
   * The buffer size
   * 
   * Since: 2.3.4
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
   * AgsWasapiDevin:samplerate:
   *
   * The samplerate
   * 
   * Since: 2.3.4
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
   * AgsWasapiDevin:buffer:
   *
   * The buffer
   * 
   * Since: 2.3.4
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to play"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsWasapiDevin:bpm:
   *
   * Beats per minute
   * 
   * Since: 2.3.4
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
   * AgsWasapiDevin:delay-factor:
   *
   * tact
   * 
   * Since: 2.3.4
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
   * AgsWasapiDevin:attack:
   *
   * Attack of the buffer
   * 
   * Since: 2.3.4
   */
  param_spec = g_param_spec_pointer("attack",
				    i18n_pspec("attack of buffer"),
				    i18n_pspec("The attack to use for the buffer"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

}

GQuark
ags_wasapi_devin_error_quark()
{
  return(g_quark_from_static_string("ags-wasapi_devin-error-quark"));
}

void
ags_wasapi_devin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  //TODO:JK: implement me
}

void
ags_wasapi_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_application_context = ags_wasapi_devin_set_application_context;
  soundcard->get_application_context = ags_wasapi_devin_get_application_context;

  soundcard->set_device = ags_wasapi_devin_set_device;
  soundcard->get_device = ags_wasapi_devin_get_device;
  
  soundcard->set_presets = ags_wasapi_devin_set_presets;
  soundcard->get_presets = ags_wasapi_devin_get_presets;

  soundcard->list_cards = ags_wasapi_devin_list_cards;
  soundcard->pcm_info = ags_wasapi_devin_pcm_info;
  soundcard->get_capability = ags_wasapi_devin_get_capability;

  soundcard->get_poll_fd = NULL;
  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_wasapi_devin_is_starting;
  soundcard->is_playing = NULL;
  soundcard->is_recording = ags_wasapi_devin_is_recording;

  soundcard->get_uptime = ags_wasapi_devin_get_uptime;
  
  soundcard->play_init = NULL;
  soundcard->play = NULL;

  soundcard->record_init = ags_wasapi_devin_client_init;
  soundcard->record = ags_wasapi_devin_client_record;

  soundcard->stop = ags_wasapi_devin_client_free;

  soundcard->tic = ags_wasapi_devin_tic;
  soundcard->offset_changed = ags_wasapi_devin_offset_changed;
    
  soundcard->set_bpm = ags_wasapi_devin_set_bpm;
  soundcard->get_bpm = ags_wasapi_devin_get_bpm;

  soundcard->set_delay_factor = ags_wasapi_devin_set_delay_factor;
  soundcard->get_delay_factor = ags_wasapi_devin_get_delay_factor;
  
  soundcard->get_absolute_delay = ags_wasapi_devin_get_absolute_delay;

  soundcard->get_delay = ags_wasapi_devin_get_delay;
  soundcard->get_attack = ags_wasapi_devin_get_attack;

  soundcard->get_buffer = ags_wasapi_devin_get_buffer;
  soundcard->get_next_buffer = ags_wasapi_devin_get_next_buffer;
  soundcard->get_prev_buffer = ags_wasapi_devin_get_prev_buffer;
  
  soundcard->lock_buffer = ags_wasapi_devin_lock_buffer;
  soundcard->unlock_buffer = ags_wasapi_devin_unlock_buffer;

  soundcard->get_delay_counter = ags_wasapi_devin_get_delay_counter;

  soundcard->set_start_note_offset = ags_wasapi_devin_set_start_note_offset;
  soundcard->get_start_note_offset = ags_wasapi_devin_get_start_note_offset;

  soundcard->set_note_offset = ags_wasapi_devin_set_note_offset;
  soundcard->get_note_offset = ags_wasapi_devin_get_note_offset;

  soundcard->set_note_offset_absolute = ags_wasapi_devin_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_wasapi_devin_get_note_offset_absolute;

  soundcard->set_loop = ags_wasapi_devin_set_loop;
  soundcard->get_loop = ags_wasapi_devin_get_loop;

  soundcard->get_loop_offset = ags_wasapi_devin_get_loop_offset;
}

void
ags_wasapi_devin_init(AgsWasapiDevin *wasapi_devin)
{
  AgsConfig *config;

  gchar *str;
  gchar *segmentation;

  guint denumerator, numerator;
  guint i;
  
  pthread_mutex_t *mutex;
  pthread_mutexattr_t *attr;

  /* flags */
  wasapi_devin->flags = 0;

  /* devin mutex */
  wasapi_devin->obj_mutexattr = 
    attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(attr);
  pthread_mutexattr_settype(attr,
			    PTHREAD_MUTEX_RECURSIVE);

  wasapi_devin->obj_mutex =
    mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex,
		     attr);

  /* parent */
  wasapi_devin->application_context = NULL;

  /* uuid */
  wasapi_devin->uuid = ags_uuid_alloc();
  ags_uuid_generate(wasapi_devin->uuid);

  /* presets */
  config = ags_config_get_instance();

  wasapi_devin->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  wasapi_devin->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  wasapi_devin->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  wasapi_devin->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  wasapi_devin->format = ags_soundcard_helper_config_get_format(config);

  /* device */
  wasapi_devin->device = NULL;

  /* buffer */
  wasapi_devin->buffer_mutex = (pthread_mutex_t **) malloc(8 * sizeof(pthread_mutex_t *));

  for(i = 0; i < 8; i++){
    wasapi_devin->buffer_mutex[i] = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));

    pthread_mutex_init(wasapi_devin->buffer_mutex[i],
		       NULL);
  }

  wasapi_devin->buffer = (void **) malloc(8 * sizeof(void*));

  wasapi_devin->buffer[0] = NULL;
  wasapi_devin->buffer[1] = NULL;
  wasapi_devin->buffer[2] = NULL;
  wasapi_devin->buffer[3] = NULL;
  wasapi_devin->buffer[4] = NULL;
  wasapi_devin->buffer[5] = NULL;
  wasapi_devin->buffer[6] = NULL;
  wasapi_devin->buffer[7] = NULL;
  
  ags_wasapi_devin_realloc_buffer(wasapi_devin);
  
  /* bpm */
  wasapi_devin->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  wasapi_devin->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denumerator,
	   &numerator);
    
    wasapi_devin->delay_factor = 1.0 / numerator * (numerator / denumerator);

    g_free(segmentation);
  }

  /* delay and attack */
  wasapi_devin->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					   sizeof(gdouble));
  
  wasapi_devin->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					  sizeof(guint));

  ags_wasapi_devin_adjust_delay_and_attack(wasapi_devin);
  
  /* counters */
  wasapi_devin->tact_counter = 0.0;
  wasapi_devin->delay_counter = 0.0;
  wasapi_devin->tic_counter = 0;

  wasapi_devin->start_note_offset = 0;
  wasapi_devin->note_offset = 0;
  wasapi_devin->note_offset_absolute = 0;

  wasapi_devin->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  wasapi_devin->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  wasapi_devin->do_loop = FALSE;

  wasapi_devin->loop_offset = 0;

  /* callback mutex */
  wasapi_devin->callback_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(wasapi_devin->callback_mutex,
		     NULL);

  wasapi_devin->callback_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(wasapi_devin->callback_cond, NULL);

  /* callback finish mutex */
  wasapi_devin->callback_finish_mutex = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(wasapi_devin->callback_finish_mutex,
		     NULL);

  wasapi_devin->callback_finish_cond = (pthread_cond_t *) malloc(sizeof(pthread_cond_t));
  pthread_cond_init(wasapi_devin->callback_finish_cond, NULL);

  /*  */
  wasapi_devin->notify_soundcard = NULL;
}

void
ags_wasapi_devin_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsWasapiDevin *wasapi_devin;
  
  pthread_mutex_t *wasapi_devin_mutex;

  wasapi_devin = AGS_WASAPI_DEVIN(gobject);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      pthread_mutex_lock(wasapi_devin_mutex);

      if(wasapi_devin->application_context == application_context){
	pthread_mutex_unlock(wasapi_devin_mutex);

	return;
      }

      if(wasapi_devin->application_context != NULL){
	g_object_unref(G_OBJECT(wasapi_devin->application_context));
      }

      if(application_context != NULL){	
	g_object_ref(G_OBJECT(application_context));
      }

      wasapi_devin->application_context = application_context;

      pthread_mutex_unlock(wasapi_devin_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      pthread_mutex_lock(wasapi_devin_mutex);

      wasapi_devin->device = g_strdup(device);

      pthread_mutex_unlock(wasapi_devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      pthread_mutex_lock(wasapi_devin_mutex);

      if(dsp_channels == wasapi_devin->dsp_channels){
	pthread_mutex_unlock(wasapi_devin_mutex);

	return;
      }

      wasapi_devin->dsp_channels = dsp_channels;

      pthread_mutex_unlock(wasapi_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      pthread_mutex_lock(wasapi_devin_mutex);

      if(pcm_channels == wasapi_devin->pcm_channels){
	pthread_mutex_unlock(wasapi_devin_mutex);

	return;
      }

      wasapi_devin->pcm_channels = pcm_channels;

      pthread_mutex_unlock(wasapi_devin_mutex);

      ags_wasapi_devin_realloc_buffer(wasapi_devin);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      pthread_mutex_lock(wasapi_devin_mutex);

      if(format == wasapi_devin->format){
	pthread_mutex_unlock(wasapi_devin_mutex);

	return;
      }

      wasapi_devin->format = format;

      pthread_mutex_unlock(wasapi_devin_mutex);

      ags_wasapi_devin_realloc_buffer(wasapi_devin);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      pthread_mutex_lock(wasapi_devin_mutex);

      if(buffer_size == wasapi_devin->buffer_size){
	pthread_mutex_unlock(wasapi_devin_mutex);

	return;
      }

      wasapi_devin->buffer_size = buffer_size;

      pthread_mutex_unlock(wasapi_devin_mutex);

      ags_wasapi_devin_realloc_buffer(wasapi_devin);
      ags_wasapi_devin_adjust_delay_and_attack(wasapi_devin);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      pthread_mutex_lock(wasapi_devin_mutex);
      
      if(samplerate == wasapi_devin->samplerate){
	pthread_mutex_unlock(wasapi_devin_mutex);

	return;
      }

      wasapi_devin->samplerate = samplerate;

      pthread_mutex_unlock(wasapi_devin_mutex);

      ags_wasapi_devin_realloc_buffer(wasapi_devin);
      ags_wasapi_devin_adjust_delay_and_attack(wasapi_devin);
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

      pthread_mutex_lock(wasapi_devin_mutex);

      wasapi_devin->bpm = bpm;

      pthread_mutex_unlock(wasapi_devin_mutex);

      ags_wasapi_devin_adjust_delay_and_attack(wasapi_devin);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      pthread_mutex_lock(wasapi_devin_mutex);

      wasapi_devin->delay_factor = delay_factor;

      pthread_mutex_unlock(wasapi_devin_mutex);

      ags_wasapi_devin_adjust_delay_and_attack(wasapi_devin);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wasapi_devin_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsWasapiDevin *wasapi_devin;

  pthread_mutex_t *wasapi_devin_mutex;

  wasapi_devin = AGS_WASAPI_DEVIN(gobject);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);
  
  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      pthread_mutex_lock(wasapi_devin_mutex);

      g_value_set_object(value, wasapi_devin->application_context);

      pthread_mutex_unlock(wasapi_devin_mutex);
    }
    break;
  case PROP_DEVICE:
    {
      pthread_mutex_lock(wasapi_devin_mutex);

      g_value_set_string(value, wasapi_devin->device);

      pthread_mutex_unlock(wasapi_devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      pthread_mutex_lock(wasapi_devin_mutex);

      g_value_set_uint(value, wasapi_devin->dsp_channels);

      pthread_mutex_unlock(wasapi_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      pthread_mutex_lock(wasapi_devin_mutex);

      g_value_set_uint(value, wasapi_devin->pcm_channels);

      pthread_mutex_unlock(wasapi_devin_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      pthread_mutex_lock(wasapi_devin_mutex);

      g_value_set_uint(value, wasapi_devin->format);

      pthread_mutex_unlock(wasapi_devin_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      pthread_mutex_lock(wasapi_devin_mutex);

      g_value_set_uint(value, wasapi_devin->buffer_size);

      pthread_mutex_unlock(wasapi_devin_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      pthread_mutex_lock(wasapi_devin_mutex);

      g_value_set_uint(value, wasapi_devin->samplerate);

      pthread_mutex_unlock(wasapi_devin_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      pthread_mutex_lock(wasapi_devin_mutex);

      g_value_set_pointer(value, wasapi_devin->buffer);

      pthread_mutex_unlock(wasapi_devin_mutex);
    }
    break;
  case PROP_BPM:
    {
      pthread_mutex_lock(wasapi_devin_mutex);

      g_value_set_double(value, wasapi_devin->bpm);

      pthread_mutex_unlock(wasapi_devin_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      pthread_mutex_lock(wasapi_devin_mutex);

      g_value_set_double(value, wasapi_devin->delay_factor);

      pthread_mutex_unlock(wasapi_devin_mutex);
    }
    break;
  case PROP_ATTACK:
    {
      pthread_mutex_lock(wasapi_devin_mutex);

      g_value_set_pointer(value, wasapi_devin->attack);

      pthread_mutex_unlock(wasapi_devin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wasapi_devin_dispose(GObject *gobject)
{
  AgsWasapiDevin *wasapi_devin;

  GList *list;

  wasapi_devin = AGS_WASAPI_DEVIN(gobject);

  //TODO:JK: implement me
  
  /* call parent */
  G_OBJECT_CLASS(ags_wasapi_devin_parent_class)->dispose(gobject);
}

void
ags_wasapi_devin_finalize(GObject *gobject)
{
  AgsWasapiDevin *wasapi_devin;

  wasapi_devin = AGS_WASAPI_DEVIN(gobject);

  pthread_mutex_destroy(wasapi_devin->obj_mutex);
  free(wasapi_devin->obj_mutex);

  pthread_mutexattr_destroy(wasapi_devin->obj_mutexattr);
  free(wasapi_devin->obj_mutexattr);

  //TODO:JK: implement me
  
  /* call parent */
  G_OBJECT_CLASS(ags_wasapi_devin_parent_class)->finalize(gobject);
}

/**
 * ags_wasapi_devin_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.3.4
 */
pthread_mutex_t*
ags_wasapi_devin_get_class_mutex()
{
  return(&ags_wasapi_devin_class_mutex);
}

/**
 * ags_wasapi_devin_test_flags:
 * @wasapi_devin: the #AgsWasapiDevin
 * @flags: the flags
 *
 * Test @flags to be set on @wasapi_devin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.3.4
 */
gboolean
ags_wasapi_devin_test_flags(AgsWasapiDevin *wasapi_devin, guint flags)
{
  gboolean retval;  
  
  pthread_mutex_t *wasapi_devin_mutex;

  if(!AGS_IS_WASAPI_DEVIN(wasapi_devin)){
    return(FALSE);
  }

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* test */
  pthread_mutex_lock(wasapi_devin_mutex);

  retval = (flags & (wasapi_devin->flags)) ? TRUE: FALSE;
  
  pthread_mutex_unlock(wasapi_devin_mutex);

  return(retval);
}

/**
 * ags_wasapi_devin_set_flags:
 * @wasapi_devin: the #AgsWasapiDevin
 * @flags: see #AgsWasapiDevinFlags-enum
 *
 * Enable a feature of @wasapi_devin.
 *
 * Since: 2.3.4
 */
void
ags_wasapi_devin_set_flags(AgsWasapiDevin *wasapi_devin, guint flags)
{
  pthread_mutex_t *wasapi_devin_mutex;

  if(!AGS_IS_WASAPI_DEVIN(wasapi_devin)){
    return;
  }

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  //TODO:JK: add more?

  /* set flags */
  pthread_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->flags |= flags;
  
  pthread_mutex_unlock(wasapi_devin_mutex);
}
    
/**
 * ags_wasapi_devin_unset_flags:
 * @wasapi_devin: the #AgsWasapiDevin
 * @flags: see #AgsWasapiDevinFlags-enum
 *
 * Disable a feature of @wasapi_devin.
 *
 * Since: 2.3.4
 */
void
ags_wasapi_devin_unset_flags(AgsWasapiDevin *wasapi_devin, guint flags)
{  
  pthread_mutex_t *wasapi_devin_mutex;

  if(!AGS_IS_WASAPI_DEVIN(wasapi_devin)){
    return;
  }

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  //TODO:JK: add more?

  /* unset flags */
  pthread_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->flags &= (~flags);
  
  pthread_mutex_unlock(wasapi_devin_mutex);
}

void
ags_wasapi_devin_set_application_context(AgsSoundcard *soundcard,
					 AgsApplicationContext *application_context)
{
  AgsWasapiDevin *wasapi_devin;

  pthread_mutex_t *wasapi_devin_mutex;

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* set application context */
  pthread_mutex_lock(wasapi_devin_mutex);
  
  wasapi_devin->application_context = application_context;
  
  pthread_mutex_unlock(wasapi_devin_mutex);
}

AgsApplicationContext*
ags_wasapi_devin_get_application_context(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  AgsApplicationContext *application_context;
  
  pthread_mutex_t *wasapi_devin_mutex;

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get application context */
  pthread_mutex_lock(wasapi_devin_mutex);

  application_context = wasapi_devin->application_context;

  pthread_mutex_unlock(wasapi_devin_mutex);
  
  return(application_context);
}

void
ags_wasapi_devin_set_device(AgsSoundcard *soundcard,
			    gchar *device)
{
  AgsWasapiDevin *wasapi_devin;

  GList *card_id, *card_id_start, *card_name, *card_name_start;
  
  pthread_mutex_t *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi_devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* list cards */
  card_id = NULL;
  card_name = NULL;
  
  ags_soundcard_list_cards(soundcard,
			   &card_id, &card_name);

  card_id_start = card_id;
  card_name_start = card_name;

  /* check card */
  pthread_mutex_lock(wasapi_devin_mutex);

  while(card_id != NULL){
    if(!g_ascii_strncasecmp(card_id->data,
			    device,
			    strlen(card_id->data))){
      wasapi_devin->device = g_strdup(device);

      break;
    }
    
    card_id = card_id->next;
  }

  pthread_mutex_unlock(wasapi_devin_mutex);

  /* free card id and name */
  g_list_free_full(card_id_start,
		   g_free);
  g_list_free_full(card_name_start,
		   g_free);
}

gchar*
ags_wasapi_devin_get_device(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;
  
  gchar *device;

  pthread_mutex_t *wasapi_devin_mutex;

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);
  
  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  device = NULL;

  /* get device */
  pthread_mutex_lock(wasapi_devin_mutex);

  device = g_strdup(wasapi_devin->device);

  pthread_mutex_unlock(wasapi_devin_mutex);
  
  return(device);
}

void
ags_wasapi_devin_set_presets(AgsSoundcard *soundcard,
			     guint channels,
			     guint rate,
			     guint buffer_size,
			     guint format)
{
  AgsWasapiDevin *wasapi_devin;

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  g_object_set(wasapi_devin,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_wasapi_devin_get_presets(AgsSoundcard *soundcard,
			     guint *channels,
			     guint *rate,
			     guint *buffer_size,
			     guint *format)
{
  AgsWasapiDevin *wasapi_devin;

  pthread_mutex_t *wasapi_devin_mutex;

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);
  
  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get presets */
  pthread_mutex_lock(wasapi_devin_mutex);

  if(channels != NULL){
    *channels = wasapi_devin->pcm_channels;
  }

  if(rate != NULL){
    *rate = wasapi_devin->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = wasapi_devin->buffer_size;
  }

  if(format != NULL){
    *format = wasapi_devin->format;
  }

  pthread_mutex_unlock(wasapi_devin_mutex);
}

/**
 * ags_wasapi_devin_list_cards:
 * @soundcard: the #AgsSoundcard
 * @card_id: WASAPI identifier
 * @card_name: card name
 *
 * List available soundcards.
 *
 * Since: 2.3.4
 */
void
ags_wasapi_devin_list_cards(AgsSoundcard *soundcard,
			    GList **card_id, GList **card_name)
{
  AgsWasapiDevin *wasapi_devin;

  AgsApplicationContext *application_context;
  
#ifdef AGS_WITH_WASAPI
  IMMDeviceEnumerator *dev_enumerator;
#endif

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  application_context = ags_application_context_get_instance();
  
  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

#ifdef AGS_WITH_WASAPI
  CoInitialize(0);

  if(!CoCreateInstance(&ags_wasapi_clsid_mm_device_enumerator_guid, 0, CLSCTX_ALL, &ags_wasapi_iid_mm_device_enumerator_guid, &dev_enumerator)){
    IMMDeviceCollection *dev_collection;

    if(!dev_enumerator->lpVtbl->EnumAudioEndpoints(dev_enumerator, eCapture, DEVICE_STATE_ACTIVE, &dev_collection)){
      IMMDevice *device;
      register UINT dev_num;

      dev_num = 0;

      while(!dev_collection->lpVtbl->Item(dev_collection, dev_num++, &device)){
	IAudioClient *audio_client;

	if(!device->lpVtbl->Activate(device, &ags_wasapi_iid_audio_client_guid, CLSCTX_ALL, 0, (void **) &audio_client)){
	  IPropertyStore *prop_store;

	  if(!device->lpVtbl->OpenPropertyStore(device, STGM_READ, &prop_store)){
	    PROPVARIANT var_name;

	    PropVariantInit(&var_name);

	    if(!prop_store->lpVtbl->GetValue(prop_store, &ags_wasapi_pkey_device_friendly_name_guid, &var_name)){
	      WCHAR *dev_id = 0;

	      // Get this device's ID and name
	      device->lpVtbl->GetId(device, &dev_id);
	      
	      *card_id = g_list_prepend(*card_id,
					g_utf16_to_utf8(dev_id,
							-1,
							NULL,
							NULL,
							NULL));
		
	      *card_name = g_list_prepend(*card_name,
					  g_utf16_to_utf8(var_name.pwszVal,
							  -1,
							  NULL,
							  NULL,
							  NULL));

	      PropVariantClear(&var_name);
	    }

	    prop_store->lpVtbl->Release(prop_store);
	  }	  

	  audio_client->lpVtbl->Release(audio_client);
	}

	device->lpVtbl->Release(device);
      }
    
      dev_collection->lpVtbl->Release(dev_collection);
    }

    dev_enumerator->lpVtbl->Release(dev_enumerator);
  }

  CoUninitialize();
#endif
  
  if(card_id != NULL && *card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL && *card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

void
ags_wasapi_devin_pcm_info(AgsSoundcard *soundcard,
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
ags_wasapi_devin_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_CAPTURE);
}

gboolean
ags_wasapi_devin_is_starting(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  gboolean is_starting;
  
  pthread_mutex_t *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* check is starting */
  pthread_mutex_lock(wasapi_devin_mutex);

  is_starting = ((AGS_WASAPI_DEVIN_START_RECORD & (wasapi_devin->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(wasapi_devin_mutex);
  
  return(is_starting);
}

gboolean
ags_wasapi_devin_is_recording(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  gboolean is_playing;
  
  pthread_mutex_t *wasapi_devin_mutex;

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);
  
  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* check is starting */
  pthread_mutex_lock(wasapi_devin_mutex);

  is_playing = ((AGS_WASAPI_DEVIN_RECORD & (wasapi_devin->flags)) != 0) ? TRUE: FALSE;

  pthread_mutex_unlock(wasapi_devin_mutex);

  return(is_playing);
}

gchar*
ags_wasapi_devin_get_uptime(AgsSoundcard *soundcard)
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
ags_wasapi_devin_client_init(AgsSoundcard *soundcard,
			     GError **error)
{
  AgsWasapiDevin *wasapi_devin;

  pthread_mutex_t *wasapi_devin_mutex; 

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  pthread_mutex_lock(wasapi_devin_mutex);

#ifdef AGS_WITH_WASAPI
#endif
  
  wasapi_devin->tact_counter = 0.0;
  wasapi_devin->delay_counter = floor(ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(wasapi_devin)));
  wasapi_devin->tic_counter = 0;

  ags_soundcard_get_poll_fd(soundcard);
  
#ifdef AGS_WITH_WASAPI
  wasapi_devin->flags |= AGS_WASAPI_DEVIN_INITIALIZED;
#endif
  wasapi_devin->flags |= (AGS_WASAPI_DEVIN_BUFFER0 |
			  AGS_WASAPI_DEVIN_START_RECORD |
			  AGS_WASAPI_DEVIN_RECORD |
			  AGS_WASAPI_DEVIN_NONBLOCKING);
  wasapi_devin->flags &= (~(AGS_WASAPI_DEVIN_BUFFER1 |
			    AGS_WASAPI_DEVIN_BUFFER2 |
			    AGS_WASAPI_DEVIN_BUFFER3 |
			    AGS_WASAPI_DEVIN_BUFFER4 |
			    AGS_WASAPI_DEVIN_BUFFER5 |
			    AGS_WASAPI_DEVIN_BUFFER6 |
			    AGS_WASAPI_DEVIN_BUFFER7));
  
  pthread_mutex_unlock(wasapi_devin_mutex);
}

void
ags_wasapi_devin_client_record(AgsSoundcard *soundcard,
			       GError **error)
{
  AgsWasapiDevin *wasapi_devin;

  AgsNotifySoundcard *notify_soundcard;
  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsThread *task_thread;
  AgsPollFd *poll_fd;

  AgsApplicationContext *application_context;

  GList *task;
  GList *list;
  
  gchar *str;
  
#ifdef AGS_WITH_WASAPI
  IAudioClient *audio_client;
  IAudioCaptureClient *audio_capture_client;

  BYTE *data;

  UINT32 buffer_frame_count;  
  UINT32 num_frames_available;
  DWORD pdw_flags;
#endif

  guint word_size;
  guint nth_buffer;
  
  pthread_mutex_t *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  application_context = ags_application_context_get_instance();

  /* get wasapi_devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* lock */
  pthread_mutex_lock(wasapi_devin_mutex);
  
  notify_soundcard = AGS_NOTIFY_SOUNDCARD(wasapi_devin->notify_soundcard);

  /* notify cyclic task */
  pthread_mutex_lock(notify_soundcard->return_mutex);

  g_atomic_int_or(&(notify_soundcard->flags),
		  AGS_NOTIFY_SOUNDCARD_DONE_RETURN);
  
  if((AGS_NOTIFY_SOUNDCARD_WAIT_RETURN & (g_atomic_int_get(&(notify_soundcard->flags)))) != 0){
    pthread_cond_signal(notify_soundcard->return_cond);
  }
  
  pthread_mutex_unlock(notify_soundcard->return_mutex);

  /* retrieve word size */
  switch(wasapi_devin->format){
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
    pthread_mutex_unlock(wasapi_devin_mutex);
    
    g_warning("ags_wasapi_devin_client_record(): unsupported word size");

    return;
  }

#ifdef AGS_WITH_WASAPI
  if((AGS_WASAPI_DEVIN_START_RECORD & (wasapi_devin->flags)) != 0){
    IMMDeviceEnumerator *dev_enumerator;
    IMMDevice *mm_device;

    WAVEFORMATEXTENSIBLE wave_format;
    WAVEFORMATEX *desired_format, *internal_format;

    WCHAR *dev_id;

    REFERENCE_TIME min_duration;

    register HRESULT hr;

    unsigned char bit_resolution;

    g_message("WASAPI initialize");    

    CoInitialize(0);
  
    if(CoCreateInstance(&ags_wasapi_clsid_mm_device_enumerator_guid, 0, CLSCTX_ALL, &ags_wasapi_iid_mm_device_enumerator_guid, (void **) &dev_enumerator)){
      if(error != NULL){
	g_set_error(error,
		    AGS_WASAPI_DEVIN_ERROR,
		    AGS_WASAPI_DEVIN_ERROR_LOCKED_SOUNDCARD,
		    "unable to open pcm device: Can't get IMMDeviceEnumerator.");
      }
    
      CoUninitialize();

      pthread_mutex_unlock(wasapi_devin_mutex);

      return;
    }

#if 0
    if(wasapi_devin->device != NULL){
      dev_id = g_utf8_to_utf16(wasapi_devin->device,
			       -1,
			       NULL,
			       NULL,
			       NULL);

      if(dev_enumerator->lpVtbl->GetDevice(dev_enumerator, dev_id, &mm_device)){
	if(error != NULL){
	  g_set_error(error,
		      AGS_WASAPI_DEVIN_ERROR,
		      AGS_WASAPI_DEVIN_ERROR_LOCKED_SOUNDCARD,
		      "unable to open pcm device: Can't get IMMDevice.");
	}

	dev_enumerator->lpVtbl->Release(dev_enumerator);
	CoUninitialize();

	pthread_mutex_unlock(wasapi_devin_mutex);

	return;
      }
    }else{
#endif
    
      if(dev_enumerator->lpVtbl->GetDefaultAudioEndpoint(dev_enumerator, eCapture, eMultimedia, &mm_device)){
	if(error != NULL){
	  g_set_error(error,
		      AGS_WASAPI_DEVIN_ERROR,
		      AGS_WASAPI_DEVIN_ERROR_LOCKED_SOUNDCARD,
		      "unable to open pcm device: Can't get IAudioClient.");
	}

	dev_enumerator->lpVtbl->Release(dev_enumerator);
	CoUninitialize();

	wasapi_devin->flags &= (~AGS_WASAPI_DEVIN_START_RECORD);
	
	pthread_mutex_unlock(wasapi_devin_mutex);

	g_message("WASAPI failed - get device");
    
	return;
      }
    
#if 0
    }
#endif

    wasapi_devin->mm_device = mm_device;
  
    // Get its IAudioClient (used to set audio format, latency, and start/stop)
    if(mm_device->lpVtbl->Activate(mm_device, &ags_wasapi_iid_audio_client_guid, CLSCTX_ALL, 0, (void **) &audio_client)){
      if(error != NULL){
	g_set_error(error,
		    AGS_WASAPI_DEVIN_ERROR,
		    AGS_WASAPI_DEVIN_ERROR_LOCKED_SOUNDCARD,
		    "unable to open pcm device: Can't get IAudioClient.");
      }

      mm_device->lpVtbl->Release(mm_device);
      dev_enumerator->lpVtbl->Release(dev_enumerator);
      CoUninitialize();

      pthread_mutex_unlock(wasapi_devin_mutex);

      g_message("WASAPI failed - get audio client");

      return;
    }
  
    wasapi_devin->audio_client = audio_client;
  
    if((AGS_WASAPI_DEVIN_SHARE_MODE_EXCLUSIVE & (wasapi_devin->flags)) != 0){
      wave_format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
      wave_format.Format.nSamplesPerSec = wasapi_devin->samplerate; // necessary
      wave_format.Format.nChannels = wasapi_devin->pcm_channels; // presumed

      bit_resolution = 16;
  
      switch(wasapi_devin->format){
      case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	bit_resolution = 16;
      }
      break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	bit_resolution = 24;
      }
      break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	bit_resolution = 32;
      }
      break;
      }  

      if(bit_resolution == 24){
	wave_format.Format.wBitsPerSample = 32;
	wave_format.Format.nBlockAlign = 2 * (32 / 8);
	wave_format.Samples.wValidBitsPerSample = 24;
      }else{
	wave_format.Format.wBitsPerSample =
	  wave_format.Samples.wValidBitsPerSample = bit_resolution;
	wave_format.Format.nBlockAlign = 2 * (bit_resolution / 8);
      }
    
      wave_format.Format.nBlockAlign = wave_format.Format.nChannels * wave_format.Format.wBitsPerSample / 8;
      wave_format.Format.nAvgBytesPerSec = wave_format.Format.nSamplesPerSec * wave_format.Format.nBlockAlign;
      wave_format.Format.cbSize = 22;
      wave_format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
      wave_format.SubFormat = ags_wasapi_pcm_subformat_guid;

      if((hr = audio_client->lpVtbl->GetMixFormat(audio_client, &internal_format))){
	g_message("get mix format");
      }
      
      desired_format = &(wave_format.Format);

      desired_format->wFormatTag = 0xFFFE;
      desired_format->cbSize = 22;
  
      if((hr = audio_client->lpVtbl->IsFormatSupported(audio_client, AUDCLNT_SHAREMODE_EXCLUSIVE, desired_format, NULL))){
      ags_wasapi_devin_client_init_EXCLUSIVE_BROKEN_CONFIGURATION:

	mm_device->lpVtbl->Release(mm_device);
	dev_enumerator->lpVtbl->Release(dev_enumerator);
	CoUninitialize();

	wasapi_devin->flags &= (~AGS_WASAPI_DEVIN_START_RECORD);
      
	pthread_mutex_unlock(wasapi_devin_mutex);

	g_message("WASAPI failed - broken configuration");
    
	return;
      }
  
      audio_client->lpVtbl->GetDevicePeriod(audio_client, NULL, &min_duration);

      if((hr = audio_client->lpVtbl->Initialize(audio_client, AUDCLNT_SHAREMODE_EXCLUSIVE, 0, min_duration, min_duration, desired_format, NULL))){
	audio_client->lpVtbl->Release(audio_client);

	goto ags_wasapi_devin_client_init_EXCLUSIVE_BROKEN_CONFIGURATION;
      }
    }else{
      wave_format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
      wave_format.Format.nSamplesPerSec = wasapi_devin->samplerate; // necessary
      wave_format.Format.nChannels = wasapi_devin->pcm_channels; // presumed

      bit_resolution = 16;
  
      switch(wasapi_devin->format){
      case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	bit_resolution = 16;
      }
      break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	bit_resolution = 24;
      }
      break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	bit_resolution = 32;
      }
      break;
      }  

      if(bit_resolution == 24){
	wave_format.Format.wBitsPerSample = 32;
	wave_format.Format.nBlockAlign = 2 * (32 / 8);
	wave_format.Samples.wValidBitsPerSample = 24;
      }else{
	wave_format.Format.wBitsPerSample =
	  wave_format.Samples.wValidBitsPerSample = bit_resolution;
	wave_format.Format.nBlockAlign = 2 * (bit_resolution / 8);
      }
    
      wave_format.Format.nBlockAlign = wave_format.Format.nChannels * wave_format.Format.wBitsPerSample / 8;
      wave_format.Format.nAvgBytesPerSec = wave_format.Format.nSamplesPerSec * wave_format.Format.nBlockAlign;
      wave_format.Format.cbSize = 22;
      wave_format.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
      wave_format.SubFormat = ags_wasapi_pcm_subformat_guid;

      if((hr = audio_client->lpVtbl->GetMixFormat(audio_client, &internal_format))){
	g_message("get mix format");

      ags_wasapi_devin_client_init_SHARED_BROKEN_CONFIGURATION:

	mm_device->lpVtbl->Release(mm_device);
	dev_enumerator->lpVtbl->Release(dev_enumerator);
	CoUninitialize();

	wasapi_devin->flags &= (~AGS_WASAPI_DEVIN_START_RECORD);
      
	pthread_mutex_unlock(wasapi_devin_mutex);

	g_message("WASAPI failed - broken configuration");
    
	return;
      }

      desired_format = internal_format;

      if((hr = audio_client->lpVtbl->IsFormatSupported(audio_client, AUDCLNT_SHAREMODE_SHARED, desired_format, &desired_format))){
	goto ags_wasapi_devin_client_init_SHARED_BROKEN_CONFIGURATION;
      }

      desired_format = internal_format;
    
      audio_client->lpVtbl->GetDevicePeriod(audio_client, NULL, &min_duration);

      min_duration = (NSEC_PER_SEC / 100) / wasapi_devin->samplerate * wasapi_devin->buffer_size;
    
      if((hr = audio_client->lpVtbl->Initialize(audio_client, AUDCLNT_SHAREMODE_SHARED, 0, min_duration, min_duration, desired_format, NULL))){
	audio_client->lpVtbl->Release(audio_client);

	goto ags_wasapi_devin_client_init_SHARED_BROKEN_CONFIGURATION;
      }
    }

    // Start audio capture
    audio_client->lpVtbl->Start(audio_client);    
  }

  audio_client = wasapi_devin->audio_client;
#endif

  /* do capture */
  wasapi_devin->flags &= (~AGS_WASAPI_DEVIN_START_RECORD);

  if((AGS_WASAPI_DEVIN_INITIALIZED & (wasapi_devin->flags)) == 0){
    pthread_mutex_unlock(wasapi_devin_mutex);
    
    return;
  }

  //  g_message("record - 0x%0x", ((AGS_WASAPI_DEVIN_BUFFER0 |
  //				AGS_WASAPI_DEVIN_BUFFER1 |
  //				AGS_WASAPI_DEVIN_BUFFER2 |
  //				AGS_WASAPI_DEVIN_BUFFER3) & (wasapi_devin->flags)));

  /* check buffer flag */
  if((AGS_WASAPI_DEVIN_SHUTDOWN & (wasapi_devin->flags)) == 0){
    /* check buffer flag */
    nth_buffer = 0;

    if((AGS_WASAPI_DEVIN_BUFFER0 & (wasapi_devin->flags)) != 0){
      nth_buffer = 0;
    }else if((AGS_WASAPI_DEVIN_BUFFER1 & (wasapi_devin->flags)) != 0){
      nth_buffer = 1;
    }else if((AGS_WASAPI_DEVIN_BUFFER2 & (wasapi_devin->flags)) != 0){
      nth_buffer = 2;
    }else if((AGS_WASAPI_DEVIN_BUFFER3 & (wasapi_devin->flags)) != 0){
      nth_buffer = 3;
    }else if((AGS_WASAPI_DEVIN_BUFFER4 & (wasapi_devin->flags)) != 0){
      nth_buffer = 4;
    }else if((AGS_WASAPI_DEVIN_BUFFER5 & (wasapi_devin->flags)) != 0){
      nth_buffer = 5;
    }else if((AGS_WASAPI_DEVIN_BUFFER6 & (wasapi_devin->flags)) != 0){
      nth_buffer = 6;
    }else if((AGS_WASAPI_DEVIN_BUFFER7 & (wasapi_devin->flags)) != 0){
      nth_buffer = 7;
    }
    
#ifdef AGS_WITH_WASAPI
    /* wait until available */
    audio_client = wasapi_devin->audio_client;
  
    audio_client->lpVtbl->GetBufferSize(audio_client, &buffer_frame_count);

    {
      HRESULT res;
    
      res = audio_client->lpVtbl->GetService(audio_client, &ags_wasapi_iid_audio_capture_client_guid, (void **) &audio_capture_client);

      switch(res){
      case E_POINTER:
	{
	  g_message("pointer");
	}
	break;
      case E_NOINTERFACE:
	{
	  g_message("no interface");
	}
	break;
      case AUDCLNT_E_NOT_INITIALIZED:
	{
	  g_message("not initialized");
	}
	break;
      case AUDCLNT_E_WRONG_ENDPOINT_TYPE:
	{
	  g_message("wrong endpoint");
	}
	break;
      case AUDCLNT_E_DEVICE_INVALIDATED:
	{
	  g_message("device invalidated");
	}
	break;
      case AUDCLNT_E_SERVICE_NOT_RUNNING:
	{
	  g_message("no service");
	}
	break;
      }
    }

    if(audio_capture_client == NULL){
      pthread_mutex_unlock(wasapi_devin_mutex);

      g_message("audio_capture_client = NULL");
      
      return;
    }

    {
      UINT32 padding_frames;

      audio_client->lpVtbl->GetCurrentPadding(audio_client, &padding_frames);
      
      while(padding_frames > 0){
	usleep(4);

	audio_client->lpVtbl->GetCurrentPadding(audio_client, &padding_frames);
      }
    }
    
    {
      HRESULT res;

      res = audio_capture_client->lpVtbl->GetBuffer(audio_capture_client, &data, &num_frames_available, &pdw_flags, NULL, NULL);
      
      switch(res){
      case AUDCLNT_E_BUFFER_ERROR:
      {
	g_message("buffer error");
      }
      break;
      case AUDCLNT_E_BUFFER_TOO_LARGE:
      {
	g_message("buffer too large");
      }
      break;
      case AUDCLNT_E_BUFFER_SIZE_ERROR:
      {
	g_message("buffer size error");
      }
      break;
      case AUDCLNT_E_OUT_OF_ORDER:
      {
	g_message("out of order");
      }
      break;
      case AUDCLNT_E_DEVICE_INVALIDATED:
      {
	g_message("invalidated");
      }
      break;
      case AUDCLNT_E_BUFFER_OPERATION_PENDING:
      {
	g_message("operation pending");
      }
      break;
      case AUDCLNT_E_SERVICE_NOT_RUNNING:
      {
	g_message("no service");
      }
      break;
      case E_POINTER:
      {
	g_message("pointer");
      }
      break;
      }
    }

    /* retrieve word size */
    if(data != NULL){
      switch(wasapi_devin->format){
      case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	ags_audio_buffer_util_copy_s16_to_s16(wasapi_devin->buffer[nth_buffer], 1,
					      data, 1,
					      wasapi_devin->pcm_channels * wasapi_devin->buffer_size);
      }
      break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	ags_audio_buffer_util_copy_s24_to_s24(wasapi_devin->buffer[nth_buffer], 1,
					      data, 1,
					      wasapi_devin->pcm_channels * wasapi_devin->buffer_size);
      }
      break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	ags_audio_buffer_util_copy_s32_to_s32(wasapi_devin->buffer[nth_buffer], 1,
					      data, 1,
					      wasapi_devin->pcm_channels * wasapi_devin->buffer_size);
      }
      break;
      }
    }else{
      g_message("data = NULL");
    }

    audio_capture_client->lpVtbl->ReleaseBuffer(audio_capture_client, wasapi_devin->pcm_channels * wasapi_devin->buffer_size);

    audio_capture_client->lpVtbl->Release(audio_capture_client);
#endif
  }
  
  if((AGS_WASAPI_DEVIN_SHUTDOWN & (wasapi_devin->flags)) != 0){
    AgsThread *audio_loop;
    AgsThread *soundcard_thread;
    
    AgsApplicationContext *application_context;
    
#ifdef AGS_WITH_WASAPI
    IMMDevice *mm_device;

    HRESULT hr;

    g_message("wasapi shutdown");

    audio_client = wasapi_devin->audio_client;
    audio_client->lpVtbl->GetService(audio_client, &ags_wasapi_iid_audio_capture_client_guid, (void **) &audio_capture_client);
    mm_device = wasapi_devin->mm_device;

    hr = audio_client->lpVtbl->Stop(audio_client);

    if(FAILED(hr)){
      g_message("failed to stop WASAPI");
    }

    audio_client->lpVtbl->Reset(audio_client);

    if(audio_capture_client != NULL){
      audio_capture_client->lpVtbl->Release(audio_capture_client);
    }
    
    audio_client->lpVtbl->Release(audio_client);
    mm_device->lpVtbl->Release(mm_device);
    
    wasapi_devin->audio_client = NULL;
    wasapi_devin->mm_device = NULL;

    CoUninitialize();    
#endif

    application_context = ags_application_context_get_instance();
    
    /* get main loop */
    audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
    
    soundcard_thread = ags_thread_find_type(audio_loop,
					    AGS_TYPE_SOUNDCARD_THREAD);

    soundcard_thread = ags_soundcard_thread_find_soundcard(soundcard_thread,
							   AGS_WASAPI_DEVIN(soundcard));

    ags_thread_stop(soundcard_thread);
  }
  
  pthread_mutex_unlock(wasapi_devin_mutex);

  if((AGS_WASAPI_DEVIN_SHUTDOWN & (wasapi_devin->flags)) == 0){
    /* update soundcard */
    task_thread = ags_concurrency_provider_get_task_thread(AGS_CONCURRENCY_PROVIDER(application_context));

    task = NULL;
  
    /* tic soundcard */
    tic_device = ags_tic_device_new((GObject *) wasapi_devin);
    task = g_list_append(task,
			 tic_device);

    /* reset - clear buffer */
    clear_buffer = ags_clear_buffer_new((GObject *) wasapi_devin);
    task = g_list_append(task,
			 clear_buffer);

    /* reset - switch buffer flags */
    switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) wasapi_devin);
    task = g_list_append(task,
			 switch_buffer_flag);

    /* append tasks */
    ags_task_thread_append_tasks((AgsTaskThread *) task_thread,
				 task);
  
    /* unref */
    g_object_unref(task_thread);
  }
}

void
ags_wasapi_devin_client_free(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  AgsNotifySoundcard *notify_soundcard;

  AgsApplicationContext *application_context;

  GList *poll_fd;

  guint i;
  
  pthread_mutex_t *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* lock */
  pthread_mutex_lock(wasapi_devin_mutex);

  notify_soundcard = AGS_NOTIFY_SOUNDCARD(wasapi_devin->notify_soundcard);

  if((AGS_WASAPI_DEVIN_INITIALIZED & (wasapi_devin->flags)) == 0){
    pthread_mutex_unlock(wasapi_devin_mutex);
    
    return;
  }

  wasapi_devin->flags |= (AGS_WASAPI_DEVIN_SHUTDOWN);

  pthread_mutex_unlock(wasapi_devin_mutex);

  /* notify cyclic task */
  pthread_mutex_lock(notify_soundcard->return_mutex);

  g_atomic_int_or(&(notify_soundcard->flags),
		  AGS_NOTIFY_SOUNDCARD_DONE_RETURN);
  
  if((AGS_NOTIFY_SOUNDCARD_WAIT_RETURN & (g_atomic_int_get(&(notify_soundcard->flags)))) != 0){
    pthread_cond_signal(notify_soundcard->return_cond);
  }
  
  pthread_mutex_unlock(notify_soundcard->return_mutex);

  pthread_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->note_offset = wasapi_devin->start_note_offset;
  wasapi_devin->note_offset_absolute = wasapi_devin->start_note_offset;

  pthread_mutex_unlock(wasapi_devin_mutex);
}

void
ags_wasapi_devin_tic(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  gdouble delay;
  gdouble delay_counter;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  
  pthread_mutex_t *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);
  
  /* determine if attack should be switched */
  pthread_mutex_lock(wasapi_devin_mutex);

  delay = wasapi_devin->delay[wasapi_devin->tic_counter];
  delay_counter = wasapi_devin->delay_counter;

  note_offset = wasapi_devin->note_offset;
  note_offset_absolute = wasapi_devin->note_offset_absolute;
  
  loop_left = wasapi_devin->loop_left;
  loop_right = wasapi_devin->loop_right;
  
  do_loop = wasapi_devin->do_loop;

  pthread_mutex_unlock(wasapi_devin_mutex);

  if(delay_counter + 1.0 >= delay){
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
    pthread_mutex_lock(wasapi_devin_mutex);
    
    wasapi_devin->delay_counter = delay_counter + 1.0 - delay;
    wasapi_devin->tact_counter += 1.0;

    pthread_mutex_unlock(wasapi_devin_mutex);
  }else{
    pthread_mutex_lock(wasapi_devin_mutex);
    
    wasapi_devin->delay_counter += 1.0;

    pthread_mutex_unlock(wasapi_devin_mutex);
  }
}

void
ags_wasapi_devin_offset_changed(AgsSoundcard *soundcard,
				guint note_offset)
{
  AgsWasapiDevin *wasapi_devin;
  
  pthread_mutex_t *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* offset changed */
  pthread_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->tic_counter += 1;

  if(wasapi_devin->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    wasapi_devin->tic_counter = 0;
  }

  pthread_mutex_unlock(wasapi_devin_mutex);
}

void
ags_wasapi_devin_set_bpm(AgsSoundcard *soundcard,
			 gdouble bpm)
{
  AgsWasapiDevin *wasapi_devin;

  pthread_mutex_t *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* set bpm */
  pthread_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->bpm = bpm;

  pthread_mutex_unlock(wasapi_devin_mutex);

  ags_wasapi_devin_adjust_delay_and_attack(wasapi_devin);
}

gdouble
ags_wasapi_devin_get_bpm(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  gdouble bpm;
  
  pthread_mutex_t *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get bpm */
  pthread_mutex_lock(wasapi_devin_mutex);

  bpm = wasapi_devin->bpm;
  
  pthread_mutex_unlock(wasapi_devin_mutex);

  return(bpm);
}

void
ags_wasapi_devin_set_delay_factor(AgsSoundcard *soundcard,
				  gdouble delay_factor)
{
  AgsWasapiDevin *wasapi_devin;

  pthread_mutex_t *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* set delay factor */
  pthread_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->delay_factor = delay_factor;

  pthread_mutex_unlock(wasapi_devin_mutex);

  ags_wasapi_devin_adjust_delay_and_attack(wasapi_devin);
}

gdouble
ags_wasapi_devin_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  gdouble delay_factor;
  
  pthread_mutex_t *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get delay factor */
  pthread_mutex_lock(wasapi_devin_mutex);

  delay_factor = wasapi_devin->delay_factor;
  
  pthread_mutex_unlock(wasapi_devin_mutex);

  return(delay_factor);
}

gdouble
ags_wasapi_devin_get_delay(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  guint delay_index;
  gdouble delay;
  
  pthread_mutex_t *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get delay */
  pthread_mutex_lock(wasapi_devin_mutex);

  delay_index = wasapi_devin->tic_counter;

  delay = wasapi_devin->delay[delay_index];
  
  pthread_mutex_unlock(wasapi_devin_mutex);
  
  return(delay);
}

gdouble
ags_wasapi_devin_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  gdouble absolute_delay;
  
  pthread_mutex_t *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);
  
  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get absolute delay */
  pthread_mutex_lock(wasapi_devin_mutex);

  absolute_delay = (60.0 * (((gdouble) wasapi_devin->samplerate / (gdouble) wasapi_devin->buffer_size) / (gdouble) wasapi_devin->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) wasapi_devin->delay_factor)));

  pthread_mutex_unlock(wasapi_devin_mutex);

  return(absolute_delay);
}

guint
ags_wasapi_devin_get_attack(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  guint attack_index;
  guint attack;
  
  pthread_mutex_t *wasapi_devin_mutex;  

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);
  
  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get attack */
  pthread_mutex_lock(wasapi_devin_mutex);

  attack_index = wasapi_devin->tic_counter;

  attack = wasapi_devin->attack[attack_index];

  pthread_mutex_unlock(wasapi_devin_mutex);
  
  return(attack);
}

void*
ags_wasapi_devin_get_buffer(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  void *buffer;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER0)){
    buffer = wasapi_devin->buffer[0];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER1)){
    buffer = wasapi_devin->buffer[1];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER2)){
    buffer = wasapi_devin->buffer[2];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER3)){
    buffer = wasapi_devin->buffer[3];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER4)){
    buffer = wasapi_devin->buffer[4];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER5)){
    buffer = wasapi_devin->buffer[5];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER6)){
    buffer = wasapi_devin->buffer[6];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER7)){
    buffer = wasapi_devin->buffer[7];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_wasapi_devin_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  void *buffer;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  //  g_message("next - 0x%0x", ((AGS_WASAPI_DEVIN_BUFFER0 |
  //				AGS_WASAPI_DEVIN_BUFFER1 |
  //				AGS_WASAPI_DEVIN_BUFFER2 |
  //				AGS_WASAPI_DEVIN_BUFFER3) & (wasapi_devin->flags)));

  if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER0)){
    buffer = wasapi_devin->buffer[1];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER1)){
    buffer = wasapi_devin->buffer[2];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER2)){
    buffer = wasapi_devin->buffer[3];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER3)){
    buffer = wasapi_devin->buffer[4];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER4)){
    buffer = wasapi_devin->buffer[5];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER5)){
    buffer = wasapi_devin->buffer[6];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER6)){
    buffer = wasapi_devin->buffer[7];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER7)){
    buffer = wasapi_devin->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_wasapi_devin_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  void *buffer;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER0)){
    buffer = wasapi_devin->buffer[7];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER1)){
    buffer = wasapi_devin->buffer[0];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER2)){
    buffer = wasapi_devin->buffer[1];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER3)){
    buffer = wasapi_devin->buffer[2];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER4)){
    buffer = wasapi_devin->buffer[3];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER5)){
    buffer = wasapi_devin->buffer[4];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER6)){
    buffer = wasapi_devin->buffer[5];
  }else if(ags_wasapi_devin_test_flags(wasapi_devin, AGS_WASAPI_DEVIN_BUFFER7)){
    buffer = wasapi_devin->buffer[6];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void
ags_wasapi_devin_lock_buffer(AgsSoundcard *soundcard,
			     void *buffer)
{
  AgsWasapiDevin *wasapi_devin;

  pthread_mutex_t *buffer_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(wasapi_devin->buffer != NULL){
    if(buffer == wasapi_devin->buffer[0]){
      buffer_mutex = wasapi_devin->buffer_mutex[0];
    }else if(buffer == wasapi_devin->buffer[1]){
      buffer_mutex = wasapi_devin->buffer_mutex[1];
    }else if(buffer == wasapi_devin->buffer[2]){
      buffer_mutex = wasapi_devin->buffer_mutex[2];
    }else if(buffer == wasapi_devin->buffer[3]){
      buffer_mutex = wasapi_devin->buffer_mutex[3];
    }else if(buffer == wasapi_devin->buffer[4]){
      buffer_mutex = wasapi_devin->buffer_mutex[4];
    }else if(buffer == wasapi_devin->buffer[5]){
      buffer_mutex = wasapi_devin->buffer_mutex[5];
    }else if(buffer == wasapi_devin->buffer[6]){
      buffer_mutex = wasapi_devin->buffer_mutex[6];
    }else if(buffer == wasapi_devin->buffer[7]){
      buffer_mutex = wasapi_devin->buffer_mutex[7];
    }
  }
  
  if(buffer_mutex != NULL){
    pthread_mutex_lock(buffer_mutex);
  }
}

void
ags_wasapi_devin_unlock_buffer(AgsSoundcard *soundcard,
			       void *buffer)
{
  AgsWasapiDevin *wasapi_devin;

  pthread_mutex_t *buffer_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(wasapi_devin->buffer != NULL){
    if(buffer == wasapi_devin->buffer[0]){
      buffer_mutex = wasapi_devin->buffer_mutex[0];
    }else if(buffer == wasapi_devin->buffer[1]){
      buffer_mutex = wasapi_devin->buffer_mutex[1];
    }else if(buffer == wasapi_devin->buffer[2]){
      buffer_mutex = wasapi_devin->buffer_mutex[2];
    }else if(buffer == wasapi_devin->buffer[3]){
      buffer_mutex = wasapi_devin->buffer_mutex[3];
    }else if(buffer == wasapi_devin->buffer[4]){
      buffer_mutex = wasapi_devin->buffer_mutex[4];
    }else if(buffer == wasapi_devin->buffer[5]){
      buffer_mutex = wasapi_devin->buffer_mutex[5];
    }else if(buffer == wasapi_devin->buffer[6]){
      buffer_mutex = wasapi_devin->buffer_mutex[6];
    }else if(buffer == wasapi_devin->buffer[7]){
      buffer_mutex = wasapi_devin->buffer_mutex[7];
    }
  }

  if(buffer_mutex != NULL){
    pthread_mutex_unlock(buffer_mutex);
  }
}

guint
ags_wasapi_devin_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  guint delay_counter;
  
  pthread_mutex_t *wasapi_devin_mutex;  

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);
  
  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* delay counter */
  pthread_mutex_lock(wasapi_devin_mutex);

  delay_counter = wasapi_devin->delay_counter;
  
  pthread_mutex_unlock(wasapi_devin_mutex);

  return(delay_counter);
}

void
ags_wasapi_devin_set_note_offset(AgsSoundcard *soundcard,
				 guint note_offset)
{
  AgsWasapiDevin *wasapi_devin;

  pthread_mutex_t *wasapi_devin_mutex;  

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* set note offset */
  pthread_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->note_offset = note_offset;

  pthread_mutex_unlock(wasapi_devin_mutex);
}

guint
ags_wasapi_devin_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  guint start_note_offset;
  
  pthread_mutex_t *wasapi_devin_mutex;  

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* set note offset */
  pthread_mutex_lock(wasapi_devin_mutex);

  start_note_offset = wasapi_devin->start_note_offset;

  pthread_mutex_unlock(wasapi_devin_mutex);

  return(start_note_offset);
}

void
ags_wasapi_devin_set_start_note_offset(AgsSoundcard *soundcard,
				       guint start_note_offset)
{
  AgsWasapiDevin *wasapi_devin;

  pthread_mutex_t *wasapi_devin_mutex;  

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* set note offset */
  pthread_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->start_note_offset = start_note_offset;

  pthread_mutex_unlock(wasapi_devin_mutex);
}

guint
ags_wasapi_devin_get_note_offset(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  guint note_offset;
  
  pthread_mutex_t *wasapi_devin_mutex;  

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* set note offset */
  pthread_mutex_lock(wasapi_devin_mutex);

  note_offset = wasapi_devin->note_offset;

  pthread_mutex_unlock(wasapi_devin_mutex);

  return(note_offset);
}

void
ags_wasapi_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
					  guint note_offset_absolute)
{
  AgsWasapiDevin *wasapi_devin;
  
  pthread_mutex_t *wasapi_devin_mutex;  

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* set note offset */
  pthread_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->note_offset_absolute = note_offset_absolute;

  pthread_mutex_unlock(wasapi_devin_mutex);
}

guint
ags_wasapi_devin_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  guint note_offset_absolute;
  
  pthread_mutex_t *wasapi_devin_mutex;  

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* set note offset */
  pthread_mutex_lock(wasapi_devin_mutex);

  note_offset_absolute = wasapi_devin->note_offset_absolute;

  pthread_mutex_unlock(wasapi_devin_mutex);

  return(note_offset_absolute);
}

void
ags_wasapi_devin_set_loop(AgsSoundcard *soundcard,
			  guint loop_left, guint loop_right,
			  gboolean do_loop)
{
  AgsWasapiDevin *wasapi_devin;

  pthread_mutex_t *wasapi_devin_mutex;  

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* set loop */
  pthread_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->loop_left = loop_left;
  wasapi_devin->loop_right = loop_right;
  wasapi_devin->do_loop = do_loop;

  if(do_loop){
    wasapi_devin->loop_offset = wasapi_devin->note_offset;
  }

  pthread_mutex_unlock(wasapi_devin_mutex);
}

void
ags_wasapi_devin_get_loop(AgsSoundcard *soundcard,
			  guint *loop_left, guint *loop_right,
			  gboolean *do_loop)
{
  AgsWasapiDevin *wasapi_devin;

  pthread_mutex_t *wasapi_devin_mutex;  

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get loop */
  pthread_mutex_lock(wasapi_devin_mutex);

  if(loop_left != NULL){
    *loop_left = wasapi_devin->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = wasapi_devin->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = wasapi_devin->do_loop;
  }

  pthread_mutex_unlock(wasapi_devin_mutex);
}

guint
ags_wasapi_devin_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  guint loop_offset;
  
  pthread_mutex_t *wasapi_devin_mutex;  

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get loop offset */
  pthread_mutex_lock(wasapi_devin_mutex);

  loop_offset = wasapi_devin->loop_offset;
  
  pthread_mutex_unlock(wasapi_devin_mutex);

  return(loop_offset);
}


/**
 * ags_wasapi_devin_switch_buffer_flag:
 * @wasapi_devin: an #AgsWasapiDevin
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 2.3.4
 */
void
ags_wasapi_devin_switch_buffer_flag(AgsWasapiDevin *wasapi_devin)
{
  pthread_mutex_t *wasapi_devin_mutex;
  
  if(!AGS_IS_WASAPI_DEVIN(wasapi_devin)){
    return;
  }

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* switch buffer flag */
  pthread_mutex_lock(wasapi_devin_mutex);

  if((AGS_WASAPI_DEVIN_BUFFER0 & (wasapi_devin->flags)) != 0){
    wasapi_devin->flags &= (~AGS_WASAPI_DEVIN_BUFFER0);
    wasapi_devin->flags |= AGS_WASAPI_DEVIN_BUFFER1;
  }else if((AGS_WASAPI_DEVIN_BUFFER1 & (wasapi_devin->flags)) != 0){
    wasapi_devin->flags &= (~AGS_WASAPI_DEVIN_BUFFER1);
    wasapi_devin->flags |= AGS_WASAPI_DEVIN_BUFFER2;
  }else if((AGS_WASAPI_DEVIN_BUFFER2 & (wasapi_devin->flags)) != 0){
    wasapi_devin->flags &= (~AGS_WASAPI_DEVIN_BUFFER2);
    wasapi_devin->flags |= AGS_WASAPI_DEVIN_BUFFER3;
  }else if((AGS_WASAPI_DEVIN_BUFFER3 & (wasapi_devin->flags)) != 0){
    wasapi_devin->flags &= (~AGS_WASAPI_DEVIN_BUFFER3);
    wasapi_devin->flags |= AGS_WASAPI_DEVIN_BUFFER4;
  }else if((AGS_WASAPI_DEVIN_BUFFER4 & (wasapi_devin->flags)) != 0){
    wasapi_devin->flags &= (~AGS_WASAPI_DEVIN_BUFFER4);
    wasapi_devin->flags |= AGS_WASAPI_DEVIN_BUFFER5;
  }else if((AGS_WASAPI_DEVIN_BUFFER5 & (wasapi_devin->flags)) != 0){
    wasapi_devin->flags &= (~AGS_WASAPI_DEVIN_BUFFER5);
    wasapi_devin->flags |= AGS_WASAPI_DEVIN_BUFFER6;
  }else if((AGS_WASAPI_DEVIN_BUFFER6 & (wasapi_devin->flags)) != 0){
    wasapi_devin->flags &= (~AGS_WASAPI_DEVIN_BUFFER6);
    wasapi_devin->flags |= AGS_WASAPI_DEVIN_BUFFER7;
  }else if((AGS_WASAPI_DEVIN_BUFFER7 & (wasapi_devin->flags)) != 0){
    wasapi_devin->flags &= (~AGS_WASAPI_DEVIN_BUFFER7);
    wasapi_devin->flags |= AGS_WASAPI_DEVIN_BUFFER0;
  }

  pthread_mutex_unlock(wasapi_devin_mutex);
}

/**
 * ags_wasapi_devin_adjust_delay_and_attack:
 * @wasapi_devin: the #AgsWasapiDevin
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 2.3.4
 */
void
ags_wasapi_devin_adjust_delay_and_attack(AgsWasapiDevin *wasapi_devin)
{
  gdouble delay;
  guint default_tact_frames;
  guint delay_tact_frames;
  guint default_period;
  gint next_attack;
  guint i;

  pthread_mutex_t *wasapi_devin_mutex;

  if(!AGS_IS_WASAPI_DEVIN(wasapi_devin)){
    return;
  }

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);
  
  /* get some initial values */
  delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(wasapi_devin));

#ifdef AGS_DEBUG
  g_message("delay : %f", delay);
#endif
  
  pthread_mutex_lock(wasapi_devin_mutex);

  default_tact_frames = (guint) (delay * wasapi_devin->buffer_size);
  delay_tact_frames = (guint) (floor(delay) * wasapi_devin->buffer_size);
  default_period = (1.0 / AGS_SOUNDCARD_DEFAULT_PERIOD) * (default_tact_frames);

  i = 0;
  
  wasapi_devin->attack[0] = (guint) floor(0.25 * wasapi_devin->buffer_size);
  next_attack = (((wasapi_devin->attack[i] + default_tact_frames) / wasapi_devin->buffer_size) - delay) * wasapi_devin->buffer_size;

  if(next_attack < 0){
    next_attack = 0;
  }

  if(next_attack >= wasapi_devin->buffer_size){
    next_attack = wasapi_devin->buffer_size - 1;
  }
  
  /* check if delay drops for next attack */
  if(next_attack < 0){
    wasapi_devin->attack[i] = wasapi_devin->attack[i] - ((gdouble) next_attack / 2.0);

    if(wasapi_devin->attack[i] < 0){
      wasapi_devin->attack[i] = 0;
    }
    
    if(wasapi_devin->attack[i] >= wasapi_devin->buffer_size){
      wasapi_devin->attack[i] = wasapi_devin->buffer_size - 1;
    }
    
    next_attack = next_attack + (next_attack / 2.0);

    if(next_attack < 0){
      next_attack = 0;
    }

    if(next_attack >= wasapi_devin->buffer_size){
      next_attack = wasapi_devin->buffer_size - 1;
    }
  }
  
  for(i = 1; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    wasapi_devin->attack[i] = next_attack;
    next_attack = (((wasapi_devin->attack[i] + default_tact_frames) / wasapi_devin->buffer_size) - delay) * wasapi_devin->buffer_size;

    if(next_attack >= wasapi_devin->buffer_size){
      next_attack = wasapi_devin->buffer_size - 1;
    }
    
    /* check if delay drops for next attack */
    if(next_attack < 0){
      wasapi_devin->attack[i] = wasapi_devin->attack[i] - ((gdouble) next_attack / 2.0);

      if(wasapi_devin->attack[i] < 0){
	wasapi_devin->attack[i] = 0;
      }

      if(wasapi_devin->attack[i] >= wasapi_devin->buffer_size){
	wasapi_devin->attack[i] = wasapi_devin->buffer_size - 1;
      }
    
      next_attack = next_attack + (next_attack / 2.0);

      if(next_attack < 0){
	next_attack = 0;
      }

      if(next_attack >= wasapi_devin->buffer_size){
	next_attack = wasapi_devin->buffer_size - 1;
      }
    }
    
#ifdef AGS_DEBUG
    g_message("%d", wasapi_devin->attack[i]);
#endif
  }

  wasapi_devin->attack[0] = wasapi_devin->attack[i - 2];
  
  for(i = 0; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD - 1; i++){
    wasapi_devin->delay[i] = ((gdouble) (default_tact_frames + wasapi_devin->attack[i] - wasapi_devin->attack[i + 1])) / (gdouble) wasapi_devin->buffer_size;
    
#ifdef AGS_DEBUG
    g_message("%f", wasapi_devin->delay[i]);
#endif
  }

  wasapi_devin->delay[i] = ((gdouble) (default_tact_frames + wasapi_devin->attack[i] - wasapi_devin->attack[0])) / (gdouble) wasapi_devin->buffer_size;

  pthread_mutex_unlock(wasapi_devin_mutex);
}

/**
 * ags_wasapi_devin_realloc_buffer:
 * @wasapi_devin: the #AgsWasapiDevin
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 2.3.4
 */
void
ags_wasapi_devin_realloc_buffer(AgsWasapiDevin *wasapi_devin)
{
  guint pcm_channels;
  guint buffer_size;
  guint format;
  guint word_size;

  pthread_mutex_t *wasapi_devin_mutex;  

  if(!AGS_IS_WASAPI_DEVIN(wasapi_devin)){
    return;
  }

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get word size */  
  pthread_mutex_lock(wasapi_devin_mutex);

  pcm_channels = wasapi_devin->pcm_channels;
  buffer_size = wasapi_devin->buffer_size;

  format = wasapi_devin->format;
  
  pthread_mutex_unlock(wasapi_devin_mutex);

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
    g_warning("ags_wasapi_devin_realloc_buffer(): unsupported word size");
    return;
  }
  
  /* AGS_WASAPI_DEVIN_BUFFER_0 */
  if(wasapi_devin->buffer[0] != NULL){
    free(wasapi_devin->buffer[0]);
  }
  
  wasapi_devin->buffer[0] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_WASAPI_DEVIN_BUFFER_1 */
  if(wasapi_devin->buffer[1] != NULL){
    free(wasapi_devin->buffer[1]);
  }

  wasapi_devin->buffer[1] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_WASAPI_DEVIN_BUFFER_2 */
  if(wasapi_devin->buffer[2] != NULL){
    free(wasapi_devin->buffer[2]);
  }

  wasapi_devin->buffer[2] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_WASAPI_DEVIN_BUFFER_3 */
  if(wasapi_devin->buffer[3] != NULL){
    free(wasapi_devin->buffer[3]);
  }
  
  wasapi_devin->buffer[3] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_WASAPI_DEVIN_BUFFER_4 */
  if(wasapi_devin->buffer[4] != NULL){
    free(wasapi_devin->buffer[4]);
  }
  
  wasapi_devin->buffer[4] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_WASAPI_DEVIN_BUFFER_5 */
  if(wasapi_devin->buffer[5] != NULL){
    free(wasapi_devin->buffer[5]);
  }
  
  wasapi_devin->buffer[5] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_WASAPI_DEVIN_BUFFER_6 */
  if(wasapi_devin->buffer[6] != NULL){
    free(wasapi_devin->buffer[6]);
  }
  
  wasapi_devin->buffer[6] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_WASAPI_DEVIN_BUFFER_7 */
  if(wasapi_devin->buffer[7] != NULL){
    free(wasapi_devin->buffer[7]);
  }
  
  wasapi_devin->buffer[7] = (void *) malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_wasapi_devin_new:
 * @application_context: the #AgsApplicationContext
 *
 * Creates a new instance of #AgsWasapiDevin.
 *
 * Returns: a new #AgsWasapiDevin
 *
 * Since: 2.3.4
 */
AgsWasapiDevin*
ags_wasapi_devin_new(AgsApplicationContext *application_context)
{
  AgsWasapiDevin *wasapi_devin;

  wasapi_devin = (AgsWasapiDevin *) g_object_new(AGS_TYPE_WASAPI_DEVIN,
						 "application-context", application_context,
						 NULL);
  
  return(wasapi_devin);
}
