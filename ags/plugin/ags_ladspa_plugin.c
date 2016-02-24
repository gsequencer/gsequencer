/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/plugin/ags_ladspa_plugin.h>

#include <ags/object/ags_soundcard.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <math.h>

void ags_ladspa_plugin_class_init(AgsLadspaPluginClass *ladspa_plugin);
void ags_ladspa_plugin_init (AgsLadspaPlugin *ladspa_plugin);
void ags_ladspa_plugin_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_ladspa_plugin_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_ladspa_plugin_finalize(GObject *gobject);

gpointer ags_ladspa_plugin_instantiate(AgsBasePlugin *base_plugin);
void ags_ladspa_plugin_connect_port(AgsBasePlugin *base_plugin,
				    gpointer plugin_handle,
				    guint port_index,
				    gpointer data_location);
void ags_ladspa_plugin_activate(AgsBasePlugin *base_plugin,
				gpointer plugin_handle);
void ags_ladspa_plugin_deactivate(AgsBasePlugin *base_plugin,
				  gpointer plugin_handle);
void ags_ladspa_plugin_run(AgsBasePlugin *base_plugin,
			   gpointer plugin_handle,
			   snd_seq_event_t *seq_event,
			   guint frame_count);
void ags_ladspa_plugin_load_plugin(AgsBasePlugin *base_plugin);

/**
 * SECTION:ags_ladspa_plugin
 * @short_description: The ladspa plugin class
 * @title: AgsLadspaPlugin
 * @section_id:
 * @include: ags/object/ags_ladspa_plugin.h
 *
 * The #AgsLadspaPlugin loads/unloads a Ladspa plugin.
 */

enum{
  PROP_0,
  PROP_UNIQUE_ID,
};

static gpointer ags_ladspa_plugin_parent_class = NULL;

GType
ags_ladspa_plugin_get_type (void)
{
  static GType ags_type_ladspa_plugin = 0;

  if(!ags_type_ladspa_plugin){
    static const GTypeInfo ags_ladspa_plugin_info = {
      sizeof (AgsLadspaPluginClass),
      NULL, /* ladspa_init */
      NULL, /* ladspa_finalize */
      (GClassInitFunc) ags_ladspa_plugin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLadspaPlugin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_ladspa_plugin_init,
    };

    ags_type_ladspa_plugin = g_type_register_static(AGS_TYPE_BASE_PLUGIN,
						    "AgsLadspaPlugin\0",
						    &ags_ladspa_plugin_info,
						    0);
  }

  return (ags_type_ladspa_plugin);
}

void
ags_ladspa_plugin_class_init(AgsLadspaPluginClass *ladspa_plugin)
{
  AgsBasePluginClass *base_plugin;

  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_ladspa_plugin_parent_class = g_type_class_peek_parent(ladspa_plugin);

  /* GObjectClass */
  gobject = (GObjectClass *) ladspa_plugin;

  gobject->set_property = ags_ladspa_plugin_set_property;
  gobject->get_property = ags_ladspa_plugin_get_property;

  gobject->finalize = ags_ladspa_plugin_finalize;

  /* properties */
  /**
   * AgsLadspaPlugin:unique-id:
   *
   * The assigned unique-id.
   * 
   * Since: 0.7.6
   */
  param_spec = g_param_spec_string("unique-id\0",
				   "unique-id of the plugin\0",
				   "The unique-id this plugin is assigned with\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UNIQUE_ID,
				  param_spec);

  /* AgsBasePluginClass */
  base_plugin = (AgsBasePluginClass *) ladspa_plugin;

  base_plugin->instantiate = ags_ladspa_plugin_instantiate;

  base_plugin->connect_port = ags_ladspa_plugin_connect_port;

  base_plugin->activate = ags_ladspa_plugin_activate;
  base_plugin->deactivate = ags_ladspa_plugin_deactivate;

  base_plugin->run = ags_ladspa_plugin_run;

  base_plugin->load_plugin = ags_ladspa_plugin_load_plugin;
}

void
ags_ladspa_plugin_init(AgsLadspaPlugin *ladspa_plugin)
{
  ladspa_plugin->unique_id = NULL;
}

