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

#include <ags/audio/audio-unit/ags_audio_unit_devout.h>

#include <ags/audio/ags_sound_provider.h>
#include <ags/audio/ags_audio_buffer_util.h>

#include <ags/audio/audio-unit/ags_audio_unit_server.h>
#include <ags/audio/audio-unit/ags_audio_unit_client.h>
#include <ags/audio/audio-unit/ags_audio_unit_port.h>

#include <ags/audio/task/ags_tic_device.h>
#include <ags/audio/task/ags_clear_buffer.h>
#include <ags/audio/task/ags_switch_buffer_flag.h>

#include <ags/audio/thread/ags_audio_loop.h>

#include <string.h>
#include <math.h>
#include <time.h>

#include <ags/config.h>
#include <ags/i18n.h>

void ags_audio_unit_devout_class_init(AgsAudioUnitDevoutClass *audio_unit_devout);
void ags_audio_unit_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_unit_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_audio_unit_devout_init(AgsAudioUnitDevout *audio_unit_devout);
void ags_audio_unit_devout_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec);
void ags_audio_unit_devout_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec);
void ags_audio_unit_devout_dispose(GObject *gobject);
void ags_audio_unit_devout_finalize(GObject *gobject);

AgsUUID* ags_audio_unit_devout_get_uuid(AgsConnectable *connectable);
gboolean ags_audio_unit_devout_has_resource(AgsConnectable *connectable);
gboolean ags_audio_unit_devout_is_ready(AgsConnectable *connectable);
void ags_audio_unit_devout_add_to_registry(AgsConnectable *connectable);
void ags_audio_unit_devout_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_audio_unit_devout_list_resource(AgsConnectable *connectable);
xmlNode* ags_audio_unit_devout_xml_compose(AgsConnectable *connectable);
void ags_audio_unit_devout_xml_parse(AgsConnectable *connectable,
			       xmlNode *node);
gboolean ags_audio_unit_devout_is_connected(AgsConnectable *connectable);
void ags_audio_unit_devout_connect(AgsConnectable *connectable);
void ags_audio_unit_devout_disconnect(AgsConnectable *connectable);

void ags_audio_unit_devout_set_device(AgsSoundcard *soundcard,
				      gchar *device);
gchar* ags_audio_unit_devout_get_device(AgsSoundcard *soundcard);

void ags_audio_unit_devout_set_presets(AgsSoundcard *soundcard,
				       guint channels,
				       guint rate,
				       guint buffer_size,
				       guint format);
void ags_audio_unit_devout_get_presets(AgsSoundcard *soundcard,
				       guint *channels,
				       guint *rate,
				       guint *buffer_size,
				       guint *format);

void ags_audio_unit_devout_list_cards(AgsSoundcard *soundcard,
				      GList **card_id, GList **card_name);
void ags_audio_unit_devout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
				    guint *channels_min, guint *channels_max,
				    guint *rate_min, guint *rate_max,
				    guint *buffer_size_min, guint *buffer_size_max,
				    GError **error);
guint ags_audio_unit_devout_get_capability(AgsSoundcard *soundcard);

gboolean ags_audio_unit_devout_is_starting(AgsSoundcard *soundcard);
gboolean ags_audio_unit_devout_is_playing(AgsSoundcard *soundcard);

gchar* ags_audio_unit_devout_get_uptime(AgsSoundcard *soundcard);

void ags_audio_unit_devout_port_init(AgsSoundcard *soundcard,
				     GError **error);
void ags_audio_unit_devout_port_play(AgsSoundcard *soundcard,
				     GError **error);
void ags_audio_unit_devout_port_free(AgsSoundcard *soundcard);

void ags_audio_unit_devout_tic(AgsSoundcard *soundcard);
void ags_audio_unit_devout_offset_changed(AgsSoundcard *soundcard,
					  guint note_offset);

void ags_audio_unit_devout_set_bpm(AgsSoundcard *soundcard,
				   gdouble bpm);
gdouble ags_audio_unit_devout_get_bpm(AgsSoundcard *soundcard);

void ags_audio_unit_devout_set_delay_factor(AgsSoundcard *soundcard,
					    gdouble delay_factor);
gdouble ags_audio_unit_devout_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_audio_unit_devout_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_audio_unit_devout_get_delay(AgsSoundcard *soundcard);
guint ags_audio_unit_devout_get_attack(AgsSoundcard *soundcard);

void* ags_audio_unit_devout_get_buffer(AgsSoundcard *soundcard);
void* ags_audio_unit_devout_get_next_buffer(AgsSoundcard *soundcard);
void* ags_audio_unit_devout_get_prev_buffer(AgsSoundcard *soundcard);

void ags_audio_unit_devout_lock_buffer(AgsSoundcard *soundcard,
				       void *buffer);
void ags_audio_unit_devout_unlock_buffer(AgsSoundcard *soundcard,
					 void *buffer);

guint ags_audio_unit_devout_get_delay_counter(AgsSoundcard *soundcard);

void ags_audio_unit_devout_set_start_note_offset(AgsSoundcard *soundcard,
						 guint start_note_offset);
guint ags_audio_unit_devout_get_start_note_offset(AgsSoundcard *soundcard);

void ags_audio_unit_devout_set_note_offset(AgsSoundcard *soundcard,
					   guint note_offset);
guint ags_audio_unit_devout_get_note_offset(AgsSoundcard *soundcard);

void ags_audio_unit_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
						    guint note_offset);
guint ags_audio_unit_devout_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_audio_unit_devout_set_loop(AgsSoundcard *soundcard,
				    guint loop_left, guint loop_right,
				    gboolean do_loop);
void ags_audio_unit_devout_get_loop(AgsSoundcard *soundcard,
				    guint *loop_left, guint *loop_right,
				    gboolean *do_loop);

guint ags_audio_unit_devout_get_loop_offset(AgsSoundcard *soundcard);

guint ags_audio_unit_devout_get_sub_block_count(AgsSoundcard *soundcard);

gboolean ags_audio_unit_devout_trylock_sub_block(AgsSoundcard *soundcard,
						 void *buffer, guint sub_block);
void ags_audio_unit_devout_unlock_sub_block(AgsSoundcard *soundcard,
					    void *buffer, guint sub_block);

