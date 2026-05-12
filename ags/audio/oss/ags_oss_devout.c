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

#include <ags/audio/oss/ags_oss_devout.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_soundcard_util.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <ags/ags_api_config.h>

#include <sys/stat.h>
#include <sys/types.h>

#if !defined(AGS_W32API)
#include <poll.h>
#endif

#if !defined(AGS_W32API)
#include <fcntl.h>
#include <sys/ioctl.h>

#if !defined(__APPLE__)
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

#include <ags/i18n.h>

void ags_oss_devout_class_init(AgsOssDevoutClass *oss_devout);
void ags_oss_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_oss_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_oss_devout_init(AgsOssDevout *oss_devout);
void ags_oss_devout_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_oss_devout_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_oss_devout_dispose(GObject *gobject);
void ags_oss_devout_finalize(GObject *gobject);

AgsUUID* ags_oss_devout_get_uuid(AgsConnectable *connectable);
gboolean ags_oss_devout_has_resource(AgsConnectable *connectable);
gboolean ags_oss_devout_is_ready(AgsConnectable *connectable);
void ags_oss_devout_add_to_registry(AgsConnectable *connectable);
void ags_oss_devout_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_oss_devout_list_resource(AgsConnectable *connectable);
xmlNode* ags_oss_devout_xml_compose(AgsConnectable *connectable);
void ags_oss_devout_xml_parse(AgsConnectable *connectable,
			      xmlNode *node);
gboolean ags_oss_devout_is_connected(AgsConnectable *connectable);
void ags_oss_devout_connect(AgsConnectable *connectable);
void ags_oss_devout_disconnect(AgsConnectable *connectable);

void ags_oss_devout_set_device(AgsSoundcard *soundcard,
			       gchar *device);
gchar* ags_oss_devout_get_device(AgsSoundcard *soundcard);

void ags_oss_devout_set_presets(AgsSoundcard *soundcard,
				guint channels,
				guint rate,
				guint buffer_size,
				AgsSoundcardFormat format);
void ags_oss_devout_get_presets(AgsSoundcard *soundcard,
				guint *channels,
				guint *rate,
				guint *buffer_size,
				AgsSoundcardFormat *format);

void ags_oss_devout_list_cards(AgsSoundcard *soundcard,
			       GList **card_id, GList **card_name);
void ags_oss_devout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			     guint *channels_min, guint *channels_max,
			     guint *rate_min, guint *rate_max,
			     guint *buffer_size_min, guint *buffer_size_max,
			     GError **error);
guint ags_oss_devout_get_capability(AgsSoundcard *soundcard);

gboolean ags_oss_devout_is_available(AgsSoundcard *soundcard);

gboolean ags_oss_devout_is_starting(AgsSoundcard *soundcard);
gboolean ags_oss_devout_is_playing(AgsSoundcard *soundcard);

gchar* ags_oss_devout_get_uptime(AgsSoundcard *soundcard);

void ags_oss_devout_device_play_init(AgsSoundcard *soundcard,
				     GError **error);
void ags_oss_devout_device_fill_backend_buffer(void *app_buffer,
					       AgsSoundcardFormat ags_format,
					       guchar *backend_buffer,
					       guint channels,
					       guint buffer_size);

gboolean ags_oss_devout_device_io_func(GIOChannel *source,
				       GIOCondition condition,
				       AgsOssDevout *oss_devout);

void ags_oss_devout_device_play(AgsSoundcard *soundcard,
				GError **error);
void ags_oss_devout_device_free(AgsSoundcard *soundcard);

void ags_oss_devout_tic(AgsSoundcard *soundcard);
void ags_oss_devout_offset_changed(AgsSoundcard *soundcard,
				   guint64 note_offset);

void ags_oss_devout_set_bpm(AgsSoundcard *soundcard,
			    gdouble bpm);
gdouble ags_oss_devout_get_bpm(AgsSoundcard *soundcard);

void ags_oss_devout_set_start_note_offset(AgsSoundcard *soundcard,
					  guint64 start_note_offset);
guint64 ags_oss_devout_get_start_note_offset(AgsSoundcard *soundcard);

GObject* ags_oss_devout_get_frame_clock(AgsSoundcard *soundcard);

void* ags_oss_devout_get_buffer(AgsSoundcard *soundcard);
void* ags_oss_devout_get_next_buffer(AgsSoundcard *soundcard);
void* ags_oss_devout_get_prev_buffer(AgsSoundcard *soundcard);

void ags_oss_devout_lock_buffer(AgsSoundcard *soundcard,
				void *buffer);
void ags_oss_devout_unlock_buffer(AgsSoundcard *soundcard,
				  void *buffer);

guint ags_oss_devout_get_sub_block_count(AgsSoundcard *soundcard);

gboolean ags_oss_devout_trylock_sub_block(AgsSoundcard *soundcard,
					  void *buffer, guint sub_block);
void ags_oss_devout_unlock_sub_block(AgsSoundcard *soundcard,
				     void *buffer, guint sub_block);

/**
 * SECTION:ags_oss_devout
 * @short_description: Output to soundcard
 * @title: AgsOssDevout
 * @section_id:
 * @include: ags/audio/ags_oss_devout.h
 *
 * #AgsOssDevout represents a soundcard and supports output.
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
};

static gpointer ags_oss_devout_parent_class = NULL;

GType
ags_oss_devout_get_type (void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_oss_devout = 0;

    static const GTypeInfo ags_oss_devout_info = {
      sizeof(AgsOssDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_oss_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsOssDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_oss_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_oss_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_oss_devout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_oss_devout = g_type_register_static(G_TYPE_OBJECT,
						 "AgsOssDevout",
						 &ags_oss_devout_info,
						 0);

    g_type_add_interface_static(ags_type_oss_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_oss_devout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_oss_devout);
  }

  return(g_define_type_id__static);
}

GType
ags_oss_devout_flags_get_type()
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_OSS_DEVOUT_INITIALIZED, "AGS_OSS_DEVOUT_INITIALIZED", "oss-devout-initialized" },
      { AGS_OSS_DEVOUT_START_PLAY, "AGS_OSS_DEVOUT_START_PLAY", "oss-devout-start-play" },
      { AGS_OSS_DEVOUT_PLAY, "AGS_OSS_DEVOUT_PLAY", "oss-devout-play" },
      { AGS_OSS_DEVOUT_SHUTDOWN, "AGS_OSS_DEVOUT_SHUTDOWN", "oss-devout-shutdown" },
      { AGS_OSS_DEVOUT_NONBLOCKING, "AGS_OSS_DEVOUT_NONBLOCKING", "oss-devout-nonblocking" },
      { AGS_OSS_DEVOUT_ATTACK_FIRST, "AGS_OSS_DEVOUT_ATTACK_FIRST", "oss-devout-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsOssDevoutFlags"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
}

void
ags_oss_devout_class_init(AgsOssDevoutClass *oss_devout)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_oss_devout_parent_class = g_type_class_peek_parent(oss_devout);

  /* GObjectClass */
  gobject = (GObjectClass *) oss_devout;

  gobject->set_property = ags_oss_devout_set_property;
  gobject->get_property = ags_oss_devout_get_property;

  gobject->dispose = ags_oss_devout_dispose;
  gobject->finalize = ags_oss_devout_finalize;

  /* properties */
  /**
   * AgsOssDevout:device:
   *
   * The OSS soundcard indentifier
   * 
   * Since: 3.13.2
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   AGS_OSS_DEVOUT_DEFAULT_OSS_DEVICE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsOssDevout:dsp-channels:
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
   * AgsOssDevout:pcm-channels:
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
   * AgsOssDevout:format:
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
   * AgsOssDevout:buffer-size:
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
   * AgsOssDevout:samplerate:
   *
   * The samplerate
   * 
   * Since: 3.13.2
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
   * AgsOssDevout:bpm:
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
				   AGS_SOUNDCARD_DEFAULT_BPM,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BPM,
				  param_spec);

  /**
   * AgsOssDevout:buffer:
   *
   * The buffer
   * 
   * Since: 3.13.2
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to play"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);
  
  /* AgsOssDevoutClass */
}

