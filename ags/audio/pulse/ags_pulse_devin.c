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

#include <ags/audio/pulse/ags_pulse_devin.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_channel.h>
#include <ags/audio/ags_soundcard_util.h>
#include <ags/audio/ags_audio_buffer_util.h>

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

void ags_pulse_devin_class_init(AgsPulseDevinClass *pulse_devin);
void ags_pulse_devin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_pulse_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_pulse_devin_init(AgsPulseDevin *pulse_devin);
void ags_pulse_devin_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_pulse_devin_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_pulse_devin_dispose(GObject *gobject);
void ags_pulse_devin_finalize(GObject *gobject);

AgsUUID* ags_pulse_devin_get_uuid(AgsConnectable *connectable);
gboolean ags_pulse_devin_has_resource(AgsConnectable *connectable);
gboolean ags_pulse_devin_is_ready(AgsConnectable *connectable);
void ags_pulse_devin_add_to_registry(AgsConnectable *connectable);
void ags_pulse_devin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_pulse_devin_list_resource(AgsConnectable *connectable);
xmlNode* ags_pulse_devin_xml_compose(AgsConnectable *connectable);
void ags_pulse_devin_xml_parse(AgsConnectable *connectable,
			       xmlNode *node);
gboolean ags_pulse_devin_is_connected(AgsConnectable *connectable);
void ags_pulse_devin_connect(AgsConnectable *connectable);
void ags_pulse_devin_disconnect(AgsConnectable *connectable);

void ags_pulse_devin_set_device(AgsSoundcard *soundcard,
				gchar *device);
gchar* ags_pulse_devin_get_device(AgsSoundcard *soundcard);

void ags_pulse_devin_set_presets(AgsSoundcard *soundcard,
				 guint channels,
				 guint rate,
				 guint buffer_size,
				 AgsSoundcardFormat format);
void ags_pulse_devin_get_presets(AgsSoundcard *soundcard,
				 guint *channels,
				 guint *rate,
				 guint *buffer_size,
				 AgsSoundcardFormat *format);

void ags_pulse_devin_list_cards(AgsSoundcard *soundcard,
				GList **card_id, GList **card_name);
void ags_pulse_devin_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			      guint *channels_min, guint *channels_max,
			      guint *rate_min, guint *rate_max,
			      guint *buffer_size_min, guint *buffer_size_max,
			      GError **error);
guint ags_pulse_devin_get_capability(AgsSoundcard *soundcard);

gboolean ags_pulse_devin_is_starting(AgsSoundcard *soundcard);
gboolean ags_pulse_devin_is_recording(AgsSoundcard *soundcard);

gchar* ags_pulse_devin_get_uptime(AgsSoundcard *soundcard);

void ags_pulse_devin_port_init(AgsSoundcard *soundcard,
			       GError **error);
void ags_pulse_devin_port_record(AgsSoundcard *soundcard,
				 GError **error);
void ags_pulse_devin_port_free(AgsSoundcard *soundcard);

void ags_pulse_devin_tic(AgsSoundcard *soundcard);
void ags_pulse_devin_offset_changed(AgsSoundcard *soundcard,
				    guint64 note_offset);

void ags_pulse_devin_set_bpm(AgsSoundcard *soundcard,
			     gdouble bpm);
gdouble ags_pulse_devin_get_bpm(AgsSoundcard *soundcard);

void ags_pulse_devin_set_start_note_offset(AgsSoundcard *soundcard,
					   guint64 start_note_offset);
guint64 ags_pulse_devin_get_start_note_offset(AgsSoundcard *soundcard);

GObject* ags_pulse_devin_get_frame_clock(AgsSoundcard *soundcard);

void* ags_pulse_devin_get_buffer(AgsSoundcard *soundcard);
void* ags_pulse_devin_get_next_buffer(AgsSoundcard *soundcard);
void* ags_pulse_devin_get_prev_buffer(AgsSoundcard *soundcard);