/**
 * SECTION:ags_audio_unit_devout
 * @short_description: Output to soundcard
 * @title: AgsAudioUnitDevout
 * @section_id:
 * @include: ags/audio/audio-unit/ags_audio_unit_devout.h
 *
 * #AgsAudioUnitDevout represents a soundcard and supports output.
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
  PROP_AUDIO_UNIT_CLIENT,
  PROP_AUDIO_UNIT_PORT,
  PROP_CHANNEL,
};

static gpointer ags_audio_unit_devout_parent_class = NULL;

GType
ags_audio_unit_devout_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_unit_devout = 0;
 
    static const GTypeInfo ags_audio_unit_devout_info = {
      sizeof(AgsAudioUnitDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_unit_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAudioUnitDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_unit_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_unit_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_audio_unit_devout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_unit_devout = g_type_register_static(G_TYPE_OBJECT,
							"AgsAudioUnitDevout",
							&ags_audio_unit_devout_info,
							0);

    g_type_add_interface_static(ags_type_audio_unit_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_audio_unit_devout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_unit_devout);
  }

  return g_define_type_id__volatile;
}

void
ags_audio_unit_devout_class_init(AgsAudioUnitDevoutClass *audio_unit_devout)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_audio_unit_devout_parent_class = g_type_class_peek_parent(audio_unit_devout);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_unit_devout;

  gobject->set_property = ags_audio_unit_devout_set_property;
  gobject->get_property = ags_audio_unit_devout_get_property;

  gobject->dispose = ags_audio_unit_devout_dispose;
  gobject->finalize = ags_audio_unit_devout_finalize;

  /* properties */
  /**
   * AgsAudioUnitDevout:device:
   *
   * The audio unit soundcard indentifier
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-audio-unit-devout-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsAudioUnitDevout:dsp-channels:
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
   * AgsAudioUnitDevout:pcm-channels:
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
   * AgsAudioUnitDevout:format:
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
   * AgsAudioUnitDevout:buffer-size:
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
   * AgsAudioUnitDevout:samplerate:
   *
   * The samplerate
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("samplerate",
				 i18n_pspec("frames per second"),
				 i18n_pspec("The frames count played during a second"),
				 8000,
				 96000,
				 44100,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SAMPLERATE,
				  param_spec);

  /**
   * AgsAudioUnitDevout:buffer:
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
   * AgsAudioUnitDevout:bpm:
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
   * AgsAudioUnitDevout:delay-factor:
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
   * AgsAudioUnitDevout:attack:
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
   * AgsAudioUnitDevout:audio-unit-client:
   *
   * The assigned #AgsAudioUnitClient
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("audio-unit-client",
				   i18n_pspec("audio unit client object"),
				   i18n_pspec("The audio unit client object"),
				   AGS_TYPE_AUDIO_UNIT_CLIENT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_UNIT_CLIENT,
				  param_spec);

  /**
   * AgsAudioUnitDevout:audio-unit-port:
   *
   * The assigned #AgsAudioUnitPort
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("audio-unit-port",
				    i18n_pspec("audio unit port object"),
				    i18n_pspec("The audio unit port object"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_UNIT_PORT,
				  param_spec);
}

GQuark
ags_audio_unit_devout_error_quark()
{
  return(g_quark_from_static_string("ags-audio_unit_devout-error-quark"));
}

void
ags_audio_unit_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_audio_unit_devout_get_uuid;
  connectable->has_resource = ags_audio_unit_devout_has_resource;

  connectable->is_ready = ags_audio_unit_devout_is_ready;
  connectable->add_to_registry = ags_audio_unit_devout_add_to_registry;
  connectable->remove_from_registry = ags_audio_unit_devout_remove_from_registry;

  connectable->list_resource = ags_audio_unit_devout_list_resource;
  connectable->xml_compose = ags_audio_unit_devout_xml_compose;
  connectable->xml_parse = ags_audio_unit_devout_xml_parse;

  connectable->is_connected = ags_audio_unit_devout_is_connected;  
  connectable->connect = ags_audio_unit_devout_connect;
  connectable->disconnect = ags_audio_unit_devout_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_audio_unit_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_audio_unit_devout_set_device;
  soundcard->get_device = ags_audio_unit_devout_get_device;
  
  soundcard->set_presets = ags_audio_unit_devout_set_presets;
  soundcard->get_presets = ags_audio_unit_devout_get_presets;

  soundcard->list_cards = ags_audio_unit_devout_list_cards;
  soundcard->pcm_info = ags_audio_unit_devout_pcm_info;
  soundcard->get_capability = ags_audio_unit_devout_get_capability;

  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_audio_unit_devout_is_starting;
  soundcard->is_playing = ags_audio_unit_devout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_audio_unit_devout_get_uptime;
  
  soundcard->play_init = ags_audio_unit_devout_port_init;
  soundcard->play = ags_audio_unit_devout_port_play;

  soundcard->record_init = NULL;
  soundcard->record = NULL;

  soundcard->stop = ags_audio_unit_devout_port_free;

  soundcard->tic = ags_audio_unit_devout_tic;
  soundcard->offset_changed = ags_audio_unit_devout_offset_changed;
    
  soundcard->set_bpm = ags_audio_unit_devout_set_bpm;
  soundcard->get_bpm = ags_audio_unit_devout_get_bpm;

  soundcard->set_delay_factor = ags_audio_unit_devout_set_delay_factor;
  soundcard->get_delay_factor = ags_audio_unit_devout_get_delay_factor;
  
  soundcard->get_absolute_delay = ags_audio_unit_devout_get_absolute_delay;

  soundcard->get_delay = ags_audio_unit_devout_get_delay;
  soundcard->get_attack = ags_audio_unit_devout_get_attack;

  soundcard->get_buffer = ags_audio_unit_devout_get_buffer;
  soundcard->get_next_buffer = ags_audio_unit_devout_get_next_buffer;
  soundcard->get_prev_buffer = ags_audio_unit_devout_get_prev_buffer;

  soundcard->lock_buffer = ags_audio_unit_devout_lock_buffer;
  soundcard->unlock_buffer = ags_audio_unit_devout_unlock_buffer;

  soundcard->get_delay_counter = ags_audio_unit_devout_get_delay_counter;

  soundcard->set_start_note_offset = ags_audio_unit_devout_set_start_note_offset;
  soundcard->get_start_note_offset = ags_audio_unit_devout_get_start_note_offset;

  soundcard->set_note_offset = ags_audio_unit_devout_set_note_offset;
  soundcard->get_note_offset = ags_audio_unit_devout_get_note_offset;

  soundcard->set_note_offset_absolute = ags_audio_unit_devout_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_audio_unit_devout_get_note_offset_absolute;

  soundcard->set_loop = ags_audio_unit_devout_set_loop;
  soundcard->get_loop = ags_audio_unit_devout_get_loop;

  soundcard->get_loop_offset = ags_audio_unit_devout_get_loop_offset;

  soundcard->get_sub_block_count = ags_audio_unit_devout_get_sub_block_count;

  soundcard->trylock_sub_block = ags_audio_unit_devout_trylock_sub_block;
  soundcard->unlock_sub_block = ags_audio_unit_devout_unlock_sub_block;
}

void
ags_audio_unit_devout_init(AgsAudioUnitDevout *audio_unit_devout)
{
  AgsConfig *config;

  gchar *str;
  gchar *segmentation;

  guint denominator, numerator;
  guint i;
  
  /* flags */
  audio_unit_devout->flags = 0;
  g_atomic_int_set(&(audio_unit_devout->sync_flags),
		   AGS_AUDIO_UNIT_DEVOUT_PASS_THROUGH);

  /* devout mutex */
  g_rec_mutex_init(&(audio_unit_devout->obj_mutex));

  /* uuid */
  audio_unit_devout->uuid = ags_uuid_alloc();
  ags_uuid_generate(audio_unit_devout->uuid);

  /* presets */
  config = ags_config_get_instance();

  audio_unit_devout->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  audio_unit_devout->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  audio_unit_devout->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  audio_unit_devout->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  audio_unit_devout->format = ags_soundcard_helper_config_get_format(config);

  /*  */
  audio_unit_devout->card_uri = NULL;
  audio_unit_devout->audio_unit_client = NULL;

  audio_unit_devout->port_name = NULL;
  audio_unit_devout->audio_unit_port = NULL;

  /* buffer */
  audio_unit_devout->buffer_mutex = (GRecMutex **) malloc(8 * sizeof(GRecMutex *));

  for(i = 0; i < 8; i++){
    audio_unit_devout->buffer_mutex[i] = (GRecMutex *) malloc(sizeof(GRecMutex));

    g_rec_mutex_init(audio_unit_devout->buffer_mutex[i]);
  }

  audio_unit_devout->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  audio_unit_devout->sub_block_mutex = (GRecMutex **) malloc(8 * audio_unit_devout->sub_block_count * audio_unit_devout->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < 8 * audio_unit_devout->sub_block_count * audio_unit_devout->pcm_channels; i++){
    audio_unit_devout->sub_block_mutex[i] = (GRecMutex *) malloc(sizeof(GRecMutex));

    g_rec_mutex_init(audio_unit_devout->sub_block_mutex[i]);
  }

  audio_unit_devout->buffer = (void **) malloc(8 * sizeof(void*));

  audio_unit_devout->buffer[0] = NULL;
  audio_unit_devout->buffer[1] = NULL;
  audio_unit_devout->buffer[2] = NULL;
  audio_unit_devout->buffer[3] = NULL;
  audio_unit_devout->buffer[4] = NULL;
  audio_unit_devout->buffer[5] = NULL;
  audio_unit_devout->buffer[6] = NULL;
  audio_unit_devout->buffer[7] = NULL;
  
  ags_audio_unit_devout_realloc_buffer(audio_unit_devout);
  
  /* bpm */
  audio_unit_devout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  audio_unit_devout->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    audio_unit_devout->delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }

  /* delay and attack */
  audio_unit_devout->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
						sizeof(gdouble));
  
  audio_unit_devout->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					       sizeof(guint));

  ags_audio_unit_devout_adjust_delay_and_attack(audio_unit_devout);
  
  /* counters */
  audio_unit_devout->tact_counter = 0.0;
  audio_unit_devout->delay_counter = 0.0;
  audio_unit_devout->tic_counter = 0;

  audio_unit_devout->start_note_offset = 0;
  audio_unit_devout->note_offset = 0;
  audio_unit_devout->note_offset_absolute = 0;

  audio_unit_devout->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  audio_unit_devout->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  audio_unit_devout->do_loop = FALSE;

  audio_unit_devout->loop_offset = 0;

  /* callback mutex */
  g_mutex_init(&(audio_unit_devout->callback_mutex));

  g_cond_init(&(audio_unit_devout->callback_cond));

  /* callback finish mutex */
  g_mutex_init(&(audio_unit_devout->callback_finish_mutex));

  g_cond_init(&(audio_unit_devout->callback_finish_cond));
}

