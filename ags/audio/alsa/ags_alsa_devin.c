/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/audio/alsa/ags_alsa_devin.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_soundcard_util.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <ags/i18n.h>

void ags_alsa_devin_class_init(AgsAlsaDevinClass *alsa_devin);
void ags_alsa_devin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_alsa_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_alsa_devin_init(AgsAlsaDevin *alsa_devin);
void ags_alsa_devin_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_alsa_devin_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_alsa_devin_dispose(GObject *gobject);
void ags_alsa_devin_finalize(GObject *gobject);

AgsUUID* ags_alsa_devin_get_uuid(AgsConnectable *connectable);
gboolean ags_alsa_devin_has_resource(AgsConnectable *connectable);
gboolean ags_alsa_devin_is_ready(AgsConnectable *connectable);
void ags_alsa_devin_add_to_registry(AgsConnectable *connectable);
void ags_alsa_devin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_alsa_devin_list_resource(AgsConnectable *connectable);
xmlNode* ags_alsa_devin_xml_compose(AgsConnectable *connectable);
void ags_alsa_devin_xml_parse(AgsConnectable *connectable,
			      xmlNode *node);
gboolean ags_alsa_devin_is_connected(AgsConnectable *connectable);
void ags_alsa_devin_connect(AgsConnectable *connectable);
void ags_alsa_devin_disconnect(AgsConnectable *connectable);

void ags_alsa_devin_set_device(AgsSoundcard *soundcard,
			       gchar *device);
gchar* ags_alsa_devin_get_device(AgsSoundcard *soundcard);

void ags_alsa_devin_set_presets(AgsSoundcard *soundcard,
				guint channels,
				guint rate,
				guint buffer_size,
				guint format);
void ags_alsa_devin_get_presets(AgsSoundcard *soundcard,
				guint *channels,
				guint *rate,
				guint *buffer_size,
				guint *format);

void ags_alsa_devin_list_cards(AgsSoundcard *soundcard,
			       GList **card_id, GList **card_name);
void ags_alsa_devin_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			     guint *channels_min, guint *channels_max,
			     guint *rate_min, guint *rate_max,
			     guint *buffer_size_min, guint *buffer_size_max,
			     GError **error);
guint ags_alsa_devin_get_capability(AgsSoundcard *soundcard);

gboolean ags_alsa_devin_is_available(AgsSoundcard *soundcard);

gboolean ags_alsa_devin_is_starting(AgsSoundcard *soundcard);
gboolean ags_alsa_devin_is_recording(AgsSoundcard *soundcard);

gchar* ags_alsa_devin_get_uptime(AgsSoundcard *soundcard);

void ags_alsa_devin_device_record_init(AgsSoundcard *soundcard,
				       GError **error);

void ags_alsa_devin_device_fill_backend_buffer(void *app_buffer,
					       guint ags_format,
					       guchar *backend_buffer,
					       guint channels,
					       guint buffer_size);

void ags_alsa_devin_device_record(AgsSoundcard *soundcard,
				  GError **error);
void ags_alsa_devin_device_free(AgsSoundcard *soundcard);

void ags_alsa_devin_tic(AgsSoundcard *soundcard);
void ags_alsa_devin_offset_changed(AgsSoundcard *soundcard,
				   guint note_offset);

void ags_alsa_devin_set_bpm(AgsSoundcard *soundcard,
			    gdouble bpm);
gdouble ags_alsa_devin_get_bpm(AgsSoundcard *soundcard);

void ags_alsa_devin_set_delay_factor(AgsSoundcard *soundcard,
				     gdouble delay_factor);
gdouble ags_alsa_devin_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_alsa_devin_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_alsa_devin_get_delay(AgsSoundcard *soundcard);
guint ags_alsa_devin_get_attack(AgsSoundcard *soundcard);

void* ags_alsa_devin_get_buffer(AgsSoundcard *soundcard);
void* ags_alsa_devin_get_next_buffer(AgsSoundcard *soundcard);
void* ags_alsa_devin_get_prev_buffer(AgsSoundcard *soundcard);

void ags_alsa_devin_lock_buffer(AgsSoundcard *soundcard,
				void *buffer);
void ags_alsa_devin_unlock_buffer(AgsSoundcard *soundcard,
				  void *buffer);

guint ags_alsa_devin_get_delay_counter(AgsSoundcard *soundcard);

void ags_alsa_devin_set_start_note_offset(AgsSoundcard *soundcard,
					  guint start_note_offset);
guint ags_alsa_devin_get_start_note_offset(AgsSoundcard *soundcard);

void ags_alsa_devin_set_note_offset(AgsSoundcard *soundcard,
				    guint note_offset);
guint ags_alsa_devin_get_note_offset(AgsSoundcard *soundcard);

void ags_alsa_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
					     guint note_offset);
guint ags_alsa_devin_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_alsa_devin_set_loop(AgsSoundcard *soundcard,
			     guint loop_left, guint loop_right,
			     gboolean do_loop);
void ags_alsa_devin_get_loop(AgsSoundcard *soundcard,
			     guint *loop_left, guint *loop_right,
			     gboolean *do_loop);

guint ags_alsa_devin_get_loop_offset(AgsSoundcard *soundcard);

/**
 * SECTION:ags_alsa_devin
 * @short_description: Input from soundcard
 * @title: AgsAlsaDevin
 * @section_id:
 * @include: ags/audio/ags_alsa_devin.h
 *
 * #AgsAlsaDevin represents a soundcard and supports input.
 */

enum{
  PROP_0,
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

static gpointer ags_alsa_devin_parent_class = NULL;

GType
ags_alsa_devin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_alsa_devin = 0;

