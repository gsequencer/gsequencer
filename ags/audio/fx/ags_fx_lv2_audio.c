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

    ags_type_fx_lv2_audio = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO,
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
  
  if(fx_lv2_audio->lv2_port != NULL){
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
  AgsFxLv2Audio *fx_lv2_audio;

  AgsLv2Plugin *lv2_plugin;
  
  guint output_port_count, input_port_count;
  guint buffer_size;
  guint i, j, k;
  gboolean is_live_instrument;
  
  GRecMutex *recall_mutex;
  
  fx_lv2_audio = AGS_FX_LV2_AUDIO(gobject);
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_audio);

  /* get lv2 plugin */
  g_rec_mutex_lock(recall_mutex);

  lv2_plugin = fx_lv2_audio->lv2_plugin;
  
  g_rec_mutex_unlock(recall_mutex);

  if(lv2_plugin == NULL ||
     !ags_base_plugin_test_flags((AgsBasePlugin *) lv2_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    return;
  }
  
  /* get buffer size */
  g_object_get(fx_lv2_audio,
	       "buffer-size", &buffer_size,
	       NULL);

  is_live_instrument = ags_fx_lv2_audio_test_flags(fx_lv2_audio, AGS_FX_LV2_AUDIO_LIVE_INSTRUMENT);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  output_port_count = fx_lv2_audio->output_port_count;
  input_port_count = fx_lv2_audio->input_port_count;

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxLv2AudioScopeData *scope_data;

    scope_data = fx_lv2_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxLv2AudioChannelData *channel_data;

	channel_data = scope_data->channel_data[j];

	if(is_live_instrument){
	  if(output_port_count > 0 &&
	     buffer_size > 0){
	    if(channel_data->output == NULL){
	      channel_data->output = (float *) g_malloc(output_port_count * buffer_size * sizeof(float));
	    }else{
	      channel_data->output = (float *) g_realloc(channel_data->output,
							 output_port_count * buffer_size * sizeof(float));	    
	    }
	  }

	  if(input_port_count > 0 &&
	     buffer_size > 0){
	    if(channel_data->input == NULL){
	      channel_data->input = (float *) g_malloc(input_port_count * buffer_size * sizeof(float));
	    }else{
	      channel_data->input = (float *) g_realloc(channel_data->input,
							input_port_count * buffer_size * sizeof(float));	    
	    }
	  }
	}
	
	if(!is_live_instrument){
	  for(k = 0; k < AGS_SEQUENCER_MAX_MIDI_KEYS; k++){
	    AgsFxLv2AudioInputData *input_data;

	    input_data = channel_data->input_data[k];
	    
	    if(output_port_count > 0 &&
	       buffer_size > 0){
	      if(input_data->output == NULL){
		input_data->output = (float *) g_malloc(output_port_count * buffer_size * sizeof(float));
	      }else{
		input_data->output = (float *) g_realloc(input_data->output,
							 output_port_count * buffer_size * sizeof(float));	    
	      }
	    }
	    
	    if(input_port_count > 0 &&
	       buffer_size > 0){
	      if(input_data->input == NULL){
		input_data->input = (float *) g_malloc(input_port_count * buffer_size * sizeof(float));
	      }else{
		input_data->input = (float *) g_realloc(input_data->input,
							input_port_count * buffer_size * sizeof(float));	    
	      }
	    }
	  }
	}
      }
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_lv2_audio_notify_samplerate_callback(GObject *gobject,
					    GParamSpec *pspec,
					    gpointer user_data)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsRecallContainer *recall_container;

  AgsFxLv2Audio *fx_lv2_audio;

  AgsLv2Plugin *lv2_plugin;

  GList *start_recall_channel, *recall_channel;

  guint input_pads;
  guint audio_channels;
  guint output_port_count, input_port_count;
  guint buffer_size;
  guint samplerate;
  guint i, j, k;
  gboolean is_live_instrument;
  
  GRecMutex *recall_mutex;
  GRecMutex *base_plugin_mutex;

  void (*deactivate)(LV2_Handle Instance);
  void (*cleanup)(LV2_Handle Instance);
  
  fx_lv2_audio = AGS_FX_LV2_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_audio);

  /* get lv2 plugin */
  g_rec_mutex_lock(recall_mutex);

  lv2_plugin = fx_lv2_audio->lv2_plugin;
  
  g_rec_mutex_unlock(recall_mutex);

  if(lv2_plugin == NULL ||
     !ags_base_plugin_test_flags((AgsBasePlugin *) lv2_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    return;
  }

  audio = NULL;

  recall_container = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  g_object_get(fx_lv2_audio,
	       "audio", &audio,
	       "recall-container", &recall_container,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);
  
  start_recall_channel = ags_recall_container_get_recall_channel(recall_container);

  start_input = NULL;

  input_pads = 0;
  audio_channels = 0;
  
  g_object_get(audio,
	       "input", &start_input,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);

  /* get port count */
  g_rec_mutex_lock(recall_mutex);

  output_port_count = fx_lv2_audio->output_port_count;
  input_port_count = fx_lv2_audio->input_port_count;

  g_rec_mutex_unlock(recall_mutex);

  /* get deactivate and cleanup */
  base_plugin_mutex = NULL;

  deactivate = NULL;
  cleanup = NULL;
  
  if(lv2_plugin != NULL){
    gpointer plugin_descriptor;
    
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(lv2_plugin);
    
    g_rec_mutex_lock(base_plugin_mutex);
  
    plugin_descriptor = AGS_BASE_PLUGIN(lv2_plugin)->plugin_descriptor;

    deactivate = AGS_LV2_PLUGIN_DESCRIPTOR(plugin_descriptor)->deactivate;
    cleanup = AGS_LV2_PLUGIN_DESCRIPTOR(plugin_descriptor)->cleanup;
      
    g_rec_mutex_unlock(base_plugin_mutex);
  }

  is_live_instrument = ags_fx_lv2_audio_test_flags(fx_lv2_audio, AGS_FX_LV2_AUDIO_LIVE_INSTRUMENT);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxLv2AudioScopeData *scope_data;

    scope_data = fx_lv2_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxLv2AudioChannelData *channel_data;

	channel_data = scope_data->channel_data[j];

	if(is_live_instrument){
	  AgsPort **iter;
	  
	  guint nth;

	  if(deactivate != NULL){
	    deactivate(channel_data->lv2_handle[0]);
	  }

	  if(cleanup != NULL){
	    cleanup(channel_data->lv2_handle[0]);
	  }	  

	  channel_data->lv2_handle = ags_base_plugin_instantiate((AgsBasePlugin *) lv2_plugin,
								 samplerate, buffer_size);

	  for(nth = 0; nth < output_port_count; nth++){
	    ags_base_plugin_connect_port((AgsBasePlugin *) lv2_plugin,
					 channel_data->lv2_handle[0],
					 fx_lv2_audio->output_port[nth],
					 &(channel_data->output[nth]));
	  }

	  for(nth = 0; nth < input_port_count; nth++){
	    ags_base_plugin_connect_port((AgsBasePlugin *) lv2_plugin,
					 channel_data->lv2_handle[0],
					 fx_lv2_audio->input_port[nth],
					 &(channel_data->input[nth]));
	  }

	  if(fx_lv2_audio->lv2_port != NULL){
	    for(iter = fx_lv2_audio->lv2_port; iter[0] != NULL; iter++){
	      AgsPluginPort *plugin_port;

	      guint port_index;

	      plugin_port = NULL;
	    
	      port_index = 0;
	    
	      g_object_get(iter[0],
			   "plugin-port", &plugin_port,
			   NULL);

	      g_object_get(plugin_port,
			   "port-index", &port_index,
			   NULL);
	    
	      ags_base_plugin_connect_port((AgsBasePlugin *) lv2_plugin,
					   channel_data->lv2_handle[0],
					   port_index,
					   &(iter[0]->port_value.ags_port_float));

	      if(plugin_port != NULL){
		g_object_unref(plugin_port);
	      }
	    }
	  }
	}
	
	if(!is_live_instrument){
	  for(k = 0; k < AGS_SEQUENCER_MAX_MIDI_KEYS; k++){
	    AgsFxLv2AudioInputData *input_data;

	    input_data = channel_data->input_data[k];

	    if(deactivate != NULL){
	      deactivate(input_data->lv2_handle[0]);
	    }

	    if(cleanup != NULL){
	      cleanup(input_data->lv2_handle[0]);
	    }

	    input_data->lv2_handle = ags_base_plugin_instantiate((AgsBasePlugin *) lv2_plugin,
								 samplerate, buffer_size);
	  }
	}
      }
    }
  }

  if(!is_live_instrument){
    for(j = 0; j < audio_channels; j++){
      for(k = 0; k < input_pads; k++){
	AgsChannel *input;

	input = ags_channel_nth(start_input,
				k * audio_channels + j);

	recall_channel = ags_recall_template_find_provider(start_recall_channel, (GObject *) input);

	if(recall_channel != NULL){
	  ags_fx_lv2_channel_load_port(recall_channel->data);
	}

	if(input != NULL){
	  g_object_unref(input);
	}
      }
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);

  /* unref */
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(recall_container != NULL){
    g_object_unref(recall_container);
  }
}

