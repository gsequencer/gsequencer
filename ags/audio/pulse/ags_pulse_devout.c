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

#include <ags/audio/pulse/ags_pulse_devout.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_soundcard_util.h>

#include <ags/audio/pulse/ags_pulse_server.h>
#include <ags/audio/pulse/ags_pulse_client.h>
#include <ags/audio/pulse/ags_pulse_port.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <string.h>
#include <math.h>
#include <time.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_pulse_devout_class_init(AgsPulseDevoutClass *pulse_devout);
void ags_pulse_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pulse_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_pulse_devout_init(AgsPulseDevout *pulse_devout);
void ags_pulse_devout_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec);
void ags_pulse_devout_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec);
void ags_pulse_devout_dispose(GObject *gobject);
void ags_pulse_devout_finalize(GObject *gobject);

AgsUUID* ags_pulse_devout_get_uuid(AgsConnectable *connectable);
gboolean ags_pulse_devout_has_resource(AgsConnectable *connectable);
gboolean ags_pulse_devout_is_ready(AgsConnectable *connectable);
void ags_pulse_devout_add_to_registry(AgsConnectable *connectable);
void ags_pulse_devout_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_pulse_devout_list_resource(AgsConnectable *connectable);
xmlNode* ags_pulse_devout_xml_compose(AgsConnectable *connectable);
void ags_pulse_devout_xml_parse(AgsConnectable *connectable,
				xmlNode *node);
gboolean ags_pulse_devout_is_connected(AgsConnectable *connectable);
void ags_pulse_devout_connect(AgsConnectable *connectable);
void ags_pulse_devout_disconnect(AgsConnectable *connectable);

void ags_pulse_devout_set_device(AgsSoundcard *soundcard,
				 gchar *device);
gchar* ags_pulse_devout_get_device(AgsSoundcard *soundcard);

void ags_pulse_devout_set_presets(AgsSoundcard *soundcard,
				  guint channels,
				  guint rate,
				  guint buffer_size,
				  AgsSoundcardFormat format);
void ags_pulse_devout_get_presets(AgsSoundcard *soundcard,
				  guint *channels,
				  guint *rate,
				  guint *buffer_size,
				  AgsSoundcardFormat *format);

void ags_pulse_devout_list_cards(AgsSoundcard *soundcard,
				 GList **card_id, GList **card_name);
void ags_pulse_devout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			       guint *channels_min, guint *channels_max,
			       guint *rate_min, guint *rate_max,
			       guint *buffer_size_min, guint *buffer_size_max,
			       GError **error);
guint ags_pulse_devout_get_capability(AgsSoundcard *soundcard);

gboolean ags_pulse_devout_is_starting(AgsSoundcard *soundcard);
gboolean ags_pulse_devout_is_playing(AgsSoundcard *soundcard);

gchar* ags_pulse_devout_get_uptime(AgsSoundcard *soundcard);

void ags_pulse_devout_port_init(AgsSoundcard *soundcard,
				GError **error);
void ags_pulse_devout_port_play(AgsSoundcard *soundcard,
				GError **error);
void ags_pulse_devout_port_free(AgsSoundcard *soundcard);

void ags_pulse_devout_tic(AgsSoundcard *soundcard);
void ags_pulse_devout_offset_changed(AgsSoundcard *soundcard,
				     guint64 note_offset);

void ags_pulse_devout_set_bpm(AgsSoundcard *soundcard,
			      gdouble bpm);
gdouble ags_pulse_devout_get_bpm(AgsSoundcard *soundcard);

void ags_pulse_devout_set_start_note_offset(AgsSoundcard *soundcard,
					    guint64 start_note_offset);
guint64 ags_pulse_devout_get_start_note_offset(AgsSoundcard *soundcard);

GObject* ags_pulse_devout_get_frame_clock(AgsSoundcard *soundcard);

void* ags_pulse_devout_get_buffer(AgsSoundcard *soundcard);
void* ags_pulse_devout_get_next_buffer(AgsSoundcard *soundcard);
void* ags_pulse_devout_get_prev_buffer(AgsSoundcard *soundcard);

void ags_pulse_devout_lock_buffer(AgsSoundcard *soundcard,
				  void *buffer);
void ags_pulse_devout_unlock_buffer(AgsSoundcard *soundcard,
				    void *buffer);

guint ags_pulse_devout_get_sub_block_count(AgsSoundcard *soundcard);

gboolean ags_pulse_devout_trylock_sub_block(AgsSoundcard *soundcard,
					    void *buffer, guint sub_block);
void ags_pulse_devout_unlock_sub_block(AgsSoundcard *soundcard,
				       void *buffer, guint sub_block);

