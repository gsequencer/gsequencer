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

#include <ags/audio/fx/ags_fx_dssi_audio.h>

#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_dssi_plugin.h>
#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall_container.h>

void ags_fx_dssi_audio_class_init(AgsFxDssiAudioClass *fx_dssi_audio);
void ags_fx_dssi_audio_init(AgsFxDssiAudio *fx_dssi_audio);
void ags_fx_dssi_audio_dispose(GObject *gobject);
void ags_fx_dssi_audio_finalize(GObject *gobject);

void ags_fx_dssi_audio_notify_audio_callback(GObject *gobject,
					     GParamSpec *pspec,
					     gpointer user_data);
void ags_fx_dssi_audio_notify_buffer_size_callback(GObject *gobject,
						   GParamSpec *pspec,
						   gpointer user_data);
void ags_fx_dssi_audio_notify_samplerate_callback(GObject *gobject,
						  GParamSpec *pspec,
						  gpointer user_data);

void ags_fx_dssi_audio_set_audio_channels_callback(AgsAudio *audio,
						   guint audio_channels, guint audio_channels_old,
						   AgsFxDssiAudio *fx_dssi_audio);
void ags_fx_dssi_audio_set_pads_callback(AgsAudio *audio,
					 GType channel_type,
					 guint pads, guint pads_old,
					 AgsFxDssiAudio *fx_dssi_audio);

/**
 * SECTION:ags_fx_dssi_audio
 * @short_description: fx dssi audio
 * @title: AgsFxDssiAudio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_dssi_audio.h
 *
 * The #AgsFxDssiAudio class provides ports to the effect processor.
 */

static gpointer ags_fx_dssi_audio_parent_class = NULL;

static const gchar *ags_fx_dssi_audio_plugin_name = "ags-fx-dssi";

GType
ags_fx_dssi_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_dssi_audio = 0;

    static const GTypeInfo ags_fx_dssi_audio_info = {
      sizeof (AgsFxDssiAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_dssi_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxDssiAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_dssi_audio_init,
    };

    ags_type_fx_dssi_audio = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO,
						    "AgsFxDssiAudio",
						    &ags_fx_dssi_audio_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_dssi_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_dssi_audio_class_init(AgsFxDssiAudioClass *fx_dssi_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_dssi_audio_parent_class = g_type_class_peek_parent(fx_dssi_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_dssi_audio;

  gobject->dispose = ags_fx_dssi_audio_dispose;
  gobject->finalize = ags_fx_dssi_audio_finalize;
}

void
ags_fx_dssi_audio_init(AgsFxDssiAudio *fx_dssi_audio)
{
  guint i;

  g_signal_connect(fx_dssi_audio, "notify::audio",
		   G_CALLBACK(ags_fx_dssi_audio_notify_audio_callback), NULL);

  g_signal_connect(fx_dssi_audio, "notify::buffer-size",
		   G_CALLBACK(ags_fx_dssi_audio_notify_buffer_size_callback), NULL);

  g_signal_connect(fx_dssi_audio, "notify::samplerate",
		   G_CALLBACK(ags_fx_dssi_audio_notify_samplerate_callback), NULL);
  
  AGS_RECALL(fx_dssi_audio)->name = "ags-fx-dssi";
  AGS_RECALL(fx_dssi_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_dssi_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_dssi_audio)->xml_type = "ags-fx-dssi-audio";

  fx_dssi_audio->flags = AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT;
  
  fx_dssi_audio->output_port_count = 0;
  fx_dssi_audio->output_port = NULL;
  
  fx_dssi_audio->input_port_count = 0;
  fx_dssi_audio->input_port = NULL;

  fx_dssi_audio->bank = 0;
  fx_dssi_audio->program = 0;

  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      fx_dssi_audio->scope_data[i] = ags_fx_dssi_audio_scope_data_alloc();
      
      fx_dssi_audio->scope_data[i]->parent = fx_dssi_audio;
    }else{
      fx_dssi_audio->scope_data[i] = NULL;
    }
  }
  
  fx_dssi_audio->dssi_plugin = NULL;

  fx_dssi_audio->dssi_port = NULL;
}

void
ags_fx_dssi_audio_dispose(GObject *gobject)
{
  AgsFxDssiAudio *fx_dssi_audio;
  
  fx_dssi_audio = AGS_FX_DSSI_AUDIO(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_dssi_audio_parent_class)->dispose(gobject);
}

void
ags_fx_dssi_audio_finalize(GObject *gobject)
{
  AgsPort **iter;
  AgsFxDssiAudio *fx_dssi_audio;
    
  guint i;
  
  fx_dssi_audio = AGS_FX_DSSI_AUDIO(gobject);

  g_free(fx_dssi_audio->output_port);
  g_free(fx_dssi_audio->input_port);
  
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      ags_fx_dssi_audio_scope_data_free(fx_dssi_audio->scope_data[i]);
    }
  }
  
  if(fx_dssi_audio->dssi_plugin != NULL){
    g_object_unref(fx_dssi_audio->dssi_plugin);
  }
  
  if(fx_dssi_audio->dssi_port == NULL){
    for(iter = fx_dssi_audio->dssi_port; iter[0] != NULL; iter++){
      g_object_unref(iter[0]);
    }

    g_free(fx_dssi_audio->dssi_port);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_dssi_audio_parent_class)->finalize(gobject);
}

