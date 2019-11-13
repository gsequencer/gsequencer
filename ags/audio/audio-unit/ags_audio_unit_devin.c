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

#include <ags/audio/audio-unit/ags_audio_unit_devin.h>

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

void ags_audio_unit_devin_class_init(AgsAudioUnitDevinClass *audio_unit_devin);
void ags_audio_unit_devin_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_unit_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard);
void ags_audio_unit_devin_init(AgsAudioUnitDevin *audio_unit_devin);
void ags_audio_unit_devin_set_property(GObject *gobject,
				       guint prop_id,
				       const GValue *value,
				       GParamSpec *param_spec);
void ags_audio_unit_devin_get_property(GObject *gobject,
				       guint prop_id,
				       GValue *value,
				       GParamSpec *param_spec);
void ags_audio_unit_devin_dispose(GObject *gobject);
void ags_audio_unit_devin_finalize(GObject *gobject);

AgsUUID* ags_audio_unit_devin_get_uuid(AgsConnectable *connectable);
gboolean ags_audio_unit_devin_has_resource(AgsConnectable *connectable);
gboolean ags_audio_unit_devin_is_ready(AgsConnectable *connectable);
void ags_audio_unit_devin_add_to_registry(AgsConnectable *connectable);
void ags_audio_unit_devin_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_audio_unit_devin_list_resource(AgsConnectable *connectable);
xmlNode* ags_audio_unit_devin_xml_compose(AgsConnectable *connectable);
void ags_audio_unit_devin_xml_parse(AgsConnectable *connectable,
				    xmlNode *node);
gboolean ags_audio_unit_devin_is_connected(AgsConnectable *connectable);
void ags_audio_unit_devin_connect(AgsConnectable *connectable);
void ags_audio_unit_devin_disconnect(AgsConnectable *connectable);

void ags_audio_unit_devin_set_device(AgsSoundcard *soundcard,
				     gchar *device);
gchar* ags_audio_unit_devin_get_device(AgsSoundcard *soundcard);

void ags_audio_unit_devin_set_presets(AgsSoundcard *soundcard,
				      guint channels,
				      guint rate,
				      guint buffer_size,
				      guint format);
void ags_audio_unit_devin_get_presets(AgsSoundcard *soundcard,
				      guint *channels,
				      guint *rate,
				      guint *buffer_size,
				      guint *format);

void ags_audio_unit_devin_list_cards(AgsSoundcard *soundcard,
				     GList **card_id, GList **card_name);
void ags_audio_unit_devin_pcm_info(AgsSoundcard *soundcard, gchar *card_id,
				   guint *channels_min, guint *channels_max,
				   guint *rate_min, guint *rate_max,
				   guint *buffer_size_min, guint *buffer_size_max,
				   GError **error);
guint ags_audio_unit_devin_get_capability(AgsSoundcard *soundcard);

gboolean ags_audio_unit_devin_is_starting(AgsSoundcard *soundcard);
gboolean ags_audio_unit_devin_is_recording(AgsSoundcard *soundcard);

gchar* ags_audio_unit_devin_get_uptime(AgsSoundcard *soundcard);

void ags_audio_unit_devin_port_init(AgsSoundcard *soundcard,
				    GError **error);
void ags_audio_unit_devin_port_record(AgsSoundcard *soundcard,
				      GError **error);
void ags_audio_unit_devin_port_free(AgsSoundcard *soundcard);

void ags_audio_unit_devin_tic(AgsSoundcard *soundcard);
void ags_audio_unit_devin_offset_changed(AgsSoundcard *soundcard,
					 guint note_offset);

void ags_audio_unit_devin_set_bpm(AgsSoundcard *soundcard,
				  gdouble bpm);
gdouble ags_audio_unit_devin_get_bpm(AgsSoundcard *soundcard);

void ags_audio_unit_devin_set_delay_factor(AgsSoundcard *soundcard,
					   gdouble delay_factor);
gdouble ags_audio_unit_devin_get_delay_factor(AgsSoundcard *soundcard);

gdouble ags_audio_unit_devin_get_absolute_delay(AgsSoundcard *soundcard);

gdouble ags_audio_unit_devin_get_delay(AgsSoundcard *soundcard);
guint ags_audio_unit_devin_get_attack(AgsSoundcard *soundcard);

void* ags_audio_unit_devin_get_buffer(AgsSoundcard *soundcard);
void* ags_audio_unit_devin_get_next_buffer(AgsSoundcard *soundcard);
void* ags_audio_unit_devin_get_prev_buffer(AgsSoundcard *soundcard);

void ags_audio_unit_devin_lock_buffer(AgsSoundcard *soundcard,
				      void *buffer);
void ags_audio_unit_devin_unlock_buffer(AgsSoundcard *soundcard,
					void *buffer);

guint ags_audio_unit_devin_get_delay_counter(AgsSoundcard *soundcard);

void ags_audio_unit_devin_set_start_note_offset(AgsSoundcard *soundcard,
						guint start_note_offset);
guint ags_audio_unit_devin_get_start_note_offset(AgsSoundcard *soundcard);

void ags_audio_unit_devin_set_note_offset(AgsSoundcard *soundcard,
					  guint note_offset);
guint ags_audio_unit_devin_get_note_offset(AgsSoundcard *soundcard);

void ags_audio_unit_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
						   guint note_offset);
guint ags_audio_unit_devin_get_note_offset_absolute(AgsSoundcard *soundcard);

void ags_audio_unit_devin_set_loop(AgsSoundcard *soundcard,
				   guint loop_left, guint loop_right,
				   gboolean do_loop);
void ags_audio_unit_devin_get_loop(AgsSoundcard *soundcard,
				   guint *loop_left, guint *loop_right,
				   gboolean *do_loop);

guint ags_audio_unit_devin_get_loop_offset(AgsSoundcard *soundcard);

