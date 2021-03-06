/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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
 * 
 * Yuri Victorovich <yuri@FreeBSD.org> (tiny change) - provided FreeBSD and
 *  DragonFly macros.
 */

#include <ags/audio/recall/ags_play_dssi_audio.h>

#include <ags/plugin/ags_dssi_manager.h>
#include <ags/plugin/ags_ladspa_conversion.h>
#include <ags/plugin/ags_plugin_port.h>

#include <ags/audio/ags_port.h>

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__DragonFly__)
#include <machine/endian.h>
#else
#ifndef AGS_W32API
#include <endian.h>
#endif
#endif

#ifndef AGS_W32API
#include <dlfcn.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <libxml/tree.h>

#include <ags/i18n.h>

void ags_play_dssi_audio_class_init(AgsPlayDssiAudioClass *play_dssi_audio);
void ags_play_dssi_audio_init(AgsPlayDssiAudio *play_dssi_audio);
void ags_play_dssi_audio_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec);
void ags_play_dssi_audio_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec);
void ags_play_dssi_audio_dispose(GObject *gobject);
void ags_play_dssi_audio_finalize(GObject *gobject);

/**
 * SECTION:ags_play_dssi_audio
 * @short_description: play audio dssi
 * @title: AgsPlayDssiAudio
 * @section_id:
 * @include: ags/audio/recall/ags_play_dssi_audio.h
 *
 * The #AgsPlayDssiAudio class provides ports to the effect processor.
 */

enum{
  PROP_0,
  PROP_BANK,
  PROP_PROGRAM,
  PROP_PLUGIN,
  PROP_INPUT_LINES,
  PROP_OUTPUT_LINES,
};

static gpointer ags_play_dssi_audio_parent_class = NULL;

const gchar *ags_play_dssi_audio_plugin_name = "ags-play-dssi";
const gchar *ags_play_dssi_audio_specifier[] = {
};
const gchar *ags_play_dssi_audio_control_port[] = {
};

GType
ags_play_dssi_audio_get_type()
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_play_dssi_audio;

    static const GTypeInfo ags_play_dssi_audio_info = {
      sizeof (AgsPlayDssiAudioClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_play_dssi_audio_class_init,
      NULL, /* class_finalize */
      NULL, /* class_audio */
      sizeof (AgsPlayDssiAudio),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_play_dssi_audio_init,
    };

    ags_type_play_dssi_audio = g_type_register_static(AGS_TYPE_RECALL_AUDIO,
						      "AgsPlayDssiAudio",
						      &ags_play_dssi_audio_info,
						      0);

    g_once_init_leave (&g_define_type_id__volatile, ags_type_play_dssi_audio);
  }

  return g_define_type_id__volatile;
}

void
ags_play_dssi_audio_class_init(AgsPlayDssiAudioClass *play_dssi_audio)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_play_dssi_audio_parent_class = g_type_class_peek_parent(play_dssi_audio);

  /* GObjectClass */
  gobject = (GObjectClass *) play_dssi_audio;

  gobject->set_property = ags_play_dssi_audio_set_property;
  gobject->get_property = ags_play_dssi_audio_get_property;

  gobject->dispose = ags_play_dssi_audio_dispose;
  gobject->finalize = ags_play_dssi_audio_finalize;

  /* properties */
  /**
   * AgsPlayDssi:bank:
   *
   * The selected bank.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("bank",
				 i18n_pspec("bank"),
				 i18n_pspec("The selected bank"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BANK,
				  param_spec);

  /**
   * AgsPlayDssi:program:
   *
   * The selected program.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("program",
				 i18n_pspec("program"),
				 i18n_pspec("The selected program"),
				 0,
				 G_MAXUINT32,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PROGRAM,
				  param_spec);


  /**
   * AgsPlayDssiAudio:plugin:
   *
   * The plugin's plugin object.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_object("plugin",
				    i18n_pspec("the plugin"),
				    i18n_pspec("The plugin as plugin object"),
				    AGS_TYPE_DSSI_PLUGIN,
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PLUGIN,
				  param_spec);

  /**
   * AgsPlayDssiAudio:input-lines:
   *
   * The effect's input lines count.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_ulong("input-lines",
				   i18n_pspec("input lines of effect"),
				   i18n_pspec("The effect's count of input lines"),
				   0,
				   65535,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INPUT_LINES,
				  param_spec);

  /**
   * AgsPlayDssiAudio:output-lines:
   *
   * The effect's output lines count.
   * 
   * Since: 3.0.0
   */
  param_spec =  g_param_spec_ulong("output-lines",
				   i18n_pspec("output lines of effect"),
				   i18n_pspec("The effect's count of output lines"),
				   0,
				   65535,
				   0,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_OUTPUT_LINES,
				  param_spec);
}

