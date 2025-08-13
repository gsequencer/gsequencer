/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_audio_unit_audio.h>

#include <ags/plugin/ags_audio_unit_manager.h>
#include <ags/plugin/ags_audio_unit_new_queue_manager.h>
#include <ags/plugin/ags_audio_unit_plugin.h>
#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_port_util.h>

#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>

#include <ags/audio/fx/ags_fx_audio_unit_channel.h>

#import <CoreFoundation/CoreFoundation.h>
#import <AVFoundation/AVFoundation.h>

#include <ags/i18n.h>

void ags_fx_audio_unit_audio_class_init(AgsFxAudioUnitAudioClass *fx_audio_unit_audio);
void ags_fx_audio_unit_audio_init(AgsFxAudioUnitAudio *fx_audio_unit_audio);
void ags_fx_audio_unit_audio_dispose(GObject *gobject);
void ags_fx_audio_unit_audio_finalize(GObject *gobject);

void ags_fx_audio_unit_audio_notify_audio_callback(GObject *gobject,
						   GParamSpec *pspec,
						   gpointer user_data);
void ags_fx_audio_unit_audio_notify_filename_callback(GObject *gobject,
						      GParamSpec *pspec,
						      gpointer user_data);
void ags_fx_audio_unit_audio_notify_effect_callback(GObject *gobject,
						    GParamSpec *pspec,
						    gpointer user_data);
void ags_fx_audio_unit_audio_notify_buffer_size_callback(GObject *gobject,
							 GParamSpec *pspec,
							 gpointer user_data);
void ags_fx_audio_unit_audio_notify_samplerate_callback(GObject *gobject,
							GParamSpec *pspec,
							gpointer user_data);

void ags_fx_audio_unit_audio_set_audio_channels_callback(AgsAudio *audio,
							 guint audio_channels, guint audio_channels_old,
							 AgsFxAudioUnitAudio *fx_audio_unit_audio);
void ags_fx_audio_unit_audio_set_pads_callback(AgsAudio *audio,
					       GType channel_type,
					       guint pads, guint pads_old,
					       AgsFxAudioUnitAudio *fx_audio_unit_audio);

void ags_fx_audio_unit_audio_scope_data_load_plugin(AgsFxAudioUnitAudio *fx_audio_unit_audio,
						    AgsFxAudioUnitAudioScopeData *scope_data);

void ags_fx_audio_unit_audio_port_safe_write_callback(AgsPort *port,
						      GValue *value,
						      AgsFxAudioUnitAudio *fx_audio_unit_audio);

void ags_fx_audio_unit_audio_scope_data_load_port(AgsFxAudioUnitAudio *fx_audio_unit_audio,
						  AgsFxAudioUnitAudioScopeData *scope_data);

/**
 * SECTION:ags_fx_audio_unit_audio
 * @short_description: fx Audio Unit audio
 * @title: AgsFxAudioUnitAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_audio_unit_audio.h
 *
 * The #AgsFxAudioUnitAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_audio_unit_audio_parent_class = NULL;

const gchar *ags_fx_audio_unit_audio_plugin_name = "ags-fx-audio-unit";

const gchar *ags_fx_audio_unit_audio_specifier[] = {
  NULL,
};

const gchar *ags_fx_audio_unit_audio_control_port[] = {
  NULL,
};

enum{
  PROP_0,
};

GType
ags_fx_audio_unit_audio_get_type()
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_fx_audio_unit_audio = 0;

    static const GTypeInfo ags_fx_audio_unit_audio_info = {
      sizeof (AgsFxAudioUnitAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_audio_unit_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxAudioUnitAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_audio_unit_audio_init,
    };

    ags_type_fx_audio_unit_audio = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO,
							  "AgsFxAudioUnitAudio",
							  &ags_fx_audio_unit_audio_info,
							  0);

    g_once_init_leave(&g_define_type_id__static, ags_type_fx_audio_unit_audio);
  }

  return(g_define_type_id__static);
}

void
ags_fx_audio_unit_audio_class_init(AgsFxAudioUnitAudioClass *fx_audio_unit_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_audio_unit_audio_parent_class = g_type_class_peek_parent(fx_audio_unit_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_audio_unit_audio;

  gobject->dispose = ags_fx_audio_unit_audio_dispose;
  gobject->finalize = ags_fx_audio_unit_audio_finalize;

  /* properties */
}

void
ags_fx_audio_unit_audio_init(AgsFxAudioUnitAudio *fx_audio_unit_audio)
{
  AgsConfig *config;

  guint pcm_channels;
  guint samplerate;
  guint i;

  g_signal_connect(fx_audio_unit_audio, "notify::filename",
		   G_CALLBACK(ags_fx_audio_unit_audio_notify_filename_callback), NULL);

  g_signal_connect(fx_audio_unit_audio, "notify::effect",
		   G_CALLBACK(ags_fx_audio_unit_audio_notify_effect_callback), NULL);

  g_signal_connect(fx_audio_unit_audio, "notify::audio",
		   G_CALLBACK(ags_fx_audio_unit_audio_notify_audio_callback), NULL);

  g_signal_connect(fx_audio_unit_audio, "notify::buffer-size",
		   G_CALLBACK(ags_fx_audio_unit_audio_notify_buffer_size_callback), NULL);

  g_signal_connect(fx_audio_unit_audio, "notify::samplerate",
		   G_CALLBACK(ags_fx_audio_unit_audio_notify_samplerate_callback), NULL);

  AGS_RECALL(fx_audio_unit_audio)->name = "ags-fx-audio-unit";
  AGS_RECALL(fx_audio_unit_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_audio_unit_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_audio_unit_audio)->xml_type = "ags-fx-audio-unit-audio";

  fx_audio_unit_audio->flags = 0;

  /* config */
  config = ags_config_get_instance();

  pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);
  samplerate = (guint) ags_soundcard_helper_config_get_samplerate(config);
  
  /* audio unit plugin */
  fx_audio_unit_audio->audio_unit_plugin = NULL;

  /* audio unit */
  fx_audio_unit_audio->av_format = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:((double) samplerate)
				    channels:pcm_channels];

  fx_audio_unit_audio->audio_engine = NULL;

  fx_audio_unit_audio->av_audio_unit = NULL;

  fx_audio_unit_audio->av_audio_sequencer = NULL;

  /* render thread */
  fx_audio_unit_audio->render_thread_running = FALSE;
  
  fx_audio_unit_audio->render_thread = NULL;
  
  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      fx_audio_unit_audio->scope_data[i] = ags_fx_audio_unit_audio_scope_data_alloc();
      
      fx_audio_unit_audio->scope_data[i]->parent = fx_audio_unit_audio;
    }else{
      fx_audio_unit_audio->scope_data[i] = NULL;
    }
  }
}

