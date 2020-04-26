/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_lv2_audio.h>

#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_port_util.h>

#include <ags/audio/fx/ags_fx_lv2_channel.h>

#include <ags/i18n.h>

void ags_fx_lv2_audio_class_init(AgsFxLv2AudioClass *fx_lv2_audio);
void ags_fx_lv2_audio_init(AgsFxLv2Audio *fx_lv2_audio);
void ags_fx_lv2_audio_dispose(GObject *gobject);
void ags_fx_lv2_audio_finalize(GObject *gobject);

void ags_fx_lv2_audio_notify_audio_callback(GObject *gobject,
					    GParamSpec *pspec,
					    gpointer user_data);
void ags_fx_lv2_audio_notify_filename_callback(GObject *gobject,
					       GParamSpec *pspec,
					       gpointer user_data);
void ags_fx_lv2_audio_notify_effect_callback(GObject *gobject,
					     GParamSpec *pspec,
					     gpointer user_data);
void ags_fx_lv2_audio_notify_buffer_size_callback(GObject *gobject,
						  GParamSpec *pspec,
						  gpointer user_data);
void ags_fx_lv2_audio_notify_samplerate_callback(GObject *gobject,
						 GParamSpec *pspec,
						 gpointer user_data);

void ags_fx_lv2_audio_set_audio_channels_callback(AgsAudio *audio,
						  guint audio_channels, guint audio_channels_old,
						  AgsFxLv2Audio *fx_lv2_audio);
void ags_fx_lv2_audio_set_pads_callback(AgsAudio *audio,
					GType channel_type,
					guint pads, guint pads_old,
					AgsFxLv2Audio *fx_lv2_audio);

/**
 * SECTION:ags_fx_lv2_audio
 * @short_description: fx lv2 audio
 * @title: AgsFxLv2Audio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_lv2_audio.h
 *
 * The #AgsFxLv2Audio class provides ports to the effect processor.
 */

static gpointer ags_fx_lv2_audio_parent_class = NULL;

static const gchar *ags_fx_lv2_audio_plugin_name = "ags-fx-lv2";

static const gchar *ags_fx_lv2_audio_specifier[] = {
  NULL,
};

static const gchar *ags_fx_lv2_audio_control_port[] = {
  NULL,
};

enum{
  PROP_0,
};

GType
ags_fx_lv2_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_lv2_audio = 0;

    static const GTypeInfo ags_fx_lv2_audio_info = {
      sizeof (AgsFxLv2AudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_lv2_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxLv2Audio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_lv2_audio_init,
    };

    ags_type_fx_lv2_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						   "AgsFxLv2Audio",
						   &ags_fx_lv2_audio_info,
						   0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_lv2_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_lv2_audio_class_init(AgsFxLv2AudioClass *fx_lv2_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_lv2_audio_parent_class = g_type_class_peek_parent(fx_lv2_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_lv2_audio;

  gobject->dispose = ags_fx_lv2_audio_dispose;
  gobject->finalize = ags_fx_lv2_audio_finalize;

  /* properties */
}

void
ags_fx_lv2_audio_init(AgsFxLv2Audio *fx_lv2_audio)
{
  guint i;

  g_signal_connect(fx_lv2_audio, "notify::audio",
		   G_CALLBACK(ags_fx_lv2_audio_notify_audio_callback), NULL);

  g_signal_connect(fx_lv2_audio, "notify::filename",
		   G_CALLBACK(ags_fx_lv2_audio_notify_filename_callback), NULL);

  g_signal_connect(fx_lv2_audio, "notify::effect",
		   G_CALLBACK(ags_fx_lv2_audio_notify_effect_callback), NULL);

  g_signal_connect(fx_lv2_audio, "notify::buffer-size",
		   G_CALLBACK(ags_fx_lv2_audio_notify_buffer_size_callback), NULL);

  g_signal_connect(fx_lv2_audio, "notify::samplerate",
		   G_CALLBACK(ags_fx_lv2_audio_notify_samplerate_callback), NULL);

  AGS_RECALL(fx_lv2_audio)->name = "ags-fx-lv2";
  AGS_RECALL(fx_lv2_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_lv2_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_lv2_audio)->xml_type = "ags-fx-lv2-audio";

  fx_lv2_audio->flags = 0;
  
  fx_lv2_audio->output_port_count = 0;
  fx_lv2_audio->output_port = NULL;
  
  fx_lv2_audio->input_port_count = 0;
  fx_lv2_audio->input_port = NULL;

  fx_lv2_audio->bank = 0;
  fx_lv2_audio->program = 0;

  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      fx_lv2_audio->scope_data[i] = ags_fx_lv2_audio_scope_data_alloc();
      
      fx_lv2_audio->scope_data[i]->parent = fx_lv2_audio;
    }else{
      fx_lv2_audio->scope_data[i] = NULL;
    }
  }
  
  fx_lv2_audio->lv2_plugin = NULL;

  fx_lv2_audio->lv2_port = NULL;
}

