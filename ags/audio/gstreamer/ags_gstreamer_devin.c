/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/audio/gstreamer/ags_gstreamer_devin.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_soundcard_util.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/gstreamer/ags_gstreamer_server.h>
#include <ags/audio/gstreamer/ags_gstreamer_client.h>
#include <ags/audio/gstreamer/ags_gstreamer_port.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <string.h>
#include <math.h>
#include <time.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_gstreamer_devin_class_init(AgsGstreamerDevinClass *gstreamer_devin);
void ags_gstreamer_devin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_gstreamer_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_gstreamer_devin_init(AgsGstreamerDevin *gstreamer_devin);
void ags_gstreamer_devin_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_gstreamer_devin_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_gstreamer_devin_dispose(GObject *gobject);
void ags_gstreamer_devin_finalize(GObject *gobject);

AgsUUID* ags_gstreamer_devin_get_uuid(AgsConnectable *connectable);
gboolean ags_gstreamer_devin_has_resource(AgsConnectable *connectable);
gboolean ags_gstreamer_devin_is_ready(AgsConnectable *connectable);
void ags_gstreamer_devin_add_to_registry(AgsConnectable *connectable);
void ags_gstreamer_devin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_gstreamer_devin_list_resource(AgsConnectable *connectable);
xmlNode* ags_gstreamer_devin_xml_compose(AgsConnectable *connectable);
void ags_gstreamer_devin_xml_parse(AgsConnectable *connectable,
				   xmlNode *node);
gboolean ags_gstreamer_devin_is_connected(AgsConnectable *connectable);
void ags_gstreamer_devin_connect(AgsConnectable *connectable);
void ags_gstreamer_devin_disconnect(AgsConnectable *connectable);

void ags_gstreamer_devin_set_device(AgsSoundcard *soundcard,
				    gchar *device);
gchar* ags_gstreamer_devin_get_device(AgsSoundcard *soundcard);

void ags_gstreamer_devin_set_presets(AgsSoundcard *soundcard,
				     guint channels,
				     guint rate,
				     guint buffer_size,
				     guint format);
void ags_gstreamer_devin_get_presets(AgsSoundcard *soundcard,
				     guint *channels,
				     guint *rate,
				     guint *buffer_size,
				     guint *format);

void ags_gstreamer_devin_list_cards(AgsSoundcard *soundcard,
				    GList **card_id, GList **card_name);
void ags_gstreamer_devin_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
				  guint *channels_min, guint *channels_max,
				  guint *rate_min, guint *rate_max,
				  guint *buffer_size_min, guint *buffer_size_max,
				  GError **error);
guint ags_gstreamer_devin_get_capability(AgsSoundcard *soundcard);

gboolean ags_gstreamer_devin_is_starting(AgsSoundcard *soundcard);
gboolean ags_gstreamer_devin_is_recording(AgsSoundcard *soundcard);

gchar* ags_gstreamer_devin_get_uptime(AgsSoundcard *soundcard);

void ags_gstreamer_devin_port_init(AgsSoundcard *soundcard,
				   GError **error);
void ags_gstreamer_devin_port_record(AgsSoundcard *soundcard,
				     GError **error);
void ags_gstreamer_devin_port_free(AgsSoundcard *soundcard);

void ags_gstreamer_devin_tic(AgsSoundcard *soundcard);
void ags_gstreamer_devin_offset_changed(AgsSoundcard *soundcard,
					guint note_offset);

void ags_gstreamer_devin_set_bpm(AgsSoundcard *soundcard,
				 gdouble bpm);
gdouble ags_gstreamer_devin_get_bpm(AgsSoundcard *soundcard);

void ags_gstreamer_devin_set_delay_factor(AgsSoundcard *soundcard,
					  gdouble delay_factor);
gdouble ags_gstreamer_devin_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_gstreamer_devin_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_gstreamer_devin_get_delay(AgsSoundcard *soundcard);
guint ags_gstreamer_devin_get_attack(AgsSoundcard *soundcard);

void* ags_gstreamer_devin_get_buffer(AgsSoundcard *soundcard);
void* ags_gstreamer_devin_get_next_buffer(AgsSoundcard *soundcard);
void* ags_gstreamer_devin_get_prev_buffer(AgsSoundcard *soundcard);

void ags_gstreamer_devin_lock_buffer(AgsSoundcard *soundcard,
				     void *buffer);
void ags_gstreamer_devin_unlock_buffer(AgsSoundcard *soundcard,
				       void *buffer);

guint ags_gstreamer_devin_get_delay_counter(AgsSoundcard *soundcard);

void ags_gstreamer_devin_set_start_note_offset(AgsSoundcard *soundcard,
					       guint start_note_offset);
guint ags_gstreamer_devin_get_start_note_offset(AgsSoundcard *soundcard);

void ags_gstreamer_devin_set_note_offset(AgsSoundcard *soundcard,
					 guint note_offset);
guint ags_gstreamer_devin_get_note_offset(AgsSoundcard *soundcard);

void ags_gstreamer_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
						  guint note_offset);
guint ags_gstreamer_devin_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_gstreamer_devin_set_loop(AgsSoundcard *soundcard,
				  guint loop_left, guint loop_right,
				  gboolean do_loop);
void ags_gstreamer_devin_get_loop(AgsSoundcard *soundcard,
				  guint *loop_left, guint *loop_right,
				  gboolean *do_loop);

guint ags_gstreamer_devin_get_loop_offset(AgsSoundcard *soundcard);

/**
 * SECTION:ags_gstreamer_devin
 * @short_description: Input from soundcard
 * @title: AgsGstreamerDevin
 * @section_id:
 * @include: ags/audio/gstreamer/ags_gstreamer_devin.h
 *
 * #AgsGstreamerDevin represents a soundcard and supports input.
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
  PROP_GSTREAMER_CLIENT,
  PROP_GSTREAMER_PORT,
  PROP_CHANNEL,
};

static gpointer ags_gstreamer_devin_parent_class = NULL;

GType
ags_gstreamer_devin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_gstreamer_devin = 0;

    static const GTypeInfo ags_gstreamer_devin_info = {
      sizeof (AgsGstreamerDevinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_gstreamer_devin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGstreamerDevin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_gstreamer_devin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_gstreamer_devin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_gstreamer_devin_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_gstreamer_devin = g_type_register_static(G_TYPE_OBJECT,
						      "AgsGstreamerDevin",
						      &ags_gstreamer_devin_info,
						      0);

    g_type_add_interface_static(ags_type_gstreamer_devin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_gstreamer_devin,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_gstreamer_devin);
  }

  return g_define_type_id__volatile;
}

GType
ags_gstreamer_devin_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_GSTREAMER_DEVIN_INITIALIZED, "AGS_GSTREAMER_DEVIN_INITIALIZED", "gstreamer-devin-initialized" },
      { AGS_GSTREAMER_DEVIN_START_RECORD, "AGS_GSTREAMER_DEVIN_START_RECORD", "gstreamer-devin-start-record" },
      { AGS_GSTREAMER_DEVIN_RECORD, "AGS_GSTREAMER_DEVIN_RECORD", "gstreamer-devin-record" },
      { AGS_GSTREAMER_DEVIN_NONBLOCKING, "AGS_GSTREAMER_DEVIN_NONBLOCKING", "gstreamer-devin-nonblocking" },
      { AGS_GSTREAMER_DEVIN_ATTACK_FIRST, "AGS_GSTREAMER_DEVIN_ATTACK_FIRST", "gstreamer-devin-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsGstreamerDevinFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

void
ags_gstreamer_devin_class_init(AgsGstreamerDevinClass *gstreamer_devin)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_gstreamer_devin_parent_class = g_type_class_peek_parent(gstreamer_devin);

  /* GObjectClass */
  gobject = (GObjectClass *) gstreamer_devin;

  gobject->set_property = ags_gstreamer_devin_set_property;
  gobject->get_property = ags_gstreamer_devin_get_property;

  gobject->dispose = ags_gstreamer_devin_dispose;
  gobject->finalize = ags_gstreamer_devin_finalize;

  /* properties */
  /**
   * AgsGstreamerDevin:device:
   *
   * The gstreamer soundcard indentifier
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-gstreamer-devin-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsGstreamerDevin:dsp-channels:
   *
   * The dsp channel count
   * 
   * Since: 3.6.0
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
   * AgsGstreamerDevin:pcm-channels:
   *
   * The pcm channel count
   * 
   * Since: 3.6.0
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
   * AgsGstreamerDevin:format:
   *
   * The precision of the buffer
   * 
   * Since: 3.6.0
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
   * AgsGstreamerDevin:buffer-size:
   *
   * The buffer size
   * 
   * Since: 3.6.0
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
   * AgsGstreamerDevin:samplerate:
   *
   * The samplerate
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("frames per second"),
				 i18n_pspec("The frames count recorded during a second"),
				 8000,
				 96000,
				 44100,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsGstreamerDevin:buffer:
   *
   * The buffer
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to record"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsGstreamerDevin:bpm:
   *
   * Beats per minute
   * 
   * Since: 3.6.0
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
   * AgsGstreamerDevin:delay-factor:
   *
   * tact
   * 
   * Since: 3.6.0
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
   * AgsGstreamerDevin:attack:
   *
   * Attack of the buffer
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_pointer("attack",
				    i18n_pspec("attack of buffer"),
				    i18n_pspec("The attack to use for the buffer"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  
  /**
   * AgsGstreamerDevin:gstreamer-client:
   *
   * The assigned #AgsGstreamerClient
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_object("gstreamer-client",
				   i18n_pspec("gstreamer client object"),
				   i18n_pspec("The gstreamer client object"),
				   AGS_TYPE_GSTREAMER_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GSTREAMER_CLIENT,
				  param_spec);

  /**
   * AgsGstreamerDevin:gstreamer-port:
   *
   * The assigned #AgsGstreamerPort
   * 
   * Since: 3.6.0
   */
  param_spec = g_param_spec_pointer("gstreamer-port",
				    i18n_pspec("gstreamer port object"),
				    i18n_pspec("The gstreamer port object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_GSTREAMER_PORT,
				  param_spec);
}

