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
#include <ags/plugin/ags_audio_unit_plugin.h>
#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_sound_enums.h>
#include <ags/audio/ags_playback_domain.h>
#include <ags/audio/ags_playback.h>
#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_port_util.h>
#include <ags/audio/ags_synth_util.h>

#include <ags/audio/thread/ags_audio_thread.h>
#include <ags/audio/thread/ags_channel_thread.h>

#include <ags/audio/fx/ags_fx_audio_unit_channel.h>

#include <CoreFoundation/CoreFoundation.h>
#include <AVFoundation/AVFoundation.h>
#include <AudioToolbox/AudioToolbox.h>
#include <AudioToolbox/AUComponent.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioUnit/AUComponent.h>
#include <CoreAudio/CoreAudio.h>

#include <mach/mach_time.h>

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

void* ags_fx_audio_unit_audio_render_thread_loop_mono(gpointer data);

gboolean ags_fx_audio_unit_audio_is_stereo_ready(AgsFxAudioUnitAudio *fx_audio_unit_audio);
GList* ags_fx_audio_unit_audio_pull_stereo_iterate_data(AgsFxAudioUnitAudio *fx_audio_unit_audio);

void* ags_fx_audio_unit_audio_render_thread_loop_stereo(gpointer data);

/**
 * SECTION:ags_fx_audio_unit_audio
 * @short_description: fx Audio Unit audio
 * @title: AgsFxAudioUnitAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_audio_unit_audio.h
 *
 * The #AgsFxAudioUnitAudio class provides ports to the effect processor.
 */

struct _AgsFxAudioUnitIterateData
{
  AgsFxAudioUnitAudioScopeData *scope_data;
  AgsFxAudioUnitAudioChannelData *channel_data;

  guint pad;
  guint audio_channel;

  gint sound_scope;
  
  AgsAudioSignal *audio_signal;
};

