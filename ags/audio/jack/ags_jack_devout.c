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

#include <ags/audio/jack/ags_jack_devout.h>

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

void ags_jack_devout_class_init(AgsJackDevoutClass *jack_devout);
void ags_jack_devout_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_jack_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_jack_devout_init(AgsJackDevout *jack_devout);
void ags_jack_devout_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_jack_devout_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_jack_devout_dispose(GObject *gobject);
void ags_jack_devout_finalize(GObject *gobject);

AgsUUID* ags_jack_devout_get_uuid(AgsConnectable *connectable);
gboolean ags_jack_devout_has_resource(AgsConnectable *connectable);
gboolean ags_jack_devout_is_ready(AgsConnectable *connectable);
void ags_jack_devout_add_to_registry(AgsConnectable *connectable);
void ags_jack_devout_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_jack_devout_list_resource(AgsConnectable *connectable);
xmlNode* ags_jack_devout_xml_compose(AgsConnectable *connectable);
void ags_jack_devout_xml_parse(AgsConnectable *connectable,
			       xmlNode *node);
gboolean ags_jack_devout_is_connected(AgsConnectable *connectable);
void ags_jack_devout_connect(AgsConnectable *connectable);
void ags_jack_devout_disconnect(AgsConnectable *connectable);

void ags_jack_devout_set_device(AgsSoundcard *soundcard,
				gchar *device);
gchar* ags_jack_devout_get_device(AgsSoundcard *soundcard);

void ags_jack_devout_set_presets(AgsSoundcard *soundcard,
				 guint channels,
				 guint rate,
				 guint buffer_size,
				 guint format);
void ags_jack_devout_get_presets(AgsSoundcard *soundcard,
				 guint *channels,
				 guint *rate,
				 guint *buffer_size,
				 guint *format);

void ags_jack_devout_list_cards(AgsSoundcard *soundcard,
				GList **card_id, GList **card_name);
void ags_jack_devout_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
			      guint *channels_min, guint *channels_max,
			      guint *rate_min, guint *rate_max,
			      guint *buffer_size_min, guint *buffer_size_max,
			      GError **error);
guint ags_jack_devout_get_capability(AgsSoundcard *soundcard);

gboolean ags_jack_devout_is_starting(AgsSoundcard *soundcard);
gboolean ags_jack_devout_is_playing(AgsSoundcard *soundcard);

gchar* ags_jack_devout_get_uptime(AgsSoundcard *soundcard);

void ags_jack_devout_port_init(AgsSoundcard *soundcard,
			       GError **error);
void ags_jack_devout_port_play(AgsSoundcard *soundcard,
			       GError **error);
void ags_jack_devout_port_free(AgsSoundcard *soundcard);

void ags_jack_devout_tic(AgsSoundcard *soundcard);
void ags_jack_devout_offset_changed(AgsSoundcard *soundcard,
				    guint note_offset);

void ags_jack_devout_set_bpm(AgsSoundcard *soundcard,
			     gdouble bpm);
gdouble ags_jack_devout_get_bpm(AgsSoundcard *soundcard);

void ags_jack_devout_set_delay_factor(AgsSoundcard *soundcard,
				      gdouble delay_factor);
gdouble ags_jack_devout_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_jack_devout_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_jack_devout_get_delay(AgsSoundcard *soundcard);
guint ags_jack_devout_get_attack(AgsSoundcard *soundcard);

void* ags_jack_devout_get_buffer(AgsSoundcard *soundcard);
void* ags_jack_devout_get_next_buffer(AgsSoundcard *soundcard);
void* ags_jack_devout_get_prev_buffer(AgsSoundcard *soundcard);

void ags_jack_devout_lock_buffer(AgsSoundcard *soundcard,
				 void *buffer);
void ags_jack_devout_unlock_buffer(AgsSoundcard *soundcard,
				   void *buffer);

guint ags_jack_devout_get_delay_counter(AgsSoundcard *soundcard);

void ags_jack_devout_set_start_note_offset(AgsSoundcard *soundcard,
					   guint start_note_offset);
guint ags_jack_devout_get_start_note_offset(AgsSoundcard *soundcard);

void ags_jack_devout_set_note_offset(AgsSoundcard *soundcard,
				     guint note_offset);
guint ags_jack_devout_get_note_offset(AgsSoundcard *soundcard);

void ags_jack_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
					      guint note_offset);
guint ags_jack_devout_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_jack_devout_set_loop(AgsSoundcard *soundcard,
			      guint loop_left, guint loop_right,
			      gboolean do_loop);
void ags_jack_devout_get_loop(AgsSoundcard *soundcard,
			      guint *loop_left, guint *loop_right,
			      gboolean *do_loop);

guint ags_jack_devout_get_loop_offset(AgsSoundcard *soundcard);

guint ags_jack_devout_get_sub_block_count(AgsSoundcard *soundcard);

gboolean ags_jack_devout_trylock_sub_block(AgsSoundcard *soundcard,
					   void *buffer, guint sub_block);
void ags_jack_devout_unlock_sub_block(AgsSoundcard *soundcard,
				      void *buffer, guint sub_block);

