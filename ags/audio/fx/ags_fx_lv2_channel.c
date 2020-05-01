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

#include <ags/audio/fx/ags_fx_lv2_channel.h>

#include <ags/plugin/ags_lv2_manager.h>
#include <ags/plugin/ags_lv2_plugin.h>
#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_port_util.h>

#include <ags/audio/fx/ags_fx_lv2_audio.h>

#include <ags/i18n.h>

void ags_fx_lv2_channel_class_init(AgsFxLv2ChannelClass *fx_lv2_channel);
void ags_fx_lv2_channel_init(AgsFxLv2Channel *fx_lv2_channel);
void ags_fx_lv2_channel_dispose(GObject *gobject);
void ags_fx_lv2_channel_finalize(GObject *gobject);

void ags_fx_lv2_channel_notify_buffer_size_callback(GObject *gobject,
						    GParamSpec *pspec,
						    gpointer user_data);
void ags_fx_lv2_channel_notify_samplerate_callback(GObject *gobject,
						   GParamSpec *pspec,
						   gpointer user_data);

/**
 * SECTION:ags_fx_lv2_channel
 * @short_description: fx lv2 channel
 * @title: AgsFxLv2Channel
 * @section_id:
 * @include: ags/audio/fx/ags_fx_lv2_channel.h
 *
 * The #AgsFxLv2Channel class provides ports to the effect processor.
 */

static gpointer ags_fx_lv2_channel_parent_class = NULL;

static const gchar *ags_fx_lv2_channel_plugin_name = "ags-fx-lv2";

GType
ags_fx_lv2_channel_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_fx_lv2_channel = 0;

    static const GTypeInfo ags_fx_lv2_channel_info = {
      sizeof (AgsFxLv2ChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_fx_lv2_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_channel */
      sizeof (AgsFxLv2Channel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_fx_lv2_channel_init,
    };

    ags_type_fx_lv2_channel = g_type_register_static(AGS_TYPE_FX_NOTATION_CHANNEL,
						     "AgsFxLv2Channel",
						     &ags_fx_lv2_channel_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_fx_lv2_channel);
  }

  return g_define_type_id__volatile;
}

void
ags_fx_lv2_channel_class_init(AgsFxLv2ChannelClass *fx_lv2_channel)
{
  GObjectClass *gobject;

  ags_fx_lv2_channel_parent_class = g_type_class_peek_parent(fx_lv2_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) fx_lv2_channel;

  gobject->dispose = ags_fx_lv2_channel_dispose;
  gobject->finalize = ags_fx_lv2_channel_finalize;
}

void
ags_fx_lv2_channel_init(AgsFxLv2Channel *fx_lv2_channel)
{
  guint i;
  
  g_signal_connect(fx_lv2_channel, "notify::buffer-size",
		   G_CALLBACK(ags_fx_lv2_channel_notify_buffer_size_callback), NULL);

  g_signal_connect(fx_lv2_channel, "notify::samplerate",
		   G_CALLBACK(ags_fx_lv2_channel_notify_samplerate_callback), NULL);

  AGS_RECALL(fx_lv2_channel)->name = "ags-fx-lv2";
  AGS_RECALL(fx_lv2_channel)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_lv2_channel)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_lv2_channel)->xml_type = "ags-fx-lv2-channel";

  fx_lv2_channel->output_port_count = 0;
  fx_lv2_channel->output_port = NULL;
  
  fx_lv2_channel->input_port_count = 0;
  fx_lv2_channel->input_port = NULL;

  /* input data */
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    fx_lv2_channel->input_data[i] = ags_fx_lv2_channel_input_data_alloc();
      
    fx_lv2_channel->input_data[i]->parent = fx_lv2_channel;
  }
  
  fx_lv2_channel->lv2_plugin = NULL;

  fx_lv2_channel->lv2_port = NULL;
}

void
ags_fx_lv2_channel_dispose(GObject *gobject)
{
  AgsFxLv2Channel *fx_lv2_channel;
  
  fx_lv2_channel = AGS_FX_LV2_CHANNEL(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_fx_lv2_channel_parent_class)->dispose(gobject);
}

