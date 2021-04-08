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

#include <ags/audio/ags_devout.h>

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

void ags_devout_class_init(AgsDevoutClass *devout);
void ags_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_devout_init(AgsDevout *devout);
void ags_devout_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_devout_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_devout_dispose(GObject *gobject);
void ags_devout_finalize(GObject *gobject);

AgsUUID* ags_devout_get_uuid(AgsConnectable *connectable);
gboolean ags_devout_has_resource(AgsConnectable *connectable);
gboolean ags_devout_is_ready(AgsConnectable *connectable);
void ags_devout_add_to_registry(AgsConnectable *connectable);
void ags_devout_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_devout_list_resource(AgsConnectable *connectable);
xmlNode* ags_devout_xml_compose(AgsConnectable *connectable);
void ags_devout_xml_parse(AgsConnectable *connectable,
			   xmlNode *node);
gboolean ags_devout_is_connected(AgsConnectable *connectable);
void ags_devout_connect(AgsConnectable *connectable);
void ags_devout_disconnect(AgsConnectable *connectable);

void ags_devout_set_device(AgsSoundcard *soundcard,
			   gchar *device);
gchar* ags_devout_get_device(AgsSoundcard *soundcard);

void ags_devout_set_presets(AgsSoundcard *soundcard,
			    guint channels,
			    guint rate,
			    guint buffer_size,
			    guint format);
void ags_devout_get_presets(AgsSoundcard *soundcard,
			    guint *channels,
			    guint *rate,
			    guint *buffer_size,
			    guint *format);

void ags_devout_list_cards(AgsSoundcard *soundcard,
			   GList **card_id, GList **card_name);
void ags_devout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			 guint *channels_min, guint *channels_max,
			 guint *rate_min, guint *rate_max,
			 guint *buffer_size_min, guint *buffer_size_max,
			 GError **error);
guint ags_devout_get_capability(AgsSoundcard *soundcard);

gboolean ags_devout_is_available(AgsSoundcard *soundcard);

gboolean ags_devout_is_starting(AgsSoundcard *soundcard);
gboolean ags_devout_is_playing(AgsSoundcard *soundcard);

gchar* ags_devout_get_uptime(AgsSoundcard *soundcard);

void ags_devout_delegate_play_init(AgsSoundcard *soundcard,
				   GError **error);
void ags_devout_delegate_play(AgsSoundcard *soundcard,
			      GError **error);
void ags_devout_delegate_stop(AgsSoundcard *soundcard);

void ags_devout_oss_init(AgsSoundcard *soundcard,
			 GError **error);
void ags_devout_oss_play_fill_ring_buffer(void *buffer,
					  guint ags_format,
					  unsigned char *ring_buffer,
					  guint channels,
					  guint buffer_size);

gboolean ags_devout_oss_io_func(GIOChannel *source,
				GIOCondition condition,
				AgsDevout *devout);

void ags_devout_oss_play(AgsSoundcard *soundcard,
			 GError **error);
void ags_devout_oss_free(AgsSoundcard *soundcard);

gboolean ags_devout_alsa_io_func(GIOChannel *source,
				 GIOCondition condition,
				 AgsDevout *devout);

void ags_devout_alsa_init(AgsSoundcard *soundcard,
			  GError **error);
void ags_devout_alsa_play_fill_ring_buffer(void *buffer,
					   guint ags_format,
					   unsigned char *ring_buffer,
					   guint channels,
					   guint buffer_size);
void ags_devout_alsa_play(AgsSoundcard *soundcard,
			  GError **error);
void ags_devout_alsa_free(AgsSoundcard *soundcard);

void ags_devout_tic(AgsSoundcard *soundcard);
void ags_devout_offset_changed(AgsSoundcard *soundcard,
			       guint note_offset);

void ags_devout_set_bpm(AgsSoundcard *soundcard,
			gdouble bpm);
gdouble ags_devout_get_bpm(AgsSoundcard *soundcard);

void ags_devout_set_delay_factor(AgsSoundcard *soundcard,
				 gdouble delay_factor);
gdouble ags_devout_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_devout_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_devout_get_delay(AgsSoundcard *soundcard);
guint ags_devout_get_attack(AgsSoundcard *soundcard);

void* ags_devout_get_buffer(AgsSoundcard *soundcard);
void* ags_devout_get_next_buffer(AgsSoundcard *soundcard);
void* ags_devout_get_prev_buffer(AgsSoundcard *soundcard);

void ags_devout_lock_buffer(AgsSoundcard *soundcard,
			    void *buffer);
void ags_devout_unlock_buffer(AgsSoundcard *soundcard,
			      void *buffer);

guint ags_devout_get_delay_counter(AgsSoundcard *soundcard);

void ags_devout_set_start_note_offset(AgsSoundcard *soundcard,
				      guint start_note_offset);
guint ags_devout_get_start_note_offset(AgsSoundcard *soundcard);

void ags_devout_set_note_offset(AgsSoundcard *soundcard,
				guint note_offset);
guint ags_devout_get_note_offset(AgsSoundcard *soundcard);

void ags_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
					 guint note_offset);
guint ags_devout_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_devout_set_loop(AgsSoundcard *soundcard,
			 guint loop_left, guint loop_right,
			 gboolean do_loop);
void ags_devout_get_loop(AgsSoundcard *soundcard,
			 guint *loop_left, guint *loop_right,
			 gboolean *do_loop);

guint ags_devout_get_loop_offset(AgsSoundcard *soundcard);

guint ags_devout_get_sub_block_count(AgsSoundcard *soundcard);

gboolean ags_devout_trylock_sub_block(AgsSoundcard *soundcard,
				      void *buffer, guint sub_block);
void ags_devout_unlock_sub_block(AgsSoundcard *soundcard,
				 void *buffer, guint sub_block);

/**
 * SECTION:ags_devout
 * @short_description: Output to soundcard
 * @title: AgsDevout
 * @section_id:
 * @include: ags/audio/ags_devout.h
 *
 * #AgsDevout represents a soundcard and supports output.
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

static gpointer ags_devout_parent_class = NULL;

GType
ags_devout_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_devout = 0;

    static const GTypeInfo ags_devout_info = {
      sizeof(AgsDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_devout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_devout = g_type_register_static(G_TYPE_OBJECT,
					     "AgsDevout",
					     &ags_devout_info,
					     0);

    g_type_add_interface_static(ags_type_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_devout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_devout);
  }

  return g_define_type_id__volatile;
}

GType
ags_devout_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_DEVOUT_ADDED_TO_REGISTRY, "AGS_DEVOUT_ADDED_TO_REGISTRY", "devout-added-to-registry" },
      { AGS_DEVOUT_CONNECTED, "AGS_DEVOUT_CONNECTED", "devout-connected" },
      { AGS_DEVOUT_BUFFER0, "AGS_DEVOUT_BUFFER0", "devout-buffer0" },
      { AGS_DEVOUT_BUFFER1, "AGS_DEVOUT_BUFFER1", "devout-buffer1" },
      { AGS_DEVOUT_BUFFER2, "AGS_DEVOUT_BUFFER2", "devout-buffer2" },
      { AGS_DEVOUT_BUFFER3, "AGS_DEVOUT_BUFFER3", "devout-buffer3" },
      { AGS_DEVOUT_ATTACK_FIRST, "AGS_DEVOUT_ATTACK_FIRST", "devout-attack-first" },
      { AGS_DEVOUT_PLAY, "AGS_DEVOUT_PLAY", "devout-play" },
      { AGS_DEVOUT_OSS, "AGS_DEVOUT_OSS", "devout-oss" },
      { AGS_DEVOUT_ALSA, "AGS_DEVOUT_ALSA", "devout-alsa" },
      { AGS_DEVOUT_SHUTDOWN, "AGS_DEVOUT_SHUTDOWN", "devout-shutdown" },
      { AGS_DEVOUT_START_PLAY, "AGS_DEVOUT_START_PLAY", "devout-start-play" },
      { AGS_DEVOUT_NONBLOCKING, "AGS_DEVOUT_NONBLOCKING", "devout-nonblocking" },
      { AGS_DEVOUT_INITIALIZED, "AGS_DEVOUT_INITIALIZED", "devout-initialized" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsDevoutFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

void
ags_devout_class_init(AgsDevoutClass *devout)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_devout_parent_class = g_type_class_peek_parent(devout);

  /* GObjectClass */
  gobject = (GObjectClass *) devout;

  gobject->set_property = ags_devout_set_property;
  gobject->get_property = ags_devout_get_property;

  gobject->dispose = ags_devout_dispose;
  gobject->finalize = ags_devout_finalize;

  /* properties */
  /**
   * AgsDevout:device:
   *
   * The alsa soundcard indentifier
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   AGS_DEVOUT_DEFAULT_ALSA_DEVICE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsDevout:dsp-channels:
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
   * AgsDevout:pcm-channels:
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
   * AgsDevout:format:
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
   * AgsDevout:buffer-size:
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
   * AgsDevout:samplerate:
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
   * AgsDevout:buffer:
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
   * AgsDevout:bpm:
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
   * AgsDevout:delay-factor:
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
   * AgsDevout:attack:
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
  
  /* AgsDevoutClass */
}

GQuark
ags_devout_error_quark()
{
  return(g_quark_from_static_string("ags-devout-error-quark"));
}

