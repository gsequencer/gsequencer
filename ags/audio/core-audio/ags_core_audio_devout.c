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

#include <ags/audio/core-audio/ags_core_audio_devout.h>

#include <ags/ags_api_config.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_soundcard_util.h>

#include <ags/audio/core-audio/ags_core_audio_server.h>
#include <ags/audio/core-audio/ags_core_audio_client.h>
#include <ags/audio/core-audio/ags_core_audio_port.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <ags/audio/thread/ags_audio_loop.h>

#if defined(AGS_WITH_CORE_AUDIO)  
#include <AudioToolbox/AudioToolbox.h>

#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <AudioUnit/AudioComponent.h>

#include <Foundation/Foundation.h>
#include <CoreFoundation/CoreFoundation.h>
#endif

#include <string.h>
#include <math.h>
#include <time.h>

#include <ags/i18n.h>

#define AGS_CORE_AUDIO_PORT_USE_HW (1)

void ags_core_audio_devout_class_init(AgsCoreAudioDevoutClass *core_audio_devout);
void ags_core_audio_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_core_audio_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_core_audio_devout_init(AgsCoreAudioDevout *core_audio_devout);
void ags_core_audio_devout_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_devout_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_core_audio_devout_dispose(GObject *gobject);
void ags_core_audio_devout_finalize(GObject *gobject);

AgsUUID* ags_core_audio_devout_get_uuid(AgsConnectable *connectable);
gboolean ags_core_audio_devout_has_resource(AgsConnectable *connectable);
gboolean ags_core_audio_devout_is_ready(AgsConnectable *connectable);
void ags_core_audio_devout_add_to_registry(AgsConnectable *connectable);
void ags_core_audio_devout_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_core_audio_devout_list_resource(AgsConnectable *connectable);
xmlNode* ags_core_audio_devout_xml_compose(AgsConnectable *connectable);
void ags_core_audio_devout_xml_parse(AgsConnectable *connectable,
				     xmlNode *node);
gboolean ags_core_audio_devout_is_connected(AgsConnectable *connectable);
void ags_core_audio_devout_connect(AgsConnectable *connectable);
void ags_core_audio_devout_disconnect(AgsConnectable *connectable);

void ags_core_audio_devout_set_device(AgsSoundcard *soundcard,
				      gchar *device);
gchar* ags_core_audio_devout_get_device(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_presets(AgsSoundcard *soundcard,
				       guint channels,
				       guint rate,
				       guint buffer_size,
				       guint format);
void ags_core_audio_devout_get_presets(AgsSoundcard *soundcard,
				       guint *channels,
				       guint *rate,
				       guint *buffer_size,
				       guint *format);

void ags_core_audio_devout_list_cards(AgsSoundcard *soundcard,
				      GList **card_id, GList **card_name);
void ags_core_audio_devout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
				    guint *channels_min, guint *channels_max,
				    guint *rate_min, guint *rate_max,
				    guint *buffer_size_min, guint *buffer_size_max,
				    GError **error);
guint ags_core_audio_devout_get_capability(AgsSoundcard *soundcard);

gboolean ags_core_audio_devout_is_starting(AgsSoundcard *soundcard);
gboolean ags_core_audio_devout_is_playing(AgsSoundcard *soundcard);

gchar* ags_core_audio_devout_get_uptime(AgsSoundcard *soundcard);

void ags_core_audio_devout_port_init(AgsSoundcard *soundcard,
				     GError **error);
void ags_core_audio_devout_port_play(AgsSoundcard *soundcard,
				     GError **error);
void ags_core_audio_devout_port_free(AgsSoundcard *soundcard);

void ags_core_audio_devout_tic(AgsSoundcard *soundcard);
void ags_core_audio_devout_offset_changed(AgsSoundcard *soundcard,
					  guint note_offset);

void ags_core_audio_devout_set_bpm(AgsSoundcard *soundcard,
				   gdouble bpm);