void
ags_audio_unit_devout_set_property(GObject *gobject,
				   guint prop_id,
				   const GValue *value,
				   GParamSpec *param_spec)
{
  AgsAudioUnitDevout *audio_unit_devout;
  
  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(gobject);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      audio_unit_devout->card_uri = g_strdup(device);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      if(dsp_channels == audio_unit_devout->dsp_channels){
	g_rec_mutex_unlock(audio_unit_devout_mutex);

	return;
      }

      audio_unit_devout->dsp_channels = dsp_channels;

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels, old_pcm_channels;
      guint i;

      pcm_channels = g_value_get_uint(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      if(pcm_channels == audio_unit_devout->pcm_channels){
	g_rec_mutex_unlock(audio_unit_devout_mutex);

	return;
      }

      old_pcm_channels = audio_unit_devout->pcm_channels;

      /* destroy if less pcm-channels */
      for(i = 8 * audio_unit_devout->sub_block_count * pcm_channels; i < 8 * audio_unit_devout->sub_block_count * old_pcm_channels; i++){
	g_rec_mutex_clear(audio_unit_devout->sub_block_mutex[i]);

	free(audio_unit_devout->sub_block_mutex[i]);
      }

      audio_unit_devout->sub_block_mutex = (GRecMutex **) realloc(audio_unit_devout->sub_block_mutex,
									8 * audio_unit_devout->sub_block_count * pcm_channels * sizeof(GRecMutex *));

      /* create if more pcm-channels */
      for(i = 8 * audio_unit_devout->sub_block_count * old_pcm_channels; i < 8 * audio_unit_devout->sub_block_count * pcm_channels; i++){
	audio_unit_devout->sub_block_mutex[i] = (GRecMutex *) malloc(sizeof(GRecMutex));

	g_rec_mutex_init(audio_unit_devout->sub_block_mutex[i]);
      }

      audio_unit_devout->pcm_channels = pcm_channels;

      g_rec_mutex_unlock(audio_unit_devout_mutex);

      ags_audio_unit_devout_realloc_buffer(audio_unit_devout);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      if(format == audio_unit_devout->format){
	g_rec_mutex_unlock(audio_unit_devout_mutex);

	return;
      }

      audio_unit_devout->format = format;

      g_rec_mutex_unlock(audio_unit_devout_mutex);

      ags_audio_unit_devout_realloc_buffer(audio_unit_devout);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      if(buffer_size == audio_unit_devout->buffer_size){
	g_rec_mutex_unlock(audio_unit_devout_mutex);

	return;
      }

      audio_unit_devout->buffer_size = buffer_size;

      g_rec_mutex_unlock(audio_unit_devout_mutex);

      ags_audio_unit_devout_realloc_buffer(audio_unit_devout);
      ags_audio_unit_devout_adjust_delay_and_attack(audio_unit_devout);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);
      
      if(samplerate == audio_unit_devout->samplerate){
	g_rec_mutex_unlock(audio_unit_devout_mutex);

	return;
      }

      audio_unit_devout->samplerate = samplerate;

      g_rec_mutex_unlock(audio_unit_devout_mutex);

      ags_audio_unit_devout_realloc_buffer(audio_unit_devout);
      ags_audio_unit_devout_adjust_delay_and_attack(audio_unit_devout);
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

      g_rec_mutex_lock(audio_unit_devout_mutex);

      audio_unit_devout->bpm = bpm;

      g_rec_mutex_unlock(audio_unit_devout_mutex);

      ags_audio_unit_devout_adjust_delay_and_attack(audio_unit_devout);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      audio_unit_devout->delay_factor = delay_factor;

      g_rec_mutex_unlock(audio_unit_devout_mutex);

      ags_audio_unit_devout_adjust_delay_and_attack(audio_unit_devout);
    }
    break;
  case PROP_AUDIO_UNIT_CLIENT:
    {
      AgsAudioUnitClient *audio_unit_client;

      audio_unit_client = (AgsAudioUnitClient *) g_value_get_object(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      if(audio_unit_devout->audio_unit_client == (GObject *) audio_unit_client){
	g_rec_mutex_unlock(audio_unit_devout_mutex);

	return;
      }

      if(audio_unit_devout->audio_unit_client != NULL){
	g_object_unref(G_OBJECT(audio_unit_devout->audio_unit_client));
      }

      if(audio_unit_client != NULL){
	g_object_ref(audio_unit_client);
      }
      
      audio_unit_devout->audio_unit_client = (GObject *) audio_unit_client;

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_AUDIO_UNIT_PORT:
    {
      AgsAudioUnitPort *audio_unit_port;

      audio_unit_port = (AgsAudioUnitPort *) g_value_get_pointer(value);

      g_rec_mutex_lock(audio_unit_devout_mutex);

      if(!AGS_IS_AUDIO_UNIT_PORT(audio_unit_port) ||
	 g_list_find(audio_unit_devout->audio_unit_port, audio_unit_port) != NULL){
	g_rec_mutex_unlock(audio_unit_devout_mutex);

	return;
      }

      g_object_ref(audio_unit_port);
      audio_unit_devout->audio_unit_port = g_list_append(audio_unit_devout->audio_unit_port,
					     audio_unit_port);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_devout_get_property(GObject *gobject,
				   guint prop_id,
				   GValue *value,
				   GParamSpec *param_spec)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(gobject);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_string(value, audio_unit_devout->card_uri);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_uint(value, audio_unit_devout->dsp_channels);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_uint(value, audio_unit_devout->pcm_channels);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_uint(value, audio_unit_devout->format);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_uint(value, audio_unit_devout->buffer_size);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_uint(value, audio_unit_devout->samplerate);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_pointer(value, audio_unit_devout->buffer);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_double(value, audio_unit_devout->bpm);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_double(value, audio_unit_devout->delay_factor);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_ATTACK:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_pointer(value, audio_unit_devout->attack);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_AUDIO_UNIT_CLIENT:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_object(value, audio_unit_devout->audio_unit_client);

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  case PROP_AUDIO_UNIT_PORT:
    {
      g_rec_mutex_lock(audio_unit_devout_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(audio_unit_devout->audio_unit_port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(audio_unit_devout_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_devout_dispose(GObject *gobject)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GList *list;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(gobject);

  /* audio_unit client */
  if(audio_unit_devout->audio_unit_client != NULL){
    g_object_unref(audio_unit_devout->audio_unit_client);

    audio_unit_devout->audio_unit_client = NULL;
  }

  /* audio_unit port */
  g_list_free_full(audio_unit_devout->audio_unit_port,
		   g_object_unref);

  audio_unit_devout->audio_unit_port = NULL;

  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_devout_parent_class)->dispose(gobject);
}

void
ags_audio_unit_devout_finalize(GObject *gobject)
{
  AgsAudioUnitDevout *audio_unit_devout;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(gobject);

  /* free output buffer */
  free(audio_unit_devout->buffer[0]);
  free(audio_unit_devout->buffer[1]);
  free(audio_unit_devout->buffer[2]);
  free(audio_unit_devout->buffer[3]);
  free(audio_unit_devout->buffer[4]);
  free(audio_unit_devout->buffer[5]);
  free(audio_unit_devout->buffer[6]);
  free(audio_unit_devout->buffer[7]);

  /* free buffer array */
  free(audio_unit_devout->buffer);

  /* free AgsAttack */
  free(audio_unit_devout->attack);

  /* audio_unit client */
  if(audio_unit_devout->audio_unit_client != NULL){
    g_object_unref(audio_unit_devout->audio_unit_client);
  }

  /* audio_unit port */
  g_list_free_full(audio_unit_devout->audio_unit_port,
		   g_object_unref);

  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_devout_parent_class)->finalize(gobject);
}

AgsUUID*
ags_audio_unit_devout_get_uuid(AgsConnectable *connectable)
{
  AgsAudioUnitDevout *audio_unit_devout;
  
  AgsUUID *ptr;

  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(connectable);

  /* get audio_unit devout signal mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get UUID */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  ptr = audio_unit_devout->uuid;

  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  return(ptr);
}

gboolean
ags_audio_unit_devout_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_audio_unit_devout_is_ready(AgsConnectable *connectable)
{
  AgsAudioUnitDevout *audio_unit_devout;
  
  gboolean is_ready;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(connectable);

  /* check is added */
  is_ready = ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_audio_unit_devout_add_to_registry(AgsConnectable *connectable)
{
  AgsAudioUnitDevout *audio_unit_devout;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(connectable);

  ags_audio_unit_devout_set_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_ADDED_TO_REGISTRY);
}

void
ags_audio_unit_devout_remove_from_registry(AgsConnectable *connectable)
{
  AgsAudioUnitDevout *audio_unit_devout;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(connectable);

  ags_audio_unit_devout_unset_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_ADDED_TO_REGISTRY);
}

xmlNode*
ags_audio_unit_devout_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_audio_unit_devout_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_audio_unit_devout_xml_parse(AgsConnectable *connectable,
		      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_audio_unit_devout_is_connected(AgsConnectable *connectable)
{
  AgsAudioUnitDevout *audio_unit_devout;
  
  gboolean is_connected;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(connectable);

  /* check is connected */
  is_connected = ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_CONNECTED);
  
  return(is_connected);
}

void
ags_audio_unit_devout_connect(AgsConnectable *connectable)
{
  AgsAudioUnitDevout *audio_unit_devout;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(connectable);

  ags_audio_unit_devout_set_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_CONNECTED);
}

void
ags_audio_unit_devout_disconnect(AgsConnectable *connectable)
{

  AgsAudioUnitDevout *audio_unit_devout;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(connectable);
  
  ags_audio_unit_devout_unset_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_CONNECTED);
}

/**
 * ags_audio_unit_devout_test_flags:
 * @audio_unit_devout: the #AgsAudioUnitDevout
 * @flags: the flags
 *
 * Test @flags to be set on @audio_unit_devout.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_audio_unit_devout_test_flags(AgsAudioUnitDevout *audio_unit_devout, guint flags)
{
  gboolean retval;  
  
  GRecMutex *audio_unit_devout_mutex;

  if(!AGS_IS_AUDIO_UNIT_DEVOUT(audio_unit_devout)){
    return(FALSE);
  }

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* test */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  retval = (flags & (audio_unit_devout->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(retval);
}

/**
 * ags_audio_unit_devout_set_flags:
 * @audio_unit_devout: the #AgsAudioUnitDevout
 * @flags: see #AgsAudioUnitDevoutFlags-enum
 *
 * Enable a feature of @audio_unit_devout.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_devout_set_flags(AgsAudioUnitDevout *audio_unit_devout, guint flags)
{
  GRecMutex *audio_unit_devout_mutex;

  if(!AGS_IS_AUDIO_UNIT_DEVOUT(audio_unit_devout)){
    return;
  }

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->flags |= flags;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);
}
    
/**
 * ags_audio_unit_devout_unset_flags:
 * @audio_unit_devout: the #AgsAudioUnitDevout
 * @flags: see #AgsAudioUnitDevoutFlags-enum
 *
 * Disable a feature of @audio_unit_devout.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_devout_unset_flags(AgsAudioUnitDevout *audio_unit_devout, guint flags)
{  
  GRecMutex *audio_unit_devout_mutex;

  if(!AGS_IS_AUDIO_UNIT_DEVOUT(audio_unit_devout)){
    return;
  }

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->flags &= (~flags);
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

void
ags_audio_unit_devout_set_device(AgsSoundcard *soundcard,
				 gchar *device)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GList *audio_unit_port, *audio_unit_port_start;

  gchar *str;

  guint pcm_channels;
  int ret;
  guint nth_card;
  guint i;
  
  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* check device */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  if(audio_unit_devout->card_uri == device ||
     !g_ascii_strcasecmp(audio_unit_devout->card_uri,
			 device)){
    g_rec_mutex_unlock(audio_unit_devout_mutex);
  
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-audio_unit-devout-")){
    g_rec_mutex_unlock(audio_unit_devout_mutex);

    g_warning("invalid audio_unitaudio device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-audio_unit-devout-%u",
	       &nth_card);

  if(ret != 1){
    g_rec_mutex_unlock(audio_unit_devout_mutex);

    g_warning("invalid audio_unitaudio device specifier");

    return;
  }

  g_free(audio_unit_devout->card_uri);
  audio_unit_devout->card_uri = g_strdup(device);

  /* apply name to port */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_port_start = 
    audio_unit_port = g_list_copy(audio_unit_devout->audio_unit_port);

  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  str = g_strdup_printf("ags-soundcard%d",
			nth_card);
    
  g_object_set(audio_unit_port->data,
	       "port-name", str,
	       NULL);
  g_free(str);

  g_list_free(audio_unit_port_start);
}

gchar*
ags_audio_unit_devout_get_device(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;
  
  gchar *device;

  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  device = NULL;

  /* get device */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  device = g_strdup(audio_unit_devout->card_uri);

  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  return(device);
}

void
ags_audio_unit_devout_set_presets(AgsSoundcard *soundcard,
				  guint channels,
				  guint rate,
				  guint buffer_size,
				  guint format)
{
  AgsAudioUnitDevout *audio_unit_devout;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  g_object_set(audio_unit_devout,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_audio_unit_devout_get_presets(AgsSoundcard *soundcard,
				  guint *channels,
				  guint *rate,
				  guint *buffer_size,
				  guint *format)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get presets */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  if(channels != NULL){
    *channels = audio_unit_devout->pcm_channels;
  }

  if(rate != NULL){
    *rate = audio_unit_devout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = audio_unit_devout->buffer_size;
  }

  if(format != NULL){
    *format = audio_unit_devout->format;
  }

  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

void
ags_audio_unit_devout_list_cards(AgsSoundcard *soundcard,
				 GList **card_id, GList **card_name)
{
  AgsAudioUnitClient *audio_unit_client;
  AgsAudioUnitDevout *audio_unit_devout;

  AgsApplicationContext *application_context;
  
  GList *list_start, *list;

  gchar *card_uri;
  gchar *client_name;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

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
    if(AGS_IS_AUDIO_UNIT_DEVOUT(list->data)){
      if(card_id != NULL){
	card_uri = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));
	
	if(AGS_AUDIO_UNIT_DEVOUT(list->data)->card_uri != NULL){
	  *card_id = g_list_prepend(*card_id,
				    card_uri);
	}else{
	  *card_id = g_list_prepend(*card_id,
				    g_strdup("(null)"));

	  g_warning("ags_audio_unit_devout_list_cards() - card id (null)");
	}
      }

      if(card_name != NULL){
	g_object_get(list->data,
		     "audio_unit-client", &audio_unit_client,
		     NULL);
	
	if(audio_unit_client != NULL){
	  /* get client name */
	  g_object_get(audio_unit_client,
		       "client-name", &client_name,
		       NULL);	  
	  
	  *card_name = g_list_prepend(*card_name,
				      client_name);

	  g_object_unref(audio_unit_client);
	}else{
	  *card_name = g_list_prepend(*card_name,
				      g_strdup("(null)"));

	  g_warning("ags_audio_unit_devout_list_cards() - audio_unitaudio client not connected (null)");
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
ags_audio_unit_devout_pcm_info(AgsSoundcard *soundcard,
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
ags_audio_unit_devout_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_PLAYBACK);
}

gboolean
ags_audio_unit_devout_is_starting(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  gboolean is_starting;
  
  GRecMutex *audio_unit_devout_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* check is starting */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  is_starting = ((AGS_AUDIO_UNIT_DEVOUT_START_PLAY & (audio_unit_devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  return(is_starting);
}

gboolean
ags_audio_unit_devout_is_playing(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  gboolean is_playing;
  
  GRecMutex *audio_unit_devout_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* check is starting */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  is_playing = ((AGS_AUDIO_UNIT_DEVOUT_PLAY & (audio_unit_devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(is_playing);
}

gchar*
ags_audio_unit_devout_get_uptime(AgsSoundcard *soundcard)
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
ags_audio_unit_devout_port_init(AgsSoundcard *soundcard,
				GError **error)
{
  AgsAudioUnitPort *audio_unit_port;
  AgsAudioUnitDevout *audio_unit_devout;

  guint format, word_size;

  GRecMutex *audio_unit_port_mutex;
  GRecMutex *audio_unit_devout_mutex;
  
  if(ags_soundcard_is_playing(soundcard)){
    return;
  }

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio-unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* port */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_port = (AgsAudioUnitPort *) audio_unit_devout->audio_unit_port->data;

  g_rec_mutex_unlock(audio_unit_devout_mutex);

  /* get port mutex */
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);

  /* retrieve word size */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  switch(audio_unit_devout->format){
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
    g_rec_mutex_unlock(audio_unit_devout_mutex);
    
    g_warning("ags_audio_unit_devout_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for playback */
  audio_unit_devout->flags |= (AGS_AUDIO_UNIT_DEVOUT_BUFFER7 |
			       AGS_AUDIO_UNIT_DEVOUT_START_PLAY |
			       AGS_AUDIO_UNIT_DEVOUT_PLAY |
			       AGS_AUDIO_UNIT_DEVOUT_NONBLOCKING);

  memset(audio_unit_devout->buffer[0], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->buffer[1], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->buffer[2], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->buffer[3], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->buffer[4], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->buffer[5], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->buffer[6], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->buffer[7], 0, audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);

  /*  */
  audio_unit_devout->tact_counter = 0.0;
  audio_unit_devout->delay_counter = floor(ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(audio_unit_devout)));
  audio_unit_devout->tic_counter = 0;

  audio_unit_devout->flags |= (AGS_AUDIO_UNIT_DEVOUT_INITIALIZED |
			       AGS_AUDIO_UNIT_DEVOUT_START_PLAY |
			       AGS_AUDIO_UNIT_DEVOUT_PLAY);
  
  g_atomic_int_or(&(audio_unit_devout->sync_flags),
		  AGS_AUDIO_UNIT_DEVOUT_INITIAL_CALLBACK);

  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

void
ags_audio_unit_devout_port_play(AgsSoundcard *soundcard,
				GError **error)
{
  AgsAudioUnitClient *audio_unit_client;
  AgsAudioUnitDevout *audio_unit_devout;
  AgsAudioUnitPort *audio_unit_port;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
      
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;
  guint word_size;
  gboolean audio_unit_client_activated;

  GRecMutex *audio_unit_devout_mutex;
  GRecMutex *audio_unit_client_mutex;
  GRecMutex *audio_unit_port_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  application_context = ags_application_context_get_instance();
  
  /* get audio-unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* client */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_client = (AgsAudioUnitClient *) audio_unit_devout->audio_unit_client;
  audio_unit_port = (AgsAudioUnitPort *) audio_unit_devout->audio_unit_port->data;
  
  callback_mutex = &(audio_unit_devout->callback_mutex);
  callback_finish_mutex = &(audio_unit_devout->callback_finish_mutex);

  /* do playback */  
  audio_unit_devout->flags &= (~AGS_AUDIO_UNIT_DEVOUT_START_PLAY);

  if((AGS_AUDIO_UNIT_DEVOUT_INITIALIZED & (audio_unit_devout->flags)) == 0){
    g_rec_mutex_unlock(audio_unit_devout_mutex);
    
    return;
  }
  
  switch(audio_unit_devout->format){
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
    g_rec_mutex_unlock(audio_unit_devout_mutex);
    
    g_warning("ags_audio_unit_devout_port_play(): unsupported word size");
    
    return;
  }

  g_rec_mutex_unlock(audio_unit_devout_mutex);

  /* get client and port mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);

  /* get activated */
  g_rec_mutex_lock(audio_unit_client_mutex);

  audio_unit_client_activated = ((AGS_AUDIO_UNIT_CLIENT_ACTIVATED & (audio_unit_client->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(audio_unit_client_mutex);

  if(audio_unit_client_activated){
    while((AGS_AUDIO_UNIT_DEVOUT_PASS_THROUGH & (g_atomic_int_get(&(audio_unit_devout->sync_flags)))) != 0){
      usleep(4);
    }
    
    /* signal */
    if((AGS_AUDIO_UNIT_DEVOUT_INITIAL_CALLBACK & (g_atomic_int_get(&(audio_unit_devout->sync_flags)))) == 0){
      g_mutex_lock(callback_mutex);

      g_atomic_int_or(&(audio_unit_devout->sync_flags),
		      AGS_AUDIO_UNIT_DEVOUT_CALLBACK_DONE);
    
      if((AGS_AUDIO_UNIT_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(audio_unit_devout->sync_flags)))) != 0){
	g_cond_signal(&(audio_unit_devout->callback_cond));
      }

      g_mutex_unlock(callback_mutex);
      //    }
    
      /* wait callback */	
      g_mutex_lock(callback_finish_mutex);
    
      if((AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(audio_unit_devout->sync_flags)))) == 0){
	g_atomic_int_or(&(audio_unit_devout->sync_flags),
			AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_WAIT);
    
	while((AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(audio_unit_devout->sync_flags)))) == 0 &&
	      (AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(audio_unit_devout->sync_flags)))) != 0){
	  g_cond_wait(&(audio_unit_devout->callback_finish_cond),
			    callback_finish_mutex);
	}
      }
      
      g_atomic_int_and(&(audio_unit_devout->sync_flags),
		       (~(AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_WAIT |
			  AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_DONE)));
    
      g_mutex_unlock(callback_finish_mutex);
    }else{
      g_atomic_int_and(&(audio_unit_devout->sync_flags),
		       (~AGS_AUDIO_UNIT_DEVOUT_INITIAL_CALLBACK));
    }
  }

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;      
  
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) audio_unit_devout);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) audio_unit_devout);
  task = g_list_append(task,
		       clear_buffer);
    
  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) audio_unit_devout);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_audio_unit_devout_port_free(AgsSoundcard *soundcard)
{
  AgsAudioUnitPort *audio_unit_port;
  AgsAudioUnitDevout *audio_unit_devout;

  guint pcm_channels;
  guint word_size;

  GRecMutex *audio_unit_port_mutex;
  GRecMutex *audio_unit_devout_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  /* get audio-unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* port */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_port = (AgsAudioUnitPort *) audio_unit_devout->audio_unit_port->data;
  pcm_channels = audio_unit_devout->pcm_channels;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

  /* get port mutex */
  audio_unit_port_mutex = AGS_AUDIO_UNIT_PORT_GET_OBJ_MUTEX(audio_unit_port);

  /*  */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  if((AGS_AUDIO_UNIT_DEVOUT_INITIALIZED & (audio_unit_devout->flags)) == 0){
    g_rec_mutex_unlock(audio_unit_devout_mutex);

    return;
  }

  callback_mutex = &(audio_unit_devout->callback_mutex);
  callback_finish_mutex = &(audio_unit_devout->callback_finish_mutex);
  
  //  g_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);
  
  audio_unit_devout->flags &= (~(AGS_AUDIO_UNIT_DEVOUT_BUFFER0 |
				 AGS_AUDIO_UNIT_DEVOUT_BUFFER1 |
				 AGS_AUDIO_UNIT_DEVOUT_BUFFER2 |
				 AGS_AUDIO_UNIT_DEVOUT_BUFFER3 |
				 AGS_AUDIO_UNIT_DEVOUT_BUFFER4 |
				 AGS_AUDIO_UNIT_DEVOUT_BUFFER5 |
				 AGS_AUDIO_UNIT_DEVOUT_BUFFER6 |
				 AGS_AUDIO_UNIT_DEVOUT_BUFFER7 |
				 AGS_AUDIO_UNIT_DEVOUT_PLAY));

  g_atomic_int_or(&(audio_unit_devout->sync_flags),
		  AGS_AUDIO_UNIT_DEVOUT_PASS_THROUGH);
  g_atomic_int_and(&(audio_unit_devout->sync_flags),
		   (~AGS_AUDIO_UNIT_DEVOUT_INITIAL_CALLBACK));

  /* signal callback */
  g_mutex_lock(callback_mutex);

  g_atomic_int_or(&(audio_unit_devout->sync_flags),
		  AGS_AUDIO_UNIT_DEVOUT_CALLBACK_DONE);
    
  if((AGS_AUDIO_UNIT_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(audio_unit_devout->sync_flags)))) != 0){
    g_cond_signal(&(audio_unit_devout->callback_cond));
  }

  g_mutex_unlock(callback_mutex);

  /* signal thread */
  g_mutex_lock(callback_finish_mutex);

  g_atomic_int_or(&(audio_unit_devout->sync_flags),
		  AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_DONE);
    
  if((AGS_AUDIO_UNIT_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(audio_unit_devout->sync_flags)))) != 0){
    g_cond_signal(&(audio_unit_devout->callback_finish_cond));
  }

  g_mutex_unlock(callback_finish_mutex);
  
  /*  */
  audio_unit_devout->note_offset = audio_unit_devout->start_note_offset;
  audio_unit_devout->note_offset_absolute = audio_unit_devout->start_note_offset;

  switch(audio_unit_devout->format){
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
    
    g_critical("ags_audio_unit_devout_free(): unsupported word size");
  }

  g_rec_mutex_unlock(audio_unit_devout_mutex);

  if(audio_unit_devout->audio_unit_port != NULL){
    audio_unit_port = audio_unit_devout->audio_unit_port->data;

    while(!g_atomic_int_get(&(audio_unit_port->is_empty))) usleep(500000);
  }

  g_rec_mutex_lock(audio_unit_devout_mutex);
  
  memset(audio_unit_devout->buffer[0], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->buffer[1], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->buffer[2], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->buffer[3], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->buffer[4], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->buffer[5], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->buffer[6], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);
  memset(audio_unit_devout->buffer[7], 0, (size_t) audio_unit_devout->pcm_channels * audio_unit_devout->buffer_size * word_size);

  g_rec_mutex_unlock(audio_unit_devout_mutex);  
}