void
ags_fx_audio_unit_audio_dispose(GObject *gobject)
{
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  
  fx_audio_unit_audio = AGS_FX_AUDIO_UNIT_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_audio_unit_audio_parent_class)->dispose(gobject);
}

void
ags_fx_audio_unit_audio_finalize(GObject *gobject)
{
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
    
  guint i;
  
  fx_audio_unit_audio = AGS_FX_AUDIO_UNIT_AUDIO(gobject);
  
  if(fx_audio_unit_audio->audio_unit_plugin != NULL){
    g_object_unref(fx_audio_unit_audio->audio_unit_plugin);
  }
  
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      ags_fx_audio_unit_audio_scope_data_free(fx_audio_unit_audio->scope_data[i]);
    }
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_audio_unit_audio_parent_class)->finalize(gobject);
}

void
ags_fx_audio_unit_audio_notify_filename_callback(GObject *gobject,
						 GParamSpec *pspec,
						 gpointer user_data)
{
  AgsFxAudioUnitAudio *fx_audio_unit_audio;

  AgsAudioUnitManager *audio_unit_manager;

  gchar *filename, *effect;
  
  GRecMutex *recall_mutex;

  fx_audio_unit_audio = AGS_FX_AUDIO_UNIT_AUDIO(gobject);

  audio_unit_manager = ags_audio_unit_manager_get_instance();

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  filename = NULL;
  effect = NULL;

  g_object_get(fx_audio_unit_audio,
	       "filename", &filename,
	       "effect", &effect,
	       NULL);

  /* get audio unit plugin */
  g_rec_mutex_lock(recall_mutex);

  if(filename != NULL &&
     effect != NULL){
    fx_audio_unit_audio->audio_unit_plugin = ags_audio_unit_manager_find_audio_unit_plugin(audio_unit_manager,
											   filename, effect);

    if(fx_audio_unit_audio->audio_unit_plugin != NULL){
      g_object_ref(fx_audio_unit_audio->audio_unit_plugin);
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);

  g_free(filename);
  g_free(effect);
}

void
ags_fx_audio_unit_audio_notify_effect_callback(GObject *gobject,
					       GParamSpec *pspec,
					       gpointer user_data)
{
  AgsFxAudioUnitAudio *fx_audio_unit_audio;

  AgsAudioUnitManager *audio_unit_manager;

  gchar *filename, *effect;
  
  GRecMutex *recall_mutex;

  fx_audio_unit_audio = AGS_FX_AUDIO_UNIT_AUDIO(gobject);

  audio_unit_manager = ags_audio_unit_manager_get_instance();

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  filename = NULL;
  effect = NULL;

  g_object_get(fx_audio_unit_audio,
	       "filename", &filename,
	       "effect", &effect,
	       NULL);

  /* get audio_unit plugin */
  g_rec_mutex_lock(recall_mutex);

  if(filename != NULL &&
     effect != NULL){
    fx_audio_unit_audio->audio_unit_plugin = ags_audio_unit_manager_find_audio_unit_plugin(audio_unit_manager,
											   filename, effect);

    if(fx_audio_unit_audio->audio_unit_plugin != NULL){
      g_object_ref(fx_audio_unit_audio->audio_unit_plugin);
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);

  g_free(filename);
  g_free(effect);
}

void
ags_fx_audio_unit_audio_notify_audio_callback(GObject *gobject,
					      GParamSpec *pspec,
					      gpointer user_data)
{
  AgsAudio *audio;
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  
  guint audio_channels;

  fx_audio_unit_audio = AGS_FX_AUDIO_UNIT_AUDIO(gobject);
  
  /* get audio */
  audio = NULL;

  g_object_get(fx_audio_unit_audio,
	       "audio", &audio,
	       NULL);

  g_signal_connect_after((GObject *) audio, "set-audio-channels",
			 G_CALLBACK(ags_fx_audio_unit_audio_set_audio_channels_callback), fx_audio_unit_audio);

  g_signal_connect_after((GObject *) audio, "set-pads",
			 G_CALLBACK(ags_fx_audio_unit_audio_set_pads_callback), fx_audio_unit_audio);

  if(audio != NULL){
    audio_channels = ags_audio_get_audio_channels(audio);

    ags_fx_audio_unit_audio_set_audio_channels_callback(audio,
							audio_channels, 0,
							fx_audio_unit_audio);
    
    g_object_unref(audio);
  }
}

void
ags_fx_audio_unit_audio_notify_buffer_size_callback(GObject *gobject,
						    GParamSpec *pspec,
						    gpointer user_data)
{
  AgsAudio *audio;

  AgsFxAudioUnitAudio *fx_audio_unit_audio;

  AgsAudioUnitPlugin *audio_unit_plugin;
  
  AVAudioFormat *av_format;
  
  guint audio_channels;
  guint buffer_size;
  guint samplerate;
  guint i;
  
  GRecMutex *recall_mutex;
  
  fx_audio_unit_audio = AGS_FX_AUDIO_UNIT_AUDIO(gobject);

  if(!ags_recall_test_state_flags(AGS_RECALL(fx_audio_unit_audio),
				  AGS_SOUND_STATE_PORT_LOADED)){
    return;
  }
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  /* get audio unit plugin */
  g_rec_mutex_lock(recall_mutex);

  audio_unit_plugin = fx_audio_unit_audio->audio_unit_plugin;
  
  g_rec_mutex_unlock(recall_mutex);

  if(audio_unit_plugin == NULL){
    return;
  }
  
  /* get properties */
  audio = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;  
  
  g_object_get(fx_audio_unit_audio,
	       "audio", &audio,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  audio_channels = 0;
  
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  av_format = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:((double) samplerate)
	       channels:audio_channels];

  fx_audio_unit_audio->av_format = (gpointer) av_format;
  
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxAudioUnitAudioScopeData *scope_data;

    AVAudioPCMBuffer *av_output_buffer, *av_input_buffer;
    
    scope_data = fx_audio_unit_audio->scope_data[i];
    	
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      av_output_buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
			  frameCapacity:buffer_size];
    
      scope_data->av_output_buffer = (gpointer) av_output_buffer;    

      av_input_buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
			 frameCapacity:buffer_size];
    
      scope_data->av_input_buffer = (gpointer) av_input_buffer;
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);

  /* unref */
  if(audio != NULL){
    g_object_unref(audio);
  }
}