    static const GTypeInfo ags_alsa_devin_info = {
      sizeof(AgsAlsaDevinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_alsa_devin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAlsaDevin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_alsa_devin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_alsa_devin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_alsa_devin_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_alsa_devin = g_type_register_static(G_TYPE_OBJECT,
						 "AgsAlsaDevin",
						 &ags_alsa_devin_info,
						 0);

    g_type_add_interface_static(ags_type_alsa_devin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_alsa_devin,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_alsa_devin);
  }

  return g_define_type_id__volatile;
}

GType
ags_alsa_devin_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_ALSA_DEVIN_ADDED_TO_REGISTRY, "AGS_ALSA_DEVIN_ADDED_TO_REGISTRY", "alsa-devin-added-to-registry" },
      { AGS_ALSA_DEVIN_CONNECTED, "AGS_ALSA_DEVIN_CONNECTED", "alsa-devin-connected" },
      { AGS_ALSA_DEVIN_INITIALIZED, "AGS_ALSA_DEVIN_INITIALIZED", "alsa-devin-initialized" },
      { AGS_ALSA_DEVIN_START_RECORD, "AGS_ALSA_DEVIN_START_RECORD", "alsa-devin-start-record" },
      { AGS_ALSA_DEVIN_RECORD, "AGS_ALSA_DEVIN_RECORD", "alsa-devin-record" },
      { AGS_ALSA_DEVIN_SHUTDOWN, "AGS_ALSA_DEVIN_SHUTDOWN", "alsa-devin-shutdown" },
      { AGS_ALSA_DEVIN_NONBLOCKING, "AGS_ALSA_DEVIN_NONBLOCKING", "alsa-devin-nonblocking" },
      { AGS_ALSA_DEVIN_ATTACK_FIRST, "AGS_ALSA_DEVIN_ATTACK_FIRST", "alsa-devin-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsAlsaDevinFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

void
ags_alsa_devin_class_init(AgsAlsaDevinClass *alsa_devin)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_alsa_devin_parent_class = g_type_class_peek_parent(alsa_devin);

  /* GObjectClass */
  gobject = (GObjectClass *) alsa_devin;

  gobject->set_property = ags_alsa_devin_set_property;
  gobject->get_property = ags_alsa_devin_get_property;

  gobject->dispose = ags_alsa_devin_dispose;
  gobject->finalize = ags_alsa_devin_finalize;

  /* properties */
  /**
   * AgsAlsaDevin:device:
   *
   * The alsa soundcard indentifier
   * 
   * Since: 3.13.2
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   AGS_ALSA_DEVIN_DEFAULT_ALSA_DEVICE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsAlsaDevin:dsp-channels:
   *
   * The dsp channel count
   * 
   * Since: 3.13.2
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
   * AgsAlsaDevin:pcm-channels:
   *
   * The pcm channel count
   * 
   * Since: 3.13.2
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
   * AgsAlsaDevin:format:
   *
   * The precision of the buffer
   * 
   * Since: 3.13.2
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
   * AgsAlsaDevin:buffer-size:
   *
   * The buffer size
   * 
   * Since: 3.13.2
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
   * AgsAlsaDevin:samplerate:
   *
   * The samplerate
   * 
   * Since: 3.13.2
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("frames per second"),
				 i18n_pspec("The frames count recorded during a second"),
				 AGS_SOUNDCARD_MIN_SAMPLERATE,
				 AGS_SOUNDCARD_MAX_SAMPLERATE,
				 AGS_SOUNDCARD_DEFAULT_SAMPLERATE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsAlsaDevin:buffer:
   *
   * The buffer
   * 
   * Since: 3.13.2
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to record"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsAlsaDevin:bpm:
   *
   * Beats per minute
   * 
   * Since: 3.13.2
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
   * AgsAlsaDevin:delay-factor:
   *
   * tact
   * 
   * Since: 3.13.2
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
   * AgsAlsaDevin:attack:
   *
   * Attack of the buffer
   * 
   * Since: 3.13.2
   */
  param_spec = g_param_spec_pointer("attack",
				    i18n_pspec("attack of buffer"),
				    i18n_pspec("The attack to use for the buffer"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);
  
  /* AgsAlsaDevinClass */
}

GQuark
ags_alsa_devin_error_quark()
{
  return(g_quark_from_static_string("ags-alsa-devin-error-quark"));
}

void
ags_alsa_devin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_alsa_devin_get_uuid;
  connectable->has_resource = ags_alsa_devin_has_resource;

  connectable->is_ready = ags_alsa_devin_is_ready;
  connectable->add_to_registry = ags_alsa_devin_add_to_registry;
  connectable->remove_from_registry = ags_alsa_devin_remove_from_registry;

  connectable->list_resource = ags_alsa_devin_list_resource;
  connectable->xml_compose = ags_alsa_devin_xml_compose;
  connectable->xml_parse = ags_alsa_devin_xml_parse;

  connectable->is_connected = ags_alsa_devin_is_connected;  
  connectable->connect = ags_alsa_devin_connect;
  connectable->disconnect = ags_alsa_devin_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_alsa_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_alsa_devin_set_device;
  soundcard->get_device = ags_alsa_devin_get_device;
  
  soundcard->set_presets = ags_alsa_devin_set_presets;
  soundcard->get_presets = ags_alsa_devin_get_presets;

  soundcard->list_cards = ags_alsa_devin_list_cards;
  soundcard->pcm_info = ags_alsa_devin_pcm_info;
  soundcard->get_capability = ags_alsa_devin_get_capability;

  soundcard->is_available = ags_alsa_devin_is_available;

  soundcard->is_starting =  ags_alsa_devin_is_starting;
  soundcard->is_playing = NULL;
  soundcard->is_recording = ags_alsa_devin_is_recording;

  soundcard->get_uptime = ags_alsa_devin_get_uptime;
  
  soundcard->play_init = NULL;
  soundcard->play = NULL;

  soundcard->record_init = ags_alsa_devin_device_record_init;
  soundcard->record = ags_alsa_devin_device_record;
  
  soundcard->stop = ags_alsa_devin_device_free;

  soundcard->tic = ags_alsa_devin_tic;
  soundcard->offset_changed = ags_alsa_devin_offset_changed;
    
  soundcard->set_bpm = ags_alsa_devin_set_bpm;
  soundcard->get_bpm = ags_alsa_devin_get_bpm;

  soundcard->set_delay_factor = ags_alsa_devin_set_delay_factor;
  soundcard->get_delay_factor = ags_alsa_devin_get_delay_factor;

  soundcard->get_absolute_delay = ags_alsa_devin_get_absolute_delay;

  soundcard->get_delay = ags_alsa_devin_get_delay;
  soundcard->get_attack = ags_alsa_devin_get_attack;

  soundcard->get_buffer = ags_alsa_devin_get_buffer;
  soundcard->get_next_buffer = ags_alsa_devin_get_next_buffer;
  soundcard->get_prev_buffer = ags_alsa_devin_get_prev_buffer;

  soundcard->lock_buffer = ags_alsa_devin_lock_buffer;
  soundcard->unlock_buffer = ags_alsa_devin_unlock_buffer;

  soundcard->get_delay_counter = ags_alsa_devin_get_delay_counter;

  soundcard->set_start_note_offset = ags_alsa_devin_set_start_note_offset;
  soundcard->get_start_note_offset = ags_alsa_devin_get_start_note_offset;

  soundcard->set_note_offset = ags_alsa_devin_set_note_offset;
  soundcard->get_note_offset = ags_alsa_devin_get_note_offset;

  soundcard->set_note_offset_absolute = ags_alsa_devin_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_alsa_devin_get_note_offset_absolute;

  soundcard->set_loop = ags_alsa_devin_set_loop;
  soundcard->get_loop = ags_alsa_devin_get_loop;

  soundcard->get_loop_offset = ags_alsa_devin_get_loop_offset;
}

void
ags_alsa_devin_init(AgsAlsaDevin *alsa_devin)
{  
  AgsConfig *config;
  
  gchar *str;
  gchar *segmentation;

  guint denominator, numerator;
  guint i;
  
  alsa_devin->flags = 0;

  /* insert alsa devin mutex */
  g_rec_mutex_init(&(alsa_devin->obj_mutex));

  /* uuid */
  alsa_devin->uuid = ags_uuid_alloc();
  ags_uuid_generate(alsa_devin->uuid);

  /* config */
  config = ags_config_get_instance();

  /* presets */
  alsa_devin->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  alsa_devin->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  alsa_devin->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  alsa_devin->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  alsa_devin->format = ags_soundcard_helper_config_get_format(config);

  /* device */
  alsa_devin->handle = NULL;
  alsa_devin->device = g_strdup(AGS_ALSA_DEVIN_DEFAULT_ALSA_DEVICE);

  /* app buffer */
  alsa_devin->app_buffer_mode = AGS_ALSA_DEVIN_APP_BUFFER_0;

  alsa_devin->app_buffer_mutex = (GRecMutex **) g_malloc(AGS_ALSA_DEVIN_DEFAULT_APP_BUFFER_SIZE * sizeof(GRecMutex *));

  for(i = 0; i < AGS_ALSA_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    alsa_devin->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(alsa_devin->app_buffer_mutex[i]);
  }
  
  alsa_devin->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  alsa_devin->sub_block_mutex = (GRecMutex **) g_malloc(AGS_ALSA_DEVIN_DEFAULT_APP_BUFFER_SIZE * alsa_devin->sub_block_count * alsa_devin->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < AGS_ALSA_DEVIN_DEFAULT_APP_BUFFER_SIZE * alsa_devin->sub_block_count * alsa_devin->pcm_channels; i++){
    alsa_devin->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(alsa_devin->sub_block_mutex[i]);
  }

  alsa_devin->app_buffer = (void **) g_malloc(AGS_ALSA_DEVIN_DEFAULT_APP_BUFFER_SIZE * sizeof(void *));
  
  for(i = 0; i < AGS_ALSA_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    alsa_devin->app_buffer[i] = NULL;
  }

  g_atomic_int_set(&(alsa_devin->available),
		   FALSE);
  
  alsa_devin->backend_buffer_mode = AGS_ALSA_DEVIN_BACKEND_BUFFER_0;
  
  alsa_devin->backend_buffer = (guchar **) g_malloc(AGS_ALSA_DEVIN_DEFAULT_BACKEND_BUFFER_SIZE * sizeof(guchar *));

  for(i = 0; i < AGS_ALSA_DEVIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    alsa_devin->backend_buffer[i] = NULL;
  }

  ags_alsa_devin_realloc_buffer(alsa_devin);
  
  /* bpm */
  alsa_devin->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  alsa_devin->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    alsa_devin->delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }

  /* delay and attack */
  alsa_devin->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					 sizeof(gdouble));
  
  alsa_devin->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					sizeof(guint));

  ags_alsa_devin_adjust_delay_and_attack(alsa_devin);
  
  /* counters */
  alsa_devin->tact_counter = 0.0;
  alsa_devin->delay_counter = 0;
  alsa_devin->tic_counter = 0;

  alsa_devin->start_note_offset = 0;
  alsa_devin->note_offset = 0;
  alsa_devin->note_offset_absolute = 0;

  alsa_devin->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  alsa_devin->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  alsa_devin->do_loop = FALSE;

  alsa_devin->loop_offset = 0;  
}