void
ags_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_devout_get_uuid;
  connectable->has_resource = ags_devout_has_resource;

  connectable->is_ready = ags_devout_is_ready;
  connectable->add_to_registry = ags_devout_add_to_registry;
  connectable->remove_from_registry = ags_devout_remove_from_registry;

  connectable->list_resource = ags_devout_list_resource;
  connectable->xml_compose = ags_devout_xml_compose;
  connectable->xml_parse = ags_devout_xml_parse;

  connectable->is_connected = ags_devout_is_connected;  
  connectable->connect = ags_devout_connect;
  connectable->disconnect = ags_devout_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_devout_set_device;
  soundcard->get_device = ags_devout_get_device;
  
  soundcard->set_presets = ags_devout_set_presets;
  soundcard->get_presets = ags_devout_get_presets;

  soundcard->list_cards = ags_devout_list_cards;
  soundcard->pcm_info = ags_devout_pcm_info;
  soundcard->get_capability = ags_devout_get_capability;
  
  soundcard->is_available = ags_devout_is_available;

  soundcard->is_starting =  ags_devout_is_starting;
  soundcard->is_playing = ags_devout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_devout_get_uptime;
  
  soundcard->play_init = ags_devout_delegate_play_init;
  soundcard->play = ags_devout_delegate_play;
  
  soundcard->record_init = NULL;
  soundcard->record = NULL;
  
  soundcard->stop = ags_devout_delegate_stop;

  soundcard->tic = ags_devout_tic;
  soundcard->offset_changed = ags_devout_offset_changed;
    
  soundcard->set_bpm = ags_devout_set_bpm;
  soundcard->get_bpm = ags_devout_get_bpm;

  soundcard->set_delay_factor = ags_devout_set_delay_factor;
  soundcard->get_delay_factor = ags_devout_get_delay_factor;

  soundcard->get_absolute_delay = ags_devout_get_absolute_delay;

  soundcard->get_delay = ags_devout_get_delay;
  soundcard->get_attack = ags_devout_get_attack;

  soundcard->get_buffer = ags_devout_get_buffer;
  soundcard->get_next_buffer = ags_devout_get_next_buffer;
  soundcard->get_prev_buffer = ags_devout_get_prev_buffer;

  soundcard->lock_buffer = ags_devout_lock_buffer;
  soundcard->unlock_buffer = ags_devout_unlock_buffer;

  soundcard->get_delay_counter = ags_devout_get_delay_counter;

  soundcard->set_start_note_offset = ags_devout_set_start_note_offset;
  soundcard->get_start_note_offset = ags_devout_get_start_note_offset;

  soundcard->set_note_offset = ags_devout_set_note_offset;
  soundcard->get_note_offset = ags_devout_get_note_offset;

  soundcard->set_note_offset_absolute = ags_devout_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_devout_get_note_offset_absolute;

  soundcard->set_loop = ags_devout_set_loop;
  soundcard->get_loop = ags_devout_get_loop;

  soundcard->get_loop_offset = ags_devout_get_loop_offset;

  soundcard->get_sub_block_count = ags_devout_get_sub_block_count;

  soundcard->trylock_sub_block = ags_devout_trylock_sub_block;
  soundcard->unlock_sub_block = ags_devout_unlock_sub_block;
}

void
ags_devout_init(AgsDevout *devout)
{  
  AgsConfig *config;
  
  gchar *str;
  gchar *segmentation;

  guint i;
  guint denominator, numerator;
  gboolean use_alsa;  
  
  devout->flags = 0;
  
  /* insert devout mutex */
  g_rec_mutex_init(&(devout->obj_mutex));

  /* uuid */
  devout->uuid = ags_uuid_alloc();
  ags_uuid_generate(devout->uuid);

  /* flags */
  config = ags_config_get_instance();

#ifdef AGS_WITH_ALSA
  use_alsa = TRUE;
#else
  use_alsa = FALSE;
#endif

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "backend");

  if(str == NULL){
    str = ags_config_get_value(config,
			       AGS_CONFIG_SOUNDCARD_0,
			       "backend");
  }
  
  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "oss",
			  4)){
    use_alsa = FALSE;
  }

  if(use_alsa){
    devout->flags |= (AGS_DEVOUT_ALSA);
  }else{
    devout->flags |= (AGS_DEVOUT_OSS);
  }

  g_free(str);

  /* presets */
  devout->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  devout->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  devout->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  devout->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  devout->format = ags_soundcard_helper_config_get_format(config);

  /* device */
  if(use_alsa){
    devout->out.alsa.handle = NULL;
    devout->out.alsa.device = AGS_DEVOUT_DEFAULT_ALSA_DEVICE;
  }else{
    devout->out.oss.device_fd = -1;
    devout->out.oss.device = AGS_DEVOUT_DEFAULT_OSS_DEVICE;
  }

  /* buffer */
  devout->buffer_mutex = (GRecMutex **) malloc(4 * sizeof(GRecMutex *));

  for(i = 0; i < 4; i++){
    devout->buffer_mutex[i] = (GRecMutex *) malloc(sizeof(GRecMutex));

    g_rec_mutex_init(devout->buffer_mutex[i]);
  }
  
  devout->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  devout->sub_block_mutex = (GRecMutex **) malloc(4 * devout->sub_block_count * devout->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < 4 * devout->sub_block_count * devout->pcm_channels; i++){
    devout->sub_block_mutex[i] = (GRecMutex *) malloc(sizeof(GRecMutex));

    g_rec_mutex_init(devout->sub_block_mutex[i]);
  }

  devout->buffer = (void **) malloc(4 * sizeof(void *));

  devout->buffer[0] = NULL;
  devout->buffer[1] = NULL;
  devout->buffer[2] = NULL;
  devout->buffer[3] = NULL;

  g_atomic_int_set(&(devout->available),
		   TRUE);
  
  devout->ring_buffer_size = AGS_DEVOUT_DEFAULT_RING_BUFFER_SIZE;
  devout->nth_ring_buffer = 0;
  
  devout->ring_buffer = NULL;

  ags_devout_realloc_buffer(devout);
  
  /* bpm */
  devout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  devout->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    devout->delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }

  /* delay and attack */
  devout->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
				     sizeof(gdouble));
  
  devout->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
				    sizeof(guint));

  ags_devout_adjust_delay_and_attack(devout);
  
  /* counters */
  devout->tact_counter = 0.0;
  devout->delay_counter = 0;
  devout->tic_counter = 0;

  devout->start_note_offset = 0;
  devout->note_offset = 0;
  devout->note_offset_absolute = 0;

  devout->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  devout->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  devout->do_loop = FALSE;

  devout->loop_offset = 0;

  devout->io_channel = NULL;
  devout->tag = NULL;
}