void
ags_fx_audio_unit_audio_notify_samplerate_callback(GObject *gobject,
						   GParamSpec *pspec,
						   gpointer user_data)
{
  AgsAudio *audio;

  AgsFxAudioUnitAudio *fx_audio_unit_audio;

  AgsAudioUnitPlugin *audio_unit_plugin;

  AVAudioFormat *av_format;

  guint audio_channels;
  guint buffer_size;
  guint samplerate;
  guint i;
  
  GRecMutex *recall_mutex;
  
  fx_audio_unit_audio = AGS_FX_AUDIO_UNIT_AUDIO(gobject);

  if(!ags_recall_test_state_flags(AGS_RECALL(fx_audio_unit_audio),
				  AGS_SOUND_STATE_PORT_LOADED)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  /* get properties */
  audio = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  g_object_get(fx_audio_unit_audio,
	       "audio", &audio,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);
  
  audio_channels = 0;
  
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  av_format = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:((double) samplerate)
	       channels:audio_channels];

  fx_audio_unit_audio->av_format = (gpointer) av_format;

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxAudioUnitAudioScopeData *scope_data;

    AVAudioPCMBuffer *av_output_buffer, *av_input_buffer;
 
    scope_data = fx_audio_unit_audio->scope_data[i];
	
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      av_output_buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
			  frameCapacity:buffer_size];
    
      scope_data->av_output_buffer = (gpointer) av_output_buffer;    

      av_input_buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
			 frameCapacity:buffer_size];
    
      scope_data->av_input_buffer = (gpointer) av_input_buffer;
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);

  /* unref */
  if(audio != NULL){
    g_object_unref(audio);
  }
}

void
ags_fx_audio_unit_audio_set_audio_channels_callback(AgsAudio *audio,
						    guint audio_channels, guint audio_channels_old,
						    AgsFxAudioUnitAudio *fx_audio_unit_audio)
{
  AVAudioFormat *av_format;
  
  guint buffer_size;
  guint samplerate;
  guint i;

  GRecMutex *recall_mutex;

  if(audio_channels == audio_channels_old){
    return;
  }
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);
  
  /* get properties */
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  g_object_get(fx_audio_unit_audio,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  av_format = [[AVAudioFormat alloc] initStandardFormatWithSampleRate:((double) samplerate)
	       channels:audio_channels];

  fx_audio_unit_audio->av_format = (gpointer) av_format;

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxAudioUnitAudioScopeData *scope_data;

    AVAudioPCMBuffer *av_output_buffer, *av_input_buffer;
 
    scope_data = fx_audio_unit_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      av_output_buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
			  frameCapacity:buffer_size];
    
      scope_data->av_output_buffer = (gpointer) av_output_buffer;

      av_input_buffer = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
			 frameCapacity:buffer_size];
    
      scope_data->av_input_buffer = (gpointer) av_input_buffer;    
    
      scope_data->channel_data[0] = ags_fx_audio_unit_audio_channel_data_alloc();
    }    
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_audio_unit_audio_set_pads_callback(AgsAudio *audio,
					  GType channel_type,
					  guint pads, guint pads_old,
					  AgsFxAudioUnitAudio *fx_audio_unit_audio)
{
  guint audio_channels;
  
  GRecMutex *recall_mutex;

  if(!g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  /* get AUDIO UNIT plugin and port */
  audio_channels = 0;
  
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       NULL);

  //TODO:JK: implement me
}

/**
 * ags_fx_audio_unit_audio_scope_data_alloc:
 * 
 * Allocate #AgsFxAudioUnitAudioScopeData-struct
 * 
 * Returns: the new #AgsFxAudioUnitAudioScopeData-struct
 * 
 * Since: 8.1.2
 */
AgsFxAudioUnitAudioScopeData*
ags_fx_audio_unit_audio_scope_data_alloc()
{
  AgsFxAudioUnitAudioScopeData *scope_data;

  scope_data = (AgsFxAudioUnitAudioScopeData *) g_malloc(sizeof(AgsFxAudioUnitAudioScopeData));

  g_rec_mutex_init(&(scope_data->strct_mutex));

  scope_data->parent = NULL;
  
  scope_data->audio_channels = 0;

  scope_data->output = NULL;
  scope_data->output_buffer_size = 0;

  scope_data->input = NULL;
  scope_data->input_buffer_size = 0;
  
  scope_data->av_output_buffer = NULL;
  scope_data->av_input_buffer = NULL;

  ags_atomic_int_set(&(scope_data->active_audio_signal_count),
		     0);

  ags_atomic_int_set(&(scope_data->completed_audio_signal_count),
		     0);

  g_mutex_init(&(scope_data->completed_mutex));

  g_cond_init(&(scope_data->completed_cond));

  /*
   * active audio signal pad index as key and GList of AgsAudioSignal as value
   */
  scope_data->active_audio_signal = g_hash_table_new_full(g_direct_hash,
							  g_direct_equal,
							  NULL,
							  NULL);

  /*
   * written audio signal pad index as key and AgsAudioSignal as value
   */
  scope_data->written_audio_signal = g_hash_table_new_full(g_direct_hash,
							   g_direct_equal,
							   NULL,
							   NULL);
  
  return(scope_data);
}

