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

#include <ags/audio/audio-unit/ags_audio_unit_devout.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/audio-unit/ags_audio_unit_server.h>
#include <ags/audio/audio-unit/ags_audio_unit_client.h>
#include <ags/audio/audio-unit/ags_audio_unit_port.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <string.h>
#include <math.h>
#include <time.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_audio_unit_devout_class_init(AgsAudioUnitDevoutClass *audio_unit_devout);
void ags_audio_unit_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_unit_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_audio_unit_devout_init(AgsAudioUnitDevout *audio_unit_devout);
void ags_audio_unit_devout_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_audio_unit_devout_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_audio_unit_devout_dispose(GObject *gobject);
void ags_audio_unit_devout_finalize(GObject *gobject);

AgsUUID* ags_audio_unit_devout_get_uuid(AgsConnectable *connectable);
gboolean ags_audio_unit_devout_has_resource(AgsConnectable *connectable);
gboolean ags_audio_unit_devout_is_ready(AgsConnectable *connectable);
void ags_audio_unit_devout_add_to_registry(AgsConnectable *connectable);
void ags_audio_unit_devout_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_audio_unit_devout_list_resource(AgsConnectable *connectable);
xmlNode* ags_audio_unit_devout_xml_compose(AgsConnectable *connectable);
void ags_audio_unit_devout_xml_parse(AgsConnectable *connectable,
			       xmlNode *node);
gboolean ags_audio_unit_devout_is_connected(AgsConnectable *connectable);
void ags_audio_unit_devout_connect(AgsConnectable *connectable);
void ags_audio_unit_devout_disconnect(AgsConnectable *connectable);

void ags_audio_unit_devout_set_device(AgsSoundcard *soundcard,
				      gchar *device);
gchar* ags_audio_unit_devout_get_device(AgsSoundcard *soundcard);

void ags_audio_unit_devout_set_presets(AgsSoundcard *soundcard,
				       guint channels,
				       guint rate,
				       guint buffer_size,
				       AgsSoundcardFormat format);
void ags_audio_unit_devout_get_presets(AgsSoundcard *soundcard,
				       guint *channels,
				       guint *rate,
				       guint *buffer_size,
				       AgsSoundcardFormat *format);

void ags_audio_unit_devout_list_cards(AgsSoundcard *soundcard,
				      GList **card_id, GList **card_name);
void ags_audio_unit_devout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
				    guint *channels_min, guint *channels_max,
				    guint *rate_min, guint *rate_max,
				    guint *buffer_size_min, guint *buffer_size_max,
				    GError **error);
guint ags_audio_unit_devout_get_capability(AgsSoundcard *soundcard);

void ags_audio_unit_devout_set_bpm(AgsSoundcard *soundcard,
				   gdouble bpm);
gdouble ags_audio_unit_devout_get_bpm(AgsSoundcard *soundcard);

void ags_audio_unit_devout_set_start_note_offset(AgsSoundcard *soundcard,
						 guint64 start_note_offset);
guint64 ags_audio_unit_devout_get_start_note_offset(AgsSoundcard *soundcard);

GObject* ags_audio_unit_devout_get_frame_clock(AgsSoundcard *soundcard);

gboolean ags_audio_unit_devout_is_starting(AgsSoundcard *soundcard);
gboolean ags_audio_unit_devout_is_playing(AgsSoundcard *soundcard);

gchar* ags_audio_unit_devout_get_uptime(AgsSoundcard *soundcard);

void ags_audio_unit_devout_port_init(AgsSoundcard *soundcard,
				     GError **error);
void ags_audio_unit_devout_port_play(AgsSoundcard *soundcard,
				     GError **error);
void ags_audio_unit_devout_port_free(AgsSoundcard *soundcard);

void ags_audio_unit_devout_tic(AgsSoundcard *soundcard);
void ags_audio_unit_devout_offset_changed(AgsSoundcard *soundcard,
					  guint64 note_offset);

void* ags_audio_unit_devout_get_buffer(AgsSoundcard *soundcard);
void* ags_audio_unit_devout_get_next_buffer(AgsSoundcard *soundcard);
void* ags_audio_unit_devout_get_prev_buffer(AgsSoundcard *soundcard);

void ags_audio_unit_devout_lock_buffer(AgsSoundcard *soundcard,
				       void *buffer);
void ags_audio_unit_devout_unlock_buffer(AgsSoundcard *soundcard,
					 void *buffer);

guint ags_audio_unit_devout_get_sub_block_count(AgsSoundcard *soundcard);

gboolean ags_audio_unit_devout_trylock_sub_block(AgsSoundcard *soundcard,
						 void *buffer, guint sub_block);
void ags_audio_unit_devout_unlock_sub_block(AgsSoundcard *soundcard,
					    void *buffer, guint sub_block);

