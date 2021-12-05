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

#include <ags/audio/ags_alsa_devout.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_soundcard_util.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <ags/i18n.h>

void ags_alsa_devout_class_init(AgsAlsaDevoutClass *alsa_devout);
void ags_alsa_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_alsa_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_alsa_devout_init(AgsAlsaDevout *alsa_devout);
void ags_alsa_devout_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_alsa_devout_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_alsa_devout_dispose(GObject *gobject);
void ags_alsa_devout_finalize(GObject *gobject);

AgsUUID* ags_alsa_devout_get_uuid(AgsConnectable *connectable);
gboolean ags_alsa_devout_has_resource(AgsConnectable *connectable);
gboolean ags_alsa_devout_is_ready(AgsConnectable *connectable);
void ags_alsa_devout_add_to_registry(AgsConnectable *connectable);
void ags_alsa_devout_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_alsa_devout_list_resource(AgsConnectable *connectable);
xmlNode* ags_alsa_devout_xml_compose(AgsConnectable *connectable);
void ags_alsa_devout_xml_parse(AgsConnectable *connectable,
			       xmlNode *node);
gboolean ags_alsa_devout_is_connected(AgsConnectable *connectable);
void ags_alsa_devout_connect(AgsConnectable *connectable);
void ags_alsa_devout_disconnect(AgsConnectable *connectable);

void ags_alsa_devout_set_device(AgsSoundcard *soundcard,
				gchar *device);
gchar* ags_alsa_devout_get_device(AgsSoundcard *soundcard);

void ags_alsa_devout_set_presets(AgsSoundcard *soundcard,
				 guint channels,
				 guint rate,
				 guint buffer_size,
				 guint format);
void ags_alsa_devout_get_presets(AgsSoundcard *soundcard,
				 guint *channels,
				 guint *rate,
				 guint *buffer_size,
				 guint *format);

void ags_alsa_devout_list_cards(AgsSoundcard *soundcard,
				GList **card_id, GList **card_name);
void ags_alsa_devout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			      guint *channels_min, guint *channels_max,
			      guint *rate_min, guint *rate_max,
			      guint *buffer_size_min, guint *buffer_size_max,
			      GError **error);
guint ags_alsa_devout_get_capability(AgsSoundcard *soundcard);

gboolean ags_alsa_devout_is_available(AgsSoundcard *soundcard);

gboolean ags_alsa_devout_is_starting(AgsSoundcard *soundcard);
gboolean ags_alsa_devout_is_playing(AgsSoundcard *soundcard);

gchar* ags_alsa_devout_get_uptime(AgsSoundcard *soundcard);

gboolean ags_alsa_devout_device_io_func(GIOChannel *source,
					GIOCondition condition,
					AgsAlsaDevout *alsa_devout);

void ags_alsa_devout_device_init(AgsSoundcard *soundcard,
				 GError **error);
void ags_alsa_devout_device_play_fill_ring_buffer(void *buffer,
						  guint ags_format,
						  unsigned char *ring_buffer,
						  guint channels,
						  guint buffer_size);
void ags_alsa_devout_device_play(AgsSoundcard *soundcard,
				 GError **error);
void ags_alsa_devout_device_free(AgsSoundcard *soundcard);

void ags_alsa_devout_tic(AgsSoundcard *soundcard);
void ags_alsa_devout_offset_changed(AgsSoundcard *soundcard,
				    guint note_offset);

void ags_alsa_devout_set_bpm(AgsSoundcard *soundcard,
			     gdouble bpm);
gdouble ags_alsa_devout_get_bpm(AgsSoundcard *soundcard);

void ags_alsa_devout_set_delay_factor(AgsSoundcard *soundcard,
				      gdouble delay_factor);
gdouble ags_alsa_devout_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_alsa_devout_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_alsa_devout_get_delay(AgsSoundcard *soundcard);
guint ags_alsa_devout_get_attack(AgsSoundcard *soundcard);

void* ags_alsa_devout_get_buffer(AgsSoundcard *soundcard);
void* ags_alsa_devout_get_next_buffer(AgsSoundcard *soundcard);
void* ags_alsa_devout_get_prev_buffer(AgsSoundcard *soundcard);

void ags_alsa_devout_lock_buffer(AgsSoundcard *soundcard,
				 void *buffer);
void ags_alsa_devout_unlock_buffer(AgsSoundcard *soundcard,
				   void *buffer);

guint ags_alsa_devout_get_delay_counter(AgsSoundcard *soundcard);

void ags_alsa_devout_set_start_note_offset(AgsSoundcard *soundcard,
					   guint start_note_offset);
guint ags_alsa_devout_get_start_note_offset(AgsSoundcard *soundcard);

void ags_alsa_devout_set_note_offset(AgsSoundcard *soundcard,
				     guint note_offset);
guint ags_alsa_devout_get_note_offset(AgsSoundcard *soundcard);

void ags_alsa_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
					      guint note_offset);
guint ags_alsa_devout_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_alsa_devout_set_loop(AgsSoundcard *soundcard,
			      guint loop_left, guint loop_right,
			      gboolean do_loop);
void ags_alsa_devout_get_loop(AgsSoundcard *soundcard,
			      guint *loop_left, guint *loop_right,
			      gboolean *do_loop);

guint ags_alsa_devout_get_loop_offset(AgsSoundcard *soundcard);

guint ags_alsa_devout_get_sub_block_count(AgsSoundcard *soundcard);

gboolean ags_alsa_devout_trylock_sub_block(AgsSoundcard *soundcard,
					   void *buffer, guint sub_block);
void ags_alsa_devout_unlock_sub_block(AgsSoundcard *soundcard,
				      void *buffer, guint sub_block);

/**
 * SECTION:ags_alsa_devout
 * @short_description: Output to soundcard
 * @title: AgsAlsaDevout
 * @section_id:
 * @include: ags/audio/ags_alsa_devout.h
 *
 * #AgsAlsaDevout represents a soundcard and supports output.
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

static gpointer ags_alsa_devout_parent_class = NULL;

GType
ags_alsa_devout_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_alsa_devout = 0;

    static const GTypeInfo ags_alsa_devout_info = {
      sizeof(AgsAlsaDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_alsa_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAlsaDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_alsa_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_alsa_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_alsa_devout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_alsa_devout = g_type_register_static(G_TYPE_OBJECT,
						  "AgsAlsaDevout",
						  &ags_alsa_devout_info,
						  0);

    g_type_add_interface_static(ags_type_alsa_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_alsa_devout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_alsa_devout);
  }

  return g_define_type_id__volatile;
}

GType
ags_alsa_devout_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_ALSA_DEVOUT_ADDED_TO_REGISTRY, "AGS_ALSA_DEVOUT_ADDED_TO_REGISTRY", "alsa-devout-added-to-registry" },
      { AGS_ALSA_DEVOUT_CONNECTED, "AGS_ALSA_DEVOUT_CONNECTED", "alsa-devout-connected" },
      { AGS_ALSA_DEVOUT_BUFFER0, "AGS_ALSA_DEVOUT_BUFFER0", "alsa-devout-buffer0" },
      { AGS_ALSA_DEVOUT_BUFFER1, "AGS_ALSA_DEVOUT_BUFFER1", "alsa-devout-buffer1" },
      { AGS_ALSA_DEVOUT_BUFFER2, "AGS_ALSA_DEVOUT_BUFFER2", "alsa-devout-buffer2" },
      { AGS_ALSA_DEVOUT_BUFFER3, "AGS_ALSA_DEVOUT_BUFFER3", "alsa-devout-buffer3" },
      { AGS_ALSA_DEVOUT_ATTACK_FIRST, "AGS_ALSA_DEVOUT_ATTACK_FIRST", "alsa-devout-attack-first" },
      { AGS_ALSA_DEVOUT_PLAY, "AGS_ALSA_DEVOUT_PLAY", "alsa-devout-play" },
      { AGS_ALSA_DEVOUT_SHUTDOWN, "AGS_ALSA_DEVOUT_SHUTDOWN", "alsa-devout-shutdown" },
      { AGS_ALSA_DEVOUT_START_PLAY, "AGS_ALSA_DEVOUT_START_PLAY", "alsa-devout-start-play" },
      { AGS_ALSA_DEVOUT_NONBLOCKING, "AGS_ALSA_DEVOUT_NONBLOCKING", "alsa-devout-nonblocking" },
      { AGS_ALSA_DEVOUT_INITIALIZED, "AGS_ALSA_DEVOUT_INITIALIZED", "alsa-devout-initialized" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsAlsaDevoutFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

void
ags_alsa_devout_class_init(AgsAlsaDevoutClass *alsa_devout)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_alsa_devout_parent_class = g_type_class_peek_parent(alsa_devout);

  /* GObjectClass */
  gobject = (GObjectClass *) alsa_devout;

  gobject->set_property = ags_alsa_devout_set_property;
  gobject->get_property = ags_alsa_devout_get_property;

  gobject->dispose = ags_alsa_devout_dispose;
  gobject->finalize = ags_alsa_devout_finalize;

  /* properties */
  /**
   * AgsAlsaDevout:device:
   *
   * The alsa soundcard indentifier
   * 
   * Since: 3.13.1
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   AGS_ALSA_DEVOUT_DEFAULT_ALSA_DEVICE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsAlsaDevout:dsp-channels:
   *
   * The dsp channel count
   * 
   * Since: 3.13.1
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
   * AgsAlsaDevout:pcm-channels:
   *
   * The pcm channel count
   * 
   * Since: 3.13.1
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
   * AgsAlsaDevout:format:
   *
   * The precision of the buffer
   * 
   * Since: 3.13.1
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
   * AgsAlsaDevout:buffer-size:
   *
   * The buffer size
   * 
   * Since: 3.13.1
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
   * AgsAlsaDevout:samplerate:
   *
   * The samplerate
   * 
   * Since: 3.13.1
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
   * AgsAlsaDevout:buffer:
   *
   * The buffer
   * 
   * Since: 3.13.1
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to play"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsAlsaDevout:bpm:
   *
   * Beats per minute
   * 
   * Since: 3.13.1
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
   * AgsAlsaDevout:delay-factor:
   *
   * tact
   * 
   * Since: 3.13.1
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
   * AgsAlsaDevout:attack:
   *
   * Attack of the buffer
   * 
   * Since: 3.13.1
   */
  param_spec = g_param_spec_pointer("attack",
				    i18n_pspec("attack of buffer"),
				    i18n_pspec("The attack to use for the buffer"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);
  
  /* AgsAlsaDevoutClass */
}

GQuark
ags_alsa_devout_error_quark()
{
  return(g_quark_from_static_string("ags-alsa-devout-error-quark"));
}