GQuark
ags_oss_devout_error_quark()
{
  return(g_quark_from_static_string("ags-oss-devout-error-quark"));
}

void
ags_oss_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_oss_devout_get_uuid;
  connectable->has_resource = ags_oss_devout_has_resource;

  connectable->is_ready = ags_oss_devout_is_ready;
  connectable->add_to_registry = ags_oss_devout_add_to_registry;
  connectable->remove_from_registry = ags_oss_devout_remove_from_registry;

  connectable->list_resource = ags_oss_devout_list_resource;
  connectable->xml_compose = ags_oss_devout_xml_compose;
  connectable->xml_parse = ags_oss_devout_xml_parse;

  connectable->is_connected = ags_oss_devout_is_connected;  
  connectable->connect = ags_oss_devout_connect;
  connectable->disconnect = ags_oss_devout_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_oss_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_oss_devout_set_device;
  soundcard->get_device = ags_oss_devout_get_device;
  
  soundcard->set_presets = ags_oss_devout_set_presets;
  soundcard->get_presets = ags_oss_devout_get_presets;

  soundcard->list_cards = ags_oss_devout_list_cards;
  soundcard->pcm_info = ags_oss_devout_pcm_info;
  soundcard->get_capability = ags_oss_devout_get_capability;
  
  soundcard->is_available = ags_oss_devout_is_available;

  soundcard->is_starting =  ags_oss_devout_is_starting;
  soundcard->is_playing = ags_oss_devout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_oss_devout_get_uptime;
  
  soundcard->play_init = ags_oss_devout_device_play_init;
  soundcard->play = ags_oss_devout_device_play;
  
  soundcard->record_init = NULL;
  soundcard->record = NULL;
  
  soundcard->stop = ags_oss_devout_device_free;

  soundcard->tic = ags_oss_devout_tic;
  soundcard->offset_changed = ags_oss_devout_offset_changed;
    
  soundcard->set_bpm = ags_oss_devout_set_bpm;
  soundcard->get_bpm = ags_oss_devout_get_bpm;

  soundcard->set_start_note_offset = ags_oss_devout_set_start_note_offset;
  soundcard->get_start_note_offset = ags_oss_devout_get_start_note_offset;

  soundcard->get_frame_clock = ags_oss_devout_get_frame_clock;

  soundcard->get_buffer = ags_oss_devout_get_buffer;
  soundcard->get_next_buffer = ags_oss_devout_get_next_buffer;
  soundcard->get_prev_buffer = ags_oss_devout_get_prev_buffer;

  soundcard->lock_buffer = ags_oss_devout_lock_buffer;
  soundcard->unlock_buffer = ags_oss_devout_unlock_buffer;

  soundcard->get_sub_block_count = ags_oss_devout_get_sub_block_count;

  soundcard->trylock_sub_block = ags_oss_devout_trylock_sub_block;
  soundcard->unlock_sub_block = ags_oss_devout_unlock_sub_block;
}

void
ags_oss_devout_init(AgsOssDevout *oss_devout)
{  
  AgsConfig *config;
  
  gchar *str;
  gchar *segmentation;

  gdouble absolute_delay;
  guint i;
  guint denominator, numerator;

  /* config */
  config = ags_config_get_instance();

  /* flags */
  oss_devout->flags = 0;
  oss_devout->connectable_flags = 0;
  
  /* insert oss_devout mutex */
  g_rec_mutex_init(&(oss_devout->obj_mutex));

  /* uuid */
  oss_devout->uuid = ags_uuid_alloc();
  ags_uuid_generate(oss_devout->uuid);

  /* presets */
  oss_devout->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  oss_devout->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  oss_devout->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  oss_devout->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  oss_devout->format = ags_soundcard_helper_config_get_format(config);

  /* bpm */
  oss_devout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* start note offset */
  oss_devout->start_note_offset = 0;
  
  /* frame clock */
  oss_devout->frame_clock = ags_frame_clock_new();

  /* device */
  oss_devout->device_fd = -1;
  oss_devout->device = g_strdup(AGS_OSS_DEVOUT_DEFAULT_OSS_DEVICE);

  /* app buffer mutex */
  oss_devout->app_buffer_mode = AGS_OSS_DEVOUT_APP_BUFFER_0;

  oss_devout->app_buffer_mutex = (GRecMutex **) g_malloc(AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE * sizeof(GRecMutex *));

  for(i = 0; i < AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE; i++){
    oss_devout->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(oss_devout->app_buffer_mutex[i]);
  }

  /* sub-block */
  oss_devout->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  oss_devout->sub_block_mutex = (GRecMutex **) g_malloc(AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE * oss_devout->sub_block_count * oss_devout->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE * oss_devout->sub_block_count * oss_devout->pcm_channels; i++){
    oss_devout->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(oss_devout->sub_block_mutex[i]);
  }

  /* app buffer */
  oss_devout->app_buffer = (void **) g_malloc(AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE * sizeof(void *));
  
  for(i = 0; i < AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE; i++){
    oss_devout->app_buffer[i] = NULL;
  }

  ags_atomic_int_set(&(oss_devout->available),
		   FALSE);

  /* backend buffer */
  oss_devout->backend_buffer_mode = AGS_OSS_DEVOUT_BACKEND_BUFFER_0;
  
  oss_devout->backend_buffer = (guchar **) g_malloc(AGS_OSS_DEVOUT_DEFAULT_BACKEND_BUFFER_SIZE * sizeof(guchar *));

  for(i = 0; i < AGS_OSS_DEVOUT_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    oss_devout->backend_buffer[i] = NULL;
  }

  ags_oss_devout_realloc_buffer(oss_devout);

  /* poll */
  oss_devout->io_channel = NULL;
  oss_devout->tag = NULL;

  oss_devout->poll_timeout = -1;
}