/**
 * SECTION:ags_audio_unit_devout
 * @short_description: Output to soundcard
 * @title: AgsAudioUnitDevout
 * @section_id:
 * @include: ags/audio/audio-unit/ags_audio_unit_devout.h
 *
 * #AgsAudioUnitDevout represents a soundcard and supports output.
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
  PROP_AUDIO_UNIT_CLIENT,
  PROP_AUDIO_UNIT_PORT,
  PROP_CHANNEL,
};

static gpointer ags_audio_unit_devout_parent_class = NULL;

GType
ags_audio_unit_devout_get_type (void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_audio_unit_devout = 0;
 
    static const GTypeInfo ags_audio_unit_devout_info = {
      sizeof(AgsAudioUnitDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_unit_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAudioUnitDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_unit_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_unit_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_audio_unit_devout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_unit_devout = g_type_register_static(G_TYPE_OBJECT,
							"AgsAudioUnitDevout",
							&ags_audio_unit_devout_info,
							0);

    g_type_add_interface_static(ags_type_audio_unit_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_audio_unit_devout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_audio_unit_devout);
  }

  return(g_define_type_id__static);
}

GType
ags_audio_unit_devout_flags_get_type()
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_AUDIO_UNIT_DEVOUT_INITIALIZED, "AGS_AUDIO_UNIT_DEVOUT_INITIALIZED", "audio-unit-devout-initialized" },
      { AGS_AUDIO_UNIT_DEVOUT_START_PLAY, "AGS_AUDIO_UNIT_DEVOUT_START_PLAY", "audio-unit-devout-start-play" },
      { AGS_AUDIO_UNIT_DEVOUT_PLAY, "AGS_AUDIO_UNIT_DEVOUT_PLAY", "audio-unit-devout-play" },
      { AGS_AUDIO_UNIT_DEVOUT_SHUTDOWN, "AGS_AUDIO_UNIT_DEVOUT_SHUTDOWN", "audio-unit-devout-shutdown" },
      { AGS_AUDIO_UNIT_DEVOUT_NONBLOCKING, "AGS_AUDIO_UNIT_DEVOUT_NONBLOCKING", "audio-unit-devout-nonblocking" },
      { AGS_AUDIO_UNIT_DEVOUT_ATTACK_FIRST, "AGS_AUDIO_UNIT_DEVOUT_ATTACK_FIRST", "audio-unit-devout-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsAudioUnitDevoutFlags"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
}

void
ags_audio_unit_devout_class_init(AgsAudioUnitDevoutClass *audio_unit_devout)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_audio_unit_devout_parent_class = g_type_class_peek_parent(audio_unit_devout);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_unit_devout;

  gobject->set_property = ags_audio_unit_devout_set_property;
  gobject->get_property = ags_audio_unit_devout_get_property;

  gobject->dispose = ags_audio_unit_devout_dispose;
  gobject->finalize = ags_audio_unit_devout_finalize;

  /* properties */
  /**
   * AgsAudioUnitDevout:device:
   *
   * The audio unit soundcard indentifier
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-audio-unit-devout-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsAudioUnitDevout:dsp-channels:
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
   * AgsAudioUnitDevout:pcm-channels:
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
   * AgsAudioUnitDevout:format:
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
   * AgsAudioUnitDevout:buffer-size:
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
   * AgsAudioUnitDevout:samplerate:
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
   * AgsAudioUnitDevout:bpm:
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
   * AgsAudioUnitDevout:buffer:
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
   * AgsAudioUnitDevout:audio-unit-client:
   *
   * The assigned #AgsAudioUnitClient
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio-unit-client",
				   i18n_pspec("audio unit client object"),
				   i18n_pspec("The audio unit client object"),
				   AGS_TYPE_AUDIO_UNIT_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_UNIT_CLIENT,
				  param_spec);

  /**
   * AgsAudioUnitDevout:audio-unit-port:
   *
   * The assigned #AgsAudioUnitPort
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("audio-unit-port",
				    i18n_pspec("audio unit port object"),
				    i18n_pspec("The audio unit port object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_UNIT_PORT,
				  param_spec);
}

GQuark
ags_audio_unit_devout_error_quark()
{
  return(g_quark_from_static_string("ags-audio_unit_devout-error-quark"));
}

void
ags_audio_unit_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_audio_unit_devout_get_uuid;
  connectable->has_resource = ags_audio_unit_devout_has_resource;

  connectable->is_ready = ags_audio_unit_devout_is_ready;
  connectable->add_to_registry = ags_audio_unit_devout_add_to_registry;
  connectable->remove_from_registry = ags_audio_unit_devout_remove_from_registry;

  connectable->list_resource = ags_audio_unit_devout_list_resource;
  connectable->xml_compose = ags_audio_unit_devout_xml_compose;
  connectable->xml_parse = ags_audio_unit_devout_xml_parse;

  connectable->is_connected = ags_audio_unit_devout_is_connected;  
  connectable->connect = ags_audio_unit_devout_connect;
  connectable->disconnect = ags_audio_unit_devout_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_audio_unit_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_audio_unit_devout_set_device;
  soundcard->get_device = ags_audio_unit_devout_get_device;
  
  soundcard->set_presets = ags_audio_unit_devout_set_presets;
  soundcard->get_presets = ags_audio_unit_devout_get_presets;

  soundcard->list_cards = ags_audio_unit_devout_list_cards;
  soundcard->pcm_info = ags_audio_unit_devout_pcm_info;
  soundcard->get_capability = ags_audio_unit_devout_get_capability;

  soundcard->set_bpm = ags_audio_unit_devout_set_bpm;
  soundcard->get_bpm = ags_audio_unit_devout_get_bpm;

  soundcard->set_start_note_offset = ags_audio_unit_devout_set_start_note_offset;
  soundcard->get_start_note_offset = ags_audio_unit_devout_get_start_note_offset;
  
  soundcard->get_frame_clock = ags_audio_unit_devout_get_frame_clock;

  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_audio_unit_devout_is_starting;
  soundcard->is_playing = ags_audio_unit_devout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_audio_unit_devout_get_uptime;
  
  soundcard->play_init = ags_audio_unit_devout_port_init;
  soundcard->play = ags_audio_unit_devout_port_play;

  soundcard->record_init = NULL;
  soundcard->record = NULL;

  soundcard->stop = ags_audio_unit_devout_port_free;

  soundcard->tic = ags_audio_unit_devout_tic;
  soundcard->offset_changed = ags_audio_unit_devout_offset_changed;
    
  soundcard->get_buffer = ags_audio_unit_devout_get_buffer;
  soundcard->get_next_buffer = ags_audio_unit_devout_get_next_buffer;
  soundcard->get_prev_buffer = ags_audio_unit_devout_get_prev_buffer;

  soundcard->lock_buffer = ags_audio_unit_devout_lock_buffer;
  soundcard->unlock_buffer = ags_audio_unit_devout_unlock_buffer;

  soundcard->get_sub_block_count = ags_audio_unit_devout_get_sub_block_count;

  soundcard->trylock_sub_block = ags_audio_unit_devout_trylock_sub_block;
  soundcard->unlock_sub_block = ags_audio_unit_devout_unlock_sub_block;
}

void
ags_audio_unit_devout_init(AgsAudioUnitDevout *audio_unit_devout)
{
  AgsConfig *config;

  gchar *str;
  gchar *segmentation;

  guint denominator, numerator;
  guint i;
  
  config = ags_config_get_instance();

  /* flags */
  audio_unit_devout->flags = 0;
  audio_unit_devout->connectable_flags = 0;
  ags_atomic_int_set(&(audio_unit_devout->sync_flags),
		   AGS_AUDIO_UNIT_DEVOUT_PASS_THROUGH);

  /* devout mutex */
  g_rec_mutex_init(&(audio_unit_devout->obj_mutex));

  /* uuid */
  audio_unit_devout->uuid = ags_uuid_alloc();
  ags_uuid_generate(audio_unit_devout->uuid);

  /* presets */
  audio_unit_devout->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  audio_unit_devout->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  audio_unit_devout->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  audio_unit_devout->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  audio_unit_devout->format = ags_soundcard_helper_config_get_format(config);

  /* start note offset */
  audio_unit_devout->start_note_offset = 0;
  
  /* frame clock */
  audio_unit_devout->frame_clock = ags_frame_clock_new();
  
  /* bpm */
  audio_unit_devout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* device */
  audio_unit_devout->card_uri = NULL;
  audio_unit_devout->audio_unit_client = NULL;

  audio_unit_devout->port_name = NULL;
  audio_unit_devout->audio_unit_port = NULL;

  /* app buffer mutex */
  audio_unit_devout->app_buffer_mode = AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_0;

  audio_unit_devout->app_buffer_mutex = (GRecMutex **) g_malloc(8 * sizeof(GRecMutex *));

  for(i = 0; i < 8; i++){
    audio_unit_devout->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(audio_unit_devout->app_buffer_mutex[i]);
  }

  /* sub-block mutex */
  audio_unit_devout->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  audio_unit_devout->sub_block_mutex = (GRecMutex **) g_malloc(8 * audio_unit_devout->sub_block_count * audio_unit_devout->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < 8 * audio_unit_devout->sub_block_count * audio_unit_devout->pcm_channels; i++){
    audio_unit_devout->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(audio_unit_devout->sub_block_mutex[i]);
  }

  /* app buffer */
  audio_unit_devout->app_buffer = (void **) g_malloc(8 * sizeof(void*));

  audio_unit_devout->app_buffer[0] = NULL;
  audio_unit_devout->app_buffer[1] = NULL;
  audio_unit_devout->app_buffer[2] = NULL;
  audio_unit_devout->app_buffer[3] = NULL;
  audio_unit_devout->app_buffer[4] = NULL;
  audio_unit_devout->app_buffer[5] = NULL;
  audio_unit_devout->app_buffer[6] = NULL;
  audio_unit_devout->app_buffer[7] = NULL;
  
  ags_audio_unit_devout_realloc_buffer(audio_unit_devout);
  
  /* callback mutex */
  g_mutex_init(&(audio_unit_devout->callback_mutex));

  g_cond_init(&(audio_unit_devout->callback_cond));

  /* callback finish mutex */
  g_mutex_init(&(audio_unit_devout->callback_finish_mutex));

  g_cond_init(&(audio_unit_devout->callback_finish_cond));
}