gdouble ags_core_audio_devout_get_bpm(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_delay_factor(AgsSoundcard *soundcard,
					    gdouble delay_factor);
gdouble ags_core_audio_devout_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_core_audio_devout_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_core_audio_devout_get_delay(AgsSoundcard *soundcard);
guint ags_core_audio_devout_get_attack(AgsSoundcard *soundcard);

void* ags_core_audio_devout_get_buffer(AgsSoundcard *soundcard);
void* ags_core_audio_devout_get_next_buffer(AgsSoundcard *soundcard);
void* ags_core_audio_devout_get_prev_buffer(AgsSoundcard *soundcard);

void ags_core_audio_devout_lock_buffer(AgsSoundcard *soundcard,
				       void *buffer);
void ags_core_audio_devout_unlock_buffer(AgsSoundcard *soundcard,
					 void *buffer);

guint ags_core_audio_devout_get_delay_counter(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_start_note_offset(AgsSoundcard *soundcard,
						 guint start_note_offset);
guint ags_core_audio_devout_get_start_note_offset(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_note_offset(AgsSoundcard *soundcard,
					   guint note_offset);
guint ags_core_audio_devout_get_note_offset(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
						    guint note_offset);
guint ags_core_audio_devout_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_core_audio_devout_set_loop(AgsSoundcard *soundcard,
				    guint loop_left, guint loop_right,
				    gboolean do_loop);
void ags_core_audio_devout_get_loop(AgsSoundcard *soundcard,
				    guint *loop_left, guint *loop_right,
				    gboolean *do_loop);

guint ags_core_audio_devout_get_loop_offset(AgsSoundcard *soundcard);

guint ags_core_audio_devout_get_sub_block_count(AgsSoundcard *soundcard);

gboolean ags_core_audio_devout_trylock_sub_block(AgsSoundcard *soundcard,
						 void *buffer, guint sub_block);
void ags_core_audio_devout_unlock_sub_block(AgsSoundcard *soundcard,
					    void *buffer, guint sub_block);

void ags_core_audio_devout_get_note_256th_offset(AgsSoundcard *soundcard,
						 guint *note_256th_offset_lower,
						 guint *note_256th_offset_upper);

void ags_core_audio_devout_get_note_256th_attack(AgsSoundcard *soundcard,
						 guint *note_256th_attack_lower,
						 guint *note_256th_attack_upper);

guint ags_core_audio_devout_get_note_256th_attack_at_position(AgsSoundcard *soundcard,
							      guint note_256th_attack_position);

void ags_core_audio_devout_get_note_256th_attack_position(AgsSoundcard *soundcard,
							  guint *note_256th_attack_position_lower,
							  guint *note_256th_attack_position_upper);

guint ags_core_audio_devout_get_note_256th_attack_of_16th_pulse(AgsSoundcard *soundcard);
guint ags_core_audio_devout_get_note_256th_attack_of_16th_pulse_position(AgsSoundcard *soundcard);

/**
 * SECTION:ags_core_audio_devout
 * @short_description: Output to soundcard
 * @title: AgsCoreAudioDevout
 * @section_id:
 * @include: ags/audio/core-audio/ags_core_audio_devout.h
 *
 * #AgsCoreAudioDevout represents a soundcard and supports output.
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
  PROP_CORE_AUDIO_CLIENT,
  PROP_CORE_AUDIO_PORT,
  PROP_CHANNEL,
};

static gpointer ags_core_audio_devout_parent_class = NULL;

GType
ags_core_audio_devout_get_type (void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_core_audio_devout = 0;
 
    static const GTypeInfo ags_core_audio_devout_info = {
      sizeof(AgsCoreAudioDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_core_audio_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsCoreAudioDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_core_audio_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_core_audio_devout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_core_audio_devout = g_type_register_static(G_TYPE_OBJECT,
							"AgsCoreAudioDevout",
							&ags_core_audio_devout_info,
							0);

    g_type_add_interface_static(ags_type_core_audio_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_core_audio_devout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_core_audio_devout);
  }

  return(g_define_type_id__static);
}

GType
ags_core_audio_devout_flags_get_type()
{
  static gsize g_flags_type_id__static;

  if(g_once_init_enter(&g_flags_type_id__static)){
    static const GFlagsValue values[] = {
      { AGS_CORE_AUDIO_DEVOUT_INITIALIZED, "AGS_CORE_AUDIO_DEVOUT_INITIALIZED", "core-audio-devout-initialized" },
      { AGS_CORE_AUDIO_DEVOUT_START_PLAY, "AGS_CORE_AUDIO_DEVOUT_START_PLAY", "core-audio-devout-start-play" },
      { AGS_CORE_AUDIO_DEVOUT_PLAY, "AGS_CORE_AUDIO_DEVOUT_PLAY", "core-audio-devout-play" },
      { AGS_CORE_AUDIO_DEVOUT_SHUTDOWN, "AGS_CORE_AUDIO_DEVOUT_SHUTDOWN", "core-audio-devout-shutdown" },
      { AGS_CORE_AUDIO_DEVOUT_NONBLOCKING, "AGS_CORE_AUDIO_DEVOUT_NONBLOCKING", "core-audio-devout-nonblocking" },
      { AGS_CORE_AUDIO_DEVOUT_ATTACK_FIRST, "AGS_CORE_AUDIO_DEVOUT_ATTACK_FIRST", "core-audio-devout-attack-first" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsCoreAudioDevoutFlags"), values);

    g_once_init_leave(&g_flags_type_id__static, g_flags_type_id);
  }
  
  return(g_flags_type_id__static);
}

void
ags_core_audio_devout_class_init(AgsCoreAudioDevoutClass *core_audio_devout)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_core_audio_devout_parent_class = g_type_class_peek_parent(core_audio_devout);

  /* GObjectClass */
  gobject = (GObjectClass *) core_audio_devout;

  gobject->set_property = ags_core_audio_devout_set_property;
  gobject->get_property = ags_core_audio_devout_get_property;

  gobject->dispose = ags_core_audio_devout_dispose;
  gobject->finalize = ags_core_audio_devout_finalize;

  /* properties */
  /**
   * AgsCoreAudioDevout:device:
   *
   * The core audio soundcard indentifier
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-core-audio-devout-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsCoreAudioDevout:dsp-channels:
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
   * AgsCoreAudioDevout:pcm-channels:
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
   * AgsCoreAudioDevout:format:
   *
   * The precision of the buffer
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("format",
				 i18n_pspec("precision of buffer"),
				 i18n_pspec("The precision to use for a frame"),
				 1,
				 G_MAXUINT,
				 AGS_SOUNDCARD_FLOAT,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FORMAT,
				  param_spec);

  /**
   * AgsCoreAudioDevout:buffer-size:
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
   * AgsCoreAudioDevout:samplerate:
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
   * AgsCoreAudioDevout:buffer:
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
   * AgsCoreAudioDevout:bpm:
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
   * AgsCoreAudioDevout:delay-factor:
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
   * AgsCoreAudioDevout:attack:
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
   * AgsCoreAudioDevout:core-audio-client:
   *
   * The assigned #AgsCoreAudioClient
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("core-audio-client",
				   i18n_pspec("core audio client object"),
				   i18n_pspec("The core audio client object"),
				   AGS_TYPE_CORE_AUDIO_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CORE_AUDIO_CLIENT,
				  param_spec);

  /**
   * AgsCoreAudioDevout:core-audio-port:
   *
   * The assigned #AgsCoreAudioPort
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("core-audio-port",
				    i18n_pspec("core audio port object"),
				    i18n_pspec("The core audio port object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CORE_AUDIO_PORT,
				  param_spec);
}

GQuark
ags_core_audio_devout_error_quark()
{
  return(g_quark_from_static_string("ags-core_audio_devout-error-quark"));
}

void
ags_core_audio_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_core_audio_devout_get_uuid;
  connectable->has_resource = ags_core_audio_devout_has_resource;

  connectable->is_ready = ags_core_audio_devout_is_ready;
  connectable->add_to_registry = ags_core_audio_devout_add_to_registry;
  connectable->remove_from_registry = ags_core_audio_devout_remove_from_registry;

  connectable->list_resource = ags_core_audio_devout_list_resource;
  connectable->xml_compose = ags_core_audio_devout_xml_compose;
  connectable->xml_parse = ags_core_audio_devout_xml_parse;

  connectable->is_connected = ags_core_audio_devout_is_connected;  
  connectable->connect = ags_core_audio_devout_connect;
  connectable->disconnect = ags_core_audio_devout_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_core_audio_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_core_audio_devout_set_device;
  soundcard->get_device = ags_core_audio_devout_get_device;
  
  soundcard->set_presets = ags_core_audio_devout_set_presets;
  soundcard->get_presets = ags_core_audio_devout_get_presets;

  soundcard->list_cards = ags_core_audio_devout_list_cards;
  soundcard->pcm_info = ags_core_audio_devout_pcm_info;
  soundcard->get_capability = ags_core_audio_devout_get_capability;

  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_core_audio_devout_is_starting;
  soundcard->is_playing = ags_core_audio_devout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_core_audio_devout_get_uptime;
  
  soundcard->play_init = ags_core_audio_devout_port_init;
  soundcard->play = ags_core_audio_devout_port_play;

  soundcard->record_init = NULL;
  soundcard->record = NULL;

  soundcard->stop = ags_core_audio_devout_port_free;

  soundcard->tic = ags_core_audio_devout_tic;
  soundcard->offset_changed = ags_core_audio_devout_offset_changed;
    
  soundcard->set_bpm = ags_core_audio_devout_set_bpm;
  soundcard->get_bpm = ags_core_audio_devout_get_bpm;

  soundcard->set_delay_factor = ags_core_audio_devout_set_delay_factor;
  soundcard->get_delay_factor = ags_core_audio_devout_get_delay_factor;
  
  soundcard->get_absolute_delay = ags_core_audio_devout_get_absolute_delay;

  soundcard->get_delay = ags_core_audio_devout_get_delay;
  soundcard->get_attack = ags_core_audio_devout_get_attack;

  soundcard->get_buffer = ags_core_audio_devout_get_buffer;
  soundcard->get_next_buffer = ags_core_audio_devout_get_next_buffer;
  soundcard->get_prev_buffer = ags_core_audio_devout_get_prev_buffer;

  soundcard->lock_buffer = ags_core_audio_devout_lock_buffer;
  soundcard->unlock_buffer = ags_core_audio_devout_unlock_buffer;

  soundcard->get_delay_counter = ags_core_audio_devout_get_delay_counter;

  soundcard->set_start_note_offset = ags_core_audio_devout_set_start_note_offset;
  soundcard->get_start_note_offset = ags_core_audio_devout_get_start_note_offset;

  soundcard->set_note_offset = ags_core_audio_devout_set_note_offset;
  soundcard->get_note_offset = ags_core_audio_devout_get_note_offset;

  soundcard->set_note_offset_absolute = ags_core_audio_devout_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_core_audio_devout_get_note_offset_absolute;

  soundcard->set_loop = ags_core_audio_devout_set_loop;
  soundcard->get_loop = ags_core_audio_devout_get_loop;

  soundcard->get_loop_offset = ags_core_audio_devout_get_loop_offset;

  soundcard->get_sub_block_count = ags_core_audio_devout_get_sub_block_count;

  soundcard->trylock_sub_block = ags_core_audio_devout_trylock_sub_block;
  soundcard->unlock_sub_block = ags_core_audio_devout_unlock_sub_block;

  soundcard->get_note_256th_offset = ags_core_audio_devout_get_note_256th_offset;

  soundcard->get_note_256th_attack = ags_core_audio_devout_get_note_256th_attack;

  soundcard->get_note_256th_attack_at_position = ags_core_audio_devout_get_note_256th_attack_at_position;

  soundcard->get_note_256th_attack_position = ags_core_audio_devout_get_note_256th_attack_position;

  soundcard->get_note_256th_attack_of_16th_pulse = ags_core_audio_devout_get_note_256th_attack_of_16th_pulse;
  soundcard->get_note_256th_attack_of_16th_pulse_position = ags_core_audio_devout_get_note_256th_attack_of_16th_pulse_position;
}

void
ags_core_audio_devout_init(AgsCoreAudioDevout *core_audio_devout)
{
  AgsConfig *config;

  GList *start_note_256th_attack;

  gchar *str;
  gchar *segmentation;

  gdouble absolute_delay;
  guint denominator, numerator;
  guint i;
  
  /* flags */
  core_audio_devout->flags = 0;
  core_audio_devout->connectable_flags = 0;
  ags_atomic_int_set(&(core_audio_devout->sync_flags),
		   AGS_CORE_AUDIO_DEVOUT_PASS_THROUGH);

  /* devout mutex */
  g_rec_mutex_init(&(core_audio_devout->obj_mutex));

  /* uuid */
  core_audio_devout->uuid = ags_uuid_alloc();
  ags_uuid_generate(core_audio_devout->uuid);

  /* presets */
  config = ags_config_get_instance();

  core_audio_devout->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  core_audio_devout->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  core_audio_devout->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  core_audio_devout->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  core_audio_devout->format = AGS_SOUNDCARD_FLOAT;

  /* buffer */
  core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0;

  core_audio_devout->app_buffer_mutex = (GRecMutex **) g_malloc(8 * sizeof(GRecMutex *));

  for(i = 0; i < 8; i++){
    core_audio_devout->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(core_audio_devout->app_buffer_mutex[i]);
  }
  
  core_audio_devout->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  core_audio_devout->sub_block_mutex = (GRecMutex **) g_malloc(8 * core_audio_devout->sub_block_count * core_audio_devout->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < 8 * core_audio_devout->sub_block_count * core_audio_devout->pcm_channels; i++){
    core_audio_devout->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(core_audio_devout->sub_block_mutex[i]);
  }

  core_audio_devout->app_buffer = (void **) g_malloc(8 * sizeof(void*));

  core_audio_devout->app_buffer[0] = NULL;
  core_audio_devout->app_buffer[1] = NULL;
  core_audio_devout->app_buffer[2] = NULL;
  core_audio_devout->app_buffer[3] = NULL;
  core_audio_devout->app_buffer[4] = NULL;
  core_audio_devout->app_buffer[5] = NULL;
  core_audio_devout->app_buffer[6] = NULL;
  core_audio_devout->app_buffer[7] = NULL;
  
  ags_core_audio_devout_realloc_buffer(core_audio_devout);
  
  /* bpm */
  core_audio_devout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  core_audio_devout->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    core_audio_devout->delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }

  /* delay and attack */
  absolute_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(core_audio_devout));
  
  core_audio_devout->delay = (gdouble *) g_malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD * sizeof(gdouble));
  
  core_audio_devout->attack = (guint *) g_malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD * sizeof(guint));

  core_audio_devout->note_256th_delay = absolute_delay / 16.0;

  core_audio_devout->note_256th_attack_of_16th_pulse = 0;
  core_audio_devout->note_256th_attack_of_16th_pulse_position = 0;

  start_note_256th_attack = NULL;

  for(i = 0; i < 32; i++){
    guint *note_256th_attack;
    
    note_256th_attack = (guint *) g_malloc((int) AGS_SOUNDCARD_DEFAULT_PERIOD * sizeof(guint));
    
    start_note_256th_attack = g_list_prepend(start_note_256th_attack,
					     note_256th_attack);
  }

  core_audio_devout->note_256th_attack = start_note_256th_attack;

  ags_core_audio_devout_adjust_delay_and_attack(core_audio_devout);
  
  /* counters */
  core_audio_devout->tact_counter = 0.0;
  core_audio_devout->delay_counter = 0.0;
  core_audio_devout->tic_counter = 0;

  core_audio_devout->start_note_offset = 0;
  core_audio_devout->note_offset = 0;
  core_audio_devout->note_offset_absolute = 0;

  core_audio_devout->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  core_audio_devout->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  core_audio_devout->do_loop = FALSE;

  core_audio_devout->loop_offset = 0;

  /*  */
  core_audio_devout->card_uri = NULL;
  core_audio_devout->core_audio_client = NULL;

  core_audio_devout->port_name = NULL;
  core_audio_devout->core_audio_port = NULL;

  /* callback mutex */
  g_mutex_init(&(core_audio_devout->callback_mutex));

  g_cond_init(&(core_audio_devout->callback_cond));

  /* callback finish mutex */
  g_mutex_init(&(core_audio_devout->callback_finish_mutex));

  g_cond_init(&(core_audio_devout->callback_finish_cond));

  /* 256th */
  //NOTE:JK: note_256th_delay was prior set

  //NOTE:JK: note_256th_attack was prior set
  
  core_audio_devout->note_256th_offset = 0;

  if(core_audio_devout->note_256th_delay >= 1.0){
    core_audio_devout->note_256th_offset_last = 0;
  }else{
    core_audio_devout->note_256th_offset_last = (guint) floor(1.0 / core_audio_devout->note_256th_delay);
  }

  /* audio device */
  core_audio_devout->device_id = NULL;
  core_audio_devout->device_name = NULL;

  core_audio_devout->audio_device = 0;
}

void
ags_core_audio_devout_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(gobject);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      ags_soundcard_set_device(AGS_SOUNDCARD(core_audio_devout),
			       device);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      g_rec_mutex_lock(core_audio_devout_mutex);

      if(dsp_channels == core_audio_devout->dsp_channels){
	g_rec_mutex_unlock(core_audio_devout_mutex);

	return;
      }

      core_audio_devout->dsp_channels = dsp_channels;

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      GList *port;

      guint pcm_channels, old_pcm_channels;
      guint i;

      pcm_channels = g_value_get_uint(value);

      g_rec_mutex_lock(core_audio_devout_mutex);

      if(pcm_channels == core_audio_devout->pcm_channels){
	g_rec_mutex_unlock(core_audio_devout_mutex);

	return;
      }

      old_pcm_channels = core_audio_devout->pcm_channels;

      /* destroy if less pcm-channels */
      for(i = 8 * core_audio_devout->sub_block_count * pcm_channels; i < 8 * core_audio_devout->sub_block_count * old_pcm_channels; i++){
	g_rec_mutex_clear(core_audio_devout->sub_block_mutex[i]);

	g_free(core_audio_devout->sub_block_mutex[i]);
      }

      core_audio_devout->sub_block_mutex = (GRecMutex **) g_realloc(core_audio_devout->sub_block_mutex,
								    8 * core_audio_devout->sub_block_count * pcm_channels * sizeof(GRecMutex *));

      /* create if more pcm-channels */
      for(i = 8 * core_audio_devout->sub_block_count * old_pcm_channels; i < 8 * core_audio_devout->sub_block_count * pcm_channels; i++){
	core_audio_devout->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

	g_rec_mutex_init(core_audio_devout->sub_block_mutex[i]);
      }

      core_audio_devout->pcm_channels = pcm_channels;

      g_rec_mutex_unlock(core_audio_devout_mutex);

#if 0
      port = core_audio_devout->core_audio_port;

      while(port != NULL){
	g_object_set(port->data,
		     "pcm-channels", pcm_channels,
		     NULL);

	port = port->next;
      }
