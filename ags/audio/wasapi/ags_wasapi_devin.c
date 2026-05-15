/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_soundcard_util.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <ags/audio/thread/ags_audio_loop.h>
#include <ags/audio/thread/ags_soundcard_thread.h>

#include <string.h>
#include <math.h>
#include <time.h>

#ifdef AGS_WITH_WASAPI
#include <windows.h>
#include <ole2.h>
#include <ksmedia.h>
#include <wchar.h>
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

void ags_wasapi_devin_set_bpm(AgsSoundcard *soundcard,
			      gdouble bpm);
gdouble ags_wasapi_devin_get_bpm(AgsSoundcard *soundcard);

void ags_wasapi_devin_set_start_note_offset(AgsSoundcard *soundcard,
					    guint64 start_note_offset);
guint64 ags_wasapi_devin_get_start_note_offset(AgsSoundcard *soundcard);

GObject* ags_wasapi_devin_get_frame_clock(AgsSoundcard *soundcard);

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
				     guint64 note_offset);

void* ags_wasapi_devin_get_buffer(AgsSoundcard *soundcard);
void* ags_wasapi_devin_get_next_buffer(AgsSoundcard *soundcard);
void* ags_wasapi_devin_get_prev_buffer(AgsSoundcard *soundcard);

void ags_wasapi_devin_lock_buffer(AgsSoundcard *soundcard,
				  void *buffer);
void ags_wasapi_devin_unlock_buffer(AgsSoundcard *soundcard,
				    void *buffer);

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
  PROP_DEVICE,
  PROP_DSP_CHANNELS,
  PROP_PCM_CHANNELS,
  PROP_FORMAT,
  PROP_BUFFER_SIZE,
  PROP_SAMPLERATE,
  PROP_BPM,
  PROP_BUFFER,
  PROP_CHANNEL,
};

static gpointer ags_wasapi_devin_parent_class = NULL;

#ifdef AGS_W32API
static const GUID ags_wasapi_clsid_mm_device_enumerator_guid = {0xBCDE0395, 0xE52F, 0x467C, 0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E};
static const GUID ags_wasapi_iid_mm_device_enumerator_guid = {0xA95664D2, 0x9614, 0x4F35, 0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6};
static const GUID ags_wasapi_iid_audio_client_guid = {0x1CB9AD4C, 0xDBFA, 0x4c32, 0xB1, 0x78, 0xC2, 0xF5, 0x68, 0xA7, 0x03, 0xB2};
static const GUID ags_wasapi_iid_audio_capture_client_guid = {0xc8adbd64, 0xe71e, 0x48a0, 0xa4,0xde, 0x18,0x5c,0x39,0x5c,0xd3,0x17};
static const GUID ags_wasapi_pcm_subformat_guid = {0x00000001, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71};
static const GUID ags_wasapi_pkey_device_friendly_name_guid = {0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0};

struct _PROPERTYKEY{
  GUID fmtid;
  DWORD id;
};

static struct _PROPERTYKEY ags_wasapi_pkey_device_friendly_name_key = {{0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0},
								       14};
#endif

GType
ags_wasapi_devin_get_type (void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
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

    g_once_init_leave(&g_define_type_id__static, ags_type_wasapi_devin);
  }

  return(g_define_type_id__static);
}

GType
ags_wasapi_devin_flags_get_type()
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_WASAPI_DEVIN_INITIALIZED, "AGS_WASAPI_DEVIN_INITIALIZED", "wasapi-devin-initialized" },
      { AGS_WASAPI_DEVIN_START_RECORD, "AGS_WASAPI_DEVIN_START_RECORD", "wasapi-devin-start-record" },
      { AGS_WASAPI_DEVIN_RECORD, "AGS_WASAPI_DEVIN_RECORD", "wasapi-devin-record" },
      { AGS_WASAPI_DEVIN_NONBLOCKING, "AGS_WASAPI_DEVIN_NONBLOCKING", "wasapi-devin-nonblocking" },
      { AGS_WASAPI_DEVIN_ATTACK_FIRST, "AGS_WASAPI_DEVIN_ATTACK_FIRST", "wasapi-devin-attack-first" },
      { AGS_WASAPI_DEVIN_SHARE_MODE_EXCLUSIVE, "AGS_WASAPI_DEVIN_SHARE_MODE_EXCLUSIVE", "wasapi-devin-share-mode-exclusive" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsWasapiDevinFlags"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
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
   * AgsWasapiDevin:device:
   *
   * The core audio soundcard indentifier
   * 
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * Since: 3.0.0
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
   * AgsWasapiDevin:bpm:
   *
   * Beats per minute
   * 
   * Since: 3.0.0
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
   * AgsWasapiDevin:buffer:
   *
   * The buffer
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to play"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
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
  connectable->get_uuid = ags_wasapi_devin_get_uuid;
  connectable->has_resource = ags_wasapi_devin_has_resource;

  connectable->is_ready = ags_wasapi_devin_is_ready;
  connectable->add_to_registry = ags_wasapi_devin_add_to_registry;
  connectable->remove_from_registry = ags_wasapi_devin_remove_from_registry;

  connectable->list_resource = ags_wasapi_devin_list_resource;
  connectable->xml_compose = ags_wasapi_devin_xml_compose;
  connectable->xml_parse = ags_wasapi_devin_xml_parse;

  connectable->is_connected = ags_wasapi_devin_is_connected;  
  connectable->connect = ags_wasapi_devin_connect;
  connectable->disconnect = ags_wasapi_devin_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_wasapi_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_wasapi_devin_set_device;
  soundcard->get_device = ags_wasapi_devin_get_device;
  
  soundcard->set_presets = ags_wasapi_devin_set_presets;
  soundcard->get_presets = ags_wasapi_devin_get_presets;

  soundcard->list_cards = ags_wasapi_devin_list_cards;
  soundcard->pcm_info = ags_wasapi_devin_pcm_info;
  soundcard->get_capability = ags_wasapi_devin_get_capability;

  soundcard->set_bpm = ags_wasapi_devin_set_bpm;
  soundcard->get_bpm = ags_wasapi_devin_get_bpm;

  soundcard->set_start_note_offset = ags_wasapi_devin_set_start_note_offset;
  soundcard->get_start_note_offset = ags_wasapi_devin_get_start_note_offset;
  
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
    
  soundcard->get_buffer = ags_wasapi_devin_get_buffer;
  soundcard->get_next_buffer = ags_wasapi_devin_get_next_buffer;
  soundcard->get_prev_buffer = ags_wasapi_devin_get_prev_buffer;
  
  soundcard->lock_buffer = ags_wasapi_devin_lock_buffer;
  soundcard->unlock_buffer = ags_wasapi_devin_unlock_buffer;
}