void
ags_fx_lv2_audio_dispose(GObject *gobject)
{
  AgsFxLv2Audio *fx_lv2_audio;
  
  fx_lv2_audio = AGS_FX_LV2_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_lv2_audio_parent_class)->dispose(gobject);
}

void
ags_fx_lv2_audio_finalize(GObject *gobject)
{
  AgsPort **iter;
  AgsFxLv2Audio *fx_lv2_audio;
    
  guint i;
  
  fx_lv2_audio = AGS_FX_LV2_AUDIO(gobject);

  g_free(fx_lv2_audio->output_port);
  g_free(fx_lv2_audio->input_port);
  
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      ags_fx_lv2_audio_scope_data_free(fx_lv2_audio->scope_data[i]);
    }
  }
  
  if(fx_lv2_audio->lv2_plugin != NULL){
    g_object_unref(fx_lv2_audio->lv2_plugin);
  }
  
  if(fx_lv2_audio->lv2_port == NULL){
    for(iter = fx_lv2_audio->lv2_port; iter[0] != NULL; iter++){
      g_object_unref(iter[0]);
    }

    g_free(fx_lv2_audio->lv2_port);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_lv2_audio_parent_class)->finalize(gobject);
}

void
ags_fx_lv2_audio_notify_audio_callback(GObject *gobject,
				       GParamSpec *pspec,
				       gpointer user_data)
{
  AgsAudio *audio;
  AgsFxLv2Audio *fx_lv2_audio;

  fx_lv2_audio = AGS_FX_LV2_AUDIO(gobject);

  /* get audio */
  audio = NULL;

  g_object_get(fx_lv2_audio,
	       "audio", &audio,
	       NULL);

  g_signal_connect_after((GObject *) audio, "set-audio-channels",
			 G_CALLBACK(ags_fx_lv2_audio_set_audio_channels_callback), fx_lv2_audio);

  g_signal_connect_after((GObject *) audio, "set-pads",
			 G_CALLBACK(ags_fx_lv2_audio_set_pads_callback), fx_lv2_audio);
}

void
ags_fx_lv2_audio_notify_filename_callback(GObject *gobject,
					  GParamSpec *pspec,
					  gpointer user_data)
{
  //TODO:JK: implement me
}

void
ags_fx_lv2_audio_notify_effect_callback(GObject *gobject,
					GParamSpec *pspec,
					gpointer user_data)
{
  //TODO:JK: implement me
}

void
ags_fx_lv2_audio_notify_buffer_size_callback(GObject *gobject,
					     GParamSpec *pspec,
					     gpointer user_data)
{
  //TODO:JK: implement me
}

void
ags_fx_lv2_audio_notify_samplerate_callback(GObject *gobject,
					    GParamSpec *pspec,
					    gpointer user_data)
{
  //TODO:JK: implement me
}

void
ags_fx_lv2_audio_set_audio_channels_callback(AgsAudio *audio,
					     guint audio_channels, guint audio_channels_old,
					     AgsFxLv2Audio *fx_lv2_audio)
{
  //TODO:JK: implement me
}

void
ags_fx_lv2_audio_set_pads_callback(AgsAudio *audio,
				   GType channel_type,
				   guint pads, guint pads_old,
				   AgsFxLv2Audio *fx_lv2_audio)
{
  //TODO:JK: implement me
}

/**
 * ags_fx_lv2_audio_scope_data_alloc:
 * 
 * Allocate #AgsFxLv2AudioScopeData-struct
 * 
 * Returns: the new #AgsFxLv2AudioScopeData-struct
 * 
 * Since: 3.3.0
 */
AgsFxLv2AudioScopeData*
ags_fx_lv2_audio_scope_data_alloc()
{
  AgsFxLv2AudioScopeData *scope_data;

  scope_data = (AgsFxLv2AudioScopeData *) g_malloc(sizeof(AgsFxLv2AudioScopeData));

  scope_data->parent = NULL;
  
  scope_data->audio_channels = 0;

  scope_data->channel_data = NULL;
  
  return(scope_data);
}

/**
 * ags_fx_lv2_audio_scope_data_free:
 * @scope_data: the #AgsFxLv2AudioScopeData-struct
 * 
 * Free @scope_data.
 * 
 * Since: 3.3.0
 */
