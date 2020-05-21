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

#include <ags/audio/ags_devin.h>

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

void ags_devin_class_init(AgsDevinClass *devin);
void ags_devin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_devin_init(AgsDevin *devin);
void ags_devin_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec);
void ags_devin_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec);
void ags_devin_dispose(GObject *gobject);
void ags_devin_finalize(GObject *gobject);

AgsUUID* ags_devin_get_uuid(AgsConnectable *connectable);
gboolean ags_devin_has_resource(AgsConnectable *connectable);
gboolean ags_devin_is_ready(AgsConnectable *connectable);
void ags_devin_add_to_registry(AgsConnectable *connectable);
void ags_devin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_devin_list_resource(AgsConnectable *connectable);
xmlNode* ags_devin_xml_compose(AgsConnectable *connectable);
void ags_devin_xml_parse(AgsConnectable *connectable,
			 xmlNode *node);
gboolean ags_devin_is_connected(AgsConnectable *connectable);
void ags_devin_connect(AgsConnectable *connectable);
void ags_devin_disconnect(AgsConnectable *connectable);

void ags_devin_set_device(AgsSoundcard *soundcard,
			  gchar *device);
gchar* ags_devin_get_device(AgsSoundcard *soundcard);

void ags_devin_set_presets(AgsSoundcard *soundcard,
			   guint channels,
			   guint rate,
			   guint buffer_size,
			   guint format);
void ags_devin_get_presets(AgsSoundcard *soundcard,
			   guint *channels,
			   guint *rate,
			   guint *buffer_size,
			   guint *format);

void ags_devin_list_cards(AgsSoundcard *soundcard,
			  GList **card_id, GList **card_name);
void ags_devin_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			guint *channels_min, guint *channels_max,
			guint *rate_min, guint *rate_max,
			guint *buffer_size_min, guint *buffer_size_max,
			GError **error);
guint ags_devin_get_capability(AgsSoundcard *soundcard);

gboolean ags_devin_is_available(AgsSoundcard *soundcard);

gboolean ags_devin_is_starting(AgsSoundcard *soundcard);
gboolean ags_devin_is_recording(AgsSoundcard *soundcard);

gchar* ags_devin_get_uptime(AgsSoundcard *soundcard);

void ags_devin_delegate_record_init(AgsSoundcard *soundcard,
				    GError **error);
void ags_devin_delegate_record(AgsSoundcard *soundcard,
			       GError **error);
void ags_devin_delegate_stop(AgsSoundcard *soundcard);

void ags_devin_oss_init(AgsSoundcard *soundcard,
			GError **error);

void ags_devin_oss_record_fill_buffer(void *buffer, guint ags_format, unsigned char *ring_buffer, guint channels, guint buffer_size);

void ags_devin_oss_record(AgsSoundcard *soundcard,
			  GError **error);
void ags_devin_oss_free(AgsSoundcard *soundcard);

void ags_devin_alsa_init(AgsSoundcard *soundcard,
			 GError **error);

void ags_devin_alsa_record_fill_buffer(void *buffer, guint ags_format, unsigned char *ring_buffer, guint channels, guint buffer_size);

void ags_devin_alsa_record(AgsSoundcard *soundcard,
			   GError **error);
void ags_devin_alsa_free(AgsSoundcard *soundcard);

void ags_devin_tic(AgsSoundcard *soundcard);
void ags_devin_offset_changed(AgsSoundcard *soundcard,
			      guint note_offset);

void ags_devin_set_bpm(AgsSoundcard *soundcard,
		       gdouble bpm);
gdouble ags_devin_get_bpm(AgsSoundcard *soundcard);

void ags_devin_set_delay_factor(AgsSoundcard *soundcard,
				gdouble delay_factor);
gdouble ags_devin_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_devin_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_devin_get_delay(AgsSoundcard *soundcard);
guint ags_devin_get_attack(AgsSoundcard *soundcard);

void* ags_devin_get_buffer(AgsSoundcard *soundcard);
void* ags_devin_get_next_buffer(AgsSoundcard *soundcard);
void* ags_devin_get_prev_buffer(AgsSoundcard *soundcard);

void ags_devin_lock_buffer(AgsSoundcard *soundcard,
			   void *buffer);
void ags_devin_unlock_buffer(AgsSoundcard *soundcard,
			     void *buffer);

guint ags_devin_get_delay_counter(AgsSoundcard *soundcard);

void ags_devin_set_start_note_offset(AgsSoundcard *soundcard,
				     guint start_note_offset);
guint ags_devin_get_start_note_offset(AgsSoundcard *soundcard);

void ags_devin_set_note_offset(AgsSoundcard *soundcard,
			       guint note_offset);
guint ags_devin_get_note_offset(AgsSoundcard *soundcard);

void ags_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
					guint note_offset);
guint ags_devin_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_devin_set_loop(AgsSoundcard *soundcard,
			guint loop_left, guint loop_right,
			gboolean do_loop);
void ags_devin_get_loop(AgsSoundcard *soundcard,
			guint *loop_left, guint *loop_right,
			gboolean *do_loop);

guint ags_devin_get_loop_offset(AgsSoundcard *soundcard);

/**
 * SECTION:ags_devin
 * @short_description: Input from soundcard
 * @title: AgsDevin
 * @section_id:
 * @include: ags/audio/ags_devin.h
 *
 * #AgsDevin represents a soundcard and supports input.
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

static gpointer ags_devin_parent_class = NULL;

GType
ags_devin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_devin = 0;

    static const GTypeInfo ags_devin_info = {
      sizeof(AgsDevinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_devin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsDevin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_devin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_devin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_devin_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_devin = g_type_register_static(G_TYPE_OBJECT,
					    "AgsDevin",
					    &ags_devin_info,
					    0);

    g_type_add_interface_static(ags_type_devin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_devin,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_devin);
  }

  return g_define_type_id__volatile;
}

void
ags_devin_class_init(AgsDevinClass *devin)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_devin_parent_class = g_type_class_peek_parent(devin);

  /* GObjectClass */
  gobject = (GObjectClass *) devin;

  gobject->set_property = ags_devin_set_property;
  gobject->get_property = ags_devin_get_property;

  gobject->dispose = ags_devin_dispose;
  gobject->finalize = ags_devin_finalize;

  /* properties */
  /**
   * AgsDevin:device:
   *
   * The alsa soundcard indentifier
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   AGS_DEVIN_DEFAULT_ALSA_DEVICE,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsDevin:dsp-channels:
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
   * AgsDevin:pcm-channels:
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
   * AgsDevin:format:
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
   * AgsDevin:buffer-size:
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
   * AgsDevin:samplerate:
   *
   * The samplerate
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("frames per second"),
				 i18n_pspec("The frames count recorded during a second"),
				 AGS_SOUNDCARD_MIN_SAMPLERATE,
				 AGS_SOUNDCARD_MAX_SAMPLERATE,
				 AGS_SOUNDCARD_DEFAULT_SAMPLERATE,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsDevin:buffer:
   *
   * The buffer
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to record"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsDevin:bpm:
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
   * AgsDevin:delay-factor:
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
   * AgsDevin:attack:
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
  
  /* AgsDevinClass */
}

GQuark
ags_devin_error_quark()
{
  return(g_quark_from_static_string("ags-devin-error-quark"));
}

void
ags_devin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_devin_get_uuid;
  connectable->has_resource = ags_devin_has_resource;

  connectable->is_ready = ags_devin_is_ready;
  connectable->add_to_registry = ags_devin_add_to_registry;
  connectable->remove_from_registry = ags_devin_remove_from_registry;

  connectable->list_resource = ags_devin_list_resource;
  connectable->xml_compose = ags_devin_xml_compose;
  connectable->xml_parse = ags_devin_xml_parse;

  connectable->is_connected = ags_devin_is_connected;  
  connectable->connect = ags_devin_connect;
  connectable->disconnect = ags_devin_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_devin_set_device;
  soundcard->get_device = ags_devin_get_device;
  
  soundcard->set_presets = ags_devin_set_presets;
  soundcard->get_presets = ags_devin_get_presets;

  soundcard->list_cards = ags_devin_list_cards;
  soundcard->pcm_info = ags_devin_pcm_info;
  soundcard->get_capability = ags_devin_get_capability;

  soundcard->is_available = ags_devin_is_available;

  soundcard->is_starting =  ags_devin_is_starting;
  soundcard->is_playing = NULL;
  soundcard->is_recording = ags_devin_is_recording;

  soundcard->get_uptime = ags_devin_get_uptime;
  
  soundcard->play_init = NULL;
  soundcard->play = NULL;

  soundcard->record_init = ags_devin_delegate_record_init;
  soundcard->record = ags_devin_delegate_record;
  
  soundcard->stop = ags_devin_delegate_stop;

  soundcard->tic = ags_devin_tic;
  soundcard->offset_changed = ags_devin_offset_changed;
    
  soundcard->set_bpm = ags_devin_set_bpm;
  soundcard->get_bpm = ags_devin_get_bpm;

  soundcard->set_delay_factor = ags_devin_set_delay_factor;
  soundcard->get_delay_factor = ags_devin_get_delay_factor;

  soundcard->get_absolute_delay = ags_devin_get_absolute_delay;

  soundcard->get_delay = ags_devin_get_delay;
  soundcard->get_attack = ags_devin_get_attack;

  soundcard->get_buffer = ags_devin_get_buffer;
  soundcard->get_next_buffer = ags_devin_get_next_buffer;
  soundcard->get_prev_buffer = ags_devin_get_prev_buffer;

  soundcard->lock_buffer = ags_devin_lock_buffer;
  soundcard->unlock_buffer = ags_devin_unlock_buffer;

  soundcard->get_delay_counter = ags_devin_get_delay_counter;

  soundcard->set_start_note_offset = ags_devin_set_start_note_offset;
  soundcard->get_start_note_offset = ags_devin_get_start_note_offset;

  soundcard->set_note_offset = ags_devin_set_note_offset;
  soundcard->get_note_offset = ags_devin_get_note_offset;

  soundcard->set_note_offset_absolute = ags_devin_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_devin_get_note_offset_absolute;

  soundcard->set_loop = ags_devin_set_loop;
  soundcard->get_loop = ags_devin_get_loop;

  soundcard->get_loop_offset = ags_devin_get_loop_offset;
}