#endif

      ags_core_audio_devout_realloc_buffer(core_audio_devout);
    }
    break;
  case PROP_FORMAT:
    {
      GList *port;

      guint format;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(core_audio_devout_mutex);

      if(format == core_audio_devout->format){
	g_rec_mutex_unlock(core_audio_devout_mutex);

	return;
      }

      core_audio_devout->format = format;

      g_rec_mutex_unlock(core_audio_devout_mutex);

#if 0
      port = core_audio_devout->core_audio_port;

      while(port != NULL){
	g_object_set(port->data,
		     "format", format,
		     NULL);

	port = port->next;
      }
#endif

      ags_core_audio_devout_realloc_buffer(core_audio_devout);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      GList *port;

      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      g_rec_mutex_lock(core_audio_devout_mutex);

      if(buffer_size == core_audio_devout->buffer_size){
	g_rec_mutex_unlock(core_audio_devout_mutex);

	return;
      }

      core_audio_devout->buffer_size = buffer_size;

      g_rec_mutex_unlock(core_audio_devout_mutex);

#if 0
      port = core_audio_devout->core_audio_port;

      while(port != NULL){
	g_object_set(port->data,
		     "buffer-size", buffer_size,
		     NULL);

	port = port->next;
      }
#endif

      ags_core_audio_devout_realloc_buffer(core_audio_devout);
      ags_core_audio_devout_adjust_delay_and_attack(core_audio_devout);
    }
    break;
  case PROP_SAMPLERATE:
    {
      GList *port;

      guint samplerate;

      samplerate = g_value_get_uint(value);

      g_rec_mutex_lock(core_audio_devout_mutex);
      
      if(samplerate == core_audio_devout->samplerate){
	g_rec_mutex_unlock(core_audio_devout_mutex);

	return;
      }

      core_audio_devout->samplerate = samplerate;

      g_rec_mutex_unlock(core_audio_devout_mutex);

#if 0
      port = core_audio_devout->core_audio_port;

      while(port != NULL){
	g_object_set(port->data,
		     "samplerate", samplerate,
		     NULL);

	port = port->next;
      }
#endif
      
      ags_core_audio_devout_realloc_buffer(core_audio_devout);
      ags_core_audio_devout_adjust_delay_and_attack(core_audio_devout);
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

      g_rec_mutex_lock(core_audio_devout_mutex);

      core_audio_devout->bpm = bpm;

      g_rec_mutex_unlock(core_audio_devout_mutex);

      ags_core_audio_devout_adjust_delay_and_attack(core_audio_devout);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      g_rec_mutex_lock(core_audio_devout_mutex);

      core_audio_devout->delay_factor = delay_factor;

      g_rec_mutex_unlock(core_audio_devout_mutex);

      ags_core_audio_devout_adjust_delay_and_attack(core_audio_devout);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      AgsCoreAudioClient *core_audio_client;

      core_audio_client = (AgsCoreAudioClient *) g_value_get_object(value);

      g_rec_mutex_lock(core_audio_devout_mutex);

      if(core_audio_devout->core_audio_client == (GObject *) core_audio_client){
	g_rec_mutex_unlock(core_audio_devout_mutex);

	return;
      }

      if(core_audio_devout->core_audio_client != NULL){
	g_object_unref(G_OBJECT(core_audio_devout->core_audio_client));
      }

      if(core_audio_client != NULL){
	g_object_ref(core_audio_client);
      }
      
      core_audio_devout->core_audio_client = (GObject *) core_audio_client;

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      AgsCoreAudioPort *core_audio_port;

      core_audio_port = (AgsCoreAudioPort *) g_value_get_pointer(value);

      g_rec_mutex_lock(core_audio_devout_mutex);

      if(!AGS_IS_CORE_AUDIO_PORT(core_audio_port) ||
	 g_list_find(core_audio_devout->core_audio_port, core_audio_port) != NULL){
	g_rec_mutex_unlock(core_audio_devout_mutex);

	return;
      }

      g_object_ref(core_audio_port);
      core_audio_devout->core_audio_port = g_list_append(core_audio_devout->core_audio_port,
							 core_audio_port);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_devout_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(gobject);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_string(value, core_audio_devout->device_name);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_uint(value, core_audio_devout->dsp_channels);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_uint(value, core_audio_devout->pcm_channels);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_uint(value, core_audio_devout->format);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_uint(value, core_audio_devout->buffer_size);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_uint(value, core_audio_devout->samplerate);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_pointer(value, core_audio_devout->app_buffer);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_double(value, core_audio_devout->bpm);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_double(value, core_audio_devout->delay_factor);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_ATTACK:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_pointer(value, core_audio_devout->attack);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_CORE_AUDIO_CLIENT:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_object(value, core_audio_devout->core_audio_client);

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  case PROP_CORE_AUDIO_PORT:
    {
      g_rec_mutex_lock(core_audio_devout_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(core_audio_devout->core_audio_port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_core_audio_devout_dispose(GObject *gobject)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(gobject);

  /* core audio client */
  if(core_audio_devout->core_audio_client != NULL){
    g_object_unref(core_audio_devout->core_audio_client);

    core_audio_devout->core_audio_client = NULL;
  }

  /* core audio port */
  g_list_free_full(core_audio_devout->core_audio_port,
		   g_object_unref);

  core_audio_devout->core_audio_port = NULL;

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_devout_parent_class)->dispose(gobject);
}

void
ags_core_audio_devout_finalize(GObject *gobject)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint i;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(gobject);

  ags_uuid_free(core_audio_devout->uuid);

  for(i = 0; i < 8; i++){
    g_rec_mutex_clear(core_audio_devout->app_buffer_mutex[i]);

    g_free(core_audio_devout->app_buffer_mutex[i]);
  }
  
  g_free(core_audio_devout->app_buffer_mutex);
  
  for(i = 0; i < 8 * core_audio_devout->sub_block_count * core_audio_devout->pcm_channels; i++){
    g_rec_mutex_clear(core_audio_devout->sub_block_mutex[i]);

    g_free(core_audio_devout->sub_block_mutex[i]);
  }

  g_free(core_audio_devout->sub_block_mutex);

  /* free output buffer */
  g_free(core_audio_devout->app_buffer[0]);
  g_free(core_audio_devout->app_buffer[1]);
  g_free(core_audio_devout->app_buffer[2]);
  g_free(core_audio_devout->app_buffer[3]);
  g_free(core_audio_devout->app_buffer[4]);
  g_free(core_audio_devout->app_buffer[5]);
  g_free(core_audio_devout->app_buffer[6]);
  g_free(core_audio_devout->app_buffer[7]);

  /* free buffer array */
  g_free(core_audio_devout->app_buffer);

  g_free(core_audio_devout->delay);
  g_free(core_audio_devout->attack);

  /* core audio client */
  if(core_audio_devout->core_audio_client != NULL){
    g_object_unref(core_audio_devout->core_audio_client);
  }

  /* core audio port */
  g_list_free_full(core_audio_devout->core_audio_port,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_core_audio_devout_parent_class)->finalize(gobject);
}

AgsUUID*
ags_core_audio_devout_get_uuid(AgsConnectable *connectable)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  AgsUUID *ptr;

  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(connectable);

  /* get core audio devout signal mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get UUID */
  g_rec_mutex_lock(core_audio_devout_mutex);

  ptr = core_audio_devout->uuid;

  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  return(ptr);
}

gboolean
ags_core_audio_devout_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_core_audio_devout_is_ready(AgsConnectable *connectable)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  gboolean is_ready;

  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(connectable);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* check is ready */
  g_rec_mutex_lock(core_audio_devout_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (core_audio_devout->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  return(is_ready);
}

void
ags_core_audio_devout_add_to_registry(AgsConnectable *connectable)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(connectable);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}

void
ags_core_audio_devout_remove_from_registry(AgsConnectable *connectable)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(connectable);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}

xmlNode*
ags_core_audio_devout_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_core_audio_devout_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_core_audio_devout_xml_parse(AgsConnectable *connectable,
				xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_core_audio_devout_is_connected(AgsConnectable *connectable)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  gboolean is_connected;

  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(connectable);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* check is connected */
  g_rec_mutex_lock(core_audio_devout_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (core_audio_devout->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  return(is_connected);
}

void
ags_core_audio_devout_connect(AgsConnectable *connectable)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(connectable);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}

void
ags_core_audio_devout_disconnect(AgsConnectable *connectable)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(connectable);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}

/**
 * ags_core_audio_devout_test_flags:
 * @core_audio_devout: the #AgsCoreAudioDevout
 * @flags: the flags
 *
 * Test @flags to be set on @core_audio_devout.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_core_audio_devout_test_flags(AgsCoreAudioDevout *core_audio_devout, AgsCoreAudioDevoutFlags flags)
{
  gboolean retval;  
  
  GRecMutex *core_audio_devout_mutex;

  if(!AGS_IS_CORE_AUDIO_DEVOUT(core_audio_devout)){
    return(FALSE);
  }

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* test */
  g_rec_mutex_lock(core_audio_devout_mutex);

  retval = (flags & (core_audio_devout->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(retval);
}

/**
 * ags_core_audio_devout_set_flags:
 * @core_audio_devout: the #AgsCoreAudioDevout
 * @flags: see #AgsCoreAudioDevoutFlags-enum
 *
 * Enable a feature of @core_audio_devout.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_devout_set_flags(AgsCoreAudioDevout *core_audio_devout, AgsCoreAudioDevoutFlags flags)
{
  GRecMutex *core_audio_devout_mutex;

  if(!AGS_IS_CORE_AUDIO_DEVOUT(core_audio_devout)){
    return;
  }

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->flags |= flags;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}
    
/**
 * ags_core_audio_devout_unset_flags:
 * @core_audio_devout: the #AgsCoreAudioDevout
 * @flags: see #AgsCoreAudioDevoutFlags-enum
 *
 * Disable a feature of @core_audio_devout.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_devout_unset_flags(AgsCoreAudioDevout *core_audio_devout, AgsCoreAudioDevoutFlags flags)
{  
  GRecMutex *core_audio_devout_mutex;

  if(!AGS_IS_CORE_AUDIO_DEVOUT(core_audio_devout)){
    return;
  }

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->flags &= (~flags);
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}

void
ags_core_audio_devout_set_device(AgsSoundcard *soundcard,
				 gchar *device)
{
  AgsCoreAudioDevout *core_audio_devout;
  
#if defined(AGS_WITH_CORE_AUDIO)
  AudioDeviceID *audio_devices;
  
  NSString *device_uid = @"";

  AudioObjectPropertyAddress devices_property_address;
  AudioObjectPropertyAddress streams_property_address;
  
  struct AudioStreamBasicDescription stream_desc;
#endif
  
  GList *core_audio_port, *start_core_audio_port;

  gchar *str;
  
#if defined(AGS_WITH_CORE_AUDIO)
  int device_count;
  int stream_count;
  int is_mic;
  int is_speaker;
  UInt32 prop_size;
  int i;
  OSStatus error;
#endif

  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* check device */
  g_rec_mutex_lock(core_audio_devout_mutex);

  if(core_audio_devout->device_name == device ||
     (core_audio_devout->device_name != NULL &&
      !g_ascii_strcasecmp(core_audio_devout->device_name,
			  device))){
    g_rec_mutex_unlock(core_audio_devout_mutex);
  
    return;
  }

  g_message("output set device - %s", device);
  
  /* get some fields */
  start_core_audio_port = 
    core_audio_port = g_list_copy(core_audio_devout->core_audio_port);

  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  /* unregister */
  if(start_core_audio_port != NULL){
    ags_core_audio_port_unregister(start_core_audio_port->data);
  }
  
  g_free(core_audio_devout->device_name);

  core_audio_devout->device_name = NULL;

  g_free(core_audio_devout->device_id);

  core_audio_devout->device_id = NULL;
  
#if defined(AGS_WITH_CORE_AUDIO)
  devices_property_address.mSelector = kAudioHardwarePropertyDevices;
  devices_property_address.mScope = kAudioObjectPropertyScopeGlobal;
  devices_property_address.mElement = kAudioObjectPropertyElementMain;

  streams_property_address.mSelector = kAudioDevicePropertyStreams;
  streams_property_address.mScope = kAudioDevicePropertyScopeOutput;

  error = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &devices_property_address, 0, NULL, &prop_size);
  
  if(error == noErr){
    device_count = prop_size / sizeof(AudioDeviceID);
    
    audio_devices = (AudioDeviceID *) malloc(prop_size);
    
    error = AudioObjectGetPropertyData(kAudioObjectSystemObject, &devices_property_address, 0, NULL, &prop_size, audio_devices);
    
    if(error == noErr) {
      for(i = 0; i < device_count; i++){
	NSString *current_manufacturer, *current_name, *current_uid;
	
	prop_size = sizeof(CFStringRef);
	
	devices_property_address.mSelector = kAudioDevicePropertyDeviceManufacturerCFString;
	error = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_manufacturer);
	
	if(error != noErr){
	  current_manufacturer = @"";
	  
	  //	  continue;
	}
	
	devices_property_address.mSelector = kAudioDevicePropertyDeviceNameCFString;
	error = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_name);
	
	if(error != noErr){
	  current_name = @"";

	  //	  continue;
	}
	
	devices_property_address.mSelector = kAudioDevicePropertyDeviceUID;
	error = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_uid);
	
	if(error != noErr){
	  current_uid = @"";
	  
	  //	  continue;
	}
	
	if([current_manufacturer isEqualToString:@"Apple Inc."] && [current_name isEqualToString:@"Built-in Output"]){
	  device_uid = current_uid;
	}

	is_speaker = 0;

	error = AudioObjectGetPropertyDataSize(audio_devices[i], 
					       &streams_property_address, 
					       0, 
					       NULL, 
					       &prop_size);
	
	stream_count = prop_size / sizeof(AudioStreamID);

	if(stream_count > 0){
	  is_speaker = YES;
	}

	str = g_strdup_printf("%s - %s",
			      [current_manufacturer UTF8String],
			      [current_name UTF8String]);

	//	g_message("test device %s - %s", device, str);
	
	if(is_speaker &&
	   !g_ascii_strcasecmp(str, device)){
	  //	  g_message(" `- success");
	  
	  core_audio_devout->device_name = g_strdup(device);
	  
	  core_audio_devout->device_id = g_strdup_printf("out-%s",
							 [current_uid UTF8String]);

	  core_audio_devout->audio_device = audio_devices[i];

	  g_free(str);
	  
	  break;
	}

	g_free(str);
      }
    }
    
    free(audio_devices);
  }
