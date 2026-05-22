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

#include <ags/audio/core-audio/ags_core_audio_devout.h>

#include <ags/ags_api_config.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_soundcard_util.h>

#include <ags/audio/core-audio/ags_core_audio_server.h>
#include <ags/audio/core-audio/ags_core_audio_client.h>
#include <ags/audio/core-audio/ags_core_audio_port.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <ags/audio/thread/ags_audio_loop.h>

#if defined(AGS_WITH_CORE_AUDIO)  
#include <AudioToolbox/AudioToolbox.h>

#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <AudioUnit/AudioComponent.h>

#include <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>
#endif

#include <string.h>
#include <math.h>
#include <time.h>

#include <ags/i18n.h>

#define AGS_CORE_AUDIO_PORT_USE_HW (1)

void ags_core_audio_devout_class_init(AgsCoreAudioDevoutClass *core_audio_devout);
void ags_core_audio_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_core_audio_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_core_audio_devout_init(AgsCoreAudioDevout *core_audio_devout);
void ags_core_audio_devout_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_devout_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_devout_dispose(GObject *gobject);
void ags_core_audio_devout_finalize(GObject *gobject);

AgsUUID* ags_core_audio_devout_get_uuid(AgsConnectable *connectable);
gboolean ags_core_audio_devout_has_resource(AgsConnectable *connectable);
gboolean ags_core_audio_devout_is_ready(AgsConnectable *connectable);
void ags_core_audio_devout_add_to_registry(AgsConnectable *connectable);
void ags_core_audio_devout_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_core_audio_devout_list_resource(AgsConnectable *connectable);
xmlNode* ags_core_audio_devout_xml_compose(AgsConnectable *connectable);
void ags_core_audio_devout_xml_parse(AgsConnectable *connectable,
				     xmlNode *node);
gboolean ags_core_audio_devout_is_connected(AgsConnectable *connectable);
void ags_core_audio_devout_connect(AgsConnectable *connectable);
void ags_core_audio_devout_disconnect(AgsConnectable *connectable);

void ags_core_audio_devout_set_device(AgsSoundcard *soundcard,
				      gchar *device);
gchar* ags_core_audio_devout_get_device(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_presets(AgsSoundcard *soundcard,
				       guint channels,
				       guint rate,
				       guint buffer_size,
				       guint format);
void ags_core_audio_devout_get_presets(AgsSoundcard *soundcard,
				       guint *channels,
				       guint *rate,
				       guint *buffer_size,
				       guint *format);

void ags_core_audio_devout_list_cards(AgsSoundcard *soundcard,
				      GList **card_id, GList **card_name);
void ags_core_audio_devout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
				    guint *channels_min, guint *channels_max,
				    guint *rate_min, guint *rate_max,
				    guint *buffer_size_min, guint *buffer_size_max,
				    GError **error);
guint ags_core_audio_devout_get_capability(AgsSoundcard *soundcard);

gboolean ags_core_audio_devout_is_starting(AgsSoundcard *soundcard);
gboolean ags_core_audio_devout_is_playing(AgsSoundcard *soundcard);

gchar* ags_core_audio_devout_get_uptime(AgsSoundcard *soundcard);

void ags_core_audio_devout_port_init(AgsSoundcard *soundcard,
				     GError **error);
void ags_core_audio_devout_port_play(AgsSoundcard *soundcard,
				     GError **error);
void ags_core_audio_devout_port_free(AgsSoundcard *soundcard);

void ags_core_audio_devout_tic(AgsSoundcard *soundcard);
void ags_core_audio_devout_offset_changed(AgsSoundcard *soundcard,
					  guint64 note_offset);

void ags_core_audio_devout_set_bpm(AgsSoundcard *soundcard,
				   gdouble bpm);
