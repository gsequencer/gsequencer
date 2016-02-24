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

#include <ags/plugin/ags_base_plugin.h>

#include <ags/object/ags_marshal.h>

void ags_base_plugin_class_init(AgsBasePluginClass *base_plugin);
void ags_base_plugin_init (AgsBasePlugin *base_plugin);
void ags_base_plugin_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_base_plugin_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_base_plugin_finalize(GObject *gobject);

/**
 * SECTION:ags_base_plugin
 * @short_description: The base plugin class
 * @title: AgsBasePlugin
 * @section_id:
 * @include: ags/object/ags_base_plugin.h
 *
 * The #AgsBasePlugin loads/unloads plugins on a abstraction level.
 */

enum{
  INSTANTIATE,
  CONNECT_PORT,
  ACTIVATE,
  DEACTIVATE,
  LOAD_PLUGIN,
  LAST_SIGNAL,
};

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_EFFECT,
  PROP_EFFECT_INDEX,
  PROP_UI_PLUGIN,
};

static gpointer ags_base_plugin_parent_class = NULL;
static guint base_plugin_signals[LAST_SIGNAL];

GType
ags_base_plugin_get_type (void)
{
  static GType ags_type_base_plugin = 0;

  if(!ags_type_base_plugin){
    static const GTypeInfo ags_base_plugin_info = {
      sizeof (AgsBasePluginClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_base_plugin_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsBasePlugin),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_base_plugin_init,
    };

    ags_type_base_plugin = g_type_register_static(G_TYPE_OBJECT,
						  "AgsBasePlugin\0",
						  &ags_base_plugin_info,
						  0);
  }

  return (ags_type_base_plugin);
}

void
ags_base_plugin_class_init(AgsBasePluginClass *base_plugin)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_base_plugin_parent_class = g_type_class_peek_parent(base_plugin);

  /* GObjectClass */
  gobject = (GObjectClass *) base_plugin;

  gobject->set_property = ags_base_plugin_set_property;
  gobject->get_property = ags_base_plugin_get_property;

  gobject->finalize = ags_base_plugin_finalize;

  /* properties */
  /**
   * AgsBasePlugin:filename:
   *
   * The assigned filename.
   * 
   * Since: 0.7.6
   */
  param_spec = g_param_spec_string("filename\0",
				   "filename of the plugin\0",
				   "The filename this plugin is located in\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsBasePlugin:effect:
   *
   * The assigned effect.
   * 
   * Since: 0.7.6
   */
  param_spec = g_param_spec_string("effect\0",
				   "effect of the plugin\0",
				   "The effect this plugin is assigned with\0",
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT,
				  param_spec);

  /**
   * AgsBasePlugin:effect-index:
   *
   * The assigned effect-index.
   * 
   * Since: 0.7.6
   */
  param_spec = g_param_spec_uint("effect-index\0",
				 "effect-index of the plugin\0",
				 "The effect-index this plugin is assigned with\0",
				 0,
				 G_MANUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_EFFECT_INDEX,
				  param_spec);

  /**
   * AgsBasePlugin:ui-plugin:
   *
   * The assigned ui-plugin.
   * 
   * Since: 0.7.6
   */
  param_spec = g_param_spec_object("ui-plugin\0",
				   "ui-plugin of the plugin\0",
				   "The ui-plugin this plugin is assigned with\0",
				   AGS_TYPE_BASE_PLUGIN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_UI_PLUGIN,
				  param_spec);

  /* AgsBasePluginClass */
  base_plugin->instantiate = NULL;

  base_plugin->connect_port = NULL;
  
  base_plugin->activate = NULL;
  base_plugin->deactivate = NULL;

  base_plugin->load_plugin = NULL;
  
  /**
   * AgsBasePlugin::instantiate:
   * @base_plugin: the plugin to instantiate
   *
   * The ::instantiate signal creates a new instance of plugin.
   */
  base_plugin_signals[INSTANTIATE] =
    g_signal_new("instantiate\0",
		 G_TYPE_FROM_CLASS (base_plugin),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsBasePluginClass, instantiate),
		 NULL, NULL,
		 g_cclosure_user_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);

  /**
   * AgsBasePlugin::connect-port:
   * @base_plugin: the plugin to connect-port
   *
   * The ::connect-port signal creates a new instance of plugin.
   */
  base_plugin_signals[CONNECT_PORT] =
    g_signal_new("connect-port\0",
		 G_TYPE_FROM_CLASS (base_plugin),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsBasePluginClass, connect_port),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__POINTER_UINT_POINTER,
		 G_TYPE_NONE, 3,
		 G_TYPE_POINTER,
		 G_TYPE_UINT,
		 G_TYPE_POINTER);

  /**
   * AgsBasePlugin::activate:
   * @base_plugin: the plugin to activate
   *
   * The ::activate signal creates a new instance of plugin.
   */
  base_plugin_signals[ACTIVATE] =
    g_signal_new("activate\0",
		 G_TYPE_FROM_CLASS (base_plugin),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsBasePluginClass, activate),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);

  /**
   * AgsBasePlugin::deactivate:
   * @base_plugin: the plugin to deactivate
   *
   * The ::deactivate signal creates a new instance of plugin.
   */
  base_plugin_signals[DEACTIVATE] =
    g_signal_new("deactivate\0",
		 G_TYPE_FROM_CLASS (base_plugin),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsBasePluginClass, deactivate),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);

  /**
   * AgsBasePlugin::load_plugin:
   * @base_plugin: the plugin to load_plugin
   *
   * The ::load_plugin signal creates a new instance of plugin.
   */
  base_plugin_signals[LOAD_PLUGIN] =
    g_signal_new("load_plugin\0",
		 G_TYPE_FROM_CLASS (base_plugin),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsBasePluginClass, load_plugin),
		 NULL, NULL,
		 g_cclosure_user_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);
}