void
ags_oss_devout_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsOssDevout *oss_devout;

  GRecMutex *oss_devout_mutex;

  oss_devout = AGS_OSS_DEVOUT(gobject);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    char *device;

    device = (char *) g_value_get_string(value);

    g_rec_mutex_lock(oss_devout_mutex);

    g_free(oss_devout->device);
    
    oss_devout->device = g_strdup(device);

    g_rec_mutex_unlock(oss_devout_mutex);
  }
  break;
  case PROP_DSP_CHANNELS:
  {
    guint dsp_channels;

    dsp_channels = g_value_get_uint(value);

    g_rec_mutex_lock(oss_devout_mutex);

    if(dsp_channels == oss_devout->dsp_channels){
      g_rec_mutex_unlock(oss_devout_mutex);
	
      return;
    }

    oss_devout->dsp_channels = dsp_channels;

    g_rec_mutex_unlock(oss_devout_mutex);
  }
  break;
  case PROP_PCM_CHANNELS:
  {
    guint pcm_channels, old_pcm_channels;
    guint i;
  
    pcm_channels = g_value_get_uint(value);

    g_rec_mutex_lock(oss_devout_mutex);

    if(pcm_channels == oss_devout->pcm_channels){
      g_rec_mutex_unlock(oss_devout_mutex);
	
      return;
    }

    old_pcm_channels = oss_devout->pcm_channels;

    /* destroy if less pcm-channels */
    for(i = AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE * oss_devout->sub_block_count * pcm_channels; i < AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE * oss_devout->sub_block_count * old_pcm_channels; i++){
      g_rec_mutex_clear(oss_devout->sub_block_mutex[i]);

      g_free(oss_devout->sub_block_mutex[i]);
    }

    oss_devout->sub_block_mutex = (GRecMutex **) g_realloc(oss_devout->sub_block_mutex,
							   AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE * oss_devout->sub_block_count * pcm_channels * sizeof(GRecMutex *));

    /* create if more pcm-channels */
    for(i = AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE * oss_devout->sub_block_count * old_pcm_channels; i < AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE * oss_devout->sub_block_count * pcm_channels; i++){
      oss_devout->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

      g_rec_mutex_init(oss_devout->sub_block_mutex[i]);
    }
      
    oss_devout->pcm_channels = pcm_channels;

    g_rec_mutex_unlock(oss_devout_mutex);
      
    ags_oss_devout_realloc_buffer(oss_devout);
  }
  break;
  case PROP_FORMAT:
  {
    guint format;

    format = g_value_get_uint(value);

    g_rec_mutex_lock(oss_devout_mutex);

    if(format == oss_devout->format){
      g_rec_mutex_unlock(oss_devout_mutex);
	
      return;
    }

    oss_devout->format = format;

    g_rec_mutex_unlock(oss_devout_mutex);

    ags_oss_devout_realloc_buffer(oss_devout);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    guint buffer_size;

    buffer_size = g_value_get_uint(value);

    g_rec_mutex_lock(oss_devout_mutex);

    if(buffer_size == oss_devout->buffer_size){
      g_rec_mutex_unlock(oss_devout_mutex);

      return;
    }

    oss_devout->buffer_size = buffer_size;

    ags_frame_clock_set_buffer_size(oss_devout->frame_clock,
				    buffer_size);

    g_rec_mutex_unlock(oss_devout_mutex);

    ags_oss_devout_realloc_buffer(oss_devout);
  }
  break;
  case PROP_SAMPLERATE:
  {
    guint samplerate;

    samplerate = g_value_get_uint(value);

    g_rec_mutex_lock(oss_devout_mutex);

    if(samplerate == oss_devout->samplerate){
      g_rec_mutex_unlock(oss_devout_mutex);

      return;
    }

    oss_devout->samplerate = samplerate;

    ags_frame_clock_set_samplerate(oss_devout->frame_clock,
				   samplerate);

    g_rec_mutex_unlock(oss_devout_mutex);
  }
  break;
  case PROP_BPM:
  {
    gdouble bpm;
      
    bpm = g_value_get_double(value);

    g_rec_mutex_lock(oss_devout_mutex);

    if(bpm == oss_devout->bpm){
      g_rec_mutex_unlock(oss_devout_mutex);

      return;
    }

    oss_devout->bpm = bpm;

    ags_frame_clock_set_bpm(oss_devout->frame_clock,
			    bpm);

    g_rec_mutex_unlock(oss_devout_mutex);
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
ags_oss_devout_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsOssDevout *oss_devout;

  GRecMutex *oss_devout_mutex;

  oss_devout = AGS_OSS_DEVOUT(gobject);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
  {
    g_rec_mutex_lock(oss_devout_mutex);

    g_value_set_string(value, oss_devout->device);

    g_rec_mutex_unlock(oss_devout_mutex);
  }
  break;
  case PROP_DSP_CHANNELS:
  {
    g_rec_mutex_lock(oss_devout_mutex);

    g_value_set_uint(value, oss_devout->dsp_channels);

    g_rec_mutex_unlock(oss_devout_mutex);
  }
  break;
  case PROP_PCM_CHANNELS:
  {
    g_rec_mutex_lock(oss_devout_mutex);

    g_value_set_uint(value, oss_devout->pcm_channels);

    g_rec_mutex_unlock(oss_devout_mutex);
  }
  break;
  case PROP_FORMAT:
  {
    g_rec_mutex_lock(oss_devout_mutex);

    g_value_set_uint(value, oss_devout->format);

    g_rec_mutex_unlock(oss_devout_mutex);
  }
  break;
  case PROP_BUFFER_SIZE:
  {
    g_rec_mutex_lock(oss_devout_mutex);

    g_value_set_uint(value, oss_devout->buffer_size);

    g_rec_mutex_unlock(oss_devout_mutex);
  }
  break;
  case PROP_SAMPLERATE:
  {
    g_rec_mutex_lock(oss_devout_mutex);

    g_value_set_uint(value, oss_devout->samplerate);

    g_rec_mutex_unlock(oss_devout_mutex);
  }
  break;
  case PROP_BPM:
  {
    g_rec_mutex_lock(oss_devout_mutex);

    g_value_set_double(value, oss_devout->bpm);

    g_rec_mutex_unlock(oss_devout_mutex);
  }
  break;
  case PROP_BUFFER:
  {
    g_rec_mutex_lock(oss_devout_mutex);

    g_value_set_pointer(value, oss_devout->app_buffer);

    g_rec_mutex_unlock(oss_devout_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_oss_devout_dispose(GObject *gobject)
{
  AgsOssDevout *oss_devout;

  oss_devout = AGS_OSS_DEVOUT(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_oss_devout_parent_class)->dispose(gobject);
}

void
ags_oss_devout_finalize(GObject *gobject)
{
  AgsOssDevout *oss_devout;

  guint i;
  
  oss_devout = AGS_OSS_DEVOUT(gobject);

  ags_uuid_free(oss_devout->uuid);  

  /* frame clock */
  if(oss_devout->frame_clock != NULL){
    g_object_unref(oss_devout->frame_clock);
  }

  /* app buffer */
  for(i = 0; i < AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE; i++){
    g_free(oss_devout->app_buffer[i]);
  }

  g_free(oss_devout->app_buffer);

  /* app buffer mutex */
  for(i = 0; i < AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE; i++){
    g_rec_mutex_clear(oss_devout->app_buffer_mutex[i]);
    
    g_free(oss_devout->app_buffer_mutex[i]);
  }

  g_free(oss_devout->app_buffer_mutex);

  /* backend buffer */
  for(i = 0; i < AGS_OSS_DEVOUT_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    g_free(oss_devout->backend_buffer[i]);
  }

  g_free(oss_devout->backend_buffer);

  /* sub-block mutex */  
  for(i = 0; i < AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE * oss_devout->sub_block_count * oss_devout->pcm_channels; i++){
    g_rec_mutex_clear(oss_devout->sub_block_mutex[i]);
    
    g_free(oss_devout->sub_block_mutex[i]);
  }

  g_free(oss_devout->sub_block_mutex);
  
  g_free(oss_devout->device);
  
  /* call parent */
  G_OBJECT_CLASS(ags_oss_devout_parent_class)->finalize(gobject);
}

AgsUUID*
ags_oss_devout_get_uuid(AgsConnectable *connectable)
{
  AgsOssDevout *oss_devout;
  
  AgsUUID *ptr;

  GRecMutex *oss_devout_mutex;

  oss_devout = AGS_OSS_DEVOUT(connectable);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* get UUID */
  g_rec_mutex_lock(oss_devout_mutex);

  ptr = oss_devout->uuid;

  g_rec_mutex_unlock(oss_devout_mutex);
  
  return(ptr);
}

gboolean
ags_oss_devout_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_oss_devout_is_ready(AgsConnectable *connectable)
{
  AgsOssDevout *oss_devout;
  
  gboolean is_ready;

  GRecMutex *oss_devout_mutex;

  oss_devout = AGS_OSS_DEVOUT(connectable);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* check is ready */
  g_rec_mutex_lock(oss_devout_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (oss_devout->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(oss_devout_mutex);
  
  return(is_ready);
}

void
ags_oss_devout_add_to_registry(AgsConnectable *connectable)
{
  AgsOssDevout *oss_devout;

  GRecMutex *oss_devout_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  oss_devout = AGS_OSS_DEVOUT(connectable);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  g_rec_mutex_lock(oss_devout_mutex);

  oss_devout->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(oss_devout_mutex);
}

void
ags_oss_devout_remove_from_registry(AgsConnectable *connectable)
{
  AgsOssDevout *oss_devout;

  GRecMutex *oss_devout_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  oss_devout = AGS_OSS_DEVOUT(connectable);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  g_rec_mutex_lock(oss_devout_mutex);

  oss_devout->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(oss_devout_mutex);
}

xmlNode*
ags_oss_devout_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_oss_devout_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_oss_devout_xml_parse(AgsConnectable *connectable,
			 xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_oss_devout_is_connected(AgsConnectable *connectable)
{
  AgsOssDevout *oss_devout;
  
  gboolean is_connected;

  GRecMutex *oss_devout_mutex;

  oss_devout = AGS_OSS_DEVOUT(connectable);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* check is connected */
  g_rec_mutex_lock(oss_devout_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (oss_devout->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(oss_devout_mutex);
  
  return(is_connected);
}

void
ags_oss_devout_connect(AgsConnectable *connectable)
{
  AgsOssDevout *oss_devout;  

  GRecMutex *oss_devout_mutex;

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  oss_devout = AGS_OSS_DEVOUT(connectable);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  g_rec_mutex_lock(oss_devout_mutex);

  oss_devout->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(oss_devout_mutex);
}

void
ags_oss_devout_disconnect(AgsConnectable *connectable)
{

  AgsOssDevout *oss_devout;

  GRecMutex *oss_devout_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  oss_devout = AGS_OSS_DEVOUT(connectable);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  g_rec_mutex_lock(oss_devout_mutex);

  oss_devout->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(oss_devout_mutex);
}

/**
 * ags_oss_devout_test_flags:
 * @oss_devout: the #AgsOssDevout
 * @flags: the flags
 *
 * Test @flags to be set on @oss_devout.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.13.2
 */
gboolean
ags_oss_devout_test_flags(AgsOssDevout *oss_devout, AgsOssDevoutFlags flags)
{
  gboolean retval;  
  
  GRecMutex *oss_devout_mutex;

  if(!AGS_IS_OSS_DEVOUT(oss_devout)){
    return(FALSE);
  }

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* test */
  g_rec_mutex_lock(oss_devout_mutex);

  retval = (flags & (oss_devout->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(oss_devout_mutex);

  return(retval);
}

/**
 * ags_oss_devout_set_flags:
 * @oss_devout: the #AgsOssDevout
 * @flags: see #AgsOssDevoutFlags-enum
 *
 * Enable a feature of @oss_devout.
 *
 * Since: 3.13.2
 */
void
ags_oss_devout_set_flags(AgsOssDevout *oss_devout, AgsOssDevoutFlags flags)
{
  GRecMutex *oss_devout_mutex;

  if(!AGS_IS_OSS_DEVOUT(oss_devout)){
    return;
  }

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(oss_devout_mutex);

  oss_devout->flags |= flags;
  
  g_rec_mutex_unlock(oss_devout_mutex);
}
    
/**
 * ags_oss_devout_unset_flags:
 * @oss_devout: the #AgsOssDevout
 * @flags: see #AgsOssDevoutFlags-enum
 *
 * Disable a feature of @oss_devout.
 *
 * Since: 3.13.2
 */
void
ags_oss_devout_unset_flags(AgsOssDevout *oss_devout, AgsOssDevoutFlags flags)
{  
  GRecMutex *oss_devout_mutex;

  if(!AGS_IS_OSS_DEVOUT(oss_devout)){
    return;
  }

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(oss_devout_mutex);

  oss_devout->flags &= (~flags);
  
  g_rec_mutex_unlock(oss_devout_mutex);
}

void
ags_oss_devout_set_device(AgsSoundcard *soundcard,
			  gchar *device)
{
  AgsOssDevout *oss_devout;

  GList *card_id, *card_id_start, *card_name, *card_name_start;
  
  GRecMutex *oss_devout_mutex;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

#if 0
  /* list cards */
  card_id = NULL;
  card_name = NULL;
  
  ags_soundcard_list_cards(soundcard,
			   &card_id, &card_name);

  card_id_start = card_id;
  card_name_start = card_name;

  /* check card */
  g_rec_mutex_lock(oss_devout_mutex);

  while(card_id != NULL){
    if(!g_ascii_strncasecmp(card_id->data,
			    device,
			    strlen(card_id->data))){
      oss_devout->device = g_strdup(device);

      break;
    }
    
    card_id = card_id->next;
  }

  g_rec_mutex_unlock(oss_devout_mutex);

  /* free card id and name */
  g_list_free_full(card_id_start,
		   g_free);
  g_list_free_full(card_name_start,
		   g_free);
#else
  g_rec_mutex_lock(oss_devout_mutex);

  oss_devout->device = g_strdup(device);

  g_rec_mutex_unlock(oss_devout_mutex);
#endif
}

gchar*
ags_oss_devout_get_device(AgsSoundcard *soundcard)
{
  AgsOssDevout *oss_devout;

  gchar *device;

  GRecMutex *oss_devout_mutex;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);
  
  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  device = NULL;
  
  g_rec_mutex_lock(oss_devout_mutex);

  device = g_strdup(oss_devout->device);

  g_rec_mutex_unlock(oss_devout_mutex);

  return(device);
}

void
ags_oss_devout_set_presets(AgsSoundcard *soundcard,
			   guint channels,
			   guint samplerate,
			   guint buffer_size,
			   AgsSoundcardFormat format)
{
  AgsOssDevout *oss_devout;

  oss_devout = AGS_OSS_DEVOUT(soundcard);

  g_object_set(oss_devout,
	       "pcm-channels", channels,
	       "samplerate", samplerate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_oss_devout_get_presets(AgsSoundcard *soundcard,
			   guint *channels,
			   guint *samplerate,
			   guint *buffer_size,
			   AgsSoundcardFormat *format)
{
  AgsOssDevout *oss_devout;

  GRecMutex *oss_devout_mutex;

  oss_devout = AGS_OSS_DEVOUT(soundcard);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* get presets */
  g_rec_mutex_lock(oss_devout_mutex);

  if(channels != NULL){
    *channels = oss_devout->pcm_channels;
  }

  if(samplerate != NULL){
    *samplerate = oss_devout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = oss_devout->buffer_size;
  }

  if(format != NULL){
    *format = oss_devout->format;
  }

  g_rec_mutex_unlock(oss_devout_mutex);
}

void
ags_oss_devout_list_cards(AgsSoundcard *soundcard,
			  GList **card_id, GList **card_name)
{
  AgsOssDevout *oss_devout;

#if defined(AGS_WITH_OSS)
  oss_sysinfo sysinfo;
  oss_audioinfo ai;
#endif
  
  char *mixer_device;
    
  int mixerfd;

  int next, n;
  int i;

  GRecMutex *oss_devout_mutex;

  oss_devout = AGS_OSS_DEVOUT(soundcard);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  if(card_id != NULL){
    card_id[0] = NULL;
  }

  if(card_name != NULL){
    card_name[0] = NULL;
  }

  mixerfd = -1;
  
  g_rec_mutex_lock(oss_devout_mutex);

#ifdef AGS_WITH_OSS
  if((mixer_device = getenv("OSS_MIXERDEV")) == NULL){
    mixer_device = AGS_OSS_DEVOUT_DEFAULT_OSS_MIXER_DEVICE;
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
	card_id[0] = g_list_prepend(card_id[0],
				    g_strdup(ai.devnode));
      }
	
      if(card_name != NULL){
	card_name[0] = g_list_prepend(card_name[0],
				      g_strdup(ai.name));
      }
    }
    
    next = ai.next_play_engine;
      
    if(next < 0){
      break;
    }
  }
#endif

  g_rec_mutex_unlock(oss_devout_mutex);

  if(card_id != NULL){
    card_id[0] = g_list_reverse(card_id[0]);
  }

  if(card_name != NULL){
    card_name[0] = g_list_reverse(card_name[0]);
  }
}

void
ags_oss_devout_pcm_info(AgsSoundcard *soundcard,
			char *card_id,
			guint *channels_min, guint *channels_max,
			guint *rate_min, guint *rate_max,
			guint *buffer_size_min, guint *buffer_size_max,
			GError **error)
{
  AgsOssDevout *oss_devout;

#if defined(AGS_WITH_OSS)
  oss_audioinfo ainfo;
#endif
  
  gchar *str;
    
  int mixerfd;
  int acc;
  unsigned int cmd;

  GRecMutex *oss_devout_mutex;

  if(card_id == NULL){
    return;
  }
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* pcm info */
  g_rec_mutex_lock(oss_devout_mutex);

#if defined(AGS_WITH_OSS)
  mixerfd = open(card_id, O_RDWR, 0);

  if(mixerfd == -1){
    int e = errno;
      
    str = strerror(e);
    g_message("unable to open pcm device: %s\n", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVOUT_ERROR,
		  AGS_OSS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		  "unable to open pcm device: %s\n",
		  str);
    }
    
    goto ags_oss_devout_pcm_info_ERR;      
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
    goto ags_oss_devout_pcm_info_ERR;
  }
    
  if(ioctl(mixerfd, cmd, &ainfo) == -1){
    int e = errno;

    str = strerror(e);
    g_message("unable to retrieve audio info: %s\n", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVOUT_ERROR,
		  AGS_OSS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		  "unable to retrieve audio info: %s\n",
		  str);
    }
    
    return;
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

ags_oss_devout_pcm_info_ERR:

  g_rec_mutex_unlock(oss_devout_mutex);
}

guint
ags_oss_devout_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_PLAYBACK);
}

gboolean
ags_oss_devout_is_available(AgsSoundcard *soundcard)
{
  AgsOssDevout *oss_devout;
  
#if defined(AGS_WITH_OSS)
  struct pollfd fds;
#endif

  gboolean is_available;

  GRecMutex *oss_devout_mutex;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);
  
  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);  

#if defined(AGS_WITH_OSS)
  fds.events = POLLOUT;
  fds.fd = oss_devout->device_fd;
  
  poll(&fds, 1, 0);
  
  /* check available */
  is_available = ((POLLOUT & (fds.revents)) != 0) ? TRUE: FALSE;
#else
  is_available = FALSE;
#endif
  
  return(is_available);
}

gboolean
ags_oss_devout_is_starting(AgsSoundcard *soundcard)
{
  AgsOssDevout *oss_devout;

  gboolean is_starting;
  
  GRecMutex *oss_devout_mutex;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* check is starting */
  g_rec_mutex_lock(oss_devout_mutex);

  is_starting = ((AGS_OSS_DEVOUT_START_PLAY & (oss_devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(oss_devout_mutex);
  
  return(is_starting);
}

gboolean
ags_oss_devout_is_playing(AgsSoundcard *soundcard)
{
  AgsOssDevout *oss_devout;

  gboolean is_playing;
  
  GRecMutex *oss_devout_mutex;

  oss_devout = AGS_OSS_DEVOUT(soundcard);
  
  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* check is starting */
  g_rec_mutex_lock(oss_devout_mutex);

  is_playing = ((AGS_OSS_DEVOUT_PLAY & (oss_devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(oss_devout_mutex);

  return(is_playing);
}

gchar*
ags_oss_devout_get_uptime(AgsSoundcard *soundcard)
{
  AgsOssDevout *oss_devout;

  gchar *uptime;

  GRecMutex *oss_devout_mutex;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);
  
  /* get core audio devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* get time string */
  g_rec_mutex_lock(oss_devout_mutex);
  
  uptime = ags_frame_clock_to_time_string(oss_devout->frame_clock);

  g_rec_mutex_unlock(oss_devout_mutex);
  
  return(uptime);
}

void
ags_oss_devout_device_play_init(AgsSoundcard *soundcard,
				GError **error)
{
  AgsOssDevout *oss_devout;

  GIOChannel *io_channel;

  guint tag;

  gchar *str;

  guint word_size;
  int format;
  int tmp;
  guint i;

  GRecMutex *oss_devout_mutex;

  if(ags_soundcard_is_playing(soundcard)){
    return;
  }

  oss_devout = AGS_OSS_DEVOUT(soundcard);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* retrieve word size */
  g_rec_mutex_lock(oss_devout_mutex);

  switch(oss_devout->format){
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
    g_rec_mutex_unlock(oss_devout_mutex);

    g_warning("ags_oss_devout_oss_init(): unsupported word size");

    return;
  }

  /* prepare for playback */
  oss_devout->flags |= (AGS_OSS_DEVOUT_START_PLAY |
			AGS_OSS_DEVOUT_PLAY |
			AGS_OSS_DEVOUT_NONBLOCKING);

  for(i = 0; i < AGS_OSS_DEVOUT_DEFAULT_APP_BUFFER_SIZE; i++){
    memset(oss_devout->app_buffer[i], 0, oss_devout->pcm_channels * oss_devout->buffer_size * word_size);
  }

  /* allocate ring buffer */
  for(i = 0; i < AGS_OSS_DEVOUT_DEFAULT_BACKEND_BUFFER_SIZE; i++){
    oss_devout->backend_buffer[i] = (guchar *) g_malloc(oss_devout->pcm_channels *
							oss_devout->buffer_size * word_size *
							sizeof(guchar));
  }

#ifdef AGS_WITH_OSS
  /* open device fd */
  str = oss_devout->device;
  oss_devout->device_fd = open(str, O_WRONLY, 0);

  if(oss_devout->device_fd == -1){
    oss_devout->flags &= (~(AGS_OSS_DEVOUT_START_PLAY |
			    AGS_OSS_DEVOUT_PLAY |
			    AGS_OSS_DEVOUT_NONBLOCKING));

    g_rec_mutex_unlock(oss_devout_mutex);

    g_warning("couldn't open device %s", oss_devout->device);

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVOUT_ERROR,
		  AGS_OSS_DEVOUT_ERROR_LOCKED_SOUNDCARD,
		  "unable to open dsp device: %s\n",
		  str);
    }

    return;
  }

  //NOTE:JK: unsupported on kfreebsd 9.0
  //  tmp = APF_NORMAL;
  //  ioctl(oss_devout->device_fd, SNDCTL_DSP_PROFILE, &tmp);

  tmp = format;

  if(ioctl(oss_devout->device_fd, SNDCTL_DSP_SETFMT, &tmp) == -1){
    oss_devout->flags &= (~(AGS_OSS_DEVOUT_START_PLAY |
			    AGS_OSS_DEVOUT_PLAY |
			    AGS_OSS_DEVOUT_NONBLOCKING));

    g_rec_mutex_unlock(oss_devout_mutex);

    str = strerror(errno);
    g_warning("failed to select bits/sample");

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVOUT_ERROR,
		  AGS_OSS_DEVOUT_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
		  "unable to open dsp device: %s",
		  str);
    }

    oss_devout->device_fd = -1;

    return;
  }
  
  if(tmp != format){
    oss_devout->flags &= (~(AGS_OSS_DEVOUT_START_PLAY |
			    AGS_OSS_DEVOUT_PLAY |
			    AGS_OSS_DEVOUT_NONBLOCKING));

    g_rec_mutex_unlock(oss_devout_mutex);

    str = strerror(errno);
    g_warning("failed to select bits/sample");

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVOUT_ERROR,
		  AGS_OSS_DEVOUT_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
		  "unable to open dsp device: %s",
		  str);
    }

    oss_devout->device_fd = -1;

    return;
  }

  tmp = oss_devout->dsp_channels;

  if(ioctl(oss_devout->device_fd, SNDCTL_DSP_CHANNELS, &tmp) == -1){
    oss_devout->flags &= (~(AGS_OSS_DEVOUT_START_PLAY |
			    AGS_OSS_DEVOUT_PLAY |
			    AGS_OSS_DEVOUT_NONBLOCKING));

    g_rec_mutex_unlock(oss_devout_mutex);

    str = strerror(errno);
    g_warning("Channels count (%i) not available for playbacks: %s", oss_devout->dsp_channels, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVOUT_ERROR,
		  AGS_OSS_DEVOUT_ERROR_CHANNELS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    oss_devout->device_fd = -1;
    
    return;
  }

  if(tmp != oss_devout->dsp_channels){
    oss_devout->flags &= (~(AGS_OSS_DEVOUT_START_PLAY |
			    AGS_OSS_DEVOUT_PLAY |
			    AGS_OSS_DEVOUT_NONBLOCKING));

    g_rec_mutex_unlock(oss_devout_mutex);

    str = strerror(errno);
    g_warning("Channels count (%i) not available for playbacks: %s", oss_devout->dsp_channels, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVOUT_ERROR,
		  AGS_OSS_DEVOUT_ERROR_CHANNELS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }
    
    oss_devout->device_fd = -1;
    
    return;
  }

  tmp = oss_devout->samplerate;

  if(ioctl(oss_devout->device_fd, SNDCTL_DSP_SPEED, &tmp) == -1){
    oss_devout->flags &= (~(AGS_OSS_DEVOUT_START_PLAY |
			    AGS_OSS_DEVOUT_PLAY |
			    AGS_OSS_DEVOUT_NONBLOCKING));

    g_rec_mutex_unlock(oss_devout_mutex);

    str = strerror(errno);
    g_warning("Rate %iHz not available for playback: %s", oss_devout->samplerate, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_OSS_DEVOUT_ERROR,
		  AGS_OSS_DEVOUT_ERROR_SAMPLERATE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    oss_devout->device_fd = -1;
    
    return;
  }

  if(tmp != oss_devout->samplerate){
    g_warning("Warning: Playback using %d Hz (file %d Hz)",
	      tmp,
	      oss_devout->samplerate);
  }
  
#if 0
  io_channel = g_io_channel_unix_new(oss_devout->device_fd);
  tag = g_io_add_watch(io_channel,
		       G_IO_OUT,
		       (GIOFunc) ags_oss_devout_oss_io_func,
		       oss_devout);
      
  oss_devout->io_channel = g_list_prepend(oss_devout->io_channel,
					  io_channel);
  oss_devout->tag = g_list_prepend(oss_devout->tag,
				   GUINT_TO_POINTER(tag));
#endif
#endif
  
  oss_devout->backend_buffer_mode = AGS_OSS_DEVOUT_BACKEND_BUFFER_0;
  
#ifdef AGS_WITH_OSS
  oss_devout->flags |= AGS_OSS_DEVOUT_INITIALIZED;
#endif

  ags_frame_clock_start(oss_devout->frame_clock);

  ags_frame_clock_set_note_offset(oss_devout->frame_clock,
				  oss_devout->start_note_offset);

  oss_devout->app_buffer_mode = AGS_OSS_DEVOUT_APP_BUFFER_0;

  oss_devout->poll_timeout = -1;
  
  g_rec_mutex_unlock(oss_devout_mutex);
}

void
ags_oss_devout_device_fill_backend_buffer(void *app_buffer,
					  AgsSoundcardFormat ags_format,
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
    g_warning("ags_oss_devout_oss_play(): unsupported word size");
    return;
  }

  /* fill the channel areas */
  for(count = 0; count < buffer_size; count++){
    for(chn = 0; chn < channels; chn++){
      res = 0;
	
      switch(ags_format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	res = (int) ((gint8 *) app_buffer)[count * channels + chn];
      }
      break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	res = (int) ((gint16 *) app_buffer)[count * channels + chn];
      }
      break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	res = (int) ((gint32 *) app_buffer)[count * channels + chn];
      }
      break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	res = (int) ((gint32 *) app_buffer)[count * channels + chn];
      }
      break;
      }
	
      /* Generate data in native endian format */
      if(ags_endian_host_is_be()){
	for(i = 0; i < bps; i++){
	  *(backend_buffer + chn * bps + word_size - 1 - i) = (res >> i * 8) & 0xff;
	}
      }else{
	for(i = 0; i < bps; i++){
	  *(backend_buffer + chn * bps + i) = (res >>  i * 8) & 0xff;
	}
      }	
    }

    backend_buffer += channels * bps;
  }
}