/**
 * ags_fx_audio_unit_audio_scope_data_free:
 * @scope_data: the #AgsFxAudioUnitAudioScopeData-struct
 * 
 * Free @scope_data.
 * 
 * Since: 8.1.2
 */
void
ags_fx_audio_unit_audio_scope_data_free(AgsFxAudioUnitAudioScopeData *scope_data)
{
  if(scope_data == NULL){
    return;
  }

  ags_stream_free(scope_data->output);

  ags_stream_free(scope_data->input);

  g_hash_table_unref(scope_data->active_audio_signal);
  g_hash_table_unref(scope_data->written_audio_signal);
  
  g_free(scope_data);
}

/**
 * ags_fx_audio_unit_audio_channel_data_alloc:
 * 
 * Allocate #AgsFxAudioUnitAudioChannelData-struct
 * 
 * Returns: the new #AgsFxAudioUnitAudioChannelData-struct
 * 
 * Since: 8.1.2
 */
AgsFxAudioUnitAudioChannelData*
ags_fx_audio_unit_audio_channel_data_alloc()
{
  AgsFxAudioUnitAudioChannelData *channel_data;

  guint i;
  
  channel_data = (AgsFxAudioUnitAudioChannelData *) g_malloc(sizeof(AgsFxAudioUnitAudioChannelData));

  g_rec_mutex_init(&(channel_data->strct_mutex));

  channel_data->parent = NULL;
  
  channel_data->event_count = 0;
  
  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    channel_data->input_data[i] = ags_fx_audio_unit_audio_input_data_alloc();

    channel_data->input_data[i]->parent = channel_data;    
  }

  return(channel_data);
}

/**
 * ags_fx_audio_unit_audio_channel_data_free:
 * @channel_data: the #AgsFxAudioUnitAudioChannelData-struct
 * 
 * Free @channel_data.
 * 
 * Since: 8.1.2
 */
void
ags_fx_audio_unit_audio_channel_data_free(AgsFxAudioUnitAudioChannelData *channel_data)
{
  guint i;

  if(channel_data == NULL){
    return;
  }
  
  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    ags_fx_audio_unit_audio_input_data_free(channel_data->input_data[i]);
  }

  g_free(channel_data);
}

/**
 * ags_fx_audio_unit_audio_input_data_alloc:
 * 
 * Allocate #AgsFxAudioUnitAudioInputData-struct
 * 
 * Returns: the new #AgsFxAudioUnitAudioInputData-struct
 * 
 * Since: 8.1.2
 */
AgsFxAudioUnitAudioInputData*
ags_fx_audio_unit_audio_input_data_alloc()
{
  AgsFxAudioUnitAudioInputData *input_data;

  input_data = (AgsFxAudioUnitAudioInputData *) g_malloc(sizeof(AgsFxAudioUnitAudioInputData));

  g_rec_mutex_init(&(input_data->strct_mutex));

  input_data->parent = NULL;

  input_data->av_music_track = NULL;

  input_data->note = NULL;
  
  input_data->key_on = 0;
  
  return(input_data);
}

/**
 * ags_fx_audio_unit_audio_input_data_free:
 * @input_data: the #AgsFxAudioUnitAudioInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 8.1.2
 */
void
ags_fx_audio_unit_audio_input_data_free(AgsFxAudioUnitAudioInputData *input_data)
{
  if(input_data == NULL){
    return;
  }
  
  g_list_free_full(input_data->note,
		   (GDestroyNotify) g_object_unref);

  g_free(input_data);
}

/**
 * ags_fx_audio_unit_audio_test_flags:
 * @fx_audio_unit_audio: the #AgsFxAudioUnitAudio
 * @flags: the flags
 * 
 * Test @flags of @fx_audio_unit_audio.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 8.1.2
 */
