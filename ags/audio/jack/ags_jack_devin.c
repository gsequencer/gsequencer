/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/audio/jack/ags_jack_devin.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_soundcard_util.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/jack/ags_jack_server.h>
#include <ags/audio/jack/ags_jack_client.h>
#include <ags/audio/jack/ags_jack_port.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <string.h>
#include <math.h>
#include <time.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_jack_devin_class_init(AgsJackDevinClass *jack_devin);
void ags_jack_devin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_jack_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_jack_devin_init(AgsJackDevin *jack_devin);
void ags_jack_devin_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_jack_devin_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_jack_devin_dispose(GObject *gobject);
void ags_jack_devin_finalize(GObject *gobject);

AgsUUID* ags_jack_devin_get_uuid(AgsConnectable *connectable);
gboolean ags_jack_devin_has_resource(AgsConnectable *connectable);
gboolean ags_jack_devin_is_ready(AgsConnectable *connectable);
void ags_jack_devin_add_to_registry(AgsConnectable *connectable);
void ags_jack_devin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_jack_devin_list_resource(AgsConnectable *connectable);
xmlNode* ags_jack_devin_xml_compose(AgsConnectable *connectable);
void ags_jack_devin_xml_parse(AgsConnectable *connectable,
			      xmlNode *node);
gboolean ags_jack_devin_is_connected(AgsConnectable *connectable);
void ags_jack_devin_connect(AgsConnectable *connectable);
void ags_jack_devin_disconnect(AgsConnectable *connectable);

void ags_jack_devin_set_device(AgsSoundcard *soundcard,
			       gchar *device);
gchar* ags_jack_devin_get_device(AgsSoundcard *soundcard);

void ags_jack_devin_set_presets(AgsSoundcard *soundcard,
				guint channels,
				guint rate,
				guint buffer_size,
				AgsSoundcardFormat format);
void ags_jack_devin_get_presets(AgsSoundcard *soundcard,
				guint *channels,
				guint *rate,
				guint *buffer_size,
				AgsSoundcardFormat *format);

void ags_jack_devin_list_cards(AgsSoundcard *soundcard,
			       GList **card_id, GList **card_name);
void ags_jack_devin_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			     guint *channels_min, guint *channels_max,
			     guint *rate_min, guint *rate_max,
			     guint *buffer_size_min, guint *buffer_size_max,
			     GError **error);
guint ags_jack_devin_get_capability(AgsSoundcard *soundcard);

gboolean ags_jack_devin_is_starting(AgsSoundcard *soundcard);
gboolean ags_jack_devin_is_recording(AgsSoundcard *soundcard);

gchar* ags_jack_devin_get_uptime(AgsSoundcard *soundcard);

void ags_jack_devin_port_init(AgsSoundcard *soundcard,
			      GError **error);
void ags_jack_devin_port_record(AgsSoundcard *soundcard,
				GError **error);
void ags_jack_devin_port_free(AgsSoundcard *soundcard);

void ags_jack_devin_tic(AgsSoundcard *soundcard);
void ags_jack_devin_offset_changed(AgsSoundcard *soundcard,
				   guint note_offset);

void ags_jack_devin_set_bpm(AgsSoundcard *soundcard,
			    gdouble bpm);
gdouble ags_jack_devin_get_bpm(AgsSoundcard *soundcard);

void ags_jack_devin_set_delay_factor(AgsSoundcard *soundcard,
				     gdouble delay_factor);
gdouble ags_jack_devin_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_jack_devin_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_jack_devin_get_delay(AgsSoundcard *soundcard);
guint ags_jack_devin_get_attack(AgsSoundcard *soundcard);

void* ags_jack_devin_get_buffer(AgsSoundcard *soundcard);
void* ags_jack_devin_get_next_buffer(AgsSoundcard *soundcard);
void* ags_jack_devin_get_prev_buffer(AgsSoundcard *soundcard);

void ags_jack_devin_lock_buffer(AgsSoundcard *soundcard,
				void *buffer);
void ags_jack_devin_unlock_buffer(AgsSoundcard *soundcard,
				  void *buffer);

guint ags_jack_devin_get_delay_counter(AgsSoundcard *soundcard);

void ags_jack_devin_set_start_note_offset(AgsSoundcard *soundcard,
					  guint start_note_offset);
guint ags_jack_devin_get_start_note_offset(AgsSoundcard *soundcard);

void ags_jack_devin_set_note_offset(AgsSoundcard *soundcard,
				    guint note_offset);
guint ags_jack_devin_get_note_offset(AgsSoundcard *soundcard);

void ags_jack_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
					     guint note_offset);
guint ags_jack_devin_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_jack_devin_set_loop(AgsSoundcard *soundcard,
			     guint loop_left, guint loop_right,
			     gboolean do_loop);
void ags_jack_devin_get_loop(AgsSoundcard *soundcard,
			     guint *loop_left, guint *loop_right,
			     gboolean *do_loop);

guint ags_jack_devin_get_loop_offset(AgsSoundcard *soundcard);

void ags_jack_devin_get_note_256th_offset(AgsSoundcard *soundcard,
					  guint *note_256th_offset_lower,
					  guint *note_256th_offset_upper);

void ags_jack_devin_get_note_256th_attack(AgsSoundcard *soundcard,
					  guint *note_256th_attack_lower,
					  guint *note_256th_attack_upper);

guint ags_jack_devin_get_note_256th_attack_at_position(AgsSoundcard *soundcard,
						       guint note_256th_attack_position);

void ags_jack_devin_get_note_256th_attack_position(AgsSoundcard *soundcard,
						   guint *note_256th_attack_position_lower,
						   guint *note_256th_attack_position_upper);

guint ags_jack_devin_get_note_256th_attack_of_16th_pulse(AgsSoundcard *soundcard);
guint ags_jack_devin_get_note_256th_attack_of_16th_pulse_position(AgsSoundcard *soundcard);