void
ags_audio_unit_devout_tic(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  gdouble delay;
  gdouble delay_counter;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  
  GRecMutex *audio_unit_devout_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  delay = audio_unit_devout->delay[audio_unit_devout->tic_counter];
  delay_counter = audio_unit_devout->delay_counter;

  note_offset = audio_unit_devout->note_offset;
  note_offset_absolute = audio_unit_devout->note_offset_absolute;
  
  loop_left = audio_unit_devout->loop_left;
  loop_right = audio_unit_devout->loop_right;
  
  do_loop = audio_unit_devout->do_loop;

  g_rec_mutex_unlock(audio_unit_devout_mutex);

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
    g_rec_mutex_lock(audio_unit_devout_mutex);
    
    audio_unit_devout->delay_counter = delay_counter + 1.0 - delay;
    audio_unit_devout->tact_counter += 1.0;

    g_rec_mutex_unlock(audio_unit_devout_mutex);
  }else{
    g_rec_mutex_lock(audio_unit_devout_mutex);
    
    audio_unit_devout->delay_counter += 1.0;

    g_rec_mutex_unlock(audio_unit_devout_mutex);
  }
}

void
ags_audio_unit_devout_offset_changed(AgsSoundcard *soundcard,
			       guint note_offset)
{
  AgsAudioUnitDevout *audio_unit_devout;
  
  GRecMutex *audio_unit_devout_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* offset changed */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->tic_counter += 1;

  if(audio_unit_devout->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    audio_unit_devout->tic_counter = 0;
  }

  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

void
ags_audio_unit_devout_set_bpm(AgsSoundcard *soundcard,
			gdouble bpm)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* set bpm */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->bpm = bpm;

  g_rec_mutex_unlock(audio_unit_devout_mutex);

  ags_audio_unit_devout_adjust_delay_and_attack(audio_unit_devout);
}

