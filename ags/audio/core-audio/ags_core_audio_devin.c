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

#include <ags/audio/core-audio/ags_core_audio_devin.h>

#include <ags/config.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_soundcard_util.h>
#include <ags/audio/ags_audio_buffer_util.h>

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

void ags_core_audio_devin_set_device(AgsSoundcard *soundcard,
				     gchar *device);
gchar* ags_core_audio_devin_get_device(AgsSoundcard *soundcard);

void ags_core_audio_devin_set_presets(AgsSoundcard *soundcard,
				      guint channels,
				      guint rate,
				      guint buffer_size,
				      AgsSoundcardFormat format);
void ags_core_audio_devin_get_presets(AgsSoundcard *soundcard,
				      guint *channels,
				      guint *rate,
				      guint *buffer_size,
				      AgsSoundcardFormat *format);

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
					 guint64 note_offset);

void ags_core_audio_devin_set_bpm(AgsSoundcard *soundcard,
				  gdouble bpm);
gdouble ags_core_audio_devin_get_bpm(AgsSoundcard *soundcard);

void ags_core_audio_devin_set_start_note_offset(AgsSoundcard *soundcard,
						guint64 start_note_offset);
guint64 ags_core_audio_devin_get_start_note_offset(AgsSoundcard *soundcard);

GObject* ags_core_audio_devin_get_frame_clock(AgsSoundcard *soundcard);

void* ags_core_audio_devin_get_buffer(AgsSoundcard *soundcard);
void* ags_core_audio_devin_get_next_buffer(AgsSoundcard *soundcard);
void* ags_core_audio_devin_get_prev_buffer(AgsSoundcard *soundcard);

void ags_core_audio_devin_lock_buffer(AgsSoundcard *soundcard,
				      void *buffer);
void ags_core_audio_devin_unlock_buffer(AgsSoundcard *soundcard,
					void *buffer);

/**
 * SECTION:ags_core_audio_devin
 * @short_description: Input from soundcard
 * @title: AgsCoreAudioDevin
 * @section_id:
 * @include: ags/audio/core-audio/ags_core_audio_devin.h
 *
 * #AgsCoreAudioDevin represents a soundcard and supports output.
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

static gpointer ags_core_audio_devin_parent_class = NULL;

GType
ags_core_audio_devin_get_type (void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
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

    g_once_init_leave(&g_define_type_id__static, ags_type_core_audio_devin);
  }

  return(g_define_type_id__static);
}

GType
ags_core_audio_devin_flags_get_type()
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_CORE_AUDIO_DEVIN_INITIALIZED, "AGS_CORE_AUDIO_DEVIN_INITIALIZED", "core-audio-devin-initialized" },
      { AGS_CORE_AUDIO_DEVIN_START_RECORD, "AGS_CORE_AUDIO_DEVIN_START_RECORD", "core-audio-devin-start-record" },
      { AGS_CORE_AUDIO_DEVIN_RECORD, "AGS_CORE_AUDIO_DEVIN_RECORD", "core-audio-devin-record" },
      { AGS_CORE_AUDIO_DEVIN_SHUTDOWN, "AGS_CORE_AUDIO_DEVIN_SHUTDOWN", "core-audio-devin-shutdown" },
      { AGS_CORE_AUDIO_DEVIN_NONBLOCKING, "AGS_CORE_AUDIO_DEVIN_NONBLOCKING", "core-audio-devin-nonblocking" },
      { AGS_CORE_AUDIO_DEVIN_ATTACK_FIRST, "AGS_CORE_AUDIO_DEVIN_ATTACK_FIRST", "core-audio-devin-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsCoreAudioDevinFlags"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
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
   * AgsCoreAudioDevin:device:
   *
   * The core audio soundcard indentifier
   * 
   * Since: 3.0.0
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
   * AgsCoreAudioDevin:pcm-channels:
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
   * AgsCoreAudioDevin:format:
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
   * AgsCoreAudioDevin:buffer-size:
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
   * AgsCoreAudioDevin:samplerate:
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
   * AgsCoreAudioDevin:bpm:
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
   * AgsCoreAudioDevin:buffer:
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
   * AgsCoreAudioDevin:core-audio-client:
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
   * AgsCoreAudioDevin:core-audio-port:
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
ags_core_audio_devin_error_quark()
{
  return(g_quark_from_static_string("ags-core_audio_devin-error-quark"));
}

void
ags_core_audio_devin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_core_audio_devin_get_uuid;
  connectable->has_resource = ags_core_audio_devin_has_resource;

  connectable->is_ready = ags_core_audio_devin_is_ready;
  connectable->add_to_registry = ags_core_audio_devin_add_to_registry;
  connectable->remove_from_registry = ags_core_audio_devin_remove_from_registry;

  connectable->list_resource = ags_core_audio_devin_list_resource;
  connectable->xml_compose = ags_core_audio_devin_xml_compose;
  connectable->xml_parse = ags_core_audio_devin_xml_parse;

  connectable->is_connected = ags_core_audio_devin_is_connected;  
  connectable->connect = ags_core_audio_devin_connect;
  connectable->disconnect = ags_core_audio_devin_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_core_audio_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_core_audio_devin_set_device;
  soundcard->get_device = ags_core_audio_devin_get_device;
  
  soundcard->set_presets = ags_core_audio_devin_set_presets;
  soundcard->get_presets = ags_core_audio_devin_get_presets;

  soundcard->list_cards = ags_core_audio_devin_list_cards;
  soundcard->pcm_info = ags_core_audio_devin_pcm_info;
  soundcard->get_capability = ags_core_audio_devin_get_capability;

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

  soundcard->set_start_note_offset = ags_core_audio_devin_set_start_note_offset;
  soundcard->get_start_note_offset = ags_core_audio_devin_get_start_note_offset;
  
  soundcard->get_frame_clock = ags_core_audio_devin_get_frame_clock;
  
  soundcard->get_buffer = ags_core_audio_devin_get_buffer;
  soundcard->get_next_buffer = ags_core_audio_devin_get_next_buffer;
  soundcard->get_prev_buffer = ags_core_audio_devin_get_prev_buffer;
  
  soundcard->lock_buffer = ags_core_audio_devin_lock_buffer;
  soundcard->unlock_buffer = ags_core_audio_devin_unlock_buffer;
}

void
ags_core_audio_devin_init(AgsCoreAudioDevin *core_audio_devin)
{
  AgsConfig *config;

  gchar *str;
  gchar *segmentation;

  gdouble absolute_delay;
  guint denominator, numerator;
  guint i;
  
  config = ags_config_get_instance();
  
  /* flags */
  core_audio_devin->flags = 0;
  core_audio_devin->connectable_flags = 0;
  ags_atomic_int_set(&(core_audio_devin->sync_flags),
		     AGS_CORE_AUDIO_DEVIN_PASS_THROUGH);

  /* devin mutex */
  g_rec_mutex_init(&(core_audio_devin->obj_mutex));

  /* uuid */
  core_audio_devin->uuid = ags_uuid_alloc();
  ags_uuid_generate(core_audio_devin->uuid);

  /* presets */
  core_audio_devin->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  core_audio_devin->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  core_audio_devin->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  core_audio_devin->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  core_audio_devin->format = AGS_SOUNDCARD_FLOAT;

  /* bpm */
  core_audio_devin->bpm = AGS_SOUNDCARD_DEFAULT_BPM;
  
  /* start note offset */
  core_audio_devin->start_note_offset = 0;
  
  /* frame clock */
  core_audio_devin->frame_clock = ags_frame_clock_new();
  
  /* app buffer mutex */
  core_audio_devin->app_buffer_mode = AGS_CORE_AUDIO_DEVIN_APP_BUFFER_0;

  core_audio_devin->app_buffer_mutex = (GRecMutex **) g_malloc(AGS_CORE_AUDIO_DEVIN_DEFAULT_APP_BUFFER_SIZE * sizeof(GRecMutex *));

  for(i = 0; i < AGS_CORE_AUDIO_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    core_audio_devin->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(core_audio_devin->app_buffer_mutex[i]);
  }
  
  /* sub-block */
  core_audio_devin->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  core_audio_devin->sub_block_mutex = (GRecMutex **) g_malloc(AGS_CORE_AUDIO_DEVIN_DEFAULT_APP_BUFFER_SIZE * core_audio_devin->sub_block_count * core_audio_devin->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < AGS_CORE_AUDIO_DEVIN_DEFAULT_APP_BUFFER_SIZE * core_audio_devin->sub_block_count * core_audio_devin->pcm_channels; i++){
    core_audio_devin->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(core_audio_devin->sub_block_mutex[i]);
  }

  /* app buffer */
  core_audio_devin->app_buffer = (void **) g_malloc(AGS_CORE_AUDIO_DEVIN_DEFAULT_APP_BUFFER_SIZE * sizeof(void*));

  core_audio_devin->app_buffer[0] = NULL;
  core_audio_devin->app_buffer[1] = NULL;
  core_audio_devin->app_buffer[2] = NULL;
  core_audio_devin->app_buffer[3] = NULL;
  core_audio_devin->app_buffer[4] = NULL;
  core_audio_devin->app_buffer[5] = NULL;
  core_audio_devin->app_buffer[6] = NULL;
  core_audio_devin->app_buffer[7] = NULL;
  
  /* realloc buffer */
  ags_core_audio_devin_realloc_buffer(core_audio_devin);
  
  /*  */
  core_audio_devin->card_uri = NULL;
  core_audio_devin->core_audio_client = NULL;

  core_audio_devin->port_name = NULL;
  core_audio_devin->core_audio_port = NULL;

  /* callback mutex */
  g_mutex_init(&(core_audio_devin->callback_mutex));

  g_cond_init(&(core_audio_devin->callback_cond));

  /* callback finish mutex */
  g_mutex_init(&(core_audio_devin->callback_finish_mutex));

  g_cond_init(&(core_audio_devin->callback_finish_cond));
  
  /* audio device */
  core_audio_devin->device_id = NULL;
  core_audio_devin->device_name = NULL;

  core_audio_devin->audio_device = 0;
}