/**
 * SECTION:ags_jack_devin
 * @short_description: Input from soundcard
 * @title: AgsJackDevin
 * @section_id:
 * @include: ags/audio/jack/ags_jack_devin.h
 *
 * #AgsJackDevin represents a soundcard and supports input.
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
  PROP_JACK_CLIENT,
  PROP_JACK_PORT,
  PROP_CHANNEL,
};

static gpointer ags_jack_devin_parent_class = NULL;

GType
ags_jack_devin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_jack_devin = 0;

    static const GTypeInfo ags_jack_devin_info = {
      sizeof (AgsJackDevinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_devin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsJackDevin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_jack_devin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_jack_devin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_jack_devin_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_jack_devin = g_type_register_static(G_TYPE_OBJECT,
						 "AgsJackDevin",
						 &ags_jack_devin_info,
						 0);

    g_type_add_interface_static(ags_type_jack_devin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_jack_devin,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_jack_devin);
  }

  return g_define_type_id__volatile;
}

GType
ags_jack_devin_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_JACK_DEVIN_INITIALIZED, "AGS_JACK_DEVIN_INITIALIZED", "jack-devin-initialized" },
      { AGS_JACK_DEVIN_START_RECORD, "AGS_JACK_DEVIN_START_RECORD", "jack-devin-start-record" },
      { AGS_JACK_DEVIN_RECORD, "AGS_JACK_DEVIN_RECORD", "jack-devin-record" },
      { AGS_JACK_DEVIN_SHUTDOWN, "AGS_JACK_DEVIN_SHUTDOWN", "jack-devin-shutdown" },
      { AGS_JACK_DEVIN_NONBLOCKING, "AGS_JACK_DEVIN_NONBLOCKING", "jack-devin-nonblocking" },
      { AGS_JACK_DEVIN_ATTACK_FIRST, "AGS_JACK_DEVIN_ATTACK_FIRST", "jack-devin-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsJackDevinFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

void
ags_jack_devin_class_init(AgsJackDevinClass *jack_devin)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_jack_devin_parent_class = g_type_class_peek_parent(jack_devin);

  /* GObjectClass */
  gobject = (GObjectClass *) jack_devin;

  gobject->set_property = ags_jack_devin_set_property;
  gobject->get_property = ags_jack_devin_get_property;

  gobject->dispose = ags_jack_devin_dispose;
  gobject->finalize = ags_jack_devin_finalize;

  /* properties */
  /**
   * AgsJackDevin:device:
   *
   * The jack soundcard indentifier
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-jack-devin-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsJackDevin:dsp-channels:
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
   * AgsJackDevin:pcm-channels:
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
   * AgsJackDevin:format:
   *
   * The precision of the buffer
   * 
   * Since: 3.0.0
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
   * AgsJackDevin:buffer-size:
   *
   * The buffer size
   * 
   * Since: 3.0.0
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
   * AgsJackDevin:samplerate:
   *
   * The samplerate
   * 
   * Since: 3.0.0
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
   * AgsJackDevin:buffer:
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
   * AgsJackDevin:bpm:
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
   * AgsJackDevin:delay-factor:
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
   * AgsJackDevin:attack:
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

  
  /**
   * AgsJackDevin:jack-client:
   *
   * The assigned #AgsJackClient
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("jack-client",
				   i18n_pspec("jack client object"),
				   i18n_pspec("The jack client object"),
				   AGS_TYPE_JACK_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_JACK_CLIENT,
				  param_spec);

  /**
   * AgsJackDevin:jack-port:
   *
   * The assigned #AgsJackPort
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("jack-port",
				    i18n_pspec("jack port object"),
				    i18n_pspec("The jack port object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_JACK_PORT,
				  param_spec);
}

GQuark
ags_jack_devin_error_quark()
{
  return(g_quark_from_static_string("ags-jack_devin-error-quark"));
}

void
ags_jack_devin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_jack_devin_get_uuid;
  connectable->has_resource = ags_jack_devin_has_resource;

  connectable->is_ready = ags_jack_devin_is_ready;
  connectable->add_to_registry = ags_jack_devin_add_to_registry;
  connectable->remove_from_registry = ags_jack_devin_remove_from_registry;

  connectable->list_resource = ags_jack_devin_list_resource;
  connectable->xml_compose = ags_jack_devin_xml_compose;
  connectable->xml_parse = ags_jack_devin_xml_parse;

  connectable->is_connected = ags_jack_devin_is_connected;  
  connectable->connect = ags_jack_devin_connect;
  connectable->disconnect = ags_jack_devin_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_jack_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_jack_devin_set_device;
  soundcard->get_device = ags_jack_devin_get_device;
  
  soundcard->set_presets = ags_jack_devin_set_presets;
  soundcard->get_presets = ags_jack_devin_get_presets;

  soundcard->list_cards = ags_jack_devin_list_cards;
  soundcard->pcm_info = ags_jack_devin_pcm_info;
  soundcard->get_capability = ags_jack_devin_get_capability;

  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_jack_devin_is_starting;
  soundcard->is_playing = NULL;
  soundcard->is_recording = ags_jack_devin_is_recording;

  soundcard->get_uptime = ags_jack_devin_get_uptime;
  
  soundcard->play_init = NULL;
  soundcard->play = NULL;

  soundcard->record_init = ags_jack_devin_port_init;
  soundcard->record = ags_jack_devin_port_record;

  soundcard->stop = ags_jack_devin_port_free;

  soundcard->tic = ags_jack_devin_tic;
  soundcard->offset_changed = ags_jack_devin_offset_changed;
    
  soundcard->set_bpm = ags_jack_devin_set_bpm;
  soundcard->get_bpm = ags_jack_devin_get_bpm;

  soundcard->set_delay_factor = ags_jack_devin_set_delay_factor;
  soundcard->get_delay_factor = ags_jack_devin_get_delay_factor;
  
  soundcard->get_absolute_delay = ags_jack_devin_get_absolute_delay;

  soundcard->get_delay = ags_jack_devin_get_delay;
  soundcard->get_attack = ags_jack_devin_get_attack;

  soundcard->get_buffer = ags_jack_devin_get_buffer;
  soundcard->get_next_buffer = ags_jack_devin_get_next_buffer;
  soundcard->get_prev_buffer = ags_jack_devin_get_prev_buffer;

  soundcard->lock_buffer = ags_jack_devin_lock_buffer;
  soundcard->unlock_buffer = ags_jack_devin_unlock_buffer;

  soundcard->get_delay_counter = ags_jack_devin_get_delay_counter;

  soundcard->set_start_note_offset = ags_jack_devin_set_start_note_offset;
  soundcard->get_start_note_offset = ags_jack_devin_get_start_note_offset;

  soundcard->set_note_offset = ags_jack_devin_set_note_offset;
  soundcard->get_note_offset = ags_jack_devin_get_note_offset;

  soundcard->set_note_offset_absolute = ags_jack_devin_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_jack_devin_get_note_offset_absolute;

  soundcard->set_loop = ags_jack_devin_set_loop;
  soundcard->get_loop = ags_jack_devin_get_loop;

  soundcard->get_loop_offset = ags_jack_devin_get_loop_offset;

  soundcard->get_note_256th_offset = ags_jack_devin_get_note_256th_offset;

  soundcard->get_note_256th_attack = ags_jack_devin_get_note_256th_attack;

  soundcard->get_note_256th_attack_at_position = ags_jack_devin_get_note_256th_attack_at_position;

  soundcard->get_note_256th_attack_position = ags_jack_devin_get_note_256th_attack_position;

  soundcard->get_note_256th_attack_of_16th_pulse = ags_jack_devin_get_note_256th_attack_of_16th_pulse;
  soundcard->get_note_256th_attack_of_16th_pulse_position = ags_jack_devin_get_note_256th_attack_of_16th_pulse_position;
}

void
ags_jack_devin_init(AgsJackDevin *jack_devin)
{
  AgsConfig *config;

  GList *start_note_256th_attack;

  gchar *str;
  gchar *segmentation;

  gdouble absolute_delay;
  guint denominator, numerator;
  guint i;
  
  /* flags */
  jack_devin->flags = 0;
  jack_devin->connectable_flags = 0;
  g_atomic_int_set(&(jack_devin->sync_flags),
		   AGS_JACK_DEVIN_PASS_THROUGH);

  /* jack devin mutex */
  g_rec_mutex_init(&(jack_devin->obj_mutex));

  /* uuid */
  jack_devin->uuid = ags_uuid_alloc();
  ags_uuid_generate(jack_devin->uuid);

  /* presets */
  config = ags_config_get_instance();

  jack_devin->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  jack_devin->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  jack_devin->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  jack_devin->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  jack_devin->format = ags_soundcard_helper_config_get_format(config);

  /* buffer */
  jack_devin->app_buffer_mode = AGS_JACK_DEVIN_APP_BUFFER_0;

  jack_devin->app_buffer_mutex = (GRecMutex **) g_malloc(4 * sizeof(GRecMutex *));

  for(i = 0; i < 4; i++){
    jack_devin->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(jack_devin->app_buffer_mutex[i]);
  }

  jack_devin->app_buffer = (void **) g_malloc(4 * sizeof(void*));

  jack_devin->app_buffer[0] = (void *) g_malloc(jack_devin->pcm_channels * jack_devin->buffer_size * sizeof(gint16));
  jack_devin->app_buffer[1] = (void *) g_malloc(jack_devin->pcm_channels * jack_devin->buffer_size * sizeof(gint16));
  jack_devin->app_buffer[2] = (void *) g_malloc(jack_devin->pcm_channels * jack_devin->buffer_size * sizeof(gint16));
  jack_devin->app_buffer[3] = (void *) g_malloc(jack_devin->pcm_channels * jack_devin->buffer_size * sizeof(gint16));
  
  ags_jack_devin_realloc_buffer(jack_devin);
  
  /* bpm */
  jack_devin->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  jack_devin->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    jack_devin->delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }

  /* delay and attack */
  absolute_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(jack_devin));

  jack_devin->delay = (gdouble *) g_malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					   sizeof(gdouble));
  
  jack_devin->attack = (guint *) g_malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					  sizeof(guint));

  jack_devin->note_256th_delay = absolute_delay / 16.0;

  jack_devin->note_256th_attack_of_16th_pulse = 0;
  jack_devin->note_256th_attack_of_16th_pulse_position = 0;

  start_note_256th_attack = NULL;

  for(i = 0; i < 32; i++){
    guint *note_256th_attack;
    
    note_256th_attack = (guint *) malloc((int) AGS_SOUNDCARD_DEFAULT_PERIOD *
					 sizeof(guint));
    
    start_note_256th_attack = g_list_prepend(start_note_256th_attack,
					     note_256th_attack);
  }

  jack_devin->note_256th_attack = start_note_256th_attack;

  ags_jack_devin_adjust_delay_and_attack(jack_devin);
  
  /* counters */
  jack_devin->tact_counter = 0.0;
  jack_devin->delay_counter = 0.0;
  jack_devin->tic_counter = 0;

  jack_devin->start_note_offset = 0;
  jack_devin->note_offset = 0;
  jack_devin->note_offset_absolute = 0;

  jack_devin->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  jack_devin->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  jack_devin->do_loop = FALSE;

  jack_devin->loop_offset = 0;

  /*  */
  jack_devin->card_uri = NULL;
  jack_devin->jack_client = NULL;

  jack_devin->port_name = NULL;
  jack_devin->jack_port = NULL;

  /* callback mutex */
  g_mutex_init(&(jack_devin->callback_mutex));

  g_cond_init(&(jack_devin->callback_cond));

  /* callback finish mutex */
  g_mutex_init(&(jack_devin->callback_finish_mutex));

  g_cond_init(&(jack_devin->callback_finish_cond));

  /* 256th */
  //NOTE:JK: note_256th_delay was prior set

  //NOTE:JK: note_256th_attack was prior set
  
  jack_devin->note_256th_offset = 0;

  if(jack_devin->note_256th_delay >= 1.0){
    jack_devin->note_256th_offset_last = 0;
  }else{
    jack_devin->note_256th_offset_last = (guint) floor(1.0 / jack_devin->note_256th_delay);
  }
}

