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

#include <ags/audio/oss/ags_oss_devin.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_soundcard_util.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifndef AGS_W32API
#include <fcntl.h>
#include <sys/ioctl.h>

#ifndef __APPLE__
#include <sys/soundcard.h>
#endif
#endif

#include <errno.h>

#define _GNU_SOURCE
#include <signal.h>

#include <string.h>
#include <math.h>

#include <time.h>
#include <signal.h>
#include <strings.h>
#include <unistd.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_oss_devin_class_init(AgsOssDevinClass *oss_devin);
void ags_oss_devin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_oss_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_oss_devin_init(AgsOssDevin *oss_devin);
void ags_oss_devin_set_property(GObject *gobject,
				guint prop_id,
				const GValue *value,
				GParamSpec *param_spec);
void ags_oss_devin_get_property(GObject *gobject,
				guint prop_id,
				GValue *value,
				GParamSpec *param_spec);
void ags_oss_devin_dispose(GObject *gobject);
void ags_oss_devin_finalize(GObject *gobject);

AgsUUID* ags_oss_devin_get_uuid(AgsConnectable *connectable);
gboolean ags_oss_devin_has_resource(AgsConnectable *connectable);
gboolean ags_oss_devin_is_ready(AgsConnectable *connectable);
void ags_oss_devin_add_to_registry(AgsConnectable *connectable);
void ags_oss_devin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_oss_devin_list_resource(AgsConnectable *connectable);
xmlNode* ags_oss_devin_xml_compose(AgsConnectable *connectable);
void ags_oss_devin_xml_parse(AgsConnectable *connectable,
			     xmlNode *node);
gboolean ags_oss_devin_is_connected(AgsConnectable *connectable);
void ags_oss_devin_connect(AgsConnectable *connectable);
void ags_oss_devin_disconnect(AgsConnectable *connectable);

void ags_oss_devin_set_device(AgsSoundcard *soundcard,
			      gchar *device);
gchar* ags_oss_devin_get_device(AgsSoundcard *soundcard);

void ags_oss_devin_set_presets(AgsSoundcard *soundcard,
			       guint channels,
			       guint rate,
			       guint buffer_size,
			       guint format);
void ags_oss_devin_get_presets(AgsSoundcard *soundcard,
			       guint *channels,
			       guint *rate,
			       guint *buffer_size,
			       guint *format);

void ags_oss_devin_list_cards(AgsSoundcard *soundcard,
			      GList **card_id, GList **card_name);
void ags_oss_devin_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			    guint *channels_min, guint *channels_max,
			    guint *rate_min, guint *rate_max,
			    guint *buffer_size_min, guint *buffer_size_max,
			    GError **error);
guint ags_oss_devin_get_capability(AgsSoundcard *soundcard);

gboolean ags_oss_devin_is_available(AgsSoundcard *soundcard);

gboolean ags_oss_devin_is_starting(AgsSoundcard *soundcard);
gboolean ags_oss_devin_is_recording(AgsSoundcard *soundcard);

gchar* ags_oss_devin_get_uptime(AgsSoundcard *soundcard);

void ags_oss_devin_device_record_init(AgsSoundcard *soundcard,
				      GError **error);

void ags_oss_devin_oss_record_fill_buffer(void *app_buffer,
					  guint ags_format,
					  guchar *backend_buffer,
					  guint channels,
					  guint buffer_size);

void ags_oss_devin_device_record(AgsSoundcard *soundcard,
				 GError **error);
void ags_oss_devin_device_free(AgsSoundcard *soundcard);

void ags_oss_devin_tic(AgsSoundcard *soundcard);
void ags_oss_devin_offset_changed(AgsSoundcard *soundcard,
				  guint note_offset);

void ags_oss_devin_set_bpm(AgsSoundcard *soundcard,
			   gdouble bpm);
gdouble ags_oss_devin_get_bpm(AgsSoundcard *soundcard);

void ags_oss_devin_set_delay_factor(AgsSoundcard *soundcard,
				    gdouble delay_factor);
gdouble ags_oss_devin_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_oss_devin_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_oss_devin_get_delay(AgsSoundcard *soundcard);
guint ags_oss_devin_get_attack(AgsSoundcard *soundcard);

void* ags_oss_devin_get_buffer(AgsSoundcard *soundcard);
void* ags_oss_devin_get_next_buffer(AgsSoundcard *soundcard);
void* ags_oss_devin_get_prev_buffer(AgsSoundcard *soundcard);

void ags_oss_devin_lock_buffer(AgsSoundcard *soundcard,
			       void *buffer);
void ags_oss_devin_unlock_buffer(AgsSoundcard *soundcard,
				 void *buffer);

guint ags_oss_devin_get_delay_counter(AgsSoundcard *soundcard);

void ags_oss_devin_set_start_note_offset(AgsSoundcard *soundcard,
					 guint start_note_offset);
guint ags_oss_devin_get_start_note_offset(AgsSoundcard *soundcard);

void ags_oss_devin_set_note_offset(AgsSoundcard *soundcard,
				   guint note_offset);
guint ags_oss_devin_get_note_offset(AgsSoundcard *soundcard);

void ags_oss_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
					    guint note_offset);
guint ags_oss_devin_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_oss_devin_set_loop(AgsSoundcard *soundcard,
			    guint loop_left, guint loop_right,
			    gboolean do_loop);
void ags_oss_devin_get_loop(AgsSoundcard *soundcard,
			    guint *loop_left, guint *loop_right,
			    gboolean *do_loop);

guint ags_oss_devin_get_loop_offset(AgsSoundcard *soundcard);

/**
 * SECTION:ags_oss_devin
 * @short_description: Input from soundcard
 * @title: AgsOssDevin
 * @section_id:
 * @include: ags/audio/ags_oss_devin.h
 *
 * #AgsOssDevin represents a soundcard and supports input.
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

static gpointer ags_oss_devin_parent_class = NULL;

GType
ags_oss_devin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_oss_devin = 0;

    static const GTypeInfo ags_oss_devin_info = {
      sizeof(AgsOssDevinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_oss_devin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsOssDevin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_oss_devin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_oss_devin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_oss_devin_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_oss_devin = g_type_register_static(G_TYPE_OBJECT,
						"AgsOssDevin",
						&ags_oss_devin_info,
						0);

    g_type_add_interface_static(ags_type_oss_devin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_oss_devin,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_oss_devin);
  }

  return g_define_type_id__volatile;
}

GType
ags_oss_devin_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_OSS_DEVIN_ADDED_TO_REGISTRY, "AGS_OSS_DEVIN_ADDED_TO_REGISTRY", "oss-devin-added-to-registry" },
      { AGS_OSS_DEVIN_CONNECTED, "AGS_OSS_DEVIN_CONNECTED", "oss-devin-connected" },
      { AGS_OSS_DEVIN_INITIALIZED, "AGS_OSS_DEVIN_INITIALIZED", "oss-devin-initialized" },
      { AGS_OSS_DEVIN_START_RECORD, "AGS_OSS_DEVIN_START_RECORD", "oss-devin-start-record" },
      { AGS_OSS_DEVIN_RECORD, "AGS_OSS_DEVIN_RECORD", "oss-devin-record" },
      { AGS_OSS_DEVIN_SHUTDOWN, "AGS_OSS_DEVIN_SHUTDOWN", "oss-devin-shutdown" },
      { AGS_OSS_DEVIN_NONBLOCKING, "AGS_OSS_DEVIN_NONBLOCKING", "oss-devin-nonblocking" },
      { AGS_OSS_DEVIN_ATTACK_FIRST, "AGS_OSS_DEVIN_ATTACK_FIRST", "oss-devin-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsOssDevinFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

void
ags_oss_devin_class_init(AgsOssDevinClass *oss_devin)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_oss_devin_parent_class = g_type_class_peek_parent(oss_devin);

  /* GObjectClass */
  gobject = (GObjectClass *) oss_devin;

  gobject->set_property = ags_oss_devin_set_property;
  gobject->get_property = ags_oss_devin_get_property;

  gobject->dispose = ags_oss_devin_dispose;
  gobject->finalize = ags_oss_devin_finalize;

  /* properties */
  /**
   * AgsOssDevin:device:
   *
   * The soundcard indentifier
   * 
   * Since: 3.13.2
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   AGS_OSS_DEVIN_DEFAULT_OSS_DEVICE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsOssDevin:dsp-channels:
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
   * AgsOssDevin:pcm-channels:
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
   * AgsOssDevin:format:
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
   * AgsOssDevin:buffer-size:
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
   * AgsOssDevin:samplerate:
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
   * AgsOssDevin:buffer:
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
   * AgsOssDevin:bpm:
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
   * AgsOssDevin:delay-factor:
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
   * AgsOssDevin:attack:
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
  
  /* AgsOssDevinClass */
}