void
ags_audio_unit_devout_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsAudioUnitDevout *audio_unit_devout;
  
  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(gobject);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      audio_unit_devout->card_uri = g_strdup(device);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      if(dsp_channels == audio_unit_devout->dsp_channels){
	g_rec_mutex_unlock(audio_unit_devout_mutex);

	return;
      }

      audio_unit_devout->dsp_channels = dsp_channels;

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels, old_pcm_channels;
      guint i;

      pcm_channels = g_value_get_uint(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      if(pcm_channels == audio_unit_devout->pcm_channels){
	g_rec_mutex_unlock(audio_unit_devout_mutex);

	return;
      }

      old_pcm_channels = audio_unit_devout->pcm_channels;

      /* destroy if less pcm-channels */
      for(i = 8 * audio_unit_devout->sub_block_count * pcm_channels; i < 8 * audio_unit_devout->sub_block_count * old_pcm_channels; i++){
	g_rec_mutex_clear(audio_unit_devout->sub_block_mutex[i]);

	g_free(audio_unit_devout->sub_block_mutex[i]);
      }

      audio_unit_devout->sub_block_mutex = (GRecMutex **) g_realloc(audio_unit_devout->sub_block_mutex,
									8 * audio_unit_devout->sub_block_count * pcm_channels * sizeof(GRecMutex *));

      /* create if more pcm-channels */
      for(i = 8 * audio_unit_devout->sub_block_count * old_pcm_channels; i < 8 * audio_unit_devout->sub_block_count * pcm_channels; i++){
	audio_unit_devout->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

	g_rec_mutex_init(audio_unit_devout->sub_block_mutex[i]);
      }

      audio_unit_devout->pcm_channels = pcm_channels;

      g_rec_mutex_unlock(audio_unit_devout_mutex);

      ags_audio_unit_devout_realloc_buffer(audio_unit_devout);
    }
    break;
  case PROP_FORMAT:
    {
      AgsSoundcardFormat format;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      if(format == audio_unit_devout->format){
	g_rec_mutex_unlock(audio_unit_devout_mutex);

	return;
      }

      audio_unit_devout->format = format;

      g_rec_mutex_unlock(audio_unit_devout_mutex);

      ags_audio_unit_devout_realloc_buffer(audio_unit_devout);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      if(buffer_size == audio_unit_devout->buffer_size){
	g_rec_mutex_unlock(audio_unit_devout_mutex);

	return;
      }

      audio_unit_devout->buffer_size = buffer_size;

      ags_frame_clock_set_buffer_size(audio_unit_devout->frame_clock,
				      buffer_size);
      
      g_rec_mutex_unlock(audio_unit_devout_mutex);

      ags_audio_unit_devout_realloc_buffer(audio_unit_devout);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);
      
      if(samplerate == audio_unit_devout->samplerate){
	g_rec_mutex_unlock(audio_unit_devout_mutex);

	return;
      }

      audio_unit_devout->samplerate = samplerate;

      ags_frame_clock_set_samplerate(audio_unit_devout->frame_clock,
				     samplerate);
      
      g_rec_mutex_unlock(audio_unit_devout_mutex);

      ags_audio_unit_devout_realloc_buffer(audio_unit_devout);
    }
    break;
  case PROP_BPM:
    {
      gdouble bpm;
      
      bpm = g_value_get_double(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      audio_unit_devout->bpm = bpm;

      ags_frame_clock_set_bpm(audio_unit_devout->frame_clock,
			      bpm);
      
      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      //TODO:JK: implement me
    }
    break;
  case PROP_AUDIO_UNIT_CLIENT:
    {
      AgsAudioUnitClient *audio_unit_client;

      audio_unit_client = (AgsAudioUnitClient *) g_value_get_object(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      if(audio_unit_devout->audio_unit_client == (GObject *) audio_unit_client){
	g_rec_mutex_unlock(audio_unit_devout_mutex);

	return;
      }

      if(audio_unit_devout->audio_unit_client != NULL){
	g_object_unref(G_OBJECT(audio_unit_devout->audio_unit_client));
      }

      if(audio_unit_client != NULL){
	g_object_ref(audio_unit_client);
      }
      
      audio_unit_devout->audio_unit_client = (GObject *) audio_unit_client;

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_AUDIO_UNIT_PORT:
    {
      AgsAudioUnitPort *audio_unit_port;

      audio_unit_port = (AgsAudioUnitPort *) g_value_get_pointer(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      if(!AGS_IS_AUDIO_UNIT_PORT(audio_unit_port) ||
	 g_list_find(audio_unit_devout->audio_unit_port, audio_unit_port) != NULL){
	g_rec_mutex_unlock(audio_unit_devout_mutex);

	return;
      }

      g_object_ref(audio_unit_port);
      audio_unit_devout->audio_unit_port = g_list_append(audio_unit_devout->audio_unit_port,
					     audio_unit_port);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_devout_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(gobject);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_string(value, audio_unit_devout->card_uri);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_uint(value, audio_unit_devout->dsp_channels);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_uint(value, audio_unit_devout->pcm_channels);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_uint(value, audio_unit_devout->format);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_uint(value, audio_unit_devout->buffer_size);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_uint(value, audio_unit_devout->samplerate);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_double(value, audio_unit_devout->bpm);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_pointer(value, audio_unit_devout->app_buffer);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_AUDIO_UNIT_CLIENT:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_object(value, audio_unit_devout->audio_unit_client);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_AUDIO_UNIT_PORT:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(audio_unit_devout->audio_unit_port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_devout_dispose(GObject *gobject)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GList *list;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(gobject);

  /* audio_unit client */
  if(audio_unit_devout->audio_unit_client != NULL){
    g_object_unref(audio_unit_devout->audio_unit_client);

    audio_unit_devout->audio_unit_client = NULL;
  }

  /* audio_unit port */
  g_list_free_full(audio_unit_devout->audio_unit_port,
		   g_object_unref);

  audio_unit_devout->audio_unit_port = NULL;

  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_devout_parent_class)->dispose(gobject);
}

void
ags_audio_unit_devout_finalize(GObject *gobject)
{
  AgsAudioUnitDevout *audio_unit_devout;

  guint i;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(gobject);

  ags_uuid_free(audio_unit_devout->uuid);

  for(i = 0; i < 8; i++){
    g_rec_mutex_clear(audio_unit_devout->app_buffer_mutex[i]);

    g_free(audio_unit_devout->app_buffer_mutex[i]);
  }
  
  g_free(audio_unit_devout->app_buffer_mutex);
  
  for(i = 0; i < 8 * audio_unit_devout->sub_block_count * audio_unit_devout->pcm_channels; i++){
    g_rec_mutex_clear(audio_unit_devout->sub_block_mutex[i]);

    g_free(audio_unit_devout->sub_block_mutex[i]);
  }

  g_free(audio_unit_devout->sub_block_mutex);

  /* free output buffer */
  g_free(audio_unit_devout->app_buffer[0]);
  g_free(audio_unit_devout->app_buffer[1]);
  g_free(audio_unit_devout->app_buffer[2]);
  g_free(audio_unit_devout->app_buffer[3]);
  g_free(audio_unit_devout->app_buffer[4]);
  g_free(audio_unit_devout->app_buffer[5]);
  g_free(audio_unit_devout->app_buffer[6]);
  g_free(audio_unit_devout->app_buffer[7]);

  /* free buffer array */
  g_free(audio_unit_devout->app_buffer);
  
  /* audio_unit client */
  if(audio_unit_devout->audio_unit_client != NULL){
    g_object_unref(audio_unit_devout->audio_unit_client);
  }

  /* audio_unit port */
  g_list_free_full(audio_unit_devout->audio_unit_port,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_devout_parent_class)->finalize(gobject);
}

AgsUUID*
ags_audio_unit_devout_get_uuid(AgsConnectable *connectable)
{
  AgsAudioUnitDevout *audio_unit_devout;
  
  AgsUUID *ptr;

  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(connectable);

  /* get audio_unit devout signal mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get UUID */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  ptr = audio_unit_devout->uuid;

  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  return(ptr);
}

gboolean
ags_audio_unit_devout_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_audio_unit_devout_is_ready(AgsConnectable *connectable)
{
  AgsAudioUnitDevout *audio_unit_devout;
  
  gboolean is_ready;

  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(connectable);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* check is ready */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (audio_unit_devout->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  return(is_ready);
}

void
ags_audio_unit_devout_add_to_registry(AgsConnectable *connectable)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(connectable);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

void
ags_audio_unit_devout_remove_from_registry(AgsConnectable *connectable)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(connectable);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

xmlNode*
ags_audio_unit_devout_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_audio_unit_devout_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_audio_unit_devout_xml_parse(AgsConnectable *connectable,
		      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_audio_unit_devout_is_connected(AgsConnectable *connectable)
{
  AgsAudioUnitDevout *audio_unit_devout;
  
  gboolean is_connected;

  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(connectable);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* check is connected */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (audio_unit_devout->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  return(is_connected);
}

void
ags_audio_unit_devout_connect(AgsConnectable *connectable)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(connectable);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

void
ags_audio_unit_devout_disconnect(AgsConnectable *connectable)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(connectable);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

/**
 * ags_audio_unit_devout_test_flags:
 * @audio_unit_devout: the #AgsAudioUnitDevout
 * @flags: the flags
 *
 * Test @flags to be set on @audio_unit_devout.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_audio_unit_devout_test_flags(AgsAudioUnitDevout *audio_unit_devout, AgsAudioUnitDevoutFlags flags)
{
  gboolean retval;  
  
  GRecMutex *audio_unit_devout_mutex;

  if(!AGS_IS_AUDIO_UNIT_DEVOUT(audio_unit_devout)){
    return(FALSE);
  }

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* test */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  retval = (flags & (audio_unit_devout->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(retval);
}

/**
 * ags_audio_unit_devout_set_flags:
 * @audio_unit_devout: the #AgsAudioUnitDevout
 * @flags: see #AgsAudioUnitDevoutFlags-enum
 *
 * Enable a feature of @audio_unit_devout.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_devout_set_flags(AgsAudioUnitDevout *audio_unit_devout, AgsAudioUnitDevoutFlags flags)
{
  GRecMutex *audio_unit_devout_mutex;

  if(!AGS_IS_AUDIO_UNIT_DEVOUT(audio_unit_devout)){
    return;
  }

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->flags |= flags;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);
}
    
/**
 * ags_audio_unit_devout_unset_flags:
 * @audio_unit_devout: the #AgsAudioUnitDevout
 * @flags: see #AgsAudioUnitDevoutFlags-enum
 *
 * Disable a feature of @audio_unit_devout.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_devout_unset_flags(AgsAudioUnitDevout *audio_unit_devout, AgsAudioUnitDevoutFlags flags)
{  
  GRecMutex *audio_unit_devout_mutex;

  if(!AGS_IS_AUDIO_UNIT_DEVOUT(audio_unit_devout)){
    return;
  }

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->flags &= (~flags);
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

void
ags_audio_unit_devout_set_device(AgsSoundcard *soundcard,
				 gchar *device)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GList *audio_unit_port, *audio_unit_port_start;

  gchar *str;

  guint pcm_channels;
  int ret;
  guint nth_card;
  guint i;
  
  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* check device */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  if(audio_unit_devout->card_uri == device ||
     !g_ascii_strcasecmp(audio_unit_devout->card_uri,
			 device)){
    g_rec_mutex_unlock(audio_unit_devout_mutex);
  
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-audio_unit-devout-")){
    g_rec_mutex_unlock(audio_unit_devout_mutex);

    g_warning("invalid audio_unitaudio device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-audio_unit-devout-%u",
	       &nth_card);

  if(ret != 1){
    g_rec_mutex_unlock(audio_unit_devout_mutex);

    g_warning("invalid audio_unitaudio device specifier");

    return;
  }

  g_free(audio_unit_devout->card_uri);
  audio_unit_devout->card_uri = g_strdup(device);

  /* apply name to port */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_port_start = 
    audio_unit_port = g_list_copy(audio_unit_devout->audio_unit_port);

  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  str = g_strdup_printf("ags-soundcard%d",
			nth_card);
    
  g_object_set(audio_unit_port->data,
	       "port-name", str,
	       NULL);
  g_free(str);

  g_list_free(audio_unit_port_start);
}

gchar*
ags_audio_unit_devout_get_device(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;
  
  gchar *device;

  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  device = NULL;

  /* get device */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  device = g_strdup(audio_unit_devout->card_uri);

  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  return(device);
}

void
ags_audio_unit_devout_set_presets(AgsSoundcard *soundcard,
				  guint channels,
				  guint rate,
				  guint buffer_size,
				  AgsSoundcardFormat format)
{
  AgsAudioUnitDevout *audio_unit_devout;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  g_object_set(audio_unit_devout,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_audio_unit_devout_get_presets(AgsSoundcard *soundcard,
				  guint *channels,
				  guint *rate,
				  guint *buffer_size,
				  AgsSoundcardFormat *format)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get presets */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  if(channels != NULL){
    *channels = audio_unit_devout->pcm_channels;
  }

  if(rate != NULL){
    *rate = audio_unit_devout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = audio_unit_devout->buffer_size;
  }

  if(format != NULL){
    *format = audio_unit_devout->format;
  }

  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

void
ags_audio_unit_devout_list_cards(AgsSoundcard *soundcard,
				 GList **card_id, GList **card_name)
{
  AgsAudioUnitClient *audio_unit_client;
  AgsAudioUnitDevout *audio_unit_devout;

  AgsApplicationContext *application_context;
  
  GList *list_start, *list;

  gchar *card_uri;
  gchar *client_name;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

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
    if(AGS_IS_AUDIO_UNIT_DEVOUT(list->data)){
      if(card_id != NULL){
	card_uri = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));
	
	if(AGS_AUDIO_UNIT_DEVOUT(list->data)->card_uri != NULL){
	  *card_id = g_list_prepend(*card_id,
				    card_uri);
	}else{
	  *card_id = g_list_prepend(*card_id,
				    g_strdup("(null)"));

	  g_warning("ags_audio_unit_devout_list_cards() - card id (null)");
	}
      }

      if(card_name != NULL){
	g_object_get(list->data,
		     "audio-unit-client", &audio_unit_client,
		     NULL);
	
	if(audio_unit_client != NULL){
	  /* get client name */
	  g_object_get(audio_unit_client,
		       "client-name", &client_name,
		       NULL);	  
	  
	  *card_name = g_list_prepend(*card_name,
				      client_name);

	  g_object_unref(audio_unit_client);
	}else{
	  *card_name = g_list_prepend(*card_name,
				      g_strdup("(null)"));

	  g_warning("ags_audio_unit_devout_list_cards() - audio_unitaudio client not connected (null)");
	}
      }      
    }

    list = list->next;
  }

  g_list_free_full(list_start,
		   g_object_unref);
  
  if(card_id != NULL && *card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL && *card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

void
ags_audio_unit_devout_pcm_info(AgsSoundcard *soundcard,
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
ags_audio_unit_devout_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_PLAYBACK);
}

gboolean
ags_audio_unit_devout_is_starting(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  gboolean is_starting;
  
  GRecMutex *audio_unit_devout_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* check is starting */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  is_starting = ((AGS_AUDIO_UNIT_DEVOUT_START_PLAY & (audio_unit_devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  return(is_starting);
}

gboolean
ags_audio_unit_devout_is_playing(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  gboolean is_playing;
  
  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* check is starting */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  is_playing = ((AGS_AUDIO_UNIT_DEVOUT_PLAY & (audio_unit_devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(is_playing);
}

gchar*
ags_audio_unit_devout_get_uptime(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  gchar *uptime;

  GRecMutex *audio_unit_devout_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  /* get core audio devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get time string */
  g_rec_mutex_lock(audio_unit_devout_mutex);
  
  uptime = ags_frame_clock_to_time_string(audio_unit_devout->frame_clock);

  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  return(uptime);
}

void
ags_audio_unit_devout_port_init(AgsSoundcard *soundcard,
				GError **error)
{
  AgsAudioUnitPort *audio_unit_port;
  AgsAudioUnitDevout *audio_unit_devout;

  AgsSoundcardFormat format;
  guint word_size;

  GRecMutex *audio_unit_port_mutex;
  GRecMutex *audio_unit_devout_mutex;
  
  if(ags_soundcard_is_playing(soundcard)){
    return;
  }

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio-unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* port */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_port = (AgsAudioUnitPort *) audio_unit_devout->audio_unit_port->data;

  g_rec_mutex_unlock(audio_unit_devout_mutex);

  /* get port mutex */
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);

  /* retrieve word size */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  switch(audio_unit_devout->format){
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
    g_rec_mutex_unlock(audio_unit_devout_mutex);
    
    g_warning("ags_audio_unit_devout_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for playback */
  audio_unit_devout->app_buffer_mode = AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_7;
  audio_unit_devout->flags |= (AGS_AUDIO_UNIT_DEVOUT_START_PLAY |
			       AGS_AUDIO_UNIT_DEVOUT_PLAY |
			       AGS_AUDIO_UNIT_DEVOUT_NONBLOCKING);

  memset(audio_unit_devout->app_buffer[0], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->app_buffer[1], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->app_buffer[2], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->app_buffer[3], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->app_buffer[4], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->app_buffer[5], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->app_buffer[6], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->app_buffer[7], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);

  /*  */
  ags_frame_clock_set_start_note_offset(audio_unit_devout->frame_clock,
					audio_unit_devout->start_note_offset);

  ags_frame_clock_start(audio_unit_devout->frame_clock);
  
  audio_unit_devout->flags |= (AGS_AUDIO_UNIT_DEVOUT_INITIALIZED |
			       AGS_AUDIO_UNIT_DEVOUT_START_PLAY |
			       AGS_AUDIO_UNIT_DEVOUT_PLAY);
  
  ags_atomic_int_or(&(audio_unit_devout->sync_flags),
		  AGS_AUDIO_UNIT_DEVOUT_INITIAL_CALLBACK);

  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

void
ags_audio_unit_devout_port_play(AgsSoundcard *soundcard,
				GError **error)
{
  AgsAudioUnitClient *audio_unit_client;
  AgsAudioUnitDevout *audio_unit_devout;
  AgsAudioUnitPort *audio_unit_port;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
      
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;
  guint word_size;
  gboolean audio_unit_client_activated;

  GRecMutex *audio_unit_devout_mutex;
  GRecMutex *audio_unit_client_mutex;
  GRecMutex *audio_unit_port_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  application_context = ags_application_context_get_instance();
  
  /* get audio-unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* client */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_client = (AgsAudioUnitClient *) audio_unit_devout->audio_unit_client;
  audio_unit_port = (AgsAudioUnitPort *) audio_unit_devout->audio_unit_port->data;
  
  callback_mutex = &(audio_unit_devout->callback_mutex);
  callback_finish_mutex = &(audio_unit_devout->callback_finish_mutex);

  /* do playback */  
  audio_unit_devout->flags &= (~AGS_AUDIO_UNIT_DEVOUT_START_PLAY);

  if((AGS_AUDIO_UNIT_DEVOUT_INITIALIZED & (audio_unit_devout->flags)) == 0){
    g_rec_mutex_unlock(audio_unit_devout_mutex);
    
    return;
  }
  
  switch(audio_unit_devout->format){
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
    g_rec_mutex_unlock(audio_unit_devout_mutex);
    
    g_warning("ags_audio_unit_devout_port_play(): unsupported word size");
    
    return;
  }

  g_rec_mutex_unlock(audio_unit_devout_mutex);

  /* get client and port mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);

  /* get activated */
  g_rec_mutex_lock(audio_unit_client_mutex);

  audio_unit_client_activated = ((AGS_AUDIO_UNIT_CLIENT_ACTIVATED & (audio_unit_client->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(audio_unit_client_mutex);

  if(audio_unit_client_activated){
    while((AGS_AUDIO_UNIT_DEVOUT_PASS_THROUGH & (ags_atomic_int_get(&(audio_unit_devout->sync_flags)))) != 0){
      usleep(4);
    }
    
    /* signal */
    if((AGS_AUDIO_UNIT_DEVOUT_INITIAL_CALLBACK & (ags_atomic_int_get(&(audio_unit_devout->sync_flags)))) == 0){
      g_mutex_lock(callback_mutex);

      ags_atomic_int_or(&(audio_unit_devout->sync_flags),
		      AGS_AUDIO_UNIT_DEVOUT_CALLBACK_DONE);
    
      if((AGS_AUDIO_UNIT_DEVOUT_CALLBACK_WAIT & (ags_atomic_int_get(&(audio_unit_devout->sync_flags)))) != 0){
	g_cond_signal(&(audio_unit_devout->callback_cond));
      }

      g_mutex_unlock(callback_mutex);
      //    }
    
      /* wait callback */	
      g_mutex_lock(callback_finish_mutex);
    
      if((AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_DONE & (ags_atomic_int_get(&(audio_unit_devout->sync_flags)))) == 0){
	ags_atomic_int_or(&(audio_unit_devout->sync_flags),
			AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_WAIT);
    
	while((AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_DONE & (ags_atomic_int_get(&(audio_unit_devout->sync_flags)))) == 0 &&
	      (AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_WAIT & (ags_atomic_int_get(&(audio_unit_devout->sync_flags)))) != 0){
	  g_cond_wait(&(audio_unit_devout->callback_finish_cond),
			    callback_finish_mutex);
	}
      }
      
      ags_atomic_int_and(&(audio_unit_devout->sync_flags),
		       (~(AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_WAIT |
			  AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_DONE)));
    
      g_mutex_unlock(callback_finish_mutex);
    }else{
      ags_atomic_int_and(&(audio_unit_devout->sync_flags),
		       (~AGS_AUDIO_UNIT_DEVOUT_INITIAL_CALLBACK));
    }
  }

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;      
  
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) audio_unit_devout);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) audio_unit_devout);
  task = g_list_append(task,
		       clear_buffer);
    
  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) audio_unit_devout);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_audio_unit_devout_port_free(AgsSoundcard *soundcard)
{
  AgsAudioUnitPort *audio_unit_port;
  AgsAudioUnitDevout *audio_unit_devout;

  guint pcm_channels;
  guint word_size;

  GRecMutex *audio_unit_port_mutex;
  GRecMutex *audio_unit_devout_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  /* get audio-unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* port */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_port = (AgsAudioUnitPort *) audio_unit_devout->audio_unit_port->data;
  pcm_channels = audio_unit_devout->pcm_channels;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

  /* get port mutex */
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);

  /*  */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  if((AGS_AUDIO_UNIT_DEVOUT_INITIALIZED & (audio_unit_devout->flags)) == 0){
    g_rec_mutex_unlock(audio_unit_devout_mutex);

    return;
  }

  callback_mutex = &(audio_unit_devout->callback_mutex);
  callback_finish_mutex = &(audio_unit_devout->callback_finish_mutex);
  
  //  ags_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);
  
  audio_unit_devout->app_buffer_mode = AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_0;
  audio_unit_devout->flags &= (~(AGS_AUDIO_UNIT_DEVOUT_PLAY));

  ags_atomic_int_or(&(audio_unit_devout->sync_flags),
		  AGS_AUDIO_UNIT_DEVOUT_PASS_THROUGH);
  ags_atomic_int_and(&(audio_unit_devout->sync_flags),
		   (~AGS_AUDIO_UNIT_DEVOUT_INITIAL_CALLBACK));

  /* signal callback */
  g_mutex_lock(callback_mutex);

  ags_atomic_int_or(&(audio_unit_devout->sync_flags),
		  AGS_AUDIO_UNIT_DEVOUT_CALLBACK_DONE);
    
  if((AGS_AUDIO_UNIT_DEVOUT_CALLBACK_WAIT & (ags_atomic_int_get(&(audio_unit_devout->sync_flags)))) != 0){
    g_cond_signal(&(audio_unit_devout->callback_cond));
  }

  g_mutex_unlock(callback_mutex);

  /* signal thread */
  g_mutex_lock(callback_finish_mutex);

  ags_atomic_int_or(&(audio_unit_devout->sync_flags),
		  AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_DONE);
    
  if((AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_WAIT & (ags_atomic_int_get(&(audio_unit_devout->sync_flags)))) != 0){
    g_cond_signal(&(audio_unit_devout->callback_finish_cond));
  }

  g_mutex_unlock(callback_finish_mutex);
  
  /*  */
  ags_frame_clock_stop(audio_unit_devout->frame_clock);
  
  switch(audio_unit_devout->format){
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
    
    g_critical("ags_audio_unit_devout_free(): unsupported word size");
  }

  g_rec_mutex_unlock(audio_unit_devout_mutex);

  if(audio_unit_devout->audio_unit_port != NULL){
    audio_unit_port = audio_unit_devout->audio_unit_port->data;

    while(!ags_atomic_int_get(&(audio_unit_port->is_empty))) usleep(500000);
  }

  g_rec_mutex_lock(audio_unit_devout_mutex);
  
  memset(audio_unit_devout->app_buffer[0], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->app_buffer[1], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->app_buffer[2], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->app_buffer[3], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->app_buffer[4], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->app_buffer[5], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->app_buffer[6], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->app_buffer[7], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);

  g_rec_mutex_unlock(audio_unit_devout_mutex);  
}

void
ags_audio_unit_devout_tic(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* frame clock */
  g_rec_mutex_lock(audio_unit_devout_mutex);
  
  ags_frame_clock_increment_counter(audio_unit_devout->frame_clock);

  /* 16th pulse */
  if(ags_frame_clock_get_has_16th_pulse(audio_unit_devout->frame_clock)){
    ags_soundcard_offset_changed(soundcard,
				 audio_unit_devout->frame_clock->note_offset);
  }
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

void
ags_audio_unit_devout_offset_changed(AgsSoundcard *soundcard,
				     guint64 note_offset)
{
  //empty
}

void
ags_audio_unit_devout_set_bpm(AgsSoundcard *soundcard,
			      gdouble bpm)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* set bpm */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->bpm = bpm;
  
  ags_frame_clock_set_bpm(audio_unit_devout->frame_clock,
			  bpm);

  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

gdouble
ags_audio_unit_devout_get_bpm(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  gdouble bpm;
  
  GRecMutex *audio_unit_devout_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get bpm */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  bpm = audio_unit_devout->bpm;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(bpm);
}

void
ags_audio_unit_devout_set_start_note_offset(AgsSoundcard *soundcard,
					    guint64 start_note_offset)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;  

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* set note offset */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

guint64
ags_audio_unit_devout_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  guint64 start_note_offset;
  
  GRecMutex *audio_unit_devout_mutex;  

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* set note offset */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  start_note_offset = audio_unit_devout->start_note_offset;

  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(start_note_offset);
}

GObject*
ags_audio_unit_devout_get_frame_clock(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GObject *frame_clock;
  
  GRecMutex *audio_unit_devout_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get core audio devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get frame clock */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  frame_clock = (GObject *) audio_unit_devout->frame_clock;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(frame_clock);
}

void*
ags_audio_unit_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  void *buffer;

  GRecMutex *audio_unit_devout_mutex;  
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  g_rec_mutex_lock(audio_unit_devout_mutex);

  if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_0){
    buffer = audio_unit_devout->app_buffer[0];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_1){
    buffer = audio_unit_devout->app_buffer[1];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_2){
    buffer = audio_unit_devout->app_buffer[2];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_3){
    buffer = audio_unit_devout->app_buffer[3];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_4){
    buffer = audio_unit_devout->app_buffer[4];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_5){
    buffer = audio_unit_devout->app_buffer[5];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_6){
    buffer = audio_unit_devout->app_buffer[6];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_7){
    buffer = audio_unit_devout->app_buffer[7];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(buffer);
}

void*
ags_audio_unit_devout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  void *buffer;

  GRecMutex *audio_unit_devout_mutex;  
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  g_rec_mutex_lock(audio_unit_devout_mutex);

  if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_0){
    buffer = audio_unit_devout->app_buffer[1];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_1){
    buffer = audio_unit_devout->app_buffer[2];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_2){
    buffer = audio_unit_devout->app_buffer[3];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_3){
    buffer = audio_unit_devout->app_buffer[4];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_4){
    buffer = audio_unit_devout->app_buffer[5];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_5){
    buffer = audio_unit_devout->app_buffer[6];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_6){
    buffer = audio_unit_devout->app_buffer[7];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_7){
    buffer = audio_unit_devout->app_buffer[0];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(buffer);
}

void*
ags_audio_unit_devout_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  void *buffer;

  GRecMutex *audio_unit_devout_mutex;  
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  g_rec_mutex_lock(audio_unit_devout_mutex);

  if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_0){
    buffer = audio_unit_devout->app_buffer[7];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_1){
    buffer = audio_unit_devout->app_buffer[0];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_2){
    buffer = audio_unit_devout->app_buffer[1];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_3){
    buffer = audio_unit_devout->app_buffer[2];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_4){
    buffer = audio_unit_devout->app_buffer[3];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_5){
    buffer = audio_unit_devout->app_buffer[4];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_6){
    buffer = audio_unit_devout->app_buffer[5];
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_7){
    buffer = audio_unit_devout->app_buffer[6];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(buffer);
}

void
ags_audio_unit_devout_lock_buffer(AgsSoundcard *soundcard,
				  void *buffer)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *buffer_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(audio_unit_devout->app_buffer != NULL){
    if(buffer == audio_unit_devout->app_buffer[0]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[0];
    }else if(buffer == audio_unit_devout->app_buffer[1]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[1];
    }else if(buffer == audio_unit_devout->app_buffer[2]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[2];
    }else if(buffer == audio_unit_devout->app_buffer[3]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[3];
    }else if(buffer == audio_unit_devout->app_buffer[4]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[4];
    }else if(buffer == audio_unit_devout->app_buffer[5]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[5];
    }else if(buffer == audio_unit_devout->app_buffer[6]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[6];
    }else if(buffer == audio_unit_devout->app_buffer[7]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[7];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_audio_unit_devout_unlock_buffer(AgsSoundcard *soundcard,
				    void *buffer)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *buffer_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(audio_unit_devout->app_buffer != NULL){
    if(buffer == audio_unit_devout->app_buffer[0]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[0];
    }else if(buffer == audio_unit_devout->app_buffer[1]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[1];
    }else if(buffer == audio_unit_devout->app_buffer[2]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[2];
    }else if(buffer == audio_unit_devout->app_buffer[3]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[3];
    }else if(buffer == audio_unit_devout->app_buffer[4]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[4];
    }else if(buffer == audio_unit_devout->app_buffer[5]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[5];
    }else if(buffer == audio_unit_devout->app_buffer[6]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[6];
    }else if(buffer == audio_unit_devout->app_buffer[7]){
      buffer_mutex = audio_unit_devout->app_buffer_mutex[7];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_audio_unit_devout_get_sub_block_count(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  guint sub_block_count;
  
  GRecMutex *audio_unit_devout_mutex;  

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get loop offset */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  sub_block_count = audio_unit_devout->sub_block_count;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(sub_block_count);
}

gboolean
ags_audio_unit_devout_trylock_sub_block(AgsSoundcard *soundcard,
					void *buffer, guint sub_block)
{
  AgsAudioUnitDevout *audio_unit_devout;

  guint pcm_channels;
  guint sub_block_count;
  gboolean success;
  
  GRecMutex *audio_unit_devout_mutex;  
  GRecMutex *sub_block_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get loop offset */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  pcm_channels = audio_unit_devout->pcm_channels;
  sub_block_count = audio_unit_devout->sub_block_count;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  sub_block_mutex = NULL;

  success = FALSE;
  
  if(audio_unit_devout->app_buffer != NULL){
    if(buffer == audio_unit_devout->app_buffer[0]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[sub_block];
    }else if(buffer == audio_unit_devout->app_buffer[1]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->app_buffer[2]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->app_buffer[3]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->app_buffer[4]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[4 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->app_buffer[5]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[5 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->app_buffer[6]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[6 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->app_buffer[7]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[7 * pcm_channels * sub_block_count + sub_block];
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
ags_audio_unit_devout_unlock_sub_block(AgsSoundcard *soundcard,
				       void *buffer, guint sub_block)
{
  AgsAudioUnitDevout *audio_unit_devout;

  guint pcm_channels;
  guint sub_block_count;
  
  GRecMutex *audio_unit_devout_mutex;  
  GRecMutex *sub_block_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get loop offset */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  pcm_channels = audio_unit_devout->pcm_channels;
  sub_block_count = audio_unit_devout->sub_block_count;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  sub_block_mutex = NULL;
  
  if(audio_unit_devout->app_buffer != NULL){
    if(buffer == audio_unit_devout->app_buffer[0]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[sub_block];
    }else if(buffer == audio_unit_devout->app_buffer[1]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->app_buffer[2]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->app_buffer[3]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->app_buffer[4]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[4 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->app_buffer[5]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[5 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->app_buffer[6]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[6 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->app_buffer[7]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[7 * pcm_channels * sub_block_count + sub_block];
    }
  }

  if(sub_block_mutex != NULL){
    g_rec_mutex_unlock(sub_block_mutex);
  }
}

/**
 * ags_audio_unit_devout_switch_buffer_flag:
 * @audio_unit_devout: an #AgsAudioUnitDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_devout_switch_buffer_flag(AgsAudioUnitDevout *audio_unit_devout)
{
  GRecMutex *audio_unit_devout_mutex;
  
  if(!AGS_IS_AUDIO_UNIT_DEVOUT(audio_unit_devout)){
    return;
  }

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* switch buffer flag */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_0){
    audio_unit_devout->app_buffer_mode = AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_1;
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_1){
    audio_unit_devout->app_buffer_mode = AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_2;
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_2){
    audio_unit_devout->app_buffer_mode = AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_3;
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_3){
    audio_unit_devout->app_buffer_mode = AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_4;
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_4){
    audio_unit_devout->app_buffer_mode = AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_5;
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_5){
    audio_unit_devout->app_buffer_mode = AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_6;
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_6){
    audio_unit_devout->app_buffer_mode = AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_7;
  }else if(audio_unit_devout->app_buffer_mode == AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_7){
    audio_unit_devout->app_buffer_mode = AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

/**
 * ags_audio_unit_devout_realloc_buffer:
 * @audio_unit_devout: the #AgsAudioUnitDevout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_devout_realloc_buffer(AgsAudioUnitDevout *audio_unit_devout)
{
  guint pcm_channels;
  guint buffer_size;
  AgsSoundcardFormat format;
  guint word_size;

  GRecMutex *audio_unit_devout_mutex;  

  if(!AGS_IS_AUDIO_UNIT_DEVOUT(audio_unit_devout)){
    return;
  }

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get word size */  
  g_rec_mutex_lock(audio_unit_devout_mutex);

  pcm_channels = audio_unit_devout->pcm_channels;
  buffer_size = audio_unit_devout->buffer_size;

  format = audio_unit_devout->format;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

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
    g_warning("ags_audio_unit_devout_realloc_buffer(): unsupported word size");
    return;
  }
  
  /* AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_0 */
  if(audio_unit_devout->app_buffer[0] != NULL){
    g_free(audio_unit_devout->app_buffer[0]);
  }
  
  audio_unit_devout->app_buffer[0] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_1 */
  if(audio_unit_devout->app_buffer[1] != NULL){
    g_free(audio_unit_devout->app_buffer[1]);
  }

  audio_unit_devout->app_buffer[1] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_2 */
  if(audio_unit_devout->app_buffer[2] != NULL){
    g_free(audio_unit_devout->app_buffer[2]);
  }

  audio_unit_devout->app_buffer[2] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_3 */
  if(audio_unit_devout->app_buffer[3] != NULL){
    g_free(audio_unit_devout->app_buffer[3]);
  }
  
  audio_unit_devout->app_buffer[3] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  /* AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_4 */
  if(audio_unit_devout->app_buffer[4] != NULL){
    g_free(audio_unit_devout->app_buffer[4]);
  }
  
  audio_unit_devout->app_buffer[4] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  /* AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_5 */
  if(audio_unit_devout->app_buffer[5] != NULL){
    g_free(audio_unit_devout->app_buffer[5]);
  }
  
  audio_unit_devout->app_buffer[5] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  /* AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_6 */
  if(audio_unit_devout->app_buffer[6] != NULL){
    g_free(audio_unit_devout->app_buffer[6]);
  }
  
  audio_unit_devout->app_buffer[6] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  /* AGS_AUDIO_UNIT_DEVOUT_APP_BUFFER_7 */
  if(audio_unit_devout->app_buffer[7] != NULL){
    g_free(audio_unit_devout->app_buffer[7]);
  }
  
  audio_unit_devout->app_buffer[7] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_audio_unit_devout_new:
 *
 * Creates a new instance of #AgsAudioUnitDevout.
 *
 * Returns: a new #AgsAudioUnitDevout
 *
 * Since: 3.0.0
 */
AgsAudioUnitDevout*
ags_audio_unit_devout_new()
{
  AgsAudioUnitDevout *audio_unit_devout;

  audio_unit_devout = (AgsAudioUnitDevout *) g_object_new(AGS_TYPE_AUDIO_UNIT_DEVOUT,
							  NULL);
  
  return(audio_unit_devout);
}