gdouble
ags_audio_unit_devout_get_bpm(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  gdouble bpm;
  
  GRecMutex *audio_unit_devout_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get bpm */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  bpm = audio_unit_devout->bpm;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(bpm);
}

void
ags_audio_unit_devout_set_delay_factor(AgsSoundcard *soundcard,
				 gdouble delay_factor)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* set delay factor */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->delay_factor = delay_factor;

  g_rec_mutex_unlock(audio_unit_devout_mutex);

  ags_audio_unit_devout_adjust_delay_and_attack(audio_unit_devout);
}

gdouble
ags_audio_unit_devout_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  gdouble delay_factor;
  
  GRecMutex *audio_unit_devout_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get delay factor */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  delay_factor = audio_unit_devout->delay_factor;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(delay_factor);
}

gdouble
ags_audio_unit_devout_get_delay(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  guint delay_index;
  gdouble delay;
  
  GRecMutex *audio_unit_devout_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get delay */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  delay_index = audio_unit_devout->tic_counter;

  delay = audio_unit_devout->delay[delay_index];
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  return(delay);
}

gdouble
ags_audio_unit_devout_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  gdouble absolute_delay;
  
  GRecMutex *audio_unit_devout_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get absolute delay */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  absolute_delay = (60.0 * (((gdouble) audio_unit_devout->samplerate / (gdouble) audio_unit_devout->buffer_size) / (gdouble) audio_unit_devout->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) audio_unit_devout->delay_factor)));

  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(absolute_delay);
}