/**
 * SECTION:ags_audio_unit_devin
 * @short_description: Output to soundcard
 * @title: AgsAudioUnitDevin
 * @section_id:
 * @include: ags/audio/audio-unit/ags_audio_unit_devin.h
 *
 * #AgsAudioUnitDevin represents a soundcard and supports output.
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

static gpointer ags_audio_unit_devin_parent_class = NULL;

GType
ags_audio_unit_devin_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_audio_unit_devin = 0;

    static const GTypeInfo ags_audio_unit_devin_info = {
      sizeof(AgsAudioUnitDevinClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_unit_devin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsAudioUnitDevin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_unit_devin_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_unit_devin_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_soundcard_interface_info = {
      (GInterfaceInitFunc) ags_audio_unit_devin_soundcard_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_audio_unit_devin = g_type_register_static(G_TYPE_OBJECT,
						       "AgsAudioUnitDevin",
						       &ags_audio_unit_devin_info,
						       0);

    g_type_add_interface_static(ags_type_audio_unit_devin,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_audio_unit_devin,
				AGS_TYPE_SOUNDCARD,
				&ags_soundcard_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_audio_unit_devin);
  }

  return g_define_type_id__volatile;
}

void
ags_audio_unit_devin_class_init(AgsAudioUnitDevinClass *audio_unit_devin)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_audio_unit_devin_parent_class = g_type_class_peek_parent(audio_unit_devin);

  /* GObjectClass */
  gobject = (GObjectClass *) audio_unit_devin;

  gobject->set_property = ags_audio_unit_devin_set_property;
  gobject->get_property = ags_audio_unit_devin_get_property;

  gobject->dispose = ags_audio_unit_devin_dispose;
  gobject->finalize = ags_audio_unit_devin_finalize;

  /* properties */
  /**
   * AgsAudioUnitDevin:device:
   *
   * The audio unit soundcard indentifier
   * 
   * Since: 2.3.14
   */
  param_spec = g_param_spec_string("device",
				   i18n_pspec("the device identifier"),
				   i18n_pspec("The device to perform output to"),
				   "ags-audio-unit-devin-0",
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DEVICE,
				  param_spec);
  
  /**
   * AgsAudioUnitDevin:dsp-channels:
   *
   * The dsp channel count
   * 
   * Since: 2.3.14
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
   * AgsAudioUnitDevin:pcm-channels:
   *
   * The pcm channel count
   * 
   * Since: 2.3.14
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
   * AgsAudioUnitDevin:format:
   *
   * The precision of the buffer
   * 
   * Since: 2.3.14
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
   * AgsAudioUnitDevin:buffer-size:
   *
   * The buffer size
   * 
   * Since: 2.3.14
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
   * AgsAudioUnitDevin:samplerate:
   *
   * The samplerate
   * 
   * Since: 2.3.14
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
   * AgsAudioUnitDevin:buffer:
   *
   * The buffer
   * 
   * Since: 2.3.14
   */
  param_spec = g_param_spec_pointer("buffer",
				    i18n_pspec("the buffer"),
				    i18n_pspec("The buffer to play"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_BUFFER,
				  param_spec);

  /**
   * AgsAudioUnitDevin:bpm:
   *
   * Beats per minute
   * 
   * Since: 2.3.14
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
   * AgsAudioUnitDevin:delay-factor:
   *
   * tact
   * 
   * Since: 2.3.14
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
   * AgsAudioUnitDevin:attack:
   *
   * Attack of the buffer
   * 
   * Since: 2.3.14
   */
  param_spec = g_param_spec_pointer("attack",
				    i18n_pspec("attack of buffer"),
				    i18n_pspec("The attack to use for the buffer"),
				    G_PARAM_READABLE);
  g_object_class_install_property(gobject,
				  PROP_ATTACK,
				  param_spec);

  
  /**
   * AgsAudioUnitDevin:audio-unit-client:
   *
   * The assigned #AgsAudioUnitClient
   * 
   * Since: 2.3.14
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
   * AgsAudioUnitDevin:audio-unit-port:
   *
   * The assigned #AgsAudioUnitPort
   * 
   * Since: 2.3.14
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
ags_audio_unit_devin_error_quark()
{
  return(g_quark_from_static_string("ags-audio_unit_devin-error-quark"));
}

void
ags_audio_unit_devin_connectable_interface_init(AgsConnectableInterface *connectable)
{
  //TODO:JK: implement me
}

void
ags_audio_unit_devin_soundcard_interface_init(AgsSoundcardInterface *soundcard)
{
  soundcard->set_device = ags_audio_unit_devin_set_device;
  soundcard->get_device = ags_audio_unit_devin_get_device;
  
  soundcard->set_presets = ags_audio_unit_devin_set_presets;
  soundcard->get_presets = ags_audio_unit_devin_get_presets;

  soundcard->list_cards = ags_audio_unit_devin_list_cards;
  soundcard->pcm_info = ags_audio_unit_devin_pcm_info;
  soundcard->get_capability = ags_audio_unit_devin_get_capability;

  soundcard->get_poll_fd = NULL;
  soundcard->is_available = NULL;

  soundcard->is_starting =  ags_audio_unit_devin_is_starting;
  soundcard->is_playing = NULL;
  soundcard->is_recording = ags_audio_unit_devin_is_recording;

  soundcard->get_uptime = ags_audio_unit_devin_get_uptime;
  
  soundcard->play_init = NULL;
  soundcard->play = NULL;

  soundcard->record_init = ags_audio_unit_devin_port_init;
  soundcard->record = ags_audio_unit_devin_port_record;

  soundcard->stop = ags_audio_unit_devin_port_free;

  soundcard->tic = ags_audio_unit_devin_tic;
  soundcard->offset_changed = ags_audio_unit_devin_offset_changed;
    
  soundcard->set_bpm = ags_audio_unit_devin_set_bpm;
  soundcard->get_bpm = ags_audio_unit_devin_get_bpm;

  soundcard->set_delay_factor = ags_audio_unit_devin_set_delay_factor;
  soundcard->get_delay_factor = ags_audio_unit_devin_get_delay_factor;
  
  soundcard->get_absolute_delay = ags_audio_unit_devin_get_absolute_delay;

  soundcard->get_delay = ags_audio_unit_devin_get_delay;
  soundcard->get_attack = ags_audio_unit_devin_get_attack;

  soundcard->get_buffer = ags_audio_unit_devin_get_buffer;
  soundcard->get_next_buffer = ags_audio_unit_devin_get_next_buffer;
  soundcard->get_prev_buffer = ags_audio_unit_devin_get_prev_buffer;
  
  soundcard->lock_buffer = ags_audio_unit_devin_lock_buffer;
  soundcard->unlock_buffer = ags_audio_unit_devin_unlock_buffer;

  soundcard->get_delay_counter = ags_audio_unit_devin_get_delay_counter;

  soundcard->set_start_note_offset = ags_audio_unit_devin_set_start_note_offset;
  soundcard->get_start_note_offset = ags_audio_unit_devin_get_start_note_offset;

  soundcard->set_note_offset = ags_audio_unit_devin_set_note_offset;
  soundcard->get_note_offset = ags_audio_unit_devin_get_note_offset;

  soundcard->set_note_offset_absolute = ags_audio_unit_devin_set_note_offset_absolute;
  soundcard->get_note_offset_absolute = ags_audio_unit_devin_get_note_offset_absolute;

  soundcard->set_loop = ags_audio_unit_devin_set_loop;
  soundcard->get_loop = ags_audio_unit_devin_get_loop;

  soundcard->get_loop_offset = ags_audio_unit_devin_get_loop_offset;
}

void
ags_audio_unit_devin_init(AgsAudioUnitDevin *audio_unit_devin)
{
  AgsConfig *config;

  gchar *str;
  gchar *segmentation;

  guint denumerator, numerator;
  guint i;

  /* flags */
  audio_unit_devin->flags = 0;
  g_atomic_int_set(&(audio_unit_devin->sync_flags),
		   AGS_AUDIO_UNIT_DEVIN_PASS_THROUGH);

  /* devin mutex */
  g_rec_mutex_init(&(audio_unit_devin->obj_mutex));

  /* uuid */
  audio_unit_devin->uuid = ags_uuid_alloc();
  ags_uuid_generate(audio_unit_devin->uuid);

  /* presets */
  config = ags_config_get_instance();

  audio_unit_devin->dsp_channels = ags_soundcard_helper_config_get_dsp_channels(config);
  audio_unit_devin->pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);

  audio_unit_devin->samplerate = ags_soundcard_helper_config_get_samplerate(config);
  audio_unit_devin->buffer_size = ags_soundcard_helper_config_get_buffer_size(config);
  audio_unit_devin->format = ags_soundcard_helper_config_get_format(config);

  /*  */
  audio_unit_devin->card_uri = NULL;
  audio_unit_devin->audio_unit_client = NULL;

  audio_unit_devin->port_name = NULL;
  audio_unit_devin->audio_unit_port = NULL;

  /* buffer */
  audio_unit_devin->buffer_mutex = (GRecMutex **) malloc(8 * sizeof(GRecMutex *));

  for(i = 0; i < 8; i++){
    audio_unit_devin->buffer_mutex[i] = (GRecMutex *) malloc(sizeof(GRecMutex));

    g_rec_mutex_init(audio_unit_devin->buffer_mutex[i]);
  }

  audio_unit_devin->buffer = (void **) malloc(8 * sizeof(void*));

  audio_unit_devin->buffer[0] = NULL;
  audio_unit_devin->buffer[1] = NULL;
  audio_unit_devin->buffer[2] = NULL;
  audio_unit_devin->buffer[3] = NULL;
  audio_unit_devin->buffer[4] = NULL;
  audio_unit_devin->buffer[5] = NULL;
  audio_unit_devin->buffer[6] = NULL;
  audio_unit_devin->buffer[7] = NULL;
  
  ags_audio_unit_devin_realloc_buffer(audio_unit_devin);
  
  /* bpm */
  audio_unit_devin->bpm = AGS_SOUNDCARD_DEFAULT_BPM;

  /* delay factor */
  audio_unit_devin->delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;
  
  /* segmentation */
  segmentation = ags_config_get_value(config,
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    sscanf(segmentation, "%d/%d",
	   &denumerator,
	   &numerator);
    
    audio_unit_devin->delay_factor = 1.0 / numerator * (numerator / denumerator);

    g_free(segmentation);
  }

  /* delay and attack */
  audio_unit_devin->delay = (gdouble *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
						sizeof(gdouble));
  
  audio_unit_devin->attack = (guint *) malloc((int) 2 * AGS_SOUNDCARD_DEFAULT_PERIOD *
					       sizeof(guint));

  ags_audio_unit_devin_adjust_delay_and_attack(audio_unit_devin);
  
  /* counters */
  audio_unit_devin->tact_counter = 0.0;
  audio_unit_devin->delay_counter = 0.0;
  audio_unit_devin->tic_counter = 0;

  audio_unit_devin->start_note_offset = 0;
  audio_unit_devin->note_offset = 0;
  audio_unit_devin->note_offset_absolute = 0;

  audio_unit_devin->loop_left = AGS_SOUNDCARD_DEFAULT_LOOP_LEFT;
  audio_unit_devin->loop_right = AGS_SOUNDCARD_DEFAULT_LOOP_RIGHT;

  audio_unit_devin->do_loop = FALSE;

  audio_unit_devin->loop_offset = 0;

  /* callback mutex */
  g_mutex_init(&(audio_unit_devin->callback_mutex));

  g_cond_init(&(audio_unit_devin->callback_cond));

  /* callback finish mutex */
  g_mutex_init(&(audio_unit_devin->callback_finish_mutex));

  g_cond_init(&(audio_unit_devin->callback_finish_cond));
}