void
ags_jack_devin_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsJackDevin *jack_devin;

  GRecMutex *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(gobject);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      g_rec_mutex_lock(jack_devin_mutex);

      jack_devin->card_uri = g_strdup(device);

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      g_rec_mutex_lock(jack_devin_mutex);

      if(dsp_channels == jack_devin->dsp_channels){
	g_rec_mutex_unlock(jack_devin_mutex);

	return;
      }

      jack_devin->dsp_channels = dsp_channels;

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      g_rec_mutex_lock(jack_devin_mutex);

      if(pcm_channels == jack_devin->pcm_channels){
	g_rec_mutex_unlock(jack_devin_mutex);

	return;
      }

      jack_devin->pcm_channels = pcm_channels;

      g_rec_mutex_unlock(jack_devin_mutex);

      ags_jack_devin_realloc_buffer(jack_devin);
    }
    break;
  case PROP_FORMAT:
    {
      AgsSoundcardFormat format;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(jack_devin_mutex);

      if(format == jack_devin->format){
	g_rec_mutex_unlock(jack_devin_mutex);

	return;
      }

      jack_devin->format = format;

      g_rec_mutex_unlock(jack_devin_mutex);

      ags_jack_devin_realloc_buffer(jack_devin);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      g_rec_mutex_lock(jack_devin_mutex);

      if(buffer_size == jack_devin->buffer_size){
	g_rec_mutex_unlock(jack_devin_mutex);

	return;
      }

      jack_devin->buffer_size = buffer_size;

      g_rec_mutex_unlock(jack_devin_mutex);

      ags_jack_devin_realloc_buffer(jack_devin);
      ags_jack_devin_adjust_delay_and_attack(jack_devin);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      g_rec_mutex_lock(jack_devin_mutex);
      
      if(samplerate == jack_devin->samplerate){
	g_rec_mutex_unlock(jack_devin_mutex);

	return;
      }

      jack_devin->samplerate = samplerate;

      g_rec_mutex_unlock(jack_devin_mutex);

      ags_jack_devin_realloc_buffer(jack_devin);
      ags_jack_devin_adjust_delay_and_attack(jack_devin);
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

      g_rec_mutex_lock(jack_devin_mutex);

      jack_devin->bpm = bpm;

      g_rec_mutex_unlock(jack_devin_mutex);

      ags_jack_devin_adjust_delay_and_attack(jack_devin);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      g_rec_mutex_lock(jack_devin_mutex);

      jack_devin->delay_factor = delay_factor;

      g_rec_mutex_unlock(jack_devin_mutex);

      ags_jack_devin_adjust_delay_and_attack(jack_devin);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      AgsJackClient *jack_client;

      jack_client = (AgsJackClient *) g_value_get_object(value);

      g_rec_mutex_lock(jack_devin_mutex);

      if(jack_devin->jack_client == (GObject *) jack_client){
	g_rec_mutex_unlock(jack_devin_mutex);

	return;
      }

      if(jack_devin->jack_client != NULL){
	g_object_unref(G_OBJECT(jack_devin->jack_client));
      }

      if(jack_client != NULL){
	g_object_ref(jack_client);
      }
      
      jack_devin->jack_client = (GObject *) jack_client;

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_JACK_PORT:
    {
      AgsJackPort *jack_port;

      jack_port = (AgsJackPort *) g_value_get_pointer(value);

      g_rec_mutex_lock(jack_devin_mutex);

      if(!AGS_IS_JACK_PORT(jack_port) ||
	 g_list_find(jack_devin->jack_port, jack_port) != NULL){
	g_rec_mutex_unlock(jack_devin_mutex);

	return;
      }

      g_object_ref(jack_port);
      jack_devin->jack_port = g_list_append(jack_devin->jack_port,
					    jack_port);

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_devin_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsJackDevin *jack_devin;

  GRecMutex *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(gobject);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(jack_devin_mutex);

      g_value_set_string(value, jack_devin->card_uri);

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_rec_mutex_lock(jack_devin_mutex);

      g_value_set_uint(value, jack_devin->dsp_channels);

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_rec_mutex_lock(jack_devin_mutex);

      g_value_set_uint(value, jack_devin->pcm_channels);

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(jack_devin_mutex);

      g_value_set_uint(value, jack_devin->format);

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(jack_devin_mutex);

      g_value_set_uint(value, jack_devin->buffer_size);

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(jack_devin_mutex);

      g_value_set_uint(value, jack_devin->samplerate);

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(jack_devin_mutex);

      g_value_set_pointer(value, jack_devin->app_buffer);

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(jack_devin_mutex);

      g_value_set_double(value, jack_devin->bpm);

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_rec_mutex_lock(jack_devin_mutex);

      g_value_set_double(value, jack_devin->delay_factor);

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_ATTACK:
    {
      g_rec_mutex_lock(jack_devin_mutex);

      g_value_set_pointer(value, jack_devin->attack);

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      g_rec_mutex_lock(jack_devin_mutex);

      g_value_set_object(value, jack_devin->jack_client);

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  case PROP_JACK_PORT:
    {
      g_rec_mutex_lock(jack_devin_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(jack_devin->jack_port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_devin_dispose(GObject *gobject)
{
  AgsJackDevin *jack_devin;

  jack_devin = AGS_JACK_DEVIN(gobject);

  /* jack client */
  if(jack_devin->jack_client != NULL){
    g_object_unref(jack_devin->jack_client);

    jack_devin->jack_client = NULL;
  }

  /* jack port */
  g_list_free_full(jack_devin->jack_port,
		   g_object_unref);

  jack_devin->jack_port = NULL;

  /* call parent */
  G_OBJECT_CLASS(ags_jack_devin_parent_class)->dispose(gobject);
}

void
ags_jack_devin_finalize(GObject *gobject)
{
  AgsJackDevin *jack_devin;

  guint i;

  jack_devin = AGS_JACK_DEVIN(gobject);

  ags_uuid_free(jack_devin->uuid);

  for(i = 0; i < 4; i++){
    g_rec_mutex_clear(jack_devin->app_buffer_mutex[i]);

    g_free(jack_devin->app_buffer_mutex[i]);
  }
  
  g_free(jack_devin->app_buffer_mutex);

  /* free output buffer */
  g_free(jack_devin->app_buffer[0]);
  g_free(jack_devin->app_buffer[1]);
  g_free(jack_devin->app_buffer[2]);
  g_free(jack_devin->app_buffer[3]);

  /* free buffer array */
  g_free(jack_devin->app_buffer);

  g_free(jack_devin->delay);
  g_free(jack_devin->attack);

  /* jack client */
  if(jack_devin->jack_client != NULL){
    g_object_unref(jack_devin->jack_client);
  }
  
  /* jack port */
  g_list_free_full(jack_devin->jack_port,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_jack_devin_parent_class)->finalize(gobject);
}

AgsUUID*
ags_jack_devin_get_uuid(AgsConnectable *connectable)
{
  AgsJackDevin *jack_devin;
  
  AgsUUID *ptr;

  GRecMutex *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(connectable);

  /* get jack devin signal mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* get UUID */
  g_rec_mutex_lock(jack_devin_mutex);

  ptr = jack_devin->uuid;

  g_rec_mutex_unlock(jack_devin_mutex);
  
  return(ptr);
}

gboolean
ags_jack_devin_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_jack_devin_is_ready(AgsConnectable *connectable)
{
  AgsJackDevin *jack_devin;
  
  gboolean is_ready;

  GRecMutex *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(connectable);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* check is ready */
  g_rec_mutex_lock(jack_devin_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (jack_devin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_devin_mutex);
  
  return(is_ready);
}

void
ags_jack_devin_add_to_registry(AgsConnectable *connectable)
{
  AgsJackDevin *jack_devin;

  GRecMutex *jack_devin_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  jack_devin = AGS_JACK_DEVIN(connectable);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  g_rec_mutex_lock(jack_devin_mutex);

  jack_devin->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(jack_devin_mutex);
}

void
ags_jack_devin_remove_from_registry(AgsConnectable *connectable)
{
  AgsJackDevin *jack_devin;

  GRecMutex *jack_devin_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  jack_devin = AGS_JACK_DEVIN(connectable);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  g_rec_mutex_lock(jack_devin_mutex);

  jack_devin->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(jack_devin_mutex);
}

xmlNode*
ags_jack_devin_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_jack_devin_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_jack_devin_xml_parse(AgsConnectable *connectable,
			 xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_jack_devin_is_connected(AgsConnectable *connectable)
{
  AgsJackDevin *jack_devin;
  
  gboolean is_connected;

  GRecMutex *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(connectable);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* check is connected */
  g_rec_mutex_lock(jack_devin_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (jack_devin->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_devin_mutex);
  
  return(is_connected);
}

void
ags_jack_devin_connect(AgsConnectable *connectable)
{
  AgsJackDevin *jack_devin;

  GRecMutex *jack_devin_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  jack_devin = AGS_JACK_DEVIN(connectable);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  g_rec_mutex_lock(jack_devin_mutex);

  jack_devin->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(jack_devin_mutex);
}

void
ags_jack_devin_disconnect(AgsConnectable *connectable)
{
  AgsJackDevin *jack_devin;

  GRecMutex *jack_devin_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  jack_devin = AGS_JACK_DEVIN(connectable);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  g_rec_mutex_lock(jack_devin_mutex);

  jack_devin->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(jack_devin_mutex);
}

/**
 * ags_jack_devin_test_flags:
 * @jack_devin: the #AgsJackDevin
 * @flags: the flags
 *
 * Test @flags to be set on @jack_devin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_jack_devin_test_flags(AgsJackDevin *jack_devin, AgsJackDevinFlags flags)
{
  gboolean retval;  
  
  GRecMutex *jack_devin_mutex;

  if(!AGS_IS_JACK_DEVIN(jack_devin)){
    return(FALSE);
  }

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* test */
  g_rec_mutex_lock(jack_devin_mutex);

  retval = (flags & (jack_devin->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(jack_devin_mutex);

  return(retval);
}

/**
 * ags_jack_devin_set_flags:
 * @jack_devin: the #AgsJackDevin
 * @flags: see #AgsJackDevinFlags-enum
 *
 * Enable a feature of @jack_devin.
 *
 * Since: 3.0.0
 */
void
ags_jack_devin_set_flags(AgsJackDevin *jack_devin, AgsJackDevinFlags flags)
{
  GRecMutex *jack_devin_mutex;

  if(!AGS_IS_JACK_DEVIN(jack_devin)){
    return;
  }

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(jack_devin_mutex);

  jack_devin->flags |= flags;
  
  g_rec_mutex_unlock(jack_devin_mutex);
}
    
/**
 * ags_jack_devin_unset_flags:
 * @jack_devin: the #AgsJackDevin
 * @flags: see #AgsJackDevinFlags-enum
 *
 * Disable a feature of @jack_devin.
 *
 * Since: 3.0.0
 */
void
ags_jack_devin_unset_flags(AgsJackDevin *jack_devin, AgsJackDevinFlags flags)
{  
  GRecMutex *jack_devin_mutex;

  if(!AGS_IS_JACK_DEVIN(jack_devin)){
    return;
  }

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(jack_devin_mutex);

  jack_devin->flags &= (~flags);
  
  g_rec_mutex_unlock(jack_devin_mutex);
}

void
ags_jack_devin_set_device(AgsSoundcard *soundcard,
			  gchar *device)
{
  AgsJackDevin *jack_devin;

  GList *jack_port, *jack_port_start;

  gchar *str;

  guint pcm_channels;
  int ret;
  guint nth_card;
  guint i;
  
  GRecMutex *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* check device */
  g_rec_mutex_lock(jack_devin_mutex);

  if(jack_devin->card_uri == device ||
     (jack_devin->card_uri != NULL &&
      !g_ascii_strcasecmp(jack_devin->card_uri,
			  device))){
    g_rec_mutex_unlock(jack_devin_mutex);
  
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-jack-devin-")){
    g_rec_mutex_unlock(jack_devin_mutex);

    g_warning("invalid JACK device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-jack-devin-%u",
	       &nth_card);

  if(ret != 1){
    g_rec_mutex_unlock(jack_devin_mutex);

    g_warning("invalid JACK device specifier");

    return;
  }

  g_free(jack_devin->card_uri);
  jack_devin->card_uri = g_strdup(device);

  /* apply name to port */
  g_rec_mutex_unlock(jack_devin_mutex);
  
#if 0
  pcm_channels = jack_devin->pcm_channels;
  
  jack_port_start = 
    jack_port = g_list_copy(jack_devin->jack_port);
  
  for(i = 0; i < pcm_channels && jack_port != NULL; i++){
    str = g_strdup_printf("ags%d-%04d",
			  nth_card,
			  i);
    
    g_object_set(jack_port->data,
		 "port-name", str,
		 NULL);
    g_free(str);

    jack_port = jack_port->next;
  }

  g_list_free(jack_port_start);
#endif
}

gchar*
ags_jack_devin_get_device(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;
  
  gchar *device;

  GRecMutex *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  device = NULL;

  /* get device */
  g_rec_mutex_lock(jack_devin_mutex);

  device = g_strdup(jack_devin->card_uri);

  g_rec_mutex_unlock(jack_devin_mutex);
  
  return(device);
}

void
ags_jack_devin_set_presets(AgsSoundcard *soundcard,
			   guint channels,
			   guint rate,
			   guint buffer_size,
			   AgsSoundcardFormat format)
{
  AgsJackDevin *jack_devin;

  jack_devin = AGS_JACK_DEVIN(soundcard);

  g_object_set(jack_devin,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_jack_devin_get_presets(AgsSoundcard *soundcard,
			   guint *channels,
			   guint *rate,
			   guint *buffer_size,
			   AgsSoundcardFormat *format)
{
  AgsJackDevin *jack_devin;

  GRecMutex *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* get presets */
  g_rec_mutex_lock(jack_devin_mutex);

  if(channels != NULL){
    *channels = jack_devin->pcm_channels;
  }

  if(rate != NULL){
    *rate = jack_devin->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = jack_devin->buffer_size;
  }

  if(format != NULL){
    *format = jack_devin->format;
  }

  g_rec_mutex_unlock(jack_devin_mutex);
}

/**
 * ags_jack_devin_list_cards:
 * @soundcard: the #AgsSoundcard
 * @card_id: JACK identifier
 * @card_name: card name
 *
 * List available soundcards.
 *
 * Since: 3.0.0
 */
void
ags_jack_devin_list_cards(AgsSoundcard *soundcard,
			  GList **card_id, GList **card_name)
{
  AgsJackClient *jack_client;
  AgsJackDevin *jack_devin;

  AgsApplicationContext *application_context;
  
  GList *list_start, *list;

  gchar *card_uri;
  gchar *client_name;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

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
    if(AGS_IS_JACK_DEVIN(list->data)){
      if(card_id != NULL){
	card_uri = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));
	
	if(AGS_JACK_DEVIN(list->data)->card_uri != NULL){
	  *card_id = g_list_prepend(*card_id,
				    card_uri);
	}else{
	  *card_id = g_list_prepend(*card_id,
				    g_strdup("(null)"));

	  g_warning("ags_jack_devin_list_cards() - card id (null)");
	}
      }

      if(card_name != NULL){
	g_object_get(list->data,
		     "jack-client", &jack_client,
		     NULL);
	
	if(jack_client != NULL){
	  /* get client name */
	  g_object_get(jack_client,
		       "client-name", &client_name,
		       NULL);
	  
	  *card_name = g_list_prepend(*card_name,
				      client_name);

	  g_object_unref(jack_client);
	}else{
	  *card_name = g_list_prepend(*card_name,
				      g_strdup("(null)"));

	  g_warning("ags_jack_devin_list_cards() - JACK client not connected (null)");
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
ags_jack_devin_pcm_info(AgsSoundcard *soundcard,
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
ags_jack_devin_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_CAPTURE);
}

gboolean
ags_jack_devin_is_starting(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  gboolean is_starting;
  
  GRecMutex *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* check is starting */
  g_rec_mutex_lock(jack_devin_mutex);

  is_starting = ((AGS_JACK_DEVIN_START_RECORD & (jack_devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_devin_mutex);
  
  return(is_starting);
}

gboolean
ags_jack_devin_is_recording(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  gboolean is_playing;
  
  GRecMutex *jack_devin_mutex;

  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* check is starting */
  g_rec_mutex_lock(jack_devin_mutex);

  is_playing = ((AGS_JACK_DEVIN_RECORD & (jack_devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_devin_mutex);

  return(is_playing);
}

gchar*
ags_jack_devin_get_uptime(AgsSoundcard *soundcard)
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
ags_jack_devin_port_init(AgsSoundcard *soundcard,
			 GError **error)
{
  AgsJackDevin *jack_devin;

  AgsSoundcardFormat format;
  guint word_size;
  
  GRecMutex *jack_devin_mutex;

  if(ags_soundcard_is_recording(soundcard)){
    return;
  }
  
  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* retrieve word size */
  g_rec_mutex_lock(jack_devin_mutex);

  switch(jack_devin->format){
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
    g_rec_mutex_unlock(jack_devin_mutex);
    
    g_warning("ags_jack_devin_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for capture */
  jack_devin->app_buffer_mode = AGS_JACK_DEVIN_APP_BUFFER_3;
  jack_devin->flags |= (AGS_JACK_DEVIN_START_RECORD |
			AGS_JACK_DEVIN_RECORD |
			AGS_JACK_DEVIN_NONBLOCKING);

  memset(jack_devin->app_buffer[0], 0, jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  memset(jack_devin->app_buffer[1], 0, jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  memset(jack_devin->app_buffer[2], 0, jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  memset(jack_devin->app_buffer[3], 0, jack_devin->pcm_channels * jack_devin->buffer_size * word_size);

  /*  */
  jack_devin->tact_counter = 0.0;
  jack_devin->delay_counter = floor(ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(jack_devin)));
  jack_devin->tic_counter = 0;

  jack_devin->note_256th_attack_of_16th_pulse = 0;
  jack_devin->note_256th_attack_of_16th_pulse_position = 0;

  jack_devin->flags |= (AGS_JACK_DEVIN_INITIALIZED |
			AGS_JACK_DEVIN_START_RECORD |
			AGS_JACK_DEVIN_RECORD);
  
  g_atomic_int_and(&(jack_devin->sync_flags),
		   (~(AGS_JACK_DEVIN_PASS_THROUGH)));
  g_atomic_int_or(&(jack_devin->sync_flags),
		  AGS_JACK_DEVIN_INITIAL_CALLBACK);

  g_rec_mutex_unlock(jack_devin_mutex);
}

void
ags_jack_devin_port_record(AgsSoundcard *soundcard,
			   GError **error)
{
  AgsJackClient *jack_client;
  AgsJackDevin *jack_devin;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
        
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;

  guint word_size;
  gboolean jack_client_activated;
  
  GRecMutex *jack_devin_mutex;
  GRecMutex *jack_client_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  application_context = ags_application_context_get_instance();

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* client */
  g_rec_mutex_lock(jack_devin_mutex);

  jack_client = (AgsJackClient *) jack_devin->jack_client;
  
  callback_mutex = &(jack_devin->callback_mutex);
  callback_finish_mutex = &(jack_devin->callback_finish_mutex);

  g_rec_mutex_unlock(jack_devin_mutex);

  /* do capture */
  g_rec_mutex_lock(jack_devin_mutex);
  
  jack_devin->flags &= (~AGS_JACK_DEVIN_START_RECORD);
  
  if((AGS_JACK_DEVIN_INITIALIZED & (jack_devin->flags)) == 0){
    g_rec_mutex_unlock(jack_devin_mutex);
    
    return;
  }

  switch(jack_devin->format){
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
    g_rec_mutex_unlock(jack_devin_mutex);
    
    g_warning("ags_jack_devin_port_record(): unsupported word size");
    
    return;
  }

  g_rec_mutex_unlock(jack_devin_mutex);

  /* get client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* get activated */
  g_rec_mutex_lock(jack_client_mutex);

  jack_client_activated = ((AGS_JACK_CLIENT_ACTIVATED & (jack_client->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_client_mutex);

  if(jack_client_activated){
    /* signal */
    if((AGS_JACK_DEVIN_INITIAL_CALLBACK & (g_atomic_int_get(&(jack_devin->sync_flags)))) == 0){
      g_mutex_lock(callback_mutex);

      g_atomic_int_or(&(jack_devin->sync_flags),
		      AGS_JACK_DEVIN_CALLBACK_DONE);
    
      if((AGS_JACK_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(jack_devin->sync_flags)))) != 0){
	g_cond_signal(&(jack_devin->callback_cond));
      }

      g_mutex_unlock(callback_mutex);
    }
    
    /* wait callback */	
    if((AGS_JACK_DEVIN_INITIAL_CALLBACK & (g_atomic_int_get(&(jack_devin->sync_flags)))) == 0){
      g_mutex_lock(callback_finish_mutex);
    
      if((AGS_JACK_DEVIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(jack_devin->sync_flags)))) == 0){
	g_atomic_int_or(&(jack_devin->sync_flags),
			AGS_JACK_DEVIN_CALLBACK_FINISH_WAIT);
    
	while((AGS_JACK_DEVIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(jack_devin->sync_flags)))) == 0 &&
	      (AGS_JACK_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_devin->sync_flags)))) != 0){
	  g_cond_wait(&(jack_devin->callback_finish_cond),
		      callback_finish_mutex);
	}
      }
    
      g_atomic_int_and(&(jack_devin->sync_flags),
		       (~(AGS_JACK_DEVIN_CALLBACK_FINISH_WAIT |
			  AGS_JACK_DEVIN_CALLBACK_FINISH_DONE)));
    
      g_mutex_unlock(callback_finish_mutex);
    }else{
      g_atomic_int_and(&(jack_devin->sync_flags),
		       (~AGS_JACK_DEVIN_INITIAL_CALLBACK));
    }
  }

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;
    
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) jack_devin);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) jack_devin);
  task = g_list_append(task,
		       clear_buffer);
    
  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) jack_devin);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);
  
  /* unref */
  g_object_unref(task_launcher);
}

void
ags_jack_devin_port_free(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  AgsApplicationContext *application_context;

  guint word_size;
  guint i;

  GRecMutex *jack_devin_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /*  */
  g_rec_mutex_lock(jack_devin_mutex);

  if((AGS_JACK_DEVIN_INITIALIZED & (jack_devin->flags)) == 0){
    g_rec_mutex_unlock(jack_devin_mutex);

    return;
  }

  //  g_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);

  callback_mutex = &(jack_devin->callback_mutex);
  callback_finish_mutex = &(jack_devin->callback_finish_mutex);
  
  jack_devin->app_buffer_mode = AGS_JACK_DEVIN_APP_BUFFER_0;
  jack_devin->flags &= (~(AGS_JACK_DEVIN_RECORD));

  g_atomic_int_or(&(jack_devin->sync_flags),
		  AGS_JACK_DEVIN_PASS_THROUGH);
  g_atomic_int_and(&(jack_devin->sync_flags),
		   (~AGS_JACK_DEVIN_INITIAL_CALLBACK));

  /* signal callback */
  g_mutex_lock(callback_mutex);

  g_atomic_int_or(&(jack_devin->sync_flags),
		  AGS_JACK_DEVIN_CALLBACK_DONE);
    
  if((AGS_JACK_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(jack_devin->sync_flags)))) != 0){
    g_cond_signal(&(jack_devin->callback_cond));
  }

  g_mutex_unlock(callback_mutex);

  /* signal thread */
  g_mutex_lock(callback_finish_mutex);

  g_atomic_int_or(&(jack_devin->sync_flags),
		  AGS_JACK_DEVIN_CALLBACK_FINISH_DONE);
    
  if((AGS_JACK_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_devin->sync_flags)))) != 0){
    g_cond_signal(&(jack_devin->callback_finish_cond));
  }

  g_mutex_unlock(callback_finish_mutex);
  
  /*  */
  jack_devin->note_offset = jack_devin->start_note_offset;
  jack_devin->note_offset_absolute = jack_devin->start_note_offset;

  jack_devin->note_256th_offset = 16 * jack_devin->start_note_offset;
  
  jack_devin->note_256th_attack_of_16th_pulse = 0;
  jack_devin->note_256th_attack_of_16th_pulse_position = 0;
  
  jack_devin->note_256th_offset_last = jack_devin->note_256th_offset;
  
  if(jack_devin->note_256th_delay < 1.0){
    guint buffer_size;
    guint note_256th_attack_lower, note_256th_attack_upper;
    guint i;
    
    buffer_size = jack_devin->buffer_size;

    note_256th_attack_lower = 0;
    note_256th_attack_upper = 0;
    
    ags_soundcard_get_note_256th_attack(AGS_SOUNDCARD(jack_devin),
					&note_256th_attack_lower,
					&note_256th_attack_upper);
    
    if(note_256th_attack_lower < note_256th_attack_upper){
      jack_devin->note_256th_offset_last = jack_devin->note_256th_offset + floor((note_256th_attack_upper - note_256th_attack_lower) / (jack_devin->note_256th_delay * (double) buffer_size));
    }
  }
  
  switch(jack_devin->format){
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
    
    g_critical("ags_jack_devin_free(): unsupported word size");
  }

  memset(jack_devin->app_buffer[1], 0, (size_t) jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  memset(jack_devin->app_buffer[2], 0, (size_t) jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  memset(jack_devin->app_buffer[3], 0, (size_t) jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  memset(jack_devin->app_buffer[0], 0, (size_t) jack_devin->pcm_channels * jack_devin->buffer_size * word_size);

  g_rec_mutex_unlock(jack_devin_mutex);
}

void
ags_jack_devin_tic(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint buffer_size;
  gdouble delay;
  gdouble delay_counter;
  guint attack;
  guint current_note_256th_attack;
  gdouble note_256th_delay;
  guint note_256th_attack_lower, note_256th_attack_upper;
  guint note_256th_attack_of_16th_pulse;
  guint note_256th_attack_of_16th_pulse_position;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  guint i;
  
  GRecMutex *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(jack_devin_mutex);

  buffer_size = jack_devin->buffer_size;

  delay = jack_devin->delay[jack_devin->tic_counter];
  delay_counter = jack_devin->delay_counter;

  note_256th_delay = jack_devin->note_256th_delay;
  
  attack = jack_devin->attack[jack_devin->tic_counter];

  note_offset = jack_devin->note_offset;
  note_offset_absolute = jack_devin->note_offset_absolute;
  
  loop_left = jack_devin->loop_left;
  loop_right = jack_devin->loop_right;
  
  do_loop = jack_devin->do_loop;

  g_rec_mutex_unlock(jack_devin_mutex);

  note_256th_attack_lower = 0;
  note_256th_attack_upper = 0;

  note_256th_attack_of_16th_pulse_position =  ags_soundcard_get_note_256th_attack_of_16th_pulse_position(soundcard);  

  ags_soundcard_get_note_256th_attack(soundcard,
				      &note_256th_attack_lower,
				      &note_256th_attack_upper);

  if(delay_counter + 1.0 >= floor(delay)){
    if(do_loop &&
       note_offset + 1 == loop_right){
      ags_soundcard_set_note_offset(soundcard,
				    loop_left);

      g_rec_mutex_lock(jack_devin_mutex);
      
      jack_devin->note_256th_offset = 16 * loop_left;

      jack_devin->note_256th_offset_last = jack_devin->note_256th_offset;
      
      if(note_256th_delay < 1.0){
	if(note_256th_attack_lower < note_256th_attack_upper){
	  jack_devin->note_256th_offset_last = jack_devin->note_256th_offset + floor((note_256th_attack_upper - note_256th_attack_lower) / (jack_devin->note_256th_delay * (double) buffer_size));
	}
      }

      note_256th_attack_of_16th_pulse = attack;

      i = 1;
      
      if(note_256th_delay < 1.0){
	for(; i < (guint) ceil(1.0 / note_256th_delay); i++){
	  if(note_256th_attack_of_16th_pulse_position - i >= 0){
	    current_note_256th_attack = ags_soundcard_get_note_256th_attack_at_position(soundcard,
											note_256th_attack_of_16th_pulse_position - i);

	    if(current_note_256th_attack < note_256th_attack_of_16th_pulse){
	      note_256th_attack_of_16th_pulse = current_note_256th_attack;
	    }else{
	      break;
	    }
	  }else{
	    break;
	  }
	}
      }

      jack_devin->note_256th_attack_of_16th_pulse = note_256th_attack_of_16th_pulse;
      jack_devin->note_256th_attack_of_16th_pulse_position += i;

      g_rec_mutex_unlock(jack_devin_mutex);
    }else{
      ags_soundcard_set_note_offset(soundcard,
				    note_offset + 1);

      g_rec_mutex_lock(jack_devin_mutex);
      
      jack_devin->note_256th_offset = 16 * (note_offset + 1);

      jack_devin->note_256th_offset_last = jack_devin->note_256th_offset;
      
      if(note_256th_delay < 1.0){
	if(note_256th_attack_lower < note_256th_attack_upper){
	  jack_devin->note_256th_offset_last = jack_devin->note_256th_offset + floor((note_256th_attack_upper - note_256th_attack_lower) / (jack_devin->note_256th_delay * (double) buffer_size));
	}
      }
      
      note_256th_attack_of_16th_pulse = attack;

      i = 1;
      
      if(note_256th_attack_of_16th_pulse_position != 0 &&
	 note_256th_delay < 1.0){
	for(; i < (guint) ceil(1.0 / note_256th_delay); i++){
	  if(note_256th_attack_of_16th_pulse_position - i >= 0){
	    current_note_256th_attack = ags_soundcard_get_note_256th_attack_at_position(soundcard,
											note_256th_attack_of_16th_pulse_position - i);

	    if(current_note_256th_attack < note_256th_attack_of_16th_pulse){
	      note_256th_attack_of_16th_pulse = current_note_256th_attack;
	    }else{
	      break;
	    }
	  }else{
	    break;
	  }
	}
      }

      jack_devin->note_256th_attack_of_16th_pulse = note_256th_attack_of_16th_pulse;
      jack_devin->note_256th_attack_of_16th_pulse_position += i;

      g_rec_mutex_unlock(jack_devin_mutex);
    }
    
    ags_soundcard_set_note_offset_absolute(soundcard,
					   note_offset_absolute + 1);

    /* delay */
    ags_soundcard_offset_changed(soundcard,
				 note_offset);
    
    /* reset - delay counter */
    g_rec_mutex_lock(jack_devin_mutex);
    
    jack_devin->delay_counter = delay_counter + 1.0 - delay;
    jack_devin->tact_counter += 1.0;

    g_rec_mutex_unlock(jack_devin_mutex);
  }else{
    g_rec_mutex_lock(jack_devin_mutex);
    
    jack_devin->note_256th_offset = (16 * jack_devin->note_offset) + (guint) floor((jack_devin->delay_counter + 1.0) * (1.0 / note_256th_delay));

    jack_devin->note_256th_offset_last = jack_devin->note_256th_offset;

    if(note_256th_attack_lower < note_256th_attack_upper){
      jack_devin->note_256th_offset_last = jack_devin->note_256th_offset + floor((note_256th_attack_upper - note_256th_attack_lower) / (jack_devin->note_256th_delay * (double) buffer_size));
    }

    jack_devin->delay_counter += 1.0;

    g_rec_mutex_unlock(jack_devin_mutex);
  }
}

void
ags_jack_devin_offset_changed(AgsSoundcard *soundcard,
			      guint note_offset)
{
  AgsJackDevin *jack_devin;
  
  GRecMutex *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* offset changed */
  g_rec_mutex_lock(jack_devin_mutex);

  jack_devin->tic_counter += 1;

  if(jack_devin->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    jack_devin->tic_counter = 0;
  }

  g_rec_mutex_unlock(jack_devin_mutex);
}

void
ags_jack_devin_set_bpm(AgsSoundcard *soundcard,
		       gdouble bpm)
{
  AgsJackDevin *jack_devin;

  GRecMutex *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* set bpm */
  g_rec_mutex_lock(jack_devin_mutex);

  jack_devin->bpm = bpm;

  g_rec_mutex_unlock(jack_devin_mutex);

  ags_jack_devin_adjust_delay_and_attack(jack_devin);
}

gdouble
ags_jack_devin_get_bpm(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  gdouble bpm;
  
  GRecMutex *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* get bpm */
  g_rec_mutex_lock(jack_devin_mutex);

  bpm = jack_devin->bpm;
  
  g_rec_mutex_unlock(jack_devin_mutex);

  return(bpm);
}

void
ags_jack_devin_set_delay_factor(AgsSoundcard *soundcard,
				gdouble delay_factor)
{
  AgsJackDevin *jack_devin;

  GRecMutex *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* set delay factor */
  g_rec_mutex_lock(jack_devin_mutex);

  jack_devin->delay_factor = delay_factor;

  g_rec_mutex_unlock(jack_devin_mutex);

  ags_jack_devin_adjust_delay_and_attack(jack_devin);
}

gdouble
ags_jack_devin_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  gdouble delay_factor;
  
  GRecMutex *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* get delay factor */
  g_rec_mutex_lock(jack_devin_mutex);

  delay_factor = jack_devin->delay_factor;
  
  g_rec_mutex_unlock(jack_devin_mutex);

  return(delay_factor);
}

gdouble
ags_jack_devin_get_delay(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint delay_index;
  gdouble delay;
  
  GRecMutex *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* get delay */
  g_rec_mutex_lock(jack_devin_mutex);

  delay_index = jack_devin->tic_counter;

  delay = jack_devin->delay[delay_index];
  
  g_rec_mutex_unlock(jack_devin_mutex);
  
  return(delay);
}

gdouble
ags_jack_devin_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  gdouble absolute_delay;
  
  GRecMutex *jack_devin_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* get absolute delay */
  g_rec_mutex_lock(jack_devin_mutex);

  absolute_delay = (60.0 * (((gdouble) jack_devin->samplerate / (gdouble) jack_devin->buffer_size) / (gdouble) jack_devin->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) jack_devin->delay_factor)));

  g_rec_mutex_unlock(jack_devin_mutex);

  return(absolute_delay);
}

guint
ags_jack_devin_get_attack(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint attack_index;
  guint attack;
  
  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* get attack */
  g_rec_mutex_lock(jack_devin_mutex);

  attack_index = jack_devin->tic_counter;

  attack = jack_devin->attack[attack_index];

  g_rec_mutex_unlock(jack_devin_mutex);
  
  return(attack);
}

void*
ags_jack_devin_get_buffer(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  void *buffer;

  GRecMutex *jack_devin_mutex;  
  
  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  g_rec_mutex_lock(jack_devin_mutex);

  if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_0){
    buffer = jack_devin->app_buffer[0];
  }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_1){
    buffer = jack_devin->app_buffer[1];
  }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_2){
    buffer = jack_devin->app_buffer[2];
  }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_3){
    buffer = jack_devin->app_buffer[3];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(jack_devin_mutex);

  return(buffer);
}