void
ags_fx_lv2_channel_finalize(GObject *gobject)
{
  AgsPort **iter;
  AgsFxLv2Channel *fx_lv2_channel;
  
  guint i;
  
  fx_lv2_channel = AGS_FX_LV2_CHANNEL(gobject);

  g_free(fx_lv2_channel->output_port);
  g_free(fx_lv2_channel->input_port);
  
  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    ags_fx_lv2_channel_input_data_free(fx_lv2_channel->input_data[i]);
  }
  
  if(fx_lv2_channel->lv2_plugin != NULL){
    g_object_unref(fx_lv2_channel->lv2_plugin);
  }
  
  if(fx_lv2_channel->lv2_port != NULL){
    for(iter = fx_lv2_channel->lv2_port; iter[0] != NULL; iter++){
      g_object_unref(iter[0]);
    }

    g_free(fx_lv2_channel->lv2_port);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_fx_lv2_channel_parent_class)->finalize(gobject);
}

void
ags_fx_lv2_channel_notify_buffer_size_callback(GObject *gobject,
					       GParamSpec *pspec,
					       gpointer user_data)
{
  AgsFxLv2Channel *fx_lv2_channel;

  guint output_port_count, input_port_count;
  guint buffer_size;
  guint i;
  
  GRecMutex *recall_mutex;
  
  fx_lv2_channel = AGS_FX_LV2_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_channel);

  /* get buffer size */
  g_object_get(fx_lv2_channel,
	       "buffer-size", &buffer_size,
	       NULL);
  
  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  output_port_count = fx_lv2_channel->output_port_count;
  input_port_count = fx_lv2_channel->input_port_count;

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxLv2ChannelInputData *input_data;

    input_data = fx_lv2_channel->input_data[i];

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
  
  g_rec_mutex_unlock(recall_mutex);
}

void
ags_fx_lv2_channel_notify_samplerate_callback(GObject *gobject,
					      GParamSpec *pspec,
					      gpointer user_data)
{
  AgsFxLv2Channel *fx_lv2_channel;

  AgsLv2Plugin *lv2_plugin;

  guint output_port_count;
  guint buffer_size;
  guint samplerate;
  guint i;
  
  GRecMutex *recall_mutex;
  GRecMutex *base_plugin_mutex;

  void (*deactivate)(LV2_Handle instance);
  void (*cleanup)(LV2_Handle instance);
  
  fx_lv2_channel = AGS_FX_LV2_CHANNEL(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_channel);

  /* get LV2 plugin */
  g_rec_mutex_lock(recall_mutex);

  lv2_plugin = fx_lv2_channel->lv2_plugin;

  g_rec_mutex_unlock(recall_mutex);

  if(lv2_plugin == NULL){
    return;
  }
  
  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate =  AGS_SOUNDCARD_DEFAULT_SAMPLERATE;

  g_object_get(fx_lv2_channel,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

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

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);

  for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
    AgsFxLv2ChannelInputData *input_data;

    input_data = fx_lv2_channel->input_data[i];

    if(deactivate != NULL){
      deactivate(input_data->lv2_handle);
    }

    if(cleanup != NULL){
      cleanup(input_data->lv2_handle);
    }

    input_data->lv2_handle = ags_base_plugin_instantiate((AgsBasePlugin *) lv2_plugin,
							 samplerate, buffer_size);
  }
  
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_lv2_channel_input_data_alloc:
 * 
 * Allocate #AgsFxLv2ChannelInputData-struct
 * 
 * Returns: the new #AgsFxLv2ChannelInputData-struct
 * 
 * Since: 3.3.0
 */
AgsFxLv2ChannelInputData*
ags_fx_lv2_channel_input_data_alloc()
{
  AgsFxLv2ChannelInputData *input_data;

  input_data = (AgsFxLv2ChannelInputData *) g_malloc(sizeof(AgsFxLv2ChannelInputData));

  input_data->parent = NULL;

  input_data->output = NULL;
  input_data->input = NULL;

  input_data->lv2_handle = NULL;

  return(input_data);
}

/**
 * ags_fx_lv2_channel_input_data_free:
 * @input_data: the #AgsFxLv2ChannelInputData-struct
 * 
 * Free @input_data.
 * 
 * Since: 3.3.0
 */
void
ags_fx_lv2_channel_input_data_free(AgsFxLv2ChannelInputData *input_data)
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
    
    if(input_data->parent != NULL){
      lv2_plugin = AGS_FX_LV2_CHANNEL(input_data->parent)->lv2_plugin;
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
  
  g_free(input_data);
}

/**
 * ags_fx_lv2_channel_load_plugin:
 * @fx_lv2_channel: the #AgsFxLv2Channel
 * 
 * Load plugin of @fx_lv2_channel.
 * 
 * Since: 3.3.0
 */
