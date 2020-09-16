/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/wasapi/ags_wasapi_devout.h>

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

void ags_wasapi_devout_class_init(AgsWasapiDevoutClass *wasapi_devout);
void ags_wasapi_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_wasapi_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_wasapi_devout_init(AgsWasapiDevout *wasapi_devout);
void ags_wasapi_devout_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_wasapi_devout_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_wasapi_devout_dispose(GObject *gobject);
void ags_wasapi_devout_finalize(GObject *gobject);

AgsUUID* ags_wasapi_devout_get_uuid(AgsConnectable *connectable);
gboolean ags_wasapi_devout_has_resource(AgsConnectable *connectable);
gboolean ags_wasapi_devout_is_ready(AgsConnectable *connectable);
void ags_wasapi_devout_add_to_registry(AgsConnectable *connectable);
void ags_wasapi_devout_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_wasapi_devout_list_resource(AgsConnectable *connectable);
xmlNode* ags_wasapi_devout_xml_compose(AgsConnectable *connectable);
void ags_wasapi_devout_xml_parse(AgsConnectable *connectable,
				 xmlNode *node);
gboolean ags_wasapi_devout_is_connected(AgsConnectable *connectable);
void ags_wasapi_devout_connect(AgsConnectable *connectable);
void ags_wasapi_devout_disconnect(AgsConnectable *connectable);

void ags_wasapi_devout_set_device(AgsSoundcard *soundcard,
				  gchar *device);
gchar* ags_wasapi_devout_get_device(AgsSoundcard *soundcard);

void ags_wasapi_devout_set_presets(AgsSoundcard *soundcard,
				   guint channels,
				   guint rate,
				   guint buffer_size,
				   guint format);
void ags_wasapi_devout_get_presets(AgsSoundcard *soundcard,
				   guint *channels,
				   guint *rate,
				   guint *buffer_size,
				   guint *format);

void ags_wasapi_devout_list_cards(AgsSoundcard *soundcard,
				  GList **card_id, GList **card_name);
void ags_wasapi_devout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
				guint *channels_min, guint *channels_max,
				guint *rate_min, guint *rate_max,
				guint *buffer_size_min, guint *buffer_size_max,
				GError **error);
guint ags_wasapi_devout_get_capability(AgsSoundcard *soundcard);

gboolean ags_wasapi_devout_is_starting(AgsSoundcard *soundcard);
gboolean ags_wasapi_devout_is_playing(AgsSoundcard *soundcard);

gchar* ags_wasapi_devout_get_uptime(AgsSoundcard *soundcard);

void ags_wasapi_devout_client_init(AgsSoundcard *soundcard,
				   GError **error);
void ags_wasapi_devout_client_play(AgsSoundcard *soundcard,
				   GError **error);
void ags_wasapi_devout_client_free(AgsSoundcard *soundcard);

void ags_wasapi_devout_tic(AgsSoundcard *soundcard);
void ags_wasapi_devout_offset_changed(AgsSoundcard *soundcard,
				      guint note_offset);

void ags_wasapi_devout_set_bpm(AgsSoundcard *soundcard,
			       gdouble bpm);
gdouble ags_wasapi_devout_get_bpm(AgsSoundcard *soundcard);

void ags_wasapi_devout_set_delay_factor(AgsSoundcard *soundcard,
					gdouble delay_factor);
gdouble ags_wasapi_devout_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_wasapi_devout_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_wasapi_devout_get_delay(AgsSoundcard *soundcard);
guint ags_wasapi_devout_get_attack(AgsSoundcard *soundcard);

void* ags_wasapi_devout_get_buffer(AgsSoundcard *soundcard);
void* ags_wasapi_devout_get_next_buffer(AgsSoundcard *soundcard);
void* ags_wasapi_devout_get_prev_buffer(AgsSoundcard *soundcard);

void ags_wasapi_devout_lock_buffer(AgsSoundcard *soundcard,
				   void *buffer);
void ags_wasapi_devout_unlock_buffer(AgsSoundcard *soundcard,
				     void *buffer);

guint ags_wasapi_devout_get_delay_counter(AgsSoundcard *soundcard);

void ags_wasapi_devout_set_start_note_offset(AgsSoundcard *soundcard,
					     guint start_note_offset);
guint ags_wasapi_devout_get_start_note_offset(AgsSoundcard *soundcard);

void ags_wasapi_devout_set_note_offset(AgsSoundcard *soundcard,
				       guint note_offset);
guint ags_wasapi_devout_get_note_offset(AgsSoundcard *soundcard);

void ags_wasapi_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
						guint note_offset);
guint ags_wasapi_devout_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_wasapi_devout_set_loop(AgsSoundcard *soundcard,
				guint loop_left, guint loop_right,
				gboolean do_loop);
void ags_wasapi_devout_get_loop(AgsSoundcard *soundcard,
				guint *loop_left, guint *loop_right,
				gboolean *do_loop);

guint ags_wasapi_devout_get_loop_offset(AgsSoundcard *soundcard);

guint ags_wasapi_devout_get_sub_block_count(AgsSoundcard *soundcard);

gboolean ags_wasapi_devout_trylock_sub_block(AgsSoundcard *soundcard,
					     void *buffer, guint sub_block);
void ags_wasapi_devout_unlock_sub_block(AgsSoundcard *soundcard,
					void *buffer, guint sub_block);

