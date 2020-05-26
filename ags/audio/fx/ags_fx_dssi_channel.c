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

#include <ags/audio/fx/ags_fx_dssi_channel.h>

#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_dssi_plugin.h>
#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_port_util.h>

#include <ags/audio/fx/ags_fx_dssi_audio.h>

void ags_fx_dssi_channel_class_init(AgsFxDssiChannelClass *fx_dssi_channel);
void ags_fx_dssi_channel_init(AgsFxDssiChannel *fx_dssi_channel);
void ags_fx_dssi_channel_dispose(GObject *gobject);
void ags_fx_dssi_channel_finalize(GObject *gobject);

/**
 * SECTION:ags_fx_dssi_channel
 * @short_description: fx dssi channel
 * @title: AgsFxDssiChannel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_dssi_channel.h
 *
 * The #AgsFxDssiChannel class provides ports to the effect processor.
 */

static gpointer ags_fx_dssi_channel_parent_class = NULL;

const gchar *ags_fx_dssi_channel_plugin_name = "ags-fx-dssi";

GType
ags_fx_dssi_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_dssi_channel = 0;

    static const GTypeInfo ags_fx_dssi_channel_info = {
      sizeof (AgsFxDssiChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_dssi_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxDssiChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_dssi_channel_init,
    };

    ags_type_fx_dssi_channel = g_type_register_static(AGS_TYPE_FX_NOTATION_CHANNEL,
						      "AgsFxDssiChannel",
						      &ags_fx_dssi_channel_info,
						      0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_dssi_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_dssi_channel_class_init(AgsFxDssiChannelClass *fx_dssi_channel)
{
  GObjectClass *gobject;

  ags_fx_dssi_channel_parent_class = g_type_class_peek_parent(fx_dssi_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_dssi_channel;

  gobject->dispose = ags_fx_dssi_channel_dispose;
  gobject->finalize = ags_fx_dssi_channel_finalize;
}

void
ags_fx_dssi_channel_init(AgsFxDssiChannel *fx_dssi_channel)
{
  AGS_RECALL(fx_dssi_channel)->name = "ags-fx-dssi";
  AGS_RECALL(fx_dssi_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_dssi_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_dssi_channel)->xml_type = "ags-fx-dssi-channel";

  fx_dssi_channel->dssi_port = NULL;
}

void
ags_fx_dssi_channel_dispose(GObject *gobject)
{
  AgsFxDssiChannel *fx_dssi_channel;
  
  fx_dssi_channel = AGS_FX_DSSI_CHANNEL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_dssi_channel_parent_class)->dispose(gobject);
}

void
ags_fx_dssi_channel_finalize(GObject *gobject)
{
  AgsPort **iter;
  AgsFxDssiChannel *fx_dssi_channel;
  
  fx_dssi_channel = AGS_FX_DSSI_CHANNEL(gobject);
  
  if(fx_dssi_channel->dssi_port == NULL){
    for(iter = fx_dssi_channel->dssi_port; iter[0] != NULL; iter++){
      g_object_unref(iter[0]);
    }

    g_free(fx_dssi_channel->dssi_port);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_dssi_channel_parent_class)->finalize(gobject);
}

/**
 * ags_fx_dssi_channel_load_port:
 * @fx_dssi_channel: the #AgsFxDssiChannel
 * 
 * Load port of @fx_dssi_channel.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_channel_load_port(AgsFxDssiChannel *fx_dssi_channel)
{
  AgsChannel *input;
  AgsFxDssiAudio *fx_dssi_audio;
  AgsPort **dssi_port;
  
  AgsDssiPlugin *dssi_plugin;

  GList *start_plugin_port, *plugin_port;

  guint *output_port;
  guint *input_port;

  guint audio_channel;
  guint pad;
  guint output_port_count, input_port_count;
  guint control_port_count;
  guint buffer_size;
  guint nth;
  guint i, j;
  gboolean is_live_instrument;
  
  GRecMutex *fx_dssi_audio_mutex;
  GRecMutex *fx_dssi_channel_mutex;

  if(!AGS_IS_FX_DSSI_CHANNEL(fx_dssi_channel)){
    return;
  }

  fx_dssi_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_channel);

  g_rec_mutex_lock(fx_dssi_channel_mutex);

  if(fx_dssi_channel->dssi_port != NULL){
    g_rec_mutex_unlock(fx_dssi_channel_mutex);
    
    return;
  }

  g_rec_mutex_unlock(fx_dssi_channel_mutex);
  
  fx_dssi_audio = NULL;

  g_object_get(fx_dssi_channel,
	       "recall-audio", &fx_dssi_audio,
	       NULL);

  if(fx_dssi_audio == NULL){
    return;
  }

  is_live_instrument = ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT);

  if(is_live_instrument){
    if(fx_dssi_audio != NULL){
      g_object_unref(fx_dssi_audio);
    }
    
    return;
  }
  
  /* get recall mutex */
  fx_dssi_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  input = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  
  g_object_get(fx_dssi_channel,
	       "source", &input,
	       "buffer-size", &buffer_size,
	       NULL);

  audio_channel = 0;
  pad = 0;

  g_object_get(input,
	       "audio-channel", &audio_channel,
	       "pad", &pad,
	       NULL);
  
  /* get DSSI plugin */
  g_rec_mutex_lock(fx_dssi_audio_mutex);

  dssi_plugin = fx_dssi_audio->dssi_plugin;
  
  output_port_count = fx_dssi_audio->output_port_count;
  output_port = fx_dssi_audio->output_port;

  input_port_count = fx_dssi_audio->input_port_count;
  input_port = fx_dssi_audio->input_port;

  g_rec_mutex_unlock(fx_dssi_audio_mutex);

  /* get DSSI port */
  g_rec_mutex_lock(fx_dssi_channel_mutex);

  dssi_port = fx_dssi_channel->dssi_port;

  g_rec_mutex_unlock(fx_dssi_channel_mutex);

  g_object_get(dssi_plugin,
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

  /*  */
  if(control_port_count > 0){
    dssi_port = (AgsPort **) g_malloc((control_port_count + 1) * sizeof(AgsPort *));

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
	plugin_name = g_strdup_printf("dssi-%u", dssi_plugin->unique_id);

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

	/* dssi port */
	dssi_port[nth] = g_object_new(AGS_TYPE_PORT,
				      "plugin-name", plugin_name,
				      "specifier", specifier,
				      "control-port", control_port,
				      "port-value-is-pointer", FALSE,
				      "port-value-type", G_TYPE_FLOAT,
				      NULL);
	ags_port_set_flags(dssi_port[nth], AGS_PORT_USE_LADSPA_FLOAT);
      
	if(ags_plugin_port_test_flags(current_plugin_port,
				      AGS_PLUGIN_PORT_OUTPUT)){
	  ags_port_set_flags(dssi_port[nth], AGS_PORT_IS_OUTPUT);
	  
	  ags_recall_set_flags((AgsRecall *) fx_dssi_channel,
			       AGS_RECALL_HAS_OUTPUT_PORT);
	
	}else{
	  if(!ags_plugin_port_test_flags(current_plugin_port,
					 AGS_PLUGIN_PORT_INTEGER) &&
	     !ags_plugin_port_test_flags(current_plugin_port,
					 AGS_PLUGIN_PORT_TOGGLED)){
	    ags_port_set_flags(dssi_port[nth], AGS_PORT_INFINITE_RANGE);
	  }
	}
	
	g_object_set(dssi_port[nth],
		     "plugin-port", current_plugin_port,
		     NULL);

	ags_port_util_load_ladspa_conversion(dssi_port[nth],
					     current_plugin_port);
	
	ags_port_safe_write_raw(dssi_port[nth],
				&default_value);

	ags_recall_add_port((AgsRecall *) fx_dssi_channel,
			    dssi_port[nth]);

	/* connect port */
	for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	  AgsFxDssiAudioScopeData *scope_data;

	  scope_data = fx_dssi_audio->scope_data[i];

	  if(i == AGS_SOUND_SCOPE_PLAYBACK ||
	     i == AGS_SOUND_SCOPE_NOTATION ||
	     i == AGS_SOUND_SCOPE_MIDI){
	    for(j = 0; j < scope_data->audio_channels; j++){
	      AgsFxDssiAudioChannelData *channel_data;

	      channel_data = scope_data->channel_data[j];

	      if(pad < 128){
		AgsFxDssiAudioInputData *input_data;
	  
		input_data = channel_data->input_data[pad];

		ags_base_plugin_connect_port((AgsBasePlugin *) dssi_plugin,
					     input_data->ladspa_handle,
					     port_index,
					     &(dssi_port[nth]->port_value.ags_port_ladspa));
	      }
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
    
    dssi_port[nth] = NULL;
  }

  /* set DSSI output */
  g_rec_mutex_lock(fx_dssi_channel_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxDssiAudioScopeData *scope_data;

    scope_data = fx_dssi_audio->scope_data[i];

    if(i == AGS_SOUND_SCOPE_PLAYBACK ||
       i == AGS_SOUND_SCOPE_NOTATION ||
       i == AGS_SOUND_SCOPE_MIDI){
      AgsFxDssiAudioChannelData *channel_data;
      AgsFxDssiAudioInputData *input_data;

      guint nth;
	  
      channel_data = scope_data->channel_data[audio_channel];	

      input_data = channel_data->input_data[pad];
      
      if(input_data->output == NULL &&
	 output_port_count > 0 &&
	 buffer_size > 0){
	input_data->output = (LADSPA_Data *) g_malloc(output_port_count * buffer_size * sizeof(LADSPA_Data));
      }
	  
      if(input_data->input == NULL &&
	 input_port_count > 0 &&
	 buffer_size > 0){
	input_data->input = (LADSPA_Data *) g_malloc(input_port_count * buffer_size * sizeof(LADSPA_Data));
      }

      for(nth = 0; nth < output_port_count; nth++){
	ags_base_plugin_connect_port((AgsBasePlugin *) dssi_plugin,
				     input_data->ladspa_handle,
				     output_port[nth],
				     &(input_data->output[nth]));
      }

      for(nth = 0; nth < input_port_count; nth++){
	ags_base_plugin_connect_port((AgsBasePlugin *) dssi_plugin,
				     input_data->ladspa_handle,
				     input_port[nth],
				     &(input_data->input[nth]));
      }

      ags_base_plugin_activate((AgsBasePlugin *) dssi_plugin,
			       input_data->ladspa_handle);
    }
  }  

  fx_dssi_channel->dssi_port = dssi_port;
  
  g_rec_mutex_unlock(fx_dssi_channel_mutex);

  /* unref */
  if(fx_dssi_audio != NULL){
    g_object_unref(fx_dssi_audio);
  }

  g_list_free_full(start_plugin_port,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_fx_dssi_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxDssiChannel
 *
 * Returns: the new #AgsFxDssiChannel
 *
 * Since: 3.3.0
 */
AgsFxDssiChannel*
ags_fx_dssi_channel_new(AgsChannel *channel)
{
  AgsFxDssiChannel *fx_dssi_channel;

  fx_dssi_channel = (AgsFxDssiChannel *) g_object_new(AGS_TYPE_FX_DSSI_CHANNEL,
						      "source", channel,
						      NULL);

  return(fx_dssi_channel);
}