void
ags_devout_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsDevout *devout;

  GRecMutex *devout_mutex;

  devout = AGS_DEVOUT(gobject);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      g_rec_mutex_lock(devout_mutex);

      if((AGS_DEVOUT_OSS & (devout->flags)) != 0){
	devout->out.oss.device = g_strdup(device);
      }else if((AGS_DEVOUT_ALSA & (devout->flags)) != 0){
	devout->out.alsa.device = g_strdup(device);
      }

      g_rec_mutex_unlock(devout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      g_rec_mutex_lock(devout_mutex);

      if(dsp_channels == devout->dsp_channels){
	g_rec_mutex_unlock(devout_mutex);
	
	return;
      }

      devout->dsp_channels = dsp_channels;

      g_rec_mutex_unlock(devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels, old_pcm_channels;
      guint i;
  
      pcm_channels = g_value_get_uint(value);

      g_rec_mutex_lock(devout_mutex);

      if(pcm_channels == devout->pcm_channels){
	g_rec_mutex_unlock(devout_mutex);
	
	return;
      }

      old_pcm_channels = devout->pcm_channels;

      /* destroy if less pcm-channels */
      for(i = 4 * devout->sub_block_count * pcm_channels; i < 4 * devout->sub_block_count * old_pcm_channels; i++){
	g_rec_mutex_clear(devout->sub_block_mutex[i]);

	free(devout->sub_block_mutex[i]);
      }

      devout->sub_block_mutex = (GRecMutex **) realloc(devout->sub_block_mutex,
							     4 * devout->sub_block_count * pcm_channels * sizeof(GRecMutex *));

      /* create if more pcm-channels */
      for(i = 4 * devout->sub_block_count * old_pcm_channels; i < 4 * devout->sub_block_count * pcm_channels; i++){
	devout->sub_block_mutex[i] = (GRecMutex *) malloc(sizeof(GRecMutex));

	g_rec_mutex_init(devout->sub_block_mutex[i]);
      }
      
      devout->pcm_channels = pcm_channels;

      g_rec_mutex_unlock(devout_mutex);
      
      ags_devout_realloc_buffer(devout);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(devout_mutex);

      if(format == devout->format){
	g_rec_mutex_unlock(devout_mutex);
	
	return;
      }

      devout->format = format;

      g_rec_mutex_unlock(devout_mutex);

      ags_devout_realloc_buffer(devout);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      g_rec_mutex_lock(devout_mutex);

      if(buffer_size == devout->buffer_size){
	g_rec_mutex_unlock(devout_mutex);

	return;
      }

      devout->buffer_size = buffer_size;

      g_rec_mutex_unlock(devout_mutex);

      ags_devout_realloc_buffer(devout);
      ags_devout_adjust_delay_and_attack(devout);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      g_rec_mutex_lock(devout_mutex);

      if(samplerate == devout->samplerate){
	g_rec_mutex_unlock(devout_mutex);

	return;
      }

      devout->samplerate = samplerate;

      g_rec_mutex_unlock(devout_mutex);

      ags_devout_adjust_delay_and_attack(devout);
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

      g_rec_mutex_lock(devout_mutex);

      if(bpm == devout->bpm){
	g_rec_mutex_unlock(devout_mutex);

	return;
      }

      devout->bpm = bpm;

      g_rec_mutex_unlock(devout_mutex);

      ags_devout_adjust_delay_and_attack(devout);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      g_rec_mutex_lock(devout_mutex);

      if(delay_factor == devout->delay_factor){
	g_rec_mutex_unlock(devout_mutex);

	return;
      }

      devout->delay_factor = delay_factor;

      g_rec_mutex_unlock(devout_mutex);

      ags_devout_adjust_delay_and_attack(devout);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_devout_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsDevout *devout;

  GRecMutex *devout_mutex;

  devout = AGS_DEVOUT(gobject);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(devout_mutex);

      if((AGS_DEVOUT_OSS & (devout->flags)) != 0){
	g_value_set_string(value, devout->out.oss.device);
      }else if((AGS_DEVOUT_ALSA & (devout->flags)) != 0){
	g_value_set_string(value, devout->out.alsa.device);
      }

      g_rec_mutex_unlock(devout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_rec_mutex_lock(devout_mutex);

      g_value_set_uint(value, devout->dsp_channels);

      g_rec_mutex_unlock(devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_rec_mutex_lock(devout_mutex);

      g_value_set_uint(value, devout->pcm_channels);

      g_rec_mutex_unlock(devout_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(devout_mutex);

      g_value_set_uint(value, devout->format);

      g_rec_mutex_unlock(devout_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(devout_mutex);

      g_value_set_uint(value, devout->buffer_size);

      g_rec_mutex_unlock(devout_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(devout_mutex);

      g_value_set_uint(value, devout->samplerate);

      g_rec_mutex_unlock(devout_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(devout_mutex);

      g_value_set_pointer(value, devout->buffer);

      g_rec_mutex_unlock(devout_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(devout_mutex);

      g_value_set_double(value, devout->bpm);

      g_rec_mutex_unlock(devout_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_rec_mutex_lock(devout_mutex);

      g_value_set_double(value, devout->delay_factor);

      g_rec_mutex_unlock(devout_mutex);
    }
    break;
  case PROP_ATTACK:
    {
      g_rec_mutex_lock(devout_mutex);

      g_value_set_pointer(value, devout->attack);

      g_rec_mutex_unlock(devout_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_devout_dispose(GObject *gobject)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_devout_parent_class)->dispose(gobject);
}

void
ags_devout_finalize(GObject *gobject)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(gobject);

  ags_uuid_free(devout->uuid);
  
  /* free output buffer */
  free(devout->buffer[0]);
  free(devout->buffer[1]);
  free(devout->buffer[2]);
  free(devout->buffer[3]);

  /* free buffer array */
  free(devout->buffer);

  /* free AgsAttack */
  free(devout->attack);
  
  /* call parent */
  G_OBJECT_CLASS(ags_devout_parent_class)->finalize(gobject);
}

AgsUUID*
ags_devout_get_uuid(AgsConnectable *connectable)
{
  AgsDevout *devout;
  
  AgsUUID *ptr;

  GRecMutex *devout_mutex;

  devout = AGS_DEVOUT(connectable);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* get UUID */
  g_rec_mutex_lock(devout_mutex);

  ptr = devout->uuid;

  g_rec_mutex_unlock(devout_mutex);
  
  return(ptr);
}

gboolean
ags_devout_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_devout_is_ready(AgsConnectable *connectable)
{
  AgsDevout *devout;
  
  gboolean is_ready;

  devout = AGS_DEVOUT(connectable);

  /* check is added */
  is_ready = ags_devout_test_flags(devout, AGS_DEVOUT_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_devout_add_to_registry(AgsConnectable *connectable)
{
  AgsDevout *devout;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  devout = AGS_DEVOUT(connectable);

  ags_devout_set_flags(devout, AGS_DEVOUT_ADDED_TO_REGISTRY);
}

void
ags_devout_remove_from_registry(AgsConnectable *connectable)
{
  AgsDevout *devout;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  devout = AGS_DEVOUT(connectable);

  ags_devout_unset_flags(devout, AGS_DEVOUT_ADDED_TO_REGISTRY);
}

xmlNode*
ags_devout_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_devout_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_devout_xml_parse(AgsConnectable *connectable,
		      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_devout_is_connected(AgsConnectable *connectable)
{
  AgsDevout *devout;
  
  gboolean is_connected;

  devout = AGS_DEVOUT(connectable);

  /* check is connected */
  is_connected = ags_devout_test_flags(devout, AGS_DEVOUT_CONNECTED);
  
  return(is_connected);
}

void
ags_devout_connect(AgsConnectable *connectable)
{
  AgsDevout *devout;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  devout = AGS_DEVOUT(connectable);

  ags_devout_set_flags(devout, AGS_DEVOUT_CONNECTED);
}

void
ags_devout_disconnect(AgsConnectable *connectable)
{

  AgsDevout *devout;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  devout = AGS_DEVOUT(connectable);
  
  ags_devout_unset_flags(devout, AGS_DEVOUT_CONNECTED);
}

/**
 * ags_devout_test_flags:
 * @devout: the #AgsDevout
 * @flags: the flags
 *
 * Test @flags to be set on @devout.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_devout_test_flags(AgsDevout *devout, guint flags)
{
  gboolean retval;  
  
  GRecMutex *devout_mutex;

  if(!AGS_IS_DEVOUT(devout)){
    return(FALSE);
  }

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* test */
  g_rec_mutex_lock(devout_mutex);

  retval = (flags & (devout->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(devout_mutex);

  return(retval);
}

/**
 * ags_devout_set_flags:
 * @devout: the #AgsDevout
 * @flags: see #AgsDevoutFlags-enum
 *
 * Enable a feature of @devout.
 *
 * Since: 3.0.0
 */
void
ags_devout_set_flags(AgsDevout *devout, guint flags)
{
  GRecMutex *devout_mutex;

  if(!AGS_IS_DEVOUT(devout)){
    return;
  }

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(devout_mutex);

  devout->flags |= flags;
  
  g_rec_mutex_unlock(devout_mutex);
}
    
/**
 * ags_devout_unset_flags:
 * @devout: the #AgsDevout
 * @flags: see #AgsDevoutFlags-enum
 *
 * Disable a feature of @devout.
 *
 * Since: 3.0.0
 */
void
ags_devout_unset_flags(AgsDevout *devout, guint flags)
{  
  GRecMutex *devout_mutex;

  if(!AGS_IS_DEVOUT(devout)){
    return;
  }

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(devout_mutex);

  devout->flags &= (~flags);
  
  g_rec_mutex_unlock(devout_mutex);
}

void
ags_devout_set_device(AgsSoundcard *soundcard,
		      gchar *device)
{
  AgsDevout *devout;

  GList *card_id, *card_id_start, *card_name, *card_name_start;
  
  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* list cards */
  card_id = NULL;
  card_name = NULL;
  
  ags_soundcard_list_cards(soundcard,
			   &card_id, &card_name);

  card_id_start = card_id;
  card_name_start = card_name;

  /* check card */
  g_rec_mutex_lock(devout_mutex);

  while(card_id != NULL){
    if(!g_ascii_strncasecmp(card_id->data,
			    device,
			    strlen(card_id->data))){
      if((AGS_DEVOUT_ALSA & (devout->flags)) != 0){
	devout->out.alsa.device = g_strdup(device);
      }else if((AGS_DEVOUT_OSS & (devout->flags)) != 0){
	devout->out.oss.device = g_strdup(device);
      }

      break;
    }
    
    card_id = card_id->next;
  }

  g_rec_mutex_unlock(devout_mutex);

  /* free card id and name */
  g_list_free_full(card_id_start,
		   g_free);
  g_list_free_full(card_name_start,
		   g_free);
}

gchar*
ags_devout_get_device(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  gchar *device;

  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);
  
  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  device = NULL;
  
  g_rec_mutex_lock(devout_mutex);

  if((AGS_DEVOUT_ALSA & (devout->flags)) != 0){
#ifdef AGS_WITH_ALSA
    device = g_strdup(devout->out.alsa.device);
#endif
  }else if((AGS_DEVOUT_OSS & (devout->flags)) != 0){
#ifdef AGS_WITH_OSS
    device = g_strdup(devout->out.oss.device);
#endif
  }

  g_rec_mutex_unlock(devout_mutex);

  return(device);
}

void
ags_devout_set_presets(AgsSoundcard *soundcard,
		       guint channels,
		       guint samplerate,
		       guint buffer_size,
		       guint format)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);

  g_object_set(devout,
	       "pcm-channels", channels,
	       "samplerate", samplerate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_devout_get_presets(AgsSoundcard *soundcard,
		       guint *channels,
		       guint *samplerate,
		       guint *buffer_size,
		       guint *format)
{
  AgsDevout *devout;

  GRecMutex *devout_mutex;

  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* get presets */
  g_rec_mutex_lock(devout_mutex);

  if(channels != NULL){
    *channels = devout->pcm_channels;
  }

  if(samplerate != NULL){
    *samplerate = devout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = devout->buffer_size;
  }

  if(format != NULL){
    *format = devout->format;
  }

  g_rec_mutex_unlock(devout_mutex);
}

void
ags_devout_list_cards(AgsSoundcard *soundcard,
		      GList **card_id, GList **card_name)
{
  AgsDevout *devout;

  GRecMutex *devout_mutex;

  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  g_rec_mutex_lock(devout_mutex);

  if((AGS_DEVOUT_ALSA & (devout->flags)) != 0){
#ifdef AGS_WITH_ALSA
    snd_ctl_t *card_handle;
    snd_ctl_card_info_t *card_info;

    char *name;
    gchar *str;
    gchar *str_err;
  
    int card_num;
    int device;
    int error;

    /* the default device */
    str = g_strdup("default");
    error = snd_ctl_open(&card_handle, str, 0);

    if(error < 0){
      g_free(str);
      
      goto ags_devout_list_cards_NO_DEFAULT_0;
    }

    snd_ctl_card_info_alloca(&card_info);
    error = snd_ctl_card_info(card_handle, card_info);

    if(error < 0){
      g_free(str);

      goto ags_devout_list_cards_NO_DEFAULT_0;
    }

    if(card_id != NULL){
      *card_id = g_list_prepend(*card_id, str);
    }

    if(card_name != NULL){
      *card_name = g_list_prepend(*card_name, g_strdup(snd_ctl_card_info_get_name(card_info)));
    }
    
    snd_ctl_close(card_handle);

  ags_devout_list_cards_NO_DEFAULT_0:
    
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
	    
	    *card_id = g_list_prepend(*card_id,
				      g_strdup(hint));

	    if(hint != NULL){
	      free(hint);
	    }
	  }

	  if(card_name != NULL){
	    char *name;

	    name = snd_ctl_card_info_get_name(card_info);
	    
	    *card_name = g_list_prepend(*card_name, g_strdup(name));
	  }
	}

	snd_device_name_free_hint(hints);
      }
      
      snd_ctl_close(card_handle);

      g_free(str);
    }

    snd_config_update_free_global();
#endif
  }else{
#ifdef AGS_WITH_OSS
    oss_sysinfo sysinfo;
    oss_audioinfo ai;

    char *mixer_device;
    
    int mixerfd = -1;

    int next, n;
    int i;

    if((mixer_device = getenv("OSS_MIXERDEV")) == NULL){
      mixer_device = "/dev/mixer";
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

      next = ai.next_play_engine;
      
      if(next <= 0){
	break;
      }
    }
#endif
  }

  g_rec_mutex_unlock(devout_mutex);

  if(card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

void
ags_devout_pcm_info(AgsSoundcard *soundcard,
		    char *card_id,
		    guint *channels_min, guint *channels_max,
		    guint *rate_min, guint *rate_max,
		    guint *buffer_size_min, guint *buffer_size_max,
		    GError **error)
{
  AgsDevout *devout;

  GRecMutex *devout_mutex;

  if(card_id == NULL){
    return;
  }
  
  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* pcm info */
  g_rec_mutex_lock(devout_mutex);

  if((AGS_DEVOUT_ALSA & (devout->flags)) != 0){
#ifdef AGS_WITH_ALSA
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    
    gchar *str;
    
    unsigned int val;
    int dir;
    snd_pcm_uframes_t frames;

    int rc;
    int err;

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
			AGS_DEVOUT_ERROR,
			AGS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
			"unable to open pcm device: %s\n",
			str);
	  }
	  
	  //    free(str);
	  
	  goto ags_devout_pcm_info_ERR;
	}
      }else{
	if(error != NULL){
	  g_set_error(error,
		      AGS_DEVOUT_ERROR,
		      AGS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		      "unable to open pcm device: %s\n",
		      str);
	}
	
	goto ags_devout_pcm_info_ERR;
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
#endif
  }else{
#ifdef AGS_WITH_OSS
    oss_audioinfo ainfo;

    gchar *str;
    
    int mixerfd;
    int acc;
    unsigned int cmd;
    
    mixerfd = open(card_id, O_RDWR, 0);

    if(mixerfd == -1){
      int e = errno;
      
      str = strerror(e);
      g_message("unable to open pcm device: %s\n", str);

      if(error != NULL){
	g_set_error(error,
		    AGS_DEVOUT_ERROR,
		    AGS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		    "unable to open pcm device: %s\n",
		    str);
      }
    
      goto ags_devout_pcm_info_ERR;      
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
      goto ags_devout_pcm_info_ERR;
    }
    
    if(ioctl(mixerfd, cmd, &ainfo) == -1){
      int e = errno;

      str = strerror(e);
      g_message("unable to retrieve audio info: %s\n", str);

      if(error != NULL){
	g_set_error(error,
		    AGS_DEVOUT_ERROR,
		    AGS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		    "unable to retrieve audio info: %s\n",
		    str);
      }
    
      return;
    }
  
    *channels_min = ainfo.min_channels;
    *channels_max = ainfo.max_channels;
    *rate_min = ainfo.min_rate;
    *rate_max = ainfo.max_rate;
    *buffer_size_min = 64;
    *buffer_size_max = 8192;
#endif
  }

 ags_devout_pcm_info_ERR:

  g_rec_mutex_unlock(devout_mutex);
}

guint
ags_devout_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_PLAYBACK);
}

gboolean
ags_devout_is_available(AgsSoundcard *soundcard)
{
  AgsDevout *devout;
  
#ifdef AGS_WITH_ALSA
  snd_pcm_t *handle;

  struct pollfd fds;
#endif

  gboolean is_available;

  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);
  
  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);  

#ifdef AGS_WITH_ALSA
  /* check is starting */
  g_rec_mutex_lock(devout_mutex);

  handle = devout->out.alsa.handle;
  
  g_rec_mutex_unlock(devout_mutex);

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
ags_devout_is_starting(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  gboolean is_starting;
  
  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* check is starting */
  g_rec_mutex_lock(devout_mutex);

  is_starting = ((AGS_DEVOUT_START_PLAY & (devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(devout_mutex);
  
  return(is_starting);
}

gboolean
ags_devout_is_playing(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  gboolean is_playing;
  
  GRecMutex *devout_mutex;

  devout = AGS_DEVOUT(soundcard);
  
  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* check is starting */
  g_rec_mutex_lock(devout_mutex);

  is_playing = ((AGS_DEVOUT_PLAY & (devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(devout_mutex);

  return(is_playing);
}

gchar*
ags_devout_get_uptime(AgsSoundcard *soundcard)
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
ags_devout_delegate_play_init(AgsSoundcard *soundcard,
			      GError **error)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);

  if(ags_devout_test_flags(devout, AGS_DEVOUT_ALSA)){
    ags_devout_alsa_init(soundcard,
			 error);
  }else if(ags_devout_test_flags(devout, AGS_DEVOUT_OSS)){
    ags_devout_oss_init(soundcard,
			error);
  }
}

void
ags_devout_delegate_play(AgsSoundcard *soundcard,
			 GError **error)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);

  if(ags_devout_test_flags(devout, AGS_DEVOUT_ALSA)){
    ags_devout_alsa_play(soundcard,
			 error);
  }else if(ags_devout_test_flags(devout, AGS_DEVOUT_OSS)){
    ags_devout_oss_play(soundcard,
			error);
  }
}

void
ags_devout_delegate_stop(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  devout = AGS_DEVOUT(soundcard);

  if(ags_devout_test_flags(devout, AGS_DEVOUT_ALSA)){
    ags_devout_alsa_free(soundcard);
  }else if(ags_devout_test_flags(devout, AGS_DEVOUT_OSS)){
    ags_devout_oss_free(soundcard);
  }
}

void
ags_devout_oss_init(AgsSoundcard *soundcard,
		    GError **error)
{
  AgsDevout *devout;

  GIOChannel *io_channel;

  guint tag;

  gchar *str;

  guint word_size;
  int format;
  int tmp;
  guint i;

  GRecMutex *devout_mutex;

  if(ags_soundcard_is_playing(soundcard)){
    return;
  }

  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* retrieve word size */
  g_rec_mutex_lock(devout_mutex);

  switch(devout->format){
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
    g_warning("ags_devout_oss_init(): unsupported word size");
    return;
  }

  /* prepare for playback */
  devout->flags |= (AGS_DEVOUT_START_PLAY |
		    AGS_DEVOUT_PLAY |
		    AGS_DEVOUT_NONBLOCKING);

  memset(devout->buffer[0], 0, devout->pcm_channels * devout->buffer_size * word_size);
  memset(devout->buffer[1], 0, devout->pcm_channels * devout->buffer_size * word_size);
  memset(devout->buffer[2], 0, devout->pcm_channels * devout->buffer_size * word_size);
  memset(devout->buffer[3], 0, devout->pcm_channels * devout->buffer_size * word_size);

  /* allocate ring buffer */
  g_atomic_int_set(&(devout->available),
		   FALSE);
  
    devout->ring_buffer = (unsigned char **) malloc(devout->ring_buffer_size * sizeof(unsigned char *));

  for(i = 0; i < devout->ring_buffer_size; i++){
    devout->ring_buffer[i] = (unsigned char *) malloc(devout->pcm_channels *
						      devout->buffer_size * word_size *
						      sizeof(unsigned char));
  }

#ifdef AGS_WITH_OSS
  /* open device fd */
  str = devout->out.oss.device;
  devout->out.oss.device_fd = open(str, O_WRONLY, 0);

  if(devout->out.oss.device_fd == -1){
    devout->flags &= (~(AGS_DEVOUT_START_PLAY |
			AGS_DEVOUT_PLAY |
			AGS_DEVOUT_NONBLOCKING));

    g_rec_mutex_unlock(devout_mutex);

    g_warning("couldn't open device %s", devout->out.oss.device);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVOUT_ERROR,
		  AGS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		  "unable to open dsp device: %s\n",
		  str);
    }

    return;
  }

  //NOTE:JK: unsupported on kfreebsd 9.0
  //  tmp = APF_NORMAL;
  //  ioctl(devout->out.oss.device_fd, SNDCTL_DSP_PROFILE, &tmp);

  tmp = format;

  if(ioctl(devout->out.oss.device_fd, SNDCTL_DSP_SETFMT, &tmp) == -1){
    devout->flags &= (~(AGS_DEVOUT_START_PLAY |
			AGS_DEVOUT_PLAY |
			AGS_DEVOUT_NONBLOCKING));

    g_rec_mutex_unlock(devout_mutex);

    str = strerror(errno);
    g_warning("failed to select bits/sample");

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVOUT_ERROR,
		  AGS_DEVOUT_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
		  "unable to open dsp device: %s",
		  str);
    }

    devout->out.oss.device_fd = -1;

    return;
  }
  
  if(tmp != format){
    devout->flags &= (~(AGS_DEVOUT_START_PLAY |
			AGS_DEVOUT_PLAY |
			AGS_DEVOUT_NONBLOCKING));

    g_rec_mutex_unlock(devout_mutex);

    str = strerror(errno);
    g_warning("failed to select bits/sample");

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVOUT_ERROR,
		  AGS_DEVOUT_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
		  "unable to open dsp device: %s",
		  str);
    }

    devout->out.oss.device_fd = -1;

    return;
  }

  tmp = devout->dsp_channels;

  if(ioctl(devout->out.oss.device_fd, SNDCTL_DSP_CHANNELS, &tmp) == -1){
    devout->flags &= (~(AGS_DEVOUT_START_PLAY |
			AGS_DEVOUT_PLAY |
			AGS_DEVOUT_NONBLOCKING));

    g_rec_mutex_unlock(devout_mutex);

    str = strerror(errno);
    g_warning("Channels count (%i) not available for playbacks: %s", devout->dsp_channels, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVOUT_ERROR,
		  AGS_DEVOUT_ERROR_CHANNELS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devout->out.oss.device_fd = -1;
    
    return;
  }

  if(tmp != devout->dsp_channels){
    devout->flags &= (~(AGS_DEVOUT_START_PLAY |
			AGS_DEVOUT_PLAY |
			AGS_DEVOUT_NONBLOCKING));

    g_rec_mutex_unlock(devout_mutex);

    str = strerror(errno);
    g_warning("Channels count (%i) not available for playbacks: %s", devout->dsp_channels, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVOUT_ERROR,
		  AGS_DEVOUT_ERROR_CHANNELS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }
    
    devout->out.oss.device_fd = -1;
    
    return;
  }

  tmp = devout->samplerate;

  if(ioctl(devout->out.oss.device_fd, SNDCTL_DSP_SPEED, &tmp) == -1){
    devout->flags &= (~(AGS_DEVOUT_START_PLAY |
			AGS_DEVOUT_PLAY |
			AGS_DEVOUT_NONBLOCKING));

    g_rec_mutex_unlock(devout_mutex);

    str = strerror(errno);
    g_warning("Rate %iHz not available for playback: %s", devout->samplerate, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVOUT_ERROR,
		  AGS_DEVOUT_ERROR_SAMPLERATE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devout->out.oss.device_fd = -1;
    
    return;
  }

  if(tmp != devout->samplerate){
    g_warning("Warning: Playback using %d Hz (file %d Hz)",
	      tmp,
	      devout->samplerate);
  }

  io_channel = g_io_channel_unix_new(devout->out.oss.device_fd);
  tag = g_io_add_watch(io_channel,
		       G_IO_OUT,
		       (GIOFunc) ags_devout_oss_io_func,
		       devout);
      
  devout->io_channel = g_list_prepend(devout->io_channel,
				      io_channel);
  devout->tag = g_list_prepend(devout->tag,
			       GUINT_TO_POINTER(tag));

#endif
  
  devout->tact_counter = 0.0;
  devout->delay_counter = floor(ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(devout)));
  devout->tic_counter = 0;

  devout->nth_ring_buffer = 0;
  
#ifdef AGS_WITH_OSS
  devout->flags |= AGS_DEVOUT_INITIALIZED;
#endif
  devout->flags |= AGS_DEVOUT_BUFFER0;
  devout->flags &= (~(AGS_DEVOUT_BUFFER1 |
		      AGS_DEVOUT_BUFFER2 |
		      AGS_DEVOUT_BUFFER3));
  
  g_rec_mutex_unlock(devout_mutex);
}

