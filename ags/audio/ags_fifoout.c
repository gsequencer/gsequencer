/* GSequencer - Advanced GTK Sequencer
  * Copyright (C) 2005-2019 Joël Krähemann
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
			     guint format);
void ags_fifoout_get_presets(AgsSoundcard *soundcard,
			     guint *channels,
			     guint *rate,
			     guint *buffer_size,
			     guint *format);

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
				guint note_offset);

void ags_fifoout_set_bpm(AgsSoundcard *soundcard,
			 gdouble bpm);
gdouble ags_fifoout_get_bpm(AgsSoundcard *soundcard);

void ags_fifoout_set_delay_factor(AgsSoundcard *soundcard,
				  gdouble delay_factor);
gdouble ags_fifoout_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_fifoout_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_fifoout_get_delay(AgsSoundcard *soundcard);
guint ags_fifoout_get_attack(AgsSoundcard *soundcard);

void* ags_fifoout_get_buffer(AgsSoundcard *soundcard);
void* ags_fifoout_get_next_buffer(AgsSoundcard *soundcard);

guint ags_fifoout_get_delay_counter(AgsSoundcard *soundcard);

void ags_fifoout_set_note_offset(AgsSoundcard *soundcard,
				 guint note_offset);
guint ags_fifoout_get_note_offset(AgsSoundcard *soundcard);

void ags_fifoout_set_note_offset_absolute(AgsSoundcard *soundcard,
					  guint note_offset);
guint ags_fifoout_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_fifoout_set_loop(AgsSoundcard *soundcard,
			  guint loop_left, guint loop_right,
			  gboolean do_loop);
void ags_fifoout_get_loop(AgsSoundcard *soundcard,
			  guint *loop_left, guint *loop_right,
			  gboolean *do_loop);

guint ags_fifoout_get_loop_offset(AgsSoundcard *soundcard);

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
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
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

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fifoout);
  }

  return g_define_type_id__volatile;
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
   * AgsFifoout:delay-factor:
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
   * AgsFifoout:attack:
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

  soundcard->set_delay_factor = ags_fifoout_set_delay_factor;
  soundcard->get_delay_factor = ags_fifoout_get_delay_factor;
  
  soundcard->get_absolute_delay = ags_fifoout_get_absolute_delay;

  soundcard->get_delay = ags_fifoout_get_delay;
  soundcard->get_attack = ags_fifoout_get_attack;

  soundcard->get_buffer = ags_fifoout_get_buffer;
  soundcard->get_next_buffer = ags_fifoout_get_next_buffer;

  soundcard->get_delay_counter = ags_fifoout_get_delay_counter;

  soundcard->set_note_offset = ags_fifoout_set_note_offset;
  soundcard->get_note_offset = ags_fifoout_get_note_offset;

  soundcard->set_note_offset_absolute = ags_fifoout_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_fifoout_get_note_offset_absolute;

  soundcard->set_loop = ags_fifoout_set_loop;
  soundcard->get_loop = ags_fifoout_get_loop;

  soundcard->get_loop_offset = ags_fifoout_get_loop_offset;
}

void
ags_fifoout_init(AgsFifoout *fifoout)
{
  AgsConfig *config;
  
  gchar *str;
  gchar *segmentation;
  
  guint denumerator, numerator;

  fifoout->flags = 0;
  
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

  /*  */
  fifoout->device = AGS_FIFOOUT_DEFAULT_DEVICE;
  fifoout->fifo_fd = -1;

  /* buffer */
  fifoout->buffer = (void **) malloc(4 * sizeof(void*));

  fifoout->buffer[0] = NULL;
  fifoout->buffer[1] = NULL;
  fifoout->buffer[2] = NULL;
  fifoout->buffer[3] = NULL;
  
  g_atomic_int_set(&(fifoout->available),
		   FALSE);

  fifoout->ring_buffer_size = AGS_FIFOOUT_DEFAULT_RING_BUFFER_SIZE;
  fifoout->nth_ring_buffer = 0;

  fifoout->ring_buffer = NULL;

  ags_fifoout_realloc_buffer(fifoout);
  
  /* bpm */
  fifoout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  fifoout->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denumerator,
	   &numerator);
    
    fifoout->delay_factor = 1.0 / numerator * (numerator / denumerator);

    g_free(segmentation);
  }

  /* delay and attack */
  fifoout->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
				      sizeof(gdouble));
  
  fifoout->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
				     sizeof(guint));

  ags_fifoout_adjust_delay_and_attack(fifoout);
  
  /* counters */
  fifoout->tact_counter = 0.0;
  fifoout->delay_counter = 0;
  fifoout->tic_counter = 0;

  fifoout->note_offset = 0;
  fifoout->note_offset_absolute = 0;

  fifoout->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  fifoout->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  fifoout->do_loop = FALSE;

  fifoout->loop_offset = 0;
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
      guint format;

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

      g_rec_mutex_unlock(fifoout_mutex);

      ags_fifoout_realloc_buffer(fifoout);
      ags_fifoout_adjust_delay_and_attack(fifoout);
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

      g_rec_mutex_unlock(fifoout_mutex);

      ags_fifoout_adjust_delay_and_attack(fifoout);
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

      g_rec_mutex_lock(fifoout_mutex);

      if(bpm == fifoout->bpm){
	g_rec_mutex_unlock(fifoout_mutex);

	return;
      }

      fifoout->bpm = bpm;

      g_rec_mutex_unlock(fifoout_mutex);

      ags_fifoout_adjust_delay_and_attack(fifoout);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      g_rec_mutex_lock(fifoout_mutex);

      if(delay_factor == fifoout->delay_factor){
	g_rec_mutex_unlock(fifoout_mutex);

	return;
      }

      fifoout->delay_factor = delay_factor;

      g_rec_mutex_unlock(fifoout_mutex);

      ags_fifoout_adjust_delay_and_attack(fifoout);
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
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(fifoout_mutex);

      g_value_set_pointer(value, fifoout->buffer);

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
  case PROP_DELAY_FACTOR:
    {
      g_rec_mutex_lock(fifoout_mutex);

      g_value_set_double(value, fifoout->delay_factor);

      g_rec_mutex_unlock(fifoout_mutex);
    }
    break;
  case PROP_ATTACK:
    {
      g_rec_mutex_lock(fifoout_mutex);

      g_value_set_pointer(value, fifoout->attack);

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

  fifoout = AGS_FIFOOUT(gobject);

  /* free output buffer */
  free(fifoout->buffer[0]);
  free(fifoout->buffer[1]);
  free(fifoout->buffer[2]);
  free(fifoout->buffer[3]);

  /* free buffer array */
  free(fifoout->buffer);

  /* free AgsAttack */
  free(fifoout->attack);
  
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

  fifoout = AGS_FIFOOUT(connectable);

  /* check is added */
  is_ready = ags_fifoout_test_flags(fifoout, AGS_FIFOOUT_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_fifoout_add_to_registry(AgsConnectable *connectable)
{
  AgsFifoout *fifoout;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  fifoout = AGS_FIFOOUT(connectable);

  ags_fifoout_set_flags(fifoout, AGS_FIFOOUT_ADDED_TO_REGISTRY);
}

void
ags_fifoout_remove_from_registry(AgsConnectable *connectable)
{
  AgsFifoout *fifoout;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  fifoout = AGS_FIFOOUT(connectable);

  ags_fifoout_unset_flags(fifoout, AGS_FIFOOUT_ADDED_TO_REGISTRY);
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

  fifoout = AGS_FIFOOUT(connectable);

  /* check is connected */
  is_connected = ags_fifoout_test_flags(fifoout, AGS_FIFOOUT_CONNECTED);
  
  return(is_connected);
}

void
ags_fifoout_connect(AgsConnectable *connectable)
{
  AgsFifoout *fifoout;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  fifoout = AGS_FIFOOUT(connectable);

  ags_fifoout_set_flags(fifoout, AGS_FIFOOUT_CONNECTED);
}

void
ags_fifoout_disconnect(AgsConnectable *connectable)
{

  AgsFifoout *fifoout;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  fifoout = AGS_FIFOOUT(connectable);
  
  ags_fifoout_unset_flags(fifoout, AGS_FIFOOUT_CONNECTED);
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
ags_fifoout_test_flags(AgsFifoout *fifoout, guint flags)
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
ags_fifoout_set_flags(AgsFifoout *fifoout, guint flags)
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
ags_fifoout_unset_flags(AgsFifoout *fifoout, guint flags)
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
			guint format)
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
			guint *format)
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
    
    note_offset = ags_soundcard_get_note_offset(soundcard);

    bpm = ags_soundcard_get_bpm(soundcard);
    delay_factor = ags_soundcard_get_delay_factor(soundcard);

    /* calculate delays */
    delay = ((gdouble) samplerate / (gdouble) buffer_size) * (gdouble)(60.0 / bpm) * delay_factor;
  
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

  gdouble delay;
  gdouble delay_counter;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  
  GRecMutex *fifoout_mutex;
  
  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(fifoout_mutex);

  delay = fifoout->delay[fifoout->tic_counter];
  delay_counter = fifoout->delay_counter;

  note_offset = fifoout->note_offset;
  note_offset_absolute = fifoout->note_offset_absolute;
  
  loop_left = fifoout->loop_left;
  loop_right = fifoout->loop_right;
  
  do_loop = fifoout->do_loop;

  g_rec_mutex_unlock(fifoout_mutex);

  if((guint) delay_counter + 1 >= (guint) delay){
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
    g_rec_mutex_lock(fifoout_mutex);
    
    fifoout->delay_counter = 0.0;
    fifoout->tact_counter += 1.0;

    g_rec_mutex_unlock(fifoout_mutex);
  }else{
    g_rec_mutex_lock(fifoout_mutex);
    
    fifoout->delay_counter += 1.0;

    g_rec_mutex_unlock(fifoout_mutex);
  }
}