void
ags_alsa_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_alsa_devout_get_uuid;
  connectable->has_resource = ags_alsa_devout_has_resource;

  connectable->is_ready = ags_alsa_devout_is_ready;
  connectable->add_to_registry = ags_alsa_devout_add_to_registry;
  connectable->remove_from_registry = ags_alsa_devout_remove_from_registry;

  connectable->list_resource = ags_alsa_devout_list_resource;
  connectable->xml_compose = ags_alsa_devout_xml_compose;
  connectable->xml_parse = ags_alsa_devout_xml_parse;

  connectable->is_connected = ags_alsa_devout_is_connected;  
  connectable->connect = ags_alsa_devout_connect;
  connectable->disconnect = ags_alsa_devout_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_alsa_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_alsa_devout_set_device;
  soundcard->get_device = ags_alsa_devout_get_device;
  
  soundcard->set_presets = ags_alsa_devout_set_presets;
  soundcard->get_presets = ags_alsa_devout_get_presets;

  soundcard->list_cards = ags_alsa_devout_list_cards;
  soundcard->pcm_info = ags_alsa_devout_pcm_info;
  soundcard->get_capability = ags_alsa_devout_get_capability;
  
  soundcard->is_available = ags_alsa_devout_is_available;

  soundcard->is_starting =  ags_alsa_devout_is_starting;
  soundcard->is_playing = ags_alsa_devout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_alsa_devout_get_uptime;
  
  soundcard->play_init = ags_alsa_devout_device_play_init;
  soundcard->play = ags_alsa_devout_device_play;
  
  soundcard->record_init = NULL;
  soundcard->record = NULL;
  
  soundcard->stop = ags_alsa_devout_device_free;

  soundcard->tic = ags_alsa_devout_tic;
  soundcard->offset_changed = ags_alsa_devout_offset_changed;
    
  soundcard->set_bpm = ags_alsa_devout_set_bpm;
  soundcard->get_bpm = ags_alsa_devout_get_bpm;

  soundcard->set_delay_factor = ags_alsa_devout_set_delay_factor;
  soundcard->get_delay_factor = ags_alsa_devout_get_delay_factor;

  soundcard->get_absolute_delay = ags_alsa_devout_get_absolute_delay;

  soundcard->get_delay = ags_alsa_devout_get_delay;
  soundcard->get_attack = ags_alsa_devout_get_attack;

  soundcard->get_buffer = ags_alsa_devout_get_buffer;
  soundcard->get_next_buffer = ags_alsa_devout_get_next_buffer;
  soundcard->get_prev_buffer = ags_alsa_devout_get_prev_buffer;

  soundcard->lock_buffer = ags_alsa_devout_lock_buffer;
  soundcard->unlock_buffer = ags_alsa_devout_unlock_buffer;

  soundcard->get_delay_counter = ags_alsa_devout_get_delay_counter;

  soundcard->set_start_note_offset = ags_alsa_devout_set_start_note_offset;
  soundcard->get_start_note_offset = ags_alsa_devout_get_start_note_offset;

  soundcard->set_note_offset = ags_alsa_devout_set_note_offset;
  soundcard->get_note_offset = ags_alsa_devout_get_note_offset;

  soundcard->set_note_offset_absolute = ags_alsa_devout_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_alsa_devout_get_note_offset_absolute;

  soundcard->set_loop = ags_alsa_devout_set_loop;
  soundcard->get_loop = ags_alsa_devout_get_loop;

  soundcard->get_loop_offset = ags_alsa_devout_get_loop_offset;

  soundcard->get_sub_block_count = ags_alsa_devout_get_sub_block_count;

  soundcard->trylock_sub_block = ags_alsa_devout_trylock_sub_block;
  soundcard->unlock_sub_block = ags_alsa_devout_unlock_sub_block;
}

void
ags_alsa_devout_init(AgsAlsaDevout *alsa_devout)
{  
  AgsConfig *config;
  
  gchar *str;
  gchar *segmentation;

  guint i;
  guint denominator, numerator;
  
  alsa_devout->flags = 0;
  
  /* insert alsa_devout mutex */
  g_rec_mutex_init(&(alsa_devout->obj_mutex));

  /* uuid */
  alsa_devout->uuid = ags_uuid_alloc();
  ags_uuid_generate(alsa_devout->uuid);

  /* flags */
  config = ags_config_get_instance();

  /* presets */
  alsa_devout->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  alsa_devout->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  alsa_devout->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  alsa_devout->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  alsa_devout->format = ags_soundcard_helper_config_get_format(config);

  /* device */
  alsa_devout->device = g_strdup(AGS_ALSA_DEVOUT_DEFAULT_ALSA_DEVICE);

  /* buffer */
  alsa_devout->buffer_mutex = (GRecMutex **) malloc(4 * sizeof(GRecMutex *));

  for(i = 0; i < 4; i++){
    alsa_devout->buffer_mutex[i] = (GRecMutex *) malloc(sizeof(GRecMutex));

    g_rec_mutex_init(alsa_devout->buffer_mutex[i]);
  }
  
  alsa_devout->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  alsa_devout->sub_block_mutex = (GRecMutex **) malloc(4 * alsa_devout->sub_block_count * alsa_devout->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < 4 * alsa_devout->sub_block_count * alsa_devout->pcm_channels; i++){
    alsa_devout->sub_block_mutex[i] = (GRecMutex *) malloc(sizeof(GRecMutex));

    g_rec_mutex_init(alsa_devout->sub_block_mutex[i]);
  }

  alsa_devout->buffer = (void **) malloc(4 * sizeof(void *));

  alsa_devout->buffer[0] = NULL;
  alsa_devout->buffer[1] = NULL;
  alsa_devout->buffer[2] = NULL;
  alsa_devout->buffer[3] = NULL;

  g_atomic_int_set(&(alsa_devout->available),
		   TRUE);
  
  alsa_devout->ring_buffer_size = AGS_ALSA_DEVOUT_DEFAULT_RING_BUFFER_SIZE;
  alsa_devout->nth_ring_buffer = 0;
  
  alsa_devout->ring_buffer = NULL;

  ags_alsa_devout_realloc_buffer(alsa_devout);
  
  /* bpm */
  alsa_devout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  alsa_devout->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    alsa_devout->delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }

  /* delay and attack */
  alsa_devout->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					  sizeof(gdouble));
  
  alsa_devout->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					 sizeof(guint));

  ags_alsa_devout_adjust_delay_and_attack(alsa_devout);
  
  /* counters */
  alsa_devout->tact_counter = 0.0;
  alsa_devout->delay_counter = 0;
  alsa_devout->tic_counter = 0;

  alsa_devout->start_note_offset = 0;
  alsa_devout->note_offset = 0;
  alsa_devout->note_offset_absolute = 0;

  alsa_devout->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  alsa_devout->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  alsa_devout->do_loop = FALSE;

  alsa_devout->loop_offset = 0;

  alsa_devout->io_channel = NULL;
  alsa_devout->tag = NULL;
}