void
ags_core_audio_devin_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsCoreAudioDevin *core_audio_devin;
  
  GRecMutex *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(gobject);

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      ags_soundcard_set_device(AGS_SOUNDCARD(core_audio_devin),
			       device);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      g_rec_mutex_lock(core_audio_devin_mutex);

      if(dsp_channels == core_audio_devin->dsp_channels){
	g_rec_mutex_unlock(core_audio_devin_mutex);

	return;
      }

      core_audio_devin->dsp_channels = dsp_channels;

      g_rec_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      GList *port;
      
      guint pcm_channels;
      gboolean is_registered;

      pcm_channels = g_value_get_uint(value);

      g_rec_mutex_lock(core_audio_devin_mutex);

      if(pcm_channels == core_audio_devin->pcm_channels){
	g_rec_mutex_unlock(core_audio_devin_mutex);

	return;
      }

      core_audio_devin->pcm_channels = pcm_channels;

      g_rec_mutex_unlock(core_audio_devin_mutex);
      
      ags_core_audio_devin_realloc_buffer(core_audio_devin);

      /* reset */
      port = core_audio_devin->core_audio_port;

      while(port != NULL){
	is_registered = (ags_core_audio_port_test_flags((AgsCoreAudioPort *) port->data, AGS_CORE_AUDIO_PORT_REGISTERED)) ? TRUE: FALSE;

	if(is_registered){
	  ags_core_audio_port_unregister((AgsCoreAudioPort *) port->data);
	}
	
	g_object_set((GObject *) port->data,
		     "pcm-channels", pcm_channels,
		     NULL);

	if(is_registered){
	  ags_core_audio_port_register((AgsCoreAudioPort *) port->data,
				       core_audio_devin->device_id,
				       TRUE, FALSE,
				       FALSE);
	}
	
	port = port->next;
      }
    }
    break;
  case PROP_FORMAT:
    {
      GList *port;

      AgsSoundcardFormat format;
      gboolean is_registered;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(core_audio_devin_mutex);

      if(format == core_audio_devin->format){
	g_rec_mutex_unlock(core_audio_devin_mutex);

	return;
      }

      core_audio_devin->format = format;

      g_rec_mutex_unlock(core_audio_devin_mutex);

      ags_core_audio_devin_realloc_buffer(core_audio_devin);

#if 0
      /* reset port */
      port = core_audio_devin->core_audio_port;

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
				       core_audio_devin->device_id,
				       TRUE, FALSE,
				       FALSE);
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

      g_rec_mutex_lock(core_audio_devin_mutex);

      if(buffer_size == core_audio_devin->buffer_size){
	g_rec_mutex_unlock(core_audio_devin_mutex);

	return;
      }

      core_audio_devin->buffer_size = buffer_size;
      
      ags_frame_clock_set_buffer_size(core_audio_devin->frame_clock,
				      buffer_size);

      g_rec_mutex_unlock(core_audio_devin_mutex);

      ags_core_audio_devin_realloc_buffer(core_audio_devin);

      /* reset port */
      port = core_audio_devin->core_audio_port;

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
				       core_audio_devin->device_id,
				       TRUE, FALSE,
				       FALSE);
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

      g_rec_mutex_lock(core_audio_devin_mutex);
      
      if(samplerate == core_audio_devin->samplerate){
	g_rec_mutex_unlock(core_audio_devin_mutex);

	return;
      }

      core_audio_devin->samplerate = samplerate;

      ags_frame_clock_set_samplerate(core_audio_devin->frame_clock,
				     samplerate);
      
      g_rec_mutex_unlock(core_audio_devin_mutex);

      ags_core_audio_devin_realloc_buffer(core_audio_devin);

      /* reset port */
      port = core_audio_devin->core_audio_port;

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
				       core_audio_devin->device_id,
				       TRUE, FALSE,
				       FALSE);
	}
	
	port = port->next;
      }
    }
    break;
  case PROP_BPM:
    {
      gdouble bpm;
      
      bpm = g_value_get_double(value);

      g_rec_mutex_lock(core_audio_devin_mutex);

      core_audio_devin->bpm = bpm;

      ags_frame_clock_set_bpm(core_audio_devin->frame_clock,
			      bpm);
      
      g_rec_mutex_unlock(core_audio_devin_mutex);
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

      g_rec_mutex_lock(core_audio_devin_mutex);

      if(core_audio_devin->core_audio_client == (GObject *) core_audio_client){
	g_rec_mutex_unlock(core_audio_devin_mutex);

	return;
      }

      if(core_audio_devin->core_audio_client != NULL){
	g_object_unref(G_OBJECT(core_audio_devin->core_audio_client));
      }

      if(core_audio_client != NULL){
	g_object_ref(core_audio_client);
      }
      
      core_audio_devin->core_audio_client = (GObject *) core_audio_client;

      g_rec_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      AgsCoreAudioPort *core_audio_port;

      core_audio_port = (AgsCoreAudioPort *) g_value_get_pointer(value);

      g_rec_mutex_lock(core_audio_devin_mutex);

      if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port) ||
	 g_list_find(core_audio_devin->core_audio_port, core_audio_port) != NULL){
	g_rec_mutex_unlock(core_audio_devin_mutex);

	return;
      }

      g_object_ref(core_audio_port);
      core_audio_devin->core_audio_port = g_list_append(core_audio_devin->core_audio_port,
							core_audio_port);

      g_rec_mutex_unlock(core_audio_devin_mutex);
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

  GRecMutex *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(gobject);

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(core_audio_devin_mutex);

      g_value_set_string(value, core_audio_devin->device_name);

      g_rec_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_rec_mutex_lock(core_audio_devin_mutex);

      g_value_set_uint(value, core_audio_devin->dsp_channels);

      g_rec_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_rec_mutex_lock(core_audio_devin_mutex);

      g_value_set_uint(value, core_audio_devin->pcm_channels);

      g_rec_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(core_audio_devin_mutex);

      g_value_set_uint(value, core_audio_devin->format);

      g_rec_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(core_audio_devin_mutex);

      g_value_set_uint(value, core_audio_devin->buffer_size);

      g_rec_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(core_audio_devin_mutex);

      g_value_set_uint(value, core_audio_devin->samplerate);

      g_rec_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(core_audio_devin_mutex);

      g_value_set_double(value, core_audio_devin->bpm);

      g_rec_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(core_audio_devin_mutex);

      g_value_set_pointer(value, core_audio_devin->app_buffer);

      g_rec_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      g_rec_mutex_lock(core_audio_devin_mutex);

      g_value_set_object(value, core_audio_devin->core_audio_client);

      g_rec_mutex_unlock(core_audio_devin_mutex);
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      g_rec_mutex_lock(core_audio_devin_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(core_audio_devin->core_audio_port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(core_audio_devin_mutex);
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

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(gobject);

  /* core audio client */
  if(core_audio_devin->core_audio_client != NULL){
    g_object_unref(core_audio_devin->core_audio_client);

    core_audio_devin->core_audio_client = NULL;
  }

  /* core audio port */
  g_list_free_full(core_audio_devin->core_audio_port,
		   g_object_unref);

  core_audio_devin->core_audio_port = NULL;
  
  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_devin_parent_class)->dispose(gobject);
}