gboolean
ags_oss_devout_device_io_func(GIOChannel *source,
			      GIOCondition condition,
			      AgsOssDevout *oss_devout)
{
  ags_atomic_int_set(&(oss_devout->available), TRUE);

  return(TRUE);
}

void
ags_oss_devout_device_play(AgsSoundcard *soundcard,
			   GError **error)
{
  AgsOssDevout *oss_devout;

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
  gboolean initial_run;

  int n_write;
  
  GRecMutex *oss_devout_mutex;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);

  application_context = ags_application_context_get_instance();

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* lock */
  g_rec_mutex_lock(oss_devout_mutex);
  
  initial_run = ((AGS_OSS_DEVOUT_START_PLAY & (oss_devout->flags)) != 0) ? TRUE: FALSE;
  
  /* retrieve word size */
  switch(oss_devout->format){
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
    g_rec_mutex_unlock(oss_devout_mutex);
    
    g_warning("ags_oss_devout_oss_play(): unsupported word size");
    
    return;
  }

  /* do playback */
  oss_devout->flags &= (~AGS_OSS_DEVOUT_START_PLAY);

  if((AGS_OSS_DEVOUT_INITIALIZED & (oss_devout->flags)) == 0){
    g_rec_mutex_unlock(oss_devout_mutex);
    
    return;
  }