void
ags_audio_unit_devin_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec)
{
  AgsAudioUnitDevin *audio_unit_devin;
  
  GRecMutex *audio_unit_devin_mutex;

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(gobject);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      char *device;

      device = (char *) g_value_get_string(value);

      g_rec_mutex_lock(audio_unit_devin_mutex);

      audio_unit_devin->card_uri = g_strdup(device);

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      guint dsp_channels;

      dsp_channels = g_value_get_uint(value);

      g_rec_mutex_lock(audio_unit_devin_mutex);

      if(dsp_channels == audio_unit_devin->dsp_channels){
	g_rec_mutex_unlock(audio_unit_devin_mutex);

	return;
      }

      audio_unit_devin->dsp_channels = dsp_channels;

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      guint pcm_channels;

      pcm_channels = g_value_get_uint(value);

      g_rec_mutex_lock(audio_unit_devin_mutex);

      if(pcm_channels == audio_unit_devin->pcm_channels){
	g_rec_mutex_unlock(audio_unit_devin_mutex);

	return;
      }

      audio_unit_devin->pcm_channels = pcm_channels;

      g_rec_mutex_unlock(audio_unit_devin_mutex);

      ags_audio_unit_devin_realloc_buffer(audio_unit_devin);
    }
    break;
  case PROP_FORMAT:
    {
      guint format;

      format = g_value_get_uint(value);

      g_rec_mutex_lock(audio_unit_devin_mutex);

      if(format == audio_unit_devin->format){
	g_rec_mutex_unlock(audio_unit_devin_mutex);

	return;
      }

      audio_unit_devin->format = format;

      g_rec_mutex_unlock(audio_unit_devin_mutex);

      ags_audio_unit_devin_realloc_buffer(audio_unit_devin);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      guint buffer_size;

      buffer_size = g_value_get_uint(value);

      g_rec_mutex_lock(audio_unit_devin_mutex);

      if(buffer_size == audio_unit_devin->buffer_size){
	g_rec_mutex_unlock(audio_unit_devin_mutex);

	return;
      }

      audio_unit_devin->buffer_size = buffer_size;

      g_rec_mutex_unlock(audio_unit_devin_mutex);

      ags_audio_unit_devin_realloc_buffer(audio_unit_devin);
      ags_audio_unit_devin_adjust_delay_and_attack(audio_unit_devin);
    }
    break;
  case PROP_SAMPLERATE:
    {
      guint samplerate;

      samplerate = g_value_get_uint(value);

      g_rec_mutex_lock(audio_unit_devin_mutex);
      
      if(samplerate == audio_unit_devin->samplerate){
	g_rec_mutex_unlock(audio_unit_devin_mutex);

	return;
      }

      audio_unit_devin->samplerate = samplerate;

      g_rec_mutex_unlock(audio_unit_devin_mutex);

      ags_audio_unit_devin_realloc_buffer(audio_unit_devin);
      ags_audio_unit_devin_adjust_delay_and_attack(audio_unit_devin);
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

      g_rec_mutex_lock(audio_unit_devin_mutex);

      audio_unit_devin->bpm = bpm;

      g_rec_mutex_unlock(audio_unit_devin_mutex);

      ags_audio_unit_devin_adjust_delay_and_attack(audio_unit_devin);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      gdouble delay_factor;
      
      delay_factor = g_value_get_double(value);

      g_rec_mutex_lock(audio_unit_devin_mutex);

      audio_unit_devin->delay_factor = delay_factor;

      g_rec_mutex_unlock(audio_unit_devin_mutex);

      ags_audio_unit_devin_adjust_delay_and_attack(audio_unit_devin);
    }
    break;
  case PROP_AUDIO_UNIT_CLIENT:
    {
      AgsAudioUnitClient *audio_unit_client;

      audio_unit_client = (AgsAudioUnitClient *) g_value_get_object(value);

      g_rec_mutex_lock(audio_unit_devin_mutex);

      if(audio_unit_devin->audio_unit_client == (GObject *) audio_unit_client){
	g_rec_mutex_unlock(audio_unit_devin_mutex);

	return;
      }

      if(audio_unit_devin->audio_unit_client != NULL){
	g_object_unref(G_OBJECT(audio_unit_devin->audio_unit_client));
      }

      if(audio_unit_client != NULL){
	g_object_ref(audio_unit_client);
      }
      
      audio_unit_devin->audio_unit_client = (GObject *) audio_unit_client;

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  case PROP_AUDIO_UNIT_PORT:
    {
      AgsAudioUnitPort *audio_unit_port;

      audio_unit_port = (AgsAudioUnitPort *) g_value_get_pointer(value);

      g_rec_mutex_lock(audio_unit_devin_mutex);

      if(!AGS_IS_AUDIO_UNIT_PORT(audio_unit_port) ||
	 g_list_find(audio_unit_devin->audio_unit_port, audio_unit_port) != NULL){
	g_rec_mutex_unlock(audio_unit_devin_mutex);

	return;
      }

      g_object_ref(audio_unit_port);
      audio_unit_devin->audio_unit_port = g_list_append(audio_unit_devin->audio_unit_port,
							audio_unit_port);

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_devin_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec)
{
  AgsAudioUnitDevin *audio_unit_devin;

  GRecMutex *audio_unit_devin_mutex;

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(gobject);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);
  
  switch(prop_id){
  case PROP_DEVICE:
    {
      g_rec_mutex_lock(audio_unit_devin_mutex);

      g_value_set_string(value, audio_unit_devin->card_uri);

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  case PROP_DSP_CHANNELS:
    {
      g_rec_mutex_lock(audio_unit_devin_mutex);

      g_value_set_uint(value, audio_unit_devin->dsp_channels);

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  case PROP_PCM_CHANNELS:
    {
      g_rec_mutex_lock(audio_unit_devin_mutex);

      g_value_set_uint(value, audio_unit_devin->pcm_channels);

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  case PROP_FORMAT:
    {
      g_rec_mutex_lock(audio_unit_devin_mutex);

      g_value_set_uint(value, audio_unit_devin->format);

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  case PROP_BUFFER_SIZE:
    {
      g_rec_mutex_lock(audio_unit_devin_mutex);

      g_value_set_uint(value, audio_unit_devin->buffer_size);

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  case PROP_SAMPLERATE:
    {
      g_rec_mutex_lock(audio_unit_devin_mutex);

      g_value_set_uint(value, audio_unit_devin->samplerate);

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  case PROP_BUFFER:
    {
      g_rec_mutex_lock(audio_unit_devin_mutex);

      g_value_set_pointer(value, audio_unit_devin->buffer);

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  case PROP_BPM:
    {
      g_rec_mutex_lock(audio_unit_devin_mutex);

      g_value_set_double(value, audio_unit_devin->bpm);

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  case PROP_DELAY_FACTOR:
    {
      g_rec_mutex_lock(audio_unit_devin_mutex);

      g_value_set_double(value, audio_unit_devin->delay_factor);

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  case PROP_ATTACK:
    {
      g_rec_mutex_lock(audio_unit_devin_mutex);

      g_value_set_pointer(value, audio_unit_devin->attack);

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  case PROP_AUDIO_UNIT_CLIENT:
    {
      g_rec_mutex_lock(audio_unit_devin_mutex);

      g_value_set_object(value, audio_unit_devin->audio_unit_client);

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  case PROP_AUDIO_UNIT_PORT:
    {
      g_rec_mutex_lock(audio_unit_devin_mutex);

      g_value_set_pointer(value,
			  g_list_copy_deep(audio_unit_devin->audio_unit_port,
					   (GCopyFunc) g_object_ref,
					   NULL));

      g_rec_mutex_unlock(audio_unit_devin_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_audio_unit_devin_dispose(GObject *gobject)
{
  AgsAudioUnitDevin *audio_unit_devin;

  GList *list;

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(gobject);

  //TODO:JK: implement me
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_devin_parent_class)->dispose(gobject);
}

void
ags_audio_unit_devin_finalize(GObject *gobject)
{
  AgsAudioUnitDevin *audio_unit_devin;

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(gobject);

  //TODO:JK: implement me
  
  /* call parent */
  G_OBJECT_CLASS(ags_audio_unit_devin_parent_class)->finalize(gobject);
}

/**
 * ags_audio_unit_devin_test_flags:
 * @audio_unit_devin: the #AgsAudioUnitDevin
 * @flags: the flags
 *
 * Test @flags to be set on @audio_unit_devin.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 2.3.14
 */
gboolean
ags_audio_unit_devin_test_flags(AgsAudioUnitDevin *audio_unit_devin, guint flags)
{
  gboolean retval;  
  
  GRecMutex *audio_unit_devin_mutex;

  if(!AGS_IS_AUDIO_UNIT_DEVIN(audio_unit_devin)){
    return(FALSE);
  }

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* test */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  retval = (flags & (audio_unit_devin->flags)) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(audio_unit_devin_mutex);

  return(retval);
}

/**
 * ags_audio_unit_devin_set_flags:
 * @audio_unit_devin: the #AgsAudioUnitDevin
 * @flags: see #AgsAudioUnitDevinFlags-enum
 *
 * Enable a feature of @audio_unit_devin.
 *
 * Since: 2.3.14
 */
void
ags_audio_unit_devin_set_flags(AgsAudioUnitDevin *audio_unit_devin, guint flags)
{
  GRecMutex *audio_unit_devin_mutex;

  if(!AGS_IS_AUDIO_UNIT_DEVIN(audio_unit_devin)){
    return;
  }

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  //TODO:JK: add more?

  /* set flags */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  audio_unit_devin->flags |= flags;
  
  g_rec_mutex_unlock(audio_unit_devin_mutex);
}
    
/**
 * ags_audio_unit_devin_unset_flags:
 * @audio_unit_devin: the #AgsAudioUnitDevin
 * @flags: see #AgsAudioUnitDevinFlags-enum
 *
 * Disable a feature of @audio_unit_devin.
 *
 * Since: 2.3.14
 */
void
ags_audio_unit_devin_unset_flags(AgsAudioUnitDevin *audio_unit_devin, guint flags)
{  
  GRecMutex *audio_unit_devin_mutex;

  if(!AGS_IS_AUDIO_UNIT_DEVIN(audio_unit_devin)){
    return;
  }

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  //TODO:JK: add more?

  /* unset flags */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  audio_unit_devin->flags &= (~flags);
  
  g_rec_mutex_unlock(audio_unit_devin_mutex);
}

void
ags_audio_unit_devin_set_device(AgsSoundcard *soundcard,
				gchar *device)
{
  AgsAudioUnitDevin *audio_unit_devin;

  GList *audio_unit_port, *audio_unit_port_start;

  gchar *str;

  guint pcm_channels;
  int ret;
  guint nth_card;
  guint i;
  
  GRecMutex *audio_unit_devin_mutex;

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* check device */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  if(audio_unit_devin->card_uri == device ||
     !g_ascii_strcasecmp(audio_unit_devin->card_uri,
			 device)){
    g_rec_mutex_unlock(audio_unit_devin_mutex);
  
    return;
  }

  if(!g_str_has_prefix(device,
		       "ags-audio-unit-devin-")){
    g_rec_mutex_unlock(audio_unit_devin_mutex);

    g_warning("invalid AudioUnit device prefix");

    return;
  }

  ret = sscanf(device,
	       "ags-audio-unit-devin-%u",
	       &nth_card);

  if(ret != 1){
    g_rec_mutex_unlock(audio_unit_devin_mutex);

    g_warning("invalid AudioUnit device specifier");

    return;
  }

  g_free(audio_unit_devin->card_uri);
  audio_unit_devin->card_uri = g_strdup(device);

  /* apply name to port */
  pcm_channels = audio_unit_devin->pcm_channels;
  
  audio_unit_port_start = 
    audio_unit_port = g_list_copy(audio_unit_devin->audio_unit_port);

  g_rec_mutex_unlock(audio_unit_devin_mutex);
  
  for(i = 0; i < pcm_channels && audio_unit_port != NULL; i++){
    str = g_strdup_printf("ags-soundcard%d-%04d",
			  nth_card,
			  i);
    
    g_object_set(audio_unit_port->data,
		 "port-name", str,
		 NULL);
    g_free(str);

    audio_unit_port = audio_unit_port->next;
  }

  g_list_free(audio_unit_port_start);
}

gchar*
ags_audio_unit_devin_get_device(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;
  
  gchar *device;

  GRecMutex *audio_unit_devin_mutex;

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);
  
  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  device = NULL;

  /* get device */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  device = g_strdup(audio_unit_devin->card_uri);

  g_rec_mutex_unlock(audio_unit_devin_mutex);
  
  return(device);
}

void
ags_audio_unit_devin_set_presets(AgsSoundcard *soundcard,
				 guint channels,
				 guint rate,
				 guint buffer_size,
				 guint format)
{
  AgsAudioUnitDevin *audio_unit_devin;

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  g_object_set(audio_unit_devin,
	       "pcm-channels", channels,
	       "samplerate", rate,
	       "buffer-size", buffer_size,
	       "format", format,
	       NULL);
}

void
ags_audio_unit_devin_get_presets(AgsSoundcard *soundcard,
				 guint *channels,
				 guint *rate,
				 guint *buffer_size,
				 guint *format)
{
  AgsAudioUnitDevin *audio_unit_devin;

  GRecMutex *audio_unit_devin_mutex;

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);
  
  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* get presets */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  if(channels != NULL){
    *channels = audio_unit_devin->pcm_channels;
  }

  if(rate != NULL){
    *rate = audio_unit_devin->samplerate;
  }

  if(buffer_size != NULL){
    *buffer_size = audio_unit_devin->buffer_size;
  }

  if(format != NULL){
    *format = audio_unit_devin->format;
  }

  g_rec_mutex_unlock(audio_unit_devin_mutex);
}

/**
 * ags_audio_unit_devin_list_cards:
 * @soundcard: the #AgsSoundcard
 * @card_id: AUDIO_UNIT identifier
 * @card_name: card name
 *
 * List available soundcards.
 *
 * Since: 2.3.14
 */
void
ags_audio_unit_devin_list_cards(AgsSoundcard *soundcard,
				GList **card_id, GList **card_name)
{
  AgsAudioUnitClient *audio_unit_client;
  AgsAudioUnitDevin *audio_unit_devin;

  AgsApplicationContext *application_context;
  
  GList *list_start, *list;

  gchar *card_uri;
  gchar *client_name;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

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
    if(AGS_IS_AUDIO_UNIT_DEVIN(list->data)){
      if(card_id != NULL){
	card_uri = ags_soundcard_get_device(AGS_SOUNDCARD(list->data));
	
	if(AGS_AUDIO_UNIT_DEVIN(list->data)->card_uri != NULL){
	  *card_id = g_list_prepend(*card_id,
				    card_uri);
	}else{
	  *card_id = g_list_prepend(*card_id,
				    g_strdup("(null)"));

	  g_warning("ags_audio_unit_devin_list_cards() - card id (null)");
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

	  g_warning("ags_audio_unit_devin_list_cards() - AUDIO_UNIT client not connected (null)");
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
ags_audio_unit_devin_pcm_info(AgsSoundcard *soundcard,
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
ags_audio_unit_devin_get_capability(AgsSoundcard *soundcard)
{
  return(AGS_SOUNDCARD_CAPABILITY_CAPTURE);
}

gboolean
ags_audio_unit_devin_is_starting(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  gboolean is_starting;
  
  GRecMutex *audio_unit_devin_mutex;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* check is starting */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  is_starting = ((AGS_AUDIO_UNIT_DEVIN_START_RECORD & (audio_unit_devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(audio_unit_devin_mutex);
  
  return(is_starting);
}

gboolean
ags_audio_unit_devin_is_recording(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  gboolean is_playing;
  
  GRecMutex *audio_unit_devin_mutex;

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);
  
  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* check is starting */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  is_playing = ((AGS_AUDIO_UNIT_DEVIN_RECORD & (audio_unit_devin->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(audio_unit_devin_mutex);

  return(is_playing);
}

gchar*
ags_audio_unit_devin_get_uptime(AgsSoundcard *soundcard)
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
ags_audio_unit_devin_port_init(AgsSoundcard *soundcard,
			       GError **error)
{
  AgsAudioUnitDevin *audio_unit_devin;

  guint format, word_size;
  
  GRecMutex *audio_unit_devin_mutex;

  if(ags_soundcard_is_recording(soundcard)){
    return;
  }
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio-unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* retrieve word size */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  switch(audio_unit_devin->format){
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
    g_rec_mutex_unlock(audio_unit_devin_mutex);
    
    g_warning("ags_audio_unit_devin_port_init(): unsupported word size");
    
    return;
  }
  
  /* prepare for playback */
  audio_unit_devin->flags |= (AGS_AUDIO_UNIT_DEVIN_BUFFER7 |
			       AGS_AUDIO_UNIT_DEVIN_START_RECORD |
			       AGS_AUDIO_UNIT_DEVIN_RECORD |
			       AGS_AUDIO_UNIT_DEVIN_NONBLOCKING);

  memset(audio_unit_devin->buffer[0], 0, audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);
  memset(audio_unit_devin->buffer[1], 0, audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);
  memset(audio_unit_devin->buffer[2], 0, audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);
  memset(audio_unit_devin->buffer[3], 0, audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);
  memset(audio_unit_devin->buffer[4], 0, audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);
  memset(audio_unit_devin->buffer[5], 0, audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);
  memset(audio_unit_devin->buffer[6], 0, audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);
  memset(audio_unit_devin->buffer[7], 0, audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);

  /*  */
  audio_unit_devin->tact_counter = 0.0;
  audio_unit_devin->delay_counter = floor(ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(audio_unit_devin)));
  audio_unit_devin->tic_counter = 0;

  audio_unit_devin->flags |= (AGS_AUDIO_UNIT_DEVIN_INITIALIZED |
			       AGS_AUDIO_UNIT_DEVIN_START_RECORD |
			       AGS_AUDIO_UNIT_DEVIN_RECORD);
  
  g_atomic_int_or(&(audio_unit_devin->sync_flags),
		  AGS_AUDIO_UNIT_DEVIN_INITIAL_CALLBACK);

  g_rec_mutex_unlock(audio_unit_devin_mutex);
}

void
ags_audio_unit_devin_port_record(AgsSoundcard *soundcard,
				 GError **error)
{
  AgsAudioUnitClient *audio_unit_client;
  AgsAudioUnitDevin *audio_unit_devin;

  AgsTicDevice *tic_device;
  AgsClearBuffer *clear_buffer;
  AgsSwitchBufferFlag *switch_buffer_flag;
  
  AgsTaskLauncher *task_launcher;

  AgsApplicationContext *application_context;

  GList *task;
  guint word_size;
  gboolean audio_unit_client_activated;

  GRecMutex *audio_unit_devin_mutex;
  GRecMutex *audio_unit_client_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  application_context = ags_application_context_get_instance();
  
  /* get audio-unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* client */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  audio_unit_client = (AgsAudioUnitClient *) audio_unit_devin->audio_unit_client;
  
  callback_mutex = &(audio_unit_devin->callback_mutex);
  callback_finish_mutex = &(audio_unit_devin->callback_finish_mutex);

  /* do playback */  
  audio_unit_devin->flags &= (~AGS_AUDIO_UNIT_DEVIN_START_RECORD);
  
  switch(audio_unit_devin->format){
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
    g_rec_mutex_unlock(audio_unit_devin_mutex);
    
    g_warning("ags_audio_unit_devin_port_record(): unsupported word size");
    
    return;
  }

  g_rec_mutex_unlock(audio_unit_devin_mutex);

  /* get client mutex */
  audio_unit_client_mutex = AGS_AUDIO_UNIT_CLIENT_GET_OBJ_MUTEX(audio_unit_client);

  /* get activated */
  g_rec_mutex_lock(audio_unit_client_mutex);

  audio_unit_client_activated = ((AGS_AUDIO_UNIT_CLIENT_ACTIVATED & (audio_unit_client->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(audio_unit_client_mutex);

  if(audio_unit_client_activated){
    while((AGS_AUDIO_UNIT_DEVIN_PASS_THROUGH & (g_atomic_int_get(&(audio_unit_devin->sync_flags)))) != 0){
	usleep(4);
    }
    
    /* signal */
    if((AGS_AUDIO_UNIT_DEVIN_INITIAL_CALLBACK & (g_atomic_int_get(&(audio_unit_devin->sync_flags)))) == 0){
      g_mutex_lock(callback_mutex);

      g_atomic_int_or(&(audio_unit_devin->sync_flags),
		      AGS_AUDIO_UNIT_DEVIN_CALLBACK_DONE);
    
      if((AGS_AUDIO_UNIT_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(audio_unit_devin->sync_flags)))) != 0){
	g_cond_signal(&(audio_unit_devin->callback_cond));
      }

      g_mutex_unlock(callback_mutex);
      //    }
    
    /* wait callback */	
      g_mutex_lock(callback_finish_mutex);
    
      if((AGS_AUDIO_UNIT_DEVIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(audio_unit_devin->sync_flags)))) == 0){
	g_atomic_int_or(&(audio_unit_devin->sync_flags),
			AGS_AUDIO_UNIT_DEVIN_CALLBACK_FINISH_WAIT);
    
	while((AGS_AUDIO_UNIT_DEVIN_CALLBACK_FINISH_DONE & (g_atomic_int_get(&(audio_unit_devin->sync_flags)))) == 0 &&
	      (AGS_AUDIO_UNIT_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(audio_unit_devin->sync_flags)))) != 0){
	  g_cond_wait(&(audio_unit_devin->callback_finish_cond),
		      callback_finish_mutex);
	}
      }
      
      g_atomic_int_and(&(audio_unit_devin->sync_flags),
		       (~(AGS_AUDIO_UNIT_DEVIN_CALLBACK_FINISH_WAIT |
			  AGS_AUDIO_UNIT_DEVIN_CALLBACK_FINISH_DONE)));
    
      g_mutex_unlock(callback_finish_mutex);
    }else{
      g_atomic_int_and(&(audio_unit_devin->sync_flags),
		       (~AGS_AUDIO_UNIT_DEVIN_INITIAL_CALLBACK));
    }
  }

  /* update soundcard */
  task_launcher = ags_concurrency_provider_get_task_launcher(AGS_CONCURRENCY_PROVIDER(application_context));

  task = NULL;      
  
  /* tic soundcard */
  tic_device = ags_tic_device_new((GObject *) audio_unit_devin);
  task = g_list_append(task,
		       tic_device);

  /* reset - clear buffer */
  clear_buffer = ags_clear_buffer_new((GObject *) audio_unit_devin);
  task = g_list_append(task,
		       clear_buffer);
    
  /* reset - switch buffer flags */
  switch_buffer_flag = ags_switch_buffer_flag_new((GObject *) audio_unit_devin);
  task = g_list_append(task,
		       switch_buffer_flag);

  /* append tasks */
  ags_task_launcher_add_task_all(task_launcher,
				 task);

  /* unref */
  g_object_unref(task_launcher);
}

void
ags_audio_unit_devin_port_free(AgsSoundcard *soundcard)
{
  AgsAudioUnitPort *audio_unit_port;
  AgsAudioUnitDevin *audio_unit_devin;

  guint word_size;

  GRecMutex *audio_unit_devin_mutex;
  GMutex *callback_mutex;
  GMutex *callback_finish_mutex;

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);
  
  /* get audio-unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /*  */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  if((AGS_AUDIO_UNIT_DEVIN_INITIALIZED & (audio_unit_devin->flags)) == 0){
    g_rec_mutex_unlock(audio_unit_devin_mutex);

    return;
  }

  callback_mutex = &(audio_unit_devin->callback_mutex);
  callback_finish_mutex = &(audio_unit_devin->callback_finish_mutex);
  
  //  g_atomic_int_or(&(AGS_THREAD(application_context->main_loop)->flags),
  //		  AGS_THREAD_TIMING);
  
  audio_unit_devin->flags &= (~(AGS_AUDIO_UNIT_DEVIN_BUFFER0 |
				 AGS_AUDIO_UNIT_DEVIN_BUFFER1 |
				 AGS_AUDIO_UNIT_DEVIN_BUFFER2 |
				 AGS_AUDIO_UNIT_DEVIN_BUFFER3 |
				 AGS_AUDIO_UNIT_DEVIN_BUFFER4 |
				 AGS_AUDIO_UNIT_DEVIN_BUFFER5 |
				 AGS_AUDIO_UNIT_DEVIN_BUFFER6 |
				 AGS_AUDIO_UNIT_DEVIN_BUFFER7 |
				 AGS_AUDIO_UNIT_DEVIN_RECORD));

  g_atomic_int_or(&(audio_unit_devin->sync_flags),
		  AGS_AUDIO_UNIT_DEVIN_PASS_THROUGH);
  g_atomic_int_and(&(audio_unit_devin->sync_flags),
		   (~AGS_AUDIO_UNIT_DEVIN_INITIAL_CALLBACK));

  /* signal callback */
  g_mutex_lock(callback_mutex);

  g_atomic_int_or(&(audio_unit_devin->sync_flags),
		  AGS_AUDIO_UNIT_DEVIN_CALLBACK_DONE);
    
  if((AGS_AUDIO_UNIT_DEVIN_CALLBACK_WAIT & (g_atomic_int_get(&(audio_unit_devin->sync_flags)))) != 0){
    g_cond_signal(&(audio_unit_devin->callback_cond));
  }

  g_mutex_unlock(callback_mutex);

  /* signal thread */
  g_mutex_lock(callback_finish_mutex);

  g_atomic_int_or(&(audio_unit_devin->sync_flags),
		  AGS_AUDIO_UNIT_DEVIN_CALLBACK_FINISH_DONE);
    
  if((AGS_AUDIO_UNIT_DEVIN_CALLBACK_FINISH_WAIT & (g_atomic_int_get(&(audio_unit_devin->sync_flags)))) != 0){
    g_cond_signal(audio_unit_devin->callback_finish_cond);
  }

  g_mutex_unlock(callback_finish_mutex);
  
  /*  */
  audio_unit_devin->note_offset = audio_unit_devin->start_note_offset;
  audio_unit_devin->note_offset_absolute = audio_unit_devin->start_note_offset;

  switch(audio_unit_devin->format){
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
    
    g_critical("ags_audio_unit_devin_free(): unsupported word size");
  }

  g_rec_mutex_unlock(audio_unit_devin_mutex);

  if(audio_unit_devin->audio_unit_port != NULL){
    audio_unit_port = audio_unit_devin->audio_unit_port->data;

    while(!g_atomic_int_get(&(audio_unit_port->is_empty))) usleep(500000);
  }

  g_rec_mutex_lock(audio_unit_devin_mutex);
  
  memset(audio_unit_devin->buffer[0], 0, (size_t) audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);
  memset(audio_unit_devin->buffer[1], 0, (size_t) audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);
  memset(audio_unit_devin->buffer[2], 0, (size_t) audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);
  memset(audio_unit_devin->buffer[3], 0, (size_t) audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);
  memset(audio_unit_devin->buffer[4], 0, (size_t) audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);
  memset(audio_unit_devin->buffer[5], 0, (size_t) audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);
  memset(audio_unit_devin->buffer[6], 0, (size_t) audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);
  memset(audio_unit_devin->buffer[7], 0, (size_t) audio_unit_devin->pcm_channels * audio_unit_devin->buffer_size * word_size);

  g_rec_mutex_unlock(audio_unit_devin_mutex);
}

void
ags_audio_unit_devin_tic(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  gdouble delay;
  gdouble delay_counter;
  guint note_offset_absolute;
  guint note_offset;
  guint loop_left, loop_right;
  gboolean do_loop;
  
  GRecMutex *audio_unit_devin_mutex;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);
  
  /* determine if attack should be switched */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  delay = audio_unit_devin->delay[audio_unit_devin->tic_counter];
  delay_counter = audio_unit_devin->delay_counter;

  note_offset = audio_unit_devin->note_offset;
  note_offset_absolute = audio_unit_devin->note_offset_absolute;
  
  loop_left = audio_unit_devin->loop_left;
  loop_right = audio_unit_devin->loop_right;
  
  do_loop = audio_unit_devin->do_loop;

  g_rec_mutex_unlock(audio_unit_devin_mutex);

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
    g_rec_mutex_lock(audio_unit_devin_mutex);
    
    audio_unit_devin->delay_counter = delay_counter + 1.0 - delay;
    audio_unit_devin->tact_counter += 1.0;

    g_rec_mutex_unlock(audio_unit_devin_mutex);
  }else{
    g_rec_mutex_lock(audio_unit_devin_mutex);
    
    audio_unit_devin->delay_counter += 1.0;

    g_rec_mutex_unlock(audio_unit_devin_mutex);
  }
}

void
ags_audio_unit_devin_offset_changed(AgsSoundcard *soundcard,
				    guint note_offset)
{
  AgsAudioUnitDevin *audio_unit_devin;
  
  GRecMutex *audio_unit_devin_mutex;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* offset changed */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  audio_unit_devin->tic_counter += 1;

  if(audio_unit_devin->tic_counter == AGS_SOUNDCARD_DEFAULT_PERIOD){
    /* reset - tic counter i.e. modified delay index within period */
    audio_unit_devin->tic_counter = 0;
  }

  g_rec_mutex_unlock(audio_unit_devin_mutex);
}

void
ags_audio_unit_devin_set_bpm(AgsSoundcard *soundcard,
			     gdouble bpm)
{
  AgsAudioUnitDevin *audio_unit_devin;

  GRecMutex *audio_unit_devin_mutex;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* set bpm */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  audio_unit_devin->bpm = bpm;

  g_rec_mutex_unlock(audio_unit_devin_mutex);

  ags_audio_unit_devin_adjust_delay_and_attack(audio_unit_devin);
}

gdouble
ags_audio_unit_devin_get_bpm(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  gdouble bpm;
  
  GRecMutex *audio_unit_devin_mutex;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* get bpm */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  bpm = audio_unit_devin->bpm;
  
  g_rec_mutex_unlock(audio_unit_devin_mutex);

  return(bpm);
}

void
ags_audio_unit_devin_set_delay_factor(AgsSoundcard *soundcard,
				      gdouble delay_factor)
{
  AgsAudioUnitDevin *audio_unit_devin;

  GRecMutex *audio_unit_devin_mutex;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* set delay factor */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  audio_unit_devin->delay_factor = delay_factor;

  g_rec_mutex_unlock(audio_unit_devin_mutex);

  ags_audio_unit_devin_adjust_delay_and_attack(audio_unit_devin);
}

gdouble
ags_audio_unit_devin_get_delay_factor(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  gdouble delay_factor;
  
  GRecMutex *audio_unit_devin_mutex;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* get delay factor */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  delay_factor = audio_unit_devin->delay_factor;
  
  g_rec_mutex_unlock(audio_unit_devin_mutex);

  return(delay_factor);
}

gdouble
ags_audio_unit_devin_get_delay(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  guint delay_index;
  gdouble delay;
  
  GRecMutex *audio_unit_devin_mutex;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* get delay */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  delay_index = audio_unit_devin->tic_counter;

  delay = audio_unit_devin->delay[delay_index];
  
  g_rec_mutex_unlock(audio_unit_devin_mutex);
  
  return(delay);
}

gdouble
ags_audio_unit_devin_get_absolute_delay(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  gdouble absolute_delay;
  
  GRecMutex *audio_unit_devin_mutex;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);
  
  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* get absolute delay */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  absolute_delay = (60.0 * (((gdouble) audio_unit_devin->samplerate / (gdouble) audio_unit_devin->buffer_size) / (gdouble) audio_unit_devin->bpm) * ((1.0 / 16.0) * (1.0 / (gdouble) audio_unit_devin->delay_factor)));

  g_rec_mutex_unlock(audio_unit_devin_mutex);

  return(absolute_delay);
}

guint
ags_audio_unit_devin_get_attack(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  guint attack_index;
  guint attack;
  
  GRecMutex *audio_unit_devin_mutex;  

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);
  
  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* get attack */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  attack_index = audio_unit_devin->tic_counter;

  attack = audio_unit_devin->attack[attack_index];

  g_rec_mutex_unlock(audio_unit_devin_mutex);
  
  return(attack);
}

void*
ags_audio_unit_devin_get_buffer(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  void *buffer;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER0)){
    buffer = audio_unit_devin->buffer[0];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER1)){
    buffer = audio_unit_devin->buffer[1];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER2)){
    buffer = audio_unit_devin->buffer[2];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER3)){
    buffer = audio_unit_devin->buffer[3];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER4)){
    buffer = audio_unit_devin->buffer[4];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER5)){
    buffer = audio_unit_devin->buffer[5];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER6)){
    buffer = audio_unit_devin->buffer[6];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER7)){
    buffer = audio_unit_devin->buffer[7];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_audio_unit_devin_get_next_buffer(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  void *buffer;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  //  g_message("next - 0x%0x", ((AGS_AUDIO_UNIT_DEVIN_BUFFER0 |
  //				AGS_AUDIO_UNIT_DEVIN_BUFFER1 |
  //				AGS_AUDIO_UNIT_DEVIN_BUFFER2 |
  //				AGS_AUDIO_UNIT_DEVIN_BUFFER3) & (audio_unit_devin->flags)));

  if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER0)){
    buffer = audio_unit_devin->buffer[1];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER1)){
    buffer = audio_unit_devin->buffer[2];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER2)){
    buffer = audio_unit_devin->buffer[3];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER3)){
    buffer = audio_unit_devin->buffer[4];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER4)){
    buffer = audio_unit_devin->buffer[5];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER5)){
    buffer = audio_unit_devin->buffer[6];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER6)){
    buffer = audio_unit_devin->buffer[7];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER7)){
    buffer = audio_unit_devin->buffer[0];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void*
ags_audio_unit_devin_get_prev_buffer(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  void *buffer;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER0)){
    buffer = audio_unit_devin->buffer[7];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER1)){
    buffer = audio_unit_devin->buffer[0];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER2)){
    buffer = audio_unit_devin->buffer[1];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER3)){
    buffer = audio_unit_devin->buffer[2];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER4)){
    buffer = audio_unit_devin->buffer[3];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER5)){
    buffer = audio_unit_devin->buffer[4];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER6)){
    buffer = audio_unit_devin->buffer[5];
  }else if(ags_audio_unit_devin_test_flags(audio_unit_devin, AGS_AUDIO_UNIT_DEVIN_BUFFER7)){
    buffer = audio_unit_devin->buffer[6];
  }else{
    buffer = NULL;
  }

  return(buffer);
}