gdouble ags_core_audio_devout_get_bpm(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_start_note_offset(AgsSoundcard *soundcard,
						 guint64 start_note_offset);
guint64 ags_core_audio_devout_get_start_note_offset(AgsSoundcard *soundcard);

GObject* ags_core_audio_devout_get_frame_clock(AgsSoundcard *soundcard);

void* ags_core_audio_devout_get_buffer(AgsSoundcard *soundcard);
void* ags_core_audio_devout_get_next_buffer(AgsSoundcard *soundcard);
void* ags_core_audio_devout_get_prev_buffer(AgsSoundcard *soundcard);

void ags_core_audio_devout_lock_buffer(AgsSoundcard *soundcard,
				       void *buffer);
void ags_core_audio_devout_unlock_buffer(AgsSoundcard *soundcard,
					 void *buffer);

guint ags_core_audio_devout_get_sub_block_count(AgsSoundcard *soundcard);

gboolean ags_core_audio_devout_trylock_sub_block(AgsSoundcard *soundcard,
						 void *buffer, guint sub_block);
void ags_core_audio_devout_unlock_sub_block(AgsSoundcard *soundcard,
					    void *buffer, guint sub_block);

/**
 * SECTION:ags_core_audio_devout
 * @short_description: Output to soundcard
 * @title: AgsCoreAudioDevout
 * @section_id:
 * @include: ags/audio/core-audio/ags_core_audio_devout.h
 *
 * #AgsCoreAudioDevout represents a soundcard and supports output.
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
  PROP_CORE_AUDIO_CLIENT,
  PROP_CORE_AUDIO_PORT,
  PROP_CHANNEL,
};

static gpointer ags_core_audio_devout_parent_class = NULL;

GType
ags_core_audio_devout_get_type (void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_core_audio_devout = 0;
 
    static const GTypeInfo ags_core_audio_devout_info = {
      sizeof(AgsCoreAudioDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_core_audio_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCoreAudioDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_core_audio_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_devout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_core_audio_devout = g_type_register_static(G_TYPE_OBJECT,
							"AgsCoreAudioDevout",
							&ags_core_audio_devout_info,
							0);

    g_type_add_interface_static(ags_type_core_audio_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_core_audio_devout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_core_audio_devout);
  }

  return(g_define_type_id__static);
}

GType
ags_core_audio_devout_flags_get_type()
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_CORE_AUDIO_DEVOUT_INITIALIZED, "AGS_CORE_AUDIO_DEVOUT_INITIALIZED", "core-audio-devout-initialized" },
      { AGS_CORE_AUDIO_DEVOUT_START_PLAY, "AGS_CORE_AUDIO_DEVOUT_START_PLAY", "core-audio-devout-start-play" },
      { AGS_CORE_AUDIO_DEVOUT_PLAY, "AGS_CORE_AUDIO_DEVOUT_PLAY", "core-audio-devout-play" },
      { AGS_CORE_AUDIO_DEVOUT_SHUTDOWN, "AGS_CORE_AUDIO_DEVOUT_SHUTDOWN", "core-audio-devout-shutdown" },
      { AGS_CORE_AUDIO_DEVOUT_NONBLOCKING, "AGS_CORE_AUDIO_DEVOUT_NONBLOCKING", "core-audio-devout-nonblocking" },
      { AGS_CORE_AUDIO_DEVOUT_ATTACK_FIRST, "AGS_CORE_AUDIO_DEVOUT_ATTACK_FIRST", "core-audio-devout-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsCoreAudioDevoutFlags"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
}

void
ags_core_audio_devout_class_init(AgsCoreAudioDevoutClass *core_audio_devout)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_core_audio_devout_parent_class = g_type_class_peek_parent(core_audio_devout);

  /* GObjectClass */
  gobject = (GObjectClass *) core_audio_devout;

  gobject->set_property = ags_core_audio_devout_set_property;
  gobject->get_property = ags_core_audio_devout_get_property;

  gobject->dispose = ags_core_audio_devout_dispose;
  gobject->finalize = ags_core_audio_devout_finalize;

  /* properties */
  /**
   * AgsCoreAudioDevout:device:
   *
   * The core audio soundcard indentifier
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-core-audio-devout-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsCoreAudioDevout:dsp-channels:
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
   * AgsCoreAudioDevout:pcm-channels:
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
   * AgsCoreAudioDevout:format:
   *
   * The precision of the buffer
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("precision of buffer"),
				 i18n_pspec("The precision to use for a frame"),
				 1,
				 G_MAXUINT,
				 AGS_SOUNDCARD_FLOAT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsCoreAudioDevout:buffer-size:
   *
   * The buffer size
   * 
   * Since: 3.0.0
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
   * AgsCoreAudioDevout:samplerate:
   *
   * The samplerate
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("frames per second"),
				 i18n_pspec("The frames count played during a second"),
				 (guint) AGS_SOUNDCARD_MIN_SAMPLERATE,
				 (guint) AGS_SOUNDCARD_MAX_SAMPLERATE,
				 (guint) AGS_SOUNDCARD_DEFAULT_SAMPLERATE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsCoreAudioDevout:bpm:
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
   * AgsCoreAudioDevout:buffer:
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
   * AgsCoreAudioDevout:core-audio-client:
   *
   * The assigned #AgsCoreAudioClient
   * 
   * Since: 3.0.0
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
   * AgsCoreAudioDevout:core-audio-port:
   *
   * The assigned #AgsCoreAudioPort
   * 
   * Since: 3.0.0
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
ags_core_audio_devout_error_quark()
{
  return(g_quark_from_static_string("ags-core_audio_devout-error-quark"));
}

void
ags_core_audio_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_core_audio_devout_get_uuid;
  connectable->has_resource = ags_core_audio_devout_has_resource;

  connectable->is_ready = ags_core_audio_devout_is_ready;
  connectable->add_to_registry = ags_core_audio_devout_add_to_registry;
  connectable->remove_from_registry = ags_core_audio_devout_remove_from_registry;

  connectable->list_resource = ags_core_audio_devout_list_resource;
  connectable->xml_compose = ags_core_audio_devout_xml_compose;
  connectable->xml_parse = ags_core_audio_devout_xml_parse;

  connectable->is_connected = ags_core_audio_devout_is_connected;  
  connectable->connect = ags_core_audio_devout_connect;
  connectable->disconnect = ags_core_audio_devout_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_core_audio_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_core_audio_devout_set_device;
  soundcard->get_device = ags_core_audio_devout_get_device;
  
  soundcard->set_presets = ags_core_audio_devout_set_presets;
  soundcard->get_presets = ags_core_audio_devout_get_presets;

  soundcard->list_cards = ags_core_audio_devout_list_cards;
  soundcard->pcm_info = ags_core_audio_devout_pcm_info;
  soundcard->get_capability = ags_core_audio_devout_get_capability;
    
  soundcard->set_bpm = ags_core_audio_devout_set_bpm;
  soundcard->get_bpm = ags_core_audio_devout_get_bpm;

  soundcard->set_start_note_offset = ags_core_audio_devout_set_start_note_offset;
  soundcard->get_start_note_offset = ags_core_audio_devout_get_start_note_offset;
  
  soundcard->get_frame_clock = ags_core_audio_devout_get_frame_clock;

  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_core_audio_devout_is_starting;
  soundcard->is_playing = ags_core_audio_devout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_core_audio_devout_get_uptime;
  
  soundcard->play_init = ags_core_audio_devout_port_init;
  soundcard->play = ags_core_audio_devout_port_play;

  soundcard->record_init = NULL;
  soundcard->record = NULL;

  soundcard->stop = ags_core_audio_devout_port_free;

  soundcard->tic = ags_core_audio_devout_tic;
  soundcard->offset_changed = ags_core_audio_devout_offset_changed;

  soundcard->get_buffer = ags_core_audio_devout_get_buffer;
  soundcard->get_next_buffer = ags_core_audio_devout_get_next_buffer;
  soundcard->get_prev_buffer = ags_core_audio_devout_get_prev_buffer;

  soundcard->lock_buffer = ags_core_audio_devout_lock_buffer;
  soundcard->unlock_buffer = ags_core_audio_devout_unlock_buffer;

  soundcard->get_sub_block_count = ags_core_audio_devout_get_sub_block_count;

  soundcard->trylock_sub_block = ags_core_audio_devout_trylock_sub_block;
  soundcard->unlock_sub_block = ags_core_audio_devout_unlock_sub_block;
}

void
ags_core_audio_devout_init(AgsCoreAudioDevout *core_audio_devout)
{
  AgsConfig *config;

  gchar *str;
  gchar *segmentation;

  gdouble absolute_delay;
  guint denominator, numerator;
  guint i;
  
  config = ags_config_get_instance();

  /* flags */
  core_audio_devout->flags = 0;
  core_audio_devout->connectable_flags = 0;
  ags_atomic_int_set(&(core_audio_devout->sync_flags),
		     AGS_CORE_AUDIO_DEVOUT_PASS_THROUGH);

  /* devout mutex */
  g_rec_mutex_init(&(core_audio_devout->obj_mutex));

  /* uuid */
  core_audio_devout->uuid = ags_uuid_alloc();
  ags_uuid_generate(core_audio_devout->uuid);

  /* presets */
  core_audio_devout->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  core_audio_devout->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  core_audio_devout->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  core_audio_devout->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  core_audio_devout->format = AGS_SOUNDCARD_FLOAT;

  /* bpm */
  core_audio_devout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* start note offset */
  core_audio_devout->start_note_offset = 0;
  
  /* frame clock */
  core_audio_devout->frame_clock = ags_frame_clock_new();

  /* app buffer mutex */
  core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0;

  core_audio_devout->app_buffer_mutex = (GRecMutex **) g_malloc(AGS_CORE_AUDIO_DEVOUT_DEFAULT_APP_BUFFER_SIZE * sizeof(GRecMutex *));

  for(i = 0; i < AGS_CORE_AUDIO_DEVOUT_DEFAULT_APP_BUFFER_SIZE; i++){
    core_audio_devout->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(core_audio_devout->app_buffer_mutex[i]);
  }

  /* sub-block */
  core_audio_devout->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  core_audio_devout->sub_block_mutex = (GRecMutex **) g_malloc(AGS_CORE_AUDIO_DEVOUT_DEFAULT_APP_BUFFER_SIZE * core_audio_devout->sub_block_count * core_audio_devout->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < AGS_CORE_AUDIO_DEVOUT_DEFAULT_APP_BUFFER_SIZE * core_audio_devout->sub_block_count * core_audio_devout->pcm_channels; i++){
    core_audio_devout->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(core_audio_devout->sub_block_mutex[i]);
  }

  /* app buffer */
  core_audio_devout->app_buffer = (void **) g_malloc(AGS_CORE_AUDIO_DEVOUT_DEFAULT_APP_BUFFER_SIZE * sizeof(void*));

  core_audio_devout->app_buffer[0] = NULL;
  core_audio_devout->app_buffer[1] = NULL;
  core_audio_devout->app_buffer[2] = NULL;
  core_audio_devout->app_buffer[3] = NULL;
  core_audio_devout->app_buffer[4] = NULL;
  core_audio_devout->app_buffer[5] = NULL;
  core_audio_devout->app_buffer[6] = NULL;
  core_audio_devout->app_buffer[7] = NULL;
  
  ags_core_audio_devout_realloc_buffer(core_audio_devout);
  
  /*  */
  core_audio_devout->card_uri = NULL;
  core_audio_devout->core_audio_client = NULL;

  core_audio_devout->port_name = NULL;
  core_audio_devout->core_audio_port = NULL;

  /* callback mutex */
  g_mutex_init(&(core_audio_devout->callback_mutex));

  g_cond_init(&(core_audio_devout->callback_cond));

  /* callback finish mutex */
  g_mutex_init(&(core_audio_devout->callback_finish_mutex));

  g_cond_init(&(core_audio_devout->callback_finish_cond));

  /* audio device */
  core_audio_devout->device_id = NULL;
  core_audio_devout->device_name = NULL;

  core_audio_devout->audio_device = 0;
}