#ifdef AGS_WITH_OSS    
  /* fill ring buffer */
  ags_soundcard_lock_buffer(soundcard,
			    oss_devout->app_buffer[oss_devout->app_buffer_mode]);

  ags_oss_devout_device_fill_backend_buffer(oss_devout->app_buffer[oss_devout->app_buffer_mode],
					    oss_devout->format,
					    oss_devout->backend_buffer[oss_devout->backend_buffer_mode],
					    oss_devout->pcm_channels,
					    oss_devout->buffer_size);

  ags_soundcard_unlock_buffer(soundcard,
			      oss_devout->app_buffer[oss_devout->app_buffer_mode]);

  /* wait until available */
  poll_timeout = oss_devout->poll_timeout;

  g_rec_mutex_unlock(oss_devout_mutex);

  //TODO:JK: implement me
  
  while(!ags_soundcard_is_available(AGS_SOUNDCARD(oss_devout))){
    if(poll_timeout < 0 ||
       g_get_monotonic_time() > poll_timeout){
      break;
    }
    
    g_usleep(1);
  }

  ags_atomic_int_set(&(oss_devout->available),
		   FALSE);
  
  g_rec_mutex_lock(oss_devout_mutex);

  oss_devout->poll_timeout = g_get_monotonic_time() + (G_USEC_PER_SEC * (1.0 / (gdouble) oss_devout->samplerate * (gdouble) oss_devout->buffer_size));

  /* write ring buffer */
  n_write = write(oss_devout->device_fd,
		  oss_devout->backend_buffer[oss_devout->backend_buffer_mode],
		  oss_devout->pcm_channels * oss_devout->buffer_size * word_size * sizeof (char));
  
  if(n_write != oss_devout->pcm_channels * oss_devout->buffer_size * word_size * sizeof (char)){
    g_critical("write() return doesn't match written bytes");
  }