void
ags_audio_unit_devin_lock_buffer(AgsSoundcard *soundcard,
				  void *buffer)
{
  AgsAudioUnitDevin *audio_unit_devin;

  GRecMutex *buffer_mutex;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(audio_unit_devin->buffer != NULL){
    if(buffer == audio_unit_devin->buffer[0]){
      buffer_mutex = audio_unit_devin->buffer_mutex[0];
    }else if(buffer == audio_unit_devin->buffer[1]){
      buffer_mutex = audio_unit_devin->buffer_mutex[1];
    }else if(buffer == audio_unit_devin->buffer[2]){
      buffer_mutex = audio_unit_devin->buffer_mutex[2];
    }else if(buffer == audio_unit_devin->buffer[3]){
      buffer_mutex = audio_unit_devin->buffer_mutex[3];
    }else if(buffer == audio_unit_devin->buffer[4]){
      buffer_mutex = audio_unit_devin->buffer_mutex[4];
    }else if(buffer == audio_unit_devin->buffer[5]){
      buffer_mutex = audio_unit_devin->buffer_mutex[5];
    }else if(buffer == audio_unit_devin->buffer[6]){
      buffer_mutex = audio_unit_devin->buffer_mutex[6];
    }else if(buffer == audio_unit_devin->buffer[7]){
      buffer_mutex = audio_unit_devin->buffer_mutex[7];
    }
  }
  
  if(buffer_mutex != NULL){
    g_rec_mutex_lock(buffer_mutex);
  }
}

