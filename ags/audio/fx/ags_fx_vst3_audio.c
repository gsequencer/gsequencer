/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/audio/fx/ags_fx_vst3_audio.h>

#include <ags/plugin/ags_vst3_manager.h>
#include <ags/plugin/ags_vst3_plugin.h>
#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_input.h>
#include <ags/audio/ags_recall_container.h>
#include <ags/audio/ags_port_util.h>

#include <ags/audio/fx/ags_fx_vst3_channel.h>

#include <ags/i18n.h>

void ags_fx_vst3_audio_class_init(AgsFxVst3AudioClass *fx_vst3_audio);
void ags_fx_vst3_audio_init(AgsFxVst3Audio *fx_vst3_audio);
void ags_fx_vst3_audio_dispose(GObject *gobject);
void ags_fx_vst3_audio_finalize(GObject *gobject);

void ags_fx_vst3_audio_notify_audio_callback(GObject *gobject,
					     GParamSpec *pspec,
					     gpointer user_data);
void ags_fx_vst3_audio_notify_filename_callback(GObject *gobject,
						GParamSpec *pspec,
						gpointer user_data);
void ags_fx_vst3_audio_notify_effect_callback(GObject *gobject,
					      GParamSpec *pspec,
					      gpointer user_data);
void ags_fx_vst3_audio_notify_buffer_size_callback(GObject *gobject,
						   GParamSpec *pspec,
						   gpointer user_data);
void ags_fx_vst3_audio_notify_samplerate_callback(GObject *gobject,
						  GParamSpec *pspec,
						  gpointer user_data);

void ags_fx_vst3_audio_set_audio_channels_callback(AgsAudio *audio,
						   guint audio_channels, guint audio_channels_old,
						   AgsFxVst3Audio *fx_vst3_audio);
void ags_fx_vst3_audio_set_pads_callback(AgsAudio *audio,
					 GType channel_type,
					 guint pads, guint pads_old,
					 AgsFxVst3Audio *fx_vst3_audio);

void ags_fx_vst3_audio_safe_write_callback(AgsPort *port, GValue *value,
					   AgsFxVst3Audio *fx_vst3_audio);

/**
 * SECTION:ags_fx_vst3_audio
 * @short_description: fx vst3 audio
 * @title: AgsFxVst3Audio
 * @section_id:
 * @include: ags/audio/fx/ags_fx_vst3_audio.h
 *
 * The #AgsFxVst3Audio class provides ports to the effect processor.
 */

static gpointer ags_fx_vst3_audio_parent_class = NULL;

const gchar *ags_fx_vst3_audio_plugin_name = "ags-fx-vst3";

const gchar *ags_fx_vst3_audio_specifier[] = {
  NULL,
};

const gchar *ags_fx_vst3_audio_control_port[] = {
  NULL,
};

enum{
  PROP_0,
};