void
ags_fx_lv2_audio_scope_data_free(AgsFxLv2AudioScopeData *scope_data)
{
  guint i;

  if(scope_data == NULL){
    return;
  }

  for(i = 0; i < scope_data->audio_channels; i++){
    ags_fx_lv2_audio_channel_data_free(scope_data->channel_data[i]);
  }

  g_free(scope_data);
}

/**
 * ags_fx_lv2_audio_channel_data_alloc:
 * 
 * Allocate #AgsFxLv2AudioChannelData-struct
 * 
 * Returns: the new #AgsFxLv2AudioChannelData-struct
 * 
 * Since: 3.3.0
 */
AgsFxLv2AudioChannelData*
ags_fx_lv2_audio_channel_data_alloc()
{
  AgsFxLv2AudioChannelData *channel_data;

  guint i;
  
  channel_data = (AgsFxLv2AudioChannelData *) g_malloc(sizeof(AgsFxLv2AudioChannelData));

  channel_data->parent = NULL;
  
  channel_data->event_count = 0;

  channel_data->output = NULL;
  channel_data->input = NULL;

  channel_data->lv2_handle = NULL;

  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    channel_data->input_data[i] = ags_fx_lv2_audio_input_data_alloc();

    channel_data->input_data[i]->parent = channel_data;
  }

  return(channel_data);
}

/**
 * ags_fx_lv2_audio_channel_data_free:
 * @channel_data: the #AgsFxLv2AudioChannelData-struct
 * 
 * Free @channel_data.
 * 
 * Since: 3.3.0
 */
void
ags_fx_lv2_audio_channel_data_free(AgsFxLv2AudioChannelData *channel_data)
{
  AgsLv2Plugin *lv2_plugin;

  guint i;

  if(channel_data == NULL){
    return;
  }

  g_free(channel_data->output);
  g_free(channel_data->input);

  if(channel_data->lv2_handle != NULL){
    gpointer plugin_descriptor;
    
    GRecMutex *base_plugin_mutex;
    
    void (*deactivate)(LV2_Handle instance);
    void (*cleanup)(LV2_Handle instance);
  
    lv2_plugin = NULL;
    
    deactivate = NULL;
    cleanup = NULL;

    base_plugin_mutex = NULL;
    
    if(channel_data->parent != NULL &&
       AGS_FX_LV2_AUDIO_SCOPE_DATA(channel_data->parent)->parent != NULL){
      lv2_plugin = AGS_FX_LV2_AUDIO(AGS_FX_LV2_AUDIO_SCOPE_DATA(channel_data->parent)->parent)->lv2_plugin;
    }
    
    if(lv2_plugin != NULL){
      base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2_plugin);

      g_rec_mutex_lock(base_plugin_mutex);
  
      plugin_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor;

      deactivate = AGS_LV2_PLUGIN_DESCRIPTOR(plugin_descriptor)->deactivate;
      cleanup = AGS_LV2_PLUGIN_DESCRIPTOR(plugin_descriptor)->cleanup;
      
      g_rec_mutex_unlock(base_plugin_mutex);
    }

    if(deactivate != NULL){
      deactivate(channel_data->lv2_handle);
    }

    if(cleanup != NULL){
      cleanup(channel_data->lv2_handle);
    }
  }
  
  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    ags_fx_lv2_audio_input_data_free(channel_data->input_data[i]);
  }

  g_free(channel_data);
}

/**
 * ags_fx_lv2_audio_input_data_alloc:
 * 
 * Allocate #AgsFxLv2AudioInputData-struct
 * 
 * Returns: the new #AgsFxLv2AudioInputData-struct
 * 
 * Since: 3.3.0
 */
AgsFxLv2AudioInputData*
ags_fx_lv2_audio_input_data_alloc()
{
  AgsFxLv2AudioInputData *input_data;

  input_data = (AgsFxLv2AudioInputData *) g_malloc(sizeof(AgsFxLv2AudioInputData));

  input_data->parent = NULL;

  input_data->output = NULL;
  input_data->input = NULL;

  input_data->lv2_handle = NULL;

  input_data->event_buffer = (snd_seq_event_t *) g_malloc(sizeof(snd_seq_event_t));

  input_data->key_on = 0;
  
  return(input_data);
}

/**
 * ags_fx_lv2_audio_input_data_free:
 * @input_data: the #AgsFxLv2AudioInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 3.3.0
 */