void
ags_wasapi_devin_init(AgsWasapiDevin *wasapi_devin)
{
  AgsConfig *config;

  gchar *str;
  gchar *segmentation;

  gdouble absolute_delay;
  guint denumerator, numerator;
  guint i;
  
  config = ags_config_get_instance();

  /* flags */
  wasapi_devin->flags = 0;
  wasapi_devin->connectable_flags = 0;

  /* devin mutex */
  g_rec_mutex_init(&(wasapi_devin->obj_mutex));

  /* uuid */
  wasapi_devin->uuid = ags_uuid_alloc();
  ags_uuid_generate(wasapi_devin->uuid);

  /* presets */
  wasapi_devin->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  wasapi_devin->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  wasapi_devin->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  wasapi_devin->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  wasapi_devin->format = ags_soundcard_helper_config_get_format(config);

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "wasapi-buffer-size");

  if(str != NULL){
    wasapi_devin->wasapi_buffer_size = g_ascii_strtoull(str,
							NULL,
							10);
  }else{
    wasapi_devin->wasapi_buffer_size = AGS_WASAPI_DEVIN_DEFAULT_WASAPI_BUFFER_SIZE;
  }
  
  g_free(str);

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "wasapi-share-mode");

  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "exclusive",
			  10)){
    wasapi_devin->flags |= AGS_WASAPI_DEVIN_SHARE_MODE_EXCLUSIVE;
  }

  g_free(str);
    
  /* bpm */
  wasapi_devin->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* start note offset */
  wasapi_devin->start_note_offset = 0;
  
  /* frame clock */
  wasapi_devin->frame_clock = ags_frame_clock_new();

  /* device */
  wasapi_devin->device = NULL;

  /* app buffer mutex */
  wasapi_devin->app_buffer_mode = AGS_WASAPI_DEVIN_APP_BUFFER_0;

  wasapi_devin->app_buffer_mutex = (GRecMutex **) g_malloc(8 * sizeof(GRecMutex *));

  for(i = 0; i < 8; i++){
    wasapi_devin->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(wasapi_devin->app_buffer_mutex[i]);
  }

  /* sub-block mutex */
  wasapi_devin->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  wasapi_devin->sub_block_mutex = (GRecMutex **) g_malloc(8 * wasapi_devin->sub_block_count * wasapi_devin->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < 8 * wasapi_devin->sub_block_count * wasapi_devin->pcm_channels; i++){
    wasapi_devin->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(wasapi_devin->sub_block_mutex[i]);
  }

  /* app buffer */
  wasapi_devin->app_buffer = (void **) g_malloc(8 * sizeof(void*));

  wasapi_devin->app_buffer[0] = NULL;
  wasapi_devin->app_buffer[1] = NULL;
  wasapi_devin->app_buffer[2] = NULL;
  wasapi_devin->app_buffer[3] = NULL;
  wasapi_devin->app_buffer[4] = NULL;
  wasapi_devin->app_buffer[5] = NULL;
  wasapi_devin->app_buffer[6] = NULL;
  wasapi_devin->app_buffer[7] = NULL;
  
  ags_wasapi_devin_realloc_buffer(wasapi_devin);
  
  /* callback mutex */
  g_mutex_init(&(wasapi_devin->callback_mutex));

  g_cond_init(&(wasapi_devin->callback_cond));

  /* callback finish mutex */
  g_mutex_init(&(wasapi_devin->callback_finish_mutex));

  g_cond_init(&(wasapi_devin->callback_finish_cond));
}