void
ags_base_plugin_init(AgsBasePlugin *base_plugin)
{
  base_plugin->filename = NULL;
  base_plugin->effect = NULL;

  base_plugin->port_group_count = 0;
  base_plugin->port_group = NULL;
  base_plugin->port = NULL;

  base_plugin->effect_index = 0;
  base_plugin->plugin_so = NULL;
  base_plugin->plugin_descriptor = NULL;
  base_plugin->plugin_handle = NULL;

  base_plugin->ui_plugin = NULL;
}

void
ags_base_plugin_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsBasePlugin *base_plugin;

  base_plugin = AGS_BASE_PLUGIN(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    {
      gchar *filename;

      filename = (gchar *) g_value_get_string(value);

      if(base_plugin->filename == filename){
	return;
      }
      
      if(base_plugin->filename != NULL){
	g_free(base_plugin->filename);
      }

      base_plugin->filename = g_strdup(filename);
    }
    break;
  case PROP_EFFECT:
    {
      gchar *effect;

      effect = (gchar *) g_value_get_string(value);

      if(base_plugin->effect == effect){
	return;
      }
      
      if(base_plugin->effect != NULL){
	g_free(base_plugin->effect);
      }

      base_plugin->effect = g_strdup(effect);
    }
    break;
  case PROP_EFFECT_INDEX:
    {
      guint effect_index;

      effect_index = g_value_get_uint(value);

      base_plugin->effect_index = effect_index;
    }
    break;
  case PROP_UI_PLUGIN:
    {
      GObject *ui_plugin;

      ui_plugin = g_value_get_object(value);

      if(base_plugin->ui_plugin == ui_plugin){
	return;
      }

      if(base_plugin->ui_plugin != NULL){
	g_object_unref(base_plugin->ui_plugin);
      }

      if(ui_plugin != NULL){
	g_object_ref(ui_plugin);
      }
      
      base_plugin->ui_plugin = ui_plugin;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_base_plugin_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsBasePlugin *base_plugin;

  base_plugin = AGS_BASE_PLUGIN(gobject);

  switch(prop_id){
  case PROP_FILENAME:
    g_value_set_string(value, base_plugin->filename);
    break;
  case PROP_EFFECT:
    g_value_set_string(value, base_plugin->effect);
    break;
  case PROP_EFFECT_INDEX:
    g_value_set_uint(value, base_plugin->effect_index);
    break;
  case PROP_UI_PLUGIN:
    g_value_set_object(value, base_plugin->ui_plugin);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_base_plugin_finalize(GObject *gobject)
{
  AgsBasePlugin *base_plugin;

  base_plugin = AGS_BASE_PLUGIN(gobject);
}

AgsPortDescriptor*
ags_port_descriptor_alloc()
{
  AgsPortDescriptor *port_descriptor;

  port_descriptor = (AgsPortDescriptor *) malloc(sizeof(AgsPortDescriptor));

  port_descriptor->flags = 0;

  port_descriptor->port_index = 0;

  port_descriptor->port_name = NULL;
  port_descriptor->port_symbol = NULL;

  port_descriptor->scale_steps = 8;
  port_descriptor->scale_points = NULL;

  port_descriptor->lower_value = g_new0(GValue,
					1);
  port_descriptor->upper_value = g_new0(GValue,
					1);

  port_descriptor->default_value = g_new0(GValue,
					  1);

  port_descriptor->user_data = NULL;
  
  return(port_descriptor);
}

void
ags_port_descriptor_free(AgsPortDescriptor *port_descriptor)
{
  if(port_descriptor->port_name != NULL){
    g_free(port_descriptor->port_name);
  }
  
  if(port_descriptor->port_symbol != NULL){
    g_free(port_descriptor->port_symbol);
  }

  if(port_descriptor->scale_points != NULL){
    g_free(port_descriptor->scale_points);
  }

  g_free(port_descriptor->lower_value);
  g_free(port_descriptor->upper_value);

  g_free(port_descriptor->default_value);

  free(port_descriptor);
}

void
ags_base_plugin_apply_port_group_by_prefix(AgsBasePlugin *base_plugin)
{
  //TODO:JK: implement me
}

/**
 * ags_base_plugin_instantiate:
 * @base_plugin: the #AgsBasePlugin
 *
 * Instantiate the plugin
 *
 * Returns: the new plugin instance handle
 *
 * Since: 0.7.6
 */
gpointer
ags_base_plugin_instantiate(AgsBasePlugin *base_plugin)
{
  gpointer retval;
  
  g_return_if_fail(AGS_IS_BASE_PLUGIN(base_plugin));
  g_object_ref(G_OBJECT(base_plugin));
  g_signal_emit(G_OBJECT(base_plugin),
		base_plugin_signals[INSTANTIATE], 0,
		&retval);
  g_object_unref(G_OBJECT(base_plugin));

  return(retval);
}

/**
 * ags_base_plugin_connect_port:
 * @base_plugin: the #AgsBasePlugin
 * @plugin_handle: the plugin instance handle
 * @port_index: the port's index to connect
 * @data_location: the data location to connect
 *
 * Connect a plugin instance.
 *
 * Since: 0.7.6
 */
void
ags_base_plugin_connect_port(AgsBasePlugin *base_plugin, gpointer plugin_handle, guint port_index, gpointer data_location)
{
  g_return_if_fail(AGS_IS_BASE_PLUGIN(base_plugin));
  g_object_ref(G_OBJECT(base_plugin));
  g_signal_emit(G_OBJECT(base_plugin),
		base_plugin_signals[CONNECT_PORT], 0,
		plugin_handle, port_index, data_location);
  g_object_unref(G_OBJECT(base_plugin));
}

/**
 * ags_base_plugin_activate:
 * @base_plugin: the #AgsBasePlugin
 * @plugin_handle: the plugin instance handle
 *
 * Activate a plugin instance
 *
 * Since: 0.7.6
 */
void
ags_base_plugin_activate(AgsBasePlugin *base_plugin, gpointer plugin_handle)
{
  g_return_if_fail(AGS_IS_BASE_PLUGIN(base_plugin));
  g_object_ref(G_OBJECT(base_plugin));
  g_signal_emit(G_OBJECT(base_plugin),
		base_plugin_signals[ACTIVATE], 0,
		plugin_handle);
  g_object_unref(G_OBJECT(base_plugin));
}

/**
 * ags_base_plugin_deactivate:
 * @base_plugin: the #AgsBasePlugin
 * @plugin_handle: the plugin instance handle
 *
 * Deactivat a plugin instance
 *
 * Since: 0.7.6
 */
void
ags_base_plugin_deactivate(AgsBasePlugin *base_plugin, gpointer plugin_handle)
{
  g_return_if_fail(AGS_IS_BASE_PLUGIN(base_plugin));
  g_object_ref(G_OBJECT(base_plugin));
  g_signal_emit(G_OBJECT(base_plugin),
		base_plugin_signals[DEACTIVATE], 0,
		plugin_handle);
  g_object_unref(G_OBJECT(base_plugin));
}

/**
 * ags_base_plugin_load_plugin:
 * @base_plugin: the #AgsBasePlugin
 *
 * Load the plugin
 *
 * Since: 0.7.6
 */
void
ags_base_plugin_load_plugin(AgsBasePlugin *base_plugin)
{
  g_return_if_fail(AGS_IS_BASE_PLUGIN(base_plugin));
  g_object_ref(G_OBJECT(base_plugin));
  g_signal_emit(G_OBJECT(base_plugin),
		base_plugin_signals[LOAD_PLUGIN], 0);
  g_object_unref(G_OBJECT(base_plugin));
}


/**
 * ags_base_plugin_new:
 * @filename: the plugin .so
 * @effect: the effect's string representation
 * @effect_index: the effect's index
 *
 * Creates an #AgsBasePlugin
 *
 * Returns: a new #AgsBasePlugin
 *
 * Since: 0.7.6
 */
AgsBasePlugin*
ags_base_plugin_new(gchar *filename, gchar *effect, guint effect_index)
{
  AgsBasePlugin *base_plugin;

  base_plugin = (AgsBasePlugin *) g_object_new(AGS_TYPE_BASE_PLUGIN,
					       "filename\0", filename,
					       "effect\0", effect,
					       "effect-index\0", effect_index,
					       NULL);

  return(base_plugin);
}