void
ags_audio_unit_devin_unlock_buffer(AgsSoundcard *soundcard,
				    void *buffer)
{
  AgsAudioUnitDevin *audio_unit_devin;

  GRecMutex *buffer_mutex;
  
  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  buffer_mutex = NULL;

  if(audio_unit_devin->buffer != NULL){
    if(buffer == audio_unit_devin->buffer[0]){
      buffer_mutex = audio_unit_devin->buffer_mutex[0];
    }else if(buffer == audio_unit_devin->buffer[1]){
      buffer_mutex = audio_unit_devin->buffer_mutex[1];
    }else if(buffer == audio_unit_devin->buffer[2]){
      buffer_mutex = audio_unit_devin->buffer_mutex[2];
    }else if(buffer == audio_unit_devin->buffer[3]){
      buffer_mutex = audio_unit_devin->buffer_mutex[3];
    }else if(buffer == audio_unit_devin->buffer[4]){
      buffer_mutex = audio_unit_devin->buffer_mutex[4];
    }else if(buffer == audio_unit_devin->buffer[5]){
      buffer_mutex = audio_unit_devin->buffer_mutex[5];
    }else if(buffer == audio_unit_devin->buffer[6]){
      buffer_mutex = audio_unit_devin->buffer_mutex[6];
    }else if(buffer == audio_unit_devin->buffer[7]){
      buffer_mutex = audio_unit_devin->buffer_mutex[7];
    }
  }

  if(buffer_mutex != NULL){
    g_rec_mutex_unlock(buffer_mutex);
  }
}