gboolean
ags_fx_audio_unit_audio_test_flags(AgsFxAudioUnitAudio *fx_audio_unit_audio, guint flags)
{
  gboolean success;
  
  GRecMutex *recall_mutex;
  
  if(!AGS_IS_FX_AUDIO_UNIT_AUDIO(fx_audio_unit_audio)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  /* test flags */
  g_rec_mutex_lock(recall_mutex);

  success = ((flags & (fx_audio_unit_audio->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(recall_mutex);

  return(success);
}

/**
 * ags_fx_audio_unit_audio_set_flags:
 * @fx_audio_unit_audio: the #AgsFxAudioUnitAudio
 * @flags: the flags
 * 
 * Set @flags of @fx_audio_unit_audio.
 * 
 * Since: 8.1.2
 */
void
ags_fx_audio_unit_audio_set_flags(AgsFxAudioUnitAudio *fx_audio_unit_audio, guint flags)
{  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_AUDIO_UNIT_AUDIO(fx_audio_unit_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);
  
  /* set flags */
  g_rec_mutex_lock(recall_mutex);

  fx_audio_unit_audio->flags |= flags;

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_audio_unit_audio_unset_flags:
 * @fx_audio_unit_audio: the #AgsFxAudioUnitAudio
 * @flags: the flags
 * 
 * Unset @flags of @fx_audio_unit_audio.
 * 
 * Since: 8.1.2
 */
void
ags_fx_audio_unit_audio_unset_flags(AgsFxAudioUnitAudio *fx_audio_unit_audio, guint flags)
{  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_AUDIO_UNIT_AUDIO(fx_audio_unit_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  /* set flags */
  g_rec_mutex_lock(recall_mutex);

  fx_audio_unit_audio->flags &= (~flags);

  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_audio_unit_audio_scope_data_load_plugin(AgsFxAudioUnitAudio *fx_audio_unit_audio,
					       AgsFxAudioUnitAudioScopeData *scope_data)
{
  //TODO:JK: implement me
}

/**
 * ags_fx_audio_unit_audio_load_plugin:
 * @fx_audio_unit_audio: the #AgsFxAudioUnitAudio
 * 
 * Load plugin of @fx_audio_unit_audio.
 * 
 * Since: 8.1.2
 */
void
ags_fx_audio_unit_audio_load_plugin(AgsFxAudioUnitAudio *fx_audio_unit_audio)
{
  AgsAudio *audio;
  
  AgsAudioUnitManager *audio_unit_manager;
  AgsAudioUnitPlugin *audio_unit_plugin;

  AVAudioEngine *audio_engine;

  AVAudioPlayerNode *av_audio_player_node;
  AVAudioUnit *av_audio_unit;
  AVAudioSequencer *av_audio_sequencer;

  AVAudioFormat *av_format;
  
  gchar *filename, *effect;

  guint audio_channels;
  guint buffer_size;
  guint samplerate;
  guint i;

  NSError *ns_error;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_AUDIO_UNIT_AUDIO(fx_audio_unit_audio) ||
     ags_recall_test_state_flags(AGS_RECALL(fx_audio_unit_audio),
				 AGS_SOUND_STATE_PLUGIN_LOADED)){
    return;
  }
  
  audio_unit_manager = ags_audio_unit_manager_get_instance();

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  /* get filename and effect */
  audio = NULL;
  
  filename = NULL;
  effect = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  g_object_get(fx_audio_unit_audio,
	       "audio", &audio,
	       "filename", &filename,
	       "effect", &effect,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  audio_channels = 0;
  
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       NULL);
  
  audio_unit_plugin = ags_audio_unit_manager_find_audio_unit_plugin(audio_unit_manager,
								    filename, effect);
  
  if(audio_unit_plugin == NULL){
    g_free(filename);
    g_free(effect);
    
    return;
  }

  fx_audio_unit_audio->audio_unit_plugin = audio_unit_plugin;

  /* audio engine */
  audio_engine = [[AVAudioEngine alloc] init];
  
  fx_audio_unit_audio->audio_engine = (gpointer) audio_engine;

  av_format = (AVAudioFormat *) fx_audio_unit_audio->av_format;

  /* av audio player node */
  av_audio_player_node = [[AVAudioPlayerNode alloc] init];

  /* av audio unit */
  av_audio_unit = (AVAudioUnit *) ags_base_plugin_instantiate((AgsBasePlugin *) audio_unit_plugin,
							      samplerate,
							      buffer_size);
  
  fx_audio_unit_audio->av_audio_unit = av_audio_unit;
  
  /* audio sequencer */
  av_audio_sequencer = [[AVAudioSequencer alloc] initWithAudioEngine:audio_engine];
  
  fx_audio_unit_audio->av_audio_sequencer = (gpointer) av_audio_sequencer;

  /* audio player and audio unit */
  [audio_engine attachNode:av_audio_player_node];
  [audio_engine attachNode:av_audio_unit];

  [audio_engine connect:av_audio_player_node to:av_audio_unit format:av_format];
  [audio_engine connect:av_audio_unit to:[audio_engine outputNode] format:av_format];

  ns_error = NULL;
  
  [audio_engine enableManualRenderingMode:AVAudioEngineManualRenderingModeOffline
   format:av_format
   maximumFrameCount:buffer_size error:&ns_error];

  if(ns_error != NULL &&
     [ns_error code] != noErr){
    g_warning("enable manual rendering mode error - %d", [ns_error code]);
  }

  /* set audio_unit plugin */    
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxAudioUnitAudioScopeData *scope_data;

    scope_data = fx_audio_unit_audio->scope_data[i];
      
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      ags_fx_audio_unit_audio_scope_data_load_plugin(fx_audio_unit_audio,
						     scope_data);
    }
  }
    
  g_rec_mutex_unlock(recall_mutex);

  ags_recall_set_state_flags(AGS_RECALL(fx_audio_unit_audio),
			     AGS_SOUND_STATE_PLUGIN_LOADED);
    
  g_free(filename);
  g_free(effect);
}

void
ags_fx_audio_unit_audio_scope_data_load_port(AgsFxAudioUnitAudio *fx_audio_unit_audio,
					     AgsFxAudioUnitAudioScopeData *scope_data)
{
  //TODO:JK: implement me
}

void
ags_fx_audio_unit_audio_port_safe_write_callback(AgsPort *port,
						 GValue *value,
						 AgsFxAudioUnitAudio *fx_audio_unit_audio)
{
  AgsAudioUnitPlugin *audio_unit_plugin;
  AgsPluginPort *plugin_port;

  AVAudioUnit *av_audio_unit;
  AUAudioUnit *au_audio_unit;
  AUParameterTree *parameter_tree;
  NSArray<AUParameter *> *parameter_arr;

  gchar *str;
  
  guint i, i_stop;
  
  GRecMutex *base_plugin_mutex;

  audio_unit_plugin = fx_audio_unit_audio->audio_unit_plugin;

  if(audio_unit_plugin == NULL){
    return;
  }
  
  av_audio_unit = (AVAudioUnit *) fx_audio_unit_audio->av_audio_unit;

  if(av_audio_unit == NULL){
    return;
  }

  plugin_port = NULL;
  
  g_object_set(port,
	       "plugin-port", &plugin_port,
	       NULL);

  au_audio_unit = [av_audio_unit AUAudioUnit];

  /* parameter */
  parameter_tree = [au_audio_unit parameterTree];

  parameter_arr = [parameter_tree allParameters];
  
  i_stop = [parameter_arr count];

  for(i = 0; i < i_stop; i++){
    UInt32 flags;
    
    str = [[parameter_arr[i] displayName] UTF8String];
     
    if(str != NULL &&
       plugin_port->port_name != NULL &&
       !g_strcmp0(plugin_port->port_name,
		  str)){
      flags = [parameter_arr[i] flags];
      
      if((kAudioUnitParameterFlag_IsWritable & flags) != 0){
	gfloat val;

	val = g_value_get_float(value);
	
	[parameter_arr[i] value:val];
      }
    }
  }
}

/**
 * ags_fx_audio_unit_audio_load_port:
 * @fx_audio_unit_audio: the #AgsFxAudioUnitAudio
 * 
 * Load port of @fx_audio_unit_audio.
 * 
 * Since: 8.1.2
 */
void
ags_fx_audio_unit_audio_load_port(AgsFxAudioUnitAudio *fx_audio_unit_audio)
{
  AgsAudio *audio;
  AgsPort **audio_unit_port;
  
  AgsAudioUnitPlugin *audio_unit_plugin;

  GList *start_plugin_port, *plugin_port;

  guint audio_channels;
  guint buffer_size;
  guint control_port_count;
  guint nth;
  guint i;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_AUDIO_UNIT_AUDIO(fx_audio_unit_audio) ||
     ags_recall_test_state_flags(AGS_RECALL(fx_audio_unit_audio),
				 AGS_SOUND_STATE_PORT_LOADED)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  g_rec_mutex_lock(recall_mutex);

  audio_unit_plugin = fx_audio_unit_audio->audio_unit_plugin;

  g_rec_mutex_unlock(recall_mutex);
  
  if(audio_unit_plugin == NULL){
    return;
  }

  audio = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  g_object_get(fx_audio_unit_audio,
	       "audio", &audio,
	       "buffer-size", &buffer_size,
	       NULL);

  audio_channels = 0;
  
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       NULL);
  
  /* get plugin port */  
  g_object_get(audio_unit_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  /* get control port count */
  plugin_port = start_plugin_port;

  control_port_count = 0;
  
  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data,
				  AGS_PLUGIN_PORT_CONTROL)){
      control_port_count++;
    }

    plugin_port = plugin_port->next;
  }

  audio_unit_port = g_malloc(control_port_count * sizeof(AgsPort *));
  
  nth = 0;

  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data,
				  AGS_PLUGIN_PORT_CONTROL)){
      AgsPluginPort *current_plugin_port;
      
      gchar *plugin_name;
      gchar *specifier;
      gchar *control_port;

      guint port_index;
      
      GValue default_value = {0,};

      GRecMutex *plugin_port_mutex;
      
      current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);

      /* get plugin port mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(current_plugin_port);

      /* plugin name, specifier and control port */
      plugin_name = g_strdup_printf("audio-unit-<%s>",
				    AGS_BASE_PLUGIN(audio_unit_plugin)->effect);

      specifier = NULL;
      
      port_index = 0;

      g_object_get(current_plugin_port,
		   "port-name", &specifier,
		   "port-index", &port_index,
		   NULL);

      control_port = g_strdup_printf("%u/%u",
				     nth,
				     control_port_count);

      /* default value */
      g_value_init(&default_value,
		   G_TYPE_FLOAT);
      
      g_rec_mutex_lock(plugin_port_mutex);
      
      g_value_copy(current_plugin_port->default_value,
		   &default_value);
      
      g_rec_mutex_unlock(plugin_port_mutex);

      /* audio_unit port */
      audio_unit_port[nth] = g_object_new(AGS_TYPE_PORT,
					  "plugin-name", plugin_name,
					  "specifier", specifier,
					  "control-port", control_port,
					  "port-value-is-pointer", FALSE,
					  "port-value-type", G_TYPE_FLOAT,
					  NULL);
      
      if(ags_plugin_port_test_flags(current_plugin_port,
				    AGS_PLUGIN_PORT_OUTPUT)){
	ags_port_set_flags(audio_unit_port[nth], AGS_PORT_IS_OUTPUT);
	  
	ags_recall_set_flags((AgsRecall *) fx_audio_unit_audio,
			     AGS_RECALL_HAS_OUTPUT_PORT);
	
      }else{
	if(!ags_plugin_port_test_flags(current_plugin_port,
				       AGS_PLUGIN_PORT_INTEGER) &&
	   !ags_plugin_port_test_flags(current_plugin_port,
				       AGS_PLUGIN_PORT_TOGGLED)){
	  ags_port_set_flags(audio_unit_port[nth], AGS_PORT_INFINITE_RANGE);
	}
      }

      ags_recall_add_port((AgsRecall *) fx_audio_unit_audio,
			  audio_unit_port[nth]);
	
      g_object_set(audio_unit_port[nth],
		   "plugin-port", current_plugin_port,
		   NULL);

      ags_port_util_load_audio_unit_conversion(audio_unit_port[nth],
					       current_plugin_port);

      g_signal_connect(audio_unit_port[nth], "safe-write",
		       G_CALLBACK(ags_fx_audio_unit_audio_port_safe_write_callback), fx_audio_unit_audio);
      
      ags_port_safe_write_raw(audio_unit_port[nth],
			      &default_value);
      
      g_free(plugin_name);
      g_free(specifier);
      g_free(control_port);

      g_value_unset(&default_value);

      nth++;
    }
      
    plugin_port = plugin_port->next;      
  }
  
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxAudioUnitAudioScopeData *scope_data;

    scope_data = fx_audio_unit_audio->scope_data[i];

    if((i == AGS_SOUND_SCOPE_PLAYBACK) ||
       (i == AGS_SOUND_SCOPE_NOTATION) ||
       (i == AGS_SOUND_SCOPE_MIDI)){
      ags_fx_audio_unit_audio_scope_data_load_port(fx_audio_unit_audio,
						   scope_data);
    }
  }  
  
  ags_recall_set_state_flags(AGS_RECALL(fx_audio_unit_audio),
			     AGS_SOUND_STATE_PORT_LOADED);
  
  /* unref */
  if(audio != NULL){
    g_object_unref(audio);
  }
}