#endif

  /* increment nth ring-buffer */
  if(oss_devout->backend_buffer_mode + 1 > AGS_OSS_DEVOUT_BACKEND_BUFFER_7){
    oss_devout->backend_buffer_mode = AGS_OSS_DEVOUT_BACKEND_BUFFER_0;
  }else{
    oss_devout->backend_buffer_mode += 1;
  }
  
  g_rec_mutex_unlock(oss_devout_mutex);

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;
  
  /* tic soundcard */
  if(!initial_run){
    tic_device = ags_tic_device_new((GObject *) oss_devout);
    task = g_list_append(task,
			 tic_device);
  }
  
  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) oss_devout);
  task = g_list_append(task,
		       clear_buffer);
  
  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) oss_devout);
  task = g_list_append(task,
		       clear_buffer);

  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) oss_devout);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);
  
  /* unref */
  g_object_unref(task_launcher);
}

void
ags_oss_devout_device_free(AgsSoundcard *soundcard)
{
  AgsOssDevout *oss_devout;

  guint i;
  
  GRecMutex *oss_devout_mutex;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /*  */
  g_rec_mutex_lock(oss_devout_mutex);

  if((AGS_OSS_DEVOUT_INITIALIZED & (oss_devout->flags)) == 0){
    g_rec_mutex_unlock(oss_devout_mutex);
    
    return;
  }

  g_rec_mutex_unlock(oss_devout_mutex);
  
  close(oss_devout->device_fd);
  oss_devout->device_fd = -1;

  /* free ring-buffer */
  g_rec_mutex_lock(oss_devout_mutex);

  if(oss_devout->backend_buffer != NULL){
    for(i = 0; i < AGS_OSS_DEVOUT_DEFAULT_BACKEND_BUFFER_SIZE; i++){
      g_free(oss_devout->backend_buffer[i]);

      oss_devout->backend_buffer[i] = NULL;
    }
  }

  /* reset flags */
  oss_devout->flags &= (~(AGS_OSS_DEVOUT_PLAY |
			  AGS_OSS_DEVOUT_INITIALIZED));

  oss_devout->app_buffer_mode = AGS_OSS_DEVOUT_APP_BUFFER_0;

  oss_devout->backend_buffer_mode = AGS_OSS_DEVOUT_BACKEND_BUFFER_0;

  ags_frame_clock_stop(oss_devout->frame_clock);

  ags_atomic_int_set(&(oss_devout->available), FALSE);

  g_rec_mutex_unlock(oss_devout_mutex);
}