void
ags_core_audio_devin_finalize(GObject *gobject)
{
  AgsCoreAudioDevin *core_audio_devin;

  guint i;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(gobject);

  ags_uuid_free(core_audio_devin->uuid);

  /* frame clock */
  if(core_audio_devin->frame_clock != NULL){
    g_object_unref(core_audio_devin->frame_clock);
  }

  /* app buffer */
  for(i = 0; i < AGS_CORE_AUDIO_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_free(core_audio_devin->app_buffer[i]);
  }

  g_free(core_audio_devin->app_buffer);

  /* app buffer mutex */
  for(i = 0; i < AGS_CORE_AUDIO_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_rec_mutex_clear(core_audio_devin->app_buffer_mutex[i]);
    
    g_free(core_audio_devin->app_buffer_mutex[i]);
  }

  g_free(core_audio_devin->app_buffer_mutex);

  /* sub-block mutex */
  for(i = 0; i < AGS_CORE_AUDIO_DEVIN_DEFAULT_APP_BUFFER_SIZE * core_audio_devin->sub_block_count * core_audio_devin->pcm_channels; i++){
    g_rec_mutex_clear(core_audio_devin->sub_block_mutex[i]);
    
    g_free(core_audio_devin->sub_block_mutex[i]);
  }

  g_free(core_audio_devin->sub_block_mutex);
  
  /* core audio client */
  if(core_audio_devin->core_audio_client != NULL){
    g_object_unref(core_audio_devin->core_audio_client);
  }

  /* core audio port */
  g_list_free_full(core_audio_devin->core_audio_port,
		   g_object_unref);
  
  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_devin_parent_class)->finalize(gobject);
}

AgsUUID*
ags_core_audio_devin_get_uuid(AgsConnectable *connectable)
{
  AgsCoreAudioDevin *core_audio_devin;
  
  AgsUUID *ptr;

  GRecMutex *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(connectable);

  /* get core audio devin signal mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* get UUID */
  g_rec_mutex_lock(core_audio_devin_mutex);

  ptr = core_audio_devin->uuid;

  g_rec_mutex_unlock(core_audio_devin_mutex);
  
  return(ptr);
}