__thread AgsFxAudioUnitAudioScopeData *ags_fx_audio_unit_iterate_scope_data = NULL;
__thread AgsFxAudioUnitAudioChannelData *ags_fx_audio_unit_iterate_channel_data = NULL;
__thread guint ags_fx_audio_unit_iterate_sub_block = 0;

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

  AVAudioFormat *av_format;
  
  AudioStreamBasicDescription stream_description;
  
  gchar *thread_model, *super_threaded_scope;
  
  gboolean super_threaded_channel;
  guint pcm_channels;
  guint plugin_channels;
  guint samplerate;
  guint i, j;
  
  /* config */
  config = ags_config_get_instance();
  
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
  
  pcm_channels = ags_soundcard_helper_config_get_pcm_channels(config);
  samplerate = (guint) ags_soundcard_helper_config_get_samplerate(config);
  
  /* thread model */
  super_threaded_channel = FALSE;
  
  thread_model = ags_config_get_value(config,
				      AGS_CONFIG_THREAD,
				      "model");
  super_threaded_scope = NULL;
  
  if(thread_model != NULL &&
     !g_ascii_strncasecmp(thread_model,
			  "super-threaded",
			  15)){
    super_threaded_scope = ags_config_get_value(config,
						AGS_CONFIG_THREAD,
						"super-threaded-scope");
    if(super_threaded_scope != NULL &&
       (!g_ascii_strncasecmp(super_threaded_scope,
			     "channel",
			     8))){
      super_threaded_channel = TRUE;
    }
  }

  g_free(thread_model);
  
  g_free(super_threaded_scope);

  /* plugin channels */
  plugin_channels = pcm_channels;
  
  if(!super_threaded_channel){
    fx_audio_unit_audio->flags = AGS_FX_AUDIO_UNIT_AUDIO_MONO;

    plugin_channels = 1;
  }else{
    switch(pcm_channels){
    case 1:
      {
	fx_audio_unit_audio->flags = AGS_FX_AUDIO_UNIT_AUDIO_MONO;
      }
      break;
    case 2:
      {
	fx_audio_unit_audio->flags = AGS_FX_AUDIO_UNIT_AUDIO_STEREO;
      }
      break;
    case 3:
      {
	fx_audio_unit_audio->flags = AGS_FX_AUDIO_UNIT_AUDIO_SURROUND_2_1;
      }
      break;
    case 6:
      {
	fx_audio_unit_audio->flags = AGS_FX_AUDIO_UNIT_AUDIO_SURROUND_5_1;
      }
      break;
    case 8:
      {
	fx_audio_unit_audio->flags = AGS_FX_AUDIO_UNIT_AUDIO_SURROUND_7_1;
      }
      break;
    default:
      {
	fx_audio_unit_audio->flags = AGS_FX_AUDIO_UNIT_AUDIO_MONO;

	plugin_channels = 1;
      }
    }
  }
  
  /* audio unit plugin */
  fx_audio_unit_audio->audio_unit_plugin = NULL;

  /* audio unit */
  stream_description = (AudioStreamBasicDescription) {0,};

  stream_description.mFormatID = kAudioFormatLinearPCM;
  stream_description.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsNonInterleaved | kLinearPCMFormatFlagIsPacked | kAudioFormatFlagsNativeEndian;
  stream_description.mSampleRate = (double) samplerate;
  stream_description.mBitsPerChannel = 8 * sizeof(gfloat);
  stream_description.mFramesPerPacket = 1;
  stream_description.mChannelsPerFrame = plugin_channels;
  stream_description.mBytesPerFrame = sizeof(gfloat);
  stream_description.mBytesPerPacket = sizeof(gfloat);

  av_format = [[AVAudioFormat alloc] initWithStreamDescription:&stream_description];

  fx_audio_unit_audio->av_format = (gpointer) av_format;
  
  fx_audio_unit_audio->audio_engine = NULL;

  fx_audio_unit_audio->av_audio_unit = NULL;

  fx_audio_unit_audio->av_audio_sequencer = NULL;

  fx_audio_unit_audio->av_music_track = NULL;

  /* render thread */
  fx_audio_unit_audio->render_thread_running = FALSE;
  
  ags_atomic_int_set(&(fx_audio_unit_audio->render_ref_count),
		     0);

  fx_audio_unit_audio->render_thread = NULL;

  /* pre sync */
  ags_atomic_boolean_set(&(fx_audio_unit_audio->pre_sync_wait),
			 FALSE);

  ags_atomic_int_set(&(fx_audio_unit_audio->active_iteration_count),
		     0);

  g_mutex_init(&(fx_audio_unit_audio->pre_sync_mutex));

  g_cond_init(&(fx_audio_unit_audio->pre_sync_cond));
  
  /* iterate data */
  ags_atomic_boolean_set(&(fx_audio_unit_audio->prepare_iteration_wait),
			 FALSE);

  ags_atomic_pointer_set(&(fx_audio_unit_audio->iterate_data),
			 NULL);

  ags_atomic_pointer_set(&(fx_audio_unit_audio->active_iterate_data),
			 NULL);
  
  g_mutex_init(&(fx_audio_unit_audio->prepare_iteration_mutex));

  g_cond_init(&(fx_audio_unit_audio->prepare_iteration_cond));
  
  /* iterate data */
  ags_atomic_boolean_set(&(fx_audio_unit_audio->completed_iteration_wait),
			 FALSE);

  ags_atomic_boolean_set(&(fx_audio_unit_audio->completed_iteration_done),
			 FALSE);
  
  g_mutex_init(&(fx_audio_unit_audio->completed_iteration_mutex));

  g_cond_init(&(fx_audio_unit_audio->completed_iteration_cond));
  
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

  AudioStreamBasicDescription stream_description;
  
  guint audio_channels;
  guint plugin_channels;
  guint buffer_size;
  guint samplerate;
  guint i, j;
  
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

  /* plugin channels */
  plugin_channels = audio_channels;
  
  if(ags_fx_audio_unit_audio_test_flags(fx_audio_unit_audio, AGS_FX_AUDIO_UNIT_AUDIO_MONO)){
    plugin_channels = 1;
  }

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  stream_description = (AudioStreamBasicDescription) {0,};

  stream_description.mFormatID = kAudioFormatLinearPCM;
  stream_description.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsNonInterleaved | kLinearPCMFormatFlagIsPacked | kAudioFormatFlagsNativeEndian;
  stream_description.mSampleRate = (double) samplerate;
  stream_description.mBitsPerChannel = 8 * sizeof(gfloat);
  stream_description.mFramesPerPacket = 1;
  stream_description.mChannelsPerFrame = plugin_channels;
  stream_description.mBytesPerFrame = sizeof(gfloat);
  stream_description.mBytesPerPacket = sizeof(gfloat);

  av_format = [[AVAudioFormat alloc] initWithStreamDescription:&stream_description];

  fx_audio_unit_audio->av_format = (gpointer) av_format;
  
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxAudioUnitAudioScopeData *scope_data;

    AVAudioPCMBuffer *av_output, *av_input;
    
    scope_data = fx_audio_unit_audio->scope_data[i];
    	
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      if(!ags_fx_audio_unit_audio_test_flags(fx_audio_unit_audio, AGS_FX_AUDIO_UNIT_AUDIO_MONO)){
	AudioBufferList *audio_buffer_list;
	
	/* output buffer */
	if(scope_data->output != NULL){
	  ags_stream_free(scope_data->output);
	}
	
	scope_data->output = ags_stream_alloc(plugin_channels * buffer_size,
					      AGS_SOUNDCARD_FLOAT);
	scope_data->output_buffer_size = buffer_size;

	/* input buffer */
	if(scope_data->input != NULL){
	  ags_stream_free(scope_data->input);
	}
	
	scope_data->input = ags_stream_alloc(plugin_channels * buffer_size,
					     AGS_SOUNDCARD_FLOAT);
	scope_data->input_buffer_size = buffer_size;

	/* av output buffer */
	if(scope_data->av_output != NULL){
	  [(AVAudioPCMBuffer *) scope_data->av_output dealloc];
	}
	
	av_output = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
		     frameCapacity:(plugin_channels * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE)];

	av_output.frameLength = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE;
      
	scope_data->av_output = (gpointer) av_output;

	/* av input buffer */
	if(scope_data->av_input != NULL){
	  [(AVAudioPCMBuffer *) scope_data->av_input dealloc];
	}

	av_input = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
		    frameCapacity:(plugin_channels * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE)];
    
	av_input.frameLength = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE;
      
	scope_data->av_input = (gpointer) av_input;

	/* audio buffer list */
	if(scope_data->audio_buffer_list != NULL){
	  free(scope_data->audio_buffer_list);
	}
	    
	audio_buffer_list = (AudioBufferList *) malloc(sizeof(AudioBufferList) + (audio_channels * sizeof(AudioBuffer)));
		
	audio_buffer_list->mNumberBuffers = audio_channels;
	
	for(j = 0; j < audio_channels; j++){
	  audio_buffer_list->mBuffers[j].mData = (float *) malloc(AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float));
	    
	  memset(audio_buffer_list->mBuffers[j].mData, 0, AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float));

	  audio_buffer_list->mBuffers[j].mDataByteSize = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float);
	  audio_buffer_list->mBuffers[j].mNumberChannels = 1;
	}
	    
	scope_data->audio_buffer_list = (gpointer) audio_buffer_list;
      }else{
	for(j = 0; j < audio_channels; j++){
	  AgsFxAudioUnitAudioChannelData *channel_data;

	  AudioBufferList *audio_buffer_list;
	  
	  channel_data = scope_data->channel_data[j];
	  
	  if(ags_fx_audio_unit_audio_test_flags(fx_audio_unit_audio, AGS_FX_AUDIO_UNIT_AUDIO_MONO)){
	    /* output buffer */
	    if(channel_data->output != NULL){
	      ags_stream_free(channel_data->output);
	    }
	    
	    channel_data->output = ags_stream_alloc(plugin_channels * buffer_size,
						    AGS_SOUNDCARD_FLOAT);
	    channel_data->output_buffer_size = buffer_size;

	    /* input buffer */
	    if(channel_data->input != NULL){
	      ags_stream_free(channel_data->input);
	    }
	    
	    channel_data->input = ags_stream_alloc(plugin_channels * buffer_size,
						   AGS_SOUNDCARD_FLOAT);
	    channel_data->input_buffer_size = buffer_size;

	    /* av output buffer */
	    if(channel_data->av_output != NULL){
	      [(AVAudioPCMBuffer *) channel_data->av_output dealloc];
	    }
	    
	    av_output = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
			 frameCapacity:(plugin_channels * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE)];

	    av_output.frameLength = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE;
    
	    channel_data->av_output = (gpointer) av_output;

	    /* av input buffer */
	    if(channel_data->av_input != NULL){
	      [(AVAudioPCMBuffer *) channel_data->av_input dealloc];
	    }

	    av_input = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
			frameCapacity:(plugin_channels * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE)];
    
	    av_input.frameLength = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE;
    
	    channel_data->av_input = (gpointer) av_input;

	    /* audio buffer list */
	    if(channel_data->audio_buffer_list != NULL){
	      free(channel_data->audio_buffer_list);
	    }
	    
	    audio_buffer_list = (AudioBufferList *) malloc(sizeof(AudioBufferList) + sizeof(AudioBuffer));
		
	    audio_buffer_list->mNumberBuffers = 1;

	    audio_buffer_list->mBuffers[0].mData = (float *) malloc(AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float));
	    
	    memset(audio_buffer_list->mBuffers[0].mData, 0, AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float));

	    audio_buffer_list->mBuffers[0].mDataByteSize = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float);
	    audio_buffer_list->mBuffers[0].mNumberChannels = 1;
	    
	    channel_data->audio_buffer_list = (gpointer) audio_buffer_list;
	  }
	}
      }
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

  AudioStreamBasicDescription stream_description;

  guint audio_channels;
  guint plugin_channels;
  guint buffer_size;
  guint samplerate;
  guint i, j;
  
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

  /* plugin channels */
  plugin_channels = audio_channels;
  
  if(ags_fx_audio_unit_audio_test_flags(fx_audio_unit_audio, AGS_FX_AUDIO_UNIT_AUDIO_MONO)){
    plugin_channels = 1;
  }

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  stream_description = (AudioStreamBasicDescription) {0,};

  stream_description.mFormatID = kAudioFormatLinearPCM;
  stream_description.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsNonInterleaved | kLinearPCMFormatFlagIsPacked | kAudioFormatFlagsNativeEndian;
  stream_description.mSampleRate = (double) samplerate;
  stream_description.mBitsPerChannel = 8 * sizeof(gfloat);
  stream_description.mFramesPerPacket = 1;
  stream_description.mChannelsPerFrame = plugin_channels;
  stream_description.mBytesPerFrame = sizeof(gfloat);
  stream_description.mBytesPerPacket = sizeof(gfloat);

  av_format = [[AVAudioFormat alloc] initWithStreamDescription:&stream_description];

  fx_audio_unit_audio->av_format = (gpointer) av_format;

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxAudioUnitAudioScopeData *scope_data;

    AVAudioPCMBuffer *av_output, *av_input;
 
    scope_data = fx_audio_unit_audio->scope_data[i];
	
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      if(!ags_fx_audio_unit_audio_test_flags(fx_audio_unit_audio, AGS_FX_AUDIO_UNIT_AUDIO_MONO)){
	AudioBufferList *audio_buffer_list;

	/* output buffer */
	if(scope_data->output != NULL){
	  ags_stream_free(scope_data->output);
	}
		    
	scope_data->output = ags_stream_alloc(plugin_channels * buffer_size,
					      AGS_SOUNDCARD_FLOAT);
	scope_data->output_buffer_size = buffer_size;

	/* input buffer */
	if(scope_data->input != NULL){
	  ags_stream_free(scope_data->input);
	}
		    
	scope_data->input = ags_stream_alloc(plugin_channels * buffer_size,
					     AGS_SOUNDCARD_FLOAT);
	scope_data->input_buffer_size = buffer_size;

	/* av output buffer */
	if(scope_data->av_output != NULL){
	  [(AVAudioPCMBuffer *) scope_data->av_output dealloc];
	}

	av_output = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
		     frameCapacity:(plugin_channels * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE)];

	av_output.frameLength = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE;
      
	scope_data->av_output = (gpointer) av_output;

	/* av input buffer */
	if(scope_data->av_input != NULL){
	  [(AVAudioPCMBuffer *) scope_data->av_input dealloc];
	}
	
	av_input = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
		    frameCapacity:(plugin_channels * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE)];
    
	av_input.frameLength = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE;
      
	scope_data->av_input = (gpointer) av_input;

	/* audio buffer list */
	if(scope_data->audio_buffer_list != NULL){
	  free(scope_data->audio_buffer_list);
	}

	audio_buffer_list = (AudioBufferList *) malloc(sizeof(AudioBufferList) + (audio_channels * sizeof(AudioBuffer)));
	
	audio_buffer_list->mNumberBuffers = audio_channels;
	
	for(j = 0; j < audio_channels; j++){
	  audio_buffer_list->mBuffers[j].mData = (float *) malloc(AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float));
	    
	  memset(audio_buffer_list->mBuffers[j].mData, 0, AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float));

	  audio_buffer_list->mBuffers[j].mDataByteSize = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float);
	  audio_buffer_list->mBuffers[j].mNumberChannels = 1;
	}
	
	scope_data->audio_buffer_list = (gpointer) audio_buffer_list;	
      }else{
	for(j = 0; j < audio_channels; j++){
	  AgsFxAudioUnitAudioChannelData *channel_data;

	  AudioBufferList *audio_buffer_list;
	  
	  channel_data = scope_data->channel_data[j];
	  
	  if(ags_fx_audio_unit_audio_test_flags(fx_audio_unit_audio, AGS_FX_AUDIO_UNIT_AUDIO_MONO)){
	    /* output buffer */
	    if(channel_data->output != NULL){
	      ags_stream_free(channel_data->output);
	    }

	    channel_data->output = ags_stream_alloc(plugin_channels * buffer_size,
						    AGS_SOUNDCARD_FLOAT);
	    channel_data->output_buffer_size = buffer_size;

	    /* input buffer */
	    if(channel_data->input != NULL){
	      ags_stream_free(channel_data->input);
	    }

	    channel_data->input = ags_stream_alloc(plugin_channels * buffer_size,
						   AGS_SOUNDCARD_FLOAT);
	    channel_data->input_buffer_size = buffer_size;

	    /* av output buffer */
	    if(channel_data->av_output != NULL){
	      [(AVAudioPCMBuffer *) channel_data->av_output dealloc];
	    }

	    av_output = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
			 frameCapacity:(plugin_channels * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE)];

	    av_output.frameLength = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE;
    
	    channel_data->av_output = (gpointer) av_output;

	    /* av input buffer */
	    if(channel_data->av_input != NULL){
	      [(AVAudioPCMBuffer *) channel_data->av_input dealloc];
	    }

	    av_input = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
			frameCapacity:(plugin_channels * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE)];
    
	    av_input.frameLength = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE;
    
	    channel_data->av_input = (gpointer) av_input;

	    /* audio buffer list */
	    if(channel_data->audio_buffer_list != NULL){
	      free(channel_data->audio_buffer_list);
	    }

	    audio_buffer_list = (AudioBufferList *) malloc(sizeof(AudioBufferList) + sizeof(AudioBuffer));
		
	    audio_buffer_list->mNumberBuffers = 1;

	    audio_buffer_list->mBuffers[0].mData = (float *) malloc(AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float));
	    
	    memset(audio_buffer_list->mBuffers[0].mData, 0, AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float));

	    audio_buffer_list->mBuffers[0].mDataByteSize = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float);
	    audio_buffer_list->mBuffers[0].mNumberChannels = 1;
	    
	    channel_data->audio_buffer_list = (gpointer) audio_buffer_list;
	  }
	}
      }
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

  AudioStreamBasicDescription stream_description;

  guint plugin_channels;
  guint buffer_size;
  guint samplerate;
  guint i, j;

  GRecMutex *recall_mutex;

  if(audio_channels == audio_channels_old){
    return;
  }
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);
  
  /* plugin channels */
  plugin_channels = audio_channels;
  
  if(ags_fx_audio_unit_audio_test_flags(fx_audio_unit_audio, AGS_FX_AUDIO_UNIT_AUDIO_MONO)){
    plugin_channels = 1;
  }
  
  /* get properties */
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  g_object_get(fx_audio_unit_audio,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  if(buffer_size < AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE){
    g_critical("buffer size smaller than %d", AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE);
  }else{
    if(buffer_size % AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE != 0){
      g_warning("buffer size not a multiple of %d", AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE);
    }
  }
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  stream_description = (AudioStreamBasicDescription) {0,};

  stream_description.mFormatID = kAudioFormatLinearPCM;
  stream_description.mFormatFlags = kAudioFormatFlagIsFloat | kAudioFormatFlagIsNonInterleaved | kLinearPCMFormatFlagIsPacked | kAudioFormatFlagsNativeEndian;
  stream_description.mSampleRate = (double) samplerate;
  stream_description.mBitsPerChannel = 8 * sizeof(gfloat);
  stream_description.mFramesPerPacket = 1;
  stream_description.mChannelsPerFrame = plugin_channels;
  stream_description.mBytesPerFrame = sizeof(gfloat);
  stream_description.mBytesPerPacket = sizeof(gfloat);

  av_format = [[AVAudioFormat alloc] initWithStreamDescription:&stream_description];

  fx_audio_unit_audio->av_format = (gpointer) av_format;

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxAudioUnitAudioScopeData *scope_data;

    scope_data = fx_audio_unit_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      if(audio_channels < audio_channels_old){
	for(j = audio_channels; j < audio_channels_old; j++){
	  AgsFxAudioUnitAudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];
	
	  ags_fx_audio_unit_audio_channel_data_free(channel_data);
	}
      }
      
      if(audio_channels > 0){
	if(scope_data->channel_data == NULL){
	  scope_data->channel_data = (AgsFxAudioUnitAudioChannelData **) g_malloc(audio_channels * sizeof(AgsFxAudioUnitAudioChannelData *)); 
	}else{
	  scope_data->channel_data = (AgsFxAudioUnitAudioChannelData **) g_realloc(scope_data->channel_data,
										   audio_channels * sizeof(AgsFxAudioUnitAudioChannelData *)); 
	}
      }else{
	scope_data->channel_data = NULL;
      }
      
      if(audio_channels > audio_channels_old){
	for(j = audio_channels_old; j < audio_channels; j++){
	  AgsFxAudioUnitAudioChannelData *channel_data;
	  
	  scope_data->channel_data[j] = ags_fx_audio_unit_audio_channel_data_alloc();

	  scope_data->channel_data[j]->parent = scope_data;
	}
      }
    }
  }
  
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxAudioUnitAudioScopeData *scope_data;

    AVAudioPCMBuffer *av_output, *av_input;
    AudioBufferList *audio_buffer_list;

    scope_data = fx_audio_unit_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      scope_data->audio_channels = audio_channels;
    
      if(!ags_fx_audio_unit_audio_test_flags(fx_audio_unit_audio, AGS_FX_AUDIO_UNIT_AUDIO_MONO)){
	AudioBufferList *audio_buffer_list;

	/* output buffer */
	if(scope_data->output != NULL){
	  ags_stream_free(scope_data->output);
	}
		    
	scope_data->output = ags_stream_alloc(plugin_channels * buffer_size,
					      AGS_SOUNDCARD_FLOAT);
	scope_data->output_buffer_size = buffer_size;

	/* input buffer */
	if(scope_data->input != NULL){
	  ags_stream_free(scope_data->input);
	}
		    
	scope_data->input = ags_stream_alloc(plugin_channels * buffer_size,
					     AGS_SOUNDCARD_FLOAT);
	scope_data->input_buffer_size = buffer_size;

	/* av output buffer */
	if(scope_data->av_output != NULL){
	  [(AVAudioPCMBuffer *) scope_data->av_output dealloc];
	}

	av_output = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
		     frameCapacity:(plugin_channels * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE)];

	av_output.frameLength = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE;
    
	scope_data->av_output = (gpointer) av_output;

	/* av input buffer */
	if(scope_data->av_input != NULL){
	  [(AVAudioPCMBuffer *) scope_data->av_input dealloc];
	}

	av_input = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
		    frameCapacity:(plugin_channels * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE)];
    
	av_input.frameLength = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE;
    
	scope_data->av_input = (gpointer) av_input;

	/* audio buffer list */
	if(scope_data->audio_buffer_list != NULL){
	  free(scope_data->audio_buffer_list);
	}

	audio_buffer_list = (AudioBufferList *) malloc(sizeof(AudioBufferList) + (plugin_channels * sizeof(AudioBuffer)));
		
	audio_buffer_list->mNumberBuffers = plugin_channels;

	for(j = 0; j < plugin_channels; j++){
	  audio_buffer_list->mBuffers[j].mData = (float *) malloc(AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float));
	    
	  memset(audio_buffer_list->mBuffers[j].mData, 0, AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float));

	  audio_buffer_list->mBuffers[j].mDataByteSize = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float);
	  audio_buffer_list->mBuffers[j].mNumberChannels = 1;
	}
	    
	scope_data->audio_buffer_list = (gpointer) audio_buffer_list;
      }
      
      if(ags_fx_audio_unit_audio_test_flags(fx_audio_unit_audio, AGS_FX_AUDIO_UNIT_AUDIO_MONO)){
	if(audio_channels_old < audio_channels){
	  for(j = audio_channels_old; j < audio_channels; j++){
	    AgsFxAudioUnitAudioChannelData *channel_data;
	  
	    channel_data = scope_data->channel_data[j];
	  
	    if(channel_data->output != NULL){
	      ags_stream_free(channel_data->output);
	    }
	
	    /* output buffer */
	    channel_data->output = ags_stream_alloc(plugin_channels * buffer_size,
						    AGS_SOUNDCARD_FLOAT);
	    channel_data->output_buffer_size = buffer_size;

	    /* input buffer */
	    if(channel_data->input != NULL){
	      ags_stream_free(channel_data->input);
	    }
	    
	    channel_data->input = ags_stream_alloc(plugin_channels * buffer_size,
						   AGS_SOUNDCARD_FLOAT);
	    channel_data->input_buffer_size = buffer_size;

	    /* av output buffer */
	    if(channel_data->av_output != NULL){
	      [(AVAudioPCMBuffer *) channel_data->av_output dealloc];
	    }
	    
	    av_output = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
			 frameCapacity:(plugin_channels * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE)];

	    av_output.frameLength = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE;
    
	    channel_data->av_output = (gpointer) av_output;

	    /* av input buffer */
	    if(channel_data->av_input != NULL){
	      [(AVAudioPCMBuffer *) channel_data->av_input dealloc];
	    }
	    
	    av_input = [[AVAudioPCMBuffer alloc] initWithPCMFormat:av_format
			frameCapacity:(plugin_channels * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE)];
    
	    av_input.frameLength = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE;
    
	    channel_data->av_input = (gpointer) av_input;

	    /* audio buffer list */
	    if(channel_data->audio_buffer_list != NULL){
	      free(channel_data->audio_buffer_list);
	    }

	    audio_buffer_list = (AudioBufferList *) malloc(sizeof(AudioBufferList) + sizeof(AudioBuffer));
		
	    audio_buffer_list->mNumberBuffers = 1;

	    audio_buffer_list->mBuffers[0].mData = (float *) malloc(AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float));
	    
	    memset(audio_buffer_list->mBuffers[0].mData, 0, AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float));

	    audio_buffer_list->mBuffers[0].mDataByteSize = AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE * sizeof(float);
	    audio_buffer_list->mBuffers[0].mNumberChannels = 1;
	    
	    channel_data->audio_buffer_list = (gpointer) audio_buffer_list;
	  }
	}
      }
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
  
  scope_data->running = FALSE;

  scope_data->audio_channels = 0;

  scope_data->output = NULL;
  scope_data->output_buffer_size = 0;

  scope_data->input = NULL;
  scope_data->input_buffer_size = 0;
  
  scope_data->av_output = NULL;

  scope_data->av_input = NULL;

  scope_data->audio_buffer_list = NULL;

  scope_data->volume_util = ags_volume_util_alloc();

  scope_data->channel_data = NULL;
  
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

  if(scope_data->av_output != NULL){
    [(AVAudioPCMBuffer *) scope_data->av_output dealloc];
  }

  if(scope_data->av_input != NULL){
    [(AVAudioPCMBuffer *) scope_data->av_input dealloc];
  }

  if(scope_data->audio_buffer_list != NULL){
    free(scope_data->audio_buffer_list);
  }
  
  ags_volume_util_free(scope_data->volume_util);
  
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

  ags_atomic_int_set(&(channel_data->active_audio_signal),
		     0);

  ags_atomic_int_set(&(channel_data->queued_audio_signal),
		     0);

  channel_data->output = NULL;
  channel_data->output_buffer_size = 0;

  channel_data->input = NULL;
  channel_data->input_buffer_size = 0;
  
  channel_data->av_output = NULL;

  channel_data->av_input = NULL;

  channel_data->audio_buffer_list = NULL;
  
  channel_data->volume_util = ags_volume_util_alloc();
  
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
  
  ags_stream_free(channel_data->output);

  ags_stream_free(channel_data->input);

  if(channel_data->av_output != NULL){
    [(AVAudioPCMBuffer *) channel_data->av_output dealloc];
  }

  if(channel_data->av_input != NULL){
    [(AVAudioPCMBuffer *) channel_data->av_input dealloc];
  }
  
  if(channel_data->audio_buffer_list != NULL){
    free(channel_data->audio_buffer_list);
  }
  
  ags_volume_util_free(channel_data->volume_util);
  
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

  ags_atomic_int_set(&(input_data->active_audio_signal),
		     0);

  ags_atomic_int_set(&(input_data->queued_audio_signal),
		     0);

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

  AVAudioOutputNode *av_output_node;
  AVAudioInputNode *av_input_node;
  AVAudioMixerNode *av_audio_mixer_node;
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
  
  av_format = (AVAudioFormat *) fx_audio_unit_audio->av_format;
  
  /* audio engine */
  audio_engine = [[AVAudioEngine alloc] init];
  
  fx_audio_unit_audio->audio_engine = (gpointer) audio_engine;

    ns_error = NULL;
  
  [audio_engine enableManualRenderingMode:AVAudioEngineManualRenderingModeOffline
   format:av_format
   maximumFrameCount:AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE
   error:&ns_error];

  if(ns_error != NULL &&
     [ns_error code] != noErr){
    g_warning("enable manual rendering mode error - %d", [ns_error code]);
  }

  /* av audio unit */
  av_audio_unit = (AVAudioUnit *) ags_base_plugin_instantiate((AgsBasePlugin *) audio_unit_plugin,
							      samplerate,
							      buffer_size);

  //  [[av_audio_unit AUAudioUnit] maximumFramesToRender:AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE];
  
  fx_audio_unit_audio->av_audio_unit = (gpointer) av_audio_unit;

  /* output node */
  av_output_node = [audio_engine outputNode];

  /* input node */
  av_input_node = [audio_engine inputNode];

  /* mixer node */
  av_audio_mixer_node = [audio_engine mainMixerNode];
  
  /* audio unit */
  [audio_engine attachNode:av_audio_unit];

  if(!ags_base_plugin_test_flags((AgsBasePlugin *) audio_unit_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    [audio_engine connect:av_input_node to:av_audio_unit format:av_format];
  }
  
  [audio_engine connect:av_audio_unit to:av_audio_mixer_node format:av_format];
  [audio_engine connect:av_audio_mixer_node to:av_output_node format:av_format];
  
  /* audio sequencer */
  av_audio_sequencer = [[AVAudioSequencer alloc] initWithAudioEngine:audio_engine];
  
  fx_audio_unit_audio->av_audio_sequencer = (gpointer) av_audio_sequencer;
  
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
  
  g_object_get(port,
	       "plugin-port", &plugin_port,
	       NULL);

  if(plugin_port == NULL){
    return;
  }
  
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
	
	[parameter_arr[i] setValue:val originator:fx_audio_unit_audio];
      }
    }
  }
  
  if(plugin_port != NULL){
    g_object_unref(plugin_port);
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
  
  plugin_port = start_plugin_port;
  
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
ags_fx_audio_unit_audio_render_thread_loop_mono(gpointer data)
{
  AgsAudio *audio;
  AgsChannel *start_input, *input;
  AgsAudioSignal *audio_signal;
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  
  AgsAudioUnitPlugin *audio_unit_plugin;

  GObject *output_soundcard;

  struct _AgsFxAudioUnitIterateData *active_iterate_data;
  AgsFxAudioUnitAudioScopeData *scope_data;
  AgsFxAudioUnitAudioChannelData *channel_data;
  
  AVAudioEngine *audio_engine;
  AVAudioOutputNode *av_output_node;
  AVAudioInputNode *av_input_node;  
  AVAudioUnit *av_audio_unit;
  AVAudioSequencer *av_audio_sequencer;
  AVMusicTrack *av_music_track;
  
  AVAudioFormat *av_format;
  AVAudioFormat *av_input_format;
  AVAudioPCMBuffer *av_output, *av_input;
  
  GList *iterate_data;

  gboolean is_instrument;
  BOOL input_success;
  guint pad;
  guint audio_channel;
  guint sub_block, sub_block_count;
  gdouble bpm;
  guint note_offset;
  gdouble delay_counter;
  gdouble absolute_delay;
  gint64 frame_position;
  guint copy_mode;
  guint audio_channels;
  guint samplerate;
  guint buffer_size;
  AgsSoundcardFormat format;
  gint status;
  gint i;
  guint j;
  guint k;
  gboolean is_running;

  NSError *ns_error;
  
  GRecMutex *recall_mutex;

  fx_audio_unit_audio = (AgsFxAudioUnitAudio *) data;
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  audio = NULL;
  
  output_soundcard = NULL;

  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  g_object_get(fx_audio_unit_audio,
	       "audio", &audio,
	       "output-soundcard", &output_soundcard,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       NULL);

  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  audio_unit_plugin = fx_audio_unit_audio->audio_unit_plugin;
  
  audio_engine = (AVAudioEngine *) fx_audio_unit_audio->audio_engine;

  av_format = (AVAudioFormat *) fx_audio_unit_audio->av_format;
  
  av_output_node = [audio_engine outputNode];

  av_input_node = [audio_engine inputNode];

  av_input_format = [av_input_node inputFormatForBus:0];
  
  av_audio_sequencer = (AVAudioSequencer *) fx_audio_unit_audio->av_audio_sequencer;

  av_audio_unit = (AVAudioUnit *) fx_audio_unit_audio->av_audio_unit;
  
  is_running = fx_audio_unit_audio->render_thread_running;

  g_rec_mutex_unlock(recall_mutex);

  is_instrument = ags_base_plugin_test_flags((AgsBasePlugin *) audio_unit_plugin,
					     AGS_BASE_PLUGIN_IS_INSTRUMENT);
  
  bpm = 120.0;

  /* get note offset */
  if(output_soundcard != NULL){
    bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(output_soundcard));

    absolute_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard));
      
    delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(output_soundcard));
      
    note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
  }
  
  /* sequencer */
  av_audio_sequencer.currentPositionInBeats = (double) note_offset / 4.0; // a 16th

  //FIXME:JK: available since macOS 13.0
  av_music_track = [av_audio_sequencer createAndAppendTrack];

  av_music_track.destinationAudioUnit = av_audio_unit;
  
  av_music_track.offsetTime = 0.0; // 0

  av_music_track.lengthInBeats = 19200.0 * 4.0; // a 16th

  //NOTE:JK: read-only
  //  av_music_track.timeResolution = 64;

  av_music_track.lengthInSeconds = 19200.0 * 4.0 * (60.0 / bpm);
  
  av_music_track.usesAutomatedParameters = NO;
  
  av_music_track.muted = NO;
  
  scope_data = NULL;

  sub_block = 0;
  
  if(!is_instrument){
    input_success = [av_input_node setManualRenderingInputPCMFormat:av_format
		     inputBlock:^(AVAudioFrameCount inNumberOfFrames){
	AudioBufferList *audio_buffer_list;

	guint audio_channels;
	guint i;

	audio_channels = [av_format channelCount];

	audio_buffer_list = NULL;
				
	/* fill av input buffer */
	if(ags_fx_audio_unit_iterate_channel_data != NULL){
	  audio_buffer_list = (AudioBufferList *) ags_fx_audio_unit_iterate_channel_data->audio_buffer_list;
	
	  if(AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE <= inNumberOfFrames){
	    ags_audio_buffer_util_clear_buffer(NULL,
					       audio_buffer_list->mBuffers[0].mData, 1,
					       AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE, AGS_AUDIO_BUFFER_UTIL_FLOAT);
	  
	    ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
							audio_buffer_list->mBuffers[0].mData, 1, 0,
							ags_fx_audio_unit_iterate_channel_data->input + (ags_fx_audio_unit_iterate_sub_block * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE), 1, 0,
							AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE, AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT);
	  }else{
	    ags_audio_buffer_util_clear_buffer(NULL,
					       audio_buffer_list->mBuffers[0].mData, 1,
					       inNumberOfFrames, AGS_AUDIO_BUFFER_UTIL_FLOAT);
	  
	    ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
							audio_buffer_list->mBuffers[0].mData, 1, 0,
							ags_fx_audio_unit_iterate_channel_data->input + (ags_fx_audio_unit_iterate_sub_block * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE), 1, 0,
							inNumberOfFrames, AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT);
	  }
	}
      
	return((const AudioBufferList *) audio_buffer_list);
      }];

    if(input_success != YES){
      g_warning("set manual rendering input failed");
    }
  }
  
  /* audio engine */
  ns_error = NULL;

  [audio_engine prepare];
  [audio_engine startAndReturnError:&ns_error];

  if(ns_error != NULL &&
     [ns_error code] != noErr){
    g_warning("error during audio engine start - %d", [ns_error code]);
  }
  
  [av_audio_sequencer prepareToPlay];

  ns_error = NULL;
  
  [av_audio_sequencer startAndReturnError:&ns_error];

  if(ns_error != NULL &&
     [ns_error code] != noErr){
    g_warning("error during audio sequencer start - %d", [ns_error code]);
  }

  while(is_running){
    audio_engine = (AVAudioEngine *) fx_audio_unit_audio->audio_engine;
    
    audio_channels = ags_audio_get_audio_channels(audio);
    
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

    /* pre sync */
    g_mutex_lock(&(fx_audio_unit_audio->pre_sync_mutex));

    if(ags_atomic_int_get(&(fx_audio_unit_audio->active_iteration_count)) <= 0){
      ags_atomic_boolean_set(&(fx_audio_unit_audio->pre_sync_wait),
			     TRUE);
      
      while(ags_atomic_boolean_get(&(fx_audio_unit_audio->pre_sync_wait)) &&
	    ags_atomic_int_get(&(fx_audio_unit_audio->active_iteration_count)) <= 0){	
	  g_cond_wait(&(fx_audio_unit_audio->pre_sync_cond),
		      &(fx_audio_unit_audio->pre_sync_mutex));
      }

      ags_atomic_int_decrement(&(fx_audio_unit_audio->active_iteration_count));
    }
    
    g_mutex_unlock(&(fx_audio_unit_audio->pre_sync_mutex));

    /* get note offset */
    if(output_soundcard != NULL){
      bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(output_soundcard));

      absolute_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard));
      
      delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(output_soundcard));
      
      note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
    }
    
    /*  */
    g_rec_mutex_lock(recall_mutex);

    is_running = fx_audio_unit_audio->render_thread_running;

    g_rec_mutex_unlock(recall_mutex);

    if(!is_running){
      break;
    }
    
    /* active audio signal */
    active_iterate_data = NULL;
    
    ags_atomic_pointer_set(&(fx_audio_unit_audio->active_iterate_data),
			   NULL);
    
    if((iterate_data = ags_atomic_pointer_get(&(fx_audio_unit_audio->iterate_data))) != NULL){
      active_iterate_data = iterate_data->data;

      /* remove from iteration audio signal */
      ags_atomic_pointer_set(&(fx_audio_unit_audio->iterate_data),
			     g_list_remove(iterate_data,
					   active_iterate_data));

      /* set active audio signal */
      ags_atomic_pointer_set(&(fx_audio_unit_audio->active_iterate_data),
			     active_iterate_data);
    }

    /* prepare iteration */
    if(active_iterate_data != NULL){
      if(ags_atomic_boolean_get(&(fx_audio_unit_audio->prepare_iteration_wait))){
	g_cond_broadcast(&(fx_audio_unit_audio->prepare_iteration_cond));
      }  
    }

    /* process audio unit */
    if(active_iterate_data != NULL){
      audio_signal = active_iterate_data->audio_signal;
      
      channel_data = 
	ags_fx_audio_unit_iterate_channel_data = active_iterate_data->channel_data;
      
      scope_data =
	ags_fx_audio_unit_iterate_scope_data = (AgsFxAudioUnitAudioScopeData *) channel_data->parent;

      av_audio_sequencer = (AVAudioSequencer *) fx_audio_unit_audio->av_audio_sequencer;

      av_output = (AVAudioPCMBuffer *) channel_data->av_output;
      av_input = (AVAudioPCMBuffer *) channel_data->av_input;

      samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

      buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

      g_object_get(fx_audio_unit_audio,
		   "samplerate", &samplerate,
		   "buffer-size", &buffer_size,
		   NULL);

      pad = active_iterate_data->pad;
      audio_channel = active_iterate_data->audio_channel;

      if(is_instrument){
	/* set position */
	double current_position_in_beats = ((double) note_offset / 4.0) + (delay_counter / absolute_delay / 4.0);
	  
	av_audio_sequencer.currentPositionInBeats = current_position_in_beats;

	/* set bpm */
	AVMusicTrack *tempo_track = [av_audio_sequencer tempoTrack];

	AVBeatRange beat_range = (AVBeatRange) {
	  .start = 0.0,
	  .length = 1.0,
	};
	  
	[tempo_track enumerateEventsInRange:beat_range usingBlock:^(AVMusicEvent *event, AVMusicTimeStamp *timeStamp, BOOL *removeEvent){
	    *removeEvent = YES;
	  }];

	AVExtendedTempoEvent *tempo_event = [[AVExtendedTempoEvent alloc] initWithTempo:bpm];

	double current_beat = (double) note_offset / 4.0;
	  
	[tempo_track addEvent:tempo_event atBeat:current_beat];
      
	/* schedule input buffer */
	if(av_music_track != NULL){
	  [av_music_track enumerateEventsInRange:beat_range usingBlock:^(AVMusicEvent *event, AVMusicTimeStamp *timeStamp, BOOL *removeEvent){
	      *removeEvent = YES;
	    }];
	}
	    
	if(channel_data->input_data[pad]->key_on > 0){
	  AVMIDINoteEvent *av_midi_note_event;

	  GList *note;

	  note = channel_data->input_data[pad]->note;

	  if(note != NULL){	      
	    while(note != NULL){
	      guint x0_256th, x1_256th;
	    
	      x0_256th = ags_note_get_x0_256th(note->data);
	      x1_256th = ags_note_get_x1_256th(note->data);
		
	      double note_event_duration = ((double) (x1_256th - x0_256th) / 64.0);
		
	      av_midi_note_event = [[AVMIDINoteEvent alloc] initWithChannel:0 key:pad velocity:127 duration:note_event_duration];

	      double note_event_current_beat = ((double) x0_256th / 16.0) / 4.0;
		
	      [av_music_track addEvent:av_midi_note_event atBeat:note_event_current_beat];

	      /* iterate */
	      note = note->next;
	    }
	  }
	}	  
      }
      
      ags_audio_buffer_util_clear_buffer(NULL,
					 channel_data->input, 1,
					 buffer_size, AGS_AUDIO_BUFFER_UTIL_FLOAT);

      /* fill midi */
      sub_block_count = floor(buffer_size / AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE);

      for(sub_block = 0; sub_block < sub_block_count; sub_block++){
	/* clear av IO buffer */
	//	ags_audio_buffer_util_clear_buffer(NULL,
	//				   [av_input floatChannelData][0], 1,
	//				   AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE, AGS_AUDIO_BUFFER_UTIL_FLOAT);
	
	ags_audio_buffer_util_clear_buffer(NULL,
					   [av_output floatChannelData][0], 1,
					   AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE, AGS_AUDIO_BUFFER_UTIL_FLOAT);

	//	g_message("schedule buffer and render audio unit");
	
	ags_fx_audio_unit_iterate_sub_block = sub_block;
	    
	/* render */
	ns_error = NULL;
	  
	status = [audio_engine renderOffline:AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE toBuffer:av_output error:&ns_error];
    
	if(ns_error != NULL &&
	   [ns_error code] != noErr){
	  g_warning("render offline error - %d", [ns_error code]);
	}

	switch(status){
	case AVAudioEngineManualRenderingStatusSuccess:
	  {
	    //NOTE:JK: this is fine
	    //	    g_message("OK");
	  }
	  break;
	case AVAudioEngineManualRenderingStatusInsufficientDataFromInputNode:
	  {
	    g_message("insufficient data from input");
	  }
	  break;
	case AVAudioEngineManualRenderingStatusCannotDoInCurrentContext:
	  {
	    g_message("cannot do in current context");
	  }
	  break;
	case AVAudioEngineManualRenderingStatusError:
	  {
	    g_message("error");
	  }
	  break;
	}
	    
	/* fill output buffer */
	ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						    channel_data->output + (sub_block * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE), 1, 0,
						    [av_output floatChannelData][0], 1, 0,
						    AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE, AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT);
      }

      buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
      format = AGS_SOUNDCARD_DEFAULT_FORMAT;
	    
      g_object_get(audio_signal,
		   "buffer-size", &buffer_size,
		   "format", &format,
		   NULL);
      
      copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(NULL,
								  ags_audio_buffer_util_format_from_soundcard(NULL, format),
								  AGS_AUDIO_BUFFER_UTIL_FLOAT);
		
      //	      g_message("out");
		
      /* fill output buffer */
      ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						  audio_signal->stream->data, 1, 0,
						  channel_data->output, 1, 0,
						  buffer_size, copy_mode);

      ags_audio_buffer_util_clear_buffer(NULL,
					 channel_data->output, 1,
					 buffer_size, AGS_AUDIO_BUFFER_UTIL_FLOAT);

      // [[av_audio_unit AUAudioUnit] reset];
    }
    
    /* iteration completed */
    ags_atomic_boolean_set(&(fx_audio_unit_audio->completed_iteration_done),
			   TRUE);
    
    if(ags_atomic_boolean_get(&(fx_audio_unit_audio->completed_iteration_wait))){
      g_cond_signal(&(fx_audio_unit_audio->completed_iteration_cond));
    }

    if(active_iterate_data == NULL){
      g_thread_yield();
    }
        
    g_free(active_iterate_data);
    
    /* check is running */
    g_rec_mutex_lock(recall_mutex);

    is_running = fx_audio_unit_audio->render_thread_running;

    g_rec_mutex_unlock(recall_mutex);
  }
  
  g_thread_exit(NULL);
  
  return(NULL);
}