void
ags_fx_lv2_channel_load_plugin(AgsFxLv2Channel *fx_lv2_channel)
{
  AgsLv2Manager *lv2_manager;
  AgsLv2Plugin *lv2_plugin;

  gchar *filename, *effect;

  guint buffer_size;
  guint samplerate;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_LV2_CHANNEL(fx_lv2_channel)){
    return;
  }

  lv2_manager = ags_lv2_manager_get_instance();
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_channel);

  /* get filename and effect */
  filename = NULL;
  effect = NULL;

  buffer_size = AGS_SOUNDCARD_DEFAULT_BUFFER_SIZE;
  samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  
  g_object_get(fx_lv2_channel,
	       "filename", &filename,
	       "effect", &effect,
	       "buffer-size", &buffer_size,
	       "samplerate", &samplerate,
	       NULL);

  /* check if already loaded */
  g_rec_mutex_lock(recall_mutex);

  lv2_plugin = fx_lv2_channel->lv2_plugin;

  if(lv2_plugin == NULL){
    lv2_plugin =
      fx_lv2_channel->lv2_plugin = ags_lv2_manager_find_lv2_plugin(lv2_manager,
								   filename, effect);
  }    
  
  g_rec_mutex_unlock(recall_mutex);
    
  if(lv2_plugin != NULL &&
     !ags_base_plugin_test_flags((AgsBasePlugin *) lv2_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
    guint i;
    
    /* set lv2 plugin */    
    g_rec_mutex_lock(recall_mutex);

    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsFxLv2ChannelInputData *input_data;

      input_data = fx_lv2_channel->input_data[i];
      
      if(input_data->lv2_handle == NULL){
	input_data->lv2_handle = ags_base_plugin_instantiate((AgsBasePlugin *) lv2_plugin,
							     samplerate, buffer_size);
      }
    }
    
    g_rec_mutex_unlock(recall_mutex);
  }  
  
  g_free(filename);
  g_free(effect);
}

/**
 * ags_fx_lv2_channel_load_port:
 * @fx_lv2_channel: the #AgsFxLv2Channel
 * 
 * Load port of @fx_lv2_channel.
 * 
 * Since: 3.3.0
 */