void
ags_ladspa_plugin_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsLadspaPlugin *ladspa_plugin;

  ladspa_plugin = AGS_LADSPA_PLUGIN(gobject);

  switch(prop_id){
  case PROP_UNIQUE_ID:
    {
      gchar *unique_id;

      unique_id = (gchar *) g_value_get_string(value);

      if(ladspa_plugin->unique_id == unique_id){
	return;
      }
      
      if(ladspa_plugin->unique_id != NULL){
	g_free(ladspa_plugin->unique_id);
      }

      ladspa_plugin->unique_id = g_strdup(unique_id);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ladspa_plugin_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsLadspaPlugin *ladspa_plugin;

  ladspa_plugin = AGS_LADSPA_PLUGIN(gobject);

  switch(prop_id){
  case PROP_UNIQUE_ID:
    g_value_set_string(value, ladspa_plugin->unique_id);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_ladspa_plugin_finalize(GObject *gobject)
{
  AgsLadspaPlugin *ladspa_plugin;

  ladspa_plugin = AGS_LADSPA_PLUGIN(gobject);
}

gpointer
ags_ladspa_plugin_instantiate(AgsBasePlugin *base_plugin)
{
  //TODO:JK: implement me
}

void
ags_ladspa_plugin_connect_port(AgsBasePlugin *base_plugin,
			       gpointer plugin_handle,
			       guint port_index,
			       gpointer data_location)
{
  //TODO:JK: implement me
}

void
ags_ladspa_plugin_activate(AgsBasePlugin *base_plugin,
			   gpointer plugin_handle)
{
  //TODO:JK: implement me
}

void
ags_ladspa_plugin_deactivate(AgsBasePlugin *base_plugin,
			     gpointer plugin_handle)
{
  //TODO:JK: implement me
}

void
ags_ladspa_plugin_run(AgsBasePlugin *base_plugin,
		      gpointer plugin_handle,
		      snd_seq_event_t *seq_event,
		      guint frame_count)
{
  //TODO:JK: implement me
}

void
ags_ladspa_plugin_load_plugin(AgsBasePlugin *base_plugin)
{
  AgsConfig *config;
  
  AgsPortDescriptor *port_descriptor;
  GList *list;

  gchar *str;
  
  guint samplerate;
  
  LADSPA_DescriptorFunction ladspa_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  LADSPA_PortRangeHintDescriptor hint_descriptor;

  unsigned long effect_index;
  unsigned long port_count;
  unsigned long i;

  config = ags_config_get_instance();

  str = ags_config_get_value(config,
			     AGS_CONFIG_SOUNDCARD,
			     "samplerate\0");

  if(str != NULL){
    samplerate = g_ascii_strtoull(str,
				  NULL,
				  10);
    
    free(str);
  }else{
    samplerate = AGS_SOUNDCARD_DEFAULT_SAMPLERATE;
  }
  
  base_plugin->plugin_so = dlopen(base_plugin->filename,
				  RTLD_NOW);
  
  if(base_plugin->plugin_so){
    g_warning("ags_ladspa_plugin.c - failed to load static object file\0");
    
    dlerror();
  }

  ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym(plugin_so,
							 "ladspa_descriptor\0");
  
  if(dlerror() == NULL && ladspa_descriptor){
    effect_index = base_plugin->effect_index;
    base_plugin->plugin_descriptor = ladspa_descriptor(effect_index);

    if(base_plugin->plugin_descriptor != NULL){
      g_object_set(base_plugin,
		   "unique-id\0", AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->UniqueID,
		   NULL);
      
      port_count = AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->PortCount;
      port_descriptor = AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->PortDescriptors;

      for(i = 0; i < port_count; i++){
	/* allocate port descriptor */
	port_descriptor = ags_port_descriptor_alloc();

	/* set flags */
	if(LADSPA_IS_PORT_INPUT(port_descriptor[i])){
	  port_descriptor->flags |= AGS_PORT_DESCRIPTOR_INPUT;
	}else if(LADSPA_IS_PORT_OUTPUT(port_descriptor[i])){
	  port_descriptor->flags |= AGS_PORT_DESCRIPTOR_OUTPUT;
	}
	
	if(LADSPA_IS_PORT_CONTROL(port_descriptor[i])){
	  port_descriptor->flags |= AGS_PORT_DESCRIPTOR_CONTROL;
	}else if(LADSPA_IS_PORT_AUDIO(port_descriptor[i])){
	  port_descriptor->flags |= AGS_PORT_DESCRIPTOR_AUDIO;
	}

	/* set index and name */
	port_descripotr->port_index = i;
	port_descriptor->port_name = g_strdup(AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->PortNames[i]);
	
	hint_descriptor = AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->PortRangeHints[i].HintDescriptor;

	if(hint_descriptor != NULL){
	  if(LADSPA_IS_HINT_TOGGLED(hint_descriptor)){
	    /* is toggled */
	    port_descriptor->flags |= AGS_PORT_DESCRIPTOR_TOGGLED;

	    /* set default */
	    if(LADSPA_HINT_DEFAULT_0(hint_descriptor)){
	      g_value_init(port_descriptor->default_value,
			   G_TYPE_BOOLEAN);
	      g_value_set_boolean(port_descriptor->default_value,
				  FALSE);
	    }else if(LADSPA_HINT_DEFAULT_1(hint_descriptor)){
	      g_value_init(port_descriptor->default_value,
			   G_TYPE_BOOLEAN);
	      g_value_set_boolean(port_descriptor->default_value,
				  TRUE);
	    }
	  }else{
	    /* set lower */
	    g_value_init(port_descriptor->lower_value,
			 G_TYPE_FLOAT);
	    g_value_set_float(port_descriptor->lower_value,
			      hint_descriptor.LowerBound);
	    
	    /* set upper */
	    g_value_init(port_descriptor->upper_value,
			 G_TYPE_FLOAT);
	    g_value_set_float(port_descriptor->upper_value,
			      hint_descriptor.UpperBound);

	    /* bounds */
	    if(LADSPA_HINT_BOUNDED_BELOW(hint_descriptor)){
	      if(LADSPA_HINT_SAMPLERATE(hint_descriptor)){
		g_value_set_float(port_descriptor->lower_value,
				  samplerate * hint_descriptor.LowerBound);
	      }
	    }

	    if(LADSPA_HINT_BOUNDED_ABOVE(hint_descriptor)){
	      if(LADSPA_HINT_SAMPLERATE(hint_descriptor)){
		g_value_set_float(port_descriptor->upper_value,
				  samplerate * hint_descriptor.UpperBound);
	      }
	    }

	    /* integer */
	    if(LADSPA_HINT_INTEGER(hint_descriptor)){
	      port_descriptor->flags |= AGS_PORT_DESCRIPTOR_INTEGER;
	    }

	    /* logarithmic */
	    if(LADSPA_HINT_LOGARITHMIC(hint_descriptor)){
	      port_descriptor->flags |= AGS_PORT_DESCRIPTOR_LOGARITHMIC;
	    }

	    /* set default value */
	    if(LADSPA_HINT_DEFAULT_MINIMUM(hint_descriptor)){
	      g_value_init(port_descriptor->default_value,
			   G_TYPE_FLOAT);
	      g_value_set_float(port_descriptor->default_value,
				hint_descriptor.LowerBound);
	    }else if(LADSPA_HINT_DEFAULT_LOW(hint_descriptor)){
	      float default_value;

	      if(LADSPA_HINT_LOGARITHMIC(hint_descriptor)){
		default_value = exp(log(hint_descriptor.LowerBound) * 0.75 +
				    log(hint_descriptor.UpperBound) * 0.25);
	      }else{
		default_value = 0.75 * hint_descriptor.LowerBound + 0.25 * hint_descriptor.UpperBound;
	      }
	      
	      g_value_init(port_descriptor->default_value,
			   G_TYPE_FLOAT);
	      g_value_set_float(port_descriptor->default_value,
				default_value);
	    }else if(LADSPA_HINT_DEFAULT_MIDDLE(hint_descriptor)){
	      float default_value;

	      if(LADSPA_HINT_LOGARITHMIC(hint_descriptor)){
		default_value = exp(log(hint_descriptor.LowerBound) * 0.55 +
				    log(hint_descriptor.UpperBound) * 0.55);
	      }else{
		default_value = (0.5 * hint_descriptor.LowerBound) + (0.5 * hint_descriptor.UpperBound);
	      }
	      
	      g_value_init(port_descriptor->default_value,
			   G_TYPE_FLOAT);
	      g_value_set_float(port_descriptor->default_value,
				hint_descriptor.UpperBound);
	    }else if(LADSPA_HINT_DEFAULT_HIGH(hint_descriptor)){
	      float default_value;

	      if(LADSPA_HINT_LOGARITHMIC(hint_descriptor)){
		default_value = exp(log(hint_descriptor.LowerBound) * 0.25 +
				    log(hint_descriptor.UpperBound) * 0.75);
	      }else{
		default_value = 0.25 * hint_descriptor.LowerBound + 0.75 * hint_descriptor.UpperBound;
	      }

	      g_value_init(port_descriptor->default_value,
			   G_TYPE_FLOAT);
	      g_value_set_float(port_descriptor->default_value,
				hint_descriptor.UpperBound);
	    }else if(LADSPA_HINT_DEFAULT_MAXIMUM(hint_descriptor)){
	      g_value_init(port_descriptor->default_value,
			   G_TYPE_FLOAT);
	      g_value_set_float(port_descriptor->default_value,
				hint_descriptor.UpperBound);
	    }else if(LADSPA_HINT_DEFAULT_100(hint_descriptor)){
	      g_value_init(port_descriptor->default_value,
			   G_TYPE_FLOAT);
	      g_value_set_float(port_descriptor->default_value,
				100.0);
	    }else if(LADSPA_HINT_DEFAULT_440(hint_descriptor)){
	      g_value_init(port_descriptor->default_value,
			   G_TYPE_FLOAT);
	      g_value_set_float(port_descriptor->default_value,
				440.0);
	    }
	  }
	}
      }      
    }
  }
}

/**
 * ags_ladspa_plugin_new:
 * @filename: the plugin .so
 * @effect: the effect's string representation
 * @effect_index: the effect's index
 *
 * Creates an #AgsLadspaPlugin
 *
 * Returns: a new #AgsLadspaPlugin
 *
 * Since: 0.7.6
 */
AgsLadspaPlugin*
ags_ladspa_plugin_new(gchar *filename, gchar *effect, guint effect_index)
{
  AgsLadspaPlugin *ladspa_plugin;

  ladspa_plugin = (AgsLadspaPlugin *) g_object_new(AGS_TYPE_LADSPA_PLUGIN,
						   "filename\0", filename,
						   "effect\0", effect,
						   "effect-index\0", effect_index,
						   NULL);

  return(ladspa_plugin);
}