gboolean
ags_core_audio_devin_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_core_audio_devin_is_ready(AgsConnectable *connectable)
{
  AgsCoreAudioDevin *core_audio_devin;
  
  gboolean is_ready;

  GRecMutex *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(connectable);

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* check is ready */
  g_rec_mutex_lock(core_audio_devin_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (core_audio_devin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_devin_mutex);
  
  return(is_ready);
}

void
ags_core_audio_devin_add_to_registry(AgsConnectable *connectable)
{
  AgsCoreAudioDevin *core_audio_devin;

  GRecMutex *core_audio_devin_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(connectable);

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  g_rec_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(core_audio_devin_mutex);
}

void
ags_core_audio_devin_remove_from_registry(AgsConnectable *connectable)
{
  AgsCoreAudioDevin *core_audio_devin;

  GRecMutex *core_audio_devin_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(connectable);

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  g_rec_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(core_audio_devin_mutex);
}

xmlNode*
ags_core_audio_devin_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_core_audio_devin_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_core_audio_devin_xml_parse(AgsConnectable *connectable,
			       xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_core_audio_devin_is_connected(AgsConnectable *connectable)
{
  AgsCoreAudioDevin *core_audio_devin;
  
  gboolean is_connected;

  GRecMutex *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(connectable);

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* check is connected */
  g_rec_mutex_lock(core_audio_devin_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (core_audio_devin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_devin_mutex);
  
  return(is_connected);
}

void
ags_core_audio_devin_connect(AgsConnectable *connectable)
{
  AgsCoreAudioDevin *core_audio_devin;

  GRecMutex *core_audio_devin_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(connectable);

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  g_rec_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(core_audio_devin_mutex);
}

void
ags_core_audio_devin_disconnect(AgsConnectable *connectable)
{
  AgsCoreAudioDevin *core_audio_devin;

  GRecMutex *core_audio_devin_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(connectable);

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  g_rec_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(core_audio_devin_mutex);
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
 * Since: 3.0.0
 */
gboolean
ags_core_audio_devin_test_flags(AgsCoreAudioDevin *core_audio_devin, AgsCoreAudioDevinFlags flags)
{
  gboolean retval;  
  
  GRecMutex *core_audio_devin_mutex;

  if(!AGS_IS_CORE_AUDIO_DEVIN(core_audio_devin)){
    return(FALSE);
  }

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* test */
  g_rec_mutex_lock(core_audio_devin_mutex);

  retval = (flags & (core_audio_devin->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(core_audio_devin_mutex);

  return(retval);
}

/**
 * ags_core_audio_devin_set_flags:
 * @core_audio_devin: the #AgsCoreAudioDevin
 * @flags: see #AgsCoreAudioDevinFlags-enum
 *
 * Enable a feature of @core_audio_devin.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_devin_set_flags(AgsCoreAudioDevin *core_audio_devin, AgsCoreAudioDevinFlags flags)
{
  GRecMutex *core_audio_devin_mutex;

  if(!AGS_IS_CORE_AUDIO_DEVIN(core_audio_devin)){
    return;
  }

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->flags |= flags;
  
  g_rec_mutex_unlock(core_audio_devin_mutex);
}
    
/**
 * ags_core_audio_devin_unset_flags:
 * @core_audio_devin: the #AgsCoreAudioDevin
 * @flags: see #AgsCoreAudioDevinFlags-enum
 *
 * Disable a feature of @core_audio_devin.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_devin_unset_flags(AgsCoreAudioDevin *core_audio_devin, AgsCoreAudioDevinFlags flags)
{  
  GRecMutex *core_audio_devin_mutex;

  if(!AGS_IS_CORE_AUDIO_DEVIN(core_audio_devin)){
    return;
  }

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->flags &= (~flags);
  
  g_rec_mutex_unlock(core_audio_devin_mutex);
}

void
ags_core_audio_devin_set_device(AgsSoundcard *soundcard,
				gchar *device)
{
  AgsCoreAudioDevin *core_audio_devin;
  
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

  GRecMutex *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* check device */
  g_rec_mutex_lock(core_audio_devin_mutex);

  if(core_audio_devin->device_name == device ||
     (core_audio_devin->device_name != NULL &&
      !g_ascii_strcasecmp(core_audio_devin->device_name,
			  device))){
    g_rec_mutex_unlock(core_audio_devin_mutex);
  
    return;
  }

  g_message("input set device - %s", device);
  
  /* get some fields */
  start_core_audio_port = 
    core_audio_port = g_list_copy(core_audio_devin->core_audio_port);

  g_rec_mutex_unlock(core_audio_devin_mutex);
  
  /* unregister */
  is_registered = FALSE;
  
  if(start_core_audio_port != NULL){
    is_registered = (ags_core_audio_port_test_flags((AgsCoreAudioPort *) start_core_audio_port->data, AGS_CORE_AUDIO_PORT_REGISTERED)) ? TRUE: FALSE;

    if(is_registered){
      ags_core_audio_port_unregister(start_core_audio_port->data);
    }
  }
  
  g_free(core_audio_devin->device_name);

  core_audio_devin->device_name = NULL;

  g_free(core_audio_devin->device_id);

  core_audio_devin->device_id = NULL;
  
#if defined(AGS_WITH_CORE_AUDIO)
  devices_property_address.mSelector = kAudioHardwarePropertyDevices;
  devices_property_address.mScope = kAudioObjectPropertyScopeGlobal;
  devices_property_address.mElement = kAudioObjectPropertyElementMaster;

  streams_property_address.mSelector = kAudioDevicePropertyStreams;
  streams_property_address.mScope = kAudioDevicePropertyScopeInput;

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
	
	if([current_manufacturer isEqualToString:@"Apple Inc."] && [current_name isEqualToString:@"Built-in Input"]){
	  device_uid = current_uid;
	}

	is_mic = 0;

	error = AudioObjectGetPropertyDataSize(audio_devices[i], 
					       &streams_property_address, 
					       0, 
					       NULL, 
					       &prop_size);
	
	stream_count = prop_size / sizeof(AudioStreamID);

	if(stream_count > 0){
	  is_mic = YES;
	}

	str = g_strdup_printf("%s - %s",
			      [current_manufacturer UTF8String],
			      [current_name UTF8String]);

	if(is_mic &&
	   !g_ascii_strcasecmp(str, device)){
	  core_audio_devin->device_name = g_strdup(device);
	  
	  core_audio_devin->device_id = g_strdup_printf("in-%s",
							[current_uid UTF8String]);

	  core_audio_devin->audio_device = audio_devices[i];

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
  str = g_strdup(core_audio_devin->device_id);
  
  if(start_core_audio_port != NULL){
    g_object_set(start_core_audio_port->data,
		 "port-name", str,
		 NULL);

    if(is_registered){
      ags_core_audio_port_register(start_core_audio_port->data,
				   str,
				   TRUE, FALSE,
				   FALSE);
    }
  }

  g_list_free(start_core_audio_port);

  g_free(str);
}

gchar*
ags_core_audio_devin_get_device(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;
  
  gchar *device;

  GRecMutex *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);
  
  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  device = NULL;

  /* get device */
  g_rec_mutex_lock(core_audio_devin_mutex);

  device = g_strdup(core_audio_devin->device_name);

  g_rec_mutex_unlock(core_audio_devin_mutex);
  
  return(device);
}

void
ags_core_audio_devin_set_presets(AgsSoundcard *soundcard,
				 guint channels,
				 guint rate,
				 guint buffer_size,
				 AgsSoundcardFormat format)
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
				 AgsSoundcardFormat *format)
{
  AgsCoreAudioDevin *core_audio_devin;

  GRecMutex *core_audio_devin_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);
  
  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* get presets */
  g_rec_mutex_lock(core_audio_devin_mutex);

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

  g_rec_mutex_unlock(core_audio_devin_mutex);
}

/**
 * ags_core_audio_devin_list_cards:
 * @soundcard: the #AgsSoundcard
 * @card_id: CORE AUDIO identifier
 * @card_name: card name
 *
 * List available soundcards.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_devin_list_cards(AgsSoundcard *soundcard,
				GList **card_id, GList **card_name)
{
  AgsCoreAudioDevin *core_audio_devin;

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
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

#if defined(AGS_WITH_CORE_AUDIO)
  devices_property_address.mSelector = kAudioHardwarePropertyDevices;
  devices_property_address.mScope = kAudioObjectPropertyScopeGlobal;
  devices_property_address.mElement = kAudioObjectPropertyElementMaster;

  streams_property_address.mSelector = kAudioDevicePropertyStreams;
  streams_property_address.mScope = kAudioDevicePropertyScopeInput;

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
	
	if([current_manufacturer isEqualToString:@"Apple Inc."] && [current_name isEqualToString:@"Built-in Input"]){
	  device_uid = current_uid;
	}

	is_mic = 0;

	error = AudioObjectGetPropertyDataSize(audio_devices[i], 
					       &streams_property_address, 
					       0, 
					       NULL, 
					       &prop_size);
	
	stream_count = prop_size / sizeof(AudioStreamID);

	if(stream_count > 0){
	  is_mic = YES;
	}

	//	g_message("found %s device: %s - %s <%s>", (!is_mic ? "output": "input"),  [current_manufacturer UTF8String], [current_name UTF8String], [current_uid UTF8String]);

	if(is_mic){
	  *card_id = g_list_prepend(*card_id,
				    g_strdup_printf("in-%s",
						    [current_uid UTF8String]));
       
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
  
  GRecMutex *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* check is starting */
  is_starting = ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_START_RECORD);
  
  return(is_starting);
}

gboolean
ags_core_audio_devin_is_recording(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  gboolean is_recording;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);
  
  /* check is recording */
  is_recording = ags_core_audio_devin_test_flags(core_audio_devin, AGS_CORE_AUDIO_DEVIN_RECORD);

  return(is_recording);
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

  AgsSoundcardFormat format;
  guint word_size;
  
  GRecMutex *core_audio_devin_mutex;

  if(ags_soundcard_is_recording(soundcard)){
    return;
  }
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core-audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* retrieve word size */
  g_rec_mutex_lock(core_audio_devin_mutex);

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
    g_rec_mutex_unlock(core_audio_devin_mutex);
    
    g_warning("ags_core_audio_devin_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for playback */
  core_audio_devin->app_buffer_mode = AGS_CORE_AUDIO_DEVIN_APP_BUFFER_7;  
  core_audio_devin->flags |= (AGS_CORE_AUDIO_DEVIN_START_RECORD |
			      AGS_CORE_AUDIO_DEVIN_RECORD |
			      AGS_CORE_AUDIO_DEVIN_NONBLOCKING);

  memset(core_audio_devin->app_buffer[0], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->app_buffer[1], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->app_buffer[2], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->app_buffer[3], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->app_buffer[4], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->app_buffer[5], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->app_buffer[6], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->app_buffer[7], 0, core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);

  /*  */
  core_audio_devin->tact_counter = 0.0;
  core_audio_devin->delay_counter = 0.0;
  core_audio_devin->tic_counter = 0;

  core_audio_devin->note_offset = core_audio_devin->start_note_offset;
  core_audio_devin->note_offset_absolute = core_audio_devin->start_note_offset;

  core_audio_devin->note_256th_attack_of_16th_pulse = 0;
  core_audio_devin->note_256th_attack_of_16th_pulse_position = 0;

  core_audio_devin->note_256th_delay_counter = 0.0;

  core_audio_devin->note_256th_offset = 16 * core_audio_devin->start_note_offset;

  if(core_audio_devin->note_256th_delay < 1.0){
    guint buffer_size;
    guint note_256th_attack_lower, note_256th_attack_upper;
    guint i;
    
    buffer_size = core_audio_devin->buffer_size;

    note_256th_attack_lower = 0;
    note_256th_attack_upper = 0;
    
    ags_soundcard_get_note_256th_attack(AGS_SOUNDCARD(core_audio_devin),
					&note_256th_attack_lower,
					&note_256th_attack_upper);
    
    if(note_256th_attack_lower < note_256th_attack_upper){
      core_audio_devin->note_256th_offset_last = core_audio_devin->note_256th_offset + floor((note_256th_attack_upper - note_256th_attack_lower) / (core_audio_devin->note_256th_delay * (double) buffer_size));
    }
  }

  core_audio_devin->flags |= (AGS_CORE_AUDIO_DEVIN_INITIALIZED |
			      AGS_CORE_AUDIO_DEVIN_START_RECORD |
			      AGS_CORE_AUDIO_DEVIN_RECORD);
  
  ags_atomic_int_or(&(core_audio_devin->sync_flags),
		    AGS_CORE_AUDIO_DEVIN_INITIAL_CALLBACK);

  g_rec_mutex_unlock(core_audio_devin_mutex);
}

void
ags_core_audio_devin_port_record(AgsSoundcard *soundcard,
				 GError **error)
{
  AgsCoreAudioClient *core_audio_client;
  AgsCoreAudioDevin *core_audio_devin;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
      
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;
  guint word_size;
  gboolean core_audio_client_activated;

  GRecMutex *core_audio_devin_mutex;
  GRecMutex *core_audio_client_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  application_context = ags_application_context_get_instance();
  
  /* get core-audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* client */
  g_rec_mutex_lock(core_audio_devin_mutex);

  core_audio_client = (AgsCoreAudioClient *) core_audio_devin->core_audio_client;
  
  callback_mutex = &(core_audio_devin->callback_mutex);
  callback_finish_mutex = &(core_audio_devin->callback_finish_mutex);

  /* do playback */  
  core_audio_devin->flags &= (~AGS_CORE_AUDIO_DEVIN_START_RECORD);
  
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
    g_rec_mutex_unlock(core_audio_devin_mutex);
    
    g_warning("ags_core_audio_devin_port_record(): unsupported word size");
    
    return;
  }

  g_rec_mutex_unlock(core_audio_devin_mutex);

  /* get client mutex */
  core_audio_client_mutex = AGS_CORE_AUDIO_CLIENT_GET_OBJ_MUTEX(core_audio_client);

  /* get activated */
  g_rec_mutex_lock(core_audio_client_mutex);

  core_audio_client_activated = ((AGS_CORE_AUDIO_CLIENT_ACTIVATED & (core_audio_client->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_client_mutex);

  if(core_audio_client_activated){
    while((AGS_CORE_AUDIO_DEVIN_PASS_THROUGH & (ags_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
      usleep(4);
    }

    if((AGS_CORE_AUDIO_DEVIN_INITIAL_CALLBACK & (ags_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
      ags_atomic_int_or(&(core_audio_devin->sync_flags),
			AGS_CORE_AUDIO_DEVIN_PRE_SYNC_DEVICE_WAIT);
	
      while(((AGS_CORE_AUDIO_DEVIN_PRE_SYNC_DEVICE_WAIT & (ags_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0 ||
	     (AGS_CORE_AUDIO_DEVIN_PRE_SYNC_CALLBACK_WAIT & (ags_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0) &&
	    ((AGS_CORE_AUDIO_DEVIN_PRE_SYNC_DEVICE_DONE & (ags_atomic_int_get(&(core_audio_devin->sync_flags)))) == 0 ||
	     (AGS_CORE_AUDIO_DEVIN_PRE_SYNC_CALLBACK_DONE & (ags_atomic_int_get(&(core_audio_devin->sync_flags)))) == 0)){
	if((AGS_CORE_AUDIO_DEVIN_PRE_SYNC_DEVICE_WAIT & (ags_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0 &&
	   (AGS_CORE_AUDIO_DEVIN_PRE_SYNC_CALLBACK_WAIT & (ags_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
	  ags_atomic_int_or(&(core_audio_devin->sync_flags),
			    AGS_CORE_AUDIO_DEVIN_PRE_SYNC_DEVICE_DONE);
	}
	
	g_thread_yield();
      }
	
      ags_atomic_int_and(&(core_audio_devin->sync_flags),
			 (~AGS_CORE_AUDIO_DEVIN_INITIAL_CALLBACK));

      ags_atomic_int_and(&(core_audio_devin->sync_flags),
			 (~(AGS_CORE_AUDIO_DEVIN_PRE_SYNC_DEVICE_WAIT |
			    AGS_CORE_AUDIO_DEVIN_PRE_SYNC_CALLBACK_WAIT |
			    AGS_CORE_AUDIO_DEVIN_PRE_SYNC_DEVICE_DONE |
			    AGS_CORE_AUDIO_DEVIN_PRE_SYNC_CALLBACK_DONE)));
    }
    
    /* signal */
    g_mutex_lock(callback_mutex);

    ags_atomic_int_or(&(core_audio_devin->sync_flags),
		      AGS_CORE_AUDIO_DEVIN_CALLBACK_DONE);
    
    if((AGS_CORE_AUDIO_DEVIN_CALLBACK_WAIT & (ags_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
      g_cond_signal(&(core_audio_devin->callback_cond));
    }

    g_mutex_unlock(callback_mutex);
    //    }
    
    /* wait callback */	
    g_mutex_lock(callback_finish_mutex);
    
    if((AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_DONE & (ags_atomic_int_get(&(core_audio_devin->sync_flags)))) == 0){
      ags_atomic_int_or(&(core_audio_devin->sync_flags),
			AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_WAIT);
    
      while((AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_DONE & (ags_atomic_int_get(&(core_audio_devin->sync_flags)))) == 0 &&
	    (AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_WAIT & (ags_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
	g_cond_wait(&(core_audio_devin->callback_finish_cond),
		    callback_finish_mutex);
      }
    }
      
    ags_atomic_int_and(&(core_audio_devin->sync_flags),
		       (~(AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_WAIT |
			  AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_DONE)));
    
    g_mutex_unlock(callback_finish_mutex);
  }
  
  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

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
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_core_audio_devin_port_free(AgsSoundcard *soundcard)
{
  AgsCoreAudioPort *core_audio_port;
  AgsCoreAudioDevin *core_audio_devin;

  guint word_size;

  guint i;

  GRecMutex *core_audio_devin_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);
  
  /* get core-audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /*  */
  g_rec_mutex_lock(core_audio_devin_mutex);

  callback_mutex = &(core_audio_devin->callback_mutex);
  callback_finish_mutex = &(core_audio_devin->callback_finish_mutex);
  
  //  ags_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);
  
  core_audio_devin->app_buffer_mode = AGS_CORE_AUDIO_DEVIN_APP_BUFFER_0;
  core_audio_devin->flags &= (~(AGS_CORE_AUDIO_DEVIN_RECORD));

  ags_atomic_int_or(&(core_audio_devin->sync_flags),
		    AGS_CORE_AUDIO_DEVIN_PASS_THROUGH);

  ags_atomic_int_and(&(core_audio_devin->sync_flags),
		     (~AGS_CORE_AUDIO_DEVIN_INITIAL_CALLBACK));

  ags_atomic_int_and(&(core_audio_devin->sync_flags),
		     (~(AGS_CORE_AUDIO_DEVIN_PRE_SYNC_DEVICE_WAIT |
			AGS_CORE_AUDIO_DEVIN_PRE_SYNC_CALLBACK_WAIT |
			AGS_CORE_AUDIO_DEVIN_PRE_SYNC_DEVICE_DONE |
			AGS_CORE_AUDIO_DEVIN_PRE_SYNC_CALLBACK_DONE)));

  /* signal callback */
  g_mutex_lock(callback_mutex);

  ags_atomic_int_or(&(core_audio_devin->sync_flags),
		    AGS_CORE_AUDIO_DEVIN_CALLBACK_DONE);
    
  if((AGS_CORE_AUDIO_DEVIN_CALLBACK_WAIT & (ags_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
    g_cond_signal(&(core_audio_devin->callback_cond));
  }

  g_mutex_unlock(callback_mutex);

  /* signal thread */
  g_mutex_lock(callback_finish_mutex);

  ags_atomic_int_or(&(core_audio_devin->sync_flags),
		    AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_DONE);
    
  if((AGS_CORE_AUDIO_DEVIN_CALLBACK_FINISH_WAIT & (ags_atomic_int_get(&(core_audio_devin->sync_flags)))) != 0){
    g_cond_signal(&(core_audio_devin->callback_finish_cond));
  }

  g_mutex_unlock(callback_finish_mutex);
  
  /*  */
  core_audio_devin->note_offset = core_audio_devin->start_note_offset;
  core_audio_devin->note_offset_absolute = core_audio_devin->start_note_offset;

  core_audio_devin->note_256th_attack_of_16th_pulse = 0;
  core_audio_devin->note_256th_attack_of_16th_pulse_position = 0;
  
  core_audio_devin->note_256th_offset = 16 * core_audio_devin->start_note_offset;
  
  if(core_audio_devin->note_256th_delay < 1.0){
    guint buffer_size;
    guint note_256th_attack_lower, note_256th_attack_upper;
    guint i;
    
    buffer_size = core_audio_devin->buffer_size;

    note_256th_attack_lower = 0;
    note_256th_attack_upper = 0;
    
    ags_soundcard_get_note_256th_attack(AGS_SOUNDCARD(core_audio_devin),
					&note_256th_attack_lower,
					&note_256th_attack_upper);
    
    if(note_256th_attack_lower < note_256th_attack_upper){
      core_audio_devin->note_256th_offset_last = core_audio_devin->note_256th_offset + floor((note_256th_attack_upper - note_256th_attack_lower) / (core_audio_devin->note_256th_delay * (double) buffer_size));
    }
  }

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
    
    g_critical("ags_core_audio_devin_free(): unsupported word size");
  }

  g_rec_mutex_unlock(core_audio_devin_mutex);

  if(core_audio_devin->core_audio_port != NULL){
    core_audio_port = core_audio_devin->core_audio_port->data;

    while(!ags_atomic_int_get(&(core_audio_port->is_empty))) usleep(500000);
  }

  g_rec_mutex_lock(core_audio_devin_mutex);
  
  memset(core_audio_devin->app_buffer[0], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->app_buffer[1], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->app_buffer[2], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->app_buffer[3], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->app_buffer[4], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->app_buffer[5], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->app_buffer[6], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);
  memset(core_audio_devin->app_buffer[7], 0, (size_t) core_audio_devin->pcm_channels * core_audio_devin->buffer_size * word_size);

  g_rec_mutex_unlock(core_audio_devin_mutex);
}

void
ags_core_audio_devin_tic(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  GRecMutex *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);
  
  /* get core_audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* frame clock */
  g_rec_mutex_lock(core_audio_devin_mutex);
  
  ags_frame_clock_increment_counter(core_audio_devin->frame_clock);

  /* 16th pulse */
  if(ags_frame_clock_get_has_16th_pulse(core_audio_devin->frame_clock)){
    ags_soundcard_offset_changed(soundcard,
				 core_audio_devin->frame_clock->note_offset);
  }
  
  g_rec_mutex_unlock(core_audio_devin_mutex);
}

void
ags_core_audio_devin_offset_changed(AgsSoundcard *soundcard,
				    guint64 note_offset)
{
  //empty
}

void
ags_core_audio_devin_set_bpm(AgsSoundcard *soundcard,
			     gdouble bpm)
{
  AgsCoreAudioDevin *core_audio_devin;

  GRecMutex *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* set bpm */
  g_rec_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->bpm = bpm;

  ags_frame_clock_set_bpm(core_audio_devin->frame_clock,
			  bpm);
  
  g_rec_mutex_unlock(core_audio_devin_mutex);
}

gdouble
ags_core_audio_devin_get_bpm(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  gdouble bpm;
  
  GRecMutex *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* get bpm */
  g_rec_mutex_lock(core_audio_devin_mutex);

  bpm = core_audio_devin->bpm;
  
  g_rec_mutex_unlock(core_audio_devin_mutex);

  return(bpm);
}

void
ags_core_audio_devin_set_start_note_offset(AgsSoundcard *soundcard,
					   guint64 start_note_offset)
{
  AgsCoreAudioDevin *core_audio_devin;

  GRecMutex *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* set start note offset */
  g_rec_mutex_lock(core_audio_devin_mutex);

  core_audio_devin->start_note_offset = start_note_offset;
  
  ags_frame_clock_set_start_note_offset(core_audio_devin->frame_clock,
					start_note_offset);
  
  g_rec_mutex_unlock(core_audio_devin_mutex);
}

guint64
ags_core_audio_devin_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  guint64 start_note_offset;
  
  GRecMutex *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* get start note offset */
  g_rec_mutex_lock(core_audio_devin_mutex);

  start_note_offset = core_audio_devin->start_note_offset;
  
  g_rec_mutex_unlock(core_audio_devin_mutex);

  return(start_note_offset);
}

GObject*
ags_core_audio_devin_get_frame_clock(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  GObject *frame_clock;
  
  GRecMutex *core_audio_devin_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* get frame clock */
  g_rec_mutex_lock(core_audio_devin_mutex);

  frame_clock = (GObject *) core_audio_devin->frame_clock;
  
  g_rec_mutex_unlock(core_audio_devin_mutex);

  return(frame_clock);
}

void*
ags_core_audio_devin_get_buffer(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  void *buffer;

  GRecMutex *core_audio_devin_mutex;  
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);
  
  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  g_rec_mutex_lock(core_audio_devin_mutex);

  if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_0){
    buffer = core_audio_devin->app_buffer[0];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_1){
    buffer = core_audio_devin->app_buffer[1];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_2){
    buffer = core_audio_devin->app_buffer[2];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_3){
    buffer = core_audio_devin->app_buffer[3];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_4){
    buffer = core_audio_devin->app_buffer[4];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_5){
    buffer = core_audio_devin->app_buffer[5];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_6){
    buffer = core_audio_devin->app_buffer[6];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_7){
    buffer = core_audio_devin->app_buffer[7];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(core_audio_devin_mutex);

  return(buffer);
}

void*
ags_core_audio_devin_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  void *buffer;

  GRecMutex *core_audio_devin_mutex;  
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);
  
  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  g_rec_mutex_lock(core_audio_devin_mutex);

  if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_0){
    buffer = core_audio_devin->app_buffer[1];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_1){
    buffer = core_audio_devin->app_buffer[2];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_2){
    buffer = core_audio_devin->app_buffer[3];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_3){
    buffer = core_audio_devin->app_buffer[4];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_4){
    buffer = core_audio_devin->app_buffer[5];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_5){
    buffer = core_audio_devin->app_buffer[6];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_6){
    buffer = core_audio_devin->app_buffer[7];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_7){
    buffer = core_audio_devin->app_buffer[0];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(core_audio_devin_mutex);

  return(buffer);
}

void*
ags_core_audio_devin_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevin *core_audio_devin;

  void *buffer;

  GRecMutex *core_audio_devin_mutex;  
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  g_rec_mutex_lock(core_audio_devin_mutex);

  if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_0){
    buffer = core_audio_devin->app_buffer[7];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_1){
    buffer = core_audio_devin->app_buffer[0];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_2){
    buffer = core_audio_devin->app_buffer[1];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_3){
    buffer = core_audio_devin->app_buffer[2];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_4){
    buffer = core_audio_devin->app_buffer[3];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_5){
    buffer = core_audio_devin->app_buffer[4];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_6){
    buffer = core_audio_devin->app_buffer[5];
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_7){
    buffer = core_audio_devin->app_buffer[6];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(core_audio_devin_mutex);

  return(buffer);
}

void
ags_core_audio_devin_lock_buffer(AgsSoundcard *soundcard,
				 void *buffer)
{
  AgsCoreAudioDevin *core_audio_devin;

  GRecMutex *buffer_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(core_audio_devin->app_buffer != NULL){
    if(buffer == core_audio_devin->app_buffer[0]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[0];
    }else if(buffer == core_audio_devin->app_buffer[1]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[1];
    }else if(buffer == core_audio_devin->app_buffer[2]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[2];
    }else if(buffer == core_audio_devin->app_buffer[3]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[3];
    }else if(buffer == core_audio_devin->app_buffer[4]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[4];
    }else if(buffer == core_audio_devin->app_buffer[5]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[5];
    }else if(buffer == core_audio_devin->app_buffer[6]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[6];
    }else if(buffer == core_audio_devin->app_buffer[7]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[7];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_core_audio_devin_unlock_buffer(AgsSoundcard *soundcard,
				   void *buffer)
{
  AgsCoreAudioDevin *core_audio_devin;

  GRecMutex *buffer_mutex;
  
  core_audio_devin = AGS_CORE_AUDIO_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(core_audio_devin->app_buffer != NULL){
    if(buffer == core_audio_devin->app_buffer[0]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[0];
    }else if(buffer == core_audio_devin->app_buffer[1]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[1];
    }else if(buffer == core_audio_devin->app_buffer[2]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[2];
    }else if(buffer == core_audio_devin->app_buffer[3]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[3];
    }else if(buffer == core_audio_devin->app_buffer[4]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[4];
    }else if(buffer == core_audio_devin->app_buffer[5]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[5];
    }else if(buffer == core_audio_devin->app_buffer[6]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[6];
    }else if(buffer == core_audio_devin->app_buffer[7]){
      buffer_mutex = core_audio_devin->app_buffer_mutex[7];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

/**
 * ags_core_audio_devin_switch_buffer_flag:
 * @core_audio_devin: an #AgsCoreAudioDevin
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_devin_switch_buffer_flag(AgsCoreAudioDevin *core_audio_devin)
{
  GRecMutex *core_audio_devin_mutex;
  
  if(!AGS_IS_CORE_AUDIO_DEVIN(core_audio_devin)){
    return;
  }

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* switch buffer flag */
  g_rec_mutex_lock(core_audio_devin_mutex);

  if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_0){
    core_audio_devin->app_buffer_mode = AGS_CORE_AUDIO_DEVIN_APP_BUFFER_1;
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_1){
    core_audio_devin->app_buffer_mode = AGS_CORE_AUDIO_DEVIN_APP_BUFFER_2;
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_2){
    core_audio_devin->app_buffer_mode = AGS_CORE_AUDIO_DEVIN_APP_BUFFER_3;
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_3){
    core_audio_devin->app_buffer_mode = AGS_CORE_AUDIO_DEVIN_APP_BUFFER_4;
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_4){
    core_audio_devin->app_buffer_mode = AGS_CORE_AUDIO_DEVIN_APP_BUFFER_5;
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_5){
    core_audio_devin->app_buffer_mode = AGS_CORE_AUDIO_DEVIN_APP_BUFFER_6;
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_6){
    core_audio_devin->app_buffer_mode = AGS_CORE_AUDIO_DEVIN_APP_BUFFER_7;
  }else if(core_audio_devin->app_buffer_mode == AGS_CORE_AUDIO_DEVIN_APP_BUFFER_7){
    core_audio_devin->app_buffer_mode = AGS_CORE_AUDIO_DEVIN_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(core_audio_devin_mutex);
}

/**
 * ags_core_audio_devin_realloc_buffer:
 * @core_audio_devin: the #AgsCoreAudioDevin
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_devin_realloc_buffer(AgsCoreAudioDevin *core_audio_devin)
{
  guint pcm_channels;
  guint buffer_size;
  AgsSoundcardFormat format;
  guint word_size;
  gboolean is_float;

  GRecMutex *core_audio_devin_mutex;  

  if(!AGS_IS_CORE_AUDIO_DEVIN(core_audio_devin)){
    return;
  }

  /* get core audio devin mutex */
  core_audio_devin_mutex = AGS_CORE_AUDIO_DEVIN_GET_OBJ_MUTEX(core_audio_devin);

  /* get word size */  
  g_rec_mutex_lock(core_audio_devin_mutex);

  pcm_channels = core_audio_devin->pcm_channels;
  buffer_size = core_audio_devin->buffer_size;

  format = core_audio_devin->format;
  
  g_rec_mutex_unlock(core_audio_devin_mutex);

  is_float = FALSE;

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

      is_float = TRUE;
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      word_size = sizeof(gdouble);

      is_float = TRUE;
    }
    break;
  default:
    g_warning("ags_core_audio_devin_realloc_buffer(): unsupported word size");
    
    return;
  }

  g_message("allocating soundcard buffer - is_float %d and word_size %d", is_float, word_size);
  
  /* AGS_CORE_AUDIO_DEVIN_APP_BUFFER_0 */
  if(core_audio_devin->app_buffer[0] != NULL){
    g_free(core_audio_devin->app_buffer[0]);
  }
  
  core_audio_devin->app_buffer[0] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devin->app_buffer[0], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVIN_APP_BUFFER_1 */
  if(core_audio_devin->app_buffer[1] != NULL){
    g_free(core_audio_devin->app_buffer[1]);
  }

  core_audio_devin->app_buffer[1] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  memset(core_audio_devin->app_buffer[1], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVIN_APP_BUFFER_2 */
  if(core_audio_devin->app_buffer[2] != NULL){
    g_free(core_audio_devin->app_buffer[2]);
  }

  core_audio_devin->app_buffer[2] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  memset(core_audio_devin->app_buffer[2], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVIN_APP_BUFFER_3 */
  if(core_audio_devin->app_buffer[3] != NULL){
    g_free(core_audio_devin->app_buffer[3]);
  }
  
  core_audio_devin->app_buffer[3] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devin->app_buffer[3], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVIN_APP_BUFFER_4 */
  if(core_audio_devin->app_buffer[4] != NULL){
    g_free(core_audio_devin->app_buffer[4]);
  }
  
  core_audio_devin->app_buffer[4] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devin->app_buffer[4], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVIN_APP_BUFFER_5 */
  if(core_audio_devin->app_buffer[5] != NULL){
    g_free(core_audio_devin->app_buffer[5]);
  }
  
  core_audio_devin->app_buffer[5] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devin->app_buffer[5], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVIN_APP_BUFFER_6 */
  if(core_audio_devin->app_buffer[6] != NULL){
    g_free(core_audio_devin->app_buffer[6]);
  }
  
  core_audio_devin->app_buffer[6] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devin->app_buffer[6], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVIN_APP_BUFFER_7 */
  if(core_audio_devin->app_buffer[7] != NULL){
    g_free(core_audio_devin->app_buffer[7]);
  }
  
  core_audio_devin->app_buffer[7] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devin->app_buffer[7], 0, pcm_channels * buffer_size * word_size);  
}

/**
 * ags_core_audio_devin_new:
 *
 * Creates a new instance of #AgsCoreAudioDevin.
 *
 * Returns: a new #AgsCoreAudioDevin
 *
 * Since: 3.0.0
 */
AgsCoreAudioDevin*
ags_core_audio_devin_new()
{
  AgsCoreAudioDevin *core_audio_devin;

  core_audio_devin = (AgsCoreAudioDevin *) g_object_new(AGS_TYPE_CORE_AUDIO_DEVIN,
							NULL);
  
  return(core_audio_devin);
}
