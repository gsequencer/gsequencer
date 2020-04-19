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
#include <ags/plugin/ags_base_plugin.h>
#include <ags/plugin/ags_plugin_port.h>

void ags_fx_dssi_audio_class_init(AgsFxDssiAudioClass *fx_dssi_audio);
void ags_fx_dssi_audio_init(AgsFxDssiAudio *fx_dssi_audio);
void ags_fx_dssi_audio_dispose(GObject *gobject);
void ags_fx_dssi_audio_finalize(GObject *gobject);

void ags_fx_dssi_audio_notify_buffer_size_callback(GObject *gobject,
						   GParamSpec *pspec,
						   gpointer user_data);

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
  guint buffer_size;
  guint i;

  g_signal_connect(fx_dssi_audio, "notify::buffer-size",
		   G_CALLBACK(ags_fx_dssi_audio_notify_buffer_size_callback), NULL);

  AGS_RECALL(fx_dssi_audio)->name = "ags-fx-dssi";
  AGS_RECALL(fx_dssi_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(fx_dssi_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(fx_dssi_audio)->xml_type = "ags-fx-dssi-audio";

  fx_dssi_audio->flags = AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT;
  
  /* get buffer size */
  g_object_get(fx_dssi_audio,
	       "buffer-size", &buffer_size,
	       NULL);

  fx_dssi_audio->input_lines = 0;
  fx_dssi_audio->output_lines = 0;
  
  fx_dssi_audio->input_port = NULL;
  fx_dssi_audio->output_port = NULL;

  fx_dssi_audio->bank = 0;
  fx_dssi_audio->program = 0;
  
  fx_dssi_audio->event_count = 0;

  fx_dssi_audio->input = (LADSPA_Data **) g_malloc(sizeof(LADSPA_Data *));
  fx_dssi_audio->input[0] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));

  fx_dssi_audio->output = (LADSPA_Data **) g_malloc(sizeof(LADSPA_Data *));
  fx_dssi_audio->output[0] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));

  for(i = 0; i < 128; i++){
    fx_dssi_audio->event_buffer[i] = (snd_seq_event_t *) g_malloc(sizeof(snd_seq_event_t));
    
    fx_dssi_audio->key_on[i] = 0;
  }
  
  fx_dssi_audio->ladspa_handle = NULL;
  
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
  AgsDssiPlugin *dssi_plugin;
  
  gpointer plugin_descriptor;
    
  guint i;

  GRecMutex *base_plugin_mutex;
  
  fx_dssi_audio = AGS_FX_DSSI_AUDIO(gobject);

  dssi_plugin = fx_dssi_audio->dssi_plugin;

  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(dssi_plugin);

  /* get plugin descriptor */
  plugin_descriptor = NULL;

  if(dssi_plugin != NULL){
    g_rec_mutex_lock(base_plugin_mutex);
  
    plugin_descriptor = AGS_BASE_PLUGIN(dssi_plugin)->plugin_descriptor;
    
    g_rec_mutex_unlock(base_plugin_mutex);
  }
  
  /* finalize */
  g_free(fx_dssi_audio->input_port);
  g_free(fx_dssi_audio->output_port);

  /* free buffer */
  if(ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT)){
    g_free(fx_dssi_audio->input[0]);
    g_free(fx_dssi_audio->output[0]);

    if(plugin_descriptor != NULL){
      g_rec_mutex_lock(base_plugin_mutex);
    
      if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate != NULL){
	AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate(fx_dssi_audio->ladspa_handle[0]);
      }

      if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup != NULL){
	AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup(fx_dssi_audio->ladspa_handle[0]);
      }
    
      g_rec_mutex_unlock(base_plugin_mutex);
    }
  }else{
    for(i = 0; i < 128; i++){
      g_free(fx_dssi_audio->input[i]);
      g_free(fx_dssi_audio->output[i]);

      if(plugin_descriptor != NULL){
	g_rec_mutex_lock(base_plugin_mutex);
    
	if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate != NULL){
	  AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate(fx_dssi_audio->ladspa_handle[i]); 
	}
	
	if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup != NULL){
	  AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup(fx_dssi_audio->ladspa_handle[i]);
	}
    
	g_rec_mutex_unlock(base_plugin_mutex);
      }
    }
  }
  
  g_free(fx_dssi_audio->input);
  g_free(fx_dssi_audio->output);
    
  g_free(fx_dssi_audio->ladspa_handle);

  for(i = 0; i < 128; i++){
    g_free(fx_dssi_audio->event_buffer[i]);
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
ags_fx_dssi_audio_notify_buffer_size_callback(GObject *gobject,
					      GParamSpec *pspec,
					      gpointer user_data)
{
  AgsFxDssiAudio *fx_dssi_audio;

  guint buffer_size;
  guint i;
  
  GRecMutex *recall_mutex;
  
  fx_dssi_audio = AGS_FX_DSSI_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  /* get buffer size */
  g_object_get(fx_dssi_audio,
	       "buffer-size", &buffer_size,
	       NULL);

  /* reallocate buffer - apply buffer size */
  g_rec_mutex_lock(recall_mutex);
  
  if(ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT)){
    g_free(fx_dssi_audio->input[0]);
    g_free(fx_dssi_audio->output[0]);
  
    fx_dssi_audio->input[0] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));
    fx_dssi_audio->output[0] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));
  }else{
    for(i = 0; i < 128; i++){
      g_free(fx_dssi_audio->input[i]);
      g_free(fx_dssi_audio->output[i]);
  
      fx_dssi_audio->input[i] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));
      fx_dssi_audio->output[i] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));
    }
  }
  
  g_rec_mutex_unlock(recall_mutex);
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

  if((AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT & (flags)) != 0){
    if(!ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT)){
      guint buffer_size;
      guint i;

      g_object_get(fx_dssi_audio,
		   "buffer-size", &buffer_size,
		   NULL);

      g_rec_mutex_lock(recall_mutex);

      fx_dssi_audio->input = (LADSPA_Data **) g_realloc(fx_dssi_audio->input,
							128 * sizeof(LADSPA_Data *));
      fx_dssi_audio->output = (LADSPA_Data **) g_realloc(fx_dssi_audio->output,
							 128 * sizeof(LADSPA_Data *));
      
      fx_dssi_audio->ladspa_handle = (LADSPA_Handle **) g_realloc(fx_dssi_audio->ladspa_handle,
								  128 * sizeof(LADSPA_Handle *));
      
      for(i = 1; i < 128; i++){
	fx_dssi_audio->input[i] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));
	
	fx_dssi_audio->output[i] = (LADSPA_Data *) g_malloc(buffer_size * sizeof(LADSPA_Data));

	fx_dssi_audio->ladspa_handle[i] = NULL;
      }
    }

    g_rec_mutex_unlock(recall_mutex);
  }
  
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

  if((AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT & (flags)) != 0){
    if(ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT)){
      g_rec_mutex_lock(recall_mutex);
      
      fx_dssi_audio->input = (LADSPA_Data **) g_realloc(fx_dssi_audio->input,
							sizeof(LADSPA_Data *));
      fx_dssi_audio->output = (LADSPA_Data **) g_realloc(fx_dssi_audio->output,
							 sizeof(LADSPA_Data *));

      fx_dssi_audio->ladspa_handle = (LADSPA_Handle **) g_realloc(fx_dssi_audio->ladspa_handle,
								  sizeof(LADSPA_Handle *));

      g_rec_mutex_unlock(recall_mutex);
    }
  }
  
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

  LADSPA_Handle **ladspa_handle;

  gchar *filename, *effect;

  guint buffer_size;
  guint samplerate;
  guint effect_index;
  guint i;
  
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
  ladspa_handle = fx_dssi_audio->ladspa_handle;
  
  g_rec_mutex_unlock(recall_mutex);

  if(dssi_plugin != NULL ||
     ladspa_handle != NULL){
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
    /* set dssi plugin */
    g_object_ref(dssi_plugin);
    
    g_rec_mutex_lock(recall_mutex);

    fx_dssi_audio->dssi_plugin = dssi_plugin;
    
    g_rec_mutex_unlock(recall_mutex);

    /* instantiate */
    if(ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT)){
      ladspa_handle = (LADSPA_Handle **) g_malloc(sizeof(LADSPA_Handle *));
    
      ladspa_handle[0] = ags_base_plugin_instantiate(dssi_plugin,
						     samplerate, buffer_size);
    }else{
      ladspa_handle = (LADSPA_Handle **) g_malloc(128 * sizeof(LADSPA_Handle *));
      
      for(i = 0; i < 128; i++){
	ladspa_handle[i] = ags_base_plugin_instantiate(dssi_plugin,
						       samplerate, buffer_size);
      }
    }

    g_rec_mutex_lock(recall_mutex);

    fx_dssi_audio->ladspa_handle = ladspa_handle;
    
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

  guint control_port_count;
  guint i;
  
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

    plugin_port != NULL;
  }

  /*  */
  if(control_port_count > 0){
    dssi_port = (AgsPort **) g_malloc((control_port_count + 1) * sizeof(AgsPort *));

    plugin_port = start_plugin_port;
    
    for(i = 0; i < control_port_count; i++){
      AgsPluginPort *current_plugin_port;

      gchar *plugin_name;
      gchar *specifier;
      gchar *control_port;
      
      GValue default_value = {0,};

      GRecMutex *plugin_port_mutex;
      
      current_plugin_port = AGS_PLUGIN_PORT(plugin_port->data);

      /* get plugin port mutex */
      plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(current_plugin_port);

      /* plugin name, specifier and control port */
      plugin_name = g_strdup_printf("dssi-%u", dssi_plugin->unique_id);

      g_object_get(current_plugin_port,
		   "port-name", &specifier,
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

      g_free(plugin_name);
      g_free(specifier);
      g_free(control_port);

      g_value_unset(&default_value);
    }

    dssi_port[i] = NULL;
  }

  /* set DSSI port */
  g_rec_mutex_lock(recall_mutex);

  fx_dssi_audio->dssi_port = dssi_port;
  
  g_rec_mutex_unlock(recall_mutex);

  /* unref */
  g_list_free_full(start_plugin_port,
		   (GDestroyNotify) g_object_unref);
}

