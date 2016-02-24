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

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

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

gpointer ags_dssi_plugin_instantiate(AgsBasePlugin *base_plugin);
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
					 gpointer dssi_handle,
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
						  "AgsDssiPlugin\0",
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
   * AgsBasePlugin:program:
   *
   * The assigned program.
   * 
   * Since: 0.7.6
   */
  param_spec = g_param_spec_string("program\0",
				   "program of the plugin\0",
				   "The program this plugin is located in\0",
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
   *
   * The ::change-program signal creates a new instance of plugin.
   */
  dssi_plugin_signals[CHANGE_PROGRAM] =
    g_signal_new("change-program\0",
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
}

gpointer
ags_dssi_plugin_instantiate(AgsBasePlugin *base_plugin)
{
  //TODO:JK: implement me
}

void
ags_dssi_plugin_connect_port(AgsBasePlugin *base_plugin,
			     gpointer plugin_handle,
			     guint port_index,
			     gpointer data_location)
{
  //TODO:JK: implement me
}

void
ags_dssi_plugin_activate(AgsBasePlugin *base_plugin,
			 gpointer plugin_handle)
{
  //TODO:JK: implement me
}

void
ags_dssi_plugin_deactivate(AgsBasePlugin *base_plugin,
			   gpointer plugin_handle)
{
  //TODO:JK: implement me
}

void
ags_dssi_plugin_run(AgsBasePlugin *base_plugin,
		    gpointer plugin_handle,
		    snd_seq_event_t *seq_event,
		    guint frame_count)
{
  //TODO:JK: implement me
}

void
ags_dssi_plugin_load_plugin(AgsBasePlugin *base_plugin)
{
  //TODO:JK: implement me
}

void
ags_dssi_plugin_real_change_program(AgsDssiPlugin *dssi_plugin,
				    gpointer dssi_handle,
				    guint bank_index,
				    guint program_index)
{
  //TODO:JK: implement me
}

void
ags_dssi_plugin_change_program(AgsDssiPlugin *dssi_plugin,
			       gpointer dssi_handle,
			       guint bank_index,
			       guint program_index)
{
  g_return_if_fail(AGS_IS_DSSI_PLUGIN(dssi_plugin));
  g_object_ref(G_OBJECT(dssi_plugin));
  g_signal_emit(G_OBJECT(dssi_plugin),
		dssi_plugin_signals[CHANGE_PROGRAM], 0,
		dssi_handle,
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
					       "filename\0", filename,
					       "effect\0", effect,
					       "effect-index\0", effect_index,
					       NULL);

  return(dssi_plugin);
}