void
ags_devout_oss_play_fill_ring_buffer(void *buffer,
				     guint ags_format,
				     unsigned char *ring_buffer,
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
    g_warning("ags_devout_oss_play(): unsupported word size");
    return;
  }

  /* fill the channel areas */
  for(count = 0; count < buffer_size; count++){
    for(chn = 0; chn < channels; chn++){
      res = 0;
	
      switch(ags_format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	res = (int) ((gint8 *) buffer)[count * channels + chn];
      }
      break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	res = (int) ((gint16 *) buffer)[count * channels + chn];
      }
      break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	res = (int) ((gint32 *) buffer)[count * channels + chn];
      }
      break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	res = (int) ((gint32 *) buffer)[count * channels + chn];
      }
      break;
      }
	
      /* Generate data in native endian format */
      if(ags_endian_host_is_be()){
	for(i = 0; i < bps; i++){
	  *(ring_buffer + chn * bps + word_size - 1 - i) = (res >> i * 8) & 0xff;
	}
      }else{
	for(i = 0; i < bps; i++){
	  *(ring_buffer + chn * bps + i) = (res >>  i * 8) & 0xff;
	}
      }	
    }

    ring_buffer += channels * bps;
  }
}

gboolean
ags_devout_oss_io_func(GIOChannel *source,
		       GIOCondition condition,
		       AgsDevout *devout)
{
  g_atomic_int_set(&(devout->available), TRUE);

  return(TRUE);
}