void*
ags_jack_devin_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  void *buffer;

  GRecMutex *jack_devin_mutex;  
  
  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  g_rec_mutex_lock(jack_devin_mutex);

  if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_0){
    buffer = jack_devin->app_buffer[1];
  }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_1){
    buffer = jack_devin->app_buffer[2];
  }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_2){
    buffer = jack_devin->app_buffer[3];
  }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_3){
    buffer = jack_devin->app_buffer[0];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(jack_devin_mutex);

  return(buffer);
}

void*
ags_jack_devin_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  void *buffer;

  GRecMutex *jack_devin_mutex;  
  
  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  g_rec_mutex_lock(jack_devin_mutex);

  if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_0){
    buffer = jack_devin->app_buffer[3];
  }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_1){
    buffer = jack_devin->app_buffer[0];
  }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_2){
    buffer = jack_devin->app_buffer[1];
  }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_3){
    buffer = jack_devin->app_buffer[2];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(jack_devin_mutex);

  return(buffer);
}

void
ags_jack_devin_lock_buffer(AgsSoundcard *soundcard,
			   void *buffer)
{
  AgsJackDevin *jack_devin;

  GRecMutex *buffer_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(jack_devin->app_buffer != NULL){
    if(buffer == jack_devin->app_buffer[0]){
      buffer_mutex = jack_devin->app_buffer_mutex[0];
    }else if(buffer == jack_devin->app_buffer[1]){
      buffer_mutex = jack_devin->app_buffer_mutex[1];
    }else if(buffer == jack_devin->app_buffer[2]){
      buffer_mutex = jack_devin->app_buffer_mutex[2];
    }else if(buffer == jack_devin->app_buffer[3]){
      buffer_mutex = jack_devin->app_buffer_mutex[3];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}


void
ags_jack_devin_unlock_buffer(AgsSoundcard *soundcard,
			     void *buffer)
{
  AgsJackDevin *jack_devin;

  GRecMutex *buffer_mutex;
  
  jack_devin = AGS_JACK_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(jack_devin->app_buffer != NULL){
    if(buffer == jack_devin->app_buffer[0]){
      buffer_mutex = jack_devin->app_buffer_mutex[0];
    }else if(buffer == jack_devin->app_buffer[1]){
      buffer_mutex = jack_devin->app_buffer_mutex[1];
    }else if(buffer == jack_devin->app_buffer[2]){
      buffer_mutex = jack_devin->app_buffer_mutex[2];
    }else if(buffer == jack_devin->app_buffer[3]){
      buffer_mutex = jack_devin->app_buffer_mutex[3];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_jack_devin_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint delay_counter;
  
  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* delay counter */
  g_rec_mutex_lock(jack_devin_mutex);

  delay_counter = jack_devin->delay_counter;
  
  g_rec_mutex_unlock(jack_devin_mutex);

  return(delay_counter);
}

void
ags_jack_devin_set_start_note_offset(AgsSoundcard *soundcard,
				     guint start_note_offset)
{
  AgsJackDevin *jack_devin;

  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* set note offset */
  g_rec_mutex_lock(jack_devin_mutex);

  jack_devin->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(jack_devin_mutex);
}

guint
ags_jack_devin_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint start_note_offset;
  
  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* set note offset */
  g_rec_mutex_lock(jack_devin_mutex);

  start_note_offset = jack_devin->start_note_offset;

  g_rec_mutex_unlock(jack_devin_mutex);

  return(start_note_offset);
}

void
ags_jack_devin_set_note_offset(AgsSoundcard *soundcard,
			       guint note_offset)
{
  AgsJackDevin *jack_devin;

  gdouble note_256th_delay;

  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* set note offset */
  g_rec_mutex_lock(jack_devin_mutex);

  jack_devin->note_offset = note_offset;

  note_256th_delay = jack_devin->note_256th_delay;

  jack_devin->note_256th_offset = 16 * note_offset;

  jack_devin->note_256th_offset_last = jack_devin->note_256th_offset;
  
  if(jack_devin->note_256th_delay < 1.0){
    guint buffer_size;
    guint note_256th_attack_lower, note_256th_attack_upper;
    
    buffer_size = jack_devin->buffer_size;

    note_256th_attack_lower = 0;
    note_256th_attack_upper = 0;
    
    ags_soundcard_get_note_256th_attack(AGS_SOUNDCARD(jack_devin),
					&note_256th_attack_lower,
					&note_256th_attack_upper);

    if(note_256th_attack_lower < note_256th_attack_upper){
      jack_devin->note_256th_offset_last = jack_devin->note_256th_offset + floor((note_256th_attack_upper - note_256th_attack_lower) / (jack_devin->note_256th_delay * (double) buffer_size));
    }
  }
  
  g_rec_mutex_unlock(jack_devin_mutex);
}

guint
ags_jack_devin_get_note_offset(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint note_offset;
  
  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* set note offset */
  g_rec_mutex_lock(jack_devin_mutex);

  note_offset = jack_devin->note_offset;

  g_rec_mutex_unlock(jack_devin_mutex);

  return(note_offset);
}

void
ags_jack_devin_get_note_256th_offset(AgsSoundcard *soundcard,
				     guint *offset_lower,
				     guint *offset_upper)
{
  AgsJackDevin *jack_devin;
  
  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* get note 256th offset */
  g_rec_mutex_lock(jack_devin_mutex);

  if(offset_lower != NULL){
    offset_lower[0] = jack_devin->note_256th_offset;
  }

  if(offset_upper != NULL){
    offset_upper[0] = jack_devin->note_256th_offset_last;
  }

  g_rec_mutex_unlock(jack_devin_mutex);
}

void
ags_jack_devin_get_note_256th_attack(AgsSoundcard *soundcard,
				     guint *note_256th_attack_lower,
				     guint *note_256th_attack_upper)
{
  AgsJackDevin *jack_devin;

  guint *note_256th_attack;

  guint nth_list;
  guint note_256th_attack_position_lower, note_256th_attack_position_upper;
  guint local_note_256th_attack_lower, local_note_256th_attack_upper;
  
  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* get note 256th attack lower and upper */
  ags_soundcard_get_note_256th_attack_position(soundcard,
					       &note_256th_attack_position_lower,
					       &note_256th_attack_position_upper);

  local_note_256th_attack_lower = 0;
  local_note_256th_attack_upper = 0;
  
  g_rec_mutex_lock(jack_devin_mutex);

  nth_list = (guint) floor(note_256th_attack_position_lower / AGS_SOUNDCARD_DEFAULT_PERIOD);
  note_256th_attack = g_list_nth_data(jack_devin->note_256th_attack,
				      nth_list);
  if(note_256th_attack != NULL){
    local_note_256th_attack_lower = note_256th_attack[note_256th_attack_position_lower % (guint) AGS_SOUNDCARD_DEFAULT_PERIOD];
  }
  
  if(note_256th_attack_lower != NULL){
    note_256th_attack_lower[0] = local_note_256th_attack_lower;
  }  

  nth_list = (guint) floor(note_256th_attack_position_upper / AGS_SOUNDCARD_DEFAULT_PERIOD);
  note_256th_attack = g_list_nth_data(jack_devin->note_256th_attack,
				      nth_list);

  if(note_256th_attack != NULL){
    local_note_256th_attack_upper = note_256th_attack[note_256th_attack_position_upper % (guint) AGS_SOUNDCARD_DEFAULT_PERIOD];
  }else{
    local_note_256th_attack_upper = local_note_256th_attack_lower;
  }
  
  if(note_256th_attack_upper != NULL){
    note_256th_attack_upper[0] = local_note_256th_attack_upper;
  }  
    
  g_rec_mutex_unlock(jack_devin_mutex);
}

guint
ags_jack_devin_get_note_256th_attack_at_position(AgsSoundcard *soundcard,
						 guint note_256th_attack_position)
{
  AgsJackDevin *jack_devin;
  
  guint *note_256th_attack;

  guint nth_list;
  guint current_note_256th_attack;

  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* get note 256th attack */
  g_rec_mutex_lock(jack_devin_mutex);
  
  current_note_256th_attack = 0;

  nth_list = (guint) floor(note_256th_attack_position / AGS_SOUNDCARD_DEFAULT_PERIOD);
  note_256th_attack = g_list_nth_data(jack_devin->note_256th_attack,
				      nth_list);

  if(note_256th_attack != NULL){
    current_note_256th_attack = note_256th_attack[note_256th_attack_position % (guint) AGS_SOUNDCARD_DEFAULT_PERIOD];
  }
  
  g_rec_mutex_unlock(jack_devin_mutex);

  return(current_note_256th_attack);
}

void
ags_jack_devin_get_note_256th_attack_position(AgsSoundcard *soundcard,
					      guint *note_256th_attack_position_lower,
					      guint *note_256th_attack_position_upper)
{
  AgsJackDevin *jack_devin;

  guint buffer_size;
  guint attack_position;
  guint local_attack;
  gdouble note_256th_delay;
  guint nth_list;
  guint *local_note_256th_attack;
  guint position_lower, position_upper;
  guint i;
  
  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);
  
  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);
  
  /* get note 256th attack */
  g_rec_mutex_lock(jack_devin_mutex);

  buffer_size = jack_devin->buffer_size;

  attack_position = jack_devin->tic_counter;

  local_attack = jack_devin->attack[attack_position];

  note_256th_delay = jack_devin->note_256th_delay;

  if(1.0 / note_256th_delay >= AGS_SOUNDCARD_DEFAULT_PERIOD){
    g_critical("unexpected time segmentation");
  }

  position_lower = 16 * jack_devin->tic_counter;

  for(i = 1; local_attack - (i * note_256th_delay * buffer_size) >= 0; i++){
    if(position_lower - 1 >= 0){
      position_lower--;
    }
  }
  
  position_upper = position_lower;

  nth_list = (guint) floor(position_lower / AGS_SOUNDCARD_DEFAULT_PERIOD);

  local_note_256th_attack = g_list_nth_data(jack_devin->note_256th_attack,
					    nth_list);

  for(i = 1; local_note_256th_attack[position_lower % (guint) AGS_SOUNDCARD_DEFAULT_PERIOD] + (guint) floor((double) i * note_256th_delay * (double) buffer_size) < buffer_size; i++){
    if((position_upper + 1) % (guint) AGS_SOUNDCARD_DEFAULT_PERIOD == 0){
      if(nth_list + 1 < 32){
	local_note_256th_attack = g_list_nth_data(jack_devin->note_256th_attack,
						  nth_list + 1);
      }else{
	local_note_256th_attack = g_list_nth_data(jack_devin->note_256th_attack,
						  0);
      }
    }

    if(position_upper + 1 < 16 * (guint) AGS_SOUNDCARD_DEFAULT_PERIOD){
      guint prev_note_256th_attack;
      guint current_note_256th_attack;

      prev_note_256th_attack = ags_soundcard_get_note_256th_attack_at_position(soundcard,
									       position_upper);

      current_note_256th_attack = ags_soundcard_get_note_256th_attack_at_position(soundcard,
										  position_upper + 1);

      if(prev_note_256th_attack < current_note_256th_attack){
	position_upper++;
      }
    }
  }
  
  if(note_256th_attack_position_lower != NULL){
    note_256th_attack_position_lower[0] = position_lower;
  }

  if(note_256th_attack_position_upper != NULL){
    note_256th_attack_position_upper[0] = position_upper;
  }
  
  g_rec_mutex_unlock(jack_devin_mutex);
}

guint
ags_jack_devin_get_note_256th_attack_of_16th_pulse(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint note_256th_attack_of_16th_pulse;
  
  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* get note 256th attack of 16th pulse */
  g_rec_mutex_lock(jack_devin_mutex);

  note_256th_attack_of_16th_pulse = jack_devin->note_256th_attack_of_16th_pulse;

  g_rec_mutex_unlock(jack_devin_mutex);

  return(note_256th_attack_of_16th_pulse);
}

guint
ags_jack_devin_get_note_256th_attack_of_16th_pulse_position(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint position;
  
  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* get note 256th attack position of 16th pulse */
  g_rec_mutex_lock(jack_devin_mutex);

  position = jack_devin->note_256th_attack_of_16th_pulse_position;

  g_rec_mutex_unlock(jack_devin_mutex);

  return(position);
}

void
ags_jack_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
					guint note_offset_absolute)
{
  AgsJackDevin *jack_devin;
  
  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* set note offset */
  g_rec_mutex_lock(jack_devin_mutex);

  jack_devin->note_offset_absolute = note_offset_absolute;

  g_rec_mutex_unlock(jack_devin_mutex);
}