void
ags_fx_dssi_audio_notify_audio_callback(GObject *gobject,
					GParamSpec *pspec,
					gpointer user_data)
{
  AgsAudio *audio;
  AgsFxDssiAudio *fx_dssi_audio;

  GRecMutex *recall_mutex;
  
  fx_dssi_audio = AGS_FX_DSSI_AUDIO(gobject);

  /* get audio */
  audio = NULL;

  g_object_get(fx_dssi_audio,
	       "audio", &audio,
	       NULL);

  g_signal_connect_after((GObject *) audio, "set-audio-channels",
			 G_CALLBACK(ags_fx_dssi_audio_set_audio_channels_callback), fx_dssi_audio);

  g_signal_connect_after((GObject *) audio, "set-pads",
			 G_CALLBACK(ags_fx_dssi_audio_set_pads_callback), fx_dssi_audio);
}

void
ags_fx_dssi_audio_notify_buffer_size_callback(GObject *gobject,
					      GParamSpec *pspec,
					      gpointer user_data)
{
  AgsFxDssiAudio *fx_dssi_audio;

  guint output_port_count;
  guint buffer_size;
  guint i, j, k;
  gboolean is_live_instrument;
  
  GRecMutex *recall_mutex;
  
  fx_dssi_audio = AGS_FX_DSSI_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  /* get buffer size */
  g_object_get(fx_dssi_audio,
	       "buffer-size", &buffer_size,
	       NULL);

  is_live_instrument = ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  output_port_count = fx_dssi_audio->output_port_count;

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxDssiAudioScopeData *scope_data;

    scope_data = fx_dssi_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxDssiAudioChannelData *channel_data;

	channel_data = scope_data->channel_data[j];

	if(is_live_instrument){
	  if(channel_data->output == NULL){
	    channel_data->output = (LADSPA_Data *) g_malloc(output_port_count * buffer_size * sizeof(LADSPA_Data));
	  }else{
	    channel_data->output = (LADSPA_Data *) g_realloc(channel_data->output,
							     output_port_count * buffer_size * sizeof(LADSPA_Data));	    
	  }
	}
	
	if(!is_live_instrument){
	  for(k = 0; k < 128; k++){
	    AgsFxDssiAudioInputData *input_data;

	    input_data = channel_data->input_data[k];
	    
	    if(input_data->output == NULL){
	      input_data->output = (LADSPA_Data *) g_malloc(output_port_count * buffer_size * sizeof(LADSPA_Data));
	    }else{
	      input_data->output = (LADSPA_Data *) g_realloc(input_data->output,
							     output_port_count * buffer_size * sizeof(LADSPA_Data));	    
	    }	    
	  }
	}
      }
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_dssi_audio_notify_samplerate_callback(GObject *gobject,
					     GParamSpec *pspec,
					     gpointer user_data)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsRecallContainer *recall_container;

  AgsFxDssiAudio *fx_dssi_audio;

  AgsDssiPlugin *dssi_plugin;

  GList *start_recall_channel, *recall_channel;

  guint input_pads;
  guint audio_channels;
  guint output_port_count;
  guint buffer_size;
  guint samplerate;
  guint i, j, k;
  gboolean is_live_instrument;
  
  GRecMutex *recall_mutex;
  GRecMutex *base_plugin_mutex;

  void (*deactivate)(LADSPA_Handle Instance);
  void (*cleanup)(LADSPA_Handle Instance);
  
  fx_dssi_audio = AGS_FX_DSSI_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  g_object_get(fx_dssi_audio,
	       "audio", &audio,
	       "audio-channels", &audio_channels,
	       NULL);

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
  
  g_object_get(fx_dssi_audio,
	       "recall-container", &recall_container,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);
  
  start_recall_channel = ags_recall_container_get_recall_channel(recall_container);

  /* get DSSI plugin */
  g_rec_mutex_lock(recall_mutex);

  output_port_count = fx_dssi_audio->output_port_count;

  dssi_plugin = fx_dssi_audio->dssi_plugin;

  g_rec_mutex_unlock(recall_mutex);

  /* get deactivate and cleanup */
  base_plugin_mutex = NULL;

  deactivate = NULL;
  cleanup = NULL;
  
  if(dssi_plugin != NULL){
    gpointer plugin_descriptor;
    
    base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(dssi_plugin);
      
    g_rec_mutex_lock(base_plugin_mutex);
  
    plugin_descriptor = AGS_BASE_PLUGIN(dssi_plugin)->plugin_descriptor;

    deactivate = AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate;
    cleanup = AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup;
      
    g_rec_mutex_unlock(base_plugin_mutex);
  }

  /* get buffer size */
  g_object_get(fx_dssi_audio,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  is_live_instrument = ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxDssiAudioScopeData *scope_data;

    scope_data = fx_dssi_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxDssiAudioChannelData *channel_data;

	channel_data = scope_data->channel_data[j];

	if(is_live_instrument){
	  AgsPort **iter;
	  
	  guint nth;

	  if(deactivate != NULL){
	    deactivate(channel_data->ladspa_handle);
	  }

	  if(cleanup != NULL){
	    cleanup(channel_data->ladspa_handle);
	  }	  

	  channel_data->ladspa_handle = ags_base_plugin_instantiate(dssi_plugin,
								    samplerate, buffer_size);

	  for(nth = 0; nth < output_port_count; nth++){
	    ags_base_plugin_connect_port(dssi_plugin,
					 channel_data->ladspa_handle,
					 fx_dssi_audio->output_port[nth],
					 &(channel_data->output[nth]));
	  }

	  for(iter = fx_dssi_audio->dssi_port; iter[0] != NULL; iter++){
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
	    
	    ags_base_plugin_connect_port(dssi_plugin,
					 channel_data->ladspa_handle,
					 port_index,
					 &(iter[0]->port_value.ags_port_ladspa));

	    if(plugin_port != NULL){
	      g_object_unref(plugin_port);
	    }
	  }
	}
	
	if(!is_live_instrument){
	  for(k = 0; k < 128; k++){
	    AgsFxDssiAudioInputData *input_data;

	    input_data = channel_data->input_data[k];

	    if(deactivate != NULL){
	      deactivate(input_data->ladspa_handle);
	    }

	    if(cleanup != NULL){
	      cleanup(input_data->ladspa_handle);
	    }

	    input_data->ladspa_handle = ags_base_plugin_instantiate(dssi_plugin,
								    samplerate, buffer_size);

	    if(k < input_pads){
	      AgsChannel *input;
	    
	      AgsFxDssiAudioInputData *input_data;
	  
	      input = ags_channel_nth(start_input,
				      k * audio_channels + j);

	      recall_channel = ags_recall_template_find_provider(start_recall_channel, input);

	      if(recall_channel != NULL){
		ags_fx_dssi_channel_load_port(recall_channel->data);
	      }

	      if(input != NULL){
		g_object_unref(input);
	      }
	    }
	  }
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

  g_list_free_full(start_recall_channel,
		   g_object_unref);
}

void
ags_fx_dssi_audio_set_audio_channels_callback(AgsAudio *audio,
					      guint audio_channels, guint audio_channels_old,
					      AgsFxDssiAudio *fx_dssi_audio)
{
  AgsChannel *start_input;
  AgsRecallContainer *recall_container;

  AgsDssiPlugin *dssi_plugin;

  GList *start_recall_channel, *recall_channel;

  guint input_pads;
  guint output_port_count;
  guint buffer_size;
  guint samplerate;
  guint i, j, k;
  gboolean is_live_instrument;

  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

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
  
  g_object_get(fx_dssi_audio,
	       "recall-container", &recall_container,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);
  
  start_recall_channel = ags_recall_container_get_recall_channel(recall_container);

  /* get DSSI plugin */
  g_rec_mutex_lock(recall_mutex);

  output_port_count = fx_dssi_audio->output_port_count;

  dssi_plugin = fx_dssi_audio->dssi_plugin;

  g_rec_mutex_unlock(recall_mutex);

  /* allocate channel data */
  is_live_instrument = ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT);
    
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxDssiAudioScopeData *scope_data;

    scope_data = fx_dssi_audio->scope_data[i];

    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      if(scope_data->audio_channels > audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxDssiAudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];
	
	  ags_fx_dssi_audio_channel_data_free(channel_data);
	}
      }
      
      if(scope_data->channel_data == NULL){
	scope_data->channel_data = (AgsFxDssiAudioChannelData **) g_malloc(audio_channels * sizeof(AgsFxDssiAudioChannelData *)); 
      }else{
	scope_data->channel_data = (AgsFxDssiAudioChannelData **) g_realloc(scope_data->channel_data,
									    audio_channels * sizeof(AgsFxDssiAudioChannelData *)); 
      }

      if(scope_data->audio_channels < audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxDssiAudioChannelData *channel_data;

	  channel_data =
	    scope_data->channel_data[j] = ags_fx_dssi_audio_channel_data_alloc();

	  if(is_live_instrument){
	    AgsPort **iter;
	  
	    guint nth;

	    channel_data->ladspa_handle = ags_base_plugin_instantiate(dssi_plugin,
								      samplerate, buffer_size);
	  
	    if(channel_data->output == NULL){
	      channel_data->output = (LADSPA_Data *) g_malloc(output_port_count * buffer_size * sizeof(LADSPA_Data));
	    }

	    for(nth = 0; nth < output_port_count; nth++){
	      ags_base_plugin_connect_port(dssi_plugin,
					   channel_data->ladspa_handle,
					   fx_dssi_audio->output_port[nth],
					   &(channel_data->output[nth]));
	    }

	    for(iter = fx_dssi_audio->dssi_port; iter[0] != NULL; iter++){
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
	    
	      ags_base_plugin_connect_port(dssi_plugin,
					   channel_data->ladspa_handle,
					   port_index,
					   &(iter[0]->port_value.ags_port_ladspa));

	      if(plugin_port != NULL){
		g_object_unref(plugin_port);
	      }
	    }
	  }
	
	  if(!is_live_instrument){
	    for(k = 0; k < 128; k++){
	      AgsFxDssiAudioInputData *input_data;

	      input_data = channel_data->input_data[k];

	      input_data->ladspa_handle = ags_base_plugin_instantiate(dssi_plugin,
								      samplerate, buffer_size);
	    
	      if(input_data->output == NULL){
		input_data->output = (LADSPA_Data *) g_malloc(output_port_count * buffer_size * sizeof(LADSPA_Data));
	      }

	      if(k < input_pads){
		AgsChannel *input;
	    
		AgsFxDssiAudioInputData *input_data;
	  
		input = ags_channel_nth(start_input,
					k * audio_channels + j);

		recall_channel = ags_recall_template_find_provider(start_recall_channel, input);

		if(recall_channel != NULL){
		  ags_fx_dssi_channel_load_port(recall_channel->data);
		}

		if(input != NULL){
		  g_object_unref(input);
		}
	      }
	    }
	  }
	}
      }
      
      scope_data->audio_channels = audio_channels;
    }
  }  

  g_rec_mutex_unlock(recall_mutex);
  
  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(recall_container != NULL){
    g_object_unref(recall_container);
  }

  g_list_free_full(start_recall_channel,
		   g_object_unref);
}