gboolean
ags_fx_audio_unit_audio_is_stereo_ready(AgsFxAudioUnitAudio *fx_audio_unit_audio)
{
  AgsAudio *audio;
  AgsAudioUnitPlugin *audio_unit_plugin;
  
  struct _AgsFxAudioUnitIterateData *current_iterate_data;

  GList *start_iterate_data, *iterate_data;
  
  gint sound_scope;
  guint audio_channels;
  guint input_pads;
  guint pad;
  guint audio_channel;
  guint line;
  gboolean is_instrument;
  gboolean is_stereo_ready;

  GRecMutex *recall_mutex;
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  /* get audio unit plugin */
  audio = ags_recall_audio_get_audio((AgsRecallAudio *) fx_audio_unit_audio);
  
  g_rec_mutex_lock(recall_mutex);

  audio_unit_plugin = fx_audio_unit_audio->audio_unit_plugin;

  start_iterate_data = g_list_copy(fx_audio_unit_audio->iterate_data);

  g_rec_mutex_unlock(recall_mutex);
  
  audio_channels = ags_audio_get_audio_channels(audio);
  input_pads = ags_audio_get_input_pads(audio);

  is_instrument = ags_base_plugin_test_flags((AgsBasePlugin *) audio_unit_plugin,
					     AGS_BASE_PLUGIN_IS_INSTRUMENT);
  
  is_stereo_ready = FALSE;

  g_rec_mutex_lock(recall_mutex);

  if(is_instrument){
    iterate_data = start_iterate_data;

    while(iterate_data != NULL){
      GList *tmp_iterate_data;

      guint key_on_count, active_key_on_count;
      
      current_iterate_data = iterate_data->data;

      pad = current_iterate_data->pad;
      
      sound_scope = current_iterate_data->sound_scope;

      /* count key on */
      key_on_count = 0;
      
      for(audio_channel = 0; audio_channel < audio_channels; audio_channel++){
	AgsFxAudioUnitAudioInputData *input_data;

	input_data = fx_audio_unit_audio->scope_data[sound_scope]->channel_data[audio_channel]->input_data[pad];

	key_on_count += input_data->key_on;
      }

      /* check iterate data and match key on count */
      tmp_iterate_data = iterate_data->next;
      
      active_key_on_count = 1;

      while(tmp_iterate_data != NULL){
	struct _AgsFxAudioUnitIterateData *tmp_current_iterate_data;

	tmp_current_iterate_data = tmp_iterate_data->data;
	
	if(tmp_current_iterate_data->pad == pad &&
	   tmp_current_iterate_data->sound_scope == sound_scope){
	  active_key_on_count++;

	  /* remove already checked data */
	  start_iterate_data = g_list_remove(start_iterate_data,
					     tmp_current_iterate_data);
	}
      }

      if(active_key_on_count >= key_on_count){
	is_stereo_ready = TRUE;

	break;
      }
      
      /* iterate */
      iterate_data = iterate_data->next;
    }
  }else{
    iterate_data = start_iterate_data;

    while(iterate_data != NULL){
      guint active_audio_signal_count;
      guint queued_audio_signal_count;
      
      current_iterate_data = iterate_data->data;
      
      sound_scope = current_iterate_data->sound_scope;

      /* count active and queued audio signal */
      active_audio_signal_count = 0;
      queued_audio_signal_count = 0;

      for(line = 0; line < audio_channels * input_pads; line++){
	AgsFxAudioUnitAudioChannelData *channel_data;

	channel_data = fx_audio_unit_audio->scope_data[sound_scope]->channel_data[line % audio_channels];

	active_audio_signal_count += ags_atomic_int_get(&(channel_data->active_audio_signal));
	queued_audio_signal_count += ags_atomic_int_get(&(channel_data->queued_audio_signal));
      }

      /* check iterate data and match queued audio signal count */
      if(queued_audio_signal_count >= active_audio_signal_count){
	is_stereo_ready = TRUE;

	break;
      }
       
      /* iterate */
      iterate_data = iterate_data->next;
    }
  }

  g_rec_mutex_unlock(recall_mutex);
  
  g_list_free(start_iterate_data);
  
  return(is_stereo_ready);
}