/**
 * SECTION:ags_wasapi_devout
 * @short_description: Output to soundcard
 * @title: AgsWasapiDevout
 * @section_id:
 * @include: ags/audio/wasapi/ags_wasapi_devout.h
 *
 * #AgsWasapiDevout represents a soundcard and supports output.
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
  PROP_CHANNEL,
};

static gpointer ags_wasapi_devout_parent_class = NULL;

#ifdef AGS_W32API
static const GUID ags_wasapi_clsid_mm_device_enumerator_guid = {0xBCDE0395, 0xE52F, 0x467C, 0x8E, 0x3D, 0xC4, 0x57, 0x92, 0x91, 0x69, 0x2E};
static const GUID ags_wasapi_iid_mm_device_enumerator_guid = {0xA95664D2, 0x9614, 0x4F35, 0xA7, 0x46, 0xDE, 0x8D, 0xB6, 0x36, 0x17, 0xE6};
static const GUID ags_wasapi_iid_audio_client_guid = {0x1CB9AD4C, 0xDBFA, 0x4c32, 0xB1, 0x78, 0xC2, 0xF5, 0x68, 0xA7, 0x03, 0xB2};
static const GUID ags_wasapi_iid_audio_render_client_guid = {0xF294ACFC, 0x3146, 0x4483, 0xA7, 0xBF, 0xAD, 0xDC, 0xA7, 0xC2, 0x60, 0xE2};
static const GUID ags_wasapi_pcm_subformat_guid = {0x00000001, 0x0000, 0x0010, 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71};
static const GUID ags_wasapi_pkey_device_friendly_name_guid = {0xa45c254e, 0xdf1c, 0x4efd, 0x80, 0x20, 0x67, 0xd1, 0x46, 0xa8, 0x50, 0xe0};

struct _PROPERTYKEY{
  GUID fmtid;
  DWORD id;
};

static struct _PROPERTYKEY ags_wasapi_pkey_device_friendly_name_key = {ags_wasapi_pkey_device_friendly_name_guid,
								       14};
#endif

GType
ags_wasapi_devout_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_wasapi_devout = 0;
 
    static const GTypeInfo ags_wasapi_devout_info = {
      sizeof(AgsWasapiDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_wasapi_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsWasapiDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_wasapi_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_wasapi_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_wasapi_devout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_wasapi_devout = g_type_register_static(G_TYPE_OBJECT,
						    "AgsWasapiDevout",
						    &ags_wasapi_devout_info,
						    0);

    g_type_add_interface_static(ags_type_wasapi_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_wasapi_devout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_wasapi_devout);
  }

  return g_define_type_id__volatile;
}

void
ags_wasapi_devout_class_init(AgsWasapiDevoutClass *wasapi_devout)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_wasapi_devout_parent_class = g_type_class_peek_parent(wasapi_devout);

  /* GObjectClass */
  gobject = (GObjectClass *) wasapi_devout;

  gobject->set_property = ags_wasapi_devout_set_property;
  gobject->get_property = ags_wasapi_devout_get_property;

  gobject->dispose = ags_wasapi_devout_dispose;
  gobject->finalize = ags_wasapi_devout_finalize;

  /* properties */
  /**
   * AgsWasapiDevout:device:
   *
   * The core audio soundcard indentifier
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-wasapi-devout-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsWasapiDevout:dsp-channels:
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
   * AgsWasapiDevout:pcm-channels:
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
   * AgsWasapiDevout:format:
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
   * AgsWasapiDevout:buffer-size:
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
   * AgsWasapiDevout:samplerate:
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
   * AgsWasapiDevout:buffer:
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

  /**
   * AgsWasapiDevout:bpm:
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
   * AgsWasapiDevout:delay-factor:
   *
   * tact
   * 
   * Since: 3.0.0
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
   * AgsWasapiDevout:attack:
   *
   * Attack of the buffer
   * 
   * Since: 3.0.0
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
ags_wasapi_devout_error_quark()
{
  return(g_quark_from_static_string("ags-wasapi_devout-error-quark"));
}

void
ags_wasapi_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_wasapi_devout_get_uuid;
  connectable->has_resource = ags_wasapi_devout_has_resource;

  connectable->is_ready = ags_wasapi_devout_is_ready;
  connectable->add_to_registry = ags_wasapi_devout_add_to_registry;
  connectable->remove_from_registry = ags_wasapi_devout_remove_from_registry;

  connectable->list_resource = ags_wasapi_devout_list_resource;
  connectable->xml_compose = ags_wasapi_devout_xml_compose;
  connectable->xml_parse = ags_wasapi_devout_xml_parse;

  connectable->is_connected = ags_wasapi_devout_is_connected;  
  connectable->connect = ags_wasapi_devout_connect;
  connectable->disconnect = ags_wasapi_devout_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_wasapi_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_wasapi_devout_set_device;
  soundcard->get_device = ags_wasapi_devout_get_device;
  
  soundcard->set_presets = ags_wasapi_devout_set_presets;
  soundcard->get_presets = ags_wasapi_devout_get_presets;

  soundcard->list_cards = ags_wasapi_devout_list_cards;
  soundcard->pcm_info = ags_wasapi_devout_pcm_info;
  soundcard->get_capability = ags_wasapi_devout_get_capability;

  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_wasapi_devout_is_starting;
  soundcard->is_playing = ags_wasapi_devout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_wasapi_devout_get_uptime;
  
  soundcard->play_init = ags_wasapi_devout_client_init;
  soundcard->play = ags_wasapi_devout_client_play;

  soundcard->record_init = NULL;
  soundcard->record = NULL;

  soundcard->stop = ags_wasapi_devout_client_free;

  soundcard->tic = ags_wasapi_devout_tic;
  soundcard->offset_changed = ags_wasapi_devout_offset_changed;
    
  soundcard->set_bpm = ags_wasapi_devout_set_bpm;
  soundcard->get_bpm = ags_wasapi_devout_get_bpm;

  soundcard->set_delay_factor = ags_wasapi_devout_set_delay_factor;
  soundcard->get_delay_factor = ags_wasapi_devout_get_delay_factor;
  
  soundcard->get_absolute_delay = ags_wasapi_devout_get_absolute_delay;

  soundcard->get_delay = ags_wasapi_devout_get_delay;
  soundcard->get_attack = ags_wasapi_devout_get_attack;

  soundcard->get_buffer = ags_wasapi_devout_get_buffer;
  soundcard->get_next_buffer = ags_wasapi_devout_get_next_buffer;
  soundcard->get_prev_buffer = ags_wasapi_devout_get_prev_buffer;

  soundcard->lock_buffer = ags_wasapi_devout_lock_buffer;
  soundcard->unlock_buffer = ags_wasapi_devout_unlock_buffer;

  soundcard->get_delay_counter = ags_wasapi_devout_get_delay_counter;

  soundcard->set_start_note_offset = ags_wasapi_devout_set_start_note_offset;
  soundcard->get_start_note_offset = ags_wasapi_devout_get_start_note_offset;

  soundcard->set_note_offset = ags_wasapi_devout_set_note_offset;
  soundcard->get_note_offset = ags_wasapi_devout_get_note_offset;

  soundcard->set_note_offset_absolute = ags_wasapi_devout_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_wasapi_devout_get_note_offset_absolute;

  soundcard->set_loop = ags_wasapi_devout_set_loop;
  soundcard->get_loop = ags_wasapi_devout_get_loop;

  soundcard->get_loop_offset = ags_wasapi_devout_get_loop_offset;

  soundcard->get_sub_block_count = ags_wasapi_devout_get_sub_block_count;

  soundcard->trylock_sub_block = ags_wasapi_devout_trylock_sub_block;
  soundcard->unlock_sub_block = ags_wasapi_devout_unlock_sub_block;
}

void
ags_wasapi_devout_init(AgsWasapiDevout *wasapi_devout)
{
  AgsConfig *config;

  gchar *str;
  gchar *segmentation;

  guint denumerator, numerator;
  guint i;
  
  /* flags */
  wasapi_devout->flags = 0;

  /* devout mutex */
  g_rec_mutex_init(&(wasapi_devout->obj_mutex));

  /* uuid */
  wasapi_devout->uuid = ags_uuid_alloc();
  ags_uuid_generate(wasapi_devout->uuid);

  /* presets */
  config = ags_config_get_instance();

  wasapi_devout->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  wasapi_devout->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  wasapi_devout->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  wasapi_devout->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  wasapi_devout->format = ags_soundcard_helper_config_get_format(config);

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "wasapi-buffer-size");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "wasapi-buffer-size");
  }

  if(str != NULL){
    wasapi_devout->wasapi_buffer_size = g_ascii_strtoull(str,
							 NULL,
							 10);
  }else{
    wasapi_devout->wasapi_buffer_size = AGS_WASAPI_DEVOUT_DEFAULT_WASAPI_BUFFER_SIZE;
  }
  
  g_free(str);

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "wasapi-share-mode");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "wasapi-share-mode");
  }

  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "exclusive",
			  10)){
    wasapi_devout->flags |= AGS_WASAPI_DEVOUT_SHARE_MODE_EXCLUSIVE;
  }

  g_free(str);
  
  /* device */
  wasapi_devout->device = NULL;

  /* buffer */
  wasapi_devout->buffer_mutex = (GRecMutex **) malloc(8 * sizeof(GRecMutex *));

  for(i = 0; i < 8; i++){
    wasapi_devout->buffer_mutex[i] = (GRecMutex *) malloc(sizeof(GRecMutex));

    g_rec_mutex_init(wasapi_devout->buffer_mutex[i]);
  }

  wasapi_devout->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  wasapi_devout->sub_block_mutex = (GRecMutex **) malloc(8 * wasapi_devout->sub_block_count * wasapi_devout->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < 8 * wasapi_devout->sub_block_count * wasapi_devout->pcm_channels; i++){
    wasapi_devout->sub_block_mutex[i] = (GRecMutex *) malloc(sizeof(GRecMutex));

    g_rec_mutex_init(wasapi_devout->sub_block_mutex[i]);
  }

  wasapi_devout->buffer = (void **) malloc(8 * sizeof(void*));

  wasapi_devout->buffer[0] = NULL;
  wasapi_devout->buffer[1] = NULL;
  wasapi_devout->buffer[2] = NULL;
  wasapi_devout->buffer[3] = NULL;
  wasapi_devout->buffer[4] = NULL;
  wasapi_devout->buffer[5] = NULL;
  wasapi_devout->buffer[6] = NULL;
  wasapi_devout->buffer[7] = NULL;
  
  ags_wasapi_devout_realloc_buffer(wasapi_devout);
  
  /* bpm */
  wasapi_devout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  wasapi_devout->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denumerator,
	   &numerator);
    
    wasapi_devout->delay_factor = 1.0 / numerator * (numerator / denumerator);

    g_free(segmentation);
  }

  /* delay and attack */
  wasapi_devout->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					    sizeof(gdouble));
  
  wasapi_devout->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					   sizeof(guint));

  ags_wasapi_devout_adjust_delay_and_attack(wasapi_devout);
  
  /* counters */
  wasapi_devout->tact_counter = 0.0;
  wasapi_devout->delay_counter = 0.0;
  wasapi_devout->tic_counter = 0;

  wasapi_devout->start_note_offset = 0;
  wasapi_devout->note_offset = 0;
  wasapi_devout->note_offset_absolute = 0;

  wasapi_devout->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  wasapi_devout->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  wasapi_devout->do_loop = FALSE;

  wasapi_devout->loop_offset = 0;

  /* callback mutex */
  g_mutex_init(&(wasapi_devout->callback_mutex));

  g_cond_init(&(wasapi_devout->callback_cond));

  /* callback finish mutex */
  g_mutex_init(&(wasapi_devout->callback_finish_mutex));

  g_cond_init(&(wasapi_devout->callback_finish_cond));
}