void
ags_fx_dssi_audio_set_pads_callback(AgsAudio *audio,
				    GType channel_type,
				    guint pads, guint pads_old,
				    AgsFxDssiAudio *fx_dssi_audio)
{
  AgsChannel *start_input;
  AgsRecallContainer *recall_container;
  
  AgsDssiPlugin *dssi_plugin;

  GList *start_recall_channel, *recall_channel;
  
  guint audio_channels;
  guint i, j, k;
  gboolean is_live_instrument;
  
  GRecMutex *recall_mutex;

  if(!g_type_is_a(channel_type, AGS_TYPE_INPUT)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  /* get DSSI plugin and port */
  g_rec_mutex_lock(recall_mutex);

  dssi_plugin = fx_dssi_audio->dssi_plugin;
  
  g_rec_mutex_unlock(recall_mutex);

  is_live_instrument = ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT);

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
  
  g_object_get(fx_dssi_audio,
	       "recall-container", &recall_container,
	       NULL);

  start_recall_channel = ags_recall_container_get_recall_channel(recall_container);
  
  /* connect */
  if(pads_old < pads){
    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsFxDssiAudioScopeData *scope_data;

      scope_data = fx_dssi_audio->scope_data[i];

      if(i == AGS_SOUND_SCOPE_PLAYBACK ||
	 i == AGS_SOUND_SCOPE_NOTATION ||
	 i == AGS_SOUND_SCOPE_MIDI){
	for(j = 0; j < scope_data->audio_channels; j++){
	  AgsFxDssiAudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];

	  for(k = pads_old; k < pads; k++){
	    AgsChannel *input;
	    
	    AgsFxDssiAudioInputData *input_data;
	  
	    input_data = channel_data->input_data[k];

	    input = ags_channel_nth(start_input,
				    k * audio_channels + j);

	    recall_channel = ags_recall_template_find_provider(start_recall_channel, input);

	    if(recall_channel != NULL){
	      ags_fx_dssi_channel_load_port(recall_channel->data);
	    }

	    if(input != NULL){
	      g_object_unref(input);
	    }
	  }
	}
      }
    }
  }

  /* disconnect */
  if(pads < pads_old){
    //NOTE:JK: this won't work see AgsFxDssiChannel::finalize()
  }  

  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(recall_container != NULL){
    g_object_unref(recall_container);
  }

  g_list_free_full(start_recall_channel,
		   g_object_unref);
}