void
ags_core_audio_devout_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(gobject);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      ags_soundcard_set_device(AGS_SOUNDCARD(core_audio_devout),
			       device);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      g_rec_mutex_lock(core_audio_devout_mutex);

      if(dsp_channels == core_audio_devout->dsp_channels){
	g_rec_mutex_unlock(core_audio_devout_mutex);

	return;
      }

      core_audio_devout->dsp_channels = dsp_channels;

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      GList *port;

      guint pcm_channels, old_pcm_channels;
      guint i;
      gboolean is_registered;

      pcm_channels = g_value_get_uint(value);

      g_rec_mutex_lock(core_audio_devout_mutex);

      if(pcm_channels == core_audio_devout->pcm_channels){
	g_rec_mutex_unlock(core_audio_devout_mutex);

	return;
      }

      old_pcm_channels = core_audio_devout->pcm_channels;

      /* destroy if less pcm-channels */
      for(i = AGS_CORE_AUDIO_DEVOUT_DEFAULT_APP_BUFFER_SIZE * core_audio_devout->sub_block_count * pcm_channels; i < AGS_CORE_AUDIO_DEVOUT_DEFAULT_APP_BUFFER_SIZE * core_audio_devout->sub_block_count * old_pcm_channels; i++){
	g_rec_mutex_clear(core_audio_devout->sub_block_mutex[i]);

	g_free(core_audio_devout->sub_block_mutex[i]);
      }

      core_audio_devout->sub_block_mutex = (GRecMutex **) g_realloc(core_audio_devout->sub_block_mutex,
								    AGS_CORE_AUDIO_DEVOUT_DEFAULT_APP_BUFFER_SIZE * core_audio_devout->sub_block_count * pcm_channels * sizeof(GRecMutex *));

      /* create if more pcm-channels */
      for(i = AGS_CORE_AUDIO_DEVOUT_DEFAULT_APP_BUFFER_SIZE * core_audio_devout->sub_block_count * old_pcm_channels; i < AGS_CORE_AUDIO_DEVOUT_DEFAULT_APP_BUFFER_SIZE * core_audio_devout->sub_block_count * pcm_channels; i++){
	core_audio_devout->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

	g_rec_mutex_init(core_audio_devout->sub_block_mutex[i]);
      }

      core_audio_devout->pcm_channels = pcm_channels;

      g_rec_mutex_unlock(core_audio_devout_mutex);

      ags_core_audio_devout_realloc_buffer(core_audio_devout);
      
      /* reset port */
      port = core_audio_devout->core_audio_port;

      while(port != NULL){
	is_registered = (ags_core_audio_port_test_flags((AgsCoreAudioPort *) port->data, AGS_CORE_AUDIO_PORT_REGISTERED)) ? TRUE: FALSE;
	
	if(is_registered){
	  ags_core_audio_port_unregister(port->data);
	}
	
	g_object_set((GObject *) port->data,
		     "pcm-channels", pcm_channels,
		     NULL);

	if(is_registered){
	  ags_core_audio_port_register(port->data,
				       core_audio_devout->device_id,
				       TRUE, FALSE,
				       TRUE);
	}
	
	port = port->next;
      }
    }
    break;
  case PROP_FORMAT:
    {
      GList *port;

      guint format;
      gboolean is_registered;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(core_audio_devout_mutex);

      if(format == core_audio_devout->format){
	g_rec_mutex_unlock(core_audio_devout_mutex);

	return;
      }

      core_audio_devout->format = format;

      g_rec_mutex_unlock(core_audio_devout_mutex);

      ags_core_audio_devout_realloc_buffer(core_audio_devout);
      
#if 0
      port = core_audio_devout->core_audio_port;

      while(port != NULL){
	is_registered = (ags_core_audio_port_test_flags((AgsCoreAudioPort *) port->data, AGS_CORE_AUDIO_PORT_REGISTERED)) ? TRUE: FALSE;

	if(is_registered){
	  ags_core_audio_port_unregister((AgsCoreAudioPort *) port->data);
	}
	
	g_object_set((GObject *) port->data,
		     "format", format,
		     NULL);

	if(is_registered){
	  ags_core_audio_port_register((AgsCoreAudioPort *) port->data,
				       core_audio_devout->device_id,
				       TRUE, FALSE,
				       TRUE);
	}
	
	port = port->next;
      }
#endif
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      GList *port;

      guint buffer_size;
      gboolean is_registered;

      buffer_size = g_value_get_uint(value);

      g_rec_mutex_lock(core_audio_devout_mutex);

      if(buffer_size == core_audio_devout->buffer_size){
	g_rec_mutex_unlock(core_audio_devout_mutex);

	return;
      }

      core_audio_devout->buffer_size = buffer_size;

      ags_frame_clock_set_buffer_size(core_audio_devout->frame_clock,
				      buffer_size);
      
      g_rec_mutex_unlock(core_audio_devout_mutex);

      ags_core_audio_devout_realloc_buffer(core_audio_devout);

      /* reset port */
      port = core_audio_devout->core_audio_port;

      while(port != NULL){
	is_registered = (ags_core_audio_port_test_flags((AgsCoreAudioPort *) port->data, AGS_CORE_AUDIO_PORT_REGISTERED)) ? TRUE: FALSE;

	if(is_registered){
	  ags_core_audio_port_unregister((AgsCoreAudioPort *) port->data);
	}
	
	g_object_set((GObject *) port->data,
		     "buffer-size", buffer_size,
		     NULL);
	
	if(is_registered){
	  ags_core_audio_port_register((AgsCoreAudioPort *) port->data,
				       core_audio_devout->device_id,
				       TRUE, FALSE,
				       TRUE);
	}
	
	port = port->next;
      }
    }
    break;
  case PROP_SAMPLERATE:
    {
      GList *port;

      guint samplerate;
      gboolean is_registered;

      samplerate = g_value_get_uint(value);

      g_rec_mutex_lock(core_audio_devout_mutex);
      
      if(samplerate == core_audio_devout->samplerate){
	g_rec_mutex_unlock(core_audio_devout_mutex);

	return;
      }

      core_audio_devout->samplerate = samplerate;

      ags_frame_clock_set_samplerate(core_audio_devout->frame_clock,
				     samplerate);
      
      g_rec_mutex_unlock(core_audio_devout_mutex);
      
      ags_core_audio_devout_realloc_buffer(core_audio_devout);
      
      /* reset port */
      port = core_audio_devout->core_audio_port;

      while(port != NULL){
	is_registered = (ags_core_audio_port_test_flags((AgsCoreAudioPort *) port->data, AGS_CORE_AUDIO_PORT_REGISTERED)) ? TRUE: FALSE;

	if(is_registered){
	  ags_core_audio_port_unregister((AgsCoreAudioPort *) port->data);
	}
	
	g_object_set((GObject *) port->data,
		     "samplerate", samplerate,
		     NULL);

	if(is_registered){
	  ags_core_audio_port_register((AgsCoreAudioPort *) port->data,
				       core_audio_devout->device_id,
				       TRUE, FALSE,
				       TRUE);
	}
	
	port = port->next;
      }
    }
    break;
  case PROP_BPM:
    {
      gdouble bpm;
      
      bpm = g_value_get_double(value);

      g_rec_mutex_lock(core_audio_devout_mutex);

      core_audio_devout->bpm = bpm;

      ags_frame_clock_set_bpm(core_audio_devout->frame_clock,
			      bpm);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      //TODO:JK: implement me
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      AgsCoreAudioClient *core_audio_client;

      core_audio_client = (AgsCoreAudioClient *) g_value_get_object(value);

      g_rec_mutex_lock(core_audio_devout_mutex);

      if(core_audio_devout->core_audio_client == (GObject *) core_audio_client){
	g_rec_mutex_unlock(core_audio_devout_mutex);

	return;
      }

      if(core_audio_devout->core_audio_client != NULL){
	g_object_unref(G_OBJECT(core_audio_devout->core_audio_client));
      }

      if(core_audio_client != NULL){
	g_object_ref(core_audio_client);
      }
      
      core_audio_devout->core_audio_client = (GObject *) core_audio_client;

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      AgsCoreAudioPort *core_audio_port;

      core_audio_port = (AgsCoreAudioPort *) g_value_get_pointer(value);

      g_rec_mutex_lock(core_audio_devout_mutex);

      if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port) ||
	 g_list_find(core_audio_devout->core_audio_port, core_audio_port) != NULL){
	g_rec_mutex_unlock(core_audio_devout_mutex);

	return;
      }

      g_object_ref(core_audio_port);
      core_audio_devout->core_audio_port = g_list_append(core_audio_devout->core_audio_port,
							 core_audio_port);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_devout_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(gobject);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_string(value, core_audio_devout->device_name);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_uint(value, core_audio_devout->dsp_channels);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_uint(value, core_audio_devout->pcm_channels);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_uint(value, core_audio_devout->format);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_uint(value, core_audio_devout->buffer_size);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_uint(value, core_audio_devout->samplerate);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_double(value, core_audio_devout->bpm);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_pointer(value, core_audio_devout->app_buffer);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_object(value, core_audio_devout->core_audio_client);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(core_audio_devout->core_audio_port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_devout_dispose(GObject *gobject)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(gobject);

  /* core audio client */
  if(core_audio_devout->core_audio_client != NULL){
    g_object_unref(core_audio_devout->core_audio_client);

    core_audio_devout->core_audio_client = NULL;
  }

  /* core audio port */
  g_list_free_full(core_audio_devout->core_audio_port,
		   g_object_unref);

  core_audio_devout->core_audio_port = NULL;

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_devout_parent_class)->dispose(gobject);
}