#endif
  
  /* apply name to port */
  str = g_strdup(core_audio_devout->device_id);
  
  if(start_core_audio_port != NULL){
    g_object_set(start_core_audio_port->data,
		 "port-name", str,
		 NULL);

    ags_core_audio_port_register(start_core_audio_port->data,
				 str,
				 TRUE, FALSE,
				 TRUE);
  }
  
  g_list_free(start_core_audio_port);

  g_free(str);
}

gchar*
ags_core_audio_devout_get_device(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  gchar *device;

  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  device = NULL;

  /* get device */
  g_rec_mutex_lock(core_audio_devout_mutex);

  device = g_strdup(core_audio_devout->device_name);

  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  return(device);
}

void
ags_core_audio_devout_set_presets(AgsSoundcard *soundcard,
				  guint channels,
				  guint rate,
				  guint buffer_size,
				  AgsSoundcardFormat format)
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  g_object_set(core_audio_devout,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_core_audio_devout_get_presets(AgsSoundcard *soundcard,
				  guint *channels,
				  guint *rate,
				  guint *buffer_size,
				  AgsSoundcardFormat *format)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get presets */
  g_rec_mutex_lock(core_audio_devout_mutex);

  if(channels != NULL){
    *channels = core_audio_devout->pcm_channels;
  }

  if(rate != NULL){
    *rate = core_audio_devout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = core_audio_devout->buffer_size;
  }

  if(format != NULL){
    *format = core_audio_devout->format;
  }

  g_rec_mutex_unlock(core_audio_devout_mutex);
}

void
ags_core_audio_devout_list_cards(AgsSoundcard *soundcard,
				 GList **card_id, GList **card_name)
{
  AgsCoreAudioDevout *core_audio_devout;
  
#if defined(AGS_WITH_CORE_AUDIO)
  AudioDeviceID *audio_devices;
  
  NSString *device_uid = @"";

  AudioObjectPropertyAddress devices_property_address;
  AudioObjectPropertyAddress streams_property_address;
  
  struct AudioStreamBasicDescription stream_desc;
#endif
  
#if defined(AGS_WITH_CORE_AUDIO)  
  int device_count;
  int stream_count;
  int is_mic;
  int is_speaker;
  UInt32 prop_size;
  int i;
  OSStatus error;
#endif
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  if(card_id != NULL){
    *card_id = NULL;
  }

  if(card_name != NULL){
    *card_name = NULL;
  }
  
#if defined(AGS_WITH_CORE_AUDIO)
  devices_property_address.mSelector = kAudioHardwarePropertyDevices;
  devices_property_address.mScope = kAudioObjectPropertyScopeGlobal;
  devices_property_address.mElement = kAudioObjectPropertyElementMain;

  streams_property_address.mSelector = kAudioDevicePropertyStreams;
  streams_property_address.mScope = kAudioDevicePropertyScopeOutput;

  error = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &devices_property_address, 0, NULL, &prop_size);
  
  if(error == noErr){
    device_count = prop_size / sizeof(AudioDeviceID);
    
    audio_devices = (AudioDeviceID *) malloc(prop_size);
    
    error = AudioObjectGetPropertyData(kAudioObjectSystemObject, &devices_property_address, 0, NULL, &prop_size, audio_devices);
    
    if(error == noErr) {
      for(i = 0; i < device_count; i++){
	NSString *current_manufacturer, *current_name, *current_uid;
	
	prop_size = sizeof(CFStringRef);
	
	devices_property_address.mSelector = kAudioDevicePropertyDeviceManufacturerCFString;
	error = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_manufacturer);
	
	if(error != noErr){
	  current_manufacturer = @"";
	  
	  //	  continue;
	}
	
	devices_property_address.mSelector = kAudioDevicePropertyDeviceNameCFString;
	error = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_name);
	
	if(error != noErr){
	  current_name = @"";

	  //	  continue;
	}
	
	devices_property_address.mSelector = kAudioDevicePropertyDeviceUID;
	error = AudioObjectGetPropertyData(audio_devices[i], &devices_property_address, 0, NULL, &prop_size, &current_uid);
	
	if(error != noErr){
	  current_uid = @"";
	  
	  //	  continue;
	}
	
	if([current_manufacturer isEqualToString:@"Apple Inc."] && [current_name isEqualToString:@"Built-in Output"]){
	  device_uid = current_uid;
	}

	is_speaker = 0;

	error = AudioObjectGetPropertyDataSize(audio_devices[i], 
					       &streams_property_address, 
					       0, 
					       NULL, 
					       &prop_size);
	
	stream_count = prop_size / sizeof(AudioStreamID);

	if(stream_count > 0){
	  is_speaker = YES;
	}

	//	g_message("found %s device: %s - %s <%s>", (is_speaker ? "output": "input"),  [current_manufacturer UTF8String], [current_name UTF8String], [current_uid UTF8String]);

	if(is_speaker){
	  *card_id = g_list_prepend(*card_id,
				    g_strdup_printf("out-%s", [current_uid UTF8String]));
       
	  *card_name = g_list_prepend(*card_name,
				      g_strdup_printf("%s - %s", [current_manufacturer UTF8String], [current_name UTF8String]));
	}
      }
    }
    
    free(audio_devices);
  }
#endif
  
  if(card_id != NULL && *card_id != NULL){
    *card_id = g_list_reverse(*card_id);
  }

  if(card_name != NULL && *card_name != NULL){
    *card_name = g_list_reverse(*card_name);
  }
}

void
ags_core_audio_devout_pcm_info(AgsSoundcard *soundcard,
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
ags_core_audio_devout_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_PLAYBACK);
}

gboolean
ags_core_audio_devout_is_starting(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  gboolean is_starting;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* check is starting */
  is_starting = ags_core_audio_devout_test_flags(core_audio_devout, AGS_CORE_AUDIO_DEVOUT_START_PLAY);

  return(is_starting);
}

gboolean
ags_core_audio_devout_is_playing(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  gboolean is_playing;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* check is playing */
  is_playing = ags_core_audio_devout_test_flags(core_audio_devout, AGS_CORE_AUDIO_DEVOUT_PLAY);

  return(is_playing);
}