GList*
ags_fx_audio_unit_audio_pull_stereo_iterate_data(AgsFxAudioUnitAudio *fx_audio_unit_audio)
{
  AgsAudio *audio;
  AgsAudioUnitPlugin *audio_unit_plugin;

  struct _AgsFxAudioUnitIterateData *matched_iterate_data;
  struct _AgsFxAudioUnitIterateData *check_iterate_data;
  struct _AgsFxAudioUnitIterateData *current_iterate_data;
  
  GList *start_iterate_data, *iterate_data;
  GList *retval;

  gboolean is_instrument;
  gint sound_scope;
  guint audio_channels;
  guint input_pads;
  guint pad;
  guint audio_channel;
  guint line;
  
  GRecMutex *recall_mutex;
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  /* get audio unit plugin */
  audio = ags_recall_audio_get_audio((AgsRecallAudio *) fx_audio_unit_audio);
  
  g_rec_mutex_lock(recall_mutex);

  audio_unit_plugin = fx_audio_unit_audio->audio_unit_plugin;

  matched_iterate_data = NULL;

  check_iterate_data = NULL;
  
  current_iterate_data = NULL;
  
  start_iterate_data = g_list_copy(fx_audio_unit_audio->iterate_data);

  g_rec_mutex_unlock(recall_mutex);
  
  audio_channels = ags_audio_get_audio_channels(audio);
  input_pads = ags_audio_get_input_pads(audio);
  
  matched_iterate_data = NULL;
  
  retval = NULL;

  is_instrument = ags_base_plugin_test_flags((AgsBasePlugin *) audio_unit_plugin,
					     AGS_BASE_PLUGIN_IS_INSTRUMENT);

  g_rec_mutex_lock(recall_mutex);
  
  if(is_instrument){
    iterate_data = start_iterate_data;

    while(iterate_data != NULL){
      GList *tmp_iterate_data;

      guint key_on_count, active_key_on_count;
      
      current_iterate_data = iterate_data->data;

      if(check_iterate_data != NULL){
	if(check_iterate_data->sound_scope != current_iterate_data->sound_scope ||
	   check_iterate_data->pad != current_iterate_data->pad){
	  check_iterate_data = current_iterate_data;
	}
      }else{
	check_iterate_data = current_iterate_data;
      }
      
      pad = current_iterate_data->pad;
      
      sound_scope = current_iterate_data->sound_scope;

      /* count key on */
      key_on_count = 0;
      
      for(audio_channel = 0; audio_channel < audio_channels; audio_channel++){
	AgsFxAudioUnitAudioInputData *input_data;

	input_data = fx_audio_unit_audio->scope_data[sound_scope]->channel_data[audio_channel]->input_data[pad];

	key_on_count += input_data->key_on;
      }

      /* check iterate data and match key on count */
      tmp_iterate_data = iterate_data->next;
      
      active_key_on_count = 1;

      while(tmp_iterate_data != NULL){
	struct _AgsFxAudioUnitIterateData *tmp_current_iterate_data;

	tmp_current_iterate_data = tmp_iterate_data->data;
	
	if(tmp_current_iterate_data->pad == pad &&
	   tmp_current_iterate_data->sound_scope == sound_scope){
	  active_key_on_count++;

	  /* remove already checked data */
	  start_iterate_data = g_list_remove(start_iterate_data,
					     tmp_current_iterate_data);
	}
      }

      if(active_key_on_count >= key_on_count){
	matched_iterate_data = check_iterate_data;

	break;
      }
      
      /* iterate */
      iterate_data = iterate_data->next;
    }
  }else{
    iterate_data = start_iterate_data;

    while(iterate_data != NULL){
      guint active_audio_signal_count;
      guint queued_audio_signal_count;
      
      current_iterate_data = iterate_data->data;

      if(check_iterate_data != NULL){
	if(check_iterate_data->sound_scope != current_iterate_data->sound_scope){
	  check_iterate_data = current_iterate_data;
	}
      }else{
	check_iterate_data = current_iterate_data;
      }
      
      sound_scope = current_iterate_data->sound_scope;

      /* count active and queued audio signal */
      active_audio_signal_count = 0;
      queued_audio_signal_count = 0;

      for(line = 0; line < audio_channels * input_pads; line++){
	AgsFxAudioUnitAudioChannelData *channel_data;

	channel_data = fx_audio_unit_audio->scope_data[sound_scope]->channel_data[line % audio_channels];

	active_audio_signal_count += ags_atomic_int_get(&(channel_data->active_audio_signal));
	queued_audio_signal_count += ags_atomic_int_get(&(channel_data->queued_audio_signal));
      }

      /* check iterate data and match queued audio signal count */
      if(queued_audio_signal_count >= active_audio_signal_count){
	matched_iterate_data = check_iterate_data;

	break;
      }
       
      /* iterate */
      iterate_data = iterate_data->next;
    }
  }

  /* set retval */
  if(matched_iterate_data != NULL){
    iterate_data = start_iterate_data;

    while(iterate_data != NULL){
      current_iterate_data = iterate_data->data;

      if(is_instrument){
	if(current_iterate_data->sound_scope == matched_iterate_data->sound_scope &&
	   current_iterate_data->pad == matched_iterate_data->pad){
	  retval = g_list_prepend(retval,
				  current_iterate_data);
	  
	  ags_atomic_int_decrement(&(fx_audio_unit_audio->active_iteration_count));
	}
      }else{
	if(current_iterate_data->sound_scope == matched_iterate_data->sound_scope){
	  retval = g_list_prepend(retval,
				  current_iterate_data);
	  
	  ags_atomic_int_decrement(&(fx_audio_unit_audio->active_iteration_count));
	}
      }
      
      /* iterate */
      iterate_data = iterate_data->next;
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
  
  g_list_free(start_iterate_data);
  
  return(retval);
}

void*
ags_fx_audio_unit_audio_render_thread_loop_stereo(gpointer data)
{
  AgsAudio *audio;
  AgsChannel *start_input, *input;
  AgsAudioSignal *audio_signal;
  AgsFxAudioUnitAudio *fx_audio_unit_audio;
  
  AgsAudioUnitPlugin *audio_unit_plugin;

  GObject *output_soundcard;

  struct _AgsFxAudioUnitIterateData *active_iterate_data;
  AgsFxAudioUnitAudioScopeData *scope_data;
  AgsFxAudioUnitAudioChannelData *channel_data;
  
  AVAudioEngine *audio_engine;
  AVAudioOutputNode *av_output_node;
  AVAudioInputNode *av_input_node;  
  AVAudioUnit *av_audio_unit;
  AVAudioSequencer *av_audio_sequencer;
  AVMusicTrack *av_music_track;
  
  AVAudioFormat *av_format;
  AVAudioFormat *av_input_format;
  AVAudioPCMBuffer *av_output, *av_input;
  
  GList *start_iterate_data, *iterate_data;

  gboolean is_instrument;
  BOOL input_success;
  guint pad;
  guint audio_channel;
  guint sub_block, sub_block_count;
  gdouble bpm;
  guint note_offset;
  gdouble delay_counter;
  gdouble absolute_delay;
  gint64 frame_position;
  guint copy_mode;
  guint audio_channels;
  guint samplerate;
  guint buffer_size;
  AgsSoundcardFormat format;
  gint status;
  gint i;
  guint j;
  guint k;
  gboolean is_running;

  NSError *ns_error;
  
  GRecMutex *recall_mutex;

  fx_audio_unit_audio = (AgsFxAudioUnitAudio *) data;
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  audio = NULL;

  output_soundcard = NULL;

  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  g_object_get(fx_audio_unit_audio,
	       "audio", &audio,
	       "output-soundcard", &output_soundcard,
	       "samplerate", &samplerate,
	       "buffer-size", &buffer_size,
	       NULL);

  /* get some fields */
  g_rec_mutex_lock(recall_mutex);

  audio_unit_plugin = fx_audio_unit_audio->audio_unit_plugin;
  
  audio_engine = (AVAudioEngine *) fx_audio_unit_audio->audio_engine;

  av_format = (AVAudioFormat *) fx_audio_unit_audio->av_format;
  
  av_output_node = [audio_engine outputNode];

  av_input_node = [audio_engine inputNode];

  av_input_format = [av_input_node inputFormatForBus:0];
  
  av_audio_sequencer = (AVAudioSequencer *) fx_audio_unit_audio->av_audio_sequencer;

  av_audio_unit = (AVAudioUnit *) fx_audio_unit_audio->av_audio_unit;
  
  is_running = fx_audio_unit_audio->render_thread_running;

  g_rec_mutex_unlock(recall_mutex);

  is_instrument = ags_base_plugin_test_flags((AgsBasePlugin *) audio_unit_plugin,
					     AGS_BASE_PLUGIN_IS_INSTRUMENT);

  bpm = 120.0;

  /* get note offset */
  if(output_soundcard != NULL){
    bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(output_soundcard));

    absolute_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard));
      
    delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(output_soundcard));
      
    note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
  }
  
  /* sequencer */
  av_audio_sequencer.currentPositionInBeats = (double) note_offset / 4.0; // a 16th

  //FIXME:JK: available since macOS 13.0
  av_music_track = [av_audio_sequencer createAndAppendTrack];

  av_music_track.destinationAudioUnit = av_audio_unit;

  av_music_track.offsetTime = 0.0; // 0

  av_music_track.lengthInBeats = 19200.0 * 4.0; // a 16th

  //NOTE:JK: read-only
  //  av_music_track.timeResolution = 64;

  av_music_track.lengthInSeconds = 19200.0 * 4.0 * (60.0 / bpm);
  
  av_music_track.usesAutomatedParameters = NO;
  
  av_music_track.muted = NO;
  
  channel_data =
    ags_fx_audio_unit_iterate_channel_data = NULL;
  
  scope_data =
    ags_fx_audio_unit_iterate_scope_data = NULL;

  sub_block = 0;
  
  if(!is_instrument){
    input_success = [av_input_node setManualRenderingInputPCMFormat:av_format
		     inputBlock:^(AVAudioFrameCount inNumberOfFrames){
	AudioBufferList *audio_buffer_list;

	guint audio_channels;
	guint i;

	audio_channels = [av_format channelCount];

	audio_buffer_list = NULL;
				
	/* fill av input buffer */
	if(ags_fx_audio_unit_iterate_scope_data != NULL){
	  audio_buffer_list = (AudioBufferList *) ags_fx_audio_unit_iterate_scope_data->audio_buffer_list;

	  for(i = 0; i < audio_channels; i++){
	    if(AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE <= inNumberOfFrames){
	      ags_audio_buffer_util_clear_buffer(NULL,
						 audio_buffer_list->mBuffers[i].mData, 1,
						 AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE, AGS_AUDIO_BUFFER_UTIL_FLOAT);
	  
	      ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
							  audio_buffer_list->mBuffers[i].mData, 1, 0,
							  ags_fx_audio_unit_iterate_scope_data->input + (audio_channels * ags_fx_audio_unit_iterate_sub_block * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE), audio_channels, i,
							  AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE, AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT);
	    }else{
	      ags_audio_buffer_util_clear_buffer(NULL,
						 audio_buffer_list->mBuffers[i].mData, 1,
						 inNumberOfFrames, AGS_AUDIO_BUFFER_UTIL_FLOAT);
	  
	      ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
							  audio_buffer_list->mBuffers[i].mData, 1, 0,
							  ags_fx_audio_unit_iterate_scope_data->input + (audio_channels * ags_fx_audio_unit_iterate_sub_block * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE), audio_channels, i,
							  inNumberOfFrames, AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT);
	    }
	  }
	}
      
	return((const AudioBufferList *) audio_buffer_list);
      }];

    if(input_success != YES){
      g_warning("set manual rendering input failed");
    }
  }
  
  /* audio engine */
  ns_error = NULL;

  [audio_engine prepare];
  [audio_engine startAndReturnError:&ns_error];

  if(ns_error != NULL &&
     [ns_error code] != noErr){
    g_warning("error during audio engine start - %d", [ns_error code]);
  }
  
  [av_audio_sequencer prepareToPlay];

  ns_error = NULL;
  
  [av_audio_sequencer startAndReturnError:&ns_error];

  if(ns_error != NULL &&
     [ns_error code] != noErr){
    g_warning("error during audio sequencer start - %d", [ns_error code]);
  }

  while(is_running){
    audio_engine = (AVAudioEngine *) fx_audio_unit_audio->audio_engine;
    
    audio_channels = ags_audio_get_audio_channels(audio);
    
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

    /* pre sync */
    g_mutex_lock(&(fx_audio_unit_audio->pre_sync_mutex));

    if(ags_atomic_int_get(&(fx_audio_unit_audio->active_iteration_count)) <= 0 ||
       !ags_fx_audio_unit_audio_is_stereo_ready(fx_audio_unit_audio)){
      ags_atomic_boolean_set(&(fx_audio_unit_audio->pre_sync_wait),
			     TRUE);
      
      while(ags_atomic_boolean_get(&(fx_audio_unit_audio->pre_sync_wait)) &&
	    (ags_atomic_int_get(&(fx_audio_unit_audio->active_iteration_count)) <= 0 ||
	     !ags_fx_audio_unit_audio_is_stereo_ready(fx_audio_unit_audio))){
	  g_cond_wait(&(fx_audio_unit_audio->pre_sync_cond),
		      &(fx_audio_unit_audio->pre_sync_mutex));
      }
    }
    
    iterate_data = 
      start_iterate_data = ags_fx_audio_unit_audio_pull_stereo_iterate_data(fx_audio_unit_audio);
    
    g_mutex_unlock(&(fx_audio_unit_audio->pre_sync_mutex));
    
    /* get note offset */
    if(output_soundcard != NULL){
      bpm = ags_soundcard_get_bpm(AGS_SOUNDCARD(output_soundcard));

      absolute_delay = ags_soundcard_get_absolute_delay(AGS_SOUNDCARD(output_soundcard));
      
      delay_counter = ags_soundcard_get_delay_counter(AGS_SOUNDCARD(output_soundcard));
      
      note_offset = ags_soundcard_get_note_offset(AGS_SOUNDCARD(output_soundcard));
    }

    /*  */
    g_rec_mutex_lock(recall_mutex);

    is_running = fx_audio_unit_audio->render_thread_running;

    g_rec_mutex_unlock(recall_mutex);

    if(!is_running){
      break;
    }

    /* active audio signal */
    active_iterate_data = NULL;
    
    ags_atomic_pointer_set(&(fx_audio_unit_audio->active_iterate_data),
			   NULL);

    iterate_data = start_iterate_data;
    
    while(iterate_data != NULL){
      active_iterate_data = iterate_data->data;
      
      /* remove from iteration audio signal */
      ags_atomic_pointer_set(&(fx_audio_unit_audio->iterate_data),
			     g_list_remove(ags_atomic_pointer_get(&(fx_audio_unit_audio->iterate_data)),
					   active_iterate_data));

      /* set active audio signal */
      ags_atomic_pointer_set(&(fx_audio_unit_audio->active_iterate_data),
			     g_list_prepend(ags_atomic_pointer_get(&(fx_audio_unit_audio->active_iterate_data)),
					    active_iterate_data));

      /* iterate */
      iterate_data = iterate_data->next;
    }

    /* prepare iteration */
    if(start_iterate_data != NULL){
      if(ags_atomic_boolean_get(&(fx_audio_unit_audio->prepare_iteration_wait))){
	g_cond_broadcast(&(fx_audio_unit_audio->prepare_iteration_cond));
      }
    }

    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

    buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;

    g_object_get(fx_audio_unit_audio,
		 "samplerate", &samplerate,
		 "buffer-size", &buffer_size,
		 NULL);

    iterate_data = start_iterate_data;
    
    while(iterate_data != NULL){
      GList *list;
      
      gint audio_signal_count;
      
      active_iterate_data = iterate_data->data;

      audio_signal = active_iterate_data->audio_signal;
      
      channel_data = 
	ags_fx_audio_unit_iterate_channel_data = active_iterate_data->channel_data;
      
      scope_data =
	ags_fx_audio_unit_iterate_scope_data = (AgsFxAudioUnitAudioScopeData *) channel_data->parent;
      
      av_output = (AVAudioPCMBuffer *) scope_data->av_output;
      av_input = (AVAudioPCMBuffer *) scope_data->av_input;    
      
      av_audio_sequencer = (AVAudioSequencer *) fx_audio_unit_audio->av_audio_sequencer;

      pad = active_iterate_data->pad;
      audio_channel = active_iterate_data->audio_channel;

      if(is_instrument){
	/* set position */
	double current_position_in_beats = ((double) note_offset / 4.0) + (delay_counter / absolute_delay / 4.0);
	  
	av_audio_sequencer.currentPositionInBeats = current_position_in_beats;

	/* set bpm */
	AVMusicTrack *tempo_track = [av_audio_sequencer tempoTrack];

	AVBeatRange beat_range = (AVBeatRange) {
	  .start = 0.0,
	  .length = 1.0,
	};
	  
	[tempo_track enumerateEventsInRange:beat_range usingBlock:^(AVMusicEvent *event, AVMusicTimeStamp *timeStamp, BOOL *removeEvent){
	    *removeEvent = YES;
	  }];

	AVExtendedTempoEvent *tempo_event = [[AVExtendedTempoEvent alloc] initWithTempo:bpm];

	double current_beat = (double) note_offset / 4.0;
	  
	[tempo_track addEvent:tempo_event atBeat:current_beat];
      
	/* schedule input buffer */
	if(av_music_track != NULL){
	  [av_music_track enumerateEventsInRange:beat_range usingBlock:^(AVMusicEvent *event, AVMusicTimeStamp *timeStamp, BOOL *removeEvent){
	      *removeEvent = YES;
	    }];
	}
	    
	if(channel_data->input_data[pad]->key_on > 0){
	  AVMIDINoteEvent *av_midi_note_event;

	  GList *note;

	  note = channel_data->input_data[pad]->note;

	  if(note != NULL){	      
	    while(note != NULL){
	      guint x0_256th, x1_256th;
	    
	      x0_256th = ags_note_get_x0_256th(note->data);
	      x1_256th = ags_note_get_x1_256th(note->data);
		
	      double note_event_duration = ((double) (x1_256th - x0_256th) / 64.0);
		
	      av_midi_note_event = [[AVMIDINoteEvent alloc] initWithChannel:0 key:pad velocity:127 duration:note_event_duration];

	      double note_event_current_beat = ((double) x0_256th / 16.0) / 4.0;
		
	      [av_music_track addEvent:av_midi_note_event atBeat:note_event_current_beat];

	      /* iterate */
	      note = note->next;
	    }
	  }
	}	  
      }
      
      ags_audio_buffer_util_clear_buffer(NULL,
					 scope_data->input, 1,
					 audio_channels * buffer_size, AGS_AUDIO_BUFFER_UTIL_FLOAT);

      if(!is_instrument){
	copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(NULL,
								    AGS_AUDIO_BUFFER_UTIL_FLOAT,
								    ags_audio_buffer_util_format_from_soundcard(NULL, format));
	
	ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						    scope_data->input, audio_channels, audio_channel,
						    audio_signal->stream->data, 1, 0,
						    buffer_size, copy_mode);
      }

      ags_audio_buffer_util_clear_buffer(NULL,
					 scope_data->output, 1,
					 audio_channels * buffer_size, AGS_AUDIO_BUFFER_UTIL_FLOAT);

      /* iterate */
      iterate_data = iterate_data->next;
    }
    
    /* fill midi */
    if(start_iterate_data != NULL){
      active_iterate_data = start_iterate_data->data;

      channel_data = 
	ags_fx_audio_unit_iterate_channel_data = active_iterate_data->channel_data;
      
      scope_data =
	ags_fx_audio_unit_iterate_scope_data = (AgsFxAudioUnitAudioScopeData *) channel_data->parent;
      
      av_output = (AVAudioPCMBuffer *) scope_data->av_output;
      av_input = (AVAudioPCMBuffer *) scope_data->av_input;
      
      sub_block_count = floor(buffer_size / AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE);

      for(sub_block = 0; sub_block < sub_block_count; sub_block++){
	guint i;
	
	/* clear av IO buffer */
	//	ags_audio_buffer_util_clear_buffer(NULL,
	//				   [av_input floatChannelData][0], 1,
	//				   AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE, AGS_AUDIO_BUFFER_UTIL_FLOAT);
	
	for(i = 0; i < audio_channels; i++){
	  ags_audio_buffer_util_clear_buffer(NULL,
					     [av_output floatChannelData][i], 1,
					     AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE, AGS_AUDIO_BUFFER_UTIL_FLOAT);
	}
	
	//	g_message("schedule buffer and render audio unit");
	
	ags_fx_audio_unit_iterate_sub_block = sub_block;
	    
	/* render */
	ns_error = NULL;
	  
	status = [audio_engine renderOffline:AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE toBuffer:av_output error:&ns_error];
    
	if(ns_error != NULL &&
	   [ns_error code] != noErr){
	  g_warning("render offline error - %d", [ns_error code]);
	}

	switch(status){
	case AVAudioEngineManualRenderingStatusSuccess:
	  {
	    //NOTE:JK: this is fine
	    //	    g_message("OK");
	  }
	  break;
	case AVAudioEngineManualRenderingStatusInsufficientDataFromInputNode:
	  {
	    g_message("insufficient data from input");
	  }
	  break;
	case AVAudioEngineManualRenderingStatusCannotDoInCurrentContext:
	  {
	    g_message("cannot do in current context");
	  }
	  break;
	case AVAudioEngineManualRenderingStatusError:
	  {
	    g_message("error");
	  }
	  break;
	}
	    
	/* fill output buffer */
	for(i = 0; i < audio_channels; i++){
	  ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						      scope_data->output + (audio_channels * sub_block * AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE), audio_channels, i,
						      [av_output floatChannelData][i], 1, 0,
						      AGS_FX_AUDIO_UNIT_AUDIO_FIXED_BUFFER_SIZE, AGS_AUDIO_BUFFER_UTIL_COPY_FLOAT_TO_FLOAT);
	}
      }
    }
    
    iterate_data = start_iterate_data;
    
    while(iterate_data != NULL){
      GList *list;
      
      gint audio_signal_count;
      
      active_iterate_data = iterate_data->data;

      audio_signal = active_iterate_data->audio_signal;
      
      channel_data = 
	ags_fx_audio_unit_iterate_channel_data = active_iterate_data->channel_data;
      
      scope_data =
	ags_fx_audio_unit_iterate_scope_data = (AgsFxAudioUnitAudioScopeData *) channel_data->parent;
      
      av_output = (AVAudioPCMBuffer *) scope_data->av_output;
      av_input = (AVAudioPCMBuffer *) scope_data->av_input;    
      
      av_audio_sequencer = (AVAudioSequencer *) fx_audio_unit_audio->av_audio_sequencer;

      pad = active_iterate_data->pad;
      audio_channel = active_iterate_data->audio_channel;
      
      buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
      format = AGS_SOUNDCARD_DEFAULT_FORMAT;
	    
      g_object_get(audio_signal,
		   "buffer-size", &buffer_size,
		   "format", &format,
		   NULL);
      
      copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(NULL,
								  ags_audio_buffer_util_format_from_soundcard(NULL, format),
								  AGS_AUDIO_BUFFER_UTIL_FLOAT);
		
      //	      g_message("out");
		
      /* fill output buffer */
      list = start_iterate_data;

      audio_signal_count = 0;
      
      while(list != NULL){
	struct _AgsFxAudioUnitIterateData *tmp_iterate_data;

	tmp_iterate_data = list->data;
	
	if(tmp_iterate_data->pad == pad &&
	   tmp_iterate_data->audio_channel == audio_channel){
	  audio_signal_count++;
	}

	list = list->next;
      }

      
      ags_audio_buffer_util_clear_buffer(NULL,
					 audio_signal->stream->data, 1,
					 buffer_size, ags_audio_buffer_util_format_from_soundcard(NULL, format));
      
      ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						  audio_signal->stream->data, 1, 0,
						  scope_data->output, audio_channels, audio_channel,
						  buffer_size, copy_mode);

      scope_data->volume_util->destination = audio_signal->stream->data;
      
      scope_data->volume_util->source = audio_signal->stream->data;

      scope_data->volume_util->buffer_length = buffer_size;
            
      scope_data->volume_util->format = format;

      if(audio_signal_count > 0){
	scope_data->volume_util->volume = 1.0 / (double) audio_signal_count;
      }else{
	scope_data->volume_util->volume = 1.0;
      }
      
      ags_volume_util_compute(scope_data->volume_util);
      
      /* iterate */
      iterate_data = iterate_data->next;
    }
    
    // [[av_audio_unit AUAudioUnit] reset];
    
    /* iteration completed */
    ags_atomic_boolean_set(&(fx_audio_unit_audio->completed_iteration_done),
			   TRUE);
    
    if(ags_atomic_boolean_get(&(fx_audio_unit_audio->completed_iteration_wait))){
      g_cond_signal(&(fx_audio_unit_audio->completed_iteration_cond));
    }

    if(start_iterate_data == NULL){
      g_thread_yield();
    }
    
    g_list_free_full(start_iterate_data,
		     (GDestroyNotify) g_free);

    /* check is running */
    g_rec_mutex_lock(recall_mutex);

    is_running = fx_audio_unit_audio->render_thread_running;

    g_rec_mutex_unlock(recall_mutex);
  }

  /* prepare iteration */
  gboolean prepare_iteration_waiting;
  
  g_mutex_lock(&(fx_audio_unit_audio->prepare_iteration_mutex));

  prepare_iteration_waiting = ags_atomic_boolean_get(&(fx_audio_unit_audio->prepare_iteration_wait));
 
  ags_atomic_boolean_set(&(fx_audio_unit_audio->prepare_iteration_wait),
			 FALSE);
  
  if(prepare_iteration_waiting){
    g_cond_broadcast(&(fx_audio_unit_audio->prepare_iteration_cond));
  }

  g_mutex_unlock(&(fx_audio_unit_audio->prepare_iteration_mutex));
    
  /* iteration completed */
  g_mutex_lock(&(fx_audio_unit_audio->completed_iteration_mutex));
  
  ags_atomic_boolean_set(&(fx_audio_unit_audio->completed_iteration_done),
			 TRUE);

  if(ags_atomic_boolean_get(&(fx_audio_unit_audio->completed_iteration_wait))){
    g_cond_broadcast(&(fx_audio_unit_audio->completed_iteration_cond));
  }

  g_mutex_unlock(&(fx_audio_unit_audio->completed_iteration_mutex));

  g_message("AV audio engine stop");
  
  [av_audio_sequencer stop];

  [audio_engine stop];

  ags_atomic_boolean_set(&(fx_audio_unit_audio->prepare_iteration_wait),
			 FALSE);

  ags_atomic_int_set(&(fx_audio_unit_audio->active_iteration_count),
		     0);
  
  ags_atomic_boolean_set(&(fx_audio_unit_audio->prepare_iteration_wait),
			 FALSE);

  ags_atomic_pointer_set(&(fx_audio_unit_audio->iterate_data),
			 NULL);

  ags_atomic_pointer_set(&(fx_audio_unit_audio->active_iterate_data),
			 NULL);

  ags_atomic_boolean_set(&(fx_audio_unit_audio->completed_iteration_wait),
			 FALSE);

  ags_atomic_boolean_set(&(fx_audio_unit_audio->completed_iteration_done),
			 FALSE);
  
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
  GRecMutex *recall_mutex;
  
  if(fx_audio_unit_audio == NULL ||
     !AGS_IS_FX_AUDIO_UNIT_AUDIO(fx_audio_unit_audio)){
    return;
  }

  if(fx_audio_unit_audio->render_thread_running){
    ags_atomic_int_increment(&(fx_audio_unit_audio->render_ref_count));
    
    return;
  }

  g_message("Audio Unit render thread start");
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  ags_atomic_int_set(&(fx_audio_unit_audio->render_ref_count),
		     1);
  
  g_rec_mutex_lock(recall_mutex);

  fx_audio_unit_audio->render_thread_running = TRUE;

  g_rec_mutex_unlock(recall_mutex);
    
  if(ags_fx_audio_unit_audio_test_flags(fx_audio_unit_audio,
					AGS_FX_AUDIO_UNIT_AUDIO_MONO)){
    fx_audio_unit_audio->render_thread = g_thread_new("Advanced Gtk+ Sequencer - Audio Unit",
						      (GThreadFunc) ags_fx_audio_unit_audio_render_thread_loop_mono,
						      (gpointer) fx_audio_unit_audio);
  }else{
    fx_audio_unit_audio->render_thread = g_thread_new("Advanced Gtk+ Sequencer - Audio Unit",
						      (GThreadFunc) ags_fx_audio_unit_audio_render_thread_loop_stereo,
						      (gpointer) fx_audio_unit_audio);
  }
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
  GRecMutex *recall_mutex;

  gboolean is_waiting;
  
  if(fx_audio_unit_audio == NULL ||
     !AGS_IS_FX_AUDIO_UNIT_AUDIO(fx_audio_unit_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  if(ags_atomic_int_get(&(fx_audio_unit_audio->render_ref_count)) > 0){
    ags_atomic_int_decrement(&(fx_audio_unit_audio->render_ref_count));
  }
  
  if(ags_atomic_int_get(&(fx_audio_unit_audio->render_ref_count)) <= 0){
    g_message("Audio Unit render thread stop");
    
    g_rec_mutex_lock(recall_mutex);
    
    fx_audio_unit_audio->render_thread_running = FALSE;

    g_rec_mutex_unlock(recall_mutex);

    /* signal */
    g_mutex_lock(&(fx_audio_unit_audio->pre_sync_mutex));

    is_waiting = ags_atomic_boolean_get(&(fx_audio_unit_audio->pre_sync_wait));

    ags_atomic_boolean_set(&(fx_audio_unit_audio->pre_sync_wait),
			   FALSE);
          
    if(is_waiting){
      g_cond_signal(&(fx_audio_unit_audio->pre_sync_cond));
    }

    g_mutex_unlock(&(fx_audio_unit_audio->pre_sync_mutex));
  }
}

/**
 * ags_fx_audio_unit_audio_render_thread_iteration:
 * @fx_audio_unit_audio: the #AgsFxAudioUnitAudio
 * @audio_signal: the #AgsAudioSignal
 * @pad: the pad
 * @audio_channel: the audio channel
 * @scope: the sound scope
 * 
 * Run iteration of @fx_audio_unit_audio with @audio_signal.
 * 
 * Since: 8.1.2
 */
void
ags_fx_audio_unit_audio_render_thread_iteration(AgsFxAudioUnitAudio *fx_audio_unit_audio,
						AgsFxAudioUnitAudioChannelData *channel_data,
						AgsAudioSignal *audio_signal,
						guint pad,
						guint audio_channel,
						gint sound_scope)
{
  AgsAudioUnitPlugin *audio_unit_plugin;

  AgsFxAudioUnitAudioScopeData *scope_data;
  
  struct _AgsFxAudioUnitIterateData *iterate_data;

  AgsSoundcardFormat format;
  guint copy_mode;

  GRecMutex *recall_mutex;
  
  if(!AGS_IS_FX_AUDIO_UNIT_AUDIO(fx_audio_unit_audio) ||
     channel_data == NULL ||
     !AGS_IS_AUDIO_SIGNAL(audio_signal)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  /* get audio unit plugin */
  g_rec_mutex_lock(recall_mutex);

  audio_unit_plugin = fx_audio_unit_audio->audio_unit_plugin;

  g_rec_mutex_unlock(recall_mutex);

  /* get some fields */
  scope_data = channel_data->parent;
  
  format = ags_audio_signal_get_format(audio_signal);

  copy_mode = ags_audio_buffer_util_get_copy_mode_from_format(NULL,
							      AGS_AUDIO_BUFFER_UTIL_FLOAT,
							      ags_audio_buffer_util_format_from_soundcard(NULL, format));
  
  if(ags_fx_audio_unit_audio_test_flags(fx_audio_unit_audio,
					AGS_FX_AUDIO_UNIT_AUDIO_MONO)){
    /* fill audio unit input */
    g_rec_mutex_lock(recall_mutex);

    ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						channel_data->input, 1, 0,
						audio_signal->stream->data, 1, 0,
						channel_data->input_buffer_size, copy_mode);
  
    g_rec_mutex_unlock(recall_mutex);
  
    /* add iteration audio signal */
    iterate_data = (struct _AgsFxAudioUnitIterateData *) g_new0(struct _AgsFxAudioUnitIterateData,
								1);
  
    iterate_data->scope_data = scope_data;
    iterate_data->channel_data = channel_data;

    iterate_data->audio_signal = audio_signal;

    iterate_data->pad = pad;
    iterate_data->audio_channel = audio_channel;
  
    iterate_data->sound_scope = sound_scope;

    g_rec_mutex_lock(&(fx_audio_unit_audio->prepare_iteration_mutex));

    ags_atomic_pointer_set(&(fx_audio_unit_audio->iterate_data),
			   g_list_append(ags_atomic_pointer_get(&(fx_audio_unit_audio->iterate_data)),
					 iterate_data));
  
    g_rec_mutex_unlock(&(fx_audio_unit_audio->prepare_iteration_mutex));
  
    /* render thread pre sync */
    g_rec_mutex_lock(&(fx_audio_unit_audio->pre_sync_mutex));

    ags_atomic_int_increment(&(fx_audio_unit_audio->active_iteration_count));
  
    if(ags_atomic_boolean_get(&(fx_audio_unit_audio->pre_sync_wait))){
      g_cond_signal(&(fx_audio_unit_audio->pre_sync_cond));
    }
  
    g_rec_mutex_unlock(&(fx_audio_unit_audio->pre_sync_mutex));

    /* prepare iteration */
    g_mutex_lock(&(fx_audio_unit_audio->prepare_iteration_mutex));

    if(ags_atomic_pointer_get(&(fx_audio_unit_audio->active_iterate_data)) != iterate_data){
      ags_atomic_boolean_set(&(fx_audio_unit_audio->prepare_iteration_wait),
			     TRUE);
      
      while(ags_atomic_boolean_get(&(fx_audio_unit_audio->prepare_iteration_wait)) &&
	    ags_atomic_pointer_get(&(fx_audio_unit_audio->active_iterate_data)) != iterate_data){	
	g_cond_wait(&(fx_audio_unit_audio->prepare_iteration_cond),
		    &(fx_audio_unit_audio->prepare_iteration_mutex));
      }

      if(ags_atomic_pointer_get(&(fx_audio_unit_audio->iterate_data)) == NULL){
	ags_atomic_boolean_set(&(fx_audio_unit_audio->prepare_iteration_wait),
			       FALSE);
      }
    }
    
    g_mutex_unlock(&(fx_audio_unit_audio->prepare_iteration_mutex));

    /* iteration completed */
    g_mutex_lock(&(fx_audio_unit_audio->completed_iteration_mutex));

    if(!ags_atomic_boolean_get(&(fx_audio_unit_audio->completed_iteration_done))){
      ags_atomic_boolean_set(&(fx_audio_unit_audio->completed_iteration_wait),
			     TRUE);
      
      while(ags_atomic_boolean_get(&(fx_audio_unit_audio->completed_iteration_wait)) &&
	    !ags_atomic_boolean_get(&(fx_audio_unit_audio->completed_iteration_done))){	
	g_cond_wait(&(fx_audio_unit_audio->completed_iteration_cond),
		    &(fx_audio_unit_audio->completed_iteration_mutex));
      }
    
      ags_atomic_boolean_set(&(fx_audio_unit_audio->completed_iteration_wait),
			     FALSE);

      ags_atomic_boolean_set(&(fx_audio_unit_audio->completed_iteration_done),
			     FALSE);
    }
    
    g_mutex_unlock(&(fx_audio_unit_audio->completed_iteration_mutex));
  }else{
    guint audio_channels;
    gboolean do_sync;

    audio_channels = scope_data->audio_channels;
    
    /* fill audio unit input */
    g_rec_mutex_lock(recall_mutex);

    ags_audio_buffer_util_copy_buffer_to_buffer(NULL,
						scope_data->input, audio_channels, audio_channel,
						audio_signal->stream->data, 1, 0,
						scope_data->input_buffer_size, copy_mode);
  
    g_rec_mutex_unlock(recall_mutex);

    /* add iteration audio signal */
    iterate_data = (struct _AgsFxAudioUnitIterateData *) g_new0(struct _AgsFxAudioUnitIterateData,
								1);
  
    iterate_data->scope_data = scope_data;
    iterate_data->channel_data = channel_data;

    iterate_data->audio_signal = audio_signal;

    iterate_data->pad = pad;
    iterate_data->audio_channel = audio_channel;

    g_rec_mutex_lock(&(fx_audio_unit_audio->prepare_iteration_mutex));

    ags_atomic_pointer_set(&(fx_audio_unit_audio->iterate_data),
			   g_list_append(ags_atomic_pointer_get(&(fx_audio_unit_audio->iterate_data)),
					 iterate_data));
  
    g_rec_mutex_unlock(&(fx_audio_unit_audio->prepare_iteration_mutex));

    /* increment queued */    
    if(ags_base_plugin_test_flags((AgsBasePlugin *) audio_unit_plugin,
				  AGS_BASE_PLUGIN_IS_INSTRUMENT)){
      ags_atomic_int_increment(&(scope_data->channel_data[audio_channel]->input_data[pad]->queued_audio_signal));
    }else{
      ags_atomic_int_increment(&(scope_data->channel_data[audio_channel]->queued_audio_signal));
    }
    
    /* render thread pre sync */
    g_rec_mutex_lock(&(fx_audio_unit_audio->pre_sync_mutex));

    ags_atomic_int_increment(&(fx_audio_unit_audio->active_iteration_count));
  
    if(ags_atomic_boolean_get(&(fx_audio_unit_audio->pre_sync_wait))){
      g_cond_signal(&(fx_audio_unit_audio->pre_sync_cond));
    }
  
    g_rec_mutex_unlock(&(fx_audio_unit_audio->pre_sync_mutex));

    /* check if sync thread */
    do_sync = FALSE;
    
    if(ags_base_plugin_test_flags((AgsBasePlugin *) audio_unit_plugin,
				  AGS_BASE_PLUGIN_IS_INSTRUMENT)){
      guint i;
      gboolean success;

      success = TRUE;
      
      for(i = 0; i < audio_channels; i++){
	if(ags_atomic_int_get(&(scope_data->channel_data[i]->input_data[pad]->queued_audio_signal)) < ags_atomic_int_get(&(scope_data->channel_data[i]->input_data[pad]->active_audio_signal))){
	  success = FALSE;

	  break;
	}
      }

      do_sync = success;
    }else{
      guint i;
      gboolean success;

      success = TRUE;
      
      for(i = 0; i < audio_channels; i++){
	if(ags_atomic_int_get(&(scope_data->channel_data[i]->queued_audio_signal)) < ags_atomic_int_get(&(scope_data->channel_data[i]->active_audio_signal))){
	  success = FALSE;

	  break;
	}
      }

      do_sync = success;
    }

    if(do_sync){
      /* prepare iteration */
      g_mutex_lock(&(fx_audio_unit_audio->prepare_iteration_mutex));

      if(g_list_find(ags_atomic_pointer_get(&(fx_audio_unit_audio->active_iterate_data)), iterate_data) == NULL){
	ags_atomic_boolean_set(&(fx_audio_unit_audio->prepare_iteration_wait),
			       TRUE);
      
	while(ags_atomic_boolean_get(&(fx_audio_unit_audio->prepare_iteration_wait)) &&
	      g_list_find(ags_atomic_pointer_get(&(fx_audio_unit_audio->active_iterate_data)), iterate_data) == NULL){
	  g_cond_wait(&(fx_audio_unit_audio->prepare_iteration_cond),
		      &(fx_audio_unit_audio->prepare_iteration_mutex));
	}

	if(ags_atomic_pointer_get(&(fx_audio_unit_audio->iterate_data)) == NULL){
	  ags_atomic_boolean_set(&(fx_audio_unit_audio->prepare_iteration_wait),
				 FALSE);
	}
      }
    
      g_mutex_unlock(&(fx_audio_unit_audio->prepare_iteration_mutex));

      /* iteration completed */
      g_mutex_lock(&(fx_audio_unit_audio->completed_iteration_mutex));

      if(!ags_atomic_boolean_get(&(fx_audio_unit_audio->completed_iteration_done))){
	ags_atomic_boolean_set(&(fx_audio_unit_audio->completed_iteration_wait),
			       TRUE);
      
	while(ags_atomic_boolean_get(&(fx_audio_unit_audio->completed_iteration_wait)) &&
	      !ags_atomic_boolean_get(&(fx_audio_unit_audio->completed_iteration_done))){	
	  g_cond_wait(&(fx_audio_unit_audio->completed_iteration_cond),
		      &(fx_audio_unit_audio->completed_iteration_mutex));
	}
    
	ags_atomic_boolean_set(&(fx_audio_unit_audio->completed_iteration_wait),
			       FALSE);

	ags_atomic_boolean_set(&(fx_audio_unit_audio->completed_iteration_done),
			       FALSE);
      }
    
      g_mutex_unlock(&(fx_audio_unit_audio->completed_iteration_mutex));
    }
  }
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