guint
ags_audio_unit_devout_get_attack(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  guint attack_index;
  guint attack;
  
  GRecMutex *audio_unit_devout_mutex;  

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get attack */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  attack_index = audio_unit_devout->tic_counter;

  attack = audio_unit_devout->attack[attack_index];

  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  return(attack);
}

void*
ags_audio_unit_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  void *buffer;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER0)){
    buffer = audio_unit_devout->buffer[0];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER1)){
    buffer = audio_unit_devout->buffer[1];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER2)){
    buffer = audio_unit_devout->buffer[2];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER3)){
    buffer = audio_unit_devout->buffer[3];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER4)){
    buffer = audio_unit_devout->buffer[4];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER5)){
    buffer = audio_unit_devout->buffer[5];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER6)){
    buffer = audio_unit_devout->buffer[6];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER7)){
    buffer = audio_unit_devout->buffer[7];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_audio_unit_devout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  void *buffer;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER0)){
    buffer = audio_unit_devout->buffer[1];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER1)){
    buffer = audio_unit_devout->buffer[2];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER2)){
    buffer = audio_unit_devout->buffer[3];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER3)){
    buffer = audio_unit_devout->buffer[4];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER4)){
    buffer = audio_unit_devout->buffer[5];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER5)){
    buffer = audio_unit_devout->buffer[6];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER6)){
    buffer = audio_unit_devout->buffer[7];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER7)){
    buffer = audio_unit_devout->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_audio_unit_devout_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  void *buffer;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER0)){
    buffer = audio_unit_devout->buffer[7];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER1)){
    buffer = audio_unit_devout->buffer[0];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER2)){
    buffer = audio_unit_devout->buffer[1];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER3)){
    buffer = audio_unit_devout->buffer[2];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER4)){
    buffer = audio_unit_devout->buffer[3];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER5)){
    buffer = audio_unit_devout->buffer[4];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER6)){
    buffer = audio_unit_devout->buffer[5];
  }else if(ags_audio_unit_devout_test_flags(audio_unit_devout, AGS_AUDIO_UNIT_DEVOUT_BUFFER7)){
    buffer = audio_unit_devout->buffer[6];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void
ags_audio_unit_devout_lock_buffer(AgsSoundcard *soundcard,
				  void *buffer)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *buffer_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(audio_unit_devout->buffer != NULL){
    if(buffer == audio_unit_devout->buffer[0]){
      buffer_mutex = audio_unit_devout->buffer_mutex[0];
    }else if(buffer == audio_unit_devout->buffer[1]){
      buffer_mutex = audio_unit_devout->buffer_mutex[1];
    }else if(buffer == audio_unit_devout->buffer[2]){
      buffer_mutex = audio_unit_devout->buffer_mutex[2];
    }else if(buffer == audio_unit_devout->buffer[3]){
      buffer_mutex = audio_unit_devout->buffer_mutex[3];
    }else if(buffer == audio_unit_devout->buffer[4]){
      buffer_mutex = audio_unit_devout->buffer_mutex[4];
    }else if(buffer == audio_unit_devout->buffer[5]){
      buffer_mutex = audio_unit_devout->buffer_mutex[5];
    }else if(buffer == audio_unit_devout->buffer[6]){
      buffer_mutex = audio_unit_devout->buffer_mutex[6];
    }else if(buffer == audio_unit_devout->buffer[7]){
      buffer_mutex = audio_unit_devout->buffer_mutex[7];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_audio_unit_devout_unlock_buffer(AgsSoundcard *soundcard,
				    void *buffer)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *buffer_mutex;
  
  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(audio_unit_devout->buffer != NULL){
    if(buffer == audio_unit_devout->buffer[0]){
      buffer_mutex = audio_unit_devout->buffer_mutex[0];
    }else if(buffer == audio_unit_devout->buffer[1]){
      buffer_mutex = audio_unit_devout->buffer_mutex[1];
    }else if(buffer == audio_unit_devout->buffer[2]){
      buffer_mutex = audio_unit_devout->buffer_mutex[2];
    }else if(buffer == audio_unit_devout->buffer[3]){
      buffer_mutex = audio_unit_devout->buffer_mutex[3];
    }else if(buffer == audio_unit_devout->buffer[4]){
      buffer_mutex = audio_unit_devout->buffer_mutex[4];
    }else if(buffer == audio_unit_devout->buffer[5]){
      buffer_mutex = audio_unit_devout->buffer_mutex[5];
    }else if(buffer == audio_unit_devout->buffer[6]){
      buffer_mutex = audio_unit_devout->buffer_mutex[6];
    }else if(buffer == audio_unit_devout->buffer[7]){
      buffer_mutex = audio_unit_devout->buffer_mutex[7];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_audio_unit_devout_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  guint delay_counter;
  
  GRecMutex *audio_unit_devout_mutex;  

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);
  
  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* delay counter */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  delay_counter = audio_unit_devout->delay_counter;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(delay_counter);
}

void
ags_audio_unit_devout_set_start_note_offset(AgsSoundcard *soundcard,
					    guint start_note_offset)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;  

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* set note offset */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

guint
ags_audio_unit_devout_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  guint start_note_offset;
  
  GRecMutex *audio_unit_devout_mutex;  

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* set note offset */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  start_note_offset = audio_unit_devout->start_note_offset;

  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(start_note_offset);
}

void
ags_audio_unit_devout_set_note_offset(AgsSoundcard *soundcard,
				      guint note_offset)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;  

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* set note offset */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->note_offset = note_offset;

  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

guint
ags_audio_unit_devout_get_note_offset(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  guint note_offset;
  
  GRecMutex *audio_unit_devout_mutex;  

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* set note offset */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  note_offset = audio_unit_devout->note_offset;

  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(note_offset);
}

void
ags_audio_unit_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
					 guint note_offset_absolute)
{
  AgsAudioUnitDevout *audio_unit_devout;
  
  GRecMutex *audio_unit_devout_mutex;  

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* set note offset */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->note_offset_absolute = note_offset_absolute;

  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

guint
ags_audio_unit_devout_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  guint note_offset_absolute;
  
  GRecMutex *audio_unit_devout_mutex;  

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* set note offset */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  note_offset_absolute = audio_unit_devout->note_offset_absolute;

  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(note_offset_absolute);
}

void
ags_audio_unit_devout_set_loop(AgsSoundcard *soundcard,
			       guint loop_left, guint loop_right,
			       gboolean do_loop)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;  

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* set loop */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  audio_unit_devout->loop_left = loop_left;
  audio_unit_devout->loop_right = loop_right;
  audio_unit_devout->do_loop = do_loop;

  if(do_loop){
    audio_unit_devout->loop_offset = audio_unit_devout->note_offset;
  }

  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

void
ags_audio_unit_devout_get_loop(AgsSoundcard *soundcard,
			       guint *loop_left, guint *loop_right,
			       gboolean *do_loop)
{
  AgsAudioUnitDevout *audio_unit_devout;

  GRecMutex *audio_unit_devout_mutex;  

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get loop */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  if(loop_left != NULL){
    *loop_left = audio_unit_devout->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = audio_unit_devout->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = audio_unit_devout->do_loop;
  }

  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

guint
ags_audio_unit_devout_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  guint loop_offset;
  
  GRecMutex *audio_unit_devout_mutex;  

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get loop offset */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  loop_offset = audio_unit_devout->loop_offset;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(loop_offset);
}

guint
ags_audio_unit_devout_get_sub_block_count(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevout *audio_unit_devout;

  guint sub_block_count;
  
  GRecMutex *audio_unit_devout_mutex;  

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get loop offset */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  sub_block_count = audio_unit_devout->sub_block_count;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

  return(sub_block_count);
}

gboolean
ags_audio_unit_devout_trylock_sub_block(AgsSoundcard *soundcard,
					void *buffer, guint sub_block)
{
  AgsAudioUnitDevout *audio_unit_devout;

  guint pcm_channels;
  guint sub_block_count;
  gboolean success;
  
  GRecMutex *audio_unit_devout_mutex;  
  GRecMutex *sub_block_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get loop offset */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  pcm_channels = audio_unit_devout->pcm_channels;
  sub_block_count = audio_unit_devout->sub_block_count;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  sub_block_mutex = NULL;

  success = FALSE;
  
  if(audio_unit_devout->buffer != NULL){
    if(buffer == audio_unit_devout->buffer[0]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[sub_block];
    }else if(buffer == audio_unit_devout->buffer[1]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->buffer[2]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->buffer[3]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->buffer[4]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[4 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->buffer[5]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[5 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->buffer[6]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[6 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->buffer[7]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[7 * pcm_channels * sub_block_count + sub_block];
    }
  }

  if(sub_block_mutex != NULL){
    if(g_rec_mutex_trylock(sub_block_mutex) == 0){
      success = TRUE;
    }
  }

  return(success);
}

void
ags_audio_unit_devout_unlock_sub_block(AgsSoundcard *soundcard,
				       void *buffer, guint sub_block)
{
  AgsAudioUnitDevout *audio_unit_devout;

  guint pcm_channels;
  guint sub_block_count;
  
  GRecMutex *audio_unit_devout_mutex;  
  GRecMutex *sub_block_mutex;

  audio_unit_devout = AGS_AUDIO_UNIT_DEVOUT(soundcard);

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get loop offset */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  pcm_channels = audio_unit_devout->pcm_channels;
  sub_block_count = audio_unit_devout->sub_block_count;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);
  
  sub_block_mutex = NULL;
  
  if(audio_unit_devout->buffer != NULL){
    if(buffer == audio_unit_devout->buffer[0]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[sub_block];
    }else if(buffer == audio_unit_devout->buffer[1]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->buffer[2]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->buffer[3]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->buffer[4]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[4 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->buffer[5]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[5 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->buffer[6]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[6 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == audio_unit_devout->buffer[7]){
      sub_block_mutex = audio_unit_devout->sub_block_mutex[7 * pcm_channels * sub_block_count + sub_block];
    }
  }

  if(sub_block_mutex != NULL){
    g_rec_mutex_unlock(sub_block_mutex);
  }
}

/**
 * ags_audio_unit_devout_switch_buffer_flag:
 * @audio_unit_devout: an #AgsAudioUnitDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_devout_switch_buffer_flag(AgsAudioUnitDevout *audio_unit_devout)
{
  GRecMutex *audio_unit_devout_mutex;
  
  if(!AGS_IS_AUDIO_UNIT_DEVOUT(audio_unit_devout)){
    return;
  }

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* switch buffer flag */
  g_rec_mutex_lock(audio_unit_devout_mutex);

  if((AGS_AUDIO_UNIT_DEVOUT_BUFFER0 & (audio_unit_devout->flags)) != 0){
    audio_unit_devout->flags &= (~AGS_AUDIO_UNIT_DEVOUT_BUFFER0);
    audio_unit_devout->flags |= AGS_AUDIO_UNIT_DEVOUT_BUFFER1;
  }else if((AGS_AUDIO_UNIT_DEVOUT_BUFFER1 & (audio_unit_devout->flags)) != 0){
    audio_unit_devout->flags &= (~AGS_AUDIO_UNIT_DEVOUT_BUFFER1);
    audio_unit_devout->flags |= AGS_AUDIO_UNIT_DEVOUT_BUFFER2;
  }else if((AGS_AUDIO_UNIT_DEVOUT_BUFFER2 & (audio_unit_devout->flags)) != 0){
    audio_unit_devout->flags &= (~AGS_AUDIO_UNIT_DEVOUT_BUFFER2);
    audio_unit_devout->flags |= AGS_AUDIO_UNIT_DEVOUT_BUFFER3;
  }else if((AGS_AUDIO_UNIT_DEVOUT_BUFFER3 & (audio_unit_devout->flags)) != 0){
    audio_unit_devout->flags &= (~AGS_AUDIO_UNIT_DEVOUT_BUFFER3);
    audio_unit_devout->flags |= AGS_AUDIO_UNIT_DEVOUT_BUFFER4;
  }else if((AGS_AUDIO_UNIT_DEVOUT_BUFFER4 & (audio_unit_devout->flags)) != 0){
    audio_unit_devout->flags &= (~AGS_AUDIO_UNIT_DEVOUT_BUFFER4);
    audio_unit_devout->flags |= AGS_AUDIO_UNIT_DEVOUT_BUFFER5;
  }else if((AGS_AUDIO_UNIT_DEVOUT_BUFFER5 & (audio_unit_devout->flags)) != 0){
    audio_unit_devout->flags &= (~AGS_AUDIO_UNIT_DEVOUT_BUFFER5);
    audio_unit_devout->flags |= AGS_AUDIO_UNIT_DEVOUT_BUFFER6;
  }else if((AGS_AUDIO_UNIT_DEVOUT_BUFFER6 & (audio_unit_devout->flags)) != 0){
    audio_unit_devout->flags &= (~AGS_AUDIO_UNIT_DEVOUT_BUFFER6);
    audio_unit_devout->flags |= AGS_AUDIO_UNIT_DEVOUT_BUFFER7;
  }else if((AGS_AUDIO_UNIT_DEVOUT_BUFFER7 & (audio_unit_devout->flags)) != 0){
    audio_unit_devout->flags &= (~AGS_AUDIO_UNIT_DEVOUT_BUFFER7);
    audio_unit_devout->flags |= AGS_AUDIO_UNIT_DEVOUT_BUFFER0;
  }

  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

/**
 * ags_audio_unit_devout_adjust_delay_and_attack:
 * @audio_unit_devout: the #AgsAudioUnitDevout
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_devout_adjust_delay_and_attack(AgsAudioUnitDevout *audio_unit_devout)
{
  gdouble delay;
  guint default_tact_frames;
  guint delay_tact_frames;
  guint default_period;
  gint next_attack;
  guint i;

  GRecMutex *audio_unit_devout_mutex;

  if(!AGS_IS_AUDIO_UNIT_DEVOUT(audio_unit_devout)){
    return;
  }

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);
  
  /* get some initial values */
  delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(audio_unit_devout));

#ifdef AGS_DEBUG
  g_message("delay : %f", delay);
#endif
  
  g_rec_mutex_lock(audio_unit_devout_mutex);

  default_tact_frames = (guint) (delay * audio_unit_devout->buffer_size);
  delay_tact_frames = (guint) (floor(delay) * audio_unit_devout->buffer_size);
  default_period = (1.0 / AGS_SOUNDCARD_DEFAULT_PERIOD) * (default_tact_frames);

  i = 0;
  
  audio_unit_devout->attack[0] = (guint) floor(0.25 * audio_unit_devout->buffer_size);
  next_attack = (((audio_unit_devout->attack[i] + default_tact_frames) / audio_unit_devout->buffer_size) - delay) * audio_unit_devout->buffer_size;

  if(next_attack < 0){
    next_attack = 0;
  }

  if(next_attack >= audio_unit_devout->buffer_size){
    next_attack = audio_unit_devout->buffer_size - 1;
  }
  
  /* check if delay drops for next attack */
  if(next_attack < 0){
    audio_unit_devout->attack[i] = audio_unit_devout->attack[i] - ((gdouble) next_attack / 2.0);

    if(audio_unit_devout->attack[i] < 0){
      audio_unit_devout->attack[i] = 0;
    }
    
    if(audio_unit_devout->attack[i] >= audio_unit_devout->buffer_size){
      audio_unit_devout->attack[i] = audio_unit_devout->buffer_size - 1;
    }
    
    next_attack = next_attack + (next_attack / 2.0);

    if(next_attack < 0){
      next_attack = 0;
    }

    if(next_attack >= audio_unit_devout->buffer_size){
      next_attack = audio_unit_devout->buffer_size - 1;
    }
  }
  
  for(i = 1; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    audio_unit_devout->attack[i] = next_attack;
    next_attack = (((audio_unit_devout->attack[i] + default_tact_frames) / audio_unit_devout->buffer_size) - delay) * audio_unit_devout->buffer_size;

    if(next_attack >= audio_unit_devout->buffer_size){
      next_attack = audio_unit_devout->buffer_size - 1;
    }
    
    /* check if delay drops for next attack */
    if(next_attack < 0){
      audio_unit_devout->attack[i] = audio_unit_devout->attack[i] - ((gdouble) next_attack / 2.0);

      if(audio_unit_devout->attack[i] < 0){
	audio_unit_devout->attack[i] = 0;
      }

      if(audio_unit_devout->attack[i] >= audio_unit_devout->buffer_size){
	audio_unit_devout->attack[i] = audio_unit_devout->buffer_size - 1;
      }
    
      next_attack = next_attack + (next_attack / 2.0);

      if(next_attack < 0){
	next_attack = 0;
      }

      if(next_attack >= audio_unit_devout->buffer_size){
	next_attack = audio_unit_devout->buffer_size - 1;
      }
    }
    
#ifdef AGS_DEBUG
    g_message("%d", audio_unit_devout->attack[i]);
#endif
  }

  audio_unit_devout->attack[0] = audio_unit_devout->attack[i - 2];
  
  for(i = 0; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD - 1; i++){
    audio_unit_devout->delay[i] = ((gdouble) (default_tact_frames + audio_unit_devout->attack[i] - audio_unit_devout->attack[i + 1])) / (gdouble) audio_unit_devout->buffer_size;
    
#ifdef AGS_DEBUG
    g_message("%f", audio_unit_devout->delay[i]);
#endif
  }

  audio_unit_devout->delay[i] = ((gdouble) (default_tact_frames + audio_unit_devout->attack[i] - audio_unit_devout->attack[0])) / (gdouble) audio_unit_devout->buffer_size;

  g_rec_mutex_unlock(audio_unit_devout_mutex);
}

/**
 * ags_audio_unit_devout_realloc_buffer:
 * @audio_unit_devout: the #AgsAudioUnitDevout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.0.0
 */
void
ags_audio_unit_devout_realloc_buffer(AgsAudioUnitDevout *audio_unit_devout)
{
  guint pcm_channels;
  guint buffer_size;
  guint format;
  guint word_size;

  GRecMutex *audio_unit_devout_mutex;  

  if(!AGS_IS_AUDIO_UNIT_DEVOUT(audio_unit_devout)){
    return;
  }

  /* get audio_unit devout mutex */
  audio_unit_devout_mutex = AGS_AUDIO_UNIT_DEVOUT_GET_OBJ_MUTEX(audio_unit_devout);

  /* get word size */  
  g_rec_mutex_lock(audio_unit_devout_mutex);

  pcm_channels = audio_unit_devout->pcm_channels;
  buffer_size = audio_unit_devout->buffer_size;

  format = audio_unit_devout->format;
  
  g_rec_mutex_unlock(audio_unit_devout_mutex);

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
    g_warning("ags_audio_unit_devout_realloc_buffer(): unsupported word size");
    return;
  }
  
  /* AGS_AUDIO_UNIT_DEVOUT_BUFFER_0 */
  if(audio_unit_devout->buffer[0] != NULL){
    free(audio_unit_devout->buffer[0]);
  }
  
  audio_unit_devout->buffer[0] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_AUDIO_UNIT_DEVOUT_BUFFER_1 */
  if(audio_unit_devout->buffer[1] != NULL){
    free(audio_unit_devout->buffer[1]);
  }

  audio_unit_devout->buffer[1] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_AUDIO_UNIT_DEVOUT_BUFFER_2 */
  if(audio_unit_devout->buffer[2] != NULL){
    free(audio_unit_devout->buffer[2]);
  }

  audio_unit_devout->buffer[2] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_AUDIO_UNIT_DEVOUT_BUFFER_3 */
  if(audio_unit_devout->buffer[3] != NULL){
    free(audio_unit_devout->buffer[3]);
  }
  
  audio_unit_devout->buffer[3] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_AUDIO_UNIT_DEVOUT_BUFFER_4 */
  if(audio_unit_devout->buffer[4] != NULL){
    free(audio_unit_devout->buffer[4]);
  }
  
  audio_unit_devout->buffer[4] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_AUDIO_UNIT_DEVOUT_BUFFER_5 */
  if(audio_unit_devout->buffer[5] != NULL){
    free(audio_unit_devout->buffer[5]);
  }
  
  audio_unit_devout->buffer[5] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_AUDIO_UNIT_DEVOUT_BUFFER_6 */
  if(audio_unit_devout->buffer[6] != NULL){
    free(audio_unit_devout->buffer[6]);
  }
  
  audio_unit_devout->buffer[6] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_AUDIO_UNIT_DEVOUT_BUFFER_7 */
  if(audio_unit_devout->buffer[7] != NULL){
    free(audio_unit_devout->buffer[7]);
  }
  
  audio_unit_devout->buffer[7] = (void *) malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_audio_unit_devout_new:
 *
 * Creates a new instance of #AgsAudioUnitDevout.
 *
 * Returns: a new #AgsAudioUnitDevout
 *
 * Since: 3.0.0
 */
AgsAudioUnitDevout*
ags_audio_unit_devout_new()
{
  AgsAudioUnitDevout *audio_unit_devout;

  audio_unit_devout = (AgsAudioUnitDevout *) g_object_new(AGS_TYPE_AUDIO_UNIT_DEVOUT,
							  NULL);
  
  return(audio_unit_devout);
}