void
ags_alsa_devout_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsAlsaDevout *alsa_devout;

  GRecMutex *alsa_devout_mutex;

  alsa_devout = AGS_ALSA_DEVOUT(gobject);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    char *device;

    device = (char *) g_value_get_string(value);

    g_rec_mutex_lock(alsa_devout_mutex);

    g_free(alsa_devout->device);
    
    alsa_devout->device = g_strdup(device);

    g_rec_mutex_unlock(alsa_devout_mutex);
  }
  break;
  case PROP_DSP_CHANNELS:
  {
    guint dsp_channels;

    dsp_channels = g_value_get_uint(value);

    g_rec_mutex_lock(alsa_devout_mutex);

    if(dsp_channels == alsa_devout->dsp_channels){
      g_rec_mutex_unlock(alsa_devout_mutex);
	
      return;
    }

    alsa_devout->dsp_channels = dsp_channels;

    g_rec_mutex_unlock(alsa_devout_mutex);
  }
  break;
  case PROP_PCM_CHANNELS:
  {
    guint pcm_channels, old_pcm_channels;
    guint i;
  
    pcm_channels = g_value_get_uint(value);

    g_rec_mutex_lock(alsa_devout_mutex);

    if(pcm_channels == alsa_devout->pcm_channels){
      g_rec_mutex_unlock(alsa_devout_mutex);
	
      return;
    }

    old_pcm_channels = alsa_devout->pcm_channels;

    /* destroy if less pcm-channels */
    for(i = 4 * alsa_devout->sub_block_count * pcm_channels; i < 4 * alsa_devout->sub_block_count * old_pcm_channels; i++){
      g_rec_mutex_clear(alsa_devout->sub_block_mutex[i]);

      free(alsa_devout->sub_block_mutex[i]);
    }

    alsa_devout->sub_block_mutex = (GRecMutex **) realloc(alsa_devout->sub_block_mutex,
							  4 * alsa_devout->sub_block_count * pcm_channels * sizeof(GRecMutex *));

    /* create if more pcm-channels */
    for(i = 4 * alsa_devout->sub_block_count * old_pcm_channels; i < 4 * alsa_devout->sub_block_count * pcm_channels; i++){
      alsa_devout->sub_block_mutex[i] = (GRecMutex *) malloc(sizeof(GRecMutex));

      g_rec_mutex_init(alsa_devout->sub_block_mutex[i]);
    }
      
    alsa_devout->pcm_channels = pcm_channels;

    g_rec_mutex_unlock(alsa_devout_mutex);
      
    ags_alsa_devout_realloc_buffer(alsa_devout);
  }
  break;
  case PROP_FORMAT:
  {
    guint format;

    format = g_value_get_uint(value);

    g_rec_mutex_lock(alsa_devout_mutex);

    if(format == alsa_devout->format){
      g_rec_mutex_unlock(alsa_devout_mutex);
	
      return;
    }

    alsa_devout->format = format;

    g_rec_mutex_unlock(alsa_devout_mutex);

    ags_alsa_devout_realloc_buffer(alsa_devout);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    guint buffer_size;

    buffer_size = g_value_get_uint(value);

    g_rec_mutex_lock(alsa_devout_mutex);

    if(buffer_size == alsa_devout->buffer_size){
      g_rec_mutex_unlock(alsa_devout_mutex);

      return;
    }

    alsa_devout->buffer_size = buffer_size;

    g_rec_mutex_unlock(alsa_devout_mutex);

    ags_alsa_devout_realloc_buffer(alsa_devout);
    ags_alsa_devout_adjust_delay_and_attack(alsa_devout);
  }
  break;
  case PROP_SAMPLERATE:
  {
    guint samplerate;

    samplerate = g_value_get_uint(value);

    g_rec_mutex_lock(alsa_devout_mutex);

    if(samplerate == alsa_devout->samplerate){
      g_rec_mutex_unlock(alsa_devout_mutex);

      return;
    }

    alsa_devout->samplerate = samplerate;

    g_rec_mutex_unlock(alsa_devout_mutex);

    ags_alsa_devout_adjust_delay_and_attack(alsa_devout);
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

    g_rec_mutex_lock(alsa_devout_mutex);

    if(bpm == alsa_devout->bpm){
      g_rec_mutex_unlock(alsa_devout_mutex);

      return;
    }

    alsa_devout->bpm = bpm;

    g_rec_mutex_unlock(alsa_devout_mutex);

    ags_alsa_devout_adjust_delay_and_attack(alsa_devout);
  }
  break;
  case PROP_DELAY_FACTOR:
  {
    gdouble delay_factor;
      
    delay_factor = g_value_get_double(value);

    g_rec_mutex_lock(alsa_devout_mutex);

    if(delay_factor == alsa_devout->delay_factor){
      g_rec_mutex_unlock(alsa_devout_mutex);

      return;
    }

    alsa_devout->delay_factor = delay_factor;

    g_rec_mutex_unlock(alsa_devout_mutex);

    ags_alsa_devout_adjust_delay_and_attack(alsa_devout);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_alsa_devout_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsAlsaDevout *alsa_devout;

  GRecMutex *alsa_devout_mutex;

  alsa_devout = AGS_ALSA_DEVOUT(gobject);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    g_rec_mutex_lock(alsa_devout_mutex);

    g_value_set_string(value, alsa_devout->device);

    g_rec_mutex_unlock(alsa_devout_mutex);
  }
  break;
  case PROP_DSP_CHANNELS:
  {
    g_rec_mutex_lock(alsa_devout_mutex);

    g_value_set_uint(value, alsa_devout->dsp_channels);

    g_rec_mutex_unlock(alsa_devout_mutex);
  }
  break;
  case PROP_PCM_CHANNELS:
  {
    g_rec_mutex_lock(alsa_devout_mutex);

    g_value_set_uint(value, alsa_devout->pcm_channels);

    g_rec_mutex_unlock(alsa_devout_mutex);
  }
  break;
  case PROP_FORMAT:
  {
    g_rec_mutex_lock(alsa_devout_mutex);

    g_value_set_uint(value, alsa_devout->format);

    g_rec_mutex_unlock(alsa_devout_mutex);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    g_rec_mutex_lock(alsa_devout_mutex);

    g_value_set_uint(value, alsa_devout->buffer_size);

    g_rec_mutex_unlock(alsa_devout_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    g_rec_mutex_lock(alsa_devout_mutex);

    g_value_set_uint(value, alsa_devout->samplerate);

    g_rec_mutex_unlock(alsa_devout_mutex);
  }
  break;
  case PROP_BUFFER:
  {
    g_rec_mutex_lock(alsa_devout_mutex);

    g_value_set_pointer(value, alsa_devout->buffer);

    g_rec_mutex_unlock(alsa_devout_mutex);
  }
  break;
  case PROP_BPM:
  {
    g_rec_mutex_lock(alsa_devout_mutex);

    g_value_set_double(value, alsa_devout->bpm);

    g_rec_mutex_unlock(alsa_devout_mutex);
  }
  break;
  case PROP_DELAY_FACTOR:
  {
    g_rec_mutex_lock(alsa_devout_mutex);

    g_value_set_double(value, alsa_devout->delay_factor);

    g_rec_mutex_unlock(alsa_devout_mutex);
  }
  break;
  case PROP_ATTACK:
  {
    g_rec_mutex_lock(alsa_devout_mutex);

    g_value_set_pointer(value, alsa_devout->attack);

    g_rec_mutex_unlock(alsa_devout_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_alsa_devout_dispose(GObject *gobject)
{
  AgsAlsaDevout *alsa_devout;

  alsa_devout = AGS_ALSA_DEVOUT(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_alsa_devout_parent_class)->dispose(gobject);
}

void
ags_alsa_devout_finalize(GObject *gobject)
{
  AgsAlsaDevout *alsa_devout;

  alsa_devout = AGS_ALSA_DEVOUT(gobject);

  ags_uuid_free(alsa_devout->uuid);
  
  /* free output buffer */
  free(alsa_devout->buffer[0]);
  free(alsa_devout->buffer[1]);
  free(alsa_devout->buffer[2]);
  free(alsa_devout->buffer[3]);

  /* free buffer array */
  free(alsa_devout->buffer);

  /* free AgsAttack */
  free(alsa_devout->attack);
  
  /* call parent */
  G_OBJECT_CLASS(ags_alsa_devout_parent_class)->finalize(gobject);
}

AgsUUID*
ags_alsa_devout_get_uuid(AgsConnectable *connectable)
{
  AgsAlsaDevout *alsa_devout;
  
  AgsUUID *ptr;

  GRecMutex *alsa_devout_mutex;

  alsa_devout = AGS_ALSA_DEVOUT(connectable);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* get UUID */
  g_rec_mutex_lock(alsa_devout_mutex);

  ptr = alsa_devout->uuid;

  g_rec_mutex_unlock(alsa_devout_mutex);
  
  return(ptr);
}

gboolean
ags_alsa_devout_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_alsa_devout_is_ready(AgsConnectable *connectable)
{
  AgsAlsaDevout *alsa_devout;
  
  gboolean is_ready;

  alsa_devout = AGS_ALSA_DEVOUT(connectable);

  /* check is added */
  is_ready = ags_alsa_devout_test_flags(alsa_devout, AGS_ALSA_DEVOUT_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_alsa_devout_add_to_registry(AgsConnectable *connectable)
{
  AgsAlsaDevout *alsa_devout;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  alsa_devout = AGS_ALSA_DEVOUT(connectable);

  ags_alsa_devout_set_flags(alsa_devout, AGS_ALSA_DEVOUT_ADDED_TO_REGISTRY);
}

void
ags_alsa_devout_remove_from_registry(AgsConnectable *connectable)
{
  AgsAlsaDevout *alsa_devout;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  alsa_devout = AGS_ALSA_DEVOUT(connectable);

  ags_alsa_devout_unset_flags(alsa_devout, AGS_ALSA_DEVOUT_ADDED_TO_REGISTRY);
}

xmlNode*
ags_alsa_devout_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_alsa_devout_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_alsa_devout_xml_parse(AgsConnectable *connectable,
			  xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_alsa_devout_is_connected(AgsConnectable *connectable)
{
  AgsAlsaDevout *alsa_devout;
  
  gboolean is_connected;

  alsa_devout = AGS_ALSA_DEVOUT(connectable);

  /* check is connected */
  is_connected = ags_alsa_devout_test_flags(alsa_devout, AGS_ALSA_DEVOUT_CONNECTED);
  
  return(is_connected);
}

void
ags_alsa_devout_connect(AgsConnectable *connectable)
{
  AgsAlsaDevout *alsa_devout;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  alsa_devout = AGS_ALSA_DEVOUT(connectable);

  ags_alsa_devout_set_flags(alsa_devout, AGS_ALSA_DEVOUT_CONNECTED);
}

void
ags_alsa_devout_disconnect(AgsConnectable *connectable)
{

  AgsAlsaDevout *alsa_devout;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  alsa_devout = AGS_ALSA_DEVOUT(connectable);
  
  ags_alsa_devout_unset_flags(alsa_devout, AGS_ALSA_DEVOUT_CONNECTED);
}

/**
 * ags_alsa_devout_test_flags:
 * @alsa_devout: the #AgsAlsaDevout
 * @flags: the flags
 *
 * Test @flags to be set on @alsa_devout.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.13.1
 */
gboolean
ags_alsa_devout_test_flags(AgsAlsaDevout *alsa_devout, guint flags)
{
  gboolean retval;  
  
  GRecMutex *alsa_devout_mutex;

  if(!AGS_IS_ALSA_DEVOUT(alsa_devout)){
    return(FALSE);
  }

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* test */
  g_rec_mutex_lock(alsa_devout_mutex);

  retval = (flags & (alsa_devout->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(alsa_devout_mutex);

  return(retval);
}

/**
 * ags_alsa_devout_set_flags:
 * @alsa_devout: the #AgsAlsaDevout
 * @flags: see #AgsAlsaDevoutFlags-enum
 *
 * Enable a feature of @alsa_devout.
 *
 * Since: 3.13.1
 */
void
ags_alsa_devout_set_flags(AgsAlsaDevout *alsa_devout, guint flags)
{
  GRecMutex *alsa_devout_mutex;

  if(!AGS_IS_ALSA_DEVOUT(alsa_devout)){
    return;
  }

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(alsa_devout_mutex);

  alsa_devout->flags |= flags;
  
  g_rec_mutex_unlock(alsa_devout_mutex);
}
    
/**
 * ags_alsa_devout_unset_flags:
 * @alsa_devout: the #AgsAlsaDevout
 * @flags: see #AgsAlsaDevoutFlags-enum
 *
 * Disable a feature of @alsa_devout.
 *
 * Since: 3.13.1
 */
void
ags_alsa_devout_unset_flags(AgsAlsaDevout *alsa_devout, guint flags)
{  
  GRecMutex *alsa_devout_mutex;

  if(!AGS_IS_ALSA_DEVOUT(alsa_devout)){
    return;
  }

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(alsa_devout_mutex);

  alsa_devout->flags &= (~flags);
  
  g_rec_mutex_unlock(alsa_devout_mutex);
}

void
ags_alsa_devout_set_device(AgsSoundcard *soundcard,
			   gchar *device)
{
  AgsAlsaDevout *alsa_devout;

  GList *card_id, *card_id_start, *card_name, *card_name_start;
  
  GRecMutex *alsa_devout_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* list cards */
  card_id = NULL;
  card_name = NULL;
  
  ags_soundcard_list_cards(soundcard,
			   &card_id, &card_name);

  card_id_start = card_id;
  card_name_start = card_name;

  /* check card */
  g_rec_mutex_lock(alsa_devout_mutex);

  while(card_id != NULL){
    if(!g_ascii_strncasecmp(card_id->data,
			    device,
			    strlen(card_id->data))){
      alsa_devout->device = g_strdup(device);

      break;
    }
    
    card_id = card_id->next;
  }

  g_rec_mutex_unlock(alsa_devout_mutex);

  /* free card id and name */
  g_list_free_full(card_id_start,
		   g_free);
  g_list_free_full(card_name_start,
		   g_free);
}

gchar*
ags_alsa_devout_get_device(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  gchar *device;

  GRecMutex *alsa_devout_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);
  
  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  device = NULL;
  
  g_rec_mutex_lock(alsa_devout_mutex);

  device = g_strdup(alsa_devout->device);

  g_rec_mutex_unlock(alsa_devout_mutex);

  return(device);
}

void
ags_alsa_devout_set_presets(AgsSoundcard *soundcard,
			    guint channels,
			    guint samplerate,
			    guint buffer_size,
			    guint format)
{
  AgsAlsaDevout *alsa_devout;

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  g_object_set(alsa_devout,
	       "pcm-channels", channels,
	       "samplerate", samplerate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_alsa_devout_get_presets(AgsSoundcard *soundcard,
			    guint *channels,
			    guint *samplerate,
			    guint *buffer_size,
			    guint *format)
{
  AgsAlsaDevout *alsa_devout;

  GRecMutex *alsa_devout_mutex;

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* get presets */
  g_rec_mutex_lock(alsa_devout_mutex);

  if(channels != NULL){
    *channels = alsa_devout->pcm_channels;
  }

  if(samplerate != NULL){
    *samplerate = alsa_devout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = alsa_devout->buffer_size;
  }

  if(format != NULL){
    *format = alsa_devout->format;
  }

  g_rec_mutex_unlock(alsa_devout_mutex);
}

void
ags_alsa_devout_list_cards(AgsSoundcard *soundcard,
			   GList **card_id, GList **card_name)
{
  AgsAlsaDevout *alsa_devout;

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

  GRecMutex *alsa_devout_mutex;

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  if(card_id != NULL){
    card_id[0] = NULL;
  }

  if(card_name != NULL){
    card_name[0] = NULL;
  }

#if defined(AGS_WITH_ALSA)
  g_rec_mutex_lock(alsa_devout_mutex);

  /* the default device */
  str = g_strdup("default");
    
  error = snd_ctl_open(&card_handle, str, 0);

  if(error < 0){
    g_free(str);
      
    goto ags_alsa_devout_list_cards_NO_DEFAULT_0;
  }

  snd_ctl_card_info_alloca(&card_info);
  error = snd_ctl_card_info(card_handle, card_info);

  if(error < 0){
    g_free(str);

    goto ags_alsa_devout_list_cards_NO_DEFAULT_0;
  }

  if(card_id != NULL){
    card_id[0] = g_list_prepend(card_id[0], str);
  }

  if(card_name != NULL){
    card_name[0] = g_list_prepend(card_name[0], g_strdup(snd_ctl_card_info_get_name(card_info)));
  }
    
  snd_ctl_close(card_handle);

ags_alsa_devout_list_cards_NO_DEFAULT_0:
    
  /* enumerated devices */
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
	    
	  card_id[0] = g_list_prepend(card_id[0],
				      g_strdup(hint));

	  if(hint != NULL){
	    free(hint);
	  }
	}

	if(card_name != NULL){
	  char *name;

	  name = snd_ctl_card_info_get_name(card_info);
	    
	  card_name[0] = g_list_prepend(card_name[0], g_strdup(name));
	}
      }

      snd_device_name_free_hint(hints);
    }
      
    snd_ctl_close(card_handle);

    g_free(str);
  }

  snd_config_update_free_global();

  g_rec_mutex_unlock(alsa_devout_mutex);

  if(card_id != NULL){
    card_id[0] = g_list_reverse(card_id[0]);
  }

  if(card_name != NULL){
    card_name[0] = g_list_reverse(card_name[0]);
  }
#endif
}

void
ags_alsa_devout_pcm_info(AgsSoundcard *soundcard,
			 char *card_id,
			 guint *channels_min, guint *channels_max,
			 guint *rate_min, guint *rate_max,
			 guint *buffer_size_min, guint *buffer_size_max,
			 GError **error)
{
  AgsAlsaDevout *alsa_devout;

#if defined(AGS_WITH_ALSA)
  snd_pcm_t *handle;
  snd_pcm_hw_params_t *params;
#endif
  
  gchar *str;
    
  unsigned int val;
  int dir;

#if defined(AGS_WITH_ALSA)
  snd_pcm_uframes_t frames;
#endif
  
  int rc;
  int err;

  GRecMutex *alsa_devout_mutex;

  if(card_id == NULL){
    return;
  }
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

#if defined(AGS_WITH_ALSA)
  /* pcm info */
  g_rec_mutex_lock(alsa_devout_mutex);

  /* Open PCM device for playback. */
  handle = NULL;

  rc = snd_pcm_open(&handle, card_id, SND_PCM_STREAM_PLAYBACK, 0);

  if(rc < 0){      
    str = snd_strerror(rc);
    g_message("unable to open pcm device (attempting fixup): %s", str);

    if(index(card_id,
	     ',') != NULL){
      gchar *device_fixup;
	
      device_fixup = g_strndup(card_id,
			       index(card_id,
				     ',') - card_id);
      handle = NULL;
	
      rc = snd_pcm_open(&handle, device_fixup, SND_PCM_STREAM_PLAYBACK, 0);
      
      if(rc < 0){
	if(error != NULL){
	  g_set_error(error,
		      AGS_ALSA_DEVOUT_ERROR,
		      AGS_ALSA_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		      "unable to open pcm device: %s\n",
		      str);
	}
	  
	//    free(str);
	  
	goto ags_alsa_devout_pcm_info_ERR;
      }
    }else{
      if(error != NULL){
	g_set_error(error,
		    AGS_ALSA_DEVOUT_ERROR,
		    AGS_ALSA_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		    "unable to open pcm device: %s\n",
		    str);
      }
	
      goto ags_alsa_devout_pcm_info_ERR;
    }
  }

  /* Allocate a hardware parameters object. */
  snd_pcm_hw_params_alloca(&params);

  /* Fill it in with default values. */
  snd_pcm_hw_params_any(handle, params);

  /* channels */
  snd_pcm_hw_params_get_channels_min(params, &val);
  *channels_min = val;

  snd_pcm_hw_params_get_channels_max(params, &val);
  *channels_max = val;

  /* samplerate */
  dir = 0;
  snd_pcm_hw_params_get_rate_min(params, &val, &dir);
  *rate_min = val;

  dir = 0;
  snd_pcm_hw_params_get_rate_max(params, &val, &dir);
  *rate_max = val;

  /* buffer size */
  dir = 0;
  snd_pcm_hw_params_get_buffer_size_min(params, &frames);
  *buffer_size_min = frames;

  dir = 0;
  snd_pcm_hw_params_get_buffer_size_max(params, &frames);
  *buffer_size_max = frames;

  snd_pcm_close(handle);

ags_alsa_devout_pcm_info_ERR:

  g_rec_mutex_unlock(alsa_devout_mutex);
#endif  
}

guint
ags_alsa_devout_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_PLAYBACK);
}

gboolean
ags_alsa_devout_is_available(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;
  
#ifdef AGS_WITH_ALSA
  snd_pcm_t *handle;

  struct pollfd fds;
#endif

  gboolean is_available;

  GRecMutex *alsa_devout_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);
  
  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);  

#ifdef AGS_WITH_ALSA
  /* check is starting */
  g_rec_mutex_lock(alsa_devout_mutex);

  handle = alsa_devout->handle;
  
  g_rec_mutex_unlock(alsa_devout_mutex);

  if(handle != NULL){
    fds.events = POLLOUT;
  
    snd_pcm_poll_descriptors(handle, &fds, 1);
  
    poll(&fds, 1, 0);
  
    /* check available */
    is_available = ((POLLOUT & (fds.revents)) != 0) ? TRUE: FALSE;
  }else{
    is_available = FALSE;
  }
#else
  is_available = FALSE;
#endif
  
  return(is_available);
}

gboolean
ags_alsa_devout_is_starting(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  gboolean is_starting;
  
  GRecMutex *alsa_devout_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* check is starting */
  g_rec_mutex_lock(alsa_devout_mutex);

  is_starting = ((AGS_ALSA_DEVOUT_START_PLAY & (alsa_devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(alsa_devout_mutex);
  
  return(is_starting);
}

gboolean
ags_alsa_devout_is_playing(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  gboolean is_playing;
  
  GRecMutex *alsa_devout_mutex;

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);
  
  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* check is starting */
  g_rec_mutex_lock(alsa_devout_mutex);

  is_playing = ((AGS_ALSA_DEVOUT_PLAY & (alsa_devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(alsa_devout_mutex);

  return(is_playing);
}

gchar*
ags_alsa_devout_get_uptime(AgsSoundcard *soundcard)
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

gboolean
ags_alsa_devout_device_io_func(GIOChannel *source,
			       GIOCondition condition,
			       AgsAlsaDevout *alsa_devout)
{
  g_atomic_int_set(&(alsa_devout->available), TRUE);

  return(TRUE);
}

void
ags_alsa_devout_device_init(AgsSoundcard *soundcard,
			    GError **error)
{
  AgsAlsaDevout *alsa_devout;

#ifdef AGS_WITH_ALSA
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
  guint i, i_stop;
  
  GRecMutex *alsa_devout_mutex; 
 
  static unsigned int period_time = 100000;
  static unsigned int buffer_time = 100000;

  if(ags_soundcard_is_playing(soundcard)){
    return;
  }
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* retrieve word size */
  g_rec_mutex_lock(alsa_devout_mutex);

  if(alsa_devout->device == NULL){
    g_rec_mutex_unlock(alsa_devout_mutex);
    
    return;
  }

#ifdef AGS_WITH_ALSA
  format = SND_PCM_FORMAT_S16;
#endif
  
  switch(alsa_devout->format){
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
    g_rec_mutex_unlock(alsa_devout_mutex);

    g_warning("ags_alsa_devout_alsa_init(): unsupported word size");

    return;
  }

  /* prepare for playback */
  alsa_devout->flags |= (AGS_ALSA_DEVOUT_START_PLAY |
			 AGS_ALSA_DEVOUT_PLAY |
			 AGS_ALSA_DEVOUT_NONBLOCKING);

  memset(alsa_devout->buffer[0], 0, alsa_devout->pcm_channels * alsa_devout->buffer_size * word_size);
  memset(alsa_devout->buffer[1], 0, alsa_devout->pcm_channels * alsa_devout->buffer_size * word_size);
  memset(alsa_devout->buffer[2], 0, alsa_devout->pcm_channels * alsa_devout->buffer_size * word_size);
  memset(alsa_devout->buffer[3], 0, alsa_devout->pcm_channels * alsa_devout->buffer_size * word_size);

  /* allocate ring buffer */
#ifdef AGS_WITH_ALSA
  alsa_devout->ring_buffer = (unsigned char **) malloc(alsa_devout->ring_buffer_size * sizeof(unsigned char *));

  for(i = 0; i < alsa_devout->ring_buffer_size; i++){
    alsa_devout->ring_buffer[i] = (unsigned char *) malloc(alsa_devout->pcm_channels *
							   alsa_devout->buffer_size * (snd_pcm_format_physical_width(format) / 8) *
							   sizeof(unsigned char));
  }
 
  /*  */
  period_event = 0;
  
  /* Open PCM device for playback. */  
  handle = NULL;

  if((err = snd_pcm_open(&handle, alsa_devout->device, SND_PCM_STREAM_PLAYBACK, 0)) < 0){
    gchar *device_fixup;
    
    str = snd_strerror(err);
    g_warning("Playback open error (attempting fixup): %s", str);
    
    device_fixup = g_strdup_printf("%s,0",
				   alsa_devout->device);

    handle = NULL;
    
    if((err = snd_pcm_open(&handle, device_fixup, SND_PCM_STREAM_PLAYBACK, 0)) < 0){
      alsa_devout->flags &= (~(AGS_ALSA_DEVOUT_START_PLAY |
			       AGS_ALSA_DEVOUT_PLAY |
			       AGS_ALSA_DEVOUT_NONBLOCKING));
      
      g_rec_mutex_unlock(alsa_devout_mutex);
      
      if(error != NULL){
	g_set_error(error,
		    AGS_ALSA_DEVOUT_ERROR,
		    AGS_ALSA_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		    "unable to open pcm device: %s",
		    str);
      }

      g_free(device_fixup);
      
      return;
    }
  }

  snd_pcm_hw_params_alloca(&hwparams);
  snd_pcm_sw_params_alloca(&swparams);

  /* choose all parameters */
  err = snd_pcm_hw_params_any(handle, hwparams);

  if (err < 0) {
    alsa_devout->flags &= (~(AGS_ALSA_DEVOUT_START_PLAY |
			     AGS_ALSA_DEVOUT_PLAY |
			     AGS_ALSA_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(alsa_devout_mutex);

    str = snd_strerror(err);
    g_warning("Broken configuration for playback: no configurations available: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVOUT_ERROR,
		  AGS_ALSA_DEVOUT_ERROR_BROKEN_CONFIGURATION,
		  "unable to open pcm device: %s",
		  str);
    }

    alsa_devout->handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set hardware resampling * /
     err = snd_pcm_hw_params_set_rate_resample(handle, hwparams, 0);
     if (err < 0) {
     g_rec_mutex_unlock(alsa_devout_mutex);

     str = snd_strerror(err);
     g_warning("Resampling setup failed for playback: %s\n", str);

     //    free(str);
    
     return;
     }
  */
  
  /* set the interleaved read/write format */
  err = snd_pcm_hw_params_set_access(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
  if (err < 0) {
    alsa_devout->flags &= (~(AGS_ALSA_DEVOUT_START_PLAY |
			     AGS_ALSA_DEVOUT_PLAY |
			     AGS_ALSA_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(alsa_devout_mutex);

    str = snd_strerror(err);
    g_warning("Access type not available for playback: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVOUT_ERROR,
		  AGS_ALSA_DEVOUT_ERROR_ACCESS_TYPE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    alsa_devout->handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the sample format */
  err = snd_pcm_hw_params_set_format(handle, hwparams, format);
  if (err < 0) {
    alsa_devout->flags &= (~(AGS_ALSA_DEVOUT_START_PLAY |
			     AGS_ALSA_DEVOUT_PLAY |
			     AGS_ALSA_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(alsa_devout_mutex);

    str = snd_strerror(err);
    g_warning("Sample format not available for playback: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVOUT_ERROR,
		  AGS_ALSA_DEVOUT_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    alsa_devout->handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the count of channels */
  channels = alsa_devout->pcm_channels;
  err = snd_pcm_hw_params_set_channels(handle, hwparams, channels);
  if (err < 0) {
    alsa_devout->flags &= (~(AGS_ALSA_DEVOUT_START_PLAY |
			     AGS_ALSA_DEVOUT_PLAY |
			     AGS_ALSA_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(alsa_devout_mutex);

    str = snd_strerror(err);
    g_warning("Channels count (%i) not available for playbacks: %s", channels, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVOUT_ERROR,
		  AGS_ALSA_DEVOUT_ERROR_CHANNELS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    alsa_devout->handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the stream rate */
  rate = alsa_devout->samplerate;
  rrate = rate;
  err = snd_pcm_hw_params_set_rate_near(handle, hwparams, &rrate, 0);
  if (err < 0) {
    alsa_devout->flags &= (~(AGS_ALSA_DEVOUT_START_PLAY |
			     AGS_ALSA_DEVOUT_PLAY |
			     AGS_ALSA_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(alsa_devout_mutex);

    str = snd_strerror(err);
    g_warning("Rate %iHz not available for playback: %s", rate, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVOUT_ERROR,
		  AGS_ALSA_DEVOUT_ERROR_SAMPLERATE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    alsa_devout->handle = NULL;
    
    //    free(str);
    
    return;
  }

  if (rrate != rate) {
    alsa_devout->flags &= (~(AGS_ALSA_DEVOUT_START_PLAY |
			     AGS_ALSA_DEVOUT_PLAY |
			     AGS_ALSA_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(alsa_devout_mutex);
    
    g_warning("Rate doesn't match (requested %iHz, get %iHz)", rate, err);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVOUT_ERROR,
		  AGS_ALSA_DEVOUT_ERROR_SAMPLERATE_NOT_AVAILABLE,
		  "unable to open pcm device");
    }

    alsa_devout->handle = NULL;
    
    return;
  }

  /* set the buffer size */
  size = 2 * alsa_devout->buffer_size;
  err = snd_pcm_hw_params_set_buffer_size(handle, hwparams, size);

  if (err < 0) {
    alsa_devout->flags &= (~(AGS_ALSA_DEVOUT_START_PLAY |
			     AGS_ALSA_DEVOUT_PLAY |
			     AGS_ALSA_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(alsa_devout_mutex);

    str = snd_strerror(err);
    g_warning("Unable to set buffer size %lu for playback: %s", size, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVOUT_ERROR,
		  AGS_ALSA_DEVOUT_ERROR_BUFFER_SIZE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    alsa_devout->handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the period size * /
     period_size = alsa_devout->buffer_size;
     err = snd_pcm_hw_params_set_period_size_near(handle, hwparams, period_size, dir);
     if (err < 0) {
     g_rec_mutex_unlock(alsa_devout_mutex);

     str = snd_strerror(err);
     g_warning("Unable to get period size for playback: %s\n", str);

     //    free(str);
    
     return;
     }
  */
  
  /* write the parameters to device */
  err = snd_pcm_hw_params(handle, hwparams);

  if (err < 0) {
    alsa_devout->flags &= (~(AGS_ALSA_DEVOUT_START_PLAY |
			     AGS_ALSA_DEVOUT_PLAY |
			     AGS_ALSA_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(alsa_devout_mutex);

    str = snd_strerror(err);
    g_warning("Unable to set hw params for playback: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_ALSA_DEVOUT_ERROR,
		  AGS_ALSA_DEVOUT_ERROR_HW_PARAMETERS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    alsa_devout->handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* get the current swparams * /
     err = snd_pcm_sw_params_current(handle, swparams);
     if (err < 0) {
     g_rec_mutex_unlock(alsa_devout_mutex);

     str = snd_strerror(err);
     g_warning("Unable to determine current swparams for playback: %s\n", str);

     //    free(str);
    
     return;
     }
  */
  /* start the transfer when the buffer is almost full: */
  /* (buffer_size / avail_min) * avail_min * /
     err = snd_pcm_sw_params_set_start_threshold(handle, swparams, (buffer_size / period_size) * period_size);
     if (err < 0) {
     g_rec_mutex_unlock(alsa_devout_mutex);

     str = snd_strerror(err);
     g_warning("Unable to set start threshold mode for playback: %s\n", str);
    
     //    free(str);
    
     return;
     }
  */
  /* allow the transfer when at least period_size samples can be processed */
  /* or disable this mechanism when period event is enabled (aka interrupt like style processing) * /
     err = snd_pcm_sw_params_set_avail_min(handle, swparams, period_event ? buffer_size : period_size);
     if (err < 0) {
     g_rec_mutex_unlock(alsa_devout_mutex);

     str = snd_strerror(err);
     g_warning("Unable to set avail min for playback: %s\n", str);

     //    free(str);
    
     return;
     }

     /* write the parameters to the playback device * /
     err = snd_pcm_sw_params(handle, swparams);
     if (err < 0) {
     g_rec_mutex_unlock(alsa_devout_mutex);

     str = snd_strerror(err);
     g_warning("Unable to set sw params for playback: %s\n", str);

     //    free(str);
    
     return;
     }
  */

  /*  */
  alsa_devout->handle = handle;

#if 0
  i_stop = snd_pcm_poll_descriptors_count(alsa_devout->handle);

  if(i_stop > 0){
    struct pollfd *fds;
    
    fds = (struct pollfd *) malloc(i_stop * sizeof(struct pollfd));
    
    snd_pcm_poll_descriptors(alsa_devout->handle, fds, i_stop);
    
    for(i = 0; i < i_stop; i++){
      GIOChannel *io_channel;

      guint tag;

      io_channel = g_io_channel_unix_new(fds[i].fd);
      tag = g_io_add_watch(io_channel,
			   G_IO_OUT,
			   (GIOFunc) ags_alsa_devout_alsa_io_func,
			   alsa_devout);
      
      alsa_devout->io_channel = g_list_prepend(alsa_devout->io_channel,
					       io_channel);
      alsa_devout->tag = g_list_prepend(alsa_devout->tag,
					GUINT_TO_POINTER(tag));
    }
  }
#endif
#endif

  alsa_devout->tact_counter = 0.0;
  alsa_devout->delay_counter = floor(ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(alsa_devout)));
  alsa_devout->tic_counter = 0;

  alsa_devout->nth_ring_buffer = 0;
    
#ifdef AGS_WITH_ALSA
  alsa_devout->flags |= AGS_ALSA_DEVOUT_INITIALIZED;
#endif
  alsa_devout->flags |= AGS_ALSA_DEVOUT_BUFFER0;
  alsa_devout->flags &= (~(AGS_ALSA_DEVOUT_BUFFER1 |
			   AGS_ALSA_DEVOUT_BUFFER2 |
			   AGS_ALSA_DEVOUT_BUFFER3));
  
  g_rec_mutex_unlock(alsa_devout_mutex);
}

void
ags_alsa_devout_device_play_fill_ring_buffer(void *buffer,
					     guint ags_format,
					     unsigned char *ring_buffer,
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

  int res[8];

  gint count;
  guint i, chn;

  format = SND_PCM_FORMAT_S16;
    
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
    g_warning("ags_alsa_devout_alsa_play(): unsupported word size");
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
  for(count = 0; count < buffer_size - (buffer_size % 8);){
    for(chn = 0; chn < channels; chn++){
      switch(ags_format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	res[0] = (int) ((gint8 *) buffer)[count * channels + chn];
	res[1] = (int) ((gint8 *) buffer)[(count + 1) * channels + chn];
	res[2] = (int) ((gint8 *) buffer)[(count + 2) * channels + chn];
	res[3] = (int) ((gint8 *) buffer)[(count + 3) * channels + chn];
	res[4] = (int) ((gint8 *) buffer)[(count + 4) * channels + chn];
	res[5] = (int) ((gint8 *) buffer)[(count + 5) * channels + chn];
	res[6] = (int) ((gint8 *) buffer)[(count + 6) * channels + chn];
	res[7] = (int) ((gint8 *) buffer)[(count + 7) * channels + chn];
      }
      break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	res[0] = (int) ((gint16 *) buffer)[count * channels + chn];
	res[1] = (int) ((gint16 *) buffer)[(count + 1) * channels + chn];
	res[2] = (int) ((gint16 *) buffer)[(count + 2) * channels + chn];
	res[3] = (int) ((gint16 *) buffer)[(count + 3) * channels + chn];
	res[4] = (int) ((gint16 *) buffer)[(count + 4) * channels + chn];
	res[5] = (int) ((gint16 *) buffer)[(count + 5) * channels + chn];
	res[6] = (int) ((gint16 *) buffer)[(count + 6) * channels + chn];
	res[7] = (int) ((gint16 *) buffer)[(count + 7) * channels + chn];
      }
      break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	res[0] = (int) ((gint32 *) buffer)[count * channels + chn];
	res[1] = (int) ((gint32 *) buffer)[(count + 1) * channels + chn];
	res[2] = (int) ((gint32 *) buffer)[(count + 2) * channels + chn];
	res[3] = (int) ((gint32 *) buffer)[(count + 3) * channels + chn];
	res[4] = (int) ((gint32 *) buffer)[(count + 4) * channels + chn];
	res[5] = (int) ((gint32 *) buffer)[(count + 5) * channels + chn];
	res[6] = (int) ((gint32 *) buffer)[(count + 6) * channels + chn];
	res[7] = (int) ((gint32 *) buffer)[(count + 7) * channels + chn];
      }
      break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	res[0] = (int) ((gint32 *) buffer)[count * channels + chn];
	res[1] = (int) ((gint32 *) buffer)[(count + 1) * channels + chn];
	res[2] = (int) ((gint32 *) buffer)[(count + 2) * channels + chn];
	res[3] = (int) ((gint32 *) buffer)[(count + 3) * channels + chn];
	res[4] = (int) ((gint32 *) buffer)[(count + 4) * channels + chn];
	res[5] = (int) ((gint32 *) buffer)[(count + 5) * channels + chn];
	res[6] = (int) ((gint32 *) buffer)[(count + 6) * channels + chn];
	res[7] = (int) ((gint32 *) buffer)[(count + 7) * channels + chn];
      }
      break;
      default:
	res[0] = 0;
	res[1] = 0;
	res[2] = 0;
	res[3] = 0;
	res[4] = 0;
	res[5] = 0;
	res[6] = 0;
	res[7] = 0;	  
      }

      if(to_unsigned){
	res[0] ^= 1U << (format_bits - 1);
	res[1] ^= 1U << (format_bits - 1);
	res[2] ^= 1U << (format_bits - 1);
	res[3] ^= 1U << (format_bits - 1);
	res[4] ^= 1U << (format_bits - 1);
	res[5] ^= 1U << (format_bits - 1);
	res[6] ^= 1U << (format_bits - 1);
	res[7] ^= 1U << (format_bits - 1);
      }
	
      /* Generate data in native endian format */
      if(big_endian){
	for(i = 0; i < bps; i++){
	  *(ring_buffer + chn * bps + phys_bps - 1 - i) = (res[0] >> i * 8) & 0xff;
	  *(ring_buffer + channels * bps + chn * bps + phys_bps - 1 - i) = (res[1] >> i * 8) & 0xff;
	  *(ring_buffer + 2 * channels * bps + chn * bps + phys_bps - 1 - i) = (res[2] >> i * 8) & 0xff;
	  *(ring_buffer + 3 * channels * bps + chn * bps + phys_bps - 1 - i) = (res[3] >> i * 8) & 0xff;
	  *(ring_buffer + 4 * channels * bps + chn * bps + phys_bps - 1 - i) = (res[4] >> i * 8) & 0xff;
	  *(ring_buffer + 5 * channels * bps + chn * bps + phys_bps - 1 - i) = (res[5] >> i * 8) & 0xff;
	  *(ring_buffer + 6 * channels * bps + chn * bps + phys_bps - 1 - i) = (res[6] >> i * 8) & 0xff;
	  *(ring_buffer + 7 * channels * bps + chn * bps + phys_bps - 1 - i) = (res[7] >> i * 8) & 0xff;
	}
      }else{
	for(i = 0; i < bps; i++){
	  *(ring_buffer + chn * bps + i) = (res[0] >> i * 8) & 0xff;
	  *(ring_buffer + channels * bps + chn * bps + i) = (res[1] >>  i * 8) & 0xff;
	  *(ring_buffer + 2 * channels * bps + chn * bps + i) = (res[2] >>  i * 8) & 0xff;
	  *(ring_buffer + 3 * channels * bps + chn * bps + i) = (res[3] >>  i * 8) & 0xff;
	  *(ring_buffer + 4 * channels * bps + chn * bps + i) = (res[4] >>  i * 8) & 0xff;
	  *(ring_buffer + 5 * channels * bps + chn * bps + i) = (res[5] >>  i * 8) & 0xff;
	  *(ring_buffer + 6 * channels * bps + chn * bps + i) = (res[6] >>  i * 8) & 0xff;
	  *(ring_buffer + 7 * channels * bps + chn * bps + i) = (res[7] >>  i * 8) & 0xff;
	}
      }
    }

    ring_buffer += 8 * channels * bps;
    count += 8;
  }

  for(; count < buffer_size; count++){
    for(chn = 0; chn < channels; chn++){
      switch(ags_format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	res[0] = (int) ((gint8 *) buffer)[count * channels + chn];
      }
      break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	res[0] = (int) ((gint16 *) buffer)[count * channels + chn];
      }
      break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	res[0] = (int) ((gint32 *) buffer)[count * channels + chn];
      }
      break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	res[0] = (int) ((gint32 *) buffer)[count * channels + chn];
      }
      break;
      default:
	res[0] = 0;
      }

      if(to_unsigned){
	res[0] ^= 1U << (format_bits - 1);
      }
	
      /* Generate data in native endian format */
      if(big_endian){
	for(i = 0; i < bps; i++){
	  *(ring_buffer + chn * bps + phys_bps - 1 - i) = (res[0] >> i * 8) & 0xff;
	}
      }else{
	for(i = 0; i < bps; i++){
	  *(ring_buffer + chn * bps + i) = (res[0] >>  i * 8) & 0xff;
	}
      }	
    }

    ring_buffer += channels * bps;
  }
#endif
}

void
ags_alsa_devout_device_play(AgsSoundcard *soundcard,
			    GError **error)
{
  AgsAlsaDevout *alsa_devout;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;
  GList *list;
  
  gchar *str;

  gint64 poll_timeout;
  guint word_size;
  guint nth_buffer;
  
  GRecMutex *alsa_devout_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);
  
  application_context = ags_application_context_get_instance();

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* lock */
  g_rec_mutex_lock(alsa_devout_mutex);
  
  /* retrieve word size */
  switch(alsa_devout->format){
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
    g_rec_mutex_unlock(alsa_devout_mutex);
    
    g_warning("ags_alsa_devout_alsa_play(): unsupported word size");

    return;
  }

  /* do playback */
  alsa_devout->flags &= (~AGS_ALSA_DEVOUT_START_PLAY);

  if((AGS_ALSA_DEVOUT_INITIALIZED & (alsa_devout->flags)) == 0){
    g_rec_mutex_unlock(alsa_devout_mutex);
    
    return;
  }

  //  g_message("play - 0x%0x", ((AGS_ALSA_DEVOUT_BUFFER0 |
  //				AGS_ALSA_DEVOUT_BUFFER1 |
  //				AGS_ALSA_DEVOUT_BUFFER2 |
  //				AGS_ALSA_DEVOUT_BUFFER3) & (alsa_devout->flags)));

  /* check buffer flag */
  nth_buffer = 0;
  
  if((AGS_ALSA_DEVOUT_BUFFER0 & (alsa_devout->flags)) != 0){
    nth_buffer = 0;
  }else if((AGS_ALSA_DEVOUT_BUFFER1 & (alsa_devout->flags)) != 0){
    nth_buffer = 1;
  }else if((AGS_ALSA_DEVOUT_BUFFER2 & (alsa_devout->flags)) != 0){
    nth_buffer = 2;
  }else if((AGS_ALSA_DEVOUT_BUFFER3 & (alsa_devout->flags)) != 0){
    nth_buffer = 3;
  }

#ifdef AGS_WITH_ALSA

  /* fill ring buffer */
  ags_soundcard_lock_buffer(soundcard,
			    alsa_devout->buffer[nth_buffer]);
  
  ags_alsa_devout_alsa_play_fill_ring_buffer(alsa_devout->buffer[nth_buffer], alsa_devout->format,
					     alsa_devout->ring_buffer[alsa_devout->nth_ring_buffer],
					     alsa_devout->pcm_channels, alsa_devout->buffer_size);

  ags_soundcard_unlock_buffer(soundcard,
			      alsa_devout->buffer[nth_buffer]);

  /* wait until available */
  poll_timeout = g_get_monotonic_time() + (G_USEC_PER_SEC * (1.0 / (gdouble) alsa_devout->samplerate * (gdouble) alsa_devout->buffer_size));

  g_rec_mutex_unlock(alsa_devout_mutex);
  
  //TODO:JK: implement me
  while(!ags_soundcard_is_available(AGS_SOUNDCARD(alsa_devout))){
    g_usleep(1);

    if(g_get_monotonic_time() > poll_timeout){
      break;
    }
  }
  
  g_atomic_int_set(&(alsa_devout->available),
		   FALSE);
  
  g_rec_mutex_lock(alsa_devout_mutex);

  /* write ring buffer */
//  g_message("write %d", alsa_devout->buffer_size);
  
  alsa_devout->rc = snd_pcm_writei(alsa_devout->handle,
				   alsa_devout->ring_buffer[alsa_devout->nth_ring_buffer],
				   (snd_pcm_uframes_t) (alsa_devout->buffer_size));
  
  /* check error flag */
  if((AGS_ALSA_DEVOUT_NONBLOCKING & (alsa_devout->flags)) == 0){
    if(alsa_devout->rc == -EPIPE){
      /* EPIPE means underrun */
      snd_pcm_prepare(alsa_devout->handle);

#ifdef AGS_DEBUG
      g_message("underrun occurred");
#endif
    }else if(alsa_devout->rc == -ESTRPIPE){
      static const struct timespec idle = {
	0,
	4000,
      };

      int err;

      while((err = snd_pcm_resume(alsa_devout->handle)) < 0){ // == -EAGAIN
	nanosleep(&idle, NULL); /* wait until the suspend flag is released */
      }
	
      if(err < 0){
	err = snd_pcm_prepare(alsa_devout->handle);
      }
    }else if(alsa_devout->rc < 0){
      str = snd_strerror(alsa_devout->rc);
      
      g_message("error from writei: %s", str);
    }else if(alsa_devout->rc != (int) alsa_devout->buffer_size) {
      g_message("short write, write %d frames", alsa_devout->rc);
    }
  }      
  
#endif

  /* increment nth ring-buffer */
  if(alsa_devout->nth_ring_buffer + 1 >= alsa_devout->ring_buffer_size){
    alsa_devout->nth_ring_buffer = 0;
  }else{
    alsa_devout->nth_ring_buffer += 1;
  }
  
  g_rec_mutex_unlock(alsa_devout_mutex);

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;
  
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) alsa_devout);
  
  task = g_list_prepend(task,
			tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) alsa_devout);

  task = g_list_prepend(task,
			clear_buffer);

  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) alsa_devout);

  task = g_list_prepend(task,
			switch_buffer_flag);

  /* append tasks */
  task = g_list_reverse(task);
  
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  g_list_free_full(task,
		   g_object_unref);
  
#ifdef AGS_WITH_ALSA
  snd_pcm_prepare(alsa_devout->handle);
#endif

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_alsa_devout_device_free(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  GList *start_list, *list;
  
  guint i;
  
  GRecMutex *alsa_devout_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* lock */
  g_rec_mutex_lock(alsa_devout_mutex);

  if((AGS_ALSA_DEVOUT_INITIALIZED & (alsa_devout->flags)) == 0){
    g_rec_mutex_unlock(alsa_devout_mutex);
    
    return;
  }

  g_rec_mutex_unlock(alsa_devout_mutex);
  
#ifdef AGS_WITH_ALSA
  //  snd_pcm_drain(alsa_devout->handle);
  snd_pcm_close(alsa_devout->handle);
  alsa_devout->handle = NULL;
#endif

  /* free ring-buffer */
  g_rec_mutex_lock(alsa_devout_mutex);

  if(alsa_devout->ring_buffer != NULL){
    for(i = 0; i < alsa_devout->ring_buffer_size; i++){
      free(alsa_devout->ring_buffer[i]);
    }
    
    free(alsa_devout->ring_buffer);
  }
  
  alsa_devout->ring_buffer = NULL;

  /* reset flags */
  alsa_devout->flags &= (~(AGS_ALSA_DEVOUT_BUFFER0 |
			   AGS_ALSA_DEVOUT_BUFFER1 |
			   AGS_ALSA_DEVOUT_BUFFER2 |
			   AGS_ALSA_DEVOUT_BUFFER3 |
			   AGS_ALSA_DEVOUT_PLAY |
			   AGS_ALSA_DEVOUT_INITIALIZED));

  g_rec_mutex_unlock(alsa_devout_mutex);

  g_rec_mutex_lock(alsa_devout_mutex);

  alsa_devout->note_offset = alsa_devout->start_note_offset;
  alsa_devout->note_offset_absolute = alsa_devout->start_note_offset;

  list = alsa_devout->tag;

  while(list != NULL){
    g_source_remove(GPOINTER_TO_UINT(list->data));

    list = list->next;
  }

  g_list_free(alsa_devout->tag);
  
  alsa_devout->tag = NULL;

  g_list_free_full(alsa_devout->io_channel,
		   g_io_channel_unref);

  alsa_devout->io_channel = NULL;

  g_atomic_int_set(&(alsa_devout->available), TRUE);
  
  g_rec_mutex_unlock(alsa_devout_mutex);
}

void
ags_alsa_devout_tic(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  gdouble delay;
  gdouble delay_counter;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  
  GRecMutex *alsa_devout_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);
  
  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(alsa_devout_mutex);

  delay = alsa_devout->delay[alsa_devout->tic_counter];
  delay_counter = alsa_devout->delay_counter;

  note_offset = alsa_devout->note_offset;
  note_offset_absolute = alsa_devout->note_offset_absolute;
  
  loop_left = alsa_devout->loop_left;
  loop_right = alsa_devout->loop_right;
  
  do_loop = alsa_devout->do_loop;

  g_rec_mutex_unlock(alsa_devout_mutex);

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
    g_rec_mutex_lock(alsa_devout_mutex);
    
    alsa_devout->delay_counter = delay_counter + 1.0 - delay;
    alsa_devout->tact_counter += 1.0;

    g_rec_mutex_unlock(alsa_devout_mutex);
  }else{
    g_rec_mutex_lock(alsa_devout_mutex);
    
    alsa_devout->delay_counter += 1.0;

    g_rec_mutex_unlock(alsa_devout_mutex);
  }
}

void
ags_alsa_devout_offset_changed(AgsSoundcard *soundcard,
			       guint note_offset)
{
  AgsAlsaDevout *alsa_devout;
  
  GRecMutex *alsa_devout_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* offset changed */
  g_rec_mutex_lock(alsa_devout_mutex);

  alsa_devout->tic_counter += 1;

  if(alsa_devout->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    alsa_devout->tic_counter = 0;
  }

  g_rec_mutex_unlock(alsa_devout_mutex);
}

void
ags_alsa_devout_set_bpm(AgsSoundcard *soundcard,
			gdouble bpm)
{
  AgsAlsaDevout *alsa_devout;

  GRecMutex *alsa_devout_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* set bpm */
  g_rec_mutex_lock(alsa_devout_mutex);

  alsa_devout->bpm = bpm;

  g_rec_mutex_unlock(alsa_devout_mutex);

  ags_alsa_devout_adjust_delay_and_attack(alsa_devout);
}

gdouble
ags_alsa_devout_get_bpm(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  gdouble bpm;
  
  GRecMutex *alsa_devout_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* get bpm */
  g_rec_mutex_lock(alsa_devout_mutex);

  bpm = alsa_devout->bpm;
  
  g_rec_mutex_unlock(alsa_devout_mutex);

  return(bpm);
}

void
ags_alsa_devout_set_delay_factor(AgsSoundcard *soundcard,
				 gdouble delay_factor)
{
  AgsAlsaDevout *alsa_devout;

  GRecMutex *alsa_devout_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* set delay factor */
  g_rec_mutex_lock(alsa_devout_mutex);

  alsa_devout->delay_factor = delay_factor;

  g_rec_mutex_unlock(alsa_devout_mutex);

  ags_alsa_devout_adjust_delay_and_attack(alsa_devout);
}

gdouble
ags_alsa_devout_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  gdouble delay_factor;
  
  GRecMutex *alsa_devout_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* get delay factor */
  g_rec_mutex_lock(alsa_devout_mutex);

  delay_factor = alsa_devout->delay_factor;
  
  g_rec_mutex_unlock(alsa_devout_mutex);

  return(delay_factor);
}

gdouble
ags_alsa_devout_get_delay(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  guint delay_index;
  gdouble delay;
  
  GRecMutex *alsa_devout_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* get delay */
  g_rec_mutex_lock(alsa_devout_mutex);

  delay_index = alsa_devout->tic_counter;

  delay = alsa_devout->delay[delay_index];
  
  g_rec_mutex_unlock(alsa_devout_mutex);
  
  return(delay);
}

gdouble
ags_alsa_devout_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  gdouble absolute_delay;
  
  GRecMutex *alsa_devout_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);
  
  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* get absolute delay */
  g_rec_mutex_lock(alsa_devout_mutex);

  absolute_delay = (60.0 * (((gdouble) alsa_devout->samplerate / (gdouble) alsa_devout->buffer_size) / (gdouble) alsa_devout->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) alsa_devout->delay_factor)));

  g_rec_mutex_unlock(alsa_devout_mutex);

  return(absolute_delay);
}

guint
ags_alsa_devout_get_attack(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  guint attack_index;
  guint attack;
  
  GRecMutex *alsa_devout_mutex;  

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);
  
  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* get attack */
  g_rec_mutex_lock(alsa_devout_mutex);

  attack_index = alsa_devout->tic_counter;

  attack = alsa_devout->attack[attack_index];

  g_rec_mutex_unlock(alsa_devout_mutex);
  
  return(attack);
}

void*
ags_alsa_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  void *buffer;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  if(ags_alsa_devout_test_flags(alsa_devout, AGS_ALSA_DEVOUT_BUFFER0)){
    buffer = alsa_devout->buffer[0];
  }else if(ags_alsa_devout_test_flags(alsa_devout, AGS_ALSA_DEVOUT_BUFFER1)){
    buffer = alsa_devout->buffer[1];
  }else if(ags_alsa_devout_test_flags(alsa_devout, AGS_ALSA_DEVOUT_BUFFER2)){
    buffer = alsa_devout->buffer[2];
  }else if(ags_alsa_devout_test_flags(alsa_devout, AGS_ALSA_DEVOUT_BUFFER3)){
    buffer = alsa_devout->buffer[3];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_alsa_devout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  void *buffer;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  //  g_message("next - 0x%0x", ((AGS_ALSA_DEVOUT_BUFFER0 |
  //				AGS_ALSA_DEVOUT_BUFFER1 |
  //				AGS_ALSA_DEVOUT_BUFFER2 |
  //				AGS_ALSA_DEVOUT_BUFFER3) & (alsa_devout->flags)));

  if(ags_alsa_devout_test_flags(alsa_devout, AGS_ALSA_DEVOUT_BUFFER0)){
    buffer = alsa_devout->buffer[1];
  }else if(ags_alsa_devout_test_flags(alsa_devout, AGS_ALSA_DEVOUT_BUFFER1)){
    buffer = alsa_devout->buffer[2];
  }else if(ags_alsa_devout_test_flags(alsa_devout, AGS_ALSA_DEVOUT_BUFFER2)){
    buffer = alsa_devout->buffer[3];
  }else if(ags_alsa_devout_test_flags(alsa_devout, AGS_ALSA_DEVOUT_BUFFER3)){
    buffer = alsa_devout->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_alsa_devout_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  void *buffer;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  if(ags_alsa_devout_test_flags(alsa_devout, AGS_ALSA_DEVOUT_BUFFER0)){
    buffer = alsa_devout->buffer[3];
  }else if(ags_alsa_devout_test_flags(alsa_devout, AGS_ALSA_DEVOUT_BUFFER1)){
    buffer = alsa_devout->buffer[0];
  }else if(ags_alsa_devout_test_flags(alsa_devout, AGS_ALSA_DEVOUT_BUFFER2)){
    buffer = alsa_devout->buffer[1];
  }else if(ags_alsa_devout_test_flags(alsa_devout, AGS_ALSA_DEVOUT_BUFFER3)){
    buffer = alsa_devout->buffer[2];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void
ags_alsa_devout_lock_buffer(AgsSoundcard *soundcard,
			    void *buffer)
{
  AgsAlsaDevout *alsa_devout;

  GRecMutex *buffer_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(alsa_devout->buffer != NULL){
    if(buffer == alsa_devout->buffer[0]){
      buffer_mutex = alsa_devout->buffer_mutex[0];
    }else if(buffer == alsa_devout->buffer[1]){
      buffer_mutex = alsa_devout->buffer_mutex[1];
    }else if(buffer == alsa_devout->buffer[2]){
      buffer_mutex = alsa_devout->buffer_mutex[2];
    }else if(buffer == alsa_devout->buffer[3]){
      buffer_mutex = alsa_devout->buffer_mutex[3];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_alsa_devout_unlock_buffer(AgsSoundcard *soundcard,
			      void *buffer)
{
  AgsAlsaDevout *alsa_devout;

  GRecMutex *buffer_mutex;
  
  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(alsa_devout->buffer != NULL){
    if(buffer == alsa_devout->buffer[0]){
      buffer_mutex = alsa_devout->buffer_mutex[0];
    }else if(buffer == alsa_devout->buffer[1]){
      buffer_mutex = alsa_devout->buffer_mutex[1];
    }else if(buffer == alsa_devout->buffer[2]){
      buffer_mutex = alsa_devout->buffer_mutex[2];
    }else if(buffer == alsa_devout->buffer[3]){
      buffer_mutex = alsa_devout->buffer_mutex[3];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_alsa_devout_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  guint delay_counter;
  
  GRecMutex *alsa_devout_mutex;  

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);
  
  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* delay counter */
  g_rec_mutex_lock(alsa_devout_mutex);

  delay_counter = alsa_devout->delay_counter;
  
  g_rec_mutex_unlock(alsa_devout_mutex);

  return(delay_counter);
}

void
ags_alsa_devout_set_start_note_offset(AgsSoundcard *soundcard,
				      guint start_note_offset)
{
  AgsAlsaDevout *alsa_devout;

  GRecMutex *alsa_devout_mutex;  

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* set note offset */
  g_rec_mutex_lock(alsa_devout_mutex);

  alsa_devout->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(alsa_devout_mutex);
}

guint
ags_alsa_devout_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  guint start_note_offset;
  
  GRecMutex *alsa_devout_mutex;  

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* set note offset */
  g_rec_mutex_lock(alsa_devout_mutex);

  start_note_offset = alsa_devout->start_note_offset;

  g_rec_mutex_unlock(alsa_devout_mutex);

  return(start_note_offset);
}

void
ags_alsa_devout_set_note_offset(AgsSoundcard *soundcard,
				guint note_offset)
{
  AgsAlsaDevout *alsa_devout;

  GRecMutex *alsa_devout_mutex;  

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* set note offset */
  g_rec_mutex_lock(alsa_devout_mutex);

  alsa_devout->note_offset = note_offset;

  g_rec_mutex_unlock(alsa_devout_mutex);
}

guint
ags_alsa_devout_get_note_offset(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  guint note_offset;
  
  GRecMutex *alsa_devout_mutex;  

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* set note offset */
  g_rec_mutex_lock(alsa_devout_mutex);

  note_offset = alsa_devout->note_offset;

  g_rec_mutex_unlock(alsa_devout_mutex);

  return(note_offset);
}

void
ags_alsa_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
					 guint note_offset_absolute)
{
  AgsAlsaDevout *alsa_devout;
  
  GRecMutex *alsa_devout_mutex;  

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* set note offset */
  g_rec_mutex_lock(alsa_devout_mutex);

  alsa_devout->note_offset_absolute = note_offset_absolute;

  g_rec_mutex_unlock(alsa_devout_mutex);
}

guint
ags_alsa_devout_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  guint note_offset_absolute;
  
  GRecMutex *alsa_devout_mutex;  

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* set note offset */
  g_rec_mutex_lock(alsa_devout_mutex);

  note_offset_absolute = alsa_devout->note_offset_absolute;

  g_rec_mutex_unlock(alsa_devout_mutex);

  return(note_offset_absolute);
}

void
ags_alsa_devout_set_loop(AgsSoundcard *soundcard,
			 guint loop_left, guint loop_right,
			 gboolean do_loop)
{
  AgsAlsaDevout *alsa_devout;

  GRecMutex *alsa_devout_mutex;  

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* set loop */
  g_rec_mutex_lock(alsa_devout_mutex);

  alsa_devout->loop_left = loop_left;
  alsa_devout->loop_right = loop_right;
  alsa_devout->do_loop = do_loop;

  if(do_loop){
    alsa_devout->loop_offset = alsa_devout->note_offset;
  }

  g_rec_mutex_unlock(alsa_devout_mutex);
}

void
ags_alsa_devout_get_loop(AgsSoundcard *soundcard,
			 guint *loop_left, guint *loop_right,
			 gboolean *do_loop)
{
  AgsAlsaDevout *alsa_devout;

  GRecMutex *alsa_devout_mutex;  

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* get loop */
  g_rec_mutex_lock(alsa_devout_mutex);

  if(loop_left != NULL){
    *loop_left = alsa_devout->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = alsa_devout->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = alsa_devout->do_loop;
  }

  g_rec_mutex_unlock(alsa_devout_mutex);
}

guint
ags_alsa_devout_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  guint loop_offset;
  
  GRecMutex *alsa_devout_mutex;  

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* get loop offset */
  g_rec_mutex_lock(alsa_devout_mutex);

  loop_offset = alsa_devout->loop_offset;
  
  g_rec_mutex_unlock(alsa_devout_mutex);

  return(loop_offset);
}

guint
ags_alsa_devout_get_sub_block_count(AgsSoundcard *soundcard)
{
  AgsAlsaDevout *alsa_devout;

  guint sub_block_count;
  
  GRecMutex *alsa_devout_mutex;  

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* get loop offset */
  g_rec_mutex_lock(alsa_devout_mutex);

  sub_block_count = alsa_devout->sub_block_count;
  
  g_rec_mutex_unlock(alsa_devout_mutex);

  return(sub_block_count);
}

gboolean
ags_alsa_devout_trylock_sub_block(AgsSoundcard *soundcard,
				  void *buffer, guint sub_block)
{
  AgsAlsaDevout *alsa_devout;

  guint pcm_channels;
  guint sub_block_count;
  gboolean success;
  
  GRecMutex *alsa_devout_mutex;  
  GRecMutex *sub_block_mutex;

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* get loop offset */
  g_rec_mutex_lock(alsa_devout_mutex);

  pcm_channels = alsa_devout->pcm_channels;
  sub_block_count = alsa_devout->sub_block_count;
  
  g_rec_mutex_unlock(alsa_devout_mutex);
  
  sub_block_mutex = NULL;

  success = FALSE;
  
  if(alsa_devout->buffer != NULL){
    if(buffer == alsa_devout->buffer[0]){
      sub_block_mutex = alsa_devout->sub_block_mutex[sub_block];
    }else if(buffer == alsa_devout->buffer[1]){
      sub_block_mutex = alsa_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == alsa_devout->buffer[2]){
      sub_block_mutex = alsa_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == alsa_devout->buffer[3]){
      sub_block_mutex = alsa_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
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
ags_alsa_devout_unlock_sub_block(AgsSoundcard *soundcard,
				 void *buffer, guint sub_block)
{
  AgsAlsaDevout *alsa_devout;

  guint pcm_channels;
  guint sub_block_count;
  
  GRecMutex *alsa_devout_mutex;  
  GRecMutex *sub_block_mutex;

  alsa_devout = AGS_ALSA_DEVOUT(soundcard);

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* get loop offset */
  g_rec_mutex_lock(alsa_devout_mutex);

  pcm_channels = alsa_devout->pcm_channels;
  sub_block_count = alsa_devout->sub_block_count;
  
  g_rec_mutex_unlock(alsa_devout_mutex);
  
  sub_block_mutex = NULL;
  
  if(alsa_devout->buffer != NULL){
    if(buffer == alsa_devout->buffer[0]){
      sub_block_mutex = alsa_devout->sub_block_mutex[sub_block];
    }else if(buffer == alsa_devout->buffer[1]){
      sub_block_mutex = alsa_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == alsa_devout->buffer[2]){
      sub_block_mutex = alsa_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == alsa_devout->buffer[3]){
      sub_block_mutex = alsa_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }
  }

  if(sub_block_mutex != NULL){
    g_rec_mutex_unlock(sub_block_mutex);
  }
}

/**
 * ags_alsa_devout_switch_buffer_flag:
 * @alsa_devout: the #AgsAlsaDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.13.1
 */
void
ags_alsa_devout_switch_buffer_flag(AgsAlsaDevout *alsa_devout)
{
  GRecMutex *alsa_devout_mutex;
  
  if(!AGS_IS_ALSA_DEVOUT(alsa_devout)){
    return;
  }

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* switch buffer flag */
  g_rec_mutex_lock(alsa_devout_mutex);

  if((AGS_ALSA_DEVOUT_BUFFER0 & (alsa_devout->flags)) != 0){
    alsa_devout->flags &= (~AGS_ALSA_DEVOUT_BUFFER0);
    alsa_devout->flags |= AGS_ALSA_DEVOUT_BUFFER1;
  }else if((AGS_ALSA_DEVOUT_BUFFER1 & (alsa_devout->flags)) != 0){
    alsa_devout->flags &= (~AGS_ALSA_DEVOUT_BUFFER1);
    alsa_devout->flags |= AGS_ALSA_DEVOUT_BUFFER2;
  }else if((AGS_ALSA_DEVOUT_BUFFER2 & (alsa_devout->flags)) != 0){
    alsa_devout->flags &= (~AGS_ALSA_DEVOUT_BUFFER2);
    alsa_devout->flags |= AGS_ALSA_DEVOUT_BUFFER3;
  }else if((AGS_ALSA_DEVOUT_BUFFER3 & (alsa_devout->flags)) != 0){
    alsa_devout->flags &= (~AGS_ALSA_DEVOUT_BUFFER3);
    alsa_devout->flags |= AGS_ALSA_DEVOUT_BUFFER0;
  }

  g_rec_mutex_unlock(alsa_devout_mutex);
}

/**
 * ags_alsa_devout_adjust_delay_and_attack:
 * @alsa_devout: the #AgsAlsaDevout
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 3.13.1
 */
void
ags_alsa_devout_adjust_delay_and_attack(AgsAlsaDevout *alsa_devout)
{
  if(!AGS_IS_ALSA_DEVOUT(alsa_devout)){
    return;
  }

  ags_soundcard_util_adjust_delay_and_attack(alsa_devout);
}

/**
 * ags_alsa_devout_realloc_buffer:
 * @alsa_devout: the #AgsAlsaDevout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.13.1
 */
void
ags_alsa_devout_realloc_buffer(AgsAlsaDevout *alsa_devout)
{
  guint pcm_channels;
  guint buffer_size;
  guint word_size;
  guint format;
  
  GRecMutex *alsa_devout_mutex;  

  if(!AGS_IS_ALSA_DEVOUT(alsa_devout)){
    return;
  }

  /* get alsa devout mutex */
  alsa_devout_mutex = AGS_ALSA_DEVOUT_GET_OBJ_MUTEX(alsa_devout);

  /* get word size */  
  g_rec_mutex_lock(alsa_devout_mutex);

  pcm_channels = alsa_devout->pcm_channels;
  buffer_size = alsa_devout->buffer_size;

  format = alsa_devout->format;
  
  g_rec_mutex_unlock(alsa_devout_mutex);
  
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
    g_warning("ags_alsa_devout_realloc_buffer(): unsupported word size");
    
    return;
  }  

  //NOTE:JK: there is no lock applicable to buffer

  /* AGS_ALSA_DEVOUT_BUFFER_0 */
  if(alsa_devout->buffer[0] != NULL){
    free(alsa_devout->buffer[0]);
  }
  
  alsa_devout->buffer[0] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_ALSA_DEVOUT_BUFFER_1 */
  if(alsa_devout->buffer[1] != NULL){
    free(alsa_devout->buffer[1]);
  }

  alsa_devout->buffer[1] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_ALSA_DEVOUT_BUFFER_2 */
  if(alsa_devout->buffer[2] != NULL){
    free(alsa_devout->buffer[2]);
  }

  alsa_devout->buffer[2] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_ALSA_DEVOUT_BUFFER_3 */
  if(alsa_devout->buffer[3] != NULL){
    free(alsa_devout->buffer[3]);
  }
  
  alsa_devout->buffer[3] = (void *) malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_alsa_devout_new:
 *
 * Creates a new instance of #AgsAlsaDevout.
 *
 * Returns: the new #AgsAlsaDevout
 *
 * Since: 3.13.1
 */
AgsAlsaDevout*
ags_alsa_devout_new()
{
  AgsAlsaDevout *alsa_devout;

  alsa_devout = (AgsAlsaDevout *) g_object_new(AGS_TYPE_ALSA_DEVOUT,
					       NULL);
  
  return(alsa_devout);
}