guint
ags_jack_devin_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint note_offset_absolute;
  
  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* set note offset */
  g_rec_mutex_lock(jack_devin_mutex);

  note_offset_absolute = jack_devin->note_offset_absolute;

  g_rec_mutex_unlock(jack_devin_mutex);

  return(note_offset_absolute);
}

void
ags_jack_devin_set_loop(AgsSoundcard *soundcard,
			guint loop_left, guint loop_right,
			gboolean do_loop)
{
  AgsJackDevin *jack_devin;

  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* set loop */
  g_rec_mutex_lock(jack_devin_mutex);

  jack_devin->loop_left = loop_left;
  jack_devin->loop_right = loop_right;
  jack_devin->do_loop = do_loop;

  if(do_loop){
    jack_devin->loop_offset = jack_devin->note_offset;
  }

  g_rec_mutex_unlock(jack_devin_mutex);
}

void
ags_jack_devin_get_loop(AgsSoundcard *soundcard,
			guint *loop_left, guint *loop_right,
			gboolean *do_loop)
{
  AgsJackDevin *jack_devin;

  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* get loop */
  g_rec_mutex_lock(jack_devin_mutex);

  if(loop_left != NULL){
    *loop_left = jack_devin->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = jack_devin->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = jack_devin->do_loop;
  }

  g_rec_mutex_unlock(jack_devin_mutex);
}