void
ags_oss_devout_tic(AgsSoundcard *soundcard)
{
  AgsOssDevout *oss_devout;

  GRecMutex *oss_devout_mutex;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);
  
  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* frame clock */
  g_rec_mutex_lock(oss_devout_mutex);
  
  ags_frame_clock_increment_counter(oss_devout->frame_clock);

  /* 16th pulse */
  if(ags_frame_clock_get_has_16th_pulse(oss_devout->frame_clock)){
    ags_soundcard_offset_changed(soundcard,
				 oss_devout->frame_clock->note_offset);
  }
  
  g_rec_mutex_unlock(oss_devout_mutex);
}

void
ags_oss_devout_offset_changed(AgsSoundcard *soundcard,
			      guint64 note_offset)
{
  //empty
}

void
ags_oss_devout_set_bpm(AgsSoundcard *soundcard,
		       gdouble bpm)
{
  AgsOssDevout *oss_devout;

  GRecMutex *oss_devout_mutex;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* set bpm */
  g_rec_mutex_lock(oss_devout_mutex);

  oss_devout->bpm = bpm;

  ags_frame_clock_set_bpm(oss_devout->frame_clock,
			  bpm);

  g_rec_mutex_unlock(oss_devout_mutex);
}

gdouble
ags_oss_devout_get_bpm(AgsSoundcard *soundcard)
{
  AgsOssDevout *oss_devout;

  gdouble bpm;
  
  GRecMutex *oss_devout_mutex;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* get bpm */
  g_rec_mutex_lock(oss_devout_mutex);

  bpm = oss_devout->bpm;
  
  g_rec_mutex_unlock(oss_devout_mutex);

  return(bpm);
}

void
ags_oss_devout_set_start_note_offset(AgsSoundcard *soundcard,
				     guint64 start_note_offset)
{
  AgsOssDevout *oss_devout;

  GRecMutex *oss_devout_mutex;  

  oss_devout = AGS_OSS_DEVOUT(soundcard);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* set note offset */
  g_rec_mutex_lock(oss_devout_mutex);

  oss_devout->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(oss_devout_mutex);
}

guint64
ags_oss_devout_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsOssDevout *oss_devout;

  guint64 start_note_offset;
  
  GRecMutex *oss_devout_mutex;  

  oss_devout = AGS_OSS_DEVOUT(soundcard);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* set note offset */
  g_rec_mutex_lock(oss_devout_mutex);

  start_note_offset = oss_devout->start_note_offset;

  g_rec_mutex_unlock(oss_devout_mutex);

  return(start_note_offset);
}

GObject*
ags_oss_devout_get_frame_clock(AgsSoundcard *soundcard)
{
  AgsOssDevout *oss_devout;

  GObject *frame_clock;
  
  GRecMutex *oss_devout_mutex;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);

  /* get core audio devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* get frame clock */
  g_rec_mutex_lock(oss_devout_mutex);

  frame_clock = (GObject *) oss_devout->frame_clock;
  
  g_rec_mutex_unlock(oss_devout_mutex);

  return(frame_clock);
}

void*
ags_oss_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsOssDevout *oss_devout;

  void *buffer;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);

  buffer = oss_devout->app_buffer[oss_devout->app_buffer_mode];

  return(buffer);
}

void*
ags_oss_devout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsOssDevout *oss_devout;

  void *buffer;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);

  if(oss_devout->app_buffer_mode == AGS_OSS_DEVOUT_APP_BUFFER_3){
    buffer = oss_devout->app_buffer[AGS_OSS_DEVOUT_APP_BUFFER_0];
  }else{
    buffer = oss_devout->app_buffer[oss_devout->app_buffer_mode + 1];
  }

  return(buffer);
}

void*
ags_oss_devout_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsOssDevout *oss_devout;

  void *buffer;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);

  if(oss_devout->app_buffer_mode == AGS_OSS_DEVOUT_APP_BUFFER_0){
    buffer = oss_devout->app_buffer[AGS_OSS_DEVOUT_APP_BUFFER_3];
  }else{
    buffer = oss_devout->app_buffer[oss_devout->app_buffer_mode - 1];
  }

  return(buffer);
}