GType
ags_fx_vst3_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_vst3_audio = 0;

    static const GTypeInfo ags_fx_vst3_audio_info = {
      sizeof (AgsFxVst3AudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_vst3_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsFxVst3Audio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_vst3_audio_init,
    };

    ags_type_fx_vst3_audio = g_type_register_static(AGS_TYPE_FX_NOTATION_AUDIO,
						    "AgsFxVst3Audio",
						    &ags_fx_vst3_audio_info,
						    0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_vst3_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_vst3_audio_class_init(AgsFxVst3AudioClass *fx_vst3_audio)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;

  ags_fx_vst3_audio_parent_class = g_type_class_peek_parent(fx_vst3_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_vst3_audio;

  gobject->dispose = ags_fx_vst3_audio_dispose;
  gobject->finalize = ags_fx_vst3_audio_finalize;

  /* properties */
}

void
ags_fx_vst3_audio_init(AgsFxVst3Audio *fx_vst3_audio)
{
  guint i;

  g_signal_connect(fx_vst3_audio, "notify::audio",
		   G_CALLBACK(ags_fx_vst3_audio_notify_audio_callback), NULL);

  g_signal_connect(fx_vst3_audio, "notify::filename",
		   G_CALLBACK(ags_fx_vst3_audio_notify_filename_callback), NULL);

  g_signal_connect(fx_vst3_audio, "notify::effect",
		   G_CALLBACK(ags_fx_vst3_audio_notify_effect_callback), NULL);

  g_signal_connect(fx_vst3_audio, "notify::buffer-size",
		   G_CALLBACK(ags_fx_vst3_audio_notify_buffer_size_callback), NULL);

  g_signal_connect(fx_vst3_audio, "notify::samplerate",
		   G_CALLBACK(ags_fx_vst3_audio_notify_samplerate_callback), NULL);

  AGS_RECALL(fx_vst3_audio)->name = "ags-fx-vst3";
  AGS_RECALL(fx_vst3_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_vst3_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_vst3_audio)->xml_type = "ags-fx-vst3-audio";

  fx_vst3_audio->flags = AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT;
  
  fx_vst3_audio->output_port_count = 0;
  fx_vst3_audio->output_port = NULL;
  
  fx_vst3_audio->input_port_count = 0;
  fx_vst3_audio->input_port = NULL;

  fx_vst3_audio->program_port_index = -1;
  fx_vst3_audio->program_param_id = -1;

  fx_vst3_audio->parameter_changes[0].param_id = ~0;
  
  /* scope data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      fx_vst3_audio->scope_data[i] = ags_fx_vst3_audio_scope_data_alloc();
      
      fx_vst3_audio->scope_data[i]->parent = fx_vst3_audio;
    }else{
      fx_vst3_audio->scope_data[i] = NULL;
    }
  }
  
  fx_vst3_audio->vst3_plugin = NULL;

  fx_vst3_audio->vst3_port = NULL;
}

void
ags_fx_vst3_audio_dispose(GObject *gobject)
{
  AgsFxVst3Audio *fx_vst3_audio;
  
  fx_vst3_audio = AGS_FX_VST3_AUDIO(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_fx_vst3_audio_parent_class)->dispose(gobject);
}

void
ags_fx_vst3_audio_finalize(GObject *gobject)
{
  AgsPort **iter;
  AgsFxVst3Audio *fx_vst3_audio;
    
  guint i;
  
  fx_vst3_audio = AGS_FX_VST3_AUDIO(gobject);

  g_free(fx_vst3_audio->output_port);
  g_free(fx_vst3_audio->input_port);
  
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      ags_fx_vst3_audio_scope_data_free(fx_vst3_audio->scope_data[i]);
    }
  }
  
  if(fx_vst3_audio->vst3_plugin != NULL){
    g_object_unref(fx_vst3_audio->vst3_plugin);
  }
  
  if(fx_vst3_audio->vst3_port != NULL){
    for(iter = fx_vst3_audio->vst3_port; iter[0] != NULL; iter++){
      g_object_unref(iter[0]);
    }

    g_free(fx_vst3_audio->vst3_port);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_vst3_audio_parent_class)->finalize(gobject);
}

void
ags_fx_vst3_audio_notify_audio_callback(GObject *gobject,
					GParamSpec *pspec,
					gpointer user_data)
{
  AgsAudio *audio;
  AgsFxVst3Audio *fx_vst3_audio;

  fx_vst3_audio = AGS_FX_VST3_AUDIO(gobject);

  /* get audio */
  audio = NULL;

  g_object_get(fx_vst3_audio,
	       "audio", &audio,
	       NULL);

  g_signal_connect_after((GObject *) audio, "set-audio-channels",
			 G_CALLBACK(ags_fx_vst3_audio_set_audio_channels_callback), fx_vst3_audio);

  g_signal_connect_after((GObject *) audio, "set-pads",
			 G_CALLBACK(ags_fx_vst3_audio_set_pads_callback), fx_vst3_audio);
}

void
ags_fx_vst3_audio_notify_filename_callback(GObject *gobject,
					   GParamSpec *pspec,
					   gpointer user_data)
{
  //TODO:JK: implement me
}

void
ags_fx_vst3_audio_notify_effect_callback(GObject *gobject,
					 GParamSpec *pspec,
					 gpointer user_data)
{
  AGS_FX_VST3_AUDIO(gobject)->vst3_plugin = ags_vst3_manager_find_vst3_plugin(ags_vst3_manager_get_instance(),
									      AGS_RECALL(gobject)->filename,
									      AGS_RECALL(gobject)->effect);
}

void
ags_fx_vst3_audio_notify_buffer_size_callback(GObject *gobject,
					      GParamSpec *pspec,
					      gpointer user_data)
{
  AgsFxVst3Audio *fx_vst3_audio;

  AgsVst3Plugin *vst3_plugin;
  
  guint output_port_count, input_port_count;
  guint buffer_size;
  guint i, j, k;
  gboolean is_live_instrument;
  
  GRecMutex *recall_mutex;
  
  fx_vst3_audio = AGS_FX_VST3_AUDIO(gobject);
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  /* get vst3 plugin */
  g_rec_mutex_lock(recall_mutex);

  vst3_plugin = fx_vst3_audio->vst3_plugin;

  if(vst3_plugin == NULL){
    vst3_plugin = 
      AGS_FX_VST3_AUDIO(gobject)->vst3_plugin = ags_vst3_manager_find_vst3_plugin(ags_vst3_manager_get_instance(),
										  AGS_RECALL(gobject)->filename,
										  AGS_RECALL(gobject)->effect);
  }
  
  g_rec_mutex_unlock(recall_mutex);

  if(vst3_plugin == NULL ||
     !ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    return;
  }
  
  /* get buffer size */
  g_object_get(fx_vst3_audio,
	       "buffer-size", &buffer_size,
	       NULL);

  is_live_instrument = ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  output_port_count = fx_vst3_audio->output_port_count;
  input_port_count = fx_vst3_audio->input_port_count;

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxVst3AudioScopeData *scope_data;

    scope_data = fx_vst3_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxVst3AudioChannelData *channel_data;

	channel_data = scope_data->channel_data[j];

	if(is_live_instrument){
//	  g_message("ags_fx_vst3_audio_notify_buffer_size_callback() - channel data buffer_size = %d", buffer_size);

	  ags_vst_process_data_set_num_samples(channel_data->process_data,
					       buffer_size);
	  
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
	    AgsFxVst3AudioInputData *input_data;

	    input_data = channel_data->input_data[k];
	    
//	    g_message("ags_fx_vst3_audio_notify_buffer_size_callback() - input data buffer_size = %d", buffer_size);
	    
	    ags_vst_process_data_set_num_samples(input_data->process_data,
						 buffer_size);
	    
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
ags_fx_vst3_audio_notify_samplerate_callback(GObject *gobject,
					     GParamSpec *pspec,
					     gpointer user_data)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsRecallContainer *recall_container;

  AgsFxVst3Audio *fx_vst3_audio;

  AgsVst3Plugin *vst3_plugin;

  GList *start_recall_channel, *recall_channel;

  gchar **parameter_name;
  
  guint n_params;
  guint input_pads;
  guint audio_channels;
  guint output_port_count, input_port_count;
  guint buffer_size;
  guint samplerate;
  guint i, j, k;
  gboolean is_live_instrument;

  GValue *value;
  
  GRecMutex *recall_mutex;
  
  fx_vst3_audio = AGS_FX_VST3_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  /* get vst3 plugin */
  g_rec_mutex_lock(recall_mutex);

  vst3_plugin = fx_vst3_audio->vst3_plugin;
  
  if(vst3_plugin == NULL){
    vst3_plugin = 
      AGS_FX_VST3_AUDIO(gobject)->vst3_plugin = ags_vst3_manager_find_vst3_plugin(ags_vst3_manager_get_instance(),
										  AGS_RECALL(gobject)->filename,
										  AGS_RECALL(gobject)->effect);
  }

  g_rec_mutex_unlock(recall_mutex);

  if(vst3_plugin == NULL ||
     !ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    return;
  }

#if HAVE_GLIB_2_68
  strv_builder = g_strv_builder_new();

  g_strv_builder_add(strv_builder,
		     "buffer-size");
  g_strv_builder_add(strv_builder,
		     "samplerate");
  g_strv_builder_add(strv_builder,
		     "iedit-controller");
  g_strv_builder_add(strv_builder,
		     "iaudio-processor");
  g_strv_builder_add(strv_builder,
		     "iedit-controller-host-editing");
  
  parameter_name = g_strv_builder_end(strv_builder);
#else
  parameter_name = (gchar **) g_malloc(6 * sizeof(gchar *));

  parameter_name[0] = g_strdup("buffer-size");
  parameter_name[1] = g_strdup("samplerate");
  parameter_name[2] = g_strdup("iedit-controller");
  parameter_name[3] = g_strdup("iaudio-processor");
  parameter_name[4] = g_strdup("iedit-controller-host-editing");
  parameter_name[5] = NULL;
#endif

  n_params = 5;

  value = g_new0(GValue,
		 5);

  g_value_init(value,
	       G_TYPE_UINT);
    
  g_value_init(value + 1,
	       G_TYPE_UINT);
    
  g_value_init(value + 2,
	       G_TYPE_POINTER);

  g_value_init(value + 3,
	       G_TYPE_POINTER);

  g_value_init(value + 4,
	       G_TYPE_POINTER);

  audio = NULL;

  recall_container = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  g_object_get(fx_vst3_audio,
	       "audio", &audio,
	       "recall-container", &recall_container,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  g_value_set_uint(value,
		   buffer_size);

  g_value_set_uint(value + 1,
		   samplerate);
  
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

  output_port_count = fx_vst3_audio->output_port_count;
  input_port_count = fx_vst3_audio->input_port_count;

  g_rec_mutex_unlock(recall_mutex);

  is_live_instrument = ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxVst3AudioScopeData *scope_data;

    scope_data = fx_vst3_audio->scope_data[i];
    
    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxVst3AudioChannelData *channel_data;

	channel_data = scope_data->channel_data[j];

	if(is_live_instrument){
	  AgsPort **iter;
	  
	  guint nth;

	  if(channel_data->icomponent != NULL){
	    ags_vst_icomponent_destroy(channel_data->icomponent);
	  }

//	  g_message("ags_fx_vst3_audio_notify_samplerate_callback() - instantiate channel data");
	  
	  ags_vst_process_context_set_samplerate(channel_data->process_context,
						 (gdouble) samplerate);	  
	  
	  channel_data->icomponent = ags_base_plugin_instantiate_with_params((AgsBasePlugin *) vst3_plugin,
									     &n_params,
									     &parameter_name, &value);
	      
	  channel_data->iedit_controller = g_value_get_pointer(value + 2);
	  channel_data->iaudio_processor = g_value_get_pointer(value + 3);

	  channel_data->iedit_controller_host_editing = g_value_get_pointer(value + 4);

	  channel_data->icomponent_handler = ags_vst_component_handler_new();
	    
	  ags_vst_iedit_controller_set_component_handler(channel_data->iedit_controller,
							 channel_data->icomponent_handler);
	}
	
	if(!is_live_instrument){
	  for(k = 0; k < AGS_SEQUENCER_MAX_MIDI_KEYS; k++){
	    AgsFxVst3AudioInputData *input_data;

	    input_data = channel_data->input_data[k];

	    if(input_data->icomponent != NULL){
	      ags_vst_icomponent_destroy(input_data->icomponent);
	    }

//	    g_message("ags_fx_vst3_audio_notify_samplerate_callback() - instantiate input data");
	  
	    ags_vst_process_context_set_samplerate(input_data->process_context,
						   (gdouble) samplerate);	  
	    
	    input_data->icomponent = ags_base_plugin_instantiate_with_params((AgsBasePlugin *) vst3_plugin,
									     &n_params,
									     &parameter_name, &value);

	    input_data->iedit_controller = g_value_get_pointer(value + 2);
	    input_data->iaudio_processor = g_value_get_pointer(value + 3);

	    input_data->iedit_controller_host_editing = g_value_get_pointer(value + 4);

	    input_data->icomponent_handler = ags_vst_component_handler_new();
	    
	    ags_vst_iedit_controller_set_component_handler(input_data->iedit_controller,
							   input_data->icomponent_handler);
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
	  ags_fx_vst3_channel_load_port(recall_channel->data);
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

  g_strfreev(parameter_name);
}

void
ags_fx_vst3_audio_set_audio_channels_callback(AgsAudio *audio,
					      guint audio_channels, guint audio_channels_old,
					      AgsFxVst3Audio *fx_vst3_audio)
{
  AgsChannel *start_input;
  AgsRecallContainer *recall_container;

  AgsVst3Plugin *vst3_plugin;

  GList *start_recall_channel, *recall_channel;

#if HAVE_GLIB_2_68
  GStrvBuilder *strv_builder;
#endif
  
  gchar **parameter_name;
    
  guint n_params;
  guint input_pads;
  guint output_port_count, input_port_count;
  guint buffer_size;
  guint samplerate;
  guint i, j, k;
  gboolean is_live_instrument;

  GValue *value;

  GRecMutex *recall_mutex;

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  /* get vst3 plugin */
  g_rec_mutex_lock(recall_mutex);

  vst3_plugin = fx_vst3_audio->vst3_plugin;
  
  if(vst3_plugin == NULL){
    vst3_plugin = 
      AGS_FX_VST3_AUDIO(fx_vst3_audio)->vst3_plugin = ags_vst3_manager_find_vst3_plugin(ags_vst3_manager_get_instance(),
											AGS_RECALL(fx_vst3_audio)->filename,
											AGS_RECALL(fx_vst3_audio)->effect);
  }

  g_rec_mutex_unlock(recall_mutex);

  if(vst3_plugin == NULL ||
     !ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    return;
  }

#if HAVE_GLIB_2_68
  strv_builder = g_strv_builder_new();

  g_strv_builder_add(strv_builder,
		     "buffer-size");
  g_strv_builder_add(strv_builder,
		     "samplerate");
  g_strv_builder_add(strv_builder,
		     "iedit-controller");
  g_strv_builder_add(strv_builder,
		     "iaudio-processor");
  g_strv_builder_add(strv_builder,
		     "iedit-controller-host-editing");
  
  parameter_name = g_strv_builder_end(strv_builder);
#else
  parameter_name = (gchar **) g_malloc(6 * sizeof(gchar *));

  parameter_name[0] = g_strdup("buffer-size");
  parameter_name[1] = g_strdup("samplerate");
  parameter_name[2] = g_strdup("iedit-controller");
  parameter_name[3] = g_strdup("iaudio-processor");
  parameter_name[4] = g_strdup("iedit-controller-host-editing");
  parameter_name[5] = NULL;
#endif

  n_params = 5;

  value = g_new0(GValue,
		 5);

  g_value_init(value,
	       G_TYPE_UINT);
  
  g_value_init(value + 1,
	       G_TYPE_UINT);
    
  g_value_init(value + 2,
	       G_TYPE_POINTER);

  g_value_init(value + 3,
	       G_TYPE_POINTER);

  g_value_init(value + 4,
	       G_TYPE_POINTER);

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
  
  g_object_get(fx_vst3_audio,
	       "recall-container", &recall_container,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);
  
  g_value_set_uint(value,
		   buffer_size);

  g_value_set_uint(value + 1,
		   samplerate);

  start_recall_channel = ags_recall_container_get_recall_channel(recall_container);

  /* get VST3 plugin */
  g_rec_mutex_lock(recall_mutex);

  output_port_count = fx_vst3_audio->output_port_count;
  input_port_count = fx_vst3_audio->input_port_count;

  g_rec_mutex_unlock(recall_mutex);

  /* allocate channel data */
  is_live_instrument = ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);
    
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxVst3AudioScopeData *scope_data;

    scope_data = fx_vst3_audio->scope_data[i];

    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      if(scope_data->audio_channels > audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxVst3AudioChannelData *channel_data;

	  channel_data = scope_data->channel_data[j];
	
	  ags_fx_vst3_audio_channel_data_free(channel_data);
	}
      }
      
      if(scope_data->channel_data == NULL){
	scope_data->channel_data = (AgsFxVst3AudioChannelData **) g_malloc(audio_channels * sizeof(AgsFxVst3AudioChannelData *)); 
      }else{
	scope_data->channel_data = (AgsFxVst3AudioChannelData **) g_realloc(scope_data->channel_data,
									    audio_channels * sizeof(AgsFxVst3AudioChannelData *)); 
      }

      if(scope_data->audio_channels < audio_channels){
	for(j = scope_data->audio_channels; j < audio_channels; j++){
	  AgsFxVst3AudioChannelData *channel_data;

	  channel_data =
	    scope_data->channel_data[j] = ags_fx_vst3_audio_channel_data_alloc();

	  if(is_live_instrument){
	    AgsPort **iter;
	  
	    guint nth;

//	    g_message("ags_fx_vst3_audio_set_audio_channels_callback() - instantiate channel data");
	    
	    channel_data->icomponent = ags_base_plugin_instantiate_with_params((AgsBasePlugin *) vst3_plugin,
									       &n_params,
									       &parameter_name, &value);
	      
	    channel_data->iedit_controller = g_value_get_pointer(value + 2);
	    channel_data->iaudio_processor = g_value_get_pointer(value + 3);

	    channel_data->iedit_controller_host_editing = g_value_get_pointer(value + 4);

	    channel_data->icomponent_handler = ags_vst_component_handler_new();
	    
	    ags_vst_iedit_controller_set_component_handler(channel_data->iedit_controller,
							   channel_data->icomponent_handler);
	    
	    ags_vst_process_data_set_num_samples(channel_data->process_data,
						 buffer_size);
	  
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
	      AgsFxVst3AudioInputData *input_data;

	      input_data = channel_data->input_data[k];
	    
//	      g_message("ags_fx_vst3_audio_set_audio_channels_callback() - instantiate input data");
	      
	      input_data->icomponent = ags_base_plugin_instantiate_with_params((AgsBasePlugin *) vst3_plugin,
									       &n_params,
									       &parameter_name, &value);

	      input_data->iedit_controller = g_value_get_pointer(value + 2);
	      input_data->iaudio_processor = g_value_get_pointer(value + 3);

	      input_data->iedit_controller_host_editing = g_value_get_pointer(value + 4);
	      
	      input_data->icomponent_handler = ags_vst_component_handler_new();
	    
	      ags_vst_iedit_controller_set_component_handler(input_data->iedit_controller,
							     input_data->icomponent_handler);
	      
	      ags_vst_process_data_set_num_samples(input_data->process_data,
						   buffer_size);
	      
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
	  ags_fx_vst3_channel_load_port(recall_channel->data);
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
ags_fx_vst3_audio_set_pads_callback(AgsAudio *audio,
				    GType channel_type,
				    guint pads, guint pads_old,
				    AgsFxVst3Audio *fx_vst3_audio)
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
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  /* get VST3 plugin and port */
  is_live_instrument = ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);

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
  
  g_object_get(fx_vst3_audio,
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
	  ags_fx_vst3_channel_load_port(recall_channel->data);
	}

	if(input != NULL){
	  g_object_unref(input);
	}
      }
    }
  }

  /* disconnect */
  if(pads < pads_old){
    //NOTE:JK: this won't work see AgsFxVst3Channel::finalize()
  }  

  if(start_input != NULL){
    g_object_unref(start_input);
  }

  if(recall_container != NULL){
    g_object_unref(recall_container);
  }
}

/**
 * ags_fx_vst3_audio_scope_data_alloc:
 * 
 * Allocate #AgsFxVst3AudioScopeData-struct
 * 
 * Returns: the new #AgsFxVst3AudioScopeData-struct
 * 
 * Since: 3.10.5
 */
AgsFxVst3AudioScopeData*
ags_fx_vst3_audio_scope_data_alloc()
{
  AgsFxVst3AudioScopeData *scope_data;

  scope_data = (AgsFxVst3AudioScopeData *) g_malloc(sizeof(AgsFxVst3AudioScopeData));

  g_rec_mutex_init(&(scope_data->strct_mutex));

  scope_data->parent = NULL;
  
  scope_data->audio_channels = 0;

  scope_data->channel_data = NULL;
  
  return(scope_data);
}

/**
 * ags_fx_vst3_audio_scope_data_free:
 * @scope_data: the #AgsFxVst3AudioScopeData-struct
 * 
 * Free @scope_data.
 * 
 * Since: 3.10.5
 */
void
ags_fx_vst3_audio_scope_data_free(AgsFxVst3AudioScopeData *scope_data)
{
  guint i;

  if(scope_data == NULL){
    return;
  }

  for(i = 0; i < scope_data->audio_channels; i++){
    ags_fx_vst3_audio_channel_data_free(scope_data->channel_data[i]);
  }

  g_free(scope_data);
}

/**
 * ags_fx_vst3_audio_channel_data_alloc:
 * 
 * Allocate #AgsFxVst3AudioChannelData-struct
 * 
 * Returns: the new #AgsFxVst3AudioChannelData-struct
 * 
 * Since: 3.10.5
 */
AgsFxVst3AudioChannelData*
ags_fx_vst3_audio_channel_data_alloc()
{
  AgsFxVst3AudioChannelData *channel_data;

  AgsVstIParameterChanges *iparameter_changes;
  AgsVstIEventList *ievent_list;
  AgsVstAudioBusBuffers *output, *input;
  
  guint i;
  
//  g_message("ags_fx_vst3_audio_channel_data_alloc()");
  
  channel_data = (AgsFxVst3AudioChannelData *) g_malloc(sizeof(AgsFxVst3AudioChannelData));

  g_rec_mutex_init(&(channel_data->strct_mutex));

  channel_data->parent = NULL;
  
  channel_data->event_count = 0;

  channel_data->output = NULL;
  channel_data->input = NULL;

  channel_data->icomponent = NULL;
  channel_data->iedit_controller = NULL;
  channel_data->iaudio_processor = NULL;

  channel_data->iedit_controller_host_editing = NULL;

  channel_data->icomponent_handler = NULL;

  channel_data->begin_edit_callback = NULL;
  channel_data->perform_edit_callback = NULL;
  channel_data->end_edit_callback = NULL;
  channel_data->restart_component_callback = NULL;

  /* process data */
  channel_data->process_data = ags_vst_process_data_alloc();

  ags_vst_process_data_set_process_mode(channel_data->process_data,
					AGS_VST_KREALTIME);
    
  ags_vst_process_data_set_symbolic_sample_size(channel_data->process_data,
						AGS_VST_KSAMPLE32);  

  ags_vst_process_data_set_num_samples(channel_data->process_data,
				       0);

  ags_vst_process_data_set_num_inputs(channel_data->process_data,
				      0);

  ags_vst_process_data_set_num_outputs(channel_data->process_data,
				       1);

  /* process context */
  channel_data->process_context = ags_vst_process_context_alloc();

  ags_vst_process_data_set_process_context(channel_data->process_data,
					   channel_data->process_context);
  
  /* parameter changes */
  channel_data->input_parameter_changes = ags_vst_parameter_changes_new();

  ags_vst_parameter_changes_set_max_parameters(channel_data->input_parameter_changes,
					       AGS_FX_VST3_AUDIO_MAX_PARAMETER_CHANGES);

  iparameter_changes = NULL;
  ags_vst_funknown_query_interface(channel_data->input_parameter_changes,
				   ags_vst_iparameter_changes_get_iid(), &iparameter_changes);
  
  ags_vst_process_data_set_input_iparameter_changes(channel_data->process_data,
						    iparameter_changes);
  
  /* event list */
  channel_data->input_event = ags_vst_event_list_new();

  ievent_list = NULL;
  ags_vst_funknown_query_interface(channel_data->input_event,
				   ags_vst_ievent_list_get_iid(), &ievent_list);

  ags_vst_process_data_set_input_events(channel_data->process_data,
					ievent_list);

  /* input */
  input = ags_vst_audio_bus_buffers_alloc();

  ags_vst_audio_bus_buffers_set_num_channels(input,
					     0);

  ags_vst_audio_bus_buffers_set_silence_flags(input,
					      1);

  ags_vst_audio_bus_buffers_set_samples32(input,
					  NULL);

  ags_vst_process_data_set_inputs(channel_data->process_data,
				  NULL);

  /* output */
  output = ags_vst_audio_bus_buffers_alloc();

  ags_vst_audio_bus_buffers_set_num_channels(output,
					     1);

  ags_vst_audio_bus_buffers_set_silence_flags(output,
					      0);

  ags_vst_audio_bus_buffers_set_samples32(output,
					  &(channel_data->output));

  ags_vst_process_data_set_outputs(channel_data->process_data,
				   output);    

  channel_data->parameter_changes[0].param_id = ~0;
  
  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    channel_data->input_data[i] = ags_fx_vst3_audio_input_data_alloc();

    channel_data->input_data[i]->parent = channel_data;
  }

  return(channel_data);
}

/**
 * ags_fx_vst3_audio_channel_data_free:
 * @channel_data: the #AgsFxVst3AudioChannelData-struct
 * 
 * Free @channel_data.
 * 
 * Since: 3.10.5
 */
void
ags_fx_vst3_audio_channel_data_free(AgsFxVst3AudioChannelData *channel_data)
{
  AgsVst3Plugin *vst3_plugin;

  guint i;

  if(channel_data == NULL){
    return;
  }

  g_free(channel_data->output);
  g_free(channel_data->input);
    
  for(i = 0; i < AGS_SEQUENCER_MAX_MIDI_KEYS; i++){
    ags_fx_vst3_audio_input_data_free(channel_data->input_data[i]);
  }

  g_free(channel_data);
}

/**
 * ags_fx_vst3_audio_input_data_alloc:
 * 
 * Allocate #AgsFxVst3AudioInputData-struct
 * 
 * Returns: the new #AgsFxVst3AudioInputData-struct
 * 
 * Since: 3.10.5
 */
AgsFxVst3AudioInputData*
ags_fx_vst3_audio_input_data_alloc()
{
  AgsFxVst3AudioInputData *input_data;

  AgsVstIParameterChanges *iparameter_changes;
  AgsVstIEventList *ievent_list;
  AgsVstAudioBusBuffers *output, *input;
  
//  g_message("ags_fx_vst3_audio_input_data_alloc()");

  input_data = (AgsFxVst3AudioInputData *) g_malloc(sizeof(AgsFxVst3AudioInputData));

  g_rec_mutex_init(&(input_data->strct_mutex));

  input_data->parent = NULL;

  input_data->output = NULL;
  input_data->input = NULL;

  input_data->icomponent = NULL;
  input_data->iedit_controller = NULL;
  input_data->iaudio_processor = NULL;

  input_data->iedit_controller_host_editing = NULL;

  input_data->icomponent_handler = NULL;

  input_data->begin_edit_callback = NULL;
  input_data->perform_edit_callback = NULL;
  input_data->end_edit_callback = NULL;
  input_data->restart_component_callback = NULL;

  /* process data */
  input_data->process_data = ags_vst_process_data_alloc();

  ags_vst_process_data_set_process_mode(input_data->process_data,
					AGS_VST_KREALTIME);
    
  ags_vst_process_data_set_symbolic_sample_size(input_data->process_data,
						AGS_VST_KSAMPLE32);  

  ags_vst_process_data_set_num_samples(input_data->process_data,
				       0);

  ags_vst_process_data_set_num_inputs(input_data->process_data,
				      0);

  ags_vst_process_data_set_num_outputs(input_data->process_data,
				       1);

  /* process context */
  input_data->process_context = ags_vst_process_context_alloc();

  ags_vst_process_data_set_process_context(input_data->process_data,
					   input_data->process_context);

  /* parameter changes */
  input_data->input_parameter_changes = ags_vst_parameter_changes_new();

  ags_vst_parameter_changes_set_max_parameters(input_data->input_parameter_changes,
					       AGS_FX_VST3_AUDIO_MAX_PARAMETER_CHANGES);

  iparameter_changes = NULL;
  ags_vst_funknown_query_interface(input_data->input_parameter_changes,
				   ags_vst_iparameter_changes_get_iid(), &iparameter_changes);

  ags_vst_process_data_set_input_iparameter_changes(input_data->process_data,
						    iparameter_changes);

  input_data->parameter_changes[0].param_id = ~0;

  /* event list */
  input_data->input_event = ags_vst_event_list_new();

  ievent_list = NULL;
  ags_vst_funknown_query_interface(input_data->input_event,
				   ags_vst_ievent_list_get_iid(), &ievent_list);

  ags_vst_process_data_set_input_events(input_data->process_data,
					ievent_list);

  /* input */
  input = ags_vst_audio_bus_buffers_alloc();

  ags_vst_audio_bus_buffers_set_num_channels(input,
					     0);

  ags_vst_audio_bus_buffers_set_silence_flags(input,
					      1);

  ags_vst_audio_bus_buffers_set_samples32(input,
					  NULL);

  ags_vst_process_data_set_inputs(input_data->process_data,
				  input);

  /* output */
  output = ags_vst_audio_bus_buffers_alloc();

  ags_vst_audio_bus_buffers_set_num_channels(output,
					     1);

  ags_vst_audio_bus_buffers_set_silence_flags(output,
					      0);

  ags_vst_audio_bus_buffers_set_samples32(output,
					  &(input_data->output));

  ags_vst_process_data_set_outputs(input_data->process_data,
				   output);    

  input_data->event_buffer = (snd_seq_event_t *) g_malloc(sizeof(snd_seq_event_t));

  input_data->event_buffer->type = SND_SEQ_EVENT_NOTEON;

  input_data->event_buffer->data.note.channel = 0;
  input_data->event_buffer->data.note.note = 0;
  input_data->event_buffer->data.note.velocity = 127;

  input_data->key_on = 0;
  
  return(input_data);
}

/**
 * ags_fx_vst3_audio_input_data_free:
 * @input_data: the #AgsFxVst3AudioInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 3.10.5
 */
void
ags_fx_vst3_audio_input_data_free(AgsFxVst3AudioInputData *input_data)
{
  AgsVst3Plugin *vst3_plugin;
  
  if(input_data == NULL){
    return;
  }

  g_free(input_data->output);
  g_free(input_data->input);
  
  g_free(input_data->event_buffer);

  g_free(input_data);
}

/**
 * ags_fx_vst3_audio_test_flags:
 * @fx_vst3_audio: the #AgsFxVst3Audio
 * @flags: the flags
 * 
 * Test @flags of @fx_vst3_audio.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.10.5
 */
gboolean
ags_fx_vst3_audio_test_flags(AgsFxVst3Audio *fx_vst3_audio, guint flags)
{
  gboolean success;
  
  GRecMutex *recall_mutex;
  
  if(!AGS_IS_FX_VST3_AUDIO(fx_vst3_audio)){
    return(FALSE);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  /* test flags */
  g_rec_mutex_lock(recall_mutex);

  success = ((flags & (fx_vst3_audio->flags)) != 0) ? TRUE: FALSE;

  g_rec_mutex_unlock(recall_mutex);

  return(success);
}

/**
 * ags_fx_vst3_audio_set_flags:
 * @fx_vst3_audio: the #AgsFxVst3Audio
 * @flags: the flags
 * 
 * Set @flags of @fx_vst3_audio.
 * 
 * Since: 3.10.5
 */
void
ags_fx_vst3_audio_set_flags(AgsFxVst3Audio *fx_vst3_audio, guint flags)
{  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_VST3_AUDIO(fx_vst3_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);
  
  /* set flags */
  g_rec_mutex_lock(recall_mutex);

  fx_vst3_audio->flags |= flags;

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_vst3_audio_unset_flags:
 * @fx_vst3_audio: the #AgsFxVst3Audio
 * @flags: the flags
 * 
 * Unset @flags of @fx_vst3_audio.
 * 
 * Since: 3.10.5
 */
void
ags_fx_vst3_audio_unset_flags(AgsFxVst3Audio *fx_vst3_audio, guint flags)
{  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_VST3_AUDIO(fx_vst3_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  /* set flags */
  g_rec_mutex_lock(recall_mutex);

  fx_vst3_audio->flags &= (~flags);

  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_vst3_audio_load_plugin:
 * @fx_vst3_audio: the #AgsFxVst3Audio
 * 
 * Load plugin of @fx_vst3_audio.
 * 
 * Since: 3.10.5
 */
void
ags_fx_vst3_audio_load_plugin(AgsFxVst3Audio *fx_vst3_audio)
{
  AgsVst3Manager *vst3_manager;
  AgsVst3Plugin *vst3_plugin;

#if HAVE_GLIB_2_68
  GStrvBuilder *strv_builder;
#endif
  
  gchar **parameter_name;
  
  gchar *filename, *effect;

  guint n_params;
  guint buffer_size;
  guint samplerate;
  
  GValue *value;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_VST3_AUDIO(fx_vst3_audio)){
    return;
  }

  vst3_manager = ags_vst3_manager_get_instance();

#if HAVE_GLIB_2_68
  strv_builder = g_strv_builder_new();

  g_strv_builder_add(strv_builder,
		     "buffer-size");
  g_strv_builder_add(strv_builder,
		     "samplerate");
  g_strv_builder_add(strv_builder,
		     "iedit-controller");
  g_strv_builder_add(strv_builder,
		     "iaudio-processor");
  g_strv_builder_add(strv_builder,
		     "iedit-controller-host-editing");
  
  parameter_name = g_strv_builder_end(strv_builder);
#else
  parameter_name = (gchar **) g_malloc(6 * sizeof(gchar *));

  parameter_name[0] = g_strdup("buffer-size");
  parameter_name[1] = g_strdup("samplerate");
  parameter_name[2] = g_strdup("iedit-controller");
  parameter_name[3] = g_strdup("iaudio-processor");
  parameter_name[4] = g_strdup("iedit-controller-host-editing");
  parameter_name[5] = NULL;
#endif
  
  n_params = 5;

  value = g_new0(GValue,
		 5);

  g_value_init(value,
	       G_TYPE_UINT);
    
  g_value_init(value + 1,
	       G_TYPE_UINT);
  
  g_value_init(value + 2,
	       G_TYPE_POINTER);

  g_value_init(value + 3,
	       G_TYPE_POINTER);

  g_value_init(value + 4,
	       G_TYPE_POINTER);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);
  
  /* get filename and effect */
  filename = NULL;
  effect = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  g_object_get(fx_vst3_audio,
	       "filename", &filename,
	       "effect", &effect,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  g_value_set_uint(value,
		   buffer_size);

  g_value_set_uint(value + 1,
		   samplerate);

  /* check if already loaded */
  g_rec_mutex_lock(recall_mutex);

  vst3_plugin = fx_vst3_audio->vst3_plugin;

  if(vst3_plugin == NULL){
    vst3_plugin =
      fx_vst3_audio->vst3_plugin = ags_vst3_manager_find_vst3_plugin(vst3_manager,
								     filename, effect);
    g_object_ref(vst3_plugin);
  }    
  
  g_rec_mutex_unlock(recall_mutex);

  if(vst3_plugin == NULL ||
     !ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    g_free(filename);
    g_free(effect);
    
    return;
  }
    
  if(vst3_plugin != NULL){    
    guint i, j, k;
    gboolean is_live_instrument;

    is_live_instrument = ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);
    
    /* set vst3 plugin */    
    g_rec_mutex_lock(recall_mutex);

    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsFxVst3AudioScopeData *scope_data;

      scope_data = fx_vst3_audio->scope_data[i];
    
      if(i == AGS_SOUND_SCOPE_PLAYBACK ||
	 i == AGS_SOUND_SCOPE_NOTATION ||
	 i == AGS_SOUND_SCOPE_MIDI){
	for(j = 0; j < scope_data->audio_channels; j++){
	  AgsFxVst3AudioChannelData *channel_data;;

	  channel_data = scope_data->channel_data[j];

	  if(is_live_instrument){
	    if(channel_data->icomponent == NULL){
//	      g_message("ags_fx_vst3_audio_load_plugin() - instantiate channel data");
	      
	      channel_data->icomponent = ags_base_plugin_instantiate_with_params((AgsBasePlugin *) vst3_plugin,
										 &n_params,
										 &parameter_name, &value);
	      
	      channel_data->iedit_controller = g_value_get_pointer(value + 2);
	      channel_data->iaudio_processor = g_value_get_pointer(value + 3);

	      channel_data->iedit_controller_host_editing = g_value_get_pointer(value + 4);

	      channel_data->icomponent_handler = ags_vst_component_handler_new();
	    
	      ags_vst_iedit_controller_set_component_handler(channel_data->iedit_controller,
							     channel_data->icomponent_handler);
	    }
	  }
	
	  if(!is_live_instrument){
	    for(k = 0; k < AGS_SEQUENCER_MAX_MIDI_KEYS; k++){
	      AgsFxVst3AudioInputData *input_data;

	      input_data = channel_data->input_data[k];

	      if(input_data->icomponent == NULL){
//		g_message("ags_fx_vst3_audio_load_plugin() - instantiate input data");
		
		input_data->icomponent = ags_base_plugin_instantiate_with_params((AgsBasePlugin *) vst3_plugin,
										 &n_params,
										 &parameter_name, &value);

		input_data->iedit_controller = g_value_get_pointer(value + 2);
		input_data->iaudio_processor = g_value_get_pointer(value + 3);

		input_data->iedit_controller_host_editing = g_value_get_pointer(value + 4);

		input_data->icomponent_handler = ags_vst_component_handler_new();
	    
		ags_vst_iedit_controller_set_component_handler(input_data->iedit_controller,
							       input_data->icomponent_handler);
	      }
	    }
	  }
	}
      }
    }
    
    g_rec_mutex_unlock(recall_mutex);

    g_free(value);
  }

  g_strfreev(parameter_name);
  
  g_free(filename);
  g_free(effect);
}

/**
 * ags_fx_vst3_audio_load_port:
 * @fx_vst3_audio: the #AgsFxVst3Audio
 * 
 * Load port of @fx_vst3_audio.
 * 
 * Since: 3.10.5
 */
void
ags_fx_vst3_audio_load_port(AgsFxVst3Audio *fx_vst3_audio)
{
  AgsAudio *audio;
  AgsChannel *start_input;
  AgsRecallContainer *recall_container;
  AgsPort **vst3_port;

  AgsVst3Plugin *vst3_plugin;

  GList *start_recall_channel, *recall_channel;
  GList *start_plugin_port, *plugin_port;

  guint *output_port;
  guint *input_port;
  
  guint input_pads;
  guint audio_channels;
  guint key;
  guint output_port_count, input_port_count;
  guint control_port_count;
  guint buffer_size;
  guint nth;
  guint i, j, k;
  gboolean is_live_instrument;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_VST3_AUDIO(fx_vst3_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  g_rec_mutex_lock(recall_mutex);

  if(fx_vst3_audio->vst3_port != NULL){
    g_rec_mutex_unlock(recall_mutex);
    
    return;
  }

  vst3_plugin = fx_vst3_audio->vst3_plugin;

  if(vst3_plugin == NULL){
    vst3_plugin = 
      AGS_FX_VST3_AUDIO(fx_vst3_audio)->vst3_plugin = ags_vst3_manager_find_vst3_plugin(ags_vst3_manager_get_instance(),
											AGS_RECALL(fx_vst3_audio)->filename,
											AGS_RECALL(fx_vst3_audio)->effect);
  }

  g_rec_mutex_unlock(recall_mutex);
  
  if(vst3_plugin == NULL ||
     !ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    return;
  }

  audio = NULL;

  recall_container = NULL;

  g_object_get(fx_vst3_audio,
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
  
  /* get VST3 plugin and port */
  g_rec_mutex_lock(recall_mutex);

  vst3_port = fx_vst3_audio->vst3_port;
  
  g_rec_mutex_unlock(recall_mutex);

  start_plugin_port = NULL;
  
  g_object_get(vst3_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  /* get control port count */
  plugin_port = start_plugin_port;

  output_port = NULL;
  input_port = NULL;
  
  output_port_count = ags_vst3_plugin_get_audio_output_port_count(vst3_plugin,
								  0);
  input_port_count = ags_vst3_plugin_get_audio_input_port_count(vst3_plugin,
								0);
  
  control_port_count = 0;

  is_live_instrument = ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);
  
  while(plugin_port != NULL){
    guint port_index;

    g_object_get(plugin_port->data,
		 "port-index", &port_index,
		 NULL);

    if(ags_plugin_port_test_flags(plugin_port->data,
				  AGS_PLUGIN_PORT_CONTROL)){
      control_port_count++;
    }

    plugin_port = plugin_port->next;
  }

  g_rec_mutex_lock(recall_mutex);

  fx_vst3_audio->output_port_count = output_port_count;
  fx_vst3_audio->output_port = output_port;

  fx_vst3_audio->input_port_count = input_port_count;
  fx_vst3_audio->input_port = input_port;
  
  g_rec_mutex_unlock(recall_mutex);

  /*  */
  if(is_live_instrument){
    if(control_port_count > 0){
      vst3_port = (AgsPort **) g_malloc((control_port_count + 1) * sizeof(AgsPort *));

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
	  plugin_name = g_strdup_printf("vst3-<%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x%.2x>",
					vst3_plugin->cid[0],
					vst3_plugin->cid[1],
					vst3_plugin->cid[2],
					vst3_plugin->cid[3],
					vst3_plugin->cid[4],
					vst3_plugin->cid[5],
					vst3_plugin->cid[6],
					vst3_plugin->cid[7],
					vst3_plugin->cid[8],
					vst3_plugin->cid[9],
					vst3_plugin->cid[10],
					vst3_plugin->cid[11],
					vst3_plugin->cid[12],
					vst3_plugin->cid[13],
					vst3_plugin->cid[14],
					vst3_plugin->cid[15]);
	  
	  specifier = NULL;
      
	  port_index = 0;

	  g_object_get(current_plugin_port,
		       "port-name", &specifier,
		       "port-index", &port_index,
		       NULL);

//	  g_message("ags_fx_vst3_audio_load_port() - live instrument %s", specifier);
	  
	  control_port = g_strdup_printf("%u/%u",
					 nth,
					 control_port_count);

	  /* default value */
	  g_value_init(&default_value,
		       G_TYPE_DOUBLE);
      
	  g_rec_mutex_lock(plugin_port_mutex);
      
	  g_value_copy(current_plugin_port->default_value,
		       &default_value);
      
	  g_rec_mutex_unlock(plugin_port_mutex);

	  /* vst3 port */
	  vst3_port[nth] = g_object_new(AGS_TYPE_PORT,
					"plugin-name", plugin_name,
					"specifier", specifier,
					"control-port", control_port,
					"port-value-is-pointer", FALSE,
					"port-value-type", G_TYPE_DOUBLE,
					NULL);
      
	  if(ags_plugin_port_test_flags(current_plugin_port,
					AGS_PLUGIN_PORT_OUTPUT)){
	    ags_port_set_flags(vst3_port[nth], AGS_PORT_IS_OUTPUT);
	  
	    ags_recall_set_flags((AgsRecall *) fx_vst3_audio,
				 AGS_RECALL_HAS_OUTPUT_PORT);
	
	  }else{
	    if(!ags_plugin_port_test_flags(current_plugin_port,
					   AGS_PLUGIN_PORT_INTEGER) &&
	       !ags_plugin_port_test_flags(current_plugin_port,
					   AGS_PLUGIN_PORT_TOGGLED)){
	      ags_port_set_flags(vst3_port[nth], AGS_PORT_INFINITE_RANGE);
	    }
	  }
	
	  g_object_set(vst3_port[nth],
		       "plugin-port", current_plugin_port,
		       NULL);

	  ags_port_util_load_vst3_conversion(vst3_port[nth],
					     current_plugin_port);
	
	  ags_port_safe_write_raw(vst3_port[nth],
				  &default_value);

	  ags_recall_add_port((AgsRecall *) fx_vst3_audio,
			      vst3_port[nth]);

	  g_signal_connect(vst3_port[nth], "safe-write",
			   ags_fx_vst3_audio_safe_write_callback, fx_vst3_audio);
	  
	  g_free(plugin_name);
	  g_free(specifier);
	  g_free(control_port);

	  g_value_unset(&default_value);

	  nth++;
	}
      
	plugin_port = plugin_port->next;      
      }

      vst3_port[nth] = NULL;
    }
  }else{
    for(j = 0; j < audio_channels; j++){
      for(k = 0; k < input_pads; k++){
	AgsChannel *input;

	input = ags_channel_nth(start_input,
				k * audio_channels + j);

	recall_channel = ags_recall_template_find_provider(start_recall_channel, (GObject *) input);

	if(recall_channel != NULL){
	  ags_fx_vst3_channel_load_port(recall_channel->data);
	}

	if(input != NULL){
	  g_object_unref(input);
	}
      }
    }
  }

  /* set VST3 output */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxVst3AudioScopeData *scope_data;

    scope_data = fx_vst3_audio->scope_data[i];

    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      for(j = 0; j < scope_data->audio_channels; j++){
	AgsFxVst3AudioChannelData *channel_data;

	channel_data = scope_data->channel_data[j];

	if(is_live_instrument){	  
	  guint nth;

//	  g_message("ags_fx_vst3_audio_load_port() - set channel data out[%d], in[%d]", output_port_count, input_port_count);

	  ags_vst_process_data_set_num_inputs(channel_data->process_data,
					      input_port_count);

	  ags_vst_process_data_set_num_outputs(channel_data->process_data,
					       output_port_count);
	  
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
	}else{
	  for(key = 0; key < AGS_SEQUENCER_MAX_MIDI_KEYS; key++){
	    AgsFxVst3AudioInputData *input_data;
	  
	    AgsVstParamID param_id;

	    input_data = channel_data->input_data[key];

//	    g_message("ags_fx_vst3_audio_load_port() - set input data out[%d], in[%d]", output_port_count, input_port_count);
	    
	    ags_vst_process_data_set_num_inputs(input_data->process_data,
						input_port_count);

	    ags_vst_process_data_set_num_outputs(input_data->process_data,
						 output_port_count);
	  
	    if(input_data->output == NULL &&
	       output_port_count > 0 &&
	       buffer_size > 0){
	      input_data->output = (float *) g_malloc(output_port_count * buffer_size * sizeof(float));
	    }
	  
	    if(input_data->input == NULL &&
	       input_port_count > 0 &&
	       buffer_size > 0){
	      input_data->input = (float *) g_malloc(input_port_count * buffer_size * sizeof(float));
	    }
	  }
	}
      }
    }
  }
  
  fx_vst3_audio->vst3_port = vst3_port;
  
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

void
ags_fx_vst3_audio_safe_write_callback(AgsPort *port, GValue *value,
				      AgsFxVst3Audio *fx_vst3_audio)
{
  AgsAudio *audio;
  AgsPluginPort *plugin_port;
  
  AgsVstParameterInfo *info;
  
  guint audio_channels;
  gint sound_scope;
  guint port_index;
  gfloat param_value;
  guint i;
  gboolean is_live_instrument;

  GRecMutex *fx_vst3_audio_mutex;
  
  fx_vst3_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  audio = NULL;
  plugin_port = NULL;

  audio_channels = 0;

  port_index = 0;
  
  g_object_get(fx_vst3_audio,
	       "audio", &audio,
	       NULL);

  if(audio != NULL){
    g_object_get(audio,
		 "audio-channels", &audio_channels,
		 NULL);
  }

  g_object_get(port,
	       "plugin-port", &plugin_port,
	       NULL);

  if(plugin_port != NULL){
    g_object_get(plugin_port,
		 "port-index", &port_index,
		 NULL);
  }
    
  is_live_instrument = ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);

  info = ags_vst_parameter_info_alloc();      
  
  g_rec_mutex_lock(fx_vst3_audio_mutex);
  
  for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){
    AgsFxVst3AudioScopeData *audio_scope_data;

    guint channel;
    
    audio_scope_data = fx_vst3_audio->scope_data[sound_scope];

    if(sound_scope == AGS_SOUND_SCOPE_PLAYBACK ||
       sound_scope == AGS_SOUND_SCOPE_NOTATION ||
       sound_scope == AGS_SOUND_SCOPE_MIDI){
      for(channel = 0; channel < audio_channels; channel++){
	AgsFxVst3AudioChannelData *channel_data;

	guint key;
	
	channel_data = audio_scope_data->channel_data[channel];

	if(is_live_instrument){
	  AgsVstParamID param_id;

	  if(channel_data->iedit_controller != NULL){
	    ags_vst_iedit_controller_get_parameter_info(channel_data->iedit_controller,
							port_index, info);

	    param_id = ags_vst_parameter_info_get_param_id(info);
	  	  
	    param_value = ags_vst_iedit_controller_plain_param_to_normalized(channel_data->iedit_controller,
									     param_id,
									     g_value_get_double(value));
	    
#if 1
	    if(channel_data->iedit_controller_host_editing != NULL){
	      ags_vst_iedit_controller_host_editing_begin_edit_from_host(channel_data->iedit_controller_host_editing,
									 param_id);
	    }
	    
	    ags_vst_iedit_controller_set_param_normalized(channel_data->iedit_controller,
							  param_id, param_value);

	    for(i = 0; i < AGS_FX_VST3_AUDIO_MAX_PARAMETER_CHANGES; i++){
	      if(fx_vst3_audio->parameter_changes[i].param_id == ~0){
		fx_vst3_audio->parameter_changes[i].param_id = param_id;
		fx_vst3_audio->parameter_changes[i].param_value = g_value_get_double(value);
		
		if(i +  1 < AGS_FX_VST3_AUDIO_MAX_PARAMETER_CHANGES){
		  fx_vst3_audio->parameter_changes[i + 1].param_id = ~0;
		}
		
		break;
	      }
	    }
	    
	    if(channel_data->iedit_controller_host_editing != NULL){
	      ags_vst_iedit_controller_host_editing_end_edit_from_host(channel_data->iedit_controller_host_editing,
								       param_id);
	    }
#else
	    ags_vst_icomponent_handler_begin_edit(channel_data->icomponent_handler,
						  param_id);

	    ags_vst_icomponent_handler_perform_edit(channel_data->icomponent_handler,
						    param_id, param_value);
      
	    ags_vst_icomponent_handler_end_edit(channel_data->icomponent_handler,
						param_id);
#endif
	  }
	}else{	
	  for(key = 0; key < AGS_SEQUENCER_MAX_MIDI_KEYS; key++){
	    AgsFxVst3AudioInputData *input_data;
	  
	    AgsVstParamID param_id;

	    input_data = channel_data->input_data[key];

	    if(input_data->iedit_controller != NULL){
	      ags_vst_iedit_controller_get_parameter_info(input_data->iedit_controller,
							  port_index, info);

	      param_id = ags_vst_parameter_info_get_param_id(info);

	      param_value = ags_vst_iedit_controller_plain_param_to_normalized(input_data->iedit_controller,
									       param_id,
									       g_value_get_double(value));
	    
#if 1
	      if(input_data->iedit_controller_host_editing != NULL){
		ags_vst_iedit_controller_host_editing_begin_edit_from_host(input_data->iedit_controller_host_editing,
									   param_id);
	      }
	    
	      ags_vst_iedit_controller_set_param_normalized(input_data->iedit_controller,
							    param_id, param_value);

	      
	      for(i = 0; i < AGS_FX_VST3_AUDIO_MAX_PARAMETER_CHANGES; i++){
		if(fx_vst3_audio->parameter_changes[i].param_id == ~0){
		  fx_vst3_audio->parameter_changes[i].param_id = param_id;
		  fx_vst3_audio->parameter_changes[i].param_value = g_value_get_double(value);
		
		  if(i +  1 < AGS_FX_VST3_AUDIO_MAX_PARAMETER_CHANGES){
		    fx_vst3_audio->parameter_changes[i + 1].param_id = ~0;
		  }
		
		  break;
		}
	      }
	      
	      if(input_data->iedit_controller_host_editing != NULL){
		ags_vst_iedit_controller_host_editing_end_edit_from_host(input_data->iedit_controller_host_editing,
									 param_id);
	      }
#else
	      ags_vst_icomponent_handler_begin_edit(input_data->icomponent_handler,
						    param_id);

	      ags_vst_icomponent_handler_perform_edit(input_data->icomponent_handler,
						      param_id, param_value);
      
	      ags_vst_icomponent_handler_end_edit(input_data->icomponent_handler,
						  param_id);
#endif	      
	    }
	  }
	}
      }
    }
  }

  g_rec_mutex_unlock(fx_vst3_audio_mutex);

  ags_vst_parameter_info_free(info);
  
  if(audio != NULL){
    g_object_unref(audio);
  }

  if(plugin_port != NULL){
    g_object_unref(plugin_port);
  }
}