void
ags_fx_lv2_audio_set_audio_channels_callback(AgsAudio *audio,
					     guint audio_channels, guint audio_channels_old,
					     AgsFxLv2Audio *fx_lv2_audio)
{
  AgsChannel *start_input;
  AgsRecallContainer *recall_container;

  AgsLv2Plugin *lv2_plugin;

  GList *start_recall_channel, *recall_channel;

  guint input_pads;
  guint output_port_count, input_port_count;
  guint buffer_size;
  guint samplerate;
  guint i, j, k;
  gboolean is_live_instrument;

  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_audio);

  /* get lv2 plugin */
  g_rec_mutex_lock(recall_mutex);

  lv2_plugin = fx_lv2_audio->lv2_plugin;
  
  g_rec_mutex_unlock(recall_mutex);

  if(lv2_plugin == NULL ||
     !ags_base_plugin_test_flags((AgsBasePlugin *) lv2_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    return;
  }

  start_input = NULL;

  input_pads = 0;

  g_object_get(audio,
	       "input", &start_input,
	       "input-pads", &input_pads,
	       NULL);
  
  /* recall container, buffer size and samplerate */
  recall_container = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  g_object_get(fx_lv2_audio,
	       "recall-container", &recall_container,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);
  
  start_recall_channel = ags_recall_container_get_recall_channel(recall_container);

  /* get LV2 plugin */
  g_rec_mutex_lock(recall_mutex);

  output_port_count = fx_lv2_audio->output_port_count;
  input_port_count = fx_lv2_audio->input_port_count;

  g_rec_mutex_unlock(recall_mutex);

  /* allocate channel data */
  is_live_instrument = ags_fx_lv2_audio_test_flags(fx_lv2_audio, AGS_FX_LV2_AUDIO_LIVE_INSTRUMENT);
    
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxLv2AudioScopeData *scope_data;

    scope_data = fx_lv2_audio->scope_data[i];

    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      if(scope_data->audio_channels > audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxLv2AudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];
	
	  ags_fx_lv2_audio_channel_data_free(channel_data);
	}
      }
      
      if(scope_data->channel_data == NULL){
	scope_data->channel_data = (AgsFxLv2AudioChannelData **) g_malloc(audio_channels * sizeof(AgsFxLv2AudioChannelData *)); 
      }else{
	scope_data->channel_data = (AgsFxLv2AudioChannelData **) g_realloc(scope_data->channel_data,
									   audio_channels * sizeof(AgsFxLv2AudioChannelData *)); 
      }

      if(scope_data->audio_channels < audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxLv2AudioChannelData *channel_data;

	  channel_data =
	    scope_data->channel_data[j] = ags_fx_lv2_audio_channel_data_alloc();

	  if(is_live_instrument){
	    AgsPort **iter;
	  
	    guint nth;

	    channel_data->lv2_handle = ags_base_plugin_instantiate((AgsBasePlugin *) lv2_plugin,
								   samplerate, buffer_size);
	  
	    if(output_port_count > 0 &&
	       buffer_size > 0){
	      if(channel_data->output == NULL){
		channel_data->output = (float *) g_malloc(output_port_count * buffer_size * sizeof(float));
	      }else{
		channel_data->output = (float *) g_realloc(channel_data->output,
							   output_port_count * buffer_size * sizeof(float));	    
	      }
	    }

	    if(input_port_count > 0 &&
	       buffer_size > 0){
	      if(channel_data->input == NULL){
		channel_data->input = (float *) g_malloc(input_port_count * buffer_size * sizeof(float));
	      }else{
		channel_data->input = (float *) g_realloc(channel_data->input,
							  input_port_count * buffer_size * sizeof(float));	    
	      }
	    }	  

	    for(nth = 0; nth < output_port_count; nth++){
	      ags_base_plugin_connect_port((AgsBasePlugin *) lv2_plugin,
					   channel_data->lv2_handle[0],
					   fx_lv2_audio->output_port[nth],
					   &(channel_data->output[nth]));
	    }

	    for(nth = 0; nth < input_port_count; nth++){
	      ags_base_plugin_connect_port((AgsBasePlugin *) lv2_plugin,
					   channel_data->lv2_handle[0],
					   fx_lv2_audio->input_port[nth],
					   &(channel_data->input[nth]));
	    }

	    for(iter = fx_lv2_audio->lv2_port; iter[0] != NULL; iter++){
	      AgsPluginPort *plugin_port;

	      guint port_index;

	      plugin_port = NULL;
	    
	      port_index = 0;
	    
	      g_object_get(iter[0],
			   "plugin-port", &plugin_port,
			   NULL);

	      g_object_get(plugin_port,
			   "port-index", &port_index,
			   NULL);
	    
	      ags_base_plugin_connect_port((AgsBasePlugin *) lv2_plugin,
					   channel_data->lv2_handle[0],
					   port_index,
					   &(iter[0]->port_value.ags_port_float));

	      if(plugin_port != NULL){
		g_object_unref(plugin_port);
	      }
	    }
	  }
	
	  if(!is_live_instrument){
	    for(k = 0; k < AGS_SEQUENCER_MAX_MIDI_KEYS; k++){
	      AgsFxLv2AudioInputData *input_data;

	      input_data = channel_data->input_data[k];

	      input_data->lv2_handle = ags_base_plugin_instantiate((AgsBasePlugin *) lv2_plugin,
								   samplerate, buffer_size);
	    
	      if(output_port_count > 0 &&
		 buffer_size > 0){
		if(input_data->output == NULL){
		  input_data->output = (float *) g_malloc(output_port_count * buffer_size * sizeof(float));
		}else{
		  input_data->output = (float *) g_realloc(input_data->output,
							   output_port_count * buffer_size * sizeof(float));	    
		}
	      }
	    
	      if(input_port_count > 0 &&
		 buffer_size > 0){
		if(input_data->input == NULL){
		  input_data->input = (float *) g_malloc(input_port_count * buffer_size * sizeof(float));
		}else{
		  input_data->input = (float *) g_realloc(input_data->input,
							  input_port_count * buffer_size * sizeof(float));	    
		}
	      }
	    }
	  }
	}
      }
      
      scope_data->audio_channels = audio_channels;
    }
  }  

  if(!is_live_instrument &&
     audio_channels_old < audio_channels){
    for(j = audio_channels_old; j < audio_channels; j++){
      for(k = 0; k < input_pads; k++){
	AgsChannel *input;

	input = ags_channel_nth(start_input,
				k * audio_channels + j);

	recall_channel = ags_recall_template_find_provider(start_recall_channel, (GObject *) input);

	if(recall_channel != NULL){
	  ags_fx_lv2_channel_load_port(recall_channel->data);
	}

	if(input != NULL){
	  g_object_unref(input);
	}
      }
    }
  }

  g_rec_mutex_unlock(recall_mutex);
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(recall_container != NULL){
    g_object_unref(recall_container);
  }
}