void
ags_core_audio_devout_finalize(GObject *gobject)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint i;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(gobject);

  ags_uuid_free(core_audio_devout->uuid);

  /* frame clock */
  if(core_audio_devout->frame_clock != NULL){
    g_object_unref(core_audio_devout->frame_clock);
  }

  /* app buffer */
  for(i = 0; i < AGS_CORE_AUDIO_DEVOUT_DEFAULT_APP_BUFFER_SIZE; i++){
    g_free(core_audio_devout->app_buffer[i]);
  }

  g_free(core_audio_devout->app_buffer);

  /* app buffer mutex */
  for(i = 0; i < AGS_CORE_AUDIO_DEVOUT_DEFAULT_APP_BUFFER_SIZE; i++){
    g_rec_mutex_clear(core_audio_devout->app_buffer_mutex[i]);

    g_free(core_audio_devout->app_buffer_mutex[i]);
  }
  
  g_free(core_audio_devout->app_buffer_mutex);

  /* sub-block mutex */
  for(i = 0; i < AGS_CORE_AUDIO_DEVOUT_DEFAULT_APP_BUFFER_SIZE * core_audio_devout->sub_block_count * core_audio_devout->pcm_channels; i++){
    g_rec_mutex_clear(core_audio_devout->sub_block_mutex[i]);

    g_free(core_audio_devout->sub_block_mutex[i]);
  }

  g_free(core_audio_devout->sub_block_mutex);
  
  /* core audio client */
  if(core_audio_devout->core_audio_client != NULL){
    g_object_unref(core_audio_devout->core_audio_client);
  }

  /* core audio port */
  g_list_free_full(core_audio_devout->core_audio_port,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_devout_parent_class)->finalize(gobject);
}

AgsUUID*
ags_core_audio_devout_get_uuid(AgsConnectable *connectable)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  AgsUUID *ptr;

  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(connectable);

  /* get core audio devout signal mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get UUID */
  g_rec_mutex_lock(core_audio_devout_mutex);

  ptr = core_audio_devout->uuid;

  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  return(ptr);
}

gboolean
ags_core_audio_devout_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_core_audio_devout_is_ready(AgsConnectable *connectable)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  gboolean is_ready;

  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(connectable);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* check is ready */
  g_rec_mutex_lock(core_audio_devout_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (core_audio_devout->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  return(is_ready);
}

void
ags_core_audio_devout_add_to_registry(AgsConnectable *connectable)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(connectable);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}

void
ags_core_audio_devout_remove_from_registry(AgsConnectable *connectable)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(connectable);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}

xmlNode*
ags_core_audio_devout_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_core_audio_devout_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_core_audio_devout_xml_parse(AgsConnectable *connectable,
				xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_core_audio_devout_is_connected(AgsConnectable *connectable)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  gboolean is_connected;

  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(connectable);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* check is connected */
  g_rec_mutex_lock(core_audio_devout_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (core_audio_devout->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  return(is_connected);
}

void
ags_core_audio_devout_connect(AgsConnectable *connectable)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(connectable);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}

void
ags_core_audio_devout_disconnect(AgsConnectable *connectable)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(connectable);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}

/**
 * ags_core_audio_devout_test_flags:
 * @core_audio_devout: the #AgsCoreAudioDevout
 * @flags: the flags
 *
 * Test @flags to be set on @core_audio_devout.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_core_audio_devout_test_flags(AgsCoreAudioDevout *core_audio_devout, AgsCoreAudioDevoutFlags flags)
{
  gboolean retval;  
  
  GRecMutex *core_audio_devout_mutex;

  if(!AGS_IS_CORE_AUDIO_DEVOUT(core_audio_devout)){
    return(FALSE);
  }

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* test */
  g_rec_mutex_lock(core_audio_devout_mutex);

  retval = (flags & (core_audio_devout->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(retval);
}

/**
 * ags_core_audio_devout_set_flags:
 * @core_audio_devout: the #AgsCoreAudioDevout
 * @flags: see #AgsCoreAudioDevoutFlags-enum
 *
 * Enable a feature of @core_audio_devout.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_devout_set_flags(AgsCoreAudioDevout *core_audio_devout, AgsCoreAudioDevoutFlags flags)
{
  GRecMutex *core_audio_devout_mutex;

  if(!AGS_IS_CORE_AUDIO_DEVOUT(core_audio_devout)){
    return;
  }

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->flags |= flags;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}
    
/**
 * ags_core_audio_devout_unset_flags:
 * @core_audio_devout: the #AgsCoreAudioDevout
 * @flags: see #AgsCoreAudioDevoutFlags-enum
 *
 * Disable a feature of @core_audio_devout.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_devout_unset_flags(AgsCoreAudioDevout *core_audio_devout, AgsCoreAudioDevoutFlags flags)
{  
  GRecMutex *core_audio_devout_mutex;

  if(!AGS_IS_CORE_AUDIO_DEVOUT(core_audio_devout)){
    return;
  }

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->flags &= (~flags);
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}

void
ags_core_audio_devout_set_device(AgsSoundcard *soundcard,
				 gchar *device)
{
  AgsCoreAudioDevout *core_audio_devout;
  
#if defined(AGS_WITH_CORE_AUDIO)
  AudioDeviceID *audio_devices;
  
  NSString *device_uid = @"";

  AudioObjectPropertyAddress devices_property_address;
  AudioObjectPropertyAddress streams_property_address;
  
  struct AudioStreamBasicDescription stream_desc;
#endif
  
  GList *core_audio_port, *start_core_audio_port;

  gchar *str;
  
#if defined(AGS_WITH_CORE_AUDIO)
  int device_count;
  int stream_count;
  int is_mic;
  int is_speaker;
  UInt32 prop_size;
  int i;
  OSStatus error;
#endif
  gboolean is_registered;

  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* check device */
  g_rec_mutex_lock(core_audio_devout_mutex);

  if(core_audio_devout->device_name == device ||
     (core_audio_devout->device_name != NULL &&
      !g_ascii_strcasecmp(core_audio_devout->device_name,
			  device))){
    g_rec_mutex_unlock(core_audio_devout_mutex);
  
    return;
  }

  g_message("output set device - %s", device);
  
  /* get some fields */
  start_core_audio_port = 
    core_audio_port = g_list_copy(core_audio_devout->core_audio_port);

  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  /* unregister */
  is_registered = FALSE;
  
  if(start_core_audio_port != NULL){
    is_registered = (ags_core_audio_port_test_flags((AgsCoreAudioPort *) start_core_audio_port->data, AGS_CORE_AUDIO_PORT_REGISTERED)) ? TRUE: FALSE;

    if(is_registered){
      ags_core_audio_port_unregister(start_core_audio_port->data);
    }
  }
  
  g_free(core_audio_devout->device_name);

  core_audio_devout->device_name = NULL;

  g_free(core_audio_devout->device_id);

  core_audio_devout->device_id = NULL;
  
#if defined(AGS_WITH_CORE_AUDIO)
  devices_property_address.mSelector = kAudioHardwarePropertyDevices;
  devices_property_address.mScope = kAudioObjectPropertyScopeGlobal;
  devices_property_address.mElement = kAudioObjectPropertyElementMain;

  streams_property_address.mSelector = kAudioDevicePropertyStreams;
  streams_property_address.mScope = kAudioDevicePropertyScopeOutput;

  error = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &devices_property_address, 0, NULL, &prop_size);
  
  if(error == noErr){
    device_count = prop_size / sizeof(AudioDeviceID);
    
    audio_devices = (AudioDeviceID *) malloc(prop_size);
    
    error = AudioObjectGetPropertyData(kAudioObjectSystemObject, &devices_property_address, 0, NULL, &prop_size, audio_devices);
    
    if(error == noErr) {
      for(i = 0; i < device_count; i++){
	NSString *current_manufacturer, *current_name, *current_uid;
	
	prop_size = sizeof(CFStringRef);
	
	devices_property_address.mSelector = kAudioDevicePropertyDeviceManufacturerCFString;
	error = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_manufacturer);
	
	if(error != noErr){
	  current_manufacturer = @"";
	  
	  //	  continue;
	}
	
	devices_property_address.mSelector = kAudioDevicePropertyDeviceNameCFString;
	error = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_name);
	
	if(error != noErr){
	  current_name = @"";

	  //	  continue;
	}
	
	devices_property_address.mSelector = kAudioDevicePropertyDeviceUID;
	error = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_uid);
	
	if(error != noErr){
	  current_uid = @"";
	  
	  //	  continue;
	}
	
	if([current_manufacturer isEqualToString:@"Apple Inc."] && [current_name isEqualToString:@"Built-in Output"]){
	  device_uid = current_uid;
	}

	is_speaker = 0;

	error = AudioObjectGetPropertyDataSize(audio_devices[i], 
					       &streams_property_address, 
					       0, 
					       NULL, 
					       &prop_size);
	
	stream_count = prop_size / sizeof(AudioStreamID);

	if(stream_count > 0){
	  is_speaker = YES;
	}

	str = g_strdup_printf("%s - %s",
			      [current_manufacturer UTF8String],
			      [current_name UTF8String]);

	//	g_message("test device %s - %s", device, str);
	
	if(is_speaker &&
	   !g_ascii_strcasecmp(str, device)){
	  //	  g_message(" `- success");
	  
	  core_audio_devout->device_name = g_strdup(device);
	  
	  core_audio_devout->device_id = g_strdup_printf("out-%s",
							 [current_uid UTF8String]);

	  core_audio_devout->audio_device = audio_devices[i];

	  g_free(str);
	  
	  break;
	}

	g_free(str);
      }
    }
    
    free(audio_devices);
  }
