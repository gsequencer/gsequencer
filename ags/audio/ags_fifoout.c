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

#include <ags/audio/ags_fifoout.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifndef AGS_W32API
#include <fcntl.h>
#include <sys/ioctl.h>
#endif

#include <errno.h>

#include <string.h>
#include <math.h>

#include <time.h>
#include <signal.h>
#include <strings.h>
#include <unistd.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_fifoout_class_init(AgsFifooutClass *fifoout);
void ags_fifoout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_fifoout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_fifoout_init(AgsFifoout *fifoout);
void ags_fifoout_set_property(GObject *gobject,
			      guint prop_id,
			      const GValue *value,
			      GParamSpec *param_spec);
void ags_fifoout_get_property(GObject *gobject,
			      guint prop_id,
			      GValue *value,
			      GParamSpec *param_spec);
void ags_fifoout_dispose(GObject *gobject);
void ags_fifoout_finalize(GObject *gobject);

AgsUUID* ags_fifoout_get_uuid(AgsConnectable *connectable);
gboolean ags_fifoout_has_resource(AgsConnectable *connectable);
gboolean ags_fifoout_is_ready(AgsConnectable *connectable);
void ags_fifoout_add_to_registry(AgsConnectable *connectable);
void ags_fifoout_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_fifoout_list_resource(AgsConnectable *connectable);
xmlNode* ags_fifoout_xml_compose(AgsConnectable *connectable);
void ags_fifoout_xml_parse(AgsConnectable *connectable,
			   xmlNode *node);
gboolean ags_fifoout_is_connected(AgsConnectable *connectable);
void ags_fifoout_connect(AgsConnectable *connectable);
void ags_fifoout_disconnect(AgsConnectable *connectable);

void ags_fifoout_set_device(AgsSoundcard *soundcard,
			    gchar *device);
gchar* ags_fifoout_get_device(AgsSoundcard *soundcard);

void ags_fifoout_set_presets(AgsSoundcard *soundcard,
			     guint channels,
			     guint rate,
			     guint buffer_size,
			     AgsSoundcardFormat format);
void ags_fifoout_get_presets(AgsSoundcard *soundcard,
			     guint *channels,
			     guint *rate,
			     guint *buffer_size,
			     AgsSoundcardFormat *format);

void ags_fifoout_list_cards(AgsSoundcard *soundcard,
			    GList **card_id, GList **card_name);
void ags_fifoout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			  guint *channels_min, guint *channels_max,
			  guint *rate_min, guint *rate_max,
			  guint *buffer_size_min, guint *buffer_size_max,
			  GError **error);

gboolean ags_fifoout_is_available(AgsSoundcard *soundcard);

gboolean ags_fifoout_is_starting(AgsSoundcard *soundcard);
gboolean ags_fifoout_is_playing(AgsSoundcard *soundcard);

gchar* ags_fifoout_get_uptime(AgsSoundcard *soundcard);

void ags_fifoout_fifo_init(AgsSoundcard *soundcard,
			   GError **error);
void ags_fifoout_fifo_play(AgsSoundcard *soundcard,
			   GError **error);
void ags_fifoout_fifo_free(AgsSoundcard *soundcard);

void ags_fifoout_tic(AgsSoundcard *soundcard);
void ags_fifoout_offset_changed(AgsSoundcard *soundcard,
				guint64 note_offset);

void ags_fifoout_set_bpm(AgsSoundcard *soundcard,
			 gdouble bpm);
gdouble ags_fifoout_get_bpm(AgsSoundcard *soundcard);

void ags_fifoout_set_start_note_offset(AgsSoundcard *soundcard,
				       guint64 start_note_offset);
guint64 ags_fifoout_get_start_note_offset(AgsSoundcard *soundcard);

GObject* ags_fifoout_get_frame_clock(AgsSoundcard *soundcard);

void* ags_fifoout_get_buffer(AgsSoundcard *soundcard);
void* ags_fifoout_get_next_buffer(AgsSoundcard *soundcard);

