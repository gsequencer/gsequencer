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

#include <ags/plugin/ags_dssi_plugin.h>

#include <ags/object/ags_marshal.h>
#include <ags/object/ags_config.h>
#include <ags/object/ags_soundcard.h>

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <ags/i18n.h>

void ags_dssi_plugin_class_init(AgsDssiPluginClass *dssi_plugin);
void ags_dssi_plugin_init (AgsDssiPlugin *dssi_plugin);
void ags_dssi_plugin_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_dssi_plugin_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_dssi_plugin_finalize(GObject *gobject);

gpointer ags_dssi_plugin_instantiate(AgsBasePlugin *base_plugin,
				     guint samplerate);
void ags_dssi_plugin_connect_port(AgsBasePlugin *base_plugin,
				  gpointer plugin_handle,
				  guint port_index,
				  gpointer data_location);
void ags_dssi_plugin_activate(AgsBasePlugin *base_plugin,
			      gpointer plugin_handle);
void ags_dssi_plugin_deactivate(AgsBasePlugin *base_plugin,
				gpointer plugin_handle);
void ags_dssi_plugin_run(AgsBasePlugin *base_plugin,
			 gpointer plugin_handle,
			 snd_seq_event_t *seq_event,
			 guint frame_count);
void ags_dssi_plugin_load_plugin(AgsBasePlugin *base_plugin);

void ags_dssi_plugin_real_change_program(AgsDssiPlugin *dssi_plugin,
					 gpointer ladspa_handle,
					 guint bank_index,
					 guint program_index);

/**
 * SECTION:ags_dssi_plugin
 * @short_description: The dssi plugin class
 * @title: AgsDssiPlugin
 * @section_id:
 * @include: ags/object/ags_dssi_plugin.h
 *
 * The #AgsDssiPlugin loads/unloads a Dssi plugin.
 */

enum{
  PROP_0,
  PROP_UNIQUE_ID,
  PROP_PROGRAM,
};

enum{
  CHANGE_PROGRAM,
  LAST_SIGNAL,
};

static gpointer ags_dssi_plugin_parent_class = NULL;
static guint dssi_plugin_signals[LAST_SIGNAL];

GType
ags_dssi_plugin_get_type (void)
{
  static GType ags_type_dssi_plugin = 0;

  if(!ags_type_dssi_plugin){
    static const GTypeInfo ags_dssi_plugin_info = {
      sizeof (AgsDssiPluginClass),
      NULL, /* dssi_init */
      NULL, /* dssi_finalize */
      (GClassInitFunc) ags_dssi_plugin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsDssiPlugin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_dssi_plugin_init,
    };

    ags_type_dssi_plugin = g_type_register_static(AGS_TYPE_BASE_PLUGIN,
						  "AgsDssiPlugin",
						  &ags_dssi_plugin_info,
						  0);
  }

  return (ags_type_dssi_plugin);
}