gchar*
ags_core_audio_devout_get_uptime(AgsSoundcard *soundcard)
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
ags_core_audio_devout_port_init(AgsSoundcard *soundcard,
				GError **error)
{
  AgsCoreAudioPort *core_audio_port;
  AgsCoreAudioDevout *core_audio_devout;

  AgsSoundcardFormat format;
  guint word_size;
  gboolean use_cache;

  GRecMutex *core_audio_port_mutex;
  GRecMutex *core_audio_devout_mutex;
  
  if(ags_soundcard_is_playing(soundcard)){
    return;
  }

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  //  g_message("Core Audio init");
  
  /* get core-audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* port */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_port = (AgsCoreAudioPort *) core_audio_devout->core_audio_port->data;

  g_rec_mutex_unlock(core_audio_devout_mutex);

  /* get port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  /* check use cache */
  g_rec_mutex_lock(core_audio_port_mutex);

  use_cache = core_audio_port->use_cache;

  if(use_cache){
    core_audio_port->completed_cache = 0;
    core_audio_port->current_cache = 1;
  }
  
  g_rec_mutex_unlock(core_audio_port_mutex);
  
  /* retrieve word size */
  g_rec_mutex_lock(core_audio_devout_mutex);

  switch(core_audio_devout->format){
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
  case AGS_SOUNDCARD_FLOAT:
    {
      word_size = sizeof(gfloat);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      word_size = sizeof(gdouble);
    }
    break;
  default:
    g_rec_mutex_unlock(core_audio_devout_mutex);
    
    g_warning("ags_core_audio_devout_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for playback */
  core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_7;
  core_audio_devout->flags |= (AGS_CORE_AUDIO_DEVOUT_START_PLAY |
			       AGS_CORE_AUDIO_DEVOUT_PLAY |
			       AGS_CORE_AUDIO_DEVOUT_NONBLOCKING);

  memset(core_audio_devout->app_buffer[0], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[1], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[2], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[3], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[4], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[5], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[6], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[7], 0, core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);

  /*  */
  core_audio_devout->tact_counter = 0.0;
  core_audio_devout->delay_counter = 0.0;
  core_audio_devout->tic_counter = 0;

  core_audio_devout->note_256th_attack_of_16th_pulse = 0;
  core_audio_devout->note_256th_attack_of_16th_pulse_position = 0;

  core_audio_devout->note_256th_delay_counter = 0.0;
  
  core_audio_devout->flags |= (AGS_CORE_AUDIO_DEVOUT_INITIALIZED |
			       AGS_CORE_AUDIO_DEVOUT_START_PLAY |
			       AGS_CORE_AUDIO_DEVOUT_PLAY);
  
  ags_atomic_int_or(&(core_audio_devout->sync_flags),
		  AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK);

  g_rec_mutex_unlock(core_audio_devout_mutex);
}

void
ags_core_audio_devout_port_play(AgsSoundcard *soundcard,
				GError **error)
{
  AgsCoreAudioClient *core_audio_client;
  AgsCoreAudioDevout *core_audio_devout;
  AgsCoreAudioPort *core_audio_port;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
      
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;

  gboolean initial_run;
  guint word_size;
  gboolean use_cache;
  gboolean core_audio_client_activated;

  GRecMutex *core_audio_devout_mutex;
  GRecMutex *core_audio_client_mutex;
  GRecMutex *core_audio_port_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  application_context = ags_application_context_get_instance();  
  
  /* get core-audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* client */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_client = (AgsCoreAudioClient *) core_audio_devout->core_audio_client;
  core_audio_port = (AgsCoreAudioPort *) core_audio_devout->core_audio_port->data;
  
  callback_mutex = &(core_audio_devout->callback_mutex);
  callback_finish_mutex = &(core_audio_devout->callback_finish_mutex);

  /* do playback */    
  initial_run = ((AGS_CORE_AUDIO_DEVOUT_START_PLAY & (core_audio_devout->flags)) != 0) ? TRUE: FALSE;

  core_audio_devout->flags &= (~AGS_CORE_AUDIO_DEVOUT_START_PLAY);

  if((AGS_CORE_AUDIO_DEVOUT_INITIALIZED & (core_audio_devout->flags)) == 0){
    g_rec_mutex_unlock(core_audio_devout_mutex);
    
    return;
  }
  
  //  g_message("Core Audio play");

  switch(core_audio_devout->format){
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
  case AGS_SOUNDCARD_FLOAT:
    {
      word_size = sizeof(gfloat);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      word_size = sizeof(gdouble);
    }
    break;
  default:
    g_rec_mutex_unlock(core_audio_devout_mutex);
    
    g_warning("ags_core_audio_devout_port_play(): unsupported word size");
    
    return;
  }

  g_rec_mutex_unlock(core_audio_devout_mutex);

  /* get port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  /* check use cache */
  g_rec_mutex_lock(core_audio_port_mutex);

  use_cache = core_audio_port->use_cache;
  
  g_rec_mutex_unlock(core_audio_port_mutex);

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

    idle_time.tv_nsec = ags_core_audio_port_get_latency(core_audio_port) / 8;

    g_rec_mutex_lock(core_audio_port_mutex);

    completed_cache = core_audio_port->completed_cache;

    cache_buffer_size = core_audio_port->cache_buffer_size;
    
    cache_offset = core_audio_port->cache_offset;
    
    g_rec_mutex_unlock(core_audio_port_mutex);

    if(completed_cache == 3){
      write_cache = 0;
    }else{
      write_cache = completed_cache + 1;
    }

    /* wait until ready */
    g_rec_mutex_lock(core_audio_port_mutex);

    current_cache = core_audio_port->current_cache;
  
    g_rec_mutex_unlock(core_audio_port_mutex);
    
    while(write_cache == current_cache){
      ags_time_nanosleep(&idle_time);

      g_rec_mutex_lock(core_audio_port_mutex);

      current_cache = core_audio_port->current_cache;

      g_rec_mutex_unlock(core_audio_port_mutex);
    }

    /* fill cache */
    g_rec_mutex_lock(core_audio_devout_mutex);

    pcm_channels = core_audio_devout->pcm_channels;
    buffer_size = core_audio_devout->buffer_size;
    format = core_audio_devout->format;
    
    g_rec_mutex_unlock(core_audio_devout_mutex);

    buffer = ags_soundcard_get_buffer(AGS_SOUNDCARD(core_audio_devout));

    // g_message("w %d", write_cache);
    
    switch(format){
    case AGS_SOUNDCARD_SIGNED_16_BIT:
      {
	if(cache_offset == 0){
	  ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					     core_audio_port->cache[write_cache], 1,
					     pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S16);
	}

	if(buffer != NULL){
	  ags_audio_buffer_util_copy_s16_to_s16(core_audio_port->audio_buffer_util,
						(gint16 *) core_audio_port->cache[write_cache] + (pcm_channels * cache_offset), 1,
						(gint16 *) buffer, 1,
						pcm_channels * buffer_size);
	}
      }
      break;
    case AGS_SOUNDCARD_SIGNED_24_BIT:
      {
	if(cache_offset == 0){
	  ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					     core_audio_port->cache[write_cache], 1,
					     pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
	}

	if(buffer != NULL){
	  ags_audio_buffer_util_copy_s24_to_s24(core_audio_port->audio_buffer_util,
						(gint32 *) core_audio_port->cache[write_cache] + (pcm_channels * cache_offset), 1,
						(gint32 *) buffer, 1,
						pcm_channels * buffer_size);
	}
      }
      break;
    case AGS_SOUNDCARD_SIGNED_32_BIT:
      {
	if(cache_offset == 0){
	  ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					     core_audio_port->cache[write_cache], 1,
					     pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
	}

	if(buffer != NULL){
	  ags_audio_buffer_util_copy_s32_to_s32(core_audio_port->audio_buffer_util,
						(gint32 *) core_audio_port->cache[write_cache] + (pcm_channels * cache_offset), 1,
						(gint32 *) buffer, 1,
						pcm_channels * buffer_size);
	}
      }
      break;
    case AGS_SOUNDCARD_FLOAT:
      {
	if(cache_offset == 0){
	  ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					     core_audio_port->cache[write_cache], 1,
					     pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_FLOAT);
	}

	if(buffer != NULL){
	  ags_audio_buffer_util_copy_float_to_float(core_audio_port->audio_buffer_util,
						    (gfloat *) core_audio_port->cache[write_cache] + (pcm_channels * cache_offset), 1,
						    (gfloat *) buffer, 1,
						    pcm_channels * buffer_size);
	}
      }
      break;
    case AGS_SOUNDCARD_DOUBLE:
      {
	if(cache_offset == 0){
	  ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					     core_audio_port->cache[write_cache], 1,
					     pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
	}

	if(buffer != NULL){
	  ags_audio_buffer_util_copy_double_to_double(core_audio_port->audio_buffer_util,
						      (gdouble *) core_audio_port->cache[write_cache] + (pcm_channels * cache_offset), 1,
						      (gdouble *) buffer, 1,
						      pcm_channels * buffer_size);
	}
      }
      break;
    }
    
    /* seek cache */
    if(cache_offset + buffer_size >= cache_buffer_size){
      g_rec_mutex_lock(core_audio_port_mutex);
      
      core_audio_port->completed_cache = write_cache;
      core_audio_port->cache_offset = 0;
      
      g_rec_mutex_unlock(core_audio_port_mutex);
    }else{
      g_rec_mutex_lock(core_audio_port_mutex);
      
      core_audio_port->cache_offset += buffer_size;
      
      g_rec_mutex_unlock(core_audio_port_mutex);
    }
  }else{
    /* get client mutex */
    core_audio_client_mutex = AGS_CORE_AUDIO_CLIENT_GET_OBJ_MUTEX(core_audio_client);

    /* get activated */
    g_rec_mutex_lock(core_audio_client_mutex);

    core_audio_client_activated = ((AGS_CORE_AUDIO_CLIENT_ACTIVATED & (core_audio_client->flags)) != 0) ? TRUE: FALSE;

    g_rec_mutex_unlock(core_audio_client_mutex);

    if(core_audio_client_activated){
      while((AGS_CORE_AUDIO_DEVOUT_PASS_THROUGH & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	usleep(4);
      }

      if((AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	ags_atomic_int_or(&(core_audio_devout->sync_flags),
			AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_WAIT);
	
	while(((AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0 ||
	       (AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_CALLBACK_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0) &&
	      ((AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_DONE & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0 ||
	       (AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_CALLBACK_DONE & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0)){
	  if((AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0 &&
	     (AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_CALLBACK_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	    ags_atomic_int_or(&(core_audio_devout->sync_flags),
			    AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_DONE);
	  }
	  
	  g_thread_yield();
	}
	
	ags_atomic_int_and(&(core_audio_devout->sync_flags),
			 (~AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK));

	ags_atomic_int_and(&(core_audio_devout->sync_flags),
			 (~(AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_WAIT |
			    AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_CALLBACK_WAIT |
			    AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_DONE |
			    AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_CALLBACK_DONE)));
      }

      /* signal */
      g_mutex_lock(callback_mutex);

      ags_atomic_int_or(&(core_audio_devout->sync_flags),
		      AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE);
    
      if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	g_cond_signal(&(core_audio_devout->callback_cond));
      }

      g_mutex_unlock(callback_mutex);
      //    }
    
      /* wait callback */	
      g_mutex_lock(callback_finish_mutex);
    
      if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0){
	ags_atomic_int_or(&(core_audio_devout->sync_flags),
			AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT);
    
	while((AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) == 0 &&
	      (AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
	  g_cond_wait(&(core_audio_devout->callback_finish_cond),
		      callback_finish_mutex);
	}
      }
      
      ags_atomic_int_and(&(core_audio_devout->sync_flags),
		       (~(AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT |
			  AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE)));
    
      g_mutex_unlock(callback_finish_mutex);
    }
  }

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;      
  
  /* tic soundcard */
  if(!initial_run){
    tic_device = ags_tic_device_new((GObject *) core_audio_devout);
    task = g_list_append(task,
			 tic_device);
  }
  
  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) core_audio_devout);
  task = g_list_append(task,
		       clear_buffer);
    
  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) core_audio_devout);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_core_audio_devout_port_free(AgsSoundcard *soundcard)
{
  AgsCoreAudioPort *core_audio_port;
  AgsCoreAudioDevout *core_audio_devout;

  guint pcm_channels;
  guint cache_buffer_size;
  guint word_size;
  gboolean use_cache;

  guint i;
  
  GRecMutex *core_audio_port_mutex;
  GRecMutex *core_audio_devout_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core-audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* port */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_port = (AgsCoreAudioPort *) core_audio_devout->core_audio_port->data;
  pcm_channels = core_audio_devout->pcm_channels;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  /* get port mutex */
  core_audio_port_mutex = AGS_CORE_AUDIO_PORT_GET_OBJ_MUTEX(core_audio_port);

  /* check use cache */
#if defined(AGS_CORE_AUDIO_PORT_USE_HW)
#else
  g_rec_mutex_lock(core_audio_port_mutex);

  cache_buffer_size = core_audio_port->cache_buffer_size;
  use_cache = core_audio_port->use_cache;

  if(use_cache){
    core_audio_port->completed_cache = 0;
    core_audio_port->current_cache = 0;
    core_audio_port->cache_offset = 0;
  }

  switch(core_audio_port->format){
  case AGS_SOUNDCARD_SIGNED_16_BIT:
    {
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[0], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S16);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[1], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S16);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[2], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S16);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[3], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S16);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_24_BIT:
    {
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[0], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[1], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[2], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[3], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
    }
    break;
  case AGS_SOUNDCARD_SIGNED_32_BIT:
    {
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[0], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[1], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[2], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[3], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_S32);
    }
    break;
  case AGS_SOUNDCARD_FLOAT:
    {
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[0], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_FLOAT);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[1], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_FLOAT);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[2], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_FLOAT);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[3], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_FLOAT);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[0], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[1], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[2], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
      ags_audio_buffer_util_clear_buffer(core_audio_port->audio_buffer_util,
					 core_audio_port->cache[3], 1,
					 pcm_channels * cache_buffer_size, AGS_AUDIO_BUFFER_UTIL_DOUBLE);
    }
    break;
  }

  g_rec_mutex_unlock(core_audio_port_mutex);
