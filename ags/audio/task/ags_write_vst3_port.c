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

#include <ags/audio/task/ags_write_vst3_port.h>

#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_recall_container.h>

#include <ags/audio/fx/ags_fx_vst3_channel.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio.h>

#include <ags/i18n.h>

void ags_write_vst3_port_class_init(AgsWriteVst3PortClass *write_vst3_port);
void ags_write_vst3_port_init(AgsWriteVst3Port *write_vst3_port);
void ags_write_vst3_port_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_write_vst3_port_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_write_vst3_port_dispose(GObject *gobject);
void ags_write_vst3_port_finalize(GObject *gobject);

void ags_write_vst3_port_launch(AgsTask *task);

/**
 * SECTION:ags_write_vst3_port
 * @short_description: start audio object to audio loop
 * @title: AgsWriteVst3Port
 * @section_id:
 * @include: ags/audio/task/ags_write_vst3_port.h
 *
 * The #AgsWriteVst3Port task starts #AgsAudio to #AgsAudioLoop.
 */

static gpointer ags_write_vst3_port_parent_class = NULL;

enum{
  PROP_0,
  PROP_FX_VST3_AUDIO,
  PROP_PORT,
  PROP_VALUE,
  PROP_SOUND_SCOPE,
  PROP_AUDIO_CHANNEL,
};

GType
ags_write_vst3_port_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_write_vst3_port = 0;

    static const GTypeInfo ags_write_vst3_port_info = {
      sizeof(AgsWriteVst3PortClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_write_vst3_port_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsWriteVst3Port),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_write_vst3_port_init,
    };

    ags_type_write_vst3_port = g_type_register_static(AGS_TYPE_TASK,
						      "AgsWriteVst3Port",
						      &ags_write_vst3_port_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_write_vst3_port);
  }

  return g_define_type_id__volatile;
}

void
ags_write_vst3_port_class_init(AgsWriteVst3PortClass *write_vst3_port)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;
  
  ags_write_vst3_port_parent_class = g_type_class_peek_parent(write_vst3_port);

  /* gobject */
  gobject = (GObjectClass *) write_vst3_port;

  gobject->set_property = ags_write_vst3_port_set_property;
  gobject->get_property = ags_write_vst3_port_get_property;

  gobject->dispose = ags_write_vst3_port_dispose;
  gobject->finalize = ags_write_vst3_port_finalize;

  /* properties */
  /**
   * AgsWriteVst3Port:fx-vst3-audio:
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
   * AgsWriteVst3Port:port:
   *
   * The assigned #AgsPort
   * 
   * Since: 3.11.0
   */
  param_spec = g_param_spec_object("port",
				   i18n_pspec("port"),
				   i18n_pspec("The port to write"),
				   AGS_TYPE_PORT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PORT,
				  param_spec);

  /**
   * AgsWriteVst3Port:value:
   *
   * The effects value.
   * 
   * Since: 3.11.0
   */
  param_spec =  g_param_spec_double("value",
				    i18n_pspec("value"),
				    i18n_pspec("The value to write"),
				    -1.0 * G_MAXDOUBLE,
				    G_MAXDOUBLE,
				    0.0,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_VALUE,
				  param_spec);
  
  /**
   * AgsWriteVst3Port:sound-scope:
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
   * AgsWriteVst3Port:audio-channel:
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
  
  /* task */
  task = (AgsTaskClass *) write_vst3_port;

  task->launch = ags_write_vst3_port_launch;
}

void
ags_write_vst3_port_init(AgsWriteVst3Port *write_vst3_port)
{
  write_vst3_port->fx_vst3_audio = NULL;

  write_vst3_port->sound_scope = -1;
  write_vst3_port->audio_channel = -1;
}