void
ags_fifoout_offset_changed(AgsSoundcard *soundcard,
			   guint note_offset)
{
  AgsFifoout *fifoout;
  
  GRecMutex *fifoout_mutex;
  
  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* offset changed */
  g_rec_mutex_lock(fifoout_mutex);

  fifoout->tic_counter += 1;

  if(fifoout->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    fifoout->tic_counter = 0;
  }

  g_rec_mutex_unlock(fifoout_mutex);
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

  g_rec_mutex_unlock(fifoout_mutex);

  ags_fifoout_adjust_delay_and_attack(fifoout);
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
  
  g_rec_mutex_unlock(fifoout_mutex);

  return(bpm);
}

void
ags_fifoout_set_delay_factor(AgsSoundcard *soundcard,
			     gdouble delay_factor)
{
  AgsFifoout *fifoout;

  GRecMutex *fifoout_mutex;
  
  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* set delay factor */
  g_rec_mutex_lock(fifoout_mutex);

  fifoout->delay_factor = delay_factor;

  g_rec_mutex_unlock(fifoout_mutex);

  ags_fifoout_adjust_delay_and_attack(fifoout);
}

gdouble
ags_fifoout_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  gdouble delay_factor;
  
  GRecMutex *fifoout_mutex;
  
  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* get delay factor */
  g_rec_mutex_lock(fifoout_mutex);

  delay_factor = fifoout->delay_factor;
  
  g_rec_mutex_unlock(fifoout_mutex);

  return(delay_factor);
}