#endif
  
  /*  */
  g_rec_mutex_lock(core_audio_devout_mutex);

  if((AGS_CORE_AUDIO_DEVOUT_INITIALIZED & (core_audio_devout->flags)) == 0){
    g_rec_mutex_unlock(core_audio_devout_mutex);

    return;
  }

  callback_mutex = &(core_audio_devout->callback_mutex);
  callback_finish_mutex = &(core_audio_devout->callback_finish_mutex);
  
  //  ags_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);
  
  core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0;
  core_audio_devout->flags &= (~(AGS_CORE_AUDIO_DEVOUT_PLAY));

  ags_atomic_int_or(&(core_audio_devout->sync_flags),
		  AGS_CORE_AUDIO_DEVOUT_PASS_THROUGH);

  ags_atomic_int_and(&(core_audio_devout->sync_flags),
		   (~AGS_CORE_AUDIO_DEVOUT_INITIAL_CALLBACK));

  ags_atomic_int_and(&(core_audio_devout->sync_flags),
		   (~(AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_WAIT |
		      AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_CALLBACK_WAIT |
		      AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_DEVICE_DONE |
		      AGS_CORE_AUDIO_DEVOUT_PRE_SYNC_CALLBACK_DONE)));

  /* signal callback */
  g_mutex_lock(callback_mutex);

  ags_atomic_int_or(&(core_audio_devout->sync_flags),
		  AGS_CORE_AUDIO_DEVOUT_CALLBACK_DONE);
    
  if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
    g_cond_signal(&(core_audio_devout->callback_cond));
  }

  g_mutex_unlock(callback_mutex);

  /* signal thread */
  g_mutex_lock(callback_finish_mutex);

  ags_atomic_int_or(&(core_audio_devout->sync_flags),
		  AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_DONE);
    
  if((AGS_CORE_AUDIO_DEVOUT_CALLBACK_FINISH_WAIT & (ags_atomic_int_get(&(core_audio_devout->sync_flags)))) != 0){
    g_cond_signal(&(core_audio_devout->callback_finish_cond));
  }

  g_mutex_unlock(callback_finish_mutex);
  
  /*  */
  core_audio_devout->note_offset = core_audio_devout->start_note_offset;
  core_audio_devout->note_offset_absolute = core_audio_devout->start_note_offset;

  core_audio_devout->note_256th_attack_of_16th_pulse = 0;
  core_audio_devout->note_256th_attack_of_16th_pulse_position = 0;
  
  core_audio_devout->note_256th_offset = 16 * core_audio_devout->start_note_offset;
  
  if(core_audio_devout->note_256th_delay < 1.0){
    guint buffer_size;
    guint note_256th_attack_lower, note_256th_attack_upper;
    guint i;
    
    buffer_size = core_audio_devout->buffer_size;

    note_256th_attack_lower = 0;
    note_256th_attack_upper = 0;
    
    ags_soundcard_get_note_256th_attack(AGS_SOUNDCARD(core_audio_devout),
					&note_256th_attack_lower,
					&note_256th_attack_upper);
    
    if(note_256th_attack_lower < note_256th_attack_upper){
      core_audio_devout->note_256th_offset_last = core_audio_devout->note_256th_offset + floor((note_256th_attack_upper - note_256th_attack_lower) / (core_audio_devout->note_256th_delay * (double) buffer_size));
    }
  }

  switch(core_audio_devout->format){
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
  case AGS_SOUNDCARD_FLOAT:
    {
      word_size = sizeof(gfloat);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      word_size = sizeof(gdouble);
    }
    break;
  default:
    word_size = 0;
    
    g_critical("ags_core_audio_devout_free(): unsupported word size");
  }

  g_rec_mutex_unlock(core_audio_devout_mutex);

  if(core_audio_devout->core_audio_port != NULL){
    core_audio_port = core_audio_devout->core_audio_port->data;

    while(!ags_atomic_int_get(&(core_audio_port->is_empty))) usleep(500000);
  }

  g_rec_mutex_lock(core_audio_devout_mutex);
  
  memset(core_audio_devout->app_buffer[0], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[1], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[2], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[3], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[4], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[5], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[6], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);
  memset(core_audio_devout->app_buffer[7], 0, (size_t) core_audio_devout->pcm_channels * core_audio_devout->buffer_size * word_size);

  g_rec_mutex_unlock(core_audio_devout_mutex);  
}

void
ags_core_audio_devout_tic(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  gdouble absolute_delay;
  gdouble delay;
  gdouble delay_counter;
  guint attack;
  guint current_note_256th_attack;
  gdouble note_256th_delay;
  guint note_256th_attack_lower, note_256th_attack_upper;
  guint note_256th_attack_of_16th_pulse;
  guint note_256th_attack_of_16th_pulse_position;
  guint buffer_size;
  guint note_offset_absolute;
  guint note_offset;
  guint prev_note_256th_offset_last;
  guint loop_left, loop_right;
  gboolean do_loop;
  guint i;
  guint note_256th_offset_lower;
  guint note_256th_offset_upper;
  guint next_note_256th_offset_lower;
  guint next_note_256th_offset_upper;
  guint next_note_256th_attack_lower;
  guint next_note_256th_attack_upper;
  
  GRecMutex *core_audio_devout_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core_audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(core_audio_devout_mutex);

  delay = core_audio_devout->delay[core_audio_devout->tic_counter];
  delay_counter = core_audio_devout->delay_counter;

  note_256th_delay = core_audio_devout->note_256th_delay;

  attack = core_audio_devout->attack[core_audio_devout->tic_counter];

  buffer_size = core_audio_devout->buffer_size;
  
  note_offset = core_audio_devout->note_offset;
  note_offset_absolute = core_audio_devout->note_offset_absolute;
  
  loop_left = core_audio_devout->loop_left;
  loop_right = core_audio_devout->loop_right;
  
  do_loop = core_audio_devout->do_loop;

  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  absolute_delay = ags_core_audio_devout_get_absolute_delay(soundcard);

  note_256th_offset_lower = 0;
  note_256th_offset_upper = 0;

  note_256th_attack_lower = 0;
  note_256th_attack_upper = 0;

  note_256th_attack_of_16th_pulse_position =  ags_soundcard_get_note_256th_attack_of_16th_pulse_position(soundcard);

  ags_soundcard_get_note_256th_offset(soundcard,
				      &note_256th_offset_lower,
				      &note_256th_offset_upper);
  
  ags_soundcard_get_note_256th_attack(soundcard,
				      &note_256th_attack_lower,
				      &note_256th_attack_upper);


  prev_note_256th_offset_last = core_audio_devout->note_256th_offset_last;

  next_note_256th_offset_lower = 0;
  next_note_256th_offset_upper = 0;

  next_note_256th_attack_lower = 0;
  next_note_256th_attack_upper = 0;
  
  ags_soundcard_util_calc_next_note_256th_offset(G_OBJECT(soundcard),
						 &next_note_256th_offset_lower,
						 &next_note_256th_offset_upper);

  ags_soundcard_util_calc_next_note_256th_attack(G_OBJECT(soundcard),
						 &next_note_256th_attack_lower,
						 &next_note_256th_attack_upper);

  //  g_message("tic -> next 256th [%d-%d]", next_note_256th_offset_lower, next_note_256th_offset_upper);

  core_audio_devout->note_256th_delay_counter += 1.0;    
  
  if((note_256th_delay <= 1.0 ||
      core_audio_devout->note_256th_delay_counter >= note_256th_delay) &&
     (16 * (note_offset + 1) <= next_note_256th_offset_upper ||
      (next_note_256th_offset_lower + 64 < note_256th_offset_lower))){
    //    g_message("16th pulse: %d (delay = %f)", note_offset + 1, delay);
    
    if(do_loop &&
       next_note_256th_offset_lower + 64 < note_256th_offset_lower){
      ags_soundcard_set_note_offset(soundcard,
				    loop_left);

      g_rec_mutex_lock(core_audio_devout_mutex);

      core_audio_devout->note_256th_offset = next_note_256th_offset_lower;
      core_audio_devout->note_256th_offset_last = next_note_256th_offset_upper;
      
      core_audio_devout->note_256th_attack_of_16th_pulse = 0;
      core_audio_devout->note_256th_attack_of_16th_pulse_position = 0;

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }else{
      ags_soundcard_set_note_offset(soundcard,
				    note_offset + 1);

      g_rec_mutex_lock(core_audio_devout_mutex);
            
      core_audio_devout->note_256th_offset = next_note_256th_offset_lower;
      core_audio_devout->note_256th_offset_last = next_note_256th_offset_upper;

      note_256th_attack_of_16th_pulse = attack;
      
      current_note_256th_attack = attack;

      i = 1;

      current_note_256th_attack = ags_soundcard_get_note_256th_attack_at_position(soundcard,
										  note_256th_attack_of_16th_pulse_position + 1);
      
      for(; attack + (guint) floor((double) i * (note_256th_delay * (double) buffer_size)) < (guint) floor(absolute_delay * (double) buffer_size); i++){
	current_note_256th_attack = ags_soundcard_get_note_256th_attack_at_position(soundcard,
										    note_256th_attack_of_16th_pulse_position + i);
      }

      core_audio_devout->note_256th_attack_of_16th_pulse = current_note_256th_attack;
      core_audio_devout->note_256th_attack_of_16th_pulse_position += i;

      g_rec_mutex_unlock(core_audio_devout_mutex);
    }
    
    ags_soundcard_set_note_offset_absolute(soundcard,
					   note_offset_absolute + 1);

    /* reset - delay counter */
    g_rec_mutex_lock(core_audio_devout_mutex);

    if(do_loop &&
       next_note_256th_offset_lower + 64 < note_256th_offset_lower){
      core_audio_devout->tic_counter = 0;

      core_audio_devout->delay_counter = 0.0;

      core_audio_devout->tact_counter = 0.0;

      core_audio_devout->note_256th_delay_counter = 0.0;
    }else{    
      core_audio_devout->tic_counter += 1;

      if(core_audio_devout->tic_counter == (guint) AGS_SOUNDCARD_DEFAULT_PERIOD){
	/* reset - tic counter i.e. modified delay index within period */
	core_audio_devout->tic_counter = 0;
      }
      
      core_audio_devout->delay_counter = 0.0;

      core_audio_devout->tact_counter += 1.0;

      core_audio_devout->note_256th_delay_counter = 0.0;
    }
    
    g_rec_mutex_unlock(core_audio_devout_mutex);

    /* 16th pulse */
    if(do_loop &&
       next_note_256th_offset_lower + 64 < note_256th_offset_lower){
      ags_soundcard_offset_changed(soundcard,
				   loop_left);
    }else{
      ags_soundcard_offset_changed(soundcard,
				   note_offset + 1);
    }
  }else{
    g_rec_mutex_lock(core_audio_devout_mutex);
    
    if(note_256th_delay <= 1.0){
      core_audio_devout->note_256th_offset = next_note_256th_offset_lower;
      core_audio_devout->note_256th_offset_last = next_note_256th_offset_upper;
    }else{
      if(core_audio_devout->note_256th_delay_counter >= note_256th_delay){
	core_audio_devout->note_256th_offset = next_note_256th_offset_lower;
	core_audio_devout->note_256th_offset_last = next_note_256th_offset_upper;

	core_audio_devout->note_256th_delay_counter -= note_256th_delay;
      }
    }

    core_audio_devout->delay_counter += 1.0;
    
    g_rec_mutex_unlock(core_audio_devout_mutex);

    if(floor(delay) + 1.0 < delay_counter + 1.0){
      ags_soundcard_set_note_offset(soundcard,
				    note_offset + 1);
            
      g_rec_mutex_lock(core_audio_devout_mutex);
    
      core_audio_devout->note_256th_offset = next_note_256th_offset_lower;
      core_audio_devout->note_256th_offset_last = next_note_256th_offset_upper;

      note_256th_attack_of_16th_pulse = attack;
      
      current_note_256th_attack = attack;

      i = 1;

      current_note_256th_attack = ags_soundcard_get_note_256th_attack_at_position(soundcard,
										  note_256th_attack_of_16th_pulse_position + 1);
      
      for(; attack + (guint) floor((double) i * (note_256th_delay * (double) buffer_size)) < (guint) floor(absolute_delay * (double) buffer_size); i++){
	current_note_256th_attack = ags_soundcard_get_note_256th_attack_at_position(soundcard,
										    note_256th_attack_of_16th_pulse_position + i);
      }

      core_audio_devout->note_256th_attack_of_16th_pulse = current_note_256th_attack;
      core_audio_devout->note_256th_attack_of_16th_pulse_position += i;
    
      g_rec_mutex_unlock(core_audio_devout_mutex);
    
      ags_soundcard_set_note_offset_absolute(soundcard,
					     note_offset_absolute + 1);

      /* reset - delay counter */
      g_rec_mutex_lock(core_audio_devout_mutex);
    
      core_audio_devout->tic_counter += 1;

      if(core_audio_devout->tic_counter == (guint) AGS_SOUNDCARD_DEFAULT_PERIOD){
	/* reset - tic counter i.e. modified delay index within period */
	core_audio_devout->tic_counter = 0;
      }
      
      core_audio_devout->delay_counter = 0.0;

      core_audio_devout->tact_counter += 1.0;

      core_audio_devout->note_256th_delay_counter = 0.0;
    
      g_rec_mutex_unlock(core_audio_devout_mutex);

      /* 16th pulse */
      ags_soundcard_offset_changed(soundcard,
				   note_offset + 1);
    }
  }
}