guint
ags_audio_unit_devin_get_delay_counter(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  guint delay_counter;
  
  GRecMutex *audio_unit_devin_mutex;  

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);
  
  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* delay counter */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  delay_counter = audio_unit_devin->delay_counter;
  
  g_rec_mutex_unlock(audio_unit_devin_mutex);

  return(delay_counter);
}

void
ags_audio_unit_devin_set_note_offset(AgsSoundcard *soundcard,
				     guint note_offset)
{
  AgsAudioUnitDevin *audio_unit_devin;

  GRecMutex *audio_unit_devin_mutex;  

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* set note offset */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  audio_unit_devin->note_offset = note_offset;

  g_rec_mutex_unlock(audio_unit_devin_mutex);
}

guint
ags_audio_unit_devin_get_start_note_offset(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  guint start_note_offset;
  
  GRecMutex *audio_unit_devin_mutex;  

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* set note offset */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  start_note_offset = audio_unit_devin->start_note_offset;

  g_rec_mutex_unlock(audio_unit_devin_mutex);

  return(start_note_offset);
}

void
ags_audio_unit_devin_set_start_note_offset(AgsSoundcard *soundcard,
					   guint start_note_offset)
{
  AgsAudioUnitDevin *audio_unit_devin;

  GRecMutex *audio_unit_devin_mutex;  

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* set note offset */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  audio_unit_devin->start_note_offset = start_note_offset;

  g_rec_mutex_unlock(audio_unit_devin_mutex);
}