void
ags_wasapi_devout_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsWasapiDevout *wasapi_devout;
  
  GRecMutex *wasapi_devout_mutex;

  wasapi_devout = AGS_WASAPI_DEVOUT(gobject);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    char *device;

    device = (char *) g_value_get_string(value);

    g_rec_mutex_lock(wasapi_devout_mutex);

    wasapi_devout->device = g_strdup(device);

    g_rec_mutex_unlock(wasapi_devout_mutex);
  }
  break;
  case PROP_DSP_CHANNELS:
  {
    guint dsp_channels;

    dsp_channels = g_value_get_uint(value);

    g_rec_mutex_lock(wasapi_devout_mutex);

    if(dsp_channels == wasapi_devout->dsp_channels){
      g_rec_mutex_unlock(wasapi_devout_mutex);

      return;
    }

    wasapi_devout->dsp_channels = dsp_channels;

    g_rec_mutex_unlock(wasapi_devout_mutex);
  }
  break;
  case PROP_PCM_CHANNELS:
  {
    guint pcm_channels, old_pcm_channels;
    guint i;

    pcm_channels = g_value_get_uint(value);

    g_rec_mutex_lock(wasapi_devout_mutex);

    if(pcm_channels == wasapi_devout->pcm_channels){
      g_rec_mutex_unlock(wasapi_devout_mutex);

      return;
    }

    old_pcm_channels = wasapi_devout->pcm_channels;

    /* destroy if less pcm-channels */
    for(i = 8 * wasapi_devout->sub_block_count * pcm_channels; i < 8 * wasapi_devout->sub_block_count * old_pcm_channels; i++){
      g_rec_mutex_clear(wasapi_devout->sub_block_mutex[i]);

      free(wasapi_devout->sub_block_mutex[i]);
    }

    wasapi_devout->sub_block_mutex = (GRecMutex **) realloc(wasapi_devout->sub_block_mutex,
								  8 * wasapi_devout->sub_block_count * pcm_channels * sizeof(GRecMutex *));

    /* create if more pcm-channels */
    for(i = 8 * wasapi_devout->sub_block_count * old_pcm_channels; i < 8 * wasapi_devout->sub_block_count * pcm_channels; i++){
      wasapi_devout->sub_block_mutex[i] = (GRecMutex *) malloc(sizeof(GRecMutex));

      g_rec_mutex_init(wasapi_devout->sub_block_mutex[i]);
    }

    wasapi_devout->pcm_channels = pcm_channels;

    g_rec_mutex_unlock(wasapi_devout_mutex);

    ags_wasapi_devout_realloc_buffer(wasapi_devout);
  }
  break;
  case PROP_FORMAT:
  {
    guint format;

    format = g_value_get_uint(value);

    g_rec_mutex_lock(wasapi_devout_mutex);

    if(format == wasapi_devout->format){
      g_rec_mutex_unlock(wasapi_devout_mutex);

      return;
    }

    wasapi_devout->format = format;

    g_rec_mutex_unlock(wasapi_devout_mutex);

    ags_wasapi_devout_realloc_buffer(wasapi_devout);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    guint buffer_size;

    buffer_size = g_value_get_uint(value);

    g_rec_mutex_lock(wasapi_devout_mutex);

    if(buffer_size == wasapi_devout->buffer_size){
      g_rec_mutex_unlock(wasapi_devout_mutex);

      return;
    }

    wasapi_devout->buffer_size = buffer_size;

    g_rec_mutex_unlock(wasapi_devout_mutex);

    ags_wasapi_devout_realloc_buffer(wasapi_devout);
    ags_wasapi_devout_adjust_delay_and_attack(wasapi_devout);
  }
  break;
  case PROP_SAMPLERATE:
  {
    guint samplerate;

    samplerate = g_value_get_uint(value);

    g_rec_mutex_lock(wasapi_devout_mutex);
      
    if(samplerate == wasapi_devout->samplerate){
      g_rec_mutex_unlock(wasapi_devout_mutex);

      return;
    }

    wasapi_devout->samplerate = samplerate;

    g_rec_mutex_unlock(wasapi_devout_mutex);

    ags_wasapi_devout_realloc_buffer(wasapi_devout);
    ags_wasapi_devout_adjust_delay_and_attack(wasapi_devout);
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

    g_rec_mutex_lock(wasapi_devout_mutex);

    wasapi_devout->bpm = bpm;

    g_rec_mutex_unlock(wasapi_devout_mutex);

    ags_wasapi_devout_adjust_delay_and_attack(wasapi_devout);
  }
  break;
  case PROP_DELAY_FACTOR:
  {
    gdouble delay_factor;
      
    delay_factor = g_value_get_double(value);

    g_rec_mutex_lock(wasapi_devout_mutex);

    wasapi_devout->delay_factor = delay_factor;

    g_rec_mutex_unlock(wasapi_devout_mutex);

    ags_wasapi_devout_adjust_delay_and_attack(wasapi_devout);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wasapi_devout_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsWasapiDevout *wasapi_devout;

  GRecMutex *wasapi_devout_mutex;

  wasapi_devout = AGS_WASAPI_DEVOUT(gobject);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    g_rec_mutex_lock(wasapi_devout_mutex);

    g_value_set_string(value, wasapi_devout->device);

    g_rec_mutex_unlock(wasapi_devout_mutex);
  }
  break;
  case PROP_DSP_CHANNELS:
  {
    g_rec_mutex_lock(wasapi_devout_mutex);

    g_value_set_uint(value, wasapi_devout->dsp_channels);

    g_rec_mutex_unlock(wasapi_devout_mutex);
  }
  break;
  case PROP_PCM_CHANNELS:
  {
    g_rec_mutex_lock(wasapi_devout_mutex);

    g_value_set_uint(value, wasapi_devout->pcm_channels);

    g_rec_mutex_unlock(wasapi_devout_mutex);
  }
  break;
  case PROP_FORMAT:
  {
    g_rec_mutex_lock(wasapi_devout_mutex);

    g_value_set_uint(value, wasapi_devout->format);

    g_rec_mutex_unlock(wasapi_devout_mutex);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    g_rec_mutex_lock(wasapi_devout_mutex);

    g_value_set_uint(value, wasapi_devout->buffer_size);

    g_rec_mutex_unlock(wasapi_devout_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    g_rec_mutex_lock(wasapi_devout_mutex);

    g_value_set_uint(value, wasapi_devout->samplerate);

    g_rec_mutex_unlock(wasapi_devout_mutex);
  }
  break;
  case PROP_BUFFER:
  {
    g_rec_mutex_lock(wasapi_devout_mutex);

    g_value_set_pointer(value, wasapi_devout->buffer);

    g_rec_mutex_unlock(wasapi_devout_mutex);
  }
  break;
  case PROP_BPM:
  {
    g_rec_mutex_lock(wasapi_devout_mutex);

    g_value_set_double(value, wasapi_devout->bpm);

    g_rec_mutex_unlock(wasapi_devout_mutex);
  }
  break;
  case PROP_DELAY_FACTOR:
  {
    g_rec_mutex_lock(wasapi_devout_mutex);

    g_value_set_double(value, wasapi_devout->delay_factor);

    g_rec_mutex_unlock(wasapi_devout_mutex);
  }
  break;
  case PROP_ATTACK:
  {
    g_rec_mutex_lock(wasapi_devout_mutex);

    g_value_set_pointer(value, wasapi_devout->attack);

    g_rec_mutex_unlock(wasapi_devout_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_wasapi_devout_dispose(GObject *gobject)
{
  AgsWasapiDevout *wasapi_devout;

  GList *list;

  wasapi_devout = AGS_WASAPI_DEVOUT(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_wasapi_devout_parent_class)->dispose(gobject);
}

void
ags_wasapi_devout_finalize(GObject *gobject)
{
  AgsWasapiDevout *wasapi_devout;

  wasapi_devout = AGS_WASAPI_DEVOUT(gobject);

  /* free output buffer */
  free(wasapi_devout->buffer[0]);
  free(wasapi_devout->buffer[1]);
  free(wasapi_devout->buffer[2]);
  free(wasapi_devout->buffer[3]);
  free(wasapi_devout->buffer[4]);
  free(wasapi_devout->buffer[5]);
  free(wasapi_devout->buffer[6]);
  free(wasapi_devout->buffer[7]);

  /* free buffer array */
  free(wasapi_devout->buffer);

  /* free AgsAttack */
  free(wasapi_devout->attack);

  /* call parent */
  G_OBJECT_CLASS(ags_wasapi_devout_parent_class)->finalize(gobject);
}

AgsUUID*
ags_wasapi_devout_get_uuid(AgsConnectable *connectable)
{
  AgsWasapiDevout *wasapi_devout;
  
  AgsUUID *ptr;

  GRecMutex *wasapi_devout_mutex;

  wasapi_devout = AGS_WASAPI_DEVOUT(connectable);

  /* get wasapi devout signal mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* get UUID */
  g_rec_mutex_lock(wasapi_devout_mutex);

  ptr = wasapi_devout->uuid;

  g_rec_mutex_unlock(wasapi_devout_mutex);
  
  return(ptr);
}

gboolean
ags_wasapi_devout_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_wasapi_devout_is_ready(AgsConnectable *connectable)
{
  AgsWasapiDevout *wasapi_devout;
  
  gboolean is_ready;

  wasapi_devout = AGS_WASAPI_DEVOUT(connectable);

  /* check is added */
  is_ready = ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_wasapi_devout_add_to_registry(AgsConnectable *connectable)
{
  AgsWasapiDevout *wasapi_devout;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  wasapi_devout = AGS_WASAPI_DEVOUT(connectable);

  ags_wasapi_devout_set_flags(wasapi_devout, AGS_WASAPI_DEVOUT_ADDED_TO_REGISTRY);
}

void
ags_wasapi_devout_remove_from_registry(AgsConnectable *connectable)
{
  AgsWasapiDevout *wasapi_devout;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  wasapi_devout = AGS_WASAPI_DEVOUT(connectable);

  ags_wasapi_devout_unset_flags(wasapi_devout, AGS_WASAPI_DEVOUT_ADDED_TO_REGISTRY);
}

xmlNode*
ags_wasapi_devout_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_wasapi_devout_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_wasapi_devout_xml_parse(AgsConnectable *connectable,
			    xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_wasapi_devout_is_connected(AgsConnectable *connectable)
{
  AgsWasapiDevout *wasapi_devout;
  
  gboolean is_connected;

  wasapi_devout = AGS_WASAPI_DEVOUT(connectable);

  /* check is connected */
  is_connected = ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_CONNECTED);
  
  return(is_connected);
}

void
ags_wasapi_devout_connect(AgsConnectable *connectable)
{
  AgsWasapiDevout *wasapi_devout;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  wasapi_devout = AGS_WASAPI_DEVOUT(connectable);

  ags_wasapi_devout_set_flags(wasapi_devout, AGS_WASAPI_DEVOUT_CONNECTED);
}

void
ags_wasapi_devout_disconnect(AgsConnectable *connectable)
{

  AgsWasapiDevout *wasapi_devout;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  wasapi_devout = AGS_WASAPI_DEVOUT(connectable);
  
  ags_wasapi_devout_unset_flags(wasapi_devout, AGS_WASAPI_DEVOUT_CONNECTED);
}

/**
 * ags_wasapi_devout_test_flags:
 * @wasapi_devout: the #AgsWasapiDevout
 * @flags: the flags
 *
 * Test @flags to be set on @wasapi_devout.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_wasapi_devout_test_flags(AgsWasapiDevout *wasapi_devout, guint flags)
{
  gboolean retval;  
  
  GRecMutex *wasapi_devout_mutex;

  if(!AGS_IS_WASAPI_DEVOUT(wasapi_devout)){
    return(FALSE);
  }

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* test */
  g_rec_mutex_lock(wasapi_devout_mutex);

  retval = (flags & (wasapi_devout->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(wasapi_devout_mutex);

  return(retval);
}

/**
 * ags_wasapi_devout_set_flags:
 * @wasapi_devout: the #AgsWasapiDevout
 * @flags: see #AgsWasapiDevoutFlags-enum
 *
 * Enable a feature of @wasapi_devout.
 *
 * Since: 3.0.0
 */
void
ags_wasapi_devout_set_flags(AgsWasapiDevout *wasapi_devout, guint flags)
{
  GRecMutex *wasapi_devout_mutex;

  if(!AGS_IS_WASAPI_DEVOUT(wasapi_devout)){
    return;
  }

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(wasapi_devout_mutex);

  wasapi_devout->flags |= flags;
  
  g_rec_mutex_unlock(wasapi_devout_mutex);
}
    
/**
 * ags_wasapi_devout_unset_flags:
 * @wasapi_devout: the #AgsWasapiDevout
 * @flags: see #AgsWasapiDevoutFlags-enum
 *
 * Disable a feature of @wasapi_devout.
 *
 * Since: 3.0.0
 */
void
ags_wasapi_devout_unset_flags(AgsWasapiDevout *wasapi_devout, guint flags)
{  
  GRecMutex *wasapi_devout_mutex;

  if(!AGS_IS_WASAPI_DEVOUT(wasapi_devout)){
    return;
  }

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(wasapi_devout_mutex);

  wasapi_devout->flags &= (~flags);
  
  g_rec_mutex_unlock(wasapi_devout_mutex);
}

void
ags_wasapi_devout_set_device(AgsSoundcard *soundcard,
			     gchar *device)
{
  AgsWasapiDevout *wasapi_devout;

  GList *card_id, *card_id_start, *card_name, *card_name_start;
  
  GRecMutex *wasapi_devout_mutex;
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi_devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* list cards */
  card_id = NULL;
  card_name = NULL;
  
  ags_soundcard_list_cards(soundcard,
			   &card_id, &card_name);

  card_id_start = card_id;
  card_name_start = card_name;

  /* check card */
  g_rec_mutex_lock(wasapi_devout_mutex);

  while(card_id != NULL){
    if(!g_ascii_strncasecmp(card_id->data,
			    device,
			    strlen(card_id->data))){
      wasapi_devout->device = g_strdup(device);

      break;
    }
    
    card_id = card_id->next;
  }

  g_rec_mutex_unlock(wasapi_devout_mutex);

  /* free card id and name */
  g_list_free_full(card_id_start,
		   g_free);
  g_list_free_full(card_name_start,
		   g_free);
}

gchar*
ags_wasapi_devout_get_device(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;
  
  gchar *device;

  GRecMutex *wasapi_devout_mutex;

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);
  
  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  device = NULL;

  /* get device */
  g_rec_mutex_lock(wasapi_devout_mutex);

  device = g_strdup(wasapi_devout->device);

  g_rec_mutex_unlock(wasapi_devout_mutex);
  
  return(device);
}

void
ags_wasapi_devout_set_presets(AgsSoundcard *soundcard,
			      guint channels,
			      guint rate,
			      guint buffer_size,
			      guint format)
{
  AgsWasapiDevout *wasapi_devout;

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  g_object_set(wasapi_devout,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_wasapi_devout_get_presets(AgsSoundcard *soundcard,
			      guint *channels,
			      guint *rate,
			      guint *buffer_size,
			      guint *format)
{
  AgsWasapiDevout *wasapi_devout;

  GRecMutex *wasapi_devout_mutex;

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);
  
  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* get presets */
  g_rec_mutex_lock(wasapi_devout_mutex);

  if(channels != NULL){
    *channels = wasapi_devout->pcm_channels;
  }

  if(rate != NULL){
    *rate = wasapi_devout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = wasapi_devout->buffer_size;
  }

  if(format != NULL){
    *format = wasapi_devout->format;
  }

  g_rec_mutex_unlock(wasapi_devout_mutex);
}

void
ags_wasapi_devout_list_cards(AgsSoundcard *soundcard,
			     GList **card_id, GList **card_name)
{
  AgsWasapiDevout *wasapi_devout;

  AgsApplicationContext *application_context;

#ifdef AGS_WITH_WASAPI
  IMMDeviceEnumerator *dev_enumerator;
  IMMDeviceCollection *dev_collection;

  UINT i, i_stop;
#endif
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

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

  dev_enumerator->lpVtbl->EnumAudioEndpoints(dev_enumerator, eRender, DEVICE_STATE_ACTIVE, &dev_collection);

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
ags_wasapi_devout_pcm_info(AgsSoundcard *soundcard,
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
ags_wasapi_devout_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_PLAYBACK);
}

gboolean
ags_wasapi_devout_is_starting(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  gboolean is_starting;
  
  GRecMutex *wasapi_devout_mutex;
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* check is starting */
  g_rec_mutex_lock(wasapi_devout_mutex);

  is_starting = ((AGS_WASAPI_DEVOUT_START_PLAY & (wasapi_devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(wasapi_devout_mutex);
  
  return(is_starting);
}

gboolean
ags_wasapi_devout_is_playing(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  gboolean is_playing;
  
  GRecMutex *wasapi_devout_mutex;

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);
  
  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* check is starting */
  g_rec_mutex_lock(wasapi_devout_mutex);

  is_playing = ((AGS_WASAPI_DEVOUT_PLAY & (wasapi_devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(wasapi_devout_mutex);

  return(is_playing);
}

gchar*
ags_wasapi_devout_get_uptime(AgsSoundcard *soundcard)
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
ags_wasapi_devout_client_init(AgsSoundcard *soundcard,
			      GError **error)
{
  AgsWasapiDevout *wasapi_devout;

  GRecMutex *wasapi_devout_mutex; 

  if(ags_soundcard_is_playing(soundcard)){
    return;
  }

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  g_rec_mutex_lock(wasapi_devout_mutex);
  
  wasapi_devout->tact_counter = 0.0;
  wasapi_devout->delay_counter = floor(ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(wasapi_devout)));
  wasapi_devout->tic_counter = 0;
  
#ifdef AGS_WITH_WASAPI
  wasapi_devout->flags |= AGS_WASAPI_DEVOUT_INITIALIZED;
#endif
  wasapi_devout->flags |= (AGS_WASAPI_DEVOUT_BUFFER0 |
			   AGS_WASAPI_DEVOUT_START_PLAY |
			   AGS_WASAPI_DEVOUT_PLAY |
			   AGS_WASAPI_DEVOUT_NONBLOCKING);
  wasapi_devout->flags &= (~(AGS_WASAPI_DEVOUT_BUFFER1 |
			     AGS_WASAPI_DEVOUT_BUFFER2 |
			     AGS_WASAPI_DEVOUT_BUFFER3 |
			     AGS_WASAPI_DEVOUT_BUFFER4 |
			     AGS_WASAPI_DEVOUT_BUFFER5 |
			     AGS_WASAPI_DEVOUT_BUFFER6 |
			     AGS_WASAPI_DEVOUT_BUFFER7));
  
  g_rec_mutex_unlock(wasapi_devout_mutex);
}

void
ags_wasapi_devout_client_play(AgsSoundcard *soundcard,
			      GError **error)
{
  AgsWasapiDevout *wasapi_devout;

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
  IAudioRenderClient *audio_render_client;

  BYTE *data;

  UINT32 buffer_frame_count;  
  UINT32 num_frames_available;
#endif

  guint word_size;
  guint nth_buffer;
  
  GRecMutex *wasapi_devout_mutex;
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  application_context = ags_application_context_get_instance();

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* lock */
  g_rec_mutex_lock(wasapi_devout_mutex);
  
  /* retrieve word size */
  switch(wasapi_devout->format){
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
    g_rec_mutex_unlock(wasapi_devout_mutex);
    
    g_warning("ags_wasapi_devout_alsa_play(): unsupported word size");

    return;
  }

  /* do playback */
#ifdef AGS_WITH_WASAPI
  if((AGS_WASAPI_DEVOUT_START_PLAY & (wasapi_devout->flags)) != 0){
    IMMDeviceEnumerator *dev_enumerator;
    IMMDevice *mm_device;

    WAVEFORMATEXTENSIBLE wave_format;
    WAVEFORMATEX *desired_format, *internal_format;

    wchar_t dev_id[1024];

    REFERENCE_TIME min_duration;

    register HRESULT hr;

    unsigned char bit_resolution;

    g_message("WASAPI initialize");
    
    CoInitialize(0);

    if(CoCreateInstance(&ags_wasapi_clsid_mm_device_enumerator_guid, 0, CLSCTX_ALL, &ags_wasapi_iid_mm_device_enumerator_guid, (void **) &dev_enumerator)){
      if(error != NULL){
	g_set_error(error,
		    AGS_WASAPI_DEVOUT_ERROR,
		    AGS_WASAPI_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		    "unable to open pcm device: Can't get IMMDeviceEnumerator.");
      }

      CoUninitialize();

      g_rec_mutex_unlock(wasapi_devout_mutex);

      g_message("WASAPI failed - device enumerator");
    
      wasapi_devout->flags &= (~AGS_WASAPI_DEVOUT_START_PLAY);
      
      return;
    }

    if(wasapi_devout->device != NULL){
      memset(dev_id, 0, 1024 * sizeof(WCHAR));
      swprintf(dev_id, 1024, L"%S", wasapi_devout->device);
      
      if(dev_enumerator->lpVtbl->GetDevice(dev_enumerator, dev_id, &mm_device)){
	if(error != NULL){
	  g_set_error(error,
		      AGS_WASAPI_DEVOUT_ERROR,
		      AGS_WASAPI_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		      "unable to open pcm device: Can't get IAudioClient.");
	}
	
	dev_enumerator->lpVtbl->Release(dev_enumerator);
	CoUninitialize();

	wasapi_devout->flags &= (~AGS_WASAPI_DEVOUT_START_PLAY);
	
	g_rec_mutex_unlock(wasapi_devout_mutex);

	g_message("WASAPI failed - get device");
    
	return;
      }
    }else{
      if(dev_enumerator->lpVtbl->GetDefaultAudioEndpoint(dev_enumerator, eRender, eMultimedia, &mm_device)){
	if(error != NULL){
	  g_set_error(error,
		      AGS_WASAPI_DEVOUT_ERROR,
		      AGS_WASAPI_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		      "unable to open pcm device: Can't get IAudioClient.");
	}

	dev_enumerator->lpVtbl->Release(dev_enumerator);
	CoUninitialize();

	wasapi_devout->flags &= (~AGS_WASAPI_DEVOUT_START_PLAY);
	
	g_rec_mutex_unlock(wasapi_devout_mutex);

	g_message("WASAPI failed - get device");
    
	return;
      }
    }
  
    wasapi_devout->mm_device = mm_device;
  
    // Get its IAudioClient (used to set audio format, latency, and start/stop)
    if(mm_device->lpVtbl->Activate(mm_device, &ags_wasapi_iid_audio_client_guid, CLSCTX_ALL, 0, (void **) &audio_client)){
      if(error != NULL){
	g_set_error(error,
		    AGS_WASAPI_DEVOUT_ERROR,
		    AGS_WASAPI_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		    "unable to open pcm device: Can't get IAudioClient.");
      }

      mm_device->lpVtbl->Release(mm_device);
      dev_enumerator->lpVtbl->Release(dev_enumerator);
      CoUninitialize();

      wasapi_devout->flags &= (~AGS_WASAPI_DEVOUT_START_PLAY);
      
      g_rec_mutex_unlock(wasapi_devout_mutex);

      g_message("WASAPI failed - get audio client");
    
      return;
    }
  
    wasapi_devout->audio_client = audio_client;

    if((AGS_WASAPI_DEVOUT_SHARE_MODE_EXCLUSIVE & (wasapi_devout->flags)) != 0){
      wave_format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
      wave_format.Format.nSamplesPerSec = wasapi_devout->samplerate; // necessary
      wave_format.Format.nChannels = wasapi_devout->pcm_channels; // presumed

      bit_resolution = 16;
  
      switch(wasapi_devout->format){
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
      ags_wasapi_devout_client_init_EXCLUSIVE_BROKEN_CONFIGURATION:

	mm_device->lpVtbl->Release(mm_device);
	dev_enumerator->lpVtbl->Release(dev_enumerator);
	CoUninitialize();

	wasapi_devout->flags &= (~AGS_WASAPI_DEVOUT_START_PLAY);
      
	g_rec_mutex_unlock(wasapi_devout_mutex);

	g_message("WASAPI failed - broken configuration");
    
	return;
      }
  
      audio_client->lpVtbl->GetDevicePeriod(audio_client, NULL, &min_duration);

      min_duration = (AGS_NSEC_PER_SEC / 100) / wasapi_devout->samplerate * wasapi_devout->wasapi_buffer_size;

      if((hr = audio_client->lpVtbl->Initialize(audio_client, AUDCLNT_SHAREMODE_EXCLUSIVE, 0, min_duration, min_duration, desired_format, NULL))){
	audio_client->lpVtbl->Release(audio_client);

	goto ags_wasapi_devout_client_init_EXCLUSIVE_BROKEN_CONFIGURATION;
      }
    }else{
      wave_format.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
      wave_format.Format.nSamplesPerSec = wasapi_devout->samplerate; // necessary
      wave_format.Format.nChannels = wasapi_devout->pcm_channels; // presumed

      bit_resolution = 16;
  
      switch(wasapi_devout->format){
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

      ags_wasapi_devout_client_init_SHARED_BROKEN_CONFIGURATION:

	mm_device->lpVtbl->Release(mm_device);
	dev_enumerator->lpVtbl->Release(dev_enumerator);
	CoUninitialize();

	wasapi_devout->flags &= (~AGS_WASAPI_DEVOUT_START_PLAY);
      
	g_rec_mutex_unlock(wasapi_devout_mutex);

	g_message("WASAPI failed - broken configuration");
    
	return;
      }

      desired_format = internal_format;

      if((hr = audio_client->lpVtbl->IsFormatSupported(audio_client, AUDCLNT_SHAREMODE_SHARED, desired_format, &desired_format))){
	goto ags_wasapi_devout_client_init_SHARED_BROKEN_CONFIGURATION;
      }

      desired_format = internal_format;
    
      audio_client->lpVtbl->GetDevicePeriod(audio_client, NULL, &min_duration);

      min_duration = (AGS_NSEC_PER_SEC / 100) / wasapi_devout->samplerate * wasapi_devout->wasapi_buffer_size;
    
      if((hr = audio_client->lpVtbl->Initialize(audio_client, AUDCLNT_SHAREMODE_SHARED, 0, min_duration, 0, desired_format, NULL))){
	audio_client->lpVtbl->Release(audio_client);

	goto ags_wasapi_devout_client_init_SHARED_BROKEN_CONFIGURATION;
      }
    }
    
    // Start audio playback
    audio_client->lpVtbl->Start(audio_client);
  }

  audio_client = wasapi_devout->audio_client;
#endif
  
  wasapi_devout->flags &= (~AGS_WASAPI_DEVOUT_START_PLAY);

  if((AGS_WASAPI_DEVOUT_INITIALIZED & (wasapi_devout->flags)) == 0){
    g_rec_mutex_unlock(wasapi_devout_mutex);
    
    return;
  }

  //  g_message("play - 0x%0x", ((AGS_WASAPI_DEVOUT_BUFFER0 |
  //				AGS_WASAPI_DEVOUT_BUFFER1 |
  //				AGS_WASAPI_DEVOUT_BUFFER2 |
  //				AGS_WASAPI_DEVOUT_BUFFER3) & (wasapi_devout->flags)));

  if((AGS_WASAPI_DEVOUT_SHUTDOWN & (wasapi_devout->flags)) == 0){
    /* check buffer flag */
    nth_buffer = 0;

    if((AGS_WASAPI_DEVOUT_BUFFER0 & (wasapi_devout->flags)) != 0){
      nth_buffer = 0;
    }else if((AGS_WASAPI_DEVOUT_BUFFER1 & (wasapi_devout->flags)) != 0){
      nth_buffer = 1;
    }else if((AGS_WASAPI_DEVOUT_BUFFER2 & (wasapi_devout->flags)) != 0){
      nth_buffer = 2;
    }else if((AGS_WASAPI_DEVOUT_BUFFER3 & (wasapi_devout->flags)) != 0){
      nth_buffer = 3;
    }else if((AGS_WASAPI_DEVOUT_BUFFER4 & (wasapi_devout->flags)) != 0){
      nth_buffer = 4;
    }else if((AGS_WASAPI_DEVOUT_BUFFER5 & (wasapi_devout->flags)) != 0){
      nth_buffer = 5;
    }else if((AGS_WASAPI_DEVOUT_BUFFER6 & (wasapi_devout->flags)) != 0){
      nth_buffer = 6;
    }else if((AGS_WASAPI_DEVOUT_BUFFER7 & (wasapi_devout->flags)) != 0){
      nth_buffer = 7;
    }
    
#ifdef AGS_WITH_WASAPI
    audio_client->lpVtbl->GetBufferSize(audio_client, &buffer_frame_count);

    {
      HRESULT res;
    
      res = audio_client->lpVtbl->GetService(audio_client, &ags_wasapi_iid_audio_render_client_guid, (void **) &audio_render_client);

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

    if(audio_render_client == NULL){
      g_rec_mutex_unlock(wasapi_devout_mutex);

      g_message("audio_render_client = NULL");
      
      return;
    }

    {
      UINT32 padding_frames;

      static const struct timespec poll_delay = {
	0,
	400,
      };
      
      audio_client->lpVtbl->GetCurrentPadding(audio_client, &padding_frames);
      
      while(buffer_frame_count - padding_frames < wasapi_devout->buffer_size &&
	    padding_frames != 0){
	nanosleep(&poll_delay, NULL);

	audio_client->lpVtbl->GetCurrentPadding(audio_client, &padding_frames);
      }
    }
    
    {
      HRESULT res;
      
      res = audio_render_client->lpVtbl->GetBuffer(audio_render_client, wasapi_devout->buffer_size, &data);
      
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
      ags_soundcard_lock_buffer(soundcard,
				wasapi_devout->buffer[nth_buffer]);
      
      switch(wasapi_devout->format){
      case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	memset(data, 0, wasapi_devout->pcm_channels * wasapi_devout->buffer_size * sizeof(gint16));
	ags_audio_buffer_util_copy_s16_to_s16((gint16 *) data, 1,
					      (gint16 *) wasapi_devout->buffer[nth_buffer], 1,
					      wasapi_devout->pcm_channels * wasapi_devout->buffer_size);
      }
      break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	memset(data, 0, wasapi_devout->pcm_channels * wasapi_devout->buffer_size * sizeof(gint32));
	ags_audio_buffer_util_copy_s24_to_s24((gint32 *) data, 1,
					      (gint32 *) wasapi_devout->buffer[nth_buffer], 1,
					      wasapi_devout->pcm_channels * wasapi_devout->buffer_size);
      }
      break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	memset(data, 0, wasapi_devout->pcm_channels * wasapi_devout->buffer_size * sizeof(gint32));
	ags_audio_buffer_util_copy_s32_to_s32((gint32 *) data, 1,
					      (gint32 *) wasapi_devout->buffer[nth_buffer], 1,
					      wasapi_devout->pcm_channels * wasapi_devout->buffer_size);
      }
      break;
      }
      
      ags_soundcard_unlock_buffer(soundcard,
				  wasapi_devout->buffer[nth_buffer]);
    }else{
      g_message("data = NULL");
    }

    audio_render_client->lpVtbl->ReleaseBuffer(audio_render_client, wasapi_devout->buffer_size, 0);

    audio_render_client->lpVtbl->Release(audio_render_client);
#endif
  }
  
  if((AGS_WASAPI_DEVOUT_SHUTDOWN & (wasapi_devout->flags)) != 0){
    AgsThread *audio_loop;
    AgsThread *soundcard_thread;
    
    AgsApplicationContext *application_context;
    
#ifdef AGS_WITH_WASAPI
    IMMDevice *mm_device;

    HRESULT hr;

    g_message("wasapi shutdown");
    
    audio_client = wasapi_devout->audio_client;
    audio_client->lpVtbl->GetService(audio_client, &ags_wasapi_iid_audio_render_client_guid, (void **) &audio_render_client);
    mm_device = wasapi_devout->mm_device;

    hr = audio_client->lpVtbl->Stop(audio_client);

    if(FAILED(hr)){
      g_message("failed to stop WASAPI");
    }

    audio_client->lpVtbl->Reset(audio_client);

    if(audio_render_client != NULL){
      audio_render_client->lpVtbl->Release(audio_render_client);
    }
    
    audio_client->lpVtbl->Release(audio_client);
    mm_device->lpVtbl->Release(mm_device);
    
    wasapi_devout->audio_client = NULL;
    wasapi_devout->mm_device = NULL;

    CoUninitialize();    
#endif
    application_context = ags_application_context_get_instance();
    
    /* get main loop */
    audio_loop = ags_concurrency_provider_get_main_loop(AGS_CONCURRENCY_PROVIDER(application_context));
    
    soundcard_thread = ags_thread_find_type(audio_loop,
					    AGS_TYPE_SOUNDCARD_THREAD);

    soundcard_thread = ags_soundcard_thread_find_soundcard(soundcard_thread,
							   AGS_WASAPI_DEVOUT(soundcard));

    ags_thread_stop(soundcard_thread);
    
    g_object_unref(soundcard_thread);    
  }
  
  g_rec_mutex_unlock(wasapi_devout_mutex);

  if((AGS_WASAPI_DEVOUT_SHUTDOWN & (wasapi_devout->flags)) == 0){
    /* update soundcard */
    task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

    task = NULL;
  
    /* tic soundcard */
    tic_device = ags_tic_device_new((GObject *) wasapi_devout);
    task = g_list_append(task,
			 tic_device);

    /* reset - clear buffer */
    clear_buffer = ags_clear_buffer_new((GObject *) wasapi_devout);
    task = g_list_append(task,
			 clear_buffer);

    /* reset - switch buffer flags */
    switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) wasapi_devout);
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
ags_wasapi_devout_client_free(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  guint i;
  
  GRecMutex *wasapi_devout_mutex;
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* lock */
  g_rec_mutex_lock(wasapi_devout_mutex);

  if((AGS_WASAPI_DEVOUT_INITIALIZED & (wasapi_devout->flags)) == 0){
    g_rec_mutex_unlock(wasapi_devout_mutex);
    
    return;
  }

  wasapi_devout->flags |= (AGS_WASAPI_DEVOUT_SHUTDOWN);

  g_rec_mutex_unlock(wasapi_devout_mutex);

  g_rec_mutex_lock(wasapi_devout_mutex);

  wasapi_devout->note_offset = wasapi_devout->start_note_offset;
  wasapi_devout->note_offset_absolute = wasapi_devout->start_note_offset;

  g_rec_mutex_unlock(wasapi_devout_mutex);
}

void
ags_wasapi_devout_tic(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  gdouble delay;
  gdouble delay_counter;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  
  GRecMutex *wasapi_devout_mutex;
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(wasapi_devout_mutex);

  delay = wasapi_devout->delay[wasapi_devout->tic_counter];
  delay_counter = wasapi_devout->delay_counter;

  note_offset = wasapi_devout->note_offset;
  note_offset_absolute = wasapi_devout->note_offset_absolute;
  
  loop_left = wasapi_devout->loop_left;
  loop_right = wasapi_devout->loop_right;
  
  do_loop = wasapi_devout->do_loop;

  g_rec_mutex_unlock(wasapi_devout_mutex);

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
    g_rec_mutex_lock(wasapi_devout_mutex);
    
    wasapi_devout->delay_counter = delay_counter + 1.0 - delay;
    wasapi_devout->tact_counter += 1.0;

    g_rec_mutex_unlock(wasapi_devout_mutex);
  }else{
    g_rec_mutex_lock(wasapi_devout_mutex);
    
    wasapi_devout->delay_counter += 1.0;

    g_rec_mutex_unlock(wasapi_devout_mutex);
  }
}

void
ags_wasapi_devout_offset_changed(AgsSoundcard *soundcard,
				 guint note_offset)
{
  AgsWasapiDevout *wasapi_devout;
  
  GRecMutex *wasapi_devout_mutex;
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* offset changed */
  g_rec_mutex_lock(wasapi_devout_mutex);

  wasapi_devout->tic_counter += 1;

  if(wasapi_devout->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    wasapi_devout->tic_counter = 0;
  }

  g_rec_mutex_unlock(wasapi_devout_mutex);
}

void
ags_wasapi_devout_set_bpm(AgsSoundcard *soundcard,
			  gdouble bpm)
{
  AgsWasapiDevout *wasapi_devout;

  GRecMutex *wasapi_devout_mutex;
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* set bpm */
  g_rec_mutex_lock(wasapi_devout_mutex);

  wasapi_devout->bpm = bpm;

  g_rec_mutex_unlock(wasapi_devout_mutex);

  ags_wasapi_devout_adjust_delay_and_attack(wasapi_devout);
}

gdouble
ags_wasapi_devout_get_bpm(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  gdouble bpm;
  
  GRecMutex *wasapi_devout_mutex;
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* get bpm */
  g_rec_mutex_lock(wasapi_devout_mutex);

  bpm = wasapi_devout->bpm;
  
  g_rec_mutex_unlock(wasapi_devout_mutex);

  return(bpm);
}

void
ags_wasapi_devout_set_delay_factor(AgsSoundcard *soundcard,
				   gdouble delay_factor)
{
  AgsWasapiDevout *wasapi_devout;

  GRecMutex *wasapi_devout_mutex;
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* set delay factor */
  g_rec_mutex_lock(wasapi_devout_mutex);

  wasapi_devout->delay_factor = delay_factor;

  g_rec_mutex_unlock(wasapi_devout_mutex);

  ags_wasapi_devout_adjust_delay_and_attack(wasapi_devout);
}

gdouble
ags_wasapi_devout_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  gdouble delay_factor;
  
  GRecMutex *wasapi_devout_mutex;
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* get delay factor */
  g_rec_mutex_lock(wasapi_devout_mutex);

  delay_factor = wasapi_devout->delay_factor;
  
  g_rec_mutex_unlock(wasapi_devout_mutex);

  return(delay_factor);
}

gdouble
ags_wasapi_devout_get_delay(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  guint delay_index;
  gdouble delay;
  
  GRecMutex *wasapi_devout_mutex;
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* get delay */
  g_rec_mutex_lock(wasapi_devout_mutex);

  delay_index = wasapi_devout->tic_counter;

  delay = wasapi_devout->delay[delay_index];
  
  g_rec_mutex_unlock(wasapi_devout_mutex);
  
  return(delay);
}

gdouble
ags_wasapi_devout_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  gdouble absolute_delay;
  
  GRecMutex *wasapi_devout_mutex;
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);
  
  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* get absolute delay */
  g_rec_mutex_lock(wasapi_devout_mutex);

  absolute_delay = (60.0 * (((gdouble) wasapi_devout->samplerate / (gdouble) wasapi_devout->buffer_size) / (gdouble) wasapi_devout->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) wasapi_devout->delay_factor)));

  g_rec_mutex_unlock(wasapi_devout_mutex);

  return(absolute_delay);
}

guint
ags_wasapi_devout_get_attack(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  guint attack_index;
  guint attack;
  
  GRecMutex *wasapi_devout_mutex;  

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);
  
  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* get attack */
  g_rec_mutex_lock(wasapi_devout_mutex);

  attack_index = wasapi_devout->tic_counter;

  attack = wasapi_devout->attack[attack_index];

  g_rec_mutex_unlock(wasapi_devout_mutex);
  
  return(attack);
}