/**
 * SECTION:ags_fifoout
 * @short_description: Output to pipe
 * @title: AgsFifoout
 * @section_id:
 * @include: ags/audio/ags_fifoout.h
 *
 * #AgsFifoout represents a pipe and supports output.
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

static gpointer ags_fifoout_parent_class = NULL;

GType
ags_fifoout_get_type (void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fifoout = 0;

    static const GTypeInfo ags_fifoout_info = {
      sizeof(AgsFifooutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fifoout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsFifoout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fifoout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_fifoout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_fifoout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_fifoout = g_type_register_static(G_TYPE_OBJECT,
					      "AgsFifoout",
					      &ags_fifoout_info,
					      0);

    g_type_add_interface_static(ags_type_fifoout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_fifoout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_fifoout);
  }

  return(g_define_type_id__static);
}

GType
ags_fifoout_flags_get_type()
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_FIFOOUT_INITIALIZED, "AGS_FIFOOUT_INITIALIZED", "fifoout-initialized" },
      { AGS_FIFOOUT_START_PLAY, "AGS_FIFOOUT_START_PLAY", "fifoout-start-play" },
      { AGS_FIFOOUT_PLAY, "AGS_FIFOOUT_PLAY", "fifoout-play" },
      { AGS_FIFOOUT_SHUTDOWN, "AGS_FIFOOUT_SHUTDOWN", "fifoout-shutdown" },
      { AGS_FIFOOUT_NONBLOCKING, "AGS_FIFOOUT_NONBLOCKING", "fifoout-nonblocking" },
      { AGS_FIFOOUT_ATTACK_FIRST, "AGS_FIFOOUT_ATTACK_FIRST", "fifoout-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsFifooutFlags"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
}

void
ags_fifoout_class_init(AgsFifooutClass *fifoout)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fifoout_parent_class = g_type_class_peek_parent(fifoout);

  /* GObjectClass */
  gobject = (GObjectClass *) fifoout;

  gobject->set_property = ags_fifoout_set_property;
  gobject->get_property = ags_fifoout_get_property;

  gobject->dispose = ags_fifoout_dispose;
  gobject->finalize = ags_fifoout_finalize;

  /* properties */
  /**
   * AgsFifoout:device:
   *
   * The fifo soundcard indentifier
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   AGS_FIFOOUT_DEFAULT_DEVICE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsFifoout:dsp-channels:
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
   * AgsFifoout:pcm-channels:
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
   * AgsFifoout:format:
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
   * AgsFifoout:buffer-size:
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
   * AgsFifoout:samplerate:
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
   * AgsFifoout:bpm:
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
   * AgsFifoout:buffer:
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

  /* AgsFifooutClass */
}

GQuark
ags_fifoout_error_quark()
{
  return(g_quark_from_static_string("ags-fifoout-error-quark"));
}

void
ags_fifoout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_fifoout_get_uuid;
  connectable->has_resource = ags_fifoout_has_resource;

  connectable->is_ready = ags_fifoout_is_ready;
  connectable->add_to_registry = ags_fifoout_add_to_registry;
  connectable->remove_from_registry = ags_fifoout_remove_from_registry;

  connectable->list_resource = ags_fifoout_list_resource;
  connectable->xml_compose = ags_fifoout_xml_compose;
  connectable->xml_parse = ags_fifoout_xml_parse;

  connectable->is_connected = ags_fifoout_is_connected;  
  connectable->connect = ags_fifoout_connect;
  connectable->disconnect = ags_fifoout_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_fifoout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_fifoout_set_device;
  soundcard->get_device = ags_fifoout_get_device;
  
  soundcard->set_presets = ags_fifoout_set_presets;
  soundcard->get_presets = ags_fifoout_get_presets;

  soundcard->list_cards = ags_fifoout_list_cards;
  soundcard->pcm_info = ags_fifoout_pcm_info;
  soundcard->get_capability = NULL;

  soundcard->is_available = ags_fifoout_is_available;

  soundcard->is_starting =  ags_fifoout_is_starting;
  soundcard->is_playing = ags_fifoout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_fifoout_get_uptime;
  
  soundcard->play_init = ags_fifoout_fifo_init;
  soundcard->play = ags_fifoout_fifo_play;

  soundcard->record_init = NULL;
  soundcard->record = NULL;

  soundcard->stop = ags_fifoout_fifo_free;

  soundcard->tic = ags_fifoout_tic;
  soundcard->offset_changed = ags_fifoout_offset_changed;
    
  soundcard->set_bpm = ags_fifoout_set_bpm;
  soundcard->get_bpm = ags_fifoout_get_bpm;
  
  soundcard->set_start_note_offset = ags_fifoout_set_start_note_offset;
  soundcard->get_start_note_offset = ags_fifoout_get_start_note_offset;
  
  soundcard->get_frame_clock = ags_fifoout_get_frame_clock;

  soundcard->get_buffer = ags_fifoout_get_buffer;
  soundcard->get_next_buffer = ags_fifoout_get_next_buffer;
}