void
ags_devout_oss_play(AgsSoundcard *soundcard,
		    GError **error)
{
  AgsDevout *devout;

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

  int n_write;
  
  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);

  application_context = ags_application_context_get_instance();

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* lock */
  g_rec_mutex_lock(devout_mutex);
  
  /* retrieve word size */
  switch(devout->format){
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
    g_warning("ags_devout_oss_play(): unsupported word size");
    return;
  }

  /* do playback */
  devout->flags &= (~AGS_DEVOUT_START_PLAY);

  if((AGS_DEVOUT_INITIALIZED & (devout->flags)) == 0){
    g_rec_mutex_unlock(devout_mutex);
    
    return;
  }

  /* check buffer flag */
  nth_buffer = 0;
  
  if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
    nth_buffer = 0;
  }else if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
    nth_buffer = 1;
  }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
    nth_buffer = 2;
  }else if((AGS_DEVOUT_BUFFER3 & (devout->flags)) != 0){
    nth_buffer = 3;
  }

#ifdef AGS_WITH_OSS    
  /* fill ring buffer */
  ags_soundcard_lock_buffer(soundcard,
			    devout->buffer[nth_buffer]);

  ags_devout_oss_play_fill_ring_buffer(devout->buffer[nth_buffer],
				       devout->format,
				       devout->ring_buffer[devout->nth_ring_buffer],
				       devout->pcm_channels,
				       devout->buffer_size);

  ags_soundcard_unlock_buffer(soundcard,
			      devout->buffer[nth_buffer]);

  /* wait until available */
  poll_timeout = g_get_monotonic_time() + (G_USEC_PER_SEC * (1.0 / (gdouble) devout->samplerate * (gdouble) devout->buffer_size));

  g_rec_mutex_unlock(devout_mutex);

  //TODO:JK: implement me
  
  while(!ags_soundcard_is_available(AGS_SOUNDCARD(devout))){
    g_usleep(1);

    if(g_get_monotonic_time() > poll_timeout){
      break;
    }
  }

  g_atomic_int_set(&(devout->available),
		   FALSE);
  
  g_rec_mutex_lock(devout_mutex);

  /* write ring buffer */
  n_write = write(devout->out.oss.device_fd,
		  devout->ring_buffer[devout->nth_ring_buffer],
		  devout->pcm_channels * devout->buffer_size * word_size * sizeof (char));
  
  if(n_write != devout->pcm_channels * devout->buffer_size * word_size * sizeof (char)){
    g_critical("write() return doesn't match written bytes");
  }
#endif

  /* increment nth ring-buffer */
  if(devout->nth_ring_buffer + 1 >= devout->ring_buffer_size){
    devout->nth_ring_buffer = 0;
  }else{
    devout->nth_ring_buffer += 1;
  }
  
  g_rec_mutex_unlock(devout_mutex);

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;
  
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) devout);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) devout);
  task = g_list_append(task,
		       clear_buffer);
  
  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) devout);
  task = g_list_append(task,
		       clear_buffer);

  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) devout);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);
  
  /* unref */
  g_object_unref(task_launcher);
}

void
ags_devout_oss_free(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  guint i;
  
  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /*  */
  g_rec_mutex_lock(devout_mutex);

  if((AGS_DEVOUT_INITIALIZED & (devout->flags)) == 0){
    g_rec_mutex_unlock(devout_mutex);
    
    return;
  }
  
  close(devout->out.oss.device_fd);
  devout->out.oss.device_fd = -1;

  /* free ring-buffer */
  g_atomic_int_set(&(devout->available), TRUE);

  if(devout->ring_buffer != NULL){
    for(i = 0; i < devout->ring_buffer_size; i++){
      free(devout->ring_buffer[i]);
    }
    
    free(devout->ring_buffer);
  }
  
  devout->ring_buffer = NULL;

  /* reset flags */
  devout->flags &= (~(AGS_DEVOUT_BUFFER0 |
		      AGS_DEVOUT_BUFFER1 |
		      AGS_DEVOUT_BUFFER2 |
		      AGS_DEVOUT_BUFFER3 |
		      AGS_DEVOUT_PLAY |
		      AGS_DEVOUT_INITIALIZED));

  devout->note_offset = devout->start_note_offset;
  devout->note_offset_absolute = devout->start_note_offset;

  g_rec_mutex_unlock(devout_mutex);
}

gboolean
ags_devout_alsa_io_func(GIOChannel *source,
			GIOCondition condition,
			AgsDevout *devout)
{
  g_atomic_int_set(&(devout->available), TRUE);

  return(TRUE);
}