void
ags_devin_init(AgsDevin *devin)
{  
  AgsConfig *config;
  
  gchar *str;
  gchar *segmentation;

  guint denominator, numerator;
  guint i;
  gboolean use_alsa;  
  
  devin->flags = 0;

  /* insert devin mutex */
  g_rec_mutex_init(&(devin->obj_mutex));

  /* uuid */
  devin->uuid = ags_uuid_alloc();
  ags_uuid_generate(devin->uuid);

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
    devin->flags |= (AGS_DEVIN_ALSA);
  }else{
    devin->flags |= (AGS_DEVIN_OSS);
  }

  g_free(str);

  /* presets */
  devin->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  devin->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  devin->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  devin->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  devin->format = ags_soundcard_helper_config_get_format(config);

  /* device */
  if(use_alsa){
    devin->out.alsa.handle = NULL;
    devin->out.alsa.device = AGS_DEVIN_DEFAULT_ALSA_DEVICE;
  }else{
    devin->out.oss.device_fd = -1;
    devin->out.oss.device = AGS_DEVIN_DEFAULT_OSS_DEVICE;
  }

  /* buffer */
  devin->buffer_mutex = (GRecMutex **) malloc(4 * sizeof(GRecMutex *));

  for(i = 0; i < 4; i++){
    devin->buffer_mutex[i] = (GRecMutex *) malloc(sizeof(GRecMutex));

    g_rec_mutex_init(devin->buffer_mutex[i]);
  }

  devin->buffer = (void **) malloc(4 * sizeof(void*));

  devin->buffer[0] = NULL;
  devin->buffer[1] = NULL;
  devin->buffer[2] = NULL;
  devin->buffer[3] = NULL;

  g_atomic_int_set(&(devin->available),
		   FALSE);
  
  devin->ring_buffer_size = AGS_DEVIN_DEFAULT_RING_BUFFER_SIZE;
  devin->nth_ring_buffer = 0;
  
  devin->ring_buffer = NULL;

  ags_devin_realloc_buffer(devin);
  
  /* bpm */
  devin->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  devin->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    devin->delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }

  /* delay and attack */
  devin->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
				    sizeof(gdouble));
  
  devin->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
				   sizeof(guint));

  ags_devin_adjust_delay_and_attack(devin);
  
  /* counters */
  devin->tact_counter = 0.0;
  devin->delay_counter = 0;
  devin->tic_counter = 0;

  devin->start_note_offset = 0;
  devin->note_offset = 0;
  devin->note_offset_absolute = 0;

  devin->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  devin->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  devin->do_loop = FALSE;

  devin->loop_offset = 0;  
}