void
ags_fx_lv2_audio_set_pads_callback(AgsAudio *audio,
				   GType channel_type,
				   guint pads, guint pads_old,
				   AgsFxLv2Audio *fx_lv2_audio)
{
  AgsChannel *start_input;
  AgsRecallContainer *recall_container;
  
  GList *start_recall_channel, *recall_channel;
  
  guint audio_channels;
  guint j, k;
  gboolean is_live_instrument;
  
  GRecMutex *recall_mutex;

  if(!g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_audio);

  /* get LV2 plugin and port */
  g_rec_mutex_lock(recall_mutex);
  
  g_rec_mutex_unlock(recall_mutex);

  is_live_instrument = ags_fx_lv2_audio_test_flags(fx_lv2_audio, AGS_FX_LV2_AUDIO_LIVE_INSTRUMENT);

  if(is_live_instrument){
    return;
  }

  start_input = NULL;

  audio_channels = 0;
  
  g_object_get(audio,
	       "input", &start_input,
	       "audio-channels", &audio_channels,
	       NULL);

  recall_container = NULL;
  
  g_object_get(fx_lv2_audio,
	       "recall-container", &recall_container,
	       NULL);

  start_recall_channel = ags_recall_container_get_recall_channel(recall_container);
  
  /* connect */
  if(pads_old < pads){
    for(j = 0; j < audio_channels; j++){
      for(k = pads_old; k < pads; k++){
	AgsChannel *input;

	input = ags_channel_nth(start_input,
				k * audio_channels + j);

	recall_channel = ags_recall_template_find_provider(start_recall_channel, (GObject *) input);

	if(recall_channel != NULL){
	  ags_fx_lv2_channel_load_port(recall_channel->data);
	}

	if(input != NULL){
	  g_object_unref(input);
	}
      }
    }
  }

  /* disconnect */
  if(pads < pads_old){
    //NOTE:JK: this won't work see AgsFxLv2Channel::finalize()
  }  

  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(recall_container != NULL){
    g_object_unref(recall_container);
  }
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

  g_rec_mutex_init(&(scope_data->strct_mutex));

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

  g_rec_mutex_init(&(channel_data->strct_mutex));

  channel_data->parent = NULL;
  
  channel_data->event_count = 0;

  channel_data->output = NULL;
  channel_data->input = NULL;

  channel_data->event_port = NULL;
  channel_data->atom_port = NULL;

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

  ags_lv2_plugin_event_buffer_free(channel_data->event_port);
  ags_lv2_plugin_atom_sequence_free(channel_data->atom_port);
  
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
      deactivate(channel_data->lv2_handle[0]);
    }

    if(cleanup != NULL){
      cleanup(channel_data->lv2_handle[0]);
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

  g_rec_mutex_init(&(input_data->strct_mutex));

  input_data->parent = NULL;

  input_data->output = NULL;
  input_data->input = NULL;

  input_data->event_port = NULL;
  input_data->atom_port = NULL;

  input_data->lv2_handle = NULL;

  input_data->event_buffer = (snd_seq_event_t *) g_malloc(sizeof(snd_seq_event_t));

  input_data->event_buffer->type = SND_SEQ_EVENT_NOTEON;

  input_data->event_buffer->data.note.channel = 0;
  input_data->event_buffer->data.note.note = 0;
  input_data->event_buffer->data.note.velocity = 127;

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

  ags_lv2_plugin_event_buffer_free(input_data->event_port);
  ags_lv2_plugin_atom_sequence_free(input_data->atom_port);

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
      deactivate(input_data->lv2_handle[0]);
    }

    if(cleanup != NULL){
      cleanup(input_data->lv2_handle[0]);
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

/**
 * ags_fx_lv2_audio_load_plugin:
 * @fx_lv2_audio: the #AgsFxLv2Audio
 * 
 * Load plugin of @fx_lv2_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_lv2_audio_load_plugin(AgsFxLv2Audio *fx_lv2_audio)
{
  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;

  gchar *filename, *effect;

  guint buffer_size;
  guint samplerate;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_LV2_AUDIO(fx_lv2_audio)){
    return;
  }

  lv2_manager = ags_lv2_manager_get_instance();
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_audio);
  
  /* get filename and effect */
  filename = NULL;
  effect = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  g_object_get(fx_lv2_audio,
	       "filename", &filename,
	       "effect", &effect,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  /* check if already loaded */
  g_rec_mutex_lock(recall_mutex);

  lv2_plugin = fx_lv2_audio->lv2_plugin;

  if(lv2_plugin == NULL){
    lv2_plugin =
      fx_lv2_audio->lv2_plugin = ags_lv2_manager_find_lv2_plugin(lv2_manager,
								 filename, effect);
    g_object_ref(lv2_plugin);
  }    
  
  g_rec_mutex_unlock(recall_mutex);

  if(lv2_plugin == NULL ||
     !ags_base_plugin_test_flags((AgsBasePlugin *) lv2_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    g_free(filename);
    g_free(effect);
    
    return;
  }
    
  if(lv2_plugin != NULL){
    guint i, j, k;
    gboolean is_live_instrument;

    is_live_instrument = ags_fx_lv2_audio_test_flags(fx_lv2_audio, AGS_FX_LV2_AUDIO_LIVE_INSTRUMENT);
    
    /* set lv2 plugin */    
    g_rec_mutex_lock(recall_mutex);

    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsFxLv2AudioScopeData *scope_data;

      scope_data = fx_lv2_audio->scope_data[i];
    
      if(i == AGS_SOUND_SCOPE_PLAYBACK ||
	 i == AGS_SOUND_SCOPE_NOTATION ||
	 i == AGS_SOUND_SCOPE_MIDI){
	for(j = 0; j < scope_data->audio_channels; j++){
	  AgsFxLv2AudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];

	  if(is_live_instrument){
	    if(channel_data->lv2_handle == NULL){
	      channel_data->lv2_handle = ags_base_plugin_instantiate((AgsBasePlugin *) lv2_plugin,
								     samplerate, buffer_size);
	    }
	  }
	
	  if(!is_live_instrument){
	    for(k = 0; k < AGS_SEQUENCER_MAX_MIDI_KEYS; k++){
	      AgsFxLv2AudioInputData *input_data;

	      input_data = channel_data->input_data[k];

	      if(input_data->lv2_handle == NULL){
		input_data->lv2_handle = ags_base_plugin_instantiate((AgsBasePlugin *) lv2_plugin,
								     samplerate, buffer_size);
	      }
	    }
	  }
	}
      }
    }
    
    g_rec_mutex_unlock(recall_mutex);
  }
  
  g_free(filename);
  g_free(effect);
}