#endif
  
  /* apply name to port */
  str = g_strdup(core_audio_devout->device_id);
  
  if(start_core_audio_port != NULL){
    g_object_set(start_core_audio_port->data,
		 "port-name", str,
		 NULL);

    if(is_registered){
      ags_core_audio_port_register(start_core_audio_port->data,
				   str,
				   TRUE, FALSE,
				   TRUE);
    }
  }
  
  g_list_free(start_core_audio_port);

  g_free(str);
}

gchar*
ags_core_audio_devout_get_device(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  gchar *device;

  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  device = NULL;

  /* get device */
  g_rec_mutex_lock(core_audio_devout_mutex);

  device = g_strdup(core_audio_devout->device_name);

  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  return(device);
}

void
ags_core_audio_devout_set_presets(AgsSoundcard *soundcard,
				  guint channels,
				  guint rate,
				  guint buffer_size,
				  AgsSoundcardFormat format)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  g_object_set(core_audio_devout,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_core_audio_devout_get_presets(AgsSoundcard *soundcard,
				  guint *channels,
				  guint *rate,
				  guint *buffer_size,
				  AgsSoundcardFormat *format)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get presets */
  g_rec_mutex_lock(core_audio_devout_mutex);

  if(channels != NULL){
    *channels = core_audio_devout->pcm_channels;
  }

  if(rate != NULL){
    *rate = core_audio_devout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = core_audio_devout->buffer_size;
  }

  if(format != NULL){
    *format = core_audio_devout->format;
  }

  g_rec_mutex_unlock(core_audio_devout_mutex);
}

void
ags_core_audio_devout_list_cards(AgsSoundcard *soundcard,
				 GList **card_id, GList **card_name)
{
  AgsCoreAudioDevout *core_audio_devout;
  
#if defined(AGS_WITH_CORE_AUDIO)
  AudioDeviceID *audio_devices;
  
  NSString *device_uid = @"";

  AudioObjectPropertyAddress devices_property_address;
  AudioObjectPropertyAddress streams_property_address;
  
  struct AudioStreamBasicDescription stream_desc;
#endif
  
#if defined(AGS_WITH_CORE_AUDIO)  
  int device_count;
  int stream_count;
  int is_mic;
  int is_speaker;
  UInt32 prop_size;
  int i;
  OSStatus error;
#endif
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }
  
#if defined(AGS_WITH_CORE_AUDIO)
  devices_property_address.mSelector = kAudioHardwarePropertyDevices;
  devices_property_address.mScope = kAudioObjectPropertyScopeGlobal;
  devices_property_address.mElement = kAudioObjectPropertyElementMain;

  streams_property_address.mSelector = kAudioDevicePropertyStreams;
  streams_property_address.mScope = kAudioDevicePropertyScopeOutput;

  error = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &devices_property_address, 0, NULL, &prop_size);
  
  if(error == noErr){
    device_count = prop_size / sizeof(AudioDeviceID);
    
    audio_devices = (AudioDeviceID *) malloc(prop_size);
    
    error = AudioObjectGetPropertyData(kAudioObjectSystemObject, &devices_property_address, 0, NULL, &prop_size, audio_devices);
    
    if(error == noErr) {
      for(i = 0; i < device_count; i++){
	NSString *current_manufacturer, *current_name, *current_uid;
	
	prop_size = sizeof(CFStringRef);
	
	devices_property_address.mSelector = kAudioDevicePropertyDeviceManufacturerCFString;
	error = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_manufacturer);
	
	if(error != noErr){
	  current_manufacturer = @"";
	  
	  //	  continue;
	}
	
	devices_property_address.mSelector = kAudioDevicePropertyDeviceNameCFString;
	error = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_name);
	
	if(error != noErr){
	  current_name = @"";

	  //	  continue;
	}
	
	devices_property_address.mSelector = kAudioDevicePropertyDeviceUID;
	error = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_uid);
	
	if(error != noErr){
	  current_uid = @"";
	  
	  //	  continue;
	}
	
	if([current_manufacturer isEqualToString:@"Apple Inc."] && [current_name isEqualToString:@"Built-in Output"]){
	  device_uid = current_uid;
	}

	is_speaker = 0;

	error = AudioObjectGetPropertyDataSize(audio_devices[i], 
					       &streams_property_address, 
					       0, 
					       NULL, 
					       &prop_size);
	
	stream_count = prop_size / sizeof(AudioStreamID);

	if(stream_count > 0){
	  is_speaker = YES;
	}

	//	g_message("found %s device: %s - %s <%s>", (is_speaker ? "output": "input"),  [current_manufacturer UTF8String], [current_name UTF8String], [current_uid UTF8String]);

	if(is_speaker){
	  *card_id = g_list_prepend(*card_id,
				    g_strdup_printf("out-%s", [current_uid UTF8String]));
       
	  *card_name = g_list_prepend(*card_name,
				      g_strdup_printf("%s - %s", [current_manufacturer UTF8String], [current_name UTF8String]));
	}
      }
    }
    
    free(audio_devices);
  }
#endif
  
  if(card_id != NULL && *card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL && *card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

void
ags_core_audio_devout_pcm_info(AgsSoundcard *soundcard,
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
ags_core_audio_devout_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_PLAYBACK);
}

gboolean
ags_core_audio_devout_is_starting(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  gboolean is_starting;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* check is starting */
  is_starting = ags_core_audio_devout_test_flags(core_audio_devout, AGS_CORE_AUDIO_DEVOUT_START_PLAY);

  return(is_starting);
}

gboolean
ags_core_audio_devout_is_playing(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  gboolean is_playing;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* check is playing */
  is_playing = ags_core_audio_devout_test_flags(core_audio_devout, AGS_CORE_AUDIO_DEVOUT_PLAY);

  return(is_playing);
}

gchar*
ags_core_audio_devout_get_uptime(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  gchar *uptime;

  GRecMutex *core_audio_devout_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get time string */
  g_rec_mutex_lock(core_audio_devout_mutex);
  
  uptime = ags_frame_clock_to_time_string(core_audio_devout->frame_clock);

  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  return(uptime);
}