gdouble
ags_fifoout_get_delay(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  guint index;
  gdouble delay;
  
  GRecMutex *fifoout_mutex;
  
  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* get delay */
  g_rec_mutex_lock(fifoout_mutex);

  index = fifoout->tic_counter;

  delay = fifoout->delay[index];
  
  g_rec_mutex_unlock(fifoout_mutex);
  
  return(delay);
}

gdouble
ags_fifoout_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  gdouble absolute_delay;
  
  GRecMutex *fifoout_mutex;
  
  fifoout = AGS_FIFOOUT(soundcard);
  
  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* get absolute delay */
  g_rec_mutex_lock(fifoout_mutex);

  absolute_delay = (60.0 * (((gdouble) fifoout->samplerate / (gdouble) fifoout->buffer_size) / (gdouble) fifoout->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) fifoout->delay_factor)));

  g_rec_mutex_unlock(fifoout_mutex);

  return(absolute_delay);
}

guint
ags_fifoout_get_attack(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  guint index;
  guint attack;
  
  GRecMutex *fifoout_mutex;  

  fifoout = AGS_FIFOOUT(soundcard);
  
  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* get attack */
  g_rec_mutex_lock(fifoout_mutex);

  index = fifoout->tic_counter;

  attack = fifoout->attack[index];

  g_rec_mutex_unlock(fifoout_mutex);
  
  return(attack);
}