void*
ags_wasapi_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  void *buffer;

  GRecMutex *wasapi_devout_mutex;  
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER0)){
    buffer = wasapi_devout->buffer[0];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER1)){
    buffer = wasapi_devout->buffer[1];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER2)){
    buffer = wasapi_devout->buffer[2];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER3)){
    buffer = wasapi_devout->buffer[3];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER4)){
    buffer = wasapi_devout->buffer[4];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER5)){
    buffer = wasapi_devout->buffer[5];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER6)){
    buffer = wasapi_devout->buffer[6];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER7)){
    buffer = wasapi_devout->buffer[7];
  }else{
    buffer = NULL;
  }
  
  return(buffer);
}

void*
ags_wasapi_devout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  void *buffer;
  
  GRecMutex *wasapi_devout_mutex;  

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  g_rec_mutex_lock(wasapi_devout_mutex);

  if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER0)){
    buffer = wasapi_devout->buffer[1];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER1)){
    buffer = wasapi_devout->buffer[2];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER2)){
    buffer = wasapi_devout->buffer[3];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER3)){
    buffer = wasapi_devout->buffer[4];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER4)){
    buffer = wasapi_devout->buffer[5];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER5)){
    buffer = wasapi_devout->buffer[6];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER6)){
    buffer = wasapi_devout->buffer[7];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER7)){
    buffer = wasapi_devout->buffer[0];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(wasapi_devout_mutex);

  return(buffer);
}