void
ags_core_audio_devout_port_init(AgsSoundcard *soundcard,
				GError **error)
{
  AgsCoreAudioPort *core_audio_port;
  AgsCoreAudioDevout *core_audio_devout;

  AgsSoundcardFormat format;
  guint word_size;
  gboolean use_cache;

  GRecMutex *core_audio_port_mutex;
  GRecMutex *core_audio_devout_mutex;
  
  if(ags_soundcard_is_playing(soundcard)){
    return;
  }

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  //  g_message("Core Audio init");
  
  /* get core-audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* port */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_port = (AgsCoreAudioPort *) core_audio_devout->core_audio_port->data;

  g_rec_mutex_unlock(core_audio_devout_mutex);

  /* get port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  /* check use cache */
  g_rec_mutex_lock(core_audio_port_mutex);

  use_cache = core_audio_port->use_cache;

  if(use_cache){
    core_audio_port->completed_cache = 0;
    core_audio_port->current_cache = 1;
  }
  
  g_rec_mutex_unlock(core_audio_port_mutex);
  
  /* retrieve word size */
  g_rec_mutex_lock(core_audio_devout_mutex);

  switch(core_audio_devout->format){
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
  case AGS_SOUNDCARD_FLOAT:
    {
      word_size = sizeof(gfloat);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      word_size = sizeof(gdouble);
    }
    break;
  default:
    g_rec_mutex_unlock(core_audio_devout_mutex);
    
    g_warning("ags_core_audio_devout_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for playback */
  core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_7;
  core_audio_devout->flags |= (AGS_CORE_AUDIO_DEVOUT_START_PLAY |
			       AGS_CORE_AUDIO_DEVOUT_PLAY |
			       AGS_CORE_AUDIO_DEVOUT_NONBLOCKING);

  memset(core_audio_devout->app_buffer[0], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[1], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[2], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[3], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[4], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[5], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[6], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[7], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);

  /*  */
  ags_frame_clock_set_start_note_offset(core_audio_devout->frame_clock,
					core_audio_devout->start_note_offset);

  ags_frame_clock_start(core_audio_devout->frame_clock);
  
  core_audio_devout->flags |= (AGS_CORE_AUDIO_DEVOUT_INITIALIZED |
			       AGS_CORE_AUDIO_DEVOUT_START_PLAY |
			       AGS_CORE_AUDIO_DEVOUT_PLAY);
  
  ags_atomic_int_or(&(core_audio_devout->sync_flags),
		    AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK);

  g_rec_mutex_unlock(core_audio_devout_mutex);
}

void
ags_core_audio_devout_port_play(AgsSoundcard *soundcard,
				GError **error)
{
  AgsCoreAudioClient *core_audio_client;
  AgsCoreAudioDevout *core_audio_devout;
  AgsCoreAudioPort *core_audio_port;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
      
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;

  gboolean initial_run;
  guint word_size;
  gboolean use_cache;
  gboolean core_audio_client_activated;

  GRecMutex *core_audio_devout_mutex;
  GRecMutex *core_audio_client_mutex;
  GRecMutex *core_audio_port_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  application_context = ags_application_context_get_instance();  
  
  /* get core-audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* client */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_client = (AgsCoreAudioClient *) core_audio_devout->core_audio_client;
  core_audio_port = (AgsCoreAudioPort *) core_audio_devout->core_audio_port->data;
  
  callback_mutex = &(core_audio_devout->callback_mutex);
  callback_finish_mutex = &(core_audio_devout->callback_finish_mutex);

  /* do playback */    
  initial_run = ((AGS_CORE_AUDIO_DEVOUT_START_PLAY & (core_audio_devout->flags)) != 0) ? TRUE: FALSE;

  core_audio_devout->flags &= (~AGS_CORE_AUDIO_DEVOUT_START_PLAY);

  if((AGS_CORE_AUDIO_DEVOUT_INITIALIZED & (core_audio_devout->flags)) == 0){
    g_rec_mutex_unlock(core_audio_devout_mutex);
    
    return;
  }
  
  //  g_message("Core Audio play");

  switch(core_audio_devout->format){
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
  case AGS_SOUNDCARD_FLOAT:
    {
      word_size = sizeof(gfloat);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      word_size = sizeof(gdouble);
    }
    break;
  default:
    g_rec_mutex_unlock(core_audio_devout_mutex);
    
    g_warning("ags_core_audio_devout_port_play(): unsupported word size");
    
    return;
  }

  g_rec_mutex_unlock(core_audio_devout_mutex);

  /* get port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  /* check use cache */
  g_rec_mutex_lock(core_audio_port_mutex);

  use_cache = core_audio_port->use_cache;
  
  g_rec_mutex_unlock(core_audio_port_mutex);

  if(use_cache){
    void *buffer;
    
    guint current_cache;
    guint completed_cache;
    guint write_cache;
    guint cache_buffer_size;
    guint cache_offset;
    guint pcm_channels;
    guint buffer_size;
    AgsSoundcardFormat format;
    
    struct timespec idle_time = {
      0,
      0,
    };

    idle_time.tv_nsec = ags_core_audio_port_get_latency(core_audio_port) / 8;

    g_rec_mutex_lock(core_audio_port_mutex);

    completed_cache = core_audio_port->completed_cache;

    cache_buffer_size = core_audio_port->cache_buffer_size;
    
    cache_offset = core_audio_port->cache_offset;
    
    g_rec_mutex_unlock(core_audio_port_mutex);

    if(completed_cache == 3){
      write_cache = 0;
    }else{
      write_cache = completed_cache + 1;
    }

    /* wait until ready */
    g_rec_mutex_lock(core_audio_port_mutex);

    current_cache = core_audio_port->current_cache;
  
    g_rec_mutex_unlock(core_audio_port_mutex);
    
    while(write_cache == current_cache){
      ags_time_nanosleep(&idle_time);

      g_rec_mutex_lock(core_audio_port_mutex);

      current_cache = core_audio_port->current_cache;

      g_rec_mutex_unlock(core_audio_port_mutex);
    }

    /* fill cache */
    g_rec_mutex_lock(core_audio_devout_mutex);

    pcm_channels = core_audio_devout->pcm_channels;
    buffer_size = core_audio_devout->buffer_size;
    format = core_audio_devout->format;
    
    g_rec_mutex_unlock(core_audio_devout_mutex);

    buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(core_audio_devout));

    // g_message("w %d", write_cache);
    
    switch(format){
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	if(cache_offset == 0){
	  ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					     core_audio_port->cache[write_cache], 1,
					     pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S16);
	}

	if(buffer != NULL){
	  ags_audio_buffer_util_copy_s16_to_s16(core_audio_port->audio_buffer_util,
						(gint16 *) core_audio_port->cache[write_cache] + (pcm_channels * cache_offset), 1,
						(gint16 *) buffer, 1,
						pcm_channels * buffer_size);
	}
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	if(cache_offset == 0){
	  ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					     core_audio_port->cache[write_cache], 1,
					     pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
	}

	if(buffer != NULL){
	  ags_audio_buffer_util_copy_s24_to_s24(core_audio_port->audio_buffer_util,
						(gint32 *) core_audio_port->cache[write_cache] + (pcm_channels * cache_offset), 1,
						(gint32 *) buffer, 1,
						pcm_channels * buffer_size);
	}
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	if(cache_offset == 0){
	  ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					     core_audio_port->cache[write_cache], 1,
					     pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
	}

	if(buffer != NULL){
	  ags_audio_buffer_util_copy_s32_to_s32(core_audio_port->audio_buffer_util,
						(gint32 *) core_audio_port->cache[write_cache] + (pcm_channels * cache_offset), 1,
						(gint32 *) buffer, 1,
						pcm_channels * buffer_size);
	}
      }
      break;
    case AGS_SOUNDCARD_FLOAT:
      {
	if(cache_offset == 0){
	  ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					     core_audio_port->cache[write_cache], 1,
					     pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_FLOAT);
	}

	if(buffer != NULL){
	  ags_audio_buffer_util_copy_float_to_float(core_audio_port->audio_buffer_util,
						    (gfloat *) core_audio_port->cache[write_cache] + (pcm_channels * cache_offset), 1,
						    (gfloat *) buffer, 1,
						    pcm_channels * buffer_size);
	}
      }
      break;
    case AGS_SOUNDCARD_DOUBLE:
      {
	if(cache_offset == 0){
	  ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					     core_audio_port->cache[write_cache], 1,
					     pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
	}

	if(buffer != NULL){
	  ags_audio_buffer_util_copy_double_to_double(core_audio_port->audio_buffer_util,
						      (gdouble *) core_audio_port->cache[write_cache] + (pcm_channels * cache_offset), 1,
						      (gdouble *) buffer, 1,
						      pcm_channels * buffer_size);
	}
      }
      break;
    }
    
    /* seek cache */
    if(cache_offset + buffer_size >= cache_buffer_size){
      g_rec_mutex_lock(core_audio_port_mutex);
      
      core_audio_port->completed_cache = write_cache;
      core_audio_port->cache_offset = 0;
      
      g_rec_mutex_unlock(core_audio_port_mutex);
    }else{
      g_rec_mutex_lock(core_audio_port_mutex);
      
      core_audio_port->cache_offset += buffer_size;
      
      g_rec_mutex_unlock(core_audio_port_mutex);
    }
  }else{
    /* get client mutex */
    core_audio_client_mutex = AGS_CORE_AUDIO_CLIENT_GET_OBJ_MUTEX(core_audio_client);

    /* get activated */
    g_rec_mutex_lock(core_audio_client_mutex);

    core_audio_client_activated = ((AGS_CORE_AUDIO_CLIENT_ACTIVATED & (core_audio_client->flags)) != 0) ? TRUE: FALSE;

    g_rec_mutex_unlock(core_audio_client_mutex);

    if(core_audio_client_activated){
      while((AGS_CORE_AUDIO_DEVOUT_PASS_THROUGH & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	usleep(4);
      }

      if((AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	ags_atomic_int_or(&(core_audio_devout->sync_flags),
			  AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_WAIT);
	
	while(((AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0 ||
	       (AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_CALLBACK_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0) &&
	      ((AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_DONE & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0 ||
	       (AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_CALLBACK_DONE & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0)){
	  if((AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0 &&
	     (AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_CALLBACK_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	    ags_atomic_int_or(&(core_audio_devout->sync_flags),
			      AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_DONE);
	  }
	  
	  g_thread_yield();
	}
	
	ags_atomic_int_and(&(core_audio_devout->sync_flags),
			   (~AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK));

	ags_atomic_int_and(&(core_audio_devout->sync_flags),
			   (~(AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_WAIT |
			      AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_CALLBACK_WAIT |
			      AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_DONE |
			      AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_CALLBACK_DONE)));
      }

      /* signal */
      g_mutex_lock(callback_mutex);

      ags_atomic_int_or(&(core_audio_devout->sync_flags),
			AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE);
    
      if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	g_cond_signal(&(core_audio_devout->callback_cond));
      }

      g_mutex_unlock(callback_mutex);
      //    }
    
      /* wait callback */	
      g_mutex_lock(callback_finish_mutex);
    
      if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0){
	ags_atomic_int_or(&(core_audio_devout->sync_flags),
			  AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT);
    
	while((AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0 &&
	      (AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	  g_cond_wait(&(core_audio_devout->callback_finish_cond),
		      callback_finish_mutex);
	}
      }
      
      ags_atomic_int_and(&(core_audio_devout->sync_flags),
			 (~(AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT |
			    AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE)));
    
      g_mutex_unlock(callback_finish_mutex);
    }
  }

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;      
  
  /* tic soundcard */
  if(!initial_run){
    tic_device = ags_tic_device_new((GObject *) core_audio_devout);
    task = g_list_append(task,
			 tic_device);
  }
  
  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) core_audio_devout);
  task = g_list_append(task,
		       clear_buffer);
    
  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) core_audio_devout);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_core_audio_devout_port_free(AgsSoundcard *soundcard)
{
  AgsCoreAudioPort *core_audio_port;
  AgsCoreAudioDevout *core_audio_devout;

  guint pcm_channels;
  guint cache_buffer_size;
  guint word_size;
  gboolean use_cache;

  guint i;
  
  GRecMutex *core_audio_port_mutex;
  GRecMutex *core_audio_devout_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core-audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* port */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_port = (AgsCoreAudioPort *) core_audio_devout->core_audio_port->data;
  pcm_channels = core_audio_devout->pcm_channels;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  /* get port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  /* check use cache */
#if defined(AGS_CORE_AUDIO_PORT_USE_HW)
#else
  g_rec_mutex_lock(core_audio_port_mutex);

  cache_buffer_size = core_audio_port->cache_buffer_size;
  use_cache = core_audio_port->use_cache;

  if(use_cache){
    core_audio_port->completed_cache = 0;
    core_audio_port->current_cache = 0;
    core_audio_port->cache_offset = 0;
  }

  switch(core_audio_port->format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[0], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S16);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[1], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S16);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[2], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S16);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[3], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S16);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[0], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[1], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[2], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[3], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[0], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[1], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[2], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[3], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[0], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_FLOAT);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[1], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_FLOAT);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[2], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_FLOAT);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[3], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_FLOAT);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[0], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[1], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[2], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[3], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    }
    break;
  }

  g_rec_mutex_unlock(core_audio_port_mutex);