void
ags_wasapi_devin_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsWasapiDevin *wasapi_devin;
  
  GRecMutex *wasapi_devin_mutex;

  wasapi_devin = AGS_WASAPI_DEVIN(gobject);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    char *device;

    device = (char *) g_value_get_string(value);

    g_rec_mutex_lock(wasapi_devin_mutex);

    wasapi_devin->device = g_strdup(device);

    g_rec_mutex_unlock(wasapi_devin_mutex);
  }
  break;
  case PROP_DSP_CHANNELS:
  {
    guint dsp_channels;

    dsp_channels = g_value_get_uint(value);

    g_rec_mutex_lock(wasapi_devin_mutex);

    if(dsp_channels == wasapi_devin->dsp_channels){
      g_rec_mutex_unlock(wasapi_devin_mutex);

      return;
    }

    wasapi_devin->dsp_channels = dsp_channels;

    g_rec_mutex_unlock(wasapi_devin_mutex);
  }
  break;
  case PROP_PCM_CHANNELS:
  {
    guint pcm_channels;

    pcm_channels = g_value_get_uint(value);

    g_rec_mutex_lock(wasapi_devin_mutex);

    if(pcm_channels == wasapi_devin->pcm_channels){
      g_rec_mutex_unlock(wasapi_devin_mutex);

      return;
    }

    wasapi_devin->pcm_channels = pcm_channels;

    g_rec_mutex_unlock(wasapi_devin_mutex);

    ags_wasapi_devin_realloc_buffer(wasapi_devin);
  }
  break;
  case PROP_FORMAT:
  {
    guint format;

    format = g_value_get_uint(value);

    g_rec_mutex_lock(wasapi_devin_mutex);

    if(format == wasapi_devin->format){
      g_rec_mutex_unlock(wasapi_devin_mutex);

      return;
    }

    wasapi_devin->format = format;

    g_rec_mutex_unlock(wasapi_devin_mutex);

    ags_wasapi_devin_realloc_buffer(wasapi_devin);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    guint buffer_size;

    buffer_size = g_value_get_uint(value);

    g_rec_mutex_lock(wasapi_devin_mutex);

    if(buffer_size == wasapi_devin->buffer_size){
      g_rec_mutex_unlock(wasapi_devin_mutex);

      return;
    }

    wasapi_devin->buffer_size = buffer_size;

    ags_frame_clock_set_buffer_size(wasapi_devin->frame_clock,
				    buffer_size);

    g_rec_mutex_unlock(wasapi_devin_mutex);

    ags_wasapi_devin_realloc_buffer(wasapi_devin);
  }
  break;
  case PROP_SAMPLERATE:
  {
    guint samplerate;

    samplerate = g_value_get_uint(value);

    g_rec_mutex_lock(wasapi_devin_mutex);
      
    if(samplerate == wasapi_devin->samplerate){
      g_rec_mutex_unlock(wasapi_devin_mutex);

      return;
    }

    wasapi_devin->samplerate = samplerate;

    ags_frame_clock_set_samplerate(wasapi_devin->frame_clock,
				   samplerate);

    g_rec_mutex_unlock(wasapi_devin_mutex);

    ags_wasapi_devin_realloc_buffer(wasapi_devin);
  }
  break;
  case PROP_BPM:
  {
    gdouble bpm;
      
    bpm = g_value_get_double(value);

    g_rec_mutex_lock(wasapi_devin_mutex);

    wasapi_devin->bpm = bpm;

    ags_frame_clock_set_bpm(wasapi_devin->frame_clock,
			    bpm);

    g_rec_mutex_unlock(wasapi_devin_mutex);
  }
  break;
  case PROP_BUFFER:
  {
    //TODO:JK: implement me
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

  GRecMutex *wasapi_devin_mutex;

  wasapi_devin = AGS_WASAPI_DEVIN(gobject);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    g_rec_mutex_lock(wasapi_devin_mutex);

    g_value_set_string(value, wasapi_devin->device);

    g_rec_mutex_unlock(wasapi_devin_mutex);
  }
  break;
  case PROP_DSP_CHANNELS:
  {
    g_rec_mutex_lock(wasapi_devin_mutex);

    g_value_set_uint(value, wasapi_devin->dsp_channels);

    g_rec_mutex_unlock(wasapi_devin_mutex);
  }
  break;
  case PROP_PCM_CHANNELS:
  {
    g_rec_mutex_lock(wasapi_devin_mutex);

    g_value_set_uint(value, wasapi_devin->pcm_channels);

    g_rec_mutex_unlock(wasapi_devin_mutex);
  }
  break;
  case PROP_FORMAT:
  {
    g_rec_mutex_lock(wasapi_devin_mutex);

    g_value_set_uint(value, wasapi_devin->format);

    g_rec_mutex_unlock(wasapi_devin_mutex);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    g_rec_mutex_lock(wasapi_devin_mutex);

    g_value_set_uint(value, wasapi_devin->buffer_size);

    g_rec_mutex_unlock(wasapi_devin_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    g_rec_mutex_lock(wasapi_devin_mutex);

    g_value_set_uint(value, wasapi_devin->samplerate);

    g_rec_mutex_unlock(wasapi_devin_mutex);
  }
  break;
  case PROP_BPM:
  {
    g_rec_mutex_lock(wasapi_devin_mutex);

    g_value_set_double(value, wasapi_devin->bpm);

    g_rec_mutex_unlock(wasapi_devin_mutex);
  }
  break;
  case PROP_BUFFER:
  {
    g_rec_mutex_lock(wasapi_devin_mutex);

    g_value_set_pointer(value, wasapi_devin->app_buffer);

    g_rec_mutex_unlock(wasapi_devin_mutex);
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

  guint i;

  wasapi_devin = AGS_WASAPI_DEVIN(gobject);

  ags_uuid_free(wasapi_devin->uuid);  

  /* frame clock */
  if(wasapi_devin->frame_clock != NULL){
    g_object_unref(wasapi_devin->frame_clock);
  }

  /* app buffer */
  for(i = 0; i < AGS_WASAPI_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_free(wasapi_devin->app_buffer[i]);
  }

  g_free(wasapi_devin->app_buffer);

  /* app buffer mutex */
  for(i = 0; i < AGS_WASAPI_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_rec_mutex_clear(wasapi_devin->app_buffer_mutex[i]);
    
    g_free(wasapi_devin->app_buffer_mutex[i]);
  }

  g_free(wasapi_devin->app_buffer_mutex);
  
  /* sub-block mutex */
  for(i = 0; i < AGS_WASAPI_DEVIN_DEFAULT_APP_BUFFER_SIZE * wasapi_devin->sub_block_count * wasapi_devin->pcm_channels; i++){
    g_rec_mutex_clear(wasapi_devin->sub_block_mutex[i]);
    
    g_free(wasapi_devin->sub_block_mutex[i]);
  }

  g_free(wasapi_devin->sub_block_mutex);

  g_free(wasapi_devin->device);
  
  /* call parent */
  G_OBJECT_CLASS(ags_wasapi_devin_parent_class)->finalize(gobject);
}

AgsUUID*
ags_wasapi_devin_get_uuid(AgsConnectable *connectable)
{
  AgsWasapiDevin *wasapi_devin;
  
  AgsUUID *ptr;

  GRecMutex *wasapi_devin_mutex;

  wasapi_devin = AGS_WASAPI_DEVIN(connectable);

  /* get wasapi devin signal mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get UUID */
  g_rec_mutex_lock(wasapi_devin_mutex);

  ptr = wasapi_devin->uuid;

  g_rec_mutex_unlock(wasapi_devin_mutex);
  
  return(ptr);
}

gboolean
ags_wasapi_devin_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_wasapi_devin_is_ready(AgsConnectable *connectable)
{
  AgsWasapiDevin *wasapi_devin;
  
  gboolean is_ready;

  GRecMutex *wasapi_devin_mutex;

  wasapi_devin = AGS_WASAPI_DEVIN(connectable);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* check is ready */
  g_rec_mutex_lock(wasapi_devin_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (wasapi_devin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(wasapi_devin_mutex);
  
  return(is_ready);
}

void
ags_wasapi_devin_add_to_registry(AgsConnectable *connectable)
{
  AgsWasapiDevin *wasapi_devin;

  GRecMutex *wasapi_devin_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  wasapi_devin = AGS_WASAPI_DEVIN(connectable);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  g_rec_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(wasapi_devin_mutex);
}

void
ags_wasapi_devin_remove_from_registry(AgsConnectable *connectable)
{
  AgsWasapiDevin *wasapi_devin;

  GRecMutex *wasapi_devin_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  wasapi_devin = AGS_WASAPI_DEVIN(connectable);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  g_rec_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(wasapi_devin_mutex);
}

xmlNode*
ags_wasapi_devin_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_wasapi_devin_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_wasapi_devin_xml_parse(AgsConnectable *connectable,
			      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_wasapi_devin_is_connected(AgsConnectable *connectable)
{
  AgsWasapiDevin *wasapi_devin;
  
  gboolean is_connected;

  GRecMutex *wasapi_devin_mutex;

  wasapi_devin = AGS_WASAPI_DEVIN(connectable);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* check is connected */
  g_rec_mutex_lock(wasapi_devin_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (wasapi_devin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(wasapi_devin_mutex);
  
  return(is_connected);
}

void
ags_wasapi_devin_connect(AgsConnectable *connectable)
{
  AgsWasapiDevin *wasapi_devin;

  GRecMutex *wasapi_devin_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  wasapi_devin = AGS_WASAPI_DEVIN(connectable);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  g_rec_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(wasapi_devin_mutex);
}

void
ags_wasapi_devin_disconnect(AgsConnectable *connectable)
{
  AgsWasapiDevin *wasapi_devin;

  GRecMutex *wasapi_devin_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  wasapi_devin = AGS_WASAPI_DEVIN(connectable);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  g_rec_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(wasapi_devin_mutex);
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
 * Since: 3.0.0
 */
gboolean
ags_wasapi_devin_test_flags(AgsWasapiDevin *wasapi_devin, guint flags)
{
  gboolean retval;  
  
  GRecMutex *wasapi_devin_mutex;

  if(!AGS_IS_WASAPI_DEVIN(wasapi_devin)){
    return(FALSE);
  }

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* test */
  g_rec_mutex_lock(wasapi_devin_mutex);

  retval = (flags & (wasapi_devin->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(wasapi_devin_mutex);

  return(retval);
}

/**
 * ags_wasapi_devin_set_flags:
 * @wasapi_devin: the #AgsWasapiDevin
 * @flags: see #AgsWasapiDevinFlags-enum
 *
 * Enable a feature of @wasapi_devin.
 *
 * Since: 3.0.0
 */
void
ags_wasapi_devin_set_flags(AgsWasapiDevin *wasapi_devin, guint flags)
{
  GRecMutex *wasapi_devin_mutex;

  if(!AGS_IS_WASAPI_DEVIN(wasapi_devin)){
    return;
  }

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->flags |= flags;
  
  g_rec_mutex_unlock(wasapi_devin_mutex);
}
    
/**
 * ags_wasapi_devin_unset_flags:
 * @wasapi_devin: the #AgsWasapiDevin
 * @flags: see #AgsWasapiDevinFlags-enum
 *
 * Disable a feature of @wasapi_devin.
 *
 * Since: 3.0.0
 */
void
ags_wasapi_devin_unset_flags(AgsWasapiDevin *wasapi_devin, guint flags)
{  
  GRecMutex *wasapi_devin_mutex;

  if(!AGS_IS_WASAPI_DEVIN(wasapi_devin)){
    return;
  }

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->flags &= (~flags);
  
  g_rec_mutex_unlock(wasapi_devin_mutex);
}

void
ags_wasapi_devin_set_device(AgsSoundcard *soundcard,
			    gchar *device)
{
  AgsWasapiDevin *wasapi_devin;

  GList *card_id, *card_id_start, *card_name, *card_name_start;
  
  GRecMutex *wasapi_devin_mutex;
  
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
  g_rec_mutex_lock(wasapi_devin_mutex);

  while(card_id != NULL){
    if(!g_ascii_strncasecmp(card_id->data,
			    device,
			    strlen(card_id->data))){
      wasapi_devin->device = g_strdup(device);

      break;
    }
    
    card_id = card_id->next;
  }

  g_rec_mutex_unlock(wasapi_devin_mutex);

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

  GRecMutex *wasapi_devin_mutex;

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);
  
  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  device = NULL;

  /* get device */
  g_rec_mutex_lock(wasapi_devin_mutex);

  device = g_strdup(wasapi_devin->device);

  g_rec_mutex_unlock(wasapi_devin_mutex);
  
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

  GRecMutex *wasapi_devin_mutex;

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);
  
  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get presets */
  g_rec_mutex_lock(wasapi_devin_mutex);

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

  g_rec_mutex_unlock(wasapi_devin_mutex);
}

/**
 * ags_wasapi_devin_list_cards:
 * @soundcard: the #AgsSoundcard
 * @card_id: WASAPI identifier
 * @card_name: card name
 *
 * List available soundcards.
 *
 * Since: 3.0.0
 */
void
ags_wasapi_devin_list_cards(AgsSoundcard *soundcard,
			    GList **card_id, GList **card_name)
{
  AgsWasapiDevin *wasapi_devin;

  AgsApplicationContext *application_context;

#ifdef AGS_WITH_WASAPI
  IMMDeviceEnumerator *dev_enumerator;
  IMMDeviceCollection *dev_collection;

  UINT i, i_stop;
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

  CoCreateInstance(&ags_wasapi_clsid_mm_device_enumerator_guid, 0, CLSCTX_ALL, &ags_wasapi_iid_mm_device_enumerator_guid, &dev_enumerator);

  dev_enumerator->lpVtbl->EnumAudioEndpoints(dev_enumerator, eCapture, DEVICE_STATE_ACTIVE, &dev_collection);

  dev_collection->lpVtbl->GetCount(dev_collection,
				   &i_stop);
  
  for(i = 0; i < i_stop; i++){
    IMMDevice *device;

    IPropertyStore *prop_store;

    LPWSTR dev_id = NULL;
    PROPVARIANT var_name;
    gchar *str;
    
    dev_collection->lpVtbl->Item(dev_collection, i, &device);

    device->lpVtbl->GetId(device, &dev_id);

    device->lpVtbl->OpenPropertyStore(device, STGM_READ, &prop_store);

    PropVariantInit(&var_name);

    prop_store->lpVtbl->GetValue(prop_store, &ags_wasapi_pkey_device_friendly_name_key, &var_name);

    g_message("%S %S", dev_id, var_name.pwszVal);
    
    if(card_id != NULL){
      str = g_strdup_printf("%S", dev_id);

      *card_id = g_list_prepend(*card_id,
				str);
    }

    if(card_name != NULL){
      str = g_strdup_printf("%S", var_name.pwszVal);
	      
      *card_name = g_list_prepend(*card_name,
				  str);
    }
    
    CoTaskMemFree(dev_id);

    PropVariantClear(&var_name);

    prop_store->lpVtbl->Release(prop_store);

    device->lpVtbl->Release(device);
  }
    
  dev_collection->lpVtbl->Release(dev_collection);

  dev_enumerator->lpVtbl->Release(dev_enumerator);

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
  
  GRecMutex *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* check is starting */
  g_rec_mutex_lock(wasapi_devin_mutex);

  is_starting = ((AGS_WASAPI_DEVIN_START_RECORD & (wasapi_devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(wasapi_devin_mutex);
  
  return(is_starting);
}

gboolean
ags_wasapi_devin_is_recording(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  gboolean is_playing;
  
  GRecMutex *wasapi_devin_mutex;

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);
  
  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* check is starting */
  g_rec_mutex_lock(wasapi_devin_mutex);

  is_playing = ((AGS_WASAPI_DEVIN_RECORD & (wasapi_devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(wasapi_devin_mutex);

  return(is_playing);
}

gchar*
ags_wasapi_devin_get_uptime(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  gchar *uptime;

  GRecMutex *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);
  
  /* get core audio devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get time string */
  g_rec_mutex_lock(wasapi_devin_mutex);
  
  uptime = ags_frame_clock_to_time_string(wasapi_devin->frame_clock);

  g_rec_mutex_unlock(wasapi_devin_mutex);
  
  return(uptime);
}
  
void
ags_wasapi_devin_client_init(AgsSoundcard *soundcard,
			     GError **error)
{
  AgsWasapiDevin *wasapi_devin;

  GRecMutex *wasapi_devin_mutex; 

  if(ags_soundcard_is_recording(soundcard)){
    return;
  }

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  g_rec_mutex_lock(wasapi_devin_mutex);

#ifdef AGS_WITH_WASAPI
  wasapi_devin->flags |= AGS_WASAPI_DEVIN_INITIALIZED;
#endif
  
  ags_frame_clock_start(wasapi_devin->frame_clock);

  ags_frame_clock_set_note_offset(wasapi_devin->frame_clock,
				  wasapi_devin->start_note_offset);
  
  wasapi_devin->app_buffer_mode = AGS_WASAPI_DEVIN_APP_BUFFER_7;
  wasapi_devin->flags |= (AGS_WASAPI_DEVIN_START_RECORD |
			  AGS_WASAPI_DEVIN_RECORD |
			  AGS_WASAPI_DEVIN_NONBLOCKING);
  
  g_rec_mutex_unlock(wasapi_devin_mutex);
}

void
ags_wasapi_devin_client_record(AgsSoundcard *soundcard,
			       GError **error)
{
  AgsWasapiDevin *wasapi_devin;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsTaskLauncher *task_launcher;

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
  
  GRecMutex *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  application_context = ags_application_context_get_instance();

  /* get wasapi_devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* lock */
  g_rec_mutex_lock(wasapi_devin_mutex);
  
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
    g_rec_mutex_unlock(wasapi_devin_mutex);
    
    g_warning("ags_wasapi_devin_client_record(): unsupported word size");

    return;
  }

#ifdef AGS_WITH_WASAPI
  if((AGS_WASAPI_DEVIN_START_RECORD & (wasapi_devin->flags)) != 0){
    IMMDeviceEnumerator *dev_enumerator;
    IMMDevice *mm_device;

    WAVEFORMATEXTENSIBLE wave_format;
    WAVEFORMATEX *desired_format, *internal_format;

    gunichar2 *dev_id;

    REFERENCE_TIME min_duration;

    register HRESULT hr;

    unsigned char bit_resolution;

    g_message("WASAPI initialize");    

    dev_id = NULL;
    
    CoInitialize(0);
  
    if(CoCreateInstance(&ags_wasapi_clsid_mm_device_enumerator_guid, 0, CLSCTX_ALL, &ags_wasapi_iid_mm_device_enumerator_guid, (void **) &dev_enumerator)){
      if(error != NULL){
	g_set_error(error,
		    AGS_WASAPI_DEVIN_ERROR,
		    AGS_WASAPI_DEVIN_ERROR_LOCKED_SOUNDCARD,
		    "unable to open pcm device: Can't get IMMDeviceEnumerator.");
      }
    
      CoUninitialize();

      g_rec_mutex_unlock(wasapi_devin_mutex);

      return;
    }

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

	g_rec_mutex_unlock(wasapi_devin_mutex);

	return;
      }
    }else{
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
	
	g_rec_mutex_unlock(wasapi_devin_mutex);

	g_message("WASAPI failed - get device");
    
	return;
      }
    }

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

      g_rec_mutex_unlock(wasapi_devin_mutex);

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
      
	g_rec_mutex_unlock(wasapi_devin_mutex);

	g_message("WASAPI failed - broken configuration");
    
	return;
      }
  
      audio_client->lpVtbl->GetDevicePeriod(audio_client, NULL, &min_duration);

      min_duration = (AGS_NSEC_PER_SEC / 100) / wasapi_devin->samplerate * wasapi_devin->wasapi_buffer_size;

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
      
	g_rec_mutex_unlock(wasapi_devin_mutex);

	g_message("WASAPI failed - broken configuration");
    
	return;
      }

      desired_format = internal_format;

      if((hr = audio_client->lpVtbl->IsFormatSupported(audio_client, AUDCLNT_SHAREMODE_SHARED, desired_format, &desired_format))){
	goto ags_wasapi_devin_client_init_SHARED_BROKEN_CONFIGURATION;
      }

      desired_format = internal_format;
    
      audio_client->lpVtbl->GetDevicePeriod(audio_client, NULL, &min_duration);

      min_duration = (AGS_NSEC_PER_SEC / 100) / wasapi_devin->samplerate * wasapi_devin->buffer_size;

      min_duration = (AGS_NSEC_PER_SEC / 100) / wasapi_devin->samplerate * wasapi_devin->wasapi_buffer_size;
    
      if((hr = audio_client->lpVtbl->Initialize(audio_client, AUDCLNT_SHAREMODE_SHARED, 0, min_duration, 0, desired_format, NULL))){
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
    g_rec_mutex_unlock(wasapi_devin_mutex);
    
    return;
  }

  /* check buffer flag */
  if((AGS_WASAPI_DEVIN_SHUTDOWN & (wasapi_devin->flags)) == 0){
    /* check buffer flag */
    nth_buffer = 0;

    if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_0){
      nth_buffer = 0;
    }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_1){
      nth_buffer = 1;
    }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_2){
      nth_buffer = 2;
    }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_3){
      nth_buffer = 3;
    }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_4){
      nth_buffer = 4;
    }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_5){
      nth_buffer = 5;
    }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_6){
      nth_buffer = 6;
    }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_7){
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
      g_rec_mutex_unlock(wasapi_devin_mutex);

      g_message("audio_capture_client = NULL");
      
      return;
    }

    {
      UINT32 padding_frames;

      static const struct timespec poll_delay = {
	0,
	400,
      };

      audio_client->lpVtbl->GetCurrentPadding(audio_client, &padding_frames);
      
      while(buffer_frame_count - padding_frames < wasapi_devin->buffer_size &&
	    padding_frames != 0){
	ags_time_nanosleep(&poll_delay);

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
	ags_audio_buffer_util_copy_s16_to_s16(wasapi_devin->app_buffer[nth_buffer], 1,
					      data, 1,
					      wasapi_devin->pcm_channels * wasapi_devin->buffer_size);
      }
      break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	ags_audio_buffer_util_copy_s24_to_s24(wasapi_devin->app_buffer[nth_buffer], 1,
					      data, 1,
					      wasapi_devin->pcm_channels * wasapi_devin->buffer_size);
      }
      break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	ags_audio_buffer_util_copy_s32_to_s32(wasapi_devin->app_buffer[nth_buffer], 1,
					      data, 1,
					      wasapi_devin->pcm_channels * wasapi_devin->buffer_size);
      }
      break;
      }
    }else{
      g_message("data = NULL");
    }

    audio_capture_client->lpVtbl->ReleaseBuffer(audio_capture_client, wasapi_devin->buffer_size);

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

    soundcard_thread = (AgsThread *) ags_soundcard_thread_find_soundcard((AgsSoundcardThread *) soundcard_thread,
									 G_OBJECT(soundcard));

    ags_thread_stop(soundcard_thread);
  }
  
  g_rec_mutex_unlock(wasapi_devin_mutex);

  if((AGS_WASAPI_DEVIN_SHUTDOWN & (wasapi_devin->flags)) == 0){
    /* update soundcard */
    task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

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
    ags_task_launcher_add_task_all(task_launcher,
				   task);
  
    /* unref */
    g_object_unref(task_launcher);
  }
}