void*
ags_fx_audio_unit_audio_render_thread_loop(AgsFxAudioUnitAudio *fx_audio_unit_audio)
{
  AgsAudio *audio;

  GObject *output_soundcard;
  
  AVAudioEngine *audio_engine;

  gdouble bpm;
  guint note_offset;
  gdouble delay_counter;
  gdouble absolute_delay;
  gint64 frame_position;
  guint copy_mode;
  guint audio_channels;
  guint input_pads;
  guint samplerate;
  guint buffer_size;
  gint status;
  gint i;
  guint j;
  guint k;
  gboolean is_running;

  NSError *ns_error;
  
  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  audio = NULL;

  g_object_get(fx_audio_unit_audio,
	       "audio", &audio,
	       NULL);
  
  /*  */
  g_rec_mutex_lock(recall_mutex);

  is_running = fx_audio_unit_audio->render_thread_running;

  g_rec_mutex_unlock(recall_mutex);

  ns_error = NULL;

  [((AVAudioEngine *) fx_audio_unit_audio->audio_engine) startAndReturnError:&ns_error];

  if(ns_error != NULL &&
     [ns_error code] != noErr){
    g_warning("error during audio engine start - %d", [ns_error code]);
  }
  
  [((AVAudioSequencer *) fx_audio_unit_audio->av_audio_sequencer) startAndReturnError:&ns_error];

  if(ns_error != NULL &&
     [ns_error code] != noErr){
    g_warning("error during audio sequencer start - %d", [ns_error code]);
  }

  [((AVAudioSequencer *) fx_audio_unit_audio->av_audio_player_node) play];
  
  while(is_running){
    audio_engine = (AVAudioEngine *) fx_audio_unit_audio->audio_engine;

    copy_mode = AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT;
    
    audio_channels = ags_audio_get_audio_channels(audio);
    
    input_pads = ags_audio_get_input_pads(audio);

    output_soundcard = NULL;
    
    buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
    
    g_object_get(fx_audio_unit_audio,
		 "output-soundcard", &output_soundcard,
		 "buffer-size", &buffer_size,
		 NULL);

    bpm = 120.0;
    
    note_offset = 0;

    absolute_delay = AGS_SOUNDCARD_DEFAULT_DELAY;

    delay_counter = 0.0;
    
    if(output_soundcard != NULL){
      bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(output_soundcard));

      absolute_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard));
      
      delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(output_soundcard));
      
      note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
    }
    
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsFxAudioUnitAudioScopeData *scope_data;
    
      AVAudioPCMBuffer *av_output_buffer, *av_input_buffer;
  
      GList *active_audio_signal;
      GList *audio_unit_audio_signal;

      guint active_pad;
	
      scope_data = fx_audio_unit_audio->scope_data[i];

      if(scope_data != NULL){
	/* wait until audio signals completed */
	g_mutex_lock(&(scope_data->completed_mutex));

	ags_atomic_boolean_set(&(scope_data->completed_wait),
			       TRUE);

	while(ags_atomic_boolean_get(&(scope_data->completed_wait)) &&
	      ags_atomic_int_get(&(scope_data->active_audio_signal_count)) != ags_atomic_int_get(&(scope_data->completed_audio_signal_count))){
	  g_cond_wait(&(scope_data->completed_cond),
		      &(scope_data->completed_mutex));
	}
	
	ags_atomic_boolean_set(&(scope_data->completed_wait),
			       FALSE);

	ags_atomic_int_set(&(scope_data->completed_audio_signal_count),
			   0);
    
	g_mutex_unlock(&(scope_data->completed_mutex));

	av_output_buffer = (AVAudioPCMBuffer *) scope_data->av_output_buffer;
	av_input_buffer = (AVAudioPCMBuffer *) scope_data->av_input_buffer;

	g_hash_table_remove_all(scope_data->written_audio_signal);
	
	for(active_pad = 0; active_pad < input_pads; active_pad++){
	  /* set position */
	  [((AVAudioSequencer *) fx_audio_unit_audio->av_audio_sequencer) currentPositionInBeats:((note_offset / 4.0) + (delay_counter / absolute_delay / 4.0))];

	  /* set bpm */
	  AVMusicTrack *tempo_track = [((AVAudioSequencer *) fx_audio_unit_audio->av_audio_sequencer) tempoTrack];

	  [tempo_track AVMusicEventEnumerationBlock:^(AVMusicEvent *event, double *time_stamp, _Bool *remove_event){
	      remove_event[0] = TRUE;
	    }];

	  AVExtendedTempoEvent *tempo_event = [[AVExtendedTempoEvent alloc] initWithTempo:bpm];
	  
	  [tempo_track addEvent:tempo_event atBeat:(note_offset / 4.0)];
	  
	  /* fill midi */
	  if(scope_data->channel_data[0]->input_data[active_pad]->av_music_track != NULL){
	    [((AVAudioSequencer *) fx_audio_unit_audio->av_audio_sequencer) removeTrack:((AVMusicTrack *) scope_data->channel_data[0]->input_data[active_pad]->av_music_track)];

	    scope_data->channel_data[0]->input_data[active_pad]->av_music_track = NULL;
	  }
	    
	  if(scope_data->channel_data[0]->input_data[active_pad]->key_on > 0){
	    AVMusicTrack *av_music_track;
	    AVMIDINoteEvent *av_midi_note_event;

	    GList *note;

	    note = scope_data->channel_data[0]->input_data[active_pad]->note;

	    if(note != NULL){
	      av_music_track = [((AVAudioSequencer *) fx_audio_unit_audio->av_audio_sequencer) createAndAppendTrack];

	      while(note != NULL){
		guint x0_256th, x1_256th;

		x0_256th = ags_note_get_x0_256th(note->data);
		x1_256th = ags_note_get_x1_256th(note->data);
		
		scope_data->channel_data[0]->input_data[active_pad]->av_music_track = av_music_track;
		
		av_midi_note_event = [[AVMIDINoteEvent alloc] initWithChannel:0 key:active_pad velocity:127 duration:((double) (x1_256th - x0_256th) / 64.0)];
	    
		[av_music_track addEvent:av_midi_note_event atBeat:(x0_256th / 64.0)];

		/* iterate */
		note = note->next;
	      }
	    }
	  }

	  /* schedule input buffer */
	  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

	  g_object_get(fx_audio_unit_audio,
		       "samplerate", &samplerate,
		       NULL);
	  
	  frame_position = (gint64) ((note_offset * absolute_delay) + delay_counter) * buffer_size;
	  
	  [((AVAudioPlayerNode *) fx_audio_unit_audio->av_audio_player_node) scheduleBuffer:av_input_buffer atTime:[[AVAudioTime alloc] initWithSampleTime:frame_position sampleRate:((double) samplerate)] options:0 completionHandler:nil];
	  
	  /* render */
	  ns_error = NULL;
	  
	  status = [audio_engine renderOffline:buffer_size toBuffer:av_output_buffer error:&ns_error];
    
	  if(ns_error != NULL &&
	     [ns_error code] != noErr){
	    g_warning("render offline error - %d", [ns_error code]);
	  }

	  /* output */
	  active_audio_signal = g_hash_table_lookup(scope_data->active_audio_signal,
						    GUINT_TO_POINTER(active_pad));
	  
	  while(active_audio_signal != NULL){
	    AgsAudioSignal *current_audio_signal;

	    guint audio_channel;
	    guint line;
	  
	    current_audio_signal = active_audio_signal->data;

	    audio_channel = AGS_CHANNEL(AGS_RECYCLING(current_audio_signal->recycling)->channel)->audio_channel;
	    line = AGS_CHANNEL(AGS_RECYCLING(current_audio_signal->recycling)->channel)->line;

	    if(g_hash_table_lookup(scope_data->written_audio_signal, GUINT_TO_POINTER(line)) == NULL){
	      /* fill output buffer */
	      ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
							  current_audio_signal->stream->data, 1, 0,
							  [av_output_buffer floatChannelData], audio_channels, audio_channel,
							  buffer_size, copy_mode);

	      g_hash_table_insert(scope_data->written_audio_signal,
				  GUINT_TO_POINTER(line),
				  current_audio_signal);
	    }
	    
	    /* iterate */
	    active_audio_signal = active_audio_signal->next;
	  }
	}

	/* signal post sync */
	g_mutex_lock(&(scope_data->render_mutex));
    
	ags_atomic_boolean_set(&(scope_data->render_done),
			       TRUE);

	if(ags_atomic_boolean_get(&(scope_data->render_wait))){
	  g_cond_signal(&(scope_data->render_cond));
	}
    
	g_mutex_unlock(&(scope_data->render_mutex));
      }
    }
    
    /*  */
    g_rec_mutex_lock(recall_mutex);

    is_running = fx_audio_unit_audio->render_thread_running;

    g_rec_mutex_unlock(recall_mutex);
  }

  g_thread_exit(NULL);
  
  return(NULL);
}