void
ags_fifoout_init(AgsFifoout *fifoout)
{
  AgsConfig *config;
  
  gchar *str;
  gchar *segmentation;
  
  guint denumerator, numerator;
  guint i;

  fifoout->flags = 0;
  fifoout->connectable_flags = 0;
  
  /* insert fifoout mutex */
  g_rec_mutex_init(&(fifoout->obj_mutex));
  
  /* uuid */
  fifoout->uuid = ags_uuid_alloc();
  ags_uuid_generate(fifoout->uuid);

  /* flags */
  config = ags_config_get_instance();

  /* presets */
  fifoout->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  fifoout->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  fifoout->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  fifoout->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  fifoout->format = ags_soundcard_helper_config_get_format(config);

  /* bpm */
  fifoout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;
  
  /* start note offset */
  fifoout->start_note_offset = 0;
  
  /* frame clock */
  fifoout->frame_clock = ags_frame_clock_new();
  
  /*  */
  fifoout->device = AGS_FIFOOUT_DEFAULT_DEVICE;
  fifoout->fifo_fd = -1;

  /* app buffer */
  fifoout->app_buffer_mode = AGS_FIFOOUT_APP_BUFFER_0;

  fifoout->app_buffer_mutex = (GRecMutex **) g_malloc(AGS_FIFOOUT_DEFAULT_APP_BUFFER_SIZE * sizeof(GRecMutex *));

  for(i = 0; i < AGS_FIFOOUT_DEFAULT_APP_BUFFER_SIZE; i++){
    fifoout->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(fifoout->app_buffer_mutex[i]);
  }
  
  fifoout->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  fifoout->sub_block_mutex = (GRecMutex **) g_malloc(AGS_FIFOOUT_DEFAULT_APP_BUFFER_SIZE * fifoout->sub_block_count * fifoout->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < AGS_FIFOOUT_DEFAULT_APP_BUFFER_SIZE * fifoout->sub_block_count * fifoout->pcm_channels; i++){
    fifoout->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(fifoout->sub_block_mutex[i]);
  }

  fifoout->app_buffer = (void **) g_malloc(AGS_FIFOOUT_DEFAULT_APP_BUFFER_SIZE * sizeof(void *));
  
  for(i = 0; i < AGS_FIFOOUT_DEFAULT_APP_BUFFER_SIZE; i++){
    fifoout->app_buffer[i] = NULL;
  }
  
  ags_atomic_int_set(&(fifoout->available),
		   FALSE);

  fifoout->ring_buffer_size = AGS_FIFOOUT_DEFAULT_RING_BUFFER_SIZE;
  fifoout->nth_ring_buffer = 0;

  fifoout->ring_buffer = NULL;

  ags_fifoout_realloc_buffer(fifoout);
}

