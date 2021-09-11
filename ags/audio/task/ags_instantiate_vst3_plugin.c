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

#include <ags/audio/task/ags_instantiate_vst3_plugin.h>

#include <ags/audio/ags_recall_container.h>

#include <ags/audio/fx/ags_fx_vst3_channel.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

#include <ags/i18n.h>

void ags_instantiate_vst3_plugin_class_init(AgsInstantiateVst3PluginClass *instantiate_vst3_plugin);
void ags_instantiate_vst3_plugin_init(AgsInstantiateVst3Plugin *instantiate_vst3_plugin);
void ags_instantiate_vst3_plugin_set_property(GObject *gobject,
					      guint prop_id,
					      const GValue *value,
					      GParamSpec *param_spec);
void ags_instantiate_vst3_plugin_get_property(GObject *gobject,
					      guint prop_id,
					      GValue *value,
					      GParamSpec *param_spec);
void ags_instantiate_vst3_plugin_dispose(GObject *gobject);
void ags_instantiate_vst3_plugin_finalize(GObject *gobject);

void ags_instantiate_vst3_plugin_launch(AgsTask *task);

/**
 * SECTION:ags_instantiate_vst3_plugin
 * @short_description: start audio object to audio loop
 * @title: AgsInstantiateVst3Plugin
 * @section_id:
 * @include: ags/audio/task/ags_instantiate_vst3_plugin.h
 *
 * The #AgsInstantiateVst3Plugin task starts #AgsAudio to #AgsAudioLoop.
 */

static gpointer ags_instantiate_vst3_plugin_parent_class = NULL;

enum{
  PROP_0,
  PROP_FX_VST3_AUDIO,
  PROP_SOUND_SCOPE,
  PROP_AUDIO_CHANNEL,
  PROP_DO_REPLACE,
};

GType
ags_instantiate_vst3_plugin_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_instantiate_vst3_plugin = 0;

    static const GTypeInfo ags_instantiate_vst3_plugin_info = {
      sizeof(AgsInstantiateVst3PluginClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_instantiate_vst3_plugin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsInstantiateVst3Plugin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_instantiate_vst3_plugin_init,
    };

    ags_type_instantiate_vst3_plugin = g_type_register_static(AGS_TYPE_TASK,
							      "AgsInstantiateVst3Plugin",
							      &ags_instantiate_vst3_plugin_info,
							      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_instantiate_vst3_plugin);
  }

  return g_define_type_id__volatile;
}

void
ags_instantiate_vst3_plugin_class_init(AgsInstantiateVst3PluginClass *instantiate_vst3_plugin)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;
  
  ags_instantiate_vst3_plugin_parent_class = g_type_class_peek_parent(instantiate_vst3_plugin);

  /* gobject */
  gobject = (GObjectClass *) instantiate_vst3_plugin;

  gobject->set_property = ags_instantiate_vst3_plugin_set_property;
  gobject->get_property = ags_instantiate_vst3_plugin_get_property;

  gobject->dispose = ags_instantiate_vst3_plugin_dispose;
  gobject->finalize = ags_instantiate_vst3_plugin_finalize;

  /* properties */
  /**
   * AgsInstantiateVst3Plugin:fx-vst3-audio:
   *
   * The assigned #AgsFxVst3Audio
   * 
   * Since: 3.11.0
   */
  param_spec = g_param_spec_object("fx-vst3-audio",
				   i18n_pspec("ags-fx-vst3 audio object"),
				   i18n_pspec("The ags-vst-vst3 audio object"),
				   AGS_TYPE_FX_VST3_AUDIO,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FX_VST3_AUDIO,
				  param_spec);

  /**
   * AgsInstantiateVst3Plugin:sound-scope:
   *
   * The effects sound-scope.
   * 
   * Since: 3.11.0
   */
  param_spec =  g_param_spec_int("sound-scope",
				 i18n_pspec("sound scope"),
				 i18n_pspec("The sound scope"),
				 -1,
				 AGS_SOUND_SCOPE_LAST,
				 -1,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_SOUND_SCOPE,
				  param_spec);

  /**
   * AgsInstantiateVst3Plugin:audio-channel:
   *
   * The effects audio channel.
   * 
   * Since: 3.11.0
   */
  param_spec =  g_param_spec_int("audio-channel",
				 i18n_pspec("audio channel"),
				 i18n_pspec("The audio channel"),
				 -1,
				 G_MAXINT,
				 -1,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_AUDIO_CHANNEL,
				  param_spec);
  
  /**
   * AgsInstantiateVst3Plugin:do-replace:
   *
   * Do replace.
   * 
   * Since: 3.11.0
   */
  param_spec =  g_param_spec_boolean("do-replace",
				     i18n_pspec("do replace"),
				     i18n_pspec("Do replace"),
				     TRUE,
				     G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_DO_REPLACE,
				  param_spec);
  
  /* task */
  task = (AgsTaskClass *) instantiate_vst3_plugin;

  task->launch = ags_instantiate_vst3_plugin_launch;
}