/**
 * ags_fx_dssi_audio_scope_data_alloc:
 * 
 * Allocate #AgsFxDssiAudioScopeData-struct
 * 
 * Returns: the new #AgsFxDssiAudioScopeData-struct
 * 
 * Since: 3.3.0
 */
AgsFxDssiAudioScopeData*
ags_fx_dssi_audio_scope_data_alloc()
{
  AgsFxDssiAudioScopeData *scope_data;

  scope_data = (AgsFxDssiAudioScopeData *) g_malloc(sizeof(AgsFxDssiAudioScopeData));

  scope_data->parent = NULL;
  
  scope_data->audio_channels = 0;

  scope_data->channel_data = NULL;
  
  return(scope_data);
}

/**
 * ags_fx_dssi_audio_scope_data_free:
 * @scope_data: the #AgsFxDssiAudioScopeData-struct
 * 
 * Free @scope_data.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_audio_scope_data_free(AgsFxDssiAudioScopeData *scope_data)
{
  guint i;

  if(scope_data == NULL){
    return;
  }

  for(i = 0; i < scope_data->audio_channels; i++){
    ags_fx_dssi_audio_channel_data_free(scope_data->channel_data[i]);
  }

  g_free(scope_data);
}

/**
 * ags_fx_dssi_audio_channel_data_alloc:
 * 
 * Allocate #AgsFxDssiAudioChannelData-struct
 * 
 * Returns: the new #AgsFxDssiAudioChannelData-struct
 * 
 * Since: 3.3.0
 */