GQuark
ags_oss_devin_error_quark()
{
  return(g_quark_from_static_string("ags-oss-devin-error-quark"));
}

void
ags_oss_devin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_oss_devin_get_uuid;
  connectable->has_resource = ags_oss_devin_has_resource;

  connectable->is_ready = ags_oss_devin_is_ready;
  connectable->add_to_registry = ags_oss_devin_add_to_registry;
  connectable->remove_from_registry = ags_oss_devin_remove_from_registry;

  connectable->list_resource = ags_oss_devin_list_resource;
  connectable->xml_compose = ags_oss_devin_xml_compose;
  connectable->xml_parse = ags_oss_devin_xml_parse;

  connectable->is_connected = ags_oss_devin_is_connected;  
  connectable->connect = ags_oss_devin_connect;
  connectable->disconnect = ags_oss_devin_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_oss_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_oss_devin_set_device;
  soundcard->get_device = ags_oss_devin_get_device;
  
  soundcard->set_presets = ags_oss_devin_set_presets;
  soundcard->get_presets = ags_oss_devin_get_presets;

  soundcard->list_cards = ags_oss_devin_list_cards;
  soundcard->pcm_info = ags_oss_devin_pcm_info;
  soundcard->get_capability = ags_oss_devin_get_capability;

  soundcard->is_available = ags_oss_devin_is_available;

  soundcard->is_starting =  ags_oss_devin_is_starting;
  soundcard->is_playing = NULL;
  soundcard->is_recording = ags_oss_devin_is_recording;

  soundcard->get_uptime = ags_oss_devin_get_uptime;
  
  soundcard->play_init = NULL;
  soundcard->play = NULL;

  soundcard->record_init = ags_oss_devin_device_record_init;
  soundcard->record = ags_oss_devin_device_record;
  
  soundcard->stop = ags_oss_devin_device_free;

  soundcard->tic = ags_oss_devin_tic;
  soundcard->offset_changed = ags_oss_devin_offset_changed;
    
  soundcard->set_bpm = ags_oss_devin_set_bpm;
  soundcard->get_bpm = ags_oss_devin_get_bpm;

  soundcard->set_delay_factor = ags_oss_devin_set_delay_factor;
  soundcard->get_delay_factor = ags_oss_devin_get_delay_factor;

  soundcard->get_absolute_delay = ags_oss_devin_get_absolute_delay;

  soundcard->get_delay = ags_oss_devin_get_delay;
  soundcard->get_attack = ags_oss_devin_get_attack;

  soundcard->get_buffer = ags_oss_devin_get_buffer;
  soundcard->get_next_buffer = ags_oss_devin_get_next_buffer;
  soundcard->get_prev_buffer = ags_oss_devin_get_prev_buffer;

  soundcard->lock_buffer = ags_oss_devin_lock_buffer;
  soundcard->unlock_buffer = ags_oss_devin_unlock_buffer;

  soundcard->get_delay_counter = ags_oss_devin_get_delay_counter;

  soundcard->set_start_note_offset = ags_oss_devin_set_start_note_offset;
  soundcard->get_start_note_offset = ags_oss_devin_get_start_note_offset;

  soundcard->set_note_offset = ags_oss_devin_set_note_offset;
  soundcard->get_note_offset = ags_oss_devin_get_note_offset;

  soundcard->set_note_offset_absolute = ags_oss_devin_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_oss_devin_get_note_offset_absolute;

  soundcard->set_loop = ags_oss_devin_set_loop;
  soundcard->get_loop = ags_oss_devin_get_loop;

  soundcard->get_loop_offset = ags_oss_devin_get_loop_offset;
}

void
ags_oss_devin_init(AgsOssDevin *oss_devin)
{  
  AgsConfig *config;
  
  gchar *str;
  gchar *segmentation;

  guint denominator, numerator;
  guint i;
  
  oss_devin->flags = 0;

  /* insert oss_devin mutex */
  g_rec_mutex_init(&(oss_devin->obj_mutex));

  /* uuid */
  oss_devin->uuid = ags_uuid_alloc();
  ags_uuid_generate(oss_devin->uuid);

  /* config */
  config = ags_config_get_instance();

  /* presets */
  oss_devin->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  oss_devin->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  oss_devin->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  oss_devin->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  oss_devin->format = ags_soundcard_helper_config_get_format(config);

  /* device */
  oss_devin->device_fd = -1;
  oss_devin->device = g_strdup(AGS_OSS_DEVIN_DEFAULT_OSS_DEVICE);

  /* app buffer */
  oss_devin->app_buffer_mode = AGS_OSS_DEVIN_APP_BUFFER_0;

  oss_devin->app_buffer_mutex = (GRecMutex **) g_malloc(AGS_OSS_DEVIN_DEFAULT_APP_BUFFER_SIZE * sizeof(GRecMutex *));

  for(i = 0; i < AGS_OSS_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    oss_devin->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(oss_devin->app_buffer_mutex[i]);
  }

  oss_devin->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  oss_devin->sub_block_mutex = (GRecMutex **) g_malloc(AGS_OSS_DEVIN_DEFAULT_APP_BUFFER_SIZE * oss_devin->sub_block_count * oss_devin->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < AGS_OSS_DEVIN_DEFAULT_APP_BUFFER_SIZE * oss_devin->sub_block_count * oss_devin->pcm_channels; i++){
    oss_devin->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(oss_devin->sub_block_mutex[i]);
  }

  oss_devin->app_buffer = (void **) g_malloc(AGS_OSS_DEVIN_DEFAULT_APP_BUFFER_SIZE * sizeof(void *));
  
  for(i = 0; i < AGS_OSS_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    oss_devin->app_buffer[i] = NULL;
  }

  g_atomic_int_set(&(oss_devin->available),
		   FALSE);
  
  oss_devin->backend_buffer_mode = AGS_OSS_DEVIN_BACKEND_BUFFER_0;
  
  oss_devin->backend_buffer = (guchar **) g_malloc(AGS_OSS_DEVIN_DEFAULT_BACKEND_BUFFER_SIZE * sizeof(guchar *));

  for(i = 0; i < AGS_OSS_DEVIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    oss_devin->backend_buffer[i] = NULL;
  }
  
  ags_oss_devin_realloc_buffer(oss_devin);
  
  /* bpm */
  oss_devin->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  oss_devin->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    oss_devin->delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }

  /* delay and attack */
  oss_devin->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					sizeof(gdouble));
  
  oss_devin->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
				       sizeof(guint));

  ags_oss_devin_adjust_delay_and_attack(oss_devin);
  
  /* counters */
  oss_devin->tact_counter = 0.0;
  oss_devin->delay_counter = 0;
  oss_devin->tic_counter = 0;

  oss_devin->start_note_offset = 0;
  oss_devin->note_offset = 0;
  oss_devin->note_offset_absolute = 0;

  oss_devin->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  oss_devin->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  oss_devin->do_loop = FALSE;

  oss_devin->loop_offset = 0;  
}