void*
ags_fifoout_get_buffer(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  void *buffer;
  
  fifoout = AGS_FIFOOUT(soundcard);

  if(ags_fifoout_test_flags(fifoout, AGS_FIFOOUT_BUFFER0)){
    buffer = fifoout->buffer[0];
  }else if(ags_fifoout_test_flags(fifoout, AGS_FIFOOUT_BUFFER1)){
    buffer = fifoout->buffer[1];
  }else if(ags_fifoout_test_flags(fifoout, AGS_FIFOOUT_BUFFER2)){
    buffer = fifoout->buffer[2];
  }else if(ags_fifoout_test_flags(fifoout, AGS_FIFOOUT_BUFFER3)){
    buffer = fifoout->buffer[3];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_fifoout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  void *buffer;
  
  fifoout = AGS_FIFOOUT(soundcard);

  //  g_message("next - 0x%0x", ((AGS_FIFOOUT_BUFFER0 |
  //				AGS_FIFOOUT_BUFFER1 |
  //				AGS_FIFOOUT_BUFFER2 |
  //				AGS_FIFOOUT_BUFFER3) & (fifoout->flags)));

  if(ags_fifoout_test_flags(fifoout, AGS_FIFOOUT_BUFFER0)){
    buffer = fifoout->buffer[1];
  }else if(ags_fifoout_test_flags(fifoout, AGS_FIFOOUT_BUFFER1)){
    buffer = fifoout->buffer[2];
  }else if(ags_fifoout_test_flags(fifoout, AGS_FIFOOUT_BUFFER2)){
    buffer = fifoout->buffer[3];
  }else if(ags_fifoout_test_flags(fifoout, AGS_FIFOOUT_BUFFER3)){
    buffer = fifoout->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_fifoout_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  void *buffer;
  
  fifoout = AGS_FIFOOUT(soundcard);

  if(ags_fifoout_test_flags(fifoout, AGS_FIFOOUT_BUFFER0)){
    buffer = fifoout->buffer[3];
  }else if(ags_fifoout_test_flags(fifoout, AGS_FIFOOUT_BUFFER1)){
    buffer = fifoout->buffer[0];
  }else if(ags_fifoout_test_flags(fifoout, AGS_FIFOOUT_BUFFER2)){
    buffer = fifoout->buffer[1];
  }else if(ags_fifoout_test_flags(fifoout, AGS_FIFOOUT_BUFFER3)){
    buffer = fifoout->buffer[2];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

guint
ags_fifoout_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  guint delay_counter;
  
  GRecMutex *fifoout_mutex;  

  fifoout = AGS_FIFOOUT(soundcard);
  
  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* delay counter */
  g_rec_mutex_lock(fifoout_mutex);

  delay_counter = fifoout->delay_counter;
  
  g_rec_mutex_unlock(fifoout_mutex);

  return(delay_counter);
}

void
ags_fifoout_set_note_offset(AgsSoundcard *soundcard,
			    guint note_offset)
{
  AgsFifoout *fifoout;

  GRecMutex *fifoout_mutex;  

  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* set note offset */
  g_rec_mutex_lock(fifoout_mutex);

  fifoout->note_offset = note_offset;

  g_rec_mutex_unlock(fifoout_mutex);
}

guint
ags_fifoout_get_note_offset(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  guint note_offset;
  
  GRecMutex *fifoout_mutex;  

  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* set note offset */
  g_rec_mutex_lock(fifoout_mutex);

  note_offset = fifoout->note_offset;

  g_rec_mutex_unlock(fifoout_mutex);

  return(note_offset);
}

void
ags_fifoout_set_note_offset_absolute(AgsSoundcard *soundcard,
				     guint note_offset_absolute)
{
  AgsFifoout *fifoout;
  
  GRecMutex *fifoout_mutex;  

  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* set note offset */
  g_rec_mutex_lock(fifoout_mutex);

  fifoout->note_offset_absolute = note_offset_absolute;

  g_rec_mutex_unlock(fifoout_mutex);
}

guint
ags_fifoout_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  guint note_offset_absolute;
  
  GRecMutex *fifoout_mutex;  

  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* set note offset */
  g_rec_mutex_lock(fifoout_mutex);

  note_offset_absolute = fifoout->note_offset_absolute;

  g_rec_mutex_unlock(fifoout_mutex);

  return(note_offset_absolute);
}

void
ags_fifoout_set_loop(AgsSoundcard *soundcard,
		     guint loop_left, guint loop_right,
		     gboolean do_loop)
{
  AgsFifoout *fifoout;

  GRecMutex *fifoout_mutex;  

  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* set loop */
  g_rec_mutex_lock(fifoout_mutex);

  fifoout->loop_left = loop_left;
  fifoout->loop_right = loop_right;
  fifoout->do_loop = do_loop;

  if(do_loop){
    fifoout->loop_offset = fifoout->note_offset;
  }

  g_rec_mutex_unlock(fifoout_mutex);
}

void
ags_fifoout_get_loop(AgsSoundcard *soundcard,
		     guint *loop_left, guint *loop_right,
		     gboolean *do_loop)
{
  AgsFifoout *fifoout;

  GRecMutex *fifoout_mutex;  

  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* get loop */
  g_rec_mutex_lock(fifoout_mutex);

  if(loop_left != NULL){
    *loop_left = fifoout->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = fifoout->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = fifoout->do_loop;
  }

  g_rec_mutex_unlock(fifoout_mutex);
}

guint
ags_fifoout_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsFifoout *fifoout;

  guint loop_offset;
  
  GRecMutex *fifoout_mutex;  

  fifoout = AGS_FIFOOUT(soundcard);

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* get loop offset */
  g_rec_mutex_lock(fifoout_mutex);

  loop_offset = fifoout->loop_offset;
  
  g_rec_mutex_unlock(fifoout_mutex);

  return(loop_offset);
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

  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);

  /* switch buffer flag */
  g_rec_mutex_lock(fifoout_mutex);

  if((AGS_FIFOOUT_BUFFER0 & (fifoout->flags)) != 0){
    fifoout->flags &= (~AGS_FIFOOUT_BUFFER0);
    fifoout->flags |= AGS_FIFOOUT_BUFFER1;
  }else if((AGS_FIFOOUT_BUFFER1 & (fifoout->flags)) != 0){
    fifoout->flags &= (~AGS_FIFOOUT_BUFFER1);
    fifoout->flags |= AGS_FIFOOUT_BUFFER2;
  }else if((AGS_FIFOOUT_BUFFER2 & (fifoout->flags)) != 0){
    fifoout->flags &= (~AGS_FIFOOUT_BUFFER2);
    fifoout->flags |= AGS_FIFOOUT_BUFFER3;
  }else if((AGS_FIFOOUT_BUFFER3 & (fifoout->flags)) != 0){
    fifoout->flags &= (~AGS_FIFOOUT_BUFFER3);
    fifoout->flags |= AGS_FIFOOUT_BUFFER0;
  }

  g_rec_mutex_unlock(fifoout_mutex);
}