void
ags_dssi_plugin_class_init(AgsDssiPluginClass *dssi_plugin)
{
  AgsBasePluginClass *base_plugin;

  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_dssi_plugin_parent_class = g_type_class_peek_parent(dssi_plugin);

  /* GObjectClass */
  gobject = (GObjectClass *) dssi_plugin;

  gobject->set_property = ags_dssi_plugin_set_property;
  gobject->get_property = ags_dssi_plugin_get_property;

  gobject->finalize = ags_dssi_plugin_finalize;

  /* properties */
  /**
   * AgsDssiPlugin:unique-id:
   *
   * The assigned unique-id.
   * 
   * Since: 0.7.6
   */
  param_spec = g_param_spec_uint("unique-id",
				 i18n_pspec("unique-id of the plugin"),
				 i18n_pspec("The unique-id this plugin is assigned with"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UNIQUE_ID,
				  param_spec);

  /**
   * AgsBasePlugin:program:
   *
   * The assigned program.
   * 
   * Since: 0.7.6
   */
  param_spec = g_param_spec_string("program",
				   i18n_pspec("program of the plugin"),
				   i18n_pspec("The program this plugin is located in"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PROGRAM,
				  param_spec);

  /* AgsBasePluginClass */
  base_plugin = (AgsBasePluginClass *) dssi_plugin;

  base_plugin->instantiate = ags_dssi_plugin_instantiate;

  base_plugin->connect_port = ags_dssi_plugin_connect_port;

  base_plugin->activate = ags_dssi_plugin_activate;
  base_plugin->deactivate = ags_dssi_plugin_deactivate;

  base_plugin->run = ags_dssi_plugin_run;

  base_plugin->load_plugin = ags_dssi_plugin_load_plugin;

  /* AgsDssiPluginClass */
  dssi_plugin->change_program = ags_dssi_plugin_real_change_program;
  
  /**
   * AgsDssiPlugin::change-program:
   * @dssi_plugin: the plugin to change-program
   * @ladspa_handle: the LADSPA handle
   * @bank: the bank number
   * @program: the program number
   *
   * The ::change-program signal creates a new instance of plugin.
   *
   * Since: 0.7.122
   */
  dssi_plugin_signals[CHANGE_PROGRAM] =
    g_signal_new("change-program",
		 G_TYPE_FROM_CLASS (dssi_plugin),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsDssiPluginClass, change_program),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__POINTER_UINT_UINT,
		 G_TYPE_NONE, 3,
		 G_TYPE_POINTER,
		 G_TYPE_UINT,
		 G_TYPE_UINT);
}

void
ags_dssi_plugin_init(AgsDssiPlugin *dssi_plugin)
{
  dssi_plugin->unique_id = 0;
  dssi_plugin->program = NULL;
}

void
ags_dssi_plugin_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsDssiPlugin *dssi_plugin;

  dssi_plugin = AGS_DSSI_PLUGIN(gobject);

  switch(prop_id){
  case PROP_UNIQUE_ID:
    {
      dssi_plugin->unique_id = g_value_get_uint(value);
    }
    break;
  case PROP_PROGRAM:
    {
      gchar *program;

      program = (gchar *) g_value_get_string(value);

      if(dssi_plugin->program == program){
	return;
      }
      
      if(dssi_plugin->program != NULL){
	g_free(dssi_plugin->program);
      }

      dssi_plugin->program = g_strdup(program);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_dssi_plugin_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsDssiPlugin *dssi_plugin;

  dssi_plugin = AGS_DSSI_PLUGIN(gobject);

  switch(prop_id){
  case PROP_UNIQUE_ID:
    g_value_set_uint(value, dssi_plugin->unique_id);
    break;
  case PROP_PROGRAM:
    g_value_set_string(value, dssi_plugin->program);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_dssi_plugin_finalize(GObject *gobject)
{
  AgsDssiPlugin *dssi_plugin;

  dssi_plugin = AGS_DSSI_PLUGIN(gobject);

  g_free(dssi_plugin->program);

  /* call parent */
  G_OBJECT_CLASS(ags_dssi_plugin_parent_class)->finalize(gobject);
}

gpointer
ags_dssi_plugin_instantiate(AgsBasePlugin *base_plugin,
			    guint samplerate)
{
  AGS_DSSI_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->LADSPA_Plugin->instantiate(AGS_DSSI_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->LADSPA_Plugin,
											 (unsigned long) samplerate);
}

void
ags_dssi_plugin_connect_port(AgsBasePlugin *base_plugin,
			     gpointer plugin_handle,
			     guint port_index,
			     gpointer data_location)
{
  AGS_DSSI_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->LADSPA_Plugin->connect_port((LADSPA_Handle) plugin_handle,
											  (unsigned long) port_index,
											  (LADSPA_Data *) data_location);
}

void
ags_dssi_plugin_activate(AgsBasePlugin *base_plugin,
			 gpointer plugin_handle)
{
  if(AGS_DSSI_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->LADSPA_Plugin->activate != NULL){
    AGS_DSSI_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->LADSPA_Plugin->activate((LADSPA_Handle) plugin_handle);
  }
}

void
ags_dssi_plugin_deactivate(AgsBasePlugin *base_plugin,
			   gpointer plugin_handle)
{
  if(AGS_DSSI_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->LADSPA_Plugin->deactivate != NULL){
    AGS_DSSI_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->LADSPA_Plugin->deactivate((LADSPA_Handle) plugin_handle);
  }
}

void
ags_dssi_plugin_run(AgsBasePlugin *base_plugin,
		    gpointer plugin_handle,
		    snd_seq_event_t *seq_event,
		    guint frame_count)
{
  if(AGS_DSSI_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->run_synth != NULL){
    AGS_DSSI_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->run_synth((LADSPA_Handle) plugin_handle,
									  frame_count,
									  seq_event,
									  (unsigned long) 1);
  }else{
    AGS_DSSI_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->LADSPA_Plugin->run((LADSPA_Handle) plugin_handle,
										   (unsigned long) frame_count);
  }
}

void
ags_dssi_plugin_load_plugin(AgsBasePlugin *base_plugin)
{
  AgsPortDescriptor *port;

  GList *port_list;

  gchar *str;
  
  DSSI_Descriptor_Function dssi_descriptor;
  LADSPA_PortDescriptor *port_descriptor;
  LADSPA_PortRangeHint *range_hint;
  LADSPA_PortRangeHintDescriptor hint_descriptor;

  unsigned long effect_index;
  unsigned long port_count;
  unsigned long i;
  
  base_plugin->plugin_so = dlopen(base_plugin->filename,
				  RTLD_NOW);
  
  if(base_plugin->plugin_so == NULL){
    g_warning("ags_dssi_plugin.c - failed to load static object file");
    
    dlerror();

    return;
  }

  dssi_descriptor = (DSSI_Descriptor_Function) dlsym(base_plugin->plugin_so,
						     "dssi_descriptor");
  
  if(dlerror() == NULL && dssi_descriptor){
    effect_index = base_plugin->effect_index;
    base_plugin->plugin_descriptor = dssi_descriptor(effect_index);

    if(base_plugin->plugin_descriptor != NULL){
      g_object_set(base_plugin,
		   "unique-id", AGS_DSSI_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->LADSPA_Plugin->UniqueID,
		   NULL);
      
      port_count = AGS_DSSI_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->LADSPA_Plugin->PortCount;
      port_descriptor = AGS_DSSI_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->LADSPA_Plugin->PortDescriptors;

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
	port->port_name = g_strdup(AGS_DSSI_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->LADSPA_Plugin->PortNames[i]);

	range_hint = &(AGS_DSSI_PLUGIN_DESCRIPTOR(base_plugin->plugin_descriptor)->LADSPA_Plugin->PortRangeHints[i]);
	hint_descriptor = range_hint->HintDescriptor;

	g_value_init(port->default_value,
		     G_TYPE_FLOAT);
	g_value_init(port->lower_value,
		     G_TYPE_FLOAT);
	g_value_init(port->upper_value,
		     G_TYPE_FLOAT);

	g_value_set_float(port->default_value,
			  0.0);

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
	  g_value_set_float(port->lower_value,
			    range_hint->LowerBound);
	    
	  /* set upper */
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
	    port->scale_steps = range_hint->UpperBound - range_hint->LowerBound;
	  }

	  /* logarithmic */
	  if(LADSPA_IS_HINT_LOGARITHMIC(hint_descriptor)){
	    port->flags |= AGS_PORT_DESCRIPTOR_LOGARITHMIC;
	  }

	  /* set default value */
	  if(LADSPA_IS_HINT_DEFAULT_0(hint_descriptor)){
	    g_value_set_float(port->default_value,
			      0.0);
	  }else if(LADSPA_IS_HINT_DEFAULT_1(hint_descriptor)){
	    g_value_set_float(port->default_value,
			      1.0);
	  }else if(LADSPA_IS_HINT_DEFAULT_MINIMUM(hint_descriptor)){
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

      base_plugin->port = g_list_reverse(port_list);
    }
  }
}

void
ags_dssi_plugin_real_change_program(AgsDssiPlugin *dssi_plugin,
				    gpointer ladspa_handle,
				    guint bank_index,
				    guint program_index)
{
  if(AGS_DSSI_PLUGIN_DESCRIPTOR(AGS_BASE_PLUGIN(dssi_plugin)->plugin_descriptor)->select_program != NULL){
    AGS_DSSI_PLUGIN_DESCRIPTOR(AGS_BASE_PLUGIN(dssi_plugin)->plugin_descriptor)->select_program((void *) ladspa_handle,
												(unsigned long) bank_index,
												(unsigned long) program_index);
  }
}

void
ags_dssi_plugin_change_program(AgsDssiPlugin *dssi_plugin,
			       gpointer ladspa_handle,
			       guint bank_index,
			       guint program_index)
{
  g_return_if_fail(AGS_IS_DSSI_PLUGIN(dssi_plugin));
  g_object_ref(G_OBJECT(dssi_plugin));
  g_signal_emit(G_OBJECT(dssi_plugin),
		dssi_plugin_signals[CHANGE_PROGRAM], 0,
		ladspa_handle,
		bank_index,
		program_index);
  g_object_unref(G_OBJECT(dssi_plugin));
}

/**
 * ags_dssi_plugin_new:
 * @filename: the plugin .so
 * @effect: the effect's string representation
 * @effect_index: the effect's index
 *
 * Creates an #AgsDssiPlugin
 *
 * Returns: a new #AgsDssiPlugin
 *
 * Since: 0.7.6
 */
AgsDssiPlugin*
ags_dssi_plugin_new(gchar *filename, gchar *effect, guint effect_index)
{
  AgsDssiPlugin *dssi_plugin;

  dssi_plugin = (AgsDssiPlugin *) g_object_new(AGS_TYPE_DSSI_PLUGIN,
					       "filename", filename,
					       "effect", effect,
					       "effect-index", effect_index,
					       NULL);

  return(dssi_plugin);
}