#endif
  
  /*  */
  g_rec_mutex_lock(core_audio_devout_mutex);

  if((AGS_CORE_AUDIO_DEVOUT_INITIALIZED & (core_audio_devout->flags)) == 0){
    g_rec_mutex_unlock(core_audio_devout_mutex);

    return;
  }

  callback_mutex = &(core_audio_devout->callback_mutex);
  callback_finish_mutex = &(core_audio_devout->callback_finish_mutex);
  
  //  ags_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);
  
  core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0;
  core_audio_devout->flags &= (~(AGS_CORE_AUDIO_DEVOUT_PLAY));

  ags_atomic_int_or(&(core_audio_devout->sync_flags),
		    AGS_CORE_AUDIO_DEVOUT_PASS_THROUGH);

  ags_atomic_int_and(&(core_audio_devout->sync_flags),
		     (~AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK));

  ags_atomic_int_and(&(core_audio_devout->sync_flags),
		     (~(AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_WAIT |
			AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_CALLBACK_WAIT |
			AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_DONE |
			AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_CALLBACK_DONE)));

  /* signal callback */
  g_mutex_lock(callback_mutex);

  ags_atomic_int_or(&(core_audio_devout->sync_flags),
		    AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE);
    
  if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
    g_cond_signal(&(core_audio_devout->callback_cond));
  }

  g_mutex_unlock(callback_mutex);

  /* signal thread */
  g_mutex_lock(callback_finish_mutex);

  ags_atomic_int_or(&(core_audio_devout->sync_flags),
		    AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE);
    
  if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
    g_cond_signal(&(core_audio_devout->callback_finish_cond));
  }

  g_mutex_unlock(callback_finish_mutex);
  
  /*  */
  ags_frame_clock_stop(core_audio_devout->frame_clock);

  switch(core_audio_devout->format){
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
  case AGS_SOUNDCARD_FLOAT:
    {
      word_size = sizeof(gfloat);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      word_size = sizeof(gdouble);
    }
    break;
  default:
    word_size = 0;
    
    g_critical("ags_core_audio_devout_free(): unsupported word size");
  }

  g_rec_mutex_unlock(core_audio_devout_mutex);

  if(core_audio_devout->core_audio_port != NULL){
    core_audio_port = core_audio_devout->core_audio_port->data;

    while(!ags_atomic_int_get(&(core_audio_port->is_empty))) usleep(500000);
  }

  g_rec_mutex_lock(core_audio_devout_mutex);
  
  memset(core_audio_devout->app_buffer[0], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[1], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[2], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[3], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[4], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[5], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[6], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[7], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  
  g_rec_mutex_unlock(core_audio_devout_mutex);  
}

void
ags_core_audio_devout_tic(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core_audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* frame clock */
  g_rec_mutex_lock(core_audio_devout_mutex);
  
  ags_frame_clock_increment_counter(core_audio_devout->frame_clock);

  /* 16th pulse */
  if(ags_frame_clock_get_has_16th_pulse(core_audio_devout->frame_clock)){
    ags_soundcard_offset_changed(soundcard,
				 core_audio_devout->frame_clock->note_offset);
  }
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}

void
ags_core_audio_devout_offset_changed(AgsSoundcard *soundcard,
				     guint64 note_offset)
{
  //empty
}

void
ags_core_audio_devout_set_bpm(AgsSoundcard *soundcard,
			      gdouble bpm)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* set bpm */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->bpm = bpm;
  
  ags_frame_clock_set_bpm(core_audio_devout->frame_clock,
			  bpm);
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}

gdouble
ags_core_audio_devout_get_bpm(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  gdouble bpm;
  
  GRecMutex *core_audio_devout_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get bpm */
  g_rec_mutex_lock(core_audio_devout_mutex);

  bpm = core_audio_devout->bpm;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(bpm);
}

void
ags_core_audio_devout_set_start_note_offset(AgsSoundcard *soundcard,
					    guint64 start_note_offset)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* set start note offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->start_note_offset = start_note_offset;
  
  ags_frame_clock_set_start_note_offset(core_audio_devout->frame_clock,
					start_note_offset);
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}

guint64
ags_core_audio_devout_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint64 start_note_offset;
  
  GRecMutex *core_audio_devout_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get start note offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  start_note_offset = core_audio_devout->start_note_offset;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(start_note_offset);
}

GObject*
ags_core_audio_devout_get_frame_clock(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  GObject *frame_clock;
  
  GRecMutex *core_audio_devout_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get frame clock */
  g_rec_mutex_lock(core_audio_devout_mutex);

  frame_clock = (GObject *) core_audio_devout->frame_clock;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(frame_clock);
}

void*
ags_core_audio_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  void *buffer;

  GRecMutex *core_audio_devout_mutex;  
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  g_rec_mutex_lock(core_audio_devout_mutex);

  if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0){
    buffer = core_audio_devout->app_buffer[0];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_1){
    buffer = core_audio_devout->app_buffer[1];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_2){
    buffer = core_audio_devout->app_buffer[2];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_3){
    buffer = core_audio_devout->app_buffer[3];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_4){
    buffer = core_audio_devout->app_buffer[4];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_5){
    buffer = core_audio_devout->app_buffer[5];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_6){
    buffer = core_audio_devout->app_buffer[6];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_7){
    buffer = core_audio_devout->app_buffer[7];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(buffer);
}

void*
ags_core_audio_devout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  void *buffer;

  GRecMutex *core_audio_devout_mutex;  
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  g_rec_mutex_lock(core_audio_devout_mutex);

  if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0){
    buffer = core_audio_devout->app_buffer[1];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_1){
    buffer = core_audio_devout->app_buffer[2];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_2){
    buffer = core_audio_devout->app_buffer[3];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_3){
    buffer = core_audio_devout->app_buffer[4];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_4){
    buffer = core_audio_devout->app_buffer[5];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_5){
    buffer = core_audio_devout->app_buffer[6];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_6){
    buffer = core_audio_devout->app_buffer[7];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_7){
    buffer = core_audio_devout->app_buffer[0];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(buffer);
}