/**
 * ags_fx_audio_unit_audio_start_render_thread:
 * @fx_audio_unit_audio: the #AgsFxAudioUnitAudio
 * 
 * Start render thread of @fx_audio_unit_audio.
 * 
 * Since: 8.1.2
 */
void
ags_fx_audio_unit_audio_start_render_thread(AgsFxAudioUnitAudio *fx_audio_unit_audio)
{
  if(fx_audio_unit_audio == NULL ||
     !AGS_IS_FX_AUDIO_UNIT_AUDIO(fx_audio_unit_audio) ||
     fx_audio_unit_audio->render_thread_running){
    return;
  }

  fx_audio_unit_audio->render_thread_running = TRUE;
  
  fx_audio_unit_audio->render_thread = g_thread_new("Advanced Gtk+ Sequencer - Audio Unit",
						    (GThreadFunc) ags_fx_audio_unit_audio_render_thread_loop,
						    (gpointer) fx_audio_unit_audio);
}

/**
 * ags_fx_audio_unit_audio_stop_render_thread:
 * @fx_audio_unit_audio: the #AgsFxAudioUnitAudio
 * 
 * Start render thread of @fx_audio_unit_audio.
 * 
 * Since: 8.1.2
 */
void
ags_fx_audio_unit_audio_stop_render_thread(AgsFxAudioUnitAudio *fx_audio_unit_audio)
{
  if(fx_audio_unit_audio == NULL ||
     !AGS_IS_FX_AUDIO_UNIT_AUDIO(fx_audio_unit_audio) ||
     !fx_audio_unit_audio->render_thread_running){
    return;
  }

  fx_audio_unit_audio->render_thread_running = FALSE;
}

/**
 * ags_fx_audio_unit_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxAudioUnitAudio
 *
 * Returns: the new #AgsFxAudioUnitAudio
 *
 * Since: 8.1.2
 */
AgsFxAudioUnitAudio*
ags_fx_audio_unit_audio_new(AgsAudio *audio)
{
  AgsFxAudioUnitAudio *fx_audio_unit_audio;

  fx_audio_unit_audio = (AgsFxAudioUnitAudio *) g_object_new(AGS_TYPE_FX_AUDIO_UNIT_AUDIO,
							     "audio", audio,
							     NULL);

  return(fx_audio_unit_audio);
}