guint
ags_jack_devin_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsJackDevin *jack_devin;

  guint loop_offset;
  
  GRecMutex *jack_devin_mutex;  

  jack_devin = AGS_JACK_DEVIN(soundcard);

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* get loop offset */
  g_rec_mutex_lock(jack_devin_mutex);

  loop_offset = jack_devin->loop_offset;
  
  g_rec_mutex_unlock(jack_devin_mutex);

  return(loop_offset);
}

/**
 * ags_jack_devin_switch_buffer_flag:
 * @jack_devin: an #AgsJackDevin
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.0.0
 */
void
ags_jack_devin_switch_buffer_flag(AgsJackDevin *jack_devin)
{
  GRecMutex *jack_devin_mutex;
  
  if(!AGS_IS_JACK_DEVIN(jack_devin)){
    return;
  }

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* switch buffer flag */
  g_rec_mutex_lock(jack_devin_mutex);

  if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_0){
    jack_devin->app_buffer_mode = AGS_JACK_DEVIN_APP_BUFFER_1;
  }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_1){
    jack_devin->app_buffer_mode = AGS_JACK_DEVIN_APP_BUFFER_2;
  }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_2){
    jack_devin->app_buffer_mode = AGS_JACK_DEVIN_APP_BUFFER_3;
  }else if(jack_devin->app_buffer_mode == AGS_JACK_DEVIN_APP_BUFFER_3){
    jack_devin->app_buffer_mode = AGS_JACK_DEVIN_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(jack_devin_mutex);
}