AgsFxDssiAudioChannelData*
ags_fx_dssi_audio_channel_data_alloc()
{
  AgsFxDssiAudioChannelData *channel_data;

  guint i;
  
  channel_data = (AgsFxDssiAudioChannelData *) g_malloc(sizeof(AgsFxDssiAudioChannelData));

  channel_data->parent = NULL;
  
  channel_data->event_count = 0;

  channel_data->output = NULL;
  channel_data->input = NULL;

  channel_data->ladspa_handle = NULL;

  for(i = 0; i < 128; i++){
    channel_data->input_data[i] = ags_fx_dssi_audio_input_data_alloc();

    channel_data->input_data[i]->parent = channel_data;
  }

  return(channel_data);
}

/**
 * ags_fx_dssi_audio_channel_data_free:
 * @channel_data: the #AgsFxDssiAudioChannelData-struct
 * 
 * Free @channel_data.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_audio_channel_data_free(AgsFxDssiAudioChannelData *channel_data)
{
  AgsDssiPlugin *dssi_plugin;

  guint i;

  if(channel_data == NULL){
    return;
  }

  g_free(channel_data->output);
  g_free(channel_data->input);

  if(channel_data->ladspa_handle != NULL){
    gpointer plugin_descriptor;
    
    GRecMutex *base_plugin_mutex;
    
    void (*deactivate)(LADSPA_Handle Instance);
    void (*cleanup)(LADSPA_Handle Instance);
  
    dssi_plugin = NULL;
    
    deactivate = NULL;
    cleanup = NULL;

    base_plugin_mutex = NULL;
    
    if(channel_data->parent != NULL &&
       AGS_FX_DSSI_AUDIO_SCOPE_DATA(channel_data->parent)->parent != NULL){
      dssi_plugin = AGS_FX_DSSI_AUDIO(AGS_FX_DSSI_AUDIO_SCOPE_DATA(channel_data->parent)->parent)->dssi_plugin;
    }
    
    if(dssi_plugin != NULL){
      base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(dssi_plugin);
      
      g_rec_mutex_lock(base_plugin_mutex);
  
      plugin_descriptor = AGS_BASE_PLUGIN(dssi_plugin)->plugin_descriptor;

      deactivate = AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate;
      cleanup = AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup;
      
      g_rec_mutex_unlock(base_plugin_mutex);
    }

    if(deactivate != NULL){
      deactivate(channel_data->ladspa_handle);
    }

    if(cleanup != NULL){
      cleanup(channel_data->ladspa_handle);
    }
  }
  
  for(i = 0; i < 128; i++){
    ags_fx_dssi_audio_input_data_free(channel_data->input_data[i]);
  }

  g_free(channel_data);
}

/**
 * ags_fx_dssi_audio_input_data_alloc:
 * 
 * Allocate #AgsFxDssiAudioInputData-struct
 * 
 * Returns: the new #AgsFxDssiAudioInputData-struct
 * 
 * Since: 3.3.0
 */
AgsFxDssiAudioInputData*
ags_fx_dssi_audio_input_data_alloc()
{
  AgsFxDssiAudioInputData *input_data;

  input_data = (AgsFxDssiAudioInputData *) g_malloc(sizeof(AgsFxDssiAudioInputData));

  input_data->parent = NULL;

  input_data->output = NULL;
  input_data->input = NULL;

  input_data->ladspa_handle = NULL;

  input_data->event_buffer = (snd_seq_event_t *) g_malloc(sizeof(snd_seq_event_t));

  input_data->key_on = 0;
  
  return(input_data);
}

/**
 * ags_fx_dssi_audio_input_data_free:
 * @input_data: the #AgsFxDssiAudioInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_audio_input_data_free(AgsFxDssiAudioInputData *input_data)
{
  AgsDssiPlugin *dssi_plugin;

  gpointer plugin_descriptor;

  void (*deactivate)(LADSPA_Handle Instance);
  void (*cleanup)(LADSPA_Handle Instance);
  
  if(input_data == NULL){
    return;
  }

  g_free(input_data->output);
  g_free(input_data->input);

  if(input_data->ladspa_handle != NULL){
    gpointer plugin_descriptor;
    
    GRecMutex *base_plugin_mutex;
    
    void (*deactivate)(LADSPA_Handle Instance);
    void (*cleanup)(LADSPA_Handle Instance);
  
    dssi_plugin = NULL;
    
    deactivate = NULL;
    cleanup = NULL;

    base_plugin_mutex = NULL;
    
    if(input_data->parent != NULL &&
       AGS_FX_DSSI_AUDIO_CHANNEL_DATA(input_data->parent)->parent != NULL &&
       AGS_FX_DSSI_AUDIO_SCOPE_DATA(AGS_FX_DSSI_AUDIO_CHANNEL_DATA(input_data->parent)->parent)->parent != NULL){
      dssi_plugin = AGS_FX_DSSI_AUDIO(AGS_FX_DSSI_AUDIO_SCOPE_DATA(AGS_FX_DSSI_AUDIO_CHANNEL_DATA(input_data->parent)->parent)->parent)->dssi_plugin;
    }
    
    if(dssi_plugin != NULL){
      base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(dssi_plugin);
      
      g_rec_mutex_lock(base_plugin_mutex);
  
      plugin_descriptor = AGS_BASE_PLUGIN(dssi_plugin)->plugin_descriptor;

      deactivate = AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate;
      cleanup = AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup;
      
      g_rec_mutex_unlock(base_plugin_mutex);
    }

    if(deactivate != NULL){
      deactivate(input_data->ladspa_handle);
    }

    if(cleanup != NULL){
      cleanup(input_data->ladspa_handle);
    }
  }
  
  g_free(input_data->event_buffer);

  g_free(input_data);
}

/**
 * ags_fx_dssi_audio_test_flags:
 * @fx_dssi_audio: the #AgsFxDssiAudio
 * @flags: the flags
 * 
 * Test @flags of @fx_dssi_audio.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.3.0
 */