/**
 * SECTION:ags_pulse_devin
 * @short_description: Input from soundcard
 * @title: AgsPulseDevin
 * @section_id:
 * @include: ags/audio/pulse/ags_pulse_devin.h
 *
 * #AgsPulseDevin represents a soundcard and supports input.
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

static gpointer ags_pulse_devin_parent_class = NULL;

GType
ags_pulse_devin_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_pulse_devin = 0;

    static const GTypeInfo ags_pulse_devin_info = {
      sizeof(AgsPulseDevinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_pulse_devin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsPulseDevin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_pulse_devin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_pulse_devin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_pulse_devin_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_pulse_devin = g_type_register_static(G_TYPE_OBJECT,
						  "AgsPulseDevin",
						  &ags_pulse_devin_info,
						  0);

    g_type_add_interface_static(ags_type_pulse_devin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_pulse_devin,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_pulse_devin);
  }

  return(g_define_type_id__static);
}

GType
ags_pulse_devin_flags_get_type()
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_PULSE_DEVIN_INITIALIZED, "AGS_PULSE_DEVIN_INITIALIZED", "pulse-devin-initialized" },
      { AGS_PULSE_DEVIN_START_RECORD, "AGS_PULSE_DEVIN_START_RECORD", "pulse-devin-start-record" },
      { AGS_PULSE_DEVIN_RECORD, "AGS_PULSE_DEVIN_RECORD", "pulse-devin-record" },
      { AGS_PULSE_DEVIN_SHUTDOWN, "AGS_PULSE_DEVIN_SHUTDOWN", "pulse-devin-shutdown" },
      { AGS_PULSE_DEVIN_NONBLOCKING, "AGS_PULSE_DEVIN_NONBLOCKING", "pulse-devin-nonblocking" },
      { AGS_PULSE_DEVIN_ATTACK_FIRST, "AGS_PULSE_DEVIN_ATTACK_FIRST", "pulse-devin-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsPulseDevinFlags"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
}

void
ags_pulse_devin_class_init(AgsPulseDevinClass *pulse_devin)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_pulse_devin_parent_class = g_type_class_peek_parent(pulse_devin);

  /* GObjectClass */
  gobject = (GObjectClass *) pulse_devin;

  gobject->set_property = ags_pulse_devin_set_property;
  gobject->get_property = ags_pulse_devin_get_property;

  gobject->dispose = ags_pulse_devin_dispose;
  gobject->finalize = ags_pulse_devin_finalize;

  /* properties */
  /**
   * AgsPulseDevin:device:
   *
   * The pulse soundcard indentifier
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-pulse-devin-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsPulseDevin:dsp-channels:
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
   * AgsPulseDevin:pcm-channels:
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
   * AgsPulseDevin:format:
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
   * AgsPulseDevin:buffer-size:
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
   * AgsPulseDevin:samplerate:
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
   * AgsPulseDevin:bpm:
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
   * AgsPulseDevin:buffer:
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
   * AgsPulseDevin:pulse-client:
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
   * AgsPulseDevin:pulse-port:
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
ags_pulse_devin_error_quark()
{
  return(g_quark_from_static_string("ags-pulse_devin-error-quark"));
}

void
ags_pulse_devin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_pulse_devin_get_uuid;
  connectable->has_resource = ags_pulse_devin_has_resource;

  connectable->is_ready = ags_pulse_devin_is_ready;
  connectable->add_to_registry = ags_pulse_devin_add_to_registry;
  connectable->remove_from_registry = ags_pulse_devin_remove_from_registry;

  connectable->list_resource = ags_pulse_devin_list_resource;
  connectable->xml_compose = ags_pulse_devin_xml_compose;
  connectable->xml_parse = ags_pulse_devin_xml_parse;

  connectable->is_connected = ags_pulse_devin_is_connected;  
  connectable->connect = ags_pulse_devin_connect;
  connectable->disconnect = ags_pulse_devin_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_pulse_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_pulse_devin_set_device;
  soundcard->get_device = ags_pulse_devin_get_device;
  
  soundcard->set_presets = ags_pulse_devin_set_presets;
  soundcard->get_presets = ags_pulse_devin_get_presets;

  soundcard->list_cards = ags_pulse_devin_list_cards;
  soundcard->pcm_info = ags_pulse_devin_pcm_info;
  soundcard->get_capability = ags_pulse_devin_get_capability;

  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_pulse_devin_is_starting;
  soundcard->is_playing = NULL;
  soundcard->is_recording = ags_pulse_devin_is_recording;

  soundcard->get_uptime = ags_pulse_devin_get_uptime;

  soundcard->play_init = NULL;
  soundcard->play = NULL;
  
  soundcard->record_init = ags_pulse_devin_port_init;
  soundcard->record = ags_pulse_devin_port_record;

  soundcard->stop = ags_pulse_devin_port_free;

  soundcard->tic = ags_pulse_devin_tic;
  soundcard->offset_changed = ags_pulse_devin_offset_changed;
    
  soundcard->set_bpm = ags_pulse_devin_set_bpm;
  soundcard->get_bpm = ags_pulse_devin_get_bpm;

  soundcard->set_start_note_offset = ags_pulse_devin_set_start_note_offset;
  soundcard->get_start_note_offset = ags_pulse_devin_get_start_note_offset;

  soundcard->get_frame_clock = ags_pulse_devin_get_frame_clock;

  soundcard->get_buffer = ags_pulse_devin_get_buffer;
  soundcard->get_next_buffer = ags_pulse_devin_get_next_buffer;
  soundcard->get_prev_buffer = ags_pulse_devin_get_prev_buffer;
}

void
ags_pulse_devin_init(AgsPulseDevin *pulse_devin)
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
  pulse_devin->flags = 0;
  pulse_devin->connectable_flags = 0;
  ags_atomic_int_set(&(pulse_devin->sync_flags),
		   AGS_PULSE_DEVIN_PASS_THROUGH);

  /* insert devin mutex */
  g_rec_mutex_init(&(pulse_devin->obj_mutex));

  /* uuid */
  pulse_devin->uuid = ags_uuid_alloc();
  ags_uuid_generate(pulse_devin->uuid);

  /* presets */
  pulse_devin->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  pulse_devin->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  pulse_devin->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  pulse_devin->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  pulse_devin->format = ags_soundcard_helper_config_get_format(config);
  
  /* bpm */
  pulse_devin->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* start note offset */
  pulse_devin->start_note_offset = 0;

  /* frame clock */
  pulse_devin->frame_clock = ags_frame_clock_new();

  /* buffer */
  pulse_devin->app_buffer_mode = AGS_PULSE_DEVIN_APP_BUFFER_0;

  pulse_devin->app_buffer_mutex = (GRecMutex **) g_malloc(8 * sizeof(GRecMutex *));

  for(i = 0; i < 8; i++){
    pulse_devin->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(pulse_devin->app_buffer_mutex[i]);
  }

  pulse_devin->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  pulse_devin->sub_block_mutex = (GRecMutex **) g_malloc(8 * pulse_devin->sub_block_count * pulse_devin->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < 8 * pulse_devin->sub_block_count * pulse_devin->pcm_channels; i++){
    pulse_devin->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(pulse_devin->sub_block_mutex[i]);
  }

  pulse_devin->app_buffer = (void **) g_malloc(8 * sizeof(void*));

  pulse_devin->app_buffer[0] = NULL;
  pulse_devin->app_buffer[1] = NULL;
  pulse_devin->app_buffer[2] = NULL;
  pulse_devin->app_buffer[3] = NULL;
  pulse_devin->app_buffer[4] = NULL;
  pulse_devin->app_buffer[5] = NULL;
  pulse_devin->app_buffer[6] = NULL;
  pulse_devin->app_buffer[7] = NULL;

  ags_pulse_devin_realloc_buffer(pulse_devin);

  /*  */
  pulse_devin->card_uri = NULL;
  pulse_devin->pulse_client = NULL;

  pulse_devin->port_name = NULL;
  pulse_devin->pulse_port = NULL;

  /* callback mutex */
  g_mutex_init(&(pulse_devin->callback_mutex));

  g_cond_init(&(pulse_devin->callback_cond));

  /* callback finish mutex */
  g_mutex_init(&(pulse_devin->callback_finish_mutex));

  g_cond_init(&(pulse_devin->callback_finish_cond));
}