void
ags_write_vst3_port_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsWriteVst3Port *write_vst3_port;

  write_vst3_port = AGS_WRITE_VST3_PORT(gobject);

  switch(prop_id){
  case PROP_FX_VST3_AUDIO:
  {
    AgsFxVst3Audio *fx_vst3_audio;

    fx_vst3_audio = (AgsFxVst3Audio *) g_value_get_object(value);

    if(write_vst3_port->fx_vst3_audio == fx_vst3_audio){
      return;
    }

    if(write_vst3_port->fx_vst3_audio != NULL){
      g_object_unref(write_vst3_port->fx_vst3_audio);
    }

    if(fx_vst3_audio != NULL){
      g_object_ref(fx_vst3_audio);
    }

    write_vst3_port->fx_vst3_audio = fx_vst3_audio;
  }
  break;
  case PROP_PORT:
  {
    AgsPort *port;

    port = (AgsPort *) g_value_get_object(value);

    if(write_vst3_port->port == port){
      return;
    }

    if(write_vst3_port->port != NULL){
      g_object_unref(write_vst3_port->port);
    }

    if(port != NULL){
      g_object_ref(port);
    }

    write_vst3_port->port = port;
  }
  break;
  case PROP_VALUE:
  {
    write_vst3_port->value = g_value_get_double(value);
  }
  break;
  case PROP_SOUND_SCOPE:
  {
    write_vst3_port->sound_scope = g_value_get_int(value);
  }
  break;
  case PROP_AUDIO_CHANNEL:
  {
    write_vst3_port->audio_channel = g_value_get_int(value);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_write_vst3_port_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsWriteVst3Port *write_vst3_port;

  write_vst3_port = AGS_WRITE_VST3_PORT(gobject);

  switch(prop_id){
  case PROP_FX_VST3_AUDIO:
  {
    g_value_set_object(value, write_vst3_port->fx_vst3_audio);
  }
  break;
  case PROP_PORT:
  {
    g_value_set_object(value, write_vst3_port->port);
  }
  break;
  case PROP_VALUE:
  {
    g_value_set_double(value, write_vst3_port->value);
  }
  break;
  case PROP_SOUND_SCOPE:
  {
    g_value_set_int(value, write_vst3_port->sound_scope);
  }
  break;
  case PROP_AUDIO_CHANNEL:
  {
    g_value_set_int(value, write_vst3_port->audio_channel);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_write_vst3_port_dispose(GObject *gobject)
{
  AgsWriteVst3Port *write_vst3_port;

  write_vst3_port = AGS_WRITE_VST3_PORT(gobject);

  if(write_vst3_port->fx_vst3_audio != NULL){
    g_object_unref(write_vst3_port->fx_vst3_audio);

    write_vst3_port->fx_vst3_audio = NULL;
  }

  if(write_vst3_port->port != NULL){
    g_object_unref(write_vst3_port->port);

    write_vst3_port->port = NULL;
  }
    
  /* call parent */
  G_OBJECT_CLASS(ags_write_vst3_port_parent_class)->dispose(gobject);
}

void
ags_write_vst3_port_finalize(GObject *gobject)
{
  AgsWriteVst3Port *write_vst3_port;

  write_vst3_port = AGS_WRITE_VST3_PORT(gobject);

  if(write_vst3_port->fx_vst3_audio != NULL){
    g_object_unref(write_vst3_port->fx_vst3_audio);
  }

  if(write_vst3_port->port != NULL){
    g_object_unref(write_vst3_port->port);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_write_vst3_port_parent_class)->finalize(gobject);
}

void
ags_write_vst3_port_launch(AgsTask *task)
{
  AgsWriteVst3Port *write_vst3_port;

  AgsVst3Plugin *vst3_plugin;
  AgsPluginPort *plugin_port;
  
  AgsFxVst3AudioScopeData *scope_data;
  
  AgsVstParameterInfo *info;
 
  guint port_index;
  gdouble param_value;
  gint sound_scope;
  guint i;
  guint j, j_start, j_stop;
  gboolean is_live_instrument;
  
  GRecMutex *fx_vst3_audio_mutex;
  
  write_vst3_port = AGS_WRITE_VST3_PORT(task);

  g_return_if_fail(AGS_IS_FX_VST3_AUDIO(write_vst3_port->fx_vst3_audio));
  g_return_if_fail(AGS_IS_PORT(write_vst3_port->port));

  vst3_plugin = write_vst3_port->fx_vst3_audio->vst3_plugin;

  /* get VST3 plugin */
  fx_vst3_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(write_vst3_port->fx_vst3_audio);

  g_rec_mutex_lock(fx_vst3_audio_mutex);

  vst3_plugin = write_vst3_port->fx_vst3_audio->vst3_plugin;

  g_rec_mutex_unlock(fx_vst3_audio_mutex);

  if(vst3_plugin == NULL){
    g_warning("can't get VST3 plugin");
    
    return;
  }

  sound_scope = write_vst3_port->sound_scope;

  if(sound_scope < 0 ||
     sound_scope >= AGS_SOUND_SCOPE_LAST){
    g_warning("sound scope invalid");
    
    return;
  }

  plugin_port = NULL;

  port_index = 0;

  g_object_get(write_vst3_port->port,
	       "plugin-port", &plugin_port,
	       NULL);

  if(plugin_port != NULL){
    g_object_get(plugin_port,
		 "port-index", &port_index,
		 NULL);
  }

  is_live_instrument = ags_fx_vst3_audio_test_flags(write_vst3_port->fx_vst3_audio, AGS_FX_VST3_AUDIO_LIVE_INSTRUMENT);

  info = ags_vst_parameter_info_alloc();      
  
  g_rec_mutex_lock(fx_vst3_audio_mutex);

  scope_data = write_vst3_port->fx_vst3_audio->scope_data[sound_scope];

  if(write_vst3_port->audio_channel < 0){
    j_start = 0;
    j_stop = scope_data->audio_channels;
  }else{
    j_start = write_vst3_port->audio_channel;
    j_stop = j_start + 1;
  }
  
  if(sound_scope == AGS_SOUND_SCOPE_PLAYBACK ||
     sound_scope == AGS_SOUND_SCOPE_NOTATION ||
     sound_scope == AGS_SOUND_SCOPE_MIDI){
    if(is_live_instrument){
      AgsFxVst3AudioScopeData *scope_data;
      
      scope_data = write_vst3_port->fx_vst3_audio->scope_data[sound_scope];

      if(write_vst3_port->audio_channel < 0){
	j_start = 0;
	j_stop = scope_data->audio_channels;
      }else{
	j_start = write_vst3_port->audio_channel;
	j_stop = j_start + 1;
      }
  
      for(j = j_start; j < j_stop && j < scope_data->audio_channels; j++){
	AgsFxVst3AudioChannelData *channel_data;;

	channel_data = scope_data->channel_data[j];

	if(is_live_instrument){
	  AgsVstParamID param_id;

	  if(channel_data->iedit_controller != NULL){
	    ags_vst_iedit_controller_get_parameter_info(channel_data->iedit_controller,
							port_index, info);

	    param_id = ags_vst_parameter_info_get_param_id(info);
	  	  
	    param_value = ags_vst_iedit_controller_plain_param_to_normalized(channel_data->iedit_controller,
									     param_id,
									     write_vst3_port->value);
	    
	    if(channel_data->iedit_controller_host_editing != NULL){
	      ags_vst_iedit_controller_host_editing_begin_edit_from_host(channel_data->iedit_controller_host_editing,
									 param_id);
	    }

	    g_message("set param normalized");
	    
	    ags_vst_iedit_controller_set_param_normalized(channel_data->iedit_controller,
							  param_id, param_value);

	    for(i = 0; i < AGS_FX_VST3_CHANNEL_MAX_PARAMETER_CHANGES; i++){
	      if(write_vst3_port->fx_vst3_audio->parameter_changes[i].param_id == param_id){
		write_vst3_port->fx_vst3_audio->parameter_changes[i].param_value = write_vst3_port->value;

		break;
	      }
	  
	      if(write_vst3_port->fx_vst3_audio->parameter_changes[i].param_id == ~0){
		write_vst3_port->fx_vst3_audio->parameter_changes[i].param_id = param_id;
		write_vst3_port->fx_vst3_audio->parameter_changes[i].param_value = write_vst3_port->value;
		
		if(i +  1 < AGS_FX_VST3_CHANNEL_MAX_PARAMETER_CHANGES){
		  write_vst3_port->fx_vst3_audio->parameter_changes[i + 1].param_id = ~0;
		}
		
		break;
	      }
	    }
	  }
	}
	
	if(!is_live_instrument){
	  guint key;
	  
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
									       write_vst3_port->value);
	    
	      if(input_data->iedit_controller_host_editing != NULL){
		ags_vst_iedit_controller_host_editing_begin_edit_from_host(input_data->iedit_controller_host_editing,
									   param_id);
	      }
	    
	      ags_vst_iedit_controller_set_param_normalized(input_data->iedit_controller,
							    param_id, param_value);

	      
	      for(i = 0; i < AGS_FX_VST3_AUDIO_MAX_PARAMETER_CHANGES; i++){
		if(write_vst3_port->fx_vst3_audio->parameter_changes[i].param_id == param_id){
		  write_vst3_port->fx_vst3_audio->parameter_changes[i].param_value = write_vst3_port->value;

		  break;
		}
		
		if(write_vst3_port->fx_vst3_audio->parameter_changes[i].param_id == ~0){
		  write_vst3_port->fx_vst3_audio->parameter_changes[i].param_id = param_id;
		  write_vst3_port->fx_vst3_audio->parameter_changes[i].param_value = write_vst3_port->value;
		
		  if(i +  1 < AGS_FX_VST3_AUDIO_MAX_PARAMETER_CHANGES){
		    write_vst3_port->fx_vst3_audio->parameter_changes[i + 1].param_id = ~0;
		  }
		
		  break;
		}
	      }
	      
	      if(input_data->iedit_controller_host_editing != NULL){
		ags_vst_iedit_controller_host_editing_end_edit_from_host(input_data->iedit_controller_host_editing,
									 param_id);
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

      g_object_get(write_vst3_port->fx_vst3_audio,
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
	   write_vst3_port->audio_channel == current_audio_channel){
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

      if(input_data != NULL &&
	 input_data->iedit_controller != NULL){
	ags_vst_iedit_controller_get_parameter_info(input_data->iedit_controller,
						    port_index, info);

	param_id = ags_vst_parameter_info_get_param_id(info);
	  
	param_value = ags_vst_iedit_controller_plain_param_to_normalized(input_data->iedit_controller,
									 param_id,
									 write_vst3_port->value);

	if(input_data->iedit_controller_host_editing != NULL){
	  ags_vst_iedit_controller_host_editing_begin_edit_from_host(input_data->iedit_controller_host_editing,
								     param_id);
	}	      

	g_message("set param normalized");	    
	      
	ags_vst_iedit_controller_set_param_normalized(input_data->iedit_controller,
						      param_id, param_value);

	for(i = 0; i < AGS_FX_VST3_CHANNEL_MAX_PARAMETER_CHANGES; i++){
	  if(AGS_FX_VST3_CHANNEL(recall->data)->parameter_changes[i].param_id == param_id){
	    AGS_FX_VST3_CHANNEL(recall->data)->parameter_changes[i].param_value = write_vst3_port->value;

	    break;
	  }
	  
	  if(AGS_FX_VST3_CHANNEL(recall->data)->parameter_changes[i].param_id == ~0){
	    AGS_FX_VST3_CHANNEL(recall->data)->parameter_changes[i].param_id = param_id;
	    AGS_FX_VST3_CHANNEL(recall->data)->parameter_changes[i].param_value = write_vst3_port->value;
		
	    if(i +  1 < AGS_FX_VST3_CHANNEL_MAX_PARAMETER_CHANGES){
	      AGS_FX_VST3_CHANNEL(recall->data)->parameter_changes[i + 1].param_id = ~0;
	    }
		
	    break;
	  }
	}

	if(input_data->iedit_controller_host_editing != NULL){
	  ags_vst_iedit_controller_host_editing_end_edit_from_host(input_data->iedit_controller_host_editing,
								   param_id);
	}
      }

      if(recall_container != NULL){
	g_object_unref(recall_container);
      }
      
      g_list_free_full(start_recall,
		       (GDestroyNotify) g_object_unref);
    }
  }

  g_rec_mutex_unlock(fx_vst3_audio_mutex);

  ags_vst_parameter_info_free(info);

  if(plugin_port != NULL){
    g_object_unref(plugin_port);
  }
}

/**
 * ags_write_vst3_port_new:
 * @fx_vst3_audio: the #AgsFxVst3Audio the port belongs to
 * @port: the #AgsPort to be written
 * @value: the value to write
 * @sound_scope: the #AgsSoundScope-enum
 * @audio_channel: the specific audio channel or -1 for all
 *
 * Create a new instance of #AgsWriteVst3Port.
 *
 * Returns: the new #AgsWriteVst3Port.
 *
 * Since: 3.11.0
 */
AgsWriteVst3Port*
ags_write_vst3_port_new(AgsFxVst3Audio *fx_vst3_audio,
			AgsPort *port,
			gdouble value,
			gint sound_scope,
			gint audio_channel)
{
  AgsWriteVst3Port *write_vst3_port;

  write_vst3_port = (AgsWriteVst3Port *) g_object_new(AGS_TYPE_WRITE_VST3_PORT,
						      "fx-vst3-audio", fx_vst3_audio,
						      "port", port,
						      "value", value,
						      "sound-scope", sound_scope,
						      "audio-channel", audio_channel,
						      NULL);
  
  return(write_vst3_port);
}