void
ags_fx_lv2_audio_input_data_free(AgsFxLv2AudioInputData *input_data)
{
  AgsLv2Plugin *lv2_plugin;
  
  if(input_data == NULL){
    return;
  }

  g_free(input_data->output);
  g_free(input_data->input);

  if(input_data->lv2_handle != NULL){
    gpointer plugin_descriptor;

    GRecMutex *base_plugin_mutex;
    
    void (*deactivate)(LV2_Handle instance);
    void (*cleanup)(LV2_Handle instance);
  
    lv2_plugin = NULL;
    
    deactivate = NULL;
    cleanup = NULL;

    base_plugin_mutex = NULL;
    
    if(input_data->parent != NULL &&
       AGS_FX_LV2_AUDIO_CHANNEL_DATA(input_data->parent)->parent != NULL &&
       AGS_FX_LV2_AUDIO_SCOPE_DATA(AGS_FX_LV2_AUDIO_CHANNEL_DATA(input_data->parent)->parent)->parent != NULL){
      lv2_plugin = AGS_FX_LV2_AUDIO(AGS_FX_LV2_AUDIO_SCOPE_DATA(AGS_FX_LV2_AUDIO_CHANNEL_DATA(input_data->parent)->parent)->parent)->lv2_plugin;
    }
    
    if(lv2_plugin != NULL){
      base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2_plugin);
      
      g_rec_mutex_lock(base_plugin_mutex);
  
      plugin_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor;

      deactivate = AGS_LV2_PLUGIN_DESCRIPTOR(plugin_descriptor)->deactivate;
      cleanup = AGS_LV2_PLUGIN_DESCRIPTOR(plugin_descriptor)->cleanup;
      
      g_rec_mutex_unlock(base_plugin_mutex);
    }

    if(deactivate != NULL){
      deactivate(input_data->lv2_handle);
    }

    if(cleanup != NULL){
      cleanup(input_data->lv2_handle);
    }
  }
  
  g_free(input_data->event_buffer);

  g_free(input_data);
}

/**
 * ags_fx_lv2_audio_test_flags:
 * @fx_lv2_audio: the #AgsFxLv2Audio
 * @flags: the flags
 * 
 * Test @flags of @fx_lv2_audio.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.3.0
 */
gboolean
ags_fx_lv2_audio_test_flags(AgsFxLv2Audio *fx_lv2_audio, guint flags)
{
  gboolean success;
  
  GRecMutex *recall_mutex;
  
  if(!AGS_IS_FX_LV2_AUDIO(fx_lv2_audio)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_audio);

  /* test flags */
  g_rec_mutex_lock(recall_mutex);

  success = ((flags & (fx_lv2_audio->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(recall_mutex);

  return(success);
}

/**
 * ags_fx_lv2_audio_set_flags:
 * @fx_lv2_audio: the #AgsFxLv2Audio
 * @flags: the flags
 * 
 * Set @flags of @fx_lv2_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_lv2_audio_set_flags(AgsFxLv2Audio *fx_lv2_audio, guint flags)
{  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_LV2_AUDIO(fx_lv2_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_audio);
  
  /* set flags */
  g_rec_mutex_lock(recall_mutex);

  fx_lv2_audio->flags |= flags;

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_lv2_audio_unset_flags:
 * @fx_lv2_audio: the #AgsFxLv2Audio
 * @flags: the flags
 * 
 * Unset @flags of @fx_lv2_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_lv2_audio_unset_flags(AgsFxLv2Audio *fx_lv2_audio, guint flags)
{  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_LV2_AUDIO(fx_lv2_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_audio);

  /* set flags */
  g_rec_mutex_lock(recall_mutex);

  fx_lv2_audio->flags &= (~flags);

  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_lv2_audio_load_plugin(AgsFxLv2Audio *fx_lv2_audio)
{
  //TODO:JK: implement me
}

void
ags_fx_lv2_audio_load_port(AgsFxLv2Audio *fx_lv2_audio)
{
  //TODO:JK: implement me
}

void
ags_fx_lv2_audio_change_program(AgsFxLv2Audio *fx_lv2_audio,
				guint bank_index,
				guint program_index)
{
  //TODO:JK: implement me
}

/**
 * ags_fx_lv2_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxLv2Audio
 *
 * Returns: the new #AgsFxLv2Audio
 *
 * Since: 3.3.0
 */
AgsFxLv2Audio*
ags_fx_lv2_audio_new(AgsAudio *audio)
{
  AgsFxLv2Audio *fx_lv2_audio;

  fx_lv2_audio = (AgsFxLv2Audio *) g_object_new(AGS_TYPE_FX_LV2_AUDIO,
						"audio", audio,
						NULL);

  return(fx_lv2_audio);
}