void
ags_alsa_devin_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsAlsaDevin *alsa_devin;

  GRecMutex *alsa_devin_mutex;

  alsa_devin = AGS_ALSA_DEVIN(gobject);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    char *device;

    device = (char *) g_value_get_string(value);

    g_rec_mutex_lock(alsa_devin_mutex);

    g_free(alsa_devin->device);
    
    alsa_devin->device = g_strdup(device);

    g_rec_mutex_unlock(alsa_devin_mutex);
  }
  break;
  case PROP_DSP_CHANNELS:
  {
    guint dsp_channels;

    dsp_channels = g_value_get_uint(value);

    g_rec_mutex_lock(alsa_devin_mutex);

    if(dsp_channels == alsa_devin->dsp_channels){
      g_rec_mutex_unlock(alsa_devin_mutex);
	
      return;
    }

    alsa_devin->dsp_channels = dsp_channels;

    g_rec_mutex_unlock(alsa_devin_mutex);
  }
  break;
  case PROP_PCM_CHANNELS:
  {
    guint pcm_channels;

    pcm_channels = g_value_get_uint(value);

    g_rec_mutex_lock(alsa_devin_mutex);

    if(pcm_channels == alsa_devin->pcm_channels){
      g_rec_mutex_unlock(alsa_devin_mutex);

      return;
    }

    alsa_devin->pcm_channels = pcm_channels;

    g_rec_mutex_unlock(alsa_devin_mutex);

    ags_alsa_devin_realloc_buffer(alsa_devin);
  }
  break;
  case PROP_FORMAT:
  {
    guint format;

    format = g_value_get_uint(value);

    g_rec_mutex_lock(alsa_devin_mutex);

    if(format == alsa_devin->format){
      g_rec_mutex_unlock(alsa_devin_mutex);

      return;
    }

    alsa_devin->format = format;

    g_rec_mutex_unlock(alsa_devin_mutex);

    ags_alsa_devin_realloc_buffer(alsa_devin);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    guint buffer_size;

    buffer_size = g_value_get_uint(value);

    g_rec_mutex_lock(alsa_devin_mutex);

    if(buffer_size == alsa_devin->buffer_size){
      g_rec_mutex_unlock(alsa_devin_mutex);

      return;
    }

    alsa_devin->buffer_size = buffer_size;

    g_rec_mutex_unlock(alsa_devin_mutex);

    ags_alsa_devin_realloc_buffer(alsa_devin);
    ags_alsa_devin_adjust_delay_and_attack(alsa_devin);
  }
  break;
  case PROP_SAMPLERATE:
  {
    guint samplerate;

    samplerate = g_value_get_uint(value);

    g_rec_mutex_lock(alsa_devin_mutex);

    if(samplerate == alsa_devin->samplerate){
      g_rec_mutex_unlock(alsa_devin_mutex);
	
      return;
    }

    alsa_devin->samplerate = samplerate;

    g_rec_mutex_unlock(alsa_devin_mutex);

    ags_alsa_devin_adjust_delay_and_attack(alsa_devin);
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

    g_rec_mutex_lock(alsa_devin_mutex);

    if(bpm == alsa_devin->bpm){
      g_rec_mutex_unlock(alsa_devin_mutex);

      return;
    }

    alsa_devin->bpm = bpm;

    g_rec_mutex_unlock(alsa_devin_mutex);

    ags_alsa_devin_adjust_delay_and_attack(alsa_devin);
  }
  break;
  case PROP_DELAY_FACTOR:
  {
    gdouble delay_factor;
      
    delay_factor = g_value_get_double(value);

    g_rec_mutex_lock(alsa_devin_mutex);

    if(delay_factor == alsa_devin->delay_factor){
      g_rec_mutex_unlock(alsa_devin_mutex);

      return;
    }

    alsa_devin->delay_factor = delay_factor;

    g_rec_mutex_unlock(alsa_devin_mutex);

    ags_alsa_devin_adjust_delay_and_attack(alsa_devin);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_alsa_devin_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsAlsaDevin *alsa_devin;

  GRecMutex *alsa_devin_mutex;

  alsa_devin = AGS_ALSA_DEVIN(gobject);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    g_rec_mutex_lock(alsa_devin_mutex);

    g_value_set_string(value, alsa_devin->device);

    g_rec_mutex_unlock(alsa_devin_mutex);
  }
  break;
  case PROP_DSP_CHANNELS:
  {
    g_rec_mutex_lock(alsa_devin_mutex);

    g_value_set_uint(value, alsa_devin->dsp_channels);

    g_rec_mutex_unlock(alsa_devin_mutex);
  }
  break;
  case PROP_PCM_CHANNELS:
  {
    g_rec_mutex_lock(alsa_devin_mutex);

    g_value_set_uint(value, alsa_devin->pcm_channels);

    g_rec_mutex_unlock(alsa_devin_mutex);
  }
  break;
  case PROP_FORMAT:
  {
    g_rec_mutex_lock(alsa_devin_mutex);

    g_value_set_uint(value, alsa_devin->format);

    g_rec_mutex_unlock(alsa_devin_mutex);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    g_rec_mutex_lock(alsa_devin_mutex);

    g_value_set_uint(value, alsa_devin->buffer_size);

    g_rec_mutex_unlock(alsa_devin_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    g_rec_mutex_lock(alsa_devin_mutex);

    g_value_set_uint(value, alsa_devin->samplerate);

    g_rec_mutex_unlock(alsa_devin_mutex);
  }
  break;
  case PROP_BUFFER:
  {
    g_rec_mutex_lock(alsa_devin_mutex);

    g_value_set_pointer(value, alsa_devin->buffer);

    g_rec_mutex_unlock(alsa_devin_mutex);
  }
  break;
  case PROP_BPM:
  {
    g_rec_mutex_lock(alsa_devin_mutex);

    g_value_set_double(value, alsa_devin->bpm);

    g_rec_mutex_unlock(alsa_devin_mutex);
  }
  break;
  case PROP_DELAY_FACTOR:
  {
    g_rec_mutex_lock(alsa_devin_mutex);

    g_value_set_double(value, alsa_devin->delay_factor);

    g_rec_mutex_unlock(alsa_devin_mutex);
  }
  break;
  case PROP_ATTACK:
  {
    g_rec_mutex_lock(alsa_devin_mutex);

    g_value_set_pointer(value, alsa_devin->attack);

    g_rec_mutex_unlock(alsa_devin_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_alsa_devin_dispose(GObject *gobject)
{
  AgsAlsaDevin *alsa_devin;

  GList *list;

  alsa_devin = AGS_ALSA_DEVIN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_alsa_devin_parent_class)->dispose(gobject);
}

void
ags_alsa_devin_finalize(GObject *gobject)
{
  AgsAlsaDevin *alsa_devin;

  alsa_devin = AGS_ALSA_DEVIN(gobject);

  ags_uuid_free(alsa_devin->uuid);

  for(i = 0; i < AGS_ALSA_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_free(alsa_devin->app_buffer[i]);
  }

  g_free(alsa_devin->app_buffer);

  for(i = 0; i < AGS_ALSA_DEVIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    g_free(alsa_devin->backend_buffer[i]);
  }

  g_free(alsa_devin->backend_buffer);

  for(i = 0; i < AGS_ALSA_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_rec_mutex_clear(alsa_devin->app_buffer_mutex[i]);
    
    g_free(alsa_devin->app_buffer_mutex[i])
  }

  g_free(alsa_devin->app_buffer_mutex);
  
  for(i = 0; i < AGS_ALSA_DEVIN_DEFAULT_APP_BUFFER_SIZE * alsa_devin->sub_block_count * alsa_devin->pcm_channels; i++){
    g_rec_mutex_clear(alsa_devin->sub_block_mutex[i]);
    
    g_free(alsa_devin->sub_block_mutex[i]);
  }

  g_free(alsa_devin->sub_block_mutex);
  
  g_free(alsa_devin->delay);
  g_free(alsa_devin->attack);
  
  g_free(alsa_devin->device);

  /* call parent */
  G_OBJECT_CLASS(ags_alsa_devin_parent_class)->finalize(gobject);
}

AgsUUID*
ags_alsa_devin_get_uuid(AgsConnectable *connectable)
{
  AgsAlsaDevin *alsa_devin;
  
  AgsUUID *ptr;

  GRecMutex *alsa_devin_mutex;

  alsa_devin = AGS_ALSA_DEVIN(connectable);

  /* get alsa_devin signal mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* get UUID */
  g_rec_mutex_lock(alsa_devin_mutex);

  ptr = alsa_devin->uuid;

  g_rec_mutex_unlock(alsa_devin_mutex);
  
  return(ptr);
}

gboolean
ags_alsa_devin_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_alsa_devin_is_ready(AgsConnectable *connectable)
{
  AgsAlsaDevin *alsa_devin;
  
  gboolean is_ready;

  alsa_devin = AGS_ALSA_DEVIN(connectable);

  /* check is added */
  is_ready = ags_alsa_devin_test_flags(alsa_devin, AGS_ALSA_DEVIN_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_alsa_devin_add_to_registry(AgsConnectable *connectable)
{
  AgsAlsaDevin *alsa_devin;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  alsa_devin = AGS_ALSA_DEVIN(connectable);

  ags_alsa_devin_set_flags(alsa_devin, AGS_ALSA_DEVIN_ADDED_TO_REGISTRY);
}

void
ags_alsa_devin_remove_from_registry(AgsConnectable *connectable)
{
  AgsAlsaDevin *alsa_devin;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  alsa_devin = AGS_ALSA_DEVIN(connectable);

  ags_alsa_devin_unset_flags(alsa_devin, AGS_ALSA_DEVIN_ADDED_TO_REGISTRY);
}

xmlNode*
ags_alsa_devin_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_alsa_devin_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_alsa_devin_xml_parse(AgsConnectable *connectable,
			 xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_alsa_devin_is_connected(AgsConnectable *connectable)
{
  AgsAlsaDevin *alsa_devin;
  
  gboolean is_connected;

  alsa_devin = AGS_ALSA_DEVIN(connectable);

  /* check is connected */
  is_connected = ags_alsa_devin_test_flags(alsa_devin, AGS_ALSA_DEVIN_CONNECTED);
  
  return(is_connected);
}

void
ags_alsa_devin_connect(AgsConnectable *connectable)
{
  AgsAlsaDevin *alsa_devin;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  alsa_devin = AGS_ALSA_DEVIN(connectable);

  ags_alsa_devin_set_flags(alsa_devin, AGS_ALSA_DEVIN_CONNECTED);
}

void
ags_alsa_devin_disconnect(AgsConnectable *connectable)
{

  AgsAlsaDevin *alsa_devin;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  alsa_devin = AGS_ALSA_DEVIN(connectable);
  
  ags_alsa_devin_unset_flags(alsa_devin, AGS_ALSA_DEVIN_CONNECTED);
}

/**
 * ags_alsa_devin_test_flags:
 * @alsa_devin: the #AgsAlsaDevin
 * @flags: the flags
 *
 * Test @flags to be set on @alsa_devin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.13.2
 */
gboolean
ags_alsa_devin_test_flags(AgsAlsaDevin *alsa_devin, guint flags)
{
  gboolean retval;  
  
  GRecMutex *alsa_devin_mutex;

  if(!AGS_IS_ALSA_DEVIN(alsa_devin)){
    return(FALSE);
  }

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* test */
  g_rec_mutex_lock(alsa_devin_mutex);

  retval = (flags & (alsa_devin->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(alsa_devin_mutex);

  return(retval);
}

/**
 * ags_alsa_devin_set_flags:
 * @alsa_devin: the #AgsAlsaDevin
 * @flags: see #AgsAlsaDevinFlags-enum
 *
 * Enable a feature of @alsa_devin.
 *
 * Since: 3.13.2
 */
void
ags_alsa_devin_set_flags(AgsAlsaDevin *alsa_devin, guint flags)
{
  GRecMutex *alsa_devin_mutex;

  if(!AGS_IS_ALSA_DEVIN(alsa_devin)){
    return;
  }

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(alsa_devin_mutex);

  alsa_devin->flags |= flags;
  
  g_rec_mutex_unlock(alsa_devin_mutex);
}
    
/**
 * ags_alsa_devin_unset_flags:
 * @alsa_devin: the #AgsAlsaDevin
 * @flags: see #AgsAlsaDevinFlags-enum
 *
 * Disable a feature of @alsa_devin.
 *
 * Since: 3.13.2
 */
void
ags_alsa_devin_unset_flags(AgsAlsaDevin *alsa_devin, guint flags)
{  
  GRecMutex *alsa_devin_mutex;

  if(!AGS_IS_ALSA_DEVIN(alsa_devin)){
    return;
  }

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(alsa_devin_mutex);

  alsa_devin->flags &= (~flags);
  
  g_rec_mutex_unlock(alsa_devin_mutex);
}

void
ags_alsa_devin_set_device(AgsSoundcard *soundcard,
			  gchar *device)
{
  AgsAlsaDevin *alsa_devin;

  GList *card_id, *card_id_start, *card_name, *card_name_start;

  GRecMutex *alsa_devin_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* list cards */
  card_id = NULL;
  card_name = NULL;
  
  ags_soundcard_list_cards(soundcard,
			   &card_id, &card_name);

  card_id_start = card_id;
  card_name_start = card_name;
  
  /* check card */
  g_rec_mutex_lock(alsa_devin_mutex);

  while(card_id != NULL){
    if(!g_ascii_strncasecmp(card_id->data,
			    device,
			    strlen(card_id->data))){
      alsa_devin->device = g_strdup(device);

      break;
    }
    
    card_id = card_id->next;
  }

  g_rec_mutex_unlock(alsa_devin_mutex);

  /* free card id and name */
  g_list_free_full(card_id_start,
		   g_free);
  g_list_free_full(card_name_start,
		   g_free);
}

gchar*
ags_alsa_devin_get_device(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  gchar *device;
  
  GRecMutex *alsa_devin_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);
  
  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  device = NULL;
  
  g_rec_mutex_lock(alsa_devin_mutex);

  device = g_strdup(alsa_devin->device);

  g_rec_mutex_unlock(alsa_devin_mutex);

  return(device);
}

void
ags_alsa_devin_set_presets(AgsSoundcard *soundcard,
			   guint channels,
			   guint rate,
			   guint buffer_size,
			   guint format)
{
  AgsAlsaDevin *alsa_devin;

  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  g_object_set(alsa_devin,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_alsa_devin_get_presets(AgsSoundcard *soundcard,
			   guint *channels,
			   guint *rate,
			   guint *buffer_size,
			   guint *format)
{
  AgsAlsaDevin *alsa_devin;

  GRecMutex *alsa_devin_mutex;

  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* get presets */
  g_rec_mutex_lock(alsa_devin_mutex);

  if(channels != NULL){
    *channels = alsa_devin->pcm_channels;
  }

  if(rate != NULL){
    *rate = alsa_devin->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = alsa_devin->buffer_size;
  }

  if(format != NULL){
    *format = alsa_devin->format;
  }

  g_rec_mutex_unlock(alsa_devin_mutex);
}

void
ags_alsa_devin_list_cards(AgsSoundcard *soundcard,
			  GList **card_id, GList **card_name)
{
  AgsAlsaDevin *alsa_devin;

#if defined(AGS_WITH_ALSA)
  snd_ctl_t *card_handle;
  snd_ctl_card_info_t *card_info;
#endif
  
  char *name;
  gchar *str;
  gchar *str_err;
  
  int card_num;
  int device;
  int error;

  GRecMutex *alsa_devin_mutex;

  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  g_rec_mutex_lock(alsa_devin_mutex);

#if defined(AGS_WITH_ALSA)
  card_num = -1;

  while(TRUE){
    char *iface;
    char **hints, **iter;

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
      snd_ctl_close(card_handle);
      g_free(str);
      
      continue;
    }

    device = -1;
    error = snd_ctl_pcm_next_device(card_handle, &device);

    if(error < 0){
      snd_ctl_close(card_handle);
      g_free(str);
      
      continue;
    }

    iface = "pcm";
    hints = NULL;
            
    error = snd_device_name_hint(card_num,
				 iface,
				 &hints);

    if(hints != NULL){
      for(iter = hints; iter[0] != NULL; iter++){
	if(card_id != NULL){
	  char *hint;

	  hint = snd_device_name_get_hint(iter[0],
					  "NAME");
	    
	  *card_id = g_list_prepend(*card_id,
				    g_strdup(hint));

	  if(hint != NULL){
	    free(hint);
	  }
	}

	if(card_name != NULL){
	  char *name;

	  name = snd_ctl_card_info_get_name(card_info);

	  *card_name = g_list_prepend(*card_name,
				      g_strdup(name));
	}
      }

      snd_device_name_free_hint(hints);
    }
    
    snd_ctl_close(card_handle);
  }

  snd_config_update_free_global();
#endif

  g_rec_mutex_unlock(alsa_devin_mutex);

  if(card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

void
ags_alsa_devin_pcm_info(AgsSoundcard *soundcard,
			char *card_id,
			guint *channels_min, guint *channels_max,
			guint *rate_min, guint *rate_max,
			guint *buffer_size_min, guint *buffer_size_max,
			GError **error)
{
  AgsAlsaDevin *alsa_devin;

#if defined(AGS_WITH_ALSA)
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
#endif
  
  gchar *str;
    
  unsigned int val;
  int dir;
  snd_pcm_uframes_t frames;

  int rc;
  int err;

  GRecMutex *alsa_devin_mutex;

  if(card_id == NULL){
    return;
  }
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);
  

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* pcm info */
  g_rec_mutex_lock(alsa_devin_mutex);

#if defined(AGS_WITH_ALSA)
  /* Open PCM device for recordback. */
  handle = NULL;

  rc = snd_pcm_open(&handle, card_id, SND_PCM_STREAM_CAPTURE, 0);

  if(rc < 0) {
    str = snd_strerror(rc);
    g_message("unable to open pcm device: %s\n", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVIN_ERROR,
		  AGS_ALSA_DEVIN_ERROR_LOCKED_SOUNDCARD,
		  "unable to open pcm device: %s\n",
		  str);
    }
    
    //    free(str);
    
    goto ags_alsa_devin_pcm_info_ERR;
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* channels */
  snd_pcm_hw_params_get_channels_min(params, &val);

  if(channels_min != NULL){
    channels_min[0] = val;
  }
  
  snd_pcm_hw_params_get_channels_max(params, &val);

  if(channels_max != NULL){
    channels_max[0] = val;
  }
  
  /* samplerate */
  dir = 0;
  snd_pcm_hw_params_get_rate_min(params, &val, &dir);

  if(rate_min != NULL){
    rate_min[0] = val;
  }
  
  dir = 0;
  snd_pcm_hw_params_get_rate_max(params, &val, &dir);

  if(rate_max != NULL){
    rate_max[0] = val;
  }
  
  /* buffer size */
  dir = 0;
  snd_pcm_hw_params_get_buffer_size_min(params, &frames);

  if(buffer_size_min != NULL){
    buffer_size_min[0] = frames;
  }

  dir = 0;
  snd_pcm_hw_params_get_buffer_size_max(params, &frames);

  if(buffer_size_max != NULL){
    buffer_size_max[0] = frames;
  }
  
  snd_pcm_close(handle);
#endif

ags_alsa_devin_pcm_info_ERR:

  g_rec_mutex_unlock(alsa_devin_mutex);
}

guint
ags_alsa_devin_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_CAPTURE);
}

gboolean
ags_alsa_devin_is_available(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  gboolean retval;
  
  GRecMutex *alsa_devin_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);
  
  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* check available */
  retval = FALSE;

  //TODO:JK: implement me

  return(retval);
}

gboolean
ags_alsa_devin_is_starting(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  gboolean is_starting;
  
  GRecMutex *alsa_devin_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* check is starting */
  g_rec_mutex_lock(alsa_devin_mutex);

  is_starting = ((AGS_ALSA_DEVIN_START_RECORD & (alsa_devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(alsa_devin_mutex);
  
  return(is_starting);
}

gboolean
ags_alsa_devin_is_recording(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  gboolean is_recording;
  
  GRecMutex *alsa_devin_mutex;

  alsa_devin = AGS_ALSA_DEVIN(soundcard);
  
  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* check is starting */
  g_rec_mutex_lock(alsa_devin_mutex);

  is_recording = ((AGS_ALSA_DEVIN_RECORD & (alsa_devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(alsa_devin_mutex);

  return(is_recording);
}

gchar*
ags_alsa_devin_get_uptime(AgsSoundcard *soundcard)
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
ags_alsa_devin_device_record_init(AgsSoundcard *soundcard,
				  GError **error)
{
  AgsAlsaDevin *alsa_devin;

#if defined(AGS_WITH_ALSA)
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
  
  GRecMutex *alsa_devin_mutex; 
 
  static unsigned int period_time = 100000;
  static unsigned int buffer_time = 100000;

  if(ags_soundcard_is_recording(soundcard)){
    return;
  }

  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* retrieve word size */
  g_rec_mutex_lock(alsa_devin_mutex);

  if(alsa_devin->device == NULL){
    g_rec_mutex_unlock(alsa_devin_mutex);
    
    return;
  }

#ifdef AGS_WITH_ALSA
  format = 0;
#endif
  
  switch(alsa_devin->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
#ifdef AGS_WITH_ALSA
    format = SND_PCM_FORMAT_S8;
#endif

    word_size = sizeof(gint8);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
#ifdef AGS_WITH_ALSA
    format = SND_PCM_FORMAT_S16;
#endif
      
    word_size = sizeof(gint16);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
#ifdef AGS_WITH_ALSA
    format = SND_PCM_FORMAT_S24;
#endif
      
    //NOTE:JK: The 24-bit linear samples use 32-bit physical space
    word_size = sizeof(gint32);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
#ifdef AGS_WITH_ALSA
    format = SND_PCM_FORMAT_S32;
#endif
      
    word_size = sizeof(gint32);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    word_size = sizeof(gint64);
  }
  break;
  default:
    g_rec_mutex_unlock(alsa_devin_mutex);

    g_warning("ags_alsa_devin_alsa_init(): unsupported word size");

    return;
  }

  /* prepare for playback */
  alsa_devin->flags |= (AGS_ALSA_DEVIN_START_RECORD |
			AGS_ALSA_DEVIN_RECORD |
			AGS_ALSA_DEVIN_NONBLOCKING);

  for(i = 0; i < AGS_ALSA_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    memset(alsa_devin->app_buffer[i], 0, alsa_devin->pcm_channels * alsa_devin->buffer_size * word_size);
  }

  /* allocate ring buffer */
#ifdef AGS_WITH_ALSA
  g_atomic_int_set(&(alsa_devin->available),
		   FALSE);
  
  for(i = 0; i < AGS_ALSA_DEVIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    alsa_devin->backend_buffer[i] = (guchar *) g_malloc(alsa_devin->pcm_channels *
							alsa_devin->buffer_size * (snd_pcm_format_physical_width(format) / 8) *
							sizeof(guchar));
  }
 
  /*  */
  period_event = 0;
  
  /* Open PCM device for playback. */  
  handle = NULL;

  if((err = snd_pcm_open(&handle, alsa_devin->device, SND_PCM_STREAM_CAPTURE, 0)) < 0){
    gchar *device_fixup;
    
    str = snd_strerror(err);
    g_warning("Playback open error (attempting fixup): %s", str);
    
    device_fixup = g_strdup_printf("%s,0",
				   alsa_devin->device);

    handle = NULL;
    
    if((err = snd_pcm_open(&handle, device_fixup, SND_PCM_STREAM_CAPTURE, 0)) < 0){
      alsa_devin->flags |= (~(AGS_ALSA_DEVIN_START_RECORD |
			      AGS_ALSA_DEVIN_RECORD |
			      AGS_ALSA_DEVIN_NONBLOCKING));
      
      g_rec_mutex_unlock(alsa_devin_mutex);
      
      if(error != NULL){
	g_set_error(error,
		    AGS_ALSA_DEVIN_ERROR,
		    AGS_ALSA_DEVIN_ERROR_LOCKED_SOUNDCARD,
		    "unable to open pcm device: %s",
		    str);
      }
      
      return;
    }
  }

  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_sw_params_alloca(&swparams);

  /* choose all parameters */
  err = snd_pcm_hw_params_any(handle, hwparams);

  if(err < 0){
    alsa_devin->flags |= (~(AGS_ALSA_DEVIN_START_RECORD |
			    AGS_ALSA_DEVIN_RECORD |
			    AGS_ALSA_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(alsa_devin_mutex);

    str = snd_strerror(err);
    g_warning("Broken configuration for playback: no configurations available: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVIN_ERROR,
		  AGS_ALSA_DEVIN_ERROR_BROKEN_CONFIGURATION,
		  "unable to open pcm device: %s",
		  str);
    }

    alsa_devin->handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set hardware resampling * /
     err = snd_pcm_hw_params_set_rate_resample(handle, hwparams, 0);
     if (err < 0) {
     g_rec_mutex_unlock(alsa_devin_mutex);

     str = snd_strerror(err);
     g_warning("Resampling setup failed for playback: %s\n", str);

     //    free(str);
    
     return;
     }
  */
  
  /* set the interleaved read/write format */
  err = snd_pcm_hw_params_set_access(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);

  if(err < 0){
    alsa_devin->flags |= (~(AGS_ALSA_DEVIN_START_RECORD |
			    AGS_ALSA_DEVIN_RECORD |
			    AGS_ALSA_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(alsa_devin_mutex);

    str = snd_strerror(err);
    g_warning("Access type not available for playback: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVIN_ERROR,
		  AGS_ALSA_DEVIN_ERROR_ACCESS_TYPE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    alsa_devin->handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the sample format */
  err = snd_pcm_hw_params_set_format(handle, hwparams, format);

  if(err < 0){
    alsa_devin->flags |= (~(AGS_ALSA_DEVIN_START_RECORD |
			    AGS_ALSA_DEVIN_RECORD |
			    AGS_ALSA_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(alsa_devin_mutex);

    str = snd_strerror(err);
    g_warning("Sample format not available for playback: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVIN_ERROR,
		  AGS_ALSA_DEVIN_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    alsa_devin->handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the count of channels */
  channels = alsa_devin->pcm_channels;
  err = snd_pcm_hw_params_set_channels(handle, hwparams, channels);

  if(err < 0){
    alsa_devin->flags |= (~(AGS_ALSA_DEVIN_START_RECORD |
			    AGS_ALSA_DEVIN_RECORD |
			    AGS_ALSA_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(alsa_devin_mutex);

    str = snd_strerror(err);
    g_warning("Channels count (%i) not available for playbacks: %s", channels, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVIN_ERROR,
		  AGS_ALSA_DEVIN_ERROR_CHANNELS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    alsa_devin->handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the stream rate */
  rate = alsa_devin->samplerate;
  rrate = rate;
  err = snd_pcm_hw_params_set_rate_near(handle, hwparams, &rrate, 0);

  if(err < 0){
    alsa_devin->flags |= (~(AGS_ALSA_DEVIN_START_RECORD |
			    AGS_ALSA_DEVIN_RECORD |
			    AGS_ALSA_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(alsa_devin_mutex);

    str = snd_strerror(err);
    g_warning("Rate %iHz not available for playback: %s", rate, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVIN_ERROR,
		  AGS_ALSA_DEVIN_ERROR_SAMPLERATE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    alsa_devin->handle = NULL;
    
    //    free(str);
    
    return;
  }

  if(rrate != rate){
    alsa_devin->flags |= (~(AGS_ALSA_DEVIN_START_RECORD |
			    AGS_ALSA_DEVIN_RECORD |
			    AGS_ALSA_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(alsa_devin_mutex);

    g_warning("Rate doesn't match (requested %iHz, get %iHz)", rate, err);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVIN_ERROR,
		  AGS_ALSA_DEVIN_ERROR_SAMPLERATE_NOT_AVAILABLE,
		  "unable to open pcm device");
    }

    alsa_devin->handle = NULL;
    
    return;
  }

  /* set the buffer size */
  size = 2 * alsa_devin->buffer_size;
  err = snd_pcm_hw_params_set_buffer_size(handle, hwparams, size);

  if(err < 0){
    alsa_devin->flags |= (~(AGS_ALSA_DEVIN_START_RECORD |
			    AGS_ALSA_DEVIN_RECORD |
			    AGS_ALSA_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(alsa_devin_mutex);

    str = snd_strerror(err);

    g_warning("Unable to set buffer size %lu for playback: %s", size, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVIN_ERROR,
		  AGS_ALSA_DEVIN_ERROR_BUFFER_SIZE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    alsa_devin->handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the period size * /
     period_size = alsa_devin->buffer_size;
     err = snd_pcm_hw_params_set_period_size_near(handle, hwparams, period_size, dir);
     if(err < 0){
     g_rec_mutex_unlock(alsa_devin_mutex);

     str = snd_strerror(err);
     g_warning("Unable to get period size for playback: %s\n", str);

     //    free(str);
    
     return;
     }
  */
  
  /* write the parameters to device */
  err = snd_pcm_hw_params(handle, hwparams);

  if(err < 0){
    alsa_devin->flags |= (~(AGS_ALSA_DEVIN_START_RECORD |
			    AGS_ALSA_DEVIN_RECORD |
			    AGS_ALSA_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(alsa_devin_mutex);

    str = snd_strerror(err);
    g_warning("Unable to set hw params for playback: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVIN_ERROR,
		  AGS_ALSA_DEVIN_ERROR_HW_PARAMETERS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    alsa_devin->handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* get the current swparams * /
     err = snd_pcm_sw_params_current(handle, swparams);
     if(err < 0){
     g_rec_mutex_unlock(alsa_devin_mutex);

     str = snd_strerror(err);
     g_warning("Unable to determine current swparams for playback: %s\n", str);

     //    free(str);
    
     return;
     }
  */
  /* start the transfer when the buffer is almost full: */
  /* (buffer_size / avail_min) * avail_min * /
     err = snd_pcm_sw_params_set_start_threshold(handle, swparams, (buffer_size / period_size) * period_size);
     if(err < 0){
     g_rec_mutex_unlock(alsa_devin_mutex);

     str = snd_strerror(err);
     g_warning("Unable to set start threshold mode for playback: %s\n", str);
    
     //    free(str);
    
     return;
     }
  */
  /* allow the transfer when at least period_size samples can be processed */
  /* or disable this mechanism when period event is enabled (aka interrupt like style processing) * /
     err = snd_pcm_sw_params_set_avail_min(handle, swparams, period_event ? buffer_size : period_size);
     if(err < 0){
     g_rec_mutex_unlock(alsa_devin_mutex);

     str = snd_strerror(err);
     g_warning("Unable to set avail min for playback: %s\n", str);

     //    free(str);
    
     return;
     }

     /* write the parameters to the playback device * /
     err = snd_pcm_sw_params(handle, swparams);
     if(err < 0){
     g_rec_mutex_unlock(alsa_devin_mutex);

     str = snd_strerror(err);
     g_warning("Unable to set sw params for playback: %s\n", str);

     //    free(str);
    
     return;
     }
  */

  /*  */
  alsa_devin->handle = handle;
#endif

  alsa_devin->tact_counter = 0.0;
  alsa_devin->delay_counter = 0.0;
  alsa_devin->tic_counter = 0;

  alsa_devin->backend_buffer_mode = AGS_ALSA_DEVIN_BACKEND_BUFFER_0;
    
#ifdef AGS_WITH_ALSA
  alsa_devin->flags |= AGS_ALSA_DEVIN_INITIALIZED;
#endif

  alsa_devin->app_buffer_mode = AGS_ALSA_DEVIN_APP_BUFFER_0;
  
  g_rec_mutex_unlock(alsa_devin_mutex);
}

void
ags_alsa_devin_device_fill_backend_buffer(void *app_buffer,
					  guint ags_format,
					  guchar *backend_buffer,
					  guint channels,
					  guint buffer_size)
{
#ifdef AGS_WITH_ALSA
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
    g_warning("ags_alsa_devin_alsa_record(): unsupported word size");
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
      /* Generate data in native endian format */
      res = 0;
	
      if (big_endian) {
	for (i = 0; i < bps; i++)
	  res |= (int) (*(backend_buffer + chn * bps + phys_bps - 1 - i) << (i * 8));
      } else {
	for (i = 0; i < bps; i++)
	  res |= (int) (*(backend_buffer + chn * bps + i) << (i * 8));
      }	

      if(to_unsigned){
	res |= 1U << (format_bits - 1);
      }
	
      switch(ags_format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	((gint8 *) app_buffer)[count * channels + chn] = res;
      }
      break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	((gint16 *) app_buffer)[count * channels + chn] = res;
      }
      break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	((gint32 *) app_buffer)[count * channels + chn] = res;
      }
      break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	((gint32 *) app_buffer)[count * channels + chn] = res;
      }
      break;
      }
    }

    backend_buffer += channels * bps;
  }
#endif
}

void
ags_alsa_devin_device_record(AgsSoundcard *soundcard,
			     GError **error)
{
  AgsAlsaDevin *alsa_devin;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;
  GList *list;
  
  gchar *str;
  
  guint word_size;
  
  GRecMutex *alsa_devin_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  application_context = ags_application_context_get_instance();

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* lock */
  g_rec_mutex_lock(alsa_devin_mutex);
  
  /* retrieve word size */
  switch(alsa_devin->format){
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
    g_rec_mutex_unlock(alsa_devin_mutex);
    
    g_warning("ags_alsa_devin_alsa_record(): unsupported word size");

    return;
  }

  /* do capture */
  alsa_devin->flags &= (~AGS_ALSA_DEVIN_START_RECORD);

  if((AGS_ALSA_DEVIN_INITIALIZED & (alsa_devin->flags)) == 0){
    g_rec_mutex_unlock(alsa_devin_mutex);
    
    return;
  }

  //  g_message("record - 0x%0x", ((AGS_ALSA_DEVIN_BUFFER0 |
  //				AGS_ALSA_DEVIN_BUFFER1 |
  //				AGS_ALSA_DEVIN_BUFFER2 |
  //				AGS_ALSA_DEVIN_BUFFER3) & (alsa_devin->flags)));

#ifdef AGS_WITH_ALSA  
  /* write ring buffer */
//  g_message("read %d", alsa_devin->buffer_size);
  
  alsa_devin->rc = snd_pcm_readi(alsa_devin->handle,
				 alsa_devin->backend_buffer[alsa_devout->backend_buffer_mode],
				 (snd_pcm_uframes_t) (alsa_devin->buffer_size));


  /* fill buffer */
  ags_soundcard_lock_buffer(soundcard,
			    alsa_devin->app_buffer[alsa_devin->app_buffer_mode]);
  
  ags_alsa_devin_device_fill_backend_buffer(alsa_devin->app_buffer[alsa_devin->app_buffer_mode],
					    alsa_devin->format,
					    alsa_devin->backend_buffer[alsa_devout->backend_buffer_mode],
					    alsa_devin->pcm_channels,
					    alsa_devin->buffer_size);

  ags_soundcard_unlock_buffer(soundcard,
			      alsa_devin->app_buffer[alsa_devin->app_buffer_mode]);
  
  g_atomic_int_set(&(alsa_devin->available),
		   FALSE);
  
  /* check error flag */
  if((AGS_ALSA_DEVIN_NONBLOCKING & (alsa_devin->flags)) == 0){
    if(alsa_devin->rc == -EPIPE){
      /* EPIPE means underrun */
      snd_pcm_prepare(alsa_devin->handle);

#ifdef AGS_DEBUG
      g_message("underrun occurred");
#endif
    }else if(alsa_devin->rc == -ESTRPIPE){
      static const struct timespec idle = {
	0,
	4000,
      };

      int err;

      while((err = snd_pcm_resume(alsa_devin->handle)) < 0){ // == -EAGAIN
	nanosleep(&idle, NULL); /* wait until the suspend flag is released */
      }
	
      if(err < 0){
	err = snd_pcm_prepare(alsa_devin->handle);
      }
    }else if(alsa_devin->rc < 0){
      str = snd_strerror(alsa_devin->rc);
      
      g_message("error from writei: %s", str);
    }else if(alsa_devin->rc != (int) alsa_devin->buffer_size) {
      g_message("short write, write %d frames", alsa_devin->rc);
    }
  }      
  
#endif

  /* increment nth backend buffer */
  if(alsa_devin->backend_buffer_mode + 1 > AGS_ALSA_DEVIN_BACKEND_BUFFER_7){
    alsa_devin->backend_buffer_mode = AGS_ALSA_DEVIN_BACKEND_BUFFER_0;
  }else{
    alsa_devin->backend_buffer_mode += 1;
  }
  
  g_rec_mutex_unlock(alsa_devin_mutex);

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;
  
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) alsa_devin);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) alsa_devin);
  task = g_list_append(task,
		       clear_buffer);

  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) alsa_devin);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);
  
#ifdef AGS_WITH_ALSA
  snd_pcm_prepare(alsa_devin->handle);
#endif

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_alsa_devin_device_free(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  AgsApplicationContext *application_context;

  guint i;
  
  GRecMutex *alsa_devin_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* lock */
  g_rec_mutex_lock(alsa_devin_mutex);

  if((AGS_ALSA_DEVIN_INITIALIZED & (alsa_devin->flags)) == 0 ||
     alsa_devin->handle == NULL){
    g_rec_mutex_unlock(alsa_devin_mutex);
    
    return;
  }
  
#ifdef AGS_WITH_ALSA
  //  snd_pcm_drain(alsa_devin->handle);
  snd_pcm_close(alsa_devin->handle);
  alsa_devin->handle = NULL;
#endif

  /* free backend buffer */
  if(alsa_devin->backend_buffer != NULL){
    for(i = 0; i < AGS_ALSA_DEVIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
      g_free(alsa_devin->backend_buffer[i]);

      alsa_devin->backend_buffer[i] = NULL;
    }
  }

  /* reset flags */
  alsa_devin->flags &= (~(AGS_ALSA_DEVIN_RECORD |
			  AGS_ALSA_DEVIN_INITIALIZED));

  alsa_devin->app_buffer_mode = AGS_ALSA_DEVIN_APP_BUFFER_0;

  alsa_devin->backend_buffer_mode = AGS_ALSA_DEVIN_BACKEND_BUFFER_0;

  alsa_devin->note_offset = alsa_devin->start_note_offset;
  alsa_devin->note_offset_absolute = alsa_devin->start_note_offset;

  g_rec_mutex_unlock(alsa_devin_mutex);
}

void
ags_alsa_devin_tic(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  gdouble delay;
  gdouble delay_counter;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  
  GRecMutex *alsa_devin_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(alsa_devin_mutex);

  delay = alsa_devin->delay[alsa_devin->tic_counter];
  delay_counter = alsa_devin->delay_counter;

  note_offset = alsa_devin->note_offset;
  note_offset_absolute = alsa_devin->note_offset_absolute;
  
  loop_left = alsa_devin->loop_left;
  loop_right = alsa_devin->loop_right;
  
  do_loop = alsa_devin->do_loop;

  g_rec_mutex_unlock(alsa_devin_mutex);

  if(delay_counter + 1.0 >= floor(delay)){
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
    g_rec_mutex_lock(alsa_devin_mutex);
    
    alsa_devin->delay_counter = delay_counter + 1.0 - delay;
    alsa_devin->tact_counter += 1.0;

    g_rec_mutex_unlock(alsa_devin_mutex);
  }else{
    g_rec_mutex_lock(alsa_devin_mutex);
    
    alsa_devin->delay_counter += 1.0;

    g_rec_mutex_unlock(alsa_devin_mutex);
  }
}

void
ags_alsa_devin_offset_changed(AgsSoundcard *soundcard,
			      guint note_offset)
{
  AgsAlsaDevin *alsa_devin;
  
  GRecMutex *alsa_devin_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* offset changed */
  g_rec_mutex_lock(alsa_devin_mutex);

  alsa_devin->tic_counter += 1;

  if(alsa_devin->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    alsa_devin->tic_counter = 0;
  }

  g_rec_mutex_unlock(alsa_devin_mutex);
}

void
ags_alsa_devin_set_bpm(AgsSoundcard *soundcard,
		       gdouble bpm)
{
  AgsAlsaDevin *alsa_devin;

  GRecMutex *alsa_devin_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* set bpm */
  g_rec_mutex_lock(alsa_devin_mutex);

  alsa_devin->bpm = bpm;

  g_rec_mutex_unlock(alsa_devin_mutex);

  ags_alsa_devin_adjust_delay_and_attack(alsa_devin);
}

gdouble
ags_alsa_devin_get_bpm(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  gdouble bpm;
  
  GRecMutex *alsa_devin_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* get bpm */
  g_rec_mutex_lock(alsa_devin_mutex);

  bpm = alsa_devin->bpm;
  
  g_rec_mutex_unlock(alsa_devin_mutex);

  return(bpm);
}

void
ags_alsa_devin_set_delay_factor(AgsSoundcard *soundcard,
				gdouble delay_factor)
{
  AgsAlsaDevin *alsa_devin;

  GRecMutex *alsa_devin_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* set delay factor */
  g_rec_mutex_lock(alsa_devin_mutex);

  alsa_devin->delay_factor = delay_factor;

  g_rec_mutex_unlock(alsa_devin_mutex);

  ags_alsa_devin_adjust_delay_and_attack(alsa_devin);
}

gdouble
ags_alsa_devin_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  gdouble delay_factor;
  
  GRecMutex *alsa_devin_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* get delay factor */
  g_rec_mutex_lock(alsa_devin_mutex);

  delay_factor = alsa_devin->delay_factor;
  
  g_rec_mutex_unlock(alsa_devin_mutex);

  return(delay_factor);
}

gdouble
ags_alsa_devin_get_delay(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  guint index;
  gdouble delay;
  
  GRecMutex *alsa_devin_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* get delay */
  g_rec_mutex_lock(alsa_devin_mutex);

  index = alsa_devin->tic_counter;

  delay = alsa_devin->delay[index];
  
  g_rec_mutex_unlock(alsa_devin_mutex);
  
  return(delay);
}

gdouble
ags_alsa_devin_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  gdouble absolute_delay;
  
  GRecMutex *alsa_devin_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);
  
  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* get absolute delay */
  g_rec_mutex_lock(alsa_devin_mutex);

  absolute_delay = (60.0 * (((gdouble) alsa_devin->samplerate / (gdouble) alsa_devin->buffer_size) / (gdouble) alsa_devin->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) alsa_devin->delay_factor)));

  g_rec_mutex_unlock(alsa_devin_mutex);

  return(absolute_delay);
}

guint
ags_alsa_devin_get_attack(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  guint index;
  guint attack;
  
  GRecMutex *alsa_devin_mutex;  

  alsa_devin = AGS_ALSA_DEVIN(soundcard);
  
  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* get attack */
  g_rec_mutex_lock(alsa_devin_mutex);

  index = alsa_devin->tic_counter;

  attack = alsa_devin->attack[index];

  g_rec_mutex_unlock(alsa_devin_mutex);
  
  return(attack);
}

void*
ags_alsa_devin_get_buffer(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  void *buffer;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  buffer = alsa_devin->app_buffer[alsa_devin->app_buffer_mode];

  return(buffer);
}

void*
ags_alsa_devin_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  void *buffer;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  if(alsa_devin->app_buffer_mode == AGS_ALSA_DEVIN_APP_BUFFER_3){
    buffer = alsa_devin->app_buffer[AGS_ALSA_DEVIN_APP_BUFFER_0];
  }else{
    buffer = alsa_devin->app_buffer[alsa_devin->app_buffer_mode + 1];
  }

  return(buffer);
}

void*
ags_alsa_devin_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  void *buffer;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  if(alsa_devin->app_buffer_mode == AGS_ALSA_DEVIN_APP_BUFFER_0){
    buffer = alsa_devin->app_buffer[AGS_ALSA_DEVIN_APP_BUFFER_3];
  }else{
    buffer = alsa_devin->app_buffer[alsa_devin->app_buffer_mode - 1];
  }

  return(buffer);
}

void
ags_alsa_devin_lock_buffer(AgsSoundcard *soundcard,
			   void *buffer)
{
  AgsAlsaDevin *alsa_devin;

  GRecMutex *buffer_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(alsa_devin->app_buffer != NULL){
    if(buffer == alsa_devin->app_buffer[0]){
      buffer_mutex = alsa_devin->app_buffer_mutex[0];
    }else if(buffer == alsa_devin->app_buffer[1]){
      buffer_mutex = alsa_devin->app_buffer_mutex[1];
    }else if(buffer == alsa_devin->app_buffer[2]){
      buffer_mutex = alsa_devin->app_buffer_mutex[2];
    }else if(buffer == alsa_devin->app_buffer[3]){
      buffer_mutex = alsa_devin->app_buffer_mutex[3];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_alsa_devin_unlock_buffer(AgsSoundcard *soundcard,
			     void *buffer)
{
  AgsAlsaDevin *alsa_devin;

  GRecMutex *buffer_mutex;
  
  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(alsa_devin->app_buffer != NULL){
    if(buffer == alsa_devin->app_buffer[0]){
      buffer_mutex = alsa_devin->app_buffer_mutex[0];
    }else if(buffer == alsa_devin->app_buffer[1]){
      buffer_mutex = alsa_devin->app_buffer_mutex[1];
    }else if(buffer == alsa_devin->app_buffer[2]){
      buffer_mutex = alsa_devin->app_buffer_mutex[2];
    }else if(buffer == alsa_devin->app_buffer[3]){
      buffer_mutex = alsa_devin->app_buffer_mutex[3];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_alsa_devin_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  guint delay_counter;
  
  GRecMutex *alsa_devin_mutex;  

  alsa_devin = AGS_ALSA_DEVIN(soundcard);
  
  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* delay counter */
  g_rec_mutex_lock(alsa_devin_mutex);

  delay_counter = alsa_devin->delay_counter;
  
  g_rec_mutex_unlock(alsa_devin_mutex);

  return(delay_counter);
}

void
ags_alsa_devin_set_start_note_offset(AgsSoundcard *soundcard,
				     guint start_note_offset)
{
  AgsAlsaDevin *alsa_devin;

  GRecMutex *alsa_devin_mutex;  

  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* set note offset */
  g_rec_mutex_lock(alsa_devin_mutex);

  alsa_devin->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(alsa_devin_mutex);
}

guint
ags_alsa_devin_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  guint start_note_offset;
  
  GRecMutex *alsa_devin_mutex;  

  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* set note offset */
  g_rec_mutex_lock(alsa_devin_mutex);

  start_note_offset = alsa_devin->start_note_offset;

  g_rec_mutex_unlock(alsa_devin_mutex);

  return(start_note_offset);
}

void
ags_alsa_devin_set_note_offset(AgsSoundcard *soundcard,
			       guint note_offset)
{
  AgsAlsaDevin *alsa_devin;

  GRecMutex *alsa_devin_mutex;  

  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* set note offset */
  g_rec_mutex_lock(alsa_devin_mutex);

  alsa_devin->note_offset = note_offset;

  g_rec_mutex_unlock(alsa_devin_mutex);
}

guint
ags_alsa_devin_get_note_offset(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  guint note_offset;
  
  GRecMutex *alsa_devin_mutex;  

  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* set note offset */
  g_rec_mutex_lock(alsa_devin_mutex);

  note_offset = alsa_devin->note_offset;

  g_rec_mutex_unlock(alsa_devin_mutex);

  return(note_offset);
}

void
ags_alsa_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
					guint note_offset_absolute)
{
  AgsAlsaDevin *alsa_devin;
  
  GRecMutex *alsa_devin_mutex;  

  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* set note offset */
  g_rec_mutex_lock(alsa_devin_mutex);

  alsa_devin->note_offset_absolute = note_offset_absolute;

  g_rec_mutex_unlock(alsa_devin_mutex);
}

guint
ags_alsa_devin_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  guint note_offset_absolute;
  
  GRecMutex *alsa_devin_mutex;  

  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* set note offset */
  g_rec_mutex_lock(alsa_devin_mutex);

  note_offset_absolute = alsa_devin->note_offset_absolute;

  g_rec_mutex_unlock(alsa_devin_mutex);

  return(note_offset_absolute);
}

void
ags_alsa_devin_set_loop(AgsSoundcard *soundcard,
			guint loop_left, guint loop_right,
			gboolean do_loop)
{
  AgsAlsaDevin *alsa_devin;

  GRecMutex *alsa_devin_mutex;  

  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* set loop */
  g_rec_mutex_lock(alsa_devin_mutex);

  alsa_devin->loop_left = loop_left;
  alsa_devin->loop_right = loop_right;
  alsa_devin->do_loop = do_loop;

  if(do_loop){
    alsa_devin->loop_offset = alsa_devin->note_offset;
  }

  g_rec_mutex_unlock(alsa_devin_mutex);
}

void
ags_alsa_devin_get_loop(AgsSoundcard *soundcard,
			guint *loop_left, guint *loop_right,
			gboolean *do_loop)
{
  AgsAlsaDevin *alsa_devin;

  GRecMutex *alsa_devin_mutex;  

  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* get loop */
  g_rec_mutex_lock(alsa_devin_mutex);

  if(loop_left != NULL){
    *loop_left = alsa_devin->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = alsa_devin->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = alsa_devin->do_loop;
  }

  g_rec_mutex_unlock(alsa_devin_mutex);
}

guint
ags_alsa_devin_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsAlsaDevin *alsa_devin;

  guint loop_offset;
  
  GRecMutex *alsa_devin_mutex;  

  alsa_devin = AGS_ALSA_DEVIN(soundcard);

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* get loop offset */
  g_rec_mutex_lock(alsa_devin_mutex);

  loop_offset = alsa_devin->loop_offset;
  
  g_rec_mutex_unlock(alsa_devin_mutex);

  return(loop_offset);
}

/**
 * ags_alsa_devin_switch_buffer_flag:
 * @alsa_devin: the #AgsAlsaDevin
 *
 * The buffer flag indicates the currently recorded buffer.
 *
 * Since: 3.13.2
 */
void
ags_alsa_devin_switch_buffer_flag(AgsAlsaDevin *alsa_devin)
{
  GRecMutex *alsa_devin_mutex;
  
  if(!AGS_IS_ALSA_DEVIN(alsa_devin)){
    return;
  }

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* switch buffer flag */
  g_rec_mutex_lock(alsa_devin_mutex);

  if(alsa_devin->app_buffer_mode < AGS_ALSA_DEVIN_APP_BUFFER_3){
    alsa_devin->app_buffer_mode += 1;
  }else{
    alsa_devin->app_buffer_mode = AGS_ALSA_DEVIN_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(alsa_devin_mutex);
}

/**
 * ags_alsa_devin_adjust_delay_and_attack:
 * @alsa_devin: the #AgsAlsaDevin
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 3.13.2
 */
void
ags_alsa_devin_adjust_delay_and_attack(AgsAlsaDevin *alsa_devin)
{
  if(!AGS_IS_ALSA_DEVIN(alsa_devin)){
    return;
  }
  
  ags_soundcard_util_adjust_delay_and_attack(alsa_devin);
}

/**
 * ags_alsa_devin_realloc_buffer:
 * @alsa_devin: the #AgsAlsaDevin
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.13.2
 */
void
ags_alsa_devin_realloc_buffer(AgsAlsaDevin *alsa_devin)
{
  guint pcm_channels;
  guint buffer_size;
  guint word_size;
  guint format;
  
  GRecMutex *alsa_devin_mutex;  

  if(!AGS_IS_ALSA_DEVIN(alsa_devin)){
    return;
  }

  /* get alsa devin mutex */
  alsa_devin_mutex = AGS_ALSA_DEVIN_GET_OBJ_MUTEX(alsa_devin);

  /* get word size */  
  g_rec_mutex_lock(alsa_devin_mutex);

  pcm_channels = alsa_devin->pcm_channels;
  buffer_size = alsa_devin->buffer_size;

  format = alsa_devin->format;
  
  g_rec_mutex_unlock(alsa_devin_mutex);
  
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
    g_warning("ags_alsa_devin_realloc_buffer(): unsupported word size");
    
    return;
  }  

  //NOTE:JK: there is no lock applicable to buffer

  if(alsa_devin->app_buffer[0] != NULL){
    g_free(alsa_devin->app_buffer[0]);
  }
  
  alsa_devin->app_buffer[0] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  if(alsa_devin->app_buffer[1] != NULL){
    g_free(alsa_devin->app_buffer[1]);
  }

  alsa_devin->app_buffer[1] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  if(alsa_devin->app_buffer[2] != NULL){
    g_free(alsa_devin->app_buffer[2]);
  }

  alsa_devin->app_buffer[2] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  if(alsa_devin->app_buffer[3] != NULL){
    g_free(alsa_devin->app_buffer[3]);
  }
  
  alsa_devin->app_buffer[3] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_alsa_devin_new:
 *
 * Creates a new instance of #AgsAlsaDevin.
 *
 * Returns: the new #AgsAlsaDevin
 *
 * Since: 3.13.2
 */
AgsAlsaDevin*
ags_alsa_devin_new()
{
  AgsAlsaDevin *alsa_devin;

  alsa_devin = (AgsAlsaDevin *) g_object_new(AGS_TYPE_ALSA_DEVIN,
					     NULL);
  
  return(alsa_devin);
}