void
ags_oss_devin_set_property(GObject *gobject,
			   guint prop_id,
			   const GValue *value,
			   GParamSpec *param_spec)
{
  AgsOssDevin *oss_devin;

  GRecMutex *oss_devin_mutex;

  oss_devin = AGS_OSS_DEVIN(gobject);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    char *device;

    device = (char *) g_value_get_string(value);

    g_rec_mutex_lock(oss_devin_mutex);

    g_free(oss_devin->device);
    
    oss_devin->device = g_strdup(device);

    g_rec_mutex_unlock(oss_devin_mutex);
  }
  break;
  case PROP_DSP_CHANNELS:
  {
    guint dsp_channels;

    dsp_channels = g_value_get_uint(value);

    g_rec_mutex_lock(oss_devin_mutex);

    if(dsp_channels == oss_devin->dsp_channels){
      g_rec_mutex_unlock(oss_devin_mutex);
	
      return;
    }

    oss_devin->dsp_channels = dsp_channels;

    g_rec_mutex_unlock(oss_devin_mutex);
  }
  break;
  case PROP_PCM_CHANNELS:
  {
    guint pcm_channels;

    pcm_channels = g_value_get_uint(value);

    g_rec_mutex_lock(oss_devin_mutex);

    if(pcm_channels == oss_devin->pcm_channels){
      g_rec_mutex_unlock(oss_devin_mutex);

      return;
    }

    oss_devin->pcm_channels = pcm_channels;

    g_rec_mutex_unlock(oss_devin_mutex);

    ags_oss_devin_realloc_buffer(oss_devin);
  }
  break;
  case PROP_FORMAT:
  {
    guint format;

    format = g_value_get_uint(value);

    g_rec_mutex_lock(oss_devin_mutex);

    if(format == oss_devin->format){
      g_rec_mutex_unlock(oss_devin_mutex);

      return;
    }

    oss_devin->format = format;

    g_rec_mutex_unlock(oss_devin_mutex);

    ags_oss_devin_realloc_buffer(oss_devin);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    guint buffer_size;

    buffer_size = g_value_get_uint(value);

    g_rec_mutex_lock(oss_devin_mutex);

    if(buffer_size == oss_devin->buffer_size){
      g_rec_mutex_unlock(oss_devin_mutex);

      return;
    }

    oss_devin->buffer_size = buffer_size;

    g_rec_mutex_unlock(oss_devin_mutex);

    ags_oss_devin_realloc_buffer(oss_devin);
    ags_oss_devin_adjust_delay_and_attack(oss_devin);
  }
  break;
  case PROP_SAMPLERATE:
  {
    guint samplerate;

    samplerate = g_value_get_uint(value);

    g_rec_mutex_lock(oss_devin_mutex);

    if(samplerate == oss_devin->samplerate){
      g_rec_mutex_unlock(oss_devin_mutex);
	
      return;
    }

    oss_devin->samplerate = samplerate;

    g_rec_mutex_unlock(oss_devin_mutex);

    ags_oss_devin_adjust_delay_and_attack(oss_devin);
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

    g_rec_mutex_lock(oss_devin_mutex);

    if(bpm == oss_devin->bpm){
      g_rec_mutex_unlock(oss_devin_mutex);

      return;
    }

    oss_devin->bpm = bpm;

    g_rec_mutex_unlock(oss_devin_mutex);

    ags_oss_devin_adjust_delay_and_attack(oss_devin);
  }
  break;
  case PROP_DELAY_FACTOR:
  {
    gdouble delay_factor;
      
    delay_factor = g_value_get_double(value);

    g_rec_mutex_lock(oss_devin_mutex);

    if(delay_factor == oss_devin->delay_factor){
      g_rec_mutex_unlock(oss_devin_mutex);

      return;
    }

    oss_devin->delay_factor = delay_factor;

    g_rec_mutex_unlock(oss_devin_mutex);

    ags_oss_devin_adjust_delay_and_attack(oss_devin);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_oss_devin_get_property(GObject *gobject,
			   guint prop_id,
			   GValue *value,
			   GParamSpec *param_spec)
{
  AgsOssDevin *oss_devin;

  GRecMutex *oss_devin_mutex;

  oss_devin = AGS_OSS_DEVIN(gobject);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    g_rec_mutex_lock(oss_devin_mutex);

    g_value_set_string(value, oss_devin->device);

    g_rec_mutex_unlock(oss_devin_mutex);
  }
  break;
  case PROP_DSP_CHANNELS:
  {
    g_rec_mutex_lock(oss_devin_mutex);

    g_value_set_uint(value, oss_devin->dsp_channels);

    g_rec_mutex_unlock(oss_devin_mutex);
  }
  break;
  case PROP_PCM_CHANNELS:
  {
    g_rec_mutex_lock(oss_devin_mutex);

    g_value_set_uint(value, oss_devin->pcm_channels);

    g_rec_mutex_unlock(oss_devin_mutex);
  }
  break;
  case PROP_FORMAT:
  {
    g_rec_mutex_lock(oss_devin_mutex);

    g_value_set_uint(value, oss_devin->format);

    g_rec_mutex_unlock(oss_devin_mutex);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    g_rec_mutex_lock(oss_devin_mutex);

    g_value_set_uint(value, oss_devin->buffer_size);

    g_rec_mutex_unlock(oss_devin_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    g_rec_mutex_lock(oss_devin_mutex);

    g_value_set_uint(value, oss_devin->samplerate);

    g_rec_mutex_unlock(oss_devin_mutex);
  }
  break;
  case PROP_BUFFER:
  {
    g_rec_mutex_lock(oss_devin_mutex);

    g_value_set_pointer(value, oss_devin->app_buffer);

    g_rec_mutex_unlock(oss_devin_mutex);
  }
  break;
  case PROP_BPM:
  {
    g_rec_mutex_lock(oss_devin_mutex);

    g_value_set_double(value, oss_devin->bpm);

    g_rec_mutex_unlock(oss_devin_mutex);
  }
  break;
  case PROP_DELAY_FACTOR:
  {
    g_rec_mutex_lock(oss_devin_mutex);

    g_value_set_double(value, oss_devin->delay_factor);

    g_rec_mutex_unlock(oss_devin_mutex);
  }
  break;
  case PROP_ATTACK:
  {
    g_rec_mutex_lock(oss_devin_mutex);

    g_value_set_pointer(value, oss_devin->attack);

    g_rec_mutex_unlock(oss_devin_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_oss_devin_dispose(GObject *gobject)
{
  AgsOssDevin *oss_devin;

  GList *list;

  oss_devin = AGS_OSS_DEVIN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_oss_devin_parent_class)->dispose(gobject);
}

void
ags_oss_devin_finalize(GObject *gobject)
{
  AgsOssDevin *oss_devin;

  guint i;
  
  oss_devin = AGS_OSS_DEVIN(gobject);

  ags_uuid_free(oss_devin->uuid);

  for(i = 0; i < AGS_OSS_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_free(oss_devin->app_buffer[i]);
  }

  g_free(oss_devin->app_buffer);

  for(i = 0; i < AGS_OSS_DEVIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    g_free(oss_devin->backend_buffer[i]);
  }

  g_free(oss_devin->backend_buffer);

  for(i = 0; i < AGS_OSS_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_rec_mutex_clear(oss_devin->app_buffer_mutex[i]);
    
    g_free(oss_devin->app_buffer_mutex[i]);
  }

  g_free(oss_devin->app_buffer_mutex);
  
  for(i = 0; i < AGS_OSS_DEVIN_DEFAULT_APP_BUFFER_SIZE * oss_devin->sub_block_count * oss_devin->pcm_channels; i++){
    g_rec_mutex_clear(oss_devin->sub_block_mutex[i]);
    
    g_free(oss_devin->sub_block_mutex[i]);
  }

  g_free(oss_devin->sub_block_mutex);
  
  g_free(oss_devin->delay);
  g_free(oss_devin->attack);
  
  g_free(oss_devin->device);

  /* call parent */
  G_OBJECT_CLASS(ags_oss_devin_parent_class)->finalize(gobject);
}

AgsUUID*
ags_oss_devin_get_uuid(AgsConnectable *connectable)
{
  AgsOssDevin *oss_devin;
  
  AgsUUID *ptr;

  GRecMutex *oss_devin_mutex;

  oss_devin = AGS_OSS_DEVIN(connectable);

  /* get oss_devin signal mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* get UUID */
  g_rec_mutex_lock(oss_devin_mutex);

  ptr = oss_devin->uuid;

  g_rec_mutex_unlock(oss_devin_mutex);
  
  return(ptr);
}

gboolean
ags_oss_devin_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_oss_devin_is_ready(AgsConnectable *connectable)
{
  AgsOssDevin *oss_devin;
  
  gboolean is_ready;

  oss_devin = AGS_OSS_DEVIN(connectable);

  /* check is added */
  is_ready = ags_oss_devin_test_flags(oss_devin, AGS_OSS_DEVIN_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_oss_devin_add_to_registry(AgsConnectable *connectable)
{
  AgsOssDevin *oss_devin;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  oss_devin = AGS_OSS_DEVIN(connectable);

  ags_oss_devin_set_flags(oss_devin, AGS_OSS_DEVIN_ADDED_TO_REGISTRY);
}

void
ags_oss_devin_remove_from_registry(AgsConnectable *connectable)
{
  AgsOssDevin *oss_devin;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  oss_devin = AGS_OSS_DEVIN(connectable);

  ags_oss_devin_unset_flags(oss_devin, AGS_OSS_DEVIN_ADDED_TO_REGISTRY);
}

xmlNode*
ags_oss_devin_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_oss_devin_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_oss_devin_xml_parse(AgsConnectable *connectable,
			xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_oss_devin_is_connected(AgsConnectable *connectable)
{
  AgsOssDevin *oss_devin;
  
  gboolean is_connected;

  oss_devin = AGS_OSS_DEVIN(connectable);

  /* check is connected */
  is_connected = ags_oss_devin_test_flags(oss_devin, AGS_OSS_DEVIN_CONNECTED);
  
  return(is_connected);
}

void
ags_oss_devin_connect(AgsConnectable *connectable)
{
  AgsOssDevin *oss_devin;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  oss_devin = AGS_OSS_DEVIN(connectable);

  ags_oss_devin_set_flags(oss_devin, AGS_OSS_DEVIN_CONNECTED);
}

void
ags_oss_devin_disconnect(AgsConnectable *connectable)
{

  AgsOssDevin *oss_devin;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  oss_devin = AGS_OSS_DEVIN(connectable);
  
  ags_oss_devin_unset_flags(oss_devin, AGS_OSS_DEVIN_CONNECTED);
}

/**
 * ags_oss_devin_test_flags:
 * @oss_devin: the #AgsOssDevin
 * @flags: the flags
 *
 * Test @flags to be set on @oss_devin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.13.2
 */
gboolean
ags_oss_devin_test_flags(AgsOssDevin *oss_devin, guint flags)
{
  gboolean retval;  
  
  GRecMutex *oss_devin_mutex;

  if(!AGS_IS_OSS_DEVIN(oss_devin)){
    return(FALSE);
  }

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* test */
  g_rec_mutex_lock(oss_devin_mutex);

  retval = (flags & (oss_devin->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(oss_devin_mutex);

  return(retval);
}

/**
 * ags_oss_devin_set_flags:
 * @oss_devin: the #AgsOssDevin
 * @flags: see #AgsOssDevinFlags-enum
 *
 * Enable a feature of @oss_devin.
 *
 * Since: 3.13.2
 */
void
ags_oss_devin_set_flags(AgsOssDevin *oss_devin, guint flags)
{
  GRecMutex *oss_devin_mutex;

  if(!AGS_IS_OSS_DEVIN(oss_devin)){
    return;
  }

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(oss_devin_mutex);

  oss_devin->flags |= flags;
  
  g_rec_mutex_unlock(oss_devin_mutex);
}
    
/**
 * ags_oss_devin_unset_flags:
 * @oss_devin: the #AgsOssDevin
 * @flags: see #AgsOssDevinFlags-enum
 *
 * Disable a feature of @oss_devin.
 *
 * Since: 3.13.2
 */
void
ags_oss_devin_unset_flags(AgsOssDevin *oss_devin, guint flags)
{  
  GRecMutex *oss_devin_mutex;

  if(!AGS_IS_OSS_DEVIN(oss_devin)){
    return;
  }

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(oss_devin_mutex);

  oss_devin->flags &= (~flags);
  
  g_rec_mutex_unlock(oss_devin_mutex);
}

void
ags_oss_devin_set_device(AgsSoundcard *soundcard,
			 gchar *device)
{
  AgsOssDevin *oss_devin;

  GList *card_id, *card_id_start, *card_name, *card_name_start;

  GRecMutex *oss_devin_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);
  
#if 0
  /* list cards */
  card_id = NULL;
  card_name = NULL;
  
  ags_soundcard_list_cards(soundcard,
			   &card_id, &card_name);

  card_id_start = card_id;
  card_name_start = card_name;
  
  /* check card */
  g_rec_mutex_lock(oss_devin_mutex);

  while(card_id != NULL){
    if(!g_ascii_strncasecmp(card_id->data,
			    device,
			    strlen(card_id->data))){
      oss_devin->device = g_strdup(device);

      break;
    }
    
    card_id = card_id->next;
  }

  g_rec_mutex_unlock(oss_devin_mutex);

  /* free card id and name */
  g_list_free_full(card_id_start,
		   g_free);
  g_list_free_full(card_name_start,
		   g_free);
#else
  g_rec_mutex_lock(oss_devin_mutex);

  oss_devin->device = g_strdup(device);

  g_rec_mutex_unlock(oss_devin_mutex);
#endif
}

gchar*
ags_oss_devin_get_device(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  gchar *device;
  
  GRecMutex *oss_devin_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);
  
  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  device = NULL;
  
  g_rec_mutex_lock(oss_devin_mutex);

  device = g_strdup(oss_devin->device);

  g_rec_mutex_unlock(oss_devin_mutex);

  return(device);
}

void
ags_oss_devin_set_presets(AgsSoundcard *soundcard,
			  guint channels,
			  guint rate,
			  guint buffer_size,
			  guint format)
{
  AgsOssDevin *oss_devin;

  oss_devin = AGS_OSS_DEVIN(soundcard);

  g_object_set(oss_devin,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_oss_devin_get_presets(AgsSoundcard *soundcard,
			  guint *channels,
			  guint *rate,
			  guint *buffer_size,
			  guint *format)
{
  AgsOssDevin *oss_devin;

  GRecMutex *oss_devin_mutex;

  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* get presets */
  g_rec_mutex_lock(oss_devin_mutex);

  if(channels != NULL){
    *channels = oss_devin->pcm_channels;
  }

  if(rate != NULL){
    *rate = oss_devin->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = oss_devin->buffer_size;
  }

  if(format != NULL){
    *format = oss_devin->format;
  }

  g_rec_mutex_unlock(oss_devin_mutex);
}

void
ags_oss_devin_list_cards(AgsSoundcard *soundcard,
			 GList **card_id, GList **card_name)
{
  AgsOssDevin *oss_devin;

#if defined(AGS_WITH_OSS)
  oss_sysinfo sysinfo;
  oss_audioinfo ai;
#endif
  
  char *mixer_device;
    
  int mixerfd = -1;

  int next, n;
  int i;

  GRecMutex *oss_devin_mutex;

  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  g_rec_mutex_lock(oss_devin_mutex);

#if defined(AGS_WITH_OSS)
  if((mixer_device = getenv("OSS_MIXERDEV")) == NULL){
    mixer_device = AGS_OSS_DEVIN_DEFAULT_OSS_MIXER_DEVICE;
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

  g_rec_mutex_unlock(oss_devin_mutex);

  if(card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

void
ags_oss_devin_pcm_info(AgsSoundcard *soundcard,
		       char *card_id,
		       guint *channels_min, guint *channels_max,
		       guint *rate_min, guint *rate_max,
		       guint *buffer_size_min, guint *buffer_size_max,
		       GError **error)
{
  AgsOssDevin *oss_devin;

#if defined(AGS_WITH_OSS)
  oss_audioinfo ainfo;
#endif
  
  gchar *str;
    
  int mixerfd;
  int acc;
  unsigned int cmd;
    
  GRecMutex *oss_devin_mutex;

  if(card_id == NULL){
    return;
  }
  
  oss_devin = AGS_OSS_DEVIN(soundcard);
  
  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* pcm info */
  g_rec_mutex_lock(oss_devin_mutex);

#if defined(AGS_WITH_OSS)
  mixerfd = open(card_id, O_RDWR, 0);

  if(mixerfd == -1){
    int e = errno;
      
    str = strerror(e);
    g_message("unable to open pcm device: %s\n", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVIN_ERROR,
		  AGS_OSS_DEVIN_ERROR_LOCKED_SOUNDCARD,
		  "unable to open pcm device: %s\n",
		  str);
    }
    
    goto ags_oss_devin_pcm_info_ERR;
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
    goto ags_oss_devin_pcm_info_ERR;
  }
    
  if(ioctl(mixerfd, cmd, &ainfo) == -1){
    int e = errno;

    str = strerror(e);
    g_message("unable to retrieve audio info: %s\n", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVIN_ERROR,
		  AGS_OSS_DEVIN_ERROR_LOCKED_SOUNDCARD,
		  "unable to retrieve audio info: %s\n",
		  str);
    }
    
    goto ags_oss_devin_pcm_info_ERR;
  }

  if(channels_min != NULL){
    channels_min[0] = ainfo.min_channels;
  }

  if(channels_max != NULL){
    channels_max[0] = ainfo.max_channels;
  }

  if(rate_min != NULL){
    rate_min[0] = ainfo.min_rate;
  }

  if(rate_max != NULL){
    rate_max[0] = ainfo.max_rate;
  }

  if(buffer_size_min != NULL){
    buffer_size_min[0] = 64;
  }

  if(buffer_size_max != NULL){
    buffer_size_max[0] = 8192;
  }
#endif

ags_oss_devin_pcm_info_ERR:

  g_rec_mutex_unlock(oss_devin_mutex);
}

guint
ags_oss_devin_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_CAPTURE);
}

gboolean
ags_oss_devin_is_available(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  gboolean retval;
  
  GRecMutex *oss_devin_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);
  
  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* check available */
  retval = FALSE;

  //TODO:JK: implement me

  return(retval);
}

gboolean
ags_oss_devin_is_starting(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  gboolean is_starting;
  
  GRecMutex *oss_devin_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* check is starting */
  g_rec_mutex_lock(oss_devin_mutex);

  is_starting = ((AGS_OSS_DEVIN_START_RECORD & (oss_devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(oss_devin_mutex);
  
  return(is_starting);
}

gboolean
ags_oss_devin_is_recording(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  gboolean is_recording;
  
  GRecMutex *oss_devin_mutex;

  oss_devin = AGS_OSS_DEVIN(soundcard);
  
  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* check is starting */
  g_rec_mutex_lock(oss_devin_mutex);

  is_recording = ((AGS_OSS_DEVIN_RECORD & (oss_devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(oss_devin_mutex);

  return(is_recording);
}

gchar*
ags_oss_devin_get_uptime(AgsSoundcard *soundcard)
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
ags_oss_devin_device_record_init(AgsSoundcard *soundcard,
				 GError **error)
{
  AgsOssDevin *oss_devin;
  
  gchar *str;

  guint word_size;
  int format;
  int tmp;
  guint i;

  GRecMutex *oss_devin_mutex;

  if(ags_soundcard_is_recording(soundcard)){
    return;
  }

  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* retrieve word size */
  g_rec_mutex_lock(oss_devin_mutex);

  switch(oss_devin->format){
  case AGS_SOUNDCARD_SIGNED_8_BIT:
  {
#ifdef AGS_WITH_OSS
    format = AFMT_U8;
#endif
      
    word_size = sizeof(gint8);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_16_BIT:
  {
#ifdef AGS_WITH_OSS
    format = AFMT_S16_NE;
#endif
      
    word_size = sizeof(gint16);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
  {
#ifdef AGS_WITH_OSS
    format = AFMT_S24_NE;
#endif
      
    word_size = sizeof(gint32);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
  {
#ifdef AGS_WITH_OSS
    format = AFMT_S32_NE;
#endif
      
    word_size = sizeof(gint32);
  }
  break;
  case AGS_SOUNDCARD_SIGNED_64_BIT:
  {
    word_size = sizeof(gint64);
  }
  default:
    g_rec_mutex_unlock(oss_devin_mutex);
    
    g_warning("ags_oss_devin_oss_init(): unsupported word size");
    
    return;
  }

  /* prepare for recording */
  oss_devin->flags |= (AGS_OSS_DEVIN_START_RECORD |
		       AGS_OSS_DEVIN_RECORD |
		       AGS_OSS_DEVIN_NONBLOCKING);

  for(i = 0; i < AGS_OSS_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    memset(oss_devin->app_buffer[i], 0, oss_devin->pcm_channels * oss_devin->buffer_size * word_size);
  }

  /* allocate ring buffer */
  g_atomic_int_set(&(oss_devin->available),
		   FALSE);
  
  for(i = 0; i < AGS_OSS_DEVIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    oss_devin->backend_buffer[i] = (guchar *) g_malloc(oss_devin->pcm_channels *
						       oss_devin->buffer_size * word_size *
						       sizeof(guchar));
  }

#ifdef AGS_WITH_OSS
  /* open device fd */
  str = oss_devin->device;
  oss_devin->device_fd = open(str, O_RDONLY, 0);

  if(oss_devin->device_fd == -1){
    oss_devin->flags &= (~(AGS_OSS_DEVIN_START_RECORD |
			   AGS_OSS_DEVIN_RECORD |
			   AGS_OSS_DEVIN_NONBLOCKING));
    
    g_rec_mutex_unlock(oss_devin_mutex);

    g_warning("couldn't open device %s", oss_devin->device);

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVIN_ERROR,
		  AGS_OSS_DEVIN_ERROR_LOCKED_SOUNDCARD,
		  "unable to open dsp device: %s\n",
		  str);
    }

    return;
  }

  //NOTE:JK: unsupported on kfreebsd 9.0
  //  tmp = APF_NORMAL;
  //  ioctl(oss_devin->device_fd, SNDCTL_DSP_PROFILE, &tmp);

  tmp = format;

  if(ioctl(oss_devin->device_fd, SNDCTL_DSP_SETFMT, &tmp) == -1){
    oss_devin->flags &= (~(AGS_OSS_DEVIN_START_RECORD |
			   AGS_OSS_DEVIN_RECORD |
			   AGS_OSS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(oss_devin_mutex);

    str = strerror(errno);
    g_warning("failed to select bits/sample");

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVIN_ERROR,
		  AGS_OSS_DEVIN_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
		  "unable to open dsp device: %s",
		  str);
    }

    oss_devin->device_fd = -1;

    return;
  }
  
  if(tmp != format){
    oss_devin->flags &= (~(AGS_OSS_DEVIN_START_RECORD |
			   AGS_OSS_DEVIN_RECORD |
			   AGS_OSS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(oss_devin_mutex);

    str = strerror(errno);
    g_warning("failed to select bits/sample");

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVIN_ERROR,
		  AGS_OSS_DEVIN_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
		  "unable to open dsp device: %s",
		  str);
    }

    oss_devin->device_fd = -1;

    return;
  }

  tmp = oss_devin->dsp_channels;

  if(ioctl(oss_devin->device_fd, SNDCTL_DSP_CHANNELS, &tmp) == -1){
    oss_devin->flags &= (~(AGS_OSS_DEVIN_START_RECORD |
			   AGS_OSS_DEVIN_RECORD |
			   AGS_OSS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(oss_devin_mutex);

    str = strerror(errno);
    g_warning("Channels count (%i) not available for recording: %s", oss_devin->dsp_channels, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVIN_ERROR,
		  AGS_OSS_DEVIN_ERROR_CHANNELS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    oss_devin->device_fd = -1;
    
    return;
  }

  if(tmp != oss_devin->dsp_channels){
    oss_devin->flags &= (~(AGS_OSS_DEVIN_START_RECORD |
			   AGS_OSS_DEVIN_RECORD |
			   AGS_OSS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(oss_devin_mutex);

    str = strerror(errno);
    g_warning("Channels count (%i) not available for capture: %s", oss_devin->dsp_channels, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVIN_ERROR,
		  AGS_OSS_DEVIN_ERROR_CHANNELS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }
    
    oss_devin->device_fd = -1;
    
    return;
  }

  tmp = oss_devin->samplerate;

  if(ioctl(oss_devin->device_fd, SNDCTL_DSP_SPEED, &tmp) == -1){
    oss_devin->flags &= (~(AGS_OSS_DEVIN_START_RECORD |
			   AGS_OSS_DEVIN_RECORD |
			   AGS_OSS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(oss_devin_mutex);

    str = strerror(errno);
    g_warning("Rate %iHz not available for capture: %s", oss_devin->samplerate, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVIN_ERROR,
		  AGS_OSS_DEVIN_ERROR_SAMPLERATE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    oss_devin->device_fd = -1;
    
    return;
  }

  if(tmp != oss_devin->samplerate){
    g_warning("Warning: Capture using %d Hz (file %d Hz)",
	      tmp,
	      oss_devin->samplerate);
  }
#endif
  
  oss_devin->tact_counter = 0.0;
  oss_devin->delay_counter = floor(ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(oss_devin)));
  oss_devin->tic_counter = 0;

  oss_devin->backend_buffer_mode = AGS_OSS_DEVIN_BACKEND_BUFFER_0;
    
#ifdef AGS_WITH_OSS
  oss_devin->flags |= AGS_OSS_DEVIN_INITIALIZED;
#endif

  oss_devin->app_buffer_mode = AGS_OSS_DEVIN_APP_BUFFER_0;
  
  g_rec_mutex_unlock(oss_devin_mutex);
}

void
ags_oss_devin_oss_record_fill_buffer(void *app_buffer,
				     guint ags_format,
				     guchar *backend_buffer,
				     guint channels,
				     guint buffer_size)
{
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
    g_warning("ags_oss_devin_oss_record(): unsupported word size");
    return;
  }

  /* fill the channel areas */
  for(count = 0; count < buffer_size; count++){
    for(chn = 0; chn < channels; chn++){
      /* Generate data in native endian format */
      res = 0;
	
      if(ags_endian_host_is_be()){
	for(i = 0; i < bps; i++){
	  res |= (int) (*(backend_buffer + chn * bps + word_size - 1 - i)) << (i * 8);
	}
      }else{
	for(i = 0; i < bps; i++){
	  res |= (int) (*(backend_buffer + chn * bps + i)) << (i * 8);
	}
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
}

void
ags_oss_devin_device_record(AgsSoundcard *soundcard,
			    GError **error)
{
  AgsOssDevin *oss_devin;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;
  GList *list;

  gchar *str;
  
  guint word_size;

  int n_write;
  
  GRecMutex *oss_devin_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  application_context = ags_application_context_get_instance();

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);
  
#ifdef AGS_WITH_OSS
  /* lock */
  g_rec_mutex_lock(oss_devin_mutex);

  /* retrieve word size */
  switch(oss_devin->format){
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
  //NOTE:JK: not available    break;
  default:
    g_rec_mutex_unlock(oss_devin_mutex);
    
    g_warning("ags_oss_devin_oss_record(): unsupported word size");
    
    return;
  }

  /* do capture */
  oss_devin->flags &= (~AGS_OSS_DEVIN_START_RECORD);

  if((AGS_OSS_DEVIN_INITIALIZED & (oss_devin->flags)) == 0){
    g_rec_mutex_unlock(oss_devin_mutex);
    
    return;
  }

  /* write ring buffer */
  n_write = read(oss_devin->device_fd,
		 oss_devin->backend_buffer[oss_devin->backend_buffer_mode],
		 oss_devin->pcm_channels * oss_devin->buffer_size * word_size * sizeof (char));

  /* fill buffer */
  ags_soundcard_lock_buffer(soundcard,
			    oss_devin->app_buffer[oss_devin->app_buffer_mode]);

  ags_oss_devin_oss_record_fill_buffer(oss_devin->app_buffer[oss_devin->app_buffer_mode],
				       oss_devin->format,
				       oss_devin->backend_buffer[oss_devin->backend_buffer_mode],
				       oss_devin->pcm_channels,
				       oss_devin->buffer_size);

  ags_soundcard_unlock_buffer(soundcard,
			      oss_devin->app_buffer[oss_devin->app_buffer_mode]);

  g_atomic_int_set(&(oss_devin->available),
		   FALSE);
  
  if(n_write != oss_devin->pcm_channels * oss_devin->buffer_size * word_size * sizeof (char)){
    g_critical("write() return doesn't match written bytes");
  }

  /* increment nth backend buffer */
  if(oss_devin->backend_buffer_mode + 1 > AGS_OSS_DEVIN_BACKEND_BUFFER_7){
    oss_devin->backend_buffer_mode = AGS_OSS_DEVIN_BACKEND_BUFFER_0;
  }else{
    oss_devin->backend_buffer_mode += 1;
  }
    
  g_rec_mutex_unlock(oss_devin_mutex);
#endif

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;
  
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) oss_devin);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) oss_devin);
  task = g_list_append(task,
		       clear_buffer);
  
  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) oss_devin);
  task = g_list_append(task,
		       clear_buffer);

  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) oss_devin);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_oss_devin_device_free(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  guint i;
  
  GRecMutex *oss_devin_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /*  */
  g_rec_mutex_lock(oss_devin_mutex);

  if((AGS_OSS_DEVIN_INITIALIZED & (oss_devin->flags)) == 0){
    g_rec_mutex_unlock(oss_devin_mutex);
    
    return;
  }
  
  close(oss_devin->device_fd);
  oss_devin->device_fd = -1;

  /* free backend buffer */
  if(oss_devin->backend_buffer != NULL){
    for(i = 0; i < AGS_OSS_DEVIN_DEFAULT_BACKEND_BUFFER_SIZE; i++){
      g_free(oss_devin->backend_buffer[i]);

      oss_devin->backend_buffer[i] = NULL;
    }
  }

  /* reset flags */
  oss_devin->flags &= (~(AGS_OSS_DEVIN_RECORD |
			 AGS_OSS_DEVIN_INITIALIZED));

  oss_devin->app_buffer_mode = AGS_OSS_DEVIN_APP_BUFFER_0;

  oss_devin->backend_buffer_mode = AGS_OSS_DEVIN_BACKEND_BUFFER_0;

  oss_devin->note_offset = oss_devin->start_note_offset;
  oss_devin->note_offset_absolute = oss_devin->start_note_offset;

  g_rec_mutex_unlock(oss_devin_mutex);
}

void
ags_oss_devin_tic(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  gdouble delay;
  gdouble delay_counter;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  
  GRecMutex *oss_devin_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(oss_devin_mutex);

  delay = oss_devin->delay[oss_devin->tic_counter];
  delay_counter = oss_devin->delay_counter;

  note_offset = oss_devin->note_offset;
  note_offset_absolute = oss_devin->note_offset_absolute;
  
  loop_left = oss_devin->loop_left;
  loop_right = oss_devin->loop_right;
  
  do_loop = oss_devin->do_loop;

  g_rec_mutex_unlock(oss_devin_mutex);

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
    g_rec_mutex_lock(oss_devin_mutex);
    
    oss_devin->delay_counter = delay_counter + 1.0 - delay;
    oss_devin->tact_counter += 1.0;

    g_rec_mutex_unlock(oss_devin_mutex);
  }else{
    g_rec_mutex_lock(oss_devin_mutex);
    
    oss_devin->delay_counter += 1.0;

    g_rec_mutex_unlock(oss_devin_mutex);
  }
}

void
ags_oss_devin_offset_changed(AgsSoundcard *soundcard,
			     guint note_offset)
{
  AgsOssDevin *oss_devin;
  
  GRecMutex *oss_devin_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* offset changed */
  g_rec_mutex_lock(oss_devin_mutex);

  oss_devin->tic_counter += 1;

  if(oss_devin->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    oss_devin->tic_counter = 0;
  }

  g_rec_mutex_unlock(oss_devin_mutex);
}

void
ags_oss_devin_set_bpm(AgsSoundcard *soundcard,
		      gdouble bpm)
{
  AgsOssDevin *oss_devin;

  GRecMutex *oss_devin_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* set bpm */
  g_rec_mutex_lock(oss_devin_mutex);

  oss_devin->bpm = bpm;

  g_rec_mutex_unlock(oss_devin_mutex);

  ags_oss_devin_adjust_delay_and_attack(oss_devin);
}

gdouble
ags_oss_devin_get_bpm(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  gdouble bpm;
  
  GRecMutex *oss_devin_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* get bpm */
  g_rec_mutex_lock(oss_devin_mutex);

  bpm = oss_devin->bpm;
  
  g_rec_mutex_unlock(oss_devin_mutex);

  return(bpm);
}

void
ags_oss_devin_set_delay_factor(AgsSoundcard *soundcard,
			       gdouble delay_factor)
{
  AgsOssDevin *oss_devin;

  GRecMutex *oss_devin_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* set delay factor */
  g_rec_mutex_lock(oss_devin_mutex);

  oss_devin->delay_factor = delay_factor;

  g_rec_mutex_unlock(oss_devin_mutex);

  ags_oss_devin_adjust_delay_and_attack(oss_devin);
}

gdouble
ags_oss_devin_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  gdouble delay_factor;
  
  GRecMutex *oss_devin_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* get delay factor */
  g_rec_mutex_lock(oss_devin_mutex);

  delay_factor = oss_devin->delay_factor;
  
  g_rec_mutex_unlock(oss_devin_mutex);

  return(delay_factor);
}

gdouble
ags_oss_devin_get_delay(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  guint index;
  gdouble delay;
  
  GRecMutex *oss_devin_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* get delay */
  g_rec_mutex_lock(oss_devin_mutex);

  index = oss_devin->tic_counter;

  delay = oss_devin->delay[index];
  
  g_rec_mutex_unlock(oss_devin_mutex);
  
  return(delay);
}

gdouble
ags_oss_devin_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  gdouble absolute_delay;
  
  GRecMutex *oss_devin_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);
  
  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* get absolute delay */
  g_rec_mutex_lock(oss_devin_mutex);

  absolute_delay = (60.0 * (((gdouble) oss_devin->samplerate / (gdouble) oss_devin->buffer_size) / (gdouble) oss_devin->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) oss_devin->delay_factor)));

  g_rec_mutex_unlock(oss_devin_mutex);

  return(absolute_delay);
}

guint
ags_oss_devin_get_attack(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  guint index;
  guint attack;
  
  GRecMutex *oss_devin_mutex;  

  oss_devin = AGS_OSS_DEVIN(soundcard);
  
  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* get attack */
  g_rec_mutex_lock(oss_devin_mutex);

  index = oss_devin->tic_counter;

  attack = oss_devin->attack[index];

  g_rec_mutex_unlock(oss_devin_mutex);
  
  return(attack);
}

void*
ags_oss_devin_get_buffer(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  void *buffer;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  buffer = oss_devin->app_buffer[oss_devin->app_buffer_mode];

  return(buffer);
}

void*
ags_oss_devin_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  void *buffer;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  if(oss_devin->app_buffer_mode == AGS_OSS_DEVIN_APP_BUFFER_3){
    buffer = oss_devin->app_buffer[AGS_OSS_DEVIN_APP_BUFFER_0];
  }else{
    buffer = oss_devin->app_buffer[oss_devin->app_buffer_mode + 1];
  }

  return(buffer);
}

void*
ags_oss_devin_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  void *buffer;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  if(oss_devin->app_buffer_mode == AGS_OSS_DEVIN_APP_BUFFER_0){
    buffer = oss_devin->app_buffer[AGS_OSS_DEVIN_APP_BUFFER_3];
  }else{
    buffer = oss_devin->app_buffer[oss_devin->app_buffer_mode - 1];
  }

  return(buffer);
}

void
ags_oss_devin_lock_buffer(AgsSoundcard *soundcard,
			   void *buffer)
{
  AgsOssDevin *oss_devin;

  GRecMutex *buffer_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(oss_devin->app_buffer != NULL){
    if(buffer == oss_devin->app_buffer[0]){
      buffer_mutex = oss_devin->app_buffer_mutex[0];
    }else if(buffer == oss_devin->app_buffer[1]){
      buffer_mutex = oss_devin->app_buffer_mutex[1];
    }else if(buffer == oss_devin->app_buffer[2]){
      buffer_mutex = oss_devin->app_buffer_mutex[2];
    }else if(buffer == oss_devin->app_buffer[3]){
      buffer_mutex = oss_devin->app_buffer_mutex[3];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_oss_devin_unlock_buffer(AgsSoundcard *soundcard,
			     void *buffer)
{
  AgsOssDevin *oss_devin;

  GRecMutex *buffer_mutex;
  
  oss_devin = AGS_OSS_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(oss_devin->app_buffer != NULL){
    if(buffer == oss_devin->app_buffer[0]){
      buffer_mutex = oss_devin->app_buffer_mutex[0];
    }else if(buffer == oss_devin->app_buffer[1]){
      buffer_mutex = oss_devin->app_buffer_mutex[1];
    }else if(buffer == oss_devin->app_buffer[2]){
      buffer_mutex = oss_devin->app_buffer_mutex[2];
    }else if(buffer == oss_devin->app_buffer[3]){
      buffer_mutex = oss_devin->app_buffer_mutex[3];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_oss_devin_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  guint delay_counter;
  
  GRecMutex *oss_devin_mutex;  

  oss_devin = AGS_OSS_DEVIN(soundcard);
  
  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* delay counter */
  g_rec_mutex_lock(oss_devin_mutex);

  delay_counter = oss_devin->delay_counter;
  
  g_rec_mutex_unlock(oss_devin_mutex);

  return(delay_counter);
}

void
ags_oss_devin_set_start_note_offset(AgsSoundcard *soundcard,
				    guint start_note_offset)
{
  AgsOssDevin *oss_devin;

  GRecMutex *oss_devin_mutex;  

  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* set note offset */
  g_rec_mutex_lock(oss_devin_mutex);

  oss_devin->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(oss_devin_mutex);
}

guint
ags_oss_devin_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  guint start_note_offset;
  
  GRecMutex *oss_devin_mutex;  

  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* set note offset */
  g_rec_mutex_lock(oss_devin_mutex);

  start_note_offset = oss_devin->start_note_offset;

  g_rec_mutex_unlock(oss_devin_mutex);

  return(start_note_offset);
}

void
ags_oss_devin_set_note_offset(AgsSoundcard *soundcard,
			      guint note_offset)
{
  AgsOssDevin *oss_devin;

  GRecMutex *oss_devin_mutex;  

  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* set note offset */
  g_rec_mutex_lock(oss_devin_mutex);

  oss_devin->note_offset = note_offset;

  g_rec_mutex_unlock(oss_devin_mutex);
}

guint
ags_oss_devin_get_note_offset(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  guint note_offset;
  
  GRecMutex *oss_devin_mutex;  

  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* set note offset */
  g_rec_mutex_lock(oss_devin_mutex);

  note_offset = oss_devin->note_offset;

  g_rec_mutex_unlock(oss_devin_mutex);

  return(note_offset);
}

void
ags_oss_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
				       guint note_offset_absolute)
{
  AgsOssDevin *oss_devin;
  
  GRecMutex *oss_devin_mutex;  

  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* set note offset */
  g_rec_mutex_lock(oss_devin_mutex);

  oss_devin->note_offset_absolute = note_offset_absolute;

  g_rec_mutex_unlock(oss_devin_mutex);
}

guint
ags_oss_devin_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  guint note_offset_absolute;
  
  GRecMutex *oss_devin_mutex;  

  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* set note offset */
  g_rec_mutex_lock(oss_devin_mutex);

  note_offset_absolute = oss_devin->note_offset_absolute;

  g_rec_mutex_unlock(oss_devin_mutex);

  return(note_offset_absolute);
}

void
ags_oss_devin_set_loop(AgsSoundcard *soundcard,
		       guint loop_left, guint loop_right,
		       gboolean do_loop)
{
  AgsOssDevin *oss_devin;

  GRecMutex *oss_devin_mutex;  

  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* set loop */
  g_rec_mutex_lock(oss_devin_mutex);

  oss_devin->loop_left = loop_left;
  oss_devin->loop_right = loop_right;
  oss_devin->do_loop = do_loop;

  if(do_loop){
    oss_devin->loop_offset = oss_devin->note_offset;
  }

  g_rec_mutex_unlock(oss_devin_mutex);
}

void
ags_oss_devin_get_loop(AgsSoundcard *soundcard,
		       guint *loop_left, guint *loop_right,
		       gboolean *do_loop)
{
  AgsOssDevin *oss_devin;

  GRecMutex *oss_devin_mutex;  

  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* get loop */
  g_rec_mutex_lock(oss_devin_mutex);

  if(loop_left != NULL){
    *loop_left = oss_devin->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = oss_devin->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = oss_devin->do_loop;
  }

  g_rec_mutex_unlock(oss_devin_mutex);
}

guint
ags_oss_devin_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsOssDevin *oss_devin;

  guint loop_offset;
  
  GRecMutex *oss_devin_mutex;  

  oss_devin = AGS_OSS_DEVIN(soundcard);

  /* get oss_devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* get loop offset */
  g_rec_mutex_lock(oss_devin_mutex);

  loop_offset = oss_devin->loop_offset;
  
  g_rec_mutex_unlock(oss_devin_mutex);

  return(loop_offset);
}

/**
 * ags_oss_devin_switch_buffer_flag:
 * @oss_devin: the #AgsOssDevin
 *
 * The buffer flag indicates the currently recorded buffer.
 *
 * Since: 3.13.2
 */
void
ags_oss_devin_switch_buffer_flag(AgsOssDevin *oss_devin)
{
  GRecMutex *oss_devin_mutex;
  
  if(!AGS_IS_OSS_DEVIN(oss_devin)){
    return;
  }

  /* get oss devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* switch buffer flag */
  g_rec_mutex_lock(oss_devin_mutex);

  if(oss_devin->app_buffer_mode < AGS_OSS_DEVIN_APP_BUFFER_3){
    oss_devin->app_buffer_mode += 1;
  }else{
    oss_devin->app_buffer_mode = AGS_OSS_DEVIN_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(oss_devin_mutex);
}

/**
 * ags_oss_devin_adjust_delay_and_attack:
 * @oss_devin: the #AgsOssDevin
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 3.13.2
 */
void
ags_oss_devin_adjust_delay_and_attack(AgsOssDevin *oss_devin)
{
  if(!AGS_IS_OSS_DEVIN(oss_devin)){
    return;
  }
  
  ags_soundcard_util_adjust_delay_and_attack(oss_devin);
}

/**
 * ags_oss_devin_realloc_buffer:
 * @oss_devin: the #AgsOssDevin
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.13.2
 */
void
ags_oss_devin_realloc_buffer(AgsOssDevin *oss_devin)
{
  guint pcm_channels;
  guint buffer_size;
  guint word_size;
  guint format;
  
  GRecMutex *oss_devin_mutex;  

  if(!AGS_IS_OSS_DEVIN(oss_devin)){
    return;
  }

  /* get oss devin mutex */
  oss_devin_mutex = AGS_OSS_DEVIN_GET_OBJ_MUTEX(oss_devin);

  /* get word size */  
  g_rec_mutex_lock(oss_devin_mutex);

  pcm_channels = oss_devin->pcm_channels;
  buffer_size = oss_devin->buffer_size;

  format = oss_devin->format;
  
  g_rec_mutex_unlock(oss_devin_mutex);
  
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
    g_warning("ags_oss_devin_realloc_buffer(): unsupported word size");
    
    return;
  }  

  //NOTE:JK: there is no lock applicable to buffer

  if(oss_devin->app_buffer[0] != NULL){
    g_free(oss_devin->app_buffer[0]);
  }
  
  oss_devin->app_buffer[0] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  if(oss_devin->app_buffer[1] != NULL){
    g_free(oss_devin->app_buffer[1]);
  }

  oss_devin->app_buffer[1] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  if(oss_devin->app_buffer[2] != NULL){
    g_free(oss_devin->app_buffer[2]);
  }

  oss_devin->app_buffer[2] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  if(oss_devin->app_buffer[3] != NULL){
    g_free(oss_devin->app_buffer[3]);
  }
  
  oss_devin->app_buffer[3] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_oss_devin_new:
 *
 * Creates a new instance of #AgsOssDevin.
 *
 * Returns: the new #AgsOssDevin
 *
 * Since: 3.13.2
 */
AgsOssDevin*
ags_oss_devin_new()
{
  AgsOssDevin *oss_devin;

  oss_devin = (AgsOssDevin *) g_object_new(AGS_TYPE_OSS_DEVIN,
					   NULL);
  
  return(oss_devin);
}