/**
 * SECTION:ags_pulse_devout
 * @short_description: Output to soundcard
 * @title: AgsPulseDevout
 * @section_id:
 * @include: ags/audio/pulse/ags_pulse_devout.h
 *
 * #AgsPulseDevout represents a soundcard and supports output.
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
  PROP_PULSE_CLIENT,
  PROP_PULSE_PORT,
};

static gpointer ags_pulse_devout_parent_class = NULL;

GType
ags_pulse_devout_get_type (void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_pulse_devout = 0;

    static const GTypeInfo ags_pulse_devout_info = {
      sizeof(AgsPulseDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pulse_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPulseDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pulse_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pulse_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_pulse_devout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_pulse_devout = g_type_register_static(G_TYPE_OBJECT,
						   "AgsPulseDevout",
						   &ags_pulse_devout_info,
						   0);

    g_type_add_interface_static(ags_type_pulse_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_pulse_devout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_pulse_devout);
  }

  return(g_define_type_id__static);
}

GType
ags_pulse_devout_flags_get_type()
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_PULSE_DEVOUT_INITIALIZED, "AGS_PULSE_DEVOUT_INITIALIZED", "pulse-devout-initialized" },
      { AGS_PULSE_DEVOUT_START_PLAY, "AGS_PULSE_DEVOUT_START_PLAY", "pulse-devout-start-play" },
      { AGS_PULSE_DEVOUT_PLAY, "AGS_PULSE_DEVOUT_PLAY", "pulse-devout-play" },
      { AGS_PULSE_DEVOUT_SHUTDOWN, "AGS_PULSE_DEVOUT_SHUTDOWN", "pulse-devout-shutdown" },
      { AGS_PULSE_DEVOUT_NONBLOCKING, "AGS_PULSE_DEVOUT_NONBLOCKING", "pulse-devout-nonblocking" },
      { AGS_PULSE_DEVOUT_ATTACK_FIRST, "AGS_PULSE_DEVOUT_ATTACK_FIRST", "pulse-devout-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsPulseDevoutFlags"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
}

void
ags_pulse_devout_class_init(AgsPulseDevoutClass *pulse_devout)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_pulse_devout_parent_class = g_type_class_peek_parent(pulse_devout);

  /* GObjectClass */
  gobject = (GObjectClass *) pulse_devout;

  gobject->set_property = ags_pulse_devout_set_property;
  gobject->get_property = ags_pulse_devout_get_property;

  gobject->dispose = ags_pulse_devout_dispose;
  gobject->finalize = ags_pulse_devout_finalize;

  /* properties */
  /**
   * AgsPulseDevout:device:
   *
   * The pulse soundcard indentifier
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-pulse-devout-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsPulseDevout:dsp-channels:
   *
   * The dsp channel count
   * 
   * Since: 3.0.0
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
   * AgsPulseDevout:pcm-channels:
   *
   * The pcm channel count
   * 
   * Since: 3.0.0
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
   * AgsPulseDevout:format:
   *
   * The precision of the buffer
   * 
   * Since: 3.0.0
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
   * AgsPulseDevout:buffer-size:
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
   * AgsPulseDevout:samplerate:
   *
   * The samplerate
   * 
   * Since: 3.0.0
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
   * AgsPulseDevout:bpm:
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
				   AGS_SOUNDCARD_DEFAULT_BPM,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  /**
   * AgsPulseDevout:buffer:
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
   * AgsPulseDevout:pulse-client:
   *
   * The assigned #AgsPulseClient
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("pulse-client",
				   i18n_pspec("pulse client object"),
				   i18n_pspec("The pulse client object"),
				   AGS_TYPE_PULSE_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PULSE_CLIENT,
				  param_spec);

  /**
   * AgsPulseDevout:pulse-port:
   *
   * The assigned #AgsPulsePort
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("pulse-port",
				    i18n_pspec("pulse port object"),
				    i18n_pspec("The pulse port object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PULSE_PORT,
				  param_spec);
}

GQuark
ags_pulse_devout_error_quark()
{
  return(g_quark_from_static_string("ags-pulse-devout-error-quark"));
}

void
ags_pulse_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_pulse_devout_get_uuid;
  connectable->has_resource = ags_pulse_devout_has_resource;

  connectable->is_ready = ags_pulse_devout_is_ready;
  connectable->add_to_registry = ags_pulse_devout_add_to_registry;
  connectable->remove_from_registry = ags_pulse_devout_remove_from_registry;

  connectable->list_resource = ags_pulse_devout_list_resource;
  connectable->xml_compose = ags_pulse_devout_xml_compose;
  connectable->xml_parse = ags_pulse_devout_xml_parse;

  connectable->is_connected = ags_pulse_devout_is_connected;  
  connectable->connect = ags_pulse_devout_connect;
  connectable->disconnect = ags_pulse_devout_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_pulse_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_pulse_devout_set_device;
  soundcard->get_device = ags_pulse_devout_get_device;
  
  soundcard->set_presets = ags_pulse_devout_set_presets;
  soundcard->get_presets = ags_pulse_devout_get_presets;

  soundcard->list_cards = ags_pulse_devout_list_cards;
  soundcard->pcm_info = ags_pulse_devout_pcm_info;
  soundcard->get_capability = ags_pulse_devout_get_capability;

  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_pulse_devout_is_starting;
  soundcard->is_playing = ags_pulse_devout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_pulse_devout_get_uptime;
  
  soundcard->play_init = ags_pulse_devout_port_init;
  soundcard->play = ags_pulse_devout_port_play;

  soundcard->record_init = NULL;
  soundcard->record = NULL;

  soundcard->stop = ags_pulse_devout_port_free;

  soundcard->tic = ags_pulse_devout_tic;
  soundcard->offset_changed = ags_pulse_devout_offset_changed;
    
  soundcard->set_bpm = ags_pulse_devout_set_bpm;
  soundcard->get_bpm = ags_pulse_devout_get_bpm;

  soundcard->set_start_note_offset = ags_pulse_devout_set_start_note_offset;
  soundcard->get_start_note_offset = ags_pulse_devout_get_start_note_offset;

  soundcard->get_frame_clock = ags_pulse_devout_get_frame_clock;

  soundcard->get_buffer = ags_pulse_devout_get_buffer;
  soundcard->get_next_buffer = ags_pulse_devout_get_next_buffer;
  soundcard->get_prev_buffer = ags_pulse_devout_get_prev_buffer;

  soundcard->lock_buffer = ags_pulse_devout_lock_buffer;
  soundcard->unlock_buffer = ags_pulse_devout_unlock_buffer;

  soundcard->get_sub_block_count = ags_pulse_devout_get_sub_block_count;

  soundcard->trylock_sub_block = ags_pulse_devout_trylock_sub_block;
  soundcard->unlock_sub_block = ags_pulse_devout_unlock_sub_block;
}

void
ags_pulse_devout_init(AgsPulseDevout *pulse_devout)
{
  AgsConfig *config;

  gchar *str;
  gchar *segmentation;

  gdouble absolute_delay;
  guint denominator, numerator;
  guint i;

  /* config */
  config = ags_config_get_instance();
    
  /* flags */
  pulse_devout->flags = 0;
  pulse_devout->connectable_flags = 0;
  ags_atomic_int_set(&(pulse_devout->sync_flags),
		   AGS_PULSE_DEVOUT_PASS_THROUGH);

  /* devout mutex */
  g_rec_mutex_init(&(pulse_devout->obj_mutex));

  /* uuid */
  pulse_devout->uuid = ags_uuid_alloc();
  ags_uuid_generate(pulse_devout->uuid);

  pulse_devout->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  pulse_devout->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  pulse_devout->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  pulse_devout->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  pulse_devout->format = ags_soundcard_helper_config_get_format(config);

  /* bpm */
  pulse_devout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* start note offset */
  pulse_devout->start_note_offset = 0;

  /* frame clock */
  pulse_devout->frame_clock = ags_frame_clock_new();

  /* app buffer mutex */
  pulse_devout->app_buffer_mode = AGS_PULSE_DEVOUT_APP_BUFFER_0;

  pulse_devout->app_buffer_mutex = (GRecMutex **) g_malloc(8 * sizeof(GRecMutex *));

  for(i = 0; i < 8; i++){
    pulse_devout->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(pulse_devout->app_buffer_mutex[i]);
  }

  /* sub-block */
  pulse_devout->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  pulse_devout->sub_block_mutex = (GRecMutex **) g_malloc(8 * pulse_devout->sub_block_count * pulse_devout->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < 8 * pulse_devout->sub_block_count * pulse_devout->pcm_channels; i++){
    pulse_devout->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(pulse_devout->sub_block_mutex[i]);
  }

  /* app buffer */
  pulse_devout->app_buffer = (void **) g_malloc(8 * sizeof(void*));

  pulse_devout->app_buffer[0] = NULL;
  pulse_devout->app_buffer[1] = NULL;
  pulse_devout->app_buffer[2] = NULL;
  pulse_devout->app_buffer[3] = NULL;
  pulse_devout->app_buffer[4] = NULL;
  pulse_devout->app_buffer[5] = NULL;
  pulse_devout->app_buffer[6] = NULL;
  pulse_devout->app_buffer[7] = NULL;
  
  ags_pulse_devout_realloc_buffer(pulse_devout);
  
  /*  */
  pulse_devout->card_uri = NULL;
  pulse_devout->pulse_client = NULL;

  pulse_devout->port_name = NULL;
  pulse_devout->pulse_port = NULL;

  /* callback mutex */
  g_mutex_init(&(pulse_devout->callback_mutex));

  g_cond_init(&(pulse_devout->callback_cond));

  /* callback finish mutex */
  g_mutex_init(&(pulse_devout->callback_finish_mutex));

  g_cond_init(&(pulse_devout->callback_finish_cond));
}