GQuark
ags_gstreamer_devin_error_quark()
{
  return(g_quark_from_static_string("ags-gstreamer_devin-error-quark"));
}

void
ags_gstreamer_devin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_gstreamer_devin_get_uuid;
  connectable->has_resource = ags_gstreamer_devin_has_resource;

  connectable->is_ready = ags_gstreamer_devin_is_ready;
  connectable->add_to_registry = ags_gstreamer_devin_add_to_registry;
  connectable->remove_from_registry = ags_gstreamer_devin_remove_from_registry;

  connectable->list_resource = ags_gstreamer_devin_list_resource;
  connectable->xml_compose = ags_gstreamer_devin_xml_compose;
  connectable->xml_parse = ags_gstreamer_devin_xml_parse;

  connectable->is_connected = ags_gstreamer_devin_is_connected;  
  connectable->connect = ags_gstreamer_devin_connect;
  connectable->disconnect = ags_gstreamer_devin_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_gstreamer_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_gstreamer_devin_set_device;
  soundcard->get_device = ags_gstreamer_devin_get_device;
  
  soundcard->set_presets = ags_gstreamer_devin_set_presets;
  soundcard->get_presets = ags_gstreamer_devin_get_presets;

  soundcard->list_cards = ags_gstreamer_devin_list_cards;
  soundcard->pcm_info = ags_gstreamer_devin_pcm_info;
  soundcard->get_capability = ags_gstreamer_devin_get_capability;

  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_gstreamer_devin_is_starting;
  soundcard->is_playing = NULL;
  soundcard->is_recording = ags_gstreamer_devin_is_recording;

  soundcard->get_uptime = ags_gstreamer_devin_get_uptime;
  
  soundcard->play_init = NULL;
  soundcard->play = NULL;

  soundcard->record_init = ags_gstreamer_devin_port_init;
  soundcard->record = ags_gstreamer_devin_port_record;

  soundcard->stop = ags_gstreamer_devin_port_free;

  soundcard->tic = ags_gstreamer_devin_tic;
  soundcard->offset_changed = ags_gstreamer_devin_offset_changed;
    
  soundcard->set_bpm = ags_gstreamer_devin_set_bpm;
  soundcard->get_bpm = ags_gstreamer_devin_get_bpm;

  soundcard->set_delay_factor = ags_gstreamer_devin_set_delay_factor;
  soundcard->get_delay_factor = ags_gstreamer_devin_get_delay_factor;
  
  soundcard->get_absolute_delay = ags_gstreamer_devin_get_absolute_delay;

  soundcard->get_delay = ags_gstreamer_devin_get_delay;
  soundcard->get_attack = ags_gstreamer_devin_get_attack;

  soundcard->get_buffer = ags_gstreamer_devin_get_buffer;
  soundcard->get_next_buffer = ags_gstreamer_devin_get_next_buffer;
  soundcard->get_prev_buffer = ags_gstreamer_devin_get_prev_buffer;

  soundcard->lock_buffer = ags_gstreamer_devin_lock_buffer;
  soundcard->unlock_buffer = ags_gstreamer_devin_unlock_buffer;

  soundcard->get_delay_counter = ags_gstreamer_devin_get_delay_counter;

  soundcard->set_start_note_offset = ags_gstreamer_devin_set_start_note_offset;
  soundcard->get_start_note_offset = ags_gstreamer_devin_get_start_note_offset;

  soundcard->set_note_offset = ags_gstreamer_devin_set_note_offset;
  soundcard->get_note_offset = ags_gstreamer_devin_get_note_offset;

  soundcard->set_note_offset_absolute = ags_gstreamer_devin_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_gstreamer_devin_get_note_offset_absolute;

  soundcard->set_loop = ags_gstreamer_devin_set_loop;
  soundcard->get_loop = ags_gstreamer_devin_get_loop;

  soundcard->get_loop_offset = ags_gstreamer_devin_get_loop_offset;
}

void
ags_gstreamer_devin_init(AgsGstreamerDevin *gstreamer_devin)
{
  AgsConfig *config;

  gchar *str;
  gchar *segmentation;

  guint denominator, numerator;
  guint i;
  
  /* flags */
  gstreamer_devin->flags = 0;
  gstreamer_devin->connectable_flags = 0;
  g_atomic_int_set(&(gstreamer_devin->sync_flags),
		   AGS_GSTREAMER_DEVIN_PASS_THROUGH);

  /* gstreamer devin mutex */
  g_rec_mutex_init(&(gstreamer_devin->obj_mutex));

  /* uuid */
  gstreamer_devin->uuid = ags_uuid_alloc();
  ags_uuid_generate(gstreamer_devin->uuid);

  /* presets */
  config = ags_config_get_instance();

  gstreamer_devin->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  gstreamer_devin->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  gstreamer_devin->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  gstreamer_devin->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  gstreamer_devin->format = ags_soundcard_helper_config_get_format(config);

  /*  */
  gstreamer_devin->card_uri = NULL;
  gstreamer_devin->gstreamer_client = NULL;

  gstreamer_devin->port_name = NULL;
  gstreamer_devin->gstreamer_port = NULL;

  /* buffer */
  gstreamer_devin->app_buffer_mode = AGS_GSTREAMER_DEVIN_APP_BUFFER_0;

  gstreamer_devin->app_buffer_mutex = (GRecMutex **) g_malloc(8 * sizeof(GRecMutex *));

  for(i = 0; i < 8; i++){
    gstreamer_devin->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(gstreamer_devin->app_buffer_mutex[i]);
  }

  gstreamer_devin->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  gstreamer_devin->sub_block_mutex = (GRecMutex **) g_malloc(8 * gstreamer_devin->sub_block_count * gstreamer_devin->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < 8 * gstreamer_devin->sub_block_count * gstreamer_devin->pcm_channels; i++){
    gstreamer_devin->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(gstreamer_devin->sub_block_mutex[i]);
  }

  gstreamer_devin->app_buffer = (void **) g_malloc(8 * sizeof(void*));

  gstreamer_devin->app_buffer[0] = NULL;
  gstreamer_devin->app_buffer[1] = NULL;
  gstreamer_devin->app_buffer[2] = NULL;
  gstreamer_devin->app_buffer[3] = NULL;
  gstreamer_devin->app_buffer[4] = NULL;
  gstreamer_devin->app_buffer[5] = NULL;
  gstreamer_devin->app_buffer[6] = NULL;
  gstreamer_devin->app_buffer[7] = NULL;
  
  ags_gstreamer_devin_realloc_buffer(gstreamer_devin);
  
  /* bpm */
  gstreamer_devin->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  gstreamer_devin->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    gstreamer_devin->delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }

  /* delay and attack */
  gstreamer_devin->delay = (gdouble *) g_malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
						sizeof(gdouble));
  
  gstreamer_devin->attack = (guint *) g_malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					       sizeof(guint));

  ags_gstreamer_devin_adjust_delay_and_attack(gstreamer_devin);
  
  /* counters */
  gstreamer_devin->tact_counter = 0.0;
  gstreamer_devin->delay_counter = 0.0;
  gstreamer_devin->tic_counter = 0;

  gstreamer_devin->start_note_offset = 0;
  gstreamer_devin->note_offset = 0;
  gstreamer_devin->note_offset_absolute = 0;

  gstreamer_devin->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  gstreamer_devin->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  gstreamer_devin->do_loop = FALSE;

  gstreamer_devin->loop_offset = 0;

  /* callback mutex */
  g_mutex_init(&(gstreamer_devin->callback_mutex));

  g_cond_init(&(gstreamer_devin->callback_cond));

  /* callback finish mutex */
  g_mutex_init(&(gstreamer_devin->callback_finish_mutex));

  g_cond_init(&(gstreamer_devin->callback_finish_cond));
}