/**
 * ags_fx_vst3_audio_change_program:
 * @fx_vst3_audio: the #AgsFxVst3Audio
 * @port_index: the port index
 * @program_list_id: the program list id
 * @program_index: the program index
 * 
 * Change program of @fx_vst3_audio.
 * 
 * Since: 3.10.10
 */
void
ags_fx_vst3_audio_change_program(AgsFxVst3Audio *fx_vst3_audio,
				 guint port_index,
				 guint program_list_id,
				 guint program_index)
{
  AgsAudio *audio;
  
  AgsVstIProgramListData *iprogram_list_data;
  AgsVstParameterInfo *info;
  
  guint audio_channels;
  gint sound_scope;
  gfloat param_value;
  gboolean is_live_instrument;
  AgsVstTResult val;

  GRecMutex *fx_vst3_audio_mutex;
  
  fx_vst3_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_vst3_audio);

  if(!AGS_IS_FX_VST3_AUDIO(fx_vst3_audio)){
    return;
  }

  if(fx_vst3_audio->vst3_plugin == NULL ||
     !ags_base_plugin_test_flags((AgsBasePlugin *) fx_vst3_audio->vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    return;
  }
  
  audio = NULL;

  audio_channels = 0;

  port_index = 0;
  
  g_object_get(fx_vst3_audio,
	       "audio", &audio,
	       NULL);

  if(audio != NULL){
    g_object_get(audio,
		 "audio-channels", &audio_channels,
		 NULL);
  }
  
  is_live_instrument = ags_fx_vst3_audio_test_flags(fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);

  param_value = 1.0;

  info = ags_vst_parameter_info_alloc();      
  
  g_rec_mutex_lock(fx_vst3_audio_mutex);
  
  for(sound_scope = 0; sound_scope < AGS_SOUND_SCOPE_LAST; sound_scope++){
    AgsFxVst3AudioScopeData *audio_scope_data;

    guint channel;
    
    audio_scope_data = fx_vst3_audio->scope_data[sound_scope];

    if(sound_scope == AGS_SOUND_SCOPE_PLAYBACK ||
       sound_scope == AGS_SOUND_SCOPE_NOTATION ||
       sound_scope == AGS_SOUND_SCOPE_MIDI){
      for(channel = 0; channel < audio_channels; channel++){
	AgsFxVst3AudioChannelData *channel_data;

	guint key;
	
	channel_data = audio_scope_data->channel_data[channel];

	if(is_live_instrument){
	  AgsVstParamID param_id;

	  if(channel_data->iedit_controller != NULL){
	    iprogram_list_data = NULL;
	    
	    val = ags_vst_funknown_query_interface(channel_data->iedit_controller,
						   ags_vst_iprogram_list_data_get_iid(), &iprogram_list_data);

	    if(iprogram_list_data != NULL){
	      AgsVstIBStream *data;

	      data = NULL;

	      ags_vst_iprogram_list_data_get_program_data(iprogram_list_data,
							  program_list_id, program_index,
							  data);
	    
	      ags_vst_iedit_controller_set_state(channel_data->iedit_controller,
						 data);
	    }
	  }
	}else{	
	  for(key = 0; key < AGS_SEQUENCER_MAX_MIDI_KEYS; key++){
	    AgsFxVst3AudioInputData *input_data;
	  
	    AgsVstParamID param_id;

	    input_data = channel_data->input_data[key];

	    if(input_data->iedit_controller != NULL){
	      iprogram_list_data = NULL;
	    
	      val = ags_vst_funknown_query_interface(input_data->iedit_controller,
						     ags_vst_iprogram_list_data_get_iid(), &iprogram_list_data);


	      if(iprogram_list_data != NULL){
		AgsVstIBStream *data;

		data = NULL;
	      
		ags_vst_iprogram_list_data_get_program_data(iprogram_list_data,
							    program_list_id, program_index,
							    data);
	    
		ags_vst_iedit_controller_set_state(input_data->iedit_controller,
						   data);
	      }
	    }
	  }
	}
      }
    }
  }
  
  g_rec_mutex_unlock(fx_vst3_audio_mutex);

  ags_vst_parameter_info_free(info);
  
  if(audio != NULL){
    g_object_unref(audio);
  }
}

/**
 * ags_fx_vst3_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsFxVst3Audio
 *
 * Returns: the new #AgsFxVst3Audio
 *
 * Since: 3.10.5
 */
AgsFxVst3Audio*
ags_fx_vst3_audio_new(AgsAudio *audio)
{
  AgsFxVst3Audio *fx_vst3_audio;

  fx_vst3_audio = (AgsFxVst3Audio *) g_object_new(AGS_TYPE_FX_VST3_AUDIO,
						  "audio", audio,
						  NULL);

  return(fx_vst3_audio);
}