void
ags_wasapi_devin_client_free(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  AgsApplicationContext *application_context;

  guint i;
  
  GRecMutex *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* lock */
  g_rec_mutex_lock(wasapi_devin_mutex);

  if((AGS_WASAPI_DEVIN_INITIALIZED & (wasapi_devin->flags)) == 0){
    g_rec_mutex_unlock(wasapi_devin_mutex);
    
    return;
  }

  wasapi_devin->flags |= (AGS_WASAPI_DEVIN_SHUTDOWN);

  g_rec_mutex_unlock(wasapi_devin_mutex);

  g_rec_mutex_lock(wasapi_devin_mutex);

  /*  */
  ags_frame_clock_stop(wasapi_devin->frame_clock);

  g_rec_mutex_unlock(wasapi_devin_mutex);
}

void
ags_wasapi_devin_tic(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  GRecMutex *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);
  
  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* frame clock */
  g_rec_mutex_lock(wasapi_devin_mutex);
  
  ags_frame_clock_increment_counter(wasapi_devin->frame_clock);

  /* 16th pulse */
  if(ags_frame_clock_get_has_16th_pulse(wasapi_devin->frame_clock)){
    ags_soundcard_offset_changed(soundcard,
				 wasapi_devin->frame_clock->note_offset);
  }
  
  g_rec_mutex_unlock(wasapi_devin_mutex);
}