/**
 * SECTION:ags_jack_devout
 * @short_description: Output to soundcard
 * @title: AgsJackDevout
 * @section_id:
 * @include: ags/audio/jack/ags_jack_devout.h
 *
 * #AgsJackDevout represents a soundcard and supports output.
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
};

static gpointer ags_jack_devout_parent_class = NULL;

GType
ags_jack_devout_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_jack_devout = 0;

    static const GTypeInfo ags_jack_devout_info = {
      sizeof(AgsJackDevoutClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_jack_devout_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsJackDevout),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_jack_devout_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_jack_devout_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_jack_devout_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_jack_devout = g_type_register_static(G_TYPE_OBJECT,
						  "AgsJackDevout",
						  &ags_jack_devout_info,
						  0);

    g_type_add_interface_static(ags_type_jack_devout,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_jack_devout,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_jack_devout);
  }

  return g_define_type_id__volatile;
}

void
ags_jack_devout_class_init(AgsJackDevoutClass *jack_devout)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_jack_devout_parent_class = g_type_class_peek_parent(jack_devout);

  /* GObjectClass */
  gobject = (GObjectClass *) jack_devout;

  gobject->set_property = ags_jack_devout_set_property;
  gobject->get_property = ags_jack_devout_get_property;

  gobject->dispose = ags_jack_devout_dispose;
  gobject->finalize = ags_jack_devout_finalize;

  /* properties */
  /**
   * AgsJackDevout:device:
   *
   * The jack soundcard indentifier
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-jack-devout-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsJackDevout:dsp-channels:
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
   * AgsJackDevout:pcm-channels:
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
   * AgsJackDevout:format:
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
   * AgsJackDevout:buffer-size:
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
   * AgsJackDevout:samplerate:
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
   * AgsJackDevout:buffer:
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
   * AgsJackDevout:bpm:
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
   * AgsJackDevout:delay-factor:
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
   * AgsJackDevout:attack:
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
   * AgsJackDevout:jack-client:
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
   * AgsJackDevout:jack-port:
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
ags_jack_devout_error_quark()
{
  return(g_quark_from_static_string("ags-jack_devout-error-quark"));
}

void
ags_jack_devout_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_jack_devout_get_uuid;
  connectable->has_resource = ags_jack_devout_has_resource;

  connectable->is_ready = ags_jack_devout_is_ready;
  connectable->add_to_registry = ags_jack_devout_add_to_registry;
  connectable->remove_from_registry = ags_jack_devout_remove_from_registry;

  connectable->list_resource = ags_jack_devout_list_resource;
  connectable->xml_compose = ags_jack_devout_xml_compose;
  connectable->xml_parse = ags_jack_devout_xml_parse;

  connectable->is_connected = ags_jack_devout_is_connected;  
  connectable->connect = ags_jack_devout_connect;
  connectable->disconnect = ags_jack_devout_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_jack_devout_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_jack_devout_set_device;
  soundcard->get_device = ags_jack_devout_get_device;
  
  soundcard->set_presets = ags_jack_devout_set_presets;
  soundcard->get_presets = ags_jack_devout_get_presets;

  soundcard->list_cards = ags_jack_devout_list_cards;
  soundcard->pcm_info = ags_jack_devout_pcm_info;
  soundcard->get_capability = ags_jack_devout_get_capability;

  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_jack_devout_is_starting;
  soundcard->is_playing = ags_jack_devout_is_playing;
  soundcard->is_recording = NULL;

  soundcard->get_uptime = ags_jack_devout_get_uptime;
  
  soundcard->play_init = ags_jack_devout_port_init;
  soundcard->play = ags_jack_devout_port_play;

  soundcard->record_init = NULL;
  soundcard->record = NULL;

  soundcard->stop = ags_jack_devout_port_free;

  soundcard->tic = ags_jack_devout_tic;
  soundcard->offset_changed = ags_jack_devout_offset_changed;
    
  soundcard->set_bpm = ags_jack_devout_set_bpm;
  soundcard->get_bpm = ags_jack_devout_get_bpm;

  soundcard->set_delay_factor = ags_jack_devout_set_delay_factor;
  soundcard->get_delay_factor = ags_jack_devout_get_delay_factor;
  
  soundcard->get_absolute_delay = ags_jack_devout_get_absolute_delay;

  soundcard->get_delay = ags_jack_devout_get_delay;
  soundcard->get_attack = ags_jack_devout_get_attack;

  soundcard->get_buffer = ags_jack_devout_get_buffer;
  soundcard->get_next_buffer = ags_jack_devout_get_next_buffer;
  soundcard->get_prev_buffer = ags_jack_devout_get_prev_buffer;

  soundcard->lock_buffer = ags_jack_devout_lock_buffer;
  soundcard->unlock_buffer = ags_jack_devout_unlock_buffer;

  soundcard->get_delay_counter = ags_jack_devout_get_delay_counter;

  soundcard->set_start_note_offset = ags_jack_devout_set_start_note_offset;
  soundcard->get_start_note_offset = ags_jack_devout_get_start_note_offset;

  soundcard->set_note_offset = ags_jack_devout_set_note_offset;
  soundcard->get_note_offset = ags_jack_devout_get_note_offset;

  soundcard->set_note_offset_absolute = ags_jack_devout_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_jack_devout_get_note_offset_absolute;

  soundcard->set_loop = ags_jack_devout_set_loop;
  soundcard->get_loop = ags_jack_devout_get_loop;

  soundcard->get_loop_offset = ags_jack_devout_get_loop_offset;

  soundcard->get_sub_block_count = ags_jack_devout_get_sub_block_count;

  soundcard->trylock_sub_block = ags_jack_devout_trylock_sub_block;
  soundcard->unlock_sub_block = ags_jack_devout_unlock_sub_block;
}

void
ags_jack_devout_init(AgsJackDevout *jack_devout)
{
  AgsConfig *config;

  gchar *str;
  gchar *segmentation;

  guint denominator, numerator;
  guint i;
  
  /* flags */
  jack_devout->flags = 0;
  jack_devout->connectable_flags = 0;
  g_atomic_int_set(&(jack_devout->sync_flags),
		   AGS_JACK_DEVOUT_PASS_THROUGH);

  /* devout mutex */
  g_rec_mutex_init(&(jack_devout->obj_mutex));

  /* uuid */
  jack_devout->uuid = ags_uuid_alloc();
  ags_uuid_generate(jack_devout->uuid);

  /* presets */
  config = ags_config_get_instance();
  
  jack_devout->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  jack_devout->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  jack_devout->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  jack_devout->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  jack_devout->format = ags_soundcard_helper_config_get_format(config);

  /* app buffer */
  jack_devout->app_buffer_mode = AGS_JACK_DEVOUT_APP_BUFFER_0;

  jack_devout->app_buffer_mutex = (GRecMutex **) g_malloc(4 * sizeof(GRecMutex *));

  for(i = 0; i < 4; i++){
    jack_devout->app_buffer_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(jack_devout->app_buffer_mutex[i]);
  }
  
  jack_devout->sub_block_count = AGS_SOUNDCARD_DEFAULT_SUB_BLOCK_COUNT;
  jack_devout->sub_block_mutex = (GRecMutex **) g_malloc(4 * jack_devout->sub_block_count * jack_devout->pcm_channels * sizeof(GRecMutex *));

  for(i = 0; i < 4 * jack_devout->sub_block_count * jack_devout->pcm_channels; i++){
    jack_devout->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

    g_rec_mutex_init(jack_devout->sub_block_mutex[i]);
  }

  jack_devout->app_buffer = (void **) g_malloc(4 * sizeof(void *));

  jack_devout->app_buffer[0] = NULL;
  jack_devout->app_buffer[1] = NULL;
  jack_devout->app_buffer[2] = NULL;
  jack_devout->app_buffer[3] = NULL;
  
  ags_jack_devout_realloc_buffer(jack_devout);
  
  /* bpm */
  jack_devout->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  jack_devout->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    jack_devout->delay_factor = 1.0 / numerator * (numerator / denominator);

    g_free(segmentation);
  }

  /* delay and attack */
  jack_devout->delay = (gdouble *) g_malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					    sizeof(gdouble));
  
  jack_devout->attack = (guint *) g_malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					   sizeof(guint));

  ags_jack_devout_adjust_delay_and_attack(jack_devout);
  
  /* counters */
  jack_devout->tact_counter = 0.0;
  jack_devout->delay_counter = 0.0;
  jack_devout->tic_counter = 0;

  jack_devout->start_note_offset = 0;
  jack_devout->note_offset = 0;
  jack_devout->note_offset_absolute = 0;

  jack_devout->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  jack_devout->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  jack_devout->do_loop = FALSE;

  jack_devout->loop_offset = 0;

  /*  */
  jack_devout->card_uri = NULL;
  jack_devout->jack_client = NULL;

  jack_devout->port_name = NULL;
  jack_devout->jack_port = NULL;

  /* callback mutex */
  g_mutex_init(&(jack_devout->callback_mutex));

  g_cond_init(&(jack_devout->callback_cond));

  /* callback finish mutex */
  g_mutex_init(&(jack_devout->callback_finish_mutex));

  g_cond_init(&(jack_devout->callback_finish_cond));
}