void*
ags_wasapi_devout_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  void *buffer;
  
  GRecMutex *wasapi_devout_mutex;  

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  g_rec_mutex_lock(wasapi_devout_mutex);

  if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER0)){
    buffer = wasapi_devout->buffer[7];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER1)){
    buffer = wasapi_devout->buffer[0];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER2)){
    buffer = wasapi_devout->buffer[1];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER3)){
    buffer = wasapi_devout->buffer[2];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER4)){
    buffer = wasapi_devout->buffer[3];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER5)){
    buffer = wasapi_devout->buffer[4];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER6)){
    buffer = wasapi_devout->buffer[5];
  }else if(ags_wasapi_devout_test_flags(wasapi_devout, AGS_WASAPI_DEVOUT_BUFFER7)){
    buffer = wasapi_devout->buffer[6];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(wasapi_devout_mutex);

  return(buffer);
}

void
ags_wasapi_devout_lock_buffer(AgsSoundcard *soundcard,
			      void *buffer)
{
  AgsWasapiDevout *wasapi_devout;

  GRecMutex *buffer_mutex;
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(wasapi_devout->buffer != NULL){
    if(buffer == wasapi_devout->buffer[0]){
      buffer_mutex = wasapi_devout->buffer_mutex[0];
    }else if(buffer == wasapi_devout->buffer[1]){
      buffer_mutex = wasapi_devout->buffer_mutex[1];
    }else if(buffer == wasapi_devout->buffer[2]){
      buffer_mutex = wasapi_devout->buffer_mutex[2];
    }else if(buffer == wasapi_devout->buffer[3]){
      buffer_mutex = wasapi_devout->buffer_mutex[3];
    }else if(buffer == wasapi_devout->buffer[4]){
      buffer_mutex = wasapi_devout->buffer_mutex[4];
    }else if(buffer == wasapi_devout->buffer[5]){
      buffer_mutex = wasapi_devout->buffer_mutex[5];
    }else if(buffer == wasapi_devout->buffer[6]){
      buffer_mutex = wasapi_devout->buffer_mutex[6];
    }else if(buffer == wasapi_devout->buffer[7]){
      buffer_mutex = wasapi_devout->buffer_mutex[7];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_wasapi_devout_unlock_buffer(AgsSoundcard *soundcard,
				void *buffer)
{
  AgsWasapiDevout *wasapi_devout;

  GRecMutex *buffer_mutex;
  
  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(wasapi_devout->buffer != NULL){
    if(buffer == wasapi_devout->buffer[0]){
      buffer_mutex = wasapi_devout->buffer_mutex[0];
    }else if(buffer == wasapi_devout->buffer[1]){
      buffer_mutex = wasapi_devout->buffer_mutex[1];
    }else if(buffer == wasapi_devout->buffer[2]){
      buffer_mutex = wasapi_devout->buffer_mutex[2];
    }else if(buffer == wasapi_devout->buffer[3]){
      buffer_mutex = wasapi_devout->buffer_mutex[3];
    }else if(buffer == wasapi_devout->buffer[4]){
      buffer_mutex = wasapi_devout->buffer_mutex[4];
    }else if(buffer == wasapi_devout->buffer[5]){
      buffer_mutex = wasapi_devout->buffer_mutex[5];
    }else if(buffer == wasapi_devout->buffer[6]){
      buffer_mutex = wasapi_devout->buffer_mutex[6];
    }else if(buffer == wasapi_devout->buffer[7]){
      buffer_mutex = wasapi_devout->buffer_mutex[7];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_wasapi_devout_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  guint delay_counter;
  
  GRecMutex *wasapi_devout_mutex;  

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);
  
  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* delay counter */
  g_rec_mutex_lock(wasapi_devout_mutex);

  delay_counter = wasapi_devout->delay_counter;
  
  g_rec_mutex_unlock(wasapi_devout_mutex);

  return(delay_counter);
}

void
ags_wasapi_devout_set_start_note_offset(AgsSoundcard *soundcard,
					guint start_note_offset)
{
  AgsWasapiDevout *wasapi_devout;

  GRecMutex *wasapi_devout_mutex;  

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* set note offset */
  g_rec_mutex_lock(wasapi_devout_mutex);

  wasapi_devout->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(wasapi_devout_mutex);
}

guint
ags_wasapi_devout_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  guint start_note_offset;
  
  GRecMutex *wasapi_devout_mutex;  

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* set note offset */
  g_rec_mutex_lock(wasapi_devout_mutex);

  start_note_offset = wasapi_devout->start_note_offset;

  g_rec_mutex_unlock(wasapi_devout_mutex);

  return(start_note_offset);
}

void
ags_wasapi_devout_set_note_offset(AgsSoundcard *soundcard,
				  guint note_offset)
{
  AgsWasapiDevout *wasapi_devout;

  GRecMutex *wasapi_devout_mutex;  

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* set note offset */
  g_rec_mutex_lock(wasapi_devout_mutex);

  wasapi_devout->note_offset = note_offset;

  g_rec_mutex_unlock(wasapi_devout_mutex);
}

guint
ags_wasapi_devout_get_note_offset(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  guint note_offset;
  
  GRecMutex *wasapi_devout_mutex;  

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* set note offset */
  g_rec_mutex_lock(wasapi_devout_mutex);

  note_offset = wasapi_devout->note_offset;

  g_rec_mutex_unlock(wasapi_devout_mutex);

  return(note_offset);
}

void
ags_wasapi_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
					   guint note_offset_absolute)
{
  AgsWasapiDevout *wasapi_devout;
  
  GRecMutex *wasapi_devout_mutex;  

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* set note offset */
  g_rec_mutex_lock(wasapi_devout_mutex);

  wasapi_devout->note_offset_absolute = note_offset_absolute;

  g_rec_mutex_unlock(wasapi_devout_mutex);
}

guint
ags_wasapi_devout_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  guint note_offset_absolute;
  
  GRecMutex *wasapi_devout_mutex;  

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* set note offset */
  g_rec_mutex_lock(wasapi_devout_mutex);

  note_offset_absolute = wasapi_devout->note_offset_absolute;

  g_rec_mutex_unlock(wasapi_devout_mutex);

  return(note_offset_absolute);
}

void
ags_wasapi_devout_set_loop(AgsSoundcard *soundcard,
			   guint loop_left, guint loop_right,
			   gboolean do_loop)
{
  AgsWasapiDevout *wasapi_devout;

  GRecMutex *wasapi_devout_mutex;  

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* set loop */
  g_rec_mutex_lock(wasapi_devout_mutex);

  wasapi_devout->loop_left = loop_left;
  wasapi_devout->loop_right = loop_right;
  wasapi_devout->do_loop = do_loop;

  if(do_loop){
    wasapi_devout->loop_offset = wasapi_devout->note_offset;
  }

  g_rec_mutex_unlock(wasapi_devout_mutex);
}

void
ags_wasapi_devout_get_loop(AgsSoundcard *soundcard,
			   guint *loop_left, guint *loop_right,
			   gboolean *do_loop)
{
  AgsWasapiDevout *wasapi_devout;

  GRecMutex *wasapi_devout_mutex;  

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* get loop */
  g_rec_mutex_lock(wasapi_devout_mutex);

  if(loop_left != NULL){
    *loop_left = wasapi_devout->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = wasapi_devout->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = wasapi_devout->do_loop;
  }

  g_rec_mutex_unlock(wasapi_devout_mutex);
}

guint
ags_wasapi_devout_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  guint loop_offset;
  
  GRecMutex *wasapi_devout_mutex;  

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* get loop offset */
  g_rec_mutex_lock(wasapi_devout_mutex);

  loop_offset = wasapi_devout->loop_offset;
  
  g_rec_mutex_unlock(wasapi_devout_mutex);

  return(loop_offset);
}