void
ags_wasapi_devin_offset_changed(AgsSoundcard *soundcard,
				guint64 note_offset)
{
  //empty
}

void
ags_wasapi_devin_set_bpm(AgsSoundcard *soundcard,
			 gdouble bpm)
{
  AgsWasapiDevin *wasapi_devin;

  GRecMutex *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* set bpm */
  g_rec_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->bpm = bpm;

  ags_frame_clock_set_bpm(wasapi_devin->frame_clock,
			  bpm);

  g_rec_mutex_unlock(wasapi_devin_mutex);
}

gdouble
ags_wasapi_devin_get_bpm(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  gdouble bpm;
  
  GRecMutex *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get bpm */
  g_rec_mutex_lock(wasapi_devin_mutex);

  bpm = wasapi_devin->bpm;
  
  g_rec_mutex_unlock(wasapi_devin_mutex);

  return(bpm);
}

guint64
ags_wasapi_devin_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  guint64 start_note_offset;
  
  GRecMutex *wasapi_devin_mutex;  

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* set note offset */
  g_rec_mutex_lock(wasapi_devin_mutex);

  start_note_offset = wasapi_devin->start_note_offset;

  g_rec_mutex_unlock(wasapi_devin_mutex);

  return(start_note_offset);
}

void
ags_wasapi_devin_set_start_note_offset(AgsSoundcard *soundcard,
				       guint64 start_note_offset)
{
  AgsWasapiDevin *wasapi_devin;

  GRecMutex *wasapi_devin_mutex;  

  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* set note offset */
  g_rec_mutex_lock(wasapi_devin_mutex);

  wasapi_devin->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(wasapi_devin_mutex);
}