/**
 * ags_jack_devin_adjust_delay_and_attack:
 * @jack_devin: the #AgsJackDevin
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 3.0.0
 */
void
ags_jack_devin_adjust_delay_and_attack(AgsJackDevin *jack_devin)
{
  if(!AGS_IS_JACK_DEVIN(jack_devin)){
    return;
  }

  ags_soundcard_util_adjust_delay_and_attack(jack_devin);
}

/**
 * ags_jack_devin_realloc_buffer:
 * @jack_devin: the #AgsJackDevin
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.0.0
 */
void
ags_jack_devin_realloc_buffer(AgsJackDevin *jack_devin)
{
  AgsJackClient *jack_client;
  
  guint port_count;
  guint pcm_channels;
  guint buffer_size;
  guint word_size;

  GRecMutex *jack_devin_mutex;  

  if(!AGS_IS_JACK_DEVIN(jack_devin)){
    return;
  }

  /* get jack devin mutex */
  jack_devin_mutex = AGS_JACK_DEVIN_GET_OBJ_MUTEX(jack_devin);

  /* get word size */  
  g_rec_mutex_lock(jack_devin_mutex);

  jack_client = jack_devin->jack_client;
  
  port_count = g_list_length(jack_devin->jack_port);
  
  pcm_channels = jack_devin->pcm_channels;
  buffer_size = jack_devin->buffer_size;

  switch(jack_devin->format){
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
    g_warning("ags_jack_devin_realloc_buffer(): unsupported word size");
    return;
  }

  g_rec_mutex_unlock(jack_devin_mutex);

  if(port_count < pcm_channels){
    AgsJackPort *jack_port;

    gchar *str;
    
    guint nth_soundcard;
    guint i;

    g_rec_mutex_lock(jack_devin_mutex);

    if(jack_devin->card_uri != NULL){
      sscanf(jack_devin->card_uri,
	     "ags-jack-devin-%u",
	     &nth_soundcard);
    }else{
      g_rec_mutex_unlock(jack_devin_mutex);

      g_warning("ags_jack_devin_realloc_buffer() - card uri not set");
      
      return;
    }

    g_rec_mutex_unlock(jack_devin_mutex);
    
    for(i = port_count; i < pcm_channels; i++){
      str = g_strdup_printf("ags%d-%04d",
			    nth_soundcard,
			    i);
      
      jack_port = ags_jack_port_new(jack_client);
      ags_jack_client_add_port((AgsJackClient *) jack_client,
			       (GObject *) jack_port);

      g_rec_mutex_lock(jack_devin_mutex);
    
      jack_devin->jack_port = g_list_prepend(jack_devin->jack_port,
					     jack_port);
    
      if(jack_devin->port_name == NULL){
	jack_devin->port_name = (gchar **) g_malloc(2 * sizeof(gchar *));
	jack_devin->port_name[0] = g_strdup(str);
      }else{
	jack_devin->port_name = (gchar **) g_realloc(jack_devin->port_name,
						     (i + 2) * sizeof(gchar *));
	jack_devin->port_name[i] = g_strdup(str);
      }
      
      g_rec_mutex_unlock(jack_devin_mutex);
      
      ags_jack_port_register(jack_port,
			     str,
			     TRUE, FALSE,
			     FALSE);
    }

    jack_devin->port_name[jack_devin->pcm_channels] = NULL;    
  }else if(port_count > pcm_channels){
    GList *jack_port_start, *jack_port;

    guint i;

    g_rec_mutex_lock(jack_devin_mutex);

    jack_port =
      jack_port_start = g_list_copy(jack_devin->jack_port);

    g_rec_mutex_unlock(jack_devin_mutex);

    for(i = 0; i < port_count - pcm_channels; i++){
      jack_devin->jack_port = g_list_remove(jack_devin->jack_port,
					    jack_port->data);
      ags_jack_port_unregister(jack_port->data);
      
      g_object_unref(jack_port->data);
      
      jack_port = jack_port->next;
    }

    g_list_free(jack_port_start);

    g_rec_mutex_lock(jack_devin_mutex);
    
    jack_devin->port_name = (gchar **) g_realloc(jack_devin->port_name,
						 (jack_devin->pcm_channels + 1) * sizeof(gchar *));
    jack_devin->port_name[jack_devin->pcm_channels] = NULL;

    g_rec_mutex_unlock(jack_devin_mutex);
  }
  
  /* AGS_JACK_DEVIN_BUFFER_0 */
  if(jack_devin->app_buffer[0] != NULL){
    g_free(jack_devin->app_buffer[0]);
  }
  
  jack_devin->app_buffer[0] = (void *) g_malloc(jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  
  /* AGS_JACK_DEVIN_BUFFER_1 */
  if(jack_devin->app_buffer[1] != NULL){
    g_free(jack_devin->app_buffer[1]);
  }

  jack_devin->app_buffer[1] = (void *) g_malloc(jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  
  /* AGS_JACK_DEVIN_BUFFER_2 */
  if(jack_devin->app_buffer[2] != NULL){
    g_free(jack_devin->app_buffer[2]);
  }

  jack_devin->app_buffer[2] = (void *) g_malloc(jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
  
  /* AGS_JACK_DEVIN_BUFFER_3 */
  if(jack_devin->app_buffer[3] != NULL){
    g_free(jack_devin->app_buffer[3]);
  }
  
  jack_devin->app_buffer[3] = (void *) g_malloc(jack_devin->pcm_channels * jack_devin->buffer_size * word_size);
}

/**
 * ags_jack_devin_new:
 *
 * Creates a new instance of #AgsJackDevin.
 *
 * Returns: the new #AgsJackDevin
 *
 * Since: 3.0.0
 */
AgsJackDevin*
ags_jack_devin_new()
{
  AgsJackDevin *jack_devin;

  jack_devin = (AgsJackDevin *) g_object_new(AGS_TYPE_JACK_DEVIN,
					     NULL);
  
  return(jack_devin);
}