void
ags_fifoout_set_property(GObject *gobject,
			 guint prop_id,
			 const GValue *value,
			 GParamSpec *param_spec)
{
  AgsFifoout *fifoout;

  GRecMutex *fifoout_mutex;

  fifoout = AGS_FIFOOUT(gobject);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);
      
      g_rec_mutex_lock(fifoout_mutex);

      fifoout->device = g_strdup(device);

      g_rec_mutex_unlock(fifoout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      g_rec_mutex_lock(fifoout_mutex);

      if(dsp_channels == fifoout->dsp_channels){
	g_rec_mutex_unlock(fifoout_mutex);
	
	return;
      }

      fifoout->dsp_channels = dsp_channels;

      g_rec_mutex_unlock(fifoout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      g_rec_mutex_lock(fifoout_mutex);

      if(pcm_channels == fifoout->pcm_channels){
	g_rec_mutex_unlock(fifoout_mutex);
	
	return;
      }

      fifoout->pcm_channels = pcm_channels;

      g_rec_mutex_unlock(fifoout_mutex);

      ags_fifoout_realloc_buffer(fifoout);
    }
    break;
  case PROP_FORMAT:
    {
      AgsSoundcardFormat format;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(fifoout_mutex);

      if(format == fifoout->format){
	g_rec_mutex_unlock(fifoout_mutex);
	
	return;
      }

      fifoout->format = format;

      g_rec_mutex_unlock(fifoout_mutex);

      ags_fifoout_realloc_buffer(fifoout);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      g_rec_mutex_lock(fifoout_mutex);

      if(buffer_size == fifoout->buffer_size){
	g_rec_mutex_unlock(fifoout_mutex);

	return;
      }

      fifoout->buffer_size = buffer_size;

      ags_frame_clock_set_buffer_size(fifoout->frame_clock,
				      buffer_size);
      
      g_rec_mutex_unlock(fifoout_mutex);

      ags_fifoout_realloc_buffer(fifoout);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      g_rec_mutex_lock(fifoout_mutex);

      if(samplerate == fifoout->samplerate){
	g_rec_mutex_unlock(fifoout_mutex);

	return;
      }

      fifoout->samplerate = samplerate;

      ags_frame_clock_set_samplerate(fifoout->frame_clock,
				     samplerate);
      
      g_rec_mutex_unlock(fifoout_mutex);
    }
    break;
  case PROP_BPM:
    {
      gdouble bpm;
      
      bpm = g_value_get_double(value);

      g_rec_mutex_lock(fifoout_mutex);

      if(bpm == fifoout->bpm){
	g_rec_mutex_unlock(fifoout_mutex);

	return;
      }

      fifoout->bpm = bpm;

      ags_frame_clock_set_bpm(fifoout->frame_clock,
			      bpm);
      
      g_rec_mutex_unlock(fifoout_mutex);
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
ags_fifoout_get_property(GObject *gobject,
			 guint prop_id,
			 GValue *value,
			 GParamSpec *param_spec)
{
  AgsFifoout *fifoout;

  GRecMutex *fifoout_mutex;

  fifoout = AGS_FIFOOUT(gobject);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(fifoout_mutex);

      g_value_set_string(value, fifoout->device);

      g_rec_mutex_unlock(fifoout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_rec_mutex_lock(fifoout_mutex);

      g_value_set_uint(value, fifoout->dsp_channels);

      g_rec_mutex_unlock(fifoout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_rec_mutex_lock(fifoout_mutex);

      g_value_set_uint(value, fifoout->pcm_channels);

      g_rec_mutex_unlock(fifoout_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(fifoout_mutex);

      g_value_set_uint(value, fifoout->format);

      g_rec_mutex_unlock(fifoout_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(fifoout_mutex);

      g_value_set_uint(value, fifoout->buffer_size);

      g_rec_mutex_unlock(fifoout_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(fifoout_mutex);

      g_value_set_uint(value, fifoout->samplerate);

      g_rec_mutex_unlock(fifoout_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(fifoout_mutex);

      g_value_set_double(value, fifoout->bpm);

      g_rec_mutex_unlock(fifoout_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(fifoout_mutex);

      g_value_set_pointer(value, fifoout->app_buffer);

      g_rec_mutex_unlock(fifoout_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_fifoout_dispose(GObject *gobject)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fifoout_parent_class)->dispose(gobject);
}

void
ags_fifoout_finalize(GObject *gobject)
{
  AgsFifoout *fifoout;

  guint i;

  fifoout = AGS_FIFOOUT(gobject);

  ags_uuid_free(fifoout->uuid);

  for(i = 0; i < AGS_FIFOOUT_DEFAULT_APP_BUFFER_SIZE; i++){
    g_free(fifoout->app_buffer[i]);
  }

  g_free(fifoout->app_buffer);

  for(i = 0; i < AGS_FIFOOUT_DEFAULT_APP_BUFFER_SIZE; i++){
    g_rec_mutex_clear(fifoout->app_buffer_mutex[i]);
    
    g_free(fifoout->app_buffer_mutex[i]);
  }

  g_free(fifoout->app_buffer_mutex);
  
  for(i = 0; i < AGS_FIFOOUT_DEFAULT_APP_BUFFER_SIZE * fifoout->sub_block_count * fifoout->pcm_channels; i++){
    g_rec_mutex_clear(fifoout->sub_block_mutex[i]);
    
    g_free(fifoout->sub_block_mutex[i]);
  }

  g_free(fifoout->sub_block_mutex);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fifoout_parent_class)->finalize(gobject);
}

AgsUUID*
ags_fifoout_get_uuid(AgsConnectable *connectable)
{
  AgsFifoout *fifoout;
  
  AgsUUID *ptr;

  GRecMutex *fifoout_mutex;

  fifoout = AGS_FIFOOUT(connectable);

  /* get fifoout signal mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* get UUID */
  g_rec_mutex_lock(fifoout_mutex);

  ptr = fifoout->uuid;

  g_rec_mutex_unlock(fifoout_mutex);
  
  return(ptr);
}

gboolean
ags_fifoout_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_fifoout_is_ready(AgsConnectable *connectable)
{
  AgsFifoout *fifoout;
  
  gboolean is_ready;

  GRecMutex *fifoout_mutex;

  fifoout = AGS_FIFOOUT(connectable);

  /* get alsa devout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* check is ready */
  g_rec_mutex_lock(fifoout_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (fifoout->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(fifoout_mutex);
  
  return(is_ready);
}

void
ags_fifoout_add_to_registry(AgsConnectable *connectable)
{
  AgsFifoout *fifoout;

  GRecMutex *fifoout_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  fifoout = AGS_FIFOOUT(connectable);

  /* get alsa devout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  g_rec_mutex_lock(fifoout_mutex);

  fifoout->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(fifoout_mutex);
}

void
ags_fifoout_remove_from_registry(AgsConnectable *connectable)
{
  AgsFifoout *fifoout;

  GRecMutex *fifoout_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  fifoout = AGS_FIFOOUT(connectable);

  /* get alsa devout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  g_rec_mutex_lock(fifoout_mutex);

  fifoout->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(fifoout_mutex);
}

xmlNode*
ags_fifoout_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_fifoout_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_fifoout_xml_parse(AgsConnectable *connectable,
		      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_fifoout_is_connected(AgsConnectable *connectable)
{
  AgsFifoout *fifoout;
  
  gboolean is_connected;

  GRecMutex *fifoout_mutex;

  fifoout = AGS_FIFOOUT(connectable);

  /* get alsa devout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* check is connected */
  g_rec_mutex_lock(fifoout_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (fifoout->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(fifoout_mutex);
  
  return(is_connected);
}

void
ags_fifoout_connect(AgsConnectable *connectable)
{
  AgsFifoout *fifoout;

  GRecMutex *fifoout_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  fifoout = AGS_FIFOOUT(connectable);

  /* get alsa devout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  g_rec_mutex_lock(fifoout_mutex);

  fifoout->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(fifoout_mutex);
}

void
ags_fifoout_disconnect(AgsConnectable *connectable)
{
  AgsFifoout *fifoout;

  GRecMutex *fifoout_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  fifoout = AGS_FIFOOUT(connectable);

  /* get alsa devout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  g_rec_mutex_lock(fifoout_mutex);

  fifoout->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(fifoout_mutex);
}

/**
 * ags_fifoout_test_flags:
 * @fifoout: the #AgsFifoout
 * @flags: the flags
 *
 * Test @flags to be set on @fifoout.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_fifoout_test_flags(AgsFifoout *fifoout, AgsFifooutFlags flags)
{
  gboolean retval;  
  
  GRecMutex *fifoout_mutex;

  if(!AGS_IS_FIFOOUT(fifoout)){
    return(FALSE);
  }

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* test */
  g_rec_mutex_lock(fifoout_mutex);

  retval = (flags & (fifoout->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(fifoout_mutex);

  return(retval);
}

/**
 * ags_fifoout_set_flags:
 * @fifoout: the #AgsFifoout
 * @flags: see #AgsFifooutFlags-enum
 *
 * Enable a feature of @fifoout.
 *
 * Since: 3.0.0
 */
void
ags_fifoout_set_flags(AgsFifoout *fifoout, AgsFifooutFlags flags)
{
  GRecMutex *fifoout_mutex;

  if(!AGS_IS_FIFOOUT(fifoout)){
    return;
  }

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(fifoout_mutex);

  fifoout->flags |= flags;
  
  g_rec_mutex_unlock(fifoout_mutex);
}
    
/**
 * ags_fifoout_unset_flags:
 * @fifoout: the #AgsFifoout
 * @flags: see #AgsFifooutFlags-enum
 *
 * Disable a feature of @fifoout.
 *
 * Since: 3.0.0
 */
void
ags_fifoout_unset_flags(AgsFifoout *fifoout, AgsFifooutFlags flags)
{  
  GRecMutex *fifoout_mutex;

  if(!AGS_IS_FIFOOUT(fifoout)){
    return;
  }

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(fifoout_mutex);

  fifoout->flags &= (~flags);
  
  g_rec_mutex_unlock(fifoout_mutex);
}


void
ags_fifoout_set_device(AgsSoundcard *soundcard,
		       gchar *device)
{
  AgsFifoout *fifoout;

  GRecMutex *fifoout_mutex;

  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* set device */
  g_rec_mutex_lock(fifoout_mutex);

  fifoout->device = g_strdup(device);

  g_rec_mutex_unlock(fifoout_mutex);
}

gchar*
ags_fifoout_get_device(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  gchar *device;

  GRecMutex *fifoout_mutex;
  
  fifoout = AGS_FIFOOUT(soundcard);
  
  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* get device */
  g_rec_mutex_lock(fifoout_mutex);

  device = fifoout->device;
  
  g_rec_mutex_unlock(fifoout_mutex);

  return(device);
}

void
ags_fifoout_set_presets(AgsSoundcard *soundcard,
			guint channels,
			guint rate,
			guint buffer_size,
			AgsSoundcardFormat format)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);
  
  g_object_set(fifoout,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_fifoout_get_presets(AgsSoundcard *soundcard,
			guint *channels,
			guint *rate,
			guint *buffer_size,
			AgsSoundcardFormat *format)
{
  AgsFifoout *fifoout;

  GRecMutex *fifoout_mutex;

  fifoout = AGS_FIFOOUT(soundcard);


  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* get presets */
  g_rec_mutex_lock(fifoout_mutex);

  if(channels != NULL){
    *channels = fifoout->pcm_channels;
  }

  if(rate != NULL){
    *rate = fifoout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = fifoout->buffer_size;
  }

  if(format != NULL){
    *format = fifoout->format;
  }

  g_rec_mutex_unlock(fifoout_mutex);
}

void
ags_fifoout_list_cards(AgsSoundcard *soundcard,
		       GList **card_id, GList **card_name)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);

  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  //TODO:JK: implement me
  
  if(card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

void
ags_fifoout_pcm_info(AgsSoundcard *soundcard,
		     char *card_id,
		     guint *channels_min, guint *channels_max,
		     guint *rate_min, guint *rate_max,
		     guint *buffer_size_min, guint *buffer_size_max,
		     GError **error)
{
  AgsFifoout *fifoout;

  fifoout = AGS_FIFOOUT(soundcard);
  
  //TODO:JK: implement me  
}

gboolean
ags_fifoout_is_available(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;
  
  //TODO:JK: implement me  

  return(TRUE);
}

gboolean
ags_fifoout_is_starting(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  gboolean is_starting;
  
  GRecMutex *fifoout_mutex;
  
  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* check is starting */
  g_rec_mutex_lock(fifoout_mutex);

  is_starting = ((AGS_FIFOOUT_START_PLAY & (fifoout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(fifoout_mutex);
  
  return(is_starting);
}

gboolean
ags_fifoout_is_playing(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  gboolean is_playing;
  
  GRecMutex *fifoout_mutex;

  fifoout = AGS_FIFOOUT(soundcard);
  
  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* check is starting */
  g_rec_mutex_lock(fifoout_mutex);

  is_playing = ((AGS_FIFOOUT_PLAY & (fifoout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(fifoout_mutex);

  return(is_playing);
}

gchar*
ags_fifoout_get_uptime(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  gchar *uptime;

  GRecMutex *fifoout_mutex;
  
  fifoout = AGS_FIFOOUT(soundcard);
  
  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* get time string */
  g_rec_mutex_lock(fifoout_mutex);
  
  uptime = ags_frame_clock_to_time_string(fifoout->frame_clock);

  g_rec_mutex_unlock(fifoout_mutex);
  
  return(uptime);
}

void
ags_fifoout_fifo_init(AgsSoundcard *soundcard,
		      GError **error)
{
  //TODO:JK: implement me  
}

void
ags_fifoout_fifo_play(AgsSoundcard *soundcard,
		      GError **error)
{
  //TODO:JK: implement me  
}

void
ags_fifoout_fifo_free(AgsSoundcard *soundcard)
{
  //TODO:JK: implement me  
}

void
ags_fifoout_tic(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  GRecMutex *fifoout_mutex;
  
  fifoout = AGS_FIFOOUT(soundcard);
  
  /* get fifoout devout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* frame clock */
  g_rec_mutex_lock(fifoout_mutex);
  
  ags_frame_clock_increment_counter(fifoout->frame_clock);

  /* 16th pulse */
  if(ags_frame_clock_get_has_16th_pulse(fifoout->frame_clock)){
    ags_soundcard_offset_changed(soundcard,
				 fifoout->frame_clock->note_offset);
  }
  
  g_rec_mutex_unlock(fifoout_mutex);
}

void
ags_fifoout_offset_changed(AgsSoundcard *soundcard,
			   guint64 note_offset)
{
  //empty
}

void
ags_fifoout_set_bpm(AgsSoundcard *soundcard,
		    gdouble bpm)
{
  AgsFifoout *fifoout;

  GRecMutex *fifoout_mutex;
  
  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* set bpm */
  g_rec_mutex_lock(fifoout_mutex);

  fifoout->bpm = bpm;

  ags_frame_clock_set_bpm(fifoout->frame_clock,
			  bpm);
  
  g_rec_mutex_unlock(fifoout_mutex);
}

gdouble
ags_fifoout_get_bpm(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  gdouble bpm;
  
  GRecMutex *fifoout_mutex;
  
  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* get bpm */
  g_rec_mutex_lock(fifoout_mutex);

  bpm = fifoout->bpm;
  
  ags_frame_clock_set_bpm(fifoout->frame_clock,
			  bpm);
  
  g_rec_mutex_unlock(fifoout_mutex);

  return(bpm);
}

void
ags_fifoout_set_start_note_offset(AgsSoundcard *soundcard,
				  guint64 start_note_offset)
{
  AgsFifoout *fifoout;

  GRecMutex *fifoout_mutex;
  
  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* set delay factor */
  g_rec_mutex_lock(fifoout_mutex);

  fifoout->start_note_offset = start_note_offset;

  ags_frame_clock_set_start_note_offset(fifoout->frame_clock,
					start_note_offset);
  
  g_rec_mutex_unlock(fifoout_mutex);
}

guint64
ags_fifoout_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  guint64 start_note_offset;
  
  GRecMutex *fifoout_mutex;
  
  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* get delay factor */
  g_rec_mutex_lock(fifoout_mutex);

  start_note_offset = fifoout->start_note_offset;
  
  g_rec_mutex_unlock(fifoout_mutex);

  return(start_note_offset);
}

GObject*
ags_fifoout_get_frame_clock(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  GObject *frame_clock;
  
  GRecMutex *fifoout_mutex;
  
  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* get frame clock */
  g_rec_mutex_lock(fifoout_mutex);

  frame_clock = (GObject *) fifoout->frame_clock;
  
  g_rec_mutex_unlock(fifoout_mutex);
  
  return(frame_clock);
}

void*
ags_fifoout_get_buffer(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  void *buffer;
  
  fifoout = AGS_FIFOOUT(soundcard);

  buffer = fifoout->app_buffer[fifoout->app_buffer_mode];

  return(buffer);
}

void*
ags_fifoout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  void *buffer;
  
  fifoout = AGS_FIFOOUT(soundcard);

  if(fifoout->app_buffer_mode == AGS_FIFOOUT_APP_BUFFER_3){
    buffer = fifoout->app_buffer[AGS_FIFOOUT_APP_BUFFER_0];
  }else{
    buffer = fifoout->app_buffer[fifoout->app_buffer_mode + 1];
  }

  return(buffer);
}

void*
ags_fifoout_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  void *buffer;
  
  fifoout = AGS_FIFOOUT(soundcard);

  if(fifoout->app_buffer_mode == AGS_FIFOOUT_APP_BUFFER_0){
    buffer = fifoout->app_buffer[AGS_FIFOOUT_APP_BUFFER_3];
  }else{
    buffer = fifoout->app_buffer[fifoout->app_buffer_mode - 1];
  }

  return(buffer);
}

/**
 * ags_fifoout_switch_buffer_flag:
 * @fifoout: the #AgsFifoout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.0.0
 */
void
ags_fifoout_switch_buffer_flag(AgsFifoout *fifoout)
{
  GRecMutex *fifoout_mutex;
  
  if(!AGS_IS_FIFOOUT(fifoout)){
    return;
  }

  /* get alsa devout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* switch buffer flag */
  g_rec_mutex_lock(fifoout_mutex);

  if(fifoout->app_buffer_mode < AGS_FIFOOUT_APP_BUFFER_3){
    fifoout->app_buffer_mode += 1;
  }else{
    fifoout->app_buffer_mode = AGS_FIFOOUT_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(fifoout_mutex);
}

/**
 * ags_fifoout_realloc_buffer:
 * @fifoout: the #AgsFifoout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.0.0
 */
void
ags_fifoout_realloc_buffer(AgsFifoout *fifoout)
{
  guint pcm_channels;
  guint buffer_size;
  guint word_size;
  
  GRecMutex *fifoout_mutex;  

  if(!AGS_IS_FIFOOUT(fifoout)){
    return;
  }

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* get word size */  
  g_rec_mutex_lock(fifoout_mutex);

  pcm_channels = fifoout->pcm_channels;
  buffer_size = fifoout->buffer_size;
  
  switch(fifoout->format){
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
    g_warning("ags_fifoout_realloc_buffer(): unsupported word size");
    return;
  }  

  g_rec_mutex_unlock(fifoout_mutex);

  //NOTE:JK: there is no lock applicable to buffer
  
  /* AGS_FIFOOUT_APP_BUFFER_0 */
  if(fifoout->app_buffer[0] != NULL){
    g_free(fifoout->app_buffer[0]);
  }
  
  fifoout->app_buffer[0] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_FIFOOUT_APP_BUFFER_1 */
  if(fifoout->app_buffer[1] != NULL){
    g_free(fifoout->app_buffer[1]);
  }

  fifoout->app_buffer[1] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_FIFOOUT_APP_BUFFER_2 */
  if(fifoout->app_buffer[2] != NULL){
    g_free(fifoout->app_buffer[2]);
  }

  fifoout->app_buffer[2] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_FIFOOUT_APP_BUFFER_3 */
  if(fifoout->app_buffer[3] != NULL){
    g_free(fifoout->app_buffer[3]);
  }
  
  fifoout->app_buffer[3] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_fifoout_new:
 *
 * Creates a new instance of #AgsFifoout.
 *
 * Returns: the new #AgsFifoout
 *
 * Since: 3.0.0
 */
AgsFifoout*
ags_fifoout_new()
{
  AgsFifoout *fifoout;

  fifoout = (AgsFifoout *) g_object_new(AGS_TYPE_FIFOOUT,
					NULL);
  
  return(fifoout);
}