GObject*
ags_wasapi_devin_get_frame_clock(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  GObject *frame_clock;
  
  GRecMutex *wasapi_devin_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get core audio devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get frame clock */
  g_rec_mutex_lock(wasapi_devin_mutex);

  frame_clock = (GObject *) wasapi_devin->frame_clock;
  
  g_rec_mutex_unlock(wasapi_devin_mutex);

  return(frame_clock);
}
void*
ags_wasapi_devin_get_buffer(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  void *buffer;

  GRecMutex *wasapi_devin_mutex;  
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);
  
  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  g_rec_mutex_lock(wasapi_devin_mutex);

  if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_0){
    buffer = wasapi_devin->app_buffer[0];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_1){
    buffer = wasapi_devin->app_buffer[1];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_2){
    buffer = wasapi_devin->app_buffer[2];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_3){
    buffer = wasapi_devin->app_buffer[3];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_4){
    buffer = wasapi_devin->app_buffer[4];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_5){
    buffer = wasapi_devin->app_buffer[5];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_6){
    buffer = wasapi_devin->app_buffer[6];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_7){
    buffer = wasapi_devin->app_buffer[7];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(wasapi_devin_mutex);

  return(buffer);
}

void*
ags_wasapi_devin_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  void *buffer;

  GRecMutex *wasapi_devin_mutex;  
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);
  
  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  g_rec_mutex_lock(wasapi_devin_mutex);

  if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_0){
    buffer = wasapi_devin->app_buffer[1];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_1){
    buffer = wasapi_devin->app_buffer[2];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_2){
    buffer = wasapi_devin->app_buffer[3];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_3){
    buffer = wasapi_devin->app_buffer[4];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_4){
    buffer = wasapi_devin->app_buffer[5];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_5){
    buffer = wasapi_devin->app_buffer[6];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_6){
    buffer = wasapi_devin->app_buffer[7];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_7){
    buffer = wasapi_devin->app_buffer[0];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(wasapi_devin_mutex);

  return(buffer);
}