void
ags_play_dssi_audio_init(AgsPlayDssiAudio *play_dssi_audio)
{
  GList *port;

  AGS_RECALL(play_dssi_audio)->name = "ags-play-dssi";
  AGS_RECALL(play_dssi_audio)->version = AGS_RECALL_DEFAULT_VERSION;
  AGS_RECALL(play_dssi_audio)->build_id = AGS_RECALL_DEFAULT_BUILD_ID;
  AGS_RECALL(play_dssi_audio)->xml_type = "ags-play-dssi-audio";

  play_dssi_audio->bank = 0;
  play_dssi_audio->program = 0;
  
  play_dssi_audio->plugin = NULL;
  play_dssi_audio->plugin_descriptor = NULL;

  play_dssi_audio->input_port = NULL;
  play_dssi_audio->input_lines = 0;

  play_dssi_audio->output_port = NULL;
  play_dssi_audio->output_lines = 0;
}

void
ags_play_dssi_audio_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec)
{
  AgsPlayDssiAudio *play_dssi_audio;

  GRecMutex *recall_mutex;

  play_dssi_audio = AGS_PLAY_DSSI_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_dssi_audio);

  switch(prop_id){
  case PROP_BANK:
    {
      g_rec_mutex_lock(recall_mutex);

      play_dssi_audio->bank = g_value_get_uint(value);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PROGRAM:
    {
      g_rec_mutex_lock(recall_mutex);

      play_dssi_audio->program = g_value_get_uint(value);
      
      g_rec_mutex_unlock(recall_mutex);	
    }
    break;
  case PROP_PLUGIN:
    {
      AgsDssiPlugin *dssi_plugin;
      
      dssi_plugin = g_value_get_object(value);

      g_rec_mutex_lock(recall_mutex);

      if(play_dssi_audio->plugin == dssi_plugin){
	g_rec_mutex_unlock(recall_mutex);
	
	return;
      }

      if(play_dssi_audio->plugin != NULL){
	g_object_unref(play_dssi_audio->plugin);
      }

      if(dssi_plugin != NULL){
	g_object_ref(dssi_plugin);
      }

      play_dssi_audio->plugin = dssi_plugin;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_INPUT_LINES:
    {
      unsigned long effect_input_lines;
      
      effect_input_lines = g_value_get_ulong(value);

      g_rec_mutex_lock(recall_mutex);

      if(effect_input_lines == play_dssi_audio->input_lines){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      play_dssi_audio->input_lines = effect_input_lines;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_OUTPUT_LINES:
    {
      unsigned long effect_output_lines;
      
      effect_output_lines = g_value_get_ulong(value);

      g_rec_mutex_lock(recall_mutex);

      if(effect_output_lines == play_dssi_audio->output_lines){
	g_rec_mutex_unlock(recall_mutex);

	return;
      }

      play_dssi_audio->output_lines = effect_output_lines;

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_play_dssi_audio_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec)
{
  AgsPlayDssiAudio *play_dssi_audio;

  GRecMutex *recall_mutex;

  play_dssi_audio = AGS_PLAY_DSSI_AUDIO(gobject);

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_dssi_audio);

  switch(prop_id){
  case PROP_BANK:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_uint(value, play_dssi_audio->bank);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_PROGRAM:
    {
      g_rec_mutex_lock(recall_mutex);
      
      g_value_set_uint(value, play_dssi_audio->program);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_PLUGIN:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_object(value, play_dssi_audio->plugin);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_INPUT_LINES:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_ulong(value, play_dssi_audio->input_lines);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  case PROP_OUTPUT_LINES:
    {
      g_rec_mutex_lock(recall_mutex);

      g_value_set_ulong(value, play_dssi_audio->output_lines);

      g_rec_mutex_unlock(recall_mutex);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_play_dssi_audio_dispose(GObject *gobject)
{
  AgsPlayDssiAudio *play_dssi_audio;

  play_dssi_audio = (AgsPlayDssiAudio *) gobject;

  if(play_dssi_audio->plugin != NULL){
    g_object_unref(play_dssi_audio->plugin);

    play_dssi_audio->plugin = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_play_dssi_audio_parent_class)->dispose(gobject);
}

void
ags_play_dssi_audio_finalize(GObject *gobject)
{
  AgsPlayDssiAudio *play_dssi_audio;

  play_dssi_audio = (AgsPlayDssiAudio *) gobject;

  if(play_dssi_audio->plugin != NULL){
    g_object_unref(play_dssi_audio->plugin);
  }

  /* call parent */
  G_OBJECT_CLASS(ags_play_dssi_audio_parent_class)->finalize(gobject);
}

/**
 * ags_play_dssi_audio_load:
 * @play_dssi_audio: the #AgsPlayDssiAudio
 *
 * Set up DSSI handle.
 * 
 * Since: 3.0.0
 */
void
ags_play_dssi_audio_load(AgsPlayDssiAudio *play_dssi_audio)
{
  AgsDssiPlugin *dssi_plugin;

  gchar *filename, *effect;

  guint effect_index;
  
  void *plugin_so;
  DSSI_Descriptor_Function dssi_descriptor;
  DSSI_Descriptor *plugin_descriptor;
  
  GRecMutex *recall_mutex;

  if(!AGS_IS_PLAY_DSSI_AUDIO(play_dssi_audio)){
    return;
  }
  
  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_dssi_audio);

  /* get filename and effect */
  g_rec_mutex_lock(recall_mutex);

  filename = g_strdup(AGS_RECALL(play_dssi_audio)->filename);
  effect = g_strdup(AGS_RECALL(play_dssi_audio)->effect);
  
  g_rec_mutex_unlock(recall_mutex);
  
  /* find AgsDssiPlugin */
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						  filename, effect);

  /* get some fields */
  g_object_get(play_dssi_audio,
	       "effect-index", &effect_index,
	       NULL);

  play_dssi_audio->plugin = dssi_plugin;
  
  g_object_get(dssi_plugin,
	       "plugin-so", &plugin_so,
	       NULL);

  /* dssi descriptor function - dlsym */
  if(plugin_so){
    gboolean success;
    
    success = FALSE;

#ifdef AGS_W32API
    dssi_descriptor = (DSSI_Descriptor_Function) GetProcAddress(plugin_so,
								"dssi_descriptor");
    
    success = (!dssi_descriptor) ? FALSE: TRUE;
#else
    dssi_descriptor = (DSSI_Descriptor_Function) dlsym(plugin_so,
						       "dssi_descriptor");
  
    success = (dlerror() == NULL) ? TRUE: FALSE;
#endif

    if(success && dssi_descriptor){
      g_rec_mutex_lock(recall_mutex);
      
      play_dssi_audio->plugin_descriptor = 
	plugin_descriptor = dssi_descriptor((unsigned long) effect_index);

      g_rec_mutex_unlock(recall_mutex);
    }
  }

  /* free */
  g_free(filename);
  g_free(effect);
}

/**
 * ags_play_dssi_audio_load_ports:
 * @play_dssi_audio: the #AgsPlayDssiAudio
 *
 * Set up DSSI ports.
 *
 * Returns: (element-type AgsAudio.Port) (transfer full): the #GList-struct containing #AgsPort.
 * 
 * Since: 3.0.0
 */
GList*
ags_play_dssi_audio_load_ports(AgsPlayDssiAudio *play_dssi_audio)
{
  AgsPort *current;

  AgsDssiPlugin *dssi_plugin;

  GList *start_port;
  GList *start_plugin_port, *plugin_port;

  gchar *filename, *effect;
  gchar *plugin_name;

  unsigned long port_count;
  unsigned long i;

  GRecMutex *recall_mutex;
  GRecMutex *base_plugin_mutex;

  if(!AGS_IS_PLAY_DSSI_AUDIO(play_dssi_audio)){
    return(NULL);
  }

  /* get recall mutex */
  recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(play_dssi_audio);

  /* get filename and effect */
  g_rec_mutex_lock(recall_mutex);

  filename = g_strdup(AGS_RECALL(play_dssi_audio)->filename);
  effect = g_strdup(AGS_RECALL(play_dssi_audio)->effect);
  
  g_rec_mutex_unlock(recall_mutex);
  
  /* find AgsDssiPlugin */
  dssi_plugin = ags_dssi_manager_find_dssi_plugin(ags_dssi_manager_get_instance(),
						  filename, effect);

   play_dssi_audio->plugin = dssi_plugin;

  /* get plugin port */
  g_object_get(dssi_plugin,
	       "plugin-port", &start_plugin_port,
	       NULL);

  /* get base plugin mutex */
  base_plugin_mutex = AGS_BASE_PLUGIN_GET_OBJ_MUTEX(dssi_plugin);
  
  /* get plugin name */
  g_rec_mutex_lock(base_plugin_mutex);

  plugin_name = g_strdup_printf("dssi-%u",
				dssi_plugin->unique_id);

  g_rec_mutex_unlock(base_plugin_mutex);
  
  plugin_port = start_plugin_port;
  start_port = NULL;
  
  if(plugin_port != NULL){
    port_count = g_list_length(start_plugin_port);
    
    for(i = 0; i < port_count; i++){
      if(ags_plugin_port_test_flags((AgsPluginPort *) plugin_port->data, AGS_PLUGIN_PORT_CONTROL)){
	gchar *specifier;

	GValue *value;
	
	GRecMutex *plugin_port_mutex;

	/* get plugin port mutex */
	plugin_port_mutex = AGS_PLUGIN_PORT_GET_OBJ_MUTEX(plugin_port->data);

	/* get specifier */
	g_rec_mutex_lock(plugin_port_mutex);
	
	specifier = g_strdup(AGS_PLUGIN_PORT(plugin_port->data)->port_name);

	g_rec_mutex_unlock(plugin_port_mutex);

	if(specifier == NULL){
	  plugin_port = plugin_port->next;
	  
	  continue;
	}

	/* new port */
	current = g_object_new(AGS_TYPE_PORT,
			       "plugin-name", plugin_name,
			       "specifier", specifier,
			       "control-port", g_strdup_printf("%lu/%lu",
								 i + 1,
								 port_count),
			       "port-value-is-pointer", FALSE,
			       "port-value-type", G_TYPE_FLOAT,
			       NULL);
	current->flags |= AGS_PORT_USE_LADSPA_FLOAT;
	g_object_ref(current);

	if(ags_plugin_port_test_flags((AgsPluginPort *) plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  ags_recall_set_flags((AgsRecall *) play_dssi_audio,
			       AGS_RECALL_HAS_OUTPUT_PORT);

	  current->flags |= AGS_PORT_IS_OUTPUT;
	}else{
	  if(!ags_plugin_port_test_flags((AgsPluginPort *) plugin_port->data, AGS_PLUGIN_PORT_INTEGER) &&
	     !ags_plugin_port_test_flags((AgsPluginPort *) plugin_port->data, AGS_PLUGIN_PORT_TOGGLED)){
	    current->flags |= AGS_PORT_INFINITE_RANGE;
	  }
	}
	
	g_object_set(current,
		     "plugin-port", plugin_port->data,
		     NULL);
	
	ags_play_dssi_audio_load_conversion(play_dssi_audio,
					    (GObject *) current,
					    (GObject *) plugin_port->data);

	g_object_get(plugin_port->data,
		     "default-value", &value,
		     NULL);
	
	current->port_value.ags_port_ladspa = g_value_get_float(value);
	//	g_message("connecting port: %d/%d %f", i, port_count, current->port_value.ags_port_float);

	start_port = g_list_prepend(start_port,
				    current);
      }else if(ags_plugin_port_test_flags((AgsPluginPort *) plugin_port->data, AGS_PLUGIN_PORT_AUDIO)){
	/* audio port */
	if(ags_plugin_port_test_flags((AgsPluginPort *) plugin_port->data, AGS_PLUGIN_PORT_INPUT)){
	  g_rec_mutex_lock(recall_mutex);
	  
	  if(play_dssi_audio->input_port == NULL){
	    play_dssi_audio->input_port = (guint *) malloc(sizeof(guint));
	    play_dssi_audio->input_port[0] = i;
	  }else{
	    play_dssi_audio->input_port = (guint *) realloc(play_dssi_audio->input_port,
							    (play_dssi_audio->input_lines + 1) * sizeof(guint));
	    play_dssi_audio->input_port[play_dssi_audio->input_lines] = i;
	  }
	  
	  play_dssi_audio->input_lines += 1;

	  g_rec_mutex_unlock(recall_mutex);
	}else if(ags_plugin_port_test_flags((AgsPluginPort *) plugin_port->data, AGS_PLUGIN_PORT_OUTPUT)){
	  g_rec_mutex_lock(recall_mutex);
	  
	  if(play_dssi_audio->output_port == NULL){
	    play_dssi_audio->output_port = (guint *) malloc(sizeof(guint));
	    play_dssi_audio->output_port[0] = i;
	  }else{
	    play_dssi_audio->output_port = (guint *) realloc(play_dssi_audio->output_port,
							     (play_dssi_audio->output_lines + 1) * sizeof(guint));
	    play_dssi_audio->output_port[play_dssi_audio->output_lines] = i;
	  }
	  
	  play_dssi_audio->output_lines += 1;

	  g_rec_mutex_unlock(recall_mutex);	  
	}
      }

      /* iterate */
      plugin_port = plugin_port->next;
    }

    start_port = g_list_reverse(start_port);
    AGS_RECALL(play_dssi_audio)->port = g_list_copy(start_port);
  }

  /* unref/free */
  g_list_free_full(start_plugin_port,
		   g_object_unref);
  
  g_free(filename);
  g_free(effect);

  g_free(plugin_name);
  
#ifdef AGS_DEBUG
  g_message("output lines: %d", play_dssi_audio->output_lines);
#endif
  
  return(start_port);
}

/**
 * ags_play_dssi_audio_load_conversion:
 * @play_dssi_audio: the #AgsPlayDssiAudio
 * @port: the #AgsPort
 * @plugin_port: the #AgsPluginPort
 * 
 * Loads conversion object by using @plugin_port and sets in on @port.
 * 
 * Since: 3.0.0
 */
void
ags_play_dssi_audio_load_conversion(AgsPlayDssiAudio *play_dssi_audio,
				    GObject *port,
				    GObject *plugin_port)
{
  AgsLadspaConversion *ladspa_conversion;

  if(!AGS_IS_PLAY_DSSI_AUDIO(play_dssi_audio) ||
     !AGS_IS_PORT(port) ||
     !AGS_PLUGIN_PORT(plugin_port)){
    return;
  }

  ladspa_conversion = NULL;

  if(ags_plugin_port_test_flags((AgsPluginPort *) plugin_port, AGS_PLUGIN_PORT_BOUNDED_BELOW)){
    if(ladspa_conversion == NULL ||
       !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
      ladspa_conversion = ags_ladspa_conversion_new();
    }

    ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_BELOW;
  }

  if(ags_plugin_port_test_flags((AgsPluginPort *) plugin_port, AGS_PLUGIN_PORT_BOUNDED_ABOVE)){
    if(ladspa_conversion == NULL ||
       !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
      ladspa_conversion = ags_ladspa_conversion_new();
    }

    ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_BOUNDED_ABOVE;
  }
  
  if(ags_plugin_port_test_flags((AgsPluginPort *) plugin_port, AGS_PLUGIN_PORT_SAMPLERATE)){
    if(ladspa_conversion == NULL ||
       !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
      ladspa_conversion = ags_ladspa_conversion_new();
    }
        
    ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_SAMPLERATE;
  }

  if(ags_plugin_port_test_flags((AgsPluginPort *) plugin_port, AGS_PLUGIN_PORT_LOGARITHMIC)){
    if(ladspa_conversion == NULL ||
       !AGS_IS_LADSPA_CONVERSION(ladspa_conversion)){
      ladspa_conversion = ags_ladspa_conversion_new();
    }
    
    ladspa_conversion->flags |= AGS_LADSPA_CONVERSION_LOGARITHMIC;
  }

  if(ladspa_conversion != NULL){
    g_object_set(port,
		 "conversion", ladspa_conversion,
		 NULL);
  }
}

/**
 * ags_play_dssi_audio_find:
 * @recall: (element-type AgsAudio.Recall) (transfer none): the #GList-struct containing #AgsRecall
 * @filename: plugin's filename
 * @effect: plugin's effect
 *
 * Find DSSI recall.
 *
 * Returns: (element-type AgsAudio.Recall) (transfer none): Next matching #GList-struct, or %NULL if not found
 * 
 * Since: 3.0.0
 */
GList*
ags_play_dssi_audio_find(GList *recall,
			 gchar *filename, gchar *effect)
{
  while(recall != NULL){
    if(AGS_IS_PLAY_DSSI_AUDIO(recall->data)){
      gboolean success;
      
      GRecMutex *recall_mutex;

      /* get recall mutex */
      recall_mutex = AGS_RECALL_GET_OBJ_MUTEX(recall->data);

      /* check current filename and effect */
      g_rec_mutex_lock(recall_mutex);

      success = (!g_strcmp0(AGS_RECALL(recall->data)->filename,
			    filename) &&
		 !g_strcmp0(AGS_RECALL(recall->data)->effect,
			    effect)) ? TRUE: FALSE;
      
      g_rec_mutex_unlock(recall_mutex);

      if(success){
	break;
      }
    }

    /* iterate */
    recall = recall->next;
  }

  return(recall);
}

/**
 * ags_play_dssi_audio_new:
 * @audio: the #AgsAudio
 *
 * Create a new instance of #AgsPlayDssiAudio
 *
 * Returns: the new #AgsPlayDssiAudio
 *
 * Since: 3.0.0
 */
AgsPlayDssiAudio*
ags_play_dssi_audio_new(AgsAudio *audio)
{
  AgsPlayDssiAudio *play_dssi_audio;

  play_dssi_audio = (AgsPlayDssiAudio *) g_object_new(AGS_TYPE_PLAY_DSSI_AUDIO,
						      "audio", audio,
						      NULL);

  return(play_dssi_audio);
}