/**
 * ags_fifoout_adjust_delay_and_attack:
 * @fifoout: the #AgsFifoout
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 3.0.0
 */
void
ags_fifoout_adjust_delay_and_attack(AgsFifoout *fifoout)
{
  gdouble delay;
  guint default_tact_frames;
  guint default_period;
  guint i;

  GRecMutex *fifoout_mutex;

  if(!AGS_IS_FIFOOUT(fifoout)){
    return;
  }
  
  /* get fifoout mutex */
  fifoout_mutex = AGS_FIFOOUT_GET_OBJ_MUTEX(fifoout);
  
  /* get some initial values */
  delay = (60.0 * (((gdouble) fifoout->samplerate / (gdouble) fifoout->buffer_size) / (gdouble) fifoout->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) fifoout->delay_factor)));

#ifdef AGS_DEBUG
  g_message("delay : %f", delay);
#endif
  
  g_rec_mutex_lock(fifoout_mutex);

  default_tact_frames = (guint) (delay * fifoout->buffer_size);
  default_period = (1.0 / AGS_SOUNDCARD_DEFAULT_PERIOD) * (default_tact_frames);

  fifoout->attack[0] = 0;
  fifoout->delay[0] = delay;
  
  for(i = 1; i < (int)  2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    fifoout->attack[i] = (guint) ((i * default_tact_frames + fifoout->attack[i - 1]) / (AGS_SOUNDCARD_DEFAULT_PERIOD / (delay * i))) % (guint) (fifoout->buffer_size);
    
#ifdef AGS_DEBUG
    g_message("%d", fifoout->attack[i]);
#endif
  }
  
  for(i = 1; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    fifoout->delay[i] = ((gdouble) (default_tact_frames + fifoout->attack[i])) / (gdouble) fifoout->buffer_size;
    
#ifdef AGS_DEBUG
    g_message("%f", fifoout->delay[i]);
#endif
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
  
  /* AGS_FIFOOUT_BUFFER_0 */
  if(fifoout->buffer[0] != NULL){
    free(fifoout->buffer[0]);
  }
  
  fifoout->buffer[0] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_FIFOOUT_BUFFER_1 */
  if(fifoout->buffer[1] != NULL){
    free(fifoout->buffer[1]);
  }

  fifoout->buffer[1] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_FIFOOUT_BUFFER_2 */
  if(fifoout->buffer[2] != NULL){
    free(fifoout->buffer[2]);
  }

  fifoout->buffer[2] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_FIFOOUT_BUFFER_3 */
  if(fifoout->buffer[3] != NULL){
    free(fifoout->buffer[3]);
  }
  
  fifoout->buffer[3] = (void *) malloc(pcm_channels * buffer_size * word_size);
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