guint
ags_wasapi_devout_get_sub_block_count(AgsSoundcard *soundcard)
{
  AgsWasapiDevout *wasapi_devout;

  guint sub_block_count;
  
  GRecMutex *wasapi_devout_mutex;  

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* get loop offset */
  g_rec_mutex_lock(wasapi_devout_mutex);

  sub_block_count = wasapi_devout->sub_block_count;
  
  g_rec_mutex_unlock(wasapi_devout_mutex);

  return(sub_block_count);
}

gboolean
ags_wasapi_devout_trylock_sub_block(AgsSoundcard *soundcard,
				    void *buffer, guint sub_block)
{
  AgsWasapiDevout *wasapi_devout;

  guint pcm_channels;
  guint sub_block_count;
  gboolean success;
  
  GRecMutex *wasapi_devout_mutex;  
  GRecMutex *sub_block_mutex;

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* get loop offset */
  g_rec_mutex_lock(wasapi_devout_mutex);

  pcm_channels = wasapi_devout->pcm_channels;
  sub_block_count = wasapi_devout->sub_block_count;
  
  g_rec_mutex_unlock(wasapi_devout_mutex);
  
  sub_block_mutex = NULL;

  success = FALSE;
  
  if(wasapi_devout->buffer != NULL){
    if(buffer == wasapi_devout->buffer[0]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[sub_block];
    }else if(buffer == wasapi_devout->buffer[1]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == wasapi_devout->buffer[2]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == wasapi_devout->buffer[3]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == wasapi_devout->buffer[4]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[4 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == wasapi_devout->buffer[5]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[5 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == wasapi_devout->buffer[6]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[6 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == wasapi_devout->buffer[7]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[7 * pcm_channels * sub_block_count + sub_block];
    }
  }

  if(sub_block_mutex != NULL){
    if(g_rec_mutex_trylock(sub_block_mutex) == 0){
      success = TRUE;
    }
  }

  return(success);
}

void
ags_wasapi_devout_unlock_sub_block(AgsSoundcard *soundcard,
				   void *buffer, guint sub_block)
{
  AgsWasapiDevout *wasapi_devout;

  guint pcm_channels;
  guint sub_block_count;
  
  GRecMutex *wasapi_devout_mutex;  
  GRecMutex *sub_block_mutex;

  wasapi_devout = AGS_WASAPI_DEVOUT(soundcard);

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* get loop offset */
  g_rec_mutex_lock(wasapi_devout_mutex);

  pcm_channels = wasapi_devout->pcm_channels;
  sub_block_count = wasapi_devout->sub_block_count;
  
  g_rec_mutex_unlock(wasapi_devout_mutex);
  
  sub_block_mutex = NULL;
  
  if(wasapi_devout->buffer != NULL){
    if(buffer == wasapi_devout->buffer[0]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[sub_block];
    }else if(buffer == wasapi_devout->buffer[1]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == wasapi_devout->buffer[2]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == wasapi_devout->buffer[3]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == wasapi_devout->buffer[4]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[4 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == wasapi_devout->buffer[5]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[5 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == wasapi_devout->buffer[6]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[6 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == wasapi_devout->buffer[7]){
      sub_block_mutex = wasapi_devout->sub_block_mutex[7 * pcm_channels * sub_block_count + sub_block];
    }
  }

  if(sub_block_mutex != NULL){
    g_rec_mutex_unlock(sub_block_mutex);
  }
}

/**
 * ags_wasapi_devout_switch_buffer_flag:
 * @wasapi_devout: an #AgsWasapiDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.0.0
 */
void
ags_wasapi_devout_switch_buffer_flag(AgsWasapiDevout *wasapi_devout)
{
  GRecMutex *wasapi_devout_mutex;
  
  if(!AGS_IS_WASAPI_DEVOUT(wasapi_devout)){
    return;
  }

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* switch buffer flag */
  g_rec_mutex_lock(wasapi_devout_mutex);

  if((AGS_WASAPI_DEVOUT_BUFFER0 & (wasapi_devout->flags)) != 0){
    wasapi_devout->flags &= (~AGS_WASAPI_DEVOUT_BUFFER0);
    wasapi_devout->flags |= AGS_WASAPI_DEVOUT_BUFFER1;
  }else if((AGS_WASAPI_DEVOUT_BUFFER1 & (wasapi_devout->flags)) != 0){
    wasapi_devout->flags &= (~AGS_WASAPI_DEVOUT_BUFFER1);
    wasapi_devout->flags |= AGS_WASAPI_DEVOUT_BUFFER2;
  }else if((AGS_WASAPI_DEVOUT_BUFFER2 & (wasapi_devout->flags)) != 0){
    wasapi_devout->flags &= (~AGS_WASAPI_DEVOUT_BUFFER2);
    wasapi_devout->flags |= AGS_WASAPI_DEVOUT_BUFFER3;
  }else if((AGS_WASAPI_DEVOUT_BUFFER3 & (wasapi_devout->flags)) != 0){
    wasapi_devout->flags &= (~AGS_WASAPI_DEVOUT_BUFFER3);
    wasapi_devout->flags |= AGS_WASAPI_DEVOUT_BUFFER4;
  }else if((AGS_WASAPI_DEVOUT_BUFFER4 & (wasapi_devout->flags)) != 0){
    wasapi_devout->flags &= (~AGS_WASAPI_DEVOUT_BUFFER4);
    wasapi_devout->flags |= AGS_WASAPI_DEVOUT_BUFFER5;
  }else if((AGS_WASAPI_DEVOUT_BUFFER5 & (wasapi_devout->flags)) != 0){
    wasapi_devout->flags &= (~AGS_WASAPI_DEVOUT_BUFFER5);
    wasapi_devout->flags |= AGS_WASAPI_DEVOUT_BUFFER6;
  }else if((AGS_WASAPI_DEVOUT_BUFFER6 & (wasapi_devout->flags)) != 0){
    wasapi_devout->flags &= (~AGS_WASAPI_DEVOUT_BUFFER6);
    wasapi_devout->flags |= AGS_WASAPI_DEVOUT_BUFFER7;
  }else if((AGS_WASAPI_DEVOUT_BUFFER7 & (wasapi_devout->flags)) != 0){
    wasapi_devout->flags &= (~AGS_WASAPI_DEVOUT_BUFFER7);
    wasapi_devout->flags |= AGS_WASAPI_DEVOUT_BUFFER0;
  }

  g_rec_mutex_unlock(wasapi_devout_mutex);
}

/**
 * ags_wasapi_devout_adjust_delay_and_attack:
 * @wasapi_devout: the #AgsWasapiDevout
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 3.0.0
 */
void
ags_wasapi_devout_adjust_delay_and_attack(AgsWasapiDevout *wasapi_devout)
{
  if(!AGS_IS_WASAPI_DEVOUT(wasapi_devout)){
    return;
  }

  ags_soundcard_util_adjust_delay_and_attack(wasapi_devout);
}

/**
 * ags_wasapi_devout_realloc_buffer:
 * @wasapi_devout: the #AgsWasapiDevout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.0.0
 */
void
ags_wasapi_devout_realloc_buffer(AgsWasapiDevout *wasapi_devout)
{
  guint pcm_channels;
  guint buffer_size;
  guint format;
  guint word_size;

  GRecMutex *wasapi_devout_mutex;  

  if(!AGS_IS_WASAPI_DEVOUT(wasapi_devout)){
    return;
  }

  /* get wasapi devout mutex */
  wasapi_devout_mutex = AGS_WASAPI_DEVOUT_GET_OBJ_MUTEX(wasapi_devout);

  /* get word size */  
  g_rec_mutex_lock(wasapi_devout_mutex);

  pcm_channels = wasapi_devout->pcm_channels;
  buffer_size = wasapi_devout->buffer_size;

  format = wasapi_devout->format;
  
  g_rec_mutex_unlock(wasapi_devout_mutex);

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
    g_warning("ags_wasapi_devout_realloc_buffer(): unsupported word size");
    return;
  }
  
  /* AGS_WASAPI_DEVOUT_BUFFER_0 */
  if(wasapi_devout->buffer[0] != NULL){
    free(wasapi_devout->buffer[0]);
  }
  
  wasapi_devout->buffer[0] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_WASAPI_DEVOUT_BUFFER_1 */
  if(wasapi_devout->buffer[1] != NULL){
    free(wasapi_devout->buffer[1]);
  }

  wasapi_devout->buffer[1] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_WASAPI_DEVOUT_BUFFER_2 */
  if(wasapi_devout->buffer[2] != NULL){
    free(wasapi_devout->buffer[2]);
  }

  wasapi_devout->buffer[2] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_WASAPI_DEVOUT_BUFFER_3 */
  if(wasapi_devout->buffer[3] != NULL){
    free(wasapi_devout->buffer[3]);
  }
  
  wasapi_devout->buffer[3] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_WASAPI_DEVOUT_BUFFER_4 */
  if(wasapi_devout->buffer[4] != NULL){
    free(wasapi_devout->buffer[4]);
  }
  
  wasapi_devout->buffer[4] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_WASAPI_DEVOUT_BUFFER_5 */
  if(wasapi_devout->buffer[5] != NULL){
    free(wasapi_devout->buffer[5]);
  }
  
  wasapi_devout->buffer[5] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_WASAPI_DEVOUT_BUFFER_6 */
  if(wasapi_devout->buffer[6] != NULL){
    free(wasapi_devout->buffer[6]);
  }
  
  wasapi_devout->buffer[6] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_WASAPI_DEVOUT_BUFFER_7 */
  if(wasapi_devout->buffer[7] != NULL){
    free(wasapi_devout->buffer[7]);
  }
  
  wasapi_devout->buffer[7] = (void *) malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_wasapi_devout_new:
 *
 * Creates a new instance of #AgsWasapiDevout.
 *
 * Returns: a new #AgsWasapiDevout
 *
 * Since: 3.0.0
 */
AgsWasapiDevout*
ags_wasapi_devout_new()
{
  AgsWasapiDevout *wasapi_devout;

  wasapi_devout = (AgsWasapiDevout *) g_object_new(AGS_TYPE_WASAPI_DEVOUT,
						   NULL);
  
  return(wasapi_devout);
}