void
ags_core_audio_devout_offset_changed(AgsSoundcard *soundcard,
				     guint note_offset)
{
#if 0
  AgsCoreAudioDevout *core_audio_devout;
  
  GRecMutex *core_audio_devout_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* offset changed */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->tic_counter += 1;

  if(core_audio_devout->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    core_audio_devout->tic_counter = 0;
  }

  g_rec_mutex_unlock(core_audio_devout_mutex);
#endif
}

void
ags_core_audio_devout_set_bpm(AgsSoundcard *soundcard,
			      gdouble bpm)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* set bpm */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->bpm = bpm;

  g_rec_mutex_unlock(core_audio_devout_mutex);

  ags_core_audio_devout_adjust_delay_and_attack(core_audio_devout);
}

gdouble
ags_core_audio_devout_get_bpm(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  gdouble bpm;
  
  GRecMutex *core_audio_devout_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get bpm */
  g_rec_mutex_lock(core_audio_devout_mutex);

  bpm = core_audio_devout->bpm;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(bpm);
}

void
ags_core_audio_devout_set_delay_factor(AgsSoundcard *soundcard,
				       gdouble delay_factor)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* set delay factor */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->delay_factor = delay_factor;

  g_rec_mutex_unlock(core_audio_devout_mutex);

  ags_core_audio_devout_adjust_delay_and_attack(core_audio_devout);
}

gdouble
ags_core_audio_devout_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  gdouble delay_factor;
  
  GRecMutex *core_audio_devout_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get delay factor */
  g_rec_mutex_lock(core_audio_devout_mutex);

  delay_factor = core_audio_devout->delay_factor;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(delay_factor);
}

gdouble
ags_core_audio_devout_get_delay(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint delay_index;
  gdouble delay;
  
  GRecMutex *core_audio_devout_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get delay */
  g_rec_mutex_lock(core_audio_devout_mutex);

  delay_index = core_audio_devout->tic_counter;

  delay = core_audio_devout->delay[delay_index];
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  return(delay);
}

gdouble
ags_core_audio_devout_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  gdouble absolute_delay;
  
  GRecMutex *core_audio_devout_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get absolute delay */
  g_rec_mutex_lock(core_audio_devout_mutex);

  absolute_delay = (60.0 * (((gdouble) core_audio_devout->samplerate / (gdouble) core_audio_devout->buffer_size) / (gdouble) core_audio_devout->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) core_audio_devout->delay_factor)));

  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(absolute_delay);
}

guint
ags_core_audio_devout_get_attack(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint attack_index;
  guint attack;
  
  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get attack */
  g_rec_mutex_lock(core_audio_devout_mutex);

  attack_index = core_audio_devout->tic_counter;

  attack = core_audio_devout->attack[attack_index];

  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  return(attack);
}

void*
ags_core_audio_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  void *buffer;

  GRecMutex *core_audio_devout_mutex;  
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  g_rec_mutex_lock(core_audio_devout_mutex);

  if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0){
    buffer = core_audio_devout->app_buffer[0];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_1){
    buffer = core_audio_devout->app_buffer[1];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_2){
    buffer = core_audio_devout->app_buffer[2];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_3){
    buffer = core_audio_devout->app_buffer[3];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_4){
    buffer = core_audio_devout->app_buffer[4];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_5){
    buffer = core_audio_devout->app_buffer[5];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_6){
    buffer = core_audio_devout->app_buffer[6];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_7){
    buffer = core_audio_devout->app_buffer[7];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(buffer);
}

void*
ags_core_audio_devout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  void *buffer;

  GRecMutex *core_audio_devout_mutex;  
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  g_rec_mutex_lock(core_audio_devout_mutex);

  if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0){
    buffer = core_audio_devout->app_buffer[1];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_1){
    buffer = core_audio_devout->app_buffer[2];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_2){
    buffer = core_audio_devout->app_buffer[3];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_3){
    buffer = core_audio_devout->app_buffer[4];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_4){
    buffer = core_audio_devout->app_buffer[5];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_5){
    buffer = core_audio_devout->app_buffer[6];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_6){
    buffer = core_audio_devout->app_buffer[7];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_7){
    buffer = core_audio_devout->app_buffer[0];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(buffer);
}

void*
ags_core_audio_devout_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  void *buffer;

  GRecMutex *core_audio_devout_mutex;  
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  g_rec_mutex_lock(core_audio_devout_mutex);

  if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0){
    buffer = core_audio_devout->app_buffer[7];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_1){
    buffer = core_audio_devout->app_buffer[0];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_2){
    buffer = core_audio_devout->app_buffer[1];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_3){
    buffer = core_audio_devout->app_buffer[2];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_4){
    buffer = core_audio_devout->app_buffer[3];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_5){
    buffer = core_audio_devout->app_buffer[4];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_6){
    buffer = core_audio_devout->app_buffer[5];
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_7){
    buffer = core_audio_devout->app_buffer[6];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(buffer);
}

void
ags_core_audio_devout_lock_buffer(AgsSoundcard *soundcard,
				  void *buffer)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *buffer_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(core_audio_devout->app_buffer != NULL){
    if(buffer == core_audio_devout->app_buffer[0]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[0];
    }else if(buffer == core_audio_devout->app_buffer[1]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[1];
    }else if(buffer == core_audio_devout->app_buffer[2]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[2];
    }else if(buffer == core_audio_devout->app_buffer[3]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[3];
    }else if(buffer == core_audio_devout->app_buffer[4]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[4];
    }else if(buffer == core_audio_devout->app_buffer[5]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[5];
    }else if(buffer == core_audio_devout->app_buffer[6]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[6];
    }else if(buffer == core_audio_devout->app_buffer[7]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[7];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_core_audio_devout_unlock_buffer(AgsSoundcard *soundcard,
				    void *buffer)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *buffer_mutex;
  
  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(core_audio_devout->app_buffer != NULL){
    if(buffer == core_audio_devout->app_buffer[0]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[0];
    }else if(buffer == core_audio_devout->app_buffer[1]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[1];
    }else if(buffer == core_audio_devout->app_buffer[2]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[2];
    }else if(buffer == core_audio_devout->app_buffer[3]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[3];
    }else if(buffer == core_audio_devout->app_buffer[4]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[4];
    }else if(buffer == core_audio_devout->app_buffer[5]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[5];
    }else if(buffer == core_audio_devout->app_buffer[6]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[6];
    }else if(buffer == core_audio_devout->app_buffer[7]){
      buffer_mutex = core_audio_devout->app_buffer_mutex[7];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_core_audio_devout_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint delay_counter;
  
  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* delay counter */
  g_rec_mutex_lock(core_audio_devout_mutex);

  delay_counter = core_audio_devout->delay_counter;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(delay_counter);
}

void
ags_core_audio_devout_set_start_note_offset(AgsSoundcard *soundcard,
					    guint start_note_offset)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* set note offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(core_audio_devout_mutex);
}

guint
ags_core_audio_devout_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint start_note_offset;
  
  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* set note offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  start_note_offset = core_audio_devout->start_note_offset;

  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(start_note_offset);
}

void
ags_core_audio_devout_set_note_offset(AgsSoundcard *soundcard,
				      guint note_offset)
{
  AgsCoreAudioDevout *core_audio_devout;

  gdouble note_256th_delay;

  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* set note offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->note_offset = note_offset;

  note_256th_delay = core_audio_devout->note_256th_delay;
  
  core_audio_devout->note_256th_offset = 16 * note_offset;

  core_audio_devout->note_256th_offset_last = core_audio_devout->note_256th_offset;
  
  if(note_256th_delay < 1.0){
    guint buffer_size;
    guint note_256th_attack_lower, note_256th_attack_upper;
    
    buffer_size = core_audio_devout->buffer_size;

    note_256th_attack_lower = 0;
    note_256th_attack_upper = 0;
    
    ags_soundcard_get_note_256th_attack(AGS_SOUNDCARD(core_audio_devout),
					&note_256th_attack_lower,
					&note_256th_attack_upper);

    if(note_256th_attack_lower < note_256th_attack_upper){
      core_audio_devout->note_256th_offset_last = core_audio_devout->note_256th_offset + floor((note_256th_attack_upper - note_256th_attack_lower) / (core_audio_devout->note_256th_delay * (double) buffer_size));
    }
  }

  g_rec_mutex_unlock(core_audio_devout_mutex);
}

guint
ags_core_audio_devout_get_note_offset(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint note_offset;
  
  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* set note offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  note_offset = core_audio_devout->note_offset;

  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(note_offset);
}

void
ags_core_audio_devout_get_note_256th_offset(AgsSoundcard *soundcard,
					    guint *note_256th_offset_lower,
					    guint *note_256th_offset_upper)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core_audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get note 256th offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  if(note_256th_offset_lower != NULL){
    note_256th_offset_lower[0] = core_audio_devout->note_256th_offset;
  }

  if(note_256th_offset_upper != NULL){
    note_256th_offset_upper[0] = core_audio_devout->note_256th_offset_last;
  }

  g_rec_mutex_unlock(core_audio_devout_mutex);
}

void
ags_core_audio_devout_get_note_256th_attack(AgsSoundcard *soundcard,
					    guint *note_256th_attack_lower,
					    guint *note_256th_attack_upper)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint *note_256th_attack;

  guint nth_list;
  guint note_256th_attack_position_lower, note_256th_attack_position_upper;
  guint local_note_256th_attack_lower, local_note_256th_attack_upper;
  
  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core_audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get note 256th attack lower and upper */
  ags_core_audio_devout_get_note_256th_attack_position(soundcard,
						       &note_256th_attack_position_lower,
						       &note_256th_attack_position_upper);

  local_note_256th_attack_lower = 0;
  local_note_256th_attack_upper = 0;
  
  g_rec_mutex_lock(core_audio_devout_mutex);

  nth_list = (guint) floor(note_256th_attack_position_lower / AGS_SOUNDCARD_DEFAULT_PERIOD);
  note_256th_attack = g_list_nth_data(core_audio_devout->note_256th_attack,
				      nth_list);
  if(note_256th_attack != NULL){
    local_note_256th_attack_lower = note_256th_attack[note_256th_attack_position_lower % (guint) AGS_SOUNDCARD_DEFAULT_PERIOD];
  }
  
  if(note_256th_attack_lower != NULL){
    note_256th_attack_lower[0] = local_note_256th_attack_lower;
  }  

  nth_list = (guint) floor(note_256th_attack_position_upper / AGS_SOUNDCARD_DEFAULT_PERIOD);
  note_256th_attack = g_list_nth_data(core_audio_devout->note_256th_attack,
				      nth_list);

  if(note_256th_attack != NULL){
    local_note_256th_attack_upper = note_256th_attack[note_256th_attack_position_upper % (guint) AGS_SOUNDCARD_DEFAULT_PERIOD];
  }else{
    local_note_256th_attack_upper = local_note_256th_attack_lower;
  }
  
  if(note_256th_attack_upper != NULL){
    note_256th_attack_upper[0] = local_note_256th_attack_upper;
  }  
    
  g_rec_mutex_unlock(core_audio_devout_mutex);
}

guint
ags_core_audio_devout_get_note_256th_attack_at_position(AgsSoundcard *soundcard,
							guint note_256th_attack_position)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  guint *note_256th_attack;

  guint nth_list;
  guint current_note_256th_attack;

  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core_audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get note 256th attack */
  g_rec_mutex_lock(core_audio_devout_mutex);
  
  current_note_256th_attack = 0;

  nth_list = (guint) floor(note_256th_attack_position / AGS_SOUNDCARD_DEFAULT_PERIOD);
  note_256th_attack = g_list_nth_data(core_audio_devout->note_256th_attack,
				      nth_list);

  if(note_256th_attack != NULL){
    current_note_256th_attack = note_256th_attack[note_256th_attack_position % (guint) AGS_SOUNDCARD_DEFAULT_PERIOD];
  }
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(current_note_256th_attack);
}