/**
 * ags_fx_dssi_audio_unload_plugin:
 * @fx_dssi_audio: the #AgsFxDssiAudio
 * 
 * Unload plugin of @fx_dssi_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_audio_unload_plugin(AgsFxDssiAudio *fx_dssi_audio)
{  
  AgsDssiPlugin *dssi_plugin;

  LADSPA_Handle **ladspa_handle;

  gpointer plugin_descriptor;

  guint i;
  
  GRecMutex *recall_mutex;
  GRecMutex *base_plugin_mutex;

  if(!AGS_IS_FX_DSSI_AUDIO(fx_dssi_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  /* check if not loaded */
  g_rec_mutex_lock(recall_mutex);

  dssi_plugin = fx_dssi_audio->dssi_plugin;
  ladspa_handle = fx_dssi_audio->ladspa_handle;
  
  g_rec_mutex_unlock(recall_mutex);

  if(dssi_plugin == NULL ||
     ladspa_handle == NULL){
    return;
  }

  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(dssi_plugin);

  /* get plugin descriptor */
  plugin_descriptor = NULL;

  if(dssi_plugin != NULL){
    g_rec_mutex_lock(base_plugin_mutex);
  
    plugin_descriptor = AGS_BASE_PLUGIN(dssi_plugin)->plugin_descriptor;
    
    g_rec_mutex_unlock(base_plugin_mutex);
  }
  
  if(ags_fx_dssi_audio_test_flags(fx_dssi_audio, AGS_FX_DSSI_AUDIO_LIVE_INSTRUMENT)){
    /* alloc empty */
    g_rec_mutex_lock(recall_mutex);

    fx_dssi_audio->ladspa_handle = NULL;
    
    g_rec_mutex_unlock(recall_mutex);

    /* destroy old */
    if(plugin_descriptor != NULL){
      g_rec_mutex_lock(base_plugin_mutex);
    
      if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate != NULL){
	AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate(ladspa_handle[0]);
      }

      if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup != NULL){
	AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup(ladspa_handle[0]);
      }
    
      g_rec_mutex_unlock(base_plugin_mutex);
    }
  }else{
    /* alloc empty */
    g_rec_mutex_lock(recall_mutex);

    fx_dssi_audio->ladspa_handle = NULL;
    
    g_rec_mutex_unlock(recall_mutex);
    
    /* destroy old */
    if(plugin_descriptor != NULL){    
      g_rec_mutex_lock(base_plugin_mutex);
    
      for(i = 0; i < 128; i++){
	if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate != NULL){
	  AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->deactivate(ladspa_handle[i]);
	}

	if(AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup != NULL){
	  AGS_DSSI_PLUGIN_DESCRIPTOR(plugin_descriptor)->LADSPA_Plugin->cleanup(ladspa_handle[i]);
	}
      }
    
      g_rec_mutex_unlock(base_plugin_mutex);
    }
  }
  
  g_rec_mutex_lock(recall_mutex);
  
  g_object_unref(fx_dssi_audio->dssi_plugin);
  
  fx_dssi_audio->dssi_plugin = NULL;
    
  g_rec_mutex_unlock(recall_mutex);
}

/**
 * ags_fx_dssi_audio_unload_port:
 * @fx_dssi_audio: the #AgsFxDssiAudio
 * 
 * Unload port of @fx_dssi_audio.
 * 
 * Since: 3.3.0
 */
void
ags_fx_dssi_audio_unload_port(AgsFxDssiAudio *fx_dssi_audio)
{
  AgsPort **dssi_port, **iter;

  guint i;

  GRecMutex *recall_mutex;

  if(!AGS_IS_FX_DSSI_AUDIO(fx_dssi_audio)){
    return;
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(fx_dssi_audio);

  g_rec_mutex_lock(recall_mutex);

  dssi_port = fx_dssi_audio->dssi_port;
  
  g_rec_mutex_unlock(recall_mutex);

  if(dssi_port == NULL){
    return;
  }
  
  g_rec_mutex_lock(recall_mutex);

  fx_dssi_audio->dssi_port = NULL;

  g_rec_mutex_unlock(recall_mutex);

  for(iter = dssi_port; iter[0] != NULL; iter++){
    g_object_unref(iter[0]);
  }

  g_free(dssi_port);
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