void
ags_fx_lv2_audio_load_port(AgsFxLv2Audio *fx_lv2_audio)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsRecallContainer *recall_container;
  AgsPort **lv2_port;

  AgsLv2Plugin *lv2_plugin;

  GList *start_recall_channel, *recall_channel;
  GList *start_plugin_port, *plugin_port;

  guint *output_port;
  guint *input_port;
  
  guint input_pads;
  guint audio_channels;
  guint output_port_count, input_port_count;
  guint control_port_count;
  guint event_port;
  guint atom_port;
  guint buffer_size;
  guint nth;
  guint i, j, k;
  gboolean is_live_instrument;
  gboolean has_atom_port;
  gboolean has_event_port;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_LV2_AUDIO(fx_lv2_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_audio);

  g_rec_mutex_lock(recall_mutex);

  if(fx_lv2_audio->lv2_port != NULL){
    g_rec_mutex_unlock(recall_mutex);
    
    return;
  }

  lv2_plugin = fx_lv2_audio->lv2_plugin;

  g_rec_mutex_unlock(recall_mutex);
  
  if(lv2_plugin == NULL ||
     !ags_base_plugin_test_flags((AgsBasePlugin *) lv2_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    return;
  }

  audio = NULL;

  recall_container = NULL;

  g_object_get(fx_lv2_audio,
	       "audio", &audio,
	       "recall-container", &recall_container,
	       "buffer-size", &buffer_size,
	       NULL);

  start_recall_channel = ags_recall_container_get_recall_channel(recall_container);

  start_input = NULL;

  input_pads = 0;
  audio_channels = 0;
  
  g_object_get(audio,
	       "input", &start_input,
	       "input-pads", &input_pads,
	       "audio-channels", &audio_channels,
	       NULL);
  
  /* get LV2 plugin and port */
  g_rec_mutex_lock(recall_mutex);

  lv2_port = fx_lv2_audio->lv2_port;
  
  g_rec_mutex_unlock(recall_mutex);
  
  g_object_get(lv2_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  /* get control port count */
  plugin_port = start_plugin_port;

  output_port = NULL;
  input_port = NULL;
  
  output_port_count = 0;
  input_port_count = 0;

  has_event_port = FALSE;
  event_port = 0;
  
  has_atom_port = FALSE;
  atom_port = 0;
  
  control_port_count = 0;

  is_live_instrument = ags_fx_lv2_audio_test_flags(fx_lv2_audio, AGS_FX_LV2_AUDIO_LIVE_INSTRUMENT);
  
  while(plugin_port != NULL){
    guint port_index;

    g_object_get(plugin_port->data,
		 "port-index", &port_index,
		 NULL);

    if(ags_plugin_port_test_flags(plugin_port->data,
				  AGS_PLUGIN_PORT_CONTROL)){
      control_port_count++;
    }else if(ags_plugin_port_test_flags(plugin_port->data,
					AGS_PLUGIN_PORT_AUDIO)){
      if(ags_plugin_port_test_flags(plugin_port->data,
				    AGS_PLUGIN_PORT_INPUT)){
	if(input_port == NULL){
	  input_port = (guint *) g_malloc(sizeof(guint));	  
	}else{
	  input_port = (guint *) g_realloc(input_port,
					   (input_port_count + 1) * sizeof(guint));
	}

	input_port[input_port_count] = port_index;
	input_port_count++;
      }else if(ags_plugin_port_test_flags(plugin_port->data,
					  AGS_PLUGIN_PORT_OUTPUT)){
	if(output_port == NULL){
	  output_port = (guint *) g_malloc(sizeof(guint));	  
	}else{
	  output_port = (guint *) g_realloc(output_port,
					    (output_port_count + 1) * sizeof(guint));
	}

	output_port[output_port_count] = port_index;
	output_port_count++;
      }
    }
    
    if(ags_plugin_port_test_flags(plugin_port->data,
				  AGS_PLUGIN_PORT_INPUT)){
      if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_EVENT)){
	has_event_port = TRUE;

	event_port = port_index;
      }else if(ags_plugin_port_test_flags(plugin_port->data, AGS_PLUGIN_PORT_ATOM)){
	has_atom_port = TRUE;

	atom_port = port_index;
      }
    }

    plugin_port = plugin_port->next;
  }

  g_rec_mutex_lock(recall_mutex);

  fx_lv2_audio->output_port_count = output_port_count;
  fx_lv2_audio->output_port = output_port;

  fx_lv2_audio->input_port_count = input_port_count;
  fx_lv2_audio->input_port = input_port;

  fx_lv2_audio->has_event_port = has_event_port;
  fx_lv2_audio->event_port = event_port;

  fx_lv2_audio->has_atom_port = has_atom_port;
  fx_lv2_audio->atom_port = atom_port;
  
  g_rec_mutex_unlock(recall_mutex);

  /*  */
  if(is_live_instrument){
    if(control_port_count > 0){
      lv2_port = (AgsPort **) g_malloc((control_port_count + 1) * sizeof(AgsPort *));

      plugin_port = start_plugin_port;
    
      for(nth = 0; nth < control_port_count && plugin_port != NULL;){
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
	  plugin_name = g_strdup_printf("lv2-<%s>", lv2_plugin->uri);

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

	  /* lv2 port */
	  lv2_port[nth] = g_object_new(AGS_TYPE_PORT,
				       "plugin-name", plugin_name,
				       "specifier", specifier,
				       "control-port", control_port,
				       "port-value-is-pointer", FALSE,
				       "port-value-type", G_TYPE_FLOAT,
				       NULL);
      
	  if(ags_plugin_port_test_flags(current_plugin_port,
					AGS_PLUGIN_PORT_OUTPUT)){
	    ags_port_set_flags(lv2_port[nth], AGS_PORT_IS_OUTPUT);
	  
	    ags_recall_set_flags((AgsRecall *) fx_lv2_audio,
				 AGS_RECALL_HAS_OUTPUT_PORT);
	
	  }else{
	    if(!ags_plugin_port_test_flags(current_plugin_port,
					   AGS_PLUGIN_PORT_INTEGER) &&
	       !ags_plugin_port_test_flags(current_plugin_port,
					   AGS_PLUGIN_PORT_TOGGLED)){
	      ags_port_set_flags(lv2_port[nth], AGS_PORT_INFINITE_RANGE);
	    }
	  }
	
	  g_object_set(lv2_port[nth],
		       "plugin-port", current_plugin_port,
		       NULL);

	  ags_port_util_load_lv2_conversion(lv2_port[nth],
					    current_plugin_port);
	
	  ags_port_safe_write_raw(lv2_port[nth],
				  &default_value);

	  ags_recall_add_port((AgsRecall *) fx_lv2_audio,
			      lv2_port[nth]);
      
	  /* connect port */
	  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	    AgsFxLv2AudioScopeData *scope_data;

	    scope_data = fx_lv2_audio->scope_data[i];

	    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
	       i == AGS_SOUND_SCOPE_NOTATION ||
	       i == AGS_SOUND_SCOPE_MIDI){
	      for(j = 0; j < scope_data->audio_channels; j++){
		AgsFxLv2AudioChannelData *channel_data;

		channel_data = scope_data->channel_data[j];
	    
		ags_base_plugin_connect_port((AgsBasePlugin *) lv2_plugin,
					     channel_data->lv2_handle[0],
					     port_index,
					     &(lv2_port[nth]->port_value.ags_port_float));
	      }
	    }
	  }
      
	  g_free(plugin_name);
	  g_free(specifier);
	  g_free(control_port);

	  g_value_unset(&default_value);

	  nth++;
	}
      
	plugin_port = plugin_port->next;      
      }

      lv2_port[nth] = NULL;
    }
  }else{
    for(j = 0; j < audio_channels; j++){
      for(k = 0; k < input_pads; k++){
	AgsChannel *input;

	input = ags_channel_nth(start_input,
				k * audio_channels + j);

	recall_channel = ags_recall_template_find_provider(start_recall_channel, (GObject *) input);

	if(recall_channel != NULL){
	  ags_fx_lv2_channel_load_port(recall_channel->data);
	}

	if(input != NULL){
	  g_object_unref(input);
	}
      }
    }
  }

  /* set LV2 output */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxLv2AudioScopeData *scope_data;

    scope_data = fx_lv2_audio->scope_data[i];

    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxLv2AudioChannelData *channel_data;

	channel_data = scope_data->channel_data[j];

	if(is_live_instrument){	  
	  guint nth;
	  
	  if(channel_data->output == NULL &&
	     output_port_count > 0 &&
	     buffer_size > 0){
	    channel_data->output = (float *) g_malloc(output_port_count * buffer_size * sizeof(float));
	  }
	  
	  if(channel_data->input == NULL &&
	     input_port_count > 0 &&
	     buffer_size > 0){
	    channel_data->input = (float *) g_malloc(input_port_count * buffer_size * sizeof(float));
	  }

	  for(nth = 0; nth < output_port_count; nth++){
	    ags_base_plugin_connect_port((AgsBasePlugin *) lv2_plugin,
					 channel_data->lv2_handle[0],
					 output_port[nth],
					 &(channel_data->output[nth]));
	  }

	  for(nth = 0; nth < input_port_count; nth++){
	    ags_base_plugin_connect_port((AgsBasePlugin *) lv2_plugin,
					 channel_data->lv2_handle[0],
					 input_port[nth],
					 &(channel_data->input[nth]));
	  }

	  if(has_event_port){
	    channel_data->event_port = ags_lv2_plugin_event_buffer_alloc(AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT);
	    
	    ags_base_plugin_connect_port((AgsBasePlugin *) lv2_plugin,
					 channel_data->lv2_handle[0],
					 event_port,
					 channel_data->event_port);
	  }

	  if(has_atom_port){
	    channel_data->atom_port = ags_lv2_plugin_alloc_atom_sequence(AGS_FX_LV2_AUDIO_DEFAULT_MIDI_LENGHT);
	    
	    ags_base_plugin_connect_port((AgsBasePlugin *) lv2_plugin,
					 channel_data->lv2_handle[0],
					 atom_port,
					 channel_data->atom_port);
	  }

	  ags_base_plugin_activate((AgsBasePlugin *) lv2_plugin,
				   channel_data->lv2_handle[0]);
	}
      }
    }
  }
  
  fx_lv2_audio->lv2_port = lv2_port;
  
  g_rec_mutex_unlock(recall_mutex);

  /* unref */
  if(audio != NULL){
    g_object_unref(audio);
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(recall_container != NULL){
    g_object_unref(recall_container);
  }

  g_list_free_full(start_plugin_port,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_fx_lv2_audio_change_program:
 * @fx_lv2_audio: the #AgsFxLv2Audio
 * @bank_index: the bank index
 * @program_index: the program index
 * 
 * Change program of @fx_lv2_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_lv2_audio_change_program(AgsFxLv2Audio *fx_lv2_audio,
				guint bank_index,
				guint program_index)
{
  AgsLv2Plugin *lv2_plugin;

  GRecMutex *recall_mutex;
  
  if(!AGS_IS_FX_LV2_AUDIO(fx_lv2_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_audio);

  /* get LV2 plugin and port */
  g_rec_mutex_lock(recall_mutex);

  lv2_plugin = fx_lv2_audio->lv2_plugin;
  
  g_rec_mutex_unlock(recall_mutex);

  if(lv2_plugin == NULL ||
     !ags_base_plugin_test_flags((AgsBasePlugin *) lv2_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    return;
  }
  
  if(lv2_plugin != NULL){
    guint i, j, k;
    gboolean is_live_instrument;

    is_live_instrument = ags_fx_lv2_audio_test_flags(fx_lv2_audio, AGS_FX_LV2_AUDIO_LIVE_INSTRUMENT);
    
    /* set lv2 plugin */
    g_object_ref(lv2_plugin);
    
    g_rec_mutex_lock(recall_mutex);

    fx_lv2_audio->lv2_plugin = lv2_plugin;

    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsFxLv2AudioScopeData *scope_data;

      scope_data = fx_lv2_audio->scope_data[i];
    
      if(i == AGS_SOUND_SCOPE_PLAYBACK ||
	 i == AGS_SOUND_SCOPE_NOTATION ||
	 i == AGS_SOUND_SCOPE_MIDI){
	for(j = 0; j < scope_data->audio_channels; j++){
	  AgsFxLv2AudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];

	  if(is_live_instrument){
	    ags_lv2_plugin_change_program(lv2_plugin,
					  channel_data->lv2_handle[0],
					  bank_index,
					  program_index);
	  }
	
	  if(!is_live_instrument){
	    for(k = 0; k < AGS_SEQUENCER_MAX_MIDI_KEYS; k++){
	      AgsFxLv2AudioInputData *input_data;

	      input_data = channel_data->input_data[k];

	      ags_lv2_plugin_change_program(lv2_plugin,
					    input_data->lv2_handle[0],
					    bank_index,
					    program_index);
	    }
	  }
	}
      }
    }
    
    g_rec_mutex_unlock(recall_mutex);
  }
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