void
ags_core_audio_devout_get_note_256th_attack_position(AgsSoundcard *soundcard,
						     guint *note_256th_attack_position_lower,
						     guint *note_256th_attack_position_upper)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint buffer_size;
  guint attack_position;
  guint local_attack;
  gdouble note_256th_delay;
  guint nth_list;
  guint *local_note_256th_attack;
  guint position_lower, position_upper;
  guint i;
  
  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);
  
  /* get core_audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);
  
  /* get note 256th attack */
  g_rec_mutex_lock(core_audio_devout_mutex);

  buffer_size = core_audio_devout->buffer_size;

  attack_position = core_audio_devout->tic_counter;

  local_attack = core_audio_devout->attack[attack_position];

  note_256th_delay = core_audio_devout->note_256th_delay;

  if(1.0 / note_256th_delay >= AGS_SOUNDCARD_DEFAULT_PERIOD){
    g_critical("unexpected time segmentation");
  }

  position_lower = 16 * core_audio_devout->tic_counter;

  for(i = 1; local_attack - (i * note_256th_delay * buffer_size) >= 0; i++){
    if(position_lower - 1 >= 0){
      position_lower--;
    }
  }
  
  position_upper = position_lower;

  nth_list = (guint) floor(position_lower / AGS_SOUNDCARD_DEFAULT_PERIOD);

  local_note_256th_attack = g_list_nth_data(core_audio_devout->note_256th_attack,
					    nth_list);

  for(i = 1; local_note_256th_attack[position_lower % (guint) AGS_SOUNDCARD_DEFAULT_PERIOD] + (guint) floor((double) i * note_256th_delay * (double) buffer_size) < buffer_size; i++){
    if((position_upper + 1) % (guint) AGS_SOUNDCARD_DEFAULT_PERIOD == 0){
      if(nth_list + 1 < 32){
	local_note_256th_attack = g_list_nth_data(core_audio_devout->note_256th_attack,
						  nth_list + 1);
      }else{
	local_note_256th_attack = g_list_nth_data(core_audio_devout->note_256th_attack,
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
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
}

guint
ags_core_audio_devout_get_note_256th_attack_of_16th_pulse(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint note_256th_attack_of_16th_pulse;
  
  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core_audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get note 256th attack of 16th pulse */
  g_rec_mutex_lock(core_audio_devout_mutex);

  note_256th_attack_of_16th_pulse = core_audio_devout->note_256th_attack_of_16th_pulse;

  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(note_256th_attack_of_16th_pulse);
}

guint
ags_core_audio_devout_get_note_256th_attack_of_16th_pulse_position(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint position;
  
  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core_audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get note 256th attack position of 16th pulse */
  g_rec_mutex_lock(core_audio_devout_mutex);

  position = core_audio_devout->note_256th_attack_of_16th_pulse_position;

  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(position);
}

void
ags_core_audio_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
					       guint note_offset_absolute)
{
  AgsCoreAudioDevout *core_audio_devout;
  
  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* set note offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->note_offset_absolute = note_offset_absolute;

  g_rec_mutex_unlock(core_audio_devout_mutex);
}

guint
ags_core_audio_devout_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint note_offset_absolute;
  
  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* set note offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  note_offset_absolute = core_audio_devout->note_offset_absolute;

  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(note_offset_absolute);
}

void
ags_core_audio_devout_set_loop(AgsSoundcard *soundcard,
			       guint loop_left, guint loop_right,
			       gboolean do_loop)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* set loop */
  g_rec_mutex_lock(core_audio_devout_mutex);

  core_audio_devout->loop_left = loop_left;
  core_audio_devout->loop_right = loop_right;
  core_audio_devout->do_loop = do_loop;

  if(do_loop){
    core_audio_devout->loop_offset = core_audio_devout->note_offset;
  }

  g_rec_mutex_unlock(core_audio_devout_mutex);
}

void
ags_core_audio_devout_get_loop(AgsSoundcard *soundcard,
			       guint *loop_left, guint *loop_right,
			       gboolean *do_loop)
{
  AgsCoreAudioDevout *core_audio_devout;

  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get loop */
  g_rec_mutex_lock(core_audio_devout_mutex);

  if(loop_left != NULL){
    *loop_left = core_audio_devout->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = core_audio_devout->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = core_audio_devout->do_loop;
  }

  g_rec_mutex_unlock(core_audio_devout_mutex);
}

guint
ags_core_audio_devout_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint loop_offset;
  
  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get loop offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  loop_offset = core_audio_devout->loop_offset;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(loop_offset);
}

guint
ags_core_audio_devout_get_sub_block_count(AgsSoundcard *soundcard)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint sub_block_count;
  
  GRecMutex *core_audio_devout_mutex;  

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get loop offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  sub_block_count = core_audio_devout->sub_block_count;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

  return(sub_block_count);
}

gboolean
ags_core_audio_devout_trylock_sub_block(AgsSoundcard *soundcard,
					void *buffer, guint sub_block)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint pcm_channels;
  guint sub_block_count;
  gboolean success;
  
  GRecMutex *core_audio_devout_mutex;  
  GRecMutex *sub_block_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get loop offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  pcm_channels = core_audio_devout->pcm_channels;
  sub_block_count = core_audio_devout->sub_block_count;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  sub_block_mutex = NULL;

  success = FALSE;
  
  if(core_audio_devout->app_buffer != NULL){
    if(buffer == core_audio_devout->app_buffer[0]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[sub_block];
    }else if(buffer == core_audio_devout->app_buffer[1]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[2]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[3]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[4]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[4 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[5]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[5 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[6]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[6 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[7]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[7 * pcm_channels * sub_block_count + sub_block];
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
ags_core_audio_devout_unlock_sub_block(AgsSoundcard *soundcard,
				       void *buffer, guint sub_block)
{
  AgsCoreAudioDevout *core_audio_devout;

  guint pcm_channels;
  guint sub_block_count;
  
  GRecMutex *core_audio_devout_mutex;  
  GRecMutex *sub_block_mutex;

  core_audio_devout = AGS_CORE_AUDIO_DEVOUT(soundcard);

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get loop offset */
  g_rec_mutex_lock(core_audio_devout_mutex);

  pcm_channels = core_audio_devout->pcm_channels;
  sub_block_count = core_audio_devout->sub_block_count;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);
  
  sub_block_mutex = NULL;
  
  if(core_audio_devout->app_buffer != NULL){
    if(buffer == core_audio_devout->app_buffer[0]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[sub_block];
    }else if(buffer == core_audio_devout->app_buffer[1]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[2]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[3]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[4]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[4 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[5]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[5 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[6]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[6 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == core_audio_devout->app_buffer[7]){
      sub_block_mutex = core_audio_devout->sub_block_mutex[7 * pcm_channels * sub_block_count + sub_block];
    }
  }

  if(sub_block_mutex != NULL){
    g_rec_mutex_unlock(sub_block_mutex);
  }
}

/**
 * ags_core_audio_devout_switch_buffer_flag:
 * @core_audio_devout: an #AgsCoreAudioDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_devout_switch_buffer_flag(AgsCoreAudioDevout *core_audio_devout)
{
  GRecMutex *core_audio_devout_mutex;
  
  if(!AGS_IS_CORE_AUDIO_DEVOUT(core_audio_devout)){
    return;
  }

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* switch buffer flag */
  g_rec_mutex_lock(core_audio_devout_mutex);

  if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_1;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_1){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_2;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_2){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_3;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_3){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_4;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_4){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_5;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_5){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_6;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_6){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_7;
  }else if(core_audio_devout->app_buffer_mode == AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_7){
    core_audio_devout->app_buffer_mode = AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(core_audio_devout_mutex);
}

/**
 * ags_core_audio_devout_adjust_delay_and_attack:
 * @core_audio_devout: the #AgsCoreAudioDevout
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_devout_adjust_delay_and_attack(AgsCoreAudioDevout *core_audio_devout)
{
  if(!AGS_IS_CORE_AUDIO_DEVOUT(core_audio_devout)){
    return;
  }

  ags_soundcard_util_adjust_delay_and_attack((GObject *) core_audio_devout);
}

/**
 * ags_core_audio_devout_realloc_buffer:
 * @core_audio_devout: the #AgsCoreAudioDevout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.0.0
 */
void
ags_core_audio_devout_realloc_buffer(AgsCoreAudioDevout *core_audio_devout)
{
  guint pcm_channels;
  guint buffer_size;
  AgsSoundcardFormat format;
  guint word_size;

  GRecMutex *core_audio_devout_mutex;  

  if(!AGS_IS_CORE_AUDIO_DEVOUT(core_audio_devout)){
    return;
  }

  /* get core audio devout mutex */
  core_audio_devout_mutex = AGS_CORE_AUDIO_DEVOUT_GET_OBJ_MUTEX(core_audio_devout);

  /* get word size */  
  g_rec_mutex_lock(core_audio_devout_mutex);

  pcm_channels = core_audio_devout->pcm_channels;
  buffer_size = core_audio_devout->buffer_size;

  format = core_audio_devout->format;
  
  g_rec_mutex_unlock(core_audio_devout_mutex);

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
  case AGS_SOUNDCARD_FLOAT:
    {
      word_size = sizeof(gfloat);
    }
    break;
  case AGS_SOUNDCARD_DOUBLE:
    {
      word_size = sizeof(gdouble);
    }
    break;
  default:
    g_warning("ags_core_audio_devout_realloc_buffer(): unsupported word size");
    return;
  }
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_0 */
  if(core_audio_devout->app_buffer[0] != NULL){
    g_free(core_audio_devout->app_buffer[0]);
  }
  
  core_audio_devout->app_buffer[0] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devout->app_buffer[0], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_1 */
  if(core_audio_devout->app_buffer[1] != NULL){
    g_free(core_audio_devout->app_buffer[1]);
  }

  core_audio_devout->app_buffer[1] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  memset(core_audio_devout->app_buffer[1], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_2 */
  if(core_audio_devout->app_buffer[2] != NULL){
    g_free(core_audio_devout->app_buffer[2]);
  }

  core_audio_devout->app_buffer[2] = (void *) g_malloc(pcm_channels * buffer_size * word_size);
  
  memset(core_audio_devout->app_buffer[2], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_3 */
  if(core_audio_devout->app_buffer[3] != NULL){
    g_free(core_audio_devout->app_buffer[3]);
  }
  
  core_audio_devout->app_buffer[3] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devout->app_buffer[3], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_4 */
  if(core_audio_devout->app_buffer[4] != NULL){
    g_free(core_audio_devout->app_buffer[4]);
  }
  
  core_audio_devout->app_buffer[4] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devout->app_buffer[4], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_5 */
  if(core_audio_devout->app_buffer[5] != NULL){
    g_free(core_audio_devout->app_buffer[5]);
  }
  
  core_audio_devout->app_buffer[5] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devout->app_buffer[5], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_6 */
  if(core_audio_devout->app_buffer[6] != NULL){
    g_free(core_audio_devout->app_buffer[6]);
  }
  
  core_audio_devout->app_buffer[6] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devout->app_buffer[6], 0, pcm_channels * buffer_size * word_size);
  
  /* AGS_CORE_AUDIO_DEVOUT_APP_BUFFER_7 */
  if(core_audio_devout->app_buffer[7] != NULL){
    g_free(core_audio_devout->app_buffer[7]);
  }
  
  core_audio_devout->app_buffer[7] = (void *) g_malloc(pcm_channels * buffer_size * word_size);

  memset(core_audio_devout->app_buffer[7], 0, pcm_channels * buffer_size * word_size);  
}

/**
 * ags_core_audio_devout_new:
 *
 * Creates a new instance of #AgsCoreAudioDevout.
 *
 * Returns: a new #AgsCoreAudioDevout
 *
 * Since: 3.0.0
 */
AgsCoreAudioDevout*
ags_core_audio_devout_new()
{
  AgsCoreAudioDevout *core_audio_devout;

  core_audio_devout = (AgsCoreAudioDevout *) g_object_new(AGS_TYPE_CORE_AUDIO_DEVOUT,
							  NULL);
  
  return(core_audio_devout);
}