void
ags_oss_devout_lock_buffer(AgsSoundcard *soundcard,
			   void *buffer)
{
  AgsOssDevout *oss_devout;

  GRecMutex *buffer_mutex;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(oss_devout->app_buffer != NULL){
    if(buffer == oss_devout->app_buffer[0]){
      buffer_mutex = oss_devout->app_buffer_mutex[0];
    }else if(buffer == oss_devout->app_buffer[1]){
      buffer_mutex = oss_devout->app_buffer_mutex[1];
    }else if(buffer == oss_devout->app_buffer[2]){
      buffer_mutex = oss_devout->app_buffer_mutex[2];
    }else if(buffer == oss_devout->app_buffer[3]){
      buffer_mutex = oss_devout->app_buffer_mutex[3];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_oss_devout_unlock_buffer(AgsSoundcard *soundcard,
			     void *buffer)
{
  AgsOssDevout *oss_devout;

  GRecMutex *buffer_mutex;
  
  oss_devout = AGS_OSS_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(oss_devout->app_buffer != NULL){
    if(buffer == oss_devout->app_buffer[0]){
      buffer_mutex = oss_devout->app_buffer_mutex[0];
    }else if(buffer == oss_devout->app_buffer[1]){
      buffer_mutex = oss_devout->app_buffer_mutex[1];
    }else if(buffer == oss_devout->app_buffer[2]){
      buffer_mutex = oss_devout->app_buffer_mutex[2];
    }else if(buffer == oss_devout->app_buffer[3]){
      buffer_mutex = oss_devout->app_buffer_mutex[3];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_oss_devout_get_sub_block_count(AgsSoundcard *soundcard)
{
  AgsOssDevout *oss_devout;

  guint sub_block_count;
  
  GRecMutex *oss_devout_mutex;  

  oss_devout = AGS_OSS_DEVOUT(soundcard);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* get loop offset */
  g_rec_mutex_lock(oss_devout_mutex);

  sub_block_count = oss_devout->sub_block_count;
  
  g_rec_mutex_unlock(oss_devout_mutex);

  return(sub_block_count);
}

gboolean
ags_oss_devout_trylock_sub_block(AgsSoundcard *soundcard,
				 void *buffer, guint sub_block)
{
  AgsOssDevout *oss_devout;

  guint pcm_channels;
  guint sub_block_count;
  gboolean success;
  
  GRecMutex *oss_devout_mutex;  
  GRecMutex *sub_block_mutex;

  oss_devout = AGS_OSS_DEVOUT(soundcard);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* get loop offset */
  g_rec_mutex_lock(oss_devout_mutex);

  pcm_channels = oss_devout->pcm_channels;
  sub_block_count = oss_devout->sub_block_count;
  
  g_rec_mutex_unlock(oss_devout_mutex);
  
  sub_block_mutex = NULL;

  success = FALSE;
  
  if(oss_devout->app_buffer != NULL){
    if(buffer == oss_devout->app_buffer[0]){
      sub_block_mutex = oss_devout->sub_block_mutex[sub_block];
    }else if(buffer == oss_devout->app_buffer[1]){
      sub_block_mutex = oss_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == oss_devout->app_buffer[2]){
      sub_block_mutex = oss_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == oss_devout->app_buffer[3]){
      sub_block_mutex = oss_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
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
ags_oss_devout_unlock_sub_block(AgsSoundcard *soundcard,
				void *buffer, guint sub_block)
{
  AgsOssDevout *oss_devout;

  guint pcm_channels;
  guint sub_block_count;
  
  GRecMutex *oss_devout_mutex;  
  GRecMutex *sub_block_mutex;

  oss_devout = AGS_OSS_DEVOUT(soundcard);

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* get loop offset */
  g_rec_mutex_lock(oss_devout_mutex);

  pcm_channels = oss_devout->pcm_channels;
  sub_block_count = oss_devout->sub_block_count;
  
  g_rec_mutex_unlock(oss_devout_mutex);
  
  sub_block_mutex = NULL;
  
  if(oss_devout->app_buffer != NULL){
    if(buffer == oss_devout->app_buffer[0]){
      sub_block_mutex = oss_devout->sub_block_mutex[sub_block];
    }else if(buffer == oss_devout->app_buffer[1]){
      sub_block_mutex = oss_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == oss_devout->app_buffer[2]){
      sub_block_mutex = oss_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == oss_devout->app_buffer[3]){
      sub_block_mutex = oss_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }
  }

  if(sub_block_mutex != NULL){
    g_rec_mutex_unlock(sub_block_mutex);
  }
}

/**
 * ags_oss_devout_switch_buffer_flag:
 * @oss_devout: the #AgsOssDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.13.2
 */
void
ags_oss_devout_switch_buffer_flag(AgsOssDevout *oss_devout)
{
  GRecMutex *oss_devout_mutex;
  
  if(!AGS_IS_OSS_DEVOUT(oss_devout)){
    return;
  }

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* switch buffer flag */
  g_rec_mutex_lock(oss_devout_mutex);

  if(oss_devout->app_buffer_mode < AGS_OSS_DEVOUT_APP_BUFFER_3){
    oss_devout->app_buffer_mode += 1;
  }else{
    oss_devout->app_buffer_mode = AGS_OSS_DEVOUT_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(oss_devout_mutex);
}

/**
 * ags_oss_devout_realloc_buffer:
 * @oss_devout: the #AgsOssDevout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.13.2
 */
void
ags_oss_devout_realloc_buffer(AgsOssDevout *oss_devout)
{
  guint pcm_channels;
  guint buffer_size;
  guint word_size;
  AgsSoundcardFormat format;
  
  GRecMutex *oss_devout_mutex;  

  if(!AGS_IS_OSS_DEVOUT(oss_devout)){
    return;
  }

  /* get oss devout mutex */
  oss_devout_mutex = AGS_OSS_DEVOUT_GET_OBJ_MUTEX(oss_devout);

  /* get word size */  
  g_rec_mutex_lock(oss_devout_mutex);

  pcm_channels = oss_devout->pcm_channels;
  buffer_size = oss_devout->buffer_size;

  format = oss_devout->format;
  
  g_rec_mutex_unlock(oss_devout_mutex);
  
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
    g_warning("ags_oss_devout_realloc_buffer(): unsupported word size");
    
    return;
  }  

  //NOTE:JK: there is no lock applicable to buffer

  if(oss_devout->app_buffer[0] != NULL){
    g_free(oss_devout->app_buffer[0]);
  }
  
  oss_devout->app_buffer[0] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  if(oss_devout->app_buffer[1] != NULL){
    g_free(oss_devout->app_buffer[1]);
  }

  oss_devout->app_buffer[1] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  if(oss_devout->app_buffer[2] != NULL){
    g_free(oss_devout->app_buffer[2]);
  }

  oss_devout->app_buffer[2] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  if(oss_devout->app_buffer[3] != NULL){
    g_free(oss_devout->app_buffer[3]);
  }
  
  oss_devout->app_buffer[3] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_oss_devout_new:
 *
 * Creates a new instance of #AgsOssDevout.
 *
 * Returns: the new #AgsOssDevout
 *
 * Since: 3.13.2
 */
AgsOssDevout*
ags_oss_devout_new()
{
  AgsOssDevout *oss_devout;

  oss_devout = (AgsOssDevout *) g_object_new(AGS_TYPE_OSS_DEVOUT,
					     NULL);
  
  return(oss_devout);
}
