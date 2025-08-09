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

void ags_fx_audio_unit_audio_scope_data_load_port(AgsFxAudioUnitAudio *fx_audio_unit_audio,
						  AgsFxAudioUnitAudioScopeData *scope_data);

void ags_fx_audio_unit_audio_safe_write_callback(AgsPort *port, GValue *value,
						 AgsFxAudioUnitAudio *fx_audio_unit_audio);

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

  fx_audio_unit_audio->flags = AGS_FX_AUDIO_UNIT_AUDIO_LIVE_INSTRUMENT;
    
  fx_audio_unit_audio->audio_unit_plugin = NULL;

  fx_audio_unit_audio->audio_unit = NULL;

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
    guint audio_channels;

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
  
  guint audio_channels;
  guint buffer_size;
  guint i;
  gboolean is_live_instrument;
  
  GRecMutex *recall_mutex;
  
  fx_audio_unit_audio = AGS_FX_AUDIO_UNIT_AUDIO(gobject);

  if(!ags_recall_test_state_flags(AGS_RECALL(fx_audio_unit_audio),
				  AGS_SOUND_STATE_PORT_LOADED)){
    return;
  }
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  audio = NULL;

  /* get audio_unit plugin */
  g_rec_mutex_lock(recall_mutex);

  audio_unit_plugin = fx_audio_unit_audio->audio_unit_plugin;
  
  g_rec_mutex_unlock(recall_mutex);

  if(audio_unit_plugin == NULL ||
     !ags_base_plugin_test_flags((AgsBasePlugin *) audio_unit_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    return;
  }
  
  /* get buffer size */
  g_object_get(fx_audio_unit_audio,
	       "audio", &audio,
	       "buffer-size", &buffer_size,
	       NULL);

  audio_channels = 0;
  
  g_object_get(audio,
	       "audio-channels", &audio_channels,
	       NULL);

  is_live_instrument = ags_fx_audio_unit_audio_test_flags(fx_audio_unit_audio,
							  AGS_FX_AUDIO_UNIT_AUDIO_LIVE_INSTRUMENT);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxAudioUnitAudioScopeData *scope_data;

    scope_data = fx_audio_unit_audio->scope_data[i];
    
    if(is_live_instrument){
      //TODO:JK: implement me
    }else{
      //TODO:JK: implement me
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

  guint audio_channels;
  guint buffer_size;
  guint samplerate;
  guint i;
  gboolean is_live_instrument;
  
  GRecMutex *recall_mutex;
  
  fx_audio_unit_audio = AGS_FX_AUDIO_UNIT_AUDIO(gobject);

  if(!ags_recall_test_state_flags(AGS_RECALL(fx_audio_unit_audio),
				  AGS_SOUND_STATE_PORT_LOADED)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

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

  is_live_instrument = ags_fx_audio_unit_audio_test_flags(fx_audio_unit_audio,
							  AGS_FX_AUDIO_UNIT_AUDIO_LIVE_INSTRUMENT);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxAudioUnitAudioScopeData *scope_data;

    scope_data = fx_audio_unit_audio->scope_data[i];
    
    if(is_live_instrument){
      //TODO:JK: implement me
    }else{
      //TODO:JK: implement me
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
  guint buffer_size;
  guint samplerate;
  guint i;
  gboolean is_live_instrument;

  GRecMutex *recall_mutex;

  if(audio_channels == audio_channels_old){
    return;
  }
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);
  
  /* recall container, buffer size and samplerate */
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  g_object_get(fx_audio_unit_audio,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  is_live_instrument = ags_fx_audio_unit_audio_test_flags(fx_audio_unit_audio,
							  AGS_FX_AUDIO_UNIT_AUDIO_LIVE_INSTRUMENT);

  //TODO:JK: implement me
}

void
ags_fx_audio_unit_audio_set_pads_callback(AgsAudio *audio,
					  GType channel_type,
					  guint pads, guint pads_old,
					  AgsFxAudioUnitAudio *fx_audio_unit_audio)
{
  guint audio_channels;
  gboolean is_live_instrument;
  
  GRecMutex *recall_mutex;

  if(!g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_audio_unit_audio);

  /* get AUDIO_UNIT plugin and port */
  is_live_instrument = ags_fx_audio_unit_audio_test_flags(fx_audio_unit_audio,
							  AGS_FX_AUDIO_UNIT_AUDIO_LIVE_INSTRUMENT);

  if(is_live_instrument){
    return;
  }

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
  
  g_free(scope_data);
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
  AgsAudioUnitManager *audio_unit_manager;
  AgsAudioUnitPlugin *audio_unit_plugin;

  gpointer audio_unit;

  gchar *filename, *effect;

  gint64 start_time, current_time;    
  guint i;

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
  filename = NULL;
  effect = NULL;

  g_object_get(fx_audio_unit_audio,
	       "filename", &filename,
	       "effect", &effect,
	       NULL);
  
  audio_unit_plugin = ags_audio_unit_manager_find_audio_unit_plugin(audio_unit_manager,
								    filename, effect);
  
  if(audio_unit_plugin == NULL){
    g_free(filename);
    g_free(effect);
    
    return;
  }

  fx_audio_unit_audio->audio_unit_plugin = audio_unit_plugin;

  start_time = g_get_monotonic_time();
	
  ags_base_plugin_async_instantiate((AgsBasePlugin *) audio_unit_plugin);

  audio_unit = NULL;
	
  while((audio_unit = ags_audio_unit_new_queue_manager_fetch_audio_unit(ags_audio_unit_new_queue_manager_get_instance(),
									effect)) == NULL){
    g_usleep(4);
      
    current_time = g_get_monotonic_time();
      
    if(current_time > start_time + (20 * G_TIME_SPAN_SECOND)){
      //NOTE:JK: give up after 20 seconds
	
      break;
    }
  }

  fx_audio_unit_audio->audio_unit = audio_unit;
  
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
  gboolean is_live_instrument;
  
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
	
      g_object_set(audio_unit_port[nth],
		   "plugin-port", current_plugin_port,
		   NULL);

      ags_port_util_load_audio_unit_conversion(audio_unit_port[nth],
					       current_plugin_port);
	
      ags_port_safe_write_raw(audio_unit_port[nth],
			      &default_value);

      ags_recall_add_port((AgsRecall *) fx_audio_unit_audio,
			  audio_unit_port[nth]);
      
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

void
ags_fx_audio_unit_audio_safe_write_callback(AgsPort *port, GValue *value,
					    AgsFxAudioUnitAudio *fx_audio_unit_audio)
{
  //TODO:JK: implement me
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