void
ags_devin_set_property(GObject *gobject,
		       guint prop_id,
		       const GValue *value,
		       GParamSpec *param_spec)
{
  AgsDevin *devin;

  GRecMutex *devin_mutex;

  devin = AGS_DEVIN(gobject);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      g_rec_mutex_lock(devin_mutex);

      if((AGS_DEVIN_OSS & (devin->flags)) != 0){
	devin->out.oss.device = g_strdup(device);
      }else if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
	devin->out.alsa.device = g_strdup(device);
      }

      g_rec_mutex_unlock(devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      g_rec_mutex_lock(devin_mutex);

      if(dsp_channels == devin->dsp_channels){
	g_rec_mutex_unlock(devin_mutex);
	
	return;
      }

      devin->dsp_channels = dsp_channels;

      g_rec_mutex_unlock(devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      g_rec_mutex_lock(devin_mutex);

      if(pcm_channels == devin->pcm_channels){
	g_rec_mutex_unlock(devin_mutex);

	return;
      }

      devin->pcm_channels = pcm_channels;

      g_rec_mutex_unlock(devin_mutex);

      ags_devin_realloc_buffer(devin);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(devin_mutex);

      if(format == devin->format){
	g_rec_mutex_unlock(devin_mutex);

	return;
      }

      devin->format = format;

      g_rec_mutex_unlock(devin_mutex);

      ags_devin_realloc_buffer(devin);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      g_rec_mutex_lock(devin_mutex);

      if(buffer_size == devin->buffer_size){
	g_rec_mutex_unlock(devin_mutex);

	return;
      }

      devin->buffer_size = buffer_size;

      g_rec_mutex_unlock(devin_mutex);

      ags_devin_realloc_buffer(devin);
      ags_devin_adjust_delay_and_attack(devin);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      g_rec_mutex_lock(devin_mutex);

      if(samplerate == devin->samplerate){
	g_rec_mutex_unlock(devin_mutex);
	
	return;
      }

      devin->samplerate = samplerate;

      g_rec_mutex_unlock(devin_mutex);

      ags_devin_adjust_delay_and_attack(devin);
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

      g_rec_mutex_lock(devin_mutex);

      if(bpm == devin->bpm){
	g_rec_mutex_unlock(devin_mutex);

	return;
      }

      devin->bpm = bpm;

      g_rec_mutex_unlock(devin_mutex);

      ags_devin_adjust_delay_and_attack(devin);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      g_rec_mutex_lock(devin_mutex);

      if(delay_factor == devin->delay_factor){
	g_rec_mutex_unlock(devin_mutex);

	return;
      }

      devin->delay_factor = delay_factor;

      g_rec_mutex_unlock(devin_mutex);

      ags_devin_adjust_delay_and_attack(devin);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_devin_get_property(GObject *gobject,
		       guint prop_id,
		       GValue *value,
		       GParamSpec *param_spec)
{
  AgsDevin *devin;

  GRecMutex *devin_mutex;

  devin = AGS_DEVIN(gobject);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(devin_mutex);

      if((AGS_DEVIN_OSS & (devin->flags)) != 0){
	g_value_set_string(value, devin->out.oss.device);
      }else if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
	g_value_set_string(value, devin->out.alsa.device);
      }

      g_rec_mutex_unlock(devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_rec_mutex_lock(devin_mutex);

      g_value_set_uint(value, devin->dsp_channels);

      g_rec_mutex_unlock(devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_rec_mutex_lock(devin_mutex);

      g_value_set_uint(value, devin->pcm_channels);

      g_rec_mutex_unlock(devin_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(devin_mutex);

      g_value_set_uint(value, devin->format);

      g_rec_mutex_unlock(devin_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(devin_mutex);

      g_value_set_uint(value, devin->buffer_size);

      g_rec_mutex_unlock(devin_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(devin_mutex);

      g_value_set_uint(value, devin->samplerate);

      g_rec_mutex_unlock(devin_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(devin_mutex);

      g_value_set_pointer(value, devin->buffer);

      g_rec_mutex_unlock(devin_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(devin_mutex);

      g_value_set_double(value, devin->bpm);

      g_rec_mutex_unlock(devin_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_rec_mutex_lock(devin_mutex);

      g_value_set_double(value, devin->delay_factor);

      g_rec_mutex_unlock(devin_mutex);
    }
    break;
  case PROP_ATTACK:
    {
      g_rec_mutex_lock(devin_mutex);

      g_value_set_pointer(value, devin->attack);

      g_rec_mutex_unlock(devin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_devin_dispose(GObject *gobject)
{
  AgsDevin *devin;

  GList *list;

  devin = AGS_DEVIN(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_devin_parent_class)->dispose(gobject);
}

void
ags_devin_finalize(GObject *gobject)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(gobject);

  ags_uuid_free(devin->uuid);

  /* free output buffer */
  free(devin->buffer[0]);
  free(devin->buffer[1]);
  free(devin->buffer[2]);
  free(devin->buffer[3]);

  /* free buffer array */
  free(devin->buffer);

  /* free AgsAttack */
  free(devin->attack);

  /* call parent */
  G_OBJECT_CLASS(ags_devin_parent_class)->finalize(gobject);
}

AgsUUID*
ags_devin_get_uuid(AgsConnectable *connectable)
{
  AgsDevin *devin;
  
  AgsUUID *ptr;

  GRecMutex *devin_mutex;

  devin = AGS_DEVIN(connectable);

  /* get devin signal mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* get UUID */
  g_rec_mutex_lock(devin_mutex);

  ptr = devin->uuid;

  g_rec_mutex_unlock(devin_mutex);
  
  return(ptr);
}

gboolean
ags_devin_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_devin_is_ready(AgsConnectable *connectable)
{
  AgsDevin *devin;
  
  gboolean is_ready;

  devin = AGS_DEVIN(connectable);

  /* check is added */
  is_ready = ags_devin_test_flags(devin, AGS_DEVIN_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_devin_add_to_registry(AgsConnectable *connectable)
{
  AgsDevin *devin;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  devin = AGS_DEVIN(connectable);

  ags_devin_set_flags(devin, AGS_DEVIN_ADDED_TO_REGISTRY);
}

void
ags_devin_remove_from_registry(AgsConnectable *connectable)
{
  AgsDevin *devin;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  devin = AGS_DEVIN(connectable);

  ags_devin_unset_flags(devin, AGS_DEVIN_ADDED_TO_REGISTRY);
}

xmlNode*
ags_devin_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_devin_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_devin_xml_parse(AgsConnectable *connectable,
		    xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_devin_is_connected(AgsConnectable *connectable)
{
  AgsDevin *devin;
  
  gboolean is_connected;

  devin = AGS_DEVIN(connectable);

  /* check is connected */
  is_connected = ags_devin_test_flags(devin, AGS_DEVIN_CONNECTED);
  
  return(is_connected);
}

void
ags_devin_connect(AgsConnectable *connectable)
{
  AgsDevin *devin;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  devin = AGS_DEVIN(connectable);

  ags_devin_set_flags(devin, AGS_DEVIN_CONNECTED);
}

void
ags_devin_disconnect(AgsConnectable *connectable)
{

  AgsDevin *devin;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  devin = AGS_DEVIN(connectable);
  
  ags_devin_unset_flags(devin, AGS_DEVIN_CONNECTED);
}

/**
 * ags_devin_test_flags:
 * @devin: the #AgsDevin
 * @flags: the flags
 *
 * Test @flags to be set on @devin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_devin_test_flags(AgsDevin *devin, guint flags)
{
  gboolean retval;  
  
  GRecMutex *devin_mutex;

  if(!AGS_IS_DEVIN(devin)){
    return(FALSE);
  }

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* test */
  g_rec_mutex_lock(devin_mutex);

  retval = (flags & (devin->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(devin_mutex);

  return(retval);
}

/**
 * ags_devin_set_flags:
 * @devin: the #AgsDevin
 * @flags: see #AgsDevinFlags-enum
 *
 * Enable a feature of @devin.
 *
 * Since: 3.0.0
 */
void
ags_devin_set_flags(AgsDevin *devin, guint flags)
{
  GRecMutex *devin_mutex;

  if(!AGS_IS_DEVIN(devin)){
    return;
  }

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(devin_mutex);

  devin->flags |= flags;
  
  g_rec_mutex_unlock(devin_mutex);
}
    
/**
 * ags_devin_unset_flags:
 * @devin: the #AgsDevin
 * @flags: see #AgsDevinFlags-enum
 *
 * Disable a feature of @devin.
 *
 * Since: 3.0.0
 */
void
ags_devin_unset_flags(AgsDevin *devin, guint flags)
{  
  GRecMutex *devin_mutex;

  if(!AGS_IS_DEVIN(devin)){
    return;
  }

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(devin_mutex);

  devin->flags &= (~flags);
  
  g_rec_mutex_unlock(devin_mutex);
}

void
ags_devin_set_device(AgsSoundcard *soundcard,
		     gchar *device)
{
  AgsDevin *devin;

  GList *card_id, *card_id_start, *card_name, *card_name_start;

  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* list cards */
  card_id = NULL;
  card_name = NULL;
  
  ags_soundcard_list_cards(soundcard,
			   &card_id, &card_name);

  card_id_start = card_id;
  card_name_start = card_name;
  
  /* check card */
  g_rec_mutex_lock(devin_mutex);

  while(card_id != NULL){
    if(!g_ascii_strncasecmp(card_id->data,
			    device,
			    strlen(card_id->data))){
      if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
	devin->out.alsa.device = g_strdup(device);
      }else{
	devin->out.oss.device = g_strdup(device);
      }

      break;
    }
    
    card_id = card_id->next;
  }

  g_rec_mutex_unlock(devin_mutex);

  /* free card id and name */
  g_list_free_full(card_id_start,
		   g_free);
  g_list_free_full(card_name_start,
		   g_free);
}

gchar*
ags_devin_get_device(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  gchar *device;
  
  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);
  
  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  device = NULL;
  
  g_rec_mutex_lock(devin_mutex);

  if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
    device = g_strdup(devin->out.alsa.device);
  }else if((AGS_DEVIN_OSS & (devin->flags)) != 0){
    device = g_strdup(devin->out.oss.device);
  }

  g_rec_mutex_unlock(devin_mutex);

  return(device);
}

void
ags_devin_set_presets(AgsSoundcard *soundcard,
		      guint channels,
		      guint rate,
		      guint buffer_size,
		      guint format)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);

  g_object_set(devin,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_devin_get_presets(AgsSoundcard *soundcard,
		      guint *channels,
		      guint *rate,
		      guint *buffer_size,
		      guint *format)
{
  AgsDevin *devin;

  GRecMutex *devin_mutex;

  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* get presets */
  g_rec_mutex_lock(devin_mutex);

  if(channels != NULL){
    *channels = devin->pcm_channels;
  }

  if(rate != NULL){
    *rate = devin->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = devin->buffer_size;
  }

  if(format != NULL){
    *format = devin->format;
  }

  g_rec_mutex_unlock(devin_mutex);
}

void
ags_devin_list_cards(AgsSoundcard *soundcard,
		     GList **card_id, GList **card_name)
{
  AgsDevin *devin;

  GRecMutex *devin_mutex;

  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }

  g_rec_mutex_lock(devin_mutex);

  if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
#ifdef AGS_WITH_ALSA
    snd_ctl_t *card_handle;
    snd_ctl_card_info_t *card_info;

    char *name;
    gchar *str;
    gchar *str_err;
  
    int card_num;
    int device;
    int error;

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
	    *card_id = g_list_prepend(*card_id, g_strdup(snd_device_name_get_hint(iter[0],
										  "NAME")));
	  }

	  if(card_name != NULL){
	    *card_name = g_list_prepend(*card_name, g_strdup(snd_ctl_card_info_get_name(card_info)));
	  }
	}

	snd_device_name_free_hint(hints);
      }
    
      snd_ctl_close(card_handle);
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

      next = ai.next_rec_engine;
      
      if(next <= 0){
	break;
      }
    }
#endif
  }

  g_rec_mutex_unlock(devin_mutex);

  if(card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

void
ags_devin_pcm_info(AgsSoundcard *soundcard,
		   char *card_id,
		   guint *channels_min, guint *channels_max,
		   guint *rate_min, guint *rate_max,
		   guint *buffer_size_min, guint *buffer_size_max,
		   GError **error)
{
  AgsDevin *devin;

  GRecMutex *devin_mutex;

  if(card_id == NULL){
    return;
  }
  
  devin = AGS_DEVIN(soundcard);
  

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* pcm info */
  g_rec_mutex_lock(devin_mutex);

  if((AGS_DEVIN_ALSA & (devin->flags)) != 0){
#ifdef AGS_WITH_ALSA
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    
    gchar *str;
    
    unsigned int val;
    int dir;
    snd_pcm_uframes_t frames;

    int rc;
    int err;

    /* Open PCM device for recordback. */
    handle = NULL;

    rc = snd_pcm_open(&handle, card_id, SND_PCM_STREAM_CAPTURE, 0);

    if(rc < 0) {
      str = snd_strerror(rc);
      g_message("unable to open pcm device: %s\n", str);

      if(error != NULL){
	g_set_error(error,
		    AGS_DEVIN_ERROR,
		    AGS_DEVIN_ERROR_LOCKED_SOUNDCARD,
		    "unable to open pcm device: %s\n",
		    str);
      }
    
      //    free(str);
    
      goto ags_devin_pcm_info_ERR;
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
		    AGS_DEVIN_ERROR,
		    AGS_DEVIN_ERROR_LOCKED_SOUNDCARD,
		    "unable to open pcm device: %s\n",
		    str);
      }
    
      goto ags_devin_pcm_info_ERR;
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
      goto ags_devin_pcm_info_ERR;
    }
    
    if(ioctl(mixerfd, cmd, &ainfo) == -1){
      int e = errno;

      str = strerror(e);
      g_message("unable to retrieve audio info: %s\n", str);

      if(error != NULL){
	g_set_error(error,
		    AGS_DEVIN_ERROR,
		    AGS_DEVIN_ERROR_LOCKED_SOUNDCARD,
		    "unable to retrieve audio info: %s\n",
		    str);
      }
    
      goto ags_devin_pcm_info_ERR;
    }
  
    *channels_min = ainfo.min_channels;
    *channels_max = ainfo.max_channels;
    *rate_min = ainfo.min_rate;
    *rate_max = ainfo.max_rate;
    *buffer_size_min = 64;
    *buffer_size_max = 8192;
#endif
  }

 ags_devin_pcm_info_ERR:

  g_rec_mutex_unlock(devin_mutex);
}

guint
ags_devin_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_CAPTURE);
}

gboolean
ags_devin_is_available(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  gboolean retval;
  
  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);
  
  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* check available */
  retval = FALSE;

  //TODO:JK: implement me

  return(retval);
}

gboolean
ags_devin_is_starting(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  gboolean is_starting;
  
  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* check is starting */
  g_rec_mutex_lock(devin_mutex);

  is_starting = ((AGS_DEVIN_START_RECORD & (devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(devin_mutex);
  
  return(is_starting);
}

gboolean
ags_devin_is_recording(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  gboolean is_recording;
  
  GRecMutex *devin_mutex;

  devin = AGS_DEVIN(soundcard);
  
  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* check is starting */
  g_rec_mutex_lock(devin_mutex);

  is_recording = ((AGS_DEVIN_RECORD & (devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(devin_mutex);

  return(is_recording);
}

gchar*
ags_devin_get_uptime(AgsSoundcard *soundcard)
{
  gchar *uptime;

  if(ags_soundcard_is_recording(soundcard)){
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
ags_devin_delegate_record_init(AgsSoundcard *soundcard,
			       GError **error)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);

  if(ags_devin_test_flags(devin, AGS_DEVIN_ALSA)){
    ags_devin_alsa_init(soundcard,
			error);
  }else if(ags_devin_test_flags(devin, AGS_DEVIN_OSS)){
    ags_devin_oss_init(soundcard,
		       error);
  }
}

void
ags_devin_delegate_record(AgsSoundcard *soundcard,
			  GError **error)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);

  if(ags_devin_test_flags(devin, AGS_DEVIN_ALSA)){
    ags_devin_alsa_record(soundcard,
			  error);
  }else if(ags_devin_test_flags(devin, AGS_DEVIN_OSS)){
    ags_devin_oss_record(soundcard,
			 error);
  }
}

void
ags_devin_delegate_stop(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  devin = AGS_DEVIN(soundcard);

  if(ags_devin_test_flags(devin, AGS_DEVIN_ALSA)){
    ags_devin_alsa_free(soundcard);
  }else if(ags_devin_test_flags(devin, AGS_DEVIN_OSS)){
    ags_devin_oss_free(soundcard);
  }
}

void
ags_devin_oss_init(AgsSoundcard *soundcard,
		   GError **error)
{
  AgsDevin *devin;
  
  gchar *str;

  guint word_size;
  int format;
  int tmp;
  guint i;

  GRecMutex *devin_mutex;

  if(ags_soundcard_is_recording(soundcard)){
    return;
  }

  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* retrieve word size */
  g_rec_mutex_lock(devin_mutex);

  switch(devin->format){
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
    g_warning("ags_devin_oss_init(): unsupported word size");
    return;
  }

  /* prepare for recordback */
  devin->flags |= (AGS_DEVIN_START_RECORD |
		   AGS_DEVIN_RECORD |
		   AGS_DEVIN_NONBLOCKING);

  memset(devin->buffer[0], 0, devin->pcm_channels * devin->buffer_size * word_size);
  memset(devin->buffer[1], 0, devin->pcm_channels * devin->buffer_size * word_size);
  memset(devin->buffer[2], 0, devin->pcm_channels * devin->buffer_size * word_size);
  memset(devin->buffer[3], 0, devin->pcm_channels * devin->buffer_size * word_size);

  /* allocate ring buffer */
  g_atomic_int_set(&(devin->available),
		   FALSE);
  
  devin->ring_buffer = (unsigned char **) malloc(devin->ring_buffer_size * sizeof(unsigned char *));

  for(i = 0; i < devin->ring_buffer_size; i++){
    devin->ring_buffer[i] = (unsigned char *) malloc(devin->pcm_channels *
						     devin->buffer_size * word_size *
						     sizeof(unsigned char));
  }

#ifdef AGS_WITH_OSS
  /* open device fd */
  str = devin->out.oss.device;
  devin->out.oss.device_fd = open(str, O_RDONLY, 0);

  if(devin->out.oss.device_fd == -1){
    devin->flags &= (~(AGS_DEVIN_START_RECORD |
		       AGS_DEVIN_RECORD |
		       AGS_DEVIN_NONBLOCKING));
    
    g_rec_mutex_unlock(devin_mutex);

    g_warning("couldn't open device %s", devin->out.oss.device);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_LOCKED_SOUNDCARD,
		  "unable to open dsp device: %s\n",
		  str);
    }

    return;
  }

  //NOTE:JK: unsupported on kfreebsd 9.0
  //  tmp = APF_NORMAL;
  //  ioctl(devin->out.oss.device_fd, SNDCTL_DSP_PROFILE, &tmp);

  tmp = format;

  if(ioctl(devin->out.oss.device_fd, SNDCTL_DSP_SETFMT, &tmp) == -1){
    devin->flags &= (~(AGS_DEVIN_START_RECORD |
		       AGS_DEVIN_RECORD |
		       AGS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(devin_mutex);

    str = strerror(errno);
    g_warning("failed to select bits/sample");

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
		  "unable to open dsp device: %s",
		  str);
    }

    devin->out.oss.device_fd = -1;

    return;
  }
  
  if(tmp != format){
    devin->flags &= (~(AGS_DEVIN_START_RECORD |
		       AGS_DEVIN_RECORD |
		       AGS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(devin_mutex);

    str = strerror(errno);
    g_warning("failed to select bits/sample");

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
		  "unable to open dsp device: %s",
		  str);
    }

    devin->out.oss.device_fd = -1;

    return;
  }

  tmp = devin->dsp_channels;

  if(ioctl(devin->out.oss.device_fd, SNDCTL_DSP_CHANNELS, &tmp) == -1){
    devin->flags &= (~(AGS_DEVIN_START_RECORD |
		       AGS_DEVIN_RECORD |
		       AGS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(devin_mutex);

    str = strerror(errno);
    g_warning("Channels count (%i) not available for recordbacks: %s", devin->dsp_channels, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_CHANNELS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.oss.device_fd = -1;
    
    return;
  }

  if(tmp != devin->dsp_channels){
    devin->flags &= (~(AGS_DEVIN_START_RECORD |
		       AGS_DEVIN_RECORD |
		       AGS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(devin_mutex);

    str = strerror(errno);
    g_warning("Channels count (%i) not available for capture: %s", devin->dsp_channels, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_CHANNELS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }
    
    devin->out.oss.device_fd = -1;
    
    return;
  }

  tmp = devin->samplerate;

  if(ioctl(devin->out.oss.device_fd, SNDCTL_DSP_SPEED, &tmp) == -1){
    devin->flags &= (~(AGS_DEVIN_START_RECORD |
		       AGS_DEVIN_RECORD |
		       AGS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(devin_mutex);

    str = strerror(errno);
    g_warning("Rate %iHz not available for capture: %s", devin->samplerate, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_SAMPLERATE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.oss.device_fd = -1;
    
    return;
  }

  if(tmp != devin->samplerate){
    g_warning("Warning: Capture using %d Hz (file %d Hz)",
	      tmp,
	      devin->samplerate);
  }
#endif
  
  devin->tact_counter = 0.0;
  devin->delay_counter = floor(ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(devin)));
  devin->tic_counter = 0;

  devin->nth_ring_buffer = 0;
    
#ifdef AGS_WITH_OSS
  devin->flags |= AGS_DEVIN_INITIALIZED;
#endif
  devin->flags |= AGS_DEVIN_BUFFER0;
  devin->flags &= (~(AGS_DEVIN_BUFFER1 |
		     AGS_DEVIN_BUFFER2 |
		     AGS_DEVIN_BUFFER3));
  
  g_rec_mutex_unlock(devin_mutex);
}

void
ags_devin_oss_record_fill_buffer(void *buffer, guint ags_format, unsigned char *ring_buffer, guint channels, guint buffer_size)
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
    g_warning("ags_devin_oss_record(): unsupported word size");
    return;
  }

  /* fill the channel areas */
  for(count = 0; count < buffer_size; count++){
    for(chn = 0; chn < channels; chn++){
      /* Generate data in native endian format */
      res = 0;
	
      if(ags_endian_host_is_be()){
	for(i = 0; i < bps; i++){
	  res |= (int) (*(ring_buffer + chn * bps + word_size - 1 - i)) << (i * 8);
	}
      }else{
	for(i = 0; i < bps; i++){
	  res |= (int) (*(ring_buffer + chn * bps + i)) << (i * 8);
	}
      }	

      switch(ags_format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	((gint8 *) buffer)[count * channels + chn] = res;
      }
      break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	((gint16 *) buffer)[count * channels + chn] = res;
      }
      break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	((gint32 *) buffer)[count * channels + chn] = res;
      }
      break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	((gint32 *) buffer)[count * channels + chn] = res;
      }
      break;
      }
    }

    ring_buffer += channels * bps;
  }
}

void
ags_devin_oss_record(AgsSoundcard *soundcard,
		     GError **error)
{
  AgsDevin *devin;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;
  GList *list;

  gchar *str;
  
  guint word_size;
  guint nth_buffer;

  int n_write;
  
  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);

  application_context = ags_application_context_get_instance();

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);
  
#ifdef AGS_WITH_OSS
  /* lock */
  g_rec_mutex_lock(devin_mutex);

  /* retrieve word size */
  switch(devin->format){
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
    g_warning("ags_devin_oss_record(): unsupported word size");
    return;
  }

  /* do capture */
  devin->flags &= (~AGS_DEVIN_START_RECORD);

  if((AGS_DEVIN_INITIALIZED & (devin->flags)) == 0){
    g_rec_mutex_unlock(devin_mutex);
    
    return;
  }

  /* check buffer flag */
  nth_buffer = 0;
  
  if((AGS_DEVIN_BUFFER0 & (devin->flags)) != 0){
    nth_buffer = 0;
  }else if((AGS_DEVIN_BUFFER1 & (devin->flags)) != 0){
    nth_buffer = 1;
  }else if((AGS_DEVIN_BUFFER2 & (devin->flags)) != 0){
    nth_buffer = 2;
  }else if((AGS_DEVIN_BUFFER3 & (devin->flags)) != 0){
    nth_buffer = 3;
  }

  /* write ring buffer */
  n_write = read(devin->out.oss.device_fd,
		 devin->ring_buffer[devin->nth_ring_buffer],
		 devin->pcm_channels * devin->buffer_size * word_size * sizeof (char));

  /* fill buffer */
  ags_soundcard_lock_buffer(soundcard,
			    devin->buffer[nth_buffer]);

  ags_devin_oss_record_fill_buffer(devin->buffer[nth_buffer],
				   devin->format,
				   devin->ring_buffer[devin->nth_ring_buffer],
				   devin->pcm_channels,
				   devin->buffer_size);

  ags_soundcard_unlock_buffer(soundcard,
			      devin->buffer[nth_buffer]);

  g_atomic_int_set(&(devin->available),
		   FALSE);
  
  if(n_write != devin->pcm_channels * devin->buffer_size * word_size * sizeof (char)){
    g_critical("write() return doesn't match written bytes");
  }

  /* increment nth ring-buffer */
  if(devin->nth_ring_buffer + 1 >= devin->ring_buffer_size){
    devin->nth_ring_buffer = 0;
  }else{
    devin->nth_ring_buffer += 1;
  }
  
  g_rec_mutex_unlock(devin_mutex);
#endif

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;
  
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) devin);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) devin);
  task = g_list_append(task,
		       clear_buffer);
  
  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) devin);
  task = g_list_append(task,
		       clear_buffer);

  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) devin);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_devin_oss_free(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  guint i;
  
  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /*  */
  g_rec_mutex_lock(devin_mutex);

  if((AGS_DEVIN_INITIALIZED & (devin->flags)) == 0){
    g_rec_mutex_unlock(devin_mutex);
    
    return;
  }
  
  close(devin->out.oss.device_fd);
  devin->out.oss.device_fd = -1;

  /* free ring-buffer */
  g_atomic_int_set(&(devin->available),
		   FALSE);
  
  for(i = 0; i < devin->ring_buffer_size; i++){
    free(devin->ring_buffer[i]);
  }
  
  free(devin->ring_buffer);

  devin->ring_buffer = NULL;

  /* reset flags */
  devin->flags &= (~(AGS_DEVIN_BUFFER0 |
		     AGS_DEVIN_BUFFER1 |
		     AGS_DEVIN_BUFFER2 |
		     AGS_DEVIN_BUFFER3 |
		     AGS_DEVIN_RECORD |
		     AGS_DEVIN_INITIALIZED));

  devin->note_offset = devin->start_note_offset;
  devin->note_offset_absolute = devin->start_note_offset;

  g_rec_mutex_unlock(devin_mutex);
}

