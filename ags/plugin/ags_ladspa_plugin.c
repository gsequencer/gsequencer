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

#include <ags/object/ags_config.h>
#include <ags/object/ags_soundcard.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <math.h>
#include <string.h>

#include <ladspa.h>

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

gpointer ags_ladspa_plugin_instantiate(AgsBasePlugin *base_plugin,
				       guint samplerate);
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
  param_spec = g_param_spec_uint("unique-id\0",
				 "unique-id of the plugin\0",
				 "The unique-id this plugin is assigned with\0",
				 0,
				 G_MAXUINT,
				 0,
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
      ladspa_plugin->unique_id = g_value_get_uint(value);
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
ags_ladspa_plugin_instantiate(AgsBasePlugin *base_plugin,
			      guint samplerate)
{
  gpointer ladspa_handle;
  
  ladspa_handle = (gpointer) AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->instantiate(AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor),
												       (unsigned long) samplerate);

  return(ladspa_handle);
}

void
ags_ladspa_plugin_connect_port(AgsBasePlugin *base_plugin,
			       gpointer plugin_handle,
			       guint port_index,
			       gpointer data_location)
{
  AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->connect_port((LADSPA_Handle) plugin_handle,
									     (unsigned long) port_index,
									     (LADSPA_Data *) data_location);
}

void
ags_ladspa_plugin_activate(AgsBasePlugin *base_plugin,
			   gpointer plugin_handle)
{
  if(AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->activate != NULL){
    AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->activate((LADSPA_Handle) plugin_handle);
  }
}

void
ags_ladspa_plugin_deactivate(AgsBasePlugin *base_plugin,
			     gpointer plugin_handle)
{
  if(AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->deactivate != NULL){
    AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->deactivate((LADSPA_Handle) plugin_handle);
  }
}

void
ags_ladspa_plugin_run(AgsBasePlugin *base_plugin,
		      gpointer plugin_handle,
		      snd_seq_event_t *seq_event,
		      guint frame_count)
{
  AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->run((LADSPA_Handle) plugin_handle,
								    (unsigned long) frame_count);
}