void
ags_devout_alsa_init(AgsSoundcard *soundcard,
		     GError **error)
{
  AgsDevout *devout;

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
  
  GRecMutex *devout_mutex; 
 
  static unsigned int period_time = 100000;
  static unsigned int buffer_time = 100000;

  if(ags_soundcard_is_playing(soundcard)){
    return;
  }
  
  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* retrieve word size */
  g_rec_mutex_lock(devout_mutex);

  if(devout->out.alsa.device == NULL){
    g_rec_mutex_unlock(devout_mutex);
    
    return;
  }

#ifdef AGS_WITH_ALSA
  format = SND_PCM_FORMAT_S16;
#endif
  
  switch(devout->format){
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
    g_rec_mutex_unlock(devout_mutex);

    g_warning("ags_devout_alsa_init(): unsupported word size");

    return;
  }

  /* prepare for playback */
  devout->flags |= (AGS_DEVOUT_START_PLAY |
		    AGS_DEVOUT_PLAY |
		    AGS_DEVOUT_NONBLOCKING);

  memset(devout->buffer[0], 0, devout->pcm_channels * devout->buffer_size * word_size);
  memset(devout->buffer[1], 0, devout->pcm_channels * devout->buffer_size * word_size);
  memset(devout->buffer[2], 0, devout->pcm_channels * devout->buffer_size * word_size);
  memset(devout->buffer[3], 0, devout->pcm_channels * devout->buffer_size * word_size);

  /* allocate ring buffer */
#ifdef AGS_WITH_ALSA
  devout->ring_buffer = (unsigned char **) malloc(devout->ring_buffer_size * sizeof(unsigned char *));

  for(i = 0; i < devout->ring_buffer_size; i++){
    devout->ring_buffer[i] = (unsigned char *) malloc(devout->pcm_channels *
						      devout->buffer_size * (snd_pcm_format_physical_width(format) / 8) *
						      sizeof(unsigned char));
  }
 
  /*  */
  period_event = 0;
  
  /* Open PCM device for playback. */  
  handle = NULL;

  if((err = snd_pcm_open(&handle, devout->out.alsa.device, SND_PCM_STREAM_PLAYBACK, 0)) < 0){
    gchar *device_fixup;
    
    str = snd_strerror(err);
    g_warning("Playback open error (attempting fixup): %s", str);
    
    device_fixup = g_strdup_printf("%s,0",
				   devout->out.alsa.device);

    handle = NULL;
    
    if((err = snd_pcm_open(&handle, device_fixup, SND_PCM_STREAM_PLAYBACK, 0)) < 0){
      devout->flags &= (~(AGS_DEVOUT_START_PLAY |
			  AGS_DEVOUT_PLAY |
			  AGS_DEVOUT_NONBLOCKING));
      
      g_rec_mutex_unlock(devout_mutex);
      
      if(error != NULL){
	g_set_error(error,
		    AGS_DEVOUT_ERROR,
		    AGS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
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

  if (err < 0) {
    devout->flags &= (~(AGS_DEVOUT_START_PLAY |
			AGS_DEVOUT_PLAY |
			AGS_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(devout_mutex);

    str = snd_strerror(err);
    g_warning("Broken configuration for playback: no configurations available: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVOUT_ERROR,
		  AGS_DEVOUT_ERROR_BROKEN_CONFIGURATION,
		  "unable to open pcm device: %s",
		  str);
    }

    devout->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set hardware resampling * /
     err = snd_pcm_hw_params_set_rate_resample(handle, hwparams, 0);
     if (err < 0) {
     g_rec_mutex_unlock(devout_mutex);

     str = snd_strerror(err);
     g_warning("Resampling setup failed for playback: %s\n", str);

     //    free(str);
    
     return;
     }
  */
  
  /* set the interleaved read/write format */
  err = snd_pcm_hw_params_set_access(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
  if (err < 0) {
    devout->flags &= (~(AGS_DEVOUT_START_PLAY |
			AGS_DEVOUT_PLAY |
			AGS_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(devout_mutex);

    str = snd_strerror(err);
    g_warning("Access type not available for playback: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVOUT_ERROR,
		  AGS_DEVOUT_ERROR_ACCESS_TYPE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devout->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the sample format */
  err = snd_pcm_hw_params_set_format(handle, hwparams, format);
  if (err < 0) {
    devout->flags &= (~(AGS_DEVOUT_START_PLAY |
			AGS_DEVOUT_PLAY |
			AGS_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(devout_mutex);

    str = snd_strerror(err);
    g_warning("Sample format not available for playback: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVOUT_ERROR,
		  AGS_DEVOUT_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devout->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the count of channels */
  channels = devout->pcm_channels;
  err = snd_pcm_hw_params_set_channels(handle, hwparams, channels);
  if (err < 0) {
    devout->flags &= (~(AGS_DEVOUT_START_PLAY |
			AGS_DEVOUT_PLAY |
			AGS_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(devout_mutex);

    str = snd_strerror(err);
    g_warning("Channels count (%i) not available for playbacks: %s", channels, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVOUT_ERROR,
		  AGS_DEVOUT_ERROR_CHANNELS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devout->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the stream rate */
  rate = devout->samplerate;
  rrate = rate;
  err = snd_pcm_hw_params_set_rate_near(handle, hwparams, &rrate, 0);
  if (err < 0) {
    devout->flags &= (~(AGS_DEVOUT_START_PLAY |
			AGS_DEVOUT_PLAY |
			AGS_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(devout_mutex);

    str = snd_strerror(err);
    g_warning("Rate %iHz not available for playback: %s", rate, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVOUT_ERROR,
		  AGS_DEVOUT_ERROR_SAMPLERATE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devout->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  if (rrate != rate) {
    devout->flags &= (~(AGS_DEVOUT_START_PLAY |
			AGS_DEVOUT_PLAY |
			AGS_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(devout_mutex);
    
    g_warning("Rate doesn't match (requested %iHz, get %iHz)", rate, err);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVOUT_ERROR,
		  AGS_DEVOUT_ERROR_SAMPLERATE_NOT_AVAILABLE,
		  "unable to open pcm device");
    }

    devout->out.alsa.handle = NULL;
    
    return;
  }

  /* set the buffer size */
  size = 2 * devout->buffer_size;
  err = snd_pcm_hw_params_set_buffer_size(handle, hwparams, size);

  if (err < 0) {
    devout->flags &= (~(AGS_DEVOUT_START_PLAY |
			AGS_DEVOUT_PLAY |
			AGS_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(devout_mutex);

    str = snd_strerror(err);
    g_warning("Unable to set buffer size %lu for playback: %s", size, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVOUT_ERROR,
		  AGS_DEVOUT_ERROR_BUFFER_SIZE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devout->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the period size * /
     period_size = devout->buffer_size;
     err = snd_pcm_hw_params_set_period_size_near(handle, hwparams, period_size, dir);
     if (err < 0) {
     g_rec_mutex_unlock(devout_mutex);

     str = snd_strerror(err);
     g_warning("Unable to get period size for playback: %s\n", str);

     //    free(str);
    
     return;
     }
  */
  
  /* write the parameters to device */
  err = snd_pcm_hw_params(handle, hwparams);

  if (err < 0) {
    devout->flags &= (~(AGS_DEVOUT_START_PLAY |
			AGS_DEVOUT_PLAY |
			AGS_DEVOUT_NONBLOCKING));
    
    g_rec_mutex_unlock(devout_mutex);

    str = snd_strerror(err);
    g_warning("Unable to set hw params for playback: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVOUT_ERROR,
		  AGS_DEVOUT_ERROR_HW_PARAMETERS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devout->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* get the current swparams * /
     err = snd_pcm_sw_params_current(handle, swparams);
     if (err < 0) {
     g_rec_mutex_unlock(devout_mutex);

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
     g_rec_mutex_unlock(devout_mutex);

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
     g_rec_mutex_unlock(devout_mutex);

     str = snd_strerror(err);
     g_warning("Unable to set avail min for playback: %s\n", str);

     //    free(str);
    
     return;
     }

     /* write the parameters to the playback device * /
     err = snd_pcm_sw_params(handle, swparams);
     if (err < 0) {
     g_rec_mutex_unlock(devout_mutex);

     str = snd_strerror(err);
     g_warning("Unable to set sw params for playback: %s\n", str);

     //    free(str);
    
     return;
     }
  */

  /*  */
  devout->out.alsa.handle = handle;

#if 0
  i_stop = snd_pcm_poll_descriptors_count(devout->out.alsa.handle);

  if(i_stop > 0){
    struct pollfd *fds;
    
    fds = (struct pollfd *) malloc(i_stop * sizeof(struct pollfd));
    
    snd_pcm_poll_descriptors(devout->out.alsa.handle, fds, i_stop);
    
    for(i = 0; i < i_stop; i++){
      GIOChannel *io_channel;

      guint tag;

      io_channel = g_io_channel_unix_new(fds[i].fd);
      tag = g_io_add_watch(io_channel,
			   G_IO_OUT,
			   (GIOFunc) ags_devout_alsa_io_func,
			   devout);
      
      devout->io_channel = g_list_prepend(devout->io_channel,
					  io_channel);
      devout->tag = g_list_prepend(devout->tag,
				   GUINT_TO_POINTER(tag));
    }
  }
#endif
#endif

  devout->tact_counter = 0.0;
  devout->delay_counter = floor(ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(devout)));
  devout->tic_counter = 0;

  devout->nth_ring_buffer = 0;
    
#ifdef AGS_WITH_ALSA
  devout->flags |= AGS_DEVOUT_INITIALIZED;
#endif
  devout->flags |= AGS_DEVOUT_BUFFER0;
  devout->flags &= (~(AGS_DEVOUT_BUFFER1 |
		      AGS_DEVOUT_BUFFER2 |
		      AGS_DEVOUT_BUFFER3));
  
  g_rec_mutex_unlock(devout_mutex);
}

void
ags_devout_alsa_play_fill_ring_buffer(void *buffer,
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
    g_warning("ags_devout_alsa_play(): unsupported word size");
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
ags_devout_alsa_play(AgsSoundcard *soundcard,
		     GError **error)
{
  AgsDevout *devout;

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
  
  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);
  
  application_context = ags_application_context_get_instance();

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* lock */
  g_rec_mutex_lock(devout_mutex);
  
  /* retrieve word size */
  switch(devout->format){
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
    g_rec_mutex_unlock(devout_mutex);
    
    g_warning("ags_devout_alsa_play(): unsupported word size");

    return;
  }

  /* do playback */
  devout->flags &= (~AGS_DEVOUT_START_PLAY);

  if((AGS_DEVOUT_INITIALIZED & (devout->flags)) == 0){
    g_rec_mutex_unlock(devout_mutex);
    
    return;
  }

  //  g_message("play - 0x%0x", ((AGS_DEVOUT_BUFFER0 |
  //				AGS_DEVOUT_BUFFER1 |
  //				AGS_DEVOUT_BUFFER2 |
  //				AGS_DEVOUT_BUFFER3) & (devout->flags)));

  /* check buffer flag */
  nth_buffer = 0;
  
  if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
    nth_buffer = 0;
  }else if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
    nth_buffer = 1;
  }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
    nth_buffer = 2;
  }else if((AGS_DEVOUT_BUFFER3 & (devout->flags)) != 0){
    nth_buffer = 3;
  }

#ifdef AGS_WITH_ALSA

  /* fill ring buffer */
  ags_soundcard_lock_buffer(soundcard,
			    devout->buffer[nth_buffer]);
  
  ags_devout_alsa_play_fill_ring_buffer(devout->buffer[nth_buffer], devout->format,
					devout->ring_buffer[devout->nth_ring_buffer],
					devout->pcm_channels, devout->buffer_size);

  ags_soundcard_unlock_buffer(soundcard,
			      devout->buffer[nth_buffer]);

  /* wait until available */
  poll_timeout = g_get_monotonic_time() + (G_USEC_PER_SEC * (1.0 / (gdouble) devout->samplerate * (gdouble) devout->buffer_size));

  g_rec_mutex_unlock(devout_mutex);
  
  //TODO:JK: implement me
  while(!ags_soundcard_is_available(AGS_SOUNDCARD(devout))){
    g_usleep(1);

    if(g_get_monotonic_time() > poll_timeout){
      break;
    }
  }
  
  g_atomic_int_set(&(devout->available),
		   FALSE);
  
  g_rec_mutex_lock(devout_mutex);

  /* write ring buffer */
//  g_message("write %d", devout->buffer_size);
  
  devout->out.alsa.rc = snd_pcm_writei(devout->out.alsa.handle,
				       devout->ring_buffer[devout->nth_ring_buffer],
				       (snd_pcm_uframes_t) (devout->buffer_size));
  
  /* check error flag */
  if((AGS_DEVOUT_NONBLOCKING & (devout->flags)) == 0){
    if(devout->out.alsa.rc == -EPIPE){
      /* EPIPE means underrun */
      snd_pcm_prepare(devout->out.alsa.handle);

#ifdef AGS_DEBUG
      g_message("underrun occurred");
#endif
    }else if(devout->out.alsa.rc == -ESTRPIPE){
      static const struct timespec idle = {
	0,
	4000,
      };

      int err;

      while((err = snd_pcm_resume(devout->out.alsa.handle)) < 0){ // == -EAGAIN
	nanosleep(&idle, NULL); /* wait until the suspend flag is released */
      }
	
      if(err < 0){
	err = snd_pcm_prepare(devout->out.alsa.handle);
      }
    }else if(devout->out.alsa.rc < 0){
      str = snd_strerror(devout->out.alsa.rc);
      
      g_message("error from writei: %s", str);
    }else if(devout->out.alsa.rc != (int) devout->buffer_size) {
      g_message("short write, write %d frames", devout->out.alsa.rc);
    }
  }      
  
#endif

  /* increment nth ring-buffer */
  if(devout->nth_ring_buffer + 1 >= devout->ring_buffer_size){
    devout->nth_ring_buffer = 0;
  }else{
    devout->nth_ring_buffer += 1;
  }
  
  g_rec_mutex_unlock(devout_mutex);

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;
  
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) devout);
  
  task = g_list_prepend(task,
			tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) devout);

  task = g_list_prepend(task,
			clear_buffer);

  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) devout);

  task = g_list_prepend(task,
			switch_buffer_flag);

  /* append tasks */
  task = g_list_reverse(task);
  
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  g_list_free_full(task,
		   g_object_unref);
  
#ifdef AGS_WITH_ALSA
  snd_pcm_prepare(devout->out.alsa.handle);
#endif

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_devout_alsa_free(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  GList *start_list, *list;
  
  guint i;
  
  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* lock */
  g_rec_mutex_lock(devout_mutex);

  if((AGS_DEVOUT_INITIALIZED & (devout->flags)) == 0){
    g_rec_mutex_unlock(devout_mutex);
    
    return;
  }

  g_rec_mutex_unlock(devout_mutex);
  
#ifdef AGS_WITH_ALSA
  //  snd_pcm_drain(devout->out.alsa.handle);
  snd_pcm_close(devout->out.alsa.handle);
  devout->out.alsa.handle = NULL;
#endif

  /* free ring-buffer */
  g_rec_mutex_lock(devout_mutex);

  if(devout->ring_buffer != NULL){
    for(i = 0; i < devout->ring_buffer_size; i++){
      free(devout->ring_buffer[i]);
    }
    
    free(devout->ring_buffer);
  }
  
  devout->ring_buffer = NULL;

  /* reset flags */
  devout->flags &= (~(AGS_DEVOUT_BUFFER0 |
		      AGS_DEVOUT_BUFFER1 |
		      AGS_DEVOUT_BUFFER2 |
		      AGS_DEVOUT_BUFFER3 |
		      AGS_DEVOUT_PLAY |
		      AGS_DEVOUT_INITIALIZED));

  g_rec_mutex_unlock(devout_mutex);

  g_rec_mutex_lock(devout_mutex);

  devout->note_offset = devout->start_note_offset;
  devout->note_offset_absolute = devout->start_note_offset;

  list = devout->tag;

  while(list != NULL){
    g_source_remove(GPOINTER_TO_UINT(list->data));

    list = list->next;
  }

  g_list_free(devout->tag);
  
  devout->tag = NULL;

  g_list_free_full(devout->io_channel,
		   g_io_channel_unref);

  devout->io_channel = NULL;

  g_atomic_int_set(&(devout->available), TRUE);
  
  g_rec_mutex_unlock(devout_mutex);
}

void
ags_devout_tic(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  gdouble delay;
  gdouble delay_counter;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  
  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);
  
  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(devout_mutex);

  delay = devout->delay[devout->tic_counter];
  delay_counter = devout->delay_counter;

  note_offset = devout->note_offset;
  note_offset_absolute = devout->note_offset_absolute;
  
  loop_left = devout->loop_left;
  loop_right = devout->loop_right;
  
  do_loop = devout->do_loop;

  g_rec_mutex_unlock(devout_mutex);

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
    g_rec_mutex_lock(devout_mutex);
    
    devout->delay_counter = delay_counter + 1.0 - delay;
    devout->tact_counter += 1.0;

    g_rec_mutex_unlock(devout_mutex);
  }else{
    g_rec_mutex_lock(devout_mutex);
    
    devout->delay_counter += 1.0;

    g_rec_mutex_unlock(devout_mutex);
  }
}

void
ags_devout_offset_changed(AgsSoundcard *soundcard,
			  guint note_offset)
{
  AgsDevout *devout;
  
  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* offset changed */
  g_rec_mutex_lock(devout_mutex);

  devout->tic_counter += 1;

  if(devout->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    devout->tic_counter = 0;
  }

  g_rec_mutex_unlock(devout_mutex);
}

void
ags_devout_set_bpm(AgsSoundcard *soundcard,
		   gdouble bpm)
{
  AgsDevout *devout;

  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* set bpm */
  g_rec_mutex_lock(devout_mutex);

  devout->bpm = bpm;

  g_rec_mutex_unlock(devout_mutex);

  ags_devout_adjust_delay_and_attack(devout);
}

gdouble
ags_devout_get_bpm(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  gdouble bpm;
  
  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* get bpm */
  g_rec_mutex_lock(devout_mutex);

  bpm = devout->bpm;
  
  g_rec_mutex_unlock(devout_mutex);

  return(bpm);
}

void
ags_devout_set_delay_factor(AgsSoundcard *soundcard,
			    gdouble delay_factor)
{
  AgsDevout *devout;

  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* set delay factor */
  g_rec_mutex_lock(devout_mutex);

  devout->delay_factor = delay_factor;

  g_rec_mutex_unlock(devout_mutex);

  ags_devout_adjust_delay_and_attack(devout);
}

gdouble
ags_devout_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  gdouble delay_factor;
  
  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* get delay factor */
  g_rec_mutex_lock(devout_mutex);

  delay_factor = devout->delay_factor;
  
  g_rec_mutex_unlock(devout_mutex);

  return(delay_factor);
}

gdouble
ags_devout_get_delay(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  guint delay_index;
  gdouble delay;
  
  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* get delay */
  g_rec_mutex_lock(devout_mutex);

  delay_index = devout->tic_counter;

  delay = devout->delay[delay_index];
  
  g_rec_mutex_unlock(devout_mutex);
  
  return(delay);
}

gdouble
ags_devout_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  gdouble absolute_delay;
  
  GRecMutex *devout_mutex;
  
  devout = AGS_DEVOUT(soundcard);
  
  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* get absolute delay */
  g_rec_mutex_lock(devout_mutex);

  absolute_delay = (60.0 * (((gdouble) devout->samplerate / (gdouble) devout->buffer_size) / (gdouble) devout->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) devout->delay_factor)));

  g_rec_mutex_unlock(devout_mutex);

  return(absolute_delay);
}

guint
ags_devout_get_attack(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  guint attack_index;
  guint attack;
  
  GRecMutex *devout_mutex;  

  devout = AGS_DEVOUT(soundcard);
  
  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* get attack */
  g_rec_mutex_lock(devout_mutex);

  attack_index = devout->tic_counter;

  attack = devout->attack[attack_index];

  g_rec_mutex_unlock(devout_mutex);
  
  return(attack);
}

void*
ags_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  void *buffer;
  
  devout = AGS_DEVOUT(soundcard);

  if(ags_devout_test_flags(devout, AGS_DEVOUT_BUFFER0)){
    buffer = devout->buffer[0];
  }else if(ags_devout_test_flags(devout, AGS_DEVOUT_BUFFER1)){
    buffer = devout->buffer[1];
  }else if(ags_devout_test_flags(devout, AGS_DEVOUT_BUFFER2)){
    buffer = devout->buffer[2];
  }else if(ags_devout_test_flags(devout, AGS_DEVOUT_BUFFER3)){
    buffer = devout->buffer[3];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_devout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  void *buffer;
  
  devout = AGS_DEVOUT(soundcard);

  //  g_message("next - 0x%0x", ((AGS_DEVOUT_BUFFER0 |
  //				AGS_DEVOUT_BUFFER1 |
  //				AGS_DEVOUT_BUFFER2 |
  //				AGS_DEVOUT_BUFFER3) & (devout->flags)));

  if(ags_devout_test_flags(devout, AGS_DEVOUT_BUFFER0)){
    buffer = devout->buffer[1];
  }else if(ags_devout_test_flags(devout, AGS_DEVOUT_BUFFER1)){
    buffer = devout->buffer[2];
  }else if(ags_devout_test_flags(devout, AGS_DEVOUT_BUFFER2)){
    buffer = devout->buffer[3];
  }else if(ags_devout_test_flags(devout, AGS_DEVOUT_BUFFER3)){
    buffer = devout->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_devout_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  void *buffer;
  
  devout = AGS_DEVOUT(soundcard);

  if(ags_devout_test_flags(devout, AGS_DEVOUT_BUFFER0)){
    buffer = devout->buffer[3];
  }else if(ags_devout_test_flags(devout, AGS_DEVOUT_BUFFER1)){
    buffer = devout->buffer[0];
  }else if(ags_devout_test_flags(devout, AGS_DEVOUT_BUFFER2)){
    buffer = devout->buffer[1];
  }else if(ags_devout_test_flags(devout, AGS_DEVOUT_BUFFER3)){
    buffer = devout->buffer[2];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void
ags_devout_lock_buffer(AgsSoundcard *soundcard,
		       void *buffer)
{
  AgsDevout *devout;

  GRecMutex *buffer_mutex;
  
  devout = AGS_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(devout->buffer != NULL){
    if(buffer == devout->buffer[0]){
      buffer_mutex = devout->buffer_mutex[0];
    }else if(buffer == devout->buffer[1]){
      buffer_mutex = devout->buffer_mutex[1];
    }else if(buffer == devout->buffer[2]){
      buffer_mutex = devout->buffer_mutex[2];
    }else if(buffer == devout->buffer[3]){
      buffer_mutex = devout->buffer_mutex[3];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_devout_unlock_buffer(AgsSoundcard *soundcard,
			 void *buffer)
{
  AgsDevout *devout;

  GRecMutex *buffer_mutex;
  
  devout = AGS_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(devout->buffer != NULL){
    if(buffer == devout->buffer[0]){
      buffer_mutex = devout->buffer_mutex[0];
    }else if(buffer == devout->buffer[1]){
      buffer_mutex = devout->buffer_mutex[1];
    }else if(buffer == devout->buffer[2]){
      buffer_mutex = devout->buffer_mutex[2];
    }else if(buffer == devout->buffer[3]){
      buffer_mutex = devout->buffer_mutex[3];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_devout_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  guint delay_counter;
  
  GRecMutex *devout_mutex;  

  devout = AGS_DEVOUT(soundcard);
  
  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* delay counter */
  g_rec_mutex_lock(devout_mutex);

  delay_counter = devout->delay_counter;
  
  g_rec_mutex_unlock(devout_mutex);

  return(delay_counter);
}

void
ags_devout_set_start_note_offset(AgsSoundcard *soundcard,
				 guint start_note_offset)
{
  AgsDevout *devout;

  GRecMutex *devout_mutex;  

  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* set note offset */
  g_rec_mutex_lock(devout_mutex);

  devout->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(devout_mutex);
}

guint
ags_devout_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  guint start_note_offset;
  
  GRecMutex *devout_mutex;  

  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* set note offset */
  g_rec_mutex_lock(devout_mutex);

  start_note_offset = devout->start_note_offset;

  g_rec_mutex_unlock(devout_mutex);

  return(start_note_offset);
}

void
ags_devout_set_note_offset(AgsSoundcard *soundcard,
			   guint note_offset)
{
  AgsDevout *devout;

  GRecMutex *devout_mutex;  

  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* set note offset */
  g_rec_mutex_lock(devout_mutex);

  devout->note_offset = note_offset;

  g_rec_mutex_unlock(devout_mutex);
}

guint
ags_devout_get_note_offset(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  guint note_offset;
  
  GRecMutex *devout_mutex;  

  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* set note offset */
  g_rec_mutex_lock(devout_mutex);

  note_offset = devout->note_offset;

  g_rec_mutex_unlock(devout_mutex);

  return(note_offset);
}

void
ags_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
				    guint note_offset_absolute)
{
  AgsDevout *devout;
  
  GRecMutex *devout_mutex;  

  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* set note offset */
  g_rec_mutex_lock(devout_mutex);

  devout->note_offset_absolute = note_offset_absolute;

  g_rec_mutex_unlock(devout_mutex);
}

guint
ags_devout_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  guint note_offset_absolute;
  
  GRecMutex *devout_mutex;  

  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* set note offset */
  g_rec_mutex_lock(devout_mutex);

  note_offset_absolute = devout->note_offset_absolute;

  g_rec_mutex_unlock(devout_mutex);

  return(note_offset_absolute);
}

void
ags_devout_set_loop(AgsSoundcard *soundcard,
		    guint loop_left, guint loop_right,
		    gboolean do_loop)
{
  AgsDevout *devout;

  GRecMutex *devout_mutex;  

  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* set loop */
  g_rec_mutex_lock(devout_mutex);

  devout->loop_left = loop_left;
  devout->loop_right = loop_right;
  devout->do_loop = do_loop;

  if(do_loop){
    devout->loop_offset = devout->note_offset;
  }

  g_rec_mutex_unlock(devout_mutex);
}

void
ags_devout_get_loop(AgsSoundcard *soundcard,
		    guint *loop_left, guint *loop_right,
		    gboolean *do_loop)
{
  AgsDevout *devout;

  GRecMutex *devout_mutex;  

  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* get loop */
  g_rec_mutex_lock(devout_mutex);

  if(loop_left != NULL){
    *loop_left = devout->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = devout->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = devout->do_loop;
  }

  g_rec_mutex_unlock(devout_mutex);
}

guint
ags_devout_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  guint loop_offset;
  
  GRecMutex *devout_mutex;  

  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* get loop offset */
  g_rec_mutex_lock(devout_mutex);

  loop_offset = devout->loop_offset;
  
  g_rec_mutex_unlock(devout_mutex);

  return(loop_offset);
}

guint
ags_devout_get_sub_block_count(AgsSoundcard *soundcard)
{
  AgsDevout *devout;

  guint sub_block_count;
  
  GRecMutex *devout_mutex;  

  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* get loop offset */
  g_rec_mutex_lock(devout_mutex);

  sub_block_count = devout->sub_block_count;
  
  g_rec_mutex_unlock(devout_mutex);

  return(sub_block_count);
}

gboolean
ags_devout_trylock_sub_block(AgsSoundcard *soundcard,
			     void *buffer, guint sub_block)
{
  AgsDevout *devout;

  guint pcm_channels;
  guint sub_block_count;
  gboolean success;
  
  GRecMutex *devout_mutex;  
  GRecMutex *sub_block_mutex;

  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* get loop offset */
  g_rec_mutex_lock(devout_mutex);

  pcm_channels = devout->pcm_channels;
  sub_block_count = devout->sub_block_count;
  
  g_rec_mutex_unlock(devout_mutex);
  
  sub_block_mutex = NULL;

  success = FALSE;
  
  if(devout->buffer != NULL){
    if(buffer == devout->buffer[0]){
      sub_block_mutex = devout->sub_block_mutex[sub_block];
    }else if(buffer == devout->buffer[1]){
      sub_block_mutex = devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == devout->buffer[2]){
      sub_block_mutex = devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == devout->buffer[3]){
      sub_block_mutex = devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
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
ags_devout_unlock_sub_block(AgsSoundcard *soundcard,
			    void *buffer, guint sub_block)
{
  AgsDevout *devout;

  guint pcm_channels;
  guint sub_block_count;
  
  GRecMutex *devout_mutex;  
  GRecMutex *sub_block_mutex;

  devout = AGS_DEVOUT(soundcard);

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* get loop offset */
  g_rec_mutex_lock(devout_mutex);

  pcm_channels = devout->pcm_channels;
  sub_block_count = devout->sub_block_count;
  
  g_rec_mutex_unlock(devout_mutex);
  
  sub_block_mutex = NULL;
  
  if(devout->buffer != NULL){
    if(buffer == devout->buffer[0]){
      sub_block_mutex = devout->sub_block_mutex[sub_block];
    }else if(buffer == devout->buffer[1]){
      sub_block_mutex = devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == devout->buffer[2]){
      sub_block_mutex = devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == devout->buffer[3]){
      sub_block_mutex = devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }
  }

  if(sub_block_mutex != NULL){
    g_rec_mutex_unlock(sub_block_mutex);
  }
}

/**
 * ags_devout_switch_buffer_flag:
 * @devout: the #AgsDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.0.0
 */
void
ags_devout_switch_buffer_flag(AgsDevout *devout)
{
  GRecMutex *devout_mutex;
  
  if(!AGS_IS_DEVOUT(devout)){
    return;
  }

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* switch buffer flag */
  g_rec_mutex_lock(devout_mutex);

  if((AGS_DEVOUT_BUFFER0 & (devout->flags)) != 0){
    devout->flags &= (~AGS_DEVOUT_BUFFER0);
    devout->flags |= AGS_DEVOUT_BUFFER1;
  }else if((AGS_DEVOUT_BUFFER1 & (devout->flags)) != 0){
    devout->flags &= (~AGS_DEVOUT_BUFFER1);
    devout->flags |= AGS_DEVOUT_BUFFER2;
  }else if((AGS_DEVOUT_BUFFER2 & (devout->flags)) != 0){
    devout->flags &= (~AGS_DEVOUT_BUFFER2);
    devout->flags |= AGS_DEVOUT_BUFFER3;
  }else if((AGS_DEVOUT_BUFFER3 & (devout->flags)) != 0){
    devout->flags &= (~AGS_DEVOUT_BUFFER3);
    devout->flags |= AGS_DEVOUT_BUFFER0;
  }

  g_rec_mutex_unlock(devout_mutex);
}

/**
 * ags_devout_adjust_delay_and_attack:
 * @devout: the #AgsDevout
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 3.0.0
 */
void
ags_devout_adjust_delay_and_attack(AgsDevout *devout)
{
  if(!AGS_IS_DEVOUT(devout)){
    return;
  }

  ags_soundcard_util_adjust_delay_and_attack(devout);
}

/**
 * ags_devout_realloc_buffer:
 * @devout: the #AgsDevout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.0.0
 */
void
ags_devout_realloc_buffer(AgsDevout *devout)
{
  guint pcm_channels;
  guint buffer_size;
  guint word_size;
  
  GRecMutex *devout_mutex;  

  if(!AGS_IS_DEVOUT(devout)){
    return;
  }

  /* get devout mutex */
  devout_mutex = AGS_DEVOUT_GET_OBJ_MUTEX(devout);

  /* get word size */  
  g_rec_mutex_lock(devout_mutex);

  pcm_channels = devout->pcm_channels;
  buffer_size = devout->buffer_size;
  
  switch(devout->format){
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
    g_warning("ags_devout_realloc_buffer(): unsupported word size");
    return;
  }  

  g_rec_mutex_unlock(devout_mutex);

  //NOTE:JK: there is no lock applicable to buffer
  
  /* AGS_DEVOUT_BUFFER_0 */
  if(devout->buffer[0] != NULL){
    free(devout->buffer[0]);
  }
  
  devout->buffer[0] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_DEVOUT_BUFFER_1 */
  if(devout->buffer[1] != NULL){
    free(devout->buffer[1]);
  }

  devout->buffer[1] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_DEVOUT_BUFFER_2 */
  if(devout->buffer[2] != NULL){
    free(devout->buffer[2]);
  }

  devout->buffer[2] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_DEVOUT_BUFFER_3 */
  if(devout->buffer[3] != NULL){
    free(devout->buffer[3]);
  }
  
  devout->buffer[3] = (void *) malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_devout_new:
 *
 * Creates a new instance of #AgsDevout.
 *
 * Returns: the new #AgsDevout
 *
 * Since: 3.0.0
 */
AgsDevout*
ags_devout_new()
{
  AgsDevout *devout;

  devout = (AgsDevout *) g_object_new(AGS_TYPE_DEVOUT,
				      NULL);
  
  return(devout);
}