guint
ags_audio_unit_devin_get_note_offset(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  guint note_offset;
  
  GRecMutex *audio_unit_devin_mutex;  

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* set note offset */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  note_offset = audio_unit_devin->note_offset;

  g_rec_mutex_unlock(audio_unit_devin_mutex);

  return(note_offset);
}

void
ags_audio_unit_devin_set_note_offset_absolute(AgsSoundcard *soundcard,
					      guint note_offset_absolute)
{
  AgsAudioUnitDevin *audio_unit_devin;
  
  GRecMutex *audio_unit_devin_mutex;  

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* set note offset */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  audio_unit_devin->note_offset_absolute = note_offset_absolute;

  g_rec_mutex_unlock(audio_unit_devin_mutex);
}

guint
ags_audio_unit_devin_get_note_offset_absolute(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  guint note_offset_absolute;
  
  GRecMutex *audio_unit_devin_mutex;  

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* set note offset */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  note_offset_absolute = audio_unit_devin->note_offset_absolute;

  g_rec_mutex_unlock(audio_unit_devin_mutex);

  return(note_offset_absolute);
}

void
ags_audio_unit_devin_set_loop(AgsSoundcard *soundcard,
			      guint loop_left, guint loop_right,
			      gboolean do_loop)
{
  AgsAudioUnitDevin *audio_unit_devin;

  GRecMutex *audio_unit_devin_mutex;  

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* set loop */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  audio_unit_devin->loop_left = loop_left;
  audio_unit_devin->loop_right = loop_right;
  audio_unit_devin->do_loop = do_loop;

  if(do_loop){
    audio_unit_devin->loop_offset = audio_unit_devin->note_offset;
  }

  g_rec_mutex_unlock(audio_unit_devin_mutex);
}

void
ags_audio_unit_devin_get_loop(AgsSoundcard *soundcard,
			      guint *loop_left, guint *loop_right,
			      gboolean *do_loop)
{
  AgsAudioUnitDevin *audio_unit_devin;

  GRecMutex *audio_unit_devin_mutex;  

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* get loop */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  if(loop_left != NULL){
    *loop_left = audio_unit_devin->loop_left;
  }

  if(loop_right != NULL){
    *loop_right = audio_unit_devin->loop_right;
  }

  if(do_loop != NULL){
    *do_loop = audio_unit_devin->do_loop;
  }

  g_rec_mutex_unlock(audio_unit_devin_mutex);
}

guint
ags_audio_unit_devin_get_loop_offset(AgsSoundcard *soundcard)
{
  AgsAudioUnitDevin *audio_unit_devin;

  guint loop_offset;
  
  GRecMutex *audio_unit_devin_mutex;  

  audio_unit_devin = AGS_AUDIO_UNIT_DEVIN(soundcard);

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* get loop offset */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  loop_offset = audio_unit_devin->loop_offset;
  
  g_rec_mutex_unlock(audio_unit_devin_mutex);

  return(loop_offset);
}


/**
 * ags_audio_unit_devin_switch_buffer_flag:
 * @audio_unit_devin: an #AgsAudioUnitDevin
 *
 * The buffer flag indicates the currently played buffer.
 *
 * Since: 2.3.14
 */
void
ags_audio_unit_devin_switch_buffer_flag(AgsAudioUnitDevin *audio_unit_devin)
{
  GRecMutex *audio_unit_devin_mutex;
  
  if(!AGS_IS_AUDIO_UNIT_DEVIN(audio_unit_devin)){
    return;
  }

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* switch buffer flag */
  g_rec_mutex_lock(audio_unit_devin_mutex);

  if((AGS_AUDIO_UNIT_DEVIN_BUFFER0 & (audio_unit_devin->flags)) != 0){
    audio_unit_devin->flags &= (~AGS_AUDIO_UNIT_DEVIN_BUFFER0);
    audio_unit_devin->flags |= AGS_AUDIO_UNIT_DEVIN_BUFFER1;
  }else if((AGS_AUDIO_UNIT_DEVIN_BUFFER1 & (audio_unit_devin->flags)) != 0){
    audio_unit_devin->flags &= (~AGS_AUDIO_UNIT_DEVIN_BUFFER1);
    audio_unit_devin->flags |= AGS_AUDIO_UNIT_DEVIN_BUFFER2;
  }else if((AGS_AUDIO_UNIT_DEVIN_BUFFER2 & (audio_unit_devin->flags)) != 0){
    audio_unit_devin->flags &= (~AGS_AUDIO_UNIT_DEVIN_BUFFER2);
    audio_unit_devin->flags |= AGS_AUDIO_UNIT_DEVIN_BUFFER3;
  }else if((AGS_AUDIO_UNIT_DEVIN_BUFFER3 & (audio_unit_devin->flags)) != 0){
    audio_unit_devin->flags &= (~AGS_AUDIO_UNIT_DEVIN_BUFFER3);
    audio_unit_devin->flags |= AGS_AUDIO_UNIT_DEVIN_BUFFER4;
  }else if((AGS_AUDIO_UNIT_DEVIN_BUFFER4 & (audio_unit_devin->flags)) != 0){
    audio_unit_devin->flags &= (~AGS_AUDIO_UNIT_DEVIN_BUFFER4);
    audio_unit_devin->flags |= AGS_AUDIO_UNIT_DEVIN_BUFFER5;
  }else if((AGS_AUDIO_UNIT_DEVIN_BUFFER5 & (audio_unit_devin->flags)) != 0){
    audio_unit_devin->flags &= (~AGS_AUDIO_UNIT_DEVIN_BUFFER5);
    audio_unit_devin->flags |= AGS_AUDIO_UNIT_DEVIN_BUFFER6;
  }else if((AGS_AUDIO_UNIT_DEVIN_BUFFER6 & (audio_unit_devin->flags)) != 0){
    audio_unit_devin->flags &= (~AGS_AUDIO_UNIT_DEVIN_BUFFER6);
    audio_unit_devin->flags |= AGS_AUDIO_UNIT_DEVIN_BUFFER7;
  }else if((AGS_AUDIO_UNIT_DEVIN_BUFFER7 & (audio_unit_devin->flags)) != 0){
    audio_unit_devin->flags &= (~AGS_AUDIO_UNIT_DEVIN_BUFFER7);
    audio_unit_devin->flags |= AGS_AUDIO_UNIT_DEVIN_BUFFER0;
  }

  g_rec_mutex_unlock(audio_unit_devin_mutex);
}