void
ags_ladspa_plugin_load_plugin(AgsBasePlugin *base_plugin)
{
  AgsConfig *config;
  
  AgsPortDescriptor *port;
  GList *port_list;

  gchar *str;
  
  guint samplerate;
  
  LADSPA_Descriptor_Function ladspa_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  LADSPA_PortRangeHint *range_hint;
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
  
  if(base_plugin->plugin_so == NULL){
    g_warning("ags_ladspa_plugin.c - failed to load static object file\0");
    
    dlerror();

    return;
  }

  ladspa_descriptor = (LADSPA_Descriptor_Function) dlsym((void *) base_plugin->plugin_so,
							 "ladspa_descriptor\0");
  
  if(dlerror() == NULL && ladspa_descriptor){
    effect_index = base_plugin->effect_index;
    base_plugin->plugin_descriptor = ladspa_descriptor((unsigned long) effect_index);

    if(base_plugin->plugin_descriptor != NULL){
      g_object_set(base_plugin,
		   "unique-id\0", AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->UniqueID,
		   NULL);
      
      port_count = AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->PortCount;
      port_descriptor = AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->PortDescriptors;

      port_list = NULL;
      
      for(i = 0; i < port_count; i++){
	/* allocate port descriptor */
	port = ags_port_descriptor_alloc();
	port_list = g_list_prepend(port_list,
				   port);
	
	/* set flags */
	if(LADSPA_IS_PORT_INPUT(port_descriptor[i])){
	  port->flags |= AGS_PORT_DESCRIPTOR_INPUT;
	}else if(LADSPA_IS_PORT_OUTPUT(port_descriptor[i])){
	  port->flags |= AGS_PORT_DESCRIPTOR_OUTPUT;
	}
	
	if(LADSPA_IS_PORT_CONTROL(port_descriptor[i])){
	  port->flags |= AGS_PORT_DESCRIPTOR_CONTROL;
	}else if(LADSPA_IS_PORT_AUDIO(port_descriptor[i])){
	  port->flags |= AGS_PORT_DESCRIPTOR_AUDIO;
	}

	/* set index and name */
	port->port_index = i;
	port->port_name = g_strdup(AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->PortNames[i]);

	range_hint = &(AGS_LADSPA_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->PortRangeHints[i]);
	hint_descriptor =  range_hint->HintDescriptor;

	g_value_init(port->default_value,
		     G_TYPE_FLOAT);
	g_value_set_float(port->default_value,
			  0.0);
	
	if(hint_descriptor != NULL){
	  if(LADSPA_IS_HINT_TOGGLED(hint_descriptor)){
	    /* is toggled */
	    port->flags |= AGS_PORT_DESCRIPTOR_TOGGLED;

	    /* set default */
	    if(LADSPA_IS_HINT_DEFAULT_0(hint_descriptor)){
	      g_value_set_float(port->default_value,
				0.0);
	    }else if(LADSPA_IS_HINT_DEFAULT_1(hint_descriptor)){
	      g_value_set_float(port->default_value,
				1.0);
	    }
	  }else{
	    /* set lower */
	    g_value_init(port->lower_value,
			 G_TYPE_FLOAT);
	    g_value_set_float(port->lower_value,
			      range_hint->LowerBound);
	    
	    /* set upper */
	    g_value_init(port->upper_value,
			 G_TYPE_FLOAT);
	    g_value_set_float(port->upper_value,
			      range_hint->UpperBound);

	    /* set default */
	    g_value_set_float(port->default_value,
			      range_hint->LowerBound);
	
	    /* bounds */
	    if(LADSPA_IS_HINT_BOUNDED_BELOW(hint_descriptor)){
	      if(LADSPA_IS_HINT_SAMPLE_RATE(hint_descriptor)){
		port->flags |= (AGS_PORT_DESCRIPTOR_SAMPLERATE |
				AGS_PORT_DESCRIPTOR_BOUNDED_BELOW);
		g_value_set_float(port->lower_value,
				  range_hint->LowerBound);
	      }
	    }

	    if(LADSPA_IS_HINT_BOUNDED_ABOVE(hint_descriptor)){
	      if(LADSPA_IS_HINT_SAMPLE_RATE(hint_descriptor)){
		port->flags |= (AGS_PORT_DESCRIPTOR_SAMPLERATE |
				AGS_PORT_DESCRIPTOR_BOUNDED_ABOVE);
		g_value_set_float(port->upper_value,
				  range_hint->UpperBound);
	      }
	    }

	    /* integer */
	    if(LADSPA_IS_HINT_INTEGER(hint_descriptor)){
	      port->flags |= AGS_PORT_DESCRIPTOR_INTEGER;
	    }

	    /* logarithmic */
	    if(LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor)){
	      port->flags |= AGS_PORT_DESCRIPTOR_LOGARITHMIC;
	    }

	    /* set default value */
	    if(LADSPA_IS_HINT_DEFAULT_MINIMUM(hint_descriptor)){
	      g_value_set_float(port->default_value,
				range_hint->LowerBound);
	    }else if(LADSPA_IS_HINT_DEFAULT_LOW(hint_descriptor)){
	      float default_value;

	      default_value = 0.75 * range_hint->LowerBound + 0.25 * range_hint->UpperBound;
	      
	      g_value_set_float(port->default_value,
				default_value);
	    }else if(LADSPA_IS_HINT_DEFAULT_MIDDLE(hint_descriptor)){
	      float default_value;

	      default_value = (0.5 * range_hint->LowerBound) + (0.5 * range_hint->UpperBound);
	      
	      g_value_set_float(port->default_value,
				default_value);
	    }else if(LADSPA_IS_HINT_DEFAULT_HIGH(hint_descriptor)){
	      float default_value;

	      default_value = 0.25 * range_hint->LowerBound + 0.75 * range_hint->UpperBound;

	      g_value_set_float(port->default_value,
				default_value);
	    }else if(LADSPA_IS_HINT_DEFAULT_MAXIMUM(hint_descriptor)){
	      g_value_set_float(port->default_value,
				range_hint->UpperBound);
	    }else if(LADSPA_IS_HINT_DEFAULT_100(hint_descriptor)){
	      g_value_set_float(port->default_value,
				100.0);
	    }else if(LADSPA_IS_HINT_DEFAULT_440(hint_descriptor)){
	      g_value_set_float(port->default_value,
				440.0);
	    }
	  }
	}
      }

      base_plugin->port = g_list_reverse(port_list);
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