gboolean
ags_fx_dssi_audio_test_flags(AgsFxDssiAudio *fx_dssi_audio, guint flags)
{
  gboolean success;
  
  GRecMutex *recall_mutex;
  
  if(!AGS_IS_FX_DSSI_AUDIO(fx_dssi_audio)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  /* test flags */
  g_rec_mutex_lock(recall_mutex);

  success = ((flags & (fx_dssi_audio->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(recall_mutex);

  return(success);
}

/**
 * ags_fx_dssi_audio_set_flags:
 * @fx_dssi_audio: the #AgsFxDssiAudio
 * @flags: the flags
 * 
 * Set @flags of @fx_dssi_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_audio_set_flags(AgsFxDssiAudio *fx_dssi_audio, guint flags)
{  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_DSSI_AUDIO(fx_dssi_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);
  
  /* set flags */
  g_rec_mutex_lock(recall_mutex);

  fx_dssi_audio->flags |= flags;

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_dssi_audio_unset_flags:
 * @fx_dssi_audio: the #AgsFxDssiAudio
 * @flags: the flags
 * 
 * Unset @flags of @fx_dssi_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_audio_unset_flags(AgsFxDssiAudio *fx_dssi_audio, guint flags)
{  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_DSSI_AUDIO(fx_dssi_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  /* set flags */
  g_rec_mutex_lock(recall_mutex);

  fx_dssi_audio->flags &= (~flags);

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_dssi_audio_load_plugin:
 * @fx_dssi_audio: the #AgsFxDssiAudio
 * 
 * Load plugin of @fx_dssi_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_audio_load_plugin(AgsFxDssiAudio *fx_dssi_audio)
{
  AgsDssiManager *dssi_manager;
  AgsDssiPlugin *dssi_plugin;

  gchar *filename, *effect;

  guint buffer_size;
  guint samplerate;
  guint effect_index;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_DSSI_AUDIO(fx_dssi_audio)){
    return;
  }

  dssi_manager = ags_dssi_manager_get_instance();
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  /* check if already loaded */
  g_rec_mutex_lock(recall_mutex);

  dssi_plugin = fx_dssi_audio->dssi_plugin;  
  
  g_rec_mutex_unlock(recall_mutex);

  if(dssi_plugin != NULL){
    return;
  }    
  
  /* get filename and effect */
  g_object_get(fx_dssi_audio,
	       "filename", &filename,
	       "effect", &effect,
	       "effect-index", &effect_index,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  dssi_plugin = ags_dssi_manager_find_dssi_plugin(dssi_manager,
						  filename, effect);    
    
  if(dssi_plugin != NULL){
    guint i, j, k;
    gboolean is_live_instrument;

    is_live_instrument = ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT);
    
    /* set dssi plugin */
    g_object_ref(dssi_plugin);
    
    g_rec_mutex_lock(recall_mutex);

    fx_dssi_audio->dssi_plugin = dssi_plugin;

    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsFxDssiAudioScopeData *scope_data;

      scope_data = fx_dssi_audio->scope_data[i];
    
      if(i == AGS_SOUND_SCOPE_PLAYBACK ||
	 i == AGS_SOUND_SCOPE_NOTATION ||
	 i == AGS_SOUND_SCOPE_MIDI){
	for(j = 0; j < scope_data->audio_channels; j++){
	  AgsFxDssiAudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];

	  if(is_live_instrument){
	    if(channel_data->ladspa_handle == NULL){
	      channel_data->ladspa_handle = ags_base_plugin_instantiate(dssi_plugin,
									samplerate, buffer_size);
	    }
	  }
	
	  if(!is_live_instrument){
	    for(k = 0; k < 128; k++){
	      AgsFxDssiAudioInputData *input_data;

	      input_data = channel_data->input_data[k];

	      if(input_data->ladspa_handle == NULL){
		input_data->ladspa_handle = ags_base_plugin_instantiate(dssi_plugin,
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

/**
 * ags_fx_dssi_audio_load_port:
 * @fx_dssi_audio: the #AgsFxDssiAudio
 * 
 * Load port of @fx_dssi_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_audio_load_port(AgsFxDssiAudio *fx_dssi_audio)
{
  AgsDssiPlugin *dssi_plugin;
  AgsPort **dssi_port, **iter;

  GList *start_plugin_port, *plugin_port;

  guint *output_port;
  guint *input_port;
  
  guint output_port_count;
  guint input_port_count;
  guint control_port_count;
  guint buffer_size;
  guint i, j, k;
  gboolean is_live_instrument;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_DSSI_AUDIO(fx_dssi_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  /* get DSSI plugin and port */
  g_rec_mutex_lock(recall_mutex);

  dssi_plugin = fx_dssi_audio->dssi_plugin;

  dssi_port = fx_dssi_audio->dssi_port;
  
  g_rec_mutex_unlock(recall_mutex);

  if(!ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT)){
    g_rec_mutex_lock(recall_mutex);
    
    fx_dssi_audio->dssi_port = NULL;
    
    g_rec_mutex_unlock(recall_mutex);

    if(dssi_port != NULL){
      for(iter = dssi_port; iter[0] != NULL; iter++){
	g_object_unref(iter[0]);
      }

      g_free(dssi_port);
    }
    
    return;
  }

  if(dssi_port != NULL ||
     dssi_plugin == NULL){
    return;
  }

  g_object_get(fx_dssi_audio,
	       "buffer-size", &buffer_size,
	       NULL);

  g_object_get(dssi_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  /* get control port count */
  plugin_port = start_plugin_port;

  output_port = NULL;
  input_port = NULL;
  
  output_port_count = 0;
  input_port_count = 0;

  control_port_count = 0;

  is_live_instrument = ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT);
  
  while(plugin_port != NULL){
    if(ags_plugin_port_test_flags(plugin_port->data,
				  AGS_PLUGIN_PORT_CONTROL)){
      control_port_count++;
    }else if(ags_plugin_port_test_flags(plugin_port->data,
					AGS_PLUGIN_PORT_AUDIO)){
      guint port_index;

      g_object_get(plugin_port->data,
		   "port-index", &port_index,
		   NULL);
      
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
  }

  /*  */
  if(!is_live_instrument &&
     control_port_count > 0){
    dssi_port = (AgsPort **) g_malloc((control_port_count + 1) * sizeof(AgsPort *));

    plugin_port = start_plugin_port;
    
    for(i = 0; i < control_port_count; i++){
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
      plugin_name = g_strdup_printf("dssi-%u", dssi_plugin->unique_id);

      g_object_get(current_plugin_port,
		   "port-name", &specifier,
		   "port-index", &port_index,
		   NULL);

      control_port = g_strdup_printf("%u/%u",
				     i,
				     control_port_count);

      /* default value */
      g_value_init(&default_value,
		   G_TYPE_FLOAT);
      
      g_rec_mutex_lock(plugin_port_mutex);
      
      g_value_copy(current_plugin_port->default_value,
		   &default_value);
      
      g_rec_mutex_unlock(plugin_port_mutex);

      /* dssi port */
      dssi_port[i] = g_object_new(AGS_TYPE_PORT,
				  "plugin-name", plugin_name,
				  "specifier", specifier,
				  "control-port", control_port,
				  "port-value-is-pointer", FALSE,
				  "port-value-type", G_TYPE_FLOAT,
				  NULL);
      ags_port_set_flags(dssi_port[i], AGS_PORT_USE_LADSPA_FLOAT);
      g_object_ref(dssi_port[i]);
      
      if(ags_plugin_port_test_flags(current_plugin_port,
				    AGS_PLUGIN_PORT_OUTPUT)){
	ags_port_set_flags(dssi_port[i], AGS_PORT_IS_OUTPUT);
	  
	ags_recall_set_flags((AgsRecall *) fx_dssi_audio,
			     AGS_RECALL_HAS_OUTPUT_PORT);
	
      }else{
	if(!ags_plugin_port_test_flags(current_plugin_port,
				       AGS_PLUGIN_PORT_INTEGER) &&
	   !ags_plugin_port_test_flags(current_plugin_port,
				       AGS_PLUGIN_PORT_TOGGLED)){
	  ags_port_set_flags(dssi_port[i], AGS_PORT_INFINITE_RANGE);
	}
      }
	
      g_object_set(dssi_port[i],
		   "plugin-port", current_plugin_port,
		   NULL);

      ags_port_util_load_ladspa_conversion(dssi_port[i],
					   current_plugin_port);
	
      ags_port_safe_write_raw(dssi_port[i],
			      &default_value);

      /* connect port */
      for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	AgsFxDssiAudioScopeData *scope_data;

	scope_data = fx_dssi_audio->scope_data[i];

	if(i == AGS_SOUND_SCOPE_PLAYBACK ||
	   i == AGS_SOUND_SCOPE_NOTATION ||
	   i == AGS_SOUND_SCOPE_MIDI){
	  for(j = 0; j < scope_data->audio_channels; j++){
	    AgsFxDssiAudioChannelData *channel_data;

	    channel_data =
	      scope_data->channel_data[j] = ags_fx_dssi_audio_channel_data_alloc();
	    
	    ags_base_plugin_connect_port(dssi_plugin,
					 channel_data->ladspa_handle,
					 port_index,
					 &(dssi_port[i]->port_value.ags_port_ladspa));
	  }
	}
      }
      
      g_free(plugin_name);
      g_free(specifier);
      g_free(control_port);

      g_value_unset(&default_value);
    }

    dssi_port[i] = NULL;
  }

  /* set DSSI output */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxDssiAudioScopeData *scope_data;

    scope_data = fx_dssi_audio->scope_data[i];

    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxDssiAudioChannelData *channel_data;

	channel_data = scope_data->channel_data[j];

	if(is_live_instrument){	  
	  guint nth;
	  
	  if(channel_data->output == NULL &&
	     output_port_count > 0 &&
	     buffer_size > 0){
	    channel_data->output = (LADSPA_Data *) g_malloc(output_port_count * buffer_size * sizeof(LADSPA_Data));
	  }

	  for(nth = 0; nth < output_port_count; nth++){
	    ags_base_plugin_connect_port(dssi_plugin,
					 channel_data->ladspa_handle,
					 output_port[nth],
					 &(channel_data->output[nth]));
	  }
	}

	if(!is_live_instrument){	  
	  for(k = 0; k < 128; k++){
	    AgsFxDssiAudioInputData *input_data;

	    guint nth;
	  
	    input_data = channel_data->input_data[k];
	  
	    if(input_data->output == NULL &&
	       output_port_count > 0 &&
	       buffer_size > 0){
	      input_data->output = (LADSPA_Data *) g_malloc(output_port_count * buffer_size * sizeof(LADSPA_Data));
	    }

	    for(nth = 0; nth < output_port_count; nth++){
	      ags_base_plugin_connect_port(dssi_plugin,
					   channel_data->ladspa_handle,
					   output_port[nth],
					   &(channel_data->output[nth]));
	    }
	  }
	}
      }
    }
  }  

  fx_dssi_audio->output_port_count = output_port_count;
  fx_dssi_audio->output_port = output_port;

  fx_dssi_audio->input_port_count = input_port_count;
  fx_dssi_audio->input_port = input_port;

  fx_dssi_audio->dssi_port = dssi_port;
  
  g_rec_mutex_unlock(recall_mutex);

  /* unref */
  g_list_free_full(start_plugin_port,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_fx_dssi_audio_change_program:
 * @fx_dssi_audio: the #AgsFxDssiAudio
 * @bank_index: the bank index
 * @program_index: the program index
 * 
 * Change program of @fx_dssi_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_audio_change_program(AgsFxDssiAudio *fx_dssi_audio,
				 guint bank_index,
				 guint program_index)
{
  AgsDssiPlugin *dssi_plugin;

  GRecMutex *recall_mutex;
  
  if(!AGS_IS_FX_DSSI_AUDIO(fx_dssi_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  /* get DSSI plugin and port */
  g_rec_mutex_lock(recall_mutex);

  dssi_plugin = fx_dssi_audio->dssi_plugin;
  
  g_rec_mutex_unlock(recall_mutex);
  
  if(dssi_plugin != NULL){
    guint i, j, k;
    gboolean is_live_instrument;

    is_live_instrument = ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT);
    
    /* set dssi plugin */
    g_object_ref(dssi_plugin);
    
    g_rec_mutex_lock(recall_mutex);

    fx_dssi_audio->dssi_plugin = dssi_plugin;

    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsFxDssiAudioScopeData *scope_data;

      scope_data = fx_dssi_audio->scope_data[i];
    
      if(i == AGS_SOUND_SCOPE_PLAYBACK ||
	 i == AGS_SOUND_SCOPE_NOTATION ||
	 i == AGS_SOUND_SCOPE_MIDI){
	for(j = 0; j < scope_data->audio_channels; j++){
	  AgsFxDssiAudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];

	  if(is_live_instrument){
	    ags_dssi_plugin_change_program(dssi_plugin,
					   channel_data->ladspa_handle,
					   bank_index,
					   program_index);
	  }
	
	  if(!is_live_instrument){
	    for(k = 0; k < 128; k++){
	      AgsFxDssiAudioInputData *input_data;

	      input_data = channel_data->input_data[k];

	      ags_dssi_plugin_change_program(dssi_plugin,
					     input_data->ladspa_handle,
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
 * ags_fx_dssi_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxDssiAudio
 *
 * Returns: the new #AgsFxDssiAudio
 *
 * Since: 3.3.0
 */
AgsFxDssiAudio*
ags_fx_dssi_audio_new(AgsAudio *audio)
{
  AgsFxDssiAudio *fx_dssi_audio;

  fx_dssi_audio = (AgsFxDssiAudio *) g_object_new(AGS_TYPE_FX_DSSI_AUDIO,
						  "audio", audio,
						  NULL);

  return(fx_dssi_audio);
}