/**
 * ags_audio_unit_devin_adjust_delay_and_attack:
 * @audio_unit_devin: the #AgsAudioUnitDevin
 *
 * Calculate delay and attack and reset it.
 *
 * Since: 2.3.14
 */
void
ags_audio_unit_devin_adjust_delay_and_attack(AgsAudioUnitDevin *audio_unit_devin)
{
  gdouble delay;
  guint default_tact_frames;
  guint delay_tact_frames;
  guint default_period;
  gint next_attack;
  guint i;

  GRecMutex *audio_unit_devin_mutex;

  if(!AGS_IS_AUDIO_UNIT_DEVIN(audio_unit_devin)){
    return;
  }

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);
  
  /* get some initial values */
  delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(audio_unit_devin));

#ifdef AGS_DEBUG
  g_message("delay : %f", delay);
#endif
  
  g_rec_mutex_lock(audio_unit_devin_mutex);

  default_tact_frames = (guint) (delay * audio_unit_devin->buffer_size);
  delay_tact_frames = (guint) (floor(delay) * audio_unit_devin->buffer_size);
  default_period = (1.0 / AGS_SOUNDCARD_DEFAULT_PERIOD) * (default_tact_frames);

  i = 0;
  
  audio_unit_devin->attack[0] = (guint) floor(0.25 * audio_unit_devin->buffer_size);
  next_attack = (((audio_unit_devin->attack[i] + default_tact_frames) / audio_unit_devin->buffer_size) - delay) * audio_unit_devin->buffer_size;

  if(next_attack < 0){
    next_attack = 0;
  }

  if(next_attack >= audio_unit_devin->buffer_size){
    next_attack = audio_unit_devin->buffer_size - 1;
  }
  
  /* check if delay drops for next attack */
  if(next_attack < 0){
    audio_unit_devin->attack[i] = audio_unit_devin->attack[i] - ((gdouble) next_attack / 2.0);

    if(audio_unit_devin->attack[i] < 0){
      audio_unit_devin->attack[i] = 0;
    }
    
    if(audio_unit_devin->attack[i] >= audio_unit_devin->buffer_size){
      audio_unit_devin->attack[i] = audio_unit_devin->buffer_size - 1;
    }
    
    next_attack = next_attack + (next_attack / 2.0);

    if(next_attack < 0){
      next_attack = 0;
    }

    if(next_attack >= audio_unit_devin->buffer_size){
      next_attack = audio_unit_devin->buffer_size - 1;
    }
  }
  
  for(i = 1; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD; i++){
    audio_unit_devin->attack[i] = next_attack;
    next_attack = (((audio_unit_devin->attack[i] + default_tact_frames) / audio_unit_devin->buffer_size) - delay) * audio_unit_devin->buffer_size;

    if(next_attack >= audio_unit_devin->buffer_size){
      next_attack = audio_unit_devin->buffer_size - 1;
    }
    
    /* check if delay drops for next attack */
    if(next_attack < 0){
      audio_unit_devin->attack[i] = audio_unit_devin->attack[i] - ((gdouble) next_attack / 2.0);

      if(audio_unit_devin->attack[i] < 0){
	audio_unit_devin->attack[i] = 0;
      }

      if(audio_unit_devin->attack[i] >= audio_unit_devin->buffer_size){
	audio_unit_devin->attack[i] = audio_unit_devin->buffer_size - 1;
      }
    
      next_attack = next_attack + (next_attack / 2.0);

      if(next_attack < 0){
	next_attack = 0;
      }

      if(next_attack >= audio_unit_devin->buffer_size){
	next_attack = audio_unit_devin->buffer_size - 1;
      }
    }
    
#ifdef AGS_DEBUG
    g_message("%d", audio_unit_devin->attack[i]);
#endif
  }

  audio_unit_devin->attack[0] = audio_unit_devin->attack[i - 2];
  
  for(i = 0; i < (int) 2.0 * AGS_SOUNDCARD_DEFAULT_PERIOD - 1; i++){
    audio_unit_devin->delay[i] = ((gdouble) (default_tact_frames + audio_unit_devin->attack[i] - audio_unit_devin->attack[i + 1])) / (gdouble) audio_unit_devin->buffer_size;
    
#ifdef AGS_DEBUG
    g_message("%f", audio_unit_devin->delay[i]);
#endif
  }

  audio_unit_devin->delay[i] = ((gdouble) (default_tact_frames + audio_unit_devin->attack[i] - audio_unit_devin->attack[0])) / (gdouble) audio_unit_devin->buffer_size;

  g_rec_mutex_unlock(audio_unit_devin_mutex);
}

/**
 * ags_audio_unit_devin_realloc_buffer:
 * @audio_unit_devin: the #AgsAudioUnitDevin
 *
 * Reallocate the internal audio buffer.
 *
 * Since: 2.3.14
 */
void
ags_audio_unit_devin_realloc_buffer(AgsAudioUnitDevin *audio_unit_devin)
{
  guint pcm_channels;
  guint buffer_size;
  guint format;
  guint word_size;

  GRecMutex *audio_unit_devin_mutex;  

  if(!AGS_IS_AUDIO_UNIT_DEVIN(audio_unit_devin)){
    return;
  }

  /* get audio_unit devin mutex */
  audio_unit_devin_mutex = AGS_AUDIO_UNIT_DEVIN_GET_OBJ_MUTEX(audio_unit_devin);

  /* get word size */  
  g_rec_mutex_lock(audio_unit_devin_mutex);

  pcm_channels = audio_unit_devin->pcm_channels;
  buffer_size = audio_unit_devin->buffer_size;

  format = audio_unit_devin->format;
  
  g_rec_mutex_unlock(audio_unit_devin_mutex);

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
    g_warning("ags_audio_unit_devin_realloc_buffer(): unsupported word size");
    return;
  }
  
  /* AGS_AUDIO_UNIT_DEVIN_BUFFER_0 */
  if(audio_unit_devin->buffer[0] != NULL){
    free(audio_unit_devin->buffer[0]);
  }
  
  audio_unit_devin->buffer[0] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_AUDIO_UNIT_DEVIN_BUFFER_1 */
  if(audio_unit_devin->buffer[1] != NULL){
    free(audio_unit_devin->buffer[1]);
  }

  audio_unit_devin->buffer[1] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_AUDIO_UNIT_DEVIN_BUFFER_2 */
  if(audio_unit_devin->buffer[2] != NULL){
    free(audio_unit_devin->buffer[2]);
  }

  audio_unit_devin->buffer[2] = (void *) malloc(pcm_channels * buffer_size * word_size);
  
  /* AGS_AUDIO_UNIT_DEVIN_BUFFER_3 */
  if(audio_unit_devin->buffer[3] != NULL){
    free(audio_unit_devin->buffer[3]);
  }
  
  audio_unit_devin->buffer[3] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_AUDIO_UNIT_DEVIN_BUFFER_4 */
  if(audio_unit_devin->buffer[4] != NULL){
    free(audio_unit_devin->buffer[4]);
  }
  
  audio_unit_devin->buffer[4] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_AUDIO_UNIT_DEVIN_BUFFER_5 */
  if(audio_unit_devin->buffer[5] != NULL){
    free(audio_unit_devin->buffer[5]);
  }
  
  audio_unit_devin->buffer[5] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_AUDIO_UNIT_DEVIN_BUFFER_6 */
  if(audio_unit_devin->buffer[6] != NULL){
    free(audio_unit_devin->buffer[6]);
  }
  
  audio_unit_devin->buffer[6] = (void *) malloc(pcm_channels * buffer_size * word_size);

  /* AGS_AUDIO_UNIT_DEVIN_BUFFER_7 */
  if(audio_unit_devin->buffer[7] != NULL){
    free(audio_unit_devin->buffer[7]);
  }
  
  audio_unit_devin->buffer[7] = (void *) malloc(pcm_channels * buffer_size * word_size);
}

/**
 * ags_audio_unit_devin_new:
 *
 * Creates a new instance of #AgsAudioUnitDevin.
 *
 * Returns: a new #AgsAudioUnitDevin
 *
 * Since: 2.3.14
 */
AgsAudioUnitDevin*
ags_audio_unit_devin_new()
{
  AgsAudioUnitDevin *audio_unit_devin;

  audio_unit_devin = (AgsAudioUnitDevin *) g_object_new(AGS_TYPE_AUDIO_UNIT_DEVIN,
							NULL);
  
  return(audio_unit_devin);
}