void*
ags_wasapi_devin_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsWasapiDevin *wasapi_devin;

  void *buffer;

  GRecMutex *wasapi_devin_mutex;  
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  g_rec_mutex_lock(wasapi_devin_mutex);

  if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_0){
    buffer = wasapi_devin->app_buffer[7];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_1){
    buffer = wasapi_devin->app_buffer[0];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_2){
    buffer = wasapi_devin->app_buffer[1];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_3){
    buffer = wasapi_devin->app_buffer[2];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_4){
    buffer = wasapi_devin->app_buffer[3];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_5){
    buffer = wasapi_devin->app_buffer[4];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_6){
    buffer = wasapi_devin->app_buffer[5];
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_7){
    buffer = wasapi_devin->app_buffer[6];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(wasapi_devin_mutex);

  return(buffer);
}

void
ags_wasapi_devin_lock_buffer(AgsSoundcard *soundcard,
			     void *buffer)
{
  AgsWasapiDevin *wasapi_devin;

  GRecMutex *buffer_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(wasapi_devin->app_buffer != NULL){
    if(buffer == wasapi_devin->app_buffer[0]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[0];
    }else if(buffer == wasapi_devin->app_buffer[1]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[1];
    }else if(buffer == wasapi_devin->app_buffer[2]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[2];
    }else if(buffer == wasapi_devin->app_buffer[3]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[3];
    }else if(buffer == wasapi_devin->app_buffer[4]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[4];
    }else if(buffer == wasapi_devin->app_buffer[5]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[5];
    }else if(buffer == wasapi_devin->app_buffer[6]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[6];
    }else if(buffer == wasapi_devin->app_buffer[7]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[7];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_wasapi_devin_unlock_buffer(AgsSoundcard *soundcard,
			       void *buffer)
{
  AgsWasapiDevin *wasapi_devin;

  GRecMutex *buffer_mutex;
  
  wasapi_devin = AGS_WASAPI_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(wasapi_devin->app_buffer != NULL){
    if(buffer == wasapi_devin->app_buffer[0]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[0];
    }else if(buffer == wasapi_devin->app_buffer[1]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[1];
    }else if(buffer == wasapi_devin->app_buffer[2]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[2];
    }else if(buffer == wasapi_devin->app_buffer[3]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[3];
    }else if(buffer == wasapi_devin->app_buffer[4]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[4];
    }else if(buffer == wasapi_devin->app_buffer[5]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[5];
    }else if(buffer == wasapi_devin->app_buffer[6]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[6];
    }else if(buffer == wasapi_devin->app_buffer[7]){
      buffer_mutex = wasapi_devin->app_buffer_mutex[7];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

/**
 * ags_wasapi_devin_switch_buffer_flag:
 * @wasapi_devin: an #AgsWasapiDevin
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.0.0
 */
void
ags_wasapi_devin_switch_buffer_flag(AgsWasapiDevin *wasapi_devin)
{
  GRecMutex *wasapi_devin_mutex;
  
  if(!AGS_IS_WASAPI_DEVIN(wasapi_devin)){
    return;
  }

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* switch buffer flag */
  g_rec_mutex_lock(wasapi_devin_mutex);

  if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_0){
    wasapi_devin->app_buffer_mode = AGS_WASAPI_DEVIN_APP_BUFFER_1;
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_1){
    wasapi_devin->app_buffer_mode = AGS_WASAPI_DEVIN_APP_BUFFER_2;
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_2){
    wasapi_devin->app_buffer_mode = AGS_WASAPI_DEVIN_APP_BUFFER_3;
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_3){
    wasapi_devin->app_buffer_mode = AGS_WASAPI_DEVIN_APP_BUFFER_4;
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_4){
    wasapi_devin->app_buffer_mode = AGS_WASAPI_DEVIN_APP_BUFFER_5;
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_5){
    wasapi_devin->app_buffer_mode = AGS_WASAPI_DEVIN_APP_BUFFER_6;
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_6){
    wasapi_devin->app_buffer_mode = AGS_WASAPI_DEVIN_APP_BUFFER_7;
  }else if(wasapi_devin->app_buffer_mode == AGS_WASAPI_DEVIN_APP_BUFFER_7){
    wasapi_devin->app_buffer_mode = AGS_WASAPI_DEVIN_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(wasapi_devin_mutex);
}

/**
 * ags_wasapi_devin_realloc_buffer:
 * @wasapi_devin: the #AgsWasapiDevin
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.0.0
 */
void
ags_wasapi_devin_realloc_buffer(AgsWasapiDevin *wasapi_devin)
{
  guint pcm_channels;
  guint buffer_size;
  guint format;
  guint word_size;

  GRecMutex *wasapi_devin_mutex;  

  if(!AGS_IS_WASAPI_DEVIN(wasapi_devin)){
    return;
  }

  /* get wasapi devin mutex */
  wasapi_devin_mutex = AGS_WASAPI_DEVIN_GET_OBJ_MUTEX(wasapi_devin);

  /* get word size */  
  g_rec_mutex_lock(wasapi_devin_mutex);

  pcm_channels = wasapi_devin->pcm_channels;
  buffer_size = wasapi_devin->buffer_size;

  format = wasapi_devin->format;
  
  g_rec_mutex_unlock(wasapi_devin_mutex);

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
  
  /* AGS_WASAPI_DEVIN_APP_BUFFER_0 */
  if(wasapi_devin->app_buffer[0] != NULL){
    g_free(wasapi_devin->app_buffer[0]);
  }
  
  wasapi_devin->app_buffer[0] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_WASAPI_DEVIN_APP_BUFFER_1 */
  if(wasapi_devin->app_buffer[1] != NULL){
    g_free(wasapi_devin->app_buffer[1]);
  }

  wasapi_devin->app_buffer[1] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_WASAPI_DEVIN_APP_BUFFER_2 */
  if(wasapi_devin->app_buffer[2] != NULL){
    g_free(wasapi_devin->app_buffer[2]);
  }

  wasapi_devin->app_buffer[2] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_WASAPI_DEVIN_APP_BUFFER_3 */
  if(wasapi_devin->app_buffer[3] != NULL){
    g_free(wasapi_devin->app_buffer[3]);
  }
  
  wasapi_devin->app_buffer[3] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  /* AGS_WASAPI_DEVIN_APP_BUFFER_4 */
  if(wasapi_devin->app_buffer[4] != NULL){
    g_free(wasapi_devin->app_buffer[4]);
  }
  
  wasapi_devin->app_buffer[4] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  /* AGS_WASAPI_DEVIN_APP_BUFFER_5 */
  if(wasapi_devin->app_buffer[5] != NULL){
    g_free(wasapi_devin->app_buffer[5]);
  }
  
  wasapi_devin->app_buffer[5] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  /* AGS_WASAPI_DEVIN_APP_BUFFER_6 */
  if(wasapi_devin->app_buffer[6] != NULL){
    g_free(wasapi_devin->app_buffer[6]);
  }
  
  wasapi_devin->app_buffer[6] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  /* AGS_WASAPI_DEVIN_APP_BUFFER_7 */
  if(wasapi_devin->app_buffer[7] != NULL){
    g_free(wasapi_devin->app_buffer[7]);
  }
  
  wasapi_devin->app_buffer[7] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_wasapi_devin_new:
 *
 * Creates a new instance of #AgsWasapiDevin.
 *
 * Returns: a new #AgsWasapiDevin
 *
 * Since: 3.0.0
 */
AgsWasapiDevin*
ags_wasapi_devin_new()
{
  AgsWasapiDevin *wasapi_devin;

  wasapi_devin = (AgsWasapiDevin *) g_object_new(AGS_TYPE_WASAPI_DEVIN,
						 NULL);
  
  return(wasapi_devin);
}