void
ags_pulse_devout_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec)
{
  AgsPulseDevout *pulse_devout;
  
  GRecMutex *pulse_devout_mutex;

  pulse_devout = AGS_PULSE_DEVOUT(gobject);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      g_rec_mutex_lock(pulse_devout_mutex);

      pulse_devout->card_uri = g_strdup(device);

      g_rec_mutex_unlock(pulse_devout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      g_rec_mutex_lock(pulse_devout_mutex);

      if(dsp_channels == pulse_devout->dsp_channels){
	g_rec_mutex_unlock(pulse_devout_mutex);

	return;
      }

      pulse_devout->dsp_channels = dsp_channels;

      g_rec_mutex_unlock(pulse_devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels, old_pcm_channels;
      guint i;

      pcm_channels = g_value_get_uint(value);

      g_rec_mutex_lock(pulse_devout_mutex);

      if(pcm_channels == pulse_devout->pcm_channels){
	g_rec_mutex_unlock(pulse_devout_mutex);

	return;
      }

      old_pcm_channels = pulse_devout->pcm_channels;

      /* destroy if less pcm-channels */
      for(i = 8 * pulse_devout->sub_block_count * pcm_channels; i < 8 * pulse_devout->sub_block_count * old_pcm_channels; i++){
	g_rec_mutex_clear(pulse_devout->sub_block_mutex[i]);

	free(pulse_devout->sub_block_mutex[i]);
      }

      pulse_devout->sub_block_mutex = (GRecMutex **) realloc(pulse_devout->sub_block_mutex,
							     8 * pulse_devout->sub_block_count * pcm_channels * sizeof(GRecMutex *));

      /* create if more pcm-channels */
      for(i = 8 * pulse_devout->sub_block_count * old_pcm_channels; i < 8 * pulse_devout->sub_block_count * pcm_channels; i++){
	pulse_devout->sub_block_mutex[i] = (GRecMutex *) malloc(sizeof(GRecMutex));

	g_rec_mutex_init(pulse_devout->sub_block_mutex[i]);
      }

      pulse_devout->pcm_channels = pcm_channels;

      g_rec_mutex_unlock(pulse_devout_mutex);

      ags_pulse_devout_realloc_buffer(pulse_devout);
    }
    break;
  case PROP_FORMAT:
    {
      AgsSoundcardFormat format;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(pulse_devout_mutex);

      if(format == pulse_devout->format){
	g_rec_mutex_unlock(pulse_devout_mutex);

	return;
      }

      pulse_devout->format = format;

      g_rec_mutex_unlock(pulse_devout_mutex);

      ags_pulse_devout_realloc_buffer(pulse_devout);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      g_rec_mutex_lock(pulse_devout_mutex);

      if(buffer_size == pulse_devout->buffer_size){
	g_rec_mutex_unlock(pulse_devout_mutex);

	return;
      }

      pulse_devout->buffer_size = buffer_size;

      ags_frame_clock_set_buffer_size(pulse_devout->frame_clock,
				      buffer_size);

      g_rec_mutex_unlock(pulse_devout_mutex);

      ags_pulse_devout_realloc_buffer(pulse_devout);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      g_rec_mutex_lock(pulse_devout_mutex);
      
      if(samplerate == pulse_devout->samplerate){
	g_rec_mutex_unlock(pulse_devout_mutex);

	return;
      }

      pulse_devout->samplerate = samplerate;

      ags_frame_clock_set_samplerate(pulse_devout->frame_clock,
				     samplerate);

      g_rec_mutex_unlock(pulse_devout_mutex);

      ags_pulse_devout_realloc_buffer(pulse_devout);
    }
    break;
  case PROP_BPM:
    {
      gdouble bpm;
      
      bpm = g_value_get_double(value);

      g_rec_mutex_lock(pulse_devout_mutex);

      if(bpm == pulse_devout->bpm){
	g_rec_mutex_unlock(pulse_devout_mutex);

	return;
      }

      pulse_devout->bpm = bpm;

      ags_frame_clock_set_bpm(pulse_devout->frame_clock,
			      bpm);

      g_rec_mutex_unlock(pulse_devout_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      //TODO:JK: implement me
    }
    break;
  case PROP_PULSE_CLIENT:
    {
      AgsPulseClient *pulse_client;

      pulse_client = (AgsPulseClient *) g_value_get_object(value);

      g_rec_mutex_lock(pulse_devout_mutex);

      if(pulse_devout->pulse_client == (GObject *) pulse_client){
	g_rec_mutex_unlock(pulse_devout_mutex);

	return;
      }

      if(pulse_devout->pulse_client != NULL){
	g_object_unref(G_OBJECT(pulse_devout->pulse_client));
      }

      if(pulse_client != NULL){
	g_object_ref(pulse_client);
      }
      
      pulse_devout->pulse_client = (GObject *) pulse_client;

      g_rec_mutex_unlock(pulse_devout_mutex);
    }
    break;
  case PROP_PULSE_PORT:
    {
      AgsPulsePort *pulse_port;

      pulse_port = (AgsPulsePort *) g_value_get_pointer(value);

      g_rec_mutex_lock(pulse_devout_mutex);

      if(!AGS_IS_PULSE_PORT(pulse_port) ||
	 g_list_find(pulse_devout->pulse_port, pulse_port) != NULL){
	g_rec_mutex_unlock(pulse_devout_mutex);

	return;
      }

      g_object_ref(pulse_port);
      pulse_devout->pulse_port = g_list_append(pulse_devout->pulse_port,
					       pulse_port);

      g_rec_mutex_unlock(pulse_devout_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_devout_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec)
{
  AgsPulseDevout *pulse_devout;

  GRecMutex *pulse_devout_mutex;

  pulse_devout = AGS_PULSE_DEVOUT(gobject);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(pulse_devout_mutex);

      g_value_set_string(value, pulse_devout->card_uri);

      g_rec_mutex_unlock(pulse_devout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_rec_mutex_lock(pulse_devout_mutex);

      g_value_set_uint(value, pulse_devout->dsp_channels);

      g_rec_mutex_unlock(pulse_devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_rec_mutex_lock(pulse_devout_mutex);

      g_value_set_uint(value, pulse_devout->pcm_channels);

      g_rec_mutex_unlock(pulse_devout_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(pulse_devout_mutex);

      g_value_set_uint(value, pulse_devout->format);

      g_rec_mutex_unlock(pulse_devout_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(pulse_devout_mutex);

      g_value_set_uint(value, pulse_devout->buffer_size);

      g_rec_mutex_unlock(pulse_devout_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(pulse_devout_mutex);

      g_value_set_uint(value, pulse_devout->samplerate);

      g_rec_mutex_unlock(pulse_devout_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(pulse_devout_mutex);

      g_value_set_double(value, pulse_devout->bpm);

      g_rec_mutex_unlock(pulse_devout_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(pulse_devout_mutex);

      g_value_set_pointer(value, pulse_devout->app_buffer);

      g_rec_mutex_unlock(pulse_devout_mutex);
    }
    break;
  case PROP_PULSE_CLIENT:
    {
      g_rec_mutex_lock(pulse_devout_mutex);

      g_value_set_object(value, pulse_devout->pulse_client);

      g_rec_mutex_unlock(pulse_devout_mutex);
    }
    break;
  case PROP_PULSE_PORT:
    {
      g_rec_mutex_lock(pulse_devout_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(pulse_devout->pulse_port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(pulse_devout_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_devout_dispose(GObject *gobject)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(gobject);

  /* pulse client */
  if(pulse_devout->pulse_client != NULL){
    g_object_unref(pulse_devout->pulse_client);

    pulse_devout->pulse_client = NULL;
  }

  /* pulse port */
  g_list_free_full(pulse_devout->pulse_port,
		   g_object_unref);

  pulse_devout->pulse_port = NULL;

  /* call parent */
  G_OBJECT_CLASS(ags_pulse_devout_parent_class)->dispose(gobject);
}

void
ags_pulse_devout_finalize(GObject *gobject)
{
  AgsPulseDevout *pulse_devout;

  guint i;

  pulse_devout = AGS_PULSE_DEVOUT(gobject);

  ags_uuid_free(pulse_devout->uuid);

  /* frame clock */
  if(pulse_devout->frame_clock != NULL){
    g_object_unref(pulse_devout->frame_clock);
  }

  /* app buffer mutex */
  for(i = 0; i < 8; i++){
    g_rec_mutex_clear(pulse_devout->app_buffer_mutex[i]);

    g_free(pulse_devout->app_buffer_mutex[i]);
  }
  
  g_free(pulse_devout->app_buffer_mutex);

  /* sub-block mutex */
  for(i = 0; i < 8 * pulse_devout->sub_block_count * pulse_devout->pcm_channels; i++){
    g_rec_mutex_clear(pulse_devout->sub_block_mutex[i]);

    g_free(pulse_devout->sub_block_mutex[i]);
  }

  g_free(pulse_devout->sub_block_mutex);

  /* free output buffer */
  g_free(pulse_devout->app_buffer[0]);
  g_free(pulse_devout->app_buffer[1]);
  g_free(pulse_devout->app_buffer[2]);
  g_free(pulse_devout->app_buffer[3]);
  g_free(pulse_devout->app_buffer[4]);
  g_free(pulse_devout->app_buffer[5]);
  g_free(pulse_devout->app_buffer[6]);
  g_free(pulse_devout->app_buffer[7]);

  /* free buffer array */
  g_free(pulse_devout->app_buffer);

  /* pulse client */
  if(pulse_devout->pulse_client != NULL){
    g_object_unref(pulse_devout->pulse_client);
  }

  /* pulse port */
  g_list_free_full(pulse_devout->pulse_port,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_pulse_devout_parent_class)->finalize(gobject);
}

AgsUUID*
ags_pulse_devout_get_uuid(AgsConnectable *connectable)
{
  AgsPulseDevout *pulse_devout;
  
  AgsUUID *ptr;

  GRecMutex *pulse_devout_mutex;

  pulse_devout = AGS_PULSE_DEVOUT(connectable);

  /* get pulse devout signal mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* get UUID */
  g_rec_mutex_lock(pulse_devout_mutex);

  ptr = pulse_devout->uuid;

  g_rec_mutex_unlock(pulse_devout_mutex);
  
  return(ptr);
}

gboolean
ags_pulse_devout_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_pulse_devout_is_ready(AgsConnectable *connectable)
{
  AgsPulseDevout *pulse_devout;
  
  gboolean is_ready;

  GRecMutex *pulse_devout_mutex;

  pulse_devout = AGS_PULSE_DEVOUT(connectable);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* check is ready */
  g_rec_mutex_lock(pulse_devout_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (pulse_devout->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(pulse_devout_mutex);
  
  return(is_ready);
}

void
ags_pulse_devout_add_to_registry(AgsConnectable *connectable)
{
  AgsPulseDevout *pulse_devout;

  GRecMutex *pulse_devout_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  pulse_devout = AGS_PULSE_DEVOUT(connectable);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  g_rec_mutex_lock(pulse_devout_mutex);

  pulse_devout->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(pulse_devout_mutex);
}

void
ags_pulse_devout_remove_from_registry(AgsConnectable *connectable)
{
  AgsPulseDevout *pulse_devout;

  GRecMutex *pulse_devout_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  pulse_devout = AGS_PULSE_DEVOUT(connectable);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  g_rec_mutex_lock(pulse_devout_mutex);

  pulse_devout->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(pulse_devout_mutex);
}

xmlNode*
ags_pulse_devout_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_pulse_devout_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_pulse_devout_xml_parse(AgsConnectable *connectable,
			   xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_pulse_devout_is_connected(AgsConnectable *connectable)
{
  AgsPulseDevout *pulse_devout;
  
  gboolean is_connected;

  GRecMutex *pulse_devout_mutex;

  pulse_devout = AGS_PULSE_DEVOUT(connectable);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* check is connected */
  g_rec_mutex_lock(pulse_devout_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (pulse_devout->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(pulse_devout_mutex);
  
  return(is_connected);
}

void
ags_pulse_devout_connect(AgsConnectable *connectable)
{
  AgsPulseDevout *pulse_devout;

  GRecMutex *pulse_devout_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  pulse_devout = AGS_PULSE_DEVOUT(connectable);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  g_rec_mutex_lock(pulse_devout_mutex);

  pulse_devout->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(pulse_devout_mutex);
}

void
ags_pulse_devout_disconnect(AgsConnectable *connectable)
{
  AgsPulseDevout *pulse_devout;

  GRecMutex *pulse_devout_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  pulse_devout = AGS_PULSE_DEVOUT(connectable);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  g_rec_mutex_lock(pulse_devout_mutex);

  pulse_devout->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(pulse_devout_mutex);
}

/**
 * ags_pulse_devout_test_flags:
 * @pulse_devout: the #AgsPulseDevout
 * @flags: the flags
 *
 * Test @flags to be set on @pulse_devout.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_pulse_devout_test_flags(AgsPulseDevout *pulse_devout, AgsPulseDevoutFlags flags)
{
  gboolean retval;  
  
  GRecMutex *pulse_devout_mutex;

  if(!AGS_IS_PULSE_DEVOUT(pulse_devout)){
    return(FALSE);
  }

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* test */
  g_rec_mutex_lock(pulse_devout_mutex);

  retval = (flags & (pulse_devout->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(pulse_devout_mutex);

  return(retval);
}

/**
 * ags_pulse_devout_set_flags:
 * @pulse_devout: the #AgsPulseDevout
 * @flags: see #AgsPulseDevoutFlags-enum
 *
 * Enable a feature of @pulse_devout.
 *
 * Since: 3.0.0
 */
void
ags_pulse_devout_set_flags(AgsPulseDevout *pulse_devout, AgsPulseDevoutFlags flags)
{
  GRecMutex *pulse_devout_mutex;

  if(!AGS_IS_PULSE_DEVOUT(pulse_devout)){
    return;
  }

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(pulse_devout_mutex);

  pulse_devout->flags |= flags;
  
  g_rec_mutex_unlock(pulse_devout_mutex);
}
    
/**
 * ags_pulse_devout_unset_flags:
 * @pulse_devout: the #AgsPulseDevout
 * @flags: see #AgsPulseDevoutFlags-enum
 *
 * Disable a feature of @pulse_devout.
 *
 * Since: 3.0.0
 */
void
ags_pulse_devout_unset_flags(AgsPulseDevout *pulse_devout, AgsPulseDevoutFlags flags)
{  
  GRecMutex *pulse_devout_mutex;

  if(!AGS_IS_PULSE_DEVOUT(pulse_devout)){
    return;
  }

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(pulse_devout_mutex);

  pulse_devout->flags &= (~flags);
  
  g_rec_mutex_unlock(pulse_devout_mutex);
}

void
ags_pulse_devout_set_device(AgsSoundcard *soundcard,
			    gchar *device)
{
  AgsPulseDevout *pulse_devout;

  GList *pulse_port, *pulse_port_start;

  gchar *str;

  guint pcm_channels;
  int ret;
  guint nth_card;
  guint i;
  
  GRecMutex *pulse_devout_mutex;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* check device */
  g_rec_mutex_lock(pulse_devout_mutex);

  if(pulse_devout->card_uri == device ||
     (pulse_devout->card_uri != NULL && 
      !g_ascii_strcasecmp(pulse_devout->card_uri,
			  device))){
    g_rec_mutex_unlock(pulse_devout_mutex);
  
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-pulse-devout-")){
    g_rec_mutex_unlock(pulse_devout_mutex);

    g_warning("invalid pulseaudio device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-pulse-devout-%u",
	       &nth_card);

  if(ret != 1){
    g_rec_mutex_unlock(pulse_devout_mutex);

    g_warning("invalid pulseaudio device specifier");

    return;
  }

  g_free(pulse_devout->card_uri);
  pulse_devout->card_uri = g_strdup(device);

  /* apply name to port */
  pcm_channels = pulse_devout->pcm_channels;

  g_rec_mutex_unlock(pulse_devout_mutex);

  if(pcm_channels > 0){
    g_rec_mutex_lock(pulse_devout_mutex);
    
    pulse_port_start = 
      pulse_port = g_list_copy(pulse_devout->pulse_port);

    pulse_devout->port_name = (gchar **) malloc((pcm_channels + 1) * sizeof(gchar *));

    g_rec_mutex_unlock(pulse_devout_mutex);
  
    for(i = 0; i < pcm_channels; i++){
      str = g_strdup_printf("ags-soundcard%d-%04d",
			    nth_card,
			    i);

      g_rec_mutex_lock(pulse_devout_mutex);
    
      pulse_devout->port_name[i] = g_strdup(str);

      g_rec_mutex_unlock(pulse_devout_mutex);
    
      g_object_set(pulse_port->data,
		   "port-name", str,
		   NULL);
      g_free(str);

      pulse_port = pulse_port->next;
    }

    g_rec_mutex_lock(pulse_devout_mutex);

    pulse_devout->port_name[i] = NULL;

    g_rec_mutex_unlock(pulse_devout_mutex);
  
    g_list_free(pulse_port_start);
  }
}

gchar*
ags_pulse_devout_get_device(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;
  
  gchar *device;

  GRecMutex *pulse_devout_mutex;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  
  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  device = NULL;

  /* get device */
  g_rec_mutex_lock(pulse_devout_mutex);

  device = g_strdup(pulse_devout->card_uri);

  g_rec_mutex_unlock(pulse_devout_mutex);
  
  return(device);
}

void
ags_pulse_devout_set_presets(AgsSoundcard *soundcard,
			     guint channels,
			     guint rate,
			     guint buffer_size,
			     AgsSoundcardFormat format)
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  g_object_set(pulse_devout,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_pulse_devout_get_presets(AgsSoundcard *soundcard,
			     guint *channels,
			     guint *rate,
			     guint *buffer_size,
			     AgsSoundcardFormat *format)
{
  AgsPulseDevout *pulse_devout;

  GRecMutex *pulse_devout_mutex;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  
  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* get presets */
  g_rec_mutex_lock(pulse_devout_mutex);

  if(channels != NULL){
    *channels = pulse_devout->pcm_channels;
  }

  if(rate != NULL){
    *rate = pulse_devout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = pulse_devout->buffer_size;
  }

  if(format != NULL){
    *format = pulse_devout->format;
  }

  g_rec_mutex_unlock(pulse_devout_mutex);
}

void
ags_pulse_devout_list_cards(AgsSoundcard *soundcard,
			    GList **card_id, GList **card_name)
{
  AgsPulseClient *pulse_client;
  AgsPulseDevout *pulse_devout;

  AgsApplicationContext *application_context;
  
  GList *list_start, *list;

  gchar *card_uri;
  gchar *client_name;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

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
    if(AGS_IS_PULSE_DEVOUT(list->data)){
      if(card_id != NULL){
	card_uri = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));
	
	if(AGS_PULSE_DEVOUT(list->data)->card_uri != NULL){
	  *card_id = g_list_prepend(*card_id,
				    card_uri);
	}else{
	  *card_id = g_list_prepend(*card_id,
				    g_strdup("(null)"));

	  g_warning("ags_pulse_devout_list_cards() - card id (null)");
	}
      }

      if(card_name != NULL){
	g_object_get(list->data,
		     "pulse-client", &pulse_client,
		     NULL);
	
	if(pulse_client != NULL){
	  /* get client name */
	  g_object_get(pulse_client,
		       "client-name", &client_name,
		       NULL);
	  
	  *card_name = g_list_prepend(*card_name,
				      client_name);

	  g_object_unref(pulse_client);
	}else{
	  *card_name = g_list_prepend(*card_name,
				      g_strdup("(null)"));

	  g_warning("ags_pulse_devout_list_cards() - pulseaudio client not connected (null)");
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
ags_pulse_devout_pcm_info(AgsSoundcard *soundcard,
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
ags_pulse_devout_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_PLAYBACK);
}

gboolean
ags_pulse_devout_is_starting(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  gboolean is_starting;
  
  GRecMutex *pulse_devout_mutex;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* check is starting */
  g_rec_mutex_lock(pulse_devout_mutex);

  is_starting = ((AGS_PULSE_DEVOUT_START_PLAY & (pulse_devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(pulse_devout_mutex);
  
  return(is_starting);
}

gboolean
ags_pulse_devout_is_playing(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  gboolean is_playing;
  
  GRecMutex *pulse_devout_mutex;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  
  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* check is starting */
  g_rec_mutex_lock(pulse_devout_mutex);

  is_playing = ((AGS_PULSE_DEVOUT_PLAY & (pulse_devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(pulse_devout_mutex);

  return(is_playing);
}

gchar*
ags_pulse_devout_get_uptime(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  gchar *uptime;

  GRecMutex *pulse_devout_mutex;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  
  /* get core audio devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* get time string */
  g_rec_mutex_lock(pulse_devout_mutex);
  
  uptime = ags_frame_clock_to_time_string(pulse_devout->frame_clock);

  g_rec_mutex_unlock(pulse_devout_mutex);
  
  return(uptime);
}
  
void
ags_pulse_devout_port_init(AgsSoundcard *soundcard,
			   GError **error)
{
  AgsPulsePort *pulse_port;
  AgsPulseDevout *pulse_devout;
  
  AgsSoundcardFormat format;
  guint word_size;
  gboolean use_cache;
  
  GRecMutex *pulse_port_mutex;
  GRecMutex *pulse_devout_mutex;

  if(ags_soundcard_is_playing(soundcard)){
    return;
  }
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* port */
  g_rec_mutex_lock(pulse_devout_mutex);

  pulse_port = (AgsPulsePort *) pulse_devout->pulse_port->data;

  g_rec_mutex_unlock(pulse_devout_mutex);

  /* get port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  /* check use cache */
  g_rec_mutex_lock(pulse_port_mutex);

  use_cache = pulse_port->use_cache;

  if(use_cache){
    pulse_port->completed_cache = 0;
    pulse_port->current_cache = 1;
  }
  
  g_rec_mutex_unlock(pulse_port_mutex);

  /* retrieve word size */
  g_rec_mutex_lock(pulse_devout_mutex);

  pulse_port = (AgsPulsePort *) pulse_devout->pulse_port->data;

  switch(pulse_devout->format){
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
    g_rec_mutex_unlock(pulse_devout_mutex);
    
    g_warning("ags_pulse_devout_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for playback */
  pulse_devout->app_buffer_mode = AGS_PULSE_DEVOUT_APP_BUFFER_7;
  pulse_devout->flags |= (AGS_PULSE_DEVOUT_START_PLAY |
			  AGS_PULSE_DEVOUT_PLAY |
			  AGS_PULSE_DEVOUT_NONBLOCKING);

  memset(pulse_devout->app_buffer[0], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->app_buffer[1], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->app_buffer[2], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->app_buffer[3], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->app_buffer[4], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->app_buffer[5], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->app_buffer[6], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->app_buffer[7], 0, pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);

  pulse_devout->flags |= (AGS_PULSE_DEVOUT_INITIALIZED |
			  AGS_PULSE_DEVOUT_START_PLAY |
			  AGS_PULSE_DEVOUT_PLAY);

  ags_frame_clock_set_start_note_offset(pulse_devout->frame_clock,
					pulse_devout->start_note_offset);
  
  ags_frame_clock_start(pulse_devout->frame_clock);

  ags_atomic_int_and(&(pulse_devout->sync_flags),
		   (~(AGS_PULSE_DEVOUT_PASS_THROUGH)));
  ags_atomic_int_or(&(pulse_devout->sync_flags),
		  AGS_PULSE_DEVOUT_INITIAL_CALLBACK);

  g_rec_mutex_unlock(pulse_devout_mutex);
}

void
ags_pulse_devout_port_play(AgsSoundcard *soundcard,
			   GError **error)
{
  AgsPulseClient *pulse_client;
  AgsPulseDevout *pulse_devout;
  AgsPulsePort *pulse_port;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;
      
  GList *task;

  gboolean initial_run;
  guint word_size;
  gboolean use_cache;
  gboolean pulse_client_activated;

  GRecMutex *pulse_devout_mutex;
  GRecMutex *pulse_client_mutex;
  GRecMutex *pulse_port_mutex;
  GRecMutex *cache_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  
  application_context = ags_application_context_get_instance();
  
  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* client */
  g_rec_mutex_lock(pulse_devout_mutex);

  pulse_client = (AgsPulseClient *) pulse_devout->pulse_client;
  pulse_port = (AgsPulsePort *) pulse_devout->pulse_port->data;
  
  callback_mutex = &(pulse_devout->callback_mutex);
  callback_finish_mutex = &(pulse_devout->callback_finish_mutex);

  /* do playback */
  initial_run = ((AGS_PULSE_DEVOUT_START_PLAY & (pulse_devout->flags)) != 0) ? TRUE: FALSE;
  
  pulse_devout->flags &= (~AGS_PULSE_DEVOUT_START_PLAY);
  
  if((AGS_PULSE_DEVOUT_INITIALIZED & (pulse_devout->flags)) == 0){
    g_rec_mutex_unlock(pulse_devout_mutex);
    
    return;
  }

  switch(pulse_devout->format){
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
    g_rec_mutex_unlock(pulse_devout_mutex);
    
    g_warning("ags_pulse_devout_port_play(): unsupported word size");
    
    return;
  }

  g_rec_mutex_unlock(pulse_devout_mutex);

  /* get port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  /* check use cache */
  g_rec_mutex_lock(pulse_port_mutex);

  use_cache = pulse_port->use_cache;
  
  g_rec_mutex_unlock(pulse_port_mutex);

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

    idle_time.tv_nsec = ags_pulse_port_get_latency(pulse_port) / 8;

    g_rec_mutex_lock(pulse_port_mutex);

    completed_cache = pulse_port->completed_cache;

    cache_buffer_size = pulse_port->cache_buffer_size;
    
    cache_offset = pulse_port->cache_offset;
    
    g_rec_mutex_unlock(pulse_port_mutex);

    if(completed_cache == AGS_PULSE_PORT_DEFAULT_CACHE_COUNT - 1){
      write_cache = 0;
    }else{
      write_cache = completed_cache + 1;
    }

    /* wait until ready */
    g_rec_mutex_lock(pulse_port_mutex);

    current_cache = pulse_port->current_cache;
  
    g_rec_mutex_unlock(pulse_port_mutex);
    
    while(write_cache == current_cache){
      ags_time_nanosleep(&idle_time);

      g_rec_mutex_lock(pulse_port_mutex);

      current_cache = pulse_port->current_cache;

      g_rec_mutex_unlock(pulse_port_mutex);
    }

    /* fill cache */
    g_rec_mutex_lock(pulse_devout_mutex);

    pcm_channels = pulse_devout->pcm_channels;
    buffer_size = pulse_devout->buffer_size;
    format = pulse_devout->format;
    
    g_rec_mutex_unlock(pulse_devout_mutex);

    buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(pulse_devout));

    cache_mutex = pulse_port->cache_mutex[write_cache];
    
    g_rec_mutex_lock(cache_mutex);

    ags_soundcard_lock_buffer(AGS_SOUNDCARD(pulse_devout),
			      buffer);
    
    switch(format){
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	if(cache_offset == 0){
	  ags_audio_buffer_util_clear_buffer(pulse_port->audio_buffer_util,
					     pulse_port->cache[write_cache], 1,
					     pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S16);
	}

	if(buffer != NULL){
	  ags_audio_buffer_util_copy_s16_to_s16(pulse_port->audio_buffer_util,
						(gint16 *) pulse_port->cache[write_cache] + (pcm_channels * cache_offset), 1,
						(gint16 *) buffer, 1,
						pcm_channels * buffer_size);
	}
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	if(cache_offset == 0){
	  ags_audio_buffer_util_clear_buffer(pulse_port->audio_buffer_util,
					     pulse_port->cache[write_cache], 1,
					     pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
	}

	if(buffer != NULL){
	  ags_audio_buffer_util_copy_s24_to_s24(pulse_port->audio_buffer_util,
						(gint32 *) pulse_port->cache[write_cache] + (pcm_channels * cache_offset), 1,
						(gint32 *) buffer, 1,
						pcm_channels * buffer_size);
	}
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	if(cache_offset == 0){
	  ags_audio_buffer_util_clear_buffer(pulse_port->audio_buffer_util,
					     pulse_port->cache[write_cache], 1,
					     pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
	}

	if(buffer != NULL){
	  ags_audio_buffer_util_copy_s32_to_s32(pulse_port->audio_buffer_util,
						(gint32 *) pulse_port->cache[write_cache] + (pcm_channels * cache_offset), 1,
						(gint32 *) buffer, 1,
						pcm_channels * buffer_size);
	}
      }
      break;
    }

    ags_soundcard_unlock_buffer(AGS_SOUNDCARD(pulse_devout),
				buffer);
    
    g_rec_mutex_unlock(cache_mutex);
    
    /* seek cache */
    if(cache_offset + buffer_size >= cache_buffer_size){
      g_rec_mutex_lock(pulse_port_mutex);
      
      pulse_port->completed_cache = write_cache;
      pulse_port->cache_offset = 0;
      
      g_rec_mutex_unlock(pulse_port_mutex);
    }else{
      g_rec_mutex_lock(pulse_port_mutex);
      
      pulse_port->cache_offset += buffer_size;
      
      g_rec_mutex_unlock(pulse_port_mutex);
    }
  }else{
    /* get client mutex */
    pulse_client_mutex = AGS_PULSE_CLIENT_GET_OBJ_MUTEX(pulse_client);

    /* get activated */
    g_rec_mutex_lock(pulse_client_mutex);

    pulse_client_activated = ((AGS_PULSE_CLIENT_ACTIVATED & (pulse_client->flags)) != 0) ? TRUE: FALSE;

    g_rec_mutex_unlock(pulse_client_mutex);

    if(pulse_client_activated){
      /* signal */
      if((AGS_PULSE_DEVOUT_INITIAL_CALLBACK & (ags_atomic_int_get(&(pulse_devout->sync_flags)))) == 0){
	g_mutex_lock(callback_mutex);

	ags_atomic_int_or(&(pulse_devout->sync_flags),
			AGS_PULSE_DEVOUT_CALLBACK_DONE);
    
	if((AGS_PULSE_DEVOUT_CALLBACK_WAIT & (ags_atomic_int_get(&(pulse_devout->sync_flags)))) != 0){
	  g_cond_signal(&(pulse_devout->callback_cond));
	}

	g_mutex_unlock(callback_mutex);
      }
    
      /* wait callback */	
      if((AGS_PULSE_DEVOUT_INITIAL_CALLBACK & (ags_atomic_int_get(&(pulse_devout->sync_flags)))) == 0){
	g_mutex_lock(callback_finish_mutex);
    
	if((AGS_PULSE_DEVOUT_CALLBACK_FINISH_DONE & (ags_atomic_int_get(&(pulse_devout->sync_flags)))) == 0){
	  ags_atomic_int_or(&(pulse_devout->sync_flags),
			  AGS_PULSE_DEVOUT_CALLBACK_FINISH_WAIT);
    
	  while((AGS_PULSE_DEVOUT_CALLBACK_FINISH_DONE & (ags_atomic_int_get(&(pulse_devout->sync_flags)))) == 0 &&
		(AGS_PULSE_DEVOUT_CALLBACK_FINISH_WAIT & (ags_atomic_int_get(&(pulse_devout->sync_flags)))) != 0){
	    g_cond_wait(&(pulse_devout->callback_finish_cond),
			callback_finish_mutex);
	  }
	}
    
	ags_atomic_int_and(&(pulse_devout->sync_flags),
			 (~(AGS_PULSE_DEVOUT_CALLBACK_FINISH_WAIT |
			    AGS_PULSE_DEVOUT_CALLBACK_FINISH_DONE)));
    
	g_mutex_unlock(callback_finish_mutex);
      }else{
	ags_atomic_int_and(&(pulse_devout->sync_flags),
			 (~AGS_PULSE_DEVOUT_INITIAL_CALLBACK));
      }
    }
  }
  
  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;
  
  /* tic soundcard */
  if(!initial_run){
    tic_device = ags_tic_device_new((GObject *) pulse_devout);
    task = g_list_append(task,
			 tic_device);
  }
  
  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) pulse_devout);
  task = g_list_append(task,
		       clear_buffer);
    
  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) pulse_devout);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_pulse_devout_port_free(AgsSoundcard *soundcard)
{
  AgsPulsePort *pulse_port;
  AgsPulseDevout *pulse_devout;

  guint pcm_channels;
  guint cache_buffer_size;
  guint word_size;
  gboolean use_cache;
  
  guint i;
  
  GRecMutex *pulse_port_mutex;
  GRecMutex *pulse_devout_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  
  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* port */
  g_rec_mutex_lock(pulse_devout_mutex);

  pulse_port = (AgsPulsePort *) pulse_devout->pulse_port->data;
  pcm_channels = pulse_devout->pcm_channels;
  
  g_rec_mutex_unlock(pulse_devout_mutex);

  /* get port mutex */
  pulse_port_mutex = AGS_PULSE_PORT_GET_OBJ_MUTEX(pulse_port);

  /* check use cache */
  g_rec_mutex_lock(pulse_port_mutex);

  cache_buffer_size = pulse_port->cache_buffer_size;
  use_cache = pulse_port->use_cache;

  if(use_cache){
    pulse_port->completed_cache = 0;
    pulse_port->current_cache = 0;
    pulse_port->cache_offset = 0;
  }
  
  switch(pulse_port->format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      ags_audio_buffer_util_clear_buffer(pulse_port->audio_buffer_util,
					 pulse_port->cache[0], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S16);
      ags_audio_buffer_util_clear_buffer(pulse_port->audio_buffer_util,
					 pulse_port->cache[1], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S16);
      ags_audio_buffer_util_clear_buffer(pulse_port->audio_buffer_util,
					 pulse_port->cache[2], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S16);
      ags_audio_buffer_util_clear_buffer(pulse_port->audio_buffer_util,
					 pulse_port->cache[3], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S16);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      ags_audio_buffer_util_clear_buffer(pulse_port->audio_buffer_util,
					 pulse_port->cache[0], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(pulse_port->audio_buffer_util,
					 pulse_port->cache[1], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(pulse_port->audio_buffer_util,
					 pulse_port->cache[2], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(pulse_port->audio_buffer_util,
					 pulse_port->cache[3], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      ags_audio_buffer_util_clear_buffer(pulse_port->audio_buffer_util,
					 pulse_port->cache[0], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(pulse_port->audio_buffer_util,
					 pulse_port->cache[1], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(pulse_port->audio_buffer_util,
					 pulse_port->cache[2], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(pulse_port->audio_buffer_util,
					 pulse_port->cache[3], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
    }
    break;
  }

  g_rec_mutex_unlock(pulse_port_mutex);

  /*  */
  g_rec_mutex_lock(pulse_devout_mutex);

  if((AGS_PULSE_DEVOUT_INITIALIZED & (pulse_devout->flags)) == 0){
    g_rec_mutex_unlock(pulse_devout_mutex);

    return;
  }

  callback_mutex = &(pulse_devout->callback_mutex);
  callback_finish_mutex = &(pulse_devout->callback_finish_mutex);
  
  //  ags_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);
  
  pulse_devout->app_buffer_mode = AGS_PULSE_DEVOUT_APP_BUFFER_0;
  pulse_devout->flags &= (~(AGS_PULSE_DEVOUT_PLAY));

  ags_atomic_int_or(&(pulse_devout->sync_flags),
		  AGS_PULSE_DEVOUT_PASS_THROUGH);
  ags_atomic_int_and(&(pulse_devout->sync_flags),
		   (~AGS_PULSE_DEVOUT_INITIAL_CALLBACK));

  /* signal callback */
  g_mutex_lock(callback_mutex);

  ags_atomic_int_or(&(pulse_devout->sync_flags),
		  AGS_PULSE_DEVOUT_CALLBACK_DONE);
    
  if((AGS_PULSE_DEVOUT_CALLBACK_WAIT & (ags_atomic_int_get(&(pulse_devout->sync_flags)))) != 0){
    g_cond_signal(&(pulse_devout->callback_cond));
  }

  g_mutex_unlock(callback_mutex);

  /* signal thread */
  g_mutex_lock(callback_finish_mutex);

  ags_atomic_int_or(&(pulse_devout->sync_flags),
		  AGS_PULSE_DEVOUT_CALLBACK_FINISH_DONE);
    
  if((AGS_PULSE_DEVOUT_CALLBACK_FINISH_WAIT & (ags_atomic_int_get(&(pulse_devout->sync_flags)))) != 0){
    g_cond_signal(&(pulse_devout->callback_finish_cond));
  }

  g_mutex_unlock(callback_finish_mutex);

  /* stop */
  ags_frame_clock_stop(pulse_devout->frame_clock);

  switch(pulse_devout->format){
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
    
    g_critical("ags_pulse_devout_free(): unsupported word size");
  }

  g_rec_mutex_unlock(pulse_devout_mutex);

  if(pulse_devout->pulse_port != NULL){
    pulse_port = pulse_devout->pulse_port->data;

    while(!ags_atomic_int_get(&(pulse_port->is_empty))) usleep(500000);
  }

  g_rec_mutex_lock(pulse_devout_mutex);
  
  memset(pulse_devout->app_buffer[0], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->app_buffer[1], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->app_buffer[2], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->app_buffer[3], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->app_buffer[4], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->app_buffer[5], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->app_buffer[6], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);
  memset(pulse_devout->app_buffer[7], 0, (size_t) pulse_devout->pcm_channels * pulse_devout->buffer_size * word_size);

  g_rec_mutex_unlock(pulse_devout_mutex);
}

void
ags_pulse_devout_tic(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  GRecMutex *pulse_devout_mutex;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  
  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* frame clock */
  g_rec_mutex_lock(pulse_devout_mutex);
  
  ags_frame_clock_increment_counter(pulse_devout->frame_clock);

  /* 16th pulse */
  if(ags_frame_clock_get_has_16th_pulse(pulse_devout->frame_clock)){
    ags_soundcard_offset_changed(soundcard,
				 pulse_devout->frame_clock->note_offset);
  }
  
  g_rec_mutex_unlock(pulse_devout_mutex);
}

void
ags_pulse_devout_offset_changed(AgsSoundcard *soundcard,
				guint64 note_offset)
{

  //empty
}

void
ags_pulse_devout_set_bpm(AgsSoundcard *soundcard,
			 gdouble bpm)
{
  AgsPulseDevout *pulse_devout;

  GRecMutex *pulse_devout_mutex;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* set bpm */
  g_rec_mutex_lock(pulse_devout_mutex);

  pulse_devout->bpm = bpm;

  ags_frame_clock_set_bpm(pulse_devout->frame_clock,
			  bpm);

  g_rec_mutex_unlock(pulse_devout_mutex);
}

gdouble
ags_pulse_devout_get_bpm(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  gdouble bpm;
  
  GRecMutex *pulse_devout_mutex;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* get bpm */
  g_rec_mutex_lock(pulse_devout_mutex);

  bpm = pulse_devout->bpm;
  
  g_rec_mutex_unlock(pulse_devout_mutex);

  return(bpm);
}

void
ags_pulse_devout_set_start_note_offset(AgsSoundcard *soundcard,
				       guint64 start_note_offset)
{
  AgsPulseDevout *pulse_devout;

  GRecMutex *pulse_devout_mutex;  

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* set note offset */
  g_rec_mutex_lock(pulse_devout_mutex);

  pulse_devout->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(pulse_devout_mutex);
}

guint64
ags_pulse_devout_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  guint64 start_note_offset;
  
  GRecMutex *pulse_devout_mutex;  

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* set note offset */
  g_rec_mutex_lock(pulse_devout_mutex);

  start_note_offset = pulse_devout->start_note_offset;

  g_rec_mutex_unlock(pulse_devout_mutex);

  return(start_note_offset);
}

GObject*
ags_pulse_devout_get_frame_clock(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  GObject *frame_clock;
  
  GRecMutex *pulse_devout_mutex;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  /* get core audio devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* get frame clock */
  g_rec_mutex_lock(pulse_devout_mutex);

  frame_clock = (GObject *) pulse_devout->frame_clock;
  
  g_rec_mutex_unlock(pulse_devout_mutex);

  return(frame_clock);
}

void*
ags_pulse_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  void *buffer;

  GRecMutex *pulse_devout_mutex;  
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  
  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  g_rec_mutex_lock(pulse_devout_mutex);

  if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_0){
    buffer = pulse_devout->app_buffer[0];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_1){
    buffer = pulse_devout->app_buffer[1];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_2){
    buffer = pulse_devout->app_buffer[2];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_3){
    buffer = pulse_devout->app_buffer[3];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_4){
    buffer = pulse_devout->app_buffer[4];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_5){
    buffer = pulse_devout->app_buffer[5];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_6){
    buffer = pulse_devout->app_buffer[6];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_7){
    buffer = pulse_devout->app_buffer[7];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(pulse_devout_mutex);

  return(buffer);
}

void*
ags_pulse_devout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  void *buffer;

  GRecMutex *pulse_devout_mutex;  
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);
  
  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  g_rec_mutex_lock(pulse_devout_mutex);

  if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_0){
    buffer = pulse_devout->app_buffer[1];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_1){
    buffer = pulse_devout->app_buffer[2];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_2){
    buffer = pulse_devout->app_buffer[3];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_3){
    buffer = pulse_devout->app_buffer[4];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_4){
    buffer = pulse_devout->app_buffer[5];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_5){
    buffer = pulse_devout->app_buffer[6];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_6){
    buffer = pulse_devout->app_buffer[7];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_7){
    buffer = pulse_devout->app_buffer[0];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(pulse_devout_mutex);

  return(buffer);
}

void*
ags_pulse_devout_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  void *buffer;

  GRecMutex *pulse_devout_mutex;  
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  g_rec_mutex_lock(pulse_devout_mutex);

  if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_0){
    buffer = pulse_devout->app_buffer[7];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_1){
    buffer = pulse_devout->app_buffer[0];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_2){
    buffer = pulse_devout->app_buffer[1];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_3){
    buffer = pulse_devout->app_buffer[2];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_4){
    buffer = pulse_devout->app_buffer[3];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_5){
    buffer = pulse_devout->app_buffer[4];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_6){
    buffer = pulse_devout->app_buffer[5];
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_7){
    buffer = pulse_devout->app_buffer[6];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(pulse_devout_mutex);

  return(buffer);
}

void
ags_pulse_devout_lock_buffer(AgsSoundcard *soundcard,
			     void *buffer)
{
  AgsPulseDevout *pulse_devout;

  GRecMutex *buffer_mutex;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(pulse_devout->app_buffer != NULL){
    if(buffer == pulse_devout->app_buffer[0]){
      buffer_mutex = pulse_devout->app_buffer_mutex[0];
    }else if(buffer == pulse_devout->app_buffer[1]){
      buffer_mutex = pulse_devout->app_buffer_mutex[1];
    }else if(buffer == pulse_devout->app_buffer[2]){
      buffer_mutex = pulse_devout->app_buffer_mutex[2];
    }else if(buffer == pulse_devout->app_buffer[3]){
      buffer_mutex = pulse_devout->app_buffer_mutex[3];
    }else if(buffer == pulse_devout->app_buffer[4]){
      buffer_mutex = pulse_devout->app_buffer_mutex[4];
    }else if(buffer == pulse_devout->app_buffer[5]){
      buffer_mutex = pulse_devout->app_buffer_mutex[5];
    }else if(buffer == pulse_devout->app_buffer[6]){
      buffer_mutex = pulse_devout->app_buffer_mutex[6];
    }else if(buffer == pulse_devout->app_buffer[7]){
      buffer_mutex = pulse_devout->app_buffer_mutex[7];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_pulse_devout_unlock_buffer(AgsSoundcard *soundcard,
			       void *buffer)
{
  AgsPulseDevout *pulse_devout;

  GRecMutex *buffer_mutex;
  
  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(pulse_devout->app_buffer != NULL){
    if(buffer == pulse_devout->app_buffer[0]){
      buffer_mutex = pulse_devout->app_buffer_mutex[0];
    }else if(buffer == pulse_devout->app_buffer[1]){
      buffer_mutex = pulse_devout->app_buffer_mutex[1];
    }else if(buffer == pulse_devout->app_buffer[2]){
      buffer_mutex = pulse_devout->app_buffer_mutex[2];
    }else if(buffer == pulse_devout->app_buffer[3]){
      buffer_mutex = pulse_devout->app_buffer_mutex[3];
    }else if(buffer == pulse_devout->app_buffer[4]){
      buffer_mutex = pulse_devout->app_buffer_mutex[4];
    }else if(buffer == pulse_devout->app_buffer[5]){
      buffer_mutex = pulse_devout->app_buffer_mutex[5];
    }else if(buffer == pulse_devout->app_buffer[6]){
      buffer_mutex = pulse_devout->app_buffer_mutex[6];
    }else if(buffer == pulse_devout->app_buffer[7]){
      buffer_mutex = pulse_devout->app_buffer_mutex[7];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_pulse_devout_get_sub_block_count(AgsSoundcard *soundcard)
{
  AgsPulseDevout *pulse_devout;

  guint sub_block_count;
  
  GRecMutex *pulse_devout_mutex;  

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* get loop offset */
  g_rec_mutex_lock(pulse_devout_mutex);

  sub_block_count = pulse_devout->sub_block_count;
  
  g_rec_mutex_unlock(pulse_devout_mutex);

  return(sub_block_count);
}

gboolean
ags_pulse_devout_trylock_sub_block(AgsSoundcard *soundcard,
				   void *buffer, guint sub_block)
{
  AgsPulseDevout *pulse_devout;

  guint pcm_channels;
  guint sub_block_count;
  gboolean success;
  
  GRecMutex *pulse_devout_mutex;  
  GRecMutex *sub_block_mutex;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* get loop offset */
  g_rec_mutex_lock(pulse_devout_mutex);

  pcm_channels = pulse_devout->pcm_channels;
  sub_block_count = pulse_devout->sub_block_count;
  
  g_rec_mutex_unlock(pulse_devout_mutex);
  
  sub_block_mutex = NULL;

  success = FALSE;
  
  if(pulse_devout->app_buffer != NULL){
    if(buffer == pulse_devout->app_buffer[0]){
      sub_block_mutex = pulse_devout->sub_block_mutex[sub_block];
    }else if(buffer == pulse_devout->app_buffer[1]){
      sub_block_mutex = pulse_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == pulse_devout->app_buffer[2]){
      sub_block_mutex = pulse_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == pulse_devout->app_buffer[3]){
      sub_block_mutex = pulse_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == pulse_devout->app_buffer[4]){
      sub_block_mutex = pulse_devout->sub_block_mutex[4 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == pulse_devout->app_buffer[5]){
      sub_block_mutex = pulse_devout->sub_block_mutex[5 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == pulse_devout->app_buffer[6]){
      sub_block_mutex = pulse_devout->sub_block_mutex[6 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == pulse_devout->app_buffer[7]){
      sub_block_mutex = pulse_devout->sub_block_mutex[7 * pcm_channels * sub_block_count + sub_block];
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
ags_pulse_devout_unlock_sub_block(AgsSoundcard *soundcard,
				  void *buffer, guint sub_block)
{
  AgsPulseDevout *pulse_devout;

  guint pcm_channels;
  guint sub_block_count;
  
  GRecMutex *pulse_devout_mutex;  
  GRecMutex *sub_block_mutex;

  pulse_devout = AGS_PULSE_DEVOUT(soundcard);

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* get loop offset */
  g_rec_mutex_lock(pulse_devout_mutex);

  pcm_channels = pulse_devout->pcm_channels;
  sub_block_count = pulse_devout->sub_block_count;
  
  g_rec_mutex_unlock(pulse_devout_mutex);
  
  sub_block_mutex = NULL;
  
  if(pulse_devout->app_buffer != NULL){
    if(buffer == pulse_devout->app_buffer[0]){
      sub_block_mutex = pulse_devout->sub_block_mutex[sub_block];
    }else if(buffer == pulse_devout->app_buffer[1]){
      sub_block_mutex = pulse_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == pulse_devout->app_buffer[2]){
      sub_block_mutex = pulse_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == pulse_devout->app_buffer[3]){
      sub_block_mutex = pulse_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == pulse_devout->app_buffer[4]){
      sub_block_mutex = pulse_devout->sub_block_mutex[4 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == pulse_devout->app_buffer[5]){
      sub_block_mutex = pulse_devout->sub_block_mutex[5 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == pulse_devout->app_buffer[6]){
      sub_block_mutex = pulse_devout->sub_block_mutex[6 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == pulse_devout->app_buffer[7]){
      sub_block_mutex = pulse_devout->sub_block_mutex[7 * pcm_channels * sub_block_count + sub_block];
    }
  }

  if(sub_block_mutex != NULL){
    g_rec_mutex_unlock(sub_block_mutex);
  }
}

/**
 * ags_pulse_devout_switch_buffer_flag:
 * @pulse_devout: an #AgsPulseDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.0.0
 */
void
ags_pulse_devout_switch_buffer_flag(AgsPulseDevout *pulse_devout)
{
  GRecMutex *pulse_devout_mutex;
  
  if(!AGS_IS_PULSE_DEVOUT(pulse_devout)){
    return;
  }

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* switch buffer flag */
  g_rec_mutex_lock(pulse_devout_mutex);

  if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_0){
    pulse_devout->app_buffer_mode = AGS_PULSE_DEVOUT_APP_BUFFER_1;
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_1){
    pulse_devout->app_buffer_mode = AGS_PULSE_DEVOUT_APP_BUFFER_2;
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_2){
    pulse_devout->app_buffer_mode = AGS_PULSE_DEVOUT_APP_BUFFER_3;
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_3){
    pulse_devout->app_buffer_mode = AGS_PULSE_DEVOUT_APP_BUFFER_4;
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_4){
    pulse_devout->app_buffer_mode = AGS_PULSE_DEVOUT_APP_BUFFER_5;
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_5){
    pulse_devout->app_buffer_mode = AGS_PULSE_DEVOUT_APP_BUFFER_6;
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_6){
    pulse_devout->app_buffer_mode = AGS_PULSE_DEVOUT_APP_BUFFER_7;
  }else if(pulse_devout->app_buffer_mode == AGS_PULSE_DEVOUT_APP_BUFFER_7){
    pulse_devout->app_buffer_mode = AGS_PULSE_DEVOUT_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(pulse_devout_mutex);
}

/**
 * ags_pulse_devout_realloc_buffer:
 * @pulse_devout: the #AgsPulseDevout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.0.0
 */
void
ags_pulse_devout_realloc_buffer(AgsPulseDevout *pulse_devout)
{
  guint pcm_channels;
  guint buffer_size;
  AgsSoundcardFormat format;
  guint word_size;

  GRecMutex *pulse_devout_mutex;  

  if(!AGS_IS_PULSE_DEVOUT(pulse_devout)){
    return;
  }

  /* get pulse devout mutex */
  pulse_devout_mutex = AGS_PULSE_DEVOUT_GET_OBJ_MUTEX(pulse_devout);

  /* get word size */  
  g_rec_mutex_lock(pulse_devout_mutex);

  pcm_channels = pulse_devout->pcm_channels;
  buffer_size = pulse_devout->buffer_size;

  format = pulse_devout->format;
  
  g_rec_mutex_unlock(pulse_devout_mutex);

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
    g_warning("ags_pulse_devout_realloc_buffer(): unsupported word size");
    return;
  }
  
  /* AGS_PULSE_DEVOUT_APP_BUFFER_0 */
  if(pulse_devout->app_buffer[0] != NULL){
    free(pulse_devout->app_buffer[0]);
  }
  
  pulse_devout->app_buffer[0] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_PULSE_DEVOUT_APP_BUFFER_1 */
  if(pulse_devout->app_buffer[1] != NULL){
    free(pulse_devout->app_buffer[1]);
  }

  pulse_devout->app_buffer[1] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_PULSE_DEVOUT_APP_BUFFER_2 */
  if(pulse_devout->app_buffer[2] != NULL){
    free(pulse_devout->app_buffer[2]);
  }

  pulse_devout->app_buffer[2] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_PULSE_DEVOUT_APP_BUFFER_3 */
  if(pulse_devout->app_buffer[3] != NULL){
    free(pulse_devout->app_buffer[3]);
  }
  
  pulse_devout->app_buffer[3] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_PULSE_DEVOUT_APP_BUFFER_4 */
  if(pulse_devout->app_buffer[4] != NULL){
    free(pulse_devout->app_buffer[4]);
  }
  
  pulse_devout->app_buffer[4] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_PULSE_DEVOUT_APP_BUFFER_5 */
  if(pulse_devout->app_buffer[5] != NULL){
    free(pulse_devout->app_buffer[5]);
  }
  
  pulse_devout->app_buffer[5] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_PULSE_DEVOUT_APP_BUFFER_6 */
  if(pulse_devout->app_buffer[6] != NULL){
    free(pulse_devout->app_buffer[6]);
  }
  
  pulse_devout->app_buffer[6] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_PULSE_DEVOUT_APP_BUFFER_7 */
  if(pulse_devout->app_buffer[7] != NULL){
    free(pulse_devout->app_buffer[7]);
  }
  
  pulse_devout->app_buffer[7] = (void *) malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_pulse_devout_new:
 *
 * Creates a new instance of #AgsPulseDevout.
 *
 * Returns: the new #AgsPulseDevout
 *
 * Since: 3.0.0
 */
AgsPulseDevout*
ags_pulse_devout_new()
{
  AgsPulseDevout *pulse_devout;

  pulse_devout = (AgsPulseDevout *) g_object_new(AGS_TYPE_PULSE_DEVOUT,
						 NULL);
  
  return(pulse_devout);
}