void
ags_fx_lv2_channel_load_port(AgsFxLv2Channel *fx_lv2_channel)
{
  AgsChannel *input;
  AgsFxLv2Audio *fx_lv2_audio;
  AgsPort **lv2_port;
  
  AgsLv2Plugin *lv2_plugin;

  GList *start_plugin_port, *plugin_port;

  guint *output_port;
  guint *input_port;
  
  guint pad;
  guint output_port_count;
  guint input_port_count;
  guint control_port_count;
  guint buffer_size;
  guint nth;
  guint i, j;
  gboolean is_live_instrument;
  
  GRecMutex *fx_lv2_audio_mutex;
  GRecMutex *fx_lv2_channel_mutex;

  if(!AGS_IS_FX_LV2_CHANNEL(fx_lv2_channel)){
    return;
  }

  fx_lv2_channel_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_channel);

  g_rec_mutex_lock(fx_lv2_channel_mutex);

  if(fx_lv2_channel->lv2_port != NULL){
    g_rec_mutex_unlock(fx_lv2_channel_mutex);
    
    return;
  }

  g_rec_mutex_unlock(fx_lv2_channel_mutex);
  
  fx_lv2_audio = NULL;

  g_object_get(fx_lv2_channel,
	       "recall-audio", &fx_lv2_audio,
	       "buffer-size", &buffer_size,
	       NULL);

  if(fx_lv2_audio == NULL){
    return;
  }

  is_live_instrument = ags_fx_lv2_audio_test_flags(fx_lv2_audio, AGS_FX_LV2_AUDIO_LIVE_INSTRUMENT);

  if(is_live_instrument){
    if(fx_lv2_audio != NULL){
      g_object_unref(fx_lv2_audio);
    }
    
    return;
  }
  
  /* get recall mutex */
  fx_lv2_audio_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_lv2_audio);

  input = NULL;

  g_object_get(fx_lv2_channel,
	       "source", &input,
	       NULL);

  pad = 0;

  g_object_get(input,
	       "pad", &pad,
	       NULL);
  
  /* get LV2 plugin */
  g_rec_mutex_lock(fx_lv2_audio_mutex);

  lv2_plugin = fx_lv2_audio->lv2_plugin;
  
  g_rec_mutex_unlock(fx_lv2_audio_mutex);

  /* get LV2 port */
  g_rec_mutex_lock(fx_lv2_channel_mutex);

  lv2_port = fx_lv2_channel->lv2_port;

  g_rec_mutex_unlock(fx_lv2_channel_mutex);

  g_object_get(lv2_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  if(ags_base_plugin_test_flags((AgsBasePlugin *) lv2_plugin, AGS_BASE_PLUGIN_IS_INSTRUMENT)){
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
      lv2_port = (AgsPort **) g_malloc((control_port_count + 1) * sizeof(AgsPort *));

      plugin_port = start_plugin_port;
    
      for(nth = 0; nth < control_port_count; nth++){
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
	  
	  ags_recall_set_flags((AgsRecall *) fx_lv2_channel,
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

	ags_recall_add_port((AgsRecall *) fx_lv2_channel,
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

	      if(pad < 128){
		AgsFxLv2AudioInputData *input_data;
	  
		input_data = channel_data->input_data[pad];

		ags_base_plugin_connect_port((AgsBasePlugin *) lv2_plugin,
					     input_data->lv2_handle,
					     port_index,
					     &(lv2_port[nth]->port_value.ags_port_float));
	      }
	    }
	  }
	}

	g_free(plugin_name);
	g_free(specifier);
	g_free(control_port);

	g_value_unset(&default_value);
      }

      lv2_port[nth] = NULL;
    }
  }else{
    /* get control port count */
    plugin_port = start_plugin_port;

    output_port = NULL;
    input_port = NULL;
  
    output_port_count = 0;
    input_port_count = 0;

    control_port_count = 0;
  
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

      plugin_port = plugin_port->next;
    }

    /*  */
    if(control_port_count > 0){
      lv2_port = (AgsPort **) g_malloc((control_port_count + 1) * sizeof(AgsPort *));

      plugin_port = start_plugin_port;
    
      for(nth = 0; nth < control_port_count; nth++){
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
	  
	  ags_recall_set_flags((AgsRecall *) fx_lv2_channel,
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

	ags_recall_add_port((AgsRecall *) fx_lv2_channel,
			    lv2_port[nth]);

	/* connect port */
	for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
	  AgsFxLv2ChannelInputData *input_data;

	  input_data = fx_lv2_channel->input_data[i];

	  ags_base_plugin_connect_port((AgsBasePlugin *) lv2_plugin,
				       input_data->lv2_handle,
				       port_index,
				       &(lv2_port[nth]->port_value.ags_port_float));
	}
      
	g_free(plugin_name);
	g_free(specifier);
	g_free(control_port);

	g_value_unset(&default_value);
      }

      lv2_port[nth] = NULL;
    }

    /* set LV2 output */
    g_rec_mutex_lock(fx_lv2_channel_mutex);

    for(i = 0; i < AGS_SOUND_SCOPE_LAST; i++){
      AgsFxLv2ChannelInputData *input_data;

      input_data = fx_lv2_channel->input_data[i];

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

      for(nth = 0; nth < output_port_count; nth++){
	ags_base_plugin_connect_port((AgsBasePlugin *) lv2_plugin,
				     input_data->lv2_handle,
				     output_port[nth],
				     &(input_data->output[nth]));
      }
    }  

    fx_lv2_channel->output_port_count = output_port_count;
    fx_lv2_channel->output_port = output_port;

    fx_lv2_channel->input_port_count = input_port_count;
    fx_lv2_channel->input_port = input_port;
  
    g_rec_mutex_unlock(fx_lv2_channel_mutex);
  }
  
  /* set LV2 port */
  g_rec_mutex_lock(fx_lv2_channel_mutex);

  fx_lv2_channel->lv2_port = lv2_port;
  
  g_rec_mutex_unlock(fx_lv2_channel_mutex);

  /* unref */
  if(fx_lv2_audio != NULL){
    g_object_unref(fx_lv2_audio);
  }

  g_list_free_full(start_plugin_port,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_fx_lv2_channel_new:
 * @channel: the #AgsChannel
 *
 * Create a new instance of #AgsFxLv2Channel
 *
 * Returns: the new #AgsFxLv2Channel
 *
 * Since: 3.3.0
 */
AgsFxLv2Channel*
ags_fx_lv2_channel_new(AgsChannel *channel)
{
  AgsFxLv2Channel *fx_lv2_channel;

  fx_lv2_channel = (AgsFxLv2Channel *) g_object_new(AGS_TYPE_FX_LV2_CHANNEL,
						    "source", channel,
						    NULL);

  return(fx_lv2_channel);
}