void
ags_devin_alsa_init(AgsSoundcard *soundcard,
		    GError **error)
{
  AgsDevin *devin;

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
  guint i;
  
  GRecMutex *devin_mutex; 
 
  static unsigned int period_time = 100000;
  static unsigned int buffer_time = 100000;

  if(ags_soundcard_is_recording(soundcard)){
    return;
  }

  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* retrieve word size */
  g_rec_mutex_lock(devin_mutex);

  if(devin->out.alsa.device == NULL){
    g_rec_mutex_unlock(devin_mutex);
    
    return;
  }

#ifdef AGS_WITH_ALSA
  format = 0;
#endif
  
  switch(devin->format){
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
    g_rec_mutex_unlock(devin_mutex);

    g_warning("ags_devin_alsa_init(): unsupported word size");

    return;
  }

  /* prepare for playback */
  devin->flags |= (AGS_DEVIN_START_RECORD |
		   AGS_DEVIN_RECORD |
		   AGS_DEVIN_NONBLOCKING);

  memset(devin->buffer[0], 0, devin->pcm_channels * devin->buffer_size * word_size);
  memset(devin->buffer[1], 0, devin->pcm_channels * devin->buffer_size * word_size);
  memset(devin->buffer[2], 0, devin->pcm_channels * devin->buffer_size * word_size);
  memset(devin->buffer[3], 0, devin->pcm_channels * devin->buffer_size * word_size);

  /* allocate ring buffer */
#ifdef AGS_WITH_ALSA
  g_atomic_int_set(&(devin->available),
		   FALSE);
  
  devin->ring_buffer = (unsigned char **) malloc(devin->ring_buffer_size * sizeof(unsigned char *));

  for(i = 0; i < devin->ring_buffer_size; i++){
    devin->ring_buffer[i] = (unsigned char *) malloc(devin->pcm_channels *
						      devin->buffer_size * (snd_pcm_format_physical_width(format) / 8) *
						      sizeof(unsigned char));
  }
 
  /*  */
  period_event = 0;
  
  /* Open PCM device for playback. */  
  handle = NULL;

  if((err = snd_pcm_open(&handle, devin->out.alsa.device, SND_PCM_STREAM_CAPTURE, 0)) < 0){
    gchar *device_fixup;
    
    str = snd_strerror(err);
    g_warning("Playback open error (attempting fixup): %s", str);
    
    device_fixup = g_strdup_printf("%s,0",
				   devin->out.alsa.device);

    handle = NULL;
    
    if((err = snd_pcm_open(&handle, device_fixup, SND_PCM_STREAM_CAPTURE, 0)) < 0){
      devin->flags |= (~(AGS_DEVIN_START_RECORD |
			 AGS_DEVIN_RECORD |
			 AGS_DEVIN_NONBLOCKING));
      
      g_rec_mutex_unlock(devin_mutex);
      
      if(error != NULL){
	g_set_error(error,
		    AGS_DEVIN_ERROR,
		    AGS_DEVIN_ERROR_LOCKED_SOUNDCARD,
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
    devin->flags |= (~(AGS_DEVIN_START_RECORD |
		       AGS_DEVIN_RECORD |
		       AGS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(devin_mutex);

    str = snd_strerror(err);
    g_warning("Broken configuration for playback: no configurations available: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_BROKEN_CONFIGURATION,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set hardware resampling * /
     err = snd_pcm_hw_params_set_rate_resample(handle, hwparams, 0);
     if (err < 0) {
     g_rec_mutex_unlock(devin_mutex);

     str = snd_strerror(err);
     g_warning("Resampling setup failed for playback: %s\n", str);

     //    free(str);
    
     return;
     }
  */
  
  /* set the interleaved read/write format */
  err = snd_pcm_hw_params_set_access(handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED);
  if (err < 0) {
    devin->flags |= (~(AGS_DEVIN_START_RECORD |
		       AGS_DEVIN_RECORD |
		       AGS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(devin_mutex);

    str = snd_strerror(err);
    g_warning("Access type not available for playback: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_ACCESS_TYPE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the sample format */
  err = snd_pcm_hw_params_set_format(handle, hwparams, format);

  if (err < 0) {
    devin->flags |= (~(AGS_DEVIN_START_RECORD |
		       AGS_DEVIN_RECORD |
		       AGS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(devin_mutex);

    str = snd_strerror(err);
    g_warning("Sample format not available for playback: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_SAMPLE_FORMAT_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the count of channels */
  channels = devin->pcm_channels;
  err = snd_pcm_hw_params_set_channels(handle, hwparams, channels);

  if (err < 0) {
    devin->flags |= (~(AGS_DEVIN_START_RECORD |
		       AGS_DEVIN_RECORD |
		       AGS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(devin_mutex);

    str = snd_strerror(err);
    g_warning("Channels count (%i) not available for playbacks: %s", channels, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_CHANNELS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the stream rate */
  rate = devin->samplerate;
  rrate = rate;
  err = snd_pcm_hw_params_set_rate_near(handle, hwparams, &rrate, 0);

  if (err < 0) {
    devin->flags |= (~(AGS_DEVIN_START_RECORD |
		       AGS_DEVIN_RECORD |
		       AGS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(devin_mutex);

    str = snd_strerror(err);
    g_warning("Rate %iHz not available for playback: %s", rate, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_SAMPLERATE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  if (rrate != rate) {
    devin->flags |= (~(AGS_DEVIN_START_RECORD |
		       AGS_DEVIN_RECORD |
		       AGS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(devin_mutex);

    g_warning("Rate doesn't match (requested %iHz, get %iHz)", rate, err);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_SAMPLERATE_NOT_AVAILABLE,
		  "unable to open pcm device");
    }

    devin->out.alsa.handle = NULL;
    
    return;
  }

  /* set the buffer size */
  size = 2 * devin->buffer_size;
  err = snd_pcm_hw_params_set_buffer_size(handle, hwparams, size);

  if (err < 0) {
    devin->flags |= (~(AGS_DEVIN_START_RECORD |
		       AGS_DEVIN_RECORD |
		       AGS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(devin_mutex);

    str = snd_strerror(err);

    g_warning("Unable to set buffer size %lu for playback: %s", size, str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_BUFFER_SIZE_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* set the period size * /
     period_size = devin->buffer_size;
     err = snd_pcm_hw_params_set_period_size_near(handle, hwparams, period_size, dir);
     if (err < 0) {
     g_rec_mutex_unlock(devin_mutex);

     str = snd_strerror(err);
     g_warning("Unable to get period size for playback: %s\n", str);

     //    free(str);
    
     return;
     }
  */
  
  /* write the parameters to device */
  err = snd_pcm_hw_params(handle, hwparams);

  if (err < 0) {
    devin->flags |= (~(AGS_DEVIN_START_RECORD |
		       AGS_DEVIN_RECORD |
		       AGS_DEVIN_NONBLOCKING));

    g_rec_mutex_unlock(devin_mutex);

    str = snd_strerror(err);
    g_warning("Unable to set hw params for playback: %s", str);

    if(error != NULL){
      g_set_error(error,
		  AGS_DEVIN_ERROR,
		  AGS_DEVIN_ERROR_HW_PARAMETERS_NOT_AVAILABLE,
		  "unable to open pcm device: %s",
		  str);
    }

    devin->out.alsa.handle = NULL;
    
    //    free(str);
    
    return;
  }

  /* get the current swparams * /
     err = snd_pcm_sw_params_current(handle, swparams);
     if (err < 0) {
     g_rec_mutex_unlock(devin_mutex);

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
     g_rec_mutex_unlock(devin_mutex);

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
     g_rec_mutex_unlock(devin_mutex);

     str = snd_strerror(err);
     g_warning("Unable to set avail min for playback: %s\n", str);

     //    free(str);
    
     return;
     }

     /* write the parameters to the playback device * /
     err = snd_pcm_sw_params(handle, swparams);
     if (err < 0) {
     g_rec_mutex_unlock(devin_mutex);

     str = snd_strerror(err);
     g_warning("Unable to set sw params for playback: %s\n", str);

     //    free(str);
    
     return;
     }
  */

  /*  */
  devin->out.alsa.handle = handle;
#endif

  devin->tact_counter = 0.0;
  devin->delay_counter = 0.0;
  devin->tic_counter = 0;

  devin->nth_ring_buffer = 0;
  
#ifdef AGS_WITH_ALSA
  devin->flags |= AGS_DEVIN_INITIALIZED;
#endif
  devin->flags |= AGS_DEVIN_BUFFER0;
  devin->flags &= (~(AGS_DEVIN_BUFFER1 |
		     AGS_DEVIN_BUFFER2 |
		     AGS_DEVIN_BUFFER3));
  
  g_rec_mutex_unlock(devin_mutex);
}

void
ags_devin_alsa_record_fill_buffer(void *buffer, guint ags_format, unsigned char *ring_buffer, guint channels, guint buffer_size)
{
#ifdef AGS_WITH_ALSA
  snd_pcm_format_t format;

  int format_bits;

  unsigned int max_val;
    
  int bps; /* bytes per sample */
  int phys_bps;

  int big_endian;
  int to_unsigned;

  int res;

  gint count;
  guint i, chn;
    
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
    g_warning("ags_devin_alsa_record(): unsupported word size");
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
  for(count = 0; count < buffer_size; count++){
    for(chn = 0; chn < channels; chn++){	
      /* Generate data in native endian format */
      res = 0;
	
      if (big_endian) {
	for (i = 0; i < bps; i++)
	  res |= (int) (*(ring_buffer + chn * bps + phys_bps - 1 - i) << (i * 8));
      } else {
	for (i = 0; i < bps; i++)
	  res |= (int) (*(ring_buffer + chn * bps + i) << (i * 8));
      }	

      if(to_unsigned){
	res |= 1U << (format_bits - 1);
      }
	
      switch(ags_format){
      case AGS_SOUNDCARD_SIGNED_8_BIT:
      {
	((gint8 *) buffer)[count * channels + chn] = res;
      }
      break;
      case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	((gint16 *) buffer)[count * channels + chn] = res;
      }
      break;
      case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	((gint32 *) buffer)[count * channels + chn] = res;
      }
      break;
      case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	((gint32 *) buffer)[count * channels + chn] = res;
      }
      break;
      }
    }

    ring_buffer += channels * bps;
  }
#endif
}

void
ags_devin_alsa_record(AgsSoundcard *soundcard,
		      GError **error)
{
  AgsDevin *devin;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;
  GList *list;
  
  gchar *str;
  
  guint word_size;
  guint nth_buffer;
  
  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);

  application_context = ags_application_context_get_instance();

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* lock */
  g_rec_mutex_lock(devin_mutex);
  
  /* retrieve word size */
  switch(devin->format){
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
    g_rec_mutex_unlock(devin_mutex);
    
    g_warning("ags_devin_alsa_record(): unsupported word size");

    return;
  }

  /* do capture */
  devin->flags &= (~AGS_DEVIN_START_RECORD);

  if((AGS_DEVIN_INITIALIZED & (devin->flags)) == 0){
    g_rec_mutex_unlock(devin_mutex);
    
    return;
  }

  //  g_message("record - 0x%0x", ((AGS_DEVIN_BUFFER0 |
  //				AGS_DEVIN_BUFFER1 |
  //				AGS_DEVIN_BUFFER2 |
  //				AGS_DEVIN_BUFFER3) & (devin->flags)));

  /* check buffer flag */
  nth_buffer = 0;
  
  if((AGS_DEVIN_BUFFER0 & (devin->flags)) != 0){
    nth_buffer = 0;
  }else if((AGS_DEVIN_BUFFER1 & (devin->flags)) != 0){
    nth_buffer = 1;
  }else if((AGS_DEVIN_BUFFER2 & (devin->flags)) != 0){
    nth_buffer = 2;
  }else if((AGS_DEVIN_BUFFER3 & (devin->flags)) != 0){
    nth_buffer = 3;
  }

#ifdef AGS_WITH_ALSA  
  /* write ring buffer */
  devin->out.alsa.rc = snd_pcm_readi(devin->out.alsa.handle,
				     devin->ring_buffer[devin->nth_ring_buffer],
				     (snd_pcm_uframes_t) (devin->buffer_size));


  /* fill buffer */
  ags_soundcard_lock_buffer(soundcard,
			    devin->buffer[nth_buffer]);
  
  ags_devin_alsa_record_fill_buffer(devin->buffer[nth_buffer], devin->format,
				    devin->ring_buffer[devin->nth_ring_buffer],
				    devin->pcm_channels, devin->buffer_size);

  ags_soundcard_unlock_buffer(soundcard,
			      devin->buffer[nth_buffer]);
  
  g_atomic_int_set(&(devin->available),
		   FALSE);
  
  /* check error flag */
  if((AGS_DEVIN_NONBLOCKING & (devin->flags)) == 0){
    if(devin->out.alsa.rc == -EPIPE){
      /* EPIPE means underrun */
      snd_pcm_prepare(devin->out.alsa.handle);

#ifdef AGS_DEBUG
      g_message("underrun occurred");
#endif
    }else if(devin->out.alsa.rc == -ESTRPIPE){
      static const struct timespec idle = {
	0,
	4000,
      };

      int err;

      while((err = snd_pcm_resume(devin->out.alsa.handle)) < 0){ // == -EAGAIN
	nanosleep(&idle, NULL); /* wait until the suspend flag is released */
      }
	
      if(err < 0){
	err = snd_pcm_prepare(devin->out.alsa.handle);
      }
    }else if(devin->out.alsa.rc < 0){
      str = snd_strerror(devin->out.alsa.rc);
      
      g_message("error from writei: %s", str);
    }else if(devin->out.alsa.rc != (int) devin->buffer_size) {
      g_message("short write, write %d frames", devin->out.alsa.rc);
    }
  }      
  
#endif

  /* increment nth ring-buffer */
  g_atomic_int_set(&(devin->available),
		   FALSE);
  
  if(devin->nth_ring_buffer + 1 >= devin->ring_buffer_size){
    devin->nth_ring_buffer = 0;
  }else{
    devin->nth_ring_buffer += 1;
  }
  
  g_rec_mutex_unlock(devin_mutex);

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;
  
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) devin);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) devin);
  task = g_list_append(task,
		       clear_buffer);

  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) devin);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);
  
#ifdef AGS_WITH_ALSA
  snd_pcm_prepare(devin->out.alsa.handle);
#endif

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_devin_alsa_free(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  AgsApplicationContext *application_context;

  guint i;
  
  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* lock */
  g_rec_mutex_lock(devin_mutex);

  if((AGS_DEVIN_INITIALIZED & (devin->flags)) == 0 ||
     devin->out.alsa.handle == NULL){
    g_rec_mutex_unlock(devin_mutex);
    
    return;
  }
  
#ifdef AGS_WITH_ALSA
  //  snd_pcm_drain(devin->out.alsa.handle);
  snd_pcm_close(devin->out.alsa.handle);
  devin->out.alsa.handle = NULL;
#endif

  /* free ring-buffer */
  for(i = 0; i < devin->ring_buffer_size; i++){
    free(devin->ring_buffer[i]);
  }

  free(devin->ring_buffer);

  devin->ring_buffer = NULL;

  /* reset flags */
  devin->flags &= (~(AGS_DEVIN_BUFFER0 |
		     AGS_DEVIN_BUFFER1 |
		     AGS_DEVIN_BUFFER2 |
		     AGS_DEVIN_BUFFER3 |
		     AGS_DEVIN_RECORD |
		     AGS_DEVIN_INITIALIZED));

  devin->note_offset = devin->start_note_offset;
  devin->note_offset_absolute = devin->start_note_offset;

  g_rec_mutex_unlock(devin_mutex);
}

void
ags_devin_tic(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  gdouble delay;
  gdouble delay_counter;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  
  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(devin_mutex);

  delay = devin->delay[devin->tic_counter];
  delay_counter = devin->delay_counter;

  note_offset = devin->note_offset;
  note_offset_absolute = devin->note_offset_absolute;
  
  loop_left = devin->loop_left;
  loop_right = devin->loop_right;
  
  do_loop = devin->do_loop;

  g_rec_mutex_unlock(devin_mutex);

  if(delay_counter + 1.0 >= delay){
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
    g_rec_mutex_lock(devin_mutex);
    
    devin->delay_counter = delay_counter + 1.0 - delay;
    devin->tact_counter += 1.0;

    g_rec_mutex_unlock(devin_mutex);
  }else{
    g_rec_mutex_lock(devin_mutex);
    
    devin->delay_counter += 1.0;

    g_rec_mutex_unlock(devin_mutex);
  }
}

void
ags_devin_offset_changed(AgsSoundcard *soundcard,
			 guint note_offset)
{
  AgsDevin *devin;
  
  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* offset changed */
  g_rec_mutex_lock(devin_mutex);

  devin->tic_counter += 1;

  if(devin->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    devin->tic_counter = 0;
  }

  g_rec_mutex_unlock(devin_mutex);
}

void
ags_devin_set_bpm(AgsSoundcard *soundcard,
		  gdouble bpm)
{
  AgsDevin *devin;

  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* set bpm */
  g_rec_mutex_lock(devin_mutex);

  devin->bpm = bpm;

  g_rec_mutex_unlock(devin_mutex);

  ags_devin_adjust_delay_and_attack(devin);
}

gdouble
ags_devin_get_bpm(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  gdouble bpm;
  
  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* get bpm */
  g_rec_mutex_lock(devin_mutex);

  bpm = devin->bpm;
  
  g_rec_mutex_unlock(devin_mutex);

  return(bpm);
}

void
ags_devin_set_delay_factor(AgsSoundcard *soundcard,
			   gdouble delay_factor)
{
  AgsDevin *devin;

  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* set delay factor */
  g_rec_mutex_lock(devin_mutex);

  devin->delay_factor = delay_factor;

  g_rec_mutex_unlock(devin_mutex);

  ags_devin_adjust_delay_and_attack(devin);
}

gdouble
ags_devin_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  gdouble delay_factor;
  
  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* get delay factor */
  g_rec_mutex_lock(devin_mutex);

  delay_factor = devin->delay_factor;
  
  g_rec_mutex_unlock(devin_mutex);

  return(delay_factor);
}

gdouble
ags_devin_get_delay(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  guint index;
  gdouble delay;
  
  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* get delay */
  g_rec_mutex_lock(devin_mutex);

  index = devin->tic_counter;

  delay = devin->delay[index];
  
  g_rec_mutex_unlock(devin_mutex);
  
  return(delay);
}

gdouble
ags_devin_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  gdouble absolute_delay;
  
  GRecMutex *devin_mutex;
  
  devin = AGS_DEVIN(soundcard);
  
  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* get absolute delay */
  g_rec_mutex_lock(devin_mutex);

  absolute_delay = (60.0 * (((gdouble) devin->samplerate / (gdouble) devin->buffer_size) / (gdouble) devin->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) devin->delay_factor)));

  g_rec_mutex_unlock(devin_mutex);

  return(absolute_delay);
}

guint
ags_devin_get_attack(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  guint index;
  guint attack;
  
  GRecMutex *devin_mutex;  

  devin = AGS_DEVIN(soundcard);
  
  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* get attack */
  g_rec_mutex_lock(devin_mutex);

  index = devin->tic_counter;

  attack = devin->attack[index];

  g_rec_mutex_unlock(devin_mutex);
  
  return(attack);
}

void*
ags_devin_get_buffer(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  void *buffer;
  
  devin = AGS_DEVIN(soundcard);

  if(ags_devin_test_flags(devin, AGS_DEVIN_BUFFER0)){
    buffer = devin->buffer[0];
  }else if(ags_devin_test_flags(devin, AGS_DEVIN_BUFFER1)){
    buffer = devin->buffer[1];
  }else if(ags_devin_test_flags(devin, AGS_DEVIN_BUFFER2)){
    buffer = devin->buffer[2];
  }else if(ags_devin_test_flags(devin, AGS_DEVIN_BUFFER3)){
    buffer = devin->buffer[3];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_devin_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  void *buffer;
  
  devin = AGS_DEVIN(soundcard);

  //  g_message("next - 0x%0x", ((AGS_DEVIN_BUFFER0 |
  //				AGS_DEVIN_BUFFER1 |
  //				AGS_DEVIN_BUFFER2 |
  //				AGS_DEVIN_BUFFER3) & (devin->flags)));

  if(ags_devin_test_flags(devin, AGS_DEVIN_BUFFER0)){
    buffer = devin->buffer[1];
  }else if(ags_devin_test_flags(devin, AGS_DEVIN_BUFFER1)){
    buffer = devin->buffer[2];
  }else if(ags_devin_test_flags(devin, AGS_DEVIN_BUFFER2)){
    buffer = devin->buffer[3];
  }else if(ags_devin_test_flags(devin, AGS_DEVIN_BUFFER3)){
    buffer = devin->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_devin_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  void *buffer;
  
  devin = AGS_DEVIN(soundcard);

  if(ags_devin_test_flags(devin, AGS_DEVIN_BUFFER0)){
    buffer = devin->buffer[3];
  }else if(ags_devin_test_flags(devin, AGS_DEVIN_BUFFER1)){
    buffer = devin->buffer[0];
  }else if(ags_devin_test_flags(devin, AGS_DEVIN_BUFFER2)){
    buffer = devin->buffer[1];
  }else if(ags_devin_test_flags(devin, AGS_DEVIN_BUFFER3)){
    buffer = devin->buffer[2];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void
ags_devin_lock_buffer(AgsSoundcard *soundcard,
		       void *buffer)
{
  AgsDevin *devin;

  GRecMutex *buffer_mutex;
  
  devin = AGS_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(devin->buffer != NULL){
    if(buffer == devin->buffer[0]){
      buffer_mutex = devin->buffer_mutex[0];
    }else if(buffer == devin->buffer[1]){
      buffer_mutex = devin->buffer_mutex[1];
    }else if(buffer == devin->buffer[2]){
      buffer_mutex = devin->buffer_mutex[2];
    }else if(buffer == devin->buffer[3]){
      buffer_mutex = devin->buffer_mutex[3];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_devin_unlock_buffer(AgsSoundcard *soundcard,
			 void *buffer)
{
  AgsDevin *devin;

  GRecMutex *buffer_mutex;
  
  devin = AGS_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(devin->buffer != NULL){
    if(buffer == devin->buffer[0]){
      buffer_mutex = devin->buffer_mutex[0];
    }else if(buffer == devin->buffer[1]){
      buffer_mutex = devin->buffer_mutex[1];
    }else if(buffer == devin->buffer[2]){
      buffer_mutex = devin->buffer_mutex[2];
    }else if(buffer == devin->buffer[3]){
      buffer_mutex = devin->buffer_mutex[3];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_devin_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  guint delay_counter;
  
  GRecMutex *devin_mutex;  

  devin = AGS_DEVIN(soundcard);
  
  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* delay counter */
  g_rec_mutex_lock(devin_mutex);

  delay_counter = devin->delay_counter;
  
  g_rec_mutex_unlock(devin_mutex);

  return(delay_counter);
}

void
ags_devin_set_start_note_offset(AgsSoundcard *soundcard,
				guint start_note_offset)
{
  AgsDevin *devin;

  GRecMutex *devin_mutex;  

  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* set note offset */
  g_rec_mutex_lock(devin_mutex);

  devin->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(devin_mutex);
}

guint
ags_devin_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  guint start_note_offset;
  
  GRecMutex *devin_mutex;  

  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* set note offset */
  g_rec_mutex_lock(devin_mutex);

  start_note_offset = devin->start_note_offset;

  g_rec_mutex_unlock(devin_mutex);

  return(start_note_offset);
}

void
ags_devin_set_note_offset(AgsSoundcard *soundcard,
			  guint note_offset)
{
  AgsDevin *devin;

  GRecMutex *devin_mutex;  

  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* set note offset */
  g_rec_mutex_lock(devin_mutex);

  devin->note_offset = note_offset;

  g_rec_mutex_unlock(devin_mutex);
}

guint
ags_devin_get_note_offset(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  guint note_offset;
  
  GRecMutex *devin_mutex;  

  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* set note offset */
  g_rec_mutex_lock(devin_mutex);

  note_offset = devin->note_offset;

  g_rec_mutex_unlock(devin_mutex);

  return(note_offset);
}

void
ags_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
				   guint note_offset_absolute)
{
  AgsDevin *devin;
  
  GRecMutex *devin_mutex;  

  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* set note offset */
  g_rec_mutex_lock(devin_mutex);

  devin->note_offset_absolute = note_offset_absolute;

  g_rec_mutex_unlock(devin_mutex);
}

guint
ags_devin_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  guint note_offset_absolute;
  
  GRecMutex *devin_mutex;  

  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* set note offset */
  g_rec_mutex_lock(devin_mutex);

  note_offset_absolute = devin->note_offset_absolute;

  g_rec_mutex_unlock(devin_mutex);

  return(note_offset_absolute);
}

void
ags_devin_set_loop(AgsSoundcard *soundcard,
		   guint loop_left, guint loop_right,
		   gboolean do_loop)
{
  AgsDevin *devin;

  GRecMutex *devin_mutex;  

  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* set loop */
  g_rec_mutex_lock(devin_mutex);

  devin->loop_left = loop_left;
  devin->loop_right = loop_right;
  devin->do_loop = do_loop;

  if(do_loop){
    devin->loop_offset = devin->note_offset;
  }

  g_rec_mutex_unlock(devin_mutex);
}

void
ags_devin_get_loop(AgsSoundcard *soundcard,
		   guint *loop_left, guint *loop_right,
		   gboolean *do_loop)
{
  AgsDevin *devin;

  GRecMutex *devin_mutex;  

  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* get loop */
  g_rec_mutex_lock(devin_mutex);

  if(loop_left != NULL){
    *loop_left = devin->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = devin->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = devin->do_loop;
  }

  g_rec_mutex_unlock(devin_mutex);
}

guint
ags_devin_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsDevin *devin;

  guint loop_offset;
  
  GRecMutex *devin_mutex;  

  devin = AGS_DEVIN(soundcard);

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* get loop offset */
  g_rec_mutex_lock(devin_mutex);

  loop_offset = devin->loop_offset;
  
  g_rec_mutex_unlock(devin_mutex);

  return(loop_offset);
}

/**
 * ags_devin_switch_buffer_flag:
 * @devin: the #AgsDevin
 *
 * The buffer flag indicates the currently recorded buffer.
 *
 * Since: 3.0.0
 */
void
ags_devin_switch_buffer_flag(AgsDevin *devin)
{
  GRecMutex *devin_mutex;
  
  if(!AGS_IS_DEVIN(devin)){
    return;
  }

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* switch buffer flag */
  g_rec_mutex_lock(devin_mutex);

  if((AGS_DEVIN_BUFFER0 & (devin->flags)) != 0){
    devin->flags &= (~AGS_DEVIN_BUFFER0);
    devin->flags |= AGS_DEVIN_BUFFER1;
  }else if((AGS_DEVIN_BUFFER1 & (devin->flags)) != 0){
    devin->flags &= (~AGS_DEVIN_BUFFER1);
    devin->flags |= AGS_DEVIN_BUFFER2;
  }else if((AGS_DEVIN_BUFFER2 & (devin->flags)) != 0){
    devin->flags &= (~AGS_DEVIN_BUFFER2);
    devin->flags |= AGS_DEVIN_BUFFER3;
  }else if((AGS_DEVIN_BUFFER3 & (devin->flags)) != 0){
    devin->flags &= (~AGS_DEVIN_BUFFER3);
    devin->flags |= AGS_DEVIN_BUFFER0;
  }

  g_rec_mutex_unlock(devin_mutex);
}

/**
 * ags_devin_adjust_delay_and_attack:
 * @devin: the #AgsDevin
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 3.0.0
 */
void
ags_devin_adjust_delay_and_attack(AgsDevin *devin)
{
  if(!AGS_IS_DEVIN(devin)){
    return;
  }
  
  ags_soundcard_util_adjust_delay_and_attack(devin);
}

/**
 * ags_devin_realloc_buffer:
 * @devin: the #AgsDevin
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.0.0
 */
void
ags_devin_realloc_buffer(AgsDevin *devin)
{
  guint pcm_channels;
  guint buffer_size;
  guint word_size;
  
  GRecMutex *devin_mutex;  

  if(!AGS_IS_DEVIN(devin)){
    return;
  }

  /* get devin mutex */
  devin_mutex = AGS_DEVIN_GET_OBJ_MUTEX(devin);

  /* get word size */  
  g_rec_mutex_lock(devin_mutex);

  pcm_channels = devin->pcm_channels;
  buffer_size = devin->buffer_size;
  
  switch(devin->format){
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
    g_warning("ags_devin_realloc_buffer(): unsupported word size");
    return;
  }  

  g_rec_mutex_unlock(devin_mutex);

  //NOTE:JK: there is no lock applicable to buffer
  
  /* AGS_DEVIN_BUFFER_0 */
  if(devin->buffer[0] != NULL){
    free(devin->buffer[0]);
  }
  
  devin->buffer[0] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_DEVIN_BUFFER_1 */
  if(devin->buffer[1] != NULL){
    free(devin->buffer[1]);
  }

  devin->buffer[1] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_DEVIN_BUFFER_2 */
  if(devin->buffer[2] != NULL){
    free(devin->buffer[2]);
  }

  devin->buffer[2] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_DEVIN_BUFFER_3 */
  if(devin->buffer[3] != NULL){
    free(devin->buffer[3]);
  }
  
  devin->buffer[3] = (void *) malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_devin_new:
 *
 * Creates a new instance of #AgsDevin.
 *
 * Returns: the new #AgsDevin
 *
 * Since: 3.0.0
 */
AgsDevin*
ags_devin_new()
{
  AgsDevin *devin;

  devin = (AgsDevin *) g_object_new(AGS_TYPE_DEVIN,
				    NULL);
  
  return(devin);
}