void*
ags_core_audio_devout_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  void *buffer;

  GRecMutex *core_audio_devout_mutex;  
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  g_rec_mutex_lock(core_audio_devout_mutex);

  if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0){
    buffer = core_audio_devout->app_buffer[7];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_1){
    buffer = core_audio_devout->app_buffer[0];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_2){
    buffer = core_audio_devout->app_buffer[1];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_3){
    buffer = core_audio_devout->app_buffer[2];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_4){
    buffer = core_audio_devout->app_buffer[3];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_5){
    buffer = core_audio_devout->app_buffer[4];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_6){
    buffer = core_audio_devout->app_buffer[5];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_7){
    buffer = core_audio_devout->app_buffer[6];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(buffer);
}

void
ags_core_audio_devout_lock_buffer(AgsSoundcard *soundcard,
				  void *buffer)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *buffer_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(core_audio_devout->app_buffer != NULL){
    if(buffer == core_audio_devout->app_buffer[0]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[0];
    }else if(buffer == core_audio_devout->app_buffer[1]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[1];
    }else if(buffer == core_audio_devout->app_buffer[2]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[2];
    }else if(buffer == core_audio_devout->app_buffer[3]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[3];
    }else if(buffer == core_audio_devout->app_buffer[4]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[4];
    }else if(buffer == core_audio_devout->app_buffer[5]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[5];
    }else if(buffer == core_audio_devout->app_buffer[6]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[6];
    }else if(buffer == core_audio_devout->app_buffer[7]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[7];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_core_audio_devout_unlock_buffer(AgsSoundcard *soundcard,
				    void *buffer)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *buffer_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(core_audio_devout->app_buffer != NULL){
    if(buffer == core_audio_devout->app_buffer[0]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[0];
    }else if(buffer == core_audio_devout->app_buffer[1]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[1];
    }else if(buffer == core_audio_devout->app_buffer[2]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[2];
    }else if(buffer == core_audio_devout->app_buffer[3]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[3];
    }else if(buffer == core_audio_devout->app_buffer[4]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[4];
    }else if(buffer == core_audio_devout->app_buffer[5]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[5];
    }else if(buffer == core_audio_devout->app_buffer[6]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[6];
    }else if(buffer == core_audio_devout->app_buffer[7]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[7];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_core_audio_devout_get_sub_block_count(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint sub_block_count;
  
  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get loop offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  sub_block_count = core_audio_devout->sub_block_count;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(sub_block_count);
}

gboolean
ags_core_audio_devout_trylock_sub_block(AgsSoundcard *soundcard,
					void *buffer, guint sub_block)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint pcm_channels;
  guint sub_block_count;
  gboolean success;
  
  GRecMutex *core_audio_devout_mutex;  
  GRecMutex *sub_block_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get loop offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  pcm_channels = core_audio_devout->pcm_channels;
  sub_block_count = core_audio_devout->sub_block_count;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  sub_block_mutex = NULL;

  success = FALSE;
  
  if(core_audio_devout->app_buffer != NULL){
    if(buffer == core_audio_devout->app_buffer[0]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[sub_block];
    }else if(buffer == core_audio_devout->app_buffer[1]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[2]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[3]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[4]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[4 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[5]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[5 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[6]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[6 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[7]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[7 * pcm_channels * sub_block_count + sub_block];
    }
  }

  if(sub_block_mutex != NULL){
    if(g_rec_mutex_trylock(sub_block_mutex)){
      success = TRUE;
    }
  }

  return(success);
}

void
ags_core_audio_devout_unlock_sub_block(AgsSoundcard *soundcard,
				       void *buffer, guint sub_block)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint pcm_channels;
  guint sub_block_count;
  
  GRecMutex *core_audio_devout_mutex;  
  GRecMutex *sub_block_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get loop offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  pcm_channels = core_audio_devout->pcm_channels;
  sub_block_count = core_audio_devout->sub_block_count;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  sub_block_mutex = NULL;
  
  if(core_audio_devout->app_buffer != NULL){
    if(buffer == core_audio_devout->app_buffer[0]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[sub_block];
    }else if(buffer == core_audio_devout->app_buffer[1]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[2]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[3]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[4]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[4 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[5]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[5 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[6]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[6 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[7]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[7 * pcm_channels * sub_block_count + sub_block];
    }
  }

  if(sub_block_mutex != NULL){
    g_rec_mutex_unlock(sub_block_mutex);
  }
}

/**
 * ags_core_audio_devout_switch_buffer_flag:
 * @core_audio_devout: an #AgsCoreAudioDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_devout_switch_buffer_flag(AgsCoreAudioDevout *core_audio_devout)
{
  GRecMutex *core_audio_devout_mutex;
  
  if(!AGS_IS_CORE_AUDIO_DEVOUT(core_audio_devout)){
    return;
  }

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* switch buffer flag */
  g_rec_mutex_lock(core_audio_devout_mutex);

  if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_1;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_1){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_2;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_2){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_3;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_3){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_4;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_4){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_5;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_5){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_6;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_6){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_7;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_7){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(core_audio_devout_mutex);
}

/**
 * ags_core_audio_devout_realloc_buffer:
 * @core_audio_devout: the #AgsCoreAudioDevout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_devout_realloc_buffer(AgsCoreAudioDevout *core_audio_devout)
{
  guint pcm_channels;
  guint buffer_size;
  AgsSoundcardFormat format;
  guint word_size;

  GRecMutex *core_audio_devout_mutex;  

  if(!AGS_IS_CORE_AUDIO_DEVOUT(core_audio_devout)){
    return;
  }

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get word size */  
  g_rec_mutex_lock(core_audio_devout_mutex);

  pcm_channels = core_audio_devout->pcm_channels;
  buffer_size = core_audio_devout->buffer_size;

  format = core_audio_devout->format;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

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
  case AGS_SOUNDCARD_FLOAT:
    {
      word_size = sizeof(gfloat);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      word_size = sizeof(gdouble);
    }
    break;
  default:
    g_warning("ags_core_audio_devout_realloc_buffer(): unsupported word size");
    return;
  }
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0 */
  if(core_audio_devout->app_buffer[0] != NULL){
    g_free(core_audio_devout->app_buffer[0]);
  }
  
  core_audio_devout->app_buffer[0] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devout->app_buffer[0], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_1 */
  if(core_audio_devout->app_buffer[1] != NULL){
    g_free(core_audio_devout->app_buffer[1]);
  }

  core_audio_devout->app_buffer[1] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  memset(core_audio_devout->app_buffer[1], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_2 */
  if(core_audio_devout->app_buffer[2] != NULL){
    g_free(core_audio_devout->app_buffer[2]);
  }

  core_audio_devout->app_buffer[2] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  memset(core_audio_devout->app_buffer[2], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_3 */
  if(core_audio_devout->app_buffer[3] != NULL){
    g_free(core_audio_devout->app_buffer[3]);
  }
  
  core_audio_devout->app_buffer[3] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devout->app_buffer[3], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_4 */
  if(core_audio_devout->app_buffer[4] != NULL){
    g_free(core_audio_devout->app_buffer[4]);
  }
  
  core_audio_devout->app_buffer[4] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devout->app_buffer[4], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_5 */
  if(core_audio_devout->app_buffer[5] != NULL){
    g_free(core_audio_devout->app_buffer[5]);
  }
  
  core_audio_devout->app_buffer[5] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devout->app_buffer[5], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_6 */
  if(core_audio_devout->app_buffer[6] != NULL){
    g_free(core_audio_devout->app_buffer[6]);
  }
  
  core_audio_devout->app_buffer[6] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devout->app_buffer[6], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_7 */
  if(core_audio_devout->app_buffer[7] != NULL){
    g_free(core_audio_devout->app_buffer[7]);
  }
  
  core_audio_devout->app_buffer[7] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devout->app_buffer[7], 0, pcm_channels * buffer_size * word_size);  
}

/**
 * ags_core_audio_devout_new:
 *
 * Creates a new instance of #AgsCoreAudioDevout.
 *
 * Returns: a new #AgsCoreAudioDevout
 *
 * Since: 3.0.0
 */
AgsCoreAudioDevout*
ags_core_audio_devout_new()
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = (AgsCoreAudioDevout *) g_object_new(AGS_TYPE_CORE_AUDIO_DEVOUT,
							  NULL);
  
  return(core_audio_devout);
}