void
ags_jack_devout_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackDevout *jack_devout;

  GRecMutex *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(gobject);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      g_rec_mutex_lock(jack_devout_mutex);

      jack_devout->card_uri = g_strdup(device);

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      g_rec_mutex_lock(jack_devout_mutex);

      if(dsp_channels == jack_devout->dsp_channels){
	g_rec_mutex_unlock(jack_devout_mutex);

	return;
      }

      jack_devout->dsp_channels = dsp_channels;

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels, old_pcm_channels;
      guint i;

      pcm_channels = g_value_get_uint(value);

      g_rec_mutex_lock(jack_devout_mutex);

      if(pcm_channels == jack_devout->pcm_channels){
	g_rec_mutex_unlock(jack_devout_mutex);

	return;
      }

      old_pcm_channels = jack_devout->pcm_channels;

      /* destroy if less pcm-channels */
      for(i = 4 * jack_devout->sub_block_count * pcm_channels; i < 4 * jack_devout->sub_block_count * old_pcm_channels; i++){
	g_rec_mutex_clear(jack_devout->sub_block_mutex[i]);

	g_free(jack_devout->sub_block_mutex[i]);
      }

      jack_devout->sub_block_mutex = (GRecMutex **) g_realloc(jack_devout->sub_block_mutex,
							      4 * jack_devout->sub_block_count * pcm_channels * sizeof(GRecMutex *));
      
      /* create if more pcm-channels */
      for(i = 4 * jack_devout->sub_block_count * old_pcm_channels; i < 4 * jack_devout->sub_block_count * pcm_channels; i++){
	jack_devout->sub_block_mutex[i] = (GRecMutex *) g_malloc(sizeof(GRecMutex));

	g_rec_mutex_init(jack_devout->sub_block_mutex[i]);
      }

      jack_devout->pcm_channels = pcm_channels;

      g_rec_mutex_unlock(jack_devout_mutex);

      ags_jack_devout_realloc_buffer(jack_devout);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(jack_devout_mutex);

      if(format == jack_devout->format){
	g_rec_mutex_unlock(jack_devout_mutex);

	return;
      }

      jack_devout->format = format;

      g_rec_mutex_unlock(jack_devout_mutex);

      ags_jack_devout_realloc_buffer(jack_devout);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      g_rec_mutex_lock(jack_devout_mutex);

      if(buffer_size == jack_devout->buffer_size){
	g_rec_mutex_unlock(jack_devout_mutex);

	return;
      }

      jack_devout->buffer_size = buffer_size;

      g_rec_mutex_unlock(jack_devout_mutex);

      ags_jack_devout_realloc_buffer(jack_devout);
      ags_jack_devout_adjust_delay_and_attack(jack_devout);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      g_rec_mutex_lock(jack_devout_mutex);
      
      if(samplerate == jack_devout->samplerate){
	g_rec_mutex_unlock(jack_devout_mutex);

	return;
      }

      jack_devout->samplerate = samplerate;

      g_rec_mutex_unlock(jack_devout_mutex);

      ags_jack_devout_realloc_buffer(jack_devout);
      ags_jack_devout_adjust_delay_and_attack(jack_devout);
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

      g_rec_mutex_lock(jack_devout_mutex);

      jack_devout->bpm = bpm;

      g_rec_mutex_unlock(jack_devout_mutex);

      ags_jack_devout_adjust_delay_and_attack(jack_devout);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      g_rec_mutex_lock(jack_devout_mutex);

      jack_devout->delay_factor = delay_factor;

      g_rec_mutex_unlock(jack_devout_mutex);

      ags_jack_devout_adjust_delay_and_attack(jack_devout);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      AgsJackClient *jack_client;

      jack_client = (AgsJackClient *) g_value_get_object(value);

      g_rec_mutex_lock(jack_devout_mutex);

      if(jack_devout->jack_client == (GObject *) jack_client){
	g_rec_mutex_unlock(jack_devout_mutex);

	return;
      }

      if(jack_devout->jack_client != NULL){
	g_object_unref(G_OBJECT(jack_devout->jack_client));
      }

      if(jack_client != NULL){
	g_object_ref(jack_client);
      }
      
      jack_devout->jack_client = (GObject *) jack_client;

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_JACK_PORT:
    {
      AgsJackPort *jack_port;

      jack_port = (AgsJackPort *) g_value_get_pointer(value);

      g_rec_mutex_lock(jack_devout_mutex);

      if(!AGS_IS_JACK_PORT(jack_port) ||
	 g_list_find(jack_devout->jack_port, jack_port) != NULL){
	g_rec_mutex_unlock(jack_devout_mutex);

	return;
      }

      g_object_ref(jack_port);
      jack_devout->jack_port = g_list_append(jack_devout->jack_port,
					     jack_port);

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_devout_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsJackDevout *jack_devout;

  GRecMutex *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(gobject);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(jack_devout_mutex);

      g_value_set_string(value, jack_devout->card_uri);

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_rec_mutex_lock(jack_devout_mutex);

      g_value_set_uint(value, jack_devout->dsp_channels);

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_rec_mutex_lock(jack_devout_mutex);

      g_value_set_uint(value, jack_devout->pcm_channels);

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(jack_devout_mutex);

      g_value_set_uint(value, jack_devout->format);

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(jack_devout_mutex);

      g_value_set_uint(value, jack_devout->buffer_size);

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(jack_devout_mutex);

      g_value_set_uint(value, jack_devout->samplerate);

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(jack_devout_mutex);

      g_value_set_pointer(value, jack_devout->app_buffer);

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(jack_devout_mutex);

      g_value_set_double(value, jack_devout->bpm);

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_rec_mutex_lock(jack_devout_mutex);

      g_value_set_double(value, jack_devout->delay_factor);

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_ATTACK:
    {
      g_rec_mutex_lock(jack_devout_mutex);

      g_value_set_pointer(value, jack_devout->attack);

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_JACK_CLIENT:
    {
      g_rec_mutex_lock(jack_devout_mutex);

      g_value_set_object(value, jack_devout->jack_client);

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  case PROP_JACK_PORT:
    {
      g_rec_mutex_lock(jack_devout_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(jack_devout->jack_port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(jack_devout_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_jack_devout_dispose(GObject *gobject)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(gobject);

  /* jack client */
  if(jack_devout->jack_client != NULL){
    g_object_unref(jack_devout->jack_client);

    jack_devout->jack_client = NULL;
  }

  /* jack port */
  g_list_free_full(jack_devout->jack_port,
		   g_object_unref);

  jack_devout->jack_port = NULL;
  
  /* call parent */
  G_OBJECT_CLASS(ags_jack_devout_parent_class)->dispose(gobject);
}

void
ags_jack_devout_finalize(GObject *gobject)
{
  AgsJackDevout *jack_devout;

  guint i;
  
  jack_devout = AGS_JACK_DEVOUT(gobject);

  for(i = 0; i < 4; i++){
    g_rec_mutex_clear(jack_devout->app_buffer_mutex[i]);

    g_free(jack_devout->app_buffer_mutex[i]);
  }
  
  g_free(jack_devout->app_buffer_mutex);
  
  for(i = 0; i < 4 * jack_devout->sub_block_count * jack_devout->pcm_channels; i++){
    g_rec_mutex_clear(jack_devout->sub_block_mutex[i]);

    g_free(jack_devout->sub_block_mutex[i]);
  }

  g_free(jack_devout->sub_block_mutex);

  /* free output buffer */
  g_free(jack_devout->app_buffer[0]);
  g_free(jack_devout->app_buffer[1]);
  g_free(jack_devout->app_buffer[2]);
  g_free(jack_devout->app_buffer[3]);

  /* free buffer array */
  g_free(jack_devout->app_buffer);

  g_free(jack_devout->delay);
  g_free(jack_devout->attack);

  g_free(jack_devout->card_uri);
  
  /* jack client */
  if(jack_devout->jack_client != NULL){
    g_object_unref(jack_devout->jack_client);
  }

  /* jack port */
  g_strfreev(jack_devout->port_name);

  g_list_free_full(jack_devout->jack_port,
		   g_object_unref);
  
  /* call parent */
  G_OBJECT_CLASS(ags_jack_devout_parent_class)->finalize(gobject);
}

AgsUUID*
ags_jack_devout_get_uuid(AgsConnectable *connectable)
{
  AgsJackDevout *jack_devout;
  
  AgsUUID *ptr;

  GRecMutex *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(connectable);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* get UUID */
  g_rec_mutex_lock(jack_devout_mutex);

  ptr = jack_devout->uuid;

  g_rec_mutex_unlock(jack_devout_mutex);
  
  return(ptr);
}

gboolean
ags_jack_devout_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_jack_devout_is_ready(AgsConnectable *connectable)
{
  AgsJackDevout *jack_devout;
  
  gboolean is_ready;

  GRecMutex *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(connectable);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* check is ready */
  g_rec_mutex_lock(jack_devout_mutex);

  is_ready = ((AGS_CONNECTABLE_ADDED_TO_REGISTRY & (jack_devout->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_devout_mutex);
  
  return(is_ready);
}

void
ags_jack_devout_add_to_registry(AgsConnectable *connectable)
{
  AgsJackDevout *jack_devout;

  GRecMutex *jack_devout_mutex;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  jack_devout = AGS_JACK_DEVOUT(connectable);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  g_rec_mutex_lock(jack_devout_mutex);

  jack_devout->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;
  
  g_rec_mutex_unlock(jack_devout_mutex);
}

void
ags_jack_devout_remove_from_registry(AgsConnectable *connectable)
{
  AgsJackDevout *jack_devout;

  GRecMutex *jack_devout_mutex;

  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  jack_devout = AGS_JACK_DEVOUT(connectable);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  g_rec_mutex_lock(jack_devout_mutex);

  jack_devout->connectable_flags &= (~AGS_CONNECTABLE_ADDED_TO_REGISTRY);
  
  g_rec_mutex_unlock(jack_devout_mutex);
}

xmlNode*
ags_jack_devout_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_jack_devout_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_jack_devout_xml_parse(AgsConnectable *connectable,
			  xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_jack_devout_is_connected(AgsConnectable *connectable)
{
  AgsJackDevout *jack_devout;
  
  gboolean is_connected;

  GRecMutex *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(connectable);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* check is connected */
  g_rec_mutex_lock(jack_devout_mutex);

  is_connected = ((AGS_CONNECTABLE_CONNECTED & (jack_devout->connectable_flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_devout_mutex);
  
  return(is_connected);
}

void
ags_jack_devout_connect(AgsConnectable *connectable)
{
  AgsJackDevout *jack_devout;

  GRecMutex *jack_devout_mutex;
  
  if(ags_connectable_is_connected(connectable)){
    return;
  }

  jack_devout = AGS_JACK_DEVOUT(connectable);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  g_rec_mutex_lock(jack_devout_mutex);

  jack_devout->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_rec_mutex_unlock(jack_devout_mutex);
}

void
ags_jack_devout_disconnect(AgsConnectable *connectable)
{
  AgsJackDevout *jack_devout;

  GRecMutex *jack_devout_mutex;

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  jack_devout = AGS_JACK_DEVOUT(connectable);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  g_rec_mutex_lock(jack_devout_mutex);

  jack_devout->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_rec_mutex_unlock(jack_devout_mutex);
}

/**
 * ags_jack_devout_test_flags:
 * @jack_devout: the #AgsJackDevout
 * @flags: the flags
 *
 * Test @flags to be set on @jack_devout.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.0.0
 */
gboolean
ags_jack_devout_test_flags(AgsJackDevout *jack_devout, guint flags)
{
  gboolean retval;  
  
  GRecMutex *jack_devout_mutex;

  if(!AGS_IS_JACK_DEVOUT(jack_devout)){
    return(FALSE);
  }

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* test */
  g_rec_mutex_lock(jack_devout_mutex);

  retval = (flags & (jack_devout->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(jack_devout_mutex);

  return(retval);
}

/**
 * ags_jack_devout_set_flags:
 * @jack_devout: the #AgsJackDevout
 * @flags: see #AgsJackDevoutFlags-enum
 *
 * Enable a feature of @jack_devout.
 *
 * Since: 3.0.0
 */
void
ags_jack_devout_set_flags(AgsJackDevout *jack_devout, guint flags)
{
  GRecMutex *jack_devout_mutex;

  if(!AGS_IS_JACK_DEVOUT(jack_devout)){
    return;
  }

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(jack_devout_mutex);

  jack_devout->flags |= flags;
  
  g_rec_mutex_unlock(jack_devout_mutex);
}
    
/**
 * ags_jack_devout_unset_flags:
 * @jack_devout: the #AgsJackDevout
 * @flags: see #AgsJackDevoutFlags-enum
 *
 * Disable a feature of @jack_devout.
 *
 * Since: 3.0.0
 */
void
ags_jack_devout_unset_flags(AgsJackDevout *jack_devout, guint flags)
{  
  GRecMutex *jack_devout_mutex;

  if(!AGS_IS_JACK_DEVOUT(jack_devout)){
    return;
  }

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(jack_devout_mutex);

  jack_devout->flags &= (~flags);
  
  g_rec_mutex_unlock(jack_devout_mutex);
}

void
ags_jack_devout_set_device(AgsSoundcard *soundcard,
			   gchar *device)
{
  AgsJackDevout *jack_devout;

  GList *jack_port, *jack_port_start;

  gchar *str;

  guint pcm_channels;
  int ret;
  guint nth_card;
  guint i;
  
  GRecMutex *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* check device */
  g_rec_mutex_lock(jack_devout_mutex);

  if(jack_devout->card_uri == device ||
     (jack_devout->card_uri != NULL &&
      !g_ascii_strcasecmp(jack_devout->card_uri,
			  device))){
    g_rec_mutex_unlock(jack_devout_mutex);
  
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-jack-devout-")){
    g_rec_mutex_unlock(jack_devout_mutex);

    g_warning("invalid JACK device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-jack-devout-%u",
	       &nth_card);

  if(ret != 1){
    g_rec_mutex_unlock(jack_devout_mutex);

    g_warning("invalid JACK device specifier");

    return;
  }

  g_free(jack_devout->card_uri);
  jack_devout->card_uri = g_strdup(device);

  /* apply name to port */
  g_rec_mutex_unlock(jack_devout_mutex);
  
#if 0
  pcm_channels = jack_devout->pcm_channels;
  
  jack_port_start = 
    jack_port = g_list_copy(jack_devout->jack_port);

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
ags_jack_devout_get_device(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;
  
  gchar *device;

  GRecMutex *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  device = NULL;

  /* get device */
  g_rec_mutex_lock(jack_devout_mutex);

  device = g_strdup(jack_devout->card_uri);

  g_rec_mutex_unlock(jack_devout_mutex);
  
  return(device);
}

void
ags_jack_devout_set_presets(AgsSoundcard *soundcard,
			    guint channels,
			    guint rate,
			    guint buffer_size,
			    guint format)
{
  AgsJackDevout *jack_devout;

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  g_object_set(jack_devout,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_jack_devout_get_presets(AgsSoundcard *soundcard,
			    guint *channels,
			    guint *rate,
			    guint *buffer_size,
			    guint *format)
{
  AgsJackDevout *jack_devout;

  GRecMutex *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* get presets */
  g_rec_mutex_lock(jack_devout_mutex);

  if(channels != NULL){
    *channels = jack_devout->pcm_channels;
  }

  if(rate != NULL){
    *rate = jack_devout->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = jack_devout->buffer_size;
  }

  if(format != NULL){
    *format = jack_devout->format;
  }

  g_rec_mutex_unlock(jack_devout_mutex);
}

void
ags_jack_devout_list_cards(AgsSoundcard *soundcard,
			   GList **card_id, GList **card_name)
{
  AgsJackClient *jack_client;
  AgsJackDevout *jack_devout;

  AgsApplicationContext *application_context;
  
  GList *list_start, *list;

  gchar *card_uri;
  gchar *client_name;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

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
    if(AGS_IS_JACK_DEVOUT(list->data)){
      if(card_id != NULL){
	card_uri = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));
	
	if(AGS_JACK_DEVOUT(list->data)->card_uri != NULL){
	  *card_id = g_list_prepend(*card_id,
				    card_uri);
	}else{
	  *card_id = g_list_prepend(*card_id,
				    g_strdup("(null)"));

	  g_warning("ags_jack_devout_list_cards() - card id (null)");
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

	  g_warning("ags_jack_devout_list_cards() - JACK client not connected (null)");
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
ags_jack_devout_pcm_info(AgsSoundcard *soundcard,
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
ags_jack_devout_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_PLAYBACK);
}

gboolean
ags_jack_devout_is_starting(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  gboolean is_starting;
  
  GRecMutex *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* check is starting */
  g_rec_mutex_lock(jack_devout_mutex);

  is_starting = ((AGS_JACK_DEVOUT_START_PLAY & (jack_devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_devout_mutex);
  
  return(is_starting);
}

gboolean
ags_jack_devout_is_playing(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  gboolean is_playing;
  
  GRecMutex *jack_devout_mutex;

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* check is starting */
  g_rec_mutex_lock(jack_devout_mutex);

  is_playing = ((AGS_JACK_DEVOUT_PLAY & (jack_devout->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_devout_mutex);

  return(is_playing);
}

gchar*
ags_jack_devout_get_uptime(AgsSoundcard *soundcard)
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
ags_jack_devout_port_init(AgsSoundcard *soundcard,
			  GError **error)
{
  AgsJackDevout *jack_devout;

  guint format, word_size;
  
  GRecMutex *jack_devout_mutex;

  if(ags_soundcard_is_playing(soundcard)){
    return;
  }
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* retrieve word size */
  g_rec_mutex_lock(jack_devout_mutex);

  switch(jack_devout->format){
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
    g_rec_mutex_unlock(jack_devout_mutex);
    
    g_warning("ags_jack_devout_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for playback */
  jack_devout->app_buffer_mode = AGS_JACK_DEVOUT_APP_BUFFER_3;
  jack_devout->flags |= (AGS_JACK_DEVOUT_START_PLAY |
			 AGS_JACK_DEVOUT_PLAY |
			 AGS_JACK_DEVOUT_NONBLOCKING);

  memset(jack_devout->app_buffer[0], 0, jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  memset(jack_devout->app_buffer[1], 0, jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  memset(jack_devout->app_buffer[2], 0, jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  memset(jack_devout->app_buffer[3], 0, jack_devout->pcm_channels * jack_devout->buffer_size * word_size);

  /*  */
  jack_devout->tact_counter = 0.0;
  jack_devout->delay_counter = floor(ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(jack_devout)));
  jack_devout->tic_counter = 0;

  jack_devout->flags |= (AGS_JACK_DEVOUT_INITIALIZED |
			 AGS_JACK_DEVOUT_START_PLAY |
			 AGS_JACK_DEVOUT_PLAY);
  
  g_atomic_int_and(&(jack_devout->sync_flags),
		   (~(AGS_JACK_DEVOUT_PASS_THROUGH)));
  g_atomic_int_or(&(jack_devout->sync_flags),
		  AGS_JACK_DEVOUT_INITIAL_CALLBACK);

  g_rec_mutex_unlock(jack_devout_mutex);
}

void
ags_jack_devout_port_play(AgsSoundcard *soundcard,
			  GError **error)
{
  AgsJackClient *jack_client;
  AgsJackDevout *jack_devout;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsTaskLauncher *task_launcher;
  
  AgsApplicationContext *application_context;

  GList *task;

  guint word_size;
  gboolean jack_client_activated;
  
  GRecMutex *jack_devout_mutex;
  GRecMutex *jack_client_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  application_context = ags_application_context_get_instance();
  
  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* client */
  g_rec_mutex_lock(jack_devout_mutex);

  jack_client = (AgsJackClient *) jack_devout->jack_client;
  
  callback_mutex = &(jack_devout->callback_mutex);
  callback_finish_mutex = &(jack_devout->callback_finish_mutex);

  g_rec_mutex_unlock(jack_devout_mutex);

  /* do playback */
  g_rec_mutex_lock(jack_devout_mutex);
  
  jack_devout->flags &= (~AGS_JACK_DEVOUT_START_PLAY);
  
  if((AGS_JACK_DEVOUT_INITIALIZED & (jack_devout->flags)) == 0){
    g_rec_mutex_unlock(jack_devout_mutex);
    
    return;
  }

  switch(jack_devout->format){
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
    g_rec_mutex_unlock(jack_devout_mutex);
    
    g_warning("ags_jack_devout_port_play(): unsupported word size");
    
    return;
  }

  g_rec_mutex_unlock(jack_devout_mutex);

  /* get client mutex */
  jack_client_mutex = AGS_JACK_CLIENT_GET_OBJ_MUTEX(jack_client);

  /* get activated */
  g_rec_mutex_lock(jack_client_mutex);

  jack_client_activated = ((AGS_JACK_CLIENT_ACTIVATED & (jack_client->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(jack_client_mutex);

  if(jack_client_activated){
    /* signal */
    if((AGS_JACK_DEVOUT_INITIAL_CALLBACK & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0){
      g_mutex_lock(callback_mutex);

      g_atomic_int_or(&(jack_devout->sync_flags),
		      AGS_JACK_DEVOUT_CALLBACK_DONE);
    
      if((AGS_JACK_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(jack_devout->sync_flags)))) != 0){
	g_cond_signal(&(jack_devout->callback_cond));
      }

      g_mutex_unlock(callback_mutex);
    }
    
    /* wait callback */	
    if((AGS_JACK_DEVOUT_INITIAL_CALLBACK & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0){
      g_mutex_lock(callback_finish_mutex);
    
      if((AGS_JACK_DEVOUT_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0){
	g_atomic_int_or(&(jack_devout->sync_flags),
			AGS_JACK_DEVOUT_CALLBACK_FINISH_WAIT);
    
	while((AGS_JACK_DEVOUT_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(jack_devout->sync_flags)))) == 0 &&
	      (AGS_JACK_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_devout->sync_flags)))) != 0){
	  g_cond_wait(&(jack_devout->callback_finish_cond),
		      callback_finish_mutex);
	}
      }
    
      g_atomic_int_and(&(jack_devout->sync_flags),
		       (~(AGS_JACK_DEVOUT_CALLBACK_FINISH_WAIT |
			  AGS_JACK_DEVOUT_CALLBACK_FINISH_DONE)));
    
      g_mutex_unlock(callback_finish_mutex);
    }else{
      g_atomic_int_and(&(jack_devout->sync_flags),
		       (~AGS_JACK_DEVOUT_INITIAL_CALLBACK));
    }
  }

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;
  
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) jack_devout);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) jack_devout);
  task = g_list_append(task,
		       clear_buffer);
    
  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) jack_devout);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_jack_devout_port_free(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  guint word_size;

  GRecMutex *jack_devout_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /*  */
  g_rec_mutex_lock(jack_devout_mutex);

  if((AGS_JACK_DEVOUT_INITIALIZED & (jack_devout->flags)) == 0){
    g_rec_mutex_unlock(jack_devout_mutex);

    return;
  }

  //  g_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);

  callback_mutex = &(jack_devout->callback_mutex);
  callback_finish_mutex = &(jack_devout->callback_finish_mutex);

  jack_devout->app_buffer_mode = AGS_JACK_DEVOUT_APP_BUFFER_0;
  jack_devout->flags &= (~(AGS_JACK_DEVOUT_PLAY));

  g_atomic_int_or(&(jack_devout->sync_flags),
		  AGS_JACK_DEVOUT_PASS_THROUGH);
  g_atomic_int_and(&(jack_devout->sync_flags),
		   (~AGS_JACK_DEVOUT_INITIAL_CALLBACK));

  /* signal callback */
  g_mutex_lock(callback_mutex);

  g_atomic_int_or(&(jack_devout->sync_flags),
		  AGS_JACK_DEVOUT_CALLBACK_DONE);
    
  if((AGS_JACK_DEVOUT_CALLBACK_WAIT & (g_atomic_int_get(&(jack_devout->sync_flags)))) != 0){
    g_cond_signal(&(jack_devout->callback_cond));
  }

  g_mutex_unlock(callback_mutex);

  /* signal thread */
  g_mutex_lock(callback_finish_mutex);

  g_atomic_int_or(&(jack_devout->sync_flags),
		  AGS_JACK_DEVOUT_CALLBACK_FINISH_DONE);
    
  if((AGS_JACK_DEVOUT_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(jack_devout->sync_flags)))) != 0){
    g_cond_signal(&(jack_devout->callback_finish_cond));
  }

  g_mutex_unlock(callback_finish_mutex);
  
  /*  */
  jack_devout->note_offset = jack_devout->start_note_offset;
  jack_devout->note_offset_absolute = jack_devout->start_note_offset;

  switch(jack_devout->format){
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
    
    g_critical("ags_jack_devout_free(): unsupported word size");
  }

  memset(jack_devout->app_buffer[1], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  memset(jack_devout->app_buffer[2], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  memset(jack_devout->app_buffer[3], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  memset(jack_devout->app_buffer[0], 0, (size_t) jack_devout->pcm_channels * jack_devout->buffer_size * word_size);

  g_rec_mutex_unlock(jack_devout_mutex);
}

void
ags_jack_devout_tic(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  gdouble delay;
  gdouble delay_counter;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  
  GRecMutex *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(jack_devout_mutex);

  delay = jack_devout->delay[jack_devout->tic_counter];
  delay_counter = jack_devout->delay_counter;

  note_offset = jack_devout->note_offset;
  note_offset_absolute = jack_devout->note_offset_absolute;
  
  loop_left = jack_devout->loop_left;
  loop_right = jack_devout->loop_right;
  
  do_loop = jack_devout->do_loop;

  g_rec_mutex_unlock(jack_devout_mutex);

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
    g_rec_mutex_lock(jack_devout_mutex);
    
    jack_devout->delay_counter = delay_counter + 1.0 - delay;
    jack_devout->tact_counter += 1.0;

    g_rec_mutex_unlock(jack_devout_mutex);
  }else{
    g_rec_mutex_lock(jack_devout_mutex);
    
    jack_devout->delay_counter += 1.0;

    g_rec_mutex_unlock(jack_devout_mutex);
  }
}

void
ags_jack_devout_offset_changed(AgsSoundcard *soundcard,
			       guint note_offset)
{
  AgsJackDevout *jack_devout;
  
  GRecMutex *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* offset changed */
  g_rec_mutex_lock(jack_devout_mutex);

  jack_devout->tic_counter += 1;

  if(jack_devout->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    jack_devout->tic_counter = 0;
  }

  g_rec_mutex_unlock(jack_devout_mutex);
}

void
ags_jack_devout_set_bpm(AgsSoundcard *soundcard,
			gdouble bpm)
{
  AgsJackDevout *jack_devout;

  GRecMutex *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* set bpm */
  g_rec_mutex_lock(jack_devout_mutex);

  jack_devout->bpm = bpm;

  g_rec_mutex_unlock(jack_devout_mutex);

  ags_jack_devout_adjust_delay_and_attack(jack_devout);
}

gdouble
ags_jack_devout_get_bpm(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  gdouble bpm;
  
  GRecMutex *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* get bpm */
  g_rec_mutex_lock(jack_devout_mutex);

  bpm = jack_devout->bpm;
  
  g_rec_mutex_unlock(jack_devout_mutex);

  return(bpm);
}

void
ags_jack_devout_set_delay_factor(AgsSoundcard *soundcard,
				 gdouble delay_factor)
{
  AgsJackDevout *jack_devout;

  GRecMutex *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* set delay factor */
  g_rec_mutex_lock(jack_devout_mutex);

  jack_devout->delay_factor = delay_factor;

  g_rec_mutex_unlock(jack_devout_mutex);

  ags_jack_devout_adjust_delay_and_attack(jack_devout);
}

gdouble
ags_jack_devout_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  gdouble delay_factor;
  
  GRecMutex *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* get delay factor */
  g_rec_mutex_lock(jack_devout_mutex);

  delay_factor = jack_devout->delay_factor;
  
  g_rec_mutex_unlock(jack_devout_mutex);

  return(delay_factor);
}

gdouble
ags_jack_devout_get_delay(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  guint delay_index;
  gdouble delay;
  
  GRecMutex *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* get delay */
  g_rec_mutex_lock(jack_devout_mutex);

  delay_index = jack_devout->tic_counter;

  delay = jack_devout->delay[delay_index];
  
  g_rec_mutex_unlock(jack_devout_mutex);
  
  return(delay);
}

gdouble
ags_jack_devout_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  gdouble absolute_delay;
  
  GRecMutex *jack_devout_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* get absolute delay */
  g_rec_mutex_lock(jack_devout_mutex);

  absolute_delay = (60.0 * (((gdouble) jack_devout->samplerate / (gdouble) jack_devout->buffer_size) / (gdouble) jack_devout->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) jack_devout->delay_factor)));

  g_rec_mutex_unlock(jack_devout_mutex);

  return(absolute_delay);
}

guint
ags_jack_devout_get_attack(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  guint attack_index;
  guint attack;
  
  GRecMutex *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* get attack */
  g_rec_mutex_lock(jack_devout_mutex);

  attack_index = jack_devout->tic_counter;

  attack = jack_devout->attack[attack_index];

  g_rec_mutex_unlock(jack_devout_mutex);
  
  return(attack);
}

void*
ags_jack_devout_get_buffer(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  void *buffer;

  GRecMutex *jack_devout_mutex;  
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  g_rec_mutex_lock(jack_devout_mutex);

  if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_0){
    buffer = jack_devout->app_buffer[0];
  }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_1){
    buffer = jack_devout->app_buffer[1];
  }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_2){
    buffer = jack_devout->app_buffer[2];
  }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_3){
    buffer = jack_devout->app_buffer[3];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(jack_devout_mutex);

  return(buffer);
}

void*
ags_jack_devout_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  void *buffer;

  GRecMutex *jack_devout_mutex;  
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  g_rec_mutex_lock(jack_devout_mutex);

  if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_0){
    buffer = jack_devout->app_buffer[1];
  }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_1){
    buffer = jack_devout->app_buffer[2];
  }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_2){
    buffer = jack_devout->app_buffer[3];
  }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_3){
    buffer = jack_devout->app_buffer[0];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(jack_devout_mutex);

  return(buffer);
}

void*
ags_jack_devout_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  void *buffer;

  GRecMutex *jack_devout_mutex;  
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  g_rec_mutex_lock(jack_devout_mutex);

  if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_0){
    buffer = jack_devout->app_buffer[3];
  }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_1){
    buffer = jack_devout->app_buffer[0];
  }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_2){
    buffer = jack_devout->app_buffer[1];
  }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_3){
    buffer = jack_devout->app_buffer[2];
  }else{
    buffer = NULL;
  }
  
  g_rec_mutex_unlock(jack_devout_mutex);

  return(buffer);
}

void
ags_jack_devout_lock_buffer(AgsSoundcard *soundcard,
			    void *buffer)
{
  AgsJackDevout *jack_devout;

  GRecMutex *buffer_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(jack_devout->app_buffer != NULL){
    if(buffer == jack_devout->app_buffer[0]){
      buffer_mutex = jack_devout->app_buffer_mutex[0];
    }else if(buffer == jack_devout->app_buffer[1]){
      buffer_mutex = jack_devout->app_buffer_mutex[1];
    }else if(buffer == jack_devout->app_buffer[2]){
      buffer_mutex = jack_devout->app_buffer_mutex[2];
    }else if(buffer == jack_devout->app_buffer[3]){
      buffer_mutex = jack_devout->app_buffer_mutex[3];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}


void
ags_jack_devout_unlock_buffer(AgsSoundcard *soundcard,
			      void *buffer)
{
  AgsJackDevout *jack_devout;

  GRecMutex *buffer_mutex;
  
  jack_devout = AGS_JACK_DEVOUT(soundcard);

  buffer_mutex = NULL;

  if(jack_devout->app_buffer != NULL){
    if(buffer == jack_devout->app_buffer[0]){
      buffer_mutex = jack_devout->app_buffer_mutex[0];
    }else if(buffer == jack_devout->app_buffer[1]){
      buffer_mutex = jack_devout->app_buffer_mutex[1];
    }else if(buffer == jack_devout->app_buffer[2]){
      buffer_mutex = jack_devout->app_buffer_mutex[2];
    }else if(buffer == jack_devout->app_buffer[3]){
      buffer_mutex = jack_devout->app_buffer_mutex[3];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_jack_devout_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  guint delay_counter;
  
  GRecMutex *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);
  
  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* delay counter */
  g_rec_mutex_lock(jack_devout_mutex);

  delay_counter = jack_devout->delay_counter;
  
  g_rec_mutex_unlock(jack_devout_mutex);

  return(delay_counter);
}

void
ags_jack_devout_set_start_note_offset(AgsSoundcard *soundcard,
				      guint start_note_offset)
{
  AgsJackDevout *jack_devout;

  GRecMutex *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* set note offset */
  g_rec_mutex_lock(jack_devout_mutex);

  jack_devout->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(jack_devout_mutex);
}

guint
ags_jack_devout_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  guint start_note_offset;
  
  GRecMutex *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* set note offset */
  g_rec_mutex_lock(jack_devout_mutex);

  start_note_offset = jack_devout->start_note_offset;

  g_rec_mutex_unlock(jack_devout_mutex);

  return(start_note_offset);
}

void
ags_jack_devout_set_note_offset(AgsSoundcard *soundcard,
				guint note_offset)
{
  AgsJackDevout *jack_devout;

  GRecMutex *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* set note offset */
  g_rec_mutex_lock(jack_devout_mutex);

  jack_devout->note_offset = note_offset;

  g_rec_mutex_unlock(jack_devout_mutex);
}

guint
ags_jack_devout_get_note_offset(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  guint note_offset;
  
  GRecMutex *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* set note offset */
  g_rec_mutex_lock(jack_devout_mutex);

  note_offset = jack_devout->note_offset;

  g_rec_mutex_unlock(jack_devout_mutex);

  return(note_offset);
}

void
ags_jack_devout_set_note_offset_absolute(AgsSoundcard *soundcard,
					 guint note_offset_absolute)
{
  AgsJackDevout *jack_devout;
  
  GRecMutex *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* set note offset */
  g_rec_mutex_lock(jack_devout_mutex);

  jack_devout->note_offset_absolute = note_offset_absolute;

  g_rec_mutex_unlock(jack_devout_mutex);
}

guint
ags_jack_devout_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  guint note_offset_absolute;
  
  GRecMutex *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* set note offset */
  g_rec_mutex_lock(jack_devout_mutex);

  note_offset_absolute = jack_devout->note_offset_absolute;

  g_rec_mutex_unlock(jack_devout_mutex);

  return(note_offset_absolute);
}

void
ags_jack_devout_set_loop(AgsSoundcard *soundcard,
			 guint loop_left, guint loop_right,
			 gboolean do_loop)
{
  AgsJackDevout *jack_devout;

  GRecMutex *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* set loop */
  g_rec_mutex_lock(jack_devout_mutex);

  jack_devout->loop_left = loop_left;
  jack_devout->loop_right = loop_right;
  jack_devout->do_loop = do_loop;

  if(do_loop){
    jack_devout->loop_offset = jack_devout->note_offset;
  }

  g_rec_mutex_unlock(jack_devout_mutex);
}

void
ags_jack_devout_get_loop(AgsSoundcard *soundcard,
			 guint *loop_left, guint *loop_right,
			 gboolean *do_loop)
{
  AgsJackDevout *jack_devout;

  GRecMutex *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* get loop */
  g_rec_mutex_lock(jack_devout_mutex);

  if(loop_left != NULL){
    *loop_left = jack_devout->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = jack_devout->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = jack_devout->do_loop;
  }

  g_rec_mutex_unlock(jack_devout_mutex);
}

guint
ags_jack_devout_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  guint loop_offset;
  
  GRecMutex *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* get loop offset */
  g_rec_mutex_lock(jack_devout_mutex);

  loop_offset = jack_devout->loop_offset;
  
  g_rec_mutex_unlock(jack_devout_mutex);

  return(loop_offset);
}

guint
ags_jack_devout_get_sub_block_count(AgsSoundcard *soundcard)
{
  AgsJackDevout *jack_devout;

  guint sub_block_count;
  
  GRecMutex *jack_devout_mutex;  

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* get loop offset */
  g_rec_mutex_lock(jack_devout_mutex);

  sub_block_count = jack_devout->sub_block_count;
  
  g_rec_mutex_unlock(jack_devout_mutex);

  return(sub_block_count);
}

gboolean
ags_jack_devout_trylock_sub_block(AgsSoundcard *soundcard,
				  void *buffer, guint sub_block)
{
  AgsJackDevout *jack_devout;

  guint pcm_channels;
  guint sub_block_count;
  gboolean success;
  
  GRecMutex *jack_devout_mutex;  
  GRecMutex *sub_block_mutex;

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* get loop offset */
  g_rec_mutex_lock(jack_devout_mutex);

  pcm_channels = jack_devout->pcm_channels;
  sub_block_count = jack_devout->sub_block_count;
  
  g_rec_mutex_unlock(jack_devout_mutex);
  
  sub_block_mutex = NULL;

  success = FALSE;
  
  if(jack_devout->app_buffer != NULL){
    if(buffer == jack_devout->app_buffer[0]){
      sub_block_mutex = jack_devout->sub_block_mutex[sub_block];
    }else if(buffer == jack_devout->app_buffer[1]){
      sub_block_mutex = jack_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == jack_devout->app_buffer[2]){
      sub_block_mutex = jack_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == jack_devout->app_buffer[3]){
      sub_block_mutex = jack_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
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
ags_jack_devout_unlock_sub_block(AgsSoundcard *soundcard,
				 void *buffer, guint sub_block)
{
  AgsJackDevout *jack_devout;

  guint pcm_channels;
  guint sub_block_count;
  
  GRecMutex *jack_devout_mutex;  
  GRecMutex *sub_block_mutex;

  jack_devout = AGS_JACK_DEVOUT(soundcard);

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* get loop offset */
  g_rec_mutex_lock(jack_devout_mutex);

  pcm_channels = jack_devout->pcm_channels;
  sub_block_count = jack_devout->sub_block_count;
  
  g_rec_mutex_unlock(jack_devout_mutex);
  
  sub_block_mutex = NULL;
  
  if(jack_devout->app_buffer != NULL){
    if(buffer == jack_devout->app_buffer[0]){
      sub_block_mutex = jack_devout->sub_block_mutex[sub_block];
    }else if(buffer == jack_devout->app_buffer[1]){
      sub_block_mutex = jack_devout->sub_block_mutex[pcm_channels * sub_block_count + sub_block];
    }else if(buffer == jack_devout->app_buffer[2]){
      sub_block_mutex = jack_devout->sub_block_mutex[2 * pcm_channels * sub_block_count + sub_block];
    }else if(buffer == jack_devout->app_buffer[3]){
      sub_block_mutex = jack_devout->sub_block_mutex[3 * pcm_channels * sub_block_count + sub_block];
    }
  }

  if(sub_block_mutex != NULL){
    g_rec_mutex_unlock(sub_block_mutex);
  }
}

/**
 * ags_jack_devout_switch_buffer_flag:
 * @jack_devout: an #AgsJackDevout
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 3.0.0
 */
void
ags_jack_devout_switch_buffer_flag(AgsJackDevout *jack_devout)
{
  GRecMutex *jack_devout_mutex;
  
  if(!AGS_IS_JACK_DEVOUT(jack_devout)){
    return;
  }

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* switch buffer flag */
  g_rec_mutex_lock(jack_devout_mutex);

  if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_0){
    jack_devout->app_buffer_mode = AGS_JACK_DEVOUT_APP_BUFFER_1;
  }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_1){
    jack_devout->app_buffer_mode = AGS_JACK_DEVOUT_APP_BUFFER_2;
  }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_2){
    jack_devout->app_buffer_mode = AGS_JACK_DEVOUT_APP_BUFFER_3;
  }else if(jack_devout->app_buffer_mode == AGS_JACK_DEVOUT_APP_BUFFER_3){
    jack_devout->app_buffer_mode = AGS_JACK_DEVOUT_APP_BUFFER_0;
  }

  g_rec_mutex_unlock(jack_devout_mutex);
}

/**
 * ags_jack_devout_adjust_delay_and_attack:
 * @jack_devout: the #AgsJackDevout
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 3.0.0
 */
void
ags_jack_devout_adjust_delay_and_attack(AgsJackDevout *jack_devout)
{
  if(!AGS_IS_JACK_DEVOUT(jack_devout)){
    return;
  }

  ags_soundcard_util_adjust_delay_and_attack(jack_devout);
}

/**
 * ags_jack_devout_realloc_buffer:
 * @jack_devout: the #AgsJackDevout
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 3.0.0
 */
void
ags_jack_devout_realloc_buffer(AgsJackDevout *jack_devout)
{
  AgsJackClient *jack_client;
  
  guint port_count;
  guint pcm_channels;
  guint buffer_size;
  guint format;
  guint word_size;

  GRecMutex *jack_devout_mutex;  

  if(!AGS_IS_JACK_DEVOUT(jack_devout)){
    return;
  }

  /* get jack devout mutex */
  jack_devout_mutex = AGS_JACK_DEVOUT_GET_OBJ_MUTEX(jack_devout);

  /* get word size */  
  g_rec_mutex_lock(jack_devout_mutex);

  jack_client = (AgsJackClient *) jack_devout->jack_client;
  
  port_count = g_list_length(jack_devout->jack_port);
  
  pcm_channels = jack_devout->pcm_channels;
  buffer_size = jack_devout->buffer_size;

  format = jack_devout->format;
  
  g_rec_mutex_unlock(jack_devout_mutex);

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
    g_warning("ags_jack_devout_realloc_buffer(): unsupported word size");
    return;
  }

  if(port_count < pcm_channels){
    AgsJackPort *jack_port;

    gchar *str;
    
    guint nth_soundcard;
    guint i;

    g_rec_mutex_lock(jack_devout_mutex);

    if(jack_devout->card_uri != NULL){
      sscanf(jack_devout->card_uri,
	     "ags-jack-devout-%u",
	     &nth_soundcard);
    }else{
      g_rec_mutex_unlock(jack_devout_mutex);

      g_warning("ags_jack_devout_realloc_buffer() - card uri not set");
      
      return;
    }

    g_rec_mutex_unlock(jack_devout_mutex);
    
    for(i = port_count; i < pcm_channels; i++){
      str = g_strdup_printf("ags%d-%04d",
			    nth_soundcard,
			    i);
      
      jack_port = ags_jack_port_new((GObject *) jack_client);
      ags_jack_client_add_port((AgsJackClient *) jack_client,
			       (GObject *) jack_port);

      g_rec_mutex_lock(jack_devout_mutex);
    
      jack_devout->jack_port = g_list_prepend(jack_devout->jack_port,
					      jack_port);
    
      if(jack_devout->port_name == NULL){
	jack_devout->port_name = (gchar **) g_malloc(2 * sizeof(gchar *));
	jack_devout->port_name[0] = g_strdup(str);
      }else{
	jack_devout->port_name = (gchar **) g_realloc(jack_devout->port_name,
						      (i + 2) * sizeof(gchar *));
	jack_devout->port_name[i] = g_strdup(str);
      }
      
      g_rec_mutex_unlock(jack_devout_mutex);
      
      ags_jack_port_register(jack_port,
			     str,
			     TRUE, FALSE,
			     TRUE);
    }

    jack_devout->port_name[jack_devout->pcm_channels] = NULL;    
  }else if(port_count > pcm_channels){
    GList *jack_port_start, *jack_port;

    guint i;

    g_rec_mutex_lock(jack_devout_mutex);

    jack_port =
      jack_port_start = g_list_copy(jack_devout->jack_port);

    g_rec_mutex_unlock(jack_devout_mutex);

    for(i = 0; i < port_count - pcm_channels; i++){
      jack_devout->jack_port = g_list_remove(jack_devout->jack_port,
					     jack_port->data);
      ags_jack_port_unregister(jack_port->data);
      
      g_object_unref(jack_port->data);
      
      jack_port = jack_port->next;
    }

    g_list_free(jack_port_start);

    g_rec_mutex_lock(jack_devout_mutex);
    
    jack_devout->port_name = (gchar **) g_realloc(jack_devout->port_name,
						  (jack_devout->pcm_channels + 1) * sizeof(gchar *));
    jack_devout->port_name[jack_devout->pcm_channels] = NULL;

    g_rec_mutex_unlock(jack_devout_mutex);
  }
  
  /* AGS_JACK_DEVOUT_BUFFER_0 */
  if(jack_devout->app_buffer[0] != NULL){
    g_free(jack_devout->app_buffer[0]);
  }
  
  jack_devout->app_buffer[0] = (void *) g_malloc(jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  
  /* AGS_JACK_DEVOUT_BUFFER_1 */
  if(jack_devout->app_buffer[1] != NULL){
    g_free(jack_devout->app_buffer[1]);
  }

  jack_devout->app_buffer[1] = (void *) g_malloc(jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  
  /* AGS_JACK_DEVOUT_BUFFER_2 */
  if(jack_devout->app_buffer[2] != NULL){
    g_free(jack_devout->app_buffer[2]);
  }

  jack_devout->app_buffer[2] = (void *) g_malloc(jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
  
  /* AGS_JACK_DEVOUT_BUFFER_3 */
  if(jack_devout->app_buffer[3] != NULL){
    g_free(jack_devout->app_buffer[3]);
  }
  
  jack_devout->app_buffer[3] = (void *) g_malloc(jack_devout->pcm_channels * jack_devout->buffer_size * word_size);
}

/**
 * ags_jack_devout_new:
 *
 * Creates a new instance of #AgsJackDevout.
 *
 * Returns: the new #AgsJackDevout
 *
 * Since: 3.0.0
 */
AgsJackDevout*
ags_jack_devout_new()
{
  AgsJackDevout *jack_devout;

  jack_devout = (AgsJackDevout *) g_object_new(AGS_TYPE_JACK_DEVOUT,
					       NULL);
  
  return(jack_devout);
}