void
ags_pulse_devin_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsPulseDevin *pulse_devin;
  
  GRecMutex *pulse_devin_mutex;

  pulse_devin = AGS_PULSE_DEVIN(gobject);

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      g_rec_mutex_lock(pulse_devin_mutex);

      pulse_devin->card_uri = g_strdup(device);

      g_rec_mutex_unlock(pulse_devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      g_rec_mutex_lock(pulse_devin_mutex);

      if(dsp_channels == pulse_devin->dsp_channels){
	g_rec_mutex_unlock(pulse_devin_mutex);

	return;
      }

      pulse_devin->dsp_channels = dsp_channels;

      g_rec_mutex_unlock(pulse_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      g_rec_mutex_lock(pulse_devin_mutex);

      if(pcm_channels == pulse_devin->pcm_channels){
	g_rec_mutex_unlock(pulse_devin_mutex);

	return;
      }

      pulse_devin->pcm_channels = pcm_channels;

      g_rec_mutex_unlock(pulse_devin_mutex);

      ags_pulse_devin_realloc_buffer(pulse_devin);
    }
    break;
  case PROP_FORMAT:
    {
      AgsSoundcardFormat format;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(pulse_devin_mutex);

      if(format == pulse_devin->format){
	g_rec_mutex_unlock(pulse_devin_mutex);

	return;
      }

      pulse_devin->format = format;

      g_rec_mutex_unlock(pulse_devin_mutex);

      ags_pulse_devin_realloc_buffer(pulse_devin);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      g_rec_mutex_lock(pulse_devin_mutex);

      if(buffer_size == pulse_devin->buffer_size){
	g_rec_mutex_unlock(pulse_devin_mutex);

	return;
      }

      pulse_devin->buffer_size = buffer_size;

      ags_frame_clock_set_buffer_size(pulse_devin->frame_clock,
				      buffer_size);

      g_rec_mutex_unlock(pulse_devin_mutex);

      ags_pulse_devin_realloc_buffer(pulse_devin);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      g_rec_mutex_lock(pulse_devin_mutex);
      
      if(samplerate == pulse_devin->samplerate){
	g_rec_mutex_unlock(pulse_devin_mutex);

	return;
      }

      pulse_devin->samplerate = samplerate;

      ags_frame_clock_set_samplerate(pulse_devin->frame_clock,
				     samplerate);

      g_rec_mutex_unlock(pulse_devin_mutex);

      ags_pulse_devin_realloc_buffer(pulse_devin);
    }
    break;
  case PROP_BPM:
    {
      gdouble bpm;
      
      bpm = g_value_get_double(value);

      g_rec_mutex_lock(pulse_devin_mutex);

      pulse_devin->bpm = bpm;

      ags_frame_clock_set_bpm(pulse_devin->frame_clock,
			      bpm);

      g_rec_mutex_unlock(pulse_devin_mutex);
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

      g_rec_mutex_lock(pulse_devin_mutex);

      if(pulse_devin->pulse_client == (GObject *) pulse_client){
	g_rec_mutex_unlock(pulse_devin_mutex);

	return;
      }

      if(pulse_devin->pulse_client != NULL){
	g_object_unref(G_OBJECT(pulse_devin->pulse_client));
      }

      if(pulse_client != NULL){
	g_object_ref(pulse_client);
      }
      
      pulse_devin->pulse_client = (GObject *) pulse_client;

      g_rec_mutex_unlock(pulse_devin_mutex);
    }
    break;
  case PROP_PULSE_PORT:
    {
      AgsPulsePort *pulse_port;

      pulse_port = (AgsPulsePort *) g_value_get_pointer(value);

      g_rec_mutex_lock(pulse_devin_mutex);

      if(!AGS_IS_PULSE_PORT(pulse_port) ||
	 g_list_find(pulse_devin->pulse_port, pulse_port) != NULL){
	g_rec_mutex_unlock(pulse_devin_mutex);

	return;
      }

      g_object_ref(pulse_port);
      pulse_devin->pulse_port = g_list_append(pulse_devin->pulse_port,
					      pulse_port);

      g_rec_mutex_unlock(pulse_devin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_devin_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsPulseDevin *pulse_devin;

  GRecMutex *pulse_devin_mutex;

  pulse_devin = AGS_PULSE_DEVIN(gobject);

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(pulse_devin_mutex);

      g_value_set_string(value, pulse_devin->card_uri);

      g_rec_mutex_unlock(pulse_devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_rec_mutex_lock(pulse_devin_mutex);

      g_value_set_uint(value, pulse_devin->dsp_channels);

      g_rec_mutex_unlock(pulse_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_rec_mutex_lock(pulse_devin_mutex);

      g_value_set_uint(value, pulse_devin->pcm_channels);

      g_rec_mutex_unlock(pulse_devin_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(pulse_devin_mutex);

      g_value_set_uint(value, pulse_devin->format);

      g_rec_mutex_unlock(pulse_devin_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(pulse_devin_mutex);

      g_value_set_uint(value, pulse_devin->buffer_size);

      g_rec_mutex_unlock(pulse_devin_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(pulse_devin_mutex);

      g_value_set_uint(value, pulse_devin->samplerate);

      g_rec_mutex_unlock(pulse_devin_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(pulse_devin_mutex);

      g_value_set_double(value, pulse_devin->bpm);

      g_rec_mutex_unlock(pulse_devin_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(pulse_devin_mutex);

      g_value_set_pointer(value, pulse_devin->app_buffer);

      g_rec_mutex_unlock(pulse_devin_mutex);
    }
    break;
  case PROP_PULSE_CLIENT:
    {
      g_rec_mutex_lock(pulse_devin_mutex);

      g_value_set_object(value, pulse_devin->pulse_client);

      g_rec_mutex_unlock(pulse_devin_mutex);
    }
    break;
  case PROP_PULSE_PORT:
    {
      g_rec_mutex_lock(pulse_devin_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(pulse_devin->pulse_port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(pulse_devin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_pulse_devin_dispose(GObject *gobject)
{
  AgsPulseDevin *pulse_devin;

  pulse_devin = AGS_PULSE_DEVIN(gobject);

  /* pulse client */
  if(pulse_devin->pulse_client != NULL){
    g_object_unref(pulse_devin->pulse_client);

    pulse_devin->pulse_client = NULL;
  }

  /* pulse port */
  g_list_free_full(pulse_devin->pulse_port,
		   g_object_unref);

  pulse_devin->pulse_port = NULL;

  /* call parent */
  G_OBJECT_CLASS(ags_pulse_devin_parent_class)->dispose(gobject);
}

void
ags_pulse_devin_finalize(GObject *gobject)
{
  AgsPulseDevin *pulse_devin;

  guint i;
  
  pulse_devin = AGS_PULSE_DEVIN(gobject);

  ags_uuid_free(pulse_devin->uuid);

  /* frame clock */
  if(pulse_devin->frame_clock != NULL){
    g_object_unref(pulse_devin->frame_clock);
  }

  /* app buffer */
  for(i = 0; i < AGS_PULSE_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_free(pulse_devin->app_buffer[i]);
  }

  g_free(pulse_devin->app_buffer);

  /* app buffer mutex */
  for(i = 0; i < AGS_PULSE_DEVIN_DEFAULT_APP_BUFFER_SIZE; i++){
    g_rec_mutex_clear(pulse_devin->app_buffer_mutex[i]);
    
    g_free(pulse_devin->app_buffer_mutex[i]);
  }

  g_free(pulse_devin->app_buffer_mutex);
  
  /* sub-block mutex */
  for(i = 0; i < AGS_PULSE_DEVIN_DEFAULT_APP_BUFFER_SIZE * pulse_devin->sub_block_count * pulse_devin->pcm_channels; i++){
    g_rec_mutex_clear(pulse_devin->sub_block_mutex[i]);
    
    g_free(pulse_devin->sub_block_mutex[i]);
  }

  g_free(pulse_devin->sub_block_mutex);

  /* pulse client */
  if(pulse_devin->pulse_client != NULL){
    g_object_unref(pulse_devin->pulse_client);
  }

  /* pulse port */
  g_list_free_full(pulse_devin->pulse_port,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_pulse_devin_parent_class)->finalize(gobject);
}

AgsUUID*
ags_pulse_devin_get_uuid(AgsConnectable *connectable)
{
  AgsPulseDevin *pulse_devin;
  
  AgsUUID *ptr;

  GRecMutex *pulse_devin_mutex;

  pulse_devin = AGS_PULSE_DEVIN(connectable);

  /* get pulse devin signal mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* get UUID */
  g_rec_mutex_lock(pulse_devin_mutex);

  ptr = pulse_devin->uuid;

  g_rec_mutex_unlock(pulse_devin_mutex);
  
  return(ptr);
}

gboolean
ags_pulse_devin_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_pulse_devin_is_ready(AgsConnectable *connectable)
{
  AgsPulseDevin *pulse_devin;
  
  gboolean is_ready;

  GRecMutex *pulse_devin_mutex;

  pulse_devin = AGS_PULSE_DEVIN(connectable);

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* check is ready */
  g_rec_mutex_lock(pulse_devin_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (pulse_devin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(pulse_devin_mutex);
  
  return(is_ready);
}

void
ags_pulse_devin_add_to_registry(AgsConnectable *connectable)
{
  AgsPulseDevin *pulse_devin;

  GRecMutex *pulse_devin_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  pulse_devin = AGS_PULSE_DEVIN(connectable);

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  g_rec_mutex_lock(pulse_devin_mutex);

  pulse_devin->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(pulse_devin_mutex);
}

void
ags_pulse_devin_remove_from_registry(AgsConnectable *connectable)
{
  AgsPulseDevin *pulse_devin;

  GRecMutex *pulse_devin_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  pulse_devin = AGS_PULSE_DEVIN(connectable);

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  g_rec_mutex_lock(pulse_devin_mutex);

  pulse_devin->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(pulse_devin_mutex);
}

xmlNode*
ags_pulse_devin_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_pulse_devin_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_pulse_devin_xml_parse(AgsConnectable *connectable,
			  xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_pulse_devin_is_connected(AgsConnectable *connectable)
{
  AgsPulseDevin *pulse_devin;
  
  gboolean is_connected;

  GRecMutex *pulse_devin_mutex;

  pulse_devin = AGS_PULSE_DEVIN(connectable);

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* check is connected */
  g_rec_mutex_lock(pulse_devin_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (pulse_devin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(pulse_devin_mutex);
  
  return(is_connected);
}

void
ags_pulse_devin_connect(AgsConnectable *connectable)
{
  AgsPulseDevin *pulse_devin;

  GRecMutex *pulse_devin_mutex;
    
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  pulse_devin = AGS_PULSE_DEVIN(connectable);

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  g_rec_mutex_lock(pulse_devin_mutex);

  pulse_devin->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(pulse_devin_mutex);
}

void
ags_pulse_devin_disconnect(AgsConnectable *connectable)
{
  AgsPulseDevin *pulse_devin;

  GRecMutex *pulse_devin_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  pulse_devin = AGS_PULSE_DEVIN(connectable);

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  g_rec_mutex_lock(pulse_devin_mutex);

  pulse_devin->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(pulse_devin_mutex);
}

/**
 * ags_pulse_devin_test_flags:
 * @pulse_devin: the #AgsPulseDevin
 * @flags: the flags
 *
 * Test @flags to be set on @pulse_devin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_pulse_devin_test_flags(AgsPulseDevin *pulse_devin, AgsPulseDevinFlags flags)
{
  gboolean retval;  
  
  GRecMutex *pulse_devin_mutex;

  if(!AGS_IS_PULSE_DEVIN(pulse_devin)){
    return(FALSE);
  }

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* test */
  g_rec_mutex_lock(pulse_devin_mutex);

  retval = (flags & (pulse_devin->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(pulse_devin_mutex);

  return(retval);
}

/**
 * ags_pulse_devin_set_flags:
 * @pulse_devin: the #AgsPulseDevin
 * @flags: see #AgsPulseDevinFlags-enum
 *
 * Enable a feature of @pulse_devin.
 *
 * Since: 3.0.0
 */
void
ags_pulse_devin_set_flags(AgsPulseDevin *pulse_devin, AgsPulseDevinFlags flags)
{
  GRecMutex *pulse_devin_mutex;

  if(!AGS_IS_PULSE_DEVIN(pulse_devin)){
    return;
  }

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(pulse_devin_mutex);

  pulse_devin->flags |= flags;
  
  g_rec_mutex_unlock(pulse_devin_mutex);
}
    
/**
 * ags_pulse_devin_unset_flags:
 * @pulse_devin: the #AgsPulseDevin
 * @flags: see #AgsPulseDevinFlags-enum
 *
 * Disable a feature of @pulse_devin.
 *
 * Since: 3.0.0
 */
void
ags_pulse_devin_unset_flags(AgsPulseDevin *pulse_devin, AgsPulseDevinFlags flags)
{  
  GRecMutex *pulse_devin_mutex;

  if(!AGS_IS_PULSE_DEVIN(pulse_devin)){
    return;
  }

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(pulse_devin_mutex);

  pulse_devin->flags &= (~flags);
  
  g_rec_mutex_unlock(pulse_devin_mutex);
}

void
ags_pulse_devin_set_device(AgsSoundcard *soundcard,
			   gchar *device)
{
  AgsPulseDevin *pulse_devin;

  GList *pulse_port, *pulse_port_start;

  gchar *str;

  guint pcm_channels;
  int ret;
  guint nth_card;
  guint i;
  
  GRecMutex *pulse_devin_mutex;

  pulse_devin = AGS_PULSE_DEVIN(soundcard);

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* check device */
  g_rec_mutex_lock(pulse_devin_mutex);

  if(pulse_devin->card_uri == device ||
     (pulse_devin->card_uri != NULL &&
      !g_ascii_strcasecmp(pulse_devin->card_uri,
			  device))){
    g_rec_mutex_unlock(pulse_devin_mutex);
  
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-pulse-devin-")){
    g_rec_mutex_unlock(pulse_devin_mutex);

    g_warning("invalid pulseaudio device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-pulse-devin-%u",
	       &nth_card);

  if(ret != 1){
    g_rec_mutex_unlock(pulse_devin_mutex);

    g_warning("invalid pulseaudio device specifier");

    return;
  }

  g_free(pulse_devin->card_uri);
  pulse_devin->card_uri = g_strdup(device);

  /* apply name to port */
  pcm_channels = pulse_devin->pcm_channels;
  
  pulse_port_start = 
    pulse_port = g_list_copy(pulse_devin->pulse_port);

  g_rec_mutex_unlock(pulse_devin_mutex);
  
  for(i = 0; i < pcm_channels; i++){
    str = g_strdup_printf("ags-soundcard%d-%04d",
			  nth_card,
			  i);
    
    g_object_set(pulse_port->data,
		 "port-name", str,
		 NULL);
    g_free(str);

    pulse_port = pulse_port->next;
  }

  g_list_free(pulse_port_start);
}

gchar*
ags_pulse_devin_get_device(AgsSoundcard *soundcard)
{
  AgsPulseDevin *pulse_devin;
  
  gchar *device;

  GRecMutex *pulse_devin_mutex;

  pulse_devin = AGS_PULSE_DEVIN(soundcard);
  
  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  device = NULL;

  /* get device */
  g_rec_mutex_lock(pulse_devin_mutex);

  device = g_strdup(pulse_devin->card_uri);

  g_rec_mutex_unlock(pulse_devin_mutex);
  
  return(device);
}

void
ags_pulse_devin_set_presets(AgsSoundcard *soundcard,
			    guint channels,
			    guint rate,
			    guint buffer_size,
			    AgsSoundcardFormat format)
{
  AgsPulseDevin *pulse_devin;

  pulse_devin = AGS_PULSE_DEVIN(soundcard);

  g_object_set(pulse_devin,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_pulse_devin_get_presets(AgsSoundcard *soundcard,
			    guint *channels,
			    guint *rate,
			    guint *buffer_size,
			    AgsSoundcardFormat *format)
{
  AgsPulseDevin *pulse_devin;

  GRecMutex *pulse_devin_mutex;

  pulse_devin = AGS_PULSE_DEVIN(soundcard);
  
  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* get presets */
  g_rec_mutex_lock(pulse_devin_mutex);

  if(channels != NULL){
    *channels = pulse_devin->pcm_channels;
  }

  if(rate != NULL){
    *rate = pulse_devin->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = pulse_devin->buffer_size;
  }

  if(format != NULL){
    *format = pulse_devin->format;
  }

  g_rec_mutex_unlock(pulse_devin_mutex);
}

void
ags_pulse_devin_list_cards(AgsSoundcard *soundcard,
			   GList **card_id, GList **card_name)
{
  AgsPulseClient *pulse_client;
  AgsPulseDevin *pulse_devin;

  AgsApplicationContext *application_context;
  
  GList *list_start, *list;

  gchar *card_uri;
  gchar *client_name;
  
  pulse_devin = AGS_PULSE_DEVIN(soundcard);

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
    if(AGS_IS_PULSE_DEVIN(list->data)){
      if(card_id != NULL){
	card_uri = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));
	
	if(AGS_PULSE_DEVIN(list->data)->card_uri != NULL){
	  *card_id = g_list_prepend(*card_id,
				    card_uri);
	}else{
	  *card_id = g_list_prepend(*card_id,
				    g_strdup("(null)"));

	  g_warning("ags_pulse_devin_list_cards() - card id (null)");
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

	  g_warning("ags_pulse_devin_list_cards() - pulseaudio client not connected (null)");
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
ags_pulse_devin_pcm_info(AgsSoundcard *soundcard,
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
ags_pulse_devin_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_CAPTURE);
}

gboolean
ags_pulse_devin_is_starting(AgsSoundcard *soundcard)
{
  AgsPulseDevin *pulse_devin;

  gboolean is_starting;
  
  GRecMutex *pulse_devin_mutex;
  
  pulse_devin = AGS_PULSE_DEVIN(soundcard);

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* check is starting */
  g_rec_mutex_lock(pulse_devin_mutex);

  is_starting = ((AGS_PULSE_DEVIN_START_RECORD & (pulse_devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(pulse_devin_mutex);
  
  return(is_starting);
}

gboolean
ags_pulse_devin_is_recording(AgsSoundcard *soundcard)
{
  AgsPulseDevin *pulse_devin;

  gboolean is_recording;
  
  GRecMutex *pulse_devin_mutex;

  pulse_devin = AGS_PULSE_DEVIN(soundcard);
  
  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* check is starting */
  g_rec_mutex_lock(pulse_devin_mutex);

  is_recording = ((AGS_PULSE_DEVIN_RECORD & (pulse_devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(pulse_devin_mutex);

  return(is_recording);
}

gchar*
ags_pulse_devin_get_uptime(AgsSoundcard *soundcard)
{
  AgsPulseDevin *pulse_devin;

  gchar *uptime;

  GRecMutex *pulse_devin_mutex;
  
  pulse_devin = AGS_PULSE_DEVIN(soundcard);
  
  /* get core audio devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* get time string */
  g_rec_mutex_lock(pulse_devin_mutex);
  
  uptime = ags_frame_clock_to_time_string(pulse_devin->frame_clock);

  g_rec_mutex_unlock(pulse_devin_mutex);
  
  return(uptime);
}
  
void
ags_pulse_devin_port_init(AgsSoundcard *soundcard,
			  GError **error)
{
  AgsPulseDevin *pulse_devin;

  AgsSoundcardFormat format;
  guint word_size;
  
  GRecMutex *pulse_devin_mutex;
  
  pulse_devin = AGS_PULSE_DEVIN(soundcard);
  
  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* retrieve word size */
  g_rec_mutex_lock(pulse_devin_mutex);

  switch(pulse_devin->format){
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
    g_rec_mutex_unlock(pulse_devin_mutex);
    
    g_warning("ags_pulse_devin_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for playback */
  pulse_devin->app_buffer_mode = AGS_PULSE_DEVIN_APP_BUFFER_7;  
  pulse_devin->flags |= (AGS_PULSE_DEVIN_START_RECORD |
			 AGS_PULSE_DEVIN_RECORD |
			 AGS_PULSE_DEVIN_NONBLOCKING);

  memset(pulse_devin->app_buffer[0], 0, pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);
  memset(pulse_devin->app_buffer[1], 0, pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);
  memset(pulse_devin->app_buffer[2], 0, pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);
  memset(pulse_devin->app_buffer[3], 0, pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);
  memset(pulse_devin->app_buffer[4], 0, pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);
  memset(pulse_devin->app_buffer[5], 0, pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);
  memset(pulse_devin->app_buffer[6], 0, pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);
  memset(pulse_devin->app_buffer[7], 0, pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);

  /*  */
  pulse_devin->flags |= (AGS_PULSE_DEVIN_INITIALIZED |
			 AGS_PULSE_DEVIN_START_RECORD |
			 AGS_PULSE_DEVIN_RECORD);
  
  ags_frame_clock_start(pulse_devin->frame_clock);

  ags_frame_clock_set_note_offset(pulse_devin->frame_clock,
				  pulse_devin->start_note_offset);

  ags_atomic_int_and(&(pulse_devin->sync_flags),
		   (~(AGS_PULSE_DEVIN_PASS_THROUGH)));
  ags_atomic_int_or(&(pulse_devin->sync_flags),
		  AGS_PULSE_DEVIN_INITIAL_CALLBACK);

  g_rec_mutex_unlock(pulse_devin_mutex);
}

void
ags_pulse_devin_port_record(AgsSoundcard *soundcard,
			    GError **error)
{
  AgsPulseClient *pulse_client;
  AgsPulseDevin *pulse_devin;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;
      
  GList *task;

  guint word_size;
  gboolean pulse_client_activated;

  GRecMutex *pulse_devin_mutex;
  GRecMutex *pulse_client_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;
  
  pulse_devin = AGS_PULSE_DEVIN(soundcard);

  application_context = ags_application_context_get_instance();
  
  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* client */
  g_rec_mutex_lock(pulse_devin_mutex);

  pulse_client = (AgsPulseClient *) pulse_devin->pulse_client;
  
  callback_mutex = &(pulse_devin->callback_mutex);
  callback_finish_mutex = &(pulse_devin->callback_finish_mutex);

  /* do playback */  
  pulse_devin->flags &= (~AGS_PULSE_DEVIN_START_RECORD);
  
  if((AGS_PULSE_DEVIN_INITIALIZED & (pulse_devin->flags)) == 0){
    g_rec_mutex_unlock(pulse_devin_mutex);
    
    return;
  }

  switch(pulse_devin->format){
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
    g_rec_mutex_unlock(pulse_devin_mutex);
    
    g_warning("ags_pulse_devin_port_record(): unsupported word size");
    
    return;
  }

  g_rec_mutex_unlock(pulse_devin_mutex);

  /* get client mutex */
  pulse_client_mutex = AGS_PULSE_CLIENT_GET_OBJ_MUTEX(pulse_client);

  /* get activated */
  g_rec_mutex_lock(pulse_client_mutex);

  pulse_client_activated = ((AGS_PULSE_CLIENT_ACTIVATED & (pulse_client->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(pulse_client_mutex);

  if(pulse_client_activated){
    /* signal */
    if((AGS_PULSE_DEVIN_INITIAL_CALLBACK & (ags_atomic_int_get(&(pulse_devin->sync_flags)))) == 0){
      g_mutex_lock(callback_mutex);

      ags_atomic_int_or(&(pulse_devin->sync_flags),
		      AGS_PULSE_DEVIN_CALLBACK_DONE);
    
      if((AGS_PULSE_DEVIN_CALLBACK_WAIT & (ags_atomic_int_get(&(pulse_devin->sync_flags)))) != 0){
	g_cond_signal(&(pulse_devin->callback_cond));
      }

      g_mutex_unlock(callback_mutex);
    }
    
    /* wait callback */	
    if((AGS_PULSE_DEVIN_INITIAL_CALLBACK & (ags_atomic_int_get(&(pulse_devin->sync_flags)))) == 0){
      g_mutex_lock(callback_finish_mutex);
    
      if((AGS_PULSE_DEVIN_CALLBACK_FINISH_DONE & (ags_atomic_int_get(&(pulse_devin->sync_flags)))) == 0){
	ags_atomic_int_or(&(pulse_devin->sync_flags),
			AGS_PULSE_DEVIN_CALLBACK_FINISH_WAIT);
    
	while((AGS_PULSE_DEVIN_CALLBACK_FINISH_DONE & (ags_atomic_int_get(&(pulse_devin->sync_flags)))) == 0 &&
	      (AGS_PULSE_DEVIN_CALLBACK_FINISH_WAIT & (ags_atomic_int_get(&(pulse_devin->sync_flags)))) != 0){
	  g_cond_wait(&(pulse_devin->callback_finish_cond),
		      callback_finish_mutex);
	}
      }
    
      ags_atomic_int_and(&(pulse_devin->sync_flags),
		       (~(AGS_PULSE_DEVIN_CALLBACK_FINISH_WAIT |
			  AGS_PULSE_DEVIN_CALLBACK_FINISH_DONE)));
    
      g_mutex_unlock(callback_finish_mutex);
    }else{
      ags_atomic_int_and(&(pulse_devin->sync_flags),
		       (~AGS_PULSE_DEVIN_INITIAL_CALLBACK));
    }
  }

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;
  
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) pulse_devin);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) pulse_devin);
  task = g_list_append(task,
		       clear_buffer);
    
  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) pulse_devin);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_pulse_devin_port_free(AgsSoundcard *soundcard)
{
  AgsPulsePort *pulse_port;
  AgsPulseDevin *pulse_devin;

  guint word_size;

  guint i;
  
  GRecMutex *pulse_devin_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  pulse_devin = AGS_PULSE_DEVIN(soundcard);
  
  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /*  */
  g_rec_mutex_lock(pulse_devin_mutex);

  if((AGS_PULSE_DEVIN_INITIALIZED & (pulse_devin->flags)) == 0){
    g_rec_mutex_unlock(pulse_devin_mutex);

    return;
  }

  callback_mutex = &(pulse_devin->callback_mutex);
  callback_finish_mutex = &(pulse_devin->callback_finish_mutex);
  
  //  ags_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);
  
  pulse_devin->app_buffer_mode = AGS_PULSE_DEVIN_APP_BUFFER_0;
  pulse_devin->flags &= (~(AGS_PULSE_DEVIN_RECORD));

  ags_atomic_int_or(&(pulse_devin->sync_flags),
		  AGS_PULSE_DEVIN_PASS_THROUGH);
  ags_atomic_int_and(&(pulse_devin->sync_flags),
		   (~AGS_PULSE_DEVIN_INITIAL_CALLBACK));

  /* signal callback */
  g_mutex_lock(callback_mutex);

  ags_atomic_int_or(&(pulse_devin->sync_flags),
		  AGS_PULSE_DEVIN_CALLBACK_DONE);
    
  if((AGS_PULSE_DEVIN_CALLBACK_WAIT & (ags_atomic_int_get(&(pulse_devin->sync_flags)))) != 0){
    g_cond_signal(&(pulse_devin->callback_cond));
  }

  g_mutex_unlock(callback_mutex);

  /* signal thread */
  g_mutex_lock(callback_finish_mutex);

  ags_atomic_int_or(&(pulse_devin->sync_flags),
		  AGS_PULSE_DEVIN_CALLBACK_FINISH_DONE);
    
  if((AGS_PULSE_DEVIN_CALLBACK_FINISH_WAIT & (ags_atomic_int_get(&(pulse_devin->sync_flags)))) != 0){
    g_cond_signal(&(pulse_devin->callback_finish_cond));
  }

  g_mutex_unlock(callback_finish_mutex);
  
  /* stop */  
  ags_frame_clock_stop(pulse_devin->frame_clock);

  switch(pulse_devin->format){
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
    
    g_critical("ags_pulse_devin_free(): unsupported word size");
  }

  g_rec_mutex_unlock(pulse_devin_mutex);

  if(pulse_devin->pulse_port != NULL){
    pulse_port = pulse_devin->pulse_port->data;

    while(!ags_atomic_int_get(&(pulse_port->is_empty))) usleep(500000);
  }

  g_rec_mutex_lock(pulse_devin_mutex);
  
  memset(pulse_devin->app_buffer[0], 0, (size_t) pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);
  memset(pulse_devin->app_buffer[1], 0, (size_t) pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);
  memset(pulse_devin->app_buffer[2], 0, (size_t) pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);
  memset(pulse_devin->app_buffer[3], 0, (size_t) pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);
  memset(pulse_devin->app_buffer[4], 0, (size_t) pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);
  memset(pulse_devin->app_buffer[5], 0, (size_t) pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);
  memset(pulse_devin->app_buffer[6], 0, (size_t) pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);
  memset(pulse_devin->app_buffer[7], 0, (size_t) pulse_devin->pcm_channels * pulse_devin->buffer_size * word_size);

  g_rec_mutex_unlock(pulse_devin_mutex);
}

void
ags_pulse_devin_tic(AgsSoundcard *soundcard)
{
  AgsPulseDevin *pulse_devin;

  GRecMutex *pulse_devin_mutex;
  
  pulse_devin = AGS_PULSE_DEVIN(soundcard);
  
  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* frame clock */
  g_rec_mutex_lock(pulse_devin_mutex);
  
  ags_frame_clock_increment_counter(pulse_devin->frame_clock);

  /* 16th pulse */
  if(ags_frame_clock_get_has_16th_pulse(pulse_devin->frame_clock)){
    ags_soundcard_offset_changed(soundcard,
				 pulse_devin->frame_clock->note_offset);
  }
  
  g_rec_mutex_unlock(pulse_devin_mutex);
}

void
ags_pulse_devin_offset_changed(AgsSoundcard *soundcard,
			       guint64 note_offset)
{
  //empty
}

void
ags_pulse_devin_set_bpm(AgsSoundcard *soundcard,
			gdouble bpm)
{
  AgsPulseDevin *pulse_devin;

  GRecMutex *pulse_devin_mutex;
  
  pulse_devin = AGS_PULSE_DEVIN(soundcard);

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* set bpm */
  g_rec_mutex_lock(pulse_devin_mutex);

  pulse_devin->bpm = bpm;

  ags_frame_clock_set_bpm(pulse_devin->frame_clock,
			  bpm);

  g_rec_mutex_unlock(pulse_devin_mutex);
}

gdouble
ags_pulse_devin_get_bpm(AgsSoundcard *soundcard)
{
  AgsPulseDevin *pulse_devin;

  gdouble bpm;
  
  GRecMutex *pulse_devin_mutex;
  
  pulse_devin = AGS_PULSE_DEVIN(soundcard);

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* get bpm */
  g_rec_mutex_lock(pulse_devin_mutex);

  bpm = pulse_devin->bpm;
  
  g_rec_mutex_unlock(pulse_devin_mutex);

  return(bpm);
}

void
ags_pulse_devin_set_start_note_offset(AgsSoundcard *soundcard,
				      guint64 start_note_offset)
{
  AgsPulseDevin *pulse_devin;

  GRecMutex *pulse_devin_mutex;  

  pulse_devin = AGS_PULSE_DEVIN(soundcard);

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* set note offset */
  g_rec_mutex_lock(pulse_devin_mutex);

  pulse_devin->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(pulse_devin_mutex);
}

guint64
ags_pulse_devin_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsPulseDevin *pulse_devin;

  guint64 start_note_offset;
  
  GRecMutex *pulse_devin_mutex;  

  pulse_devin = AGS_PULSE_DEVIN(soundcard);

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* set note offset */
  g_rec_mutex_lock(pulse_devin_mutex);

  start_note_offset = pulse_devin->start_note_offset;

  g_rec_mutex_unlock(pulse_devin_mutex);

  return(start_note_offset);
}

GObject*
ags_pulse_devin_get_frame_clock(AgsSoundcard *soundcard)
{
  AgsPulseDevin *pulse_devin;

  GObject *frame_clock;
  
  GRecMutex *pulse_devin_mutex;
  
  pulse_devin = AGS_PULSE_DEVIN(soundcard);

  /* get core audio devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* get frame clock */
  g_rec_mutex_lock(pulse_devin_mutex);

  frame_clock = (GObject *) pulse_devin->frame_clock;
  
  g_rec_mutex_unlock(pulse_devin_mutex);

  return(frame_clock);
}

void*
ags_pulse_devin_get_buffer(AgsSoundcard *soundcard)
{
  AgsPulseDevin *pulse_devin;

  void *buffer;

  GRecMutex *pulse_devin_mutex;  
  
  pulse_devin = AGS_PULSE_DEVIN(soundcard);
  
  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  g_rec_mutex_lock(pulse_devin_mutex);

  if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_0){
    buffer = pulse_devin->app_buffer[0];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_1){
    buffer = pulse_devin->app_buffer[1];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_2){
    buffer = pulse_devin->app_buffer[2];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_3){
    buffer = pulse_devin->app_buffer[3];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_4){
    buffer = pulse_devin->app_buffer[4];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_5){
    buffer = pulse_devin->app_buffer[5];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_6){
    buffer = pulse_devin->app_buffer[6];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_7){
    buffer = pulse_devin->app_buffer[7];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(pulse_devin_mutex);

  return(buffer);
}

void*
ags_pulse_devin_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsPulseDevin *pulse_devin;

  void *buffer;

  GRecMutex *pulse_devin_mutex;  
  
  pulse_devin = AGS_PULSE_DEVIN(soundcard);
  
  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  g_rec_mutex_lock(pulse_devin_mutex);

  if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_0){
    buffer = pulse_devin->app_buffer[1];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_1){
    buffer = pulse_devin->app_buffer[2];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_2){
    buffer = pulse_devin->app_buffer[3];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_3){
    buffer = pulse_devin->app_buffer[4];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_4){
    buffer = pulse_devin->app_buffer[5];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_5){
    buffer = pulse_devin->app_buffer[6];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_6){
    buffer = pulse_devin->app_buffer[7];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_7){
    buffer = pulse_devin->app_buffer[0];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(pulse_devin_mutex);

  return(buffer);
}

void*
ags_pulse_devin_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsPulseDevin *pulse_devin;

  void *buffer;

  GRecMutex *pulse_devin_mutex;  
  
  pulse_devin = AGS_PULSE_DEVIN(soundcard);

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  g_rec_mutex_lock(pulse_devin_mutex);

  if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_0){
    buffer = pulse_devin->app_buffer[7];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_1){
    buffer = pulse_devin->app_buffer[0];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_2){
    buffer = pulse_devin->app_buffer[1];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_3){
    buffer = pulse_devin->app_buffer[2];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_4){
    buffer = pulse_devin->app_buffer[3];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_5){
    buffer = pulse_devin->app_buffer[4];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_6){
    buffer = pulse_devin->app_buffer[5];
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_7){
    buffer = pulse_devin->app_buffer[6];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(pulse_devin_mutex);

  return(buffer);
}

/**
 * ags_pulse_devin_switch_buffer_flag:
 * @pulse_devin: an #AgsPulseDevin
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.0.0
 */
void
ags_pulse_devin_switch_buffer_flag(AgsPulseDevin *pulse_devin)
{
  GRecMutex *pulse_devin_mutex;
  
  if(!AGS_IS_PULSE_DEVIN(pulse_devin)){
    return;
  }

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* switch buffer flag */
  g_rec_mutex_lock(pulse_devin_mutex);

  if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_0){
    pulse_devin->app_buffer_mode = AGS_PULSE_DEVIN_APP_BUFFER_1;
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_1){
    pulse_devin->app_buffer_mode = AGS_PULSE_DEVIN_APP_BUFFER_2;
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_2){
    pulse_devin->app_buffer_mode = AGS_PULSE_DEVIN_APP_BUFFER_3;
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_3){
    pulse_devin->app_buffer_mode = AGS_PULSE_DEVIN_APP_BUFFER_4;
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_4){
    pulse_devin->app_buffer_mode = AGS_PULSE_DEVIN_APP_BUFFER_5;
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_5){
    pulse_devin->app_buffer_mode = AGS_PULSE_DEVIN_APP_BUFFER_6;
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_6){
    pulse_devin->app_buffer_mode = AGS_PULSE_DEVIN_APP_BUFFER_7;
  }else if(pulse_devin->app_buffer_mode == AGS_PULSE_DEVIN_APP_BUFFER_7){
    pulse_devin->app_buffer_mode = AGS_PULSE_DEVIN_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(pulse_devin_mutex);
}

/**
 * ags_pulse_devin_realloc_buffer:
 * @pulse_devin: the #AgsPulseDevin
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.0.0
 */
void
ags_pulse_devin_realloc_buffer(AgsPulseDevin *pulse_devin)
{
  guint pcm_channels;
  guint buffer_size;
  AgsSoundcardFormat format;
  guint word_size;

  GRecMutex *pulse_devin_mutex;  

  if(!AGS_IS_PULSE_DEVIN(pulse_devin)){
    return;
  }

  /* get pulse devin mutex */
  pulse_devin_mutex = AGS_PULSE_DEVIN_GET_OBJ_MUTEX(pulse_devin);

  /* get word size */  
  g_rec_mutex_lock(pulse_devin_mutex);

  pcm_channels = pulse_devin->pcm_channels;
  buffer_size = pulse_devin->buffer_size;

  format = pulse_devin->format;
  
  g_rec_mutex_unlock(pulse_devin_mutex);

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
    g_warning("ags_pulse_devin_realloc_buffer(): unsupported word size");
    return;
  }
  
  /* AGS_PULSE_DEVIN_APP_BUFFER_0 */
  if(pulse_devin->app_buffer[0] != NULL){
    g_free(pulse_devin->app_buffer[0]);
  }
  
  pulse_devin->app_buffer[0] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_PULSE_DEVIN_APP_BUFFER_1 */
  if(pulse_devin->app_buffer[1] != NULL){
    g_free(pulse_devin->app_buffer[1]);
  }

  pulse_devin->app_buffer[1] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_PULSE_DEVIN_APP_BUFFER_2 */
  if(pulse_devin->app_buffer[2] != NULL){
    g_free(pulse_devin->app_buffer[2]);
  }

  pulse_devin->app_buffer[2] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_PULSE_DEVIN_APP_BUFFER_3 */
  if(pulse_devin->app_buffer[3] != NULL){
    g_free(pulse_devin->app_buffer[3]);
  }
  
  pulse_devin->app_buffer[3] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  /* AGS_PULSE_DEVIN_APP_BUFFER_4 */
  if(pulse_devin->app_buffer[4] != NULL){
    g_free(pulse_devin->app_buffer[4]);
  }
  
  pulse_devin->app_buffer[4] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  /* AGS_PULSE_DEVIN_APP_BUFFER_5 */
  if(pulse_devin->app_buffer[5] != NULL){
    g_free(pulse_devin->app_buffer[5]);
  }
  
  pulse_devin->app_buffer[5] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  /* AGS_PULSE_DEVIN_APP_BUFFER_6 */
  if(pulse_devin->app_buffer[6] != NULL){
    g_free(pulse_devin->app_buffer[6]);
  }
  
  pulse_devin->app_buffer[6] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  /* AGS_PULSE_DEVIN_APP_BUFFER_7 */
  if(pulse_devin->app_buffer[7] != NULL){
    g_free(pulse_devin->app_buffer[7]);
  }
  
  pulse_devin->app_buffer[7] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_pulse_devin_new:
 *
 * Creates a new instance of #AgsPulseDevin.
 *
 * Returns: the new #AgsPulseDevin
 *
 * Since: 3.0.0
 */
AgsPulseDevin*
ags_pulse_devin_new()
{
  AgsPulseDevin *pulse_devin;

  pulse_devin = (AgsPulseDevin *) g_object_new(AGS_TYPE_PULSE_DEVIN,
					       NULL);
  
  return(pulse_devin);
}