void
ags_gstreamer_devin_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsGstreamerDevin *gstreamer_devin;

  GRecMutex *gstreamer_devin_mutex;

  gstreamer_devin = AGS_GSTREAMER_DEVIN(gobject);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      g_rec_mutex_lock(gstreamer_devin_mutex);

      gstreamer_devin->card_uri = g_strdup(device);

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      g_rec_mutex_lock(gstreamer_devin_mutex);

      if(dsp_channels == gstreamer_devin->dsp_channels){
	g_rec_mutex_unlock(gstreamer_devin_mutex);

	return;
      }

      gstreamer_devin->dsp_channels = dsp_channels;

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      g_rec_mutex_lock(gstreamer_devin_mutex);

      if(pcm_channels == gstreamer_devin->pcm_channels){
	g_rec_mutex_unlock(gstreamer_devin_mutex);

	return;
      }

      gstreamer_devin->pcm_channels = pcm_channels;

      g_rec_mutex_unlock(gstreamer_devin_mutex);

      ags_gstreamer_devin_realloc_buffer(gstreamer_devin);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(gstreamer_devin_mutex);

      if(format == gstreamer_devin->format){
	g_rec_mutex_unlock(gstreamer_devin_mutex);

	return;
      }

      gstreamer_devin->format = format;

      g_rec_mutex_unlock(gstreamer_devin_mutex);

      ags_gstreamer_devin_realloc_buffer(gstreamer_devin);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      g_rec_mutex_lock(gstreamer_devin_mutex);

      if(buffer_size == gstreamer_devin->buffer_size){
	g_rec_mutex_unlock(gstreamer_devin_mutex);

	return;
      }

      gstreamer_devin->buffer_size = buffer_size;

      g_rec_mutex_unlock(gstreamer_devin_mutex);

      ags_gstreamer_devin_realloc_buffer(gstreamer_devin);
      ags_gstreamer_devin_adjust_delay_and_attack(gstreamer_devin);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      g_rec_mutex_lock(gstreamer_devin_mutex);
      
      if(samplerate == gstreamer_devin->samplerate){
	g_rec_mutex_unlock(gstreamer_devin_mutex);

	return;
      }

      gstreamer_devin->samplerate = samplerate;

      g_rec_mutex_unlock(gstreamer_devin_mutex);

      ags_gstreamer_devin_realloc_buffer(gstreamer_devin);
      ags_gstreamer_devin_adjust_delay_and_attack(gstreamer_devin);
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

      g_rec_mutex_lock(gstreamer_devin_mutex);

      gstreamer_devin->bpm = bpm;

      g_rec_mutex_unlock(gstreamer_devin_mutex);

      ags_gstreamer_devin_adjust_delay_and_attack(gstreamer_devin);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      g_rec_mutex_lock(gstreamer_devin_mutex);

      gstreamer_devin->delay_factor = delay_factor;

      g_rec_mutex_unlock(gstreamer_devin_mutex);

      ags_gstreamer_devin_adjust_delay_and_attack(gstreamer_devin);
    }
    break;
  case PROP_GSTREAMER_CLIENT:
    {
      AgsGstreamerClient *gstreamer_client;

      gstreamer_client = (AgsGstreamerClient *) g_value_get_object(value);

      g_rec_mutex_lock(gstreamer_devin_mutex);

      if(gstreamer_devin->gstreamer_client == (GObject *) gstreamer_client){
	g_rec_mutex_unlock(gstreamer_devin_mutex);

	return;
      }

      if(gstreamer_devin->gstreamer_client != NULL){
	g_object_unref(G_OBJECT(gstreamer_devin->gstreamer_client));
      }

      if(gstreamer_client != NULL){
	g_object_ref(gstreamer_client);
      }
      
      gstreamer_devin->gstreamer_client = (GObject *) gstreamer_client;

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  case PROP_GSTREAMER_PORT:
    {
      AgsGstreamerPort *gstreamer_port;

      gstreamer_port = (AgsGstreamerPort *) g_value_get_pointer(value);

      g_rec_mutex_lock(gstreamer_devin_mutex);

      if(!AGS_IS_GSTREAMER_PORT(gstreamer_port) ||
	 g_list_find(gstreamer_devin->gstreamer_port, gstreamer_port) != NULL){
	g_rec_mutex_unlock(gstreamer_devin_mutex);

	return;
      }

      g_object_ref(gstreamer_port);
      gstreamer_devin->gstreamer_port = g_list_append(gstreamer_devin->gstreamer_port,
						      gstreamer_port);

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_gstreamer_devin_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsGstreamerDevin *gstreamer_devin;

  GRecMutex *gstreamer_devin_mutex;

  gstreamer_devin = AGS_GSTREAMER_DEVIN(gobject);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(gstreamer_devin_mutex);

      g_value_set_string(value, gstreamer_devin->card_uri);

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_rec_mutex_lock(gstreamer_devin_mutex);

      g_value_set_uint(value, gstreamer_devin->dsp_channels);

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_rec_mutex_lock(gstreamer_devin_mutex);

      g_value_set_uint(value, gstreamer_devin->pcm_channels);

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(gstreamer_devin_mutex);

      g_value_set_uint(value, gstreamer_devin->format);

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(gstreamer_devin_mutex);

      g_value_set_uint(value, gstreamer_devin->buffer_size);

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(gstreamer_devin_mutex);

      g_value_set_uint(value, gstreamer_devin->samplerate);

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(gstreamer_devin_mutex);

      g_value_set_pointer(value, gstreamer_devin->app_buffer);

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(gstreamer_devin_mutex);

      g_value_set_double(value, gstreamer_devin->bpm);

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_rec_mutex_lock(gstreamer_devin_mutex);

      g_value_set_double(value, gstreamer_devin->delay_factor);

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  case PROP_ATTACK:
    {
      g_rec_mutex_lock(gstreamer_devin_mutex);

      g_value_set_pointer(value, gstreamer_devin->attack);

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  case PROP_GSTREAMER_CLIENT:
    {
      g_rec_mutex_lock(gstreamer_devin_mutex);

      g_value_set_object(value, gstreamer_devin->gstreamer_client);

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  case PROP_GSTREAMER_PORT:
    {
      g_rec_mutex_lock(gstreamer_devin_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(gstreamer_devin->gstreamer_port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(gstreamer_devin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_gstreamer_devin_dispose(GObject *gobject)
{
  AgsGstreamerDevin *gstreamer_devin;

  gstreamer_devin = AGS_GSTREAMER_DEVIN(gobject);

  ags_uuid_free(gstreamer_devin->uuid);

  /* gstreamer client */
  if(gstreamer_devin->gstreamer_client != NULL){
    g_object_unref(gstreamer_devin->gstreamer_client);

    gstreamer_devin->gstreamer_client = NULL;
  }

  /* gstreamer port */
  g_list_free_full(gstreamer_devin->gstreamer_port,
		   g_object_unref);

  gstreamer_devin->gstreamer_port = NULL;

  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_devin_parent_class)->dispose(gobject);
}

void
ags_gstreamer_devin_finalize(GObject *gobject)
{
  AgsGstreamerDevin *gstreamer_devin;

  guint i;
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(gobject);

  for(i = 0; i < 8; i++){
    g_rec_mutex_clear(gstreamer_devin->app_buffer_mutex[i]);

    g_free(gstreamer_devin->app_buffer_mutex[i]);
  }

  g_free(gstreamer_devin->app_buffer_mutex);

  /* free output buffer */
  g_free(gstreamer_devin->app_buffer[0]);
  g_free(gstreamer_devin->app_buffer[1]);
  g_free(gstreamer_devin->app_buffer[2]);
  g_free(gstreamer_devin->app_buffer[3]);
  g_free(gstreamer_devin->app_buffer[4]);
  g_free(gstreamer_devin->app_buffer[5]);
  g_free(gstreamer_devin->app_buffer[6]);
  g_free(gstreamer_devin->app_buffer[7]);

  /* free buffer array */
  g_free(gstreamer_devin->app_buffer);

  g_free(gstreamer_devin->delay);
  g_free(gstreamer_devin->attack);

  /* gstreamer client */
  if(gstreamer_devin->gstreamer_client != NULL){
    g_object_unref(gstreamer_devin->gstreamer_client);
  }
  
  /* gstreamer port */
  g_list_free_full(gstreamer_devin->gstreamer_port,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_gstreamer_devin_parent_class)->finalize(gobject);
}

AgsUUID*
ags_gstreamer_devin_get_uuid(AgsConnectable *connectable)
{
  AgsGstreamerDevin *gstreamer_devin;
  
  AgsUUID *ptr;

  GRecMutex *gstreamer_devin_mutex;

  gstreamer_devin = AGS_GSTREAMER_DEVIN(connectable);

  /* get gstreamer devin signal mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* get UUID */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  ptr = gstreamer_devin->uuid;

  g_rec_mutex_unlock(gstreamer_devin_mutex);
  
  return(ptr);
}

gboolean
ags_gstreamer_devin_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_gstreamer_devin_is_ready(AgsConnectable *connectable)
{
  AgsGstreamerDevin *gstreamer_devin;
  
  gboolean is_ready;

  GRecMutex *gstreamer_devin_mutex;

  gstreamer_devin = AGS_GSTREAMER_DEVIN(connectable);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* check is ready */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (gstreamer_devin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(gstreamer_devin_mutex);
  
  return(is_ready);
}

void
ags_gstreamer_devin_add_to_registry(AgsConnectable *connectable)
{
  AgsGstreamerDevin *gstreamer_devin;

  GRecMutex *gstreamer_devin_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(connectable);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  g_rec_mutex_lock(gstreamer_devin_mutex);

  gstreamer_devin->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(gstreamer_devin_mutex);
}

void
ags_gstreamer_devin_remove_from_registry(AgsConnectable *connectable)
{
  AgsGstreamerDevin *gstreamer_devin;

  GRecMutex *gstreamer_devin_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  gstreamer_devin = AGS_GSTREAMER_DEVIN(connectable);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  g_rec_mutex_lock(gstreamer_devin_mutex);

  gstreamer_devin->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(gstreamer_devin_mutex);
}

xmlNode*
ags_gstreamer_devin_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_gstreamer_devin_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_gstreamer_devin_xml_parse(AgsConnectable *connectable,
			      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_gstreamer_devin_is_connected(AgsConnectable *connectable)
{
  AgsGstreamerDevin *gstreamer_devin;
  
  gboolean is_connected;

  GRecMutex *gstreamer_devin_mutex;

  gstreamer_devin = AGS_GSTREAMER_DEVIN(connectable);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* check is connected */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (gstreamer_devin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(gstreamer_devin_mutex);
  
  return(is_connected);
}

void
ags_gstreamer_devin_connect(AgsConnectable *connectable)
{
  AgsGstreamerDevin *gstreamer_devin;

  GRecMutex *gstreamer_devin_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  gstreamer_devin = AGS_GSTREAMER_DEVIN(connectable);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  g_rec_mutex_lock(gstreamer_devin_mutex);

  gstreamer_devin->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(gstreamer_devin_mutex);
}

void
ags_gstreamer_devin_disconnect(AgsConnectable *connectable)
{
  AgsGstreamerDevin *gstreamer_devin;

  GRecMutex *gstreamer_devin_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  gstreamer_devin = AGS_GSTREAMER_DEVIN(connectable);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  g_rec_mutex_lock(gstreamer_devin_mutex);

  gstreamer_devin->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(gstreamer_devin_mutex);
}

/**
 * ags_gstreamer_devin_test_flags:
 * @gstreamer_devin: the #AgsGstreamerDevin
 * @flags: the flags
 *
 * Test @flags to be set on @gstreamer_devin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.6.0
 */
gboolean
ags_gstreamer_devin_test_flags(AgsGstreamerDevin *gstreamer_devin, guint flags)
{
  gboolean retval;  
  
  GRecMutex *gstreamer_devin_mutex;

  if(!AGS_IS_GSTREAMER_DEVIN(gstreamer_devin)){
    return(FALSE);
  }

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* test */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  retval = (flags & (gstreamer_devin->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(gstreamer_devin_mutex);

  return(retval);
}

/**
 * ags_gstreamer_devin_set_flags:
 * @gstreamer_devin: the #AgsGstreamerDevin
 * @flags: see #AgsGstreamerDevinFlags-enum
 *
 * Enable a feature of @gstreamer_devin.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_devin_set_flags(AgsGstreamerDevin *gstreamer_devin, guint flags)
{
  GRecMutex *gstreamer_devin_mutex;

  if(!AGS_IS_GSTREAMER_DEVIN(gstreamer_devin)){
    return;
  }

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  gstreamer_devin->flags |= flags;
  
  g_rec_mutex_unlock(gstreamer_devin_mutex);
}
    
/**
 * ags_gstreamer_devin_unset_flags:
 * @gstreamer_devin: the #AgsGstreamerDevin
 * @flags: see #AgsGstreamerDevinFlags-enum
 *
 * Disable a feature of @gstreamer_devin.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_devin_unset_flags(AgsGstreamerDevin *gstreamer_devin, guint flags)
{  
  GRecMutex *gstreamer_devin_mutex;

  if(!AGS_IS_GSTREAMER_DEVIN(gstreamer_devin)){
    return;
  }

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  gstreamer_devin->flags &= (~flags);
  
  g_rec_mutex_unlock(gstreamer_devin_mutex);
}

void
ags_gstreamer_devin_set_device(AgsSoundcard *soundcard,
			       gchar *device)
{
  AgsGstreamerDevin *gstreamer_devin;

  GList *gstreamer_port, *gstreamer_port_start;

  gchar *str;

  guint pcm_channels;
  int ret;
  guint nth_card;
  guint i;
  
  GRecMutex *gstreamer_devin_mutex;

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* check device */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  if(gstreamer_devin->card_uri == device ||
     !g_ascii_strcasecmp(gstreamer_devin->card_uri,
			 device)){
    g_rec_mutex_unlock(gstreamer_devin_mutex);
  
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-gstreamer-devin-")){
    g_rec_mutex_unlock(gstreamer_devin_mutex);

    g_warning("invalid GSTREAMER device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-gstreamer-devin-%u",
	       &nth_card);

  if(ret != 1){
    g_rec_mutex_unlock(gstreamer_devin_mutex);

    g_warning("invalid GSTREAMER device specifier");

    return;
  }

  g_free(gstreamer_devin->card_uri);
  gstreamer_devin->card_uri = g_strdup(device);

  /* apply name to port */
  g_rec_mutex_unlock(gstreamer_devin_mutex);
  
#if 0
  pcm_channels = gstreamer_devin->pcm_channels;
  
  gstreamer_port_start = 
    gstreamer_port = g_list_copy(gstreamer_devin->gstreamer_port);
  
  for(i = 0; i < pcm_channels && gstreamer_port != NULL; i++){
    str = g_strdup_printf("ags%d-%04d",
			  nth_card,
			  i);
    
    g_object_set(gstreamer_port->data,
		 "port-name", str,
		 NULL);
    g_free(str);

    gstreamer_port = gstreamer_port->next;
  }

  g_list_free(gstreamer_port_start);
#endif
}

gchar*
ags_gstreamer_devin_get_device(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;
  
  gchar *device;

  GRecMutex *gstreamer_devin_mutex;

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);
  
  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  device = NULL;

  /* get device */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  device = g_strdup(gstreamer_devin->card_uri);

  g_rec_mutex_unlock(gstreamer_devin_mutex);
  
  return(device);
}

void
ags_gstreamer_devin_set_presets(AgsSoundcard *soundcard,
				guint channels,
				guint rate,
				guint buffer_size,
				guint format)
{
  AgsGstreamerDevin *gstreamer_devin;

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  g_object_set(gstreamer_devin,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_gstreamer_devin_get_presets(AgsSoundcard *soundcard,
				guint *channels,
				guint *rate,
				guint *buffer_size,
				guint *format)
{
  AgsGstreamerDevin *gstreamer_devin;

  GRecMutex *gstreamer_devin_mutex;

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);
  
  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* get presets */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  if(channels != NULL){
    *channels = gstreamer_devin->pcm_channels;
  }

  if(rate != NULL){
    *rate = gstreamer_devin->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = gstreamer_devin->buffer_size;
  }

  if(format != NULL){
    *format = gstreamer_devin->format;
  }

  g_rec_mutex_unlock(gstreamer_devin_mutex);
}

/**
 * ags_gstreamer_devin_list_cards:
 * @soundcard: the #AgsSoundcard
 * @card_id: GSTREAMER identifier
 * @card_name: card name
 *
 * List available soundcards.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_devin_list_cards(AgsSoundcard *soundcard,
			       GList **card_id, GList **card_name)
{
  AgsGstreamerClient *gstreamer_client;
  AgsGstreamerDevin *gstreamer_devin;

  AgsApplicationContext *application_context;
  
  GList *list_start, *list;

  gchar *card_uri;
  gchar *client_name;
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

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
    if(AGS_IS_GSTREAMER_DEVIN(list->data)){
      if(card_id != NULL){
	card_uri = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));
	
	if(AGS_GSTREAMER_DEVIN(list->data)->card_uri != NULL){
	  *card_id = g_list_prepend(*card_id,
				    card_uri);
	}else{
	  *card_id = g_list_prepend(*card_id,
				    g_strdup("(null)"));

	  g_warning("ags_gstreamer_devin_list_cards() - card id (null)");
	}
      }

      if(card_name != NULL){
	g_object_get(list->data,
		     "gstreamer-client", &gstreamer_client,
		     NULL);
	
	if(gstreamer_client != NULL){
	  /* get client name */
	  g_object_get(gstreamer_client,
		       "client-name", &client_name,
		       NULL);
	  
	  *card_name = g_list_prepend(*card_name,
				      client_name);

	  g_object_unref(gstreamer_client);
	}else{
	  *card_name = g_list_prepend(*card_name,
				      g_strdup("(null)"));

	  g_warning("ags_gstreamer_devin_list_cards() - GSTREAMER client not connected (null)");
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
ags_gstreamer_devin_pcm_info(AgsSoundcard *soundcard,
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
ags_gstreamer_devin_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_CAPTURE);
}

gboolean
ags_gstreamer_devin_is_starting(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  gboolean is_starting;
  
  GRecMutex *gstreamer_devin_mutex;
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* check is starting */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  is_starting = ((AGS_GSTREAMER_DEVIN_START_RECORD & (gstreamer_devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(gstreamer_devin_mutex);
  
  return(is_starting);
}

gboolean
ags_gstreamer_devin_is_recording(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  gboolean is_playing;
  
  GRecMutex *gstreamer_devin_mutex;

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);
  
  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* check is starting */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  is_playing = ((AGS_GSTREAMER_DEVIN_RECORD & (gstreamer_devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(gstreamer_devin_mutex);

  return(is_playing);
}

gchar*
ags_gstreamer_devin_get_uptime(AgsSoundcard *soundcard)
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
ags_gstreamer_devin_port_init(AgsSoundcard *soundcard,
			      GError **error)
{
  AgsGstreamerDevin *gstreamer_devin;

  guint format, word_size;
  
  GRecMutex *gstreamer_devin_mutex;

  if(ags_soundcard_is_recording(soundcard)){
    return;
  }
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);
  
  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* retrieve word size */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  switch(gstreamer_devin->format){
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
    g_rec_mutex_unlock(gstreamer_devin_mutex);
    
    g_warning("ags_gstreamer_devin_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for capture */
  gstreamer_devin->app_buffer_mode = AGS_GSTREAMER_DEVIN_APP_BUFFER_7;
  gstreamer_devin->flags |= (AGS_GSTREAMER_DEVIN_START_RECORD |
			     AGS_GSTREAMER_DEVIN_RECORD |
			     AGS_GSTREAMER_DEVIN_NONBLOCKING);

  memset(gstreamer_devin->app_buffer[0], 0, gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  memset(gstreamer_devin->app_buffer[1], 0, gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  memset(gstreamer_devin->app_buffer[2], 0, gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  memset(gstreamer_devin->app_buffer[3], 0, gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  memset(gstreamer_devin->app_buffer[4], 0, gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  memset(gstreamer_devin->app_buffer[5], 0, gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  memset(gstreamer_devin->app_buffer[6], 0, gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  memset(gstreamer_devin->app_buffer[7], 0, gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);

  /*  */
  gstreamer_devin->tact_counter = 0.0;
  gstreamer_devin->delay_counter = floor(ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(gstreamer_devin)));
  gstreamer_devin->tic_counter = 0;

  gstreamer_devin->flags |= (AGS_GSTREAMER_DEVIN_INITIALIZED |
			     AGS_GSTREAMER_DEVIN_START_RECORD |
			     AGS_GSTREAMER_DEVIN_RECORD);
  
  g_atomic_int_and(&(gstreamer_devin->sync_flags),
		   (~(AGS_GSTREAMER_DEVIN_PASS_THROUGH)));
  g_atomic_int_or(&(gstreamer_devin->sync_flags),
		  AGS_GSTREAMER_DEVIN_INITIAL_CALLBACK);

  g_rec_mutex_unlock(gstreamer_devin_mutex);
}

void
ags_gstreamer_devin_port_record(AgsSoundcard *soundcard,
				GError **error)
{
  AgsGstreamerClient *gstreamer_client;
  AgsGstreamerDevin *gstreamer_devin;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
        
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;

  guint word_size;
  gboolean gstreamer_client_activated;
  
  GRecMutex *gstreamer_devin_mutex;
  GRecMutex *gstreamer_client_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);
  
  application_context = ags_application_context_get_instance();

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* client */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  gstreamer_client = (AgsGstreamerClient *) gstreamer_devin->gstreamer_client;
  
  callback_mutex = &(gstreamer_devin->callback_mutex);
  callback_finish_mutex = &(gstreamer_devin->callback_finish_mutex);

  g_rec_mutex_unlock(gstreamer_devin_mutex);

  /* do capture */
  g_rec_mutex_lock(gstreamer_devin_mutex);
  
  gstreamer_devin->flags &= (~AGS_GSTREAMER_DEVIN_START_RECORD);
  
  if((AGS_GSTREAMER_DEVIN_INITIALIZED & (gstreamer_devin->flags)) == 0){
    g_rec_mutex_unlock(gstreamer_devin_mutex);
    
    return;
  }

  switch(gstreamer_devin->format){
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
    g_rec_mutex_unlock(gstreamer_devin_mutex);
    
    g_warning("ags_gstreamer_devin_port_record(): unsupported word size");
    
    return;
  }

  g_rec_mutex_unlock(gstreamer_devin_mutex);

  /* get client mutex */
  gstreamer_client_mutex = AGS_GSTREAMER_CLIENT_GET_OBJ_MUTEX(gstreamer_client);

  /* get activated */
  g_rec_mutex_lock(gstreamer_client_mutex);

  gstreamer_client_activated = ((AGS_GSTREAMER_CLIENT_ACTIVATED & (gstreamer_client->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(gstreamer_client_mutex);

  if(gstreamer_client_activated){
    /* signal */
    if((AGS_GSTREAMER_DEVIN_INITIAL_CALLBACK & (g_atomic_int_get(&(gstreamer_devin->sync_flags)))) == 0){
      g_mutex_lock(callback_mutex);

      g_atomic_int_or(&(gstreamer_devin->sync_flags),
		      AGS_GSTREAMER_DEVIN_CALLBACK_DONE);
    
      if((AGS_GSTREAMER_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(gstreamer_devin->sync_flags)))) != 0){
	g_cond_signal(&(gstreamer_devin->callback_cond));
      }

      g_mutex_unlock(callback_mutex);
    }
    
    /* wait callback */	
    if((AGS_GSTREAMER_DEVIN_INITIAL_CALLBACK & (g_atomic_int_get(&(gstreamer_devin->sync_flags)))) == 0){
      g_mutex_lock(callback_finish_mutex);
    
      if((AGS_GSTREAMER_DEVIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(gstreamer_devin->sync_flags)))) == 0){
	g_atomic_int_or(&(gstreamer_devin->sync_flags),
			AGS_GSTREAMER_DEVIN_CALLBACK_FINISH_WAIT);
    
	while((AGS_GSTREAMER_DEVIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(gstreamer_devin->sync_flags)))) == 0 &&
	      (AGS_GSTREAMER_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(gstreamer_devin->sync_flags)))) != 0){
	  g_cond_wait(&(gstreamer_devin->callback_finish_cond),
		      callback_finish_mutex);
	}
      }
    
      g_atomic_int_and(&(gstreamer_devin->sync_flags),
		       (~(AGS_GSTREAMER_DEVIN_CALLBACK_FINISH_WAIT |
			  AGS_GSTREAMER_DEVIN_CALLBACK_FINISH_DONE)));
    
      g_mutex_unlock(callback_finish_mutex);
    }else{
      g_atomic_int_and(&(gstreamer_devin->sync_flags),
		       (~AGS_GSTREAMER_DEVIN_INITIAL_CALLBACK));
    }
  }

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;
    
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) gstreamer_devin);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) gstreamer_devin);
  task = g_list_append(task,
		       clear_buffer);
    
  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) gstreamer_devin);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);
  
  /* unref */
  g_object_unref(task_launcher);
}

void
ags_gstreamer_devin_port_free(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  AgsApplicationContext *application_context;

  guint word_size;

  GRecMutex *gstreamer_devin_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /*  */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  if((AGS_GSTREAMER_DEVIN_INITIALIZED & (gstreamer_devin->flags)) == 0){
    g_rec_mutex_unlock(gstreamer_devin_mutex);

    return;
  }

  //  g_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);

  callback_mutex = &(gstreamer_devin->callback_mutex);
  callback_finish_mutex = &(gstreamer_devin->callback_finish_mutex);
  
  gstreamer_devin->app_buffer_mode = AGS_GSTREAMER_DEVIN_APP_BUFFER_0;
  gstreamer_devin->flags &= (~(AGS_GSTREAMER_DEVIN_RECORD));

  g_atomic_int_or(&(gstreamer_devin->sync_flags),
		  AGS_GSTREAMER_DEVIN_PASS_THROUGH);
  g_atomic_int_and(&(gstreamer_devin->sync_flags),
		   (~AGS_GSTREAMER_DEVIN_INITIAL_CALLBACK));

  /* signal callback */
  g_mutex_lock(callback_mutex);

  g_atomic_int_or(&(gstreamer_devin->sync_flags),
		  AGS_GSTREAMER_DEVIN_CALLBACK_DONE);
    
  if((AGS_GSTREAMER_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(gstreamer_devin->sync_flags)))) != 0){
    g_cond_signal(&(gstreamer_devin->callback_cond));
  }

  g_mutex_unlock(callback_mutex);

  /* signal thread */
  g_mutex_lock(callback_finish_mutex);

  g_atomic_int_or(&(gstreamer_devin->sync_flags),
		  AGS_GSTREAMER_DEVIN_CALLBACK_FINISH_DONE);
    
  if((AGS_GSTREAMER_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(gstreamer_devin->sync_flags)))) != 0){
    g_cond_signal(&(gstreamer_devin->callback_finish_cond));
  }

  g_mutex_unlock(callback_finish_mutex);
  
  /*  */
  gstreamer_devin->note_offset = gstreamer_devin->start_note_offset;
  gstreamer_devin->note_offset_absolute = gstreamer_devin->start_note_offset;

  switch(gstreamer_devin->format){
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
    
    g_critical("ags_gstreamer_devin_free(): unsupported word size");
  }

  memset(gstreamer_devin->app_buffer[0], 0, (size_t) gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  memset(gstreamer_devin->app_buffer[1], 0, (size_t) gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  memset(gstreamer_devin->app_buffer[2], 0, (size_t) gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  memset(gstreamer_devin->app_buffer[3], 0, (size_t) gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  memset(gstreamer_devin->app_buffer[4], 0, (size_t) gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  memset(gstreamer_devin->app_buffer[5], 0, (size_t) gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  memset(gstreamer_devin->app_buffer[6], 0, (size_t) gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  memset(gstreamer_devin->app_buffer[7], 0, (size_t) gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);

  g_rec_mutex_unlock(gstreamer_devin_mutex);
}

void
ags_gstreamer_devin_tic(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  gdouble delay;
  gdouble delay_counter;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  
  GRecMutex *gstreamer_devin_mutex;
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  delay = gstreamer_devin->delay[gstreamer_devin->tic_counter];
  delay_counter = gstreamer_devin->delay_counter;

  note_offset = gstreamer_devin->note_offset;
  note_offset_absolute = gstreamer_devin->note_offset_absolute;
  
  loop_left = gstreamer_devin->loop_left;
  loop_right = gstreamer_devin->loop_right;
  
  do_loop = gstreamer_devin->do_loop;

  g_rec_mutex_unlock(gstreamer_devin_mutex);

  if(delay_counter + 1 >= delay){
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
    g_rec_mutex_lock(gstreamer_devin_mutex);
    
    gstreamer_devin->delay_counter = delay_counter + 1.0 - delay;
    gstreamer_devin->tact_counter += 1.0;

    g_rec_mutex_unlock(gstreamer_devin_mutex);
  }else{
    g_rec_mutex_lock(gstreamer_devin_mutex);
    
    gstreamer_devin->delay_counter += 1.0;

    g_rec_mutex_unlock(gstreamer_devin_mutex);
  }
}

void
ags_gstreamer_devin_offset_changed(AgsSoundcard *soundcard,
				   guint note_offset)
{
  AgsGstreamerDevin *gstreamer_devin;
  
  GRecMutex *gstreamer_devin_mutex;
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* offset changed */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  gstreamer_devin->tic_counter += 1;

  if(gstreamer_devin->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    gstreamer_devin->tic_counter = 0;
  }

  g_rec_mutex_unlock(gstreamer_devin_mutex);
}

void
ags_gstreamer_devin_set_bpm(AgsSoundcard *soundcard,
			    gdouble bpm)
{
  AgsGstreamerDevin *gstreamer_devin;

  GRecMutex *gstreamer_devin_mutex;
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* set bpm */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  gstreamer_devin->bpm = bpm;

  g_rec_mutex_unlock(gstreamer_devin_mutex);

  ags_gstreamer_devin_adjust_delay_and_attack(gstreamer_devin);
}

gdouble
ags_gstreamer_devin_get_bpm(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  gdouble bpm;
  
  GRecMutex *gstreamer_devin_mutex;
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* get bpm */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  bpm = gstreamer_devin->bpm;
  
  g_rec_mutex_unlock(gstreamer_devin_mutex);

  return(bpm);
}

void
ags_gstreamer_devin_set_delay_factor(AgsSoundcard *soundcard,
				     gdouble delay_factor)
{
  AgsGstreamerDevin *gstreamer_devin;

  GRecMutex *gstreamer_devin_mutex;
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* set delay factor */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  gstreamer_devin->delay_factor = delay_factor;

  g_rec_mutex_unlock(gstreamer_devin_mutex);

  ags_gstreamer_devin_adjust_delay_and_attack(gstreamer_devin);
}

gdouble
ags_gstreamer_devin_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  gdouble delay_factor;
  
  GRecMutex *gstreamer_devin_mutex;
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* get delay factor */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  delay_factor = gstreamer_devin->delay_factor;
  
  g_rec_mutex_unlock(gstreamer_devin_mutex);

  return(delay_factor);
}

gdouble
ags_gstreamer_devin_get_delay(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  guint delay_index;
  gdouble delay;
  
  GRecMutex *gstreamer_devin_mutex;
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* get delay */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  delay_index = gstreamer_devin->tic_counter;

  delay = gstreamer_devin->delay[delay_index];
  
  g_rec_mutex_unlock(gstreamer_devin_mutex);
  
  return(delay);
}

gdouble
ags_gstreamer_devin_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  gdouble absolute_delay;
  
  GRecMutex *gstreamer_devin_mutex;
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);
  
  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* get absolute delay */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  absolute_delay = (60.0 * (((gdouble) gstreamer_devin->samplerate / (gdouble) gstreamer_devin->buffer_size) / (gdouble) gstreamer_devin->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) gstreamer_devin->delay_factor)));

  g_rec_mutex_unlock(gstreamer_devin_mutex);

  return(absolute_delay);
}

guint
ags_gstreamer_devin_get_attack(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  guint attack_index;
  guint attack;
  
  GRecMutex *gstreamer_devin_mutex;  

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);
  
  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* get attack */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  attack_index = gstreamer_devin->tic_counter;

  attack = gstreamer_devin->attack[attack_index];

  g_rec_mutex_unlock(gstreamer_devin_mutex);
  
  return(attack);
}

void*
ags_gstreamer_devin_get_buffer(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  void *buffer;

  GRecMutex *gstreamer_devin_mutex;  
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);
  
  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  g_rec_mutex_lock(gstreamer_devin_mutex);

  if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_0){
    buffer = gstreamer_devin->app_buffer[0];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_1){
    buffer = gstreamer_devin->app_buffer[1];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_2){
    buffer = gstreamer_devin->app_buffer[2];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_3){
    buffer = gstreamer_devin->app_buffer[3];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_4){
    buffer = gstreamer_devin->app_buffer[4];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_5){
    buffer = gstreamer_devin->app_buffer[5];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_6){
    buffer = gstreamer_devin->app_buffer[6];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_7){
    buffer = gstreamer_devin->app_buffer[7];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(gstreamer_devin_mutex);

  return(buffer);
}

void*
ags_gstreamer_devin_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  void *buffer;

  GRecMutex *gstreamer_devin_mutex;  
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);
  
  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  g_rec_mutex_lock(gstreamer_devin_mutex);

  if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_0){
    buffer = gstreamer_devin->app_buffer[1];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_1){
    buffer = gstreamer_devin->app_buffer[2];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_2){
    buffer = gstreamer_devin->app_buffer[3];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_3){
    buffer = gstreamer_devin->app_buffer[4];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_4){
    buffer = gstreamer_devin->app_buffer[5];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_5){
    buffer = gstreamer_devin->app_buffer[6];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_6){
    buffer = gstreamer_devin->app_buffer[7];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_7){
    buffer = gstreamer_devin->app_buffer[0];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(gstreamer_devin_mutex);

  return(buffer);
}

void*
ags_gstreamer_devin_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  void *buffer;

  GRecMutex *gstreamer_devin_mutex;  
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  g_rec_mutex_lock(gstreamer_devin_mutex);

  if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_0){
    buffer = gstreamer_devin->app_buffer[7];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_1){
    buffer = gstreamer_devin->app_buffer[0];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_2){
    buffer = gstreamer_devin->app_buffer[1];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_3){
    buffer = gstreamer_devin->app_buffer[2];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_4){
    buffer = gstreamer_devin->app_buffer[3];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_5){
    buffer = gstreamer_devin->app_buffer[4];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_6){
    buffer = gstreamer_devin->app_buffer[5];
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_7){
    buffer = gstreamer_devin->app_buffer[6];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(gstreamer_devin_mutex);

  return(buffer);
}

void
ags_gstreamer_devin_lock_buffer(AgsSoundcard *soundcard,
				void *buffer)
{
  AgsGstreamerDevin *gstreamer_devin;

  GRecMutex *buffer_mutex;
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(gstreamer_devin->app_buffer != NULL){
    if(buffer == gstreamer_devin->app_buffer[0]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[0];
    }else if(buffer == gstreamer_devin->app_buffer[1]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[1];
    }else if(buffer == gstreamer_devin->app_buffer[2]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[2];
    }else if(buffer == gstreamer_devin->app_buffer[3]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[3];
    }else if(buffer == gstreamer_devin->app_buffer[4]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[4];
    }else if(buffer == gstreamer_devin->app_buffer[5]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[5];
    }else if(buffer == gstreamer_devin->app_buffer[6]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[6];
    }else if(buffer == gstreamer_devin->app_buffer[7]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[7];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}


void
ags_gstreamer_devin_unlock_buffer(AgsSoundcard *soundcard,
				  void *buffer)
{
  AgsGstreamerDevin *gstreamer_devin;

  GRecMutex *buffer_mutex;
  
  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(gstreamer_devin->app_buffer != NULL){
    if(buffer == gstreamer_devin->app_buffer[0]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[0];
    }else if(buffer == gstreamer_devin->app_buffer[1]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[1];
    }else if(buffer == gstreamer_devin->app_buffer[2]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[2];
    }else if(buffer == gstreamer_devin->app_buffer[3]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[3];
    }else if(buffer == gstreamer_devin->app_buffer[4]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[4];
    }else if(buffer == gstreamer_devin->app_buffer[5]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[5];
    }else if(buffer == gstreamer_devin->app_buffer[6]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[6];
    }else if(buffer == gstreamer_devin->app_buffer[7]){
      buffer_mutex = gstreamer_devin->app_buffer_mutex[7];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_gstreamer_devin_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  guint delay_counter;
  
  GRecMutex *gstreamer_devin_mutex;  

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);
  
  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* delay counter */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  delay_counter = gstreamer_devin->delay_counter;
  
  g_rec_mutex_unlock(gstreamer_devin_mutex);

  return(delay_counter);
}

void
ags_gstreamer_devin_set_start_note_offset(AgsSoundcard *soundcard,
					  guint start_note_offset)
{
  AgsGstreamerDevin *gstreamer_devin;

  GRecMutex *gstreamer_devin_mutex;  

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* set note offset */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  gstreamer_devin->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(gstreamer_devin_mutex);
}

guint
ags_gstreamer_devin_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  guint start_note_offset;
  
  GRecMutex *gstreamer_devin_mutex;  

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* set note offset */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  start_note_offset = gstreamer_devin->start_note_offset;

  g_rec_mutex_unlock(gstreamer_devin_mutex);

  return(start_note_offset);
}

void
ags_gstreamer_devin_set_note_offset(AgsSoundcard *soundcard,
				    guint note_offset)
{
  AgsGstreamerDevin *gstreamer_devin;

  GRecMutex *gstreamer_devin_mutex;  

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* set note offset */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  gstreamer_devin->note_offset = note_offset;

  g_rec_mutex_unlock(gstreamer_devin_mutex);
}

guint
ags_gstreamer_devin_get_note_offset(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  guint note_offset;
  
  GRecMutex *gstreamer_devin_mutex;  

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* set note offset */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  note_offset = gstreamer_devin->note_offset;

  g_rec_mutex_unlock(gstreamer_devin_mutex);

  return(note_offset);
}

void
ags_gstreamer_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
					     guint note_offset_absolute)
{
  AgsGstreamerDevin *gstreamer_devin;
  
  GRecMutex *gstreamer_devin_mutex;  

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* set note offset */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  gstreamer_devin->note_offset_absolute = note_offset_absolute;

  g_rec_mutex_unlock(gstreamer_devin_mutex);
}

guint
ags_gstreamer_devin_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  guint note_offset_absolute;
  
  GRecMutex *gstreamer_devin_mutex;  

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* set note offset */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  note_offset_absolute = gstreamer_devin->note_offset_absolute;

  g_rec_mutex_unlock(gstreamer_devin_mutex);

  return(note_offset_absolute);
}

void
ags_gstreamer_devin_set_loop(AgsSoundcard *soundcard,
			     guint loop_left, guint loop_right,
			     gboolean do_loop)
{
  AgsGstreamerDevin *gstreamer_devin;

  GRecMutex *gstreamer_devin_mutex;  

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* set loop */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  gstreamer_devin->loop_left = loop_left;
  gstreamer_devin->loop_right = loop_right;
  gstreamer_devin->do_loop = do_loop;

  if(do_loop){
    gstreamer_devin->loop_offset = gstreamer_devin->note_offset;
  }

  g_rec_mutex_unlock(gstreamer_devin_mutex);
}

void
ags_gstreamer_devin_get_loop(AgsSoundcard *soundcard,
			     guint *loop_left, guint *loop_right,
			     gboolean *do_loop)
{
  AgsGstreamerDevin *gstreamer_devin;

  GRecMutex *gstreamer_devin_mutex;  

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* get loop */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  if(loop_left != NULL){
    *loop_left = gstreamer_devin->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = gstreamer_devin->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = gstreamer_devin->do_loop;
  }

  g_rec_mutex_unlock(gstreamer_devin_mutex);
}

guint
ags_gstreamer_devin_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsGstreamerDevin *gstreamer_devin;

  guint loop_offset;
  
  GRecMutex *gstreamer_devin_mutex;  

  gstreamer_devin = AGS_GSTREAMER_DEVIN(soundcard);

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* get loop offset */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  loop_offset = gstreamer_devin->loop_offset;
  
  g_rec_mutex_unlock(gstreamer_devin_mutex);

  return(loop_offset);
}

/**
 * ags_gstreamer_devin_switch_buffer_flag:
 * @gstreamer_devin: an #AgsGstreamerDevin
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_devin_switch_buffer_flag(AgsGstreamerDevin *gstreamer_devin)
{
  GRecMutex *gstreamer_devin_mutex;
  
  if(!AGS_IS_GSTREAMER_DEVIN(gstreamer_devin)){
    return;
  }

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* switch buffer flag */
  g_rec_mutex_lock(gstreamer_devin_mutex);

  if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_0){
    gstreamer_devin->app_buffer_mode = AGS_GSTREAMER_DEVIN_APP_BUFFER_1;
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_1){
    gstreamer_devin->app_buffer_mode = AGS_GSTREAMER_DEVIN_APP_BUFFER_2;
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_2){
    gstreamer_devin->app_buffer_mode = AGS_GSTREAMER_DEVIN_APP_BUFFER_3;
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_3){
    gstreamer_devin->app_buffer_mode = AGS_GSTREAMER_DEVIN_APP_BUFFER_4;
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_4){
    gstreamer_devin->app_buffer_mode = AGS_GSTREAMER_DEVIN_APP_BUFFER_5;
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_5){
    gstreamer_devin->app_buffer_mode = AGS_GSTREAMER_DEVIN_APP_BUFFER_6;
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_6){
    gstreamer_devin->app_buffer_mode = AGS_GSTREAMER_DEVIN_APP_BUFFER_7;
  }else if(gstreamer_devin->app_buffer_mode == AGS_GSTREAMER_DEVIN_APP_BUFFER_7){
    gstreamer_devin->app_buffer_mode = AGS_GSTREAMER_DEVIN_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(gstreamer_devin_mutex);
}

/**
 * ags_gstreamer_devin_adjust_delay_and_attack:
 * @gstreamer_devin: the #AgsGstreamerDevin
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_devin_adjust_delay_and_attack(AgsGstreamerDevin *gstreamer_devin)
{
  if(!AGS_IS_GSTREAMER_DEVIN(gstreamer_devin)){
    return;
  }

  ags_soundcard_util_adjust_delay_and_attack(gstreamer_devin);
}

/**
 * ags_gstreamer_devin_realloc_buffer:
 * @gstreamer_devin: the #AgsGstreamerDevin
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.6.0
 */
void
ags_gstreamer_devin_realloc_buffer(AgsGstreamerDevin *gstreamer_devin)
{
  AgsGstreamerClient *gstreamer_client;
  
  guint port_count;
  guint pcm_channels;
  guint buffer_size;
  guint word_size;

  GRecMutex *gstreamer_devin_mutex;  

  if(!AGS_IS_GSTREAMER_DEVIN(gstreamer_devin)){
    return;
  }

  /* get gstreamer devin mutex */
  gstreamer_devin_mutex = AGS_GSTREAMER_DEVIN_GET_OBJ_MUTEX(gstreamer_devin);

  /* get word size */  
  g_rec_mutex_lock(gstreamer_devin_mutex);

  gstreamer_client = gstreamer_devin->gstreamer_client;
  
  port_count = g_list_length(gstreamer_devin->gstreamer_port);
  
  pcm_channels = gstreamer_devin->pcm_channels;
  buffer_size = gstreamer_devin->buffer_size;

  switch(gstreamer_devin->format){
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
    g_warning("ags_gstreamer_devin_realloc_buffer(): unsupported word size");
    return;
  }

  g_rec_mutex_unlock(gstreamer_devin_mutex);

  if(port_count < pcm_channels){
    AgsGstreamerPort *gstreamer_port;

    gchar *str;
    
    guint nth_soundcard;
    guint i;

    g_rec_mutex_lock(gstreamer_devin_mutex);

    if(gstreamer_devin->card_uri != NULL){
      sscanf(gstreamer_devin->card_uri,
	     "ags-gstreamer-devin-%u",
	     &nth_soundcard);
    }else{
      g_rec_mutex_unlock(gstreamer_devin_mutex);

      g_warning("ags_gstreamer_devin_realloc_buffer() - card uri not set");
      
      return;
    }

    g_rec_mutex_unlock(gstreamer_devin_mutex);
    
    for(i = port_count; i < pcm_channels; i++){
      str = g_strdup_printf("ags%d-%04d",
			    nth_soundcard,
			    i);
      
      gstreamer_port = ags_gstreamer_port_new(gstreamer_client);
      ags_gstreamer_client_add_port((AgsGstreamerClient *) gstreamer_client,
				    (GObject *) gstreamer_port);

      g_rec_mutex_lock(gstreamer_devin_mutex);
    
      gstreamer_devin->gstreamer_port = g_list_prepend(gstreamer_devin->gstreamer_port,
						       gstreamer_port);
    
      if(gstreamer_devin->port_name == NULL){
	gstreamer_devin->port_name = (gchar **) g_malloc(2 * sizeof(gchar *));
	gstreamer_devin->port_name[0] = g_strdup(str);
      }else{
	gstreamer_devin->port_name = (gchar **) g_realloc(gstreamer_devin->port_name,
							  (i + 2) * sizeof(gchar *));
	gstreamer_devin->port_name[i] = g_strdup(str);
      }
      
      g_rec_mutex_unlock(gstreamer_devin_mutex);
      
      ags_gstreamer_port_register(gstreamer_port,
				  str,
				  TRUE, FALSE,
				  FALSE);
    }

    gstreamer_devin->port_name[gstreamer_devin->pcm_channels] = NULL;    
  }else if(port_count > pcm_channels){
    GList *gstreamer_port_start, *gstreamer_port;

    guint i;

    g_rec_mutex_lock(gstreamer_devin_mutex);

    gstreamer_port =
      gstreamer_port_start = g_list_copy(gstreamer_devin->gstreamer_port);

    g_rec_mutex_unlock(gstreamer_devin_mutex);

    for(i = 0; i < port_count - pcm_channels; i++){
      gstreamer_devin->gstreamer_port = g_list_remove(gstreamer_devin->gstreamer_port,
						      gstreamer_port->data);
      ags_gstreamer_port_unregister(gstreamer_port->data);
      
      g_object_unref(gstreamer_port->data);
      
      gstreamer_port = gstreamer_port->next;
    }

    g_list_free(gstreamer_port_start);

    g_rec_mutex_lock(gstreamer_devin_mutex);
    
    gstreamer_devin->port_name = (gchar **) g_realloc(gstreamer_devin->port_name,
						      (gstreamer_devin->pcm_channels + 1) * sizeof(gchar *));
    gstreamer_devin->port_name[gstreamer_devin->pcm_channels] = NULL;

    g_rec_mutex_unlock(gstreamer_devin_mutex);
  }
  
  /* AGS_GSTREAMER_DEVIN_APP_BUFFER_0 */
  if(gstreamer_devin->app_buffer[0] != NULL){
    g_free(gstreamer_devin->app_buffer[0]);
  }
  
  gstreamer_devin->app_buffer[0] = (void *) g_malloc(gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  
  /* AGS_GSTREAMER_DEVIN_APP_BUFFER_1 */
  if(gstreamer_devin->app_buffer[1] != NULL){
    g_free(gstreamer_devin->app_buffer[1]);
  }

  gstreamer_devin->app_buffer[1] = (void *) g_malloc(gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  
  /* AGS_GSTREAMER_DEVIN_APP_BUFFER_2 */
  if(gstreamer_devin->app_buffer[2] != NULL){
    g_free(gstreamer_devin->app_buffer[2]);
  }

  gstreamer_devin->app_buffer[2] = (void *) g_malloc(gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
  
  /* AGS_GSTREAMER_DEVIN_APP_BUFFER_3 */
  if(gstreamer_devin->app_buffer[3] != NULL){
    g_free(gstreamer_devin->app_buffer[3]);
  }

  /* AGS_GSTREAMER_DEVIN_APP_BUFFER_4 */
  if(gstreamer_devin->app_buffer[4] != NULL){
    g_free(gstreamer_devin->app_buffer[4]);
  }
  
  gstreamer_devin->app_buffer[4] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  /* AGS_GSTREAMER_DEVIN_APP_BUFFER_5 */
  if(gstreamer_devin->app_buffer[5] != NULL){
    g_free(gstreamer_devin->app_buffer[5]);
  }
  
  gstreamer_devin->app_buffer[5] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  /* AGS_GSTREAMER_DEVIN_APP_BUFFER_6 */
  if(gstreamer_devin->app_buffer[6] != NULL){
    g_free(gstreamer_devin->app_buffer[6]);
  }
  
  gstreamer_devin->app_buffer[6] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  /* AGS_GSTREAMER_DEVIN_APP_BUFFER_7 */
  if(gstreamer_devin->app_buffer[7] != NULL){
    g_free(gstreamer_devin->app_buffer[7]);
  }
  
  gstreamer_devin->app_buffer[3] = (void *) g_malloc(gstreamer_devin->pcm_channels * gstreamer_devin->buffer_size * word_size);
}

/**
 * ags_gstreamer_devin_new:
 *
 * Creates a new instance of #AgsGstreamerDevin.
 *
 * Returns: the new #AgsGstreamerDevin
 *
 * Since: 3.6.0
 */
AgsGstreamerDevin*
ags_gstreamer_devin_new()
{
  AgsGstreamerDevin *gstreamer_devin;

  gstreamer_devin = (AgsGstreamerDevin *) g_object_new(AGS_TYPE_GSTREAMER_DEVIN,
						       NULL);
  
  return(gstreamer_devin);
}