void
ags_instantiate_vst3_plugin_init(AgsInstantiateVst3Plugin *instantiate_vst3_plugin)
{
  instantiate_vst3_plugin->fx_vst3_audio = NULL;

  instantiate_vst3_plugin->sound_scope = -1;
  instantiate_vst3_plugin->audio_channel = -1;

  instantiate_vst3_plugin->do_replace = TRUE;
}

void
ags_instantiate_vst3_plugin_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec)
{
  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  instantiate_vst3_plugin = AGS_INSTANTIATE_VST3_PLUGIN(gobject);

  switch(prop_id){
  case PROP_FX_VST3_AUDIO:
  {
    AgsFxVst3Audio *fx_vst3_audio;

    fx_vst3_audio = (AgsFxVst3Audio *) g_value_get_object(value);

    if(instantiate_vst3_plugin->fx_vst3_audio == fx_vst3_audio){
      return;
    }

    if(instantiate_vst3_plugin->fx_vst3_audio != NULL){
      g_object_unref(instantiate_vst3_plugin->fx_vst3_audio);
    }

    if(fx_vst3_audio != NULL){
      g_object_ref(fx_vst3_audio);
    }

    instantiate_vst3_plugin->fx_vst3_audio = fx_vst3_audio;
  }
  break;
  case PROP_SOUND_SCOPE:
  {
    instantiate_vst3_plugin->sound_scope = g_value_get_int(value);
  }
  break;
  case PROP_AUDIO_CHANNEL:
  {
    instantiate_vst3_plugin->audio_channel = g_value_get_int(value);
  }
  break;
  case PROP_DO_REPLACE:
  {
    instantiate_vst3_plugin->do_replace = g_value_get_boolean(value);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_instantiate_vst3_plugin_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec)
{
  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  instantiate_vst3_plugin = AGS_INSTANTIATE_VST3_PLUGIN(gobject);

  switch(prop_id){
  case PROP_FX_VST3_AUDIO:
  {
    g_value_set_object(value, instantiate_vst3_plugin->fx_vst3_audio);
  }
  break;
  case PROP_SOUND_SCOPE:
  {
    g_value_set_int(value, instantiate_vst3_plugin->sound_scope);
  }
  break;
  case PROP_AUDIO_CHANNEL:
  {
    g_value_set_int(value, instantiate_vst3_plugin->audio_channel);
  }
  break;
  case PROP_DO_REPLACE:
  {
    g_value_set_boolean(value, instantiate_vst3_plugin->do_replace);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_instantiate_vst3_plugin_dispose(GObject *gobject)
{
  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  instantiate_vst3_plugin = AGS_INSTANTIATE_VST3_PLUGIN(gobject);

  if(instantiate_vst3_plugin->fx_vst3_audio != NULL){
    g_object_unref(instantiate_vst3_plugin->fx_vst3_audio);

    instantiate_vst3_plugin->fx_vst3_audio = NULL;
  }
    
  /* call parent */
  G_OBJECT_CLASS(ags_instantiate_vst3_plugin_parent_class)->dispose(gobject);
}

void
ags_instantiate_vst3_plugin_finalize(GObject *gobject)
{
  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  instantiate_vst3_plugin = AGS_INSTANTIATE_VST3_PLUGIN(gobject);

  if(instantiate_vst3_plugin->fx_vst3_audio != NULL){
    g_object_unref(instantiate_vst3_plugin->fx_vst3_audio);
  }
    
  /* call parent */
  G_OBJECT_CLASS(ags_instantiate_vst3_plugin_parent_class)->finalize(gobject);
}

void
ags_instantiate_vst3_plugin_launch(AgsTask *task)
{
  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  AgsVst3Plugin *vst3_plugin;
  
  gchar **parameter_name;

  gint sound_scope;
  guint buffer_size;
  guint samplerate;
  guint n_params;
  guint parameter_count;
  guint i;
  guint j, j_start, j_stop;
  guint k;
  gboolean is_live_instrument;
  
  GValue *value;

  GRecMutex *fx_vst3_audio_mutex;

  instantiate_vst3_plugin = AGS_INSTANTIATE_VST3_PLUGIN(task);

  g_return_if_fail(AGS_IS_FX_VST3_AUDIO(instantiate_vst3_plugin->fx_vst3_audio));

  /* get VST3 plugin */
  fx_vst3_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(instantiate_vst3_plugin->fx_vst3_audio);

  g_rec_mutex_lock(fx_vst3_audio_mutex);

  vst3_plugin = instantiate_vst3_plugin->fx_vst3_audio->vst3_plugin;

  g_rec_mutex_unlock(fx_vst3_audio_mutex);

  if(vst3_plugin == NULL){
    g_warning("can't get VST3 plugin");
    
    return;
  }

  sound_scope = instantiate_vst3_plugin->sound_scope;

  if(sound_scope < 0 ||
     sound_scope >= AGS_SOUND_SCOPE_LAST){
    g_warning("sound scope invalid");
    
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

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  g_object_get(instantiate_vst3_plugin->fx_vst3_audio,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  g_value_set_uint(value,
		   buffer_size);

  g_value_set_uint(value + 1,
		   samplerate);

  is_live_instrument = ags_fx_vst3_audio_test_flags(instantiate_vst3_plugin->fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);
  
  g_rec_mutex_lock(fx_vst3_audio_mutex);

  if(sound_scope == AGS_SOUND_SCOPE_PLAYBACK ||
     sound_scope == AGS_SOUND_SCOPE_NOTATION ||
     sound_scope == AGS_SOUND_SCOPE_MIDI){
    if(ags_base_plugin_test_flags((AgsBasePlugin *) vst3_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
      AgsFxVst3AudioScopeData *scope_data;
      
      scope_data = instantiate_vst3_plugin->fx_vst3_audio->scope_data[sound_scope];

      if(instantiate_vst3_plugin->audio_channel < 0){
	j_start = 0;
	j_stop = scope_data->audio_channels;
      }else{
	j_start = instantiate_vst3_plugin->audio_channel;
	j_stop = j_start + 1;
      }
  
      for(j = j_start; j < j_stop && j < scope_data->audio_channels; j++){
	AgsFxVst3AudioChannelData *channel_data;;

	channel_data = scope_data->channel_data[j];

	if(is_live_instrument){
	  if(channel_data->icomponent == NULL ||
	     instantiate_vst3_plugin->do_replace){
	    if(channel_data->icomponent != NULL){
	      ags_vst_funknown_release(channel_data->icomponent);
	    }
	    
	    channel_data->icomponent = ags_base_plugin_instantiate_with_params((AgsBasePlugin *) vst3_plugin,
									       &n_params,
									       &parameter_name, &value);
	      
	    channel_data->iedit_controller = g_value_get_pointer(value + 2);
	    channel_data->iaudio_processor = g_value_get_pointer(value + 3);

	    channel_data->iedit_controller_host_editing = g_value_get_pointer(value + 4);

	    channel_data->icomponent_handler = ags_vst_component_handler_new();
	    
	    ags_vst_iedit_controller_set_component_handler(channel_data->iedit_controller,
							   channel_data->icomponent_handler);

	    parameter_count = ags_vst_iedit_controller_get_parameter_count(channel_data->iedit_controller);
  
	    for(i = 0; i < parameter_count; i++){
	      AgsVstParameterInfo *info;
	      AgsVstParamID param_id;
    
	      guint flags;
	      gdouble default_normalized_value;
    
	      info = ags_vst_parameter_info_alloc();
    
	      ags_vst_iedit_controller_get_parameter_info(channel_data->iedit_controller,
							  i, info);

	      flags = ags_vst_parameter_info_get_flags(info);

	      if((AGS_VST_KIS_PROGRAM_CHANGE & (flags)) != 0){
		ags_vst_parameter_info_free(info);
      
		continue;
	      }

	      param_id = ags_vst_parameter_info_get_param_id(info);
    
	      default_normalized_value = ags_vst_parameter_info_get_default_normalized_value(info);

	      if(channel_data->iedit_controller_host_editing != NULL){
		ags_vst_iedit_controller_host_editing_begin_edit_from_host(channel_data->iedit_controller_host_editing,
									   param_id);
	      }
    
	      ags_vst_iedit_controller_set_param_normalized(channel_data->iedit_controller,
							    param_id,
							    default_normalized_value);

	      if(channel_data->iedit_controller_host_editing != NULL){
		ags_vst_iedit_controller_host_editing_end_edit_from_host(channel_data->iedit_controller_host_editing,
									 param_id);
	      }
    
	      ags_vst_parameter_info_free(info);
	    }	      
	  }
	}

	if(!is_live_instrument){
	  for(k = 0; k < AGS_SEQUENCER_MAX_MIDI_KEYS; k++){
	    AgsFxVst3AudioInputData *input_data;

	    input_data = channel_data->input_data[k];

	    if(input_data->icomponent == NULL ||
	       instantiate_vst3_plugin->do_replace){
	      if(input_data->icomponent != NULL){
		ags_vst_funknown_release(input_data->icomponent);
	      }
	      
	      input_data->icomponent = ags_base_plugin_instantiate_with_params((AgsBasePlugin *) vst3_plugin,
									       &n_params,
									       &parameter_name, &value);

	      input_data->iedit_controller = g_value_get_pointer(value + 2);
	      input_data->iaudio_processor = g_value_get_pointer(value + 3);

	      input_data->iedit_controller_host_editing = g_value_get_pointer(value + 4);

	      input_data->icomponent_handler = ags_vst_component_handler_new();
	    
	      ags_vst_iedit_controller_set_component_handler(input_data->iedit_controller,
							     input_data->icomponent_handler);

	      parameter_count = ags_vst_iedit_controller_get_parameter_count(input_data->iedit_controller);
  
	      for(i = 0; i < parameter_count; i++){
		AgsVstParameterInfo *info;
		AgsVstParamID param_id;
    
		guint flags;
		gdouble default_normalized_value;
    
		info = ags_vst_parameter_info_alloc();
    
		ags_vst_iedit_controller_get_parameter_info(input_data->iedit_controller,
							    i, info);

		flags = ags_vst_parameter_info_get_flags(info);

		if((AGS_VST_KIS_PROGRAM_CHANGE & (flags)) != 0){
		  ags_vst_parameter_info_free(info);
      
		  continue;
		}

		param_id = ags_vst_parameter_info_get_param_id(info);
    
		default_normalized_value = ags_vst_parameter_info_get_default_normalized_value(info);

		if(input_data->iedit_controller_host_editing != NULL){
		  ags_vst_iedit_controller_host_editing_begin_edit_from_host(input_data->iedit_controller_host_editing,
									     param_id);
		}
    
		ags_vst_iedit_controller_set_param_normalized(input_data->iedit_controller,
							      param_id,
							      default_normalized_value);

		if(input_data->iedit_controller_host_editing != NULL){
		  ags_vst_iedit_controller_host_editing_end_edit_from_host(input_data->iedit_controller_host_editing,
									   param_id);
		}
    
		ags_vst_parameter_info_free(info);
	      }	      
	    }
	  }
	}
      }
    }else{
      AgsRecallContainer *recall_container;
      
      AgsFxVst3ChannelInputData *input_data;

      GList *start_recall, *recall;
      
      AgsVstParamID param_id;

      recall_container = NULL;

      g_object_get(instantiate_vst3_plugin->fx_vst3_audio,
		   "recall-container", &recall_container,
		   NULL);

      recall = 
	start_recall = ags_recall_container_get_recall_channel(recall_container);

      while(recall != NULL){
	AgsChannel *source;

	guint current_pad;
	guint current_audio_channel;
	gboolean success;
	
	source = NULL;

	current_pad = ~0;
	current_audio_channel = ~0;

	success = FALSE;
	
	g_object_get(recall->data,
		     "source", &source,
		     NULL);

	if(source != NULL){
	  g_object_get(source,
		       "pad", &current_pad,
		       "audio-channel", &current_audio_channel,
		       NULL);
	}
	
	if(current_pad == 0 &&
	   instantiate_vst3_plugin->audio_channel == current_audio_channel){
	  success = TRUE;
	}

	if(source != NULL){
	  g_object_unref(source);
	}

	if(success){
	  break;
	}
	
	recall = recall->next;
      }
      
      input_data = AGS_FX_VST3_CHANNEL(recall->data)->input_data[sound_scope];

      if(input_data->icomponent != NULL){
	ags_vst_icomponent_destroy(input_data->icomponent);
      }

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

      parameter_count = ags_vst_iedit_controller_get_parameter_count(input_data->iedit_controller);
  
      for(i = 0; i < parameter_count; i++){
	AgsVstParameterInfo *info;
	AgsVstParamID param_id;
    
	guint flags;
	gdouble default_normalized_value;
    
	info = ags_vst_parameter_info_alloc();
    
	ags_vst_iedit_controller_get_parameter_info(input_data->iedit_controller,
						    i, info);

	flags = ags_vst_parameter_info_get_flags(info);

	if((AGS_VST_KIS_PROGRAM_CHANGE & (flags)) != 0){
	  ags_vst_parameter_info_free(info);
      
	  continue;
	}

	param_id = ags_vst_parameter_info_get_param_id(info);
    
	default_normalized_value = ags_vst_parameter_info_get_default_normalized_value(info);

	if(input_data->iedit_controller_host_editing != NULL){
	  ags_vst_iedit_controller_host_editing_begin_edit_from_host(input_data->iedit_controller_host_editing,
								     param_id);
	}
    
	ags_vst_iedit_controller_set_param_normalized(input_data->iedit_controller,
						      param_id,
						      default_normalized_value);

	if(input_data->iedit_controller_host_editing != NULL){
	  ags_vst_iedit_controller_host_editing_end_edit_from_host(input_data->iedit_controller_host_editing,
								   param_id);
	}
    
	ags_vst_parameter_info_free(info);
      }
      
      if(recall_container != NULL){
	g_object_unref(recall_container);
      }
      
      g_list_free_full(start_recall,
		       (GDestroyNotify) g_object_unref);
    }
  }

  g_rec_mutex_unlock(fx_vst3_audio_mutex);

  g_strfreev(parameter_name);
  g_free(value);
}

/**
 * ags_instantiate_vst3_plugin_new:
 * @fx_vst3_audio: the #AgsFxVst3Audio the port belongs to
 * @sound_scope: the #AgsSoundScope-enum
 * @audio_channel: the specific audio channel or -1 for all
 * @do_replace: do replace any prior added plugin instances
 *
 * Create a new instance of #AgsInstantiateVst3Plugin.
 *
 * Returns: the new #AgsInstantiateVst3Plugin.
 *
 * Since: 3.11.0
 */
AgsInstantiateVst3Plugin*
ags_instantiate_vst3_plugin_new(AgsFxVst3Audio *fx_vst3_audio,
				gint sound_scope,
				gint audio_channel,
				gboolean do_replace)
{
  AgsInstantiateVst3Plugin *instantiate_vst3_plugin;

  instantiate_vst3_plugin = (AgsInstantiateVst3Plugin *) g_object_new(AGS_TYPE_INSTANTIATE_VST3_PLUGIN,
								      "fx-vst3-audio", fx_vst3_audio,
								      "sound-scope", sound_scope,
								      "audio-channel", audio_channel,
								      "do-replace", do_replace,
								      NULL);
  
  return(instantiate_vst3_plugin);
}
